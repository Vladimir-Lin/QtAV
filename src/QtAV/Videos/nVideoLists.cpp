#include <qtav.h>

N::VideoLists:: VideoLists   (QWidget * parent,Plan * p)
              : TreeDock     (          parent,       p)
              , VideoManager (                        p)
              , SearchEditor (NULL                     )
              , KeepGo       (false                    )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoLists::~VideoLists(void)
{
}

void N::VideoLists::Configure(void)
{
  PassDragDrop = false                                              ;
  setAccessibleName            ( "N::VideoLists"                  ) ;
  setObjectName                ( "N::VideoLists"                  ) ;
  ///////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            (  head                            ) ;
  head -> setIcon              (  0 , QIcon(":/images/ready.png") ) ;
  head -> setText              (  0 , " "                         ) ;
  head -> setText              (  1 , tr("Name"       )           ) ;
  head -> setText              (  2 , tr("Length"     )           ) ;
  head -> setText              (  3 , tr("Format"     )           ) ;
  head -> setText              (  4 , tr("Size"       )           ) ;
  head -> setText              (  5 , tr("Video"      )           ) ;
  head -> setText              (  6 , tr("Video codec")           ) ;
  head -> setText              (  7 , tr("Audio"      )           ) ;
  head -> setText              (  8 , tr("Audio Codec")           ) ;
  head -> setText              (  9 , tr("Channels"   )           ) ;
  head -> setText              ( 10 , tr("Bitrate"    )           ) ;
  head -> setText              ( 11 , tr("Data Bits"  )           ) ;
  setWindowTitle               (      tr("Video clips")           ) ;
  setDragDropMode              ( DragDrop                         ) ;
  setRootIsDecorated           ( false                            ) ;
  setAlternatingRowColors      ( true                             ) ;
  setSelectionMode             ( ExtendedSelection                ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded            ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded            ) ;
  setColumnCount               ( 12                               ) ;
  setAllAlignments             ( head , Qt::AlignCenter           ) ;
  setHeaderItem                ( head                             ) ;
  setColumnHidden              (  6 , true                        ) ;
  setColumnHidden              (  8 , true                        ) ;
  setColumnHidden              ( 10 , true                        ) ;
  setColumnHidden              ( 11 , true                        ) ;
  resizeColumnToContents       ( 0                                ) ;
  plan -> setFont              ( this                             ) ;
  ///////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropMember , true                ) ;
  ///////////////////////////////////////////////////////////////////
  LocalIcons [ "Finish"  ] = QIcon ( ":/images/finish.png"        ) ;
  LocalIcons [ "Missing" ] = QIcon ( ":/images/imagemissing.png"  ) ;
  LocalIcons [ "Delete"  ] = QIcon ( ":/images/delete.png"        ) ;
  ///////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                                ) ;
  MountClicked                 ( 2                                ) ;
  ///////////////////////////////////////////////////////////////////
  setFunction                  ( 17301 , true                     ) ;
  setFunction                  ( 17302 , true                     ) ;
  setFunction                  ( 17303 , true                     ) ;
  setFunction                  ( 17304 , true                     ) ;
  setFunction                  ( 17305 , true                     ) ;
}

bool N::VideoLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  if                ( isFunction ( 17301 )           ) {
    LinkAction      ( Refresh      , Lists       ( ) ) ;
  }                                                    ;
  if                ( isFunction ( 17302 )           ) {
    LinkAction      ( Search       , Search      ( ) ) ;
  }                                                    ;
  if                ( isFunction ( 17303 )           ) {
    LinkAction      ( Rename       , Rename      ( ) ) ;
  }                                                    ;
  return true                                          ;
}

bool N::VideoLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::VideoLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::VideoLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

QMimeData * N::VideoLists::dragMime(void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->isSelected())                        {
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

void N::VideoLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::VideoLists::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::VideoLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler(mime)       ;
}

bool N::VideoLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::VideoLists::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::VideoLists::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::VideoLists::dropUrls(QWidget * source,QPointF pos,const QList<QUrl> & urls)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( pos    ) ;
  Q_UNUSED ( urls   ) ;
  return true         ;
}

bool N::VideoLists::dropVideos(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( pos    ) ;
  Q_UNUSED ( Uuids  ) ;
  return true         ;
}

bool N::VideoLists::dropAlbums(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( pos    ) ;
  Q_UNUSED ( Uuids  ) ;
  return true         ;
}

void N::VideoLists::singleClicked(QTreeWidgetItem * item,int column)
{
  Alert ( Click ) ;
}

void N::VideoLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column!=1 )                     ;
  removeEditor()                             ;
  QLineEdit * NameEditor = setLineEdit       (
               item                          ,
               column                        ,
               SIGNAL(editingFinished())     ,
               SLOT  (nameFinished   ())   ) ;
  NameEditor -> setFocus(Qt::TabFocusReason) ;
  Alert ( Click )                            ;
}

void N::VideoLists::removeEditor(void)
{
  nDropOut ( IsNull(ItemEditing) )          ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::VideoLists::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                        ;
  QLineEdit * NameEditor = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(NameEditor )) return                        ;
  QString name = NameEditor->text()                      ;
  SUID    uuid = nTreeUuid(ItemEditing,0)                ;
  if (name.length()>0)                                   {
    EnterSQL(SC,plan->sql)                               ;
      QString   Q                                        ;
      SUID nuid = 0                                      ;
      Q = SC.sql.SelectFrom("name",PlanTable(NameMaps)   ,
            QString("where `uuid` = %1 "
                    "order by `prefer` asc limit 0,1"    )
                    .arg(uuid)                         ) ;
      if (SC.Fetch(Q)) nuid = SC.Uuid(0)                 ;
      if (nuid>0)                                        {
        SC.assureName                                    (
          PlanTable(Names)                               ,
          nuid                                           ,
          vLanguageId                                    ,
          name                                         ) ;
      }                                                  ;
    LeaveSQL(SC,plan->sql)                               ;
    ItemEditing->setText(1,name)                         ;
  }                                                      ;
  removeEditor()                                         ;
  Alert ( Done )                                         ;
}

bool N::VideoLists::Load(SqlConnection & SC,QStringList & Depots,QTreeWidgetItem * item,SUID uuid)
{
  QString     Q                                                              ;
  QString     name  = ""                                                     ;
  QStringList names = Names ( SC , uuid )                                    ;
  if ( names . count  ( ) > 0 ) name = names[0]                              ;
  if ( name  . length ( ) > 0 )                                              {
    name  = name . replace ( "\r" , ""   )                                   ;
    name  = name . replace ( "\n" , ""   )                                   ;
    item -> setText        ( 1    , name )                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Q = SC . sql . SelectFrom                                                  (
        "suffix,filesize,filename,audiocodec,videocodec,"
        "duration,actual,width,height,"
        "samplerate,channels,bitrate,bits"                                   ,
        PlanTable(Videos)                                                    ,
        SC . WhereUuid ( uuid )                                            ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! SC . Fetch ( Q ) ) return true                                      ;
  QString   depot  = SC . String ( 2 )                                       ;
  bool      exists = false                                                   ;
  long long fsize  = SC . Tuid   ( 1 )                                       ;
  long long slen   = SC . Tuid   ( 5 )                                       ;
  long long tlen   = SC . Tuid   ( 6 )                                       ;
  QTime     T ( 0 , 0 , 0 )                                                  ;
  QTime     S ( 0 , 0 , 0 )                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  S = T . addMSecs   ( slen / 1000 )                                         ;
  T = T . addMSecs   ( tlen / 1000 )                                         ;
  item -> setData    (  2 , Qt::UserRole , SC . Value ( 2 )                ) ;
  item -> setText    (  2 , tr("%1/%2").arg(T.toString("hh:mm:ss")).arg(S.toString("hh:mm:ss"))) ;
  item -> setText    (  3 , SC . String ( 0 )                              ) ;
  item -> setToolTip (  3 , depot                                          ) ;
  item -> setText    (  4 , QString("%1").arg(fsize)                       ) ;
  item -> setText    (  5 , tr("%1 x %2").arg(SC.Int(7)).arg(SC.Int(8))    ) ;
  item -> setText    (  6 , SC . String ( 4 )                              ) ;
  item -> setText    (  7 , tr("%1 Hz").arg(SC.Int(9))                     ) ;
  item -> setText    (  8 , SC . String ( 3 )                              ) ;
  item -> setText    (  9 , QString("%1").arg(SC.Int(10))                  ) ;
  item -> setText    ( 10 , QString("%1").arg(SC.Int(11))                  ) ;
  item -> setText    ( 11 , QString("%1").arg(SC.Int(12))                  ) ;
  ////////////////////////////////////////////////////////////////////////////
  item -> setTextAlignment (  2 , Qt::AlignRight )                           ;
  item -> setTextAlignment (  3 , Qt::AlignRight )                           ;
  item -> setTextAlignment (  4 , Qt::AlignRight )                           ;
  item -> setTextAlignment (  5 , Qt::AlignRight )                           ;
  item -> setTextAlignment (  7 , Qt::AlignRight )                           ;
  item -> setTextAlignment ( 10 , Qt::AlignRight )                           ;
  ////////////////////////////////////////////////////////////////////////////
  if ( S != T ) item -> setBackgroundColor ( 2 , QColor(255,224,224) )       ;
  exists           = false                                                   ;
  QString filepath = Find ( Depots , depot , fsize )                         ;
  exists = ( filepath.length() > 0 )                                         ;
  if ( exists ) item -> setIcon ( 0 , LocalIcons [ "Finish"  ] )             ;
           else item -> setIcon ( 0 , LocalIcons [ "Missing" ] )             ;
  return true                                                                ;
}

bool N::VideoLists::RemoveClip(SqlConnection & SC,SUID clip)
{
  QStringList T                                                ;
  QString     t                                                ;
  QString     Q                                                ;
  //////////////////////////////////////////////////////////////
  T << PlanTable(Identifiers)                                  ;
  T << PlanTable(UFlags)                                       ;
  T << PlanTable(Variables)                                    ;
  T << PlanTable(Names)                                        ;
  T << PlanTable(NameMaps)                                     ;
  T << PlanTable(Videos)                                       ;
  T << PlanTable(MajorUuid)                                    ;
  //////////////////////////////////////////////////////////////
  Q = SC . sql . DeleteFrom                                    (
        PlanTable(Groups)                                      ,
        QString ( "where `first` = %1 or `second` = %2"        )
        . arg ( clip ) . arg ( clip )                        ) ;
  SC . Query ( Q )                                             ;
  //////////////////////////////////////////////////////////////
  foreach ( t, T )                                             {
    Q = SC . sql . DeleteFrom ( t , SC . WhereUuid ( clip ) )  ;
    if ( ! SC . Query ( Q ) )                                  {
      plan -> Debug ( 60 , SC . q . lastError ( ) . text ( ) ) ;
    }                                                          ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  return true                                                  ;
}

void N::VideoLists::Delete(void)
{
  QStringList Depots = plan->settings.getStringList("VideoDepot") ;
  if (Depots.count()<=0) return                                   ;
  QTreeWidgetItem * item = currentItem()                          ;
  if (IsNull(item)) return                                        ;
  SUID uuid = nTreeUuid(item,0)                                   ;
  if (uuid<=0) return                                             ;
  bool success = false                                            ;
  EnterSQL ( SC , plan -> sql )                                   ;
    QString Q                                                     ;
    QString filename = ""                                         ;
    QString suffix   = ""                                         ;
    QString path     = ""                                         ;
    qint64  filesize = 0                                          ;
    Q = SC.sql.SelectFrom                                         (
          "suffix,filesize,filename"                              ,
          PlanTable(Videos) , SC.WhereUuid(uuid)                ) ;
    if (SC.Fetch(Q))                                              {
      suffix   = SC . String ( 0 )                                ;
      filesize = SC . Value  ( 1 ) . toLongLong ( )               ;
      filename = SC . String ( 2 )                                ;
      if (filename.length()>0) success = true                     ;
    }                                                             ;
    ///////////////////////////////////////////////////////////////
    path = Find ( Depots , filename , filesize )                  ;
    if (path.length()<=0) success = false                         ;
    ///////////////////////////////////////////////////////////////
    if ( ! RemoveClip ( SC , uuid ) ) success = false ; else      {
      takeItem ( item )                                           ;
    }                                                             ;
    if ( success                    ) QFile::remove(path)         ;
  LeaveSQL ( SC , plan->sql )                                     ;
  if ( success )                                                  {
    plan -> Talk ( tr("Video clip deleted") )                     ;
    alert ( "Done" , tr("Video clip deleted") )                   ;
  } else                                                          {
    plan -> Talk ( tr("Video clip can not be deleted") )          ;
    alert ( "Error" , tr("Video clip can not be deleted") )       ;
  }                                                               ;
}

bool N::VideoLists::Lists(void)
{
  clear                  (       ) ;
  resizeColumnToContents ( 0     ) ;
  start                  ( 10001 ) ;
  return true                      ;
}

bool N::VideoLists::Lists(int total)
{
  VarArgs Args                            ;
  Args << total                           ;
  clear                  (              ) ;
  resizeColumnToContents ( 0            ) ;
  start                  ( 10002 , Args ) ;
  return true                             ;
}

bool N::VideoLists::Lists(UUIDs & U)
{
  VarArgs Args                            ;
  SUID    u                               ;
  foreach                ( u , U        ) {
    Args << u                             ;
  }                                       ;
  clear                  (              ) ;
  resizeColumnToContents ( 0            ) ;
  start                  ( 10003 , Args ) ;
  return true                             ;
}

void N::VideoLists::ListAll(void)
{
  QStringList Depots                                         ;
  Depots = plan -> settings . getStringList ( "VideoDepot" ) ;
  if ( Depots . count ( ) <= 0 ) return                      ;
  ////////////////////////////////////////////////////////////
  plan -> StartBusy      (       )                           ;
  setEnabled             ( false )                           ;
  blockSignals           ( true  )                           ;
  EnterSQL(SC,plan->sql)                                     ;
    UUIDs   Uuids                                            ;
    SUID    uuid                                             ;
    qint64  all                                              ;
    qint64  index = 0                                        ;
    bool    go    = true                                     ;
    int     id    = -1                                       ;
    Uuids = SC . Uuids ( PlanTable(Videos)                   ,
                         "uuid"                              ,
                         SC . OrderByDesc ( "id" )         ) ;
    all = Uuids . count ( )                                  ;
    id  = plan->Progress(tr("Load clips"),tr("%v/%m clips")) ;
    plan -> Start    ( id , &index , &go )                   ;
    plan -> setRange ( id , 0      , all )                   ;
    while ( go && ( index < all ) )                          {
      uuid = Uuids [ index ]                                 ;
      NewTreeWidgetItem(item)                                ;
      item -> setData    ( 0 , Qt::UserRole , uuid  )        ;
      item -> setToolTip ( 1 , tr("%1") . arg(uuid) )        ;
      addTopLevelItem ( item                      )          ;
      Load            ( SC , Depots , item , uuid )          ;
      index++                                                ;
    }                                                        ;
    plan->Finish(id)                                         ;
    plan->Talk(tr("%1 video clips").arg(Uuids.count()))      ;
  LeaveSQL(SC,plan->sql)                                     ;
  ////////////////////////////////////////////////////////////
  plan->StopBusy (       )                                   ;
  blockSignals   ( false )                                   ;
  setEnabled     ( true  )                                   ;
  emit AutoFit   (       )                                   ;
  Alert          ( Done  )                                   ;
}

void N::VideoLists::List(int total)
{
  QStringList Depots                                         ;
  Depots = plan -> settings . getStringList ( "VideoDepot" ) ;
  if ( Depots . count ( ) <= 0 ) return                      ;
  ////////////////////////////////////////////////////////////
  plan -> StartBusy      (       )                           ;
  setEnabled             ( false )                           ;
  blockSignals           ( true  )                           ;
  EnterSQL(SC,plan->sql)                                     ;
    UUIDs Uuids                                              ;
    SUID  uuid                                               ;
    int   maxv = total                                       ;
    qint64  all                                              ;
    qint64  index = 0                                        ;
    bool    go    = true                                     ;
    int     id    = -1                                       ;
    Uuids = SC . Uuids                                       (
              PlanTable(Videos)                              ,
              "uuid"                                         ,
              SC.OrderByDesc("id")                         ) ;
    if (maxv>Uuids.count()) maxv = Uuids.count()             ;
    all = Uuids . count ( )                                  ;
    id  = plan->Progress(tr("Load clips"),tr("%v/%m clips")) ;
    plan -> Start    ( id , &index , &go )                   ;
    plan -> setRange ( id , 0      , all )                   ;
    for (int i=0;i<maxv;i++)                                 {
      NewTreeWidgetItem(item)                                ;
      uuid = Uuids[i]                                        ;
      item -> setData    ( 0 , Qt::UserRole , uuid  )        ;
      item -> setToolTip ( 1 , tr("%1") . arg(uuid) )        ;
      addTopLevelItem ( item                      )          ;
      Load            ( SC , Depots , item , uuid )          ;
      index++                                                ;
    }                                                        ;
    plan->Finish(id)                                         ;
    plan->Talk(tr("%1 video clips").arg(Uuids.count()))      ;
  LeaveSQL(SC,plan->sql)                                     ;
  ////////////////////////////////////////////////////////////
  plan->StopBusy (       )                                   ;
  blockSignals   ( false )                                   ;
  setEnabled     ( true  )                                   ;
  emit AutoFit   (       )                                   ;
  Alert          ( Done  )                                   ;
}

void N::VideoLists::List(VarArgs & args)
{
  QStringList Depots                                         ;
  Depots = plan -> settings . getStringList ( "VideoDepot" ) ;
  if ( Depots . count ( ) <= 0 ) return                      ;
  ////////////////////////////////////////////////////////////
  plan -> StartBusy      (       )                           ;
  setEnabled             ( false )                           ;
  blockSignals           ( true  )                           ;
  ////////////////////////////////////////////////////////////
  EnterSQL(SC,plan->sql)                                     ;
    UUIDs  Uuids                                             ;
    SUID   uuid                                              ;
    qint64 all                                               ;
    qint64 index = 0                                         ;
    bool   go    = true                                      ;
    int    id    = -1                                        ;
    for (int i=0;i<args.count();i++)                         {
      Uuids << args [ i ] . toULongLong ( )                  ;
    }                                                        ;
    all = Uuids . count ( )                                  ;
    id  = plan->Progress(tr("Load clips"),tr("%v/%m clips")) ;
    plan -> Start    ( id , &index , &go )                   ;
    plan -> setRange ( id , 0      , all )                   ;
    while ( go && ( index < all ) )                          {
      uuid = Uuids [ index ]                                 ;
      NewTreeWidgetItem(item)                                ;
      item -> setData    ( 0 , Qt::UserRole , uuid  )        ;
      item -> setToolTip ( 1 , tr("%1") . arg(uuid) )        ;
      addTopLevelItem ( item                      )          ;
      Load            ( SC , Depots , item , uuid )          ;
      index++                                                ;
    }                                                        ;
    plan->Finish(id)                                         ;
    plan->Talk(tr("%1 video clips").arg(Uuids.count()))      ;
  LeaveSQL(SC,plan->sql)                                     ;
  ////////////////////////////////////////////////////////////
  plan->StopBusy (       )                                   ;
  blockSignals   ( false )                                   ;
  setEnabled     ( true  )                                   ;
  emit AutoFit   (       )                                   ;
  Alert          ( Done  )                                   ;
}

void N::VideoLists::DistillAll(void)
{
  QTreeWidgetItem * ctem = currentItem ( )    ;
  int               sidx = 0                  ;
  if ( NotNull ( ctem ) )                     {
    sidx = indexOfTopLevelItem ( ctem )       ;
  }                                           ;
  /////////////////////////////////////////////
  qint64 total = topLevelItemCount ( )        ;
  qint64 index = sidx                         ;
  bool   go    = true                         ;
  int    id    = -1                           ;
  id = plan -> Progress ( tr("Distill all")   ,
                          tr("%v/%m clips") ) ;
  plan -> Start    ( id , &index , &go   )    ;
  plan -> setRange ( id , sidx   , total )    ;
  while ( go && ( index < total ) )           {
    Distill ( topLevelItem ( index ) )        ;
  }                                           ;
}

void N::VideoLists::Distill(QTreeWidgetItem * item)
{
  if (isLocked(100)) return                                         ;
  scrollToItem ( item )                                             ;
  QStringList Depots = plan->VideoDepots                            ;
  SUID        uuid   = nTreeUuid(item,0)                            ;
  QString filename   = ""                                           ;
  bool        exists = false                                        ;
  EnterSQL ( SC , plan->sql )                                       ;
    QString Q                                                       ;
    qint64 fsize = 0                                                ;
    Q = SC.sql.SelectFrom                                           (
          "suffix,filesize,filename,samplerate,channels"            ,
          PlanTable(Videos)                                         ,
          SC.WhereUuid(uuid)                                      ) ;
    if (SC.Fetch(Q))                                                {
      fsize        = SC . Value  (1) . toLongLong()                 ;
      filename     = SC . String (2)                                ;
      int sr       = SC . Int    (3)                                ;
      int channels = SC . Int    (4)                                ;
      if (sr      <=0) exists = true                                ;
      if (channels<=0) exists = true                                ;
    }                                                               ;
    if (!exists)                                                    {
      filename = Find ( Depots , filename , fsize )                 ;
      Q = SC.sql.SelectFrom("second" , PlanTable(Groups)            ,
             QString("where first = %1"
                     " and t1 = %2"
                     " and t2 = %3"
                     " and relation = %4"                           )
             .arg(uuid                                              )
             .arg(Types::Video                                      )
             .arg(Types::Audio                                      )
             .arg(Groups::Subordination                             )
          )                                                         ;
      if (SC.Fetch(Q)) exists = true                                ;
    }                                                               ;
  LeaveSQL ( SC , plan->sql )                                       ;
  QString basename = QString("%1.mp3").arg(uuid)                    ;
  QString audio    = plan->Temporary(basename)                      ;
  if (audio   .length()<=0) return                                  ;
  if (filename.length()<=0) return                                  ;
  if (exists              ) return                                  ;
  plan->showMessage ( item->text(1) )                               ;
  Mutex.lock()                                                      ;
  if (Encoders::Convert                                             (
                plan                                                ,
                Encoders::MP3                                       ,
                filename,audio,true,tr("%v/%m"))                  ) {
    AppendAudio(audio,uuid,item->text(1))                           ;
    QFile::remove(audio)                                            ;
    Alert ( Done  )                                                 ;
  } else                                                            {
    Alert ( Error )                                                 ;
    plan->Talk(tr("%1 can not convert successfully"                 )
               .arg(item->text(1))                                ) ;
  }                                                                 ;
  Mutex.unlock()                                                    ;
}

SUID N::VideoLists::AppendAudio(QString filename,SUID vuid,QString name)
{
  SUID auid = 0                                           ;
  plan->showMessage (filename)                            ;
  plan->processEvents (        )                          ;
  int seconds = Acoustics::Load(filename)                 ;
  if (seconds<=0) return auid                             ;
  QByteArray Data                                         ;
  QFileInfo FI(filename)                                  ;
  QFile File(filename)                                    ;
  QString suffix = FI.suffix()                            ;
  if (!File.open(QIODevice::ReadOnly)) return auid        ;
  Data = File.readAll()                                   ;
  File.close()                                            ;
  bool append = true                                      ;
  EnterSQL ( SC , plan->sql )                             ;
    int checksum = qChecksum(Data.data(),Data.size())     ;
    QString Q                                             ;
    UUIDs   Uuids                                         ;
    Q = SC.sql.SelectFrom                                 (
           "uuid"                                         ,
           PlanTable(AudioFiles)                          ,
           QString("where suffix = '%1' and filesize = %2")
            .arg(suffix).arg(Data.size()                  )
        )                                                 ;
    SqlLoopNow(SC,Q)                                      ;
      Uuids << SC.Uuid(0)                                 ;
    SqlLoopErr(SC,Q)                                      ;
    SqlLoopEnd(SC,Q)                                      ;
    if (Uuids.count()>0)                                  {
      for (int i=0;append && i<Uuids.count();i++)         {
        SUID uuid = Uuids[i]                              ;
        Q = SC.sql.SelectFrom                             (
          "suffix"                                        ,
          PlanTable(AudioFiles)                           ,
          SC.WhereUuid(uuid)                            ) ;
        if (SC.Fetch(Q))                                  {
          QString suff = SC.String(0)                     ;
          QString f                                       ;
          f = QString("%1.%2").arg(uuid).arg(suff)        ;
          f = plan->AudioFile(f)                          ;
          QFileInfo FF(f)                                 ;
          if (FF.exists())                                {
            QByteArray V                                  ;
            if (File::toByteArray(f,V))                   {
              if (V==Data) append = false                 ;
            }                                             ;
          }                                               ;
        }                                                 ;
      }                                                   ;
    }                                                     ;
    if (append)                                           {
      SUID uuid = SC.Unique(PlanTable(MajorUuid),"uuid" ) ;
      SC.insertUuid(PlanTable(MajorUuid),uuid             ,
                    Types::Audio                        ) ;
      SC.insertUuid(PlanTable(Audios),uuid,"uuid"       ) ;
      SC.assureName(PlanTable(Names)                      ,
                    uuid,plan->LanguageId,name          ) ;
      Q = SC.sql.InsertInto                               (
            PlanTable(AudioFiles)                         ,
            5                                             ,
            "uuid"                                        ,
            "suffix"                                      ,
            "seconds"                                     ,
            "checksum"                                    ,
            "filesize"                                  ) ;
      SC.Prepare(Q)                                       ;
      SC.Bind("uuid"    ,uuid           )                 ;
      SC.Bind("suffix"  ,suffix.toUtf8())                 ;
      SC.Bind("seconds" ,seconds        )                 ;
      SC.Bind("checksum",checksum       )                 ;
      SC.Bind("filesize",Data.size()    )                 ;
      SC.Exec()                                           ;
      QString f = QString("%1.%2").arg(uuid).arg(suffix)  ;
      QDir D(plan->AudioDepots[0])                        ;
      f = D.absoluteFilePath(f)                           ;
      if (File::toFile(f,Data)) auid = uuid               ;
    }                                                     ;
    if (auid>0)                                           {
      Q = SC.sql.InsertInto                               (
            PlanTable(Groups)                             ,
            5                                             ,
            "first"                                       ,
            "second"                                      ,
            "t1"                                          ,
            "t2"                                          ,
            "relation"                                  ) ;
      SC.Prepare(Q)                                       ;
      SC.Bind("first"   ,vuid                           ) ;
      SC.Bind("second"  ,auid                           ) ;
      SC.Bind("t1"      ,Types::Video                   ) ;
      SC.Bind("t2"      ,Types::Audio                   ) ;
      SC.Bind("relation",Groups::Subordination          ) ;
      SC.Exec()                                           ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  return auid                                             ;
}

void N::VideoLists::Dump(QTreeWidgetItem * item)
{
  if (IsNull(item)) return                                           ;
  qint64    filesize = item->text(4).toLongLong()                    ;
  qint64    total    = 0                                             ;
  int       bps      = 0                                             ;
  QDateTime StartT   = QDateTime::currentDateTime   (            )   ;
  QStringList Depots = plan->settings.getStringList ("VideoDepot")   ;
  if (Depots.count()<=0) return                                      ;
  ////////////////////////////////////////////////////////////////////
  plan->site.beginGroup ( "Videos" )                                 ;
  if (plan->site.contains("VerifyBPS"))                              {
    bps = plan->site.value("VerifyBPS").toInt()                      ;
  }                                                                  ;
  plan->site.endGroup   (          )                                 ;
  ////////////////////////////////////////////////////////////////////
  if (bps>0)                                                         {
    int ds = (int)(filesize/bps)                                     ;
    QTime T(0,0,0)                                                   ;
    QString TS                                                       ;
    T = T.addSecs(ds)                                                ;
    if (ds>3600) TS = T.toString(tr("hh hours mm minutes ss seconds")) ; else
    if (ds>  60) TS = T.toString(tr("mm minutes ss seconds"         )) ; else
                 TS = T.toString(tr("ss seconds"                    )) ;
    if (ds>  60) plan->Talk(tr("Check video clips, it will take about %1").arg(TS));
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  total = checkClip(Depots,item)                                     ;
  ////////////////////////////////////////////////////////////////////
  if (total>0 && StartT.secsTo(QDateTime::currentDateTime())>0)      {
    bps = (int)(total/StartT.secsTo(QDateTime::currentDateTime()))   ;
  }                                                                  ;
  if (bps>0)                                                         {
    plan->site.beginGroup ( "Videos"          )                      ;
    plan->site.setValue   ( "VerifyBPS" , bps )                      ;
    plan->site.endGroup   (                   )                      ;
  }                                                                  ;
  Alert ( Done )                                                     ;
}

void N::VideoLists::DumpAll(void)
{
  qint64    total    = 0                                             ;
  qint64    filesize = 0                                             ;
  int       bps      = 0                                             ;
  QDateTime StartT   = QDateTime::currentDateTime()                  ;
  QStringList Depots                                                 ;
  Depots = plan -> settings . getStringList ( "VideoDepot" )         ;
  if ( Depots . count ( ) <= 0 ) return                              ;
  ////////////////////////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)                            {
    QTreeWidgetItem * it = topLevelItem(i)                           ;
    filesize += it->text(4).toLongLong()                             ;
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  plan->site.beginGroup ( "Videos" )                                 ;
  if (plan->site.contains("VerifyBPS"))                              {
    bps = plan->site.value("VerifyBPS").toInt()                      ;
  }                                                                  ;
  plan->site.endGroup   (          )                                 ;
  ////////////////////////////////////////////////////////////////////
  if (bps>0)                                                         {
    int ds = (int)(filesize/bps)                                     ;
    QTime T(0,0,0)                                                   ;
    QString TS                                                       ;
    T = T.addSecs(ds)                                                ;
    if (ds>3600) TS = T.toString(tr("hh hours mm minutes ss seconds")) ; else
    if (ds>  60) TS = T.toString(tr("mm minutes ss seconds"         )) ; else
                 TS = T.toString(tr("ss seconds"                    )) ;
    if (ds>  30) plan->Talk(tr("Check video clips, it will take about %1").arg(TS));
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  qint64 all   = topLevelItemCount ( )                               ;
  qint64 index = 0                                                   ;
  bool   go    = true                                                ;
  int    id    = -1                                                  ;
  id = plan -> Progress ( tr("Dump all clips") , tr("%v/%m clips") ) ;
  plan -> Start    ( id , &index , &go )                             ;
  plan -> setRange ( id , 0      , all )                             ;
  ////////////////////////////////////////////////////////////////////
  while ( go && ( index < all ) )                                    {
    QString tts = tr("Verify %1 video clip of %2 clips"              )
                  .arg(index+1).arg(all)                             ;
    QTreeWidgetItem * it = topLevelItem(index)                       ;
    plan -> Talk ( tts )                                             ;
    total += checkClip ( Depots , it )                               ;
    index++                                                          ;
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  plan -> Finish ( id )                                              ;
  ////////////////////////////////////////////////////////////////////
  if (total>0 && StartT.secsTo(QDateTime::currentDateTime())>0)      {
    bps = (int)(total/StartT.secsTo(QDateTime::currentDateTime()))   ;
  }                                                                  ;
  if (bps>0)                                                         {
    plan->site.beginGroup ( "Videos"          )                      ;
    plan->site.setValue   ( "VerifyBPS" , bps )                      ;
    plan->site.endGroup   (                   )                      ;
  }                                                                  ;
  Alert ( Done )                                                     ;
}

void N::VideoLists::Verify(QTreeWidgetItem * item)
{
  if (IsNull(item)) return                                           ;
  qint64      filesize = item -> text(4) . toLongLong()              ;
  qint64      total    = 0                                           ;
  int         bps      = 0                                           ;
  QDateTime   StartT   = QDateTime::currentDateTime()                ;
  QStringList Depots   = plan->settings.getStringList ("VideoDepot") ;
  if (Depots.count()<=0) return                                      ;
  ////////////////////////////////////////////////////////////////////
  plan->site.beginGroup ( "Videos" )                                 ;
  if (plan->site.contains("VerifyBPS"))                              {
    bps = plan->site.value("VerifyBPS").toInt()                      ;
  }                                                                  ;
  plan->site.endGroup   (          )                                 ;
  ////////////////////////////////////////////////////////////////////
  if (bps>0)                                                         {
    int ds = (int)(filesize/bps)                                     ;
    QTime T(0,0,0)                                                   ;
    QString TS                                                       ;
    T = T.addSecs(ds)                                                ;
    if (ds>3600) TS = T.toString(tr("hh hours mm minutes ss seconds")); else
    if (ds>  60) TS = T.toString(tr("mm minutes ss seconds"         )); else
                 TS = T.toString(tr("ss seconds"                    ));
    if (ds>  30) plan->Talk(tr("Verify video clips, it will take about %1").arg(TS));
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  total = verifyClip ( Depots , item )                               ;
  ////////////////////////////////////////////////////////////////////
  if (total>0 && StartT.secsTo(QDateTime::currentDateTime())>0)      {
    bps = (int)(total/StartT.secsTo(QDateTime::currentDateTime()))   ;
  }                                                                  ;
  if (bps>0)                                                         {
    plan->site.beginGroup ( "Videos"          )                      ;
    plan->site.setValue   ( "VerifyBPS" , bps )                      ;
    plan->site.endGroup   (                   )                      ;
  }                                                                  ;
  Alert ( Done )                                                     ;
}

void N::VideoLists::VerifyAll(void)
{
  qint64      total    = 0                                             ;
  qint64      filesize = 0                                             ;
  int         bps      = 0                                             ;
  QDateTime   StartT   = QDateTime::currentDateTime()                  ;
  QStringList Depots   = plan -> VideoDepots                           ;
  if (Depots.count()<=0) return                                        ;
  //////////////////////////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)                              {
    QTreeWidgetItem * it = topLevelItem(i)                             ;
    filesize += it->text(4).toLongLong()                               ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  plan->site.beginGroup ( "Videos" )                                   ;
  if (plan->site.contains("VerifyBPS"))                                {
    bps = plan->site.value("VerifyBPS").toInt()                        ;
  }                                                                    ;
  plan->site.endGroup   (          )                                   ;
  //////////////////////////////////////////////////////////////////////
  if (bps>0)                                                           {
    int     ds = (int)(filesize/bps)                                   ;
    QTime   T (0,0,0)                                                  ;
    QString TS                                                         ;
    T = T.addSecs(ds)                                                  ;
    if (ds>3600) TS = T.toString(tr("hh hours mm minutes ss seconds")); else
    if (ds>  60) TS = T.toString(tr("mm minutes ss seconds"         )); else
                 TS = T.toString(tr("ss seconds"                    ));
    if (ds>  30) plan->Talk(tr("Verify video clips, it will take about %1").arg(TS)) ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  qint64 all   = topLevelItemCount ( )                                 ;
  qint64 index = 0                                                     ;
  bool   go    = true                                                  ;
  int    id    = -1                                                    ;
  id = plan -> Progress ( tr("Verify all clips") , tr("%v/%m clips") ) ;
  plan -> Start    ( id , &index , &go )                               ;
  plan -> setRange ( id , 0      , all )                               ;
  //////////////////////////////////////////////////////////////////////
  while ( go & ( index < all ) )                                       {
    QString tts = tr("Verify %1 video clip of %2 clips").arg(index+1).arg(all) ;
    QTreeWidgetItem * it = topLevelItem ( index )                      ;
    plan->Talk(tts)                                                    ;
    total += verifyClip ( Depots , it )                                ;
    index++                                                            ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  plan -> Finish ( id )                                                ;
  //////////////////////////////////////////////////////////////////////
  if (total>0 && StartT.secsTo(QDateTime::currentDateTime())>0)        {
    bps = (int)(total/StartT.secsTo(QDateTime::currentDateTime()))     ;
  }                                                                    ;
  if (bps>0)                                                           {
    plan->site.beginGroup ( "Videos"          )                        ;
    plan->site.setValue   ( "VerifyBPS" , bps )                        ;
    plan->site.endGroup   (                   )                        ;
  }                                                                    ;
  Alert ( Done )                                                       ;
}

qint64 N::VideoLists::verifyClip(QStringList & Depots,QTreeWidgetItem * item)
{
  TUID   FLAGS = 0                                                           ;
  qint64 total = 0                                                           ;
  SUID   uuid  = nTreeUuid(item,0)                                           ;
  if    (uuid<=0) return total                                               ;
  ////////////////////////////////////////////////////////////////////////////
  Mutex . lock ( )                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  QBrush fg = item->foreground(0)                                            ;
  for (int ii=0;ii<item->columnCount();ii++)                                 {
    item->setForeground(ii,QColor(255,0,0))                                  ;
  }                                                                          ;
  scrollToItem(item)                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  bool    success = false                                                    ;
  qint64  fsize   = 0                                                        ;
  QString path    = ""                                                       ;
  QString suffix  = ""                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                                                ;
    QString Q                                                                ;
    QString filename = ""                                                    ;
    Q = SC.sql.SelectFrom("flags",PlanTable(UFlags),SC.WhereUuid(uuid)     ) ;
    if (SC.Fetch(Q)) FLAGS = SC.Tuid(0)                                      ;
    Q = SC.sql.SelectFrom("suffix,filesize,filename",PlanTable(Videos)       ,
                          SC.WhereUuid(uuid)                               ) ;
    if (SC.Fetch(Q))                                                         {
      suffix   = SC . String (0)                                             ;
      fsize    = SC . Value  (1) . toLongLong()                              ;
      filename = SC . String (2)                                             ;
      if (filename.length()>0) success = true                                ;
    }                                                                        ;
    path = Find ( Depots , filename , fsize )                                ;
    if (path.length()<=0) success = false                                    ;
  LeaveSQL ( SC , plan->sql )                                                ;
  ////////////////////////////////////////////////////////////////////////////
  if (FLAGS>0)                                                               {
    for (int ii=0;ii<item->columnCount();ii++)                               {
      item->setForeground(ii,fg)                                             ;
    }                                                                        ;
    Mutex . unlock()                                                         ;
    return total                                                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (success)                                                               {
    if (path.length()<=0) success = false                                    ;
    if (fsize        <=0) success = false                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (success)                                                               {
    QFileInfo FI(path)                                                       ;
    if (FI.exists()) total = FI.size(); else success = false                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AvDecoder AVX                                                              ;
  plan -> Talk ( tr("Verify video file integrity")                         ) ;
  if ( ! AVX . StartVideo ( path ) )                                         {
    plan -> Talk ( tr("Video file can not open successfully!")             ) ;
    AVX . Close ( )                                                          ;
    success = false                                                          ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  long long duration   = 0                                                   ;
  long long tmax       = AVX . Duration ( )                                  ;
  int       Bits       = 0                                                   ;
  int       VideoW     = AVX . Width    ( )                                  ;
  int       VideoH     = AVX . Height   ( )                                  ;
  int       SampleRate = AVX . AudioCtx -> sample_rate                       ;
  int       CHANNELS   = AVX . Channels ( )                                  ;
  int       BitRATE    = AVX . BitRate  ( )                                  ;
  QString   AudioCodec = ""                                                  ;
  QString   VideoCodec = ""                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( success )                                                             {
    double         vrate       = AVX.VideoRate()                             ;
    int            vFrames     = 0                                           ;
    int            aFrames     = 0                                           ;
    int            rt                                                        ;
    int            ActualFrame = AVX.Frames()                                ;
    QDateTime      last        = QDateTime::currentDateTime()                ;
    AVPacket     * AVP                                                       ;
    int            pid                                                       ;
    qint64         value = 0                                                 ;
    bool           go    = true                                              ;
    pid   = plan->Progress(tr("Check integrity"),tr("%v/%m"))                ;
    plan -> Start ( pid , &value , &go )                                     ;
    //////////////////////////////////////////////////////////////////////////
    if (ActualFrame>0) plan -> setRange (pid,0,ActualFrame                 ) ;
                  else plan -> setRange (pid,0,(int)(AVX.Duration()/1000000));
    //////////////////////////////////////////////////////////////////////////
    AVP = new AVPacket()                                                     ;
    AVX.Initialize(AVP)                                                      ;
    while (go && 0==(rt=AVX.ReadPacket(*AVP)))                               {
      if (AVX.isAudioChannel(*AVP)) aFrames++                           ; else
      if (AVX.isVideoChannel(*AVP))                                          {
        vFrames++                                                            ;
        if (last!=QDateTime::currentDateTime())                              {
          if (AVX.Frames()>0) value = vFrames ; else                         {
            long long pts = AVX.PTS(*AVP)                                    ;
            if (pts!=AV_NOPTS_VALUE)                                         {
              pts = pts * vrate                                              ;
              value = (int)pts                                               ;
            }                                                                ;
          }                                                                  ;
          last = QDateTime::currentDateTime()                                ;
          DoProcessEvents                                                    ;
        }                                                                    ;
      } else                                                                 {
      }                                                                      ;
      AVX.Release(AVP)                                                       ;
      AVP = new AVPacket()                                                   ;
      AVX.Initialize(AVP)                                                    ;
    }                                                                        ;
    AVX.Release(AVP)                                                         ;
    if (ActualFrame>0) value = (int) vFrames                                 ;
    //////////////////////////////////////////////////////////////////////////
    if (ActualFrame>0 && vFrames!=ActualFrame)                               {
      switch (rt)                                                            {
        case AVERROR_INVALIDDATA                                             :
        case AVERROR_BSF_NOT_FOUND                                           :
        case AVERROR_BUG                                                     :
        case AVERROR_BUFFER_TOO_SMALL                                        :
        case AVERROR_DECODER_NOT_FOUND                                       :
        case AVERROR_DEMUXER_NOT_FOUND                                       :
        case AVERROR_ENCODER_NOT_FOUND                                       :
        case AVERROR_EXIT                                                    :
        case AVERROR_EXTERNAL                                                :
        case AVERROR_FILTER_NOT_FOUND                                        :
        case AVERROR_MUXER_NOT_FOUND                                         :
        case AVERROR_OPTION_NOT_FOUND                                        :
        case AVERROR_PATCHWELCOME                                            :
        case AVERROR_PROTOCOL_NOT_FOUND                                      :
        case AVERROR_STREAM_NOT_FOUND                                        :
        case AVERROR_BUG2                                                    :
        case AVERROR_UNKNOWN                                                 :
        case AVERROR_EXPERIMENTAL                                            :
          success = false                                                    ;
        break                                                                ;
        case AVERROR_EOF                                                     :
          success = true                                                     ;
        break                                                                ;
        default                                                              :
          success = true                                                     ;
        break                                                                ;
      }                                                                      ;
    } else                                                                   {
      success = true                                                         ;
      plan->Talk(tr("Video file is ready")                                 ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    AVX   . Close  (     )                                                   ;
    plan -> Finish ( pid )                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (duration>tmax) duration = tmax                                         ;
  long long dt = tmax - duration                                             ;
  if (dt<0) dt = -dt                                                         ;
  if (dt<1000000) duration = tmax                                            ;
  ////////////////////////////////////////////////////////////////////////////
  QTime T(0,0,0)                                                             ;
  QTime S(0,0,0)                                                             ;
  if (KeepGo)                                                                {
    EnterSQL ( SC , plan->sql )                                              ;
      QString   Q                                                            ;
      Q = SC . sql . Update                                                  (
            PlanTable(Videos)                                                ,
            "where uuid = :UUID"                                             ,
            12                                                               ,
            "suffix"                                                         ,
            "filesize"                                                       ,
            "audiocodec"                                                     ,
            "videocodec"                                                     ,
            "duration"                                                       ,
            "actual"                                                         ,
            "width"                                                          ,
            "height"                                                         ,
            "samplerate"                                                     ,
            "channels"                                                       ,
            "bitrate"                                                        ,
            "bits"                                                         ) ;
      SC . Prepare ( Q                                                     ) ;
      SC . Bind    ( "uuid"       , uuid                                   ) ;
      SC . Bind    ( "suffix"     , suffix     . toUtf8 ( )                ) ;
      SC . Bind    ( "filesize"   , fsize                                  ) ;
      SC . Bind    ( "audiocodec" , AudioCodec . toUtf8 ( )                ) ;
      SC . Bind    ( "videocodec" , VideoCodec . toUtf8 ( )                ) ;
      SC . Bind    ( "duration"   , tmax                                   ) ;
      SC . Bind    ( "actual"     , duration                               ) ;
      SC . Bind    ( "width"      , VideoW                                 ) ;
      SC . Bind    ( "height"     , VideoH                                 ) ;
      SC . Bind    ( "samplerate" , SampleRate                             ) ;
      SC . Bind    ( "channels"   , CHANNELS                               ) ;
      SC . Bind    ( "bitrate"    , BitRATE                                ) ;
      SC . Bind    ( "bits"       , Bits                                   ) ;
      SC . Exec    (                                                       ) ;
      ////////////////////////////////////////////////////////////////////////
      Q = SC.sql.InsertInto ( PlanTable(UFlags) , 1 , "uuid"               ) ;
      SC . Prepare ( Q             )                                         ;
      SC . Bind    ( "uuid" , uuid )                                         ;
      SC . Exec    (               )                                         ;
      ////////////////////////////////////////////////////////////////////////
      Q = SC.sql.Update(PlanTable(UFlags),"where uuid = :UUID",1,"flags")    ;
      SC . Prepare ( Q             )                                         ;
      SC . Bind    ( "uuid"  ,uuid )                                         ;
      SC . Bind    ( "flags" ,1    )                                         ;
      SC . Exec    (               )                                         ;
      ////////////////////////////////////////////////////////////////////////
      Load         ( SC , Depots , item , uuid )                             ;
    LeaveSQL ( SC , plan->sql )                                              ;
    //////////////////////////////////////////////////////////////////////////
    S = T . addSecs ( tmax    /1000000 )                                     ;
    T = T . addSecs ( duration/1000000 )                                     ;
    item->setText( 2,tr("%1/%2"                                              )
                   .arg(T.toString("hh:mm:ss")                               )
                   .arg(S.toString("hh:mm:ss"))                            ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  for (int ii=0;ii<item->columnCount();ii++) item->setForeground(ii,fg)      ;
  if (S!=T) item->setBackgroundColor(2,QColor(255,224,224))                  ;
  ////////////////////////////////////////////////////////////////////////////
  Mutex . unlock ( )                                                         ;
  return total                                                               ;
}

qint64 N::VideoLists::checkClip(QStringList & Depots,QTreeWidgetItem * item)
{
  qint64 total = 0                                                           ;
  SUID   uuid  = nTreeUuid ( item , 0 )                                      ;
  if    (uuid<=0) return total                                               ;
  ////////////////////////////////////////////////////////////////////////////
  Mutex.lock()                                                               ;
  ////////////////////////////////////////////////////////////////////////////
  QBrush fg = item->foreground(0)                                            ;
  for (int ii=0;ii<item->columnCount();ii++)                                 {
    item->setForeground(ii,QColor(255,0,0))                                  ;
  }                                                                          ;
  scrollToItem ( item )                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  bool    success  = false                                                   ;
  qint64  fsize    = 0                                                       ;
  QString path     = ""                                                      ;
  QString suffix   = ""                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                                                ;
    QString   Q                                                              ;
    QString   filename = ""                                                  ;
    Q = SC.sql.SelectFrom                                                    (
          "suffix,filesize,filename"                                         ,
          PlanTable(Videos),SC.WhereUuid(uuid))                              ;
    if (SC.Fetch(Q))                                                         {
      suffix   = SC.String(0)                                                ;
      fsize    = SC.Value (1).toLongLong()                                   ;
      filename = SC.String(2)                                                ;
      if (filename.length()>0) success = true                                ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    path = Find(Depots,filename,fsize)                                       ;
    if (path.length()<=0) success = false                                    ;
  LeaveSQL ( SC , plan->sql )                                                ;
  ////////////////////////////////////////////////////////////////////////////
  if (success)                                                               {
    if (path.length()<=0) success = false                                    ;
    if (fsize        <=0) success = false                                    ;
    QFileInfo FI(path)                                                       ;
    if (FI.exists())                                                         {
      total = FI.size()                                                      ;
    } else success = false                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AvDecoder AVX                                                              ;
  plan -> Talk ( tr("Verify video file integrity")                         ) ;
  if ( ! AVX . StartVideo ( path ) )                                         {
    plan -> Talk ( tr("Video file can not open successfully!")             ) ;
    AVX . Close ( )                                                          ;
    success = false                                                          ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  long long duration   = 0                                                   ;
  long long tmax       = AVX . Duration ( )                                  ;
  int       Bits       = 0                                                   ;
  int       VideoW     = AVX . Width    ( )                                  ;
  int       VideoH     = AVX . Height   ( )                                  ;
  int       SampleRate = AVX . AudioCtx -> sample_rate                       ;
  int       CHANNELS   = AVX . Channels ( )                                  ;
  int       BitRATE    = AVX . BitRate  ( )                                  ;
  QString   AudioCodec = ""                                                  ;
  QString   VideoCodec = ""                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( success )                                                             {
    double         vrate       = AVX.VideoRate()                             ;
    int            vFrames     = 0                                           ;
    int            aFrames     = 0                                           ;
    int            rt                                                        ;
    int            ActualFrame = AVX.Frames()                                ;
    QDateTime      last        = QDateTime::currentDateTime()                ;
    AVPacket     * AVP                                                       ;
    int            pid                                                       ;
    qint64         value = 0                                                 ;
    bool           go    = true                                              ;
    pid   = plan->Progress(tr("Check integrity"),tr("%v/%m"))                ;
    plan -> Start ( pid , &value , &go )                                     ;
    //////////////////////////////////////////////////////////////////////////
    if (ActualFrame>0) plan -> setRange (pid,0,ActualFrame                 ) ;
                  else plan -> setRange (pid,0,(int)(AVX.Duration()/1000000));
    //////////////////////////////////////////////////////////////////////////
    QDir d = plan->Path("Temp")                                              ;
    QString pathname = d.absoluteFilePath                                    (
                       QString("%1.%2.txt"                                   )
                       .arg(uuid).arg(suffix)                              ) ;
    QFile F(pathname)                                                        ;
    //////////////////////////////////////////////////////////////////////////
    if ( F . open ( QIODevice::WriteOnly ) )                                 {
      QString UUID = QString::number(uuid)                                   ;
      F.write(UUID.toUtf8())                                                 ;
      F.write("\r\n")                                                        ;
      for (int i=1;i<columnCount();i++)                                      {
        F.write(item->text(i).toUtf8()); F.write("\r\n")                     ;
      }                                                                      ;
      UUID = QString("Video codec : %1\r\n"                                  )
              .arg ( AVX . VideoCtx -> codec_id                            ) ;
      F.write(UUID.toUtf8())                                                 ;
      UUID = QString("Video rate : %1 / %2\r\n"                              )
              .arg(AVX.VideoStream->time_base.num                            )
              .arg(AVX.VideoStream->time_base.den                          ) ;
      F.write(UUID.toUtf8())                                                 ;
      UUID = QString("Audio codec : %1\r\n"                                  )
              .arg(AVX.AudioCtx->codec_id                                  ) ;
      F.write(UUID.toUtf8())                                                 ;
      UUID = QString("Audio rate : %1 / %2\r\n"                              )
              .arg(AVX.AudioStream->time_base.num                            )
              .arg(AVX.AudioStream->time_base.den                          ) ;
      F.write(UUID.toUtf8())                                                 ;
      for (int i=0;i<(int)AVX.FormatCtx->nb_streams;i++)                     {
        UUID = QString("Stream : %1 , Type : %2\r\n"                         )
            .arg(i                                                           )
            .arg(AVX.FormatCtx->streams[i]->codec->codec_type)               ;
        F.write(UUID.toUtf8())                                               ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      AVP = new AVPacket()                                                   ;
      AVX.Initialize(AVP)                                                    ;
      while (go && 0==(rt=AVX.ReadPacket(*AVP)))                             {
        if (AVX.isAudioChannel(*AVP))                                        {
          long long PTS = AVP -> pts                                         ;
          long long DTS = AVP -> dts                                         ;
          #define KKK(PDTS)                                                  \
            if (PDTS!=AV_NOPTS_VALUE)                                      { \
              PDTS *= AVX . AudioStream -> time_base.num                   ; \
              PDTS *= 1000000                                              ; \
              PDTS /= AVX . AudioStream -> time_base.den                   ; \
            }
          KKK(PTS)                                                           ;
          KKK(DTS)                                                           ;
          #undef  KKK
          QString f = QString( "Audio : %1[%2] , %3[%4] , %3"                )
                      . arg  ( AVP -> pts                                    )
                      . arg  ( PTS                                           )
                      . arg  ( AVP -> dts                                    )
                      . arg  ( DTS                                           )
                      . arg  ( AVP -> size                                 ) ;
          F.write(f.toUtf8())                                                ;
          F.write("\r\n")                                                    ;
          aFrames++                                                          ;
        } else
        if (AVX.isVideoChannel(*AVP))                                        {
          long long PTS = AVP -> pts                                         ;
          long long DTS = AVP -> dts                                         ;
          #define KKK(PDTS)                                                  \
            if (PDTS!=AV_NOPTS_VALUE)                                      { \
              PDTS *= AVX . VideoStream -> time_base . num                 ; \
              PDTS *= 1000000                                              ; \
              PDTS /= AVX . VideoStream -> time_base . den                 ; \
            }
          KKK(PTS)                                                           ;
          KKK(DTS)                                                           ;
          #undef  KKK
          QString f = QString("Video : %1[%2] , %3[%4] , %3"                 )
                      . arg  ( AVP -> pts                                    )
                      . arg  ( PTS                                           )
                      . arg  ( AVP -> dts                                    )
                      . arg  ( DTS                                           )
                      . arg  ( AVP -> size                                 ) ;
          F.write(f.toUtf8())                                                ;
          F.write("\r\n")                                                    ;
          ////////////////////////////////////////////////////////////////////
          vFrames++                                                          ;
          if (last!=QDateTime::currentDateTime())                            {
            if (AVX.Frames()>0) value = vFrames ; else                       {
              long long pts = AVX.PTS(*AVP)                                  ;
              if (pts!=AV_NOPTS_VALUE)                                       {
                pts = pts * vrate                                            ;
                value = (int)pts                                             ;
              }                                                              ;
            }                                                                ;
            last = QDateTime::currentDateTime()                              ;
            DoProcessEvents                                                  ;
          }                                                                  ;
        } else                                                               {
          // Other packet types
          QString f = QString("%1 : %2 , %3 , %4"                            )
                      . arg ( AVP -> stream_index                            )
                      . arg ( AVP -> pts                                     )
                      . arg ( AVP -> dts                                     )
                      . arg ( AVP -> size                                  ) ;
          F.write(f.toUtf8())                                                ;
          F.write("\r\n")                                                    ;
        }                                                                    ;
        AVX.Release(AVP)                                                     ;
        AVP = new AVPacket()                                                 ;
        AVX.Initialize(AVP)                                                  ;
      }                                                                      ;
      AVX.Release(AVP)                                                       ;
      if (ActualFrame>0) value = (int) vFrames                               ;
      ////////////////////////////////////////////////////////////////////////
      QString rstr = ""                                                      ;
      if (ActualFrame>0 && vFrames!=ActualFrame)                             {
        switch (rt)                                                          {
          case AVERROR_INVALIDDATA                                           :
            rstr = tr("Video file has invalid data")                         ;
          break                                                              ;
          case AVERROR_BSF_NOT_FOUND                                         :
            rstr = tr("Bitstream filter not found")                          ;
          break                                                              ;
          case AVERROR_BUG                                                   :
            rstr = tr("Internal bug")                                        ;
          break                                                              ;
          case AVERROR_BUFFER_TOO_SMALL                                      :
            rstr = tr("Buffer too small")                                    ;
          break                                                              ;
          case AVERROR_DECODER_NOT_FOUND                                     :
            rstr = tr("Decoder not found")                                   ;
          break                                                              ;
          case AVERROR_DEMUXER_NOT_FOUND                                     :
            rstr = tr("Demuxer not found")                                   ;
          break                                                              ;
          case AVERROR_ENCODER_NOT_FOUND                                     :
            rstr = tr("Encoder not found")                                   ;
          break                                                              ;
          case AVERROR_EXIT                                                  :
            rstr = tr("Immediate exit was requested")                        ;
          break                                                              ;
          case AVERROR_EXTERNAL                                              :
            rstr = tr("Generic error in an external library")                ;
          break                                                              ;
          case AVERROR_FILTER_NOT_FOUND                                      :
            rstr = tr("Filter not found")                                    ;
          break                                                              ;
          case AVERROR_MUXER_NOT_FOUND                                       :
            rstr = tr("Muxer not found")                                     ;
          break                                                              ;
          case AVERROR_OPTION_NOT_FOUND                                      :
            rstr = tr("Option not found")                                    ;
          break                                                              ;
          case AVERROR_PATCHWELCOME                                          :
            rstr = tr("Not yet implemented")                                 ;
          break                                                              ;
          case AVERROR_PROTOCOL_NOT_FOUND                                    :
            rstr = tr("Protocol not found")                                  ;
          break                                                              ;
          case AVERROR_STREAM_NOT_FOUND                                      :
            rstr = tr("Stream not found")                                    ;
          break                                                              ;
          case AVERROR_BUG2                                                  :
            rstr = tr("Internal bug 2")                                      ;
          break                                                              ;
          case AVERROR_UNKNOWN                                               :
            rstr = tr("Unknown error")                                       ;
          break                                                              ;
          case AVERROR_EXPERIMENTAL                                          :
            rstr = tr("Requested feature is flagged experimental")           ;
          break                                                              ;
          case AVERROR_EOF                                                   :
            rstr = tr("Video file end of file error")                        ;
            success = true                                                   ;
          break                                                              ;
          default                                                            :
            success = true                                                   ;
            rstr = tr("Video file unknown error")                            ;
          break                                                              ;
        }                                                                    ;
      } else                                                                 {
        success = true                                                       ;
        rstr = tr("Video file is ready")                                     ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      F . write ("\r\n"             )                                        ;
      F . write ( rstr . toUtf8 ( ) )                                        ;
      F . write ("\r\n"             )                                        ;
      if ( ! go )                                                            {
        QString uip = tr("User interrupts the process")                      ;
        F . write ("\r\n"            )                                       ;
        F . write ( uip . toUtf8 ( ) )                                       ;
        F . write ("\r\n"            )                                       ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      F.close  ( )                                                           ;
    }                                                                        ;
    AVX   . Close  (     )                                                   ;
    plan -> Finish ( pid )                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  for (int ii=0;ii<item->columnCount();ii++)                                 {
    item->setForeground(ii,fg)                                               ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  Mutex . unlock()                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  return total                                                               ;
}

bool N::VideoLists::Integrity(QString name,QString filename)
{
  if (filename.length()<=0) return false                                ;
  ///////////////////////////////////////////////////////////////////////
  bool correct = false                                                  ;
  AvDecoder AVX                                                         ;
  plan -> Talk (tr("Verify video file integrity"            )         ) ;
  if (!AVX.StartVideo(filename))                                        {
    plan->Talk (tr("Video file can not open successfully!"  )         ) ;
    AVX.Close()                                                         ;
    return false                                                        ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  double         vrate       = AVX.VideoRate()                          ;
  bool           keepReading = true                                     ;
  int            vFrames     = 0                                        ;
  int            aFrames     = 0                                        ;
  int            rt                                                     ;
  int            ActualFrame = AVX.Frames()                             ;
  QDateTime      last        = QDateTime::currentDateTime()             ;
  AVPacket     * AVP                                                    ;
  int            pid                                                    ;
  qint64         value = 0                                              ;
  bool           go    = true                                           ;
  QDateTime      T = nTimeNow                                           ;
  pid   = plan->Progress(tr("Check integrity"),tr("%v/%m"))             ;
  plan -> Start ( pid , &value , &go )                                  ;
  ////////////////////////////////////////////////////////////////////////////
  if (ActualFrame>0) plan->setRange(pid,0,ActualFrame                 )      ;
                else plan->setRange(pid,0,(int)(AVX.Duration()/1000000))     ;
  ////////////////////////////////////////////////////////////////////////////
  AVP = new AVPacket()                                                  ;
  AVX.Initialize(AVP)                                                   ;
  while (keepReading && 0==(rt=AVX.ReadPacket(*AVP)))                   {
    if (AVX.isAudioChannel(*AVP)) aFrames++                        ; else
    if (AVX.isVideoChannel(*AVP))                                       {
      vFrames++                                                         ;
      if (last!=QDateTime::currentDateTime())                           {
        if (AVX.Frames()>0) value = vFrames ; else                      {
          long long pts = AVX.PTS(*AVP)                                 ;
          if (pts!=AV_NOPTS_VALUE)                                      {
            pts = pts * vrate                                           ;
            value = (int)pts                                            ;
          }                                                             ;
        }                                                               ;
        last = QDateTime::currentDateTime()                             ;
        DoProcessEvents                                                 ;
      }                                                                 ;
    } else                                                              {
    }                                                                   ;
    AVX.Release(AVP)                                                    ;
    AVP = new AVPacket()                                                ;
    AVX.Initialize(AVP)                                                 ;
  }                                                                     ;
  AVX.Release(AVP)                                                      ;
  if (ActualFrame>0) value = (int)vFrames                               ;
  ///////////////////////////////////////////////////////////////////////
  if (ActualFrame>0 && vFrames!=ActualFrame)                            {
    switch (rt)                                                         {
      case AVERROR_INVALIDDATA                                          :
        plan->Talk(tr("Video file has invalid data")                  ) ;
      break                                                             ;
      case AVERROR_BSF_NOT_FOUND                                        :
        plan->Talk(tr("Bitstream filter not found")                   ) ;
      break                                                             ;
      case AVERROR_BUG                                                  :
        plan->Talk(tr("Internal bug")                                 ) ;
      break                                                             ;
      case AVERROR_BUFFER_TOO_SMALL                                     :
        plan->Talk(tr("Buffer too small")                             ) ;
      break                                                             ;
      case AVERROR_DECODER_NOT_FOUND                                    :
        plan->Talk(tr("Decoder not found")                            ) ;
      break                                                             ;
      case AVERROR_DEMUXER_NOT_FOUND                                    :
        plan->Talk(tr("Demuxer not found")                            ) ;
      break                                                             ;
      case AVERROR_ENCODER_NOT_FOUND                                    :
        plan->Talk(tr("Encoder not found")                            ) ;
      break                                                             ;
      case AVERROR_EXIT                                                 :
        plan->Talk(tr("Immediate exit was requested")                 ) ;
      break                                                             ;
      case AVERROR_EXTERNAL                                             :
        plan->Talk(tr("Generic error in an external library")         ) ;
      break                                                             ;
      case AVERROR_FILTER_NOT_FOUND                                     :
        plan->Talk(tr("Filter not found")                             ) ;
      break                                                             ;
      case AVERROR_MUXER_NOT_FOUND                                      :
        plan->Talk(tr("Muxer not found")                              ) ;
      break                                                             ;
      case AVERROR_OPTION_NOT_FOUND                                     :
        plan->Talk(tr("Option not found")                             ) ;
      break                                                             ;
      case AVERROR_PATCHWELCOME                                         :
        plan->Talk(tr("Not yet implemented")                          ) ;
      break                                                             ;
      case AVERROR_PROTOCOL_NOT_FOUND                                   :
        plan->Talk(tr("Protocol not found")                           ) ;
      break                                                             ;
      case AVERROR_STREAM_NOT_FOUND                                     :
        plan->Talk(tr("Stream not found")                             ) ;
      break                                                             ;
      case AVERROR_BUG2                                                 :
        plan->Talk(tr("Internal bug 2")                               ) ;
      break                                                             ;
      case AVERROR_UNKNOWN                                              :
        plan->Talk(tr("Unknown error")                                ) ;
      break                                                             ;
      case AVERROR_EXPERIMENTAL                                         :
        plan->Talk(tr("Requested feature is flagged experimental")    ) ;
      break                                                             ;
      case AVERROR_EOF                                                  :
        plan->Talk(tr("Video file end of file error")                 ) ;
        correct = true                                                  ;
      break                                                             ;
      default                                                           :
        correct = true                                                  ;
        plan->Talk(tr("Video file unknown error")                     ) ;
      break                                                             ;
    }                                                                   ;
  } else                                                                {
    correct = true                                                      ;
    plan->Talk(tr("Video file is ready")                              ) ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  AVX.Close()                                                           ;
  ///////////////////////////////////////////////////////////////////////
  if ( T.msecsTo(nTimeNow) < 1500 ) Time::skip(1500)                    ;
  plan -> Finish ( pid )                                                ;
  ///////////////////////////////////////////////////////////////////////
  return correct                                                        ;
}

bool N::VideoLists::ReplaceClip(SUID u,QString filename,QTreeWidgetItem * item)
{
  if ( u                     <= 0 ) return false                            ;
  if ( filename . length ( ) <= 0 ) return false                            ;
  ///////////////////////////////////////////////////////////////////////////
  QFileInfo FI ( filename )                                                 ;
  if ( ! FI . exists ( ) ) return false                                     ;
  ///////////////////////////////////////////////////////////////////////////
  long long  duration     = 0                                               ;
  long long  ActualLength = 0                                               ;
  int        Bits         = 0                                               ;
  QString    Depot        = ""                                              ;
  AvDecoder  AVX                                                            ;
  ///////////////////////////////////////////////////////////////////////////
  if (plan->VideoDepots . count  ( ) >  0) Depot = plan->VideoDepots[0]     ;
  if (Depot  . length ( ) <= 0) return false                                ;
  ///////////////////////////////////////////////////////////////////////////
  plan -> Talk ( tr("Add video clip") )                                     ;
  if (!AVX.StartVideo(filename))                                            {
    AVX.Close()                                                             ;
    return false                                                            ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  bool      moved       = false                                             ;
  QString   oldClip     = Filename ( u )                                    ;
  QString   oldClipPart = QString("%1.old").arg(oldClip)                    ;
  QFileInfo OFI ( oldClip )                                                 ;
  if ( OFI . exists ( ) ) moved = QFile::rename ( oldClip , oldClipPart )   ;
  ///////////////////////////////////////////////////////////////////////////
  duration     = AVX . Duration ( )                                         ;
  ActualLength = duration                                                   ;
  Bits         = AVX . AvBits   ( )                                         ;
  ///////////////////////////////////////////////////////////////////////////
  QDir    DDD ( Depot )                                                     ;
  bool    success = false                                                   ;
  QString TFile                                                             ;
  ///////////////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                          ;
  if ( SC . open ( FunctionString ) )                                       {
    QString Q                                                               ;
    SUID uuid = u                                                           ;
    TFile = QString("%1.%2") . arg ( uuid ) . arg ( FI . suffix ( ) )       ;
    int MimeId = -1                                                         ;
    Q = SC . sql . SelectFrom                                               (
          "mimeid"                                                          ,
          PlanTable(Extensions)                                             ,
          "where name = :NAME"                                            ) ;
    SC . Prepare ( Q )                                                      ;
    SC . Bind    ( "name" , FI.suffix().toLower().toUtf8()                ) ;
    if (SC.Exec() && SC.Next()) MimeId = SC.Int(0)                          ;
    if (MimeId>0)                                                           {
      Q = SC . sql . ReplaceInto                                            (
            PlanTable(Videos)                                               ,
            15                                                              ,
            "uuid"                                                          ,
            "mimeid"                                                        ,
            "suffix"                                                        ,
            "filesize"                                                      ,
            "filename"                                                      ,
            "audiocodec"                                                    ,
            "videocodec"                                                    ,
            "duration"                                                      ,
            "actual"                                                        ,
            "width"                                                         ,
            "height"                                                        ,
            "samplerate"                                                    ,
            "channels"                                                      ,
            "bitrate"                                                       ,
            "bits"                                                        ) ;
      SC.Prepare(Q                                                        ) ;
      SC.Bind   ("uuid"      ,uuid                                        ) ;
      SC.Bind   ("mimeid"    ,MimeId                                      ) ;
      SC.Bind   ("suffix"    ,FI.suffix().toUtf8()                        ) ;
      SC.Bind   ("filesize"  ,FI.size()                                   ) ;
      SC.Bind   ("filename"  ,TFile.toUtf8()                              ) ;
      SC.Bind   ("audiocodec",AVX.AudioCodecName().toUtf8()               ) ;
      SC.Bind   ("videocodec",AVX.VideoCodecName().toUtf8()               ) ;
      SC.Bind   ("duration"  ,duration                                    ) ;
      SC.Bind   ("actual"    ,ActualLength                                ) ;
      SC.Bind   ("width"     ,AVX.Width        ()                         ) ;
      SC.Bind   ("height"    ,AVX.Height       ()                         ) ;
      SC.Bind   ("samplerate",AVX.AvSampleRate ()                         ) ;
      SC.Bind   ("channels"  ,AVX.Channels     ()                         ) ;
      SC.Bind   ("bitrate"   ,AVX.BitRate      ()                         ) ;
      SC.Bind   ("bits"      ,Bits                                        ) ;
      if ( SC . Exec ( ) ) success = true                                   ;
    }                                                                       ;
    SC.close()                                                              ;
  }                                                                         ;
  SC.remove()                                                               ;
  ///////////////////////////////////////////////////////////////////////////
  AVX.Close()                                                               ;
  ///////////////////////////////////////////////////////////////////////////
  if ( success )                                                            {
    QFileInfo SFI ( filename )                                              ;
    QString   dFile      = DDD . absoluteFilePath ( TFile )                 ;
    qint64    sourceSize = SFI . size             (       )                 ;
    QString   pName      = tr("Copy video clip")                            ;
    QString   pFmt       = tr("%v/%m KB/s")                                 ;
    plan -> Talk ( tr("Copy video file size %1 bytes into video depot").arg(sourceSize) ) ;
    success = CopyToFile ( filename , dFile , pName , pFmt )                ;
    if ( success ) plan -> Talk ( tr("Video file added") )                  ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if ( success )                                                            {
    if ( moved ) QFile::remove ( oldClipPart           )                    ;
    SqlConnection XC ( plan -> sql )                                        ;
    if ( XC . open ( FunctionString ) )                                     {
      Load ( XC , plan->VideoDepots , item , u )                            ;
      XC . close  ( )                                                       ;
    }                                                                       ;
    XC   . remove ( )                                                       ;
  } else                                                                    {
    if ( moved ) QFile::rename ( oldClipPart , oldClip )                    ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  return success                                                            ;
}

void N::VideoLists::Search(void)
{
  if (NotNull(SearchEditor)) SearchEditor->deleteLater() ;
  plan -> processEvents ( )                              ;
  ////////////////////////////////////////////////////////
  QFont font = plan->status->font()                      ;
  QSize s    = plan->status->size()                      ;
  QSize m ( s.width() / 6,s.height() )                   ;
  QSize x ( s.width() / 6,plan->screen.heightPixels(5))  ;
  ////////////////////////////////////////////////////////
  SearchEditor    = new LineEdit       (              )  ;
  if (font.pixelSize()>0)                                {
    m             . setHeight    (font.pixelSize()-2)    ;
    font          . setPixelSize (font.pixelSize()-2)    ;
    SearchEditor -> setFont      (font              )    ;
  }                                                      ;
  SearchEditor   -> setMinimumSize     ( m            )  ;
  SearchEditor   -> setMaximumSize     ( x            )  ;
  plan -> status -> addPermanentWidget ( SearchEditor )  ;
  SearchEditor   -> setToolTip (tr("Search clip name"))  ;
  SearchEditor   -> show       (                      )  ;
  DoProcessEvents                                        ;
  ////////////////////////////////////////////////////////
  s = SearchEditor -> geometry() . size ()               ;
  font              . setPixelSize (s.height()-6)        ;
  SearchEditor     -> setFont      (font        )        ;
  ////////////////////////////////////////////////////////
  connect(SearchEditor,SIGNAL(editingFinished())         ,
          this        ,SLOT  (searchFinished ())       ) ;
  SearchEditor -> setFocus   ( Qt::TabFocusReason      ) ;
  Alert ( Click )                                        ;
}

void N::VideoLists::searchFinished(void)
{
  if (IsNull(SearchEditor)) return                            ;
  QString name  = SearchEditor -> text ( )                    ;
  SearchEditor -> deleteLater          ( )                    ;
  SearchEditor  = NULL                                        ;
  DoProcessEvents                                             ;
  if (name.length()<=0) return                                ;
  /////////////////////////////////////////////////////////////
  int StartId = -1                                            ;
  QTreeWidgetItem * item = NULL                               ;
  item = currentItem ( )                                      ;
  if (NotNull(item)) StartId = indexOfTopLevelItem(item)      ;
  StartId++                                                   ;
  item = NULL                                                 ;
  for (int i=StartId;item==NULL && i<topLevelItemCount();i++) {
    QTreeWidgetItem * it = topLevelItem ( i )                 ;
    if (it->text(1).contains(name)) item = it                 ;
  }                                                           ;
  if (IsNull(item))                                           {
    Alert ( Error )                                           ;
    return                                                    ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  scrollToItem     ( item  )                                  ;
  setCurrentItem   ( item  )                                  ;
  Alert            ( Click )                                  ;
}

void N::VideoLists::Rename(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  doubleClicked ( item , 1 )               ;
}

void N::VideoLists::Stop(void)
{
  KeepGo = false                                            ;
  if ( plan -> actions . contains ( Menus::Stop ) )         {
    plan -> actions [ Menus::Stop ] -> setVisible ( false ) ;
  }                                                         ;
}

void N::VideoLists::Clear(void)
{
  start ( 10004 ) ;
}

void N::VideoLists::Replace(QTreeWidgetItem * item)
{
  SUID u = nTreeUuid ( item , 0 )                            ;
  if ( u <= 0 ) return                                       ;
  ////////////////////////////////////////////////////////////
  QString path = plan -> Temporary ( "" )                    ;
  if ( plan -> Variables . contains ( "VideoLists" ) )       {
    path = plan -> Variables [ "VideoLists" ] . toString ( ) ;
  }                                                          ;
  path = QFileDialog::getOpenFileName                        (
                        this                                 ,
                        tr("Replace video clip")             ,
                        path                                 ,
                        tr("Video clip (*.*)")             ) ;
  if ( path . length ( ) <= 0 ) return                       ;
  ////////////////////////////////////////////////////////////
  QFileInfo FXI ( path )                                     ;
  path  = FXI . absoluteDir ( ) . absolutePath ( )           ;
  plan -> Variables [ "VideoLists" ] = path                  ;
  path  = FXI . absoluteFilePath ( )                         ;
  ////////////////////////////////////////////////////////////
  VarArgs args                                               ;
  args << u                                                  ;
  args << item -> text ( 1 )                                 ;
  args << path                                               ;
  start ( 20023 , args )                                     ;
}

void N::VideoLists::Replace(VarArgs & args)
{
  if ( args . count ( ) < 2 ) return                                         ;
  SUID      u    = args [ 0 ] . toULongLong ( )                              ;
  QString   name = args [ 1 ] . toString    ( )                              ;
  QString   path = args [ 2 ] . toString    ( )                              ;
  QFileInfo FEI ( path )                                                     ;
  ////////////////////////////////////////////////////////////////////////////
  if ( u                  <= 0 ) return                                      ;
  if ( ! FEI . exists ( )      ) return                                      ;
  if (   FEI . size   ( ) <= 0 ) return                                      ;
  ////////////////////////////////////////////////////////////////////////////
  QTreeWidgetItem * item = NULL                                              ;
  for (int i=0; ( NULL == item ) && ( i < topLevelItemCount ( ) ) ; i++ )    {
     QTreeWidgetItem * it = topLevelItem ( i      )                          ;
     SUID              xx = nTreeUuid    ( it , 0 )                          ;
     if ( xx == u ) item = it                                                ;
  }                                                                          ;
  if ( NULL == item )                                                        {
    Alert ( Error )                                                          ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  QBrush fg = item -> foreground ( 0 )                                       ;
  for (int ii=0;ii<item->columnCount();ii++)                                 {
    item -> setForeground ( ii , QColor ( 255 , 0 , 0 ) )                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Integrity ( name , path ) )                                           {
    if ( ReplaceClip ( u , path , item ) )                                   {
      Alert ( Done  )                                                        ;
    } else                                                                   {
      Alert ( Error )                                                        ;
    }                                                                        ;
  } else                                                                     {
    Alert   ( Error )                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  for (int ii = 0 ; ii < item -> columnCount ( ) ; ii++ )                    {
    item -> setForeground ( ii , fg )                                        ;
  }                                                                          ;
}

void N::VideoLists::ClearFlags(void)
{
  setEnabled ( false )                                   ;
  plan -> StartBusy ( )                                  ;
  EnterSQL         ( SC , plan->sql )                    ;
    QString Q                                            ;
    qint64  total = topLevelItemCount ( )                ;
    qint64  index = 0                                    ;
    int     id    = -1                                   ;
    bool    go    = true                                 ;
    id = plan -> Progress ( tr("Clear video flags")      ,
                            tr("%v/%m clips")          ) ;
    plan -> Start    ( id , &index , &go   )             ;
    plan -> setRange ( id , 0      , total )             ;
    while ( go && ( index < total ) )                    {
      QTreeWidgetItem * item = topLevelItem ( index    ) ;
      SUID              u    = nTreeUuid    ( item , 0 ) ;
      if ( u > 0 )                                       {
        Q = SC . sql . Update                            (
              "uflags"                                   ,
              "where uuid = :UUID"                       ,
              1 , "flags"                              ) ;
        SC . Prepare ( Q           )                     ;
        SC . Bind    ( "uuid"  , u )                     ;
        SC . Bind    ( "flags" , 0 )                     ;
        SC . Exec    (             )                     ;
      }                                                  ;
    }                                                    ;
    plan -> Finish ( id             )                    ;
  LeaveSQL         ( SC , plan->sql )                    ;
  plan -> StopBusy (                )                    ;
  setEnabled       ( true           )                    ;
  Alert            ( Done           )                    ;
}

void N::VideoLists::run(int Type,ThreadData * data)
{
  int               index                           ;
  QTreeWidgetItem * it                              ;
  switch ( Type )                                   {
    case 10001                                      :
      startLoading (                              ) ;
      ListAll      (                              ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 10002                                      :
      startLoading (                              ) ;
      index = data -> Arguments [ 0 ] . toInt ( )   ;
      List         ( index                        ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 10003                                      :
      startLoading (                              ) ;
      List         ( data -> Arguments            ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 10004                                      :
      startLoading (                              ) ;
      ClearFlags   (                              ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20001                                      :
      startLoading (                              ) ;
      DistillAll   (                              ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20002                                      :
      startLoading (                              ) ;
      index = data -> Arguments [ 0 ] . toInt ( )   ;
      it    = topLevelItem ( index )                ;
      Distill      ( it                           ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20011                                      :
      startLoading (                              ) ;
      VerifyAll    (                              ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20012                                      :
      startLoading (                              ) ;
      index = data -> Arguments [ 0 ] . toInt ( )   ;
      it    = topLevelItem ( index )                ;
      Verify       ( it                           ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20021                                      :
      startLoading (                              ) ;
      DumpAll      (                              ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20022                                      :
      startLoading (                              ) ;
      index = data -> Arguments [ 0 ] . toInt ( )   ;
      it    = topLevelItem ( index )                ;
      Dump         ( it                           ) ;
      stopLoading  (                              ) ;
    break                                           ;
    case 20023                                      :
      startLoading (                              ) ;
      Replace      ( data -> Arguments            ) ;
      stopLoading  (                              ) ;
    break                                           ;
  }                                                 ;
}

bool N::VideoLists::Menu(QPoint pos)
{
  if ( ! Mutex . tryLock ( 250 ) ) return true                 ;
  nScopedMenu ( mm , this )                                    ;
  QTreeWidgetItem * item  = itemAt(pos)                        ;
  QMenu           * mi                                         ;
  QMenu           * ms                                         ;
  QMenu           * ma                                         ;
  QMenu           * mp                                         ;
  QAction         * a                                          ;
  QTreeWidgetItem * hitem = headerItem ( )                     ;
  if ( NotNull ( item ) )                                      {
    mm . add ( 101,tr("Play"                  ))               ;
    mm . add ( 102,tr("Verify clip"           ))               ;
    mm . add ( 103,tr("Dump information"      ))               ;
    mm . add ( 104,tr("Replace"               ))               ;
    mm . addSeparator ( )                                      ;
    mm . add ( 401,tr("Distill audio channels"))               ;
    mm . add ( 402,tr("Distill all audio channels"))           ;
  }                                                            ;
  mm . add ( 201,tr("Verify all"           ))                  ;
  mm . add ( 202,tr("Dump all information" ))                  ;
  mm . add ( 203,tr("Clear verifications"  ))                  ;
  mm . addSeparator ( )                                        ;
  if ( isFunction ( 17301 ) ) mm . add ( 301 , tr("Refresh") ) ;
  if ( isFunction ( 17302 ) ) mm . add ( 302 , tr("Search" ) ) ;
  if ( isFunction ( 17303 ) ) mm . add ( 303 , tr("Rename" ) ) ;
  if ( isFunction ( 17304 ) )                                  {
    mm . add ( 304                                             ,
               LocalIcons [ "Delete" ]                         ,
               tr("Delete")                                  ) ;
  }                                                            ;
  if ( isFunction ( 17305 ) && NotNull ( item ) )              {
    mm . add ( 305 , tr("Edit clip names" ) )                  ;
  }                                                            ;
  mm . addSeparator ( )                                        ;
  mi = mm . addMenu ( tr("Items") )                            ;
  mm . add ( mi , 901 , tr("Show all columns") )               ;
  mm . addSeparator ( )                                        ;
  for (int i = 2 ; i <= 11 ; i++ )                             {
    a  = mm . add ( mi                                         ,
                    900 + i                                    ,
                    hitem -> text ( i )                        ,
                    true                                       ,
                    ! isColumnHidden ( i )                   ) ;
  }                                                            ;
  ma = mm . addMenu ( tr("Adjustments")   )                    ;
  mp = mm . addMenu ( tr("Selection"  )   )                    ;
  ms = mm . addMenu ( tr("Sorting"    )   )                    ;
  AdjustMenu        ( mm , ma             )                    ;
  SelectionsMenu    ( mm , mp             )                    ;
  SortingMenu       ( mm , ms             )                    ;
  DockingMenu       ( mm                  )                    ;
  mm . setFont(plan)                                           ;
  Mutex . unlock ( )                                           ;
  a = mm . exec ( )                                            ;
  if (IsNull(a)) return true                                   ;
  //////////////////////////////////////////////////////////////
  if ( RunSorting    ( mm , a ) ) return true                  ;
  if ( RunAdjustment ( mm , a ) ) return true                  ;
  if ( RunSelections ( mm , a ) ) return true                  ;
  if ( RunDocking    ( mm , a ) ) return true                  ;
  //////////////////////////////////////////////////////////////
  VarArgs args                                                 ;
  int     mid = mm [ a ]                                       ;
  switch ( mid )                                               {
    case 101                                                   :
      emit Play  ( nTreeUuid(item,0) , item->text(1) )         ;
    break                                                      ;
    case 102                                                   :
      args << indexOfTopLevelItem ( item )                     ;
      start      ( 20012 , args )                              ;
    break                                                      ;
    case 103                                                   :
      args << indexOfTopLevelItem ( item )                     ;
      start      ( 20022 , args )                              ;
    break                                                      ;
    case 104                                                   :
      Replace    ( item         )                              ;
    break                                                      ;
    case 201                                                   :
      start      ( 20011        )                              ;
    break                                                      ;
    case 202                                                   :
      start      ( 20021        )                              ;
    break                                                      ;
    case 203                                                   :
      Clear      (      )                                      ;
    break                                                      ;
    case 301                                                   :
      Lists      (      )                                      ;
    break                                                      ;
    case 302                                                   :
      Search     (      )                                      ;
    break                                                      ;
    case 303                                                   :
      Rename     (      )                                      ;
    break                                                      ;
    case 304                                                   :
      Delete     (      )                                      ;
    break                                                      ;
    case 305                                                   :
      emit ClipNames ( nTreeUuid (item,0) , item->text(1)    ) ;
    break                                                      ;
    case 401                                                   :
      args << indexOfTopLevelItem ( item )                     ;
      start      ( 20002 , args )                              ;
    break                                                      ;
    case 402                                                   :
      start      ( 20001        )                              ;
    break                                                      ;
    case 901                                                   :
      for (int i = 0 ; i < columnCount ( ) ; i++ )             {
        setColumnHidden ( i , false )                          ;
      }                                                        ;
    break                                                      ;
    case 902                                                   :
    case 903                                                   :
    case 904                                                   :
    case 905                                                   :
    case 906                                                   :
    case 907                                                   :
    case 908                                                   :
    case 909                                                   :
    case 910                                                   :
    case 911                                                   :
      setColumnHidden( mid - 900 , ! a -> isChecked ( ) )      ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
