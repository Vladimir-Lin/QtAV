#include <qtav.h>

#ifdef QT_OPENGL_LIB

#define MasterEditor VideoEditor

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

//////////////////////////////////////////////////////////////////////////////

typedef struct               {
  N::VideoEditor * editor    ;
  N::Universe    * universe  ;
  N::Universe    * attach    ;
  N::SceneObject * scene     ;
  N::SceneObject * mounting  ;
} PrivateVideoFunctionBinder ;

//////////////////////////////////////////////////////////////////////////////

N::VideoEditor:: VideoEditor        ( QWidget * parent , Plan * p , QString IPC )
               : WorldView          (           parent ,        p ,         IPC )
               , UnifiedResources   (                                           )
               , VideoTimer         ( new QTimer ( this )                       )
               , cli                ( NULL                                      )
               , glyph              ( NULL                                      )
{
  Configure ( ) ;
}

N::VideoEditor::~VideoEditor (void)
{
  AutoDeletion ( ) ;
}

void N::VideoEditor::Configure (void)
{
  setAccessibleName ( "N::VideoEditor"   )                                   ;
  setObjectName     ( "N::VideoEditor"   )                                   ;
  setWindowTitle    ( tr("Video editor") )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  RPC . setParent ( this                                                     ,
                    SLOT ( Existing (                   ) )                  ,
                    SLOT ( Login    ( QString , QString ) )                ) ;
  Server  = & ( RPC . RPC )                                                  ;
  Server -> addMethod ( this )                                               ;
  ////////////////////////////////////////////////////////////////////////////
  VideoTimer -> setParent ( this        )                                    ;
  addConnector ( "Rendering"                                                 ,
                 VideoTimer                                                  ,
                 SIGNAL ( timeout      ( ) )                                 ,
                 this                                                        ,
                 SLOT   ( RenderUpdate ( ) )                               ) ;
  ////////////////////////////////////////////////////////////////////////////
  SceneCreator::add ( "CameraBox" , CameraCreator   )                        ;
  SceneCreator::add ( "Wave"      , WaveCreator     )                        ;
  SceneCreator::add ( "Video"     , VideoCreator    )                        ;
  SceneCreator::add ( "Editable"  , EditableCreator )                        ;
  SceneCreator::add ( "Group"     , GroupCreator    )                        ;
  ////////////////////////////////////////////////////////////////////////////
  onlyConnector ( "Rendering"  )                                             ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( this , SIGNAL ( PrivateEmptyShell  (    ) )                     ,
             this , SLOT   ( EmitEmptyShell     (    ) )                   ) ;
  nConnect ( this , SIGNAL ( PrivateEmitPlaying (SUID) )                     ,
             this , SLOT   ( PrivateSayPlaying  (SUID) )                   ) ;
  nConnect ( this , SIGNAL ( PrivateEmitStopped (SUID) )                     ,
             this , SLOT   ( PrivateSayStopped  (SUID) )                   ) ;
  ////////////////////////////////////////////////////////////////////////////
  setProperty ( "Scheduler" , false )                                        ;
//  Properties [ "Timer" ] = startTimer ( 15 )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  glyph = new GlyphManager     (                                   )         ;
}

int N::VideoEditor::CalendarConnector(void * data,void * dispatcher,QVariants & values)
{
  CalendarObject * cal = (CalendarObject *) data       ;
  VideoEditor    * fev = (VideoEditor    *) dispatcher ;
  if ( NULL == cal ) return 0                          ;
  if ( NULL == fev ) return 0                          ;
  nConnect ( cal , SIGNAL (EditEvent(QObject*,SUID,QDateTime,QDateTime,bool))   ,
             fev , SLOT   (EditEvent(QObject*,SUID,QDateTime,QDateTime,bool)) ) ;
  return 1                                             ;
}

int N::VideoEditor::PeriodConnector(void * data,void * dispatcher,QVariants & values)
{
  PeriodBlock * pob = (PeriodBlock *) data       ;
  VideoEditor * fev = (VideoEditor *) dispatcher ;
  if ( NULL == pob ) return 0                    ;
  if ( NULL == fev ) return 0                    ;
  return 1                                       ;
}

int N::VideoEditor::EventConnector(void * data,void * dispatcher,QVariants & values)
{
  EventObject * eol = (EventObject *) data           ;
  VideoEditor * fev = (VideoEditor *) dispatcher     ;
  if ( NULL == eol ) return 0                        ;
  if ( NULL == fev ) return 0                        ;
  nConnect ( eol , SIGNAL (EditEvent(QObject*,SUID,QDateTime,QDateTime,bool))   ,
             fev , SLOT   (EditEvent(QObject*,SUID,QDateTime,QDateTime,bool)) ) ;
  nConnect ( eol , SIGNAL (EditEvent   (int,SUID))   ,
             fev , SLOT   (EventHandler(int,SUID)) ) ;
  nConnect ( eol , SIGNAL (OpenBackup  (QString))    ,
             fev , SIGNAL (OpenBackup  (QString))  ) ;
  return 1                                           ;
}

int N::VideoEditor::PeriodActions(void * data,void * dispatcher,QVariants & values)
{
  if ( 2 != values . count ( ) ) return 0              ;
  CalendarObject * cal = (CalendarObject *) data       ;
  VideoEditor    * fev = (VideoEditor    *) dispatcher ;
  if ( NULL == cal ) return 0                          ;
  if ( NULL == fev ) return 0                          ;
  QDateTime st = values [ 0 ] . toDateTime ( )         ;
  QDateTime et = values [ 1 ] . toDateTime ( )         ;
  return 1                                             ;
}

int N::VideoEditor::AttacheMenu(void * data,void * dispatcher,QVariants & values)
{
  QMenu       * menu = (QMenu       *) data       ;
  VideoEditor * fev  = (VideoEditor *) dispatcher ;
  if ( NULL == menu ) return 0                    ;
  if ( NULL == fev  ) return 0                    ;
  fev -> VexMenu ( menu )                         ;
  return 1                                        ;
}

typedef struct            {
  N::MenuManager * menu   ;
  QAction        * action ;
  N::VideoEditor * editor ;
  void           * extra  ;
} PrivateMenuBinder       ;

int N::VideoEditor::MenuBindings(void * data,void * dispatcher,QVariants & v)
{
  PrivateMenuBinder * pmb = (PrivateMenuBinder *) dispatcher ;
  return pmb -> editor -> MenuBinder ( * ( pmb -> menu )     ,
                                           pmb -> action     ,
                                           v                 ,
                                           dispatcher      ) ;
}

int N::VideoEditor::MenuBinder (
      MenuManager & mm         ,
      QAction     * aa         ,
      QVariants   & v          ,
      void        * dispatcher )
{
  bool completed = v [ 0 ] . toBool ( )                                      ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! completed )                                                         {
    QMenu        * wm = mm . addMenu ( tr("Window details") )                ;
    QMenu        * objects                                                   ;
    QMenu        * resources                                                 ;
    QMenu        * options                                                   ;
    QMenu        * threads                                                   ;
    QMenu        * bridges                                                   ;
    QMenu        * devices                                                   ;
    QMenu        * audioOutput                                               ;
    QMenu        * audioInput                                                ;
    QMenu        * cameras                                                   ;
    QActionGroup * aig                                                       ;
    QActionGroup * aog                                                       ;
    QActionGroup * acg                                                       ;
    //////////////////////////////////////////////////////////////////////////
    WindowMenu                          ( mm                               ) ;
    objects     = mm . addMenu          ( wm , tr("Objects"  )             ) ;
    resources   = mm . addMenu          ( wm , tr("Resources")             ) ;
    options     = mm . addMenu          ( wm , tr("Options"  )             ) ;
    threads     = mm . addMenu          ( wm , tr("Threads"  )             ) ;
    bridges     = mm . addMenu          ( wm , tr("Bridges"  )             ) ;
    devices     = mm . addMenu          ( wm , tr("Devices"  )             ) ;
    audioOutput = mm . addMenu          ( devices , tr("Audio output")       ) ;
    audioInput  = mm . addMenu          ( devices , tr("Audio input" )       ) ;
    cameras     = mm . addMenu          ( devices , tr("Cameras"     )       ) ;
    aog         = Acoustics::DeviceMenu ( plan,Acoustics::Output,audioOutput ) ;
    aig         = Acoustics::DeviceMenu ( plan,Acoustics::Input ,audioInput  ) ;
    acg         = Acoustics::DeviceMenu ( plan,Acoustics::Camera,cameras     ) ;
    mm          . setGroup              ( 1001 , aig                       ) ;
    mm          . setGroup              ( 1002 , aog                       ) ;
    mm          . setGroup              ( 1003 , acg                       ) ;
    PrepareAudioOutput                  ( aog                              ) ;
    PrepareAudioInput                   ( aig                              ) ;
    PrepareCameraMenu                   ( acg                              ) ;
    //////////////////////////////////////////////////////////////////////////
    ResourceMenu                        ( mm , resources                   ) ;
    ThreadsMenu                         ( mm , threads                     ) ;
    BridgeMenu                          ( mm , bridges                     ) ;
    ObjectsMenu                         ( mm , objects                     ) ;
    //////////////////////////////////////////////////////////////////////////
    mm  . add ( options                                                      ,
                30001                                                        ,
                tr("Universe menu")                                          ,
                true                                                         ,
                isFunction(VeUniverse)                                     ) ;
    //////////////////////////////////////////////////////////////////////////
    return 1                                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( WindowProcessor     ( mm , aa ) ) return 1                            ;
  if ( MultimediaProcessor ( mm , aa ) ) return 1                            ;
  if ( ResourceProcessor   ( mm , aa ) ) return 1                            ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 10001                                                               :
      if ( aa -> isChecked ( ) ) Estimate ( 0 )                              ;
                            else Estimate ( 1 )                              ;
    return 1                                                                 ;
    case 10011                                                               :
      assignBackground (                                  )                  ;
      Corporeal        (                                  )                  ;
    return 1                                                                 ;
    case 30001                                                               :
      setFunction      ( VeUniverse , aa -> isChecked ( ) )                  ;
    return 1                                                                 ;
    case 37001                                                               :
      AddBoard         (                                  )                  ;
    return 1                                                                 ;
    case 37002                                                               :
      NewAlbum         (                                  )                  ;
    return 1                                                                 ;
    case 37003                                                               :
      ListCameras      (                                  )                  ;
    return 1                                                                 ;
    case 53001                                                               :
      AudioProcessor   (                                  )                  ;
    return 1                                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return 0                                                                   ;
}

void N::VideoEditor::BindingMenu(SceneObject * s)
{
  PrivateMenuBinder * pmb = new PrivateMenuBinder ( )                     ;
  ::memset(pmb,0,sizeof(PrivateMenuBinder))                               ;
  pmb -> editor = this                                                    ;
  s   -> userobjs         [ "MenuBinder" ] = pmb                          ;
  s   -> DynamicFunctions [ "MenuBinder" ] = N::VideoEditor::MenuBindings ;
}

void N::VideoEditor::BindingMenu(Universe * u)
{
  PrivateMenuBinder * pmb = new PrivateMenuBinder ( )                     ;
  ::memset(pmb,0,sizeof(PrivateMenuBinder))                               ;
  pmb -> editor = this                                                    ;
  u   -> userobjs         [ "MenuBinder" ] = pmb                          ;
  u   -> DynamicFunctions [ "MenuBinder" ] = N::VideoEditor::MenuBindings ;
}

int N::VideoEditor::BindFunctions(void * data,void * dispatcher,QVariants & values)
{
  if ( IsNull ( dispatcher ) ) return 0                          ;
  PrivateVideoFunctionBinder * pvfb                              ;
  pvfb = (PrivateVideoFunctionBinder *) dispatcher               ;
  return pvfb -> editor -> CallFunctions ( dispatcher , values ) ;
}

int N::VideoEditor::CallFunctions(void * dispatcher,QVariants & args)
{
  if ( IsNull ( dispatcher ) ) return 0                          ;
  if ( args . count ( ) <= 0 ) return 0                          ;
  PrivateVideoFunctionBinder * pvfb                              ;
  pvfb = (PrivateVideoFunctionBinder *) dispatcher               ;
  int cmd = args [ 0 ] . toInt ( )                               ;
  switch ( cmd )                                                 {
    case   CiosMenuId ( 99 , 38 , 11 )                           :
      CorpFunctions ( pvfb -> attach   )                         ;
    return 1                                                     ;
    case   CiosMenuId ( 99 , 38 , 12 )                           :
      CorpFunctions ( pvfb -> mounting )                         ;
    return 1                                                     ;
    case   CiosMenuId ( 99 , 38 , 13 )                           :
    return 1                                                     ;
    case   CiosMenuId ( 99 , 38 , 14 )                           :
    return 1                                                     ;
    case   CiosMenuId ( 99 , 38 , 15 )                           :
    return 1                                                     ;
    case   CiosMenuId ( 99 , 38 , 16 )                           :
    return 1                                                     ;
    case   CiosMenuId ( 99 , 40 , 11 )                           :
      if ( args . count ( ) > 1 )                                {
        SUID    album = args [ 1 ] . toULongLong ( )             ; // Play album uuid
        VarArgs xargs                                            ;
        xargs << album                                           ;
        start ( 50025 , xargs )                                  ;
      }                                                          ;
    return 1                                                     ;
    case   CiosMenuId ( 99 , 40 , 12 )                           : // New album group
    return 1                                                     ;
    case   CiosMenuId ( 99 , 40 , 13 )                           : // New detailed album group
    return 1                                                     ;
    case   CiosMenuId ( 99 , 40 , 14 )                           : // Play video
      if ( args . count ( ) > 1 )                                {
        VarArgs xargs                                            ;
        xargs = args                                             ;
        xargs . takeAt ( 0 )                                     ;
        start ( 50013 , xargs )                                  ;
      }                                                          ;
    return 1                                                     ;
  }                                                              ;
  return 1                                                       ;
}

void N::VideoEditor::CorpFunctions(Universe * u)
{
  PrivateVideoFunctionBinder * pvfb                            ;
  pvfb = new PrivateVideoFunctionBinder ( )                    ;
  ::memset ( pvfb , 0 , sizeof(PrivateVideoFunctionBinder) )   ;
  u    -> DynamicFunctions [ "BindFunctions" ] = BindFunctions ;
  u    -> userobjs         [ "BindFunctions" ] = pvfb          ;
  pvfb -> editor                               = this          ;
  pvfb -> universe                             = u             ;
}

void N::VideoEditor::CorpFunctions(SceneObject * s)
{
  PrivateVideoFunctionBinder * pvfb                            ;
  pvfb = new PrivateVideoFunctionBinder ( )                    ;
  ::memset ( pvfb , 0 , sizeof(PrivateVideoFunctionBinder) )   ;
  s    -> DynamicFunctions [ "BindFunctions" ] = BindFunctions ;
  s    -> userobjs         [ "BindFunctions" ] = pvfb          ;
  pvfb -> editor                               = this          ;
  pvfb -> scene                                = s             ;
}

void N::VideoEditor::AutoDeletion(void)
{
  if ( ! Properties . contains ( "AutoDeletion" )   ) return ;
  if (   Properties [ "AutoDeletion" ] . toBool ( ) ) return ;
  ////////////////////////////////////////////////////////////
  if ( NULL != glyph )                                       {
    delete glyph                                             ;
    glyph = NULL                                             ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if ( NULL != destructor )                                  {
    delete destructor                                        ;
    destructor = NULL                                        ;
  }                                                          ;
}

bool N::VideoEditor::FocusIn(void)
{
  if ( Booleans [ "Finalize" ]  ) return false                        ;
  /////////////////////////////////////////////////////////////////////
  DisableAllActions (                            )                    ;
  AssignAction      ( Label    , windowTitle ( ) )                    ;
  LinkAction        ( Copy     , Copy        ( ) )                    ;
  LinkAction        ( Delete   , Delete      ( ) )                    ;
  LinkAction        ( Refresh  , Refresh     ( ) )                    ;
  LinkAction        ( Insert   , PlayVideo   ( ) )                    ;
  /////////////////////////////////////////////////////////////////////
  QAction * a = PrivateActions [ "UniverseMenu" ]                     ;
  if ( NotNull(a) )                                                   {
    QObject::disconnect ( a , SIGNAL(triggered()) , NULL , NULL     ) ;
    if ( QObject::connect                                             (
         a    , SIGNAL(triggered())                                   ,
         this , SLOT(SwitchUniverseMenu()) ) )                        {
      a -> setEnabled ( true )                                        ;
      QKeySequence kkk ( Properties [ "MenuSwitch" ] . toString ( ) ) ;
      PrivateActions [ "UniverseMenu" ] -> setShortcut ( kkk        ) ;
    }                                                                 ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  return true                                                         ;
}

bool N::VideoEditor::dropObjects(int id,QPointF pos,const UUIDs & uuids)
{
  if ( uuids . count ( ) <= 0 ) return false        ;
  VarArgs args                                      ;
  args << pos                                       ;
  args << uuids.count()                             ;
  toVariants ( implicit_cast<UUIDs>(uuids) , args ) ;
  start      ( id                          , args ) ;
  return true                                       ;
}

SUID N::VideoEditor::UniverseAdd(QString key,Universe * universe)
{
  return 0 ;
}

SUID N::VideoEditor::UniverseAdd(Universe * universe)
{
  return UniverseAdd ( "Universes" , universe ) ;
}

bool N::VideoEditor::UniverseMove(SUID from,SUID to)
{
  return 0 ;
}

bool N::VideoEditor::UniverseDelete(Universe * universe)
{
  return 0 ;
}

SUID N::VideoEditor::CameraAdd(Universe * universe)
{
  return 0 ;
}

bool N::VideoEditor::CameraMove(SUID from,SUID to)
{
  return false ;
}

bool N::VideoEditor::CameraDelete(Universe * universe)
{
  return false ;
}

void N::VideoEditor::DispatchCommands(void)
{
  if (GuiSequences.count()<=0)                           {
    StartCommando ( )                                    ;
    return                                               ;
  }                                                      ;
  mutex [ 1003 ] . lock   ( )                            ;
  VarArgs args = GuiSequences [ 0 ]                      ;
  GuiSequences . takeAt ( 0 )                            ;
  mutex [ 1003 ] . unlock ( )                            ;
  switch (args[0].toInt())                               {
    case 40001                                           :
      AudioFeederClosed     ( args )                     ;
    break                                                ;
    case 50001                                           :
      VideoFeederClosed     ( args )                     ;
    break                                                ;
    case 50013                                           :
      args . takeAt  ( 0            )                    ;
      start          ( 50013 , args )                    ;
    break                                                ;
    case 60001                                           :
      args . takeAt  ( 0            )                    ;
      if ( args . count ( ) > 0 )                        {
        VcfItem * item                                   ;
        item = (VcfItem *) VariantToVoid ( args [ 0 ] )  ;
        DeleteVCF ( item )                               ;
      }                                                  ;
    break                                                ;
    case 60002                                           :
      args . takeAt  ( 0            )                    ;
      if ( args . count ( ) > 0 )                        {
        if ( VideoTimer -> isActive ( ) )                {
          VideoTimer -> setInterval ( args [0].toInt() ) ;
        }                                                ;
      }                                                  ;
    break                                                ;
  }                                                      ;
  StartCommando ( )                                      ;
}

bool N::VideoEditor::startup(void)
{
  return WorldView::startup ( ) ;
}

bool N::VideoEditor::startup(int type,SUID uuid)
{
  switch ( type )                {
    case Types::Album            :
      ImportAlbums ( uuid )      ;
    return true                  ;
    case Types::Video            :
      MUIDs [ "Videos" ] << uuid ;
      ScheduleNext ( )           ;
    return true                  ;
  }                              ;
  return false                   ;
}

bool N::VideoEditor::ItemRunner(int type,ThreadData * data)
{
  switch ( type )                                       {
    case 20001                                          :
      Animation       (                   )             ;
    return true                                         ;
    case 20002                                          :
      ExecuteCommands (                   )             ;
    return true                                         ;
    case 40001                                          :
      PicturesDropIn  ( data -> Arguments )             ;
    return true                                         ;
    case 40002                                          :
      PeopleDropIn    ( data -> Arguments )             ;
    return true                                         ;
    case 40003                                          :
      AlbumsDropIn    ( data -> Arguments )             ;
    return true                                         ;
    case 40004                                          :
      VideosDropIn    ( data -> Arguments )             ;
    return true                                         ;
    case 40005                                          :
      AudiosDropIn    ( data -> Arguments )             ;
    return true                                         ;
    case 40006                                          :
      CamerasDropIn   ( data -> Arguments )             ;
    return true                                         ;
    case 50001                                          :
      if ( 1 == data->Arguments.count() )               {
        SUID u                                          ;
        u   = data -> Arguments [ 0 ] . toULongLong ( ) ;
        ExamAudio ( u )                                 ;
      }                                                 ;
    return true                                         ;
    case 50002                                          :
      if ( 2 == data->Arguments.count() )               {
        SUID u                                          ;
        int  MID                                        ;
        u   = data -> Arguments [ 0 ] . toULongLong ( ) ;
        MID = data -> Arguments [ 1 ] . toInt       ( ) ;
        LaunchAudio ( u , MID )                         ;
      }                                                 ;
    return true                                         ;
    case 50011                                          :
      if ( 1 == data->Arguments.count() )               {
        SUID u                                          ;
        u   = data -> Arguments [ 0 ] . toULongLong ( ) ;
        ExamVideo ( u )                                 ;
      }                                                 ;
    return true                                         ;
    case 50012                                          :
      if ( 2 == data->Arguments.count() )               {
        SUID u                                          ;
        int  MID                                        ;
        u   = data -> Arguments [ 0 ] . toULongLong ( ) ;
        MID = data -> Arguments [ 1 ] . toInt       ( ) ;
        LaunchVideo ( u , MID )                         ;
      }                                                 ;
    return true                                         ;
    case 50013                                          :
      AccessVideo   ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50014                                          :
      StayAudio     ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50015                                          :
      RollVideo     ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50016                                          :
      RollAudio     ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50017                                          :
      ExportPTS     (              data -> Arguments  ) ;
    return true                                         ;
    case 50018                                          :
      ListUniverses (                                 ) ;
    return true                                         ;
    case 50019                                          :
      NewAlbum      ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50020                                          :
      Download      ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50021                                          :
      Copy          ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50022                                          :
      Compare       ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50023                                          :
      Verify        ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50024                                          :
      AccessAlbum   ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50025                                          :
      AccessAlbum   ( data -> Id , data -> Arguments  ) ;
      if ( Streamings . count ( ) <= 0 )                {
        ScheduleNext  (                               ) ;
      }                                                 ;
    return true                                         ;
    case 50026                                          :
      DescribeAlbum ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 50027                                          :
      DescribeVideo ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 52001                                          :
      VideoEvent    ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 52002                                          :
      PlayEvent     ( data -> Id , data -> Arguments  ) ;
    return true                                         ;
    case 91001                                          :
    return true                                         ;
  }                                                     ;
  return false                                          ;
}

void N::VideoEditor::RunCommand(VarArgs & arguments)
{
  nDropOut ( arguments.count() <= 0 )   ;
  QString cmd = arguments[0].toString() ;
  #define RC(NAME,FUNC)                 \
           if (cmd==NAME) FUNC(arguments)
  RC ( "ShutAudio"   , ShutAudio   )    ;
  RC ( "ShutVideo"   , ShutVideo   )    ;
//  RC ( "AddRecorder" , AddRecorder )    ;
  #undef  RC
}

void N::VideoEditor::Existing(void)
{
}

void N::VideoEditor::Login(QString login,QString hostname)
{
}

void N::VideoEditor::PicturesDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  PicturesDropIn ( pos , uuids      )     ;
}

void N::VideoEditor::PeopleDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  PeopleDropIn   ( pos , uuids      )     ;
}

void N::VideoEditor::AlbumsDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  AlbumsDropIn   ( pos , uuids      )     ;
}

void N::VideoEditor::VideosDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  VideosDropIn   ( pos , uuids      )     ;
}

void N::VideoEditor::AudiosDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  AudiosDropIn   ( pos , uuids      )     ;
}

void N::VideoEditor::CamerasDropIn(VarArgs & args)
{
  UUIDs   uuids                           ;
  QPointF pos = args [ 0 ] . toPointF ( ) ;
  ArgsToUuids    ( 1 , args , uuids )     ;
  CamerasDropIn  ( pos , uuids      )     ;
}

void N::VideoEditor::PicturesDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "Pictures" ] << uuids ;
  Alert ( Done )                ;
}

void N::VideoEditor::PeopleDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "People"   ] << uuids ;
  Alert ( Done )                ;
}

void N::VideoEditor::AlbumsDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "Albums"   ] << uuids ;
  Alert ( Done )                ;
}

void N::VideoEditor::VideosDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "Videos"   ] << uuids ;
  Alert ( Done )                ;
}

void N::VideoEditor::AudiosDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "Audios" ] << uuids ;
  Alert ( Done )              ;
}

void N::VideoEditor::CamerasDropIn(QPointF pos,UUIDs & uuids)
{
  MUIDs [ "Cameras"  ] << uuids ;
  Alert ( Done )                ;
}

void N::VideoEditor::Estimate(int step)
{
  switch ( step )                                                    {
    case 0                                                           :
      StartReflush           = nTimeNow                              ;
      FrameRate  . factor    = 1000.0                                ;
      FrameRate  . frequency = 0.0                                   ;
      FrameRate  . events    = 0                                     ;
      FrameRate  . duration  = 0                                     ;
      RenderCost . frequency = 0                                     ;
      RenderCost . events    = 0                                     ;
      RenderCost . duration  = 0                                     ;
      if ( ! VideoTimer -> isActive() )                              {
        VideoTimer -> start ( Properties [ "Interval" ] . toInt ( ) ) ;
      }                                                              ;
    break                                                            ;
    case 1                                                           :
      if ( VideoTimer -> isActive ( ) ) VideoTimer -> stop ( )       ;
      FrameRate  . duration = StartReflush . msecsTo ( nTimeNow )    ;
      FrameRate  . evaluate ( )                                      ;
    break                                                            ;
    case 2                                                           :
      Booleans [ "Painting" ] = true                                 ;
      FlushTime               = nTimeNow                             ;
      ObtainLights ( )                                               ;
    break                                                            ;
    case 3                                                           :
      FrameRate  . events   ++                                       ;
      FrameRate  . duration   = StartReflush . msecsTo ( nTimeNow )  ;
      FrameRate  . evaluate ( )                                      ;
      RenderCost . events    += FlushTime    . msecsTo ( nTimeNow )  ;
      RenderCost . duration ++                                       ;
      RenderCost . evaluate ( )                                      ;
      Booleans [ "Painting" ] = false                                ;
    break                                                            ;
  }                                                                  ;
}

QMap<SUID,QImage *> N::VideoEditor::FetchImages(SqlConnection & SC,UUIDs & Uuids)
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

QMap<SUID,QImage *> N::VideoEditor::FetchImages(UUIDs & Uuids)
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

bool N::VideoEditor::RemoveImage(SUID u)
{
  if (!ImageDepot.contains(u)) return false ;
  QImage * I = ImageDepot [ u ]             ;
  ImageDepot . take ( u )                   ;
  delete I                                  ;
  return true                               ;
}

N::Universe * N::VideoEditor::CreateWorld(QString name)
{
  Universe * wu                                              ;
//  wu = SceneCreator :: createUniverse ( name , plan , this ) ;
  wu = SceneCreator :: createUniverse ( name , plan , NULL ) ;
  JoinDestructor                      ( wu                 ) ;
  return wu                                                  ;
}

N::WaveUniverse * N::VideoEditor::AllocateWave (void)
{
  return (WaveUniverse *) CreateWorld ( "Wave" ) ;
}

N::VideoUniverse * N::VideoEditor::AllocateVideo (void)
{
  VideoUniverse * vu = (VideoUniverse *) CreateWorld ( "Video" ) ;
  if ( NULL != destructor )                                      {
    int id = vu -> limits [ RasterUniverse::RasterObjectId ]     ;
    (*destructor) += vu                                          ;
    (*destructor) += vu -> CurrentCamera                         ;
    (*destructor) += vu -> Actors [ id ]                         ;
  }                                                              ;
  return vu                                                      ;
}

N::Junction * N::VideoEditor::MountChannel(int MID)
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

N::Junction * N::VideoEditor::MountVideo(int MID)
{
  nKickOut ( !plan->AudioChannels.contains(MID) , NULL )          ;
  VideoChannel  * vc   = (VideoChannel *)plan->AudioChannels[MID] ;
  VideoConduit  * acot                                            ;
  VideoJunction * junc                                            ;
  acot = new VideoConduit  ( this , vc )                          ;
  junc = new VideoJunction ( this      )                          ;
  plan->junctions << junc                                         ;
  junc->interval   = 15                                           ;
  junc->addConduit ( acot )                                       ;
  acot->channel    = vc                                           ;
  return (Junction *) junc                                        ;
}

N::AudioAcceptor * N::VideoEditor::SpeakerOutlet(AudioFeeder * feeder)
{
  if (!EquipmentIDs.contains(PurePlan::DefaultAudioOutput)) return NULL ;
  int             outd = EquipmentIDs[PurePlan::DefaultAudioOutput]     ;
  AudioAcceptor * accp = NULL                                           ;
  AudioOut      * outp = new AudioOut ( this )                          ;
  outp   -> setDeviceId       (  outd            )                      ;
  feeder -> assign            ( *outp            )                      ;
  accp    = new AudioAcceptor ( this , outp      )                      ;
  outp   -> setMethod         ( AudioIO::Removal )                      ;
  return accp                                                           ;
}

N::AudioAcceptor * N::VideoEditor::SpeakerOutlet(VideoFeeder * feeder)
{
  if (!EquipmentIDs.contains(PurePlan::DefaultAudioOutput)) return NULL ;
  int             outd = EquipmentIDs[PurePlan::DefaultAudioOutput]     ;
  AudioAcceptor * accp = NULL                                           ;
  AudioOut      * outp = new AudioOut ( this )                          ;
  outp   -> setDeviceId       (  outd            )                      ;
  feeder -> assign            ( *outp            )                      ;
  accp    = new AudioAcceptor ( this , outp      )                      ;
  outp   -> setMethod         ( AudioIO::Removal )                      ;
  outp   -> setAutoStop       ( false            )                      ;
  return accp                                                           ;
}

N::AudioAcceptor * N::VideoEditor::WaveDisplay(Junction * junc,AudioChannel * feeder)
{
  AudioAcceptor  * accp = NULL                                      ;
  AudioTributary * trib = NULL                                      ;
  trib    = new AudioTributary ( this        )                      ;
  feeder -> assign             ( *trib       )                      ;
  accp    = new AudioAcceptor  ( this , trib )                      ;
  ///////////////////////////////////////////////////////////////////
  QRectF vw ( 0 , 0 , width() , height() )                          ;
  for (int i=0;i<trib->Channels();i++)                              {
    WaveUniverse * wu = AllocateWave ( )                            ;
    wu -> setConfigure                                              (
          trib -> Rate   ( )                                        ,
          trib -> Format ( )                                        ,
          QPoint (   0 , i * 96 )                                   ,
          QSize  ( 256 ,     96 )                                   ,
          5000                                                      ,
          10000                                                     ,
          12500                                                     ,
          QColor(128, 96, 96, 64)                                   ,
          QColor(255,  0,  0,255)                                   ,
          QColor(  0,255, 64,255)                                 ) ;
    addUniverse ( RequestUniverse(21001) , wu )                     ;
    wu   -> Relocation ( vw )                                       ;
    trib -> Acceptors [ i ] = wu                                    ;
    nMountSlot ( wu   , this , RequestPanel(WaveUniverse*,bool)   ) ;
    nMountSlot ( wu   , this , DisableMenu (                  )   ) ;
    nConnect   ( wu   , SIGNAL (Movable     (WaveUniverse*,bool))   ,
                 this , SLOT   (AudioMovable(WaveUniverse*,bool)) ) ;
    WaveToChannels [ wu ] = feeder                                  ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  return accp                                                       ;
}

N::VideoAcceptor * N::VideoEditor::VideoDisplay(Junction * junc,VideoChannel * feeder)
{
  QRectF           vw ( 0 , 0 , width() , height() )               ;
  VideoAcceptor  * accp = NULL                                     ;
  VideoUniverse  * vu   = AllocateVideo ( )                        ;
  //////////////////////////////////////////////////////////////////
  vu   -> setTransmission   ( feeder -> Transmission         )     ;
  accp  = new VideoAcceptor ( this , vu                      )     ;
  addUniverse               ( RequestUniverse(11001) , vu    )     ;
  vu   -> Relocation        ( vw                             )     ;
  //////////////////////////////////////////////////////////////////
  nMountSlot ( vu   , this , Finished    (VideoUniverse*     )   ) ;
  nMountSlot ( vu   , this , VideoStop   (VideoUniverse*     )   ) ;
  nMountSlot ( vu   , this , RequestAudio(VideoUniverse*,bool)   ) ;
  nMountSlot ( vu   , this , RequestPanel(VideoUniverse*,bool)   ) ;
  nMountSlot ( vu   , this , DisableMenu (                   )   ) ;
  nConnect   ( vu   , SIGNAL (Movable     (VideoUniverse*,bool))   ,
               this , SLOT   (VideoMovable(VideoUniverse*,bool)) ) ;
  //////////////////////////////////////////////////////////////////
  UniverseToChannels [ vu ] = feeder                               ;
  UniverseToJunction [ vu ] = junc                                 ;
  return accp                                                      ;
}

void N::VideoEditor::EnableWaveThrottle(AudioAcceptor * waveu)
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

void N::VideoEditor::AudioFeederClosed(VarArgs args)
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

void N::VideoEditor::ChannelClosed(AudioFeeder * feeder)
{
  VarArgs Args                   ;
  Args << "ShutAudio"            ;
  Args << VoidVariant ( feeder ) ;
  addSequence(Args)              ;
}

void N::VideoEditor::AudioFailure(SUID uuid,QString filename)
{ Q_UNUSED   ( uuid     ) ;
  Q_UNUSED   ( filename ) ;
}

void N::VideoEditor::UnmountAudioData(AudioData * audio)
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

void N::VideoEditor::VideoFeederClosed(VarArgs args)
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

void N::VideoEditor::VideoClosed(VideoFeeder * feeder)
{
  VarArgs Args                   ;
  Args << "ShutVideo"            ;
  Args << VoidVariant ( feeder ) ;
  addSequence(Args)              ;
}

void N::VideoEditor::VideoFailure(SUID uuid,QString filename)
{ Q_UNUSED   ( uuid                ) ;
  Q_UNUSED   ( filename            ) ;
  PlanDebug  ( 30 , "VideoFailure" ) ;
}

void N::VideoEditor::UnmountSyncData(SyncAcceptor * sync)
{
  nDropOut  ( IsNull(sync) )                   ;
  int t = sync->SyncType()                     ;
  if ( t == 30001 )                            {
    VideoUniverse * VU = (VideoUniverse *)sync ;
    RemoveCanvas   ( VU )                      ;
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

int N::VideoEditor::RequestChannel(QString filename)
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

int N::VideoEditor::RequestVideo(QString filename)
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

int N::VideoEditor::RequestInput(QString device,int rate,int bits,int channels,int format)
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

bool N::VideoEditor::RenderingMenu (
        MenuManager & mm           ,
        QMenu       * rendering    ,
        int         & interval     ,
        int         & motion       )
{
  QString   m                                                                ;
  QAction * aa                                                               ;
  SpinBox * sp                                                               ;
  SpinBox * sm                                                               ;
  ////////////////////////////////////////////////////////////////////////////
  FrameRate  . evaluate ( )                                                  ;
  RenderCost . evaluate ( )                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  sp  = new SpinBox  ( NULL , plan                                         ) ;
  sp -> setAlignment ( Qt::AlignVCenter | Qt::AlignRight                   ) ;
  sp -> setRange     ( 5    , 10000                                        ) ;
  sp -> setPrefix    ( tr("Interval : ")                                   ) ;
  sp -> setValue     ( interval                                            ) ;
  sp -> setSuffix    ( tr(" ms")                                           ) ;
  sp -> External = &interval                                                 ;
  mm  . add          ( rendering , 10002 , sp                              ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm  . add ( rendering                                                      ,
              10001                                                          ,
              tr("Rendering timer")                                          ,
              true                                                           ,
              VideoTimer -> isActive ( )                                   ) ;
  ////////////////////////////////////////////////////////////////////////////
  m   = tr("%1 (frame/s)"  ).arg((double)FrameRate ,0,'f',4 )                ;
  aa  = mm . add     ( rendering , 10003 , m                               ) ;
  aa -> setEnabled   ( false                                               ) ;
  ////////////////////////////////////////////////////////////////////////////
  m   = tr("%1 (ms/render)").arg((double)RenderCost,0,'f',4 )                ;
  aa  = mm . add     ( rendering , 10004 , m                               ) ;
  aa -> setEnabled   ( false                                               ) ;
  ////////////////////////////////////////////////////////////////////////////
  sm  = new SpinBox  ( NULL , plan                                         ) ;
  sm -> setAlignment ( Qt::AlignVCenter | Qt::AlignRight                   ) ;
  sm -> setRange     ( 1    , 10000                                        ) ;
  sm -> setPrefix    ( tr("Motion : ")                                     ) ;
  sm -> setValue     ( motion                                              ) ;
  sm -> setSuffix    ( tr(" ms")                                           ) ;
  sm -> External = &motion                                                   ;
  mm  . add          ( rendering , 10005 , sm                              ) ;
  mm  . addSeparator ( rendering                                           ) ;
  m   = tr("%1 universes").arg(Universes.count())                            ;
  aa  = mm . add     ( rendering , 10006 , m                               ) ;
  aa -> setEnabled   ( false                                               ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm  . addSeparator ( rendering                                           ) ;
  mm  . add          ( rendering , 10011 , tr("Background")                ) ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::VideoEditor::ResourceMenu(MenuManager & mm,QMenu * resources)
{
  QString m                                                                  ;
  m = tr("Albums")                                                           ;
  mm   . add                          ( resources , 22001 , m              ) ;
  m = tr("Crowds")                                                           ;
  mm   . add                          ( resources , 22002 , m              ) ;
  m = tr("Galleries")                                                        ;
  mm   . add                          ( resources , 22003 , m              ) ;
  m = tr("Scheduler")                                                        ;
  mm   . add                          ( resources , 22004 , m              ) ;
  m = tr("Open video file")                                                  ;
  mm   . add                          ( resources , 21001 , m              ) ;
  m = tr("Play list")                                                        ;
  mm   . add                          ( resources , 21002 , m              ) ;
  mm   . addSeparator                 ( resources                          ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "Albums" ] .count() > 0 )                                     {
    m = tr("%1 albums").arg(MUIDs [ "Albums" ] .count())                     ;
    mm . add                          ( resources , 20001 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "Videos" ] .count() > 0 )                                     {
    m = tr("%1 videos").arg(MUIDs [ "Videos" ] .count())                     ;
    mm . add                          ( resources , 20002 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "Audios" ] .count() > 0 )                                     {
    m = tr("%1 audios").arg(MUIDs [ "Audios" ] .count())                     ;
    mm . add                          ( resources , 20003 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "Pictures" ] .count() > 0 )                                   {
    m = tr("%1 pictures").arg(MUIDs [ "Pictures" ] .count())                 ;
    mm . add                          ( resources , 20004 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "People" ] .count() > 0 )                                     {
    m = tr("%1 people").arg(MUIDs [ "People" ] .count())                     ;
    mm . add                          ( resources , 20005 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( MUIDs [ "Cameras" ] .count() > 0 )                                    {
    m = tr("%1 cameras").arg(MUIDs [ "Cameras" ] .count())                   ;
    mm . add                          ( resources , 20006 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Items.count() > 0 )                                                   {
    m = tr("%1 graphics items").arg(Items.count())                           ;
    mm . add                          ( resources , 20007 , m              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::VideoEditor::ThreadsMenu(MenuManager & menu,QMenu * threads)
{
  cleanup ( )                                            ;
  QString m                                              ;
  int     ac = 0                                         ;
  int     vc = 0                                         ;
  ////////////////////////////////////////////////////////
  CUIDs IDs = plan -> AudioChannels . keys ( )           ;
  int   i                                                ;
  foreach ( i , IDs )                                    {
    switch ( plan -> AudioChannels [ i ] -> type ( ) )   {
      case    2 : ac++ ; break                           ;
      case 1001 : vc++ ; break                           ;
    }                                                    ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  m = tr("%1 activated threads").arg(AllThreads.count()) ;
  menu . add ( threads , 851 , m )                       ;
  ////////////////////////////////////////////////////////
  if ( ac > 0 )                                          {
    m = tr("%1 audio channels") . arg ( ac )             ;
    menu . add ( threads , 852 , m )                     ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  if ( vc > 0 )                                          {
    m = tr("%1 vidoe channels") . arg ( vc )             ;
    menu . add ( threads , 853 , m )                     ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  return true                                            ;
}

bool N::VideoEditor::BridgeMenu(MenuManager & menu,QMenu * bridge)
{
  QMenu * tj = menu.addMenu(bridge,tr("Junctions")) ;
  QMenu * ta = menu.addMenu(bridge,tr("Conduits" )) ;
  QString js                                        ;
  QString cs                                        ;
  js = tr("%1 junctions").arg(JunctionCount())      ;
  cs = tr("%1 conduits" ).arg(ConduitCount ())      ;
  menu . add ( tj , 4447001 , js )                  ;
  menu . add ( ta , 4448001 , cs )                  ;
  return true                                       ;
}

bool N::VideoEditor::ObjectsMenu(MenuManager & menu,QMenu * objects)
{
  menu . add ( objects , 37001 , tr("Add board"  ) ) ;
  menu . add ( objects , 37002 , tr("New album"  ) ) ;
  menu . add ( objects , 37003 , tr("Camera list") ) ;
  return true                                        ;
}

bool N::VideoEditor::WindowMenu(MenuManager & menu)
{
  if ( ! isFunction(VeWindow) ) return true         ;
  QMenu   * mw = menu.addMenu(tr("Window"))         ;
  QAction * aa                                      ;
  QString   ms                                      ;
  ///////////////////////////////////////////////////
  if ( isFunction(VeFullScreen) )                   {
    menu . add ( mw , 401001 , tr("Normal window")) ;
    menu . addSeparator ( mw )                      ;
  } else                                            {
    menu . add ( mw , 401002 , tr("Full screen"  )) ;
    menu . addSeparator ( mw )                      ;
  }                                                 ;
  ///////////////////////////////////////////////////
  ms   = tr("%1 x %2").arg(width()).arg(height())   ;
  aa   = menu . add   ( mw , 401011 , ms )          ;
  aa  -> setEnabled   ( true             )          ;
  menu . addSeparator ( mw               )          ;
  ///////////////////////////////////////////////////
  bool hasCmd = false                               ;
  if ( NotNull(cli) )                               {
    hasCmd = cli -> isCommanding ( )                ;
  }                                                 ;
  ms   = tr("Command interface")                    ;
  menu . add ( mw , 401012 , ms , true , hasCmd   ) ;
  ms   = tr("Arrow cursor")                         ;
  menu . add ( mw , 401013 , ms                   ) ;
  ms   = tr("No cursor")                            ;
  menu . add ( mw , 401014 , ms                   ) ;
  ms   = tr("No tool tips")                         ;
  menu . add ( mw , 401015 , ms                   ) ;
  ///////////////////////////////////////////////////
  return true                                       ;
}

bool N::VideoEditor::Menu(QPoint pos)
{
  if (   isFunction ( VeBlockade ) ) return true                             ;
  if ( ! isFunction ( VeMenu     ) ) return true                             ;
  ////////////////////////////////////////////////////////////////////////////
  if (   isFunction ( VeUniverse ) )                                         {
    if (UniverseMenu(pos)) return true                                       ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  nScopedMenu ( mm , this )                                                  ;
  QMenu        * rendering                                                   ;
  QMenu        * objects                                                     ;
  QMenu        * resources                                                   ;
  QMenu        * options                                                     ;
  QMenu        * threads                                                     ;
  QMenu        * bridges                                                     ;
  QMenu        * devices                                                     ;
  QMenu        * audioOutput                                                 ;
  QMenu        * audioInput                                                  ;
  QMenu        * cameras                                                     ;
  QAction      * aa                                                          ;
  QActionGroup * aig                                                         ;
  QActionGroup * aog                                                         ;
  QActionGroup * acg                                                         ;
  int            interval = Properties [ "Interval"       ] . toInt ( )      ;
  int            motion   = Properties [ "MotionInterval" ] . toInt ( )      ;
  ////////////////////////////////////////////////////////////////////////////
  WindowMenu                          ( mm                                 ) ;
  rendering   = mm . addMenu          ( tr("Rendering")                    ) ;
  objects     = mm . addMenu          ( tr("Objects"  )                    ) ;
  resources   = mm . addMenu          ( tr("Resources")                    ) ;
  options     = mm . addMenu          ( tr("Options"  )                    ) ;
  threads     = mm . addMenu          ( tr("Threads"  )                    ) ;
  bridges     = mm . addMenu          ( tr("Bridges"  )                    ) ;
  devices     = mm . addMenu          ( tr("Devices"  )                    ) ;
  audioOutput = mm . addMenu          ( devices , tr("Audio output")       ) ;
  audioInput  = mm . addMenu          ( devices , tr("Audio input" )       ) ;
  cameras     = mm . addMenu          ( devices , tr("Cameras"     )       ) ;
  aog         = Acoustics::DeviceMenu ( plan,Acoustics::Output,audioOutput ) ;
  aig         = Acoustics::DeviceMenu ( plan,Acoustics::Input ,audioInput  ) ;
  acg         = Acoustics::DeviceMenu ( plan,Acoustics::Camera,cameras     ) ;
  mm          . setGroup              ( 1001 , aig                         ) ;
  mm          . setGroup              ( 1002 , aog                         ) ;
  mm          . setGroup              ( 1003 , acg                         ) ;
  PrepareAudioOutput                  ( aog                                ) ;
  PrepareAudioInput                   ( aig                                ) ;
  PrepareCameraMenu                   ( acg                                ) ;
  mm . addSeparator                   ( devices                            ) ;
  mm . add                            ( devices                              ,
                                        53001                                ,
                                        tr("Audio processor")              ) ;
  ////////////////////////////////////////////////////////////////////////////
  RenderingMenu                       ( mm , rendering , interval , motion ) ;
  ResourceMenu                        ( mm , resources                     ) ;
  ThreadsMenu                         ( mm , threads                       ) ;
  BridgeMenu                          ( mm , bridges                       ) ;
  ObjectsMenu                         ( mm , objects                       ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm  . add ( options                                                        ,
              30001                                                          ,
              tr("Universe menu")                                            ,
              true                                                           ,
              isFunction(VeUniverse)                                       ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm . setFont ( plan )                                                      ;
  aa = mm.exec (      )                                                      ;
  if ( interval != Properties [ "Interval" ] . toInt ( ) )                   {
    Properties [ "Interval" ] = interval                                     ;
    if ( VideoTimer -> isActive ( ) ) VideoTimer -> setInterval ( interval ) ;
  }                                                                          ;
  if ( motion != Properties [ "MotionInterval" ] . toInt ( ) )               {
    Properties [ "MotionInterval" ] = motion                                 ;
  }                                                                          ;
  nKickOut ( IsNull(aa) , true )                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( WindowProcessor     ( mm , aa ) ) return true                         ;
  if ( MultimediaProcessor ( mm , aa ) ) return true                         ;
  if ( ResourceProcessor   ( mm , aa ) ) return true                         ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 10001                                                               :
      if ( aa -> isChecked ( ) ) Estimate ( 0 )                              ;
                            else Estimate ( 1 )                              ;
    break                                                                    ;
    case 10011                                                               :
      assignBackground (                                  )                  ;
      Corporeal        (                                  )                  ;
    break                                                                    ;
    case 30001                                                               :
      setFunction      ( VeUniverse , aa -> isChecked ( ) )                  ;
    break                                                                    ;
    case 37001                                                               :
      AddBoard         (                                  )                  ;
    break                                                                    ;
    case 37002                                                               :
      NewAlbum         (                                  )                  ;
    break                                                                    ;
    case 37003                                                               :
      ListCameras      (                                  )                  ;
    break                                                                    ;
    case 53001                                                               :
      AudioProcessor   (                                  )                  ;
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

void N::VideoEditor::PrepareAudioInput(QActionGroup * group)
{
  if (IsNull(group)) return                                         ;
  if (!Equipments.contains(N::PurePlan::DefaultAudioInput )) return ;
  QList<QAction *> actions = group->actions()                       ;
  int              n                                                ;
  n = (int)EquipmentIDs [ N::PurePlan::DefaultAudioInput  ]         ;
  for (int i=0;i<actions.count();i++)                               {
    actions[i] -> setChecked ( actions[i]->data().toInt() == n )    ;
  }                                                                 ;
}

void N::VideoEditor::PrepareAudioOutput(QActionGroup * group)
{
  if (IsNull(group)) return                                         ;
  if (!Equipments.contains(N::PurePlan::DefaultAudioOutput)) return ;
  QList<QAction *> actions = group->actions()                       ;
  int              n                                                ;
  n = (int)EquipmentIDs [ N::PurePlan::DefaultAudioOutput ]         ;
  for (int i=0;i<actions.count();i++)                               {
    actions[i] -> setChecked ( actions[i]->data().toInt() == n )    ;
  }                                                                 ;
}

void N::VideoEditor::PrepareCameraMenu(QActionGroup * group)
{
  if (IsNull(group)) return                                         ;
  if (!Equipments.contains(N::PurePlan::DefaultCamera     )) return ;
  QList<QAction *> actions = group->actions()                       ;
  int              n                                                ;
  n = (int)EquipmentIDs [ N::PurePlan::DefaultCamera      ]         ;
  for (int i=0;i<actions.count();i++)                               {
    actions[i] -> setChecked ( actions[i]->data().toInt() == n )    ;
  }                                                                 ;
}

bool N::VideoEditor::AudioInputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                      ;
  Equipments   [ N::PurePlan::DefaultAudioInput  ] = aa->text()         ;
  EquipmentIDs [ N::PurePlan::DefaultAudioInput  ] = aa->data().toInt() ;
  return true                                                           ;
}

bool N::VideoEditor::AudioOutputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                      ;
  Equipments   [ N::PurePlan::DefaultAudioOutput ] = aa->text()         ;
  EquipmentIDs [ N::PurePlan::DefaultAudioOutput ] = aa->data().toInt() ;
  return true                                                           ;
}

bool N::VideoEditor::CameraInputMenu(QActionGroup * group,QAction * aa)
{
  if (!group->actions().contains(aa)) return false                      ;
  Equipments   [ N::PurePlan::DefaultCamera      ] = aa->text()         ;
  EquipmentIDs [ N::PurePlan::DefaultCamera      ] = aa->data().toInt() ;
  return true                                                           ;
}

void N::VideoEditor::AudioProcessor(void)
{
}

bool N::VideoEditor::ResourceProcessor(MenuManager & mm,QAction * aa)
{
  if (IsNull(aa)) return false                                               ;
  switch ( mm [ aa ] )                                                       {
    case 20001                                                               :
      ListAlbums ( )                                                         ;
    return true                                                              ;
    case 20002                                                               :
      ScheduleNext ( )                                                       ;
    return true                                                              ;
    case 20003                                                               :
      if ( MUIDs [ "Audios" ] . count ( ) > 0 )                              {
        VarArgs args                                                         ;
        SUID    u = MUIDs [ "Audios" ] . first ( )                           ;
        args << u                                                            ;
        start ( 50001 , args )                                               ;
        MUIDs [ "Audios" ] . takeAt ( 0 )                                    ;
      }                                                                      ;
    return true                                                              ;
    case 21001                                                               :
      OpenVideo     ( )                                                      ;
    return true                                                              ;
    case 21002                                                               :
      PlayList      ( )                                                      ;
    return true                                                              ;
    case 22001                                                               :
      ShowAlbums    ( )                                                      ;
    return true                                                              ;
    case 22002                                                               :
      ShowCrowds    ( )                                                      ;
    return true                                                              ;
    case 22003                                                               :
      ShowGalleries ( )                                                      ;
    return true                                                              ;
    case 22004                                                               :
      ShowScheduler ( )                                                      ;
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::VideoEditor::MultimediaProcessor(MenuManager & menu,QAction * aa)
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
  return false                                     ;
}

bool N::VideoEditor::WindowProcessor(MenuManager & menu,QAction * action)
{
  switch ( menu [ action ] )               {
    case 401001                            :
      if ( NULL == parent ( ) )            {
        showNormal      ( )                ;
      } else                               {
        emit Normal     ( )                ;
      }                                    ;
      setFunction ( VeFullScreen , false ) ;
    return true                            ;
    case 401002                            :
      emit ForceFullScreen ( )             ;
      setFunction ( VeFullScreen , true  ) ;
    return true                            ;
    case 401012                            :
      if ( action->isChecked() )           {
        AttacheCLI ( )                     ;
        if ( NotNull ( cli ) )             {
          cli -> vcf -> show ( )           ;
        }                                  ;
      } else                               {
        if ( NotNull ( cli ) )             {
          cli -> vcf -> hide ( )           ;
        }                                  ;
      }                                    ;
    return true                            ;
    case 401013                            :
      setCursor  ( Qt::ArrowCursor       ) ;
    return true                            ;
    case 401014                            :
      setCursor  ( Qt::BlankCursor       ) ;
    return true                            ;
    case 401015                            :
      setToolTip ( ""                    ) ;
    return true                            ;
  }                                        ;
  return false                             ;
}

void N::VideoEditor::AttacheCLI(void)
{
  if ( NotNull ( cli ) ) return                                     ;
  ///////////////////////////////////////////////////////////////////
  ScrollDock * scroll = new N::ScrollDock ( NULL   , plan         ) ;
  cli     = new VideoEditorCLI            ( scroll , plan         ) ;
  cli    -> scroll = scroll                                         ;
  cli    -> vcf    = NewInterface         ( NULL                  ) ;
  cli    -> setPrompt                     ( ">"                   ) ;
  cli    -> connectWidget                 ( this                  ) ;
  scroll -> setSuggestion                 ( cli  -> sizeHint ( )  ) ;
  ///////////////////////////////////////////////////////////////////
  QPointF CP ( 1.0 , 1.0 )                                          ;
  QPoint  PP = plan -> toScreen ( CP )                              ;
  QSize   RX = cli  -> sizeHint (    )                              ;
  int     ph = width ( ) - 4 - ( PP . x ( ) * 2 )                   ;
  QRect   RN ( 0 ,  0 , ph                 , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP ( 1.0 , 1.0                                          ) ;
  cli -> vcf -> showHeader = true                                   ;
  cli -> vcf -> HeaderRect = RH                                     ;
  cli -> vcf -> Title      = tr ( "Command interface"    )          ;
  cli        -> resize          ( RN . size ( )          )          ;
  cli -> vcf -> setOptions      ( Options , true         )          ;
  cli -> vcf -> setZValue       ( 1.00f                  )          ;
  cli -> vcf -> setOpacity      ( 0.60                   )          ;
  cli -> vcf -> setWidget       ( scroll  , GP , RF , RC )          ;
  cli -> vcf -> adjustTransform (                        )          ;
  Append                        ( cli  -> vcf , NULL     )          ;
  cli -> vcf -> AdjustHeader    (                        )          ;
  nConnect ( cli  , SIGNAL ( Commanding      ( QStringList ) )      ,
             this , SLOT   ( PendingCommands ( QStringList ) )    ) ;
  nConnect ( this , SIGNAL ( SendStatus      ( QString     ) )      ,
             cli  , SLOT   ( AcceptMessage   ( QString     ) )    ) ;
}

void N::VideoEditor::Speak(QString message)
{
  emit SendStatus ( message ) ;
}

bool N::VideoEditor::PendingCommands(QStringList commands)
{
  if ( commands . count ( ) <= 0 ) return false ;
  ///////////////////////////////////////////////
  if ( "list" == commands [ 0 ] )               {
    if ( commands . count ( ) > 1 )             {
      if ( "universes" == commands [ 1 ] )      {
        start ( 50018 )                         ;
        return true                             ;
      }                                         ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  if ( "search" == commands [ 0 ] )             {
  }                                             ;
  ///////////////////////////////////////////////
  if ( "set" == commands [ 0 ] )                {
    if ( commands . count ( ) > 1 )             {
      if ( "background" == commands [ 1 ] )     {
        if ( commands . count ( ) > 4 )         {
          int r = commands[2].toInt()           ;
          int g = commands[3].toInt()           ;
          int b = commands[4].toInt()           ;
          int a = 255                           ;
          if ( commands . count ( ) > 5 )       {
            a = commands[5].toInt()             ;
          }                                     ;
          QColor R ( r , g , b , a)             ;
          setBackground ( R )                   ;
          return true                           ;
        }                                       ;
      }                                         ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  return false                                  ;
}

void N::VideoEditor::Copy(void)
{
}

void N::VideoEditor::Delete(void)
{
}

void N::VideoEditor::Refresh(void)
{
}

void N::VideoEditor::Copy(int Id,VarArgs args)
{ Q_UNUSED ( Id )
  QString from    = ""                                              ;
  QString to      = ""                                              ;
  int     size    = 64 * 1024                                       ;
  bool    correct = true                                            ;
  if ( args . count  ( ) >  0 ) from    = args [ 0 ] . toString ( ) ;
  if ( args . count  ( ) >  1 ) to      = args [ 1 ] . toString ( ) ;
  if ( args . count  ( ) >  2 ) size    = args [ 2 ] . toInt    ( ) ;
  if ( from . length ( ) <= 0 ) correct = false                     ;
  if ( to   . length ( ) <= 0 ) correct = false                     ;
  if ( correct ) correct = Copy ( from , to , size )                ;
  if ( correct )                                                    {
    Alert ( Done  )                                                 ;
  } else                                                            {
    Alert ( Error )                                                 ;
  }                                                                 ;
}

bool N::VideoEditor::Copy(QString from,QString to,int size)
{
  QFile      F ( from )                                          ;
  QFile      T ( to   )                                          ;
  QByteArray B                                                   ;
  qint64     fsize = 0                                           ;
  qint64     tsize = 0                                           ;
  qint64     rsize = 0                                           ;
  qint64     wsize = 0                                           ;
  qint64     psize = 0                                           ;
  int        Id    = -1                                          ;
  bool       go    = true                                        ;
  ////////////////////////////////////////////////////////////////
  if ( size <= 0 ) size = 64 * 1024                              ;
  if ( ! F . exists (                     ) ) return false       ;
  if ( ! F . open   ( QIODevice::ReadOnly ) ) return false       ;
  fsize = F . size ( )                                           ;
  if ( fsize <= 0 )                                              {
    F . close ( )                                                ;
    return false                                                 ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if ( ! T . open ( QIODevice::WriteOnly ) )                     {
    F . close ( )                                                ;
    return false                                                 ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  B . resize ( size )                                            ;
  Id    = plan->Progress(tr("Copy file"),tr("%v / %m , %p%") )   ;
  plan -> ProgressName ( Id , from                             ) ;
  plan -> ProgressText ( Id , to                               ) ;
  plan -> Start        ( Id , &psize , &go                     ) ;
  plan -> setRange     ( Id , 0      , ( fsize + 1023 ) / 1024 ) ;
  ////////////////////////////////////////////////////////////////
  do                                                             {
    rsize = F . read ( (char *) B . data ( ) , size )            ;
    if ( rsize > 0 )                                             {
      wsize = T . write ( (const char *) B . data ( ) , rsize )  ;
      if ( wsize > 0 ) tsize += wsize                            ;
    }                                                            ;
    psize  = tsize                                               ;
    psize /= 1024                                                ;
    if ( Booleans [ "Finalize" ] ) go = false                    ;
  } while ( go && ( rsize > 0 ) && ( rsize == wsize) )           ;
  ////////////////////////////////////////////////////////////////
  plan -> Finish       ( Id                  )                   ;
  T . close ( )                                                  ;
  F . close ( )                                                  ;
  ////////////////////////////////////////////////////////////////
  return nEqual ( fsize , tsize )                                ;
}

void N::VideoEditor::Compare(int Id,VarArgs args)
{ Q_UNUSED ( Id )
  QString from    = ""                                              ;
  QString to      = ""                                              ;
  int     size    = 64 * 1024                                       ;
  bool    correct = true                                            ;
  if ( args . count  ( ) >  0 ) from    = args [ 0 ] . toString ( ) ;
  if ( args . count  ( ) >  1 ) to      = args [ 1 ] . toString ( ) ;
  if ( args . count  ( ) >  2 ) size    = args [ 2 ] . toInt    ( ) ;
  if ( from . length ( ) <= 0 ) correct = false                     ;
  if ( to   . length ( ) <= 0 ) correct = false                     ;
  if ( correct ) correct = Compare ( from , to , size )             ;
  if ( correct )                                                    {
    Alert ( Done  )                                                 ;
  } else                                                            {
    Alert ( Error )                                                 ;
  }                                                                 ;
}

bool N::VideoEditor::Compare(QString from,QString to,int size)
{
  QFile      F ( from )                                                  ;
  QFile      T ( to   )                                                  ;
  QByteArray A                                                           ;
  QByteArray B                                                           ;
  qint64     fsize   = 0                                                 ;
  qint64     tsize   = 0                                                 ;
  qint64     rsize   = 0                                                 ;
  qint64     wsize   = 0                                                 ;
  qint64     psize   = 0                                                 ;
  int        Id      = -1                                                ;
  bool       go      = true                                              ;
  bool       correct = true                                              ;
  ////////////////////////////////////////////////////////////////////////
  if ( size <= 0 ) size = 64 * 1024                                      ;
  if ( ! F . exists (                     ) ) return false               ;
  if ( ! T . exists (                     ) ) return false               ;
  if ( ! F . open   ( QIODevice::ReadOnly ) ) return false               ;
  if ( ! T . open   ( QIODevice::ReadOnly ) )                            {
    F . close ( )                                                        ;
    return false                                                         ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  fsize = F . size ( )                                                   ;
  tsize = T . size ( )                                                   ;
  if ( fsize != tsize )                                                  {
    F . close ( )                                                        ;
    T . close ( )                                                        ;
    return false                                                         ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  tsize = 0                                                              ;
  Id    = plan->Progress(tr("Compare file"),tr("%v / %m , %p%") )        ;
  plan -> ProgressName ( Id , from                             )         ;
  plan -> ProgressText ( Id , to                               )         ;
  plan -> Start        ( Id , &psize , &go                     )         ;
  plan -> setRange     ( Id , 0      , ( fsize + 1023 ) / 1024 )         ;
  ////////////////////////////////////////////////////////////////////////
  do                                                                     {
    A . resize ( size )                                                  ;
    B . resize ( size )                                                  ;
    rsize  = F . read ( (char *) A . data ( ) , size )                   ;
    wsize  = T . read ( (char *) B . data ( ) , size )                   ;
    tsize += rsize                                                       ;
    psize  = tsize                                                       ;
    psize /= 1024                                                        ;
    if ( ( rsize > 0 ) && ( wsize == rsize ) )                           {
      if ( rsize != size )                                               {
        A . resize ( rsize )                                             ;
        B . resize ( rsize )                                             ;
      }                                                                  ;
      correct = ( A == B )                                               ;
    }                                                                    ;
    if ( ! correct               ) go = false                            ;
    if ( Booleans [ "Finalize" ] ) go = false                            ;
  } while ( go && ( rsize > 0 ) && ( wsize > 0 ) && ( wsize == rsize ) ) ;
  ////////////////////////////////////////////////////////////////////////
  plan -> Finish ( Id )                                                  ;
  T . close ( )                                                          ;
  F . close ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////
  return correct                                                         ;
}

void N::VideoEditor::Verify(int Id,VarArgs args)
{ Q_UNUSED ( Id )
  QString filename = ""                                                  ;
  bool    strict   = true                                                ;
  bool    correct  = true                                                ;
  if ( args     . count  ( ) >  0 ) filename = args [ 0 ] . toString ( ) ;
  if ( args     . count  ( ) >  1 ) strict   = args [ 1 ] . toBool   ( ) ;
  if ( filename . length ( ) <= 0 ) correct = false                      ;
  if ( correct ) correct = Verify ( filename , strict )                  ;
  if ( correct )                                                         {
    Alert ( Done  )                                                      ;
  } else                                                                 {
    Alert ( Error )                                                      ;
  }                                                                      ;
}

bool N::VideoEditor::Verify(QString filename,bool strict)
{
  if ( filename . length ( ) <= 0 ) return false                             ;
  ////////////////////////////////////////////////////////////////////////////
  bool correct = false                                                       ;
  AvDecoder AVX                                                              ;
  plan -> Talk (tr("Verify video file integrity"            )              ) ;
  if ( ! AVX . StartVideo ( filename ) )                                     {
    plan -> Talk ( tr("Video file can not open successfully!")             ) ;
    AVX . Close ( )                                                          ;
    return false                                                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  double         vrate       = AVX . VideoRate ( )                           ;
  bool           keepReading = true                                          ;
  int            vFrames     = 0                                             ;
  int            aFrames     = 0                                             ;
  int            rt                                                          ;
  int            ActualFrame = AVX . Frames ( )                              ;
  QDateTime      last        = QDateTime::currentDateTime()                  ;
  AVPacket     * AVP                                                         ;
  int            pid                                                         ;
  qint64         value = 0                                                   ;
  bool           go    = true                                                ;
  QDateTime      T = nTimeNow                                                ;
  pid   = plan -> Progress ( tr("Check integrity") , tr("%v / %m , %p%") )   ;
  plan -> Start ( pid , &value , &go )                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if (ActualFrame>0) plan->setRange(pid,0,ActualFrame                 )      ;
                else plan->setRange(pid,0,(int)(AVX.Duration()/1000000))     ;
  ////////////////////////////////////////////////////////////////////////////
  AVP = new AVPacket ( )                                                     ;
  AVX . Initialize ( AVP )                                                   ;
  while ( keepReading && ( 0 == ( rt = AVX . ReadPacket ( *AVP ) ) ) )       {
    if ( AVX . isAudioChannel ( *AVP) ) aFrames++                       ; else
    if ( AVX . isVideoChannel ( *AVP) )                                      {
      vFrames++                                                              ;
      if ( last != QDateTime::currentDateTime() )                            {
        if ( AVX . Frames ( ) > 0 ) value = vFrames ; else                   {
          long long pts = AVX . PTS ( *AVP )                                 ;
          if ( pts != AV_NOPTS_VALUE )                                       {
            pts = pts * vrate                                                ;
            value = (int)pts                                                 ;
          }                                                                  ;
        }                                                                    ;
        last = QDateTime::currentDateTime()                                  ;
      }                                                                      ;
    } else                                                                   {
    }                                                                        ;
    AVX . Release      ( AVP )                                               ;
    AVP = new AVPacket (     )                                               ;
    AVX . Initialize   ( AVP )                                               ;
  }                                                                          ;
  AVX . Release ( AVP )                                                      ;
  if ( ActualFrame > 0 ) value = (int) vFrames                               ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( ActualFrame > 0 ) && ( vFrames != ActualFrame ) )                   {
    switch ( rt )                                                            {
      case AVERROR_INVALIDDATA                                               :
        plan -> Talk ( tr("Video file has invalid data")                   ) ;
        if ( ! strict ) correct = true                                       ;
      break                                                                  ;
      case AVERROR_BSF_NOT_FOUND                                             :
        plan -> Talk ( tr("Bitstream filter not found")                    ) ;
      break                                                                  ;
      case AVERROR_BUG                                                       :
        plan -> Talk ( tr("Internal bug")                                  ) ;
      break                                                                  ;
      case AVERROR_BUFFER_TOO_SMALL                                          :
        plan -> Talk ( tr("Buffer too small")                              ) ;
      break                                                                  ;
      case AVERROR_DECODER_NOT_FOUND                                         :
        plan -> Talk ( tr("Decoder not found")                             ) ;
      break                                                                  ;
      case AVERROR_DEMUXER_NOT_FOUND                                         :
        plan -> Talk ( tr("Demuxer not found")                             ) ;
      break                                                                  ;
      case AVERROR_ENCODER_NOT_FOUND                                         :
        plan -> Talk ( tr("Encoder not found")                             ) ;
      break                                                                  ;
      case AVERROR_EXTERNAL                                                  :
        plan -> Talk ( tr("Generic error in an external library")          ) ;
      break                                                                  ;
      case AVERROR_FILTER_NOT_FOUND                                          :
        plan -> Talk ( tr("Filter not found")                              ) ;
      break                                                                  ;
      case AVERROR_MUXER_NOT_FOUND                                           :
        plan -> Talk ( tr("Muxer not found")                               ) ;
      break                                                                  ;
      case AVERROR_OPTION_NOT_FOUND                                          :
        plan -> Talk ( tr("Option not found")                              ) ;
      break                                                                  ;
      case AVERROR_PATCHWELCOME                                              :
        plan -> Talk ( tr("Not yet implemented")                           ) ;
      break                                                                  ;
      case AVERROR_PROTOCOL_NOT_FOUND                                        :
        plan -> Talk ( tr("Protocol not found")                            ) ;
      break                                                                  ;
      case AVERROR_STREAM_NOT_FOUND                                          :
        plan -> Talk ( tr("Stream not found")                              ) ;
      break                                                                  ;
      case AVERROR_BUG2                                                      :
        plan -> Talk ( tr("Internal bug 2")                                ) ;
      break                                                                  ;
      case AVERROR_UNKNOWN                                                   :
        plan -> Talk ( tr("Unknown error")                                 ) ;
      break                                                                  ;
      case AVERROR_EXPERIMENTAL                                              :
        plan -> Talk ( tr("Requested feature is flagged experimental")     ) ;
      break                                                                  ;
      case AVERROR_EXIT                                                      :
        plan -> Talk ( tr("Immediate exit was requested")                  ) ;
        if ( ! strict ) correct = true                                       ;
      break                                                                  ;
      case AVERROR_EOF                                                       :
        plan -> Talk ( tr("Video file end of file error")                  ) ;
        correct = true                                                       ;
      break                                                                  ;
      default                                                                :
        correct = true                                                       ;
        plan -> Talk ( tr("Video file unknown error")                      ) ;
      break                                                                  ;
    }                                                                        ;
  } else                                                                     {
    correct = true                                                           ;
    plan -> Talk ( tr("Video file is ready")                               ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AVX . Close ( )                                                            ;
  ////////////////////////////////////////////////////////////////////////////
  if ( T . msecsTo ( nTimeNow ) < 1500 ) Time::skip(1500)                    ;
  plan -> Finish ( pid )                                                     ;
  ////////////////////////////////////////////////////////////////////////////
  return correct                                                             ;
}

void N::VideoEditor::OpenVideo(void)
{
  QString filename = GetPath ( )           ;
  filename = QFileDialog::getOpenFileName  (
               this                        ,
               tr("Play video file")       ,
               filename                    ,
               tr("Video (*.*)")         ) ;
  if ( filename . length ( ) <= 0 ) return ;
  KeepPath     ( filename )                ;
  PendingVideo ( filename )                ;
}

void N::VideoEditor::PendingVideo(QString filename)
{
  VarArgs args           ;
  args << 0              ;
  args << 0              ;
  args << false          ;
  args << false          ;
  args << 0              ;
  args << 1000           ;
  args << filename       ;
  start ( 50013 , args ) ;
}

void N::VideoEditor::KeepPath(QString path)
{
  WorldView::KeepPath ( "VideoEditor" , path ) ;
}

QString N::VideoEditor::GetPath(void)
{
  return WorldView::GetPath ( "VideoEditor" ) ;
}

void N::VideoEditor::ShutAudio(VarArgs & args)
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
}

void N::VideoEditor::ShutVideo(VarArgs & args)
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
  QList<Conduit *>  CONDUITs                                 ;
  QList<Conduit *> VCONDUITs                                 ;
  for (int i=0;i<plan->junctions.count();i++)                {
    for (int j=0;j<plan->junctions[i]->conduits.count();j++) {
      Conduit * cx = plan->junctions[i]->conduits[j]         ;
      if ( 2001 == cx->type() )                              {
        AudioChannel * vcc = (AudioChannel *)feeder          ;
        VideoConduit * vcx = (VideoConduit *)cx              ;
        if ( ((AudioChannel *)(vcx->channel)) == vcc )       {
          VCONDUITs << cx                                    ;
        }                                                    ;
      }                                                      ;
      if ( 1001 == cx->type() )                              {
        AudioChannel * acc = (AudioChannel *)feeder          ;
        AudioConduit * acx = (AudioConduit *)cx              ;
        if ( acx->channel == acc )                           {
          CONDUITs << cx                                     ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
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
  CONDUITs = VCONDUITs                                       ;
  ////////////////////////////////////////////////////////////
  if ( CONDUITs.count() > 0 )                                {
    for (int i=0;i<CONDUITs.count();i++)                     {
      junc << plan->Connectivity(CONDUITs[i])                ;
    }                                                        ;
  }                                                          ;
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
}

void N::VideoEditor::AddRecorder(VarArgs & args)
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
  if ( PANEL )                                 {
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

void N::VideoEditor::playVideo(VarArgs & arguments)
{
  nDropOut ( arguments.count() != 2 )    ;
  SUID uuid = arguments[1].toULongLong() ;
  ExamVideo ( uuid )                     ;
}

bool N::VideoEditor::assureObjectName(SUID uuid)
{
  nKickOut ( uuid <= 0 , false )                           ;
  SqlConnection SC ( plan -> sql )                         ;
  if ( SC . open ( "VideoEditor" , "assureObjectNames" ) ) {
    QString nn                                             ;
    nn = SC . getName                                      (
           PlanTable(Names)                                ,
           "uuid"                                          ,
           vLanguageId                                     ,
           uuid                                          ) ;
    if ( nn . length ( ) > 0 ) ObjectNames [ uuid ] = nn   ;
    SC . close ( )                                         ;
  }                                                        ;
  SC . remove ( )                                          ;
  return true                                              ;
}

bool N::VideoEditor::assureObjectNames(SUID uuid)
{
  nKickOut ( uuid <= 0 , false )                           ;
  VideoManager  VM ( plan        )                         ;
  SqlConnection SC ( plan -> sql )                         ;
  if ( SC . open ( "VideoEditor" , "assureObjectNames" ) ) {
    QStringList ns = VM . Names ( SC , uuid )              ;
    QString     nn = ""                                    ;
    if ( ns.count() > 0 ) nn = ns . first ( )              ;
    if ( nn . length ( ) > 0 ) ObjectNames [ uuid ] = nn   ;
    SC . close ( )                                         ;
  }                                                        ;
  SC . remove ( )                                          ;
  return true                                              ;
}

bool N::VideoEditor::assureVideos(UUIDs & Uuids)
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

bool N::VideoEditor::assureAlbums(UUIDs & Uuids)
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

void N::VideoEditor::AttachWaveJunction(VideoFeeder * feeder,Junction * junc)
{
  VideoUniverse       *  universe                                     ;
  QList<VideoUniverse *> vu       = UniverseToChannels . keys ( )     ;
  foreach ( universe , vu )                                           {
    VideoFeeder * f = (VideoFeeder *) UniverseToChannels [ universe ] ;
    if ( feeder == f )                                                {
      WaveToJunction [ universe ] = junc                              ;
      return                                                          ;
    }                                                                 ;
  }                                                                   ;
}

void N::VideoEditor::RequestAudio(VideoUniverse * universe,bool enabled)
{
  if ( ! StreamVideos . contains ( universe ) ) return                       ;
  VideoStreaming * vs = StreamVideos [ universe ]                            ;
  if ( IsNull ( vs ) ) return                                                ;
  if ( ! enabled )                                                           {
    if ( vs -> waves . count ( ) > 0 )                                       {
      QList < WaveUniverse * > wx = vs -> waves                              ;
      for (int i = 0 ; i < wx . count ( ) ; i++ )                            {
        SUID wuid = wx [ i ] -> node                                         ;
        vs -> removeWave ( wx [ i ] )                                        ;
        removeUniverse ( wuid )                                              ;
        if ( NotNull(destructor) ) destructor -> Destroy ( wx [ i ] )        ;
        Time::msleep ( 100 )                                                 ;
      }                                                                      ;
    }                                                                        ;
    universe -> setFunction ( Universe::AudioAmplitude , false )             ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  QRectF vw ( 0 , 0 , width() , height() )                                   ;
  for (int i = 0 ; i < vs -> AudioBuffer . Channels ( ) ; i++ )              {
    WaveUniverse * wu   = AllocateWave ( )                                   ;
    SUID           wuid = 0                                                  ;
    wu -> setConfigure                                                       (
          vs -> AudioBuffer . Rate   ( )                                     ,
          vs -> AudioBuffer . Format ( )                                     ,
          QPoint (   0 , i * 96 )                                            ,
          QSize  ( 256 ,     96 )                                            ,
          5000                                                               ,
          10000                                                              ,
          12500                                                              ,
          QColor(128, 96, 96, 64)                                            ,
          QColor(255,  0,  0,255)                                            ,
          QColor(  0,255, 64,255)                                          ) ;
    wu   -> BypassThrottle = false                                           ;
    wuid  = RequestUniverse ( 21001 )                                        ;
    addUniverse ( wuid , wu )                                                ;
    wu   -> node = wuid                                                      ;
    wu   -> Relocation ( vw )                                                ;
    nMountSlot ( wu , this , RequestPanel(WaveUniverse*,bool)              ) ;
    nMountSlot ( wu , this , DisableMenu (                  )              ) ;
    nConnect   ( wu   , SIGNAL (Movable     (WaveUniverse*,bool))            ,
                 this , SLOT   (AudioMovable(WaveUniverse*,bool))          ) ;
    vs -> addWave ( i , wu )                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! vs -> hasWave ( ) )                                                 {
    VarArgs args                                                             ;
    vs -> Booleans [ "hasWave" ] = true                                      ;
    args << vs -> ThreadId                                                   ;
    start ( 50016 , args )                                                   ;
  }                                                                          ;
  universe -> setFunction ( Universe::AudioAmplitude , true )                ;
  ////////////////////////////////////////////////////////////////////////////
  emit Visualize (      )                                                    ;
}

void N::VideoEditor::VideoMovable(VideoUniverse * universe,bool enabled)
{
  QRectF vw ( 0 , 0 , width() ,height() )                               ;
  universe -> Relocation ( vw )                                         ;
  if ( enabled )                                                        {
    PlaceCanvas  ( universe )                                           ;
  } else                                                                {
    if ( UniverseCanvas . contains ( universe ) )                       {
      AssignCanvas ( universe )                                         ;
    } else                                                              {
      if ( universe -> isFunction ( VideoUniverse::FollowWindowSize ) ) {
        universe -> setFunction   ( Universe::FixedPosition , false )   ;
        universe -> Relocation    ( vw                              )   ;
      }                                                                 ;
    }                                                                   ;
    RemoveCanvas ( universe )                                           ;
    if ( StreamVideos . contains ( universe ) )                         {
      VideoStreaming * vs = StreamVideos [ universe ]                   ;
      if ( NotNull ( vs ) ) vs -> canvas = NULL                         ;
    }                                                                   ;
  }                                                                     ;
  emit Visualize ( )                                                    ;
  plan -> processEvents ( )                                             ;
}

void N::VideoEditor::PlaceCanvas(VideoUniverse * universe)
{
  ControlPoint Img                                                           ;
  ControlPoint Pos                                                           ;
  ControlPoint Zoom                                                          ;
  ControlPoint Size                                                          ;
  QPoint       pw                                                            ;
  QPoint       pt                                                            ;
  QPointF      pp                                                            ;
  QSizeF       ps                                                            ;
  QSizeF       pc                                                            ;
  QRectF       vr                                                            ;
  VcfVideo  *  canvas                                                        ;
  ////////////////////////////////////////////////////////////////////////////
  Img = universe->Users[75000]                                               ;
  if ( universe -> Users . contains ( 75001 ) )                              {
    QPoint C(width()/2,height()/2)                                           ;
    Pos  = universe->Users[75001]                                            ;
    Zoom = universe->Users[75002]                                            ;
    Size = universe->Users[75003]                                            ;
    pt   = QPoint ( Pos  . x + C . x ( ) , C . y ( ) - Pos  . y )            ;
    pw   = QPoint ( Size . x             , Size . y             )            ;
  } else                                                                     {
    pt   = QPoint ( width ( ) - Img . x  , height ( ) - Img . y )            ;
    pt  /= 2                                                                 ;
    pw   = QPoint (             Img . x  ,              Img . y )            ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  pp = toPaper             ( pw                      )                       ;
  ps = QSizeF              ( pp . x ( ) , pp . y ( ) )                       ;
  ps = VcfDisplay::toPaper ( ps                      )                       ;
  ////////////////////////////////////////////////////////////////////////////
  pp = toPaper             ( pt                      )                       ;
  pc = QSizeF              ( pp . x ( ) , pp . y ( ) )                       ;
  pc = VcfDisplay::toPaper ( pc                      )                       ;
  ////////////////////////////////////////////////////////////////////////////
  vr = QRectF  ( pc.width() , pc.height() , ps.width() , ps.height() )       ;
  ////////////////////////////////////////////////////////////////////////////
  Pen Penx                                                                   ;
  Penx    . setWidthF  ( 7.2                                               ) ;
  Penx    . setColor   ( QColor ( 0 , 0 , 255 , 255 )                      ) ;
  canvas  = new VcfVideo  ( this , NULL , plan )                             ;
  addItem                 ( canvas , NULL      )                             ;
  scene ( ) -> addItem    ( canvas             )                             ;
  canvas -> Mode    = VcfCanvas::Border                                      ;
  canvas -> Options = &Options                                               ;
  canvas -> setFunction ( 32001 , true                                     ) ;
  canvas -> setFlag     ( QGraphicsItem::ItemSendsGeometryChanges , true   ) ;
  canvas -> setCanvas   ( vr                                               ) ;
  canvas -> setZValue   ( 0.5                                              ) ;
  canvas -> setOpacity  ( 0.35f                                            ) ;
  canvas -> Painter . pens [ 0 ] = Penx                                      ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( canvas , SIGNAL ( GeometryChanged(VcfItem*) )                   ,
             this   , SLOT   ( VideoChanged   (VcfItem*) )                 ) ;
  ////////////////////////////////////////////////////////////////////////////
  UniverseCanvas [ universe ] = canvas                                       ;
  CanvasUniverse [ canvas   ] = universe                                     ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! StreamVideos . contains ( universe ) ) return                       ;
  VideoStreaming * vs = StreamVideos [ universe ]                            ;
  if ( IsNull ( vs ) ) return                                                ;
  vs -> canvas = canvas                                                      ;
}

void N::VideoEditor::AssignCanvas(VideoUniverse * universe)
{
  QRectF vw ( 0 , 0 , width() , height() )                                   ;
  if ( universe -> isFunction  ( VideoUniverse::FollowWindowSize ) )         {
    universe    -> setFunction ( Universe::FixedPosition , false )           ;
    universe    -> Relocation  ( vw                              )           ;
    return                                                                   ;
  }                                                                          ;
  if ( ! UniverseCanvas . contains ( universe )                   ) return   ;
  ////////////////////////////////////////////////////////////////////////////
  VcfVideo * canvas = UniverseCanvas [ universe ]                            ;
  if ( IsNull(canvas) ) return                                               ;
  ////////////////////////////////////////////////////////////////////////////
  QString      s                                                             ;
  ControlPoint Img = universe->Users[75000]                                  ;
  QPointF      p                                                             ;
  QPointF      m ( width() / 2 , height() / 2 )                              ;
  QRectF       r  = canvas->mapToScene(canvas->ScreenRect).boundingRect()    ;
  double       fw                                                            ;
  double       fh                                                            ;
  ////////////////////////////////////////////////////////////////////////////
  r   = mapFromScene(r).boundingRect()                                       ;
  p   = QPointF ( r.left() , r.top() )                                       ;
  p   = QPointF ( p.x() - m.x() , m.y() - p.y() )                            ;
  fw  = r   . width  ( )                                                     ;
  fw /= Img . x                                                              ;
  fh  = r   . height ( )                                                     ;
  fh /= Img . y                                                              ;
  s   = tr("%1 x %2") . arg ( r . width ( ) ) . arg ( r . height ( ) )       ;
  ////////////////////////////////////////////////////////////////////////////
  universe -> setFunction ( Universe::FixedPosition , true )                 ;
  universe -> Users[75001] = ControlPoint ( p.x    () , p.y     () , 0 , 1 ) ;
  universe -> Users[75002] = ControlPoint ( fw        , -fh        , 0 , 1 ) ;
  universe -> Users[75003] = ControlPoint ( r.width() , r.height() , 0 , 1 ) ;
  universe -> Relocation                  ( vw                             ) ;
  ////////////////////////////////////////////////////////////////////////////
  canvas   -> setToolTip                  ( s                              ) ;
  UpdateRatio                             ( universe                       ) ;
}

void N::VideoEditor::RemoveCanvas(VideoUniverse * universe)
{
  if ( ! UniverseCanvas . contains ( universe ) ) return ;
  VcfVideo * canvas = UniverseCanvas [ universe ]        ;
  UniverseCanvas . remove ( universe )                   ;
  if ( IsNull(canvas) ) return                           ;
  CanvasUniverse . remove ( canvas   )                   ;
  RemoveVCF               ( canvas   )                   ;
}

void N::VideoEditor::VideoChanged(VcfItem * item)
{
  if ( IsNull ( item     ) ) return                    ;
  VcfVideo     * canvas = (VcfVideo *) item          ;
  VideoUniverse * universe                             ;
  if ( ! CanvasUniverse . contains ( canvas ) ) return ;
  universe = CanvasUniverse [ canvas ]                 ;
  if ( IsNull ( universe ) ) return                    ;
  AssignCanvas ( universe )                            ;
}

void N::VideoEditor::Play(QString filename)
{
  if ( filename . length ( ) <= 0 )       {
    VideoFailure ( 0 , "" )               ;
    return                                ;
  }                                       ;
  /////////////////////////////////////////
  QFileInfo FIE(filename)                 ;
  if ( ! FIE . exists ( ) )               {
    VideoFailure ( 0 , filename )         ;
    return                                ;
  }                                       ;
  /////////////////////////////////////////
  VarArgs args                            ;
  int     MID = RequestVideo ( filename ) ;
  args << 0                               ;
  args << MID                             ;
  start ( 50012 , args )                  ;
}

void N::VideoEditor::PlayVideo(SUID uuid)
{
  ExamVideo ( uuid ) ;
}

void N::VideoEditor::PlayVideo(void)
{
  QString filename = QFileDialog::getOpenFileName (
                       this                       ,
                       tr("Play video")           ,
                       ""                         ,
                       "*.*"                    ) ;
  if ( filename . length ( ) <= 0 ) return        ;
  Play ( filename )                               ;
}

void N::VideoEditor::ExamVideo(SUID u)
{
  VideoManager VM ( plan )                       ;
  QString filename = VM . Filename ( u )         ;
  ////////////////////////////////////////////////
  if ( filename . length ( ) <= 0 )              {
    VideoFailure ( u , "" )                      ;
    return                                       ;
  }                                              ;
  ////////////////////////////////////////////////
  QFileInfo FIE(filename)                        ;
  if ( ! FIE . exists ( ) )                      {
    VideoFailure ( u , filename )                ;
    return                                       ;
  }                                              ;
  ////////////////////////////////////////////////
  assureObjectNames ( u )                        ;
  ////////////////////////////////////////////////
  VarArgs args                                   ;
  int     MID = RequestVideo ( filename )        ;
  args << u                                      ;
  args << MID                                    ;
  start ( 50012 , args )                         ;
}

void N::VideoEditor::LaunchVideo(SUID u,int MID)
{
  Junction      * ajun = NULL                                       ;
  Junction      * vjun = NULL                                       ;
  VideoAcceptor * vacc = NULL                                       ;
  AudioAcceptor * accp = NULL                                       ;
  VideoFeeder   * vio  = NULL                                       ;
  AudioOut      * outp = NULL                                       ;
  ///////////////////////////////////////////////////////////////////
  vio  = (VideoFeeder *)(plan->AudioChannels[MID])                  ;
  if (IsNull(vio))                                                  {
    VideoFailure ( u , "" )                                         ;
    return                                                          ;
  }                                                                 ;
  if ( ! vio -> Peep ( ) )                                          {
    VideoFailure ( u , vio->VideoPath() )                           ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  vjun = MountVideo    ( MID        )                               ;
  vacc = VideoDisplay  ( vjun , vio )                               ;
  ajun = MountChannel  ( MID        )                               ;
  accp = SpeakerOutlet ( vio        )                               ;
  ///////////////////////////////////////////////////////////////////
  if (ObjectNames.contains(u))                                      {
    vjun->name = ObjectNames[u]                                     ;
    ajun->name = ObjectNames[u]                                     ;
    if ( plan -> Talkative ( EnableDebugger ) )                     {
      plan -> Debug ( 45 , tr("Start video %1").arg(vjun->name)   ) ;
    }                                                               ;
    setWindowTitle ( vjun -> name )                                 ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if (NotNull(vacc))                                                {
    vjun -> addConduit ( vacc )                                     ;
  }                                                                 ;
  if (NotNull(accp))                                                {
    ajun -> addConduit ( accp )                                     ;
    outp = (AudioOut *)accp->audio                                  ;
    AttachWaveJunction ( vio , ajun )                               ;
  }                                                                 ;
  plan -> Dredge       ( true       )                               ;
  ///////////////////////////////////////////////////////////////////
  vio  -> play         (            )                               ;
  if (IsNull(outp)) return                                          ;
  int ms                                                            ;
  do                                                                {
    Time::skip(50)                                                  ;
    ms = outp->Milliseconds()                                       ;
  } while ( ms < vio->Drift )                                       ;
  outp -> start  (          )                                       ;
  ///////////////////////////////////////////////////////////////////
  emit Visualize (          )                                       ;
}

void N::VideoEditor::RequestPanel(WaveUniverse * universe,bool enabled)
{ FunctionDebug ;
}

void N::VideoEditor::AudioMovable(WaveUniverse * universe,bool enabled)
{
  QRectF vw ( 0 , 0 , width() ,height() )                               ;
  universe -> Relocation ( vw )                                         ;
  if ( enabled )                                                        {
    PlaceCanvas  ( universe )                                           ;
  } else                                                                {
    if ( WaveCanvas . contains ( universe ) )                           {
      AssignCanvas ( universe )                                         ;
    } else                                                              {
      if ( universe -> isFunction ( VideoUniverse::FollowWindowSize ) ) {
        universe -> setFunction ( Universe::FixedPosition , false )     ;
        universe -> Relocation ( vw )                                   ;
      }                                                                 ;
    }                                                                   ;
    RemoveCanvas ( universe )                                           ;
  }                                                                     ;
  emit Visualize ( )                                                    ;
  plan -> processEvents ( )                                             ;
}

void N::VideoEditor::PlaceCanvas(WaveUniverse * universe)
{
  if ( IsNull ( universe -> CurrentCamera ) ) return                         ;
  Camera * camera = universe->CurrentCamera                                  ;
  if ( ! camera -> references . contains ( 0 ) ) return                      ;
  if ( ! camera -> references . contains ( 1 ) ) return                      ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint p1 = camera -> references [ 0 ]                               ;
  ControlPoint p2 = camera -> references [ 1 ]                               ;
  QPoint       pw ( p2.x ,                     p2.y )                        ;
  QPoint       pt ( p1.x , height ( ) - p1.y - p2.y )                        ;
  QPointF      pp                                                            ;
  QSizeF       ps                                                            ;
  QSizeF       pc                                                            ;
  QRectF       vr                                                            ;
  VcfVideo *  canvas = NULL                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  pp = toPaper             ( pw                      )                       ;
  ps = QSizeF              ( pp . x ( ) , pp . y ( ) )                       ;
  ps = VcfDisplay::toPaper ( ps                      )                       ;
  ////////////////////////////////////////////////////////////////////////////
  pp = toPaper             ( pt                      )                       ;
  pc = QSizeF              ( pp . x ( ) , pp . y ( ) )                       ;
  pc = VcfDisplay::toPaper ( pc                      )                       ;
  ////////////////////////////////////////////////////////////////////////////
  vr = QRectF  ( pc.width() , pc.height() , ps.width() , ps.height() )       ;
  ////////////////////////////////////////////////////////////////////////////
  Pen Penx                ( QColor ( 128 , 128 , 255 , 192 )               ) ;
  Penx     . setWidthF    ( 7                                              ) ;
  canvas   = new VcfVideo ( this , NULL , plan )                             ;
  addItem                 (  canvas , NULL                                 ) ;
  scene() -> addItem      ( canvas                                         ) ;
  canvas  -> Options = &Options                                              ;
  canvas  -> Mode    = VcfVideo::Border                                      ;
  canvas  -> setFunction  ( 32001 , true                                   ) ;
  canvas  -> setFlag      ( QGraphicsItem::ItemSendsGeometryChanges , true ) ;
  canvas  -> setCanvas    ( vr                                             ) ;
  canvas  -> setZValue    ( 0.5                                            ) ;
  canvas  -> setOpacity   ( 0.35f                                          ) ;
  canvas  -> Painter . pens [ 0 ] = Penx                                     ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( canvas , SIGNAL ( GeometryChanged ( VcfItem * ) )               ,
             this   , SLOT   ( AudioChanged    ( VcfItem * ) )             ) ;
  ////////////////////////////////////////////////////////////////////////////
  WaveCanvas [ universe ] = canvas                                           ;
  CanvasWave [ canvas   ] = universe                                         ;
}

void N::VideoEditor::AssignCanvas(WaveUniverse * universe)
{
  QRectF vw ( 0 , 0 , width() , height() )                                   ;
  if ( universe -> isFunction ( VideoUniverse::FollowWindowSize ) )          {
    universe -> setFunction ( Universe::FixedPosition , false )              ;
    universe -> Relocation  ( vw                              )              ;
    return                                                                   ;
  }                                                                          ;
  if ( ! WaveCanvas . contains ( universe ) ) return                         ;
  ////////////////////////////////////////////////////////////////////////////
  VcfVideo * canvas = WaveCanvas [ universe ]                               ;
  if ( IsNull(canvas) ) return                                               ;
  ////////////////////////////////////////////////////////////////////////////
  ControlPoint pos = universe->CurrentCamera->references[0]                  ;
  ControlPoint geo = universe->CurrentCamera->references[1]                  ;
  QString      s                                                             ;
  QPointF      p                                                             ;
  QPointF      m ( 0 , height ( ) )                                          ;
  QRectF       r  = canvas->mapToScene(canvas->ScreenRect).boundingRect()    ;
  ////////////////////////////////////////////////////////////////////////////
  r   = mapFromScene(r).boundingRect()                                       ;
  p   = QPointF ( r.left( ) , r . bottom ( )        )                        ;
  p   = QPointF ( p . x ( ) , m . y ( ) - p . y ( ) )                        ;
  s   = tr("%1 x %2") . arg ( r . width ( ) ) . arg ( r . height ( ) )       ;
  ////////////////////////////////////////////////////////////////////////////
  pos       = ControlPoint ( p . x     ( ) , p . y      ( ) )                ;
  geo       = ControlPoint ( r . width ( ) , r . height ( ) )                ;
  ////////////////////////////////////////////////////////////////////////////
  universe -> setFunction ( Universe::FixedPosition , true )                 ;
  universe -> CurrentCamera -> references [ 0 ]      = pos                   ;
  universe -> CurrentCamera -> references [ 1 ]      = geo                   ;
  universe -> Relocation                  ( vw                             ) ;
  ////////////////////////////////////////////////////////////////////////////
  canvas   -> setToolTip                  ( s                              ) ;
}

void N::VideoEditor::RemoveCanvas(WaveUniverse * universe)
{
  if ( ! WaveCanvas . contains ( universe ) ) return ;
  VcfVideo * canvas = WaveCanvas [ universe ]        ;
  WaveCanvas . remove ( universe )                   ;
  if ( IsNull(canvas) ) return                       ;
  CanvasWave . remove ( canvas   )                   ;
  RemoveVCF           ( canvas   )                   ;
}

void N::VideoEditor::AudioChanged(VcfItem * item)
{
  if ( IsNull ( item     ) ) return                ;
  VcfVideo    * canvas = (VcfVideo *) item       ;
  WaveUniverse * universe                          ;
  if ( ! CanvasWave . contains ( canvas ) ) return ;
  universe = CanvasWave [ canvas ]                 ;
  if ( IsNull ( universe ) ) return                ;
  AssignCanvas ( universe )                        ;
}

void N::VideoEditor::ExamAudio(SUID u)
{
  VideoManager VM ( plan )                       ;
  QString filename = VM . Location ( *plan , u ) ;
  ////////////////////////////////////////////////
  if ( filename . length ( ) <= 0 )              {
    AudioFailure ( u , "" )                      ;
    return                                       ;
  }                                              ;
  ////////////////////////////////////////////////
  QFileInfo FIE(filename)                        ;
  if ( ! FIE . exists ( ) )                      {
    AudioFailure ( u , filename )                ;
    return                                       ;
  }                                              ;
  ////////////////////////////////////////////////
  assureObjectName ( u )                         ;
  ////////////////////////////////////////////////
  VarArgs args                                   ;
  int     MID = RequestChannel ( filename )      ;
  args << u                                      ;
  args << MID                                    ;
  start ( 50002 , args )                         ;
}

void N::VideoEditor::LaunchAudio(SUID u,int MID)
{
  Junction      * junc = NULL                                       ;
  AudioAcceptor * accp = NULL                                       ;
  AudioAcceptor * wact = NULL                                       ;
  AudioFeeder   * aio  = NULL                                       ;
  AudioOut      * outp = NULL                                       ;
  ///////////////////////////////////////////////////////////////////
  aio  = (AudioFeeder *)(plan->AudioChannels[MID])                  ;
  if (IsNull(aio))                                                  {
    AudioFailure ( u , "" )                                         ;
    return                                                          ;
  }                                                                 ;
  if ( ! aio -> Peep ( ) )                                          {
    AudioFailure ( u , aio->Filename )                              ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  junc  = MountChannel  ( MID        )                              ;
  accp  = SpeakerOutlet ( aio        )                              ;
  wact  = WaveDisplay   ( junc , aio )                              ;
  ///////////////////////////////////////////////////////////////////
  if ( ObjectNames . contains ( u ) )                               {
    junc -> name = ObjectNames [ u ]                                ;
    if (plan->Talkative(EnableDebugger))                            {
      plan -> Debug ( 45 , tr("Start audio %1").arg(junc->name)   ) ;
    }                                                               ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if (NotNull(accp))                                                {
    junc -> addConduit ( accp )                                     ;
    outp = (AudioOut *)accp->audio                                  ;
  }                                                                 ;
  if (NotNull(wact))                                                {
    junc -> addConduit ( wact                                     ) ;
  }                                                                 ;
  plan -> Dredge       ( true                                     ) ;
  ///////////////////////////////////////////////////////////////////
  if ( NotNull ( wact ) ) EnableWaveThrottle ( wact )               ;
  aio->start()                                                      ;
  ///////////////////////////////////////////////////////////////////
  if (IsNull(outp)) return                                          ;
  int ms                                                            ;
  do                                                                {
    Time::skip(50)                                                  ;
    ms = outp->Milliseconds()                                       ;
  } while ( ms < 250 )                                              ;
  outp -> start  (   )                                              ;
  ///////////////////////////////////////////////////////////////////
  emit Visualize (   )                                              ;
}

void N::VideoEditor::LaunchRecorder(VarArgs args)
{
  AudioJack * jack                             ;
  int MID = args[1].toInt()                    ;
  jack = (AudioJack *)plan->AudioChannels[MID] ;
  jack -> play ( )                             ;
}

SUID N::VideoEditor::addCamera(SUID startUuid,Camera * camera)
{
  while ( Cameras . contains ( startUuid ) ) startUuid++ ;
  Cameras        [ startUuid ] = camera                  ;
  CameraUUIDs    [ camera    ] = startUuid               ;
  JoinDestructor ( camera    )                           ;
  camera -> node               = startUuid               ;
  return startUuid                                       ;
}

void N::VideoEditor::Close(CameraBox * camera)
{
  DeleteUniverse ( camera ) ;
}

void N::VideoEditor::EditCamera(EditableUniverse * universe,Camera * camera)
{
  QRectF      vw ( 0 , 0 , width ( ) , height ( ) ) ;
  CameraBox * cb = new CameraBox   ( this         ) ;
  SUID        vb = RequestUniverse ( 93001        ) ;
  QFont       f                                     ;
  f  = plan->fonts [Fonts::ListView] . toQFont (  ) ;
  cb -> setLength                  ( 240 , f      ) ;
  addUniverse                      ( vb  , cb     ) ;
  cb -> Relocation                 ( vw           ) ;
  nConnect ( cb   , SIGNAL ( Close (CameraBox*) )   ,
             this , SLOT   ( Close (CameraBox*) ) ) ;
}

void N::VideoEditor::Close(ContainEditor * cpe)
{
  if ( IsNull ( cpe        ) ) return ;
  if ( IsNull ( cpe -> vcf ) ) return ;
  EraseVIF    ( cpe -> vcf )          ;
}

void N::VideoEditor::RenderSequences(QString title,SceneObject * obj,SMAPs * maps)
{ Q_UNUSED ( obj )
  QPoint pos ( width ( ) / 2 , height ( ) / 2 )                     ;
  ///////////////////////////////////////////////////////////////////
  RenderingSequences * rse  = NULL                                  ;
  ContainEditor      * ce   = NULL                                  ;
  VcfInterface       * vine = NULL                                  ;
  vine         = NewInterface           ( NULL        )             ;
  rse          = new RenderingSequences ( NULL , plan )             ;
  rse -> maps = maps                                                ;
  rse -> prepare        (                    )                      ;
  rse -> setWindowTitle ( title              )                      ;
  rse -> setGeometry    ( QRect(0,0,240,480) )                      ;
  ///////////////////////////////////////////////////////////////////
  ce           = new ContainEditor ( NULL , plan )                  ;
  ce  -> vcf   = vine                                               ;
  ce  -> setWindowTitle            ( title       )                  ;
  ce  -> addStacked                ( rse         )                  ;
  ce  -> setStacked                ( rse         )                  ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( ce   , SIGNAL ( Close ( ContainEditor * ) )            ,
             this , SLOT   ( Close ( ContainEditor * ) )          ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = ce -> geometry ( )                                   ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter           ( GP . toPoint ( )  )      ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = ce -> windowTitle (                   )      ;
  vine -> setOptions                     ( Options , true    )      ;
  vine -> setZValue                      ( 1.00f             )      ;
  vine -> setOpacity                     ( 0.60              )      ;
  vine -> setWidget                      ( ce , GP , RF , RC )      ;
  vine -> adjustTransform                (                   )      ;
  vine -> setFunction                    ( 32001 , false     )      ;
  vine -> setFunction                    ( 32002 , false     )      ;
  vine -> setFunction                    ( 33001 , false     )      ;
  vine -> setFunction                    ( 33002 , false     )      ;
  vine -> setFunction                    ( 33003 , false     )      ;
  vine -> setFunction                    ( 33004 , false     )      ;
  vine -> setFunction                    ( 33005 , false     )      ;
  vine -> setFunction                    ( 33006 , false     )      ;
  vine -> setFunction                    ( 33007 , false     )      ;
  vine -> setFunction                    ( 33008 , false     )      ;
  Append                                 ( vine , NULL       )      ;
  vine -> AdjustHeader                   (                   )      ;
}

void N::VideoEditor::EditFTP(int evType,SUID ev,bool panel)
{
  FileTransferUI * FTU   = NULL                                     ;
  QString          title = ""                                       ;
  switch ( evType )                                                 {
    case N::Download                                                : // Download details
      FTU   = new FileTransferUI ( NULL , plan )                    ;
      title = tr("Download")                                        ;
      if ( panel )                                                  {
        plan -> addControl  ( title , FTU , this )                  ;
        plan -> pad -> Show ( true               )                  ;
        FTU  -> setEvent    ( N::Download , ev   )                  ;
        return                                                      ;
      }                                                             ;
    break                                                           ;
    case N::Upload                                                  : // Upload details
      FTU   = new FileTransferUI ( NULL , plan )                    ;
      title = tr("Upload")                                          ;
      if ( panel )                                                  {
        plan -> addControl  ( title , FTU , this )                  ;
        plan -> pad -> Show ( true               )                  ;
        FTU  -> setEvent    ( N::Upload   , ev   )                  ;
        return                                                      ;
      }                                                             ;
    break                                                           ;
    default                                                         :
    return                                                          ;
  }                                                                 ;
  if ( IsNull ( FTU ) ) return                                      ;
  ///////////////////////////////////////////////////////////////////
  QPoint pos ( width ( ) / 2 , height ( ) / 2 )                     ;
  ///////////////////////////////////////////////////////////////////
  ContainEditor * ce   = NULL                                       ;
  VcfInterface  * vine = NULL                                       ;
  ///////////////////////////////////////////////////////////////////
  vine         = NewInterface           ( NULL        )             ;
  ce           = new ContainEditor ( NULL , plan )                  ;
  ce  -> vcf   = vine                                               ;
  ce  -> setWindowTitle            ( title       )                  ;
  ce  -> addStacked                ( FTU         )                  ;
  ce  -> setStacked                ( FTU         )                  ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( ce   , SIGNAL ( Close ( ContainEditor * ) )            ,
             this , SLOT   ( Close ( ContainEditor * ) )          ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = ce -> geometry ( )                                   ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )               ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = ce -> windowTitle ( )                        ;
  vine -> setOptions           ( Options , true    )                ;
  vine -> setZValue            ( 1.00f             )                ;
  vine -> setOpacity           ( 0.60              )                ;
  vine -> setWidget            ( ce , GP , RF , RC )                ;
  vine -> adjustTransform      (                   )                ;
  vine -> setFunction          ( 32001 , false     )                ;
  vine -> setFunction          ( 32002 , false     )                ;
  vine -> setFunction          ( 33001 , false     )                ;
  vine -> setFunction          ( 33002 , false     )                ;
  vine -> setFunction          ( 33003 , false     )                ;
  vine -> setFunction          ( 33004 , false     )                ;
  vine -> setFunction          ( 33005 , false     )                ;
  vine -> setFunction          ( 33006 , false     )                ;
  vine -> setFunction          ( 33007 , false     )                ;
  vine -> setFunction          ( 33008 , false     )                ;
  Append                       ( vine , NULL       )                ;
  vine -> AdjustHeader         (                   )                ;
  ///////////////////////////////////////////////////////////////////
  switch ( evType )                                                 {
    case N::Download                                                : // Download details
      FTU  -> setEvent (N::Download,ev)                             ;
    break                                                           ;
    case N::Upload                                                  : // Upload details
      FTU  -> setEvent (N::Upload,ev)                               ;
    break                                                           ;
  }                                                                 ;
}

void N::VideoEditor::EventHandler(int ServiceId,SUID ev)
{
  switch ( ServiceId )                         {
    case 40001                                 : // Event note
    break                                      ;
    case 40002                                 : // Event tasks
    break                                      ;
    case 40003                                 : // Decisions
    break                                      ;
    case 40401                                 : // Edit meeting details
    break                                      ;
    case 40501                                 : // Edit automation settings
    break                                      ;
    case 40701                                 : // Edit file sync details
    break                                      ;
    case 40801                                 : // Download details
      EditFTP ( N::Download , ev )             ;
    break                                      ;
    case 40901                                 : // Upload details
      EditFTP ( N::Upload   , ev )             ;
    break                                      ;
    case 41001                                 : // Assign music
    break                                      ;
    case 41101                                 : // Schedule video clip
    break                                      ;
    case 41401                                 : // Backup plan
    break                                      ;
    case 41501                                 : // File update plan
    break                                      ;
    case 41601                                 : // Recipts
    break                                      ;
    case 41901                                 : // Shopping list
    break                                      ;
    case 42001                                 : // Communication profile
    break                                      ;
  }                                            ;
}

void N::VideoEditor::EditPoint(QString title,EditableUniverse * universe,ControlPoint * point)
{ Q_UNUSED ( universe )
  QPoint pos ( width ( ) / 2 , height ( ) / 2 )                     ;
  ///////////////////////////////////////////////////////////////////
  ControlPointEditor * cpe  = NULL                                  ;
  ContainEditor      * ce   = NULL                                  ;
  VcfInterface       * vine = NULL                                  ;
  vine         = NewInterface           ( NULL        )             ;
  cpe          = new ControlPointEditor ( NULL , plan )             ;
  cpe -> point = point                                              ;
  cpe -> prepare        (               )                           ;
  cpe -> setWindowTitle ( title         )                           ;
  ///////////////////////////////////////////////////////////////////
  ce           = new ContainEditor ( NULL , plan )                  ;
  ce  -> vcf   = vine                                               ;
  ce  -> setWindowTitle            ( title       )                  ;
  ce  -> addStacked                ( cpe         )                  ;
  ce  -> setStacked                ( cpe         )                  ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( ce   , SIGNAL ( Close ( ContainEditor * ) )            ,
             this , SLOT   ( Close ( ContainEditor * ) )          ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = ce -> geometry ( )                                   ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )               ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = ce -> windowTitle ( )                        ;
  vine -> setOptions           ( Options , true    )                ;
  vine -> setZValue            ( 1.00f             )                ;
  vine -> setOpacity           ( 0.60              )                ;
  vine -> setWidget            ( ce , GP , RF , RC )                ;
  vine -> adjustTransform      (                   )                ;
  vine -> setFunction          ( 32001 , false     )                ;
  vine -> setFunction          ( 32002 , false     )                ;
  vine -> setFunction          ( 33001 , false     )                ;
  vine -> setFunction          ( 33002 , false     )                ;
  vine -> setFunction          ( 33003 , false     )                ;
  vine -> setFunction          ( 33004 , false     )                ;
  vine -> setFunction          ( 33005 , false     )                ;
  vine -> setFunction          ( 33006 , false     )                ;
  vine -> setFunction          ( 33007 , false     )                ;
  vine -> setFunction          ( 33008 , false     )                ;
  Append                       ( vine , NULL       )                ;
  vine     -> AdjustHeader     (                   )                ;
  universe -> addVcf           ( vine              )                ;
}

N::GroupUniverse * N::VideoEditor::RequestWorld(void)
{
  UUIDs U = Universes . keys ( )                                             ;
  SUID  u                                                                    ;
  foreach ( u , U )                                                          {
    if ( Universes [ u ] -> containsType ( 53002 ) )                         {
      return (GroupUniverse *) Universes [ u ]                               ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  QRect           gr = geometry          (                           )       ;
  QRectF          vw                     ( 0 , 0 , width(),height()  )       ;
  GroupUniverse * vu = new GroupUniverse ( NULL , plan , destructor  )       ;
  SUID            vd = RequestUniverse   ( 11001                     )       ;
  ////////////////////////////////////////////////////////////////////////////
  vu     -> gui                                      = this                  ;
  vu     -> DynamicFunctions [ "EventConnector"    ] = EventConnector        ;
  vu     -> DynamicFunctions [ "PeriodConnector"   ] = PeriodConnector       ;
  vu     -> DynamicFunctions [ "CalendarConnector" ] = CalendarConnector     ;
  vu     -> DynamicFunctions [ "PeriodActions"     ] = PeriodActions         ;
  vu     -> DynamicFunctions [ "AttacheCalendar"   ] = AttacheMenu           ;
  vu     -> DynamicFunctions [ "AttacheEvent"      ] = AttacheMenu           ;
  vu     -> DynamicFunctions [ "AttachePeriod"     ] = AttacheMenu           ;
  CorpFunctions            ( vu                                 )            ;
  BindingMenu              ( vu                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  vu     -> setFunction    ( Universe::FollowWindowSize , true  )            ;
  vu     -> setFunction    ( Universe::WindowMovable    , false )            ;
  vu     -> setFunction    ( Universe::HasUniverseMenu  , true  )            ;
  vu     -> setActivate    ( Universe::MouseDClick      , true  )            ;
  vu     -> setVariable    ( "Hover"                    , true  )            ;
  ////////////////////////////////////////////////////////////////////////////
  vu     -> BuildCameras   ( 90001                              )            ;
  vu     -> SelectCamera   ( "Parking" , gr                     )            ;
  ////////////////////////////////////////////////////////////////////////////
  vu     -> node = vd                                                        ;
  addUniverse              ( vd  , vu                           )            ;
  vu     -> Relocation     ( vw                                 )            ;
  ////////////////////////////////////////////////////////////////////////////
  nConnect ( vu   , SIGNAL ( Close          (Universe*) )                    ,
             this , SLOT   ( DeleteUniverse (Universe*) )                  ) ;
  nConnect ( vu   , SIGNAL ( EditPoint(QString,EditableUniverse*,ControlPoint*))  ,
             this , SLOT   ( EditPoint(QString,EditableUniverse*,ControlPoint*))) ;
  nConnect ( vu   , SIGNAL ( RenderSequences(QString,SceneObject*,SMAPs*))   ,
             this , SLOT   ( RenderSequences(QString,SceneObject*,SMAPs*)) ) ;
  nConnect ( vu   , SIGNAL ( Albums      (SUID) )                            ,
             this , SLOT   ( ImportAlbums(SUID) )                          ) ;
  nConnect ( vu   , SIGNAL ( EditCamera(EditableUniverse*,Camera*))          ,
             this , SLOT   ( EditCamera(EditableUniverse*,Camera*))        ) ;
  ////////////////////////////////////////////////////////////////////////////
  return vu                                                                  ;
}

void N::VideoEditor::StartGroup(int id)
{
  GroupUniverse * vu = RequestWorld ( ) ;
  vu -> start ( id )                    ;
}

void N::VideoEditor::ShowAlbums(void)
{
  StartGroup ( CiosMenuId ( 89 , 45 , 92 ) ) ;
}

void N::VideoEditor::ShowCrowds(void)
{
  StartGroup ( CiosMenuId ( 89 , 45 , 93 ) ) ;
}

void N::VideoEditor::ShowGalleries(void)
{
  StartGroup ( CiosMenuId ( 89 , 45 , 91 ) ) ;
}

void N::VideoEditor::ShowScheduler(void)
{
  StartGroup ( CiosMenuId ( 89 , 45 , 98 ) ) ;
}

void N::VideoEditor::EditEvent (
       QObject * calendar     ,
       SUID      uuid         ,
       QDateTime start        ,
       QDateTime end          ,
       bool      panel        )
{
  BriefEvent * be = new BriefEvent ( NULL , plan )        ;
  QPoint       pt = WidgetVcfPos   ( be          )        ;
  /////////////////////////////////////////////////////////
  if ( panel )                                            {
    plan -> addControl  ( be -> windowTitle ( )           ,
                          be                              ,
                          this                          ) ;
    plan -> pad -> Show ( true                          ) ;
    nConnect ( be          , SIGNAL ( Leave(QWidget*) )   ,
               plan -> pad , SLOT   ( Leave(QWidget*) ) ) ;
    return                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  be -> vif = NewInterface ( NULL                       ) ;
  PlaceVIF                 ( pt , be , be -> vif , 0.70 ) ;
  /////////////////////////////////////////////////////////
  be -> vif -> setFunction ( 32001 , false              ) ;
  be -> vif -> setFunction ( 32002 , false              ) ;
  be -> vif -> setFunction ( 33001 , false              ) ;
  be -> vif -> setFunction ( 33002 , false              ) ;
  be -> vif -> setFunction ( 33003 , false              ) ;
  be -> vif -> setFunction ( 33004 , false              ) ;
  be -> vif -> setFunction ( 33005 , false              ) ;
  be -> vif -> setFunction ( 33006 , false              ) ;
  be -> vif -> setFunction ( 33007 , false              ) ;
  be -> vif -> setFunction ( 33008 , false              ) ;
  Append                   ( be -> vif , NULL           ) ;
  be -> vif -> AdjustHeader(                            ) ;
  be -> startup            ( uuid , start , end         ) ;
  /////////////////////////////////////////////////////////
  QVariant p = calendar -> property ( "Type" )            ;
  int      x = 0                                          ;
  if ( p . isValid ( ) )                                  {
    QString sx = p . toString ( )                         ;
    if ( "EventObject"    == sx ) x = 1                   ;
    if ( "CalendarObject" == sx ) x = 2                   ;
  }                                                       ;
  if ( 1 == x )                                           {
    EventObject    * eol = (EventObject    *) calendar    ;
    nConnect ( be       , SIGNAL( Complete ( ) )          ,
               eol      , SLOT  ( startup  ( ) )        ) ;
  } else
  if ( 2 == x )                                           {
    CalendarObject * cal = (CalendarObject *) calendar    ;
    nConnect ( be       , SIGNAL( Complete ( ) )          ,
               cal      , SLOT  ( startup  ( ) )        ) ;
  } else                                                  {
    nConnect ( be       , SIGNAL( Complete ( ) )          ,
               calendar , SLOT  ( startup  ( ) )        ) ;
  }                                                       ;
}

void N::VideoEditor::ListAlbums(void)
{
  UUIDs U = MUIDs [ "Albums" ]                                 ;
  if ( U . count ( ) <= 0 ) return                             ;
  VideoContainer * VC                                          ;
  VC    = new VideoContainer ( this            , plan        ) ;
  VC   -> setFirst           ( 0               , Types::None ) ;
  plan -> addControl         ( windowTitle ( ) , VC , this   ) ;
  VC   -> startup            ( U                             ) ;
}

void N::VideoEditor::Download(int Id,VarArgs args)
{ Q_UNUSED ( Id )
  QString url                                                            ;
  QString filename                                                       ;
  QString agent = XML::RandUserAgents ( )                                ;
  int     t     = 60 * 1000                                              ;
  if ( args     . count  ( ) >  0 ) url      = args [ 0 ] . toString ( ) ;
  if ( args     . count  ( ) >  1 ) filename = args [ 1 ] . toString ( ) ;
  if ( args     . count  ( ) >  2 ) t        = args [ 2 ] . toInt    ( ) ;
  if ( args     . count  ( ) >  3 ) agent    = args [ 3 ] . toString ( ) ;
  if ( url      . length ( ) <= 0 ) return                               ;
  if ( filename . length ( ) <= 0 ) return                               ;
  ////////////////////////////////////////////////////////////////////////
  Ftp   ftp                                                              ;
  QUrl  u ( url )                                                        ;
  QFile f                 ( filename  ) ;
  ftp . Requests [ "user-agent" ] = agent                                ;
  if ( ftp . download ( u , f , t ) )                                    {
  } else                                                                 {
  }                                                                      ;
}

void N::VideoEditor::SchedulePrevious(void)
{
}

void N::VideoEditor::ScheduleNext(void)
{
  if ( MUIDs [ "EventUUID" ] . count ( ) > 0 )    {
    VarArgs args                                  ;
    SUID    u = MUIDs [ "EventUUID" ] . first ( ) ;
    MUIDs [ "EventUUID" ] . takeAt ( 0 )          ;
    args << u                                     ;
    start ( 52002 , args )                        ;
    return                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  if ( MUIDs [ "Videos" ] . count ( ) > 0 )       {
    VarArgs args                                  ;
    SUID    u = MUIDs [ "Videos" ] . first ( )    ;
    QString n = QString::number(u)                ;
    MUIDs [ "Videos" ] . takeAt ( 0 )             ;
    args << u                                     ; // video(uuid)
    if ( MUIDs . contains ( n ) )                 {
      if ( MUIDs [ n ] . count ( ) > 0 )          {
        args << MUIDs [ n ] . first ( )           ;
      } else args << 0                            ;
    } else args << 0                              ; // album(uuid)
    args << ( MUIDs [ "Videos" ] . count () > 0 ) ; // next
    args << true                                  ; // record
    args << 0                                     ; // description(uuid)
    start ( 50013 , args )                        ;
    return                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  Time::msleep           ( 500 )                  ;
  emit PrivateEmptyShell (     )                  ;
}

void N::VideoEditor::EmitEmptyShell(void)
{
  emit EmptyShell ( ) ;
}

void N::VideoEditor::EmitPlaying(SUID u)
{
  emit PrivateEmitPlaying ( u ) ;
}

void N::VideoEditor::EmitStopped(SUID u)
{
  emit PrivateEmitStopped ( u ) ;
}

void N::VideoEditor::PrivateSayPlaying(SUID u)
{
  emit SayPlaying ( u ) ;
}

void N::VideoEditor::PrivateSayStopped(SUID u)
{
  emit SayStopped ( u ) ;
}

void N::VideoEditor::Finished(VideoUniverse * universe)
{
  if ( ! StreamVideos . contains ( universe ) ) return ;
  StreamVideos . remove ( universe )                   ;
}

void N::VideoEditor::VideoStop(VideoUniverse * universe)
{
  if ( ! StreamVideos . contains ( universe ) ) return ;
  VideoStreaming * vs = StreamVideos [ universe ]      ;
  if ( IsNull ( vs ) ) return                          ;
  vs -> Stop ( )                                       ;
}

void N::VideoEditor::ListUniverses(void)
{
  UUIDs K = Multiverse ( )          ;
  SUID  u                           ;
  foreach ( u , K )                 {
    Speak ( QString::number ( u ) ) ;
  }                                 ;
}

void N::VideoEditor::AddBoard(void)
{
  QRectF     R                                                 ;
  QRectF     P                                                 ;
  QRect      V ( 0 , 0 , width ( ) , height ( ) )              ;
  VcfVideo * vc                                                ;
  //////////////////////////////////////////////////////////////
  P = plan -> toCentimeter ( V )                               ;
  R . setTop           ( ( P . height ( ) / 2 ) - 2.0 )        ;
  R . setLeft          ( ( P . width  ( ) / 2 ) - 2.0 )        ;
  R . setWidth         ( 4.00                         )        ;
  R . setHeight        ( 4.00                         )        ;
  R = Options . Region ( R                            )        ;
  //////////////////////////////////////////////////////////////
  Pen pen                                                      ;
  pen . setColor  ( QColor ( 192 , 255 , 192 , 192 ) )         ;
  pen . setWidthF ( 4.2                        )               ;
  vc = new VcfVideo    ( this , NULL , plan )                  ;
  addItem              ( vc   , NULL        )                  ;
  scene ( ) -> addItem ( vc                 )                  ;
  vc -> Mode    = VcfVideo::Board                              ;
  vc -> Options = &Options                                     ;
  nConnect ( vc   , SIGNAL (Menu        (VcfVideo*,QPointF))   ,
             this , SLOT   (VcfVideoMenu(VcfVideo*,QPointF)) ) ;
  vc -> setCanvas   ( R             )                          ;
  vc -> setZValue   ( 0.8           )                          ;
  vc -> setOpacity  ( 0.25f         )                          ;
  vc -> setFunction ( 32001 , false )                          ;
  vc -> setFunction ( 32002 , true  )                          ;
  vc -> Painter . pens [ 0 ] = pen                             ;
  vc -> show        (               )                          ;
}

void N::VideoEditor::VcfVideoMenu(VcfVideo * canvas,QPointF pos)
{
  nScopedMenu ( mm , this )                                             ;
  QMenu      * mx                                                       ;
  QMenu      * me                                                       ;
  QMenu      * mp                                                       ;
  QMenu      * mv                                                       ;
  QMenu      * mc                                                       ;
  QAction    * aa                                                       ;
  VcfPicture * picture = VcfCasting(VcfPicture,Maps[canvas])            ;
  QList<VcfContours *> contours = ItemContours(canvas)                  ;
  mx = mm . addMenu(tr("Functionality"                  ))              ;
  if ( canvas->Mode == VcfVideo::Board  )                              {
    mm . add(mx,90001,tr("Border"                       ))              ;
  }                                                                     ;
  if ( canvas->Mode == VcfVideo::Border )                              {
    mm . add(mx,90002,tr("Board"                        ))              ;
  }                                                                     ;
  canvas -> addAdjustMenu   ( mm )                                      ;
  me = mm . addMenu(tr("Edit"                           ))              ;
  mm . add(me,20001,tr("Delete"                         ))              ;
  mm . add(me,20002,tr("Remove from database"           ))              ;
  if (NotNull(picture))                                                 {
    QMenu * msobel                                                      ;
    mp = mm.addMenu(tr("Pictures"                       ))              ;
    mm.add(mp,30001,tr("Crop image within selected area"))              ;
    mm.add(mp,30002,tr("Save selection"                 ))              ;
    mm.addSeparator   (mp                                )              ;
    msobel = mm.addMenu(mp,tr("Sobel"))                                 ;
    mm.add(msobel,30101,tr("Sobel operator"             ))              ;
    mm.add(msobel,30102,tr("Vertical Sobel operator"    ))              ;
    mm.add(msobel,30103,tr("Horizontal Sobel operator"  ))              ;
    mm.add(mp,30003,tr("Canny operator"                 ))              ;
    mm.add(mp,30004,tr("Laplace operator"               ))              ;
    mv = mm.addMenu(tr("Vectorizations"                 ))              ;
    mm.add(mv,40001,tr("Canny edge detection"           ))              ;
    mm.add(mv,40002,tr("Sobel edge detection"           ))              ;
    mm.add(mv,40003,tr("Laplace edge detection"         ))              ;
    mm.addSeparator   (mv                                )              ;
    mm.add(mv,40004,tr("Contours detection"             ))              ;
  }                                                                     ;
  if (contours.count()>0)                                               {
    mc = mm.addMenu(tr("Shapes"                         ))              ;
    mm.add(mc,50001,tr("Save"                           ))              ;
    mm.add(mc,50002,tr("Delete all"                     ))              ;
  }                                                                     ;
  canvas -> addFontMenu     ( mm )                                      ;
  canvas -> addPenMenu      ( mm )                                      ;
  canvas -> addBrushMenu    ( mm )                                      ;
  canvas -> addGradientMenu ( mm )                                      ;
  mm . setFont ( plan )                                                 ;
  aa = mm.exec(QCursor::pos()   )                                       ;
  switch (mm[aa])                                                       {
    case 10001                                                          :
      canvas->setFlag( QGraphicsItem::ItemIsMovable , aa->isChecked() ) ;
    break                                                               ;
    case 10002                                                          :
      canvas -> Scaling = aa->isChecked()                               ;
    break                                                               ;
    case 11001                                                          :
      canvas -> SavePen ( )                                             ;
    break                                                               ;
    case 11002                                                          :
      EditPenColor   ( canvas )                                         ;
    break                                                               ;
    case 12001                                                          :
      canvas -> SaveBrush ( )                                           ;
    break                                                               ;
    case 12002                                                          :
      EditBrushColor ( canvas )                                         ;
    break                                                               ;
    case 13001                                                          :
      canvas -> SaveGradient ( )                                        ;
    break                                                               ;
    case 13002                                                          :
      EditGradient   ( canvas )                                         ;
    break                                                               ;
    case 20001                                                          :
      takeItem (canvas)                                                 ;
    break                                                               ;
    case 20002                                                          :
      if (NotNull(picture)) RemoveSelection ( picture , canvas )        ;
      takeItem (canvas)                                                 ;
    break                                                               ;
    case 30001                                                          :
      if (NotNull(picture)) CropPicture     ( picture , canvas )        ;
    break                                                               ;
    case 30002                                                          :
      SaveSelection     ( picture , canvas )                            ;
    break                                                               ;
    case 30003                                                          :
      CannyPicture      ( picture , canvas )                            ;
    break                                                               ;
    case 30004                                                          :
      LaplacePicture    ( picture , canvas )                            ;
    break                                                               ;
    case 30101                                                          :
      SobelPicture      ( picture , canvas )                            ;
    break                                                               ;
    case 30102                                                          :
      VerticalSobel     ( picture , canvas )                            ;
    break                                                               ;
    case 30103                                                          :
      HorizontalSobel   ( picture , canvas )                            ;
    break                                                               ;
    case 40001                                                          :
      CannyDetection    ( picture , canvas )                            ;
    break                                                               ;
    case 40002                                                          :
      SobelDetection    ( picture , canvas )                            ;
    break                                                               ;
    case 40003                                                          :
      LaplaceDetection  ( picture , canvas )                            ;
    break                                                               ;
    case 40004                                                          :
      ContoursDetection ( picture , canvas )                            ;
    break                                                               ;
    case 50001                                                          :
      canvas -> SaveContours ( )                                        ;
    break                                                               ;
    case 50002                                                          :
      for (int i=0;i<contours.count();i++)                              {
        takeItem ( contours [ i ] )                                     ;
      }                                                                 ;
    break                                                               ;
    case 90001                                                          :
      canvas -> Mode = VcfVideo::Border                                ;
    break                                                               ;
    case 90002                                                          :
      canvas -> Mode = VcfVideo::Board                                 ;
    break                                                               ;
    default                                                             :
      canvas->MenuCommands(mm[aa],aa)                                   ;
    break                                                               ;
  }                                                                     ;
  repaint ( )                                                           ;
}

void N::VideoEditor::PlayList(void)
{
}

void N::VideoEditor::NewAlbum(int Id,VarArgs args)
{ Q_UNUSED ( Id )                                 ;
  if ( args . count ( ) < 3 ) return              ;
  QString name     = args [ 0 ] . toString ( )    ;
  int     language = args [ 1 ] . toInt    ( )    ;
  bool    details  = args [ 2 ] . toBool   ( )    ;
  UUIDs   video                                   ;
  /////////////////////////////////////////////////
  if ( args . count ( ) > 3 )                     {
    for (int i = 3 ; i < args . count ( ) ; i++ ) {
      video << args [ 3 ] . toULongLong ( )       ;
    }                                             ;
  }                                               ;
  /////////////////////////////////////////////////
  bool appended = false                           ;
  SUID auid     = 0                               ;
  emit OnBusy ( )                                 ;
  mutex [ -10001 ] . lock   ( )                   ;
  SqlConnection SC ( plan->sql )                  ;
  if ( SC . open ( "VideoEditor" , "NewAlbum" ) ) {
    VideoManager VM ( plan )                      ;
    SUID         u                                ;
    u = VM . AppendAlbum ( SC , language , name ) ;
    appended = ( u > 0 )                          ;
    if ( appended && ( video . count ( ) > 0 ) )  {
      SUID         v                              ;
      foreach ( v , video ) if ( v > 0 )          {
        VM . AlbumClip ( SC , u , v )             ;
      }                                           ;
    }                                             ;
    auid = u                                      ;
    SC . close    (    )                          ;
  }                                               ;
  SC   . remove   (    )                          ;
  mutex [ -10001 ] . unlock ( )                   ;
  /////////////////////////////////////////////////
  if ( ! appended )                               {
    emit GoRelax ( )                              ;
    Alert ( Error )                               ;
    return                                        ;
  }                                               ;
  if ( auid > 0 ) MUIDs [ "Albums"   ] << auid    ;
  /////////////////////////////////////////////////
  emit GoRelax ( )                                ;
  if ( ! details )                                {
    Alert ( Done )                                ;
    return                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  // emit details
}

void N::VideoEditor::Add(AddNewAlbum * widget)
{
  int     language = widget -> Language  ( ) ;
  QString name     = widget -> AlbumName ( ) ;
  VarArgs args                               ;
  Cancel ( widget )                          ;
  args << name                               ;
  args << language                           ;
  args << false                              ;
  for (int i=0;i<widget->videos.count();i++) {
    args << widget -> videos [ i ]           ;
  }                                          ;
  start ( 50019 , args )                     ;
}

void N::VideoEditor::Cancel(AddNewAlbum * widget)
{
  if ( IsNull ( widget        ) ) return ;
  if ( IsNull ( widget -> vcf ) ) return ;
  takeItem    ( widget -> vcf )          ;
}

void N::VideoEditor::Details(AddNewAlbum * widget)
{
  int     language = widget -> Language  ( ) ;
  QString name     = widget -> AlbumName ( ) ;
  VarArgs args                               ;
  Cancel ( widget )                          ;
  args << name                               ;
  args << language                           ;
  args << true                               ;
  for (int i=0;i<widget->videos.count();i++) {
    args << widget -> videos [ i ]           ;
  }                                          ;
  start ( 50019 , args )                     ;
}

void N::VideoEditor::NewAlbum(void)
{
  UUIDs v        ;
  NewAlbum ( v ) ;
}

void N::VideoEditor::NewAlbum(UUIDs videos)
{
  QPoint pos = mapFromGlobal ( QCursor::pos ( ) )                   ;
  ///////////////////////////////////////////////////////////////////
  AddNewAlbum  * ana  = NULL                                        ;
  VcfInterface * vine = NULL                                        ;
  vine = NewInterface    ( NULL        )                            ;
  ana  = new AddNewAlbum ( NULL , plan )                            ;
  ana -> vcf    = vine                                              ;
  ana -> videos = videos                                            ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( ana  , SIGNAL ( Add     ( AddNewAlbum * ) )            ,
             this , SLOT   ( Add     ( AddNewAlbum * ) )          ) ;
  nConnect ( ana  , SIGNAL ( Cancel  ( AddNewAlbum * ) )            ,
             this , SLOT   ( Cancel  ( AddNewAlbum * ) )          ) ;
  nConnect ( ana  , SIGNAL ( Details ( AddNewAlbum * ) )            ,
             this , SLOT   ( Details ( AddNewAlbum * ) )          ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = ana -> geometry ( )                                  ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )               ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = ana -> windowTitle ( )                       ;
  vine -> setOptions           ( Options , true     )               ;
  vine -> setZValue            ( 1.00f              )               ;
  vine -> setOpacity           ( 0.75               )               ;
  vine -> setWidget            ( ana , GP , RF , RC )               ;
  vine -> adjustTransform      (                    )               ;
  vine -> setFunction          ( 32001 , false      )               ;
  vine -> setFunction          ( 32002 , false      )               ;
  vine -> setFunction          ( 33001 , false      )               ;
  vine -> setFunction          ( 33002 , false      )               ;
  vine -> setFunction          ( 33003 , false      )               ;
  vine -> setFunction          ( 33004 , false      )               ;
  vine -> setFunction          ( 33005 , false      )               ;
  vine -> setFunction          ( 33006 , false      )               ;
  vine -> setFunction          ( 33007 , false      )               ;
  vine -> setFunction          ( 33008 , false      )               ;
  Append                       ( vine , NULL        )               ;
  vine -> AdjustHeader         (                    )               ;
  ana  -> GainFocus            (                    )               ;
}

void N::VideoEditor::AccessAlbum(int Id,VarArgs args)
{ Q_UNUSED ( Id )
  SUID  u = 0                                                  ;
  //////////////////////////////////////////////////////////////
  if ( args . count ( ) > 0 ) u = args [ 0 ] . toULongLong ( ) ;
  //////////////////////////////////////////////////////////////
  if ( u <= 0 ) return                                         ;
  emit OnBusy ( )                                              ;
  VideoManager VM ( plan )                                     ;
  mutex [ -10001 ] . lock   ( )                                ;
  SqlConnection SC ( plan -> sql )                             ;
  if ( SC . open ( __FUNCTION__ , QtUUID::createUuidString() ) ) {
    UUIDs   C                                                  ;
    QString nn                                                 ;
    SUID    z                                                  ;
    nn = SC . getName                                          (
           PlanTable(Names)                                    ,
           "uuid"                                              ,
           vLanguageId                                         ,
           u                                                 ) ;
    if ( nn . length ( ) > 0 ) ObjectNames [ u ] = nn          ;
    C = VM . Clips ( SC , u )                                  ;
    if ( ! MUIDs [ "Albums" ] . contains ( u ) )               {
      MUIDs [ "Albums" ] << u                                  ;
      foreach ( z , C )                                        {
        nn = QString::number ( z )                             ;
        if ( ! MUIDs [ "Videos" ] . contains ( z ) )           {
          MUIDs [ "Videos" ] << z                              ;
          if ( ! MUIDs [ nn ] . contains ( u ) )               {
            MUIDs [ nn ] << u                                  ;
          }                                                    ;
        }                                                      ;
      }                                                        ;
    }                                                          ;
    SC . close    (    )                                       ;
  }                                                            ;
  SC   . remove   (    )                                       ;
  mutex [ -10001 ] . unlock ( )                                ;
  emit GoRelax ( )                                             ;
  Alert ( Done )                                               ;
}

void N::VideoEditor::DescribeAlbum(int Id,VarArgs args)
{
}

void N::VideoEditor::DescribeVideo(int Id,VarArgs args)
{
}

void N::VideoEditor::VideoEvent(int Id,VarArgs args)
{
  SUID evid = args [ 0 ] . toULongLong ( )                ;
  if ( evid <= 0 ) return                                 ;
  if ( MUIDs [ "EventUUID" ] . contains ( evid ) ) return ;
  /////////////////////////////////////////////////////////
  if ( VideosCount ( ) > 0 )                              {
    MUIDs [ "EventUUID" ] << evid                         ;
    return                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  start ( 52002 , args )                                  ;
}

void N::VideoEditor::PlayEvent(int Id,VarArgs args)
{
  SUID evid = args [ 0 ] . toULongLong ( )                                   ;
  if ( evid <= 0 ) return                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  SUID    vcid     = 0                                                       ;
  QString filename = ""                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  EventManager  EM ( plan        )                                           ;
  GroupItems    GI ( plan        )                                           ;
  SqlConnection SC ( plan -> sql )                                           ;
  if ( SC . open ( FunctionString ) )                                        {
    QVariant V                                                               ;
    UUIDs    U                                                               ;
    SUID     u                                                               ;
    //////////////////////////////////////////////////////////////////////////
    if ( EM . LoadVariable ( SC , evid ,"VideoUUID" , V ) )                  {
      vcid = V . toULongLong ( )                                             ;
    }                                                                        ;
    if ( ( vcid <= 0 ) && EM . LoadVariable ( SC , evid ,"VideoFile" , V ) ) {
      filename = V . toString ( )                                            ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( vcid > 0 ) && ( filename . length ( ) <= 0 ) )                      {
    filename = ObtainVideoFile ( vcid )                                      ;
  }                                                                          ;
  if ( filename . length ( ) <= 0 ) return                                   ;
  ////////////////////////////////////////////////////////////////////////////
  // syntax : video(uuid) album(uuid) go_next(bool) record_event(bool) description(uuid) delay(ms) filename(string) event(uuid)
  VarArgs vargs                                                              ;
  vargs << vcid                                                              ;
  vargs << 0                                                                 ;
  vargs << true                                                              ;
  vargs << false                                                             ;
  vargs << 0                                                                 ;
  vargs << 1000                                                              ;
  vargs << filename                                                          ;
  vargs << evid                                                              ;
  start ( 50013 , vargs )                                                    ;
}

void N::VideoEditor::ImportAlbums(SUID uuid)
{
  VarArgs args           ;
  args << uuid           ;
  start ( 50025 , args ) ;
}

void N::VideoEditor::ListCameras(void)
{
}

void N::VideoEditor::StatusClose(int Id,VideoStatus * vsx)
{
  if ( ! Streamings . contains ( Id ) ) return                       ;
  VideoStreaming * vs = Streamings [ Id ]                            ;
  if ( NotNull ( vs -> status ) && NotNull ( vs -> status -> vcf ) ) {
    VcfInterface * vcf = vs -> status -> vcf                         ;
    vs -> status = NULL                                              ;
    takeItem ( vcf )                                                 ;
  }                                                                  ;
}

void N::VideoEditor::ShowStatus(VideoStreaming * vs)
{
  if ( NotNull ( vs -> status ) ) return                            ;
  ///////////////////////////////////////////////////////////////////
  QPoint pos ( width ( ) / 2 , height ( ) / 2 )                     ;
  ///////////////////////////////////////////////////////////////////
  VideoStatus  * vsx  = NULL                                        ;
  VcfInterface * vine = NULL                                        ;
  vine            = NewInterface    ( NULL        )                 ;
  vsx             = new VideoStatus ( NULL , plan )                 ;
  vsx -> vcf      = vine                                            ;
  vsx -> stream   = vs                                              ;
  vs  -> status   = vsx                                             ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( vsx  , SIGNAL ( Close       ( int,VideoStatus * ) )    ,
             this , SLOT   ( StatusClose ( int,VideoStatus * ) )  ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = vsx -> geometry ( )                                  ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )               ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = vsx -> windowTitle ( )                       ;
  vine -> setOptions           ( Options , true     )               ;
  vine -> setZValue            ( 1.00f              )               ;
  vine -> setOpacity           ( 0.60               )               ;
  vine -> setWidget            ( vsx , GP , RF , RC )               ;
  vine -> adjustTransform      (                    )               ;
  vine -> setFunction          ( 32001 , false      )               ;
  vine -> setFunction          ( 32002 , false      )               ;
  vine -> setFunction          ( 33001 , false      )               ;
  vine -> setFunction          ( 33002 , false      )               ;
  vine -> setFunction          ( 33003 , false      )               ;
  vine -> setFunction          ( 33004 , false      )               ;
  vine -> setFunction          ( 33005 , false      )               ;
  vine -> setFunction          ( 33006 , false      )               ;
  vine -> setFunction          ( 33007 , false      )               ;
  vine -> setFunction          ( 33008 , false      )               ;
  Append                       ( vine , NULL        )               ;
  vine -> AdjustHeader         (                    )               ;
  vsx  -> Start                (                    )               ;
}

void N::VideoEditor::RatioChanged(int Id,QRect r)
{
  if ( ! Streamings . contains ( Id ) ) return                               ;
  VideoStreaming * vs = Streamings [ Id ]                                    ;
  if ( IsNull ( vs             ) ) return                                    ;
  if ( IsNull ( vs -> universe ) ) return                                    ;
  ////////////////////////////////////////////////////////////////////////////
  VideoUniverse * universe = vs -> universe                                  ;
  if ( universe -> isFunction  ( VideoUniverse::FollowWindowSize ) )         {
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  QRectF  vw ( 0 , 0 , width() , height() )                                  ;
  QPointF p  = r  .topLeft ( )                                               ;
  double  fw = r . width   ( )                                               ;
  double  fh = r . height  ( )                                               ;
  ////////////////////////////////////////////////////////////////////////////
  fw /= vs -> Dimension . width  ( )                                         ;
  fh /= vs -> Dimension . height ( )                                         ;
  ////////////////////////////////////////////////////////////////////////////
  universe -> Users[75001] = ControlPoint ( p.x    () , p.y     () , 0 , 1 ) ;
  universe -> Users[75002] = ControlPoint ( fw        , -fh        , 0 , 1 ) ;
  universe -> Users[75003] = ControlPoint ( r.width() , r.height() , 0 , 1 ) ;
  universe -> Relocation                  ( vw                             ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! UniverseCanvas . contains ( universe ) ) return                     ;
  VcfVideo * canvas = UniverseCanvas [ universe ]                            ;
  if ( IsNull ( canvas ) ) return                                            ;
  QRectF R                                                                   ;
  R . setLeft   ( r  . left   ( ) + ( vw.width() / 2 )   )                   ;
  R . setTop    ( ( vw . height ( ) / 2 ) -  r . top ( ) )                   ;
  R . setWidth  ( r . width  ( )                         )                   ;
  R . setHeight ( r . height ( )                         )                   ;
  R = plan   -> toCentimeter ( R . toRect ( ) )                              ;
  R = canvas -> toPaper      ( R              )                              ;
  canvas -> setCanvas        ( R              )                              ;
}

void N::VideoEditor::RatioClose(int Id,VideoRatio * vr)
{
  if ( ! Streamings . contains ( Id ) ) return                     ;
  VideoStreaming * vs = Streamings [ Id ]                          ;
  if ( NotNull ( vs -> ratio ) && NotNull ( vs -> ratio -> vcf ) ) {
    VcfInterface * vcf = vs -> ratio -> vcf                        ;
    vs -> ratio = NULL                                             ;
    takeItem ( vcf )                                               ;
  }                                                                ;
}

void N::VideoEditor::DoubleClicked(VideoUniverse  * vu)
{
  if ( vu -> isFunction ( Universe::FollowWindowSize ) )             {
    vu -> setFunction   ( Universe::FollowWindowSize , false       ) ;
    vu -> setFunction   ( Universe::WindowMovable    , true        ) ;
//    emit Normal         (                                          ) ;
  } else                                                             {
    vu -> setFunction   ( Universe::FollowWindowSize , true        ) ;
    vu -> setFunction   ( Universe::WindowMovable    , false       ) ;
//    emit FullScreen     (                                          ) ;
  }                                                                  ;
  #pragma message("This will have a thread message passing problem")
  VideoMovable ( vu , vu -> isFunction ( Universe::WindowMovable ) ) ;
}

void N::VideoEditor::DisplayRatio(VideoUniverse  * vu)
{
  if ( ! StreamVideos . contains ( vu ) ) return ;
  VideoStreaming * vs = StreamVideos [ vu ]      ;
  if ( IsNull(vs) ) return                       ;
  EditRatio ( vs )                               ;
}

void N::VideoEditor::UpdateRatio(VideoUniverse * vu)
{
  if ( ! StreamVideos . contains ( vu ) ) return                 ;
  VideoStreaming * vs = StreamVideos [ vu ]                      ;
  if ( IsNull ( vs          ) ) return                           ;
  if ( IsNull ( vs -> ratio ) ) return                           ;
  VideoRatio * vrx = vs -> ratio                                 ;
  QSize        v   = vs -> Dimension                             ;
  QSize        w   = size            ( )                         ;
  QRect        r                                                 ;
  QPoint       p                                                 ;
  p    . setX       ( vs -> universe -> Users[75001] . x       ) ;
  p    . setY       ( vs -> universe -> Users[75001] . y       ) ;
  r    . setTopLeft ( p                                        ) ;
  r    . setSize    ( vs -> universe -> Rectangle . toSize ( ) ) ;
  vrx -> setRange   ( v , w, r                                 ) ;
}

void N::VideoEditor::EditRatio(VideoStreaming * vs)
{
  if ( NotNull ( vs -> ratio ) ) return                             ;
  ///////////////////////////////////////////////////////////////////
  QPoint pos ( width ( ) / 2 , height ( ) / 2 )                     ;
  ///////////////////////////////////////////////////////////////////
  VideoRatio   * vrx  = NULL                                        ;
  VcfInterface * vine = NULL                                        ;
  vine            = NewInterface    ( NULL        )                 ;
  vrx             = new VideoRatio  ( NULL , plan )                 ;
  vrx -> vcf      = vine                                            ;
  vrx -> ThreadId = vs -> ThreadId                                  ;
  vs  -> ratio    = vrx                                             ;
  ///////////////////////////////////////////////////////////////////
  nConnect ( vrx  , SIGNAL ( Changed      ( int,QRect        ) )    ,
             this , SLOT   ( RatioChanged ( int,QRect        ) )  ) ;
  nConnect ( vrx  , SIGNAL ( Close        ( int,VideoRatio * ) )    ,
             this , SLOT   ( RatioClose   ( int,VideoRatio * ) )  ) ;
  ///////////////////////////////////////////////////////////////////
  QRect   RX = vrx -> geometry ( )                                  ;
  QRect   RN ( 0 ,  0 , RX . width ( )     , RX . height ( )      ) ;
  QRect   RF ( 0 ,  0 , RN . width ( ) + 4 , RN . height ( ) + 28 ) ;
  QRect   RC ( 2 , 26 , RN . width ( )     , RN . height ( )      ) ;
  QRect   RH ( 2 ,  2 , RN . width ( )     ,                   24 ) ;
  QPointF GP = pos                                                  ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )               ;
  vine -> showHeader = true                                         ;
  vine -> Scaling    = false                                        ;
  vine -> HeaderRect = RH                                           ;
  vine -> Title      = vrx -> windowTitle ( )                       ;
  vine -> setOptions           ( Options , true     )               ;
  vine -> setZValue            ( 1.00f              )               ;
  vine -> setOpacity           ( 0.60               )               ;
  vine -> setWidget            ( vrx , GP , RF , RC )               ;
  vine -> adjustTransform      (                    )               ;
  vine -> setFunction          ( 32001 , false      )               ;
  vine -> setFunction          ( 32002 , false      )               ;
  vine -> setFunction          ( 33001 , false      )               ;
  vine -> setFunction          ( 33002 , false      )               ;
  vine -> setFunction          ( 33003 , false      )               ;
  vine -> setFunction          ( 33004 , false      )               ;
  vine -> setFunction          ( 33005 , false      )               ;
  vine -> setFunction          ( 33006 , false      )               ;
  vine -> setFunction          ( 33007 , false      )               ;
  vine -> setFunction          ( 33008 , false      )               ;
  Append                       ( vine , NULL        )               ;
  vine -> AdjustHeader         (                    )               ;
  UpdateRatio                  ( vs -> universe     )               ;
}

void N::VideoEditor::SavePTS(VideoStreaming * vs)
{
  QString filename = vs -> VideoPath ( )       ;
  QString output   = GetPath ( )               ;
  output = QFileDialog::getSaveFileName        (
               this                            ,
               tr("Export timestamps to file") ,
               output                          ,
               tr("Plain text (*.txt)")      ) ;
  if ( output . length ( ) <= 0 ) return       ;
  KeepPath ( output )                          ;
  VarArgs args                                 ;
  args << filename                             ;
  args << output                               ;
  start ( 50017 , args )                       ;
}

void N::VideoEditor::ExportPTS(VarArgs & args)
{
  QString filename = ""                                                  ;
  QString output   = ""                                                  ;
  if ( args . count ( )      >  0 ) filename = args [ 0 ] . toString ( ) ;
  if ( args . count ( )      >  1 ) output   = args [ 1 ] . toString ( ) ;
  if ( filename . length ( ) <= 0 ) return                               ;
  if ( output   . length ( ) <= 0 ) return                               ;
  Alert    ( Action            )                                         ;
  WritePTS ( filename , output )                                         ;
  Alert    ( Done              )                                         ;
}

void N::VideoEditor::WritePTS(QString filename,QString output)
{
  FFmpeg::Decoder decoder                                                    ;
  if ( ! decoder . StartVideo ( filename ) ) return                          ;
  AVPacket AVP                                                               ;
  int      r                                                                 ;
  QFile    F                                                                 ;
  QString  s                                                                 ;
  QString  err                                                               ;
  qint64   pts =  0                                                          ;
  int      Id  = -1                                                          ;
  bool     go  = true                                                        ;
  int      cid = 0                                                           ;
  F . setFileName ( output               )                                   ;
  if ( ! F . open ( QIODevice::WriteOnly ) ) return                          ;
  decoder . Initialize ( &AVP )                                              ;
  s  = filename                                                              ;
  s += "\r\n"                                                                ;
  F . write ( s . toUtf8 ( ) )                                               ;
  for (int i=0;i<(int)decoder.FormatCtx->nb_streams;i++)                     {
    int t = (int)decoder.FormatCtx->streams[i]->codec->codec_type            ;
    s = ""                                                                   ;
    switch ( t )                                                             {
      case AVMEDIA_TYPE_VIDEO                                                :
        s = QString("Index %1 is video channel\r\n").arg(i)                  ;
      break                                                                  ;
      case AVMEDIA_TYPE_AUDIO                                                :
        s = QString("Index %1 is audio channel\r\n").arg(i)                  ;
      break                                                                  ;
      case AVMEDIA_TYPE_SUBTITLE                                             :
        s = QString("Index %1 is subtitle channel\r\n").arg(i)               ;
      break                                                                  ;
      case AVMEDIA_TYPE_DATA                                                 :
        s = QString("Index %1 is data channel\r\n").arg(i)                   ;
      break                                                                  ;
      case AVMEDIA_TYPE_ATTACHMENT                                           :
        s = QString("Index %1 is attachment channel\r\n").arg(i)             ;
      break                                                                  ;
      case AVMEDIA_TYPE_NB                                                   :
        s = QString("Index %1 is nb channel\r\n").arg(i)                     ;
      break                                                                  ;
      case AVMEDIA_TYPE_UNKNOWN                                              :
      default                                                                :
        s = QString("Index %1 is unknown channel\r\n").arg(i)                ;
      break                                                                  ;
    }                                                                        ;
    if ( s . length ( ) > 0 )                                                {
      AVCodec * codec = NULL                                                 ;
      char * v                                                               ;
      v     = decoder.FormatCtx->streams[i]->codec->codec_name               ;
      cid   = decoder.FormatCtx->streams[i]->codec->codec_id                 ;
      F     . write             ( s . toUtf8 ( ) )                           ;
      s     = QString::fromUtf8 ( v              )                           ;
      codec = ::avcodec_find_decoder ( (AVCodecID)cid )                      ;
      if ( NotNull(codec) )                                                  {
        s = QString::fromUtf8 ( codec -> long_name )                         ;
        r = ::avcodec_open2                                                  (
                decoder.FormatCtx->streams[i]->codec                         ,
                codec                                                        ,
                NULL                                                       ) ;
        if ( r >= 0 )                                                        {
          s = QString("<%1> success").arg(s)                                 ;
          ::avcodec_close ( decoder.FormatCtx->streams[i]->codec )           ;
        } else                                                               {
          s = QString("<%1> can not open").arg(s)                            ;
        }                                                                    ;
      } else                                                                 {
        s   = QString("Codec not found")                                     ;
      }                                                                      ;
      s     = QString ( "Codec %1 => %2\r\n" ) . arg ( cid ) . arg ( s )     ;
      F     . write             ( s . toUtf8 ( ) )                           ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  s  = QString("Video frames %1\r\n").arg(decoder.VideoStream->nb_frames)    ;
  F . write ( s . toUtf8 ( ) )                                               ;
  s  = QString("Audio frames %1\r\n").arg(decoder.AudioStream->nb_frames)    ;
  F . write ( s . toUtf8 ( ) )                                               ;
  s  = QString("Video %1 / %2\r\n").arg(decoder.VideoStream->time_base.num).arg(decoder.VideoStream->time_base.den) ;
  F . write ( s . toUtf8 ( ) )                                               ;
  s  = QString("Audio %1 / %2\r\n").arg(decoder.AudioStream->time_base.num).arg(decoder.AudioStream->time_base.den) ;
  F . write ( s . toUtf8 ( ) )                                               ;
  ////////////////////////////////////////////////////////////////////////////
  if ( decoder . VideoStream -> nb_frames > 0 )                              {
    Id = plan -> Progress ( tr("Export PTS") , tr("%v / %m , %p%") )         ;
    if ( Id >= 0 )                                                           {
      plan -> ProgressText ( Id , filename                                 ) ;
      plan -> Start        ( Id , &pts , &go                               ) ;
      plan -> setRange     ( Id , 0    , decoder.VideoStream->nb_frames    ) ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  do                                                                         {
    r = decoder . ReadPacket ( AVP )                                         ;
    if ( 0 == r )                                                            {
      s = QString("%1 %2 %3").arg(AVP.pts,20,10,QChar(' ')).arg(AVP.dts,20,10,QChar(' ')).arg(AVP.duration,20,10,QChar(' ')) ;
      if ( decoder . isAudioChannel ( AVP ) )                                {
        s = QString("A %1\r\n").arg(s)                                       ;
      } else
      if ( decoder . isVideoChannel ( AVP ) )                                {
        s = QString("V %1\r\n").arg(s)                                       ;
        pts++                                                                ;
      } else
      if ( decoder . isSubtitle     ( AVP ) )                                {
        s = QString("S %1\r\n").arg(s)                                       ;
      } else s = ""                                                          ;
    } else                                                                   {
      switch ( r )                                                           {
        case AVERROR_BUFFER_TOO_SMALL                                        :
        case AVERROR_EXTERNAL                                                :
        case AVERROR_BUG2                                                    :
        case AVERROR_BSF_NOT_FOUND                                           :
        case AVERROR_BUG                                                     :
        case AVERROR_DECODER_NOT_FOUND                                       :
        case AVERROR_DEMUXER_NOT_FOUND                                       :
        case AVERROR_ENCODER_NOT_FOUND                                       :
        case AVERROR_EXIT                                                    :
        case AVERROR_FILTER_NOT_FOUND                                        :
        case AVERROR_INVALIDDATA                                             :
        case AVERROR_MUXER_NOT_FOUND                                         :
        case AVERROR_OPTION_NOT_FOUND                                        :
        case AVERROR_PATCHWELCOME                                            :
        case AVERROR_PROTOCOL_NOT_FOUND                                      :
        case AVERROR_STREAM_NOT_FOUND                                        :
        case AVERROR_UNKNOWN                                                 :
        case AVERROR_EXPERIMENTAL                                            :
          err = decoder . ErrorString ( r )                                  ;
          s = QString("E %1 => End of file\r\n") . arg ( r ) . arg ( err )   ;
        break                                                                ;
        case AVERROR_EOF                                                     :
          s = QString("E %1 => End of file\r\n") . arg ( r )                 ;
        break                                                                ;
        default                                                              :
        break                                                                ;
      }                                                                      ;
    }                                                                        ;
    if ( s . length ( ) > 0 )                                                {
      F . write ( s . toUtf8 ( ) )                                           ;
    }                                                                        ;
  } while ( 0 == r )                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Id >= 0 ) plan -> Finish ( Id )                                       ;
  ////////////////////////////////////////////////////////////////////////////
  F       . close ( )                                                        ;
  decoder . Close ( )                                                        ;
}

void N::VideoEditor::PanelMenu(int ThreadId,QPoint pos)
{
  if ( ! Streamings . contains ( ThreadId ) ) return                         ;
  VideoStreaming * vs = Streamings [ ThreadId ]                              ;
  if ( IsNull ( vs )                        ) return                         ;
  VideoUniverse  * vu = vs -> universe                                       ;
  nScopedMenu ( mm , this )                                                  ;
  QAction       * aa   = NULL                                                ;
  QMenu         * ma   = NULL                                                ;
  QMenu         * mv   = NULL                                                ;
  QMenu         * mi   = NULL                                                ;
  QMenu         * md   = NULL                                                ;
  QMenu         * mp   = NULL                                                ;
  QMenu         * mt   = NULL                                                ;
  QString         name = ""                                                  ;
  int             uid  = (int) rUniverses [ vu ]                             ;
  int             vid  = uid                                                 ;
  SpinBox       * sb   = new SpinBox ( NULL , plan )                         ;
  ////////////////////////////////////////////////////////////////////////////
  sb -> setPrefix ( tr("Universe ID : ") )                                   ;
  sb -> setRange  ( 1 , 10000000         )                                   ;
  sb -> setValue  ( uid                  )                                   ;
  sb -> External = &vid                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( vs -> universe ) )                                          {
    name = vs -> universe -> name                                            ;
    if ( name . length ( ) > 30 )                                            {
      name = name . left ( 30 )                                              ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( name . length ( ) > 0 )                                               {
    aa  = mm . add    ( 0 , name )                                           ;
    aa -> setEnabled  ( false    )                                           ;
    mm . addSeparator (          )                                           ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( vs -> album > 0 )                                                     {
    ma = mm . addMenu ( tr("Album") )                                        ;
    mm . add ( ma , 101 , tr("Roles") )                                      ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( vs -> description > 0 )                                               {
    md = mm . addMenu ( tr("Description") )                                  ;
    mm . add ( md , 201 , tr("Add time spot") )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mv = mm . addMenu ( tr("Video") )                                          ;
  if ( vs -> video > 0 )                                                     {
    mm . add ( mv , 301 , tr("Details"          )                          ) ;
  }                                                                          ;
  mm   . add ( mv , 302 , tr("Export Timestamps")                          ) ;
  mm   . add ( mv , 303 , tr("Status"           )                          ) ;
  ////////////////////////////////////////////////////////////////////////////
  mi   = mm . addMenu ( tr("Images") )                                       ;
  mm   . add ( mi , 401 , tr("Capture")            )                         ;
  mm   . add ( mi , 402 , tr("Facial recognition") )                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( vu ) )                                                      {
    QSizeF  RR = vu -> Rectangle                                             ;
    double  ww = RR . width  ( )                                             ;
    double  hh = RR . height ( )                                             ;
    QString ss                                                               ;
    ss     = QString("%1 x %2").arg(ww).arg(hh)                              ;
    md     = mm . addMenu ( tr("Display")   )                                ;
    aa     = mm . add     ( md , -101 , ss  )                                ;
    aa    -> setEnabled   ( false           )                                ;
    mm          . add     ( md , -102 , sb  )                                ;
    if ( ! vs -> universe -> isFunction( VideoUniverse::FollowWindowSize ) ) {
      mm        . add     ( md ,  501 , tr("Ratio") )                        ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mp   = mm . addMenu ( tr("Panel") )                                        ;
  mm   . add ( mp , 601 , tr("Load style sheet")   )                         ;
  mm   . add ( mp , 602 , tr("Save style sheet")   )                         ;
  ////////////////////////////////////////////////////////////////////////////
  mt   = mm . addMenu ( tr("Tools") )                                        ;
  mm   . add ( mt , 701 , tr("Search") )                                     ;
  ////////////////////////////////////////////////////////////////////////////
  aa = mm . exec ( pos )                                                     ;
  if ( uid != vid ) MoveUniverse ( uid , vid )                               ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull ( aa ) ) return                                                ;
  switch ( mm [ aa ] )                                                       {
    case 302                                                                 :
      SavePTS    ( vs )                                                      ;
    break                                                                    ;
    case 303                                                                 :
      ShowStatus ( vs )                                                      ;
    break                                                                    ;
    case 501                                                                 :
      EditRatio  ( vs )                                                      ;
    break                                                                    ;
    case 601                                                                 :
      if ( NotNull ( vs -> controller ) )                                    {
        vs -> controller -> LoadStyleSheet ( )                               ;
      }                                                                      ;
    break                                                                    ;
    case 602                                                                 :
      if ( NotNull ( vs -> controller ) )                                    {
        vs -> controller -> SaveStyleSheet ( )                               ;
      }                                                                      ;
    break                                                                    ;
  }                                                                          ;
}

void N::VideoEditor::RequestPanel(VideoUniverse * vu,bool enabled)
{
  if ( ! StreamVideos . contains ( vu ) ) return                  ;
  VideoStreaming   * vs = StreamVideos [ vu ]                     ;
  StreamController * cc = NULL                                    ;
  if ( IsNull(vs) ) return                                        ;
  /////////////////////////////////////////////////////////////////
  if ( ! enabled )                                                {
    VcfInterface * vcf = vs -> controller -> vcf                  ;
    vs -> controller  -> vcf = NULL                               ;
    vs -> controller  -> Finalize ( )                             ;
    vs -> controller   = NULL                                     ;
    if ( NotNull(vcf) ) vcf -> Finish ( )                         ;
    takeItem ( vcf )                                              ;
    return                                                        ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  VcfInterface * vine = NULL                                      ;
  vine = NewInterface         ( NULL        )                     ;
  cc   = new StreamController ( NULL , plan )                     ;
  cc  -> vcf        = vine                                        ;
  cc  -> stream     = vs                                          ;
  vs  -> controller = cc                                          ;
  /////////////////////////////////////////////////////////////////
  nConnect ( cc   , SIGNAL ( PanelMenu ( int , QPoint ) )         ,
             this , SLOT   ( PanelMenu ( int , QPoint ) )       ) ;
  /////////////////////////////////////////////////////////////////
  QPointF CP ( 1.0 , 1.0 )                                        ;
  QPoint  PP = plan -> toScreen ( CP )                            ;
  QRect   RX = cc -> geometry ( )                                 ;
  int     ph = width ( ) - 4 - ( PP . x ( ) * 2 )                 ;
  QRect   RN ( 0 , 0 , ph                 , RX . height ( )     ) ;
  QRect   RF ( 0 , 0 , RN . width ( ) + 4 , RN . height ( ) + 4 ) ;
  QRect   RC ( 2 , 2 , RN . width ( )     , RN . height ( )     ) ;
  QPointF GP ( 0.0 , 0.0 )                                        ;
  double  p = height ( ) - RF . height ( )                        ;
  vine -> showHeader = false                                      ;
  GP    . setY                 ( p - PP . y ( )     )             ;
  GP    = plan -> toCentimeter ( GP . toPoint ( )   )             ;
  GP    . setX                 ( 1.0                )             ;
  cc   -> resize               ( RN . size ( )      )             ;
  vine -> setOptions           ( Options , true     )             ;
  vine -> setZValue            ( 1.00f              )             ;
  vine -> setOpacity           ( 0.40               )             ;
  vine -> setWidget            ( cc  , GP , RF , RC )             ;
  vine -> adjustTransform      (                    )             ;
  vine -> setFunction          ( 32001 , false      )             ;
  vine -> setFunction          ( 32002 , false      )             ;
  vine -> setFunction          ( 33003 , false      )             ;
  vine -> setFunction          ( 33004 , false      )             ;
  vine -> setFunction          ( 33005 , false      )             ;
  vine -> setFunction          ( 33006 , false      )             ;
  vine -> setFunction          ( 33007 , false      )             ;
  vine -> setFunction          ( 33008 , false      )             ;
  Append                       ( vine , NULL        )             ;
  vine -> AdjustHeader         (                    )             ;
  cc   -> Sync                 (                    )             ;
}

void N::VideoEditor::StayAudio(int tid,VarArgs args)
{
  CiosAudio::Core         * core     = new CiosAudio::Core ( )               ;
  CiosAudio::Stream       * stream   = NULL                                  ;
  CiosAudio::CaSampleFormat sf                                               ;
  int                       deviceId = -1                                    ;
  int                       Id       = -1                                    ;
  int                       fc       =  0                                    ;
  int                       interval = 100                                   ;
  bool                      playing  = false                                 ;
  ////////////////////////////////////////////////////////////////////////////
  core -> Initialize ( )                                                     ;
  if ( args . count ( ) > 0 ) Id       = args [ 0 ] . toInt ( )              ;
  if ( args . count ( ) > 1 ) deviceId = args [ 1 ] . toInt ( )              ;
  if ( Id               < 0 ) return                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( EquipmentIDs . contains ( PurePlan::DefaultAudioOutput ) )            {
    deviceId = EquipmentIDs [ PurePlan::DefaultAudioOutput ]                 ;
  }                                                                          ;
  if ( deviceId < 0 ) deviceId = core -> DefaultOutputDevice ( )             ;
  ////////////////////////////////////////////////////////////////////////////
  #define SS                                                 Streamings [ Id ]
  #define VD                                              ( (VideoData *) SS )
  #define AD                                              ( (AvDecoder *) SS )
  #define STEP                                                      SS -> step
  ////////////////////////////////////////////////////////////////////////////
  SS -> core = core                                                          ;
  sf = Acoustics::toAudioCore ( SS -> AudioBuffer . Format ( ) )             ;
  fc = ( SS -> AudioBuffer . Rate ( ) * interval ) / 1000                    ;
  CiosAudio::StreamParameters ONSP ( deviceId , SS -> Channels ( ) , sf )    ;
  CiosAudio::CaError          rt                                             ;
  bool                        correct = true                                 ;
  ONSP . suggestedLatency = core -> GetDeviceInfo ( deviceId ) -> defaultLowOutputLatency ;
  ////////////////////////////////////////////////////////////////////////////
  CiosAudio::Conduit * conduit = SS -> conduit                               ;
  rt = core -> Open ( &stream                                                ,
                      NULL                                                   ,
                      &ONSP                                                  ,
                      SS -> AudioBuffer . Rate ( )                           ,
                      fc                                                     ,
                      0                                                      ,
                      conduit                                              ) ;
  if ( ( CiosAudio::NoError != rt ) || ( NULL == stream ) ) correct = false  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( correct )                                                             {
    rt = core -> Start ( stream )                                            ;
    if ( CiosAudio::NoError != rt ) correct = false                          ;
    playing = true                                                           ;
    SS -> stream = stream                                                    ;
    //////////////////////////////////////////////////////////////////////////
    if ( correct && stream -> hasVolume ( ) )                                {
      ////////////////////////////////////////////////////////////////////////
      if ( Volume . count ( ) <= 0 )                                         {
        Volume [ -1 ] = 10000.0                                              ;
        Volume [  0 ] = 10000.0                                              ;
        Volume [  1 ] = 10000.0                                              ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( Volume.count()==1 && Volume.contains(-1) )                        {
        stream -> setVolume ( Volume[-1] , -1 )                              ;
      } else                                                                 {
        CUIDs VID = Volume . keys ( )                                        ;
        int   vid                                                            ;
        foreach ( vid , VID ) if ( vid >= 0 )                                {
          stream -> setVolume ( Volume [ vid ] , vid )                       ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    while ( playing && correct && ( 1 == core -> IsActive  ( stream ) )    ) {
      for (int z=0;playing && (z<interval);z++ )                             {
        Time :: msleep ( 1 )                                                 ;
        if ( CiosAudio::Conduit::Complete == conduit -> Output . Result )    {
          playing = false                                                    ;
        }                                                                    ;
        if ( 93 == STEP              ) playing = false                       ;
        if ( Booleans [ "Finalize" ] ) playing = false                       ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SS -> stream  = NULL                                                     ;
    playing = false                                                          ;
    if ( correct && ( 0 != core -> IsStopped ( stream ) ) )                  {
      core -> Stop ( stream )                                                ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  core -> Close     ( stream )                                               ;
  if ( ! Booleans [ "Finalize" ] ) core -> Terminate (        )              ;
  stream        = NULL                                                       ;
  while ( ( ! Booleans [ "Finalize" ] ) && ( STEP != 93 ) )                  {
    Time::msleep ( 1 )                                                       ;
  }                                                                          ;
  Time::msleep(10)                                                           ;
  if ( Booleans [ "Finalize" ] && ( STEP < 94 ) ) STEP = 94 ; else STEP = 94 ;
  ////////////////////////////////////////////////////////////////////////////
  #undef  STEP
  #undef  AD
  #undef  VD
  #undef  SS
}

void N::VideoEditor::RollVideo(int tid,VarArgs args)
{
  int  Id       = -1                                            ;
  int  interval = 10                                            ;
  bool loop     = true                                          ;
  ///////////////////////////////////////////////////////////////
  if ( args . count ( ) > 0 ) Id       = args [ 0 ] . toInt ( ) ;
  if ( Id               < 0 ) return                            ;
  if ( args . count ( ) > 1 ) interval = args [ 1 ] . toInt ( ) ;
  #define SS                                    Streamings [ Id ]
  #define VD                                 ( (VideoData *) SS )
  #define AD                                 ( (AvDecoder *) SS )
  #define STEP                                         SS -> step
  while ( loop )                                                {
    switch ( STEP )                                             {
      case 51                                                   :
      case 52                                                   :
        Time::msleep ( 1 )                                      ;
      break                                                     ;
      case 94                                                   :
        loop = false                                            ;
      break                                                     ;
      default                                                   :
        if ( SS -> hasStream ( ) )                              {
          SS -> RollVideo ( )                                   ;
        } else                                                  {
        }                                                       ;
      break                                                     ;
    }                                                           ;
    if ( Booleans [ "Finalize" ] ) loop = false                 ;
    if ( STEP < 94 ) Time::msleep ( interval )                  ;
  }                                                             ;
  if ( Booleans [ "Finalize" ] )                                {
    while ( STEP < 94 ) Time::msleep ( 10 )                     ;
  }                                                             ;
  STEP = 95                                                     ;
  #undef  STEP
  #undef  AD
  #undef  VD
  #undef  SS
}

void N::VideoEditor::RollAudio(int tid,VarArgs args)
{
  int  Id       = -1                                            ;
  int  interval = 75                                            ;
  bool loop     = true                                          ;
  ///////////////////////////////////////////////////////////////
  if ( args . count ( ) > 0 ) Id       = args [ 0 ] . toInt ( ) ;
  if ( Id               < 0 ) return                            ;
  if ( args . count ( ) > 1 ) interval = args [ 1 ] . toInt ( ) ;
  #define SS                                    Streamings [ Id ]
  #define VD                                 ( (VideoData *) SS )
  #define AD                                 ( (AvDecoder *) SS )
  #define STEP                                         SS -> step
  while ( loop )                                                {
    switch ( STEP )                                             {
      case 51                                                   :
      case 52                                                   :
        Time::msleep ( 1 )                                      ;
      break                                                     ;
      case 93                                                   :
        loop = false                                            ;
      break                                                     ;
      default                                                   :
        if ( SS -> hasWave ( ) )                                {
          SS -> FeedWave ( )                                    ;
        } else                                                  {
          loop = false                                          ;
          SS -> Booleans [ "hasWave" ] = false                  ;
        }                                                       ;
      break                                                     ;
    }                                                           ;
    if ( Booleans [ "Finalize" ] ) loop = false                 ;
    if ( STEP < 93 ) Time::msleep ( interval )                  ;
  }                                                             ;
  #undef  STEP
  #undef  AD
  #undef  VD
  #undef  SS
}

QString N::VideoEditor::ObtainVideoFile(SUID u)
{
  VideoManager VM ( plan )          ;
  QString      filename             ;
  filename = VM . Filename ( u )    ;
  assureObjectNames ( u )           ;
  if ( filename . length ( ) <= 0 ) {
    VideoFailure ( u , "" )         ;
    return ""                       ;
  }                                 ;
  QFileInfo FIE ( filename )        ;
  if ( ! FIE . exists ( ) )         {
    VideoFailure ( u , filename )   ;
    return ""                       ;
  }                                 ;
  return filename                   ;
}

void N::VideoEditor::DeleteVideo(int Id)
{
  if ( ! Streamings . contains ( Id ) ) return                         ;
  VideoStreaming * vs = Streamings [ Id ]                              ;
  if ( IsNull ( vs )                  ) return                         ;
  //////////////////////////////////////////////////////////////////////
  if ( vs -> waves . count ( ) > 0 )                                   {
    QList<WaveUniverse *> wx = vs->waves                               ;
    for (int i = 0 ; i < wx . count ( ) ; i++ )                        {
      WaveUniverse * wu = wx [ i ]                                     ;
      RemoveCanvas     ( wu )                                          ;
      vs -> removeWave ( wu )                                          ;
      DeleteUniverse   ( wu )                                          ;
    }                                                                  ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  if ( NotNull( vs -> universe )  )                                    {
    VideoUniverse * vu = vs -> universe                                ;
    vs -> universe = NULL                                              ;
    RemoveCanvas   ( vu )                                              ;
    DeleteUniverse ( vu )                                              ;
    if ( NotNull ( vs -> canvas ) ) vs -> canvas = NULL                ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  vs -> core = NULL                                                    ;
  //////////////////////////////////////////////////////////////////////
  if ( NotNull ( vs -> ratio ) && NotNull ( vs -> ratio -> vcf ) )     {
    VcfInterface * vcf = vs -> ratio -> vcf                            ;
    vs -> ratio = NULL                                                 ;
    RemoveVCF    ( vcf )                                               ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  if ( NotNull ( vs -> status ) && NotNull ( vs -> status -> vcf ) )   {
    VcfInterface * vcf = vs -> status -> vcf                           ;
    vs -> status -> vcf = NULL                                         ;
    vs -> status -> Finalize ( )                                       ;
    while ( ! vs -> status -> isStopped ( ) ) Time::skip ( 10 )        ;
    vs -> status  = NULL                                               ;
    if ( NotNull(vcf) ) vcf -> Finish ( )                              ;
    RemoveInterface ( vcf )                                            ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  if ( NotNull ( vs -> controller ) )                                  {
    VcfInterface * vcf = vs -> controller -> vcf                       ;
    vs -> controller  -> vcf = NULL                                    ;
    vs -> controller  -> Finalize ( )                                  ;
    while ( ! vs -> controller -> isStopped ( ) ) Time::skip ( 10 )    ;
    vs -> controller   = NULL                                          ;
    if ( NotNull(vcf) ) vcf -> Finish ( )                              ;
    RemoveInterface ( vcf )                                            ;
  }                                                                    ;
}

void N::VideoEditor::Schedule(VideoStreaming * vs)
{
  vs -> Booleans [ "Next" ] = ( MUIDs [ "Videos" ] . count ( ) > 0 ) ;
}

N::VideoStreaming * N::VideoEditor::NewVideoStreaming(void)
{
  return new VideoStreaming ( ) ;
}

N::AudioStreaming * N::VideoEditor::NewAudioStreaming(void)
{
  return new AudioStreaming ( ) ;
}

int N::VideoEditor::VideosCount(void)
{
  return Streamings . count ( ) ;
}

void N::VideoEditor::AccessVideo(int Id,VarArgs args)
{ // syntax : video(uuid) album(uuid) go_next(bool) record_event(bool) description(uuid) delay(ms) filename(string) event(uuid)
  ////////////////////////////////////////////////////////////////////////////
  if ( Streamings . contains ( Id ) ) return                                 ;
  if ( args . count ( ) <= 0        ) return                                 ;
  SUID      u        = args [ 0 ] . toULongLong ( )                          ;
  SUID      a        = 0                                                     ;
  SUID      duid     = 0                                                     ;
  bool      doNext   = true                                                  ;
  bool      doRecord = false                                                 ;
  SUID      euid     = 0                                                     ;
  SUID      evid     = 0                                                     ;
  int       delay    = 0                                                     ;
  QString   filename = ""                                                    ;
  QDateTime RST       = nTimeNow                                             ;
  if ( args . count ( ) > 1 ) a        = args [ 1 ] . toULongLong ( )        ;
  if ( args . count ( ) > 2 ) doNext   = args [ 2 ] . toBool      ( )        ;
  if ( args . count ( ) > 3 ) doRecord = args [ 3 ] . toBool      ( )        ;
  if ( args . count ( ) > 4 ) duid     = args [ 4 ] . toULongLong ( )        ;
  if ( args . count ( ) > 5 ) delay    = args [ 5 ] . toInt       ( )        ;
  if ( args . count ( ) > 6 ) filename = args [ 6 ] . toString    ( )        ;
  if ( args . count ( ) > 7 ) evid     = args [ 7 ] . toULongLong ( )        ;
  if ( delay                ) Time::msleep ( delay )                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( u > 0 ) filename = ObtainVideoFile ( u )                              ;
  if ( filename . length ( ) <= 0 )                                          {
    if ( doNext ) ScheduleNext ( )                                           ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  #define VD                                              ( (VideoData *) vs )
  #define AD                                              ( (AvDecoder *) vs )
  #define STEP                                                      vs -> step
  VideoStreaming * vs = NewVideoStreaming ( )                                ;
  Streamings [ Id ] = vs                                                     ;
  vs -> ThreadId    = Id                                                     ;
  vs -> video       = u                                                      ;
  vs -> album       = a                                                      ;
  vs -> description = duid                                                   ;
  vs -> setFilename ( filename )                                             ;
  vs -> Booleans [ "GoNext" ] = doNext                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! vs -> Peep ( ) )                                                    {
    Streamings . remove ( Id )                                               ;
    delete vs                                                                ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( isFunction ( VeFullScreen ) )                                         {
    emit ForceFullScreen ( )                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( doRecord )                                                            {
    RST = nTimeNow                                                           ;
    QDateTime RET = RST                                                      ;
    qint64    dt  = vs -> msecs ( )                                          ;
    QString   vn  = ""                                                       ;
    int       EVT = N::Video                                                 ;
    if ( ObjectNames . contains ( u ) ) vn = ObjectNames [ u ] ; else        {
      vn = filename                                                          ;
    }                                                                        ;
    RET  = RET . addMSecs ( dt   )                                           ;
    RET  = RET . addMSecs ( 3000 )                                           ;
    euid = N::EventManager::Append ( plan , EVT , vn , RST , RET   )         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  FoundationManager FMX ( NULL , plan )                                      ;
  EventManager      EM  ( plan        )                                      ;
  if ( evid )                                                                {
    RST = nTimeNow                                                           ;
    QDateTime RET = RST                                                      ;
    qint64    dt  = vs -> msecs ( )                                          ;
    int       EVT = N::Video                                                 ;
    RET  = RET . addMSecs ( dt   )                                           ;
    RET  = RET . addMSecs ( 1000 )                                           ;
    //////////////////////////////////////////////////////////////////////////
    SqlConnection SC ( plan -> sql )                                         ;
    StarDate      ST                                                         ;
    StarDate      FT                                                         ;
    ST = RST                                                                 ;
    FT = RET                                                                 ;
    //////////////////////////////////////////////////////////////////////////
    if ( SC . open ( FunctionString ) )                                      {
      EM . UpdateEvent      ( SC                                             ,
                              evid                                           ,
                              N::Calendars::Duration                         ,
                              ST . Stardate                                  ,
                              FT . Stardate                                ) ;
      EM . attachSpot       ( SC                                             ,
                              evid                                           ,
                              N::History::WorkingPeriod                      ,
                              ST . Stardate                                  ,
                              FT . Stardate                                ) ;
      EM . AssureEvent      ( SC , evid , EVT )                              ;
      EM . DeleteScheduling ( SC , evid      )                               ;
      SC . close ( )                                                         ;
    }                                                                        ;
    SC . remove ( )                                                          ;
    //////////////////////////////////////////////////////////////////////////
    FMX . HandleDependency ( evid , Groups::FinalTrigger )                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  bool             loop  = true                                              ;
  int              retry = 0                                                 ;
  AudioStreaming * AS    = NewAudioStreaming ( )                             ;
  ////////////////////////////////////////////////////////////////////////////
  AS -> stream  = vs                                                         ;
  vs -> conduit = AS                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  while ( loop )                                                             {
    if ( Booleans [ "Finalize" ] )                                           {
      if ( ( STEP < 93 ) && NotNull ( vs ) )                                 {
        vs -> Stop ( )                                                       ;
        while ( STEP < 94 ) Time::msleep ( 10 )                              ;
      }                                                                      ;
    }                                                                        ;
    switch ( STEP )                                                          {
      case  0                                                                :
        vs -> Preparation ( )                                                ;
        Schedule ( vs )                                                      ;
        if ( Properties [ "Interval" ] > vs -> vds )                         {
          VarArgs args                                                       ;
          int     vds = vs -> vds                                            ;
          vds -= 4                                                           ;
          if ( vds <= 1 ) vds = 3                                            ;
          Properties [ "Interval" ] = vds                                    ;
          args << 60002                                                      ;
          args << vs -> vds                                                  ;
          PostSequence ( args )                                              ;
        }                                                                    ;
      break                                                                  ;
      case  1                                                                :
        if ( vs -> hasVideo ( ) )                                            {
          VarArgs         vrgs                                               ;
          QRectF          vw ( 0 , 0 , width() , height() )                  ;
          VideoUniverse * vu = AllocateVideo   (       )                     ;
          SUID            vd = RequestUniverse ( 11001 )                     ;
          StreamVideos [ vu ] = vs                                           ;
          vs -> universe = vu                                                ;
          if ( ( u > 0 ) &&  ObjectNames . contains ( u ) )                  {
            vu -> name = ObjectNames [ u ]                                   ;
          }                                                                  ;
          if ( vu -> name . length ( ) <= 0 )                                {
            vu -> name = filename                                            ;
          }                                                                  ;
          vu -> setTransmission ( vs -> transmission )                       ;
          addUniverse           ( vd  , vu           )                       ;
          vu -> Relocation      ( vw                 )                       ;
          vs -> Precache        (                    )                       ;
          Time::msleep          ( 1000               )                       ;
          ////////////////////////////////////////////////////////////////////
          nMountSlot ( vu   , this , DisableMenu   (                   )   ) ;
          nMountSlot ( vu   , this , Finished      (VideoUniverse*     )   ) ;
          nMountSlot ( vu   , this , VideoStop     (VideoUniverse*     )   ) ;
          nMountSlot ( vu   , this , DisplayRatio  (VideoUniverse*     )   ) ;
          nMountSlot ( vu   , this , DoubleClicked (VideoUniverse*     )   ) ;
          nMountSlot ( vu   , this , RequestPanel  (VideoUniverse*,bool)   ) ;
          nMountSlot ( vu   , this , RequestAudio  (VideoUniverse*,bool)   ) ;
          nConnect   ( vu   , SIGNAL (Movable      (VideoUniverse*,bool) )   ,
                       this , SLOT   (VideoMovable (VideoUniverse*,bool) ) ) ;
          ////////////////////////////////////////////////////////////////////
          vrgs << Id                                                         ;
          vrgs << 3                                                          ;
          start ( 50015 , vrgs )                                             ;
          ////////////////////////////////////////////////////////////////////
        }                                                                    ;
        STEP = 2                                                             ;
      break                                                                  ;
      case  2                                                                :
        if ( vs -> hasAudio ( ) )                                            {
          VarArgs args                                                       ;
          args << Id                                                         ;
          if ( EquipmentIDs . contains ( PurePlan::DefaultAudioOutput ) )    {
            args << EquipmentIDs [ PurePlan::DefaultAudioOutput ]            ;
          }                                                                  ;
          start ( 50014 , args )                                             ;
        }                                                                    ;
        STEP = 3                                                             ;
      break                                                                  ;
      case  3                                                                :
        if ( vs -> hasAudio ( ) || vs -> hasVideo ( ) )                      {
          if ( u > 0 ) EmitPlaying ( u )                                     ;
          STEP = 11                                                          ;
          emit Visualize ( )                                                 ;
        } else                                                               {
          STEP = 92                                                          ;
        }                                                                    ;
      break                                                                  ;
      case 11                                                                :
        if ( vs -> Having ( ) )                                              {
          if ( STEP != 93 ) STEP = 12                                        ;
        } else                                                               {
          if ( STEP != 93 ) STEP = 92                                        ;
        }                                                                    ;
      break                                                                  ;
      case 12                                                                :
        if ( vs -> decode ( ) )                                              {
          if ( STEP != 93 ) STEP = 13                                        ;
        } else                                                               {
          if ( STEP != 93 ) STEP = 92                                        ;
        }                                                                    ;
      break                                                                  ;
      case 13                                                                :
        if ( vs -> isFull ( ) )                                              {
          Time::msleep ( 10 )                                                ;
        } else                                                               {
          if ( STEP != 93 ) STEP = 11                                        ;
          Time::msleep (  1 )                                                ;
        }                                                                    ;
      break                                                                  ;
      case 51                                                                :
        Time::msleep ( 1 )                                                   ;
      break                                                                  ;
      case 52                                                                :
        vs -> Locate (   )                                                   ;
        Time::msleep ( 1 )                                                   ;
      break                                                                  ;
      case 91                                                                :
        if ( retry >= 5 ) loop = false ; else                                {
          Time::msleep ( 100 )                                               ;
          STEP = 0                                                           ;
          retry++                                                            ;
        }                                                                    ;
      break                                                                  ;
      case 92                                                                :
        if ( vs -> AudioPending ( ) )                                        {
        } else
        if ( vs -> hasStream    ( ) )                                        {
        } else                                                               {
          vs -> conduit -> Output . Result = CiosAudio::Conduit::Complete    ;
          STEP = 93                                                          ;
        }                                                                    ;
        Time::msleep ( 1 )                                                   ;
      break                                                                  ;
      case 95                                                                :
        DeleteVideo ( Id )                                                   ;
        STEP = 96                                                            ;
      break                                                                  ;
      case 96                                                                :
        STEP = 97                                                            ;
      break                                                                  ;
      case 97                                                                :
        STEP = 98                                                            ;
      break                                                                  ;
      case 98                                                                :
        STEP = 99                                                            ;
      break                                                                  ;
      case 99                                                                :
        Time::msleep ( 1 )                                                   ;
        loop = false                                                         ;
      break                                                                  ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  vs -> cease ( )                                                            ;
  if ( u > 0 ) EmitStopped ( u )                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( ! Booleans [ "Finalize" ] ) && vs -> Booleans [ "GoPrevious" ] )    {
    SchedulePrevious ( )                                                     ;
  } else
  if ( ( ! Booleans [ "Finalize" ] )                                        &&
       ( vs -> Booleans [ "GoNext" ]                                        ||
         Properties [ "Scheduler" ] . toBool ( )                         ) ) {
    ScheduleNext ( )                                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Streamings . remove ( Id )                                                 ;
  delete vs                                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( ! Booleans [ "Finalize" ] ) && doRecord && ( euid > 0 ) )           {
    N::EventManager::Complete   ( plan , euid , RST , nTimeNow )             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( evid )                                                                {
    if ( FMX . isOwner ( evid ) )                                            {
      FMX . CompleteEvent    ( evid                        )                 ;
      FMX . HandleRepeated   ( uuid , Groups::FinalTrigger )                 ;
      FMX . HandleDependency ( evid , Groups::FinalTrigger )                 ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  #undef  STEP
  #undef  AD
  #undef  VD
  #undef  SS
}

#endif
