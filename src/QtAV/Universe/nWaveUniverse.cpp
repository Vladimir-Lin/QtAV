#include <qtav.h>

N::WaveUniverse:: WaveUniverse   ( QObject * parent )
                : QObject        (           parent )
                , Universe       (                  )
                , BlobAcceptor   (                  )
                , BypassThrottle ( true             )
                , EachSample     ( 0                )
                , EachRate       ( 0                )
{
  name = "Wave"                            ;
  setFunction ( FollowWindowSize , true  ) ;
  setFunction ( WindowMovable    , false ) ;
  setFunction ( HasUniverseMenu  , true  ) ;
  setFunction ( ControlPanel     , false ) ;
  setFunction ( FixedPosition    , false ) ;
}

N::WaveUniverse::~WaveUniverse (void)
{
}

bool N::WaveUniverse::SampleRange(int sampleFmt,double & minValue,double & maxValue)
{
  switch (sampleFmt)           {
    case Acoustics::Int8       :
      minValue = -128.0        ;
      maxValue =  128.0        ;
    return true                ;
    case Acoustics::UInt8      :
      minValue =    0.0        ;
      maxValue =  255.0        ;
    return true                ;
    case Acoustics::Int16      :
      minValue = -32768.0      ;
      maxValue =  32768.0      ;
    return true                ;
    case Acoustics::Int24      :
    return false               ;
    case Acoustics::Int32      :
      minValue = -2147483648.0 ;
      maxValue =  2147483647.0 ;
    return true                ;
    case Acoustics::Float32    :
      minValue = -1.0          ;
      maxValue =  1.0          ;
    return true                ;
    case Acoustics::Double     :
      minValue = -1.0          ;
      maxValue =  1.0          ;
    return true                ;
    case Acoustics::Custom     :
    return false               ;
  }                            ;
  return true                  ;
}

int N::WaveUniverse::AudioSampleSize(int sampleFmt)
{
  switch (sampleFmt)        {
    case Acoustics::Int8    :
    return 1                ;
    case Acoustics::UInt8   :
    return 1                ;
    case Acoustics::Int16   :
    return 2                ;
    case Acoustics::Int24   :
    return 3                ;
    case Acoustics::Int32   :
    return 4                ;
    case Acoustics::Float32 :
    return 4                ;
    case Acoustics::Double  :
    return 8                ;
    case Acoustics::Custom  :
    return 0                ;
  }                         ;
  return 0                  ;
}

int N::WaveUniverse::SampleType(int sampleFmt)
{
  switch (sampleFmt)        {
    case Acoustics::Int8    :
    return Cpp::Char        ;
    case Acoustics::UInt8   :
    return Cpp::Byte        ;
    case Acoustics::Int16   :
    return Cpp::Short       ;
    case Acoustics::Int24   :
    return Cpp::Void        ;
    case Acoustics::Int32   :
    return Cpp::Integer     ;
    case Acoustics::Float32 :
    return Cpp::Float       ;
    case Acoustics::Double  :
    return Cpp::Double      ;
  }                         ;
  return Cpp::Void          ;
}

void N::WaveUniverse::setWaveCamera (
       int    id                  ,
       int    mode                ,
       int    CameraOp            ,
       QPoint p                   ,
       QSize  s                   ,
       int    Samples             ,
       double minValue            ,
       double maxValue            )
{
  Camera * c                   = new Camera()                            ;
  double   depth               = 1000000                                 ;
  c -> mode                    = mode                                    ;
  c -> FOV                     = 60                                      ;
  c -> eye                     = ControlPoint ( 100.0 , 100.0 , 260.0  ) ;
  c -> center                  = ControlPoint (   0.0 ,   0.0 ,   0.0  ) ;
  c -> up                      = ControlPoint (   0.0 ,   1.0 ,   0.0  ) ;
  c -> setNearFar                             ( -depth ,  depth )        ;
  c -> references [ 0        ] = ControlPoint ( p.x    () , p.y     () ) ;
  c -> references [ 1        ] = ControlPoint ( s.width() , s.height() ) ;
  c -> operating  [ CameraOp ] = true                                    ;
  c -> minimum.x               = 0                                       ;
  c -> maximum.x               = Samples                                 ;
  c -> minimum.y               = minValue                                ; // -32768
  c -> maximum.y               = maxValue                                ; //  32768
  addCamera        ( id , c )                                            ;
  setCurrentCamera ( id     )                                            ;
}

N::SceneObject * N::WaveUniverse::addPlane (
                    int    aid             ,
                    int    SampleRate      ,
                    double minValue        ,
                    double maxValue        ,
                    double z               ,
                    QColor border          ,
                    QColor inside          )
{
  SceneObject * nso = new SceneObject ( )           ;
  Plane         plane                               ;
  plane . O = ControlPoint (0,minValue,z)           ;
  plane . X = ControlPoint (SampleRate,0,0)         ;
  plane . Y = ControlPoint (0,maxValue-minValue,0)  ;
  plane . T = ControlPoint (1,1,1)                  ;
  plane . N = ControlPoint (0,0,1)                  ;
  plane . W = 1                                     ;
  plane . H = 1                                     ;
  plane . A = true                                  ;
  nso  -> addPlane(plane)                           ;
  ///////////////////////////////////////////////////
  CUIDs IDs                                         ;
  int   cids = 1                                    ;
  int   id                                          ;
  ControlPoint psc                                  ;
  ControlPoint psf                                  ;
  ///////////////////////////////////////////////////
  psc  = border                                     ;
  psf  = inside                                     ;
  nso -> segments        [ 1 ] . pointColor = true  ;
  nso -> faces           [ 1 ] . pointColor = true  ;
  ///////////////////////////////////////////////////
  IDs  = nso -> segments [ 1 ] . index              ;
  foreach (id,IDs)                                  {
    nso -> segments [ 1    ] . colors [ id ] = cids ;
    nso -> colors   [ cids ]                 = psc  ;
    cids++                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  IDs = nso->faces[1].index                         ;
  foreach (id,IDs)                                  {
    nso -> faces    [ 1    ] . colors [ id ] = cids ;
    nso -> colors   [ cids ]                 = psf  ;
    cids++                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  addActor       ( aid , nso  )                     ;
  Perform        ( aid , true )                     ;
  ///////////////////////////////////////////////////
  return nso                                        ;
}

N::SceneObject * N::WaveUniverse::addWave(int SampleRate,int SampleFmt,double z,QColor color)
{
  int Fmt     = SampleType(SampleFmt)                          ;
  int FmtSize = Cpp::SizeOf((Cpp::ValueTypes)Fmt)              ;
  QByteArray ZC ( sizeof(int) * SampleRate , 0 )               ;
  QByteArray AC ( FmtSize     * SampleRate , 0 )               ;
  SceneLinear * nsl = new SceneLinear()                        ;
  nsl->colors[0] = color                                       ;
  nsl->setSize(SampleRate)                                     ;
  nsl->staticValues[SceneLinear::Z] = QVariant(z)              ;
  nsl->setType (SceneLinear::X,Cpp::Integer)                   ;
  nsl->setType (SceneLinear::Y,Fmt)                            ;
  nsl->allocate(SceneLinear::X)                                ;
  nsl->allocate(SceneLinear::Y)                                ;
  nsl->byteArrays[SceneLinear::X] = ZC                         ;
  nsl->byteArrays[SceneLinear::Y] = AC                         ;
  nsl->byteArrays[SceneLinear::Y] . fill(0)                    ;
  int    * idx = (int *)nsl->byteArrays[SceneLinear::X].data() ;
  for (int i=0;i<SampleRate;i++) idx[i] = i                    ;
  nsl->assign(SceneLinear::X)                                  ;
  nsl->assign(SceneLinear::Y)                                  ;
  nsl->addSequence(101,SceneObject::RenderSegments)            ;
  addActor       ( 1001 , nsl  )                               ;
  Perform        ( 1001 , true )                               ;
  return nsl                                                   ;
}

void N::WaveUniverse::setConfigure (
       int    SampleRate         ,
       int    SampleFmt          ,
       QPoint pos                ,
       QSize  camera             ,
       double plane              ,
       double line               ,
       double wave               ,
       QColor planeColor         ,
       QColor lineColor          ,
       QColor waveColor          )
{
  double minValue                                       ;
  double maxValue                                       ;
  ///////////////////////////////////////////////////////
  EachSample   = AudioSampleSize ( SampleFmt )          ;
  EachRate     = SampleRate                             ;
  ThrottleTime = nTimeNow                               ;
  if (!SampleRange(SampleFmt,minValue,maxValue)) return ;
  setWaveCamera                                         (
    0                                                   ,
    Camera::Parking                                     ,
    Camera::AlignTopLeft                                ,
    pos                                                 ,
    camera                                              ,
    SampleRate                                          ,
    minValue                                            ,
    maxValue                                          ) ;
  ///////////////////////////////////////////////////////
  SceneObject * nso = addPlane                          (
                         2001                           ,
                         SampleRate                     ,
                         minValue                       ,
                         maxValue                       ,
                         plane                          ,
                         QColor(240,240,240,255)        ,
                         planeColor                   ) ;
  SceneObject * nsi = addPlane                          (
                         102                            ,
                         SampleRate                     ,
                         minValue                       ,
                         maxValue                       ,
                         line                           ,
                         lineColor                      ,
                         QColor(64,64,96,32)          ) ;
  nso->addSequence(1,SceneObject::RenderFaces   )       ;
  nsi->addSequence(2,SceneObject::RenderSegments)       ;
  ///////////////////////////////////////////////////////
  addWave(SampleRate,SampleFmt,wave,waveColor)          ;
}

void N::WaveUniverse::Relocation(QRectF viewport)
{
  Viewport = viewport                                  ;
  nDropOut ( IsNull(CurrentCamera) )                   ;
  if ( isFunction ( FollowWindowSize ) )               {
    CurrentCamera->references[1].x = viewport.width () ;
  }                                                    ;
  //////////////////////////////////////////////////////
  QRectF W = viewport                                  ;
  QRectF V ( 0 , 0 , W . width ( ) , W . height ( ) )  ;
  CurrentCamera -> assignViewport ( V )                ;
}

bool N::WaveUniverse::Animation(void)
{
  if ( BypassThrottle ) return true        ;
  if (!Actors.contains(1001)) return false ;
  QDateTime NTN = nTimeNow                 ;
  int ds = ThrottleTime.msecsTo(NTN)       ;
  ds *= EachRate                           ;
  ds /= 1000                               ;
  ds *= EachSample                         ;
  if (ds>0)                                {
    SceneLinear * nsl                      ;
    nsl = (SceneLinear *)Actors[1001]      ;
    QByteArray KB                          ;
    ThrottleMutex . lock   (      )        ;
    int ks = Throttle.size()               ;
    if (ds>ks) ds = ks                     ;
    if (ds>0)                              {
      KB = Throttle . left   ( ds     )    ;
      Throttle      . remove ( 0 , ds )    ;
    }                                      ;
    ThrottleMutex . unlock (        )      ;
    if (KB.size()>0)                       {
      nsl->tail(SceneLinear::Y,KB)         ;
    }                                      ;
  }                                        ;
  ThrottleTime = NTN                       ;
  return true                              ;
}

bool N::WaveUniverse::mouseDoubleClickEvent(QWidget * widget,QMouseEvent * event)
{
  return Universe::mouseDoubleClickEvent(widget,event) ;
}

bool N::WaveUniverse::mousePressEvent(QWidget * widget,QMouseEvent * event)
{
  return Universe::mousePressEvent(widget,event) ;
}

bool N::WaveUniverse::mouseMoveEvent(QWidget * widget,QMouseEvent * event)
{
  return Universe::mouseMoveEvent(widget,event)   ;
}

bool N::WaveUniverse::mouseReleaseEvent(QWidget * widget,QMouseEvent * event)
{
  return Universe::mouseReleaseEvent(widget,event) ;
}

void N::WaveUniverse::ChangeBorder(QWidget * widget)
{
  SceneObject * n = (SceneObject *)Actors[ 102]           ;
  CUIDs         IDs = n->colors.keys()                    ;
  if (IDs.count()<=0) return                              ;
  QColor c = n->colors[IDs[0]].Color()                    ;
  c = QColorDialog::getColor(c,widget,tr("Border color")) ;
  int id                                                  ;
  ControlPoint pc                                         ;
  pc = c                                                  ;
  foreach (id,IDs) n->colors[id] = pc                     ;
}

void N::WaveUniverse::ChangeBackground(QWidget * widget)
{
  SceneObject * n   = (SceneObject *)Actors[2001]             ;
  CUIDs         IDs = n->colors.keys()                        ;
  if (IDs.count()<=0) return                                  ;
  QColor c = n->colors[IDs[0]].Color()                        ;
  c = QColorDialog::getColor(c,widget,tr("Background color")) ;
  int id                                                      ;
  ControlPoint pc                                             ;
  pc = c                                                      ;
  foreach (id,IDs) n->colors[id] = pc                         ;
}

void N::WaveUniverse::ChangeWaveColor(QWidget * widget)
{
  SceneLinear  * nsl = (SceneLinear *)Actors[1001]      ;
  QColor c                                              ;
  c = nsl->colors[0].Color()                            ;
  c = QColorDialog::getColor(c,widget,tr("Wave color")) ;
  nsl->colors[0] = c                                    ;
}

bool N::WaveUniverse::Menu(QWidget * widget,QPointF pos)
{
  Camera * C = CurrentCamera                                                 ;
  nKickOut ( IsNull(C) , false )                                             ;
  QPoint  wos (pos.x(),pos.y())                                              ;
  QRectF  area                                                               ;
  QPointF portPos                                                            ;
  if ( ! C -> ParkingArea ( area                 ) ) return false            ;
  if ( ! C -> atPort      ( area , wos , portPos ) ) return false            ;
  ////////////////////////////////////////////////////////////////////////////
  nScopedMenu ( mm , widget )                                                ;
  QAction * aa                                                               ;
  mm   . add (101,tr("Wave color"      )                                   ) ;
  mm   . add (102,tr("Background color")                                   ) ;
  mm   . add (103,tr("Border color"    )                                   ) ;
  mm   . add (104,tr("Full window"     ),true,isFunction(FollowWindowSize) ) ;
  mm   . add (105,tr("Window movable"  ),true,isFunction(WindowMovable   ) ) ;
  mm   . add (106,tr("Panel"           ),true,isFunction(ControlPanel    ) ) ;
  if ( isFunction ( HasUniverseMenu )  )                                     {
    mm . add (107,tr("Disable menu"    )                                   ) ;
  }                                                                          ;
  if (NotNull(qPlan)) mm . setFont ( AppPlan )                               ;
  aa = mm . exec ( )                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if (IsNull(aa)) return true                                                ;
  switch (mm[aa])                                                            {
    case 101                                                                 :
      ChangeWaveColor  (widget)                                              ;
    break                                                                    ;
    case 102                                                                 :
      ChangeBackground (widget)                                              ;
    break                                                                    ;
    case 103                                                                 :
      ChangeBorder     (widget)                                              ;
    break                                                                    ;
    case 104                                                                 :
      setFunction ( FollowWindowSize , aa->isChecked() )                     ;
      if ( isFunction ( FollowWindowSize ) )                                 {
        setFunction ( WindowMovable , false )                                ;
      }                                                                      ;
      emit Movable ( this , properties[WindowMovable] )                      ;
    break                                                                    ;
    case 105                                                                 :
      setFunction ( WindowMovable , aa->isChecked() )                        ;
      if ( isFunction ( WindowMovable ) )                                    {
        setFunction ( FollowWindowSize , false )                             ;
      }                                                                      ;
      emit Movable ( this , isFunction ( WindowMovable ) )                   ;
    break                                                                    ;
    case 106                                                                 :
      setFunction ( ControlPanel , aa->isChecked() )                         ;
      emit RequestPanel ( this , isFunction ( ControlPanel ) )               ;
    break                                                                    ;
    case 107                                                                 :
      emit DisableMenu ( )                                                   ;
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}

bool N::WaveUniverse::Demolish(void)
{
  return true ;
}

bool N::WaveUniverse::Feeding(QByteArray & data)
{
  if (!Actors.contains(1001)) return false           ;
  if (BypassThrottle)                                {
    SceneLinear  * nsl = (SceneLinear *)Actors[1001] ;
    nsl->tail(SceneLinear::Y,data)                   ;
  } else                                             {
    ThrottleMutex . lock   (      )                  ;
    Throttle      . append ( data )                  ;
    ThrottleMutex . unlock (      )                  ;
  }                                                  ;
  return true                                        ;
}
