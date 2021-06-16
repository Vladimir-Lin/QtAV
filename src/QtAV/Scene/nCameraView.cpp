#include <qtav.h>

N::CameraView:: CameraView ( QWidget * parent , Plan * p    )
              : GLWidget   ( QGLFormat(QGL::DoubleBuffer    |
                                       QGL::DepthBuffer     |
                                       QGL::Rgba            |
                                       QGL::AlphaChannel    |
                                       QGL::AccumBuffer     |
                                       QGL::StencilBuffer   |
                                       QGL::HasOverlay      |
                                       QGL::SampleBuffers ) ,
                                       parent ,         p   )
{
  Configure ( ) ;
}

N::CameraView::~CameraView(void)
{
}

void N::CameraView::Configure(void)
{
  setMouseTracking ( true                    )                  ;
  setFocusPolicy   ( Qt::WheelFocus          )                  ;
  setGeometry      ( QRect(0,0,256,256)      )                  ;
  setWindowTitle   ( tr("Camera controller") )                  ;
  ///////////////////////////////////////////////////////////////
  lights = 0                                                    ;
  root.setProperty(GL_DEPTH_TEST    ,true)                      ;
  root.setProperty(GL_BLEND         ,true)                      ;
  root.setProperty(GL_LINE_SMOOTH   ,true)                      ;
  root.setProperty(GL_COLOR_MATERIAL,true)                      ;
  ///////////////////////////////////////////////////////////////
  camera  . mode   = Camera::Perspective                        ;
  camera  . eye    = ControlPoint ( 0.0 ,  0.0 , 500.0 )        ;
  camera  . center = ControlPoint ( 0.0 ,  0.0 ,   0.0 )        ;
  camera  . up     = ControlPoint ( 0.0 ,  1.0 ,   0.0 )        ;
  camera  . setNearFar            ( 1.0 ,  10000.0     )        ;
  ///////////////////////////////////////////////////////////////
  QColor       CX1                                              ;
  QColor       CX2                                              ;
  ControlPoint PX1                                              ;
  Light L1                                                      ;
  Light L2                                                      ;
  Light L3                                                      ;
  Light L4                                                      ;
  L1.lightId  =  GL_LIGHT0                                      ;
  CX1 = QColor(255,224,224)                                     ;
  CX2 = QColor(255,224,224)                                     ;
  PX1 = ControlPoint(-450.00, 450.00,-450.00,1.0)               ;
  L1.setDiffuse  ( CX1                                        ) ;
  L1.setSpecular ( CX2                                        ) ;
  L1.setPosition ( PX1                                        ) ;
  L2.lightId  =  GL_LIGHT1                                      ;
  CX1 = QColor(224,224,255)                                     ;
  CX2 = QColor(224,224,255)                                     ;
  PX1 = ControlPoint( 450.00, 450.00,-450.00,1.0)               ;
  L2.setDiffuse  ( CX1                                        ) ;
  L2.setSpecular ( CX2                                        ) ;
  L2.setPosition ( PX1                                        ) ;
  L1.lightId  =  GL_LIGHT2                                      ;
  CX1 = QColor(224,255,224)                                     ;
  CX2 = QColor(224,255,224)                                     ;
  PX1 = ControlPoint( 450.00, 450.00, 450.00,1.0)               ;
  L1.setDiffuse  ( CX1                                        ) ;
  L1.setSpecular ( CX2                                        ) ;
  L1.setPosition ( PX1                                        ) ;
  L1.lightId  =  GL_LIGHT3                                      ;
  CX1 = QColor(224,224,224)                                     ;
  CX2 = QColor(224,224,224)                                     ;
  PX1 = ControlPoint(-450.00, 450.00, 450.00,1.0)               ;
  L1.setDiffuse  ( CX1                                        ) ;
  L1.setSpecular ( CX2                                        ) ;
  L1.setPosition ( PX1                                        ) ;
  lightings << L1                                               ;
  lightings << L2                                               ;
  lightings << L3                                               ;
  lightings << L4                                               ;
  ///////////////////////////////////////////////////////////////
  Plane plane                                                   ;
  plane.O = ControlPoint(-1000,-1000,0.0,1.0)                   ;
  plane.X = ControlPoint(   50,    0,0.0,0.0)                   ;
  plane.Y = ControlPoint(    0,   50,0.0,0.0)                   ;
  plane.W = 40                                                  ;
  plane.H = 40                                                  ;
  ///////////////////////////////////////////////////////////////
  SceneObject object                                            ;
  ControlPoint CF(0.90,0.90,0.90,0.25)                          ;
  ControlPoint CP(1.00,0.25,0.25,0.25)                          ;
  ControlPoint CL(0.50,0.50,0.50,1.00)                          ;
  ControlPoint CX(1.00,0.84,0.00,1.00)                          ;
  int face = 0                                                  ;
  int last = 0                                                  ;
  int line = 0                                                  ;
  int cid  = 0                                                  ;
  objects  <<  object                                           ;
  objects  <<  object                                           ;
  objects  <<  object                                           ;
  face = objects[0] . Last ( SceneObject::Face    )             ;
  line = objects[0] . Last ( SceneObject::Segment )             ;
  last = objects[0] . Last (                       )            ;
  objects[0] . addColor  ( 0 , CF )                             ;
  objects[0] . addColor  ( 1 , CP )                             ;
  objects[0] . addColor  ( 2 , CL )                             ;
  objects[0] . addColor  ( 3 , CX )                             ;
  objects[0] . addCube   ( 90     )                             ;
  objects[0] . faces . remove ( 1 )                             ;
  objects[0] . faces . remove ( 2 )                             ;
  objects[0] . faces . remove ( 3 )                             ;
  objects[0] . faces . remove ( 4 )                             ;
  cid = objects[0] . faces [ 5 ] . index [ 0 ]                  ;
  objects[0] . faces [ 5 ] . colors [ cid ] = 0                 ;
  cid = objects[0] . segments [ 1 ] . index [ 0 ]               ;
  objects[0] . segments [ 1 ] . colors [ cid ] = 3              ;
  objects[0] . segments [ 1 ] . lineWidth = 1.75                ;
  objects[0] . segments [ 1 ] . pointColor = true               ;
  face = objects[0] . Last ( SceneObject::Face    )            ;
  line = objects[0] . Last ( SceneObject::Segment )            ;
  last = objects[0] . Last (                       )            ;
  objects[0] . addPlane ( plane )                               ;
  cid = objects[0] . faces [ face ] . index [ 0 ]               ;
  objects[0] . faces [ face ] . colors [ cid ] = 1              ;
  objects[0] . segments [ line ] . colors [ cid ] = 2           ;
  objects[0] . segments [ line ] . lineWidth = 1.2              ;
  objects[0] . segments [ line ] . pointColor = true            ;
  QList<int> FS = objects[0] . faces . keys ()                  ;
  for (int xx=0;xx<FS.count();xx++)                             {
    int fi = FS [ xx ]                                          ;
    objects[0] . faces [ fi ] . pointColor = true               ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  float al    = 120.0                                           ;
  float bx    = 45.0                                            ;
  int   index                                                   ;
  ControlPoint X1 ( -bx, 0.0, 0.0 )                            ;
  ControlPoint X2 (  al, 0.0, 0.0 )                            ;
  ControlPoint Y1 ( 0.0, -bx, 0.0 )                            ;
  ControlPoint Y2 ( 0.0,  al, 0.0 )                            ;
  ControlPoint Z1 ( 0.0, 0.0, -bx )                            ;
  ControlPoint Z2 ( 0.0, 0.0,  al )                            ;
  ControlPoint C0 ( 1.0, 0.0 , 0.0 , 1.0 )                     ;
  ControlPoint C1 ( 0.0, 1.0 , 0.0 , 1.0 )                     ;
  ControlPoint C2 ( 0.0, 0.0 , 1.0 , 1.0 )                     ;
  SceneSegment F                                               ;
  objects[1].segments[0] = F                                    ;
  objects[1].segments[1] = F                                    ;
  objects[1].segments[2] = F                                    ;
  objects[1].addColor ( 0 , C0 )                                ;
  objects[1].addColor ( 1 , C1 )                                ;
  objects[1].addColor ( 2 , C2 )                                ;
  ///////////////////////////////////////////////////////////////
  index      = objects  [ 1 ] . Last()                          ;
  ///////////////////////////////////////////////////////////////
  objects[1] . segments [ 0 ] . type = SceneSegment::Lines     ;
  objects[1] . segments [ 0 ] . pointColor = true               ;
  objects[1] . segments [ 0 ] . lineWidth  = 2.5                ;
  objects[1] . segments [ 0 ] . colors [index] = 0              ;
  index      = objects  [ 1 ] . addPoint ( index , X1 )         ;
  objects[1] . segments [ 0 ] . add ( index - 1 )               ;
  index      = objects  [ 1 ] . addPoint ( index , X2 )         ;
  objects[1] . segments [ 0 ] . add ( index - 1 )               ;
  objects[1] . segments [ 1 ] . type = SceneSegment::Lines     ;
  objects[1] . segments [ 1 ] . pointColor = true               ;
  objects[1] . segments [ 1 ] . lineWidth  = 2.5                ;
  objects[1] . segments [ 1 ] . colors [index] = 1              ;
  index      = objects  [ 1 ] . addPoint ( index , Y1 )         ;
  objects[1] . segments [ 1 ] . add ( index - 1 )               ;
  index      = objects  [ 1 ] . addPoint ( index , Y2 )         ;
  objects[1] . segments [ 1 ] . add ( index - 1 )               ;
  objects[1] . segments [ 2 ] . type = SceneSegment::Lines     ;
  objects[1] . segments [ 2 ] . pointColor = true               ;
  objects[1] . segments [ 2 ] . lineWidth  = 2.5                ;
  objects[1] . segments [ 2 ] . colors [index] = 2              ;
  index      = objects  [ 1 ] . addPoint ( index , Z1 )         ;
  objects[1] . segments [ 2 ] . add ( index - 1 )               ;
  index      = objects  [ 1 ] . addPoint ( index , Z2 )         ;
  objects[1] . segments [ 2 ] . add ( index - 1 )               ;
  ///////////////////////////////////////////////////////////////
  rotates[0] = ControlPoint(1.0,0.0,0.0,270.0) ;
  rotates[1] = ControlPoint(0.0,0.0,1.0,270.0) ;
}

void N::CameraView::initializeGL(void)
{
  //////////////////////////////////////////
  GLint ilights = 0                        ;
  ::glGetIntegerv(GL_MAX_LIGHTS, &ilights) ;
  lights = ilights                         ;
  //////////////////////////////////////////
}

void N::CameraView::resizeGL(int w,int h)
{
  QRectF    W(0,0,w,h)        ;
  camera  = W                 ;
  camera  . setWindowPort ( ) ;
}

void N::CameraView::paintGL(void)
{
  root . setProperties (                  )      ;
  glShadeModel   ( GL_SMOOTH              )      ;
  glLineWidth    ( 1.0                    )      ;
  ////////////////////////////////////////////////
  glDepthFunc    ( GL_LESS                )      ;
  glBlendFunc    ( GL_SRC_ALPHA                  ,
                   GL_ONE_MINUS_SRC_ALPHA )      ;
  glMatrixMode   ( GL_PROJECTION          )      ;
  glLoadIdentity (                        )      ;
  glMatrixMode   ( GL_MODELVIEW           )      ;
  ////////////////////////////////////////////////
  glClear      (GL_COLOR_BUFFER_BIT              |
                GL_DEPTH_BUFFER_BIT            ) ;
  glClearColor ( 1.00f , 1.00f , 1.00f , 0.00f ) ;
  ////////////////////////////////////////////////
  for (int i=0;i<lightings.count();i++)          {
    lightings[i] . Place  ( )                    ;
    lightings[i] . Enable ( )                    ;
  }                                              ;
  glEnable    ( GL_LIGHTING )                    ;
  ////////////////////////////////////////////////
  ControlPoint Ambient  (1.0 , 1.0 , 1.0 , 1.0) ;
  ControlPoint Diffuse  (1.0 , 1.0 , 1.0 , 1.0) ;
  ControlPoint Specular (1.0 , 1.0 , 1.0 , 1.0) ;
  ControlPoint Shininess(60.0 , 0 , 0 , 0     ) ;
  ////////////////////////////////////////////////
  glPushMatrix            (                    ) ;
  camera . setCamera      (                    ) ;
  camera . GetMatrix      ( GL_PROJECTION      ) ;
  ////////////////////////////////////////////////
  ControlPoint  B ( 0.25 , 0.25 , 1.00 , 1.0 )  ;
  ControlPoint  Z ( 0.75 , 0.75 , 0.50 , 0.5 )  ;
  ControlPoint  E                               ;
  ControlPoint  C                               ;
  ControlPoint  U                               ;
  ControlPoint  A                               ;
  ControlPoint  X                               ;
  ControlPoint  Y                               ;
  ControlPoints P                               ;
  SceneSegment  S                               ;
  SceneFace     F                               ;
  double         len                             ;
  double         w = camera.viewport.width ()    ;
  double         h = camera.viewport.height()    ;
  w /= 2                                         ;
  h /= 2                                         ;
  E  = camera . eye                              ;
  C  = camera . center                           ;
  U  = camera . up                               ;
  A  = E                                         ;
  A -= C                                         ;
  len = A.length( )                              ;
  A  . normalize ( )                             ;
  U  . normalize ( )                             ;
  X  = crossProduct ( U , A )                    ;
  X  . normalize ( )                             ;
  Y  = crossProduct ( A , X )                    ;
  Y  . normalize ( )                             ;
  X *= w                                         ;
  Y *= h                                         ;
  P[0]  = C                                      ;
  P[1]  = C                                      ;
  P[2]  = C                                      ;
  P[3]  = C                                      ;
  P[0] += X                                      ;
  P[0] += Y                                      ;
  P[1] += X                                      ;
  P[1] -= Y                                      ;
  P[2] -= X                                      ;
  P[2] -= Y                                      ;
  P[3] -= X                                      ;
  P[3] += Y                                      ;
  objects[2] . clear  ( )                        ;
  objects[2] . addColor ( 1 , B    )             ;
  objects[2] . addColor ( 2 , Z    )             ;
  objects[2] . addPoint ( 1 , E    )             ;
  objects[2] . addPoint ( 2 , P[0] )             ;
  objects[2] . addPoint ( 3 , P[1] )             ;
  objects[2] . addPoint ( 4 , P[2] )             ;
  objects[2] . addPoint ( 5 , P[3] )             ;
  objects[2] . segments [1] = S                               ;
  objects[2] . segments [1] . type      = SceneSegment::Loop  ;
  objects[2] . segments [1] . pointColor = true               ;
  objects[2] . segments [1] . lineWidth = 2                   ;
  objects[2] . segments [1] . add ( 2 )                       ;
  objects[2] . segments [1] . add ( 3 )                       ;
  objects[2] . segments [1] . add ( 4 )                       ;
  objects[2] . segments [1] . add ( 5 )                       ;
  objects[2] . segments [1] . colors [ 2 ] = 1                ;
  /////////////////////////////////////////////////////////////
  objects[2] . segments [2] = S                               ;
  objects[2] . segments [2] . type      = SceneSegment::Lines ;
  objects[2] . segments [2] . pointColor = true               ;
  objects[2] . segments [2] . lineWidth = 2                   ;
  objects[2] . segments [2] . add ( 1 )                       ;
  objects[2] . segments [2] . add ( 2 )                       ;
  objects[2] . segments [2] . colors [ 1 ] = 1                ;
  /////////////////////////////////////////////////////////////
  objects[2] . segments [3] = S                               ;
  objects[2] . segments [3] . type      = SceneSegment::Lines ;
  objects[2] . segments [3] . pointColor = true               ;
  objects[2] . segments [3] . lineWidth = 2                   ;
  objects[2] . segments [3] . add ( 1 )                       ;
  objects[2] . segments [3] . add ( 3 )                       ;
  objects[2] . segments [3] . colors [ 1 ] = 1                ;
  /////////////////////////////////////////////////////////////
  objects[2] . segments [4] = S                               ;
  objects[2] . segments [4] . type      = SceneSegment::Lines ;
  objects[2] . segments [4] . pointColor = true               ;
  objects[2] . segments [4] . lineWidth = 2                   ;
  objects[2] . segments [4] . add ( 1 )                       ;
  objects[2] . segments [4] . add ( 4 )                       ;
  objects[2] . segments [4] . colors [ 1 ] = 1                ;
  /////////////////////////////////////////////////////////////
  objects[2] . segments [5] = S                               ;
  objects[2] . segments [5] . type      = SceneSegment::Lines ;
  objects[2] . segments [5] . pointColor = true               ;
  objects[2] . segments [5] . lineWidth = 2                   ;
  objects[2] . segments [5] . add ( 1 )                       ;
  objects[2] . segments [5] . add ( 5 )                       ;
  objects[2] . segments [5] . colors [ 1 ] = 1                ;
  /////////////////////////////////////////////////////////////
  objects[2] . faces    [1] = F                               ;
  objects[2] . faces    [1] . type = SceneFace::Quad          ;
  objects[2] . faces    [1] . pointColor = true               ;
  objects[2] . faces    [1] . add ( 2 )                       ;
  objects[2] . faces    [1] . add ( 3 )                       ;
  objects[2] . faces    [1] . add ( 4 )                       ;
  objects[2] . faces    [1] . add ( 5 )                       ;
  objects[2] . faces    [1] . colors [ 2 ] = 2                ;
  /////////////////////////////////////////////////////////////
  Ambient   . Material (GL_FRONT , GL_AMBIENT  ) ;
  Diffuse   . Material (GL_FRONT , GL_DIFFUSE  ) ;
  Specular  . Material (GL_FRONT , GL_SPECULAR ) ;
  Shininess . Material (GL_FRONT , GL_SHININESS) ;
  ////////////////////////////////////////////////
  for (int i=0;i<rotates.count();i++)            {
    rotates[i].Rotate ( )                        ;
  }                                              ;
  objects[2] . render ( )                        ;
  objects[1] . render ( )                        ;
  objects[0] . render ( )                        ;
  ////////////////////////////////////////////////
  glPopMatrix ( )                                ;
  glFlush     ( )                                ;
}
