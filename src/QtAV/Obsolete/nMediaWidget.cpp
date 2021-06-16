#include <AV>
#include <windows.h>

//#include "SDL.h"
//#include "SDL_audio.h"
//#include "SDL_mutex.h"

//#include "libav.hpp"

nMediaPlayer:: nMediaPlayer  (QWidget * parent,nPlan * plan)
            :  QWidget       (          parent             )
            ,  Father        (          parent             )
            ,  nMediaDecoder (                             )
            ,  PrivateThread (NULL                         )
            ,  PrivateDevice (NULL                         )
            ,  Halting       (false                        )
            ,  Playing       (false                        )
            ,  BufferSeconds (20                           )
            ,  CacheSeconds  (30                           )
            ,  Plan          (plan                         )
{
  DecodePlan = Plan                  ;
  setAccessibleName ("nMediaPlayer") ;
  setMouseTracking  (true          ) ;
  CreateThread      (              ) ;
  CreateDevices     (              ) ;
}

nMediaPlayer::~nMediaPlayer(void)
{
  DestroyThread  ( ) ;
  DestroyDevices ( ) ;
}

void nMediaPlayer::enterEvent(QEvent * event)
{
  QWidget :: enterEvent ( event ) ;
//  grabMouse    ( )                ;
}

void nMediaPlayer::leaveEvent(QEvent * event)
{
  QWidget :: leaveEvent ( event ) ;
//  releaseMouse ( )                ;
}

void nMediaPlayer::paintEvent(QPaintEvent * event)
{
  bool nopaint = false                  ;
  if (Icon.isNull()   ) nopaint = true  ;
  if (Icon.width ()==0) nopaint = true  ;
  if (Icon.height()==0) nopaint = true  ;
  if (Playing         ) nopaint = true  ;
  if (nopaint)                          {
    QWidget::paintEvent(event)          ;
  } else                                {
    int x = (width ()-Icon.width ())/2  ;
    int y = (height()-Icon.height())/2  ;
    QRect     R(0,0,width(),height()  ) ;
    QPainter  p(this         )          ;
    p.setPen   (QColor(0,0,0))          ;
    p.setBrush (QColor(0,0,0))          ;
    p.drawRect (R            )          ;
    if (!Playing) p.drawImage(x,y,Icon) ;
  }                                     ;
}

void nMediaPlayer::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event)                 ;
  if (!Playing) return                        ;
  Halting = true                              ;
  QDateTime CS = QDateTime::currentDateTime() ;
  if (Mutex.tryLock(300))                     {
    int w = width  () ; w -= (w%2)            ;
    int h = height () ; h -= (h%2)            ;
    QSize S(w,h)                              ;
    Screen  = Dimension                       ;
    Screen  . scale(S,Qt::KeepAspectRatio)    ;
    Scaling = !(Screen==Dimension)            ;
    Surface       (Screen)                    ;
    CreateDisplay (      )                    ;
    Mutex.unlock  (      )                    ;
  }                                           ;
  QDateTime ES = QDateTime::currentDateTime() ;
  qint64    ms = CS.msecsTo(ES)               ;
  Start        = Start.addMSecs(ms)           ;
  Halting = false                             ;
}

void nMediaPlayer::closeEvent(QCloseEvent * event)
{
  if (stop()) event->accept (       ) ;
  else QWidget::closeEvent  ( event ) ;
}

void nMediaPlayer::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept() ;
  else QWidget::contextMenuEvent(event)   ;
}

void nMediaPlayer::mouseDoubleClickEvent(QMouseEvent * event)
{
  if (IsMask(event->buttons(),Qt::LeftButton)) {
//    switchDisplay()                            ;
//    if (stop()) event->accept ( )              ;
    Menu(event->pos())                         ;
  }                                            ;
}

void nMediaPlayer::mouseMoveEvent(QMouseEvent * event)
{
  QWidget::mouseMoveEvent(event) ;
}

bool nMediaPlayer::prepare(void)
{
  if (!open()) return false                 ;
  Resample = false                          ;
  if (Screen!=Dimension) Scaling = true     ;
  Surface       (Screen)                    ;
  CreateDisplay (  )                        ;
  setAudioBuffer(BufferSeconds,AudioBuffer) ;
  CreateAudio   (1152)                      ;
  Start = QDateTime::currentDateTime()      ;
  return true                               ;
}

bool nMediaPlayer::frame(void)
{
  if (Halting)                                         {
    ::Sleep(1)                                         ;
    return true                                        ;
  }                                                    ;
  QDateTime Now = QDateTime::currentDateTime()         ;
  long long cts = Timestamp(Start,Now)                 ;
  if (Mutex.tryLock())                                 {
    for (int i=0;i<8;i++)                              {
      FreshVideo(VideoList,cts)                        ;
      FreshAudio(AudioList,cts,BufferSeconds/2)        ;
    }                                                  ;
    FreeBuffer(ExpireList)                             ;
    Mutex.unlock()                                     ;
  }                                                    ;
  for (int i=0;i<15;i++)                               {
    Buffer(VideoList,AudioList,Start,Now,CacheSeconds) ;
  }                                                    ;
  cts    = Timestamp(Start)                            ;
  int ms = Idle(VideoList,AudioList,cts)               ;
  if (ms>0) ::Sleep(ms)                                ;
  return (VideoList.count()>0 || AudioList.count()>0)  ;
}

bool nMediaPlayer::finish(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  stopAudio ( )                 ;
  SDL_Quit  ( )                 ;
  delete PrivateDevice          ;
  PrivateDevice = NULL          ;
#endif
  return nMediaDecoder::close() ;
}

void nMediaPlayer::PutVideo(void * packet)
{
  DecodeVideo    ( packet ) ;
  prepareDisplay (        ) ;
  DisplayVideo   (        ) ;
  display        (        ) ;
  ExpireList   <<  packet   ;
}

void nMediaPlayer::PutAudio(void * packet)
{
  int length = DecodeAudio(packet) ;
  if (length<=0) return            ;
  lockAudio   (        )           ;
  BufferAudio ( length )           ;
  unlockAudio (        )           ;
  ExpireList << packet             ;
}

bool nMediaPlayer::stop(void)
{
  QDateTime C = QDateTime::currentDateTime()                  ;
  Launch = false                                              ;
  while (Playing && C.secsTo(QDateTime::currentDateTime())<3) {
    Plan -> processEvents()                                   ;
    nTime :: msleep ( 10  )                                   ;
  }                                                           ;
  return !Playing                                             ;
}

void nMediaPlayer::run(void)
{
  Playing       = true                                    ;
  Launch        = true                                    ;
  /*                       Prefetch                      */
  Halting       = true                                    ;
  Start         = QDateTime::currentDateTime()            ;
  QDateTime Now = Start                                   ;
  Now   = Now.addSecs(10)                                 ;
  for (int i=0;i<300;i++)                                 {
    Buffer ( VideoList,AudioList,Start,Now,CacheSeconds ) ;
  }                                                       ;
  Start   = QDateTime::currentDateTime()                  ;
  Halting = false                                         ;
  /////////////////////////////////////////////////////////
  while (Launch && Playing && frame()) { ; }              ;
  finish ( )                                              ;
  Playing = false                                         ;
}

void nMediaPlayer::FlushVideo(void)
{
}

void nMediaPlayer::FlushAudio(void)
{
}

void nMediaPlayer::Debug(QString message)
{
  setWindowTitle(message) ;
}

void nMediaPlayer::Initialize(void)
{
}

void nMediaPlayer::play(QString name,QStringList files)
{
  if (files.count()<0) return     ;
  setWindowTitle(name)            ;
  Playlists = files               ;
  QString filename = Playlists[0] ;
  Playlists.takeAt(0)             ;
  play(filename)                  ;
}

void nMediaPlayer::play(QString filename)
{
  if (!nMediaDecoder::open(filename)) return                      ;
  QWidget * w = parentWidget()                                    ;
  resize(Dimension)                                               ;
  if (w!=NULL)                                                    {
    QMdiSubWindow * subw = Casting(QMdiSubWindow,w)               ;
    if (subw!=NULL)                                               {
      subw->resize(Dimension)                                     ;
      qApp->processEvents()                                       ;
      QSize W = subw->frameSize()                                 ;
      QSize G(width(),height())                                   ;
      QSize D(W . width  () - G . width  () + Dimension.width ()  ,
              W . height () - G . height () + Dimension.height()) ;
      subw->resize(D)                                             ;
    }                                                             ;
  }                                                               ;
  resize(Dimension)                                               ;
  Screen = Dimension                                              ;
  nMediaDecoder::close()                                          ;
  qApp -> processEvents    ()                                     ;
  qApp -> sendPostedEvents ()                                     ;
  /////////////////////////////////////////////////////////////////
  if (prepare()) start()                                          ;
}

bool nMediaPlayer::Menu(QPoint pos)
{
  nMenuManager mm(this)                              ;
  QMenu      * mr                                    ;
  QAction    * aa                                    ;
  if (isPanel())                                     {
    aa = mm.add(101,tr("Panel"))                     ;
    aa -> setCheckable(true)                         ;
  }                                                  ;
  if (!Playing && Playlists.count()>0)               {
    mm.add(201,tr("Play"))                           ;
  }                                                  ;
  if ( Playing && !Halting) mm.add(202,tr("Pause" )) ;
  if ( Playing &&  Halting) mm.add(203,tr("Resume")) ;
  if ( Playing) mm.add(204,tr("Stop"))               ;
  mr = mm.addMenu ( tr("Resampling" ))               ;
  aa = mm.add(mr,301,tr("Bilinear"  ))               ;
  aa -> setCheckable(true)                           ;
  aa -> setChecked (ResampleWay==SWS_FAST_BILINEAR)  ;
  aa = mm.add(mr,302,tr("Bicubic"   ))               ;
  aa -> setCheckable(true)                           ;
  aa -> setChecked (ResampleWay==SWS_BICUBIC       ) ;
  aa = mm.add(mr,303,tr("Lanczos"   ))               ;
  aa -> setCheckable(true)                           ;
  aa -> setChecked (ResampleWay==SWS_LANCZOS       ) ;
  mm  . setFont  ( Plan )                            ;
  aa = mm . exec ( )                                 ;
  if (IsNull(aa)) return true                        ;
  bool display = false                               ;
  QDateTime NT = QDateTime::currentDateTime()        ;
  switch (mm[aa])                                    {
    case 101                                         :
      SwitchPanel()                                  ;
    break                                            ;
    case 201                                         :
      if (Playlists.count()>0)                       {
        QString filename = Playlists[0]              ;
        Playlists.takeAt(0)                          ;
        play(filename)                               ;
      }                                              ;
    break                                            ;
    case 202                                         :
      HaltingTS = Start.msecsTo(NT)                  ;
      Halting   = true                               ;
    break                                            ;
    case 203                                         :
      Start     = NT.addMSecs(-HaltingTS)            ;
      Halting   = false                              ;
    break                                            ;
    case 204                                         :
      stop    ( )                                    ;
      repaint ( )                                    ;
    break                                            ;
    case 301                                         :
      ResampleWay = SWS_FAST_BILINEAR                ;
      display = true                                 ;
    break                                            ;
    case 302                                         :
      ResampleWay = SWS_BICUBIC                      ;
      display = true                                 ;
    break                                            ;
    case 303                                         :
      ResampleWay = SWS_LANCZOS                      ;
      display = true                                 ;
    break                                            ;
  }                                                  ;
  if (!display) return true                          ;
  Halting = true                                     ;
  QDateTime CS = QDateTime::currentDateTime()        ;
  if (Mutex.tryLock(300))                            {
    int w = width  () ; w -= (w%2)                   ;
    int h = height () ; h -= (h%2)                   ;
    QSize S(w,h)                                     ;
    Screen  = Dimension                              ;
    Screen  . scale(S,Qt::KeepAspectRatio)           ;
    Scaling = !(Screen==Dimension)                   ;
    Surface       (Screen)                           ;
    CreateDisplay (      )                           ;
    Mutex.unlock  (      )                           ;
  }                                                  ;
  QDateTime ES = QDateTime::currentDateTime()        ;
  qint64    ms = CS.msecsTo(ES)                      ;
  Start        = Start.addMSecs(ms)                  ;
  Halting = false                                    ;
  return true                                        ;
}

bool nMediaPlayer::isPanel(void)
{
  return false ;
}

void nMediaPlayer::SwitchPanel(void)
{
}
