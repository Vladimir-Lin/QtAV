#include <qtav.h>

N::VideoStreaming:: VideoStreaming ( void )
                  : VideoReader    (      )
                  , ThreadId       ( -1   )
                  , step           ( 0    )
                  , LastStep       ( 0    )
                  , Roll           ( 0    )
                  , CacheMs        ( 5000 )
                  , video          ( 0    )
                  , album          ( 0    )
                  , description    ( 0    )
                  , position       ( 0    )
                  , core           ( NULL )
                  , conduit        ( NULL )
                  , stream         ( NULL )
                  , transmission   ( NULL )
                  , universe       ( NULL )
                  , controller     ( NULL )
                  , ratio          ( NULL )
                  , canvas         ( NULL )
                  , status         ( NULL )
                  , extras         ( NULL )
                  , vds            ( 27   )
{
  transmission = new VideoTransmission (         ) ;
  setCache                             ( CacheMs ) ;
  Booleans     [ "GoPrevious" ] = false            ;
  Booleans     [ "GoNext"     ] = false            ;
  Booleans     [ "hasWave"    ] = false            ;
  ChannelBlobs [ -1           ] = QByteArray ( )   ;
  Drift                         = 2000             ;
}

N::VideoStreaming::~VideoStreaming (void)
{
  Booleans     . clear (              ) ;
  ChannelBlobs . clear (              ) ;
  AvPackets    . clear (              ) ;
  nEnsureNull          ( transmission ) ;
  nEnsureNull          ( conduit      ) ;
}

void N::VideoStreaming::Preparation(void)
{
  if ( ! prepare ( ) )                {
    step = 91                         ;
  } else                              {
    step =  1                         ;
    setTransmission ( )               ;
    AvConvertCtx                      (
      VideoData::Dimension            ,
      VideoData::AvPixelFormat        ,
      SWS_FAST_BILINEAR             ) ;
  }                                   ;
  /////////////////////////////////////
  if ( hasVideo ( ) )                 {
    qint64 d = VideoData::Duration    ;
    qint64 f = VideoStream->nb_frames ;
    if ( ( d > 0 ) && ( f > 0 ) )     {
      vds = d / f                     ;
    }                                 ;
  }                                   ;
}

void N::VideoStreaming::Precache(void)
{
  bool gogo                               ;
  do                                      {
    if ( Having ( ) && ( ! isFull ( ) ) ) {
      gogo = decode ( )                   ;
    } else gogo = false                   ;
  } while ( gogo )                        ;
}

void N::VideoStreaming::setCache(int ms)
{
  CacheMs                       = ms      ;
  transmission -> ThrottleSize  = CacheMs ;
  transmission -> ThrottleSize *= 3       ;
  transmission -> ThrottleSize /= 100     ;
}

TUID N::VideoStreaming::msecs(void)
{
  return VideoData::Duration ;
}

TUID N::VideoStreaming::current(void)
{
  if ( IsNull(transmission) ) return 0 ;
  return AudioFlushed ( )              ;
}

bool N::VideoStreaming::hasVideo(void)
{
  return VideoData::isVideo ;
}

bool N::VideoStreaming::hasAudio(void)
{
  return VideoData::isAudio ;
}

bool N::VideoStreaming::Having(void)
{
  return going ;
}

void N::VideoStreaming::setTransmission(void)
{
  if (IsNull(transmission)) return                                    ;
  transmission -> Lock   ( )                                          ;
  transmission -> BaseTime       = nTimeNow . addMSecs ( Drift      ) ;
  transmission -> LastTime       = nTimeNow . addMSecs ( Drift - 30 ) ;
  transmission -> Dimension      = VideoData::Dimension               ;
  transmission -> TotalAudioSize =  0                                 ;
  transmission -> AudioFlushed   =  0                                 ;
  VideoFrames                    = transmission -> ThrottleSize       ;
  VideoPending                   =  0                                 ;
  VideoAt                        = -1                                 ;
  setPixelFormat ( transmission -> Format )                           ;
  transmission -> setTime (VideoFrames,transmission->BaseTime )       ;
  /////////////////////////////////////////////////////////////////////
  for (int i=0;i<transmission->ThrottleSize;i++)                      {
    if (transmission->Displays.contains(i))                           {
      (*(transmission->Displays[i])) = false                          ;
    }                                                                 ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  if (transmission->Displays.contains(0))                             {
    (*(transmission->Displays[0])) = true                             ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  transmission -> Unlock ( )                                          ;
}

bool N::VideoStreaming::ObtainAudio(void * buffer,int total)
{
  if ( IsNull ( conduit ) )                                              {
    return false                                                         ;
  }                                                                      ;
  if ( CiosAudio::StreamIO::Stalled == conduit -> Output . Situation )   {
    ::memset ( buffer , 0 , total )                                      ;
    return true                                                          ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  if ( CiosAudio::StreamIO::Stagnated == conduit -> Output . Situation ) {
    if ( AudioBuffer . Seconds ( ) >= 1 )                                {
      if ( transmission -> BaseTime > nTimeNow )                         {
        ::memset ( buffer , 0 , total )                                  ;
        return true                                                      ;
      }                                                                  ;
      conduit -> Output . Situation = CiosAudio::StreamIO::Started       ;
    } else                                                               {
      ::memset ( buffer , 0 , total )                                    ;
      return true                                                        ;
    }                                                                    ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  bool   vx = ( waves . count ( ) > 0 )                                  ;
  char * p  = AudioBuffer . index ( 0 )                                  ;
  LockAudio   ( )                                                        ;
  if ( total > AudioBuffer . AudioSize ( ) )                             {
    int t = AudioBuffer . AudioSize ( )                                  ;
    if ( t > 0 )                                                         {
      if ( vx )                                                          {
        mutex [ 0 ] . lock   ( )                                         ;
        ChannelBlobs [ -1 ] . append ( p , t )                           ;
        mutex [ 0 ] . unlock ( )                                         ;
      }                                                                  ;
      if ( NULL != p ) ::memcpy ( buffer , p , t )                       ;
      AudioBuffer . takeAt ( 0 , t                  )                    ;
      transmission -> AudioFlushed += t                                  ;
    }                                                                    ;
    p  = (char *) buffer                                                 ;
    p += t                                                               ;
    ::memset               ( p      , 0 , total - t )                    ;
  } else                                                                 {
    if ( vx )                                                            {
      mutex [ 0 ] . lock   ( )                                           ;
      ChannelBlobs [ -1 ] . append ( p , total )                         ;
      mutex [ 0 ] . unlock ( )                                           ;
    }                                                                    ;
    ::memcpy               ( buffer   , p , total   )                    ;
    AudioBuffer . takeAt   ( 0            , total   )                    ;
    transmission -> AudioFlushed += total                                ;
  }                                                                      ;
  UnlockAudio ( )                                                        ;
  return true                                                            ;
}

bool N::VideoStreaming::CacheFull(void)
{
  return ( AudioBuffer . Milliseconds ( ) > CacheMs ) ;
}

QImage * N::VideoStreaming::ObtainImage(void)
{
  if (IsNull(transmission)) return NULL                                  ;
  QImage * I                                                             ;
  if ( !transmission->Images.contains(VideoPending) ) return NULL        ;
  transmission -> Lock   ( )                                             ;
  I = transmission -> Images [ VideoPending ]                            ;
  transmission -> LastTime                    = GuessTime ( )            ;
  transmission -> FrameTimes [ VideoPending ] = transmission -> LastTime ;
  transmission -> Unlock ( )                                             ;
  VideoPending ++                                                        ;
  VideoPending %= VideoFrames                                            ;
  return I                                                               ;
}

void N::VideoStreaming::FetchImage(void)
{
  QImage * I  = NULL                        ;
  int      vp = VideoPending                ;
  TUID     pt = -1                          ;
  ///////////////////////////////////////////
  LockVideo       (                      )  ;
  I = ObtainImage (                      )  ;
  AssignImage     ( I                    )  ;
  ScaleImage      ( VideoData::Dimension )  ;
  if ( NotNull(I) && ( vp >= 0 ) )          {
    QDateTime FT                            ;
    QDateTime BT                            ;
    vp %= VideoFrames                       ;
    BT  = transmission -> BaseTime          ;
    FT  = transmission -> FrameTimes [ vp ] ;
    pt  = BT . msecsTo ( FT )               ;
  }                                         ;
  UnlockVideo     (                       ) ;
  ///////////////////////////////////////////
  if ( NotNull(I) && ( pt >= 0 ) )          {
    ProcessImage ( pt , I )                 ;
  }                                         ;
}

void N::VideoStreaming::ProcessImage(TUID pts,QImage * image)
{
}

void N::VideoStreaming::AudioAppended(int newSize)
{
  if ( IsNull(transmission) ) return        ;
  transmission -> TotalAudioSize += newSize ;
}

TUID N::VideoStreaming::AudioPending(void)
{
  if ( IsNull(transmission) ) return 0 ;
  TUID DAS                             ;
  DAS  = transmission->TotalAudioSize  ;
  DAS -= transmission->AudioFlushed    ;
  DAS *= 1000                          ;
  DAS /= AudioBuffer.BytesPerSecond()  ;
  return DAS                           ;
}

TUID N::VideoStreaming::AudioFlushed(void)
{
  if ( IsNull(transmission) ) return 0 ;
  TUID DAS                             ;
  DAS  = transmission->AudioFlushed    ;
  DAS *= 1000                          ;
  DAS /= AudioBuffer.BytesPerSecond()  ;
  return DAS                           ;
}

QDateTime N::VideoStreaming::GuessTime(void)
{
  QDateTime D = transmission->BaseTime   ;
  QDateTime T                            ;
  TUID      pts                          ;
  if (HasPTS(PTS(Packet)))               {
    pts = toMilliseconds ( PTS(Packet) ) ;
    T   = D . addMSecs   ( pts         ) ;
  } else
  if (HasPTS(DTS(Packet)))               {
    pts = toMilliseconds ( DTS(Packet) ) ;
    T   = D . addMSecs   ( pts         ) ;
  } else                                 {
    T   = AudioTime      (             ) ;
  }                                      ;
  if ( transmission -> LastTime > T )    {
    D = transmission -> LastTime         ;
    T = D . addMSecs   ( vds )           ;
  }                                      ;
  return T                               ;
}

QDateTime N::VideoStreaming::AudioTime(void)
{
  QDateTime D = transmission->BaseTime ;
  QDateTime T                          ;
  TUID DAS                             ;
  DAS  = transmission->TotalAudioSize  ;
  DAS *= 1000                          ;
  DAS /= AudioBuffer.BytesPerSecond()  ;
  T    = D . addMSecs ( DAS )          ;
  return T                             ;
}

bool N::VideoStreaming::isVideoFull(void)
{
  if ( VideoAt == -1 && VideoPending == 0 ) return false ;
  return ( VideoAt == VideoPending )                     ;
}

bool N::VideoStreaming::isFull(void)
{
  if ( CacheFull   ( ) ) return true ;
  if ( isVideoFull ( ) ) return true ;
  return false                       ;
}

bool N::VideoStreaming::hasStream(void)
{
  if ( ( -1 == VideoAt ) && ( 0 == VideoPending ) ) return false ;
  if ( VideoAt >= 0 )                                            {
    int  va = VideoAt    % VideoFrames                           ;
    int  vn = ( va + 1 ) % VideoFrames                           ;
    TUID DT                                                      ;
    TUID CT                                                      ;
    DT  = transmission -> Elapsed ( va )                         ;
    CT  = transmission -> Elapsed ( vn )                         ;
    DT -= CT                                                     ;
    if ( DT > 1000 ) return false                                ;
  }                                                              ;
  return true                                                    ;
}

void N::VideoStreaming::RollVideo(void)
{
  TUID CT                                                              ;
  TUID DT                                                              ;
  TUID ET                                                              ;
  int  situation = CiosAudio::StreamIO::Started                        ;
  bool correct   = false                                               ;
  if ( NotNull(conduit) )                                              {
    situation = conduit -> Output . Situation                          ;
  } else return                                                        ;
  if ( CiosAudio::StreamIO::Started   == situation ) correct = true    ;
  if ( CiosAudio::StreamIO::Completed == situation ) correct = true    ;
  if ( ! correct ) return                                              ;
  if ( VideoAt < 0 )                                                   {
    DT = transmission -> Elapsed (    )                                ;
    if ( DT < 0 ) return                                               ;
  } else                                                               {
    int va = VideoAt    % VideoFrames                                  ;
    int vn = ( va + 1 ) % VideoFrames                                  ;
    CT = transmission -> Elapsed ( vn )                                ;
    CT -= 5                                                            ;
    ET  = transmission -> Elapsed ( )                                  ;
    if ( ( CT >= 0 ) && ( CT > ET ) ) return                           ;
  }                                                                    ;
  Roll = 20                                                            ;
  transmission -> Lock   ( )                                           ;
  if (VideoAt>=0)                                                      {
    (*(transmission->Displays[VideoAt])) = false                       ;
  }                                                                    ;
  VideoAt ++                                                           ;
  VideoAt %= VideoFrames                                               ;
  (*(transmission->Displays[VideoAt])) = true                          ;
  transmission -> Unlock ( )                                           ;
}

bool N::VideoStreaming::hasPrevious(void)
{
  if ( Booleans . contains ( "Previous" ) ) {
    return Booleans [ "Previous" ]          ;
  }                                         ;
  return false                              ;
}

bool N::VideoStreaming::hasNext(void)
{
  if ( Booleans . contains ( "Next" ) ) {
    return Booleans [ "Next" ]          ;
  }                                     ;
  return false                          ;
}

bool N::VideoStreaming::hasForward(void)
{
  TUID ms = msecs   ( )         ;
  TUID cs = current ( )         ;
  ms -= cs                      ;
  return ( ms > ( 60 * 1000 ) ) ;
}

bool N::VideoStreaming::hasBackward(void)
{
  TUID cs = current ( )         ;
  return ( cs > ( 60 * 1000 ) ) ;
}

void N::VideoStreaming::Previous(void)
{
  Booleans [ "GoPrevious" ] = true ;
  Stop     (              )        ;
}

void N::VideoStreaming::Next(void)
{
  Booleans [ "GoNext"     ] = true ;
  Stop     (              )        ;
}

void N::VideoStreaming::Halt(void)
{
  step = 51                                                    ;
  if ( IsNull(conduit) ) return                                ;
  conduit -> Output . Situation = CiosAudio::StreamIO::Stalled ;
}

void N::VideoStreaming::Resume(void)
{
  if ( IsNull(transmission) )                                    {
    step = 11                                                    ;
    return                                                       ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  transmission -> Lock    ( )                                    ;
  TUID      cs  = current ( )                                    ;
  QDateTime BT  = nTimeNow                                       ;
  QDateTime PT  = transmission -> BaseTime                       ;
  CUIDs     IDs = transmission -> FrameTimes . keys ( )          ;
  int       id                                                   ;
  BT = BT . addMSecs ( -cs )                                     ;
  transmission -> BaseTime    = BT                               ;
  transmission -> CurrentTime = nTimeNow                         ;
  transmission -> LastTime    = GuessTime ( )                    ;
  foreach ( id , IDs )                                           {
    cs = PT . msecsTo  ( transmission -> FrameTimes [ id ] )     ;
    BT = transmission -> BaseTime                                ;
    BT = BT . addMSecs ( cs                                )     ;
    transmission -> FrameTimes [ id ] = BT                       ;
  }                                                              ;
  transmission -> Unlock  ( )                                    ;
  ////////////////////////////////////////////////////////////////
  step = 11                                                      ;
  if ( NotNull(conduit) )                                        {
    conduit -> Output . Situation = CiosAudio::StreamIO::Started ;
  }                                                              ;
}

void N::VideoStreaming::Stop(void)
{
  LockAudio          ( )                                      ;
  AudioBuffer . zero ( )                                      ;
  if ( NotNull ( conduit ) )                                  {
    conduit -> Output . Result = CiosAudio::Conduit::Complete ;
  }                                                           ;
  UnlockAudio        ( )                                      ;
  if ( VideoAt >= 0 ) VideoPending = VideoAt                  ;
  step = 93                                                   ;
}

void N::VideoStreaming::Forward(void)
{
  TUID cs = current ( ) ;
  cs += 60 * 1000       ;
  if ( cs < msecs ( ) ) {
    Seeking ( cs )      ;
  }                     ;
}

void N::VideoStreaming::Backward(void)
{
  TUID cs = current ( ) ;
  cs -= 60 * 1000       ;
  if ( cs < 0 ) cs = 0  ;
  Seeking ( cs )        ;
}

void N::VideoStreaming::Seeking(TUID pts)
{
  if ( 52 == step ) return ;
  LastStep = step          ;
  position = pts           ;
  step     = 52            ;
}

void N::VideoStreaming::Parking(TUID pts)
{
  int  count   = 0                                                           ;
  int  ret     = 0                                                           ;
  bool correct = false                                                       ;
  bool poke    = true                                                        ;
  TUID cts     = -1                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  while ( poke )                                                             {
    ret = ReadPacket ( Packet )                                              ;
    count++                                                                  ;
    switch (ret)                                                             {
      case AVERROR_BUFFER_TOO_SMALL                                          :
      case AVERROR_EXTERNAL                                                  :
      case AVERROR_BUG2                                                      :
      case AVERROR_BSF_NOT_FOUND                                             :
      case AVERROR_BUG                                                       :
      case AVERROR_DECODER_NOT_FOUND                                         :
      case AVERROR_DEMUXER_NOT_FOUND                                         :
      case AVERROR_ENCODER_NOT_FOUND                                         :
      case AVERROR_EXIT                                                      :
      case AVERROR_FILTER_NOT_FOUND                                          :
      case AVERROR_INVALIDDATA                                               :
      case AVERROR_MUXER_NOT_FOUND                                           :
      case AVERROR_OPTION_NOT_FOUND                                          :
      case AVERROR_PATCHWELCOME                                              :
      case AVERROR_PROTOCOL_NOT_FOUND                                        :
      case AVERROR_STREAM_NOT_FOUND                                          :
      case AVERROR_UNKNOWN                                                   :
      case AVERROR_EXPERIMENTAL                                              :
      case AVERROR_EOF                                                       :
        poke = false                                                         ;
      break                                                                  ;
      default                                                                :
        //////////////////////////////////////////////////////////////////////
        if ( ret < 0 ) poke = false ;                                     else
        if ( AvDecoder :: isAudioChannel ( Packet          ) )               {
        } else
        if ( AvDecoder :: isVideoChannel ( Packet          ) )               {
          if ( HasPTS ( PTS ( Packet ) ) )                                   {
            cts = toMilliseconds ( PTS ( Packet ) )                          ;
          } else
          if ( HasPTS ( DTS ( Packet ) ) )                                   {
            cts = toMilliseconds ( DTS ( Packet ) )                          ;
          }                                                                  ;
          AssignImage ( transmission -> Images [ VideoPending ] )            ;
        } else
        if (  AvDecoder :: isSubtitle    ( Packet          ) )               {
        } else                                                               {
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( ( pts - cts ) < 200 )                                           {
          correct = true                                                     ;
          poke    = false                                                    ;
        }                                                                    ;
      break                                                                  ;
    }                                                                        ;
    AvDecoder :: Release ( &Packet )                                         ;
    if ( poke )                                                              {
      if ( count > 100 ) poke = false                                        ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( correct )                                                             {
    transmission -> Lock   ( )                                               ;
    //////////////////////////////////////////////////////////////////////////
    transmission -> AudioFlushed  = AudioBuffer . BytesPerSecond ( )         ;
    transmission -> AudioFlushed *= cts                                      ;
    transmission -> AudioFlushed /= 1000                                     ;
    AudioBuffer   . zero ( )                                                 ;
    //////////////////////////////////////////////////////////////////////////
    transmission -> BaseTime       = nTimeNow . addMSecs ( -cts )            ;
    transmission -> LastTime       = transmission -> BaseTime                ;
    transmission -> TotalAudioSize =  transmission -> AudioFlushed           ;
    VideoPending                   =  0                                      ;
    VideoAt                        = -1                                      ;
    transmission -> setTime ( VideoFrames , transmission -> BaseTime )       ;
    //////////////////////////////////////////////////////////////////////////
    for (int i=0;i<transmission->ThrottleSize;i++)                           {
      if (transmission->Displays.contains(i))                                {
        (*(transmission->Displays[i])) = false                               ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    if (transmission->Displays.contains(0))                                  {
      (*(transmission->Displays[0])) = true                                  ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    transmission -> Unlock ( )                                               ;
  } else                                                                     {
    N::printf("Detail error",true,true) ;
  }                                                                          ;
}

void N::VideoStreaming::Locate(void)
{
  TUID p = position - 1000                                       ;
  TUID s = current ( )                                           ;
  TUID d = 60 * 1000                                             ;
  TUID z = 0                                                     ;
  if ( p < 0 ) p = 0                                             ;
  ////////////////////////////////////////////////////////////////
  if ( NotNull(conduit) )                                        {
    conduit -> Output . Situation = CiosAudio::StreamIO::Stalled ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  s = p - s                                                      ;
  if ( s > 0 )                                                   {
    s /= d                                                       ;
    if ( s > 0 )                                                 {
      z = s                                                      ;
      s = current ( )                                            ;
    }                                                            ;
  } else                                                         {
    s  = -s                                                      ;
    s /= d                                                       ;
    if ( s > 0 )                                                 {
      z = s                                                      ;
      s = current ( )                                            ;
      d = -d                                                     ;
    }                                                            ;
  }                                                              ;
  if ( z > 0 )                                                   {
    for (int i=0;i<z;i++) RawSeek ( s + ( i * d ) )              ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( RawSeek ( p        ) )                                    {
    Parking    ( position )                                      ;
    Precache   (          )                                      ;
  } else                                                         {
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( NotNull(conduit) )                                        {
    conduit -> Output . Situation = CiosAudio::StreamIO::Started ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  step = LastStep                                                ;
  if ( 52 == step ) step = 51                                    ;
}

void N::VideoStreaming::addWave(int channel,WaveUniverse * wave)
{
  mutex [ 1 ] . lock   ( )        ;
  waves       << wave             ;
  waveChannels [ wave ] = channel ;
  mutex [ 1 ] . unlock ( )        ;
}

void N::VideoStreaming::removeWave(WaveUniverse * wave)
{
  if ( ! waves . contains ( wave ) ) return  ;
  int index                                  ;
  mutex [ 1 ] . lock   ( )                   ;
  index = waves . indexOf ( wave )           ;
  if ( index >= 0 ) waves . takeAt ( index ) ;
  waveChannels . remove ( wave )             ;
  mutex [ 1 ] . unlock ( )                   ;
}

bool N::VideoStreaming::hasWave(void)
{
  return Booleans [ "hasWave" ] ;
}

void N::VideoStreaming::FeedWave(void)
{
  if ( ! Booleans [ "hasWave" ] ) return            ;
  ///////////////////////////////////////////////////
  int ws = waves . count ( )                        ;
  if ( ws <= 0 )                                    {
    Booleans [ "hasWave" ] = false                  ;
    return                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  int    channels = AudioBuffer . Channels ( )      ;
  int    s        = AudioBuffer . Bits  (   ) / 8   ;
  int    b        = s * channels                    ;
  int    bpf      = AudioBuffer . BytesPerFrame ( ) ;
  int    bs       = ChannelBlobs [ -1 ] . size ( )  ;
  int    f                                          ;
  char * d                                          ;
  if ( bs <= 0 ) return                             ;
  mutex [ 1 ] . lock ( )                            ;
  mutex [ 0 ] . lock ( )                            ;
  f = bs / bpf                                      ;
  d = ChannelBlobs [ -1 ] . data ( )                ;
  ///////////////////////////////////////////////////
  for (int i = 0 ; i < channels ; i++ )             {
    if ( ! ChannelBlobs . contains ( i ) )          {
      ChannelBlobs [ i ] = QByteArray ( )           ;
    }                                               ;
    char * v = d + ( s * i )                        ;
    for (int j=0;j<f;j++)                           {
      ChannelBlobs [ i ] . append ( v , s )         ;
      v += b                                        ;
    }                                               ;
  }                                                 ;
  ChannelBlobs [ -1 ] . clear ( )                   ;
  mutex [ 0 ] . unlock ( )                          ;
  ///////////////////////////////////////////////////
  for (int i = 0 ; i < ws ; i++ )                   {
    WaveUniverse * wu = waves [ i ]                 ;
    if ( waveChannels . contains ( wu ) )           {
      int ch = waveChannels [ wu ]                  ;
      if ( ch >= 0 )                                {
        wu -> Feeding ( ChannelBlobs [ ch ] )       ;
      }                                             ;
    }                                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  for (int i=0;i<channels;i++)                      {
    ChannelBlobs [ i ] . clear ( )                  ;
  }                                                 ;
  mutex [ 1 ] . unlock ( )                          ;
}
