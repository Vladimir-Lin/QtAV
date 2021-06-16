#include <qtav.h>

N::VideoData:: VideoData (void)
{
  Drift        = 800  ;
  VideoFrames  = 150  ;
  VideoPending =   0  ;
  VideoAt      =   0  ;
}

N::VideoData::~VideoData (void)
{
}

int N::VideoData::setPixelFormat(QImage::Format format)
{
  switch ( format )                            {
    case QImage::Format_RGB888                 :
      AvPixelFormat = AV_PIX_FMT_BGR24         ;
    break                                      ;
    case QImage::Format_RGB32                  :
      AvPixelFormat = AV_PIX_FMT_BGRA          ;
    break                                      ;
    case QImage::Format_ARGB32                 :
      AvPixelFormat = AV_PIX_FMT_BGRA          ;
    break                                      ;
    case QImage::Format_ARGB32_Premultiplied   :
      AvPixelFormat = AV_PIX_FMT_BGRA          ;
    break                                      ;
    case QImage::Format_Invalid                :
    case QImage::Format_Mono                   :
    case QImage::Format_MonoLSB                :
    case QImage::Format_Indexed8               :
    case QImage::Format_RGB16                  :
    case QImage::Format_ARGB8565_Premultiplied :
    case QImage::Format_RGB666                 :
    case QImage::Format_ARGB6666_Premultiplied :
    case QImage::Format_RGB555                 :
    case QImage::Format_ARGB8555_Premultiplied :
    case QImage::Format_RGB444                 :
    case QImage::Format_ARGB4444_Premultiplied :
    default                                    :
      AvPixelFormat = AV_PIX_FMT_NONE          ;
    break                                      ;
  }                                            ;
  return AvPixelFormat                         ;
}
