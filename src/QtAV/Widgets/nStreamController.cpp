#include <qtav.h>
#include "ui_nStreamController.h"

N::StreamController:: StreamController ( QWidget * parent , Plan * p )
                    : Widget           (           parent ,        p )
                    , StreamInterface  (                             )
                    , ui               ( new Ui::nStreamController   )
                    , moving           ( false                       )
                    , TimeId           ( -1                          )
                    , TimeID           ( -1                          )
                    , closeIt          ( false                       )
{
  QString css                                     ;
  css = "QSlider::groove:horizontal { border: 1px solid #bbb; background: white; height: 10px; border-radius: 4px; } "
        "QSlider::sub-page:horizontal { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #66e, stop: 1 #bbf); background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #bbf, stop: 1 #55f); border: 1px solid #777; height: 10px; border-radius: 4px; } "
        "QSlider::add-page:horizontal { background: #fff; border: 1px solid #777; height: 10px; border-radius: 4px; } "
        "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc); border: 1px solid #777; width: 13px; margin-top: -2px; margin-bottom: -2px; border-radius: 4px; } "
        "QSlider::handle:horizontal:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fff, stop:1 #ddd); border: 1px solid #444; border-radius: 4px; } "
        "QSlider::sub-page:horizontal:disabled { background: #bbb; border-color: #999; } "
        "QSlider::add-page:horizontal:disabled { background: #eee; border-color: #999; } "
        "QSlider::handle:horizontal:disabled { background: #eee; border: 1px solid #aaa; border-radius: 4px; }" ;
  //////////////////////////////
  WidgetClass                  ;
  ui -> setupUi       ( this ) ;
  setStyleSheet       ( css  ) ;
  TimeID = startTimer ( 200  ) ;
}

N::StreamController::~StreamController(void)
{
  stream = NULL          ;
  vcf    = NULL          ;
  if ( closeIt )         {
    closeIt = false      ;
    killTimer ( TimeID ) ;
    TimeID = -1          ;
  }                      ;
  delete ui              ;
}

void N::StreamController::LoadStyleSheet(void)
{
  QString filename = GetPath ( )               ;
  filename = QFileDialog::getOpenFileName      (
               this                            ,
               tr("Load stylesheet from file") ,
               filename                        ,
               tr("Style Sheet (*.css)")     ) ;
  if ( filename . length ( ) <= 0 ) return     ;
  KeepPath ( filename )                        ;
  QByteArray  B                                ;
  QString     S                                ;
  File::toByteArray ( filename , B )           ;
  S = QString::fromUtf8(B)                     ;
  setStyleSheet ( S    )                       ;
  Alert         ( Done )                       ;
}

void N::StreamController::SaveStyleSheet(void)
{
  QString css = styleSheet ( )               ;
  if ( css . length ( ) <= 0 ) return        ;
  QString filename = GetPath ( )             ;
  filename = QFileDialog::getSaveFileName    (
               this                          ,
               tr("Save stylesheet to file") ,
               filename                      ,
               tr("Style Sheet (*.css)")   ) ;
  if ( filename . length ( ) <= 0 ) return   ;
  KeepPath ( filename )                      ;
  QString     S                              ;
  QByteArray  B                              ;
  S = styleSheet (              )            ;
  B = S . toUtf8 (              )            ;
  File::toFile   ( filename , B )            ;
  Alert          ( Done         )            ;
}

void N::StreamController::Restart(void)
{
  TimeID = startTimer ( 200  ) ;
}

bool N::StreamController::isStopped(void)
{
  return ( TimeID < 0 ) ;
}

void N::StreamController::Finalize(void)
{
  TimeId  = -1   ;
  closeIt = true ;
}

void N::StreamController::KeepPath(QString path)
{
  QFileInfo F ( path )                         ;
  QString   p = F . absolutePath ( )           ;
  plan -> Variables [ "StreamController" ] = p ;
}

QString N::StreamController::GetPath(void)
{
  QString p                                                     ;
  if ( plan  -> Variables . contains ( "StreamController" ) )   {
    p = plan -> Variables [ "StreamController" ] . toString ( ) ;
  } else                                                        {
    p = plan -> Temporary ( "" )                                ;
  }                                                             ;
  return p                                                      ;
}

void N::StreamController::changeEvent(QEvent * e)
{
  QWidget::changeEvent ( e )       ;
  switch ( e -> type ( ) )         {
    case QEvent::LanguageChange    :
      ui -> retranslateUi ( this ) ;
    break                          ;
    default                        :
    break                          ;
  }                                ;
}

void N::StreamController::resizeEvent(QResizeEvent * e)
{
  QWidget::resizeEvent ( e )                           ;
  QSize  s                                             ;
  QSize  r                                             ;
  QPoint p                                             ;
  QPoint t                                             ;
  s   = ui -> Left -> size ( )                         ;
  p   = QPoint ( width() - s . width ( ) , 0 )         ;
  ui -> Left -> move ( p )                             ;
  //////////////////////////////////////////////////////
  s   = ui -> Left -> size ( )                         ;
  p   . setX ( width() - s . width ( ) )               ;
  p   . setY ( s . height ( )          )               ;
  ui -> Right -> move ( p )                            ;
  //////////////////////////////////////////////////////
  s   = ui -> Left   -> size ( )                       ;
  r   = ui -> Length -> size ( )                       ;
  p   . setX ( width() - s . width ( ) - r.width()   ) ;
  p   . setY ( 0                                     ) ;
  ui -> Length -> move ( p )                           ;
  //////////////////////////////////////////////////////
  p   . setY ( r . height ( ) )                        ;
  ui -> Time -> move ( p )                             ;
  //////////////////////////////////////////////////////
  r   = ui -> Opacity -> size ( )                      ;
  p   . setX ( p . x ( ) - r . width ( ) )             ;
  p   . setY ( 0                         )             ;
  ui -> Opacity -> move ( p )                          ;
  //////////////////////////////////////////////////////
  s   = ui -> Message -> size ( )                      ;
  s   . setWidth ( p . x ( ) - 7 * 48 )                ;
  ui -> Message -> resize ( s )                        ;
  //////////////////////////////////////////////////////
  p   . setX ( p . x ( ) + r . width () )              ;
  r   = ui -> video -> size ( )                        ;
  r   . setWidth ( p . x ( ) - 7 * 48 )                ;
  ui -> video -> resize ( r )                          ;
}

void N::StreamController::timerEvent(QTimerEvent * e)
{
  QWidget::timerEvent ( e )           ;
  if ( closeIt )                      {
    closeIt = false                   ;
    killTimer ( TimeID )              ;
    TimeID  = -1                      ;
    stream  = NULL                    ;
    vcf     = NULL                    ;
  } else                              {
    if ( e -> timerId ( ) == TimeId ) {
      Refresh ( )                     ;
      e -> accept ( )                 ;
    }                                 ;
  }                                   ;
}

void N::StreamController::Sync(void)
{
  ui -> video   -> setEnabled   ( true                                   ) ;
  ui -> video   -> setRange     ( 0 , stream -> msecs   ( )              ) ;
  ui -> video   -> setValue     (     stream -> current ( )              ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Left    -> setEnabled   ( true                                   ) ;
  ui -> Left    -> blockSignals ( true                                   ) ;
  ui -> Left    -> setValue     ( stream -> stream -> Volume ( 0 ) / 100 ) ;
  ui -> Left    -> blockSignals ( false                                  ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Right   -> setEnabled   ( true                                   ) ;
  ui -> Right   -> setValue     ( stream -> stream -> Volume ( 1 ) / 100 ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Opacity -> setEnabled   ( true                                   ) ;
  ui -> Opacity -> blockSignals ( true                                   ) ;
  ui -> Opacity -> setValue     ( ((QGraphicsItem *)vcf)->opacity()*1000 ) ;
  ui -> Opacity -> blockSignals ( false                                  ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Play    -> setEnabled   ( true                                   ) ;
  ui -> Play    -> blockSignals ( true                                   ) ;
  ui -> Play    -> setChecked   ( true                                   ) ;
  ui -> Play    -> setToolTip   ( tr("Pause")                            ) ;
  ui -> Play    -> blockSignals ( false                                  ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Stop    -> setEnabled   ( true                                   ) ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Length  -> setText      ( TimeLength ( stream -> msecs   ( ) )   ) ;
  ui -> Time    -> setText      ( TimeLength ( stream -> current ( ) )   ) ;
  ui -> Message -> setText      ( stream -> universe -> name             ) ;
  //////////////////////////////////////////////////////////////////////////
  SyncButtons                   (                                        ) ;
  leftMsg                       (                                        ) ;
  rightMsg                      (                                        ) ;
  opacityMsg                    (                                        ) ;
  //////////////////////////////////////////////////////////////////////////
  TimeId = TimeID                                                          ;
}

void N::StreamController::SyncButtons(void)
{
  if ( IsNull(stream) ) return                               ;
  ui -> Previous -> setEnabled ( stream -> hasPrevious ( ) ) ;
  ui -> Next     -> setEnabled ( stream -> hasNext     ( ) ) ;
  ui -> Backward -> setEnabled ( stream -> hasBackward ( ) ) ;
  ui -> Forward  -> setEnabled ( stream -> hasForward  ( ) ) ;
}

void N::StreamController::Refresh(void)
{
  if ( IsNull ( stream ) ) return                       ;
  QString cs = TimeLength     ( stream -> current ( ) ) ;
  ui   -> video -> setToolTip ( cs                    ) ;
  ui   -> Time  -> setText    ( cs                    ) ;
  if ( ! moving )                                       {
    ui -> video -> setValue   ( stream -> current ( ) ) ;
  }                                                     ;
  SyncButtons ( )                                       ;
}

QString N::StreamController::TimeLength(TUID t)
{
  int     s                                                  ;
  int     m                                                  ;
  int     h                                                  ;
  int     z                                                  ;
  QString H                                                  ;
  QString M                                                  ;
  QString S                                                  ;
  QString Z                                                  ;
  z  = t % 1000                                              ;
  t /= 1000                                                  ;
  s  = t % 60                                                ;
  t /= 60                                                    ;
  m  = t % 60                                                ;
  t /= 60                                                    ;
  h  = t                                                     ;
  H = QString("%1"    ).arg(h)                               ;
  M = QString("%1%2"  ).arg(m/10).arg(m%10)                  ;
  S = QString("%1%2"  ).arg(s/10).arg(s%10)                  ;
  Z = QString("%1%2%3").arg(z/100).arg((z%100)/10).arg(z%10) ;
  return QString("%1:%2:%3.%4").arg(H).arg(M).arg(S).arg(Z)  ;
}

void N::StreamController::leftChanged(int value)
{
  if ( IsNull(stream        ) ) return              ;
  if ( IsNull(stream->stream) ) return              ;
  stream -> stream -> setVolume ( value * 100 , 0 ) ;
  leftMsg                       (                 ) ;
}

void N::StreamController::rightChanged(int value)
{
  if ( IsNull(stream        ) ) return              ;
  if ( IsNull(stream->stream) ) return              ;
  stream -> stream -> setVolume ( value * 100 , 1 ) ;
  rightMsg                      (                 ) ;
}

void N::StreamController::opacityChanged(int value)
{
  if ( IsNull(vcf) ) return ;
  double v = value          ;
  if ( v >= 1000   ) return ;
  v /= 1000                 ;
  if ( v > 0.05 )           {
    vcf -> setOpacity ( v ) ;
  }                         ;
  opacityMsg ( )            ;
}

void N::StreamController::leftMsg(void)
{
  QString s                       ;
  int     v = ui->Left->value()   ;
  s = tr("Left volume %1").arg(v) ;
  ui -> Left -> setToolTip ( s )  ;
}

void N::StreamController::rightMsg(void)
{
  QString s                        ;
  int     v = ui->Right->value()   ;
  s = tr("Right volume %1").arg(v) ;
  ui -> Right -> setToolTip ( s )  ;
}

void N::StreamController::opacityMsg(void)
{
  int     value = ui->Opacity->value()                  ;
  QString s                                             ;
  int     z     = value / 1000                          ;
  int     a     = value % 1000                          ;
  int     b     = value %  100                          ;
  int     c     = value %   10                          ;
  a /= 100                                              ;
  b /= 10                                               ;
  s  = QString("%1.%2%3%4").arg(z).arg(a).arg(b).arg(c) ;
  s  = tr("Opacity %1").arg(s)                          ;
  ui -> Opacity -> setToolTip ( s )                     ;
}

void N::StreamController::videoChanged(int value)
{
  moving = true ;
}

void N::StreamController::videoReleased(void)
{
  TUID v = ui->video->value()  ;
  moving = false               ;
  if ( IsNull(stream) ) return ;
  stream -> Seeking ( v )      ;
  SyncButtons ( )              ;
}

void N::StreamController::startMenu(void)
{
  if ( IsNull ( stream ) ) return           ;
  QPoint P ( 0 , 0 )                        ;
  P = mapToGlobal ( P )                     ;
  emit PanelMenu ( stream -> ThreadId , P ) ;
}

void N::StreamController::Play(bool enabled)
{
  if ( IsNull(stream) ) return               ;
  if ( enabled )                             {
    stream -> Resume ( )                     ;
    ui -> Play -> setToolTip ( tr("Pause") ) ;
  } else                                     {
    stream -> Halt   ( )                     ;
    ui -> Play -> setToolTip ( tr("Play" ) ) ;
  }                                          ;
  SyncButtons ( )                            ;
}

void N::StreamController::Stop(void)
{
  TimeId = -1                             ;
  ui -> Stop -> setEnabled ( false )      ;
  if ( IsNull(stream) ) return            ;
  stream -> Booleans [ "GoNext" ] = false ;
  stream -> Stop ( )                      ;
}

void N::StreamController::Previous(void)
{
  if ( IsNull(stream) ) return ;
  stream -> Previous ( )       ;
  SyncButtons ( )              ;
}

void N::StreamController::Next(void)
{
  if ( IsNull(stream) ) return ;
  stream -> Next ( )           ;
  SyncButtons ( )              ;
}

void N::StreamController::Forward(void)
{
  if ( IsNull(stream) ) return ;
  stream -> Forward ( )        ;
  SyncButtons ( )              ;
}

void N::StreamController::Backward(void)
{
  if ( IsNull(stream) ) return ;
  stream -> Backward ( )       ;
  SyncButtons ( )              ;
}
