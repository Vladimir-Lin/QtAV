#include <qtav.h>

#ifdef QT_OPENGL_LIB

N::EarIn:: EarIn              ( QObject * parent , Plan * p )
         : SceneWidget        (           parent ,        p )
         , CiosAudio::Conduit (                             )
         , UsingCamera        ( NULL                        )
{
  Configure ( ) ;
}

N::EarIn:: EarIn              ( QObject * parent , Plan * p , SceneDestructor * des )
         : SceneWidget        (           parent ,        p ,                   des )
         , CiosAudio::Conduit (                                                     )
         , UsingCamera        ( NULL                                                )
{
  Configure ( ) ;
}

N::EarIn::~EarIn (void)
{
}

void N::EarIn::Configure(void)
{
  ControlPoint color                                               ;
  //////////////////////////////////////////////////////////////////
  color  = QColor       ( 192 ,  192 , 192 , 192    )              ;
  addColor              (    0 , color              )              ;
  color  = ControlPoint ( 0.00 , 0.00 , 1.00 , 0.15 )              ;
  addColor              (    1 , color              )              ;
  color  = ControlPoint ( 0.75 , 0.75 , 1.00 , 0.05 )              ;
  addColor              (    2 , color              )              ;
  color  = ControlPoint ( 0.20 , 0.20 , 1.00 , 0.25 )              ;
  addColor              (    3 , color              )              ;
  color  = ControlPoint ( 0.50 , 1.00 , 0.50 , 0.25 )              ;
  addColor              (    4 , color              )              ;
  color  = ControlPoint ( 0.25 , 0.50 , 1.00 , 0.20 )              ;
  addColor              (    5 , color              )              ;
  color  = ControlPoint ( 1.00 , 0.20 , 0.20 , 0.20 )              ;
  addColor              (    6 , color              )              ;
  color  = ControlPoint ( 1.00 , 0.20 , 0.20 , 0.40 )              ;
  addColor              (    7 , color              )              ;
  color  = ControlPoint ( 0.25 , 1.00 , 0.25 , 0.35 )              ;
  addColor              (    8 , color              )              ;
  color  = ControlPoint ( 0.00 , 0.00 , 0.00 , 0.30 )              ;
  addColor              (    9 , color              )              ;
  //////////////////////////////////////////////////////////////////
  QObject::setProperty ( "Type"         , "EarIn" )                ;
  setVariable          ( "Type"         , "EarIn" )                ;
  setVariable          ( "Focus"        , false   )                ;
  setVariable          ( "Hold"         , false   )                ;
  setVariable          ( "Menu"         , true    )                ;
  setVariable          ( "Mouse"        , true    )                ;
  setVariable          ( "Hover"        , true    )                ;
  setVariable          ( "Emit"         , true    )                ;
  setVariable          ( "Finished"     , false   )                ;
  //////////////////////////////////////////////////////////////////
  setVariable          ( "Selectable"   , false   )                ;
  setVariable          ( "Movable"      , true    )                ;
  setVariable          ( "Resizable"    , true    )                ;
  setVariable          ( "Margin"       , 3       )                ;
  //////////////////////////////////////////////////////////////////
  setVariable          ( "Top/Line"     , true    )                ;
  setVariable          ( "Bottom/Line"  , true    )                ;
  setVariable          ( "Left/Line"    , true    )                ;
  setVariable          ( "Right/Line"   , true    )                ;
  setVariable          ( "Top/Left"     , true    )                ;
  setVariable          ( "Top/Right"    , true    )                ;
  setVariable          ( "Bottom/Left"  , true    )                ;
  setVariable          ( "Bottom/Right" , true    )                ;
  //////////////////////////////////////////////////////////////////
  setVariable          ( "Playing"      , false   )                ;
  setVariable          ( "Saving"       , false   )                ;
  setVariable          ( "DrawBorder"   , true    )                ;
  setVariable          ( "DrawPlane"    , true    )                ;
  //////////////////////////////////////////////////////////////////
  setVariable          ( "BorderColor"  , 5       )                ;
  setVariable          ( "FocusColor"   , 7       )                ;
  setVariable          ( "PlaneColor"   , 2       )                ;
  setVariable          ( "WaveColor"    , 3       )                ;
  //////////////////////////////////////////////////////////////////
  int did = plan -> EquipmentIDs [ Plan::DefaultAudioInput ]       ;
  int cid = (int) AV_CODEC_ID_MP3                                  ;
  setVariable          ( "ShowLength"   , 1000    )                ;
  setVariable          ( "BufferLength" , 10000   )                ;
  setVariable          ( "Amplitude"    , 1.000   )                ;
  setVariable          ( "Debug"        , false   )                ;
  setVariable          ( "Rate"         , 48000   )                ;
  setVariable          ( "DeviceId"     , did     )                ;
  setVariable          ( "Codec"        , cid     )                ;
  setVariable          ( "SaveTo"       , ""      )                ;
  //////////////////////////////////////////////////////////////////
  InputBuffer . resize ( 96000                    )                ;
  InputBuffer . fill   ( 0                        )                ;
}

void N::EarIn::render(Camera & camera,QMap<int,Matrix> & matrices)
{
  if ( isLocked ( "Operate" ) ) return       ;
  LockAt               ( "Operate"         ) ;
  UsingCamera = &camera                      ;
  UsingMatrix = matrices                     ;
  UnlockAt             ( "Operate"         ) ;
  SceneObject::render  ( camera , matrices ) ;
}

void N::EarIn::renderSegments(void)
{
  qint64 shown        = Value ( "ShowLength"   ) . toLongLong ( ) ;
  qint64 ibs          = InputBuffer . size ( )                    ;
  qint64 rendx        = 0                                         ;
  qint64 total        = 0                                         ;
  qint64 i            = 0                                         ;
  qint64 j            = 0                                         ;
  qint64 n            = 0                                         ;
  qint64 d            = 0                                         ;
  qint64 SamplingRate = Value ( "Rate" ) . toLongLong ( )         ;
  /////////////////////////////////////////////////////////////////
  shown *= SamplingRate                                           ;
  shown /= 500                                                    ;
  total  = shown                                                  ;
  total /= 2                                                      ;
  if ( shown > ibs ) return                                       ;
  rendx  = total * sizeof(double) * 2                             ;
  if ( RenderBuffer . size ( ) < rendx )                          {
    RenderBuffer . resize ( rendx )                               ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  short  * b = (short  *) InputBuffer  . data ( )                 ;
  double * p = (double *) RenderBuffer . data ( )                 ;
  double   z = ( minimum . z + maximum . z ) / 2                  ;
  double   w = maximum . x - minimum . x                          ;
  double   h = maximum . y - minimum . y                          ;
  double   a = Value ( "Amplitude" ) .toDouble( )                 ;
  /////////////////////////////////////////////////////////////////
  i = 0                                                           ;
  j = 0                                                           ;
  n = 1                                                           ;
  d = total - 1                                                   ;
  while ( i < total )                                             {
    p [ j ]  = i                                                  ;
    p [ j ] /= d                                                  ;
    p [ j ] *= w                                                  ;
    p [ j ] += minimum . x                                        ;
    ///////////////////////////////////////////////////////////////
    p [ n ]  = b [ i ]                                            ;
    p [ n ] *= a                                                  ;
    p [ n ] += 32768.0                                            ;
    p [ n ] /= 65535.0                                            ;
    p [ n ] *= h                                                  ;
    p [ n ] += minimum . y                                        ;
    ///////////////////////////////////////////////////////////////
    i++                                                           ;
    j++                                                           ;
    j++                                                           ;
    n++                                                           ;
    n++                                                           ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  QtGL::Enable     ( GL_LINE_SMOOTH        )                      ;
  QtGL::Enable     ( GL_POLYGON_SMOOTH     )                      ;
  QtGL::Disable    ( GL_CULL_FACE          )                      ;
  QtGL::Disable    ( GL_DEPTH_TEST         )                      ;
  QtGL::LineWidth  ( 0.25                  )                      ;
  /////////////////////////////////////////////////////////////////
  assumeColor      ( "WaveColor"           )                      ;
  QtGL::Strips     (                       )                      ;
  i = 0                                                           ;
  j = 0                                                           ;
  n = 1                                                           ;
  while ( i < total )                                             {
    QtGL::Vertex3d ( p [ j ] , p [ n ] , z )                      ;
    i++                                                           ;
    j++                                                           ;
    j++                                                           ;
    n++                                                           ;
    n++                                                           ;
  }                                                               ;
  QtGL::End        (                       )                      ;
}

void N::EarIn::renderBox(void)
{
  QtGL::PushMatrix  (                         ) ;
  if ( hasFunction  ( "DrawPlane"           ) ) {
    QtGL::Disable   ( GL_DEPTH_TEST           ) ;
    QtGL::Disable   ( GL_CULL_FACE            ) ;
    QtGL::Disable   ( GL_LINE_SMOOTH          ) ;
    QtGL::Disable   ( GL_POLYGON_SMOOTH       ) ;
    assumeColor     ( "PlaneColor"            ) ;
    CubeVertex      ( 106 , minimum , maximum ) ;
  }                                             ;
  if ( hasFunction  ( "DrawBorder"          ) ) {
    QtGL::Enable    ( GL_LINE_SMOOTH          ) ;
    QtGL::Enable    ( GL_POLYGON_SMOOTH       ) ;
    QtGL::Enable    ( GL_CULL_FACE            ) ;
    QtGL::Enable    ( GL_DEPTH_TEST           ) ;
    QtGL::LineWidth ( 1.5                     ) ;
    if ( isFocus   (                        ) ) {
      assumeColor  ( "FocusColor"             ) ;
    } else                                      {
      assumeColor  ( "BorderColor"            ) ;
    }                                           ;
    CubeVertex     ( 206 , minimum , maximum  ) ;
  }                                             ;
  QtGL::PopMatrix  (                          ) ;
}

bool N::EarIn::FocusIn(void)
{
  nKickOut   ( IsNull ( plan ) , false  ) ;
  LinkAction ( Start  , StartRecord ( ) ) ;
  LinkAction ( Stop   , StopRecord  ( ) ) ;
  LinkAction ( SaveAs , SaveAudio   ( ) ) ;
  return true                             ;
}

bool N::EarIn::FocusOut(void)
{
  return false ;
}

int N::EarIn::AtRegion(ControlPoint & At)
{
  return RectRegion ( At ) ;
}

bool N::EarIn::Spot        (
       ControlPoint & From ,
       ControlPoint & To   ,
       ControlPoint & At   )
{
  if (   Value ( "Finalize" ) . toBool ( ) ) return false ;
  if ( ! Value ( "Mouse"    ) . toBool ( ) ) return false ;
  return xymSpot ( From , To , At )                       ;
}

bool N::EarIn::LocalClicked    (
       ControlPoint & From     ,
       ControlPoint & To       ,
       ControlPoint & At       ,
       BMAPs        & buttons  ,
       bool           after    ,
       bool           overlap  )
{
  if ( ! after )                                 {
    if ( buttons [ Universe::MouseLeft   ] )     {
      if ( ! isFocus ( ) )                       {
        Alert ( Click )                          ;
      }                                          ;
      int region = AtRegion ( At )               ;
      if ( region <= 0 )                         {
        setFocus ( false )                       ;
      } else                                     {
        setFocus ( true  )                       ;
      }                                          ;
      setVariable  ( "Hit" , true    )           ;
    } else                                       {
      if ( Value ( "Hit" ) . toBool ( ) )        {
        setVariable      ( "Hit" , false   )     ;
      }                                          ;
    }                                            ;
    return true                                  ;
  }                                              ;
  ////////////////////////////////////////////////
  if ( ! overlap )                               {
    return true                                  ;
  }                                              ;
  ////////////////////////////////////////////////
  return true                                    ;
}

bool N::EarIn::LocalVector    (
       ControlPoint & From    ,
       ControlPoint & To      ,
       ControlPoint & At      ,
       BMAPs        & buttons ,
       bool           after   ,
       bool           overlap )
{
  if ( ! after )                                                 {
    //////////////////////////////////////////////////////////////
    if ( buttons [ Universe::MouseLeft ] )                       {
      if ( ! isFocus ( ) )                                       {
        Alert ( Click )                                          ;
      }                                                          ;
      int region = AtRegion ( At )                               ;
      if ( region <= 0 )                                         {
        setFocus ( false )                                       ;
      } else                                                     {
        setFocus ( true  )                                       ;
      }                                                          ;
    } else                                                       {
      if ( Value ( "Hit" ) . toBool ( ) )                        {
        return MouseEditing ( From , To , At , buttons , false ) ;
      }                                                          ;
    }                                                            ;
    //////////////////////////////////////////////////////////////
    return true                                                  ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  return MouseEditing ( From , To , At , buttons , false )       ;
}

bool N::EarIn::LocalHover     (
       ControlPoint & From    ,
       ControlPoint & To      ,
       ControlPoint & At      ,
       BMAPs        & buttons ,
       bool           after   ,
       bool           overlap )
{
  if ( ! after )                                              {
    QPointF AT  ( At . x , At . y   )                         ;
    setVariable ( "CurrentPos" , AT )                         ;
    return true                                               ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  if ( ! overlap )                                            {
    if ( MouseEditing ( From , To , At , buttons , true ) )   {
      return true                                             ;
    }                                                         ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  int region = AtRegion ( At )                                ;
  if ( region <= 0 )                                          {
    setCursor ( Qt::ArrowCursor )                             ;
    return false                                              ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  bool onlyMoving = true                                      ;
  if ( buttons [ Universe::MouseLeft   ] ) onlyMoving = false ;
  if ( buttons [ Universe::MouseMiddle ] ) onlyMoving = false ;
  if ( buttons [ Universe::MouseRight  ] ) onlyMoving = false ;
  if ( ! onlyMoving ) return true                             ;
  /////////////////////////////////////////////////////////////
  AssignCursor ( region )                                     ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::EarIn::MouseClicked   (
       ControlPoint & From    ,
       ControlPoint & To      ,
       BMAPs        & buttons )
{
  if ( hasFunction ( "Finalize" ) ) return false            ;
  return SceneObject::CursorClicked ( From , To , buttons ) ;
}

bool N::EarIn::MouseVector    (
       ControlPoint & From    ,
       ControlPoint & To      ,
       BMAPs        & buttons )
{
  if ( hasFunction ( "Finalize" ) ) return false           ;
  return SceneObject::CursorVector ( From , To , buttons ) ;
}

bool N::EarIn::MouseHover     (
       ControlPoint & From    ,
       ControlPoint & To      ,
       BMAPs        & buttons )
{
  if (   Value ( "Finalize" ) . toBool ( ) ) return false ;
  return SceneObject::CursorHover ( From , To , buttons ) ;
}

bool N::EarIn::MouseMove      (
       ControlPoint & From1   ,
       ControlPoint & To1     ,
       ControlPoint & From2   ,
       ControlPoint & To2     ,
       BMAPs        & buttons )
{
  return false ;
}

bool N::EarIn::prepare(void)
{
  Finish      (                     ) ;
  Dual        (                     ) ;
  addSequence (  9 , RenderSegments ) ;
  addSequence ( 17 , RenderBox      ) ;
  return true                         ;
}

void N::EarIn::run(int type,ThreadData * data)
{
  switch ( type )                         {
    case CiosMenuId ( 58 , 24 , 11      ) :
      Recording     ( data -> Arguments ) ;
    break                                 ;
    case CiosMenuId ( 58 , 24 , 12      ) :
      Saving        ( data -> Arguments ) ;
    break                                 ;
  }                                       ;
}

bool N::EarIn::startup(void)
{
  return true ;
}

int N::EarIn::obtain(void)
{
  return Conduit::Abort ;
}

int N::EarIn::put(void)
{
  if ( Input . Total ( ) <= 0 ) return Conduit::Continue            ;
  QByteArray B ( (const char *)Input . Buffer , Input . Total ( ) ) ;
  qint64     shown  = Value ( "ShowLength"   ) . toLongLong ( )     ;
  qint64     buffer = Value ( "BufferLength" ) . toLongLong ( )     ;
  qint64     ibs    = InputBuffer . size ( )                        ;
  qint64     dbs    = DataBuffer  . size ( )                        ;
  qint64     ris    = B           . size ( )                        ;
  qint64     irs    = 0                                             ;
  qint64     drs    = 0                                             ;
  qint64     SamplingRate = Value("Rate") . toLongLong ( )          ;
  ///////////////////////////////////////////////////////////////////
  if ( B     . size  ( ) <= 0 ) return Conduit::Continue            ;
  ///////////////////////////////////////////////////////////////////
  shown  *= SamplingRate                                            ;
  shown  /= 500                                                     ;
  buffer *= SamplingRate                                            ;
  buffer /= 500                                                     ;
  ///////////////////////////////////////////////////////////////////
  if ( ( ibs + ris ) > shown  ) irs = ( ibs + ris ) - shown         ;
  if ( ( dbs + ris ) > buffer ) drs = ( dbs + ris ) - buffer        ;
  ///////////////////////////////////////////////////////////////////
  LockAt     ( "Operate" )                                          ;
  InputBuffer . append ( B )                                        ;
  DataBuffer  . append ( B )                                        ;
  if ( irs > 0 ) InputBuffer . remove ( 0 , irs )                   ;
  if ( drs > 0 ) DataBuffer  . remove ( 0 , drs )                   ;
  UnlockAt   ( "Operate" )                                          ;
  Processing ( B         )                                          ;
  ///////////////////////////////////////////////////////////////////
  return Conduit::Continue                                          ;
}

void N::EarIn::finish(ConduitDirection direction,FinishCondition condition)
{
}

void N::EarIn::Processing(QByteArray & extras)
{
}

bool N::EarIn::KeepRecording(void)
{
  if ( hasFunction ( "Finalize" ) ) return false ;
  return hasFunction ( "Playing" )               ;
}

void N::EarIn::Recording(VarArgs & args)
{
  using namespace CiosAudio                                                  ;
  Core             core                                                      ;
  StreamParameters INSP                                                      ;
  CaError          rt                                                        ;
  Stream         * stream        = NULL                                      ;
  qint64           SamplingRate  = Value ( "Rate"     ) . toLongLong ( )     ;
  int              InputDeviceId = Value ( "DeviceId" ) . toInt      ( )     ;
  ////////////////////////////////////////////////////////////////////////////
  core  . Initialize ( )                                                     ;
  INSP  . device           = InputDeviceId                                   ;
  INSP  . channelCount     = 1                                               ;
  INSP  . sampleFormat     = cafInt16                                        ;
  INSP  . suggestedLatency = core . GetDeviceInfo ( InputDeviceId ) -> defaultLowInputLatency ;
  INSP  . streamInfo       = NULL                                            ;
  Input . BytesPerSample   = 2                                               ;
  ////////////////////////////////////////////////////////////////////////////
  rt = core . Open ( &stream                                                 ,
                     &INSP                                                   ,
                     NULL                                                    ,
                     SamplingRate                                            ,
                     SamplingRate / 10                                       ,
                     0                                                       ,
                     (Conduit *) this                                      ) ;
  if ( ( rt != NoError ) || ( NULL == stream ) )                             {
    if ( hasFunction ( "Debug" ) )                                           {
      plan -> Debug ( 30 , tr("Open audio input failure") )                  ;
    }                                                                        ;
    core . Terminate  ( )                                                    ;
    setVariable ( "Playing" , false )                                        ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  DataBuffer . clear ( )                                                     ;
  rt = core . Start ( stream )                                               ;
  if ( rt != NoError )                                                       {
    if ( hasFunction ( "Debug" ) )                                           {
      plan -> Debug ( 30 , tr("Audio input can not start") )                 ;
    }                                                                        ;
    core . Terminate  ( )                                                    ;
    setVariable ( "Playing" , false )                                        ;
    return                                                                   ;
  }                                                                          ;
  if ( hasFunction ( "Debug" ) )                                             {
    plan -> Debug ( 30 , tr("Media encoder started") )                       ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  setVariable ( "Playing" , true )                                           ;
  while ( KeepRecording ( ) )                                                {
    if ( 1 != core . IsActive ( stream ) )                                   {
      setVariable ( "Playing" , false )                                      ;
    }                                                                        ;
    Time::msleep ( 50 )                                                      ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Debug" ) )                                             {
    plan -> Debug ( 30 , tr("Stream stopped") )                              ;
  }                                                                          ;
  if ( 0 != core . IsStopped ( stream ) ) core . Stop ( stream )             ;
  if ( hasFunction ( "Debug" ) )                                             {
    plan -> Debug ( 30 , tr("Stream closing") )                              ;
  }                                                                          ;
  core  . Close ( stream                      )                              ;
  if ( hasFunction ( "Debug" ) )                                             {
    plan -> Debug ( 30 , tr("Stream closed" ) )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  core . Terminate  ( )                                                      ;
  setVariable ( "Playing" , false )                                          ;
}

bool N::EarIn::Encode     (
       AvEncoder & encode ,
       QIODevice & IO     ,
       bool        flush  )
{
  int as = DataBuffer . size ( )                  ;
  if ( as <= 0                     ) return false ;
  if ( IsNull ( encode . context ) ) return false ;
  /////////////////////////////////////////////////
  unsigned char * src                             ;
  unsigned char * cps = new unsigned char [ as ]  ;
  LockAt   ( "Operate"      )                     ;
  src = (unsigned char *) DataBuffer . data ( )   ;
  ::memcpy ( cps , src , as )                     ;
  UnlockAt ( "Operate"      )                     ;
  as  = encode . Encode ( IO , as , cps , flush ) ;
  delete [ ] cps                                  ;
  /////////////////////////////////////////////////
  if ( ( as > 0 ) && ( ! flush ) )                {
    LockAt              ( "Operate" )             ;
    DataBuffer . remove ( 0 , as    )             ;
    UnlockAt            ( "Operate" )             ;
  }                                               ;
  return true                                     ;
}

void N::EarIn::Saving(VarArgs & args)
{
#pragma message("Revise this code and look at the FFmpeg example output-example.c")
  if ( hasFunction ( "Saving" ) ) return                            ;
  QString SaveTo = Value ( "SaveTo" ) . toString ( )                ;
  int     ID     = Value ( "Codec"  ) . toInt    ( )                ;
  int     Rate   = Value ( "Rate"   ) . toInt    ( )                ;
  int     fmt    = AV_SAMPLE_FMT_S16                                ;
  ///////////////////////////////////////////////////////////////////
  if ( SaveTo . length ( ) <= 0 ) return                            ;
  if ( ID                  <= 0 ) return                            ;
  if ( Rate                <= 0 ) return                            ;
  ///////////////////////////////////////////////////////////////////
  AvEncoder encode                                                  ;
  if ( ! encode . OpenAudio ( ID , Rate , 1 , fmt ) )               {
    if ( hasFunction ( "Debug" ) )                                  {
      plan -> Debug ( 30 , tr("Failure to find encoder") )          ;
    }                                                               ;
    setVariable ( "Saving" , false )                                ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  QFile IO ( SaveTo )                                               ;
  if ( ! IO . open ( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
    if ( hasFunction ( "Debug" ) )                                  {
      plan -> Debug ( 30 , tr("Failure to open %1").arg(SaveTo) )   ;
    }                                                               ;
    setVariable ( "Saving" , false )                                ;
    encode . Close ( )                                              ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  setVariable ( "Saving" , true )                                   ;
  if ( hasFunction ( "Debug" ) )                                    {
    plan -> Debug ( 30 , tr("Saving %1").arg(SaveTo) )              ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if ( ! Value ( "Playing" ) . toBool ( ) )                         {
    while ( ( ! Value ( "Playing"  ) . toBool ( ) )                &&
            ( ! Value ( "Finalize" ) . toBool ( ) )               ) {
      Time::msleep ( 10 )                                           ;
    }                                                               ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  while ( KeepRecording ( ) )                                       {
    if ( DataBuffer . size ( ) > 0  )                               {
      Encode  ( encode , IO , false )                               ;
    }                                                               ;
    Time::msleep ( 50 )                                             ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  Encode         ( encode , IO , true )                             ;
  IO     . close (                    )                             ;
  encode . Close (                    )                             ;
  ///////////////////////////////////////////////////////////////////
  setVariable ( "Saving" , false )                                  ;
  if ( hasFunction ( "Debug" ) )                                    {
    plan -> Debug ( 30 , tr("Stop saving %1").arg(SaveTo) )         ;
  }                                                                 ;
}

void N::EarIn::SaveAudio(void)
{
  QString   SaveTo  = Value ( "SaveTo" ) . toString ( )                  ;
  int       cid     = Value ( "Codec"  ) . toInt    ( )                  ;
  AVCodec * Encoder = NULL                                               ;
  QString   name    = ""                                                 ;
  QString   title   = ""                                                 ;
  ////////////////////////////////////////////////////////////////////////
  Encoder = ::avcodec_find_encoder ( (enum AVCodecID) cid )              ;
  if ( NotNull(Encoder) ) name = Encoder -> long_name                    ;
  title   = tr("Save as %1") . arg ( name )                              ;
  ////////////////////////////////////////////////////////////////////////
  SaveTo = QFileDialog::getSaveFileName                                  (
             gui                                                         ,
             title                                                       ,
             SaveTo                                                      ,
             tr("Audio file (*.*)")                                    ) ;
  setVariable ( "SaveTo" , SaveTo )                                      ;
  ////////////////////////////////////////////////////////////////////////
  if ( ( SaveTo . length ( ) > 0 ) && hasFunction ( "Playing" ) )        {
    start     ( CiosMenuId ( 58 , 24 , 12 ) )                            ;
  }                                                                      ;
}

void N::EarIn::StartRecord(void)
{
  if ( hasFunction ( "Playing" ) ) return                           ;
  QString SaveTo = Value ( "SaveTo" ) . toString ( )                ;
  start       ( CiosMenuId ( 58 , 24 , 11 ) )                       ;
  if ( ( SaveTo . length ( ) > 0 ) && ( ! hasFunction("Saving") ) ) {
    start     ( CiosMenuId ( 58 , 24 , 12 ) )                       ;
  }                                                                 ;
}

void N::EarIn::StopRecord(void)
{
  setVariable ( "Playing" , false ) ;
}

void N::EarIn::SelectColor(QString key)
{
  int    id = Value ( key ) . toInt ( )  ;
  QColor c  = colors [ id ] . Color ( )  ;
  c = QColorDialog::getColor             (
        c                                ,
        gui                              ,
        tr("Select color")               ,
        QColorDialog::ShowAlphaChannel ) ;
  colors [ id ] = c                      ;
}

void N::EarIn::PrepareAudioInput(QActionGroup * group)
{
  if (IsNull(group)) return                                                  ;
  QList<QAction *> actions = group->actions()                                ;
  int InputDeviceId = Value ( "DeviceId" ) . toLongLong ( )                  ;
  for (int i=0;i<actions.count();i++)                                        {
    actions[i] -> setChecked ( actions[i]->data().toInt() == InputDeviceId ) ;
  }                                                                          ;
}

void N::EarIn::InputDevice(QAction * device)
{
  if ( IsNull ( device ) ) return                             ;
  setVariable ( "DeviceId" , device -> data ( ) . toInt ( ) ) ;
}

void N::EarIn::GetSampleRate(QAction * rate)
{
  if ( IsNull ( rate ) ) return                ;
  qint64 b = rate -> data ( ) . toInt      ( ) ;
  b *= Value ( "ShowLength" ) . toLongLong ( ) ;
  b /= 500                                     ;
  LockAt               ( "Operate" )           ;
  InputBuffer . resize ( b         )           ;
  InputBuffer . fill   ( 0         )           ;
  UnlockAt             ( "Operate" )           ;
}

void N::EarIn::GetCodec(QAction * codec)
{
  if ( IsNull ( codec ) ) return          ;
  int cid = codec -> data ( ) . toInt ( ) ;
  setVariable ( "Codec" , cid )           ;
}

QActionGroup * N::EarIn::RateMenu(MenuManager & mm,QMenu * rate)
{
  QActionGroup * rag = new QActionGroup ( rate )                  ;
  QAction      * aaa = NULL                                       ;
  int            iii = 0                                          ;
  qint64         SamplingRate = Value ( "Rate" ) . toLongLong ( ) ;
  while ( N::Acoustics::SamplingRates [ iii ] > 0 )               {
    int     r = N::Acoustics::SamplingRates [ iii ]               ;
    QString n = QString::number ( r )                             ;
    aaa  = mm . add     ( rate , 1214411241+iii , n )             ;
    aaa -> setData      ( r                         )             ;
    aaa -> setCheckable ( true                      )             ;
    aaa -> setChecked   ( r == SamplingRate         )             ;
    rag -> addAction    ( aaa                       )             ;
    iii++                                                         ;
  }                                                               ;
  return rag                                                      ;
}

void N::EarIn::DisplayMenu(MenuManager & mm,QMenu * mc)
{
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 19 , 67 ) ,
             tr("Draw border")           ,
             true                        ,
             hasFunction("DrawBorder") ) ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 19 , 68 ) ,
             tr("Plane color")           ,
             true                        ,
             hasFunction("DrawPlane")  ) ;
  mm . addSeparator ( mc )               ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 20 , 11 ) ,
             tr("Assign wave color")   ) ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 20 , 12 ) ,
             tr("Assign border color") ) ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 20 , 13 ) ,
             tr("Assign focus color")  ) ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 20 , 14 ) ,
             tr("Assign plane color")  ) ;
  mm . addSeparator ( mc )               ;
  mm . add ( mc                          ,
             CiosMenuId ( 37 , 19 , 69 ) ,
             tr("Debug")                 ,
             true                        ,
             hasFunction("Debug")      ) ;
}

bool N::EarIn::DisplayProcessor(MenuManager & mm,QAction * aa)
{
  switch ( mm [ aa ] )                                   {
    case CiosMenuId ( 37 , 19 , 67  )                    :
      setVariable ( "DrawBorder" , aa -> isChecked ( ) ) ;
    return true                                          ;
    case CiosMenuId ( 37 , 19 , 68  )                    :
      setVariable ( "DrawPlane"  , aa -> isChecked ( ) ) ;
    return true                                          ;
    case CiosMenuId ( 37 , 19 , 69  )                    :
      setVariable ( "Debug"      , aa -> isChecked ( ) ) ;
    return true                                          ;
    case CiosMenuId ( 37 , 20 , 11  )                    :
      SelectColor   ( "WaveColor"   )                    ;
    return true                                          ;
    case CiosMenuId ( 37 , 20 , 12  )                    :
      SelectColor   ( "BorderColor" )                    ;
    return true                                          ;
    case CiosMenuId ( 37 , 20 , 13  )                    :
      SelectColor   ( "FocusColor"  )                    ;
    return true                                          ;
    case CiosMenuId ( 37 , 20 , 14  )                    :
      SelectColor   ( "PlaneColor"  )                    ;
    return true                                          ;
  }                                                      ;
  return false                                           ;
}

QActionGroup * N::EarIn::CodecMenu(MenuManager & mm,QMenu * me)
{
  QActionGroup * cag = new QActionGroup ( me ) ;
  #define ADDX(ID) addCodec ( mm , me , cag , ID )
  ADDX(AV_CODEC_ID_MPEG1VIDEO);
  ADDX(AV_CODEC_ID_MPEG2VIDEO);
  ADDX(AV_CODEC_ID_H261);
  ADDX(AV_CODEC_ID_H263);
  ADDX(AV_CODEC_ID_RV10);
  ADDX(AV_CODEC_ID_RV20);
  ADDX(AV_CODEC_ID_MJPEG);
  ADDX(AV_CODEC_ID_MJPEGB);
  ADDX(AV_CODEC_ID_LJPEG);
  ADDX(AV_CODEC_ID_SP5X);
  ADDX(AV_CODEC_ID_JPEGLS);
  ADDX(AV_CODEC_ID_MPEG4);
  ADDX(AV_CODEC_ID_RAWVIDEO);
  ADDX(AV_CODEC_ID_MSMPEG4V1);
  ADDX(AV_CODEC_ID_MSMPEG4V2);
  ADDX(AV_CODEC_ID_MSMPEG4V3);
  ADDX(AV_CODEC_ID_WMV1);
  ADDX(AV_CODEC_ID_WMV2);
  ADDX(AV_CODEC_ID_H263P);
  ADDX(AV_CODEC_ID_H263I);
  ADDX(AV_CODEC_ID_FLV1);
  ADDX(AV_CODEC_ID_SVQ1);
  ADDX(AV_CODEC_ID_SVQ3);
  ADDX(AV_CODEC_ID_DVVIDEO);
  ADDX(AV_CODEC_ID_HUFFYUV);
  ADDX(AV_CODEC_ID_CYUV);
  ADDX(AV_CODEC_ID_H264);
  ADDX(AV_CODEC_ID_INDEO3);
  ADDX(AV_CODEC_ID_VP3);
  ADDX(AV_CODEC_ID_THEORA);
  ADDX(AV_CODEC_ID_ASV1);
  ADDX(AV_CODEC_ID_ASV2);
  ADDX(AV_CODEC_ID_FFV1);
  ADDX(AV_CODEC_ID_4XM);
  ADDX(AV_CODEC_ID_VCR1);
  ADDX(AV_CODEC_ID_CLJR);
  ADDX(AV_CODEC_ID_MDEC);
  ADDX(AV_CODEC_ID_ROQ);
  ADDX(AV_CODEC_ID_INTERPLAY_VIDEO);
  ADDX(AV_CODEC_ID_XAN_WC3);
  ADDX(AV_CODEC_ID_XAN_WC4);
  ADDX(AV_CODEC_ID_RPZA);
  ADDX(AV_CODEC_ID_CINEPAK);
  ADDX(AV_CODEC_ID_WS_VQA);
  ADDX(AV_CODEC_ID_MSRLE);
  ADDX(AV_CODEC_ID_MSVIDEO1);
  ADDX(AV_CODEC_ID_IDCIN);
  ADDX(AV_CODEC_ID_8BPS);
  ADDX(AV_CODEC_ID_SMC);
  ADDX(AV_CODEC_ID_FLIC);
  ADDX(AV_CODEC_ID_TRUEMOTION1);
  ADDX(AV_CODEC_ID_VMDVIDEO);
  ADDX(AV_CODEC_ID_MSZH);
  ADDX(AV_CODEC_ID_ZLIB);
  ADDX(AV_CODEC_ID_QTRLE);
  ADDX(AV_CODEC_ID_TSCC);
  ADDX(AV_CODEC_ID_ULTI);
  ADDX(AV_CODEC_ID_QDRAW);
  ADDX(AV_CODEC_ID_VIXL);
  ADDX(AV_CODEC_ID_QPEG);
  ADDX(AV_CODEC_ID_PNG);
  ADDX(AV_CODEC_ID_PPM);
  ADDX(AV_CODEC_ID_PBM);
  ADDX(AV_CODEC_ID_PGM);
  ADDX(AV_CODEC_ID_PGMYUV);
  ADDX(AV_CODEC_ID_PAM);
  ADDX(AV_CODEC_ID_FFVHUFF);
  ADDX(AV_CODEC_ID_RV30);
  ADDX(AV_CODEC_ID_RV40);
  ADDX(AV_CODEC_ID_VC1);
  ADDX(AV_CODEC_ID_WMV3);
  ADDX(AV_CODEC_ID_LOCO);
  ADDX(AV_CODEC_ID_WNV1);
  ADDX(AV_CODEC_ID_AASC);
  ADDX(AV_CODEC_ID_INDEO2);
  ADDX(AV_CODEC_ID_FRAPS);
  ADDX(AV_CODEC_ID_TRUEMOTION2);
  ADDX(AV_CODEC_ID_BMP);
  ADDX(AV_CODEC_ID_CSCD);
  ADDX(AV_CODEC_ID_MMVIDEO);
  ADDX(AV_CODEC_ID_ZMBV);
  ADDX(AV_CODEC_ID_AVS);
  ADDX(AV_CODEC_ID_SMACKVIDEO);
  ADDX(AV_CODEC_ID_NUV);
  ADDX(AV_CODEC_ID_KMVC);
  ADDX(AV_CODEC_ID_FLASHSV);
  ADDX(AV_CODEC_ID_CAVS);
  ADDX(AV_CODEC_ID_JPEG2000);
  ADDX(AV_CODEC_ID_VMNC);
  ADDX(AV_CODEC_ID_VP5);
  ADDX(AV_CODEC_ID_VP6);
  ADDX(AV_CODEC_ID_VP6F);
  ADDX(AV_CODEC_ID_TARGA);
  ADDX(AV_CODEC_ID_DSICINVIDEO);
  ADDX(AV_CODEC_ID_TIERTEXSEQVIDEO);
  ADDX(AV_CODEC_ID_TIFF);
  ADDX(AV_CODEC_ID_GIF);
  ADDX(AV_CODEC_ID_DXA);
  ADDX(AV_CODEC_ID_DNXHD);
  ADDX(AV_CODEC_ID_THP);
  ADDX(AV_CODEC_ID_SGI);
  ADDX(AV_CODEC_ID_C93);
  ADDX(AV_CODEC_ID_BETHSOFTVID);
  ADDX(AV_CODEC_ID_PTX);
  ADDX(AV_CODEC_ID_TXD);
  ADDX(AV_CODEC_ID_VP6A);
  ADDX(AV_CODEC_ID_AMV);
  ADDX(AV_CODEC_ID_VB);
  ADDX(AV_CODEC_ID_PCX);
  ADDX(AV_CODEC_ID_SUNRAST);
  ADDX(AV_CODEC_ID_INDEO4);
  ADDX(AV_CODEC_ID_INDEO5);
  ADDX(AV_CODEC_ID_MIMIC);
  ADDX(AV_CODEC_ID_RL2);
  ADDX(AV_CODEC_ID_ESCAPE124);
  ADDX(AV_CODEC_ID_DIRAC);
  ADDX(AV_CODEC_ID_BFI);
  ADDX(AV_CODEC_ID_CMV);
  ADDX(AV_CODEC_ID_MOTIONPIXELS);
  ADDX(AV_CODEC_ID_TGV);
  ADDX(AV_CODEC_ID_TGQ);
  ADDX(AV_CODEC_ID_TQI);
  ADDX(AV_CODEC_ID_AURA);
  ADDX(AV_CODEC_ID_AURA2);
  ADDX(AV_CODEC_ID_V210X);
  ADDX(AV_CODEC_ID_TMV);
  ADDX(AV_CODEC_ID_V210);
  ADDX(AV_CODEC_ID_DPX);
  ADDX(AV_CODEC_ID_MAD);
  ADDX(AV_CODEC_ID_FRWU);
  ADDX(AV_CODEC_ID_FLASHSV2);
  ADDX(AV_CODEC_ID_CDGRAPHICS);
  ADDX(AV_CODEC_ID_R210);
  ADDX(AV_CODEC_ID_ANM);
  ADDX(AV_CODEC_ID_BINKVIDEO);
  ADDX(AV_CODEC_ID_IFF_ILBM);
  ADDX(AV_CODEC_ID_IFF_BYTERUN1);
  ADDX(AV_CODEC_ID_KGV1);
  ADDX(AV_CODEC_ID_YOP);
  ADDX(AV_CODEC_ID_VP8);
  ADDX(AV_CODEC_ID_PICTOR);
  ADDX(AV_CODEC_ID_ANSI);
  ADDX(AV_CODEC_ID_A64_MULTI);
  ADDX(AV_CODEC_ID_A64_MULTI5);
  ADDX(AV_CODEC_ID_R10K);
  ADDX(AV_CODEC_ID_MXPEG);
  ADDX(AV_CODEC_ID_LAGARITH);
  ADDX(AV_CODEC_ID_PRORES);
  ADDX(AV_CODEC_ID_JV);
  ADDX(AV_CODEC_ID_DFA);
  ADDX(AV_CODEC_ID_WMV3IMAGE);
  ADDX(AV_CODEC_ID_VC1IMAGE);
  ADDX(AV_CODEC_ID_UTVIDEO);
  ADDX(AV_CODEC_ID_BMV_VIDEO);
  ADDX(AV_CODEC_ID_VBLE);
  ADDX(AV_CODEC_ID_DXTORY);
  ADDX(AV_CODEC_ID_V410);
  ADDX(AV_CODEC_ID_XWD);
  ADDX(AV_CODEC_ID_CDXL);
  ADDX(AV_CODEC_ID_XBM);
  ADDX(AV_CODEC_ID_ZEROCODEC);
  ADDX(AV_CODEC_ID_MSS1);
  ADDX(AV_CODEC_ID_MSA1);
  ADDX(AV_CODEC_ID_TSCC2);
  ADDX(AV_CODEC_ID_MTS2);
  ADDX(AV_CODEC_ID_CLLC);
  ADDX(AV_CODEC_ID_MSS2);
  ADDX(AV_CODEC_ID_VP9);
  ADDX(AV_CODEC_ID_AIC);
  #ifdef Q_OS_WIN
  ADDX(AV_CODEC_ID_HNM4_VIDEO);
  #endif
  ADDX(AV_CODEC_ID_BRENDER_PIX);
  ADDX(AV_CODEC_ID_Y41P);
  ADDX(AV_CODEC_ID_ESCAPE130);
  ADDX(AV_CODEC_ID_EXR);
  ADDX(AV_CODEC_ID_AVRP);
  ADDX(AV_CODEC_ID_012V);
  ADDX(AV_CODEC_ID_G2M);
  ADDX(AV_CODEC_ID_AVUI);
  ADDX(AV_CODEC_ID_AYUV);
  ADDX(AV_CODEC_ID_TARGA_Y216);
  ADDX(AV_CODEC_ID_V308);
  ADDX(AV_CODEC_ID_V408);
  ADDX(AV_CODEC_ID_YUV4);
  ADDX(AV_CODEC_ID_SANM);
  ADDX(AV_CODEC_ID_PAF_VIDEO);
  ADDX(AV_CODEC_ID_AVRN);
  ADDX(AV_CODEC_ID_CPIA);
  ADDX(AV_CODEC_ID_XFACE);
  ADDX(AV_CODEC_ID_SGIRLE);
  ADDX(AV_CODEC_ID_MVC1);
  ADDX(AV_CODEC_ID_MVC2);
  ADDX(AV_CODEC_ID_SNOW);
  ADDX(AV_CODEC_ID_WEBP);
  ADDX(AV_CODEC_ID_SMVJPEG);
  ADDX(AV_CODEC_ID_HEVC);
  ADDX(AV_CODEC_ID_FIRST_AUDIO);
  ADDX(AV_CODEC_ID_PCM_S16LE);
  ADDX(AV_CODEC_ID_PCM_S16BE);
  ADDX(AV_CODEC_ID_PCM_U16LE);
  ADDX(AV_CODEC_ID_PCM_U16BE);
  ADDX(AV_CODEC_ID_PCM_S8);
  ADDX(AV_CODEC_ID_PCM_U8);
  ADDX(AV_CODEC_ID_PCM_MULAW);
  ADDX(AV_CODEC_ID_PCM_ALAW);
  ADDX(AV_CODEC_ID_PCM_S32LE);
  ADDX(AV_CODEC_ID_PCM_S32BE);
  ADDX(AV_CODEC_ID_PCM_U32LE);
  ADDX(AV_CODEC_ID_PCM_U32BE);
  ADDX(AV_CODEC_ID_PCM_S24LE);
  ADDX(AV_CODEC_ID_PCM_S24BE);
  ADDX(AV_CODEC_ID_PCM_U24LE);
  ADDX(AV_CODEC_ID_PCM_U24BE);
  ADDX(AV_CODEC_ID_PCM_S24DAUD);
  ADDX(AV_CODEC_ID_PCM_ZORK);
  ADDX(AV_CODEC_ID_PCM_S16LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_DVD);
  ADDX(AV_CODEC_ID_PCM_F32BE);
  ADDX(AV_CODEC_ID_PCM_F32LE);
  ADDX(AV_CODEC_ID_PCM_F64BE);
  ADDX(AV_CODEC_ID_PCM_F64LE);
  ADDX(AV_CODEC_ID_PCM_BLURAY);
  ADDX(AV_CODEC_ID_PCM_LXF);
  ADDX(AV_CODEC_ID_S302M);
  ADDX(AV_CODEC_ID_PCM_S8_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S24LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S32LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S16BE_PLANAR);
  ADDX(AV_CODEC_ID_ADPCM_IMA_QT);
  ADDX(AV_CODEC_ID_ADPCM_IMA_WAV);
  ADDX(AV_CODEC_ID_ADPCM_IMA_DK3);
  ADDX(AV_CODEC_ID_ADPCM_IMA_DK4);
  ADDX(AV_CODEC_ID_ADPCM_IMA_WS);
  ADDX(AV_CODEC_ID_ADPCM_IMA_SMJPEG);
  ADDX(AV_CODEC_ID_ADPCM_MS);
  ADDX(AV_CODEC_ID_ADPCM_4XM);
  ADDX(AV_CODEC_ID_ADPCM_XA);
  ADDX(AV_CODEC_ID_ADPCM_ADX);
  ADDX(AV_CODEC_ID_ADPCM_EA);
  ADDX(AV_CODEC_ID_ADPCM_G726);
  ADDX(AV_CODEC_ID_ADPCM_CT);
  ADDX(AV_CODEC_ID_ADPCM_SWF);
  ADDX(AV_CODEC_ID_ADPCM_YAMAHA);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_4);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_3);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_2);
  ADDX(AV_CODEC_ID_ADPCM_THP);
  ADDX(AV_CODEC_ID_ADPCM_IMA_AMV);
  ADDX(AV_CODEC_ID_ADPCM_EA_R1);
  ADDX(AV_CODEC_ID_ADPCM_EA_R3);
  ADDX(AV_CODEC_ID_ADPCM_EA_R2);
  ADDX(AV_CODEC_ID_ADPCM_IMA_EA_SEAD);
  ADDX(AV_CODEC_ID_ADPCM_IMA_EA_EACS);
  ADDX(AV_CODEC_ID_ADPCM_EA_XAS);
  ADDX(AV_CODEC_ID_ADPCM_EA_MAXIS_XA);
  ADDX(AV_CODEC_ID_ADPCM_IMA_ISS);
  ADDX(AV_CODEC_ID_ADPCM_G722);
  ADDX(AV_CODEC_ID_ADPCM_IMA_APC);
  ADDX(AV_CODEC_ID_VIMA);
  ADDX(AV_CODEC_ID_ADPCM_AFC);
  ADDX(AV_CODEC_ID_ADPCM_IMA_OKI);
  ADDX(AV_CODEC_ID_ADPCM_DTK);
  ADDX(AV_CODEC_ID_ADPCM_IMA_RAD);
  ADDX(AV_CODEC_ID_ADPCM_G726LE);
  ADDX(AV_CODEC_ID_AMR_NB);
  ADDX(AV_CODEC_ID_AMR_WB);
  ADDX(AV_CODEC_ID_RA_144);
  ADDX(AV_CODEC_ID_RA_288);
  ADDX(AV_CODEC_ID_ROQ_DPCM);
  ADDX(AV_CODEC_ID_INTERPLAY_DPCM);
  ADDX(AV_CODEC_ID_XAN_DPCM);
  ADDX(AV_CODEC_ID_SOL_DPCM);
  ADDX(AV_CODEC_ID_MP2);
  ADDX(AV_CODEC_ID_MP3);
  ADDX(AV_CODEC_ID_AAC);
  ADDX(AV_CODEC_ID_AC3);
  ADDX(AV_CODEC_ID_DTS);
  ADDX(AV_CODEC_ID_VORBIS);
  ADDX(AV_CODEC_ID_DVAUDIO);
  ADDX(AV_CODEC_ID_WMAV1);
  ADDX(AV_CODEC_ID_WMAV2);
  ADDX(AV_CODEC_ID_MACE3);
  ADDX(AV_CODEC_ID_MACE6);
  ADDX(AV_CODEC_ID_VMDAUDIO);
  ADDX(AV_CODEC_ID_FLAC);
  ADDX(AV_CODEC_ID_MP3ADU);
  ADDX(AV_CODEC_ID_MP3ON4);
  ADDX(AV_CODEC_ID_SHORTEN);
  ADDX(AV_CODEC_ID_ALAC);
  ADDX(AV_CODEC_ID_WESTWOOD_SND1);
  ADDX(AV_CODEC_ID_GSM);
  ADDX(AV_CODEC_ID_QDM2);
  ADDX(AV_CODEC_ID_COOK);
  ADDX(AV_CODEC_ID_TRUESPEECH);
  ADDX(AV_CODEC_ID_TTA);
  ADDX(AV_CODEC_ID_SMACKAUDIO);
  ADDX(AV_CODEC_ID_QCELP);
  ADDX(AV_CODEC_ID_WAVPACK);
  ADDX(AV_CODEC_ID_DSICINAUDIO);
  ADDX(AV_CODEC_ID_IMC);
  ADDX(AV_CODEC_ID_MUSEPACK7);
  ADDX(AV_CODEC_ID_MLP);
  ADDX(AV_CODEC_ID_GSM_MS);
  ADDX(AV_CODEC_ID_ATRAC3);
  ADDX(AV_CODEC_ID_APE);
  ADDX(AV_CODEC_ID_NELLYMOSER);
  ADDX(AV_CODEC_ID_MUSEPACK8);
  ADDX(AV_CODEC_ID_SPEEX);
  ADDX(AV_CODEC_ID_WMAVOICE);
  ADDX(AV_CODEC_ID_WMAPRO);
  ADDX(AV_CODEC_ID_WMALOSSLESS);
  ADDX(AV_CODEC_ID_ATRAC3P);
  ADDX(AV_CODEC_ID_EAC3);
  ADDX(AV_CODEC_ID_SIPR);
  ADDX(AV_CODEC_ID_MP1);
  ADDX(AV_CODEC_ID_TWINVQ);
  ADDX(AV_CODEC_ID_TRUEHD);
  ADDX(AV_CODEC_ID_MP4ALS);
  ADDX(AV_CODEC_ID_ATRAC1);
  ADDX(AV_CODEC_ID_BINKAUDIO_RDFT);
  ADDX(AV_CODEC_ID_BINKAUDIO_DCT);
  ADDX(AV_CODEC_ID_AAC_LATM);
  ADDX(AV_CODEC_ID_QDMC);
  ADDX(AV_CODEC_ID_CELT);
  ADDX(AV_CODEC_ID_G723_1);
  ADDX(AV_CODEC_ID_G729);
  ADDX(AV_CODEC_ID_8SVX_EXP);
  ADDX(AV_CODEC_ID_8SVX_FIB);
  ADDX(AV_CODEC_ID_BMV_AUDIO);
  ADDX(AV_CODEC_ID_RALF);
  ADDX(AV_CODEC_ID_IAC);
  ADDX(AV_CODEC_ID_ILBC);
  ADDX(AV_CODEC_ID_METASOUND);
  ADDX(AV_CODEC_ID_FFWAVESYNTH);
  ADDX(AV_CODEC_ID_SONIC);
  ADDX(AV_CODEC_ID_SONIC_LS);
  ADDX(AV_CODEC_ID_PAF_AUDIO);
  ADDX(AV_CODEC_ID_OPUS);
  ADDX(AV_CODEC_ID_TAK);
  ADDX(AV_CODEC_ID_EVRC);
  ADDX(AV_CODEC_ID_SMV);
  ADDX(AV_CODEC_ID_FIRST_SUBTITLE);
  ADDX(AV_CODEC_ID_DVD_SUBTITLE);
  ADDX(AV_CODEC_ID_DVB_SUBTITLE);
  ADDX(AV_CODEC_ID_TEXT);
  ADDX(AV_CODEC_ID_XSUB);
  ADDX(AV_CODEC_ID_SSA);
  ADDX(AV_CODEC_ID_MOV_TEXT);
  ADDX(AV_CODEC_ID_HDMV_PGS_SUBTITLE);
  ADDX(AV_CODEC_ID_DVB_TELETEXT);
  ADDX(AV_CODEC_ID_SRT);
  ADDX(AV_CODEC_ID_MICRODVD);
  ADDX(AV_CODEC_ID_EIA_608);
  ADDX(AV_CODEC_ID_JACOSUB);
  ADDX(AV_CODEC_ID_SAMI);
  ADDX(AV_CODEC_ID_REALTEXT);
  ADDX(AV_CODEC_ID_SUBVIEWER1);
  ADDX(AV_CODEC_ID_SUBVIEWER);
  ADDX(AV_CODEC_ID_SUBRIP);
  ADDX(AV_CODEC_ID_WEBVTT);
  ADDX(AV_CODEC_ID_MPL2);
  ADDX(AV_CODEC_ID_VPLAYER);
  ADDX(AV_CODEC_ID_PJS);
  ADDX(AV_CODEC_ID_ASS);
  ADDX(AV_CODEC_ID_FIRST_UNKNOWN);
  ADDX(AV_CODEC_ID_TTF);
  ADDX(AV_CODEC_ID_BINTEXT);
  ADDX(AV_CODEC_ID_XBIN);
  ADDX(AV_CODEC_ID_IDF);
  ADDX(AV_CODEC_ID_OTF);
  ADDX(AV_CODEC_ID_SMPTE_KLV);
  ADDX(AV_CODEC_ID_DVD_NAV);
  #undef  ADDX
  return cag                                   ;
}

void N::EarIn::addCodec   (
       MenuManager  & mm  ,
       QMenu        * me  ,
       QActionGroup * cag ,
       int            ID  )
{
  AVCodec * Encoder = NULL                                     ;
  //////////////////////////////////////////////////////////////
  Encoder = ::avcodec_find_encoder ( (enum AVCodecID) ID    )  ;
  nDropOut            ( IsNull(Encoder)                     )  ;
  nDropOut            ( Encoder->type != AVMEDIA_TYPE_AUDIO )  ;
  //////////////////////////////////////////////////////////////
  AVCodecContext * Context                                     ;
  int              fmt     = AV_SAMPLE_FMT_S16                 ;
  bool             correct = true                              ;
  Context                  = ::avcodec_alloc_context3(Encoder) ;
  Context -> sample_rate   = Value ( "Rate" ) . toInt ( )      ;
  Context -> channels      = 1                                 ;
  Context -> sample_fmt    = (AVSampleFormat)fmt               ;
  //////////////////////////////////////////////////////////////
  if (::avcodec_open2(Context,Encoder,NULL)<0) correct = false ;
  if (correct && Context->frame_size<=0      ) correct = false ;
  if (!correct) return                                         ;
  ::avcodec_close ( Context )                                  ;
  QString name = Encoder -> long_name                          ;
  //////////////////////////////////////////////////////////////
  int       cid = Value ( "Codec" ) . toInt ( )                ;
  QAction * aa  = mm . add ( me                                ,
                            141199121+ID                       ,
                            name                               ,
                            true                               ,
                            ( ID == cid )                    ) ;
  aa  -> setData   ( ID )                                      ;
  cag -> addAction ( aa )                                      ;
}

bool N::EarIn::Menu          (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  if ( ! Value ( "Mouse" ) . toBool ( ) ) return false                       ;
  setCursor ( Qt::ArrowCursor )                                              ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint At                                                            ;
  if ( ! Collide ( From , To , At ) ) return false                           ;
  int region = AtRegion ( At )                                               ;
  if ( region <= 0 ) return false                                            ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager     mm ( widget )                                              ;
  QMenu         * mr                                                         ;
  QMenu         * md                                                         ;
  QMenu         * mc                                                         ;
  QMenu         * me                                                         ;
  QAction       * aa                                                         ;
  QActionGroup  * aig                                                        ;
  QActionGroup  * rag                                                        ;
  QActionGroup  * cag                                                        ;
  SpinBox       * ssb    = new SpinBox       ( )                             ;
  DoubleSpinBox * dsb    = new DoubleSpinBox ( )                             ;
  double          factor = Value ( "Amplitude"  ) . toDouble ( )             ;
  int             slen   = Value ( "ShowLength" ) . toInt    ( )             ;
  QString         SaveTo = Value ( "SaveTo"     ) . toString ( )             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! hasFunction ( "Playing" ) )                                         {
    mm  . add          ( 101 , tr("Listen")       )                          ;
  } else                                                                     {
    mm  . add          ( 102 , tr("Stop")         )                          ;
  }                                                                          ;
  if ( ! hasFunction ( "Saving" ) )                                          {
    mm  . add          ( 201 , tr("Save as ...")  )                          ;
  }                                                                          ;
  mm  . addSeparator   (                          )                          ;
  mr  = mm . addMenu   ( tr("Sampling rate")      )                          ;
  md  = mm . addMenu   ( tr("Recording devices" ) )                          ;
  mc  = mm . addMenu   ( tr("Display")            )                          ;
  if ( ! hasFunction ( "Saving" ) )                                          {
    me  = mm . addMenu ( tr("Codecs")             )                          ;
  }                                                                          ;
  if ( ! hasFunction ( "Playing" ) )                                         {
    mm  . addSeparator (                          )                          ;
    mm  . add          ( 901 , tr("Close")        )                          ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ssb -> setPrefix               ( tr("Length") + " : "                    ) ;
  ssb -> setRange                ( 300 , 3600 * 1000                       ) ;
  ssb -> setValue                ( slen                                    ) ;
  ssb -> External = &slen                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  dsb -> setPrefix               ( tr("Amplitude") + " : "                 ) ;
  dsb -> setRange                ( 0.000001 , 10000.0                      ) ;
  dsb -> setValue                ( factor                                  ) ;
  dsb -> External = &factor                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  mm . add                       ( mc , 1214411231 , ssb                   ) ;
  mm . add                       ( mc , 1214411232 , dsb                   ) ;
  DisplayMenu                    ( mm , mc                                 ) ;
  rag = RateMenu                 ( mm , mr                                 ) ;
  if                             ( ! hasFunction ( "Saving" )              ) {
    cag = CodecMenu              ( mm , me                                 ) ;
  }                                                                          ;
  aig = N::Acoustics::DeviceMenu ( plan , N::Acoustics::Input , md         ) ;
  PrepareAudioInput              ( aig                                     ) ;
  if ( ! hasFunction ( "Playing" ) )                                         {
    nConnect ( rag  , SIGNAL ( triggered     (QAction*) )                    ,
               this , SLOT   ( GetSampleRate (QAction*) )                    ,
               Qt::DirectConnection                                        ) ;
    nConnect ( aig  , SIGNAL ( triggered     (QAction*) )                    ,
               this , SLOT   ( InputDevice   (QAction*) )                    ,
               Qt::DirectConnection                                        ) ;
  }                                                                          ;
  if ( ! hasFunction ( "Saving" ) )                                          {
    nConnect ( cag  , SIGNAL ( triggered     (QAction*) )                    ,
               this , SLOT   ( GetCodec      (QAction*) )                    ,
               Qt::DirectConnection                                        ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator (                      )                                 ;
  CallToMenu        ( mm , NULL , false    )                                 ;
  mm  . setFont     ( plan                 )                                 ;
  aa  = mm . exec   (                      )                                 ;
  setVariable       ( "Amplitude" , factor )                                 ;
  if ( slen != Value ( "ShowLength" ) . toInt ( ) )                          {
    qint64 b = Value ( "Rate"       ) . toLongLong ( )                       ;
    setVariable ( "ShowLength" , slen )                                      ;
    b *= slen                                                                ;
    b /= 500                                                                 ;
    LockAt               ( "Operate" )                                       ;
    InputBuffer . resize ( b         )                                       ;
    UnlockAt             ( "Operate" )                                       ;
  }                                                                          ;
  nKickOut          ( IsNull ( aa ) , true )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( DisplayProcessor ( mm , aa        ) ) return true                     ;
  if ( 0 != CallToMenu  ( mm , aa , true ) ) return true                     ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case  101                                                                :
      start       ( CiosMenuId ( 58 , 24 , 11 ) )                            ;
      if ( ( SaveTo . length ( ) > 0 ) && ( ! hasFunction("Saving") ) )      {
        start     ( CiosMenuId ( 58 , 24 , 12 ) )                            ;
      }                                                                      ;
    break                                                                    ;
    case  102                                                                :
      setVariable ( "Playing" , false           )                            ;
    break                                                                    ;
    case  201                                                                :
      SaveAudio   (                             )                            ;
    break                                                                    ;
    case  901                                                                :
      emit CloseObject ( node )                                              ;
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

#endif
