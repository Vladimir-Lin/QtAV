#include <AV>

//#include "libav.hpp"

#define DSTBITS 32
#define DSTFMT  AV_SAMPLE_FMT_S32

typedef struct {

  AVFormatContext   *  pFormatCtx      ;
  AVCodecContext    *  vCodecCtx       ;
  AVCodecContext    *  aCodecCtx       ;
  AVStream          *  vStream         ;
  AVStream          *  aStream         ;
  AVCodec           *  vCodec          ;
  AVCodec           *  aCodec          ;
  AVFrame           *  pFrame          ;

  struct SwsContext *  ConvertCtx      ;

  int                  audioStream     ;
  int                  videoStream     ;

  int                  BytesPerSecond  ;
  int                  BytesPerSample  ;
  int                  SampleBits      ;
  int                  PlayBits        ;
  bool                 Float           ;
  int                  AudioBufferSize ;
  int                  AudioLimit      ;
  int                  AudioSize       ;
  int                  AudioIndex      ;
  long long            AudioBytes      ;
  long long            ActualBytes     ;
  int                  DecodeLength    ;
  unsigned char     *  DecodeBuffer    ;
  unsigned char     *  AudioBuffer     ;
  bool                 Streaming       ;
  ReSampleContext   *  ResampleCtx     ;
  unsigned char     *  ResampleBuffer  ;
  int                  DstPerSample    ;
  int                  SampleSize      ;
  int                  SampleBucket    ;

  QDateTime            VideoFresh      ;

  unsigned char     *  data     [4]    ;
  int                  linesize [4]    ;

} PrivateVideo                         ;

nMediaDecoder:: nMediaDecoder ( void              )
             :  PrivatePacket ( NULL              )
             ,  PlayRatio     ( 1,1               )
             ,  ResampleWay   ( SWS_FAST_BILINEAR )
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivatePacket = Create() ;
  Length . setHMS(0,0,0,0) ;
#endif
}

nMediaDecoder::~nMediaDecoder (void)
{
//  Destroy();
}

void * nMediaDecoder::Create(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (PrivatePacket!=NULL) Destroy()     ;
  PrivateVideo * vi = new PrivateVideo() ;
  PrivatePacket = (void *)vi             ;
  Clear           (      )               ;
  PrivatePacket = NULL                   ;
  return (void *)vi                      ;
#endif
  return NULL ;
}

void nMediaDecoder::Destroy(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  delete vi                                         ;
  PrivatePacket = NULL                              ;
#endif
}

void nMediaDecoder::Clear(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket;
  vi->pFormatCtx      = NULL  ;
  vi->vCodecCtx       = NULL  ;
  vi->aCodecCtx       = NULL  ;
  vi->vStream         = NULL  ;
  vi->aStream         = NULL  ;
  vi->vCodec          = NULL  ;
  vi->aCodec          = NULL  ;
  vi->pFrame          = NULL  ;
  vi->ConvertCtx      = NULL  ;
  vi->audioStream     = -1    ;
  vi->videoStream     = -1    ;
  vi->BytesPerSecond  = 0     ;
  vi->SampleBits      = 0     ;
  vi->PlayBits        = 0     ;
  vi->Float           = false ;
  vi->AudioBufferSize = 0     ;
  vi->AudioLimit      = 0     ;
  vi->AudioSize       = 0     ;
  vi->AudioIndex      = 0     ;
  vi->ActualBytes     = 0     ;
  vi->DecodeLength    = 0     ;
  vi->DecodeBuffer    = NULL  ;
  vi->AudioBuffer     = NULL  ;
  vi->Streaming       = false ;
  vi->ResampleCtx     = NULL  ;
  vi->ResampleBuffer  = NULL  ;
  vi->DstPerSample    = 0     ;
  vi->data    [0]     = NULL  ;
  vi->data    [1]     = NULL  ;
  vi->data    [2]     = NULL  ;
  vi->data    [3]     = NULL  ;
  vi->linesize[0]     = 0     ;
  vi->linesize[1]     = 0     ;
  vi->linesize[2]     = 0     ;
  vi->linesize[3]     = 0     ;
  Resample            = false ;
  Scaling             = false ;
  vi->VideoFresh      = QDateTime::currentDateTime() ;
}

QString nMediaDecoder::filePath(void) const
{
  return VideoFile ;
}

QByteArray nMediaDecoder::NativeFile(void)
{
  QByteArray   Native                                     ;
  QTextCodec * codec = QTextCodec::codecForLocale()       ;
  if (codec==NULL) return Native                          ;
  QString localname = QDir::toNativeSeparators(VideoFile) ;
  Native = codec->fromUnicode(localname)                  ;
  return Native                                           ;
}

void nMediaDecoder::setFile(QString filename)
{
  VideoFile = filename ;
}

bool nMediaDecoder::open(QString filename)
{
  setFile(filename) ;
  return open()     ;
}

bool nMediaDecoder::open(void)
{
  bool success = true                              ;
  if (            !OpenVideo   ()) success = false ;
  if ( success && !FindStreams ()) success = false ;
  if ( success && !OpenCodecs  ()) success = false ;
  if ( success && !PrepareVideo()) success = false ;
  if (!success                   ) CloseVideo()    ;
  return success                                   ;
}

bool nMediaDecoder::close(void)
{
  if (!CloseVideo()) return false ;
  return true ;
}

/////////////////////////////////////////////////////////////////////////////

long long nMediaDecoder::Timestamp(void * data)
{
  AVPacket     * packet = (AVPacket     *)data          ;
  PrivateVideo * vi     = (PrivateVideo *)PrivatePacket ;
  ///////////////////////////////////////////////////////
  if        (packet->stream_index==vi->audioStream)     {
    long long pts = Timestamp(packet->pts,VideoRatio)   ;
    long long dts = Timestamp(packet->dts,VideoRatio)   ;
    if (pts!=AV_NOPTS_VALUE) return pts                 ;
    if (dts!=AV_NOPTS_VALUE) return dts                 ;
  } else if (packet->stream_index==vi->videoStream)     {
    long long pts = Timestamp(packet->pts,AudioRatio)   ;
    long long dts = Timestamp(packet->dts,AudioRatio)   ;
    if (pts!=AV_NOPTS_VALUE) return pts                 ;
    if (dts!=AV_NOPTS_VALUE) return dts                 ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  return AV_NOPTS_VALUE                                 ;
}

long long nMediaDecoder::Timestamp(long long ts,QSize & Size)
{
  if (ts==AV_NOPTS_VALUE) return AV_NOPTS_VALUE ;
  int num = Size.width ()                       ;
  int den = Size.height()                       ;
  if (num<=0)             return AV_NOPTS_VALUE ;
  if (den<=0)             return AV_NOPTS_VALUE ;
  ts *= num                                     ;
  ts *= AV_TIME_BASE                            ;
  ts /= den                                     ;
  return ts                                     ;
}

long long nMediaDecoder::Timestamp(QDateTime Start,QDateTime Now)
{
  long long ts = Start.msecsTo(Now) ;
  ts *= 1000                        ;
  int num = PlayRatio.width ()      ;
  int den = PlayRatio.height()      ;
  ts     *= num                     ;
  ts     /= den                     ;
  return ts                         ;
}

long long nMediaDecoder::Timestamp(QDateTime Start)
{
  QDateTime Now = QDateTime::currentDateTime() ;
  return Timestamp(Start,Now)                  ;
}

QDateTime nMediaDecoder::Seek(long long ts)
{
  QDateTime Now = QDateTime::currentDateTime() ;
  ts /= 1000                                   ;
  Now = Now.addMSecs(-ts)                      ;
  return Now                                   ;
}

/////////////////////////////////////////////////////////////////////////////

bool nMediaDecoder::OpenVideo(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  QFile          F (VideoFile)                      ;
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;

  Clear();

  if (!F.exists()) return false ;

  QByteArray Native = NativeFile();

  if (::av_open_input_file(&vi->pFormatCtx,Native.constData(),NULL,0,NULL)!=0) return false;
  if (::av_find_stream_info(vi->pFormatCtx)<0) return false;
#endif
  return true;
}

bool nMediaDecoder::CloseVideo(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return false                        ;
  if (vi->ResampleCtx!=NULL                       ) {
    ::audio_resample_close(vi->ResampleCtx)         ;
    delete [] vi->ResampleBuffer                    ;
  }                                                 ;
  if (vi->vCodecCtx     !=NULL && vi->vCodec!=NULL) ::avcodec_close       (vi->vCodecCtx ) ;
  if (vi->aCodecCtx     !=NULL && vi->aCodec!=NULL) ::avcodec_close       (vi->aCodecCtx ) ;
  if (vi->pFormatCtx    !=NULL                    ) ::av_close_input_file (vi->pFormatCtx) ;
  if (vi->pFrame        !=NULL                    ) ::av_free             (vi->pFrame    ) ;
  if (vi->DecodeBuffer  !=NULL                    ) delete [] vi->DecodeBuffer             ;
  vi->ResampleCtx    = NULL ;
  vi->ResampleBuffer = NULL ;
  vi->DecodeBuffer   = NULL ;
  vi->vStream        = NULL ;
  vi->aStream        = NULL ;
  vi->vCodec         = NULL ;
  vi->aCodec         = NULL ;
  vi->vCodecCtx      = NULL ;
  vi->aCodecCtx      = NULL ;
  vi->pFormatCtx     = NULL ;
  vi->pFrame         = NULL ;
#endif
  return true               ;
}

bool nMediaDecoder::FindStreams(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket;
  if (vi==NULL) return false ;

  vi->videoStream = -1 ;
  vi->audioStream = -1 ;

  for (int i=0;i<(int)vi->pFormatCtx->nb_streams;i++) {
    if (vi->pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO &&
        vi->videoStream < 0) vi->videoStream = i;
    if (vi->pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO &&
        vi->audioStream < 0) vi->audioStream = i;
  };

  if (vi->videoStream==-1) return false ;
  if (vi->audioStream==-1) return false ;

  vi->vStream = vi->pFormatCtx->streams[vi->videoStream] ;
  vi->aStream = vi->pFormatCtx->streams[vi->audioStream] ;

  if (vi->vStream==NULL) return false ;
  if (vi->aStream==NULL) return false ;

  vi->vCodecCtx = vi->vStream->codec;
  vi->aCodecCtx = vi->aStream->codec;

  if (vi->vCodecCtx==NULL) return false ;
  if (vi->aCodecCtx==NULL) return false ;

  VideoRatio . setWidth  (vi->vStream->time_base.num) ;
  VideoRatio . setHeight (vi->vStream->time_base.den) ;

  AudioRatio . setWidth  (vi->aStream->time_base.num) ;
  AudioRatio . setHeight (vi->aStream->time_base.den) ;
#endif
  return true;
}

bool nMediaDecoder::OpenCodecs(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket;
  if (vi==NULL) return false ;

  vi->vCodec    = ::avcodec_find_decoder(vi->vCodecCtx->codec_id);
  vi->aCodec    = ::avcodec_find_decoder(vi->aCodecCtx->codec_id);

  if (vi->vCodec==NULL) return false ;
  if (vi->aCodec==NULL) return false ;

  if (::avcodec_open(vi->vCodecCtx,vi->vCodec)<0) return false ;
  if (::avcodec_open(vi->aCodecCtx,vi->aCodec)<0) return false ;
#endif
  return true;
}

bool nMediaDecoder::PrepareVideo(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket                         ;
  if (vi==NULL) return false                                                ;
  ///////////////////////////////////////////////////////////////////////////
  Dimension . setWidth  (vi->vCodecCtx->width )                             ;
  Dimension . setHeight (vi->vCodecCtx->height)                             ;
  ///////////////////////////////////////////////////////////////////////////
  long long duration = vi->pFormatCtx->duration                             ;
  QTime    T(0,0,0,0)                                                       ;
  T      = T.addMSecs(duration/1000)                                        ;
  Length = T                                                                ;
  ///////////////////////////////////////////////////////////////////////////
  int Bits = 0                                                              ;
  ///////////////////////////////////////////////////////////////////////////
  vi->BytesPerSecond  = vi->aCodecCtx->sample_rate                          ;
  vi->BytesPerSecond *= vi->aCodecCtx->channels                             ;
  ///////////////////////////////////////////////////////////////////////////
  int sampleFormat = vi->aCodecCtx->sample_fmt                              ;
  int sampleRate   = vi->aCodecCtx->sample_rate                             ;
  int channels     = vi->aCodecCtx->channels                                ;
  ///////////////////////////////////////////////////////////////////////////
  switch (sampleFormat)                                                     {
    case AV_SAMPLE_FMT_U8                                                   :
      Bits                =  8                                              ;
      vi->PlayBits        =  8                                              ;
    break                                                                   ;
    case AV_SAMPLE_FMT_S16                                                  :
      Bits                = 16                                              ;
      vi->PlayBits        = 16                                              ;
      vi->BytesPerSecond *=  2                                              ;
    break                                                                   ;
    case AV_SAMPLE_FMT_S32                                                  :
      Bits                = 32                                              ;
      vi->PlayBits        = 32                                              ;
      vi->BytesPerSecond *=  4                                              ;
    break                                                                   ;
    case AV_SAMPLE_FMT_FLT                                                  :
      Bits                = 32                                              ;
      vi->PlayBits        = 32                                              ;
      vi->BytesPerSecond *=  4                                              ;
      vi->Float           = true                                            ;
    break                                                                   ;
    case AV_SAMPLE_FMT_DBL                                                  :
      Bits                = sizeof(double) * 8                              ;
      vi->PlayBits        = DSTBITS                                         ;
      vi->BytesPerSecond *= sizeof(double)                                  ;
      vi ->ResampleCtx = av_audio_resample_init                             (
        channels   , channels                                               ,
        sampleRate , sampleRate                                             ,
        AV_SAMPLE_FMT_DBL                                                   ,
        DSTFMT                                                              ,
        0, 0, 0, 0                                                        ) ;
      vi->SampleSize = ::av_get_bits_per_sample_format(AV_SAMPLE_FMT_DBL)/8 ;
    break                                                                   ;
    case AV_SAMPLE_FMT_NB                                                   :
      Bits                =  0                                              ;
      vi->PlayBits        =  0                                              ;
    break                                                                   ;
    default                                                                 :
      Bits                =  0                                              ;
      vi->PlayBits        =  0                                              ;
    break                                                                   ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  vi->SampleBits     = Bits                                                 ;
  vi->PlayBits      /= 8                                                    ;
  vi->BytesPerSample = vi->BytesPerSecond / sampleRate                      ;
  vi->pFrame         = avcodec_alloc_frame()                                ;
  vi->DecodeBuffer   = new unsigned char[AVCODEC_MAX_AUDIO_FRAME_SIZE*16]   ;
  memset(vi->DecodeBuffer,0,AVCODEC_MAX_AUDIO_FRAME_SIZE*16)                ;
  ///////////////////////////////////////////////////////////////////////////
  if (NotNull(vi->ResampleCtx))                                             {
    vi->ResampleBuffer = new unsigned char[AVCODEC_MAX_AUDIO_FRAME_SIZE*16] ;
    vi->SampleBucket   = vi->SampleSize * channels                          ;
    memset(vi->ResampleBuffer,0,AVCODEC_MAX_AUDIO_FRAME_SIZE*16)            ;
  }                                                                         ;
  QTextCodec * codec = QTextCodec::codecForLocale()                         ;
  Name = codec->toUnicode(vi->pFormatCtx->title)                            ;
  ///////////////////////////////////////////////////////////////////////////
#endif
  return true                                                               ;
}

bool nMediaDecoder::setAudioBuffer(int seconds,QByteArray & AudioBuffer)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket                    ;
  if (vi==NULL) return false                                           ;
  //////////////////////////////////////////////////////////////////////
  vi->Streaming        = false                                         ;
  vi->AudioSize        = 0                                             ;
  vi->AudioIndex       = 0                                             ;
  vi->AudioBufferSize  = vi->BytesPerSecond                            ;
  vi->AudioBufferSize *= seconds                                       ;
  vi->AudioLimit       = vi->AudioBufferSize / 2                       ;
  //////////////////////////////////////////////////////////////////////
  AudioBuffer          . resize (vi->AudioBufferSize)                  ;
  vi->AudioBuffer      = (unsigned char *)AudioBuffer . data()         ;
  memset (vi->AudioBuffer,0,vi->AudioBufferSize)                       ;
  //////////////////////////////////////////////////////////////////////
  return true                                                          ;
}

bool nMediaDecoder::setVideoOutput(QSize screen,unsigned char ** data,int * linesize)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return false                        ;
  Screen = screen                                   ;
  vi->data    [0] = data    [0]                     ;
  vi->data    [1] = data    [1]                     ;
  vi->data    [2] = data    [2]                     ;
  vi->data    [3] = data    [3]                     ;
  vi->linesize[0] = linesize[0]                     ;
  vi->linesize[1] = linesize[1]                     ;
  vi->linesize[2] = linesize[2]                     ;
  vi->linesize[3] = linesize[3]                     ;
  vi->ConvertCtx  = ::sws_getContext                (
                      Dimension . width  ()         ,
                      Dimension . height ()         ,
                      vi->vCodecCtx->pix_fmt        ,
                      Screen    . width  ()         ,
                      Screen    . height ()         ,
                      PIX_FMT_YUV420P               ,
                      ResampleWay                   ,
//      SWS_FAST_BILINEAR , SWS_BICUBIC , SWS_LANCZOS
                      NULL                          ,
                      NULL                          ,
                      NULL                        ) ;
#endif
  return true                                       ;
}

/////////////////////////////////////////////////////////////////////////////

bool nMediaDecoder::FreeBuffer(QLinkedList<void *> & List)
{
  QLinkedList<void *>::iterator it = List.begin() ;
  if (it==List.end()) return false                ;
  while (it!=List.end())                          {
    AVPacket * Packet = (AVPacket *)(*it)         ;
    Timestamps.remove((void *)Packet)             ;
    if (Packet!=NULL) av_free_packet(Packet)      ;
    it++                                          ;
  }                                               ;
  List.clear()                                    ;
  return true                                     ;
}

long long nMediaDecoder::BufferSize(QLinkedList<void *> & List)
{
  long long total = 0                                       ;
  QLinkedList<void *>::iterator it = List.begin()           ;
  if (it==List.end()) return total                          ;
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket         ;
  while (it!=List.end())                                    {
    AVPacket * Packet = (AVPacket *)(*it)                   ;
    if (Packet!=NULL)                                       {
      if        (Packet->stream_index==vi->audioStream)     {
        total += Packet->size                               ;
      } else if (Packet->stream_index==vi->videoStream)     {
        total += Packet->size                               ;
      }                                                     ;
    }                                                       ;
    it++                                                    ;
  }                                                         ;
  return total                                              ;
}

bool nMediaDecoder::Buffer(QLinkedList<void *> & Video ,
                           QLinkedList<void *> & Audio ,
                           QDateTime Start             ,
                           QDateTime Now               ,
                           int seconds                 )
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket                    ;
  if (vi==NULL) return false                                           ;
  Now = Now.addSecs(seconds)                                           ;
  long long TargetTime = Timestamp(Start,Now)                          ; // Target buffering time
  void     * vp                                                        ;
  void     * ap                                                        ;
  AVPacket * VideoPacket                                               ;
  AVPacket * AudioPacket                                               ;
  bool   stopbuffer = false                                            ;
  do                                                                   { // Check the lastest packets
    if (Video.count()>0 && Audio.count()>0)                            {
      vp = Video.last()                                                ;
      ap = Audio.last()                                                ;
      VideoPacket = (AVPacket *)vp                                     ;
      AudioPacket = (AVPacket *)ap                                     ;
      long long vts  = Timestamp((void *)VideoPacket)                  ;
      long long ats  = Timestamp((void *)AudioPacket)                  ;
      if (vts !=AV_NOPTS_VALUE && ats!=AV_NOPTS_VALUE)                 {
        if (vts>TargetTime && ats>TargetTime) stopbuffer = true        ;
      }                                                                ;
    }                                                                  ;
    if (!stopbuffer)                                                   { // Read packet
      AVPacket * Packet = new AVPacket()                               ;
      int rt =:: av_read_frame(vi->pFormatCtx,Packet)                  ;
      if (rt==0)                                                       {
        bool queue = false                                             ;
        if        (Packet->stream_index==vi->audioStream)              {
          queue = true                                                 ;
          Audio.append((void *)Packet)                                 ;
        } else if (Packet->stream_index==vi->videoStream)              {
          queue = true                                                 ;
          Video.append((void *)Packet)                                 ;
        }                                                              ;
        if (queue)                                                     {
          Timestamps[Packet] = Timestamp(Packet)                       ;
        }                                                              ;
      } else                                                           {
        ::av_free_packet(Packet)                                       ;
        switch (rt)                                                    {
          case AVERROR_INVALIDDATA                                     :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_IO                                              :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_NOENT                                           :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_NOFMT                                           :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_NOMEM                                           :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_NOTSUPP                                         :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_NUMEXPECTED                                     :
            stopbuffer = true                                          ;
          return false                                                 ;
          case AVERROR_EOF                                             :
            stopbuffer = true                                          ;
          break                                                        ;
          default                                                      :
            stopbuffer = true                                          ;
          return false                                                 ;
        }                                                              ;
      }                                                                ;
    }                                                                  ;
  } while (!stopbuffer)                                                ;
#endif
  return true                                                          ;
}

////////////////////////////////////////////////////////////////////////

void nMediaDecoder::FreshVideo(QLinkedList<void *> & List,long long cts)
{
  if (List.count()<=0) return                        ;
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket  ;
  if (vi==NULL) return                               ;
  QDateTime CC = QDateTime::currentDateTime()        ;
  long long ms = vi->VideoFresh.msecsTo(CC)          ;
  if (ms>=0 && ms<15) return                         ;
  void * it = List.first()                           ;
  AVPacket * Packet = (AVPacket *)it                 ;
  if (Packet==NULL) return                           ;
  if (!Timestamps.contains(Packet)) return           ;
  long long pts = Timestamps[Packet]                 ;
  bool playit   = false                              ;
  if (pts==AV_NOPTS_VALUE)                           {
    long long dts = Packet->dts                      ;
    if (dts==AV_NOPTS_VALUE) playit = true ;      else
    if (cts>=dts) playit = true                      ;
  } else
  if (cts>=pts) playit = true                        ;
  if (!playit) return                                ;
  vi->VideoFresh = QDateTime::currentDateTime()      ;
  PutVideo(Packet)                                   ;
  List.takeFirst()                                   ;
}

void nMediaDecoder::FreshAudio(QLinkedList<void *> & List,long long cts,int seconds)
{
  if (List.count()<=0) return                        ;
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket  ;
  if (vi==NULL) return                               ;
  int audio_buf = seconds / 2                        ;
  long long ats = audio_buf                          ;
  ats *= AV_TIME_BASE                                ;
  cts += ats                                         ;
  void * it = List.first()                           ;
  AVPacket * Packet = (AVPacket *)it                 ;
  if (Packet==NULL) return                           ;
  long long pts = Timestamps[Packet]                 ;
  bool playit   = false                              ;
  if (pts==AV_NOPTS_VALUE)                           {
    long long dts = Packet->dts                      ;
    if (dts==AV_NOPTS_VALUE) playit = true ;      else
    if (cts>=dts) playit = true                      ;
  } else
  if (cts>=pts) playit = true                        ;
  if (!playit) return                                ;
  PutAudio(Packet)                                   ;
  List.takeFirst()                                   ;
}

int nMediaDecoder::Idle(QLinkedList<void *> & Video,QLinkedList<void *> & Audio,long long cts)
{
  if (Video.count()<=0) return 0                           ;
  if (Audio.count()<=0) return 0                           ;
  AVPacket * vPacket = (AVPacket *)Video.first()           ;
  AVPacket * aPacket = (AVPacket *)Audio.first()           ;
  long long vts = AV_NOPTS_VALUE                           ;
  long long ats = AV_NOPTS_VALUE                           ;
  long long rts = AV_NOPTS_VALUE                           ;
  if (Timestamps.contains(vPacket))                        {
    vts = Timestamps[vPacket]                              ;
  }                                                        ;
  if (Timestamps.contains(aPacket))                        {
    ats = Timestamps[aPacket]                              ;
  }                                                        ;
  if (vts==AV_NOPTS_VALUE && ats==AV_NOPTS_VALUE) return 0 ;
  if (vts!=AV_NOPTS_VALUE) rts = vts                       ;
  if (ats!=AV_NOPTS_VALUE && rts>ats) rts = ats            ;
  if (rts==AV_NOPTS_VALUE) return 0                        ;
  if (cts>rts) return 0                                    ;
  rts -= cts                                               ;
  rts /= 1000                                              ;
  rts -= 1                                                 ;
  if (rts<=0) return 0                                     ;
  return (int)rts                                          ;
}

void nMediaDecoder::Debug(QString message)
{
}

/////////////////////////////////////////////////////////////////////////////

int nMediaDecoder::DecodeAudio(void * data)
{
  AVPacket     * packet = (AVPacket     *)data          ;
  PrivateVideo * vi     = (PrivateVideo *)PrivatePacket ;
  ///////////////////////////////////////////////////////
  unsigned char * pdata    = packet->data               ;
  int             psize    = packet->size               ;
  int             audiolen = 0                          ;
  int             LEN      = 0                          ;
  int             aslen                                 ;
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  ///////////////////////////////////////////////////////
  if (psize==0)                                         {
    if (Timestamps.contains(packet))                    {
      Timestamps.remove(packet)                         ;
    }                                                   ;
    av_free_packet(packet)                              ;
    return 0                                            ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  unsigned char * DB = vi->DecodeBuffer                 ;
  if (NotNull(vi->ResampleCtx)) DB = vi->ResampleBuffer ;
                           else DB = vi->DecodeBuffer   ;
  do                                                    {
    LEN = AVCODEC_MAX_AUDIO_FRAME_SIZE                  ;
    aslen  = avcodec_decode_audio2                      (
               vi->aCodecCtx                            ,
               (int16_t *)DB                            ,
               &LEN                                     ,
               pdata                                    ,
               psize                                  ) ;
    if (aslen<0)                                        {
      return 0                                          ;
    }                                                   ;
    if (aslen>0)                                        {
      pdata += aslen                                    ;
      psize -= aslen                                    ;
    }                                                   ;
    if (LEN>0 && LEN<AVCODEC_MAX_AUDIO_FRAME_SIZE)      {
      audiolen += LEN                                   ;
    }                                                   ;
  } while (aslen>0 && psize>0)                          ;
  ///////////////////////////////////////////////////////
  if (NotNull(vi->ResampleCtx))                         {
    int bucket = audiolen                               ;
    bucket    /= vi->SampleBucket                       ;
    bucket     = ::audio_resample                       (
      vi->ResampleCtx                                   ,
      (short *)vi->DecodeBuffer                         ,
      (short *)vi->ResampleBuffer                       ,
      bucket                                          ) ;
    bucket  *= vi->PlayBits                             ;
    bucket  *= vi->aCodecCtx->channels                  ;
    audiolen = bucket                                   ;
  }                                                     ;
#endif
  ///////////////////////////////////////////////////////
  return audiolen                                       ;
}

bool nMediaDecoder::BufferAudio(int length)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket     ;
  if (vi==NULL) return false                            ;
//  if (length>AVCODEC_MAX_AUDIO_FRAME_SIZE) return false ;
//  if (vi->AudioBufferSize<=0) return false              ;
  int ds = vi->AudioBufferSize - vi->AudioSize          ;
  if (ds>=length)                                       {
    unsigned char * s = vi->AudioBuffer + vi->AudioSize ;
    unsigned char * d = vi->DecodeBuffer                ;
    memcpy(s,d,length)                                  ;
    vi->AudioSize += length                             ;
    vi->AudioSize %= vi->AudioBufferSize                ;
  } else                                                {
    unsigned char * s = vi->AudioBuffer + vi->AudioSize ;
    unsigned char * d = vi->DecodeBuffer                ;
    memcpy(s,d,ds)                                      ;
    length -= ds                                        ;
    s = vi->AudioBuffer                                 ;
    d = vi->DecodeBuffer+ds                             ;
    memcpy(s,d,length)                                  ;
    vi->AudioSize  = length                             ;
    vi->AudioSize %= vi->AudioBufferSize                ;
  }                                                     ;
  return true                                           ;
}

bool nMediaDecoder::FeedAudio(unsigned char * stream,int length)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket        ;
  if (vi==NULL) return false                               ;
  int ds = vi->AudioBufferSize - vi->AudioIndex            ;
  if (ds>=length)                                          {
    unsigned char * s = vi->AudioBuffer + vi->AudioIndex   ;
    memcpy(stream,s,length)                                ;
    vi->AudioIndex += length                               ;
    vi->AudioIndex %= vi->AudioBufferSize                  ;
  } else                                                   {
    unsigned char * s = vi->AudioBuffer + vi->AudioIndex   ;
    memcpy(stream,s,ds)                                    ;
    length -= ds                                           ;
    memcpy(stream+ds,vi->AudioBuffer,length)               ;
    vi->AudioIndex = length                                ;
    vi->AudioIndex %= vi->AudioBufferSize                  ;
  }                                                        ;
  return true                                              ;
}

int nMediaDecoder::SampleRate(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return 0                            ;
  return vi->aCodecCtx->sample_rate                 ;
}

int nMediaDecoder::SampleBits(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return 0                            ;
  return vi->SampleBits                             ;
}

int nMediaDecoder::PlayBits(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return 0                            ;
  return vi->PlayBits                               ;
}

int nMediaDecoder::Channels(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return 0                            ;
  return vi->aCodecCtx->channels                    ;
}

bool nMediaDecoder::isFloat(void)
{
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return 0                            ;
  return vi->Float                                  ;
}

/////////////////////////////////////////////////////////////////////////////

bool nMediaDecoder::DecodeVideo(void * data)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  AVPacket     * packet = (AVPacket     *)data          ;
  PrivateVideo * vi     = (PrivateVideo *)PrivatePacket ;
  ///////////////////////////////////////////////////////

  unsigned char * pdata         = packet->data ;
  int             bytes         = packet->size ;
  int             dbyte         = 0            ;
  int             frameFinished = 0            ;

  if (bytes==0)                                                {
    if (Timestamps.contains(packet)) Timestamps.remove(packet) ;
    av_free_packet(packet)                                     ;
    return false                                               ;
  }                                                            ;

  ///////////////////////////////////////////////////////////////
  // Decode Video Frame
  ///////////////////////////////////////////////////////////////

  do                                 {
    dbyte = ::avcodec_decode_video   (
              vi->vCodecCtx          ,
              vi->pFrame             ,
              &frameFinished         ,
              pdata                  ,
              bytes                ) ;
    if (dbyte>0)                     {
      bytes -= dbyte                 ;
      pdata += dbyte                 ;
    }                                ;
  } while (dbyte>0 && bytes>0)       ;
  if (frameFinished==0) return false ;
#endif
  return true ;
}

bool nMediaDecoder::DisplayVideo(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  PrivateVideo * vi = (PrivateVideo *)PrivatePacket ;
  if (vi==NULL) return false                        ;
  sws_scale                                         (
    vi->ConvertCtx                                  ,
    vi->pFrame->data                                ,
    vi->pFrame->linesize                            ,
    0                                               ,
    Screen . height()                               ,
    vi->data                                        ,
    vi->linesize                                  ) ;
#endif
  return true                                       ;
}
