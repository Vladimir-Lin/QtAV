#include <qtav.h>

N::VideoFeeder:: VideoFeeder    ( QObject * parent )
               : QObject        (           parent )
               , Object         ( 0 , Types::Video )
               , VideoReader    (                  )
               , VideoChannel   (                  )
               , AudioChannel   (                  )
               , Thread         (                  )
               , PostSequences  ( NULL             )
               , bufferTime     ( 5 * 1000         )
               , interval       ( 10               )
               , waitCompletion ( true             )
               , keepWaiting    ( false            )
{
  Transmission = NewTransmission ( ) ;
}

N::VideoFeeder::~VideoFeeder (void)
{
}

int N::VideoFeeder::type(void) const
{
  return 1001 ;
}

QString N::VideoFeeder::name(void)
{
  return tr("Video File Feeder") ;
}

QString N::VideoFeeder::source(void)
{
  return VideoData::Filename ;
}

bool N::VideoFeeder::hasData(void)
{
  return bytes ( ) > 0 ;
}

TUID N::VideoFeeder::length(void)
{
  return AvDecoder :: Duration ( ) / 1000 ;
}

TUID N::VideoFeeder::bytes(void)
{
  return AudioBuffer . AudioSize ( ) ;
}

int N::VideoFeeder::rate(void)
{
  return AudioBuffer . Rate ( ) ;
}

int N::VideoFeeder::channels(void)
{
  return AudioBuffer . Channels ( ) ;
}

bool N::VideoFeeder::hasChannel(int id)
{
  nKickOut ( id <  0                      , false ) ;
  nKickOut ( id >= AudioBuffer.Channels() , false ) ;
  return true                                       ;
}

QString N::VideoFeeder::channel(int id)
{
  nKickOut ( !hasChannel(id) , "" )             ;
  return tr("%1 : %2").arg(VideoPath()).arg(id) ;
}

bool N::VideoFeeder::canSeek(void)
{
  return true ;
}

bool N::VideoFeeder::assign(AudioData & data)
{
  data . set                                          (
    AudioBuffer.Rate                            ( )   ,
    AudioBuffer.Bits                            ( )   ,
    AudioBuffer.Channels                        ( )   ,
    (Acoustics::SampleFormat)AudioBuffer.Format ( ) ) ;
  return true                                         ;
}

bool N::VideoFeeder::obtain(AudioData & data,int bytes)
{
  if (AudioBuffer.AudioSize() <= 0)                 {
    return false                                    ;
  }                                                 ;
  char * src = AudioBuffer.index(0)                 ;
  nKickOut ( IsNull(src)      , false )             ;
  LockAudio   ( )                                   ;
  src = AudioBuffer.index(0)                        ;
  if (AudioBuffer.AudioSize()<bytes)                {
    bytes = AudioBuffer.AudioSize()                 ;
  }                                                 ;
  if (!Mute) data . add ( src , bytes )             ;
  AudioBuffer . takeAt ( 0 , bytes )                ;
  Transmission -> AudioFlushed  += bytes            ;
  UnlockAudio ( )                                   ;
  return true                                       ;
}

bool N::VideoFeeder::play(void)
{
  return channelPlay ( ) ;
}

bool N::VideoFeeder::Pause(void)
{
  return true ;
}

bool N::VideoFeeder::seek(TUID timestamp)
{
  return channelSeek ( timestamp ) ;
}

bool N::VideoFeeder::Resume(void)
{
  return true ;
}

bool N::VideoFeeder::abort(void)
{
  going = false ;
  return true   ;
}

int N::VideoFeeder::channelType(void) const
{
  return 1001 ;
}

QString N::VideoFeeder::channelName(void)
{
  return tr("Video File Feeder") ;
}

QString N::VideoFeeder::channelSource(void)
{
  return VideoData::Filename ;
}

bool N::VideoFeeder::channelHasData(void)
{
  if (AudioBuffer.size()>0) return true ;
  return hasVideo ( )                   ;
}

TUID N::VideoFeeder::channelLength(void)
{
  return AvDecoder :: Duration ( ) / 1000 ;
}

bool N::VideoFeeder::canSeekChannel(void)
{
  return true ;
}

bool N::VideoFeeder::channelPlay(void)
{
  keepWaiting = true ;
  start ( 1001 )     ;
  start ( 1002 )     ;
  return true        ;
}

bool N::VideoFeeder::channelPause(void)
{
  return false ;
}

bool N::VideoFeeder::channelSeek(TUID timestamp)
{
  bool correct = AvDecoder :: RawSeek ( timestamp ) ;
  if (!correct) return false                        ;
  ///////////////////////////////////////////////////
  // Information correction code here
  ///////////////////////////////////////////////////
  return true                                       ;
}

bool N::VideoFeeder::channelResume(void)
{
  return true ;
}

bool N::VideoFeeder::channelAbort(void)
{
  going = false ;
  return true   ;
}

int N::VideoFeeder::Interval(void)
{
  return interval ;
}

void N::VideoFeeder::setInterval(int value)
{
  interval = value ;
}

int N::VideoFeeder::BufferTime(void)
{
  return bufferTime ;
}

void N::VideoFeeder::setBufferTime(int buft)
{
  bufferTime = buft ;
}

bool N::VideoFeeder::Waiting(void)
{
  return waitCompletion ;
}

void N::VideoFeeder::setCompletion(bool wait)
{
  waitCompletion = wait ;
}

void N::VideoFeeder::Completion(void)
{
  while (waitCompletion && interval>0 && AudioPending()>0) {
    Time :: msleep ( interval )                            ;
    RollVideo      (          )                            ;
  }                                                        ;
  while (waitCompletion && interval>0 && hasVideo ( )    ) {
    Time :: msleep ( interval )                            ;
    RollVideo      (          )                            ;
  }                                                        ;
}

bool N::VideoFeeder::Halt(void)
{
  if ( bufferTime       <= 0          ) return true  ;
  if ( isVideoFull  ( )               ) return false ;
  if ( AudioPending ( ) >  bufferTime ) return false ;
  return true                                        ;
}

bool N::VideoFeeder::Peep(void)
{
  if (!VideoReader::Peep()) return false ;
  setTransmission ( )                    ;
  return true                            ;
}

bool N::VideoFeeder::prepare(void)
{
  if (!VideoReader::prepare())              {
    keepWaiting = false                     ;
    return false                            ;
  }                                         ;
  setTransmission ( )                       ;
  AvConvertCtx    ( VideoData :: Dimension  ,
                    AvPixelFormat           ,
                    SWS_FAST_BILINEAR     ) ;
  keepWaiting = false                       ;
  return true                               ;
}

void N::VideoFeeder::setTransmission(void)
{
  if (IsNull(Transmission)) return                                    ;
  Transmission -> Lock   ( )                                          ;
  Transmission -> BaseTime       = nTimeNow . addMSecs ( Drift      ) ;
  Transmission -> LastTime       = nTimeNow . addMSecs ( Drift - 30 ) ;
  Transmission -> Dimension      = VideoData::Dimension               ;
  Transmission -> TotalAudioSize =  0                                 ;
  Transmission -> AudioFlushed   =  0                                 ;
  VideoFrames                    = Transmission -> ThrottleSize       ;
  VideoPending                   =  0                                 ;
  VideoAt                        = -1                                 ;
  setPixelFormat ( Transmission->Format )                             ;
  Transmission -> setTime (VideoFrames,Transmission->BaseTime )       ;
  for (int i=0;i<Transmission->ThrottleSize;i++)                      {
    if (Transmission->Displays.contains(i))                           {
      (*(Transmission->Displays[i])) = false                          ;
    }                                                                 ;
  }                                                                   ;
  Transmission -> Unlock ( )                                          ;
}

QImage * N::VideoFeeder::ObtainImage(void)
{
  if (IsNull(Transmission)) return NULL                                  ;
  QImage * I                                                             ;
  if ( !Transmission->Images.contains(VideoPending) ) return NULL        ;
  Transmission -> Lock   ( )                                             ;
  I = Transmission -> Images [ VideoPending ]                            ;
  Transmission -> LastTime                    = GuessTime ( )            ;
  Transmission -> FrameTimes [ VideoPending ] = Transmission -> LastTime ;
  Transmission -> Unlock ( )                                             ;
  VideoPending ++                                                        ;
  VideoPending %= VideoFrames                                            ;
  return I                                                               ;
}

void N::VideoFeeder::AudioAppended(int newSize)
{
  Transmission -> TotalAudioSize += newSize ;
}

TUID N::VideoFeeder::AudioPending(void)
{
  TUID DAS                            ;
  DAS  = Transmission->TotalAudioSize ;
  DAS -= Transmission->AudioFlushed   ;
  DAS *= 1000                         ;
  DAS /= AudioBuffer.BytesPerSecond() ;
  return DAS                          ;
}

TUID N::VideoFeeder::AudioFlushed(void)
{
  TUID DAS                            ;
  DAS  = Transmission->AudioFlushed   ;
  DAS *= 1000                         ;
  DAS /= AudioBuffer.BytesPerSecond() ;
  return DAS                          ;
}

QDateTime N::VideoFeeder::GuessTime(void)
{
  QDateTime D = Transmission->BaseTime ;
  QDateTime T                          ;
  TUID      pts                        ;
  if (HasPTS(PTS(Packet)))             {
    TUID dts                           ;
    dts = PTS(Packet)                  ;
    pts = toMilliseconds ( dts )       ;
    T   = D . addMSecs   ( pts )       ;
  } else
  if (HasPTS(DTS(Packet)))             {
    pts = toMilliseconds(DTS(Packet))  ;
    T   = D . addMSecs ( pts )         ;
  } else                               {
    T   = AudioTime    (     )         ;
  }                                    ;
  if (Transmission->LastTime>T)        {
    D = Transmission->LastTime         ;
    T = D . addMSecs   ( 27  )         ;
  }                                    ;
  return T                             ;
}

QDateTime N::VideoFeeder::AudioTime(void)
{
  QDateTime D = Transmission->BaseTime ;
  QDateTime T                          ;
  TUID DAS                             ;
  DAS  = Transmission->TotalAudioSize  ;
  DAS *= 1000                          ;
  DAS /= AudioBuffer.BytesPerSecond()  ;
  T    = D . addMSecs ( DAS )          ;
  return T                             ;
}

bool N::VideoFeeder::isVideoFull(void)
{
  if ( VideoAt == -1 && VideoPending == 0 ) return false ;
  return ( VideoAt == VideoPending )                     ;
}

bool N::VideoFeeder::hasVideo(void)
{
  if ( VideoAt == -1 && VideoPending == 0 ) return false ;
  if ( VideoAt == VideoPending            ) return false ;
  if ( VideoAt >= 0 )                                    {
    int  va = VideoAt                                    ;
    int  vn = ( va + 1 ) % VideoFrames                   ;
    TUID DT                                              ;
    TUID CT                                              ;
    DT = Transmission -> Elapsed ( va )                  ;
    CT = Transmission -> Elapsed ( vn )                  ;
    if (DT>CT) return false                              ;
  }                                                      ;
  return true                                            ;
}

void N::VideoFeeder::RollVideo(void)
{
  TUID      DT                                   ;
  TUID      CT                                   ;
  if ( VideoAt < 0 )                             {
    CT = Transmission->Elapsed(0        )        ;
  } else                                         {
    int va = VideoAt                             ;
    int vn = ( va + 1 ) % VideoFrames            ;
    DT = Transmission -> Elapsed ( va )          ;
    CT = Transmission -> Elapsed ( vn )          ;
    if (DT>CT) return                            ;
  }                                              ;
  CT -= 15                                       ;
  DT  = Transmission -> Elapsed ( )              ;
  if (CT>DT) return                              ;
  Transmission -> Lock   ( )                     ;
  if (VideoAt>=0)                                {
    (*(Transmission->Displays[VideoAt])) = false ;
  }                                              ;
  VideoAt ++                                     ;
  VideoAt %= VideoFrames                         ;
  (*(Transmission->Displays[VideoAt])) = true    ;
  Transmission -> Unlock ( )                     ;
}

void N::VideoFeeder::finished(void)
{
  if (IsNull(PostSequences)) return ;
  VarArgs args                      ;
  args << 50001                     ;
  args << "VideoFeeder"             ;
  args << "Finished"                ;
  args << VoidVariant ( this )      ;
  (*PostSequences) << args          ;
}

void N::VideoFeeder::Loading(void)
{
  nDropOut ( ! VideoFeeder::prepare ( ) )       ;
  started (      )                              ;
  while ( VideoReader::proceed ( ) )            {
    bool shouldRead = Halt ( )                  ;
    if (shouldRead) decode ( )                  ;
    if (VideoReader::proceed ( ) && interval>0) {
      Time::msleep(interval)                    ;
    }                                           ;
  }                                             ;
  cease       ( )                               ;
  Completion  ( )                               ;
  finished    ( )                               ;
}

void N::VideoFeeder::Watching(void)
{
  while ( keepWaiting || VideoReader::proceed ( ) ) {
    Time::msleep(interval)                          ;
    RollVideo ( )                                   ;
  }                                                 ;
}

void N::VideoFeeder::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type )   {
    case 1001       :
      Loading  ( )  ;
    break           ;
    case 1002       :
      Watching ( )  ;
    break           ;
  }                 ;
}
