#include <qtav.h>

typedef struct               {
  N::VideoEditor * editor    ;
  N::Universe    * universe  ;
  N::Universe    * attach    ;
  N::SceneObject * scene     ;
  N::SceneObject * mounting  ;
} PrivateVideoFunctionBinder ;

typedef struct                 {
  N::EditableUniverse * editor ;
  N::SceneObject      * scene  ;
} PrivateEditableBinder        ;

N::Universe * N::EditableCreator(void * p,QObject * parent)
{
  return new N::EditableUniverse ( parent , (Plan *) p ) ;
}

N::EditableUniverse:: EditableUniverse ( QObject * parent , Plan * p )
                    : UniverseWidget   (           parent ,        p )
                    , Enabler          (                             )
                    , unified          ( NULL                        )
                    , glyph            ( NULL                        )
{
  Configure ( ) ;
}

N::EditableUniverse:: EditableUniverse ( QObject         * parent ,
                                         Plan            * p      ,
                                         SceneDestructor * des    )
                    : UniverseWidget   ( parent , p , des         )
                    , Enabler          (                          )
                    , unified          ( NULL                     )
                    , plan             (                   p      )
                    , glyph            ( NULL                     )
{
  Configure      ( ) ;
}

N::EditableUniverse::~EditableUniverse (void)
{
}

void N::EditableUniverse::Configure (void)
{
  name = "Editable"                                               ;
  addType ( 53001 )                                               ;
  /////////////////////////////////////////////////////////////////
  QImage * album = new QImage ( ":/images/videos.png" )           ;
  Textures [ Types::Album   ]  = new SceneTexture ( destructor )  ;
  Textures [ Types::Album   ] -> create ( album , true , true )   ;
  Textures [ Types::Album   ] -> node = Types::Album              ;
  Textures [ Types::Album   ] -> setVariable ( "Type","Picture" ) ;
  Names    [ Types::Album   ]         = tr("All albums")          ;
  /////////////////////////////////////////////////////////////////
  QImage * crowds = new QImage ( ":/images/peoplegroups.png" )    ;
  Textures [ Types::People  ]  = new SceneTexture ( destructor )  ;
  Textures [ Types::People  ] -> create ( crowds , true , true )  ;
  Textures [ Types::People  ] -> node = Types::People             ;
  Textures [ Types::People  ] -> setVariable ( "Type","Picture" ) ;
  Names    [ Types::People  ]         = tr("All people")          ;
  /////////////////////////////////////////////////////////////////
  QImage * gallery = new QImage ( ":/images/gallery.png" )        ;
  Textures [ Types::Gallery ]  = new SceneTexture ( destructor )  ;
  Textures [ Types::Gallery ] -> create ( gallery , true , true ) ;
  Textures [ Types::Gallery ] -> node = Types::Gallery            ;
  Textures [ Types::Gallery ] -> setVariable ( "Type","Picture" ) ;
  Names    [ Types::Gallery ]         = tr("All pictures")        ;
  /////////////////////////////////////////////////////////////////
  QImage * picture = new QImage ( ":/images/pictures.png" )       ;
  Textures [ Types::Picture ]  = new SceneTexture ( destructor  ) ;
  Textures [ Types::Picture ] -> create ( picture , true , true ) ;
  Textures [ Types::Picture ] -> node = Types::Gallery            ;
  Textures [ Types::Picture ] -> setVariable ( "Type","Picture" ) ;
  Names    [ Types::Picture ]         = tr("Picture")             ;
  /////////////////////////////////////////////////////////////////
  setFunction ( FollowWindowSize , true  )                        ;
  setFunction ( WindowMovable    , false )                        ;
  setFunction ( HasUniverseMenu  , true  )                        ;
  setActivate ( MouseDClick      , true  )                        ;
  /////////////////////////////////////////////////////////////////
  setVariable ( "AlbumsIndex"    ,  0    )                        ;
  setVariable ( "AlbumsPage"     , 48    )                        ;
  setVariable ( "Animation"      , true  )                        ;
  setVariable ( "Hover"          , true  )                        ;
}

int N::EditableUniverse::CallBinding(SceneObject * s,QVariants & args)
{
  if ( ! userobjs . contains ( "BindFunctions" ) ) return 0 ;
  PrivateVideoFunctionBinder * pvfb                         ;
  pvfb = new PrivateVideoFunctionBinder ( )                 ;
  ::memcpy ( pvfb                                           ,
             userobjs [ "BindFunctions" ]                   ,
             sizeof(PrivateVideoFunctionBinder)           ) ;
  pvfb -> mounting = s                                      ;
  int r = CallDynamic ( "BindFunctions"                     ,
                        this                                ,
                        pvfb                                ,
                        args                              ) ;
  delete pvfb                                               ;
  return r                                                  ;
}

int N::EditableUniverse::RequestingFor(void * data,void * dispatch,QVariants & args)
{
  if ( IsNull ( dispatch ) ) return 0                              ;
  PrivateEditableBinder * peb = (PrivateEditableBinder *) dispatch ;
  return peb -> editor -> RequestsFor ( args )                     ;
}

int N::EditableUniverse::RequestsFor(QVariants & args)
{
  if ( args . count ( ) <= 0 ) return 0 ;
  int     cmd  = args [ 0 ] . toInt ( ) ;
  VarArgs xrgs = args                   ;
  xrgs . takeAt ( 0          )          ;
  start         ( cmd , xrgs )          ;
  return 1                              ;
}

void N::EditableUniverse::BindRequest(SceneObject * s)
{
  PrivateEditableBinder * peb = new PrivateEditableBinder ( )                    ;
  peb -> editor = this                                                           ;
  peb -> scene  = s                                                              ;
  s   -> DynamicFunctions [ "RequestsFor" ] = N::EditableUniverse::RequestingFor ;
  s   -> userobjs         [ "RequestsFor" ] = peb                                ;
}

N::Camera * N::EditableUniverse::FlatCamera(void)
{
  return CreateCamera ( Camera::Parking ) ;
}

SUID N::EditableUniverse::Lastest(void)
{
  UUIDs U = Actors . keys ( )                   ;
  SUID  u = 1                                   ;
  if ( U . count ( ) > 0 ) u = U . last ( ) + 1 ;
  return u                                      ;
}

N::Camera * N::EditableUniverse::CreateCamera(int style)
{
  Camera * C = NULL                                                          ;
  switch ( style )                                                           {
    case N::Camera::Reality                                                  :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Reality                                          ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 0.0 , 0.0 , 400.0 )                       ;
      C -> center = ControlPoint ( 0.0 , 0.0 ,   0.0 )                       ;
      C -> up     = ControlPoint ( 0.0 , 1.0 ,   0.0 )                       ;
      C -> setNearFar            ( 1.0 , 10000000000 )                       ;
      C -> minimum.x = -400                                                  ;
      C -> minimum.y = -400                                                  ;
      C -> maximum.x =  400                                                  ;
      C -> maximum.y =  400                                                  ;
      C -> setVariable ( "Name"       , tr("Reality") )                      ;
      C -> setVariable ( "Identifier" ,    "Reality"  )                      ;
    break                                                                    ;
    case N::Camera::Perfect                                                  :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Perfect                                          ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 0.0 , 0.0 , 400.0 )                       ;
      C -> center = ControlPoint ( 0.0 , 0.0 ,   0.0 )                       ;
      C -> up     = ControlPoint ( 0.0 , 1.0 ,   0.0 )                       ;
      C -> setNearFar            ( 1.0 , 10000000000 )                       ;
      C -> minimum.x = -400                                                  ;
      C -> minimum.y = -400                                                  ;
      C -> maximum.x =  400                                                  ;
      C -> maximum.y =  400                                                  ;
      C -> setVariable ( "Name"       , tr("Perfect") )                      ;
      C -> setVariable ( "Identifier" ,    "Perfect"  )                      ;
    break                                                                    ;
    case N::Camera::Perspective                                              :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Perspective                                      ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 0.0 , 0.0 , 400.0 )                       ;
      C -> center = ControlPoint ( 0.0 , 0.0 ,   0.0 )                       ;
      C -> up     = ControlPoint ( 0.0 , 1.0 ,   0.0 )                       ;
      C -> setNearFar            ( 1.0 , 10000000000 )                       ;
      C -> minimum.x = -400                                                  ;
      C -> minimum.y = -400                                                  ;
      C -> maximum.x =  400                                                  ;
      C -> maximum.y =  400                                                  ;
      C -> setVariable ( "Name"       , tr("Perspective") )                  ;
      C -> setVariable ( "Identifier" ,    "Perspective"  )                  ;
    break                                                                    ;
    case N::Camera::Orthographic                                             :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Orthographic                                     ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Orthographic") )                 ;
      C -> setVariable ( "Identifier" ,    "Orthographic"  )                 ;
    break                                                                    ;
    case N::Camera::Frustum                                                  :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Frustum                                          ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Frustum") )                      ;
      C -> setVariable ( "Identifier" ,    "Frustum"  )                      ;
    break                                                                    ;
    case N::Camera::Fit                                                      :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Fit                                              ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Fit") )                          ;
      C -> setVariable ( "Identifier" ,    "Fit"  )                          ;
    break                                                                    ;
    case N::Camera::Parking                                                  :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Parking                                          ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Parking") )                      ;
      C -> setVariable ( "Identifier" ,    "Parking"  )                      ;
    break                                                                    ;
    case N::Camera::Floating                                                 :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::Floating                                         ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Floating") )                     ;
      C -> setVariable ( "Identifier" ,    "Floating"  )                     ;
    break                                                                    ;
    case N::Camera::NodeMatrix                                               :
      C           = new Camera( destructor )                                 ;
      C -> mode   = Camera::NodeMatrix                                       ;
      C -> FOV    = 45                                                       ;
      C -> eye    = ControlPoint ( 400.0,400.0,800.0 )                       ;
      C -> center = ControlPoint (   0.0,  0.0,  0.0 )                       ;
      C -> up     = ControlPoint (   0.0,  1.0,  0.0 )                       ;
      C -> setNearFar            ( -100000 , 100000  )                       ;
      C -> references[Camera::ReferenceCorner] = ControlPoint( 0, 0,0)       ;
      C -> references[Camera::ReferenceSize  ] = ControlPoint(10,10,0)       ;
      C -> minimum.x =   0                                                   ;
      C -> minimum.y =   0                                                   ;
      C -> maximum.x = 200                                                   ;
      C -> maximum.y = 200                                                   ;
      C -> operating [Camera::AlignTopLeft] = true                           ;
      C -> setVariable ( "Name"       , tr("Matrix") )                       ;
      C -> setVariable ( "Identifier" ,    "Matrix"  )                       ;
    break                                                                    ;
  }                                                                          ;
  return C                                                                   ;
}

void N::EditableUniverse::BuildCameras(int startId)
{
  Camera * camera                                                ;
  for (int i = Camera::Reality ; i <= Camera::NodeMatrix ; i++ ) {
    camera  = CreateCamera ( i                    )              ;
    addCamera              ( startId + i , camera )              ;
    camera -> node = startId + i                                 ;
    JoinDestructor         ( camera               )              ;
  }                                                              ;
}

void N::EditableUniverse::SelectCamera(QString identifier,QRect g)
{
  QString ix = identifier . toLower ( )                                 ;
  UUIDs   U  = Cameras . keys ( )                                       ;
  SUID    u                                                             ;
  ///////////////////////////////////////////////////////////////////////
  QRectF  vw ( 0 , 0 , g.width() , g.height() )                         ;
  QRect   WR ( - vw . width ( ) / 2 , - vw . height ( ) / 2             ,
                 vw . width ( )     ,   vw . height ( )               ) ;
  QRectF  VR = WR                                                       ;
  ///////////////////////////////////////////////////////////////////////
  foreach ( u , U )                                                     {
    QString id = Cameras [ u ] -> Value ( "Identifier" ) . toString ( ) ;
    id = id . toLower ( )                                               ;
    if ( ix == id )                                                     {
      Camera * C = Cameras [ u ]                                        ;
      switch ( C -> mode )                                              {
        case N::Camera::Reality                                         :
          setCurrentCamera ( u                   )                      ;
          setArea          ( WR    , VR          )                      ;
        break                                                           ;
        case N::Camera::Perfect                                         :
          setCurrentCamera ( u                   )                      ;
          setArea          ( WR    , VR          )                      ;
        break                                                           ;
        case N::Camera::Perspective                                     :
          C -> eye    = ControlPoint ( 800.0 , 200.0 , 1400.0 )         ;
          C -> center = ControlPoint (   0.0 ,   0.0 ,    0.0 )         ;
          C -> up     = ControlPoint (   0.0 ,   1.0 ,    0.0 )         ;
          setCurrentCamera ( u                   )                      ;
        break                                                           ;
        case N::Camera::Orthographic                                    :
          setCurrentCamera ( u                   )                      ;
          setArea          ( WR    , VR          )                      ;
        break                                                           ;
        case N::Camera::Frustum                                         :
          setArea          ( WR    , VR          )                      ;
          setCurrentCamera ( u                   )                      ;
        break                                                           ;
        case N::Camera::Fit                                             :
          setArea          ( WR    , VR          )                      ;
          setCurrentCamera ( u                   )                      ;
        break                                                           ;
        case N::Camera::Parking                                         :
          setCurrentCamera ( u                   )                      ;
          setArea          ( WR    , VR          )                      ;
          setAlign         ( Camera::AlignCenter )                      ;
        break                                                           ;
        case N::Camera::Floating                                        :
          setArea          ( WR    , VR          )                      ;
          setCurrentCamera ( u                   )                      ;
        break                                                           ;
        case N::Camera::NodeMatrix                                      :
          setArea          ( WR    , VR          )                      ;
          setCurrentCamera ( u                   )                      ;
        break                                                           ;
      }                                                                 ;
      return                                                            ;
    }                                                                   ;
  }                                                                     ;
}

void N::EditableUniverse::CameraSelected(VarArgs & args)
{
  if ( args . count ( ) < 3 ) return                                  ;
  QRectF  vw ( 0 , 0 , Viewport . width ( ) , Viewport . height ( ) ) ;
  SUID    g  = args [ 0 ] . toULongLong ( )                           ;
  SUID    p  = args [ 1 ] . toULongLong ( )                           ;
  QString id = Cameras [ g ] -> Value ( "Identifier" ) . toString ( ) ;
  if ( args [ 2 ] . toBool ( ) ) CloseChild ( p )                     ;
  SelectCamera ( id , vw . toRect ( ) )                               ;
  Relocation   ( vw                   )                               ;
}

void N::EditableUniverse::ChooseCamera(void)
{
  UUIDs U = Cameras . keys ( )                                               ;
  NAMEs N                                                                    ;
  SUID  u                                                                    ;
  SUID  i = 0                                                                ;
  ////////////////////////////////////////////////////////////////////////////
  foreach ( u , U )                                                          {
    N [ u ] = Cameras [ u ] -> Value ( "Name" ) . toString ( )               ;
    if ( CurrentCamera == Cameras [ u ] ) i = u                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CameraShelf * camera = new CameraShelf ( NULL , plan , destructor )        ;
  ////////////////////////////////////////////////////////////////////////////
  camera -> setUniverse                  ( node                     )        ;
  camera -> gui              = gui                                           ;
  camera -> DynamicFunctions = DynamicFunctions                              ;
  BindRequest                            ( camera                   )        ;
  ////////////////////////////////////////////////////////////////////////////
  u              = Lastest               (                          )        ;
  camera -> node = u                                                         ;
  addActor                               ( u , camera               )        ;
  Perform                                ( u , true                 )        ;
  camera -> prepare                      (                          )        ;
  camera -> setCameras                   ( i , N                    )        ;
  camera -> startup                      (                          )        ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( camera , SIGNAL ( CloseObject(SUID))                            ,
             this   , SLOT   ( CloseChild (SUID))                            ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::setAlign(int align)
{
  nDropOut ( IsNull(CurrentCamera) )                              ;
  for (int i=Camera::AlignCenter;i<=Camera::AlignRightBottom;i++) {
    CurrentCamera->operating[i] = false                           ;
  }                                                               ;
  CurrentCamera->operating[align] = true                          ;
}

void N::EditableUniverse::setArea(QRect Port,QRectF Region)
{
  Camera * C = CurrentCamera                     ;
  nDropOut ( IsNull(C) )                         ;
  QPointF       RC = Region.center()             ;
  ControlPoint TL(Port.x    (),Port.y     (),0)  ;
  ControlPoint WH(Port.width(),Port.height(),0)  ;
  ControlPoint EP(RC.x(),RC.y(),1000.0)          ;
  ControlPoint EC(RC.x(),RC.y(),   0.0)          ;
  C-> references[Camera::ReferenceCorner] = TL   ;
  C-> references[Camera::ReferenceSize  ] = WH   ;
  C-> eye       = EP                             ;
  C-> center    = EC                             ;
  C-> minimum.x = Region . left   ()             ;
  C-> minimum.y = Region . top    ()             ;
  C-> maximum.x = Region . right  ()             ;
  C-> maximum.y = Region . bottom ()             ;
}

void N::EditableUniverse::clear(void)
{
  CurrentCamera = NULL      ;
  Illuminates   . clear ( ) ;
  Performs      . clear ( ) ;
  oglenables    . clear ( ) ;
  Cameras       . clear ( ) ;
  Actors        . clear ( ) ;
  Lights        . clear ( ) ;
  widgets       . clear ( ) ;
  VCFs          . clear ( ) ;
  nMaps         . clear ( ) ;
}

void N::EditableUniverse::Flush(void)
{
  if ( Performs . count ( ) <= 0 ) return ;
  Universe::Flush ( )                     ;
}

bool N::EditableUniverse::Animation(void)
{
  Universe::Animation ( )                       ;
  return Variables [ "Animation" ] . toBool ( ) ;
}

bool N::EditableUniverse::mouseDoubleClickEvent(QWidget * w,QMouseEvent * e)
{
  return LocalDoubleClick ( w , e ) ;
}

bool N::EditableUniverse::mouseMoveEvent(QWidget * w,QMouseEvent * e)
{
  return LocalHover ( w , e ) ;
}

bool N::EditableUniverse::mousePressEvent(QWidget * w,QMouseEvent * e)
{
  return LocalPressed ( w , e ) ;
}

bool N::EditableUniverse::mouseReleaseEvent(QWidget * w,QMouseEvent * e)
{
  return LocalReleased ( w , e ) ;
}

bool N::EditableUniverse::Moving(ControlPoint & From,ControlPoint & To)
{
  return LocalMoving ( From , To ) ;
}

void N::EditableUniverse::Relocation(QRectF viewport)
{
  Viewport = viewport                                 ;
  nDropOut ( IsNull ( CurrentCamera ) )               ;
  QRectF W = viewport                                 ;
  QRectF V ( 0 , 0 , W . width ( ) , W . height ( ) ) ;
  CurrentCamera -> assignViewport ( V )               ;
  AreaChanged                     ( W )               ;
}

void N::EditableUniverse::CloseChild(SUID u)
{
  VarArgs args           ;
  args << u              ;
  start ( 99001 , args ) ;
}

void N::EditableUniverse::ActualCloseChild(VarArgs & args)
{
  SUID u = args [ 0 ] . toULongLong ( )   ;
  Perform ( u , false )                   ;
  if ( ! Actors . contains ( u ) ) return ;
  LockAt   ( "UsingActors" )              ;
  LockAt   ( "Flush"       )              ;
  LockAt   ( "Animation"   )              ;
  LockAt   ( "Actors"      )              ;
  /////////////////////////////////////////
  SceneNode * sn = Actors [ u ]           ;
  removeActor ( u )                       ;
  /////////////////////////////////////////
  UnlockAt ( "Actors"      )              ;
  UnlockAt ( "Animation"   )              ;
  UnlockAt ( "Flush"       )              ;
  UnlockAt ( "UsingActors" )              ;
  /////////////////////////////////////////
  Time::msleep(750)                       ;
  /////////////////////////////////////////
  if ( NotNull ( sn ) ) sn -> suicide ( ) ;
}

void N::EditableUniverse::addWidget(QWidget * widget)
{
  if ( widgets . contains ( widget ) ) return ;
  widgets << widget                           ;
}

void N::EditableUniverse::takeWidget(QWidget * widget)
{
  if ( ! widgets . contains ( widget ) ) return ;
  int index = widgets . indexOf ( widget )      ;
  if ( index < 0 ) return                       ;
  widgets . takeAt ( index )                    ;
}

void N::EditableUniverse::addVcf(VcfInterface * vcf)
{
  if ( VCFs . contains ( vcf ) ) return ;
  VCFs << vcf                           ;
}

void N::EditableUniverse::takeVcf(VcfInterface * vcf)
{
  if ( ! VCFs . contains ( vcf ) ) return ;
  int index = VCFs . indexOf ( vcf )      ;
  if ( index < 0 ) return                 ;
  VCFs . takeAt ( index )                 ;
}

bool N::EditableUniverse::LoadPicture(SqlConnection & SC,SUID uuid)
{
  if ( Textures . contains ( uuid ) ) return true         ;
  PictureManager PM ( plan )                              ;
  QImage       * I = PM . Picture ( SC , uuid )           ;
  if ( IsNull ( I ) ) return false                        ;
  Textures [ uuid ]  = new SceneTexture ( destructor )    ;
  Textures [ uuid ] -> create ( I , true , true )         ;
  Textures [ uuid ] -> node     = uuid                    ;
  Textures [ uuid ] -> setVariable ( "Type" , "Picture" ) ;
  return true                                             ;
}

bool N::EditableUniverse::LoadThumb(SqlConnection & SC,SUID uuid)
{
  if ( Textures . contains ( uuid ) ) return true       ;
  PictureManager PM ( plan )                            ;
  QImage       * I = PM . Thumb ( SC , uuid )           ;
  if ( IsNull ( I ) ) return false                      ;
  Textures [ uuid ]  = new SceneTexture ( destructor )  ;
  Textures [ uuid ] -> create ( I , true , false )      ;
  Textures [ uuid ] -> node     = uuid                  ;
  Textures [ uuid ] -> setVariable ( "Type" , "Thumb" ) ;
  return true                                           ;
}

void N::EditableUniverse::addBookcase(SUID u)
{
  Bookcase * bc                                                         ;
  bc = new Bookcase ( destructor                                      ) ;
  addActor          ( u , bc                                          ) ;
  Perform           ( u , true                                        ) ;
  bc -> attache     ( Bookcase::FrontSide , Textures [ Types::Album ] ) ;
  bc -> addSequence ( 2 , SceneObject::RenderTextures                 ) ;
}

#pragma message("Okay, addAxis still has some problems")
void N::EditableUniverse::addAxis(double length,ControlPoint at,bool edit)
{
  double        L1                                                  ;
  double        L2                                                  ;
  double        R1                                                  ;
  double        R2                                                  ;
  SceneObject * axis                                                ;
  ///////////////////////////////////////////////////////////////////
  L1    = length * 0.95                                             ;
  L2    = length                                                    ;
  R1    = 2                                                         ;
  R2    = 3.5                                                       ;
  axis  = Scene::Axis   ( L1 , L2 , R1 , R2 )                       ;
  axis -> addDof        ( 0  , at           )                       ;
  axis -> setDestructor ( destructor        )                       ;
  axis -> Finish        (                   )                       ;
  axis -> Dual          (                   )                       ;
  ///////////////////////////////////////////////////////////////////
  SUID u = Lastest      (                   )                       ;
  axis -> node = u                                                  ;
  addActor              ( u , axis          )                       ;
  Perform               ( u , true          )                       ;
  ///////////////////////////////////////////////////////////////////
  if ( ! edit ) return                                              ;
  emit EditPoint ( tr("Original point") ,this , &(axis->DOFs[0] ) ) ;
}

void N::EditableUniverse::addAxis(bool edit)
{
  ControlPoint CP ( 0 , 0 , 0 , 1     ) ;
  CP . DOF = ControlPoint::DofTranslate ;
  addAxis         ( 400.0 , CP , edit ) ;
}

void N::EditableUniverse::AreaChanged(QRectF area)
{
  if ( ! isFunction ( FollowWindowSize ) ) return                      ;
  UUIDs U = Performs                                                   ;
  SUID  u                                                              ;
  foreach ( u , U )                                                    {
    SceneNode * sou = NULL                                             ;
    if ( Actors . contains ( u ) )                                     {
      sou = Actors [ u ]                                               ;
      if ( Scene::Object == sou -> nodeType )                          {
        SceneObject * sox = (SceneObject *) sou                        ;
        if ( sox -> Variables . contains ( "FollowWindow" ) )          {
          bool isFollow = sox -> Value ( "FollowWindow" ) . toBool ( ) ;
          if ( isFollow )                                              {
            if ( sox -> Relocation ( area ) )                          {
            }                                                          ;
          }                                                            ;
        }                                                              ;
      }                                                                ;
    }                                                                  ;
  }                                                                    ;
}

SUID N::EditableUniverse::CreateCalendar(QRectF & v,QDateTime st,QDateTime et,QDateTime sr,qint64 gap)
{
  double           ww  = v . width  ( ) / 2                                  ;
  double           hh  = v . height ( ) / 2                                  ;
  CalendarObject * rtt = new CalendarObject ( NULL , plan , destructor )     ;
  ControlPoint     at     ( -ww , -hh ,  0 , 1 )                             ;
  ControlPoint     scale  (   1 ,   1 ,  1 , 1 )                             ;
  ControlPoint     rotate (   0 ,   1 ,  0 , 0 )                             ;
  ////////////////////////////////////////////////////////////////////////////
  rtt    -> setUniverse            ( node                            )       ;
  rtt    -> gui     = gui                                                    ;
  rtt    -> minimum = ControlPoint (           0 ,            0 , -8 )       ;
  rtt    -> maximum = ControlPoint ( v . width() , v . height() ,  8 )       ;
  rtt    -> DynamicFunctions = DynamicFunctions                              ;
  BindingMenu                      ( rtt                             )       ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  scale   . DOF = ControlPoint::DofScale                                     ;
  rotate  . DOF = ControlPoint::DofRotate                                    ;
  rtt    -> setVariable  ( "CanClose" , true                    )            ;
  rtt    -> setVariable  ( "Tracking" , true                    )            ;
  rtt    -> addDof       (   0 , at                             )            ;
  rtt    -> addDof       (   1 , scale                          )            ;
  rtt    -> addDof       (   2 , rotate                         )            ;
  rtt    -> Finish       (                                      )            ;
  rtt    -> Dual         (                                      )            ;
  rtt    -> setRange     (  st , et                             )            ;
  rtt    -> setGap       (  sr , gap                            )            ;
  rtt    -> addSequence  (  11 , SceneObject::RenderGrids       )            ;
  rtt    -> addSequence  (  15 , SceneObject::AssignColor       )            ;
  rtt    -> addSequence  (  16 , SceneObject::RenderBox         )            ;
  rtt    -> addSequence  (  20 , SceneObject::RenderChildren    )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( rtt  , SIGNAL ( CloseObject (SUID) )                            ,
             this , SLOT   ( CloseChild  (SUID) )               )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest       (                                      )            ;
  rtt -> node = u                                                            ;
  addActor               ( u , rtt                              )            ;
  Perform                ( u , true                             )            ;
  rtt    -> start        ( 10001                                )            ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants vx                                                               ;
  CallDynamic ( "CalendarConnector" , rtt , gui , vx )                       ;
  return u                                                                   ;
}

SUID N::EditableUniverse::CreateCalendar(QRectF & vw)
{
  QDateTime st                                      ;
  QDateTime et  = nTimeNow                          ;
  QDateTime sr                                      ;
  qint64    gap = 86400 * 1000                      ;
  QDate     bd                                      ;
  QTime     zt ( 0 , 0 , 0 , 0 )                    ;
  ///////////////////////////////////////////////////
  et = et . addSecs (   86400 * 4 )                 ;
  zt = QTime ( et.time().hour() , 0 , 0 , 0 )       ;
  bd = et . date ( )                                ;
  et = QDateTime ( bd , zt )                        ;
  st = et                                           ;
  st = st . addSecs ( - 86400 * 7 )                 ;
  sr = QDateTime ( st . date ( ) , QTime(0,0,0,0) ) ;
  sr = sr . addSecs ( 86400       )                 ;
  ///////////////////////////////////////////////////
  return CreateCalendar ( vw , st , et , sr , gap ) ;
}

void N::EditableUniverse::run(int type,ThreadData * data)
{
  ItemRunner ( type , data ) ;
}

bool N::EditableUniverse::ItemRunner(int type,ThreadData * data)
{
  switch ( type )                                                            {
    case 99001                                                               :
      ActualCloseChild   ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 61        )                             : // Gallery
      LaunchPictures     ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 62        )                             : // Actors, People
      LaunchRoles        ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 63        )                             : // Clips
      LaunchClips        ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 64        )                             : // Torrents
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 65        )                             : // All related items
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 66        )                             : // Picture Manipulator
      EditPicture        ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 67        )                             : // Video albums
      LaunchAlbums       ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 44 , 68        )                             : // Subgroups
      LaunchSubgroup     ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 91        )                             : // All galleries
      SeeGalleries       ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 92        )                             : // All album groups
      SeeAlbums          ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 93        )                             : // All crowds
      SeeCrowds          ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 94        )                             : // Play clip
      PlayClip           ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 95        )                             : // Choose camera
      ChooseCamera       (                     )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 96        )                             : // Camera selected
      CameraSelected     ( data -> Arguments   )                             ;
    return true                                                              ;
    case CiosMenuId      ( 89 , 45 , 97        )                             :
      addEar             (                     )                             ;
    break                                                                    ;
    case CiosMenuId      ( 89 , 45 , 98        )                             :
      if ( Viewport . width ( ) > 0  )                                       {
        QRectF vw ( 0 , 0 , Viewport . width ( ) , Viewport . height ( ) )   ;
        CreateCalendar ( vw )                                                ;
      }                                                                      ;
    break                                                                    ;
  }                                                                          ;
  return false                                                               ;
}

void N::EditableUniverse::RemovePerformers(QString TypeName)
{
  UUIDs R                                                 ;
  SUID  u                                                 ;
  /////////////////////////////////////////////////////////
  foreach ( u , Performs ) if ( Actors . contains ( u ) ) {
    SceneNode * sn = Actors [ u ]                         ;
    if ( sn -> Variables . contains ( "Type" ) )          {
      QString tn = sn->Value("Type").toString()           ;
      if ( TypeName == tn )                               {
        if ( ! R . contains ( u ) ) R << u                ;
      }                                                   ;
    }                                                     ;
  }                                                       ;
  if ( R . count ( ) <= 0 ) return                        ;
  /////////////////////////////////////////////////////////
  foreach ( u , R ) Perform ( u , false )                 ;
}

#pragma message("N::EditableUniverse::doubleClicked is about to remove")
void N::EditableUniverse::doubleClicked(QWidget * widget,QPointF pos,UUIDs & U)
{
  SUID u                                           ;
  foreach ( u , U )                                {
    QString n                                      ;
    if ( Names . contains ( u ) ) n = Names [ u ]  ;
    if ( Actors . contains ( u ) )                 {
      SceneNode * sn = Actors [ u ]                ;
      if ( sn -> Variables . contains ( "Type" ) ) {
        QString tn = sn->Value("Type").toString()  ;
        if ( "Albums" == tn )                      {
          VarArgs args                             ;
          if ( Types::Album == u )                 {
            args << 0                              ;
          } else                                   {
            args << 1                              ;
          }                                        ;
          args << u                                ;
          args << Types::Division                  ;
          args << Types::Album                     ;
          args << Groups::Subordination            ;
          args << 0                                ;
          args << 40                               ;
          args << (int) Qt::DescendingOrder        ;
          start ( 90014 , args )                   ;
        }                                          ;
        if ( "Album" == tn )                       {
          emit Albums      ( u       )             ;
          RemovePerformers ( "Album" )             ;
        }                                          ;
        if ( "Crowd" == tn )                       {
          VarArgs args                             ;
          if ( Types::People == u )                {
            args << 0                              ;
          } else                                   {
            args << 1                              ;
          }                                        ;
          args << u                                ;
          args << Types::Division                  ;
          args << Types::People                    ;
          args << Groups::Subordination            ;
          args << 0                                ;
          args << 40                               ;
          args << (int) Qt::AscendingOrder         ;
          start ( 91014 , args )                   ;
        }                                          ;
        if ( "People" == tn )                      {
          emit People      ( this , u )            ;
          RemovePerformers ( "People" )            ;
        }                                          ;
        if ( "Gallery" == tn )                     {
          VarArgs args                             ;
          if ( Types::Gallery == u )               {
            args << 0                              ;
          } else                                   {
            args << 1                              ;
          }                                        ;
          args << u                                ;
          args << Types::Gallery                   ;
          args << Types::Picture                   ;
          args << Groups::Subordination            ;
          args << 0                                ;
          args << 50                               ;
          args << (int) Qt::AscendingOrder         ;
          start ( 92014 , args )                   ;
        }                                          ;
        if ( "Picture" == tn )                     {
          emit Picture     ( this , u  )           ;
          RemovePerformers ( "Picture" )           ;
        }                                          ;
      }                                            ;
    }                                              ;
  }                                                ;
}

#pragma message("N::EditableUniverse::addText is about to remove")
void N::EditableUniverse::addText(void)
{
  if ( IsNull(glyph) )                                      {
    QStrings f                                              ;
    QString  s                                              ;
    glyph = new GlyphManager (           )                  ;
    f     = glyph -> Fonts   (           )                  ;
    s     = glyph -> path    ( "Courier New" )              ;
    if ( s . length() > 0 ) glyph -> add ( 0 , s )          ;
    s     = glyph -> path    ( "MingLiU" )                  ;
    if ( s . length() > 0 ) glyph -> add ( 1 , s )          ;
    glyph -> setPixels       ( 48 , 48 )                    ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  ControlPoint P ( 0 , 0 , 32 )                             ;
  Bookcase * bc                                             ;
  bc = new Bookcase ( destructor                          ) ;
  P . DOF = ControlPoint::DofTranslate                      ;
  bc -> node = 1                                            ;
  bc -> setVariable ( "Type"      , "Text"                ) ;
  bc -> addDof      ( 0 , P                               ) ;
  ///////////////////////////////////////////////////////////
  QString      whoever = Variables["NAME"].toString()       ;
  ControlPoint Z ( -320 , 0 , 32 )                          ;
  Z . r = 640                                               ;
  bc -> addText     ( *glyph , Z , whoever                ) ;
  bc -> Finish      (                                     ) ;
  bc -> Dual        (                                     ) ;
  bc -> addSequence ( 1 , SceneObject::AssignColor        ) ;
  bc -> addSequence ( 9 , SceneObject::RenderTessllations ) ;
  addActor          ( bc -> node , bc                     ) ;
  Perform           ( bc -> node , true                   ) ;
}

void N::EditableUniverse::PlayClip(VarArgs & args)
{
  // syntax : video(uuid) album(uuid) go_next(bool) record_event(bool) description(uuid) delay(ms) filename(string) event(uuid)
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return           ;
  if ( args [ 11 ] . toBool ( ) )               {
    CloseChild ( args [ 9 ] . toULongLong ( ) ) ;
  }                                             ;
  ///////////////////////////////////////////////
  VarArgs xrgs                                  ;
  xrgs << CiosMenuId ( 99 , 40 , 14 )           ;
  xrgs << args [ 0 ] . toULongLong ( )          ;
  xrgs << args [ 6 ] . toULongLong ( )          ;
  xrgs << true                                  ;
  xrgs << false                                 ;
  xrgs << 0                                     ;
  xrgs << 3000                                  ;
  UniverseWidget::CallBinding ( xrgs )          ;
}

void N::EditableUniverse::EditPicture(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID                 t   = args [ 0 ] . toULongLong ( )                    ;
  PictureManipulator * pmo = new PictureManipulator ( NULL                   ,
                                                      plan                   ,
                                                      destructor           ) ;
  ControlPoint         at          ( -850 , -400 ,  96 , 1 )                 ;
  ////////////////////////////////////////////////////////////////////////////
  pmo    -> setUniverse            ( node                  )                 ;
  pmo    -> gui     = gui                                                    ;
  pmo    -> minimum = ControlPoint (    0 ,    0 ,   0 , 1 )                 ;
  pmo    -> maximum = ControlPoint ( 1700 ,  800 ,  32 , 1 )                 ;
  pmo    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pmo , bargs                         )            ;
  BindingMenu             ( pmo                                 )            ;
  BindRequest             ( pmo                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pmo    -> addDof        (   0 , at                            )            ;
  pmo    -> Finish        (                                     )            ;
  pmo    -> Dual          (                                     )            ;
  pmo    -> addSequence   (  11 , SceneObject::RenderUser       )            ;
  pmo    -> addSequence   (  15 , SceneObject::RenderFaces      )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pmo -> node = u                                                            ;
  addActor                ( u , pmo                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pmo    -> LoadTexture   ( t                                   )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pmo  , SIGNAL (CloseObject(SUID))                               ,
             this , SLOT   (CloseChild (SUID))                               ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::LaunchPictures(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    g = args [ 0 ] . toULongLong ( )                                   ;
  QString n = args [ 1 ] . toString    ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  PictureShelf * pso = new PictureShelf ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                 ( node                     )         ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  pso    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pso , bargs                         )            ;
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  616 ,  2 ,   1              )            ;
  ControlPoint box        (  136 ,  136 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -144 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  136 ,  136 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    4 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setBase       ( base                                )            ;
  pso    -> setBox        ( box                                 )            ;
  pso    -> setGap        ( gap                                 )            ;
  pso    -> setAppear     ( appear                              )            ;
  pso    -> setDefaultMin ( minp                                )            ;
  pso    -> setDefaultMax ( maxp                                )            ;
  pso    -> setContainer  ( minz , maxz                         )            ;
  pso    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  pso    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  pso    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  pso    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  pso    -> setAccess     ( "First"                             )            ;
  pso    -> setVariable   ( "T1"    , args [ 2 ] . toInt ( )    )            ;
  pso    -> setVariable   ( "First" , g                         )            ;
  pso    -> setVariable   ( "Relation" , args [ 8 ]             )            ;
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setTitle      ( n                                   )            ;
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::LaunchRoles(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    g = args [ 0 ] . toULongLong ( )                                   ;
  QString n = args [ 1 ] . toString    ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  PeopleShelf  * pso = new PeopleShelf  ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                 ( node                     )         ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  pso    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pso , bargs                         )            ;
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setBase       ( base                                )            ;
  pso    -> setBox        ( box                                 )            ;
  pso    -> setGap        ( gap                                 )            ;
  pso    -> setAppear     ( appear                              )            ;
  pso    -> setDefaultMin ( minp                                )            ;
  pso    -> setDefaultMax ( maxp                                )            ;
  pso    -> setContainer  ( minz , maxz                         )            ;
  pso    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  pso    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  pso    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  pso    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  pso    -> setAccess     ( "First"                             )            ;
  pso    -> setVariable   ( "T1"    , args [ 4 ] . toInt ( )    )            ;
  pso    -> setVariable   ( "T2"    , Types::People             )            ;
  pso    -> setVariable   ( "First" , g                         )            ;
  pso    -> setVariable   ( "Relation" , args [ 8 ]             )            ;
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setTitle      ( n                                   )            ;
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::LaunchAlbums(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    g = args [ 0 ] . toULongLong ( )                                   ;
  QString n = args [ 1 ] . toString    ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  VideoShelf   * pso = new VideoShelf   ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                 ( node                     )         ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  pso    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pso , bargs                         )            ;
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setBase       ( base                                )            ;
  pso    -> setBox        ( box                                 )            ;
  pso    -> setGap        ( gap                                 )            ;
  pso    -> setAppear     ( appear                              )            ;
  pso    -> setDefaultMin ( minp                                )            ;
  pso    -> setDefaultMax ( maxp                                )            ;
  pso    -> setContainer  ( minz , maxz                         )            ;
  pso    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  pso    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  pso    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  pso    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  pso    -> setAccess     ( "First"                             )            ;
  pso    -> setVariable   ( "T1"    , args [ 4 ] . toInt ( )    )            ;
  pso    -> setVariable   ( "T2"    , Types::Album              )            ;
  pso    -> setVariable   ( "First" , g                         )            ;
  pso    -> setVariable   ( "Relation" , args [ 8 ]             )            ;
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setTitle      ( n                                   )            ;
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
//  nConnect ( pso  , SIGNAL ( DoubleClicked (SelectItem*) )                   ,
//             this , SLOT   ( VideoClicked  (SelectItem*) )                 ) ;
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::LaunchClips(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    g = args [ 0 ] . toULongLong ( )                                   ;
  QString n = args [ 1 ] . toString    ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  ClipsShelf   * pso = new ClipsShelf   ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                 ( node                     )         ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint      (   0 ,    0 ,   0 , 1     )         ;
  pso    -> maximum = ControlPoint      ( 580 ,  800 ,  32 , 1     )         ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pso , bargs                         )            ;
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  720 ,  2 ,   1              )            ;
  ControlPoint box        (  516 ,   20 , 28 ,   1              )            ;
  ControlPoint gap        (    0 ,  -24 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  516 ,   20 , 28 ,   1              )            ;
  ControlPoint minz       (   10 ,   10 ,  0 ,   1              )            ;
  ControlPoint maxz       (  550 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    (  560 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setBase       ( base                                )            ;
  pso    -> setBox        ( box                                 )            ;
  pso    -> setGap        ( gap                                 )            ;
  pso    -> setAppear     ( appear                              )            ;
  pso    -> setDefaultMin ( minp                                )            ;
  pso    -> setDefaultMax ( maxp                                )            ;
  pso    -> setContainer  ( minz , maxz                         )            ;
  pso    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  pso    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  pso    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  pso    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  pso    -> setAccess     ( "First"                             )            ;
  pso    -> setVariable   ( "T1"    , args [ 2 ] . toInt ( )    )            ;
  pso    -> setVariable   ( "T2"    , Types::Video              )            ;
  pso    -> setVariable   ( "First" , g                         )            ;
  pso    -> setVariable   ( "Relation" , args [ 8 ]             )            ;
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setTitle      ( n                                   )            ;
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::LaunchSubgroup(VarArgs & args)
{
  //  0 => UUID
  //  1 => Name
  //  2 => GroupType ( 2 )
  //  3 => GroupType ( 3 )
  //  4 => GroupType ( 4 )
  //  5 => GroupType ( 5 )
  //  6 => First
  //  7 => Second
  //  8 => Relation
  //  9 => Node
  // 10 => Universe
  // 11 => Suicide
  if ( args . count ( ) < 12 ) return                                        ;
  if ( args [ 11 ] . toBool ( ) )                                            {
    CloseChild ( args [ 9 ] . toULongLong ( ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    g = args [ 0 ] . toULongLong ( )                                   ;
  QString n = args [ 1 ] . toString    ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  SubgroupShelf * pso = new SubgroupShelf ( NULL , plan , destructor )       ;
  ControlPoint    at                      ( -850 , -400 ,  96 , 1    )       ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                   ( node                     )       ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint        (    0 ,    0 ,   0 , 1    )       ;
  pso    -> maximum = ControlPoint        ( 1700 ,  800 ,  32 , 1    )       ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( pso , bargs                         )            ;
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setBase       ( base                                )            ;
  pso    -> setBox        ( box                                 )            ;
  pso    -> setGap        ( gap                                 )            ;
  pso    -> setAppear     ( appear                              )            ;
  pso    -> setDefaultMin ( minp                                )            ;
  pso    -> setDefaultMax ( maxp                                )            ;
  pso    -> setContainer  ( minz , maxz                         )            ;
  pso    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  pso    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  pso    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  pso    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  pso    -> setAccess     ( "First"                             )            ;
  pso    -> setVariable   ( "T1"    , args [ 5 ] . toInt ( )    )            ;
  pso    -> setVariable   ( "T2"    , Types::Subgroup           )            ;
  pso    -> setVariable   ( "First" , g                         )            ;
  pso    -> setVariable   ( "Relation" , args [ 8 ]             )            ;
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setTitle      ( n                                   )            ;
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::SeeGalleries(VarArgs & args)
{
  GalleryShelf * rbo = new GalleryShelf ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setUniverse                 ( node                     )         ;
  rbo    -> gui     = gui                                                    ;
  rbo    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  rbo    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  rbo    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( rbo , bargs                         )            ;
  BindingMenu             ( rbo                                 )            ;
  BindRequest             ( rbo                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  rbo    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setBase       ( base                                )            ;
  rbo    -> setBox        ( box                                 )            ;
  rbo    -> setGap        ( gap                                 )            ;
  rbo    -> setAppear     ( appear                              )            ;
  rbo    -> setDefaultMin ( minp                                )            ;
  rbo    -> setDefaultMax ( maxp                                )            ;
  rbo    -> setContainer  ( minz , maxz                         )            ;
  rbo    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  rbo    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  rbo    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  rbo    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  rbo    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  rbo -> node = u                                                            ;
  addActor                ( u , rbo                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setTitle      ( tr("Galleries")                     )            ;
  rbo    -> Relocation    ( Viewport                            )            ;
  rbo    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( rbo  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::SeeAlbums(VarArgs & args)
{
  AlbumShelf   * rbo = new AlbumShelf   ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setUniverse                 ( node                     )         ;
  rbo    -> gui     = gui                                                    ;
  rbo    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  rbo    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  rbo    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( rbo , bargs                         )            ;
  BindingMenu             ( rbo                                 )            ;
  BindRequest             ( rbo                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  rbo    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setBase       ( base                                )            ;
  rbo    -> setBox        ( box                                 )            ;
  rbo    -> setGap        ( gap                                 )            ;
  rbo    -> setAppear     ( appear                              )            ;
  rbo    -> setDefaultMin ( minp                                )            ;
  rbo    -> setDefaultMax ( maxp                                )            ;
  rbo    -> setContainer  ( minz , maxz                         )            ;
  rbo    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  rbo    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  rbo    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  rbo    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  rbo    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  rbo -> node = u                                                            ;
  addActor                ( u , rbo                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setTitle      ( tr("Albums")                        )            ;
  rbo    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( rbo  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::SeeCrowds(VarArgs & args)
{
  CrowdsShelf  * rbo = new CrowdsShelf  ( NULL , plan , destructor )         ;
  ControlPoint   at                     ( -850 , -400 ,  96 , 1    )         ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setUniverse                 ( node                     )         ;
  rbo    -> gui     = gui                                                    ;
  rbo    -> minimum = ControlPoint      (    0 ,    0 ,   0 , 1    )         ;
  rbo    -> maximum = ControlPoint      ( 1700 ,  800 ,  32 , 1    )         ;
  rbo    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  QVariants      bargs                                                       ;
  bargs << CiosMenuId     ( 99 , 38 , 12                        )            ;
  CallBinding             ( rbo , bargs                         )            ;
  BindingMenu             ( rbo                                 )            ;
  BindRequest             ( rbo                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  rbo    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint header     (   20 ,  764 ,  2 ,   1              )            ;
  ControlPoint base       (   20 ,  580 ,  2 ,   1              )            ;
  ControlPoint box        (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint gap        (  144 , -180 ,  0 ,   1              )            ;
  ControlPoint appear     (   20 ,   20 ,  2 ,   1              )            ;
  ControlPoint minp       (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint maxp       (  128 ,  168 , 28 ,   1              )            ;
  ControlPoint minz       (   20 ,   40 ,  0 ,   1              )            ;
  ControlPoint maxz       ( 1680 ,  760 , 32 ,   1              )            ;
  ControlPoint hscroll    (    0 ,    0 ,  0 ,   1              )            ;
  ControlPoint vscroll    ( 1680 ,  760 ,  0 ,   1              )            ;
  ControlPoint vsbangle   (    0 ,    0 ,  1 , 270              )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setBase       ( base                                )            ;
  rbo    -> setBox        ( box                                 )            ;
  rbo    -> setGap        ( gap                                 )            ;
  rbo    -> setAppear     ( appear                              )            ;
  rbo    -> setDefaultMin ( minp                                )            ;
  rbo    -> setDefaultMax ( maxp                                )            ;
  rbo    -> setContainer  ( minz , maxz                         )            ;
  rbo    -> AssignPoint   ( "HeaderCorner" , header             )            ;
  rbo    -> AssignPoint   ( "HScrollPos"   , hscroll            )            ;
  rbo    -> AssignPoint   ( "VScrollPos"   , vscroll            )            ;
  rbo    -> AssignPoint   ( "VSBAngle"     , vsbangle           )            ;
  rbo    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  rbo -> node = u                                                            ;
  addActor                ( u , rbo                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  rbo    -> setTitle      ( tr("Crowds")                        )            ;
  rbo    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( rbo  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::addEar(void)
{
  EarIn       * pso = new EarIn ( NULL , plan , destructor )                 ;
  ControlPoint  at              (    0 ,    0 ,  144 , 1   )                 ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> setUniverse                 ( node                     )         ;
  pso    -> gui     = gui                                                    ;
  pso    -> minimum = ControlPoint      ( -400 , -100 ,  -2 , 1    )         ;
  pso    -> maximum = ControlPoint      (  400 ,  100 ,   2 , 1    )         ;
  pso    -> DynamicFunctions = DynamicFunctions                              ;
  ////////////////////////////////////////////////////////////////////////////
  BindingMenu             ( pso                                 )            ;
  BindRequest             ( pso                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  at      . DOF = ControlPoint::DofTranslate                                 ;
  pso    -> addDof        (   0 , at                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> prepare       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  SUID u = Lastest        (                                     )            ;
  pso -> node = u                                                            ;
  addActor                ( u , pso                             )            ;
  Perform                 ( u , true                            )            ;
  ////////////////////////////////////////////////////////////////////////////
  pso    -> startup       (                                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( pso  , SIGNAL ( CloseObject(SUID))                              ,
             this , SLOT   ( CloseChild (SUID))                              ,
             Qt::DirectConnection                                          ) ;
}

void N::EditableUniverse::Clean(void)
{
  UUIDs U = Actors . keys ( )        ;
  SUID  u                            ;
  foreach ( u , U ) CloseChild ( u ) ;
}

bool N::EditableUniverse::ObjectsMenu (
       MenuManager & mm               ,
       QMenu       * mo               )
{
  if ( Actors . count ( ) > 0 )                          {
    mm . add        ( mo , 7711001 , tr("Clean"      ) ) ;
  }                                                      ;
  mm . add          ( mo , 7711002 , tr("Add an axis") ) ;
  mm . add          ( mo , 7711003 , tr("Camera"     ) ) ;
  mm . addSeparator ( mo                               ) ;
  mm . add          ( mo                                 ,
                      CiosMenuId ( 77 , 13 , 1 )         ,
                      tr("Gallery")                    ) ;
  mm . add          ( mo                                 ,
                      CiosMenuId ( 77 , 13 , 2 )         ,
                      tr("Album groups")               ) ;
  mm . add          ( mo                                 ,
                      CiosMenuId ( 77 , 13 , 3 )         ,
                      tr("Crowds")                     ) ;
  mm . add          ( mo                                 ,
                      CiosMenuId ( 77 , 13 , 4 )         ,
                      tr("Scheduler")                  ) ;
  return true                                            ;
}

bool N::EditableUniverse::ControlMenu (
       MenuManager & mm               ,
       QMenu       * mc               )
{
  mm . add ( mc                         ,
             CiosMenuId ( 77 , 14 , 1 ) ,
             tr("Select camera")      ) ;
  return true                           ;
}

bool N::EditableUniverse::InterfaceMenu(MenuManager & mm,QMenu * mi)
{
  mm . add ( mi                         ,
             CiosMenuId ( 77 , 15 , 1 ) ,
             tr("Ears")               ) ;
  return true                           ;
}

int N::EditableUniverse::MenuBinder (
      MenuManager & mm              ,
      QAction     * aa              ,
      QVariants   & v               ,
      void        * dispatcher      )
{
  bool completed = v [ 0 ] . toBool ( )                                      ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! completed )                                                         {
    QMenu   * wm = mm . addMenu ( tr("World menu") )                         ;
    QMenu   * mo                                                             ;
    QMenu   * mi                                                             ;
    QMenu   * mc                                                             ;
    QMenu   * mp                                                             ;
    //////////////////////////////////////////////////////////////////////////
    if ( Variables . contains ( "Name" ) )                                   {
      QString n = Variables [ "Name" ] . toString ( )                        ;
      aa  = mm . add          ( wm , 0 , n                   )               ;
      aa -> setEnabled        ( false                        )               ;
      mm       . addSeparator (                              )               ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    mo  = mm . addMenu      ( wm , tr("Objects"  )           )               ;
    mi  = mm . addMenu      ( wm , tr("Interface")           )               ;
    mc  = mm . addMenu      ( wm , tr("Control"  )           )               ;
    mp  = mm . addMenu      ( wm , tr("Paraments")           )               ;
    //////////////////////////////////////////////////////////////////////////
    ObjectsMenu             ( mm , mo                        )               ;
    InterfaceMenu           ( mm , mi                        )               ;
    ControlMenu             ( mm , mc                        )               ;
    if ( isFunction ( HasUniverseMenu ) )                                    {
      mm . addSeparator ( mi                                            )    ;
      mm . add          ( mi , CiosMenuId(77,11,5) , tr("Disable menu") )    ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    mm       . addSeparator ( wm                             )               ;
    mm       . add          ( wm                                             ,
                              CiosMenuId ( 77 , 11 , 1 )                     ,
                              tr("Close")                                  ) ;
    CallToMenu              ( mm , aa , false                              ) ;
    return 1                                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  switch               ( mm [ aa ]                   )                       {
    case 7711001                                                             :
      Clean            (                             )                       ;
    return 1                                                                 ;
    case 7711002                                                             :
      addAxis          ( true                        )                       ;
    return true                                                              ;
    case 7711003                                                             :
      emit EditCamera  ( this , CurrentCamera        )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 11 , 1                 )                       :
      emit Close       ( this                        )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 11 , 5                 )                       :
      setFunction      ( HasUniverseMenu , false     )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 13 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 91 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 13 , 2                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 92 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 13 , 3                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 93 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 13 , 4                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 98 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 14 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 95 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 15 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 97 ) )                       ;
    return 1                                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return CallToMenu    ( mm , aa , true              )                       ;
}

bool N::EditableUniverse::Menu(QWidget * widget,QPointF pos)
{
  UUIDs U = Intersected ( pos )                                              ;
  if ( U . count ( ) > 0 )                                                   {
    if ( Universe::Menu ( widget , pos , U ) ) return true                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager mm ( widget )                                                  ;
  QAction   * aa                                                             ;
  QMenu     * mo                                                             ;
  QMenu     * mi                                                             ;
  QMenu     * mc                                                             ;
  QMenu     * mp                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Variables . contains ( "Name" ) )                                     {
    QString n = Variables [ "Name" ] . toString ( )                          ;
    aa  = mm . add          ( 0   , n                      )                 ;
    aa -> setEnabled        ( false                        )                 ;
    mm       . addSeparator (                              )                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mo  = mm . addMenu      ( tr("Objects"  )                )                 ;
  mi  = mm . addMenu      ( tr("Interface")                )                 ;
  mc  = mm . addMenu      ( tr("Control"  )                )                 ;
  mp  = mm . addMenu      ( tr("Paraments")                )                 ;
  ////////////////////////////////////////////////////////////////////////////
  ObjectsMenu             ( mm , mo                        )                 ;
  InterfaceMenu           ( mm , mi                        )                 ;
  ControlMenu             ( mm , mc                        )                 ;
  if ( isFunction ( HasUniverseMenu ) )                                      {
    mm . addSeparator ( mi                                            )      ;
    mm . add          ( mi , CiosMenuId(77,11,5) , tr("Disable menu") )      ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mm       . addSeparator (                                )                 ;
  CallToMenu              ( mm , NULL , false              )                 ;
  mm       . addSeparator (                                )                 ;
  mm       . add          ( 101 , tr("Close")              )                 ;
  ////////////////////////////////////////////////////////////////////////////
  mm       . setFont      ( plan                           )                 ;
  ////////////////////////////////////////////////////////////////////////////
  aa  = mm . exec      (                             )                       ;
  nKickOut             ( IsNull ( aa ) , true        )                       ;
  if ( 0 != CallToMenu ( mm , aa , true ) ) return true                      ;
  ////////////////////////////////////////////////////////////////////////////
  switch               ( mm [ aa ]                   )                       {
    case  101                                                                :
      emit Close       ( this                        )                       ;
    return true                                                              ;
    case 7711001                                                             :
      Clean            (                             )                       ;
    return true                                                              ;
    case 7711002                                                             :
      addAxis          ( true                        )                       ;
    return true                                                              ;
    case 7711003                                                             :
      emit EditCamera  ( this , CurrentCamera        )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 11 , 5                 )                       :
      setFunction      ( HasUniverseMenu , false     )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 13 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 91 ) )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 13 , 2                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 92 ) )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 13 , 3                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 93 ) )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 13 , 4                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 98 ) )                       ;
    return 1                                                                 ;
    case CiosMenuId    ( 77 , 14 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 95 ) )                       ;
    return true                                                              ;
    case CiosMenuId    ( 77 , 15 , 1                 )                       :
      start            ( CiosMenuId ( 89 , 45 , 97 ) )                       ;
    return 1                                                                 ;
  }                                                                          ;
  return true                                                                ;
}
