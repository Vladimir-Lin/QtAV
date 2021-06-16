#include <qtav.h>

N::PlayList:: PlayList     ( QWidget * parent , Plan * p )
            : TreeDock     (           parent ,        p )
            , VideoManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PlayList::~PlayList(void)
{
}

void N::PlayList::Configure(void)
{
  setWindowTitle               (     tr("Play lists")  )               ;
  setAccessibleName            ( "N::PlayList"         )               ;
  setObjectName                ( "N::PlayList"         )               ;
  //////////////////////////////////////////////////////////////////////
  PassDragDrop = false                                                 ;
  //////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                  )               ;
  head -> setText              ( 0 , tr("Name"      )  )               ;
  head -> setText              ( 1 , tr("Length"    )  )               ;
  head -> setText              ( 2 , tr("Schedule"  )  )               ;
  head -> setText              ( 3 , ""                )               ;
  setDragDropMode              ( DragDrop              )               ;
  setRootIsDecorated           ( false                 )               ;
  setAlternatingRowColors      ( true                  )               ;
  setSelectionMode             ( ExtendedSelection     )               ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )               ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )               ;
  setColumnCount               ( 4                     )               ;
  assignHeaderItems            ( head                  )               ;
  setColumnHidden              ( 1 , true              )               ;
  setColumnHidden              ( 2 , true              )               ;
  setColumnWidth               ( 3 , 3                 )               ;
  plan -> setFont              ( this                  )               ;
  //////////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropUrls  , true      )               ;
  setDropFlag                  ( DropVideo , true      )               ;
  setDropFlag                  ( DropAlbum , true      )               ;
  MountClicked                 ( 1                     )               ;
  MountClicked                 ( 2                     )               ;
  //////////////////////////////////////////////////////////////////////
  LimitValues [ 180001    ] = 0                                        ;
  LimitValues [ 180002    ] = 0                                        ;
  //////////////////////////////////////////////////////////////////////
  LocalIcons [ "Play"     ] = QIcon ( ":/images/PlayerPlay.png" )      ;
  LocalIcons [ "Export"   ] = QIcon ( ":/images/export.png"     )      ;
  LocalIcons [ "Clear"    ] = QIcon ( ":/images/undecided.png"  )      ;
  LocalIcons [ "Schedule" ] = QIcon ( ":/images/calendar.png"   )      ;
}

bool N::PlayList::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , Lists       ( ) ) ;
  LinkAction        ( Insert       , New         ( ) ) ;
  LinkAction        ( Delete       , Remove      ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  return true                                          ;
}

bool N::PlayList::hasItem(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  return NotNull ( item )                  ;
}

bool N::PlayList::startDrag(QMouseEvent * e)
{
  QTreeWidgetItem * atItem = itemAt ( e -> pos ( ) )                 ;
  if (   IsNull ( atItem                            ) ) return false ;
  if ( ! IsMask ( e -> buttons ( ) , Qt::LeftButton ) ) return false ;
  dragPoint = e -> pos ( )                                           ;
  if ( ! atItem -> isSelected ( )                     ) return false ;
  if ( ! PassDragDrop                                 ) return true  ;
  return true                                                        ;
}

bool N::PlayList::fetchDrag(QMouseEvent * e)
{
  if ( ! IsMask ( e -> buttons ( ) , Qt::LeftButton ) ) return false   ;
  QPoint pos = e -> pos ( )                                            ;
  pos -= dragPoint                                                     ;
  return ( pos . manhattanLength ( ) > qApp -> startDragDistance ( ) ) ;
}

QMimeData * N::PlayList::dragMime(void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->checkState(0)==Qt::Checked)          {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  if (Uuids.count()<=0)                          {
    QTreeWidgetItem * it = currentItem()         ;
    if (NotNull(it))                             {
      uuid = nTreeUuid(it,0)                     ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "video/uuid"  , uuid  )     ;
  } else                                         {
    setMime ( mime , "video/uuids" , Uuids )     ;
  }                                              ;
  return mime                                    ;
}

void N::PlayList::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::PlayList::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::PlayList::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler(mime)       ;
}

bool N::PlayList::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::PlayList::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::PlayList::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::PlayList::dropUrls(QWidget * source,QPointF pos,const QList<QUrl> & urls)
{ Q_UNUSED ( source )                                                         ;
  Q_UNUSED ( urls   )                                                         ;
  plan->showMessage(tr("Please add clips into database before you play it.")) ;
  Alert ( Error )                                                             ;
  return true                                                                 ;
}

bool N::PlayList::dropVideos(QWidget *,QPointF,const UUIDs & U)
{
  VarArgs V                ;
  toVariants ( U     , V ) ;
  start      ( 10001 , V ) ;
  return true              ;
}

bool N::PlayList::dropAlbums(QWidget *,QPointF,const UUIDs & U)
{
  VarArgs V                ;
  toVariants ( U     , V ) ;
  start      ( 10002 , V ) ;
  return true              ;
}

void N::PlayList::addClip(SqlConnection & Connection,QStringList & Depots,SUID clip)
{
  NewTreeWidgetItem ( item )                                              ;
  QStringList names                                                       ;
  QString     filename                                                    ;
  QString     path                                                        ;
  QString     Q                                                           ;
  qint64      filesize = 0                                                ;
  TUID        duration = 0                                                ;
  QTime       T ( 0 , 0 , 0 , 0 )                                         ;
  Q = Connection.sql.SelectFrom                                           (
        "filesize,actual"                                                 ,
        PlanTable ( Videos )                                              ,
        Connection . WhereUuid ( clip )                                 ) ;
  if ( Connection . Fetch ( Q ) )                                         {
    filesize = Connection . Tuid     ( 0               )                  ;
    duration = Connection . Tuid     ( 1               )                  ;
    T        = T          . addMSecs ( duration / 1000 )                  ;
  }                                                                       ;
  names      = Names            ( Connection , clip                     ) ;
  filename   = Filename         ( Connection , clip                     ) ;
  path       = Find             ( Depots , filename , filesize          ) ;
  item      -> setText          ( 0 , names [ 0 ]                       ) ;
  item      -> setText          ( 1 , T.toString("hh:mm:ss")            ) ;
  item      -> setData          ( 0 , Qt::UserRole , path . toUtf8 ( )  ) ;
  item      -> setData          ( 1 , Qt::UserRole , duration           ) ;
  item      -> setData          ( 2 , Qt::UserRole , clip               ) ;
  item      -> setData          ( 3 , Qt::UserRole , 0                  ) ;
  item      -> setTextAlignment ( 1 , Qt::AlignRight | Qt::AlignVCenter ) ;
  if                            ( path . length ( ) <= 0                ) {
    item    -> setIcon          ( 3 , QIcon(":/images/removefiles.png") ) ;
  }                                                                       ;
  addTopLevelItem               ( item                                  ) ;
}

bool N::PlayList::AddVideos(ThreadData * d)
{
  nKickOut       ( ! IsContinue ( d ) , false )                    ;
  UUIDs U                                                          ;
  FromVariants   ( d -> Arguments , U         )                    ;
  nKickOut       ( U . count ( ) <= 0 , false )                    ;
  //////////////////////////////////////////////////////////////////
  QStringList Depots = plan->settings.getStringList ("VideoDepot") ;
  if ( Depots . count ( ) <= 0 ) return false                      ;
  //////////////////////////////////////////////////////////////////
  EnterSQL       ( SC , plan -> sql           )                    ;
    SUID u                                                         ;
    foreach      ( u , U                      )                    {
      addClip    ( SC , Depots , u            )                    ;
    }                                                              ;
  LeaveSQL       ( SC , plan -> sql           )                    ;
  Arrange        (                            )                    ;
  CheckTotalTime (                            )                    ;
  //////////////////////////////////////////////////////////////////
  return true                                                      ;
}

bool N::PlayList::AddAlbums(ThreadData * d)
{
  nKickOut       ( ! IsContinue ( d ) , false )                    ;
  UUIDs U                                                          ;
  FromVariants   ( d -> Arguments , U         )                    ;
  nKickOut       ( U . count ( ) <= 0 , false )                    ;
  //////////////////////////////////////////////////////////////////
  QStringList Depots = plan->settings.getStringList ("VideoDepot") ;
  if ( Depots . count ( ) <= 0 ) return false                      ;
  //////////////////////////////////////////////////////////////////
  EnterSQL       ( SC , plan -> sql           )                    ;
    UUIDs c                                                        ;
    SUID  u                                                        ;
    SUID  cuid                                                     ;
    foreach      ( u  , U                     )                    {
      c = Clips  ( SC , u                     )                    ;
      foreach    ( cuid , c                   )                    {
        addClip  ( SC , Depots , cuid         )                    ;
      }                                                            ;
    }                                                              ;
  LeaveSQL       ( SC , plan -> sql           )                    ;
  Arrange        (                            )                    ;
  CheckTotalTime (                            )                    ;
  //////////////////////////////////////////////////////////////////
  return true                                                      ;
}

void N::PlayList::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      AddVideos    ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10002                            :
      startLoading (                    ) ;
      AddAlbums    ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::PlayList::startup(void)
{
  return true ;
}

bool N::PlayList::Lists(void)
{
  return true ;
}

bool N::PlayList::New(void)
{
  return true ;
}

void N::PlayList::AcceptShell(void)
{
  if ( 0 == LimitValues [ 180002 ] ) return          ;
  QTreeWidgetItem * it = NULL                        ;
  int               st = 0                           ;
  ////////////////////////////////////////////////////
  for (int i = 0 ; i < topLevelItemCount ( ) ; i++ ) {
    it = topLevelItem ( i )                          ;
    if ( NULL != it )                                {
      st = nTreeInt ( it , 3 )                       ;
      if ( 0 == st )                                 {
        SUID    u = nTreeUuid  ( it , 2 )            ;
        QString n = it -> text (      0 )            ;
        if ( u > 0 )                                 {
          Time::skip ( 1000  )                       ;
          emit Play  ( u , n )                       ;
          return                                     ;
        }                                            ;
      }                                              ;
    }                                                ;
  }                                                  ;
}

void N::PlayList::AcceptPlaying(SUID u)
{
  QTreeWidgetItem * it = uuidAtItem ( u , 2 )                        ;
  if ( NULL == it ) return                                           ;
  QDateTime DT = QDateTime::currentDateTime ( )                      ;
  it -> setText ( 2 , DT . toString ( "yyyy/MM/dd hh:mm:ss" ) )      ;
  it -> setData ( 3 , Qt::UserRole , 1                        )      ;
  it -> setIcon ( 3 , QIcon(":/images/PlayerPlay.png")        )      ;
  if ( ! plan -> Variables . contains ( "PlayList(SaveTo)") ) return ;
  QString filename                                                   ;
  filename = plan -> Variables [ "PlayList(SaveTo)" ] . toString ( ) ;
  SaveTo ( filename )                                                ;
}

void N::PlayList::AcceptStopped(SUID u)
{
  QTreeWidgetItem * it = uuidAtItem ( u , 2 )     ;
  if ( NULL == it ) return                        ;
  it -> setData ( 3 , Qt::UserRole , 2          ) ;
  it -> setIcon ( 3 , QIcon(":/images/yes.png") ) ;
}

bool N::PlayList::Remove(void)
{
  QTreeWidgetItem * item = currentItem ( )    ;
  if ( IsNull ( item )         ) return false ;
  int index = -1                              ;
  index = indexOfTopLevelItem ( item )        ;
  if               ( index < 0 ) return false ;
  takeTopLevelItem ( index     )              ;
  Arrange          (           )              ;
  return true                                 ;
}

void N::PlayList::Arrange(void)
{
  TUID      ts = 0                                                     ;
  QDateTime B  = QDateTime::currentDateTime ( )                        ;
  for (int i=0;i<topLevelItemCount();i++)                              {
    QTreeWidgetItem * item = topLevelItem(i)                           ;
    TUID duration = item -> data ( 1 , Qt::UserRole ) . toLongLong ( ) ;
    QDateTime T = B                                                    ;
    T     = T.addMSecs(ts)                                             ;
    item -> setText(2,T.toString("yyyy/MM/dd hh:mm:ss"))               ;
    ts   += duration / 1000                                            ;
  }                                                                    ;
}

void N::PlayList::Export(void)
{
  QString filename = plan -> Temporary ( "" )                  ;
  if ( plan -> Variables . contains ( "PlayList" ) )           {
    filename = plan -> Variables [ "PlayList" ] . toString ( ) ;
  }                                                            ;
  filename = QFileDialog::getSaveFileName                      (
                       this, tr("Save playlist")               ,
                       filename                                ,
                       tr("Playlists (*.m3u)") )               ;
  if (filename.length()<=0) return                             ;
  QFileInfo FXI ( filename )                                   ;
  QString   pz = FXI . absoluteDir ( ) . absolutePath ( )      ;
  plan -> Variables [ "PlayList" ] = pz                        ;
  //////////////////////////////////////////////////////////////
  QFile   F (filename)                                         ;
  QString L                                                    ;
  if (!F.open(QIODevice::WriteOnly)) return                    ;
  L = "#EXTM3U\r\n"
      "# Playlist created by Neutrino Sakra Video Manager\r\n" ;
  F . write ( L.toUtf8() )                                     ;
  for (int i=0;i<topLevelItemCount();i++)                      {
    QTreeWidgetItem * item = topLevelItem(i)                   ;
    QByteArray bath = item->data(0,Qt::UserRole).toByteArray() ;
    QString    path = QString::fromUtf8(bath)                  ;
    QString    name = item->text(0)                            ;
    QFileInfo  FI(path)                                        ;
    QString    EXTINF                                          ;
    EXTINF  = "#EXTINF:"                                       ;
    EXTINF += name                                             ;
    EXTINF += ","                                              ;
    EXTINF += FI.fileName()                                    ;
    EXTINF += "\r\n"                                           ;
    path   += "\r\n"                                           ;
    F . write ( EXTINF . toUtf8 ( ) )                          ;
    F . write ( path   . toUtf8 ( ) )                          ;
  }                                                            ;
  F . close ( )                                                ;
  alert ( "Done" , "" )                                        ;
}

void N::PlayList::Submit(void)
{
  EventManager  EM ( plan        )                                           ;
  SqlConnection SC ( plan -> sql )                                           ;
  ////////////////////////////////////////////////////////////////////////////
  if ( SC.open(FunctionString))                                              {
    bool    prev                                                             ;
    SUID    pu = 0                                                           ;
    QString Q                                                                ;
    for (int i = 0 ; i < topLevelItemCount ( ) ; i++ )                       {
      QTreeWidgetItem * it = topLevelItem ( i      )                         ;
      QString           nn = it -> text   ( 0      )                         ;
      qint64            dt = it -> data   ( 1 , Qt::UserRole ).toLongLong () ;
      QString           st = it -> text   ( 2      )                         ;
      SUID              uu = nTreeUuid    ( it , 2 )                         ;
      SUID              ev = 0                                               ;
      QDateTime         xt = QDateTime::fromString ( st , "yyyy/MM/dd hh:mm:ss" ) ;
      QDateTime         ft = xt                                              ;
      StarDate          ST                                                   ;
      StarDate          FT                                                   ;
      ////////////////////////////////////////////////////////////////////////
      ft   = ft . addMSecs ( dt / 1000 )                                     ;
      ST   = xt                                                              ;
      FT   = ft                                                              ;
      prev = ( i > 0 )                                                       ;
      ////////////////////////////////////////////////////////////////////////
      ev = EM . AssureUuid ( SC                                            ) ;
      EM . UpdateType      ( SC                                              ,
                             PlanTable ( EventHistory )                      ,
                             ev                                              ,
                             N::Video                                      ) ;
      EM . AssureEvent     ( SC , ev , N::Video                            ) ;
      EM . UpdateType      ( SC                                              ,
                             PlanTable ( Events )                            ,
                             ev                                              ,
                             N::Video                                      ) ;
      SC . assureName      ( PlanTable ( Names )                             ,
                             ev                                              ,
                             plan -> LanguageId                              ,
                             nn                                            ) ;
      EM . UpdateEvent     ( SC                                              ,
                             ev                                              ,
                             N::Calendars::Duration                          ,
                             ST . Stardate                                   ,
                             FT . Stardate                                 ) ;
      EM . attachSpot      ( SC                                              ,
                             ev                                              ,
                             N::History::WorkingPeriod                       ,
                             ST.Stardate                                     ,
                             FT.Stardate                                   ) ;
      ////////////////////////////////////////////////////////////////////////
      if ( uu > 0 )                                                          {
        QVariant V                                                           ;
        V = uu                                                               ;
        EM . SaveVariable ( SC , ev , "VideoUUID" , V )                      ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( plan -> Machine )                                                 {
        Q = QString ( "insert into `%1`"
                      " (`first`,`second`,`t1`,`t2`,`relation`,`position`)"
                      " values (%2,%3,%4,%5,%6,%7) ;"                        )
            . arg   ( PlanTable(Groups)                                      )
            . arg   ( plan -> Machine                                        )
            . arg   ( ev                                                     )
            . arg   ( Types::Equipment                                       )
            . arg   ( Types::Schedule                                        )
            . arg   ( Groups::Subordination                                  )
            . arg   ( 1                                                    ) ;
        SC . Query  ( Q                                                    ) ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( prev && ( pu > 0 ) )                                              {
        qint64 ds                                                            ;
        it = topLevelItem ( i - 1 )                                          ;
        dt = it -> data   ( 1 , Qt::UserRole ) . toLongLong ( )              ;
        st = it -> text   ( 2      )                                         ;
        ft = QDateTime::fromString ( st , "yyyy/MM/dd hh:mm:ss" )            ;
        ft = ft . addMSecs ( dt / 1000 )                                     ;
        ds = ft . secsTo   ( xt        )                                     ;
        Q  = QString ( "insert into `%1`"
                       " (`first`,`second`,`third`,`type`,`flags`)"
                       " values (%2,%3,%4,%5,%6) ;"                          )
             . arg   ( PlanTable(Triples)                                    )
             . arg   ( pu                                                    )
             . arg   ( ev                                                    )
             . arg   ( RelationUUID(Before)                                  )
             . arg   ( Types::Schedule                                       )
             . arg   ( ds                                                  ) ;
        SC . Query   ( Q                                                   ) ;
        //////////////////////////////////////////////////////////////////////
        Q  = QString ( "insert into `%1`"
                       " (`first`,`second`,`third`,`type`,`flags`)"
                       " values (%2,%3,%4,%5,%6) ;"                          )
             . arg   ( PlanTable(Triples)                                    )
             . arg   ( ev                                                    )
             . arg   ( pu                                                    )
             . arg   ( RelationUUID(After)                                   )
             . arg   ( Types::Schedule                                       )
             . arg   ( ds                                                  ) ;
        SC . Query   ( Q                                                   ) ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      EM . AddScheduling   ( SC , ev                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      pu = ev                                                                ;
    }                                                                        ;
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  alert ( "Done" , "" )                                                      ;
}

void N::PlayList::clipChanged(int index)
{
}

void N::PlayList::selectClips(void)
{
}

void N::PlayList::timeChanged(void)
{
}

void N::PlayList::setPlayTime(void)
{
  if (IsNull(ItemEditing)) return                                   ;
  QDateTimeEdit * DTE = Casting(QDateTimeEdit,ItemWidget)           ;
  if (IsNull(DTE )) return                                          ;
  QDateTime         DT = DTE -> dateTime ( )                        ;
  QTreeWidgetItem * it = ItemEditing                                ;
  removeOldItem (      )                                            ;
  ///////////////////////////////////////////////////////////////////
  it -> setText ( 2 , DT . toString ( "yyyy/MM/dd hh:mm:ss" ) )     ;
  ///////////////////////////////////////////////////////////////////
  int index = indexOfTopLevelItem ( it )                            ;
  if ( index >= 0 )                                                 {
    qint64 dt                                                       ;
    dt = it -> data ( 1 , Qt::UserRole ) . toLongLong ( )           ;
    DT = DT . addMSecs ( dt / 1000 )                                ;
    DT = DT . addMSecs ( 50 * 1000 )                                ;
    for (int i = index + 1 ; i < topLevelItemCount ( ) ; i++ )      {
      it = topLevelItem ( i )                                       ;
      it -> setText ( 2 , DT . toString ( "yyyy/MM/dd hh:mm:ss" ) ) ;
      dt = it -> data ( 1 , Qt::UserRole ) . toLongLong ( )         ;
      DT = DT . addMSecs ( dt / 1000 )                              ;
      DT = DT . addMSecs ( 50 * 1000 )                              ;
    }                                                               ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  alert ( "Done" , "" )                                             ;
}

void N::PlayList::singleClicked(QTreeWidgetItem * item,int column)
{
  alert ( "Click" , "" ) ;
}

void N::PlayList::doubleClicked(QTreeWidgetItem * item,int column)
{
  QDateTimeEdit * DTE                                               ;
  QDateTime       CDT                                               ;
  QString         DTS                                               ;
  switch ( column )                                                 {
    case 0                                                          :
    break                                                           ;
    case 1                                                          :
    break                                                           ;
    case 2                                                          :
      DTS = item -> text      ( 2    )                              ;
      DTE = new QDateTimeEdit ( this )                              ;
      if ( DTS . length ( ) > 0 )                                   {
        CDT = QDateTime::fromString ( DTS , "yyyy/MM/dd hh:mm:ss" ) ;
      } else                                                        {
        CDT = nTimeNow                                              ;
      }                                                             ;
      DTE -> setDateTime      ( CDT                   )             ;
      DTE -> setDisplayFormat ( "yyyy/MM/dd hh:mm:ss" )             ;
      setItemWidget           ( item , column , DTE   )             ;
      ItemEditing = item                                            ;
      ItemColumn  = column                                          ;
      ItemWidget  = DTE                                             ;
      nConnect ( DTE  , SIGNAL(editingFinished ( ) )                ,
                 this , SLOT  (setPlayTime     ( ) )              ) ;
    break                                                           ;
  }                                                                 ;
}

void N::PlayList::Play(QString filename)
{
#pragma message("Play(filename) is deprecated")
}

UUIDs N::PlayList::WaitingUUIDs(void)
{
  QTreeWidgetItem * it                               ;
  UUIDs             U                                ;
  SUID              u                                ;
  int               st                               ;
  for (int i = 0 ; i < topLevelItemCount ( ) ; i++ ) {
    it = topLevelItem ( i )                          ;
    if ( NULL != it )                                {
      u  = nTreeUuid ( it , 2 )                      ;
      st = nTreeInt  ( it , 3 )                      ;
      if ( ( u > 0 ) && ( 0 == st ) )                {
        U << u                                       ;
      }                                              ;
    }                                                ;
  }                                                  ;
  return U                                           ;
}

void N::PlayList::SaveTo(QString filename)
{
  UUIDs       U = WaitingUUIDs (              )       ;
  QStringList L = toStrings    ( U            )       ;
  QString     S = L . join     ( "\r\n"       )       ;
  QByteArray  B = S . toUtf8   (              )       ;
  /////////////////////////////////////////////////////
  plan -> Variables [ "PlayList(SaveTo)" ] = filename ;
  File::toFile                 ( filename , B )       ;
}

void N::PlayList::LoadFrom(QString filename)
{
  QStringList L                                       ;
  UUIDs       U                                       ;
  VarArgs     V                                       ;
  L = File::StringList  ( filename , "\n"    )        ;
  if                    ( L . count ( ) <= 0 ) return ;
  L = File::PurifyLines ( L                  )        ;
  U = toUUIDs           ( L                  )        ;
  if                    ( U . count ( ) <= 0 ) return ;
  toVariants            ( U        , V       )        ;
  start                 ( 10001    , V       )        ;
}

void N::PlayList::CheckNext(void)
{
  if ( topLevelItemCount ( ) <= 0 ) return       ;
  CheckTotalTime()                               ;
  QTreeWidgetItem * it = topLevelItem ( 0      ) ;
  SUID              u  = nTreeUuid    ( it , 2 ) ;
  QString           n  = it -> text   ( 0      ) ;
  plan -> processEvents               (        ) ;
  takeTopLevelItem                    ( 0      ) ;
  plan -> processEvents               (        ) ;
  emit Play                           ( u  , n ) ;
  plan -> processEvents               (        ) ;
}

void N::PlayList::CheckTotalTime(void)
{
  int total = 0                                              ;
  for (int i=0;i<topLevelItemCount();i++)                    {
    QString d = topLevelItem(i)->text(1)                     ;
    QTime   Z(0,0,0,0)                                       ;
    QTime   T = QTime::fromString(d)                         ;
    int     s = Z.secsTo(T)                                  ;
    total += s                                               ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  int days                                                   ;
  days  = total / 86400                                      ;
  total = total % 86400                                      ;
  QTime X(0,0,0,0)                                           ;
  X = X.addSecs(total)                                       ;
  ////////////////////////////////////////////////////////////
  QString m                                                  ;
  m = tr("%1 days %2").arg(days).arg(X.toString("hh:mm:ss")) ;
  emit assignTitle ( m )                                     ;
}

void N::PlayList::FileList(void)
{
  QStringList FL                                               ;
  for (int i=0;i<topLevelItemCount();i++)                      {
    QTreeWidgetItem * item = topLevelItem(i)                   ;
    QByteArray B = item->data(0,Qt::UserRole).toByteArray()    ;
    if (B.size()>0) FL << QString::fromUtf8(B)                 ;
  }                                                            ;
  if (FL.count()<=0) return                                    ;
  QString filename = plan -> Temporary ( "" )                  ;
  if ( plan -> Variables . contains ( "PlayList" ) )           {
    filename = plan -> Variables [ "PlayList" ] . toString ( ) ;
  }                                                            ;
  filename = QFileDialog::getSaveFileName                      (
                       this, tr("Save file lists")             ,
                       filename                                ,
                       tr("File list(*.txt)") )                ;
  if (filename.length()<=0) return                             ;
  QFileInfo FXI ( filename )                                   ;
  QString   pz = FXI . absoluteDir ( ) . absolutePath ( )      ;
  plan -> Variables [ "PlayList" ] = pz                        ;
  //////////////////////////////////////////////////////////////
  QString    L = FL . join   ( "\r\n"        )                 ;
  QByteArray B = L  . toUtf8 (               )                 ;
  File::toFile               ( filename , B  )                 ;
  alert                      ( "Done"   , "" )                 ;
}

void N::PlayList::OpList(void)
{
  QStringList FL                                               ;
  for (int i=0;i<topLevelItemCount();i++)                      {
    QTreeWidgetItem * item = topLevelItem(i)                   ;
    QByteArray B = item->data(0,Qt::UserRole).toByteArray()    ;
    if (B.size()>0) FL << QString::fromUtf8(B)                 ;
  }                                                            ;
  if (FL.count()<=0) return                                    ;
  QString filename = plan -> Temporary ( "" )                  ;
  if ( plan -> Variables . contains ( "PlayList" ) )           {
    filename = plan -> Variables [ "PlayList" ] . toString ( ) ;
  }                                                            ;
  filename = QFileDialog::getSaveFileName                      (
                       this, tr("Save operation lists")        ,
                       filename                                ,
                       tr("Operation list(*.bat)\n"
                          "Shell(*.sh)\n"
                          "Anything(*.*)"                  ) ) ;
  if (filename.length()<=0) return                             ;
  QFileInfo FXI ( filename )                                   ;
  QString   pz = FXI . absoluteDir ( ) . absolutePath ( )      ;
  plan -> Variables [ "PlayList" ] = pz                        ;
  //////////////////////////////////////////////////////////////
  QFileInfo FI ( filename )                                    ;
  QFile     F  ( filename )                                    ;
  QString   L                                                  ;
  QString   S      = "\r\n"                                    ;
  QString   Suffix = FI.suffix()                               ;
  if (!F.open(QIODevice::WriteOnly)) return                    ;
  if (Suffix.toLower()=="sh")                                  {
    S = "\n"                                                   ;
    L = "#!/bin/sh\n\n"                                        ;
    F.write(L.toUtf8())                                        ;
  }                                                            ;
  for (int i=0;i<FL.count();i++)                               {
    QString   B = FL[i]                                        ;
    QFileInfo FX(B)                                            ;
    QString   T = FX.fileName()                                ;
    L = QString("OpA %1 OpB %2%3").arg(B).arg(T).arg(S)        ;
    F.write(L.toUtf8())                                        ;
  }                                                            ;
  F . close ( )                                                ;
  alert ( "Done" , "" )                                        ;
}

void N::PlayList::RemoveAll(void)
{
  emit clearItems ( ) ;
}

void N::PlayList::ClearPlayed(void)
{
  TreeWidgetItems   items                            ;
  QTreeWidgetItem * it                               ;
  int               st                               ;
  ////////////////////////////////////////////////////
  for (int i = 0 ; i < topLevelItemCount ( ) ; i++ ) {
    it = topLevelItem ( i )                          ;
    if ( NULL != it )                                {
      st = nTreeInt ( it , 3 )                       ;
      if ( 0 != st ) items << it                     ;
    }                                                ;
  }                                                  ;
  ////////////////////////////////////////////////////
  if ( items . count ( ) <= 0 ) return               ;
  ////////////////////////////////////////////////////
  for (int i = 0 ; i < items . count ( ) ; i++ )     {
    int idx = indexOfTopLevelItem ( items [ i ] )    ;
    if ( idx >= 0 ) takeTopLevelItem ( idx )         ;
  }                                                  ;
  items . clear ( )                                  ;
}

bool N::PlayList::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                                ;
  QTreeWidgetItem * item = itemAt(pos)                                     ;
  QMenu           * ms                                                     ;
  QAction         * a                                                      ;
  if (NotNull(item)) mm . add ( 101 , LocalIcons [ "Play" ] , tr("Play") ) ;
  if ( 0 == LimitValues [ 180001 ] )                                       {
    mm . add ( 102 , tr("Play all"              )                        ) ;
  } else                                                                   {
    if ( 0 == LimitValues [ 180002 ] )                                     {
      mm . add ( 302 , tr("Play all"            )                        ) ;
    }                                                                      ;
    mm . add ( 303                                                         ,
               tr("Accept requesting")                                     ,
               true                                                        ,
               ( 1 == LimitValues [ 180002 ] )                           ) ;
  }                                                                        ;
  mm   . addSeparator ( )                                                  ;
  mm   . add ( 103                                                         ,
               LocalIcons [ "Schedule" ]                                   ,
               tr("Submit to scheduler")                                 ) ;
  mm   . add ( 104 , LocalIcons [ "Export" ] , tr("Export playlist")     ) ;
  mm   . add ( 105 , tr("Export file lists"     )                        ) ;
  mm   . add ( 106 , tr("Export operation lists")                        ) ;
  mm   . addSeparator ( )                                                  ;
  if ( topLevelItemCount ( ) > 0 )                                         {
    mm . add ( 107 , LocalIcons [ "Clear" ] , tr("Clear clips")          ) ;
    mm . add ( 108 , tr("Clear played clips")                            ) ;
  }                                                                        ;
  mm   . addSeparator ( )                                                  ;
  a    = mm . add ( 201 , tr("Length"  )   )                               ;
  a   -> setCheckable ( true               )                               ;
  a   -> setChecked   ( !isColumnHidden(1) )                               ;
  a    = mm . add ( 202 , tr("Schedule") )                                 ;
  a   -> setCheckable ( true               )                               ;
  a   -> setChecked   ( !isColumnHidden(2) )                               ;
  ms   = mm . addMenu ( tr("Sorting")      )                               ;
  SortingMenu ( mm , ms )                                                  ;
  DockingMenu ( mm      )                                                  ;
  mm     . setFont ( plan )                                                ;
  a = mm . exec    (      )                                                ;
  //////////////////////////////////////////////////////////////////////////
  if ( IsNull     ( a      ) ) return true                                 ;
  if ( RunSorting ( mm , a ) ) return true                                 ;
  if ( RunDocking ( mm , a ) ) return true                                 ;
  //////////////////////////////////////////////////////////////////////////
  switch (mm[a])                                                           {
    case 101                                                               :
      emit Play(nTreeUuid(item,2),item->text(0))                           ;
    break                                                                  ;
    case 102                                                               :
      CheckNext   ( )                                                      ;
    break                                                                  ;
    case 103                                                               :
      Submit      ( )                                                      ;
    break                                                                  ;
    case 104                                                               :
      Export      ( )                                                      ;
    break                                                                  ;
    case 105                                                               :
      FileList    ( )                                                      ;
    break                                                                  ;
    case 106                                                               :
      OpList      ( )                                                      ;
    break                                                                  ;
    case 107                                                               :
      clear       ( )                                                      ;
    break                                                                  ;
    case 108                                                               :
      ClearPlayed ( )                                                      ;
    break                                                                  ;
    case 201                                                               :
      setColumnHidden ( 1 , ! a -> isChecked ( ) )                         ;
    break                                                                  ;
    case 202                                                               :
      setColumnHidden ( 2 , ! a -> isChecked ( ) )                         ;
    break                                                                  ;
    case 302                                                               :
      LimitValues [ 180002 ] = 1                                           ;
      AcceptShell ( )                                                      ;
    break                                                                  ;
    case 303                                                               :
      LimitValues [ 180002 ] = a -> isChecked ( ) ? 1 : 0                  ;
    break                                                                  ;
  }                                                                        ;
  return true                                                              ;
}
