#include <qtav.h>

N::CameraBox:: CameraBox    ( QObject * parent )
             : QObject      (           parent )
             , Universe     (                  )
             , UsingCamera  ( -1               )
             , Modification ( false            )
             , MovingFactor ( 2.0              )
{
  name = "Camera" ;
}

N::CameraBox::~CameraBox (void)
{
}

void N::CameraBox::setLength(double length,QFont & f)
{
  Camera * C = new Camera ( )                                                ;
  C-> destructor = destructor                                                ;
  C-> mode       = Camera::Floating                                          ;
  C-> FOV        = 45                                                        ;
  C-> eye        = ControlPoint (400.0,400.0,800.0)                          ;
  C-> center     = ControlPoint (  0.0,  0.0,  0.0)                          ;
  C-> up         = ControlPoint (  0.0,  1.0,  0.0)                          ;
  C-> setNearFar            (    1, 10000     )                              ;
  C-> references[Camera::ReferenceCorner] = ControlPoint(-length,-length,0)  ;
  C-> references[Camera::ReferenceSize  ] = ControlPoint( length, length,0)  ;
  C-> minimum.x =   0                                                        ;
  C-> minimum.y =   0                                                        ;
  C-> maximum.x = length                                                     ;
  C-> maximum.y = length                                                     ;
  C-> operating [Camera::AlignRightBottom] = true                            ;
  addCamera        ( 0 , C )                                                 ;
  setCurrentCamera ( 0     )                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  SceneObject * box = new SceneObject ( destructor )                         ;
  SceneFace     F                                                            ;
  ControlPoint  s(0,0,0)                                                     ;
  ControlPoint  c                                                            ;
  for (int i=0;i<8;i++) box -> addPoint ( i , c )                            ;
  s.DOF = ControlPoint::DofTranslate                                         ;
  c     = QColor(224,224,224)                                                ;
  box->colors[0] = c                                                         ;
  box->addDof      ( 0 , s     )                                             ;
  box->appendFaces ( 0 , 6 , F )                                             ;
  JoinDestructor   ( box       )                                             ;
  for (int i=0;i<6;i++)                                                      {
    Textures[i] = new SceneTexture ( destructor )                            ;
    box->textures[i] = Textures[i]                                           ;
    box->faces   [i] . textureId = i                                         ;
    Textures[i]->createImage(QSize(length,length))                           ;
    Textures[i]->setImageColor(QColor(192,192,192,96))                       ;
    JoinDestructor ( Textures[i] )                                           ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  setBox(box,length)                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  SceneSegment SS                                                            ;
  SS.pointColor = true                                                       ;
  box->appendSegments ( 0 , 6 , SS )                                         ;
  box->segments [ 0 ] . type = SceneSegment::Loop                            ;
  box->segments [ 0 ] . add ( 3 )                                            ;
  box->segments [ 0 ] . add ( 0 )                                            ;
  box->segments [ 0 ] . add ( 4 )                                            ;
  box->segments [ 0 ] . add ( 7 )                                            ;
  box->segments [ 1 ] . type = SceneSegment::Loop                            ;
  box->segments [ 1 ] . add ( 5 )                                            ;
  box->segments [ 1 ] . add ( 6 )                                            ;
  box->segments [ 1 ] . add ( 2 )                                            ;
  box->segments [ 1 ] . add ( 1 )                                            ;
  box->segments [ 2 ] . type = SceneSegment::Lines                           ;
  box->segments [ 2 ] . add ( 3 )                                            ;
  box->segments [ 2 ] . add ( 2 )                                            ;
  box->segments [ 3 ] . type = SceneSegment::Lines                           ;
  box->segments [ 3 ] . add ( 0 )                                            ;
  box->segments [ 3 ] . add ( 1 )                                            ;
  box->segments [ 4 ] . type = SceneSegment::Lines                           ;
  box->segments [ 4 ] . add ( 4 )                                            ;
  box->segments [ 4 ] . add ( 5 )                                            ;
  box->segments [ 5 ] . type = SceneSegment::Lines                           ;
  box->segments [ 5 ] . add ( 7 )                                            ;
  box->segments [ 5 ] . add ( 6 )                                            ;
  ////////////////////////////////////////////////////////////////////////////
  box->texCoords[0] = ControlPoint(1,0,0)                                    ;
  box->texCoords[1] = ControlPoint(1,1,0)                                    ;
  box->texCoords[2] = ControlPoint(0,1,0)                                    ;
  box->texCoords[3] = ControlPoint(0,0,0)                                    ;
  ////////////////////////////////////////////////////////////////////////////
  int faces[6][4] =                                                          {
    { 0 , 1 , 2 , 3 }                                                        ,
    { 3 , 2 , 6 , 7 }                                                        ,
    { 7 , 6 , 5 , 4 }                                                        ,
    { 4 , 5 , 1 , 0 }                                                        ,
    { 5 , 6 , 2 , 1 }                                                        ,
    { 7 , 4 , 0 , 3 }                                                      } ;
  for (int i=0;i<6;i++)                                                      {
    box->faces [ i ] . type = SceneFace::Quad                                ;
    for (int j=0;j<4;j++)                                                    {
      box->faces [ i ] . add ( faces[i][j] )                                 ;
      box->faces [ i ] . texCoords [ faces[i][j] ] = j                       ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  PaintBox ( length , f )                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  box->setProperty ( GL_CULL_FACE , false        )                           ;
  box->addSequence (0,SceneObject::RenderTextures)                           ;
  box->addSequence (1,SceneObject::AssignColor   )                           ;
  box->addSequence (2,SceneObject::RenderSegments)                           ;
  ////////////////////////////////////////////////////////////////////////////
  addActor         (0,box                        )                           ;
  Perform          (0,true                       )                           ;
}

void N::CameraBox::setBox(SceneObject * box,double length)
{
  double e = length / 2                      ;
  double v[8][3]                             ;
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -e ;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  e ;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -e ;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  e ;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -e ;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  e ;
  for (int i=0;i<8;i++)                      {
    ControlPoint p(v[i][0],v[i][1],v[i][2])  ;
    box->points[i] = p                       ;
  }                                          ;
}

void N::CameraBox::setBox(double length)
{
  if (!Actors.contains(0)) return         ;
  setBox((SceneObject *)Actors[0],length) ;
}

void N::CameraBox::assignCamera(int index)
{
  CurrentCamera->FOV       = Cameras[index]->FOV       ;
  CurrentCamera->eye       = Cameras[index]->eye       ;
  CurrentCamera->center    = Cameras[index]->center    ;
  CurrentCamera->up        = Cameras[index]->up        ;
  CurrentCamera->minimum.z = Cameras[index]->minimum.z ;
  CurrentCamera->maximum.z = Cameras[index]->maximum.z ;
  ControlPoint E = Cameras[0]->center                  ;
  Actors [0]->DOFs[0].x = E.x                          ;
  Actors [0]->DOFs[0].y = E.y                          ;
  Actors [0]->DOFs[0].z = E.z                          ;
  UsingCamera  = index                                 ;
  Modification = true                                  ;
}

void N::CameraBox::assignCamera(int index,Camera * camera)
{
  Cameras[index] = camera ;
  assignCamera(index)     ;
}

bool N::CameraBox::mouseDoubleClickEvent(QWidget * widget,QMouseEvent * event)
{
  return Universe::mouseDoubleClickEvent(widget,event) ;
}

bool N::CameraBox::mousePressEvent(QWidget * widget,QMouseEvent * event)
{
  Universe::mousePressEvent(widget,event)                 ;
  nKickOut ( UsingCamera < 0 , false )                    ;
  Activations[1001] = false                               ;
  nKickOut ( !isActivated(MouseLeft) , false )            ;
  assignCamera ( UsingCamera )                            ;
  /////////////////////////////////////////////////////////
  Camera * C = CurrentCamera                              ;
  QRectF    area                                          ;
  QPointF   portPos                                       ;
  if (!C->ParkingArea(area)                ) return false ;
  if (!C->atPort(area,event->pos(),portPos)) return false ;
  Activations[1001] = true                                ;
  return true                                             ;
}

bool N::CameraBox::mouseMoveEvent(QWidget * widget,QMouseEvent * event)
{
  Universe::mouseMoveEvent(widget,event)    ;
  if (!Activations[1001]     ) return false ;
  if (!isActivated(MouseLeft)) return false ;
  QPointF difference                        ;
  if (!Moving(difference)    ) return false ;
  Preview ( difference )                    ;
  return true                               ;
}

bool N::CameraBox::mouseReleaseEvent(QWidget * widget,QMouseEvent * event)
{
  if (Activations[1001])                              {
    QPointF difference                                ;
    Activations[1001] = false                         ;
    Universe::mouseReleaseEvent(widget,event)         ;
    if (!Moving(difference))                          {
      assignCamera ( UsingCamera )                    ;
      return false                                    ;
    }                                                 ;
    Apply        ( difference  )                      ;
    assignCamera ( UsingCamera )                      ;
  } else                                              {
    return Universe::mouseReleaseEvent(widget,event)  ;
  }                                                   ;
  return true                                         ;
}

bool N::CameraBox::wheelEvent(QWidget * widget,QWheelEvent * event)
{
  if (!Activations[1001]     ) return false      ;
  if (!isActivated(MouseLeft)) return false      ;
  if (UsingCamera < 0        ) return false      ;
  ////////////////////////////////////////////////
  double factor = 1.0 / 24.0                     ;
  double length                                  ;
  ControlPoint E                                 ;
  E       = CurrentCamera->eye                   ;
  E      -= CurrentCamera->center                ;
  length  = E.length()                           ;
  factor *= event->delta()                       ;
  length -= factor                               ;
  if (length<1.0) return false                   ;
  E  . normalize ( )                             ;
  E *= length                                    ;
  E += CurrentCamera->center                     ;
  CurrentCamera->eye = E                         ;
  ////////////////////////////////////////////////
  Cameras[UsingCamera]->eye = CurrentCamera->eye ;
  Cameras[UsingCamera]->up  = CurrentCamera->up  ;
  ////////////////////////////////////////////////
  assignCamera ( UsingCamera )                   ;
  ////////////////////////////////////////////////
  QPointF PX = event->pos()                      ;
  ControlPoint p                                 ;
  p = PX                                         ;
  Users[MousePress] = p                          ;
  return true                                    ;
}

bool N::CameraBox::Moving(QPointF & difference)
{
  ControlPoint startp = Users[MousePress]       ;
  ControlPoint cursor = Users[MouseMove ]       ;
  QPoint        P (startp.x,startp.y)           ;
  QPoint        N (cursor.x,cursor.y)           ;
  Camera      * C = CurrentCamera               ;
  QRectF        area                            ;
  QPointF       startPos                        ;
  QPointF       portPos                         ;
  if (!C->ParkingArea(area)      ) return false ;
  if (!C->atPort(area,P,startPos)) return false ;
  if (!C->atPort(area,N,portPos )) return false ;
  difference  = portPos                         ;
  difference -= startPos                        ;
  return true                                   ;
}

void N::CameraBox::Apply(QPointF & difference)
{
  if (UsingCamera<=0) return                     ;
  Preview ( difference )                         ;
  Cameras[UsingCamera]->eye = CurrentCamera->eye ;
  Cameras[UsingCamera]->up  = CurrentCamera->up  ;
}

void N::CameraBox::Preview(QPointF & difference)
{
  if (UsingCamera<=0) return                               ;
  ControlPoint X                                           ;
  ControlPoint Y                                           ;
  ControlPoint Z                                           ;
  ControlPoint N                                           ;
  double       L                                           ;
  double       f                                           ;
  //////////////////////////////////////////////////////////
  Z  = Cameras[UsingCamera]->eye                           ;
  Z -= Cameras[UsingCamera]->center                        ;
  Y  = Cameras[UsingCamera]->up                            ;
  f  = Cameras[UsingCamera]->viewport.height()             ;
  f /= CurrentCamera->references[Camera::ReferenceSize].y  ;
  f *= MovingFactor                                        ;
  //////////////////////////////////////////////////////////
  L  = Z.length  ( )                                       ;
  N  = Z                                                   ;
  Z  . normalize ( )                                       ;
  Y  . normalize ( )                                       ;
  X  = crossProduct ( Y , Z )                              ;
  X  . normalize ( )                                       ;
  Y  = crossProduct ( Z , X )                              ;
  Y  . normalize ( )                                       ;
  CurrentCamera->up = Y                                    ;
  X *= difference.x() * f                                  ;
  Y *= difference.y() * f                                  ;
  N -= X                                                   ;
  N -= Y                                                   ;
  N  . normalize ( )                                       ;
  N *= L                                                   ;
  N += Cameras[UsingCamera]->center                        ;
  //////////////////////////////////////////////////////////
  CurrentCamera->eye = N                                   ;
  //////////////////////////////////////////////////////////
  double fov = CurrentCamera->FOV / 2                      ;
  double cos = Math::fastCosine (fov)                      ;
  double sin = Math::fastSine   (fov)                      ;
  L /= cos                                                 ;
  L *= sin                                                 ;
  setBox(L)                                                ;
  //////////////////////////////////////////////////////////
  Modification = true                                      ;
}

void N::CameraBox::PaintBox(double e,QFont & f)
{
  QRectF  E  (  0,0,e,  e)                          ;
  QColor  C1 (255,0,0,255)                          ;
  QColor  C2 (0,255,0,255)                          ;
  QColor  C3 (255,0,0,255)                          ;
  QColor  C4 (0,255,0,255)                          ;
  QColor  C5 (0,0,255,255)                          ;
  QColor  C6 (0,0,255,255)                          ;
  QString S1 = QObject::tr("X/L")                   ;
  QString S2 = QObject::tr("Y/U")                   ;
  QString S3 = QObject::tr("X/R")                   ;
  QString S4 = QObject::tr("Y/D")                   ;
  QString S5 = QObject::tr("Z/F")                   ;
  QString S6 = QObject::tr("Z/B")                   ;
  Textures[XL]->drawText(C1,f,E,Qt::AlignCenter,S1) ;
  Textures[YU]->drawText(C2,f,E,Qt::AlignCenter,S2) ;
  Textures[XR]->drawText(C3,f,E,Qt::AlignCenter,S3) ;
  Textures[YD]->drawText(C4,f,E,Qt::AlignCenter,S4) ;
  Textures[ZF]->drawText(C5,f,E,Qt::AlignCenter,S5) ;
  Textures[ZB]->drawText(C6,f,E,Qt::AlignCenter,S6) ;
}

void N::CameraBox::setImageColor(QColor color)
{
  for (int i=0;i<6;i++)               {
    Textures[i]->setImageColor(color) ;
  }                                   ;
}

bool N::CameraBox::Animation(void)
{
  bool mox = Modification ;
  Modification = false    ;
  return mox              ;
}

bool N::CameraBox::Menu(QWidget * widget,QPointF pos)
{
  Camera * C = CurrentCamera                     ;
  nKickOut ( IsNull(C) , false )                 ;
  QPoint  wos (pos.x(),pos.y())                  ;
  QRectF  area                                   ;
  QPointF portPos                                ;
  if (!C->ParkingArea(area)       ) return false ;
  if (!C->atPort(area,wos,portPos)) return false ;
  nScopedMenu ( mm , widget )                    ;
  QAction    * aa                                ;
  mm.add(101,tr("Remove camera controller"))     ;
  if (NotNull(qPlan))                            {
    mm.setFont(AppPlan)                          ;
  }                                              ;
  aa = mm.exec()                                 ;
  if (IsNull(aa)) return true                    ;
  switch (mm[aa])                                {
    case 101                                     :
      emit Close(this)                           ;
    break                                        ;
  }                                              ;
  return true                                    ;
}
