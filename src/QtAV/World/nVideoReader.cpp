#include <qtav.h>

N::VideoReader:: VideoReader ( void  )
               : VideoData   (       )
               , AvDecoder   (       )
               , going       ( false )
{
}

N::VideoReader::~VideoReader (void)
{
}

QString N::VideoReader::VideoPath(void) const
{
  return VideoData::Filename ;
}

void N::VideoReader::setFilename(QString filename)
{
  VideoData::Filename = filename ;
  AvDecoder::Filename = filename ;
}

bool N::VideoReader::Peep(void)
{
  nKickOut ( !StartVideo(VideoData::Filename) , false ) ;
  int CHANNELS = AvDecoder :: Channels       ( )        ;
  int SAMPLE   = AvDecoder :: AvSampleFormat ( )        ;
  int BITS     = Acoustics :: AvBits   (SAMPLE)         ;
  int FORMAT   = Acoustics :: AvFormat (SAMPLE)         ;
  AvDecoder :: Initialize ( &Packet )                   ;
  AudioBuffer . reset      (         )                  ;
  AudioBuffer . set                                     (
    AvDecoder :: AvSampleRate ( )                       ,
    BITS                                                ,
    CHANNELS                                            ,
    (N::Acoustics::SampleFormat)FORMAT                ) ;
  VideoData :: Dimension = QSize (  Width(),Height()  ) ;
  VideoData :: Duration  = AvDecoder :: Duration ( )    ;
  VideoData :: isAudio   = AvDecoder::isAudio           ;
  VideoData :: isVideo   = AvDecoder::isVideo           ;
  VideoData :: Frames    = AvDecoder::Frames ( )        ;
  VideoData :: Duration /= 1000                         ;
  AvDecoder :: Close ( )                                ;
  return true                                           ;
}

bool N::VideoReader::prepare(void)
{
  going = false                                         ;
  nKickOut ( !StartVideo(VideoData::Filename) , false ) ;
  int CHANNELS = AvDecoder :: Channels       ( )        ;
  int SAMPLE   = AvDecoder :: AvSampleFormat ( )        ;
  int BITS     = Acoustics :: AvBits   (SAMPLE)         ;
  int FORMAT   = Acoustics :: AvFormat (SAMPLE)         ;
  AvDecoder :: Initialize ( &Packet )                   ;
  AudioBuffer . reset      (         )                  ;
  AudioBuffer . set                                     (
    AvDecoder :: AvSampleRate ( )                       ,
    BITS                                                ,
    CHANNELS                                            ,
    (N::Acoustics::SampleFormat)FORMAT                ) ;
  AudioCache  . reset      (         )                  ;
  AudioCache  . set                                     (
    AvDecoder :: AvSampleRate ( )                       ,
    BITS                                                ,
    CHANNELS                                            ,
    (N::Acoustics::SampleFormat)FORMAT                ) ;
  if (AudioCache.BytesPerSecond()>0)                    {
    char * dd = new char [AudioCache.BytesPerSecond()]  ;
    AudioCache.add(dd,AudioCache.BytesPerSecond())      ;
    delete [] dd                                        ;
  }                                                     ;
  VideoData :: Dimension = QSize (  Width(),Height()  ) ;
  VideoData :: Duration  = AvDecoder :: Duration ( )    ;
  VideoData :: isAudio   = AvDecoder::isAudio           ;
  VideoData :: isVideo   = AvDecoder::isVideo           ;
  VideoData :: Frames    = AvDecoder::Frames ( )        ;
  VideoData :: Duration /= 1000                         ;
  going = true                                          ;
  return true                                           ;
}

bool N::VideoReader::decode(void)
{
  int Ret = ReadPacket(Packet)                                 ;
  switch (Ret)                                                 {
    case AVERROR_BUFFER_TOO_SMALL                              :
    case AVERROR_EXTERNAL                                      :
    case AVERROR_BUG2                                          :
    case AVERROR_BSF_NOT_FOUND                                 :
    case AVERROR_BUG                                           :
    case AVERROR_DECODER_NOT_FOUND                             :
    case AVERROR_DEMUXER_NOT_FOUND                             :
    case AVERROR_ENCODER_NOT_FOUND                             :
    case AVERROR_EXIT                                          :
    case AVERROR_FILTER_NOT_FOUND                              :
    case AVERROR_INVALIDDATA                                   :
    case AVERROR_MUXER_NOT_FOUND                               :
    case AVERROR_OPTION_NOT_FOUND                              :
    case AVERROR_PATCHWELCOME                                  :
    case AVERROR_PROTOCOL_NOT_FOUND                            :
    case AVERROR_STREAM_NOT_FOUND                              :
    case AVERROR_UNKNOWN                                       :
    case AVERROR_EXPERIMENTAL                                  :
    case AVERROR_EOF                                           :
      StreamError ( Ret )                                      ;
    break                                                      ;
    default                                                    :
      if (Ret<0) going = false ;                            else
      if ( AvDecoder :: isAudioChannel ( Packet            ) ) {
        FetchAudio   ( )                                       ;
      } else
      if ( AvDecoder :: isVideoChannel ( Packet            ) ) {
        int gotFrame                                           ;
        AvDecoder    :: DecodeImage    ( Packet,gotFrame     ) ;
        if ( gotFrame > 0 )                                    {
          FetchImage ( )                                       ;
        }                                                      ;
      } else
      if (  AvDecoder :: isSubtitle    ( Packet            ) ) {
        FetchSubtitle ( )                                      ;
      } else                                                   {
        PacketError   ( )                                      ;
      }                                                        ;
    break                                                      ;
  }                                                            ;
  AvDecoder :: Release ( &Packet )                             ;
  return going                                                 ;
}

bool N::VideoReader::cease(void)
{
  AvDecoder :: Close ( ) ;
  return true            ;
}

bool N::VideoReader::fresh(void)
{
  return true ;
}

bool N::VideoReader::demolish(void)
{
  return true ;
}

bool N::VideoReader::LockAudio(void)
{
  AudioMutex . lock ( ) ;
  return true           ;
}

bool N::VideoReader::UnlockAudio(void)
{
  AudioMutex . unlock ( ) ;
  return true             ;
}

bool N::VideoReader::LockVideo(void)
{
  VideoMutex . lock ( ) ;
  return true           ;
}

bool N::VideoReader::UnlockVideo(void)
{
  VideoMutex . unlock ( ) ;
  return true             ;
}

void N::VideoReader::Error(int error,QString message)
{
}

void N::VideoReader::started(void)
{
}

bool N::VideoReader::proceed(void)
{
  return going ;
}

void N::VideoReader::finished(void)
{
}

void N::VideoReader::progress(int current,int total)
{
}

void N::VideoReader::AssignImage(QImage * image)
{
  if (IsNull(image         )) return                         ;
  if (IsNull(AvDecoder::RGB)) return                          ;
  AVPicture * avp = (AVPicture *) AvDecoder :: RGB            ;
  int         bpl = image -> bytesPerLine ( )                 ;
  int         dpl = avp -> linesize [ 0 ]                     ;
  if ( bpl == dpl )                                           {
    ::avpicture_fill                                          (
        avp                                                   ,
        image -> bits()                                       ,
        (AVPixelFormat) AvPixelFormat                         ,
        VideoData :: Dimension . width  ( )                   ,
        VideoData :: Dimension . height ( )                 ) ;
  } else                                                      {
    unsigned char * s = (unsigned char *) avp   -> data [ 0 ] ;
    unsigned char * t = (unsigned char *) image -> bits (   ) ;
    int             h = VideoData :: Dimension . height (   ) ;
    int             w = bpl                                   ;
    if ( w > dpl ) w = dpl                                    ;
    for (int i = 0 ; i < h ; i++ )                            {
      ::memcpy ( t , s , w )                                  ;
      s += dpl                                                ;
      t += bpl                                                ;
    }                                                         ;
  }                                                           ;
}

QImage * N::VideoReader::ObtainImage(void)
{
  return NULL ;
}

void N::VideoReader::FetchAudio(void)
{
  int Length                                         ;
  Length = FFmpeg::Decoder :: DecodeAudio            (
             Packet                                  ,
             (unsigned char *)AudioCache.index(0)  ) ;
  if (Length<=0) return                              ;
  LockAudio         (                              ) ;
  AudioBuffer . add ( AudioCache.index(0) , Length ) ;
  UnlockAudio       (                              ) ;
  AudioAppended     ( Length                       ) ;
}

void N::VideoReader::AudioAppended(int newSize)
{
}

void N::VideoReader::FetchImage(void)
{
  LockVideo   (                      ) ;
  AssignImage ( ObtainImage ( )      ) ;
  ScaleImage  ( VideoData::Dimension ) ;
  UnlockVideo (                      ) ;
}

void N::VideoReader::FetchSubtitle(void)
{
  AVSubtitle * subtitle = new AVSubtitle ( )                    ;
  int        got = 0                                            ;
  ::avcodec_decode_subtitle2 (CaptionCtx,subtitle,&got,&Packet) ;
  if ( ( 0 != got ) && ( subtitle -> num_rects <= 0 ) )         {
    for (int i = 0 ; i < subtitle -> num_rects ; i++ )          {
      if ( NotNull ( subtitle -> rects       )                 &&
           NotNull ( subtitle -> rects [ i ] )                ) {
        #pragma message("subtitle text processing need to be completed")
        if ( NotNull ( subtitle -> rects [ i ] -> text ) )      {
        }                                                       ;
        /////////////////////////////////////////////////////////
        #pragma message("subtitle rectangle pictures processing need to be completed")
        #ifdef UNCOMMENT_THIS_EXAMPLE_CODE_TO_ENABLE_SUBTITLE
        struct SwsContext * mSwsContext                         ;
        AVFrame           * pPicture                            ;
        mSwsContext = ::sws_getContext                          (
                        subtitle->rects[i]->w                   ,
                        subtitle->rects[i]->h                   ,
                        CaptionCtx->pix_fmt                     ,
                        subtitle->rects[i]->w                   ,
                        subtitle->rects[i]->h                   ,
                        PIX_FMT_RGBA                            ,
                        SWS_BICUBIC                             ,
                        NULL                                    ,
                        NULL                                    ,
                        NULL                                  ) ;
        pPicture = AllocatePicture                              (
                    PIX_FMT_RGBA                                ,
                    subtitle->rects[i]->w                       ,
                    subtitle->rects[i]->h                     ) ;
        ::sws_scale                                             (
          mSwsContext                                           ,
          (const uint8_t **) subtitle->rects[i]->pict.data      ,
                             subtitle->rects[i]->pict.linesize  ,
          0                                                     ,
          subtitle->rects[i]->h                                 ,
          pPicture->data                                        ,
          pPicture->linesize                                  ) ;
        ::av_free         ( pPicture    )                       ;
        ::sws_freeContext ( mSwsContext )                       ;
        /////////////////////////////////////////////////////////
        #endif
      }                                                         ;
    }                                                           ;
  }                                                             ;
  ::avsubtitle_free ( subtitle )                                ;
}

void N::VideoReader::PacketError(void)
{
}

void N::VideoReader::StreamError(int Error)
{ Q_UNUSED ( Error ) ;
  going = false      ;
}

TUID N::VideoReader::toMilliseconds(TUID timestamp)
{
  TUID msecs                          ;
  msecs  = AV_TIME_BASE               ;
  msecs *= timestamp                  ;
  msecs *= VideoStream->time_base.num ;
  msecs /= VideoStream->time_base.den ;
  msecs /= 1000                       ;
  return msecs                        ;
}
