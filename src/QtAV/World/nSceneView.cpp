#include <qtav.h>

///////////////////////////////////////////////////////////////////////////////

#if   defined(Q_OS_ANDROID)
#define DISABLE_DESKTOP 1
#elif defined(Q_OS_IOS)
#define DISABLE_DESKTOP 1
#else
#define DISABLE_MOBILE 1
#endif

#define Decide decisions[GuiMainId]

#if !defined(DISABLE_DESKTOP)


#endif

#if !defined(DISABLE_MOBILE)


#endif

///////////////////////////////////////////////////////////////////////////////

N::SceneView:: SceneView      ( QWidget * parent , Plan *      p )
             : GLWidget       ( QGLFormat ( QGL::DoubleBuffer    |
                                            QGL::DepthBuffer     |
                                            QGL::Rgba            |
                                            QGL::AlphaChannel    |
                                            QGL::AccumBuffer     |
                                            QGL::StencilBuffer   |
                                            QGL::HasOverlay      |
                                            QGL::SampleBuffers ) ,
                                            parent,            p )
             , SceneManager   (                                p )
             , Timer          ( new QTimer ( this )              )
             , Stopped        ( true                             )
             , destructor     ( NULL                             )
{
  Configure ( ) ;
}

N::SceneView::~SceneView (void)
{
  if (NotNull(destructor)) {
    delete destructor      ;
    destructor = NULL      ;
  }                        ;
}

QSize N::SceneView::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize (  640 ,  480 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::SceneView::ForceRendering(int ms)
{
  if (ms<=0) Timer -> stop  (    ) ;
        else Timer -> start ( ms ) ;
}

int N::SceneView::ForcingInterval(void)
{
  if (Timer -> isActive ( ) ) return 0 ;
  return Timer -> interval ( )         ;
}

int N::SceneView::RenderingInterval(void)
{
  return Interval ;
}

int N::SceneView::setRenderingInterval(int interval)
{
  Interval = interval ;
  return interval     ;
}

SUID N::SceneView::RequestUniverse(SUID startUuid)
{
  while (Universes.contains(startUuid)) startUuid++ ;
  return startUuid                                  ;
}

UUIDs N::SceneView::Multiverse(void)
{
  return Universes.keys() ;
}

N::Universe * N::SceneView::GetUniverse(SUID uuid)
{
  nKickOut ( ! Universes . contains ( uuid ) , NULL ) ;
  return Universes [ uuid ]                           ;
}

N::Universe * N::SceneView::removeUniverse(SUID uuid)
{
  if (!Universes.contains(uuid)) return NULL ;
  Universe * u = Universes[uuid]             ;
  Universes.remove(uuid)                     ;
  return u                                   ;
}

bool N::SceneView::DeleteUniverse(Universe * universe)
{
  UUIDs U = Multiverse ( )            ;
  SUID  u                             ;
  foreach (u,U)                       {
    if ( universe == Universes[u] )   {
      Universes . remove ( u )        ;
      if (NotNull(destructor))        {
        destructor->Destroy(universe) ;
      } else                          {
        delete universe               ;
      }                               ;
      Modified = true                 ;
      return true                     ;
    }                                 ;
  }                                   ;
  return false                        ;
}

double N::SceneView::ReflushRate(void)
{
  int    ms   = StartReflush.msecsTo(nTimeNow) ;
  double pgls = PaintGLs                                           ;
  if (ms<=0) return 0                                              ;
  pgls *= 1000                                                     ;
  pgls /= ms                                                       ;
  return pgls                                                      ;
}

void N::SceneView::setBackground(QColor & bgColor)
{
  background = bgColor ;
}

void N::SceneView::assignBackground(void)
{
  QColor c = background.Color()                                 ;
  c = QColorDialog::getColor(c,this,tr("Set background color")) ;
  background = c                                                ;
}

void N::SceneView::Configure(void)
{
  StartReflush     = nTimeNow                                  ;
  Alive            = true                                      ;
  Modified         = false                                     ;
  Painting         = false                                     ;
  Finalize         = false                                     ;
  Adaptive         = false                                     ;
  Moment           = false                                     ;
  Interval         = 35                                        ;
  MotionInterval   = 100                                       ;
  PaintGLs         = 0                                         ;
  PaintCosts       = 0                                         ;
  Intervals[13]    = 15                                        ;
  DeviceProperties = plan->Equipments                          ;
  //////////////////////////////////////////////////////////////
  decisions . Blank ( GuiMainId )                              ;
  //////////////////////////////////////////////////////////////
  #if !defined(DISABLE_MOBILE)
  allowGesture = true                                          ;
  decisions . Blank ( GuiMainId             )                  ;
  grabGesture       ( Qt::TapGesture        )                  ;
  grabGesture       ( Qt::TapAndHoldGesture )                  ;
  grabGesture       ( Qt::PanGesture        )                  ;
  grabGesture       ( Qt::PinchGesture      )                  ;
  grabGesture       ( Qt::SwipeGesture      )                  ;
  #endif
  //////////////////////////////////////////////////////////////
  setAcceptDrops   ( true             )                        ;
  setMouseTracking ( true             )                        ;
  setFocusPolicy   ( Qt::WheelFocus   )                        ;
  nConnect ( this , SIGNAL ( Go  (int,int) )                   ,
             this , SLOT   ( Run (int,int) ) )                 ;
  nConnect ( this , SIGNAL ( Reflush  (  ) )                   ,
             this , SLOT   ( updateGL (  ) ) )                 ;
  //////////////////////////////////////////////////////////////
  Lights = 0                                                   ;
  //////////////////////////////////////////////////////////////
  Properties [GL_DEPTH_TEST    ] = true                        ;
  Properties [GL_CULL_FACE     ] = true                        ;
  Properties [GL_BLEND         ] = true                        ;
  Properties [GL_LINE_SMOOTH   ] = true                        ;
  Properties [GL_COLOR_MATERIAL] = true                        ;
  Properties [GL_LIGHTING      ] = true                        ;
  //////////////////////////////////////////////////////////////
  QColor bg                                                    ;
  bg = palette().color(QPalette::Base)                         ;
  background = bg                                              ;
  //////////////////////////////////////////////////////////////
  plan -> setFont ( this               )                       ;
  setDropFlag     ( DropCamera  , true )                       ;
  setDropFlag     ( DropAudio   , true )                       ;
  setDropFlag     ( DropVideo   , true )                       ;
  setDropFlag     ( DropAlbum   , true )                       ;
  setDropFlag     ( DropPeople  , true )                       ;
  setDropFlag     ( DropPicture , true )                       ;
  //////////////////////////////////////////////////////////////
  Timer -> setParent ( this )                                  ;
  nConnect ( Timer    , SIGNAL ( timeout       ( ) )           ,
             this     , SLOT   ( updateGL      ( ) )         ) ;
  nConnect ( this     , SIGNAL ( audioAccepted ( ) )           ,
             this     , SLOT   ( tossAudios    ( ) )         ) ;
  nConnect ( this     , SIGNAL ( videoAccepted ( ) )           ,
             this     , SLOT   ( tossVideos    ( ) )         ) ;
  nConnect ( this     , SIGNAL ( Going(int        ))           ,
             this     , SLOT   ( GoFor(int        ))         ) ;
  nConnect ( this     , SIGNAL ( Going(int,VarArgs))           ,
             this     , SLOT   ( GoFor(int,VarArgs))         ) ;
  nConnect ( this     , SIGNAL ( audioStarting   (VarArgs))    ,
             this     , SLOT   ( LaunchAudio     (VarArgs))  ) ;
  nConnect ( this     , SIGNAL ( videoStarting   (VarArgs))    ,
             this     , SLOT   ( LaunchVideo     (VarArgs))  ) ;
  nConnect ( this     , SIGNAL ( recorderStarting(VarArgs))    ,
             this     , SLOT   ( LaunchRecorder  (VarArgs))  ) ;
  nConnect ( Commando , SIGNAL ( timeout    ())                ,
             this     , SLOT   ( GuiDispatch())              ) ;
  //////////////////////////////////////////////////////////////
  StartCommando ( )                                            ;
}

void N::SceneView::resizeEvent(QResizeEvent * event)
{
  QGLWidget :: resizeEvent ( event ) ;
  Relocation               (       ) ;
}

void N::SceneView::showEvent(QShowEvent * event)
{
  QGLWidget :: showEvent   ( event ) ;
  Relocation               (       ) ;
}

void N::SceneView::closeEvent(QCloseEvent * event)
{
  shutdown       ( ) ?
  event -> accept( ) :
  event -> ignore( ) ;
}

void N::SceneView::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QGLWidget::focusInEvent (event) ;
}

void N::SceneView::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QGLWidget::focusOutEvent(event) ;
}

void N::SceneView::actionEvent(QActionEvent * event)
{
  QGLWidget::actionEvent(event) ;
}

void N::SceneView::changeEvent(QEvent * event)
{
  QGLWidget::changeEvent(event) ;
}

void N::SceneView::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept() ; else
  QGLWidget::contextMenuEvent(event)           ;
}

void N::SceneView::dragEnterEvent(QDragEnterEvent * event)
{
  if (dragEnter(event)) event->acceptProposedAction() ; else {
    if (PassDragDrop) QGLWidget::dragEnterEvent(event)       ;
    else event->ignore()                                     ;
  }                                                          ;
}

void N::SceneView::dragLeaveEvent(QDragLeaveEvent * event)
{
  if (removeDrop()) event->accept() ; else             {
    if (PassDragDrop) QGLWidget::dragLeaveEvent(event) ;
    else event->ignore()                               ;
  }                                                    ;
}

void N::SceneView::dragMoveEvent(QDragMoveEvent * event)
{
  if (dragMove(event)) event->acceptProposedAction() ; else {
    if (PassDragDrop) QGLWidget::dragMoveEvent(event)       ;
    else event->ignore()                                    ;
  }                                                         ;
}

void N::SceneView::dropEvent(QDropEvent * event)
{
  if (drop(event)) event->acceptProposedAction() ; else {
    if (PassDragDrop) QGLWidget::dropEvent(event)       ;
    else event->ignore()                                ;
  }                                                     ;
}

void N::SceneView::enterEvent(QEvent * event)
{
  QGLWidget::enterEvent(event) ;
}

void N::SceneView::hideEvent(QHideEvent * event)
{
  QGLWidget::hideEvent(event) ;
}

void N::SceneView::inputMethodEvent(QInputMethodEvent * event)
{
  QGLWidget::inputMethodEvent(event) ;
}

void N::SceneView::keyPressEvent(QKeyEvent * event)
{
  QGLWidget::keyPressEvent(event) ;
}

void N::SceneView::keyReleaseEvent(QKeyEvent * event)
{
  QGLWidget::keyReleaseEvent(event) ;
}

void N::SceneView::leaveEvent(QEvent * event)
{
  QGLWidget::leaveEvent(event) ;
}

void N::SceneView::mouseDoubleClickEvent(QMouseEvent * event)
{
  UUIDs U = Universes.keys()                             ;
  SUID  u                                                ;
  foreach (u,U)                                          {
    if (Universes[u]->mouseDoubleClickEvent(this,event)) {
      event->accept()                                    ;
      return                                             ;
    }                                                    ;
  }                                                      ;
  QGLWidget::mouseDoubleClickEvent(event)                ;
}

void N::SceneView::mouseMoveEvent(QMouseEvent * event)
{
  UUIDs U = Universes.keys()                      ;
  SUID  u                                         ;
  foreach (u,U)                                   {
    if (Universes[u]->mouseMoveEvent(this,event)) {
      event->accept()                             ;
      return                                      ;
    }                                             ;
  }                                               ;
  QGLWidget::mouseMoveEvent(event)                ;
}

void N::SceneView::mousePressEvent(QMouseEvent * event)
{
  UUIDs U = Universes.keys()                       ;
  SUID  u                                          ;
  foreach (u,U)                                    {
    if (Universes[u]->mousePressEvent(this,event)) {
      event->accept()                              ;
      return                                       ;
    }                                              ;
  }                                                ;
  QGLWidget::mousePressEvent(event)                ;
}

void N::SceneView::mouseReleaseEvent(QMouseEvent * event)
{
  UUIDs U = Universes.keys()                         ;
  SUID  u                                            ;
  foreach (u,U)                                      {
    if (Universes[u]->mouseReleaseEvent(this,event)) {
      event->accept()                                ;
      return                                         ;
    }                                                ;
  }                                                  ;
  QGLWidget::mouseReleaseEvent(event)                ;
}

void N::SceneView::moveEvent(QMoveEvent * event)
{
  QGLWidget::moveEvent(event) ;
}

void N::SceneView::paintEvent(QPaintEvent * event)
{
  nIsolatePainter(QGLWidget) ;
}

void N::SceneView::tabletEvent(QTabletEvent * event)
{
  QGLWidget::tabletEvent(event) ;
}

void N::SceneView::wheelEvent(QWheelEvent * event)
{
  UUIDs U = Universes.keys()                  ;
  SUID  u                                     ;
  foreach (u,U)                               {
    if (Universes[u]->wheelEvent(this,event)) {
      event->accept()                         ;
      return                                  ;
    }                                         ;
  }                                           ;
  QGLWidget::wheelEvent(event)                ;
}

bool N::SceneView::acceptTap(QTapGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::acceptTapHold(QTapAndHoldGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::acceptPan(QPanGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::acceptPinch(QPinchGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::acceptSwipe(QSwipeGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::acceptCustom(QGesture * gesture)
{ Q_UNUSED ( gesture ) ;
  return false         ;
}

bool N::SceneView::FocusIn(void)
{
  LinkAction ( Copy , CopyToClipboard ( ) ) ;
  return true                               ;
}

bool N::SceneView::FocusOut(void)
{
  return true ;
}

void N::SceneView::initializeGL(void)
{
  GLint iLights = 0                            ;
  ::glGetIntegerv ( GL_MAX_LIGHTS , &iLights ) ;
  Lights = iLights                             ;
}

void N::SceneView::resizeGL(int,int)
{
  QRectF W = geometry ( )       ;
  if (W.width ()<=0) return     ;
  if (W.height()<=0) return     ;
  ///////////////////////////////
  UUIDs U = Universes.keys()    ;
  SUID  u                       ;
  foreach (u,U)                 {
    Universes[u]->Relocation(W) ;
  }                             ;
}

void N::SceneView::paintGL(void)
{
  nDropOut ( Painting )                ;
  if (diehardPainter()) return         ;
  Painting    = true                   ;
  QDateTime Last = nTimeNow            ;
  PaintGLs ++                          ;
  Prepare     ( )                      ;
  doUniverses ( )                      ;
  Flush       ( )                      ;
  PaintCosts += Last.msecsTo(nTimeNow) ;
  Painting = false                     ;
}

bool N::SceneView::diehardPainter(void)
{
  return false ;
}

void N::SceneView::initializeOverlayGL(void)
{
}

void N::SceneView::resizeOverlayGL(int width,int height)
{
}

void N::SceneView::paintOverlayGL(void)
{
}

bool N::SceneView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return true ;
}

bool N::SceneView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::SceneView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::SceneView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::SceneView::dropCameras(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  VarArgs Args                       ;
  Args << "DropCamera"               ;
  Args << pos                        ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

bool N::SceneView::dropAudios(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  AudioQueues << Uuids               ;
  VarArgs Args                       ;
  Args << "DropAudio"                ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

bool N::SceneView::dropVideos(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  VideoQueues << Uuids               ;
  VarArgs Args                       ;
  Args << "DropVideo"                ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

bool N::SceneView::dropAlbums(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  VarArgs Args                       ;
  Args << "DropAlbum"                ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

bool N::SceneView::dropPeople(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  VarArgs Args                       ;
  Args << "DropPeople"               ;
  Args << pos                        ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

bool N::SceneView::dropPictures(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  if (Uuids.count()<=0) return false ;
  SUID uuid                          ;
  VarArgs Args                       ;
  Args << "DropPicture"              ;
  Args << pos                        ;
  Args << Uuids.count()              ;
  foreach (uuid,Uuids) Args << uuid  ;
  addSequence(Args)                  ;
  return true                        ;
}

void N::SceneView::run(int Type,ThreadData * data)
{
  Run ( Type , data->Id ) ;
}

void N::SceneView::InstallDecisions(void)
{
  if ( plan  -> Booleans [ "Desktop" ] )         {
    Decide . setCondition ( DesktopGui , true  ) ;
    Decide . setCondition ( PadGui     , false ) ;
    Decide . setCondition ( PhoneGui   , false ) ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] )         {
    Decide . setCondition ( DesktopGui , false ) ;
    Decide . setCondition ( PadGui     , true  ) ;
    Decide . setCondition ( PhoneGui   , false ) ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] )         {
    Decide . setCondition ( DesktopGui , false ) ;
    Decide . setCondition ( PadGui     , false ) ;
    Decide . setCondition ( PhoneGui   , true  ) ;
  }
}

bool N::SceneView::startup(void)
{
  if (IsNull(destructor))                  {
    destructor = new SceneDestructor()     ;
  }                                        ;
  setAutoBufferSwap ( true               ) ;
  resizeGL          ( width() , height() ) ;
  InstallDecisions  (                    ) ;
  start             ( RunRendering       ) ;
  start             ( RunLoading         ) ;
  start             ( RunAnimation       ) ;
  start             ( RunExecuteCommand  ) ;
  resizeGL          ( width() , height() ) ;
  return true                              ;
}

bool N::SceneView::shutdown(void)
{
  if (!Alive)             {
    Finalize = true       ;
    return true           ;
  }                       ;
  Alive    = false        ;
  Finalize = true         ;
  setEnabled(false)       ;
  Commando -> stop ( )    ;
  finalize ( 100 )        ;
  while (!Stopped)        {
    plan->processEvents() ;
    Time::msleep(200)     ;
  }                       ;
  return true             ;
}

bool N::SceneView::shutup(void)
{
  Alive = false           ;
  setEnabled(false)       ;
  finalize ( 100 )        ;
  while (!Stopped)        {
    plan->processEvents() ;
    Time::msleep(200)     ;
  }                       ;
  return true             ;
}

void N::SceneView::Relocation(void)
{
  UUIDs  U = Multiverse()         ;
  if (U.count()<=0) return        ;
  SUID   u                        ;
  QRectF VW(0,0,width(),height()) ;
  foreach (u,U)                   {
    Universes[u]->Relocation(VW)  ;
  }                               ;
}

void N::SceneView::ThreadEvent(void)
{
  qApp -> processEvents ( ) ;
}

void N::SceneView::CopyToClipboard(void)
{
}

void N::SceneView::Prepare(void)
{
  if (Properties.count()>0)                        {
    QList<int> K = Properties.keys()               ;
    int k                                          ;
    foreach (k,K)                                  {
      bool enable = Properties[k]                  ;
      if (enable) ::glEnable  ( k )                ;
             else ::glDisable ( k )                ;
    }                                              ;
  }                                                ;
  //////////////////////////////////////////////////
  ::glShadeModel   ( GL_SMOOTH              )      ;
  ::glLineWidth    ( 1.0                    )      ;
  //////////////////////////////////////////////////
  ::glDepthFunc    ( GL_LESS                )      ;
  ::glBlendFunc    ( GL_SRC_ALPHA                  ,
                     GL_ONE_MINUS_SRC_ALPHA )      ;
  ::glMatrixMode   ( GL_PROJECTION          )      ;
  ::glLoadIdentity (                        )      ;
  ::glMatrixMode   ( GL_MODELVIEW           )      ;
  //////////////////////////////////////////////////
  ControlPoint pc = background                     ;
  ::glClear      (GL_COLOR_BUFFER_BIT              |
                  GL_DEPTH_BUFFER_BIT            ) ;
  ::glClearColor ( pc.x , pc.y , pc.z , 0.00f    ) ;
  ::glHint       ( GL_LINE_SMOOTH_HINT,GL_NICEST ) ;
}

void N::SceneView::Flush(void)
{
  ::glFlush ( ) ;
}

void N::SceneView::Motion(void)
{
  nDropOut ( Universes.count() <= 0 ) ;
  UUIDs Uuids = Universes.keys()      ;
  SUID  uuid                          ;
  foreach (uuid,Uuids)                {
    Universe * u = Universes[uuid]    ;
    if (NotNull(u))                   {
      if (u->Animation())             {
        Modified = true               ;
      }                               ;
    }                                 ;
  }                                   ;
}

void N::SceneView::doUniverses(void)
{
  UUIDs U = Universes.keys() ;
  SUID  u                    ;
  foreach (u,U)              {
    Universes[u]->Flush()    ;
  }                          ;
}

bool N::SceneView::JoinDestructor(SceneNode * node)
{
  if (IsNull(destructor)) return false ;
  (*destructor) += node                ;
  return true                          ;
}

int N::SceneView::addUniverse(SUID uuid,Universe * universe)
{
  universe->MaxLights  = Lights ;
  JoinDestructor ( universe )   ;
  Universes[uuid] = universe    ;
  return Universes.count()      ;
}

bool N::SceneView::moveUniverse(SUID from,SUID to)
{
  nKickOut ( !Universes.contains(from) , false ) ;
  Universe * u = Universes[from]                 ;
  nKickOut ( IsNull(u)                 , false ) ;
  Universes . remove ( from )                    ;
  Universes [ to ] = u                           ;
  return true                                    ;
}

void N::SceneView::GoFor(int Type)
{
  start ( Type ) ;
}

void N::SceneView::GoFor(int Type,VarArgs arguments)
{
  start ( Type , arguments ) ;
}

void N::SceneView::Run(int Type,int Id)
{
  switch (Type)              {
    case RunRendering        :
      Rendering        (Id)  ;
    break                    ;
    case RunLoading          :
      Loading          (Id)  ;
    break                    ;
    case RunAnimation        :
      Animation        (Id)  ;
    break                    ;
    case RunInputAudio       :
      InputAudio       (Id)  ;
    break                    ;
    case RunOutputAudio      :
      OutputAudio      (Id)  ;
    break                    ;
    case RunNetInputAudio    :
      NetInputAudio    (Id)  ;
    break                    ;
    case RunNetOutputAudio   :
      NetOutputAudio   (Id)  ;
    break                    ;
    case RunFeedAudio        :
      FeedAudio        (Id)  ;
    break                    ;
    case RunFeedVideo        :
      FeedVideo        (Id)  ;
    break                    ;
    case RunFeedInterface    :
      FeedInterface    (Id)  ;
    break                    ;
    case RunAcceptInterface  :
      AcceptInterface  (Id)  ;
    break                    ;
    case RunFeedCommand      :
      FeedCommand      (Id)  ;
    break                    ;
    case RunAcceptCommand    :
      AcceptCommand    (Id)  ;
    break                    ;
    case RunExecuteCommand   :
      ExecuteCommand   (Id)  ;
    break                    ;
    case RunSemanticAnalysis :
      SemanticAnalysis (Id)  ;
    break                    ;
    case RunDownload         :
      Download         (Id)  ;
    break                    ;
    case RunUpload           :
      Upload           (Id)  ;
    break                    ;
    case RunFeedObjects      :
      FeedObjects      (Id)  ;
    break                    ;
    case RunAcceptObjects    :
      AcceptObjects    (Id)  ;
    break                    ;
    case RunCapturing        :
      Capturing        (Id)  ;
    break                    ;
    case RunCustom           :
      Custom           (Id)  ;
    break                    ;
  }                          ;
}

void N::SceneView::Rendering(int Id)
{ Q_UNUSED ( Id )                                ;
  bool      next = false                         ;
  QDateTime Now  = nTimeNow                      ;
  QDateTime Last = nTimeNow                      ;
  Stopped        = false                         ;
  StartReflush   = nTimeNow                      ;
  PaintGLs       = 0                             ;
  PaintCosts     = 0                             ;
  while (Alive)                                  {
    next = true                                  ;
    if (!Modified) next = false                  ;
    if ( Painting) next = false                  ;
    if (Last.msecsTo(Now)<Interval) next = false ;
    if (next)                                    {
      emit Reflush ( )                           ;
      Modified = false                           ;
      Last     = nTimeNow                        ;
    }                                            ;
    Now = nTimeNow                               ;
    int ds = Last.msecsTo(Now)                   ;
    if (ds>=0 && ds<Interval)                    {
      ds = Interval - ds                         ;
      if (ds>20) ds = 20                         ;
      if (ds>3 ) Time :: msleep ( ds )           ;
    }                                            ;
    Now = nTimeNow                               ;
    if (Adaptive && PaintGLs>10)                 {
      int ms = StartReflush.msecsTo(nTimeNow)    ;
      ms /= PaintGLs                             ;
      if (ms>=17) Interval = ms + 3              ;
    }                                            ;
  }                                              ;
  Stopped = true                                 ;
}

void N::SceneView::Loading(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::Animation(int Id)
{ Q_UNUSED ( Id )                     ;
  QDateTime Now  = nTimeNow           ;
  QDateTime Last = nTimeNow           ;
  while (Alive)                       {
    int ds = Now.msecsTo(Last)        ;
    if (ds<MotionInterval)            {
      ds = MotionInterval - ds        ;
      if (ds>3) Time :: msleep ( ds ) ;
    }                                 ;
    Now  = nTimeNow                   ;
    Last = nTimeNow                   ;
    Motion ( )                        ;
  }                                   ;
}

bool N::SceneView::UniverseMenu(QPoint pos)
{
  UUIDs U = Universes.keys()          ;
  SUID  u                             ;
  foreach (u,U)                       {
    if (Universes[u]->Menu(this,pos)) {
      return true                     ;
    }                                 ;
  }                                   ;
  return false                        ;
}

bool N::SceneView::ReflushMenu(MenuManager & menu,QPoint)
{
  if (PaintGLs<=0) return true                   ;
  QString   m                                    ;
  double    r                                    ;
  QAction * aa                                   ;
  r  = ReflushRate()                             ;
  m  = tr("%1 (frame/s)"  ).arg(r,0,'f',4)       ;
  aa = menu.add(1000009,m)                       ;
  aa-> setEnabled(false)                         ;
  r  = PaintCosts                                ;
  r /= PaintGLs                                  ;
  m  = tr("%1 (ms/render)").arg(r,0,'f',4)       ;
  aa = menu.add(1000010,m)                       ;
  aa-> setEnabled(false)                         ;
  m  = tr("%1 universes").arg(Universes.count()) ;
  aa = menu.add(1000011,m)                       ;
  aa-> setEnabled(false)                         ;
  menu.addSeparator()                            ;
  return true                                    ;
}

bool N::SceneView::ContentMenu(MenuManager & menu,QPoint pos)
{
  QMenu   * mc = menu.addMenu(tr("Device contents"))         ;
  QAction * aa                                               ;
  menu.add(mc,901,tr("Background color"       ))             ;
  menu.add(mc,902,tr("Set reflush interval"   ))             ;
  menu.addSeparator(mc)                                      ;
  if (Alive) menu.add(mc,951,tr("Stop automatic painting" )) ;
        else menu.add(mc,952,tr("Start automatic painting")) ;
  aa = menu.add(mc,961,tr("Adaptive reflush rate"))          ;
  aa->setCheckable(true)                                     ;
  aa->setChecked(Adaptive)                                   ;
  aa = menu.add(mc,962,tr("Forcing rendering"))              ;
  aa->setCheckable(true)                                     ;
  aa->setChecked(Timer->isActive())                          ;
  return true                                                ;
}

bool N::SceneView::ContentProcessor(MenuManager & menu,QAction * aa)
{
  if (IsNull(aa)) return false               ;
  switch (menu[aa])                          {
    case 901                                 :
      assignBackground()                     ;
      Modified = true                        ;
    return true                              ;
    case 902                               : {
      int  interval = Interval               ;
      bool okay     = false                  ;
      interval = QInputDialog::getInt        (
                  this                       ,
                  tr("Set reflush interval") ,
                  tr("Interval(ms):"       ) ,
                  interval                   ,
                  1                          ,
                  3600 * 1000                ,
                  1                          ,
                  &okay                    ) ;
      if (okay) Interval = interval          ;
    }                                        ;
    return true                              ;
    case 951                                 :
      Alive    = false                       ;
    return true                              ;
    case 952                                 :
      Alive    = true                        ;
      Modified = true                        ;
      start ( RunRendering      )            ;
      start ( RunAnimation      )            ;
      start ( RunExecuteCommand )            ;
    return true                              ;
    case 961                                 :
      Adaptive = aa->isChecked()             ;
    return true                              ;
    case 962                                 :
      if (aa->isChecked())                   {
        Timer -> start ( Interval )          ;
      } else                                 {
        Timer -> stop  (          )          ;
      }                                      ;
    return true                              ;
  }                                          ;
  return false                               ;
}

bool N::SceneView::ThreadsMenu(MenuManager & menu,QPoint)
{
  cleanup ( )                                            ;
  QMenu * tc = menu.addMenu(tr("Threads"))               ;
  QString m                                              ;
  m = tr("%1 activated threads").arg(AllThreads.count()) ;
  menu.add(tc,851,m)                                     ;
  return true                                            ;
}

bool N::SceneView::ThreadsProcessor(MenuManager & menu,QAction * aa)
{
  if (IsNull(aa)) return false ;
  switch (menu[aa])            {
    case 851                   :
      cleanup          ( )     ;
      ThreadController ( )     ;
    return true                ;
  }                            ;
  return false                 ;
}

void N::SceneView::ThreadController(void)
{
}

bool N::SceneView::BridgeMenu(MenuManager & menu,QPoint)
{
  QMenu * tc = menu.addMenu(tr("Data bridges")) ;
  QMenu * tj = menu.addMenu(tc,tr("Junctions")) ;
  QMenu * ta = menu.addMenu(tc,tr("Conduits" )) ;
  QString js                                    ;
  QString cs                                    ;
  js = tr("%1 junctions").arg(JunctionCount())  ;
  cs = tr("%1 conduits" ).arg(ConduitCount ())  ;
  menu . add ( tj , 4447001 , js )              ;
  menu . add ( ta , 4448001 , cs )              ;
  return true                                   ;
}

bool N::SceneView::BridgeProcessor(MenuManager & menu,QAction * aa)
{
  if (IsNull(aa)) return false ;
  switch (menu[aa])            {
    case 4447001               :
    return true                ;
    case 4448001               :
    return true                ;
  }                            ;
  return false                 ;
}

bool N::SceneView::MultimediaMenu(MenuManager & menu,QPoint)
{
  QMenu        * mu = menu.addMenu(   tr("Multimedia"  )) ;
  /////////////////////////////////////////////////////////
  QString        m                                        ;
  m    = tr("%1 albums").arg(AlbumQueues.count())         ;
  menu . add(mu,4441001,m)                                ;
  m    = tr("%1 video clips").arg(VideoQueues.count())    ;
  menu . add(mu,4441002,m)                                ;
  m    = tr("%1 audios").arg(AudioQueues.count())         ;
  menu . add(mu,4441003,m)                                ;
  menu . addSeparator( mu         )                       ;
  /////////////////////////////////////////////////////////
  QMenu        * ai = menu.addMenu(mu,tr("Audio input" )) ;
  QMenu        * ao = menu.addMenu(mu,tr("Audio output")) ;
  QMenu        * ac = menu.addMenu(mu,tr("Cameras"     )) ;
  QActionGroup * aig                                      ;
  QActionGroup * aog                                      ;
  QActionGroup * acg                                      ;
  aig = Acoustics::DeviceMenu (plan,Acoustics::Input ,ai) ;
  aog = Acoustics::DeviceMenu (plan,Acoustics::Output,ao) ;
  acg = Acoustics::DeviceMenu (plan,Acoustics::Camera,ac) ;
  menu . setGroup    ( 1001 , aig      )                  ;
  menu . setGroup    ( 1002 , aog      )                  ;
  menu . setGroup    ( 1003 , acg      )                  ;
  PrepareAudioInput  ( aig             )                  ;
  PrepareAudioOutput ( aog             )                  ;
  PrepareCameraMenu  ( acg             )                  ;
  AskForAudioInput   ( menu , ai , aig )                  ;
  return true                                             ;
}

bool N::SceneView::AskForAudioInput(MenuManager & menu,QMenu * m,QActionGroup * group)
{ Q_UNUSED ( group )                                                          ;
  /////////////////////////////////////////////////////////////////////////////
  if (!DeviceProperties.contains(N::PurePlan::DefaultAudioInput)) return true ;
  QString dn = DeviceProperties[N::PurePlan::DefaultAudioInput]               ;
  QList<QAudioDeviceInfo> dis                                                 ;
  QAudioDeviceInfo        adi                                                 ;
  bool                    correct = false                                     ;
  dis = QAudioDeviceInfo::availableDevices(QAudio::AudioInput)                ;
  for (int i=0;!correct && i<dis.count();i++)                                 {
    if ( dis[i].deviceName() == dn )                                          {
      adi     = dis [ i ]                                                     ;
      correct = true                                                          ;
    }                                                                         ;
  }                                                                           ;
  if (!correct) return true                                                   ;
  /////////////////////////////////////////////////////////////////////////////
  menu . addSeparator ( m )                                                   ;
  QComboBox * recChannels = new QComboBox ( this )                            ;
  QComboBox * recRate     = new QComboBox ( this )                            ;
  QComboBox * recTypes    = new QComboBox ( this )                            ;
  QCheckBox * recPanel    = new QCheckBox ( this )                            ;
  CUIDs       cc          = adi . supportedChannelCounts ( )                  ;
  CUIDs       cr          = adi . supportedSampleRates   ( )                  ;
  CUIDs       cs          = adi . supportedSampleSizes   ( )                  ;
  int         id                                                              ;
  int         counting    =  0                                                ;
  int         defaultRate = -1                                                ;
  int         defaultBits = -1                                                ;
  recPanel -> setText    ( tr("Display panel") )                              ;
  recPanel -> setChecked ( true                )                              ;
  foreach (id,cc)                                                             {
    switch (id)                                                               {
      case 1                                                                  :
        recChannels -> addItem ( tr("Mono"  )              , id )             ;
      break                                                                   ;
      case 2                                                                  :
        recChannels -> addItem ( tr("Stereo")              , id )             ;
      break                                                                   ;
      default                                                                 :
        recChannels -> addItem ( tr("%1 channels").arg(id) , id )             ;
      break                                                                   ;
    }                                                                         ;
  }                                                                           ;
  counting = 0                                                                ;
  foreach (id,cr)                                                             {
    recRate     -> addItem ( QString("%1").arg(id) ,id )                      ;
    if (id==44100) defaultRate = counting                                     ;
    if (id==48000) defaultRate = counting                                     ;
    counting++                                                                ;
  }                                                                           ;
  counting = 0                                                                ;
  foreach (id,cs)                                                             {
    recTypes    -> addItem ( tr("%1 bits").arg(id) ,id )                      ;
    if (id==16) defaultBits = counting                                        ;
    counting++                                                                ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  if (defaultRate>=0) recRate  -> setCurrentIndex ( defaultRate )             ;
  if (defaultBits>=0) recTypes -> setCurrentIndex ( defaultBits )             ;
  /////////////////////////////////////////////////////////////////////////////
  recChannels -> setInsertPolicy ( QComboBox::NoInsert )                      ;
  recRate     -> setInsertPolicy ( QComboBox::NoInsert )                      ;
  recTypes    -> setInsertPolicy ( QComboBox::NoInsert )                      ;
  recChannels -> setEditable     ( true                )                      ;
  recRate     -> setEditable     ( true                )                      ;
  recTypes    -> setEditable     ( true                )                      ;
  /////////////////////////////////////////////////////////////////////////////
  menu . add ( m , 4442101 , recChannels )                                    ;
  menu . add ( m , 4442102 , recRate     )                                    ;
  menu . add ( m , 4442103 , recTypes    )                                    ;
  menu . add ( m , 4442104 , recPanel    )                                    ;
  menu . addSeparator ( m )                                                   ;
  menu . add ( m , 4442001 , tr("Add recording channel") )                    ;
  return true                                                                 ;
}

bool N::SceneView::AudioInputProcessor(MenuManager & menu,QAction * aa)
{
  if ( IsNull(aa)          ) return false                                            ;
  if ( 4442001 != menu[aa] ) return false                                            ;
  QComboBox * recChannels = (QComboBox *)menu.widget(4442101)                        ;
  QComboBox * recRate     = (QComboBox *)menu.widget(4442102)                        ;
  QComboBox * recTypes    = (QComboBox *)menu.widget(4442103)                        ;
  QCheckBox * recPanel    = (QCheckBox *)menu.widget(4442104)                        ;
  int  CHANNELS = recChannels -> itemData  ( recChannels -> currentIndex() ).toInt() ;
  int  RATE     = recRate     -> itemData  ( recRate     -> currentIndex() ).toInt() ;
  int  BITXS    = recTypes    -> itemData  ( recTypes    -> currentIndex() ).toInt() ;
  bool PANEL    = recPanel    -> isChecked (                                       ) ;
  QString dn = DeviceProperties[N::PurePlan::DefaultAudioInput]                      ;
  VarArgs Args                                                                       ;
  Args << "AddRecorder"                                                              ;
  Args << dn                                                                         ;
  Args << CHANNELS                                                                   ;
  Args << RATE                                                                       ;
  Args << BITXS                                                                      ;
  Args << PANEL                                                                      ;
  addSequence(Args)                                                                  ;
  return true                                                                        ;
}

void N::SceneView::AddRecorder(VarArgs & args)
{
  if (args.count()!=6) return                  ;
  QString dn       = args[1].toString ( )      ;
  int     CHANNELS = args[2].toInt    ( )      ;
  int     RATE     = args[3].toInt    ( )      ;
  int     BITXS    = args[4].toInt    ( )      ;
  bool    PANEL    = args[5].toBool   ( )      ;
  //////////////////////////////////////////////
  QMap<int,int> SampleMaps                     ;
  SampleMaps[ 8] = Acoustics::UInt8            ;
  SampleMaps[16] = Acoustics::Int16            ;
  SampleMaps[32] = Acoustics::Float32          ;
  SampleMaps[64] = Acoustics::Double           ;
  if (!SampleMaps.contains(BITXS)) return      ;
  //////////////////////////////////////////////
  Junction  * junc                             ;
  AudioJack * jack                             ;
  int MID = RequestInput                       (
             dn                                ,
             RATE                              ,
             BITXS                             ,
             CHANNELS                          ,
             SampleMaps[BITXS]               ) ;
  jack = (AudioJack *)plan->AudioChannels[MID] ;
  //////////////////////////////////////////////
  junc  = MountChannel ( MID                 ) ;
  if (PANEL)                                   {
    AudioAcceptor * wact = NULL                ;
    wact = WaveDisplay ( junc , jack )         ;
    if (NotNull(wact))                         {
      #ifdef USE_AUDIO_THROTTLE
      EnableWaveThrottle ( wact )              ;
      #endif
      junc -> addConduit ( wact              ) ;
    }                                          ;
  }                                            ;
  plan -> Dredge ( true )                      ;
  //////////////////////////////////////////////
  VarArgs Args                                 ;
  Args << 30003                                ;
  Args << MID                                  ;
  PostSequence ( Args )                        ;
}

void N::SceneView::LaunchRecorder(VarArgs args)
{
  AudioJack * jack                             ;
  int MID = args[1].toInt()                    ;
  jack = (AudioJack *)plan->AudioChannels[MID] ;
  jack -> play ( )                             ;
}

bool N::SceneView::MultimediaProcessor(MenuManager & menu,QAction * aa)
{
  if (IsNull(aa)) return false                     ;
  QActionGroup * aig                               ;
  QActionGroup * aog                               ;
  QActionGroup * acg                               ;
  aig = menu.group(1001)                           ;
  aog = menu.group(1002)                           ;
  acg = menu.group(1003)                           ;
  if (NotNull(aig) && aig->actions().contains(aa)) {
    return AudioInputMenu  ( aig , aa )            ;
  }                                                ;
  if (NotNull(aog) && aog->actions().contains(aa)) {
    return AudioOutputMenu ( aog , aa )            ;
  }                                                ;
  if (NotNull(acg) && acg->actions().contains(aa)) {
    return CameraInputMenu ( acg , aa )            ;
  }                                                ;
  switch (menu[aa])                                {
    case 4441001                                   :
    return CallForAlbum ( )                        ;
    case 4441002                                   :
    return CallForVideo ( )                        ;
    case 4441003                                   :
    return CallForAudio ( )                        ;
  }                                                ;
  return false                                     ;
}

bool N::SceneView::CallForAlbum(void)
{
  if (!plan->Talkative(EnableDebugger)) return true ;
  if (AlbumQueues.count()<=0          ) return true ;
  SUID u                                            ;
  foreach (u,AlbumQueues)                           {
    if (AlbumNames.contains(u))                     {
      PlanDebug ( 45 , AlbumNames[u] )              ;
    }                                               ;
  }                                                 ;
  return true                                       ;
}

bool N::SceneView::CallForVideo(void)
{
  if (!plan->Talkative(EnableDebugger)) return true   ;
  if (VideoQueues.count()<=0          ) return true   ;
  SUID u                                              ;
  foreach (u,VideoQueues)                             {
    if (VideoNames.contains(u))                       {
      QString nn = VideoNames  [ u ]                  ;
      QString ap = VideoPathes [ u ]                  ;
      QString mm = QString("%1 : %2").arg(nn).arg(ap) ;
      PlanDebug ( 45 , mm )                           ;
    }                                                 ;
  }                                                   ;
  return true                                         ;
}

bool N::SceneView::CallForAudio(void)
{
  if (!plan->Talkative(EnableDebugger)) return true   ;
  if (AudioQueues.count()<=0          ) return true   ;
  SUID u                                              ;
  foreach (u,AudioQueues)                             {
    if (AudioNames.contains(u))                       {
      QString nn = AudioNames  [ u ]                  ;
      QString ap = AudioPathes [ u ]                  ;
      QString mm = QString("%1 : %2").arg(nn).arg(ap) ;
      PlanDebug ( 45 , mm )                           ;
    }                                                 ;
  }                                                   ;
  return true                                         ;
}

bool N::SceneView::Menu(QPoint pos)
{
  if (UniverseMenu(pos)) return true                 ;
  MenuManager mm(this)                               ;
  nKickOut ( ! ReflushMenu    ( mm , pos ) , false ) ;
  nKickOut ( ! ContentMenu    ( mm , pos ) , false ) ;
  nKickOut ( ! ThreadsMenu    ( mm , pos ) , false ) ;
  nKickOut ( ! BridgeMenu     ( mm , pos ) , false ) ;
  nKickOut ( ! MultimediaMenu ( mm , pos ) , false ) ;
  QAction * aa = NULL                                ;
  mm . setFont ( plan )                              ;
  aa = mm.exec (      )                              ;
  nKickOut ( IsNull(aa) , true )                     ;
  if ( ContentProcessor    ( mm , aa ) ) return true ;
  if ( ThreadsProcessor    ( mm , aa ) ) return true ;
  if ( BridgeProcessor     ( mm , aa ) ) return true ;
  if ( AudioInputProcessor ( mm , aa ) ) return true ;
  if ( MultimediaProcessor ( mm , aa ) ) return true ;
  return false                                       ;
}

N::CameraBox * N::SceneView::addCameraControl(SUID cameraUuid,double length)
{
  CameraBox * T = new CameraBox ( this )                     ;
  JoinDestructor ( T )                                       ;
  T-> node = cameraUuid                                      ;
  T-> setLength ( length     , plan->fonts[Fonts::Default] ) ;
  addUniverse   ( cameraUuid , T                           ) ;
  connect(T   ,SIGNAL(Close          (nCameraBox*))          ,
          this,SLOT  (CloseController(nCameraBox*))        ) ;
  return T                                                   ;
}

void N::SceneView::CloseController(CameraBox * camera)
{
  removeUniverse(camera->node) ;
}

N::Universe * N::SceneView::addColorDistribution(SUID uuid,QByteArray & colors)
{
  ControlPoint   R (0,0,1,  0)                       ;
  ControlPoint   Z (0,0,1,270)                       ;
  ControlPoint   X (1,0,0,270)                       ;
  ControlPoint   S ( 128 , 128 , 128 )               ;
  Universe     * U = new Universe     ( )            ;
  Camera       * C = new Camera       ( )            ;
  SceneObject  * O = new SceneObject  ( )            ;
  SceneObject  * G = new SceneObject  ( )            ;
  ////////////////////////////////////////////////////
  JoinDestructor ( U )                               ;
  JoinDestructor ( C )                               ;
  JoinDestructor ( O )                               ;
  JoinDestructor ( G )                               ;
  ////////////////////////////////////////////////////
  R.DOF = ControlPoint::DofRotate                    ;
  Z.DOF = ControlPoint::DofRotate                    ;
  X.DOF = ControlPoint::DofRotate                    ;
  ////////////////////////////////////////////////////
  O-> addColorCube( 256 , S                        ) ;
  O-> addSequence ( 0,SceneObject::RenderSegments  ) ;
  ////////////////////////////////////////////////////
  G-> addColors   ( colors                         ) ;
  G-> addSequence ( 0,SceneObject::RenderColors    ) ;
  ////////////////////////////////////////////////////
  C-> mode   = Camera::Perfect                       ;
  C-> FOV    = 30                                    ;
  C-> eye    = ControlPoint (100.0,100.0,260.0)      ;
  C-> center = ControlPoint (  0.0,  0.0,  0.0)      ;
  C-> up     = ControlPoint (  0.0,  1.0,  0.0)      ;
  C-> addDof           ( 0 , X     )                 ;
  C-> addDof           ( 1 , Z     )                 ;
  C-> setNearFar       ( 1 , 10000 )                 ;
  ////////////////////////////////////////////////////
  U-> addDof           ( 0 , R     )                 ;
  U-> addCamera        ( 0 , C     )                 ;
  U-> setCurrentCamera ( 0         )                 ;
  U-> addActor         ( 0 , O     )                 ;
  U-> addActor         ( 1 , G     )                 ;
  U-> Perform          ( 0 , true  )                 ;
  U-> Perform          ( 1 , true  )                 ;
  ////////////////////////////////////////////////////
  addUniverse          ( uuid , U  )                 ;
  ////////////////////////////////////////////////////
  return U                                           ;
}

int N::SceneView::JunctionCount(void)
{
  plan -> Rehabilitate ( )       ;
  return plan->junctions.count() ;
}

int N::SceneView::ConduitCount(void)
{
  if (plan->junctions.count()<=0) return 0       ;
  Conduits conduits                              ;
  plan -> Rehabilitate ( )                       ;
  for (int i=0;i<plan->junctions.count();i++)    {
    Junction * junc                              ;
    junc = plan->junctions[i]                    ;
    if (junc->conduits.count()>0)                {
      Conduit * cds                              ;
      for (int j=0;j<junc->conduits.count();j++) {
        cds = junc->conduits[j]                  ;
        if (!conduits.contains(cds))             {
          conduits << cds                        ;
        }                                        ;
      }                                          ;
    }                                            ;
  }                                              ;
  return conduits . count ( )                    ;
}

QMap<SUID,QImage *> N::SceneView::FetchImages(SqlConnection & SC,UUIDs & Uuids)
{
  QMap<SUID,QImage *> Images                     ;
  SUID u                                         ;
  PictureManager PM ( plan )                     ;
  foreach (u,Uuids)                              {
    if (ImageDepot.contains(u))                  {
      Images     [ u ] = ImageDepot [ u ]        ;
    } else                                       {
      Images     [ u ] = PM . Picture ( SC , u ) ;
      ImageDepot [ u ] = Images [ u ]            ;
    }                                            ;
  }                                              ;
  return Images                                  ;
}

QMap<SUID,QImage *> N::SceneView::FetchImages(UUIDs & Uuids)
{
  QMap<SUID,QImage *> Images           ;
  SqlConnection SC ( plan -> sql )     ;
  if (SC.open("SceneV","FetchImages")) {
    Images = FetchImages(SC,Uuids)     ;
    SC.close()                         ;
  }                                    ;
  SC.remove()                          ;
  return Images                        ;
}

bool N::SceneView::RemoveImage(SUID u)
{
  if (!ImageDepot.contains(u)) return false ;
  QImage * I = ImageDepot [ u ]             ;
  ImageDepot . take ( u )                   ;
  delete I                                  ;
  return true                               ;
}

void N::SceneView::PrepareAudioInput(QActionGroup * group)
{
  if (IsNull(group)) return                                               ;
  if (!DeviceProperties.contains(N::PurePlan::DefaultAudioInput )) return ;
  QList<QAction *> actions = group->actions()                             ;
  QString          n                                                      ;
  n = DeviceProperties [ N::PurePlan::DefaultAudioInput  ]                ;
  for (int i=0;i<actions.count();i++)                                     {
    actions[i] -> setChecked ( actions[i]->text() == n )                  ;
  }                                                                       ;
}

void N::SceneView::PrepareAudioOutput(QActionGroup * group)
{
  if (IsNull(group)) return                                               ;
  if (!DeviceProperties.contains(N::PurePlan::DefaultAudioOutput)) return ;
  QList<QAction *> actions = group->actions()                             ;
  QString          n                                                      ;
  n = DeviceProperties [ N::PurePlan::DefaultAudioOutput ]                ;
  for (int i=0;i<actions.count();i++)                                     {
    actions[i] -> setChecked ( actions[i]->text() == n )                  ;
  }                                                                       ;
}

void N::SceneView::PrepareCameraMenu(QActionGroup * group)
{
  if (IsNull(group)) return                                               ;
  if (!DeviceProperties.contains(N::PurePlan::DefaultCamera     )) return ;
  QList<QAction *> actions = group->actions()                             ;
  QString          n                                                      ;
  n = DeviceProperties [ N::PurePlan::DefaultCamera      ]                ;
  for (int i=0;i<actions.count();i++)                                     {
    actions[i] -> setChecked ( actions[i]->text() == n )                  ;
  }                                                                       ;
}

bool N::SceneView::AudioInputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                  ;
  DeviceProperties [ N::PurePlan::DefaultAudioInput  ] = aa->text() ;
  return true                                                       ;
}

bool N::SceneView::AudioOutputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                  ;
  DeviceProperties [ N::PurePlan::DefaultAudioOutput ] = aa->text() ;
  return true                                                       ;
}

bool N::SceneView::CameraInputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                  ;
  DeviceProperties [ N::PurePlan::DefaultCamera      ] = aa->text() ;
  return true                                                       ;
}

N::WaveUniverse * N::SceneView::AllocateWave (void)
{
  WaveUniverse * wu              ;
  wu = new WaveUniverse ( this ) ;
  JoinDestructor ( wu )          ;
  return wu                      ;
}

N::AudioAcceptor * N::SceneView::WaveDisplay(Junction * junc,AudioChannel * feeder)
{ Q_UNUSED ( junc )                                   ;
  AudioAcceptor  * accp = NULL                        ;
  AudioTributary * trib = new AudioTributary ( this ) ;
  feeder -> assign            ( *trib            )    ;
  accp    = new AudioAcceptor ( this , trib      )    ;
  /////////////////////////////////////////////////////
  QRectF           vw ( 0 , 0 , width() , height() )  ;
  for (int i=0;i<trib->Channels();i++)                {
    WaveUniverse * wu = AllocateWave ( )              ;
    wu -> setConfigure                                (
          trib -> Rate   ( )                          ,
          trib -> Format ( )                          ,
          QPoint (   0 , i * 96 )                     ,
          QSize  ( 256 ,     96 )                     ,
          5000                                        ,
          10000                                       ,
          12500                                       ,
          QColor(128, 96, 96, 64)                     ,
          QColor(255,  0,  0,255)                     ,
          QColor(  0,255, 64,255)                   ) ;
    addUniverse ( RequestUniverse(21001) , wu )       ;
    wu   -> Relocation ( vw )                         ;
    trib -> Acceptors [ i ] = wu                      ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return accp                                         ;
}

void N::SceneView::EnableWaveThrottle(AudioAcceptor * waveu)
{
  if (IsNull(waveu)) return                    ;
  QDateTime        BT = nTimeNow               ;
  AudioTributary * trib                        ;
  CUIDs            IDx                         ;
  int              idx                         ;
  trib = (AudioTributary *)waveu->audio        ;
  IDx = trib->Acceptors.keys()                 ;
  foreach (idx,IDx)                            {
    WaveUniverse * wux                         ;
    wux = (WaveUniverse *)trib->Acceptors[idx] ;
    wux->BypassThrottle = false                ;
    wux->ThrottleTime   = BT                   ;
  }                                            ;
}

N::AudioAcceptor * N::SceneView::SpeakerOutlet(AudioFeeder * feeder)
{
  if (!DeviceProperties.contains(PurePlan::DefaultAudioOutput)) return NULL ;
  QString         outd = DeviceProperties[PurePlan::DefaultAudioOutput]     ;
  AudioAcceptor * accp = NULL                                               ;
  AudioOut      * outp = new AudioOut ( this )                              ;
  outp   -> setDevice         (  outd            )                          ;
  feeder -> assign            ( *outp            )                          ;
  accp    = new AudioAcceptor ( this , outp      )                          ;
  outp   -> setMethod         ( AudioIO::Removal )                          ;
  return accp                                                               ;
}

int N::SceneView::RequestChannel(QString filename)
{
  CUIDs         IDs = plan->AudioChannels.keys()    ;
  int           MID = 0                             ;
  AudioFeeder * aio                                 ;
  aio  = new AudioFeeder (      )                   ;
  aio -> setParent       ( this )                   ;
  aio -> Filename = filename                        ;
  if (IDs.count()>0)                                {
    for (int i=0;i<IDs.count();i++)                 {
      if (IDs[i]>MID) MID = IDs[i]                  ;
    }                                               ;
  }                                                 ;
  MID++                                             ;
  plan->AudioChannels[MID] = (N::AudioChannel *)aio ;
  aio->PostSequences = &GuiSequences                ;
  return MID                                        ;
}

int N::SceneView::RequestInput(QString device,int rate,int bits,int channels,int format)
{
  CUIDs       IDs = plan->AudioChannels.keys()    ;
  int         MID = 0                             ;
  AudioJack * jack                                ;
  jack  = new AudioJack (        )                ;
  jack -> setParent     ( this   )                ;
  jack -> setDevice     ( device )                ;
  jack -> set                                     (
    rate                                          ,
    bits                                          ,
    channels                                      ,
    (N::Acoustics::SampleFormat)format          ) ;
  if (IDs.count()>0)                              {
    for (int i=0;i<IDs.count();i++)               {
      if (IDs[i]>MID) MID = IDs[i]                ;
    }                                             ;
  }                                               ;
  MID++                                           ;
  plan->AudioChannels[MID] = (AudioChannel *)jack ;
  return MID                                      ;
}

N::Junction * N::SceneView::MountChannel(int MID)
{
  nKickOut ( !plan->AudioChannels.contains(MID) , NULL )       ;
  AudioConduit  * acot                                         ;
  AudioJunction * junc                                         ;
  acot = new AudioConduit  ( this , plan->AudioChannels[MID] ) ;
  junc = new AudioJunction ( this                            ) ;
  plan->junctions << junc                                      ;
  junc->interval = 90                                          ;
  junc->addConduit ( acot )                                    ;
  return (Junction *)junc                                      ;
}

void N::SceneView::AudioFeederClosed(VarArgs args)
{
  if (args.count()!=4) return                         ;
  int           Id = args[0].toInt   ()               ;
  QString       AR = args[1].toString()               ;
  QString       FD = args[2].toString()               ;
  AudioFeeder * AF = FromVariant(AudioFeeder,args[3]) ;
  if ( Id != 40001         ) return                   ;
  if ( AR != "AudioReader" ) return                   ;
  if ( FD != "Finished"    ) return                   ;
  if ( IsNull(AF)          ) return                   ;
  ChannelClosed ( AF )                                ;
}

void N::SceneView::ChannelClosed(AudioFeeder * feeder)
{
  VarArgs Args                   ;
  Args << "ShutAudio"            ;
  Args << VoidVariant ( feeder ) ;
  addSequence(Args)              ;
}

void N::SceneView::AudioFailure(SUID uuid,QString filename)
{ Q_UNUSED   ( uuid     ) ;
  Q_UNUSED   ( filename ) ;
  tossAudios (          ) ;
}

void N::SceneView::ShutAudio(VarArgs & args)
{
  if (args.count()!=2) return                                ;
  AudioFeeder * feeder = FromVariant(AudioFeeder,args[1])    ;
  if (IsNull(feeder)) return                                 ;
  feeder -> finalize ( 1000  )                               ;
  ////////////////////////////////////////////////////////////
  int   AC  = -1                                             ;
  CUIDs IDs = plan->AudioChannels.keys()                     ;
  for (int i=0;AC<0 && i<IDs.count();i++)                    {
    int id = IDs[i]                                          ;
    if ( plan->AudioChannels[id] == feeder ) AC = id         ;
  }                                                          ;
  if (AC<0) return                                           ;
  plan -> AudioChannels . take ( AC )                        ;
  ////////////////////////////////////////////////////////////
  QList<Conduit   *> CONDUITs                                ;
  for (int i=0;i<plan->junctions.count();i++)                {
    for (int j=0;j<plan->junctions[i]->conduits.count();j++) {
      Conduit * cx = plan->junctions[i]->conduits[j]         ;
      if ( 1001 == cx->type() )                              {
        AudioConduit * acx = (AudioConduit *)cx              ;
        if ( acx->channel == feeder )                        {
          CONDUITs << cx                                     ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  if (CONDUITs.count()<=0) return                            ;
  ////////////////////////////////////////////////////////////
  Junctions junc                                             ;
  for (int i=0;i<CONDUITs.count();i++)                       {
    junc << plan->Connectivity(CONDUITs[i])                  ;
  }                                                          ;
  if (junc.count()<=0) return                                ;
  ////////////////////////////////////////////////////////////
  QList<AudioData *> DATAs                                   ;
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 1001 == jc->type() )                                {
      AudioJunction * aj = (AudioJunction *)jc               ;
      Conduits        cs = aj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        if ( 1002 == ca->type() )                            {
          AudioAcceptor * aa = (AudioAcceptor *)ca           ;
          AudioData     * ad = aa->audio                     ;
          if (NotNull(ad)) DATAs << ad                       ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  bool done = false                                          ;
  while (!done)                                              {
    done = true                                              ;
    for (int i=0;i<junc.count();i++)                         {
      for (int j=0;j<junc[i]->conduits.count();j++)          {
        Conduit * cx = junc[i]->conduits[j]                  ;
        if (!CONDUITs.contains(cx))                          {
          if ( 1002 == cx->type() )                          {
            AudioAcceptor * accp = (AudioAcceptor *)cx       ;
            if (NotNull(accp) && NotNull(accp->audio))       {
              if (accp->audio->AudioSize()>0) done = false   ;
            }                                                ;
          }                                                  ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 1001 == jc->type() )                                {
      AudioJunction * aj = (AudioJunction *)jc               ;
      Conduits        cs = aj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        aj->takeConduit(ca)                                  ;
        delete ca                                            ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<CONDUITs.count();i++)                       {
    Conduit * cx = CONDUITs[i]                               ;
    for (int j=0;j<junc.count();j++)                         {
      if (junc[j]->conduits.contains(cx))                    {
        junc[j]->takeConduit(cx)                             ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if (DATAs.count()>0)                                       {
    for (int i=0;i<DATAs.count();i++)                        {
      UnmountAudioData ( DATAs [ i ] )                       ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  delete feeder                                              ;
  ////////////////////////////////////////////////////////////
  plan -> Rehabilitate ( )                                   ;
  tossAudios           ( )                                   ;
}

void N::SceneView::UnmountAudioData(AudioData * audio)
{
  nDropOut  ( IsNull(audio) )                      ;
  int t = audio->BlobType()                        ;
  if (t==0)                                        {
    delete audio                                   ;
    return                                         ;
  }                                                ;
  if (t==1)                                        {
    delete audio                                   ;
    return                                         ;
  }                                                ;
  if (t==11)                                       {
    AudioIn * ai = (AudioIn *)audio                ;
    ai -> stop        ( )                          ;
    ai -> deleteLater ( )                          ;
    return                                         ;
  }                                                ;
  if (t==12)                                       {
    AudioOut * ao = (AudioOut *)audio              ;
    ao -> stop        ( )                          ;
    ao -> deleteLater ( )                          ;
    return                                         ;
  }                                                ;
  if (t==13)                                       {
    AudioTributary * AT = (AudioTributary *)audio  ;
    QMap<int,BlobAcceptor *> Acceptors             ;
    CUIDs                    IDs                   ;
    int                      id                    ;
    Acceptors = AT->Acceptors                      ;
    AT -> deleteLater ( )                          ;
    IDs = Acceptors.keys()                         ;
    foreach (id,IDs)                               {
      BlobAcceptor * ba = Acceptors[id]            ;
      if ( NotNull(ba) && 20001 == ba->EatType() ) {
        WaveUniverse * wu                          ;
        wu = (WaveUniverse *)ba                    ;
        DeleteUniverse ( wu )                      ;
      }                                            ;
    }                                              ;
    return                                         ;
  }                                                ;
  if (plan->Talkative(EnableDebugger))             {
    PlanDebug                                      (
      45                                           ,
      QString ( "BlobType %1"                      )
      . arg ( audio -> BlobType ( ) )            ) ;
  }                                                ;
}

void N::SceneView::ExamAudio(SUID u)
{
  if ( !AudioPathes.contains(u) ) {
    AudioFailure ( u , "" )       ;
    return                        ;
  }                               ;
  QString filename                ;
  filename = AudioPathes[u]       ;
  QFileInfo FIE(filename)         ;
  if (!FIE.exists())              {
    AudioFailure ( u , filename ) ;
    return                        ;
  }                               ;
  int MID                         ;
  MID = RequestChannel(filename)  ;
  VarArgs args                    ;
  args << 30001                   ;
  args << u                       ;
  args << MID                     ;
  PostSequence ( args )           ;
}

void N::SceneView::LaunchAudio(VarArgs arguments)
{
  Junction      * junc = NULL                       ;
  AudioAcceptor * accp = NULL                       ;
  AudioAcceptor * wact = NULL                       ;
  AudioFeeder   * aio  = NULL                       ;
  AudioOut      * outp = NULL                       ;
  SUID            u    = arguments[1].toULongLong() ;
  int             MID  = arguments[2].toInt()       ;
  ///////////////////////////////////////////////////
  aio  = (AudioFeeder *)(plan->AudioChannels[MID])  ;
  if (IsNull(aio))                                  {
    AudioFailure ( u , "" )                         ;
    return                                          ;
  }                                                 ;
  if ( ! aio -> Peep ( ) )                          {
    AudioFailure ( u , aio->Filename )              ;
    return                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  junc = MountChannel  ( MID                      ) ;
  accp = SpeakerOutlet ( aio                      ) ;
  wact = WaveDisplay   ( junc , aio               ) ;
  ///////////////////////////////////////////////////
  if (AudioNames.contains(u))                       {
    junc->name = AudioNames[u]                      ;
    if (plan->Talkative(EnableDebugger))            {
      plan->Debug                                   (
        45                                          ,
        tr("Start audio %1").arg(junc->name)      ) ;
    }                                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  if (NotNull(accp))                                {
    junc -> addConduit ( accp )                     ;
    outp = (AudioOut *)accp->audio                  ;
  }                                                 ;
  if (NotNull(wact))                                {
    junc -> addConduit ( wact                     ) ;
  }                                                 ;
  plan -> Dredge       ( true                     ) ;
  ///////////////////////////////////////////////////
  EnableWaveThrottle ( wact )                       ;
  aio->start()                                      ;
  ///////////////////////////////////////////////////
  if (IsNull(outp)) return                          ;
  int ms                                            ;
  do                                                {
    Time::skip(50)                                  ;
    ms = outp->Milliseconds()                       ;
  } while ( ms < 250 )                              ;
  outp -> start   ( )                               ;
}

void N::SceneView::VideoFeederClosed(VarArgs args)
{
  if (args.count()!=4) return                         ;
  int           Id = args[0].toInt   ()               ;
  QString       AR = args[1].toString()               ;
  QString       FD = args[2].toString()               ;
  VideoFeeder * VF = FromVariant(VideoFeeder,args[3]) ;
  if ( Id != 50001         ) return                   ;
  if ( AR != "VideoFeeder" ) return                   ;
  if ( FD != "Finished"    ) return                   ;
  if ( IsNull(VF)          ) return                   ;
  VideoClosed ( VF )                                  ;
}

void N::SceneView::VideoClosed(VideoFeeder * feeder)
{
  VarArgs Args                   ;
  Args << "ShutVideo"            ;
  Args << VoidVariant ( feeder ) ;
  addSequence(Args)              ;
}

void N::SceneView::VideoFailure(SUID uuid,QString filename)
{ Q_UNUSED   ( uuid                ) ;
  Q_UNUSED   ( filename            ) ;
  PlanDebug  ( 30 , "VideoFailure" ) ;
  tossVideos (                     ) ;
}

void N::SceneView::UnmountSyncData(SyncAcceptor * sync)
{
  nDropOut  ( IsNull(sync) )                   ;
  int t = sync->SyncType()                     ;
  if ( t == 30001 )                            {
    VideoUniverse * VU = (VideoUniverse *)sync ;
    DeleteUniverse ( VU )                      ;
    return                                     ;
  }                                            ;
  if (plan->Talkative(EnableDebugger))         {
    PlanDebug                                  (
      45                                       ,
      QString ( "SyncType %1"                  )
      . arg ( sync -> SyncType ( ) )         ) ;
  }                                            ;
}

void N::SceneView::ShutVideo(VarArgs & args)
{
  if (args.count()!=2) return                                ;
  VideoFeeder * feeder = FromVariant(VideoFeeder,args[1])    ;
  if (IsNull(feeder)) return                                 ;
  feeder -> finalize ( 1000  )                               ;
  ////////////////////////////////////////////////////////////
  int   AC  = -1                                             ;
  CUIDs IDs = plan->AudioChannels.keys()                     ;
  for (int i=0;AC<0 && i<IDs.count();i++)                    {
    int id = IDs[i]                                          ;
    if ( plan->AudioChannels[id] == feeder ) AC = id         ;
  }                                                          ;
  if (AC<0) return                                           ;
  plan -> AudioChannels . take ( AC )                        ;
  ////////////////////////////////////////////////////////////
  QList<Conduit   *> CONDUITs                                ;
  for (int i=0;i<plan->junctions.count();i++)                {
    for (int j=0;j<plan->junctions[i]->conduits.count();j++) {
      Conduit * cx = plan->junctions[i]->conduits[j]         ;
      if ( 1001 == cx->type() )                              {
        AudioConduit * acx = (AudioConduit *)cx              ;
        if ( acx->channel == feeder )                        {
          CONDUITs << cx                                     ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  if (CONDUITs.count()<=0) return                            ;
  ////////////////////////////////////////////////////////////
  Junctions junc                                             ;
  for (int i=0;i<CONDUITs.count();i++)                       {
    junc << plan->Connectivity(CONDUITs[i])                  ;
  }                                                          ;
  if (junc.count()<=0) return                                ;
  ////////////////////////////////////////////////////////////
  QList<AudioData *> DATAs                                   ;
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 1001 == jc->type() )                                {
      AudioJunction * aj = (AudioJunction *)jc               ;
      Conduits        cs = aj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        if ( 1002 == ca->type() )                            {
          AudioAcceptor * aa = (AudioAcceptor *)ca           ;
          AudioData     * ad = aa->audio                     ;
          if (NotNull(ad)) DATAs << ad                       ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  bool done = false                                          ;
  while (!done)                                              {
    done = true                                              ;
    for (int i=0;i<junc.count();i++)                         {
      for (int j=0;j<junc[i]->conduits.count();j++)          {
        Conduit * cx = junc[i]->conduits[j]                  ;
        if (!CONDUITs.contains(cx))                          {
          if ( 1002 == cx->type() )                          {
            AudioAcceptor * accp = (AudioAcceptor *)cx       ;
            if (NotNull(accp) && NotNull(accp->audio))       {
              if (accp->audio->AudioSize()>0) done = false   ;
            }                                                ;
          }                                                  ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 1001 == jc->type() )                                {
      AudioJunction * aj = (AudioJunction *)jc               ;
      Conduits        cs = aj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        aj->takeConduit(ca)                                  ;
        delete ca                                            ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<CONDUITs.count();i++)                       {
    Conduit * cx = CONDUITs[i]                               ;
    for (int j=0;j<junc.count();j++)                         {
      if (junc[j]->conduits.contains(cx))                    {
        junc[j]->takeConduit(cx)                             ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if (DATAs.count()>0)                                       {
    for (int i=0;i<DATAs.count();i++)                        {
      UnmountAudioData ( DATAs [ i ] )                       ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  CONDUITs . clear ( )                                       ;
  junc     . clear ( )                                       ;
  DATAs    . clear ( )                                       ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<plan->junctions.count();i++)                {
    for (int j=0;j<plan->junctions[i]->conduits.count();j++) {
      Conduit * cx = plan->junctions[i]->conduits[j]         ;
      if ( 2001 == cx->type() )                              {
        VideoConduit * vcx = (VideoConduit *)cx              ;
        if ( vcx->channel == feeder )                        {
          CONDUITs << cx                                     ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  if (CONDUITs.count()<=0) return                            ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<CONDUITs.count();i++)                       {
    junc << plan->Connectivity(CONDUITs[i])                  ;
  }                                                          ;
  if (junc.count()<=0) return                                ;
  ////////////////////////////////////////////////////////////
  QList<SyncAcceptor *> SYNCs                                ;
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 2001 == jc->type() )                                {
      VideoJunction * vj = (VideoJunction *)jc               ;
      Conduits        cs = vj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        if ( 2002 == ca->type() )                            {
          VideoAcceptor * aa = (VideoAcceptor *)ca           ;
          SyncAcceptor  * ad = aa->acceptor                  ;
          if (NotNull(ad)) SYNCs << ad                       ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<junc.count();i++)                           {
    Junction * jc = junc[i]                                  ;
    if ( 2001 == jc->type() )                                {
      VideoJunction * vj = (VideoJunction *)jc               ;
      Conduits        cs = vj->conduits                      ;
      for (int j=0;j<cs.count();j++)                         {
        Conduit * ca = cs[j]                                 ;
        vj->takeConduit(ca)                                  ;
        delete ca                                            ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  for (int i=0;i<CONDUITs.count();i++)                       {
    Conduit * cx = CONDUITs[i]                               ;
    for (int j=0;j<junc.count();j++)                         {
      if (junc[j]->conduits.contains(cx))                    {
        junc[j]->takeConduit(cx)                             ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if (SYNCs.count()>0)                                       {
    for (int i=0;i<SYNCs.count();i++)                        {
      UnmountSyncData ( SYNCs[i] )                           ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  delete feeder                                              ;
  ////////////////////////////////////////////////////////////
  plan -> Rehabilitate ( )                                   ;
  tossVideos           ( )                                   ;
}

int N::SceneView::RequestVideo(QString filename)
{
  CUIDs         IDs = plan->AudioChannels.keys()      ;
  int           MID = 0                               ;
  VideoFeeder * vio                                   ;
  vio  = new VideoFeeder ( this     )                 ;
  vio -> setFilename     ( filename )                 ;
  vio -> PostSequences = &GuiSequences                ;
  vio -> setInterval   ( 10       )                   ;
  vio -> setBufferTime ( 5 * 1000 )                   ;
  vio -> setCompletion ( true     )                   ;
  if (IDs.count()>0)                                  {
    for (int i=0;i<IDs.count();i++)                   {
      if (IDs[i]>MID) MID = IDs[i]                    ;
    }                                                 ;
  }                                                   ;
  MID++                                               ;
  plan -> AudioChannels[MID] = (N::AudioChannel *)vio ;
  return MID                                          ;
}

N::AudioAcceptor * N::SceneView::SpeakerOutlet(VideoFeeder * feeder)
{
  if (!DeviceProperties.contains(PurePlan::DefaultAudioOutput)) return NULL ;
  QString         outd = DeviceProperties[PurePlan::DefaultAudioOutput]     ;
  AudioAcceptor * accp = NULL                                               ;
  AudioOut      * outp = new AudioOut ( this )                              ;
  outp   -> setDevice         (  outd            )                          ;
  feeder -> assign            ( *outp            )                          ;
  accp    = new AudioAcceptor ( this , outp      )                          ;
  outp   -> setMethod         ( AudioIO::Removal )                          ;
  outp   -> setAutoStop       ( false            )                          ;
  return accp                                                               ;
}

N::Junction * N::SceneView::MountVideo(int MID)
{
  nKickOut ( !plan->AudioChannels.contains(MID) , NULL )          ;
  VideoChannel  * vc   = (VideoChannel *)plan->AudioChannels[MID] ;
  VideoConduit  * acot                                            ;
  VideoJunction * junc                                            ;
  acot = new VideoConduit  ( this , vc )                          ;
  junc = new VideoJunction ( this      )                          ;
  plan->junctions << junc                                         ;
  junc->interval = 15                                             ;
  junc->addConduit ( acot )                                       ;
  return (Junction *)junc                                         ;
}

N::VideoUniverse * N::SceneView::AllocateVideo (void)
{
  VideoUniverse * vu              ;
  vu = new VideoUniverse ( this ) ;
  JoinDestructor ( vu )           ;
  return vu                       ;
}

N::VideoAcceptor * N::SceneView::VideoDisplay(Junction * junc,VideoChannel * feeder)
{ Q_UNUSED ( junc )                                    ;
  QRectF           vw ( 0 , 0 , width() , height() )   ;
  VideoAcceptor  * accp = NULL                         ;
  VideoUniverse  * vu   = AllocateVideo ( )            ;
  vu   -> setTransmission   ( feeder -> Transmission ) ;
  accp  = new VideoAcceptor ( this , vu              ) ;
  addUniverse ( RequestUniverse(11001) , vu )          ;
  vu   -> Relocation ( vw )                            ;
  return accp                                          ;
}

void N::SceneView::ExamVideo(SUID u)
{
  if ( !VideoPathes.contains(u) )     {
    VideoFailure ( u , "" )           ;
    return                            ;
  }                                   ;
  QString filename                    ;
  filename = VideoPathes[u]           ;
  QFileInfo FIE(filename)             ;
  if (!FIE.exists())                  {
    VideoFailure ( u , filename )     ;
    return                            ;
  }                                   ;
  /////////////////////////////////////
  int MID = RequestVideo ( filename ) ;
  VarArgs args                        ;
  args << 30002                       ;
  args << u                           ;
  args << MID                         ;
  PostSequence ( args )               ;
}

void N::SceneView::LaunchVideo(VarArgs arguments)
{
  Junction      * ajun = NULL                       ;
  Junction      * vjun = NULL                       ;
  VideoAcceptor * vacc = NULL                       ;
  AudioAcceptor * accp = NULL                       ;
  AudioAcceptor * wact = NULL                       ;
  VideoFeeder   * vio  = NULL                       ;
  AudioOut      * outp = NULL                       ;
  SUID            u    = arguments[1].toULongLong() ;
  int             MID  = arguments[2].toInt()       ;
  ///////////////////////////////////////////////////
  vio  = (VideoFeeder *)(plan->AudioChannels[MID])  ;
  if (IsNull(vio))                                  {
    VideoFailure ( u , "" )                         ;
    return                                          ;
  }                                                 ;
  if ( ! vio -> Peep ( ) )                          {
    VideoFailure ( u , vio->VideoPath() )           ;
    return                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  vjun = MountVideo    ( MID                      ) ;
  vacc = VideoDisplay  ( vjun , vio               ) ;
  ajun = MountChannel  ( MID                      ) ;
  accp = SpeakerOutlet ( vio                      ) ;
  wact = WaveDisplay   ( ajun , vio               ) ;
  ///////////////////////////////////////////////////
  if (VideoNames.contains(u))                       {
    vjun->name = VideoNames[u]                      ;
    ajun->name = VideoNames[u]                      ;
    if (plan->Talkative(EnableDebugger))            {
      plan->Debug                                   (
        45                                          ,
        tr("Start video %1").arg(vjun->name)      ) ;
    }                                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  if (NotNull(vacc))                                {
    vjun -> addConduit ( vacc )                     ;
  }                                                 ;
  if (NotNull(accp))                                {
    ajun -> addConduit ( accp )                     ;
    outp = (AudioOut *)accp->audio                  ;
  }                                                 ;
  if (NotNull(wact))                                {
    ajun -> addConduit ( wact                     ) ;
  }                                                 ;
  plan -> Dredge       ( true                     ) ;
  ///////////////////////////////////////////////////
  vio->play()                                       ;
  if (IsNull(outp)) return                          ;
  int ms                                            ;
  do                                                {
    Time::skip(50)                                  ;
    ms = outp->Milliseconds()                       ;
  } while ( ms < vio->Drift )                       ;
  outp -> start   ( )                               ;
}

void N::SceneView::QueueAlbum(SUID u,QString name)
{
  nDropOut ( u <= 0 )                  ;
  VarArgs Args                         ;
  Args << "DropAlbum"                  ;
  Args << 1                            ;
  Args << u                            ;
  addSequence ( Args )                 ;
  if (plan->Talkative(EnableDebugger)) {
    plan->Debug                        (
      45                               ,
      tr("Append album [%1]:[%2]"      )
      .arg(name).arg(u)              ) ;
  }                                    ;
}

void N::SceneView::QueueVideo(SUID u,QString name)
{
  nDropOut ( u <= 0 )                  ;
  VarArgs Args                         ;
  Args << "DropVideo"                  ;
  Args << 1                            ;
  Args << u                            ;
  addSequence ( Args )                 ;
  if (plan->Talkative(EnableDebugger)) {
    plan->Debug                        (
      45                               ,
      tr("Append video [%1]:[%2]"      )
      .arg(name).arg(u)              ) ;
  }                                    ;
}

void N::SceneView::QueueAudio(SUID u,QString name)
{
  nDropOut ( u <= 0 )                  ;
  VarArgs Args                         ;
  Args << "DropAudio"                  ;
  Args << 1                            ;
  Args << u                            ;
  addSequence ( Args )                 ;
  if (plan->Talkative(EnableDebugger)) {
    plan->Debug                        (
      45                               ,
      tr("Append audio [%1]:[%2]"      )
      .arg(name).arg(u)              ) ;
  }                                    ;
}

void N::SceneView::QueuePeople(SUID u,QString name,QPointF pos)

{
  nDropOut ( u <= 0 )                  ;
  VarArgs Args                         ;
  Args << "DropPeople"                 ;
  Args << pos                          ;
  Args << 1                            ;
  Args << u                            ;
  addSequence ( Args )                 ;
  if (plan->Talkative(EnableDebugger)) {
    plan->Debug                        (
      45                               ,
      tr("Append people [%1]:[%2]"     )
      .arg(name).arg(u)              ) ;
  }                                    ;
}

void N::SceneView::QueuePictures(SUID u,QString name,QPointF pos)
{
  nDropOut ( u <= 0 )                  ;
  VarArgs Args                         ;
  Args << "DropPicture"                ;
  Args << pos                          ;
  Args << 1                            ;
  Args << u                            ;
  addSequence ( Args )                 ;
  if (plan->Talkative(EnableDebugger)) {
    plan->Debug                        (
      45                               ,
      tr("Append people [%1]:[%2]"     )
      .arg(name).arg(u)              ) ;
  }                                    ;
}

void N::SceneView::dropVideo(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 2    ) ;
  int total = arguments[1].toInt()          ;
  nDropOut ( arguments.count()!=(total+2) ) ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+2].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
    if (assureVideos(Uuids))                {
      emit videoAccepted()                  ;
    }                                       ;
  }                                         ;
}

void N::SceneView::dropAlbum(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 2    ) ;
  int total = arguments[1].toInt()          ;
  nDropOut ( arguments.count()!=(total+2) ) ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+2].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
    if (assureAlbums(Uuids))                {
      emit videoAccepted()                  ;
    }                                       ;
  }                                         ;
}

void N::SceneView::dropAudio(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 2    ) ;
  int total = arguments[1].toInt()          ;
  nDropOut ( arguments.count()!=(total+2) ) ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+2].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
    if (assureAudios(Uuids))                {
      emit audioAccepted()                  ;
    }                                       ;
  }                                         ;
}

void N::SceneView::dropPeople(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 3    ) ;
  int total = arguments[2].toInt()          ;
  nDropOut ( arguments.count()!=(total+3) ) ;
  QPointF pos = arguments[1].toPointF()     ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+3].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
      dropPictureAt ( pos , Uuids )         ;
  }                                         ;
}

void N::SceneView::dropPeopleAt(QPointF pos,UUIDs & Uuids)
{ Q_UNUSED ( pos   ) ;
  Q_UNUSED ( Uuids ) ;
}

void N::SceneView::dropCamera(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 3    ) ;
  int total = arguments[2].toInt()          ;
  nDropOut ( arguments.count()!=(total+3) ) ;
  QPointF pos = arguments[1].toPointF()     ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+3].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
    dropCameraAt ( pos , Uuids )            ;
  }                                         ;
}

void N::SceneView::dropCameraAt(QPointF pos,UUIDs & Uuids)
{ Q_UNUSED ( pos   ) ;
  Q_UNUSED ( Uuids ) ;
}

void N::SceneView::dropPicture(VarArgs & arguments)
{
  nDropOut ( arguments . count ( ) < 3    ) ;
  int total = arguments[2].toInt()          ;
  nDropOut ( arguments.count()!=(total+3) ) ;
  QPointF pos = arguments[1].toPointF()     ;
  UUIDs Uuids                               ;
  for (int i=0;i<total;i++)                 {
    Uuids << arguments[i+3].toULongLong()   ;
  }                                         ;
  if (Uuids.count()>0)                      {
    dropPictureAt ( pos , Uuids )           ;
  }                                         ;
}

void N::SceneView::dropPictureAt(QPointF pos,UUIDs & Uuids)
{ Q_UNUSED ( pos   ) ;
  Q_UNUSED ( Uuids ) ;
}

void N::SceneView::playAudio(VarArgs & arguments)
{
  nDropOut ( arguments.count() !=2 )  ;
  SUID u = arguments[1].toULongLong() ;
  ExamAudio ( u )                     ;
}

void N::SceneView::playVideo(VarArgs & arguments)
{
  nDropOut ( arguments.count() != 2 )    ;
  SUID uuid = arguments[1].toULongLong() ;
  ExamVideo ( uuid )                     ;
}

bool N::SceneView::assureAlbums(UUIDs & Uuids)
{
  nKickOut ( Uuids.count() <=0 , false ) ;
  UUIDs Clips                            ;
  SUID  u                                ;
  SUID  z                                ;
  VideoManager VM ( plan )               ;
  EnterSQL ( SC , plan->sql            ) ;
    foreach (u,Uuids)                    {
      UUIDs C                            ;
      QString nn                         ;
      nn = SC . getName                  (
             PlanTable(Names)            ,
             "uuid"                      ,
             vLanguageId                 ,
             u                         ) ;
      if (nn.length()>0)                 {
        AlbumNames[u] = nn               ;
      }                                  ;
      C = VM.Clips(SC,u)                 ;
      if (C.count()>0) Clips << C        ;
      foreach (z,C)                      {
        VideoAlbumMaps[z] = u            ;
      }                                  ;
    }                                    ;
  LeaveSQL ( SC , plan->sql            ) ;
  nKickOut ( Clips.count() <=0 , false ) ;
  AlbumQueues << Uuids                   ;
  VideoQueues << Clips                   ;
  return assureVideos(Clips)             ;
}

bool N::SceneView::assureVideos(UUIDs & Uuids)
{
  nKickOut ( Uuids.count() <=0 , false ) ;
  VideoManager VM ( plan )               ;
  EnterSQL ( SC , plan->sql            ) ;
    SUID u                               ;
    foreach (u,Uuids)                    {
      QString filename                   ;
      filename = VM.Filename(SC,u)       ;
      if (filename.length()>0)           {
        QString fn                       ;
        fn = plan->VideoFile(filename)   ;
        if (fn.length()>0)               {
          QStringList Names              ;
          Names = VM.Names(SC,u)         ;
          if (Names.count()>0)           {
            VideoNames[u] = Names[0]     ;
          }                              ;
          VideoPathes[u] = fn            ;
          if (plan->Talkative(EnableDebugger)) {
            plan->Debug                  (
              45                         ,
              tr("Append video [%1]:[%2]")
              .arg(fn).arg(u)          ) ;
          }                              ;
        }                                ;
      }                                  ;
    }                                    ;
  LeaveSQL ( SC , plan -> sql          ) ;
  return true                            ;
}

void N::SceneView::tossVideos(void)
{
  nDropOut ( VideoQueues.count() <= 0 ) ;
  VarArgs PA                            ;
  PA << "PlayVideo"                     ;
  PA << VideoQueues[0]                  ;
  VideoQueues . takeAt ( 0 )            ;
  addSequence ( PA )                    ;
}

bool N::SceneView::assureAudios(UUIDs & Uuids)
{
  nKickOut ( Uuids.count() <=0 , false )               ;
  EnterSQL ( SC , plan->sql            )               ;
    QString Q                                          ;
    SUID    u                                          ;
    foreach (u,Uuids)                                  {
      QString filename = ""                            ;
      QString suffix   = ""                            ;
      Q = SC.sql.SelectFrom                            (
            "suffix"                                   ,
            PlanTable(AudioFiles)                      ,
            SC.WhereUuid(u)                          ) ;
      if (SC.Fetch(Q)) suffix = SC.String(0)           ;
      if (suffix.length()>0)                           {
        filename = QString("%1.%2").arg(u).arg(suffix) ;
        filename = plan -> AudioFile ( filename )      ;
        QFileInfo FX(filename)                         ;
        if (FX.exists())                               {
          QString nn                                   ;
          nn = SC . getName                            (
                 PlanTable(Names)                      ,
                 "uuid"                                ,
                 vLanguageId                           ,
                 u                                   ) ;
          if (nn.length()>0) AudioNames[u] = nn        ;
          AudioPathes[u] = filename                    ;
          if (plan->Verbose>40)                        {
            plan->Debug                                (
              45                                       ,
              tr("Append audio [%1]:[%2]"              )
              .arg(filename).arg(u)                  ) ;
          }                                            ;
        }                                              ;
      }                                                ;
    }                                                  ;
  LeaveSQL ( SC , plan->sql            )               ;
  return true                                          ;
}

void N::SceneView::tossAudios(void)
{
  nDropOut ( AudioQueues.count() <= 0 ) ;
  VarArgs PA                            ;
  PA << "PlayAudio"                     ;
  PA << AudioQueues[0]                  ;
  AudioQueues . takeAt ( 0 )            ;
  addSequence ( PA )                    ;
}

void N::SceneView::DispatchCommands(void)
{
  if (GuiSequences.count()<=0)        {
    StartCommando ( )                 ;
    return                            ;
  }                                   ;
  VarArgs args = GuiSequences [ 0 ]   ;
  GuiSequences . takeAt ( 0 )         ;
  switch (args[0].toInt())            {
    case 30001                        :
      emit audioStarting    ( args )  ;
    break                             ;
    case 30002                        :
      emit videoStarting    ( args )  ;
    break                             ;
    case 30003                        :
      emit recorderStarting ( args )  ;
    break                             ;
    case 40001                        :
      AudioFeederClosed     ( args )  ;
    break                             ;
    case 50001                        :
      VideoFeederClosed     ( args )  ;
    break                             ;
  }                                   ;
  StartCommando ( )                   ;
}

void N::SceneView::GuiDispatch(void)
{
  LaunchCommands ( ) ;
}

void N::SceneView::PostSequence(VarArgs args)
{
  GuiSequences << args ;
}

void N::SceneView::InputAudio(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::OutputAudio(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::NetInputAudio(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::NetOutputAudio(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::FeedAudio(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::FeedVideo(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::FeedInterface(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::AcceptInterface(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::FeedCommand(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::AcceptCommand(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::RunCommand(VarArgs & arguments)
{
  nDropOut ( arguments.count() <= 0 )   ;
  QString cmd = arguments[0].toString() ;
  #define RC(NAME,FUNC)                 \
           if (cmd==NAME) FUNC(arguments)
  RC ( "DropAudio"   , dropAudio   )    ;
  RC ( "DropVideo"   , dropVideo   )    ;
  RC ( "DropAlbum"   , dropAlbum   )    ;
  RC ( "DropPeople"  , dropPeople  )    ;
  RC ( "DropCamera"  , dropCamera  )    ;
  RC ( "DropPicture" , dropPicture )    ;
  RC ( "ShutAudio"   , ShutAudio   )    ;
  RC ( "ShutVideo"   , ShutVideo   )    ;
  RC ( "AddRecorder" , AddRecorder )    ;
  RC ( "PlayAudio"   , playAudio   )    ;
  RC ( "PlayVideo"   , playVideo   )    ;
  #undef  RC
}

void N::SceneView::ExecuteCommand(int Id)
{ Q_UNUSED ( Id )               ;
  while (Alive)                 {
    int ds = 15                 ;
    if (Intervals.contains(13)) {
      ds = Intervals[13]        ;
    }                           ;
    if (Sequences.count()>0)    {
      RunCommand(Sequences[0])  ;
      Sequences.takeAt(0)       ;
    }                           ;
    Time::msleep(ds)            ;
  }                             ;
}

void N::SceneView::SemanticAnalysis(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::Download(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::Upload(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::FeedObjects(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::AcceptObjects(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::Capturing(int Id)
{ Q_UNUSED ( Id ) ;
}

void N::SceneView::Custom(int Id)
{ Q_UNUSED ( Id ) ;
}
