#include <qtav.h>

N::ScreenCapturer:: ScreenCapturer ( QObject * parent                    )
                  : QThread        (           parent                    )
                  , running        ( false                               )
                  , Interval       ( 200                                 )
                  , MaxSeconds     ( -1                                  )
                  , DelayStart     ( -1                                  )
                  , Widget         ( (QWidget *) QApplication::desktop() )
{
}

N::ScreenCapturer::~ScreenCapturer (void)
{
}

bool N::ScreenCapturer::compare(void)
{
  WindowImage = WindowPixmap.toImage()                                   ;
  if ( LastestImage . size  () != WindowImage . size   () ) return false ;
  if ( LastestImage . width () != WindowImage . width  () ) return false ;
  if ( LastestImage . height() != WindowImage . height () ) return false ;
  int s = WindowImage.byteCount()                                        ;
  unsigned char * l = LastestImage.bits()                                ;
  unsigned char * w = WindowImage .bits()                                ;
  if (NULL==l         ) return false                                     ;
  if (NULL==w         ) return false                                     ;
  if (memcmp(l,w,s)==0) return true                                      ;
  return false                                                           ;
}

void N::ScreenCapturer::run(void)
{
  QDateTime StartTime = nTimeNow                        ;
  running = true                                        ;
  if (DelayStart>0)                                     {
    Time :: sleep ( DelayStart )                        ;
  }                                                     ;
  while (running)                                       {
    WindowPixmap = QPixmap::grabWindow(Widget->winId()) ;
    if (!compare())                                     {
      QDateTime D = QDateTime::currentDateTime()        ;
      LastestPixmap = WindowPixmap                      ;
      LastestImage  = WindowImage                       ;
      Store(D,LastestImage)                             ;
    }                                                   ;
    msleep(Interval)                                    ;
    if (MaxSeconds>0)                                   {
      QDateTime NT = nTimeNow                           ;
      QDateTime ST = StartTime                          ;
      ST = ST.addSecs(MaxSeconds)                       ;
      if (ST>NT) running = false                        ;
    }                                                   ;
  }                                                     ;
}

void N::ScreenCapturer::Store(QDateTime Time,QImage & Image)
{
  QString S                                        ;
  S = Time.toString("yyyy-MM-dd-hh-mm-ss-zzz.png") ;
  S = Directory.absoluteFilePath(S)                ;
  Image . save ( S , "PNG" )                       ;
}

void N::ScreenCapturer::Start(void)
{
  if (running) return ;
  running = true      ;
  start ( )           ;
}

void N::ScreenCapturer::Stop(void)
{
  running = false ;
}

void N::ScreenCapturer::setInterval(int interval)
{
  Interval = interval ;
}

void N::ScreenCapturer::setMaxTime(int maxSeconds)
{
  MaxSeconds = maxSeconds ;
}

void N::ScreenCapturer::setDelay(int delay)
{
  DelayStart = delay ;
}

void N::ScreenCapturer::setDirectory(QDir dir)
{
  Directory = dir ;
}

void N::ScreenCapturer::setWidget(QWidget * widget)
{
  Widget = widget ;
}
