#include <qtav.h>

N::VideoUniverse:: VideoUniverse  (QObject * parent)
                 : QObject        (          parent)
                 , RasterUniverse (                )
{
  name = "Video"                           ;
  setFunction ( FollowWindowSize , true  ) ;
  setFunction ( WindowMovable    , false ) ;
  setFunction ( HasUniverseMenu  , true  ) ;
  setFunction ( ControlPanel     , false ) ;
  setFunction ( FixedPosition    , false ) ;
  setFunction ( AudioAmplitude   , false ) ;
  setFunction ( RasterSizing     , true  ) ;
  setActivate ( MouseDClick      , true  ) ;
}

N::VideoUniverse:: VideoUniverse  (QObject * parent,SceneDestructor * des)
                 : QObject        (          parent                      )
                 , RasterUniverse (                                      )
{
  destructor = des                         ;
  JoinDestructor ( )                       ;
  name       = "Video"                     ;
  setFunction ( FollowWindowSize , true  ) ;
  setFunction ( WindowMovable    , false ) ;
  setFunction ( HasUniverseMenu  , true  ) ;
  setFunction ( ControlPanel     , false ) ;
  setFunction ( FixedPosition    , false ) ;
  setFunction ( AudioAmplitude   , false ) ;
  setFunction ( RasterSizing     , true  ) ;
  setActivate ( MouseDClick      , true  ) ;
}

N::VideoUniverse::~VideoUniverse(void)
{
}

bool N::VideoUniverse::mouseDoubleClickEvent(QWidget * w,QMouseEvent * e)
{
  Universe::mouseDoubleClickEvent ( w , e       ) ;
  bool dbc = isActivated          ( MouseDClick ) ;
  if ( dbc ) emit DoubleClicked   ( this        ) ;
  return dbc                                      ;
}

bool N::VideoUniverse::Demolish(void)
{
  emit Finished ( this ) ;
  return true            ;
}

bool N::VideoUniverse::Sync(void * data)
{
  if (IsNull(data)) return false           ;
  VideoChannel * vc = (VideoChannel *)data ;
  //////////////////////////////////////////
  // Do nothing for now
              Q_UNUSED ( vc ) ;
  //////////////////////////////////////////
  return true                              ;
}

bool N::VideoUniverse::setTransmission(VideoTransmission * transmission)
{
  if ( IsNull(transmission) )                       {
    return false                                    ;
  }                                                 ;
  if ( !Actors.contains(limits[RasterObjectId]) )   {
    return false                                    ;
  }                                                 ;
  SceneObject * P                                   ;
  P = (SceneObject *)Actors[limits[RasterObjectId]] ;
  setDimension                                      (
    transmission -> Dimension                       ,
    transmission -> ThrottleSize                    ,
    transmission -> Format                        ) ;
  transmission   -> SituationChanged                (
    transmission -> Dimension                       ,
    P            -> images                          ,
    P            -> pictures                      ) ;
  return true                                       ;
}

bool N::VideoUniverse::contains(QPointF pos)
{
  bool    z = false                                                      ;
  QPointF p = ScreenRange.center()                                       ;
  QSizeF  r = Rectangle                                                  ;
  QRectF  v                                                              ;
  ////////////////////////////////////////////////////////////////////////
  if ( isFunction ( FixedPosition ) )                                    {
    if ( Users.contains(75001)                                          &&
         Users.contains(75002)                                          &&
         Users.contains(75003)                                         ) {
      z = true                                                           ;
    }                                                                    ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  if ( isFunction ( FixedPosition ) )                                    {
    ControlPoint Pos = Users[75001]                                      ;
    p  = ScreenRange.center( )                                           ;
    v  . setLeft (  Pos.x    )                                           ;
    v  . setTop  ( -Pos.y    )                                           ;
    v  . setSize ( Rectangle )                                           ;
    p  = pos - p                                                         ;
  } else                                                                 {
    p  = ScreenRange.center()                                            ;
    r  = Rectangle                                                       ;
    r /= 2                                                               ;
    v  . setLeft ( - r.width () )                                        ;
    v  . setTop  ( - r.height() )                                        ;
    v  . setSize ( Rectangle    )                                        ;
    p  = pos - p                                                         ;
  }                                                                      ;
  return v . contains ( p )                                              ;
}

void N::VideoUniverse::Relocation(QRectF viewport)
{
  Universe::Relocation(viewport)                                         ;
  nDropOut ( !Actors.contains(limits[RasterObjectId]) )                  ;
  SceneObject * P = (SceneObject *)Actors[limits[RasterObjectId]]        ;
  double vw = viewport.width () - 2                                      ;
  double vh = viewport.height() - 2                                      ;
  double z  = 0                                                          ;
  if ( isFunction ( FixedPosition ) )                                    {
    if ( Users.contains(75001)                                          &&
         Users.contains(75002)                                          &&
         Users.contains(75003)                                         ) {
      ControlPoint Pos  = Users[75001]                                   ;
      ControlPoint Zoom = Users[75002]                                   ;
      ControlPoint Size = Users[75003]                                   ;
      Users [ RasterSizing  ] = Pos                                      ;
      Users [ RasterZooming ] = Zoom                                     ;
      for (int i=0;i<P->textures.count();i++)                            {
        SceneTexture * T  = (SceneTexture *) P -> textures [ i ]         ;
        T -> rasterizers[SceneTexture::RasterPos ] = Pos                 ;
        T -> rasterizers[SceneTexture::RasterZoom] = Zoom                ;
        Rectangle . setWidth  ( Size.x )                                 ;
        Rectangle . setHeight ( Size.y )                                 ;
      }                                                                  ;
    }                                                                    ;
  } else                                                                 {
    if ( Users . contains ( 75000 ) ) z = Users [ 75000 ] . z            ;
    for (int i=0;i<P->textures.count();i++)                              {
      ControlPoint   S ( 1 , -1 , 0 , 1 )                                ;
      SceneTexture * T  = (SceneTexture *) P -> textures [ i ]           ;
      QImage       * I  = T -> original                                  ;
      double         w  = I -> width  ( )                                ;
      double         h  = I -> height ( )                                ;
      double         fw = vw / w                                         ;
      double         fh = vh / h                                         ;
      double         f  = fw                                             ;
      h   =  h / 2                                                       ;
      w   = -w / 2                                                       ;
      if ( isFunction ( FollowWindowSize )  )                            {
        if ( Activations [ RasterScaling ] )                             {
          if ( f > fh ) f = fh                                           ;
          h  *= f                                                        ;
          w  *= f                                                        ;
          S   = ControlPoint(f,-f,0,1)                                   ;
        }                                                                ;
      } else f = 1.0                                                     ;
      ControlPoint RP ( w , h , z , 1 )                                  ;
      T -> rasterizers [ SceneTexture::RasterPos  ] = RP                 ;
      T -> rasterizers [ SceneTexture::RasterZoom ] = S                  ;
      Users            [ RasterSizing             ] = RP                 ;
      Users            [ RasterZooming            ] = S                  ;
      w  = I -> width  ( )                                               ;
      h  = I -> height ( )                                               ;
      Rectangle . setWidth  ( f * w )                                    ;
      Rectangle . setHeight ( f * h )                                    ;
    }                                                                    ;
  }                                                                      ;
  ScreenRange = viewport                                                 ;
  ScreenRange . setLeft ( 0 )                                            ;
  ScreenRange . setTop  ( 0 )                                            ;
}

bool N::VideoUniverse::Menu(QWidget * widget,QPointF pos)
{
  nKickOut ( ! contains ( pos ) , false )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager   mm ( widget )                                                ;
  QMenu       * mi                                                           ;
  QMenu       * mc                                                           ;
  QMenu       * ma                                                           ;
  QMenu       * mv                                                           ;
  QMenu       * mp                                                           ;
  QAction     * aa                                                           ;
  SceneObject * PP = (SceneObject *) Actors [ limits [ RasterObjectId ] ]    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( name . length ( ) > 0 )                                               {
    aa  = mm . add     ( 0 , name )                                          ;
    aa -> setEnabled   ( false    )                                          ;
    mm  . add ( 101 , tr("Panel") , true , isFunction(ControlPanel) )        ;
  } else                                                                     {
    mm  . add ( 101 , tr("Panel") , true , isFunction(ControlPanel) )        ;
  }                                                                          ;
  mm  . addSeparator (                 )                                     ;
  ////////////////////////////////////////////////////////////////////////////
  mi  = mm . addMenu ( tr("Interface") )                                     ;
  ma  = mm . addMenu ( tr("Audio"    ) )                                     ;
  mv  = mm . addMenu ( tr("Video"    ) )                                     ;
  mc  = mm . addMenu ( tr("Control"  ) )                                     ;
  mp  = mm . addMenu ( tr("Paraments") )                                     ;
  ////////////////////////////////////////////////////////////////////////////
  mm  . add ( mi,102,tr("Full window"   ),true,isFunction(FollowWindowSize) );
  mm  . add ( mi,104,tr("Window movable"),true,isFunction(WindowMovable   ) );
  if ( isFunction ( HasUniverseMenu ) )                                      {
    mm . add ( mi,105,tr("Disable menu"    )                               ) ;
  }                                                                          ;
  aa  = mm . add ( ma                                                        ,
                   201                                                       ,
                   tr("Audio amplitude")                                     ,
                   true                                                      ,
                   isFunction ( AudioAmplitude )                           ) ;
  aa  = mm . add (mv,301,tr("Stop"  ))                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! isFunction ( FollowWindowSize )  )                                  {
    mm . add ( mc , 401 , tr("Display ratio") )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( PP -> textures . count ( ) > 0 )                                      {
    SceneTexture * T  = (SceneTexture *) PP -> textures [ 0 ]                ;
    QImage       * I  = T -> original                                        ;
    int            w  = I -> width  ( )                                      ;
    int            h  = I -> height ( )                                      ;
    QSizeF         RR = Rectangle                                            ;
    double         ww = RR . width  ( )                                      ;
    double         hh = RR . height ( )                                      ;
    QString        ms                                                        ;
    QString        ss                                                        ;
    ms  = tr("%1 x %2") . arg ( w  ) . arg ( h  )                            ;
    ss  = tr("%1 x %2") . arg ( ww ) . arg ( hh )                            ;
    aa  = mm . add     ( mp , 901 , ms )                                     ;
    aa -> setEnabled   ( false         )                                     ;
    aa  = mm . add     ( mp , 902 , ss )                                     ;
    aa -> setEnabled   ( false         )                                     ;
  }                                                                          ;
  if (NotNull(qPlan)) mm . setFont ( AppPlan )                               ;
  ////////////////////////////////////////////////////////////////////////////
  aa  = mm.exec ( )                                                          ;
  nKickOut ( IsNull(aa) , true )                                             ;
  switch (mm[aa])                                                            {
    case 101                                                                 :
      setFunction ( ControlPanel , aa->isChecked() )                         ;
      emit RequestPanel ( this , isFunction ( ControlPanel ) )               ;
    break                                                                    ;
    case 102                                                                 :
      setFunction ( FollowWindowSize , aa->isChecked() )                     ;
      if ( isFunction ( FollowWindowSize ) )                                 {
        setFunction ( WindowMovable , false )                                ;
      }                                                                      ;
      emit Movable ( this , isFunction ( WindowMovable ) )                   ;
    break                                                                    ;
    case 104                                                                 :
      setFunction ( WindowMovable , aa->isChecked() )                        ;
      if ( isFunction ( WindowMovable ) )                                    {
        setFunction ( FollowWindowSize , false )                             ;
      }                                                                      ;
      emit Movable      ( this , isFunction ( WindowMovable ) )              ;
    break                                                                    ;
    case 105                                                                 :
      emit DisableMenu  (      )                                             ;
    break                                                                    ;
    case 201                                                                 :
      emit RequestAudio ( this , aa->isChecked() )                           ;
    break                                                                    ;
    case 301                                                                 :
      emit VideoStop    ( this )                                             ;
    break                                                                    ;
    case 401                                                                 :
      emit DisplayRatio ( this )                                             ;
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}
