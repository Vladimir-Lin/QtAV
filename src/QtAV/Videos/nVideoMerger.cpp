#include <qtav.h>

N::VideoMerger:: VideoMerger ( QWidget * parent , Plan * p )
               : TreeDock    (           parent ,        p )
               , GroupItems  (                           p )
               , Deletion    ( true                        )
               , Execute     ( true                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoMerger::~VideoMerger(void)
{
}

QSize N::VideoMerger::sizeHint(void) const
{
  return QSize ( 320 , 240 ) ;
}

void N::VideoMerger::Configure(void)
{
  dockingOrientation = Qt::Vertical                        ;
  //////////////////////////////////////////////////////////
  setAccessibleName       ( "N::VideoMerger"    )          ;
  //////////////////////////////////////////////////////////
  NewTreeWidgetItem       ( head                )          ;
  setWindowTitle          ( tr("Merge Albums" ) )          ;
  head->setText           ( 0 , tr("Name")      )          ;
  head->setText           ( 1 , tr("UUID")      )          ;
  //////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus      )          ;
  setDragDropMode         ( DragDrop            )          ;
  setAlternatingRowColors ( true                )          ;
  setRootIsDecorated      ( false               )          ;
  setSelectionMode        ( ExtendedSelection   )          ;
  setColumnCount          ( 2                   )          ;
  assignHeaderItems       ( head                )          ;
  MountClicked            ( 1                   )          ;
  plan -> setFont         ( this                )          ;
  //////////////////////////////////////////////////////////
  setDropFlag             ( DropAlbum , true    )          ;
  //////////////////////////////////////////////////////////
  LocalIcons [ "Delete" ] = QIcon(":/images/delete.png"  ) ;
  LocalIcons [ "Merge"  ] = QIcon(":/images/minimize.png") ;
  LocalIcons [ "Clear"  ] = QIcon(":/images/blank.png"   ) ;
  //////////////////////////////////////////////////////////
  PassDragDrop            = false                          ;
}

bool N::VideoMerger::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::VideoMerger::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::VideoMerger::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return true                              ;
}

bool N::VideoMerger::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut         ( nEqual(source,this) , false ) ;
  return dropItems ( source , mime , pos         ) ;
}

bool N::VideoMerger::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  return true                                          ;
}

void N::VideoMerger::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Action ) ;
}

void N::VideoMerger::Delete(void)
{
  QTreeWidgetItem * item    ;
  item = currentItem()      ;
  nDropOut ( IsNull(item) ) ;
  takeItem ( item         ) ;
}

bool N::VideoMerger::dropAlbums(QWidget * source,QPointF pos,const UUIDs & U)
{ Q_UNUSED ( source )                               ;
  Q_UNUSED ( pos    )                               ;
  UUIDs P = itemUuids(0)                            ;
  EnterSQL ( SC , plan -> sql )                     ;
    SUID u                                          ;
    foreach ( u , U )                               {
      if ( ! P . contains ( u ) )                   {
        QString n = ""                              ;
        n = SC . getName                            (
              PlanTable(Names)                      ,
              "uuid"                                ,
              plan -> LanguageId                    ,
              u                                   ) ;
        NewTreeWidgetItem(item)                     ;
        item->setData    ( 0 , Qt::UserRole , u   ) ;
        item->setText    ( 0 , n                  ) ;
        item->setText    ( 1 , QString::number(u) ) ;
        item->setToolTip ( 0 , QString::number(u) ) ;
        addTopLevelItem  ( item                   ) ;
        P << u                                      ;
      }                                             ;
    }                                               ;
  LeaveSQL ( SC , plan->sql )                       ;
  Alert    ( Done           )                       ;
  return true                                       ;
}

bool N::VideoMerger::DeleteMerged(N::SqlConnection & SC,SUID deletion)
{
  QStringList T                                                ;
  QString     Q                                                ;
  QString     t                                                ;
  //////////////////////////////////////////////////////////////
  T << PlanTable(Divisions)                                    ;
  T << PlanTable(MajorUuid)                                    ;
  //////////////////////////////////////////////////////////////
  foreach ( t , T )                                            {
    Q = SC . sql . DeleteFrom                                  (
          t                                                    ,
          SC . WhereUuid ( deletion )                        ) ;
    if ( ! SC . Query ( Q ) )                                  {
      PlanDebug ( 30 , tr  ( "Can not delete %1 from table %2" )
                     . arg ( deletion                          )
                     . arg ( PlanTable(Divisions)          ) ) ;
      PlanDebug ( 30 , SC . q . lastError ( ) . text ( ) )     ;
      return false                                             ;
    }                                                          ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  return true                                                  ;
}

bool N::VideoMerger::NameMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false  ;
  if ( deletion <= 0               ) return false  ;
  if ( master   == deletion        ) return false  ;
  //////////////////////////////////////////////////
  TUIDs   IDs                                      ;
  QString Q                                        ;
  TUID    u                                        ;
  Q = SC . sql . SelectFrom                        (
        "id"                                       ,
        PlanTable(Names)                           ,
        SC . WhereUuid  ( deletion )               ,
        SC . OrderByAsc ( "id"     )             ) ;
  SqlLoopNow ( SC , Q )                            ;
    IDs << SC . Uuid ( 0 )                         ;
  SqlLoopErr ( SC , Q )                            ;
  SqlLoopEnd ( SC , Q )                            ;
  //////////////////////////////////////////////////
  foreach ( u , IDs )                              {
    Q = SC . sql . SelectFrom                      (
          "language"                               ,
          PlanTable(Names)                         ,
          QString("where `id` = %1") . arg ( u ) ) ;
    if ( SC . Fetch ( Q ) )                        {
      int  language = SC . Int ( 0 )               ;
      TUID id       = -1                           ;
      Q = SC . sql . SelectFrom                    (
            "id"                                   ,
            PlanTable(Names)                       ,
            QString ( "where `uuid` = %1"
                      " and `language` = %2"       )
            . arg ( master                         )
            . arg ( language )                   ) ;
      if ( SC . Fetch ( Q ) ) id = SC . Uuid ( 0 ) ;
      if ( id > 0 )                                {
        Q = SC . sql . DeleteFrom                  (
              PlanTable(Names)                     ,
              QString("where `id` = %1").arg(u)  ) ;
        SC . Query ( Q )                           ;
      } else                                       {
        Q = SC . sql . Update                      (
              PlanTable(Names)                     ,
              SC . sql . Where ( 1 , "id" )        ,
              1                                    ,
              "uuid"                             ) ;
        SC . Prepare ( Q                         ) ;
        SC . Bind    ( "id"   , u                ) ;
        SC . Bind    ( "uuid" , master           ) ;
        SC . Exec    (                           ) ;
      }                                            ;
    }                                              ;
  }                                                ;
  //////////////////////////////////////////////////
  return true                                      ;
}

bool N::VideoMerger::NamesMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  NameMapItems DeleteMaps                                     ;
  DeleteMaps . where ( SC , PlanTable(NameMaps) , deletion )  ;
  if ( DeleteMaps . count ( ) <= 0 ) return false             ;
  /////////////////////////////////////////////////////////////
  NameMapItems MasterMaps                                     ;
  MasterMaps . where ( SC , PlanTable(NameMaps) , master   )  ;
  /////////////////////////////////////////////////////////////
  int prefer = 0                                              ;
  if ( MasterMaps.count() > 0      )                          {
    prefer = MasterMaps.last()->prefer                        ;
  }                                                           ;
  prefer++                                                    ;
  DeleteMaps . remove ( SC , PlanTable(NameMaps) )            ;
  /////////////////////////////////////////////////////////////
  QString Q                                                   ;
  Q = SC . sql . InsertInto                                   (
        PlanTable(NameMaps)                                   ,
        4                                                     ,
        "uuid"                                                ,
        "name"                                                ,
        "prefer"                                              ,
        "usage"                                             ) ;
  for (int i=0;i<DeleteMaps.count();i++)                      {
    SC . Prepare ( Q                               )          ;
    SC . Bind    ( "uuid"   , master               )          ;
    SC . Bind    ( "name"   , DeleteMaps[i]->name  )          ;
    SC . Bind    ( "prefer" , prefer               )          ;
    SC . Bind    ( "usage"  , DeleteMaps[i]->usage )          ;
    if ( SC . Exec ( ) ) prefer++                             ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::VideoMerger::GroupsRemove (SqlConnection & SC,QList<Group> & groups)
{
  for (int i=0;i<groups.count();i++) {
    ExactDetach ( SC , groups[i] )   ;
  }                                  ;
  return true                        ;
}

bool N::VideoMerger::GroupsJoin (SqlConnection & SC,QList<Group> & groups)
{
  QString Q                                              ;
  for (int i=0;i<groups.count();i++)                     {
    bool exists = false                                  ;
    //////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                                (
          "id"                                           ,
          PlanTable(Groups)                              ,
          ExactItem                                      (
            groups [ i ] . first                         ,
            groups [ i ] . second                        ,
            groups [ i ] . t1                            ,
            groups [ i ] . t2                            ,
            groups [ i ] . relation                  ) ) ;
    if (SC.Fetch(Q)) exists = true                       ;
    if (exists) continue                                 ;
    //////////////////////////////////////////////////////
    UUIDs U                                              ;
    U << groups[i].second                                ;
    GroupItems :: Join                                   (
      SC                                                 ,
      groups[i].first                                    ,
      groups[i].t1                                       ,
      groups[i].t2                                       ,
      groups[i].relation                                 ,
      0                                                  ,
      U                                                  ,
      false                                            ) ;
    Q = SC.sql.Update                                    (
          PlanTable(Groups)                              ,
          ExactItem                                      (
            groups[i].first                              ,
            groups[i].second                             ,
            groups[i].t1                                 ,
            groups[i].t2                                 ,
            groups[i].relation                         ) ,
          2                                              ,
          "membership"                                   ,
          "reversal"                                   ) ;
    SC . Prepare ( Q                                   ) ;
    SC . Bind    ( "membership" , groups[i].membership ) ;
    SC . Bind    ( "reversal"   , groups[i].reversal   ) ;
    SC . Exec    (                                     ) ;
  }                                                      ;
  return true                                            ;
}

bool N::VideoMerger::GroupsMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  QList<Group> G                                              ;
  QString         Q                                           ;
  Q = SC.sql.SelectFrom                                       (
        SC . Columns                                          (
          8                                                   ,
          "first"                                             ,
          "second"                                            ,
          "t1"                                                ,
          "t2"                                                ,
          "relation"                                          ,
          "membership"                                        ,
          "position"                                          ,
          "reversal"                                        ) ,
        PlanTable(Groups)                                     ,
        QString("where `first` = %1").arg(deletion)           ,
        SC.OrderByAsc("id")                                 ) ;
  SqlLoopNow ( SC , Q )                                       ;
    Group M                                                   ;
    M.first      = SC . Uuid   ( 0 )                          ;
    M.second     = SC . Uuid   ( 1 )                          ;
    M.t1         = SC . Int    ( 2 )                          ;
    M.t2         = SC . Int    ( 3 )                          ;
    M.relation   = SC . Int    ( 4 )                          ;
    M.membership = SC . Double ( 5 )                          ;
    M.position   = SC . Int    ( 6 )                          ;
    M.reversal   = SC . Int    ( 7 )                          ;
    G << M                                                    ;
  SqlLoopErr ( SC , Q )                                       ;
  SqlLoopEnd ( SC , Q )                                       ;
  if (G.count()<=0) return false                              ;
  GroupsRemove ( SC , G )                                     ;
  /////////////////////////////////////////////////////////////
  for (int i=0;i<G.count();i++) G[i].first = master           ;
  GroupsJoin ( SC , G )                                       ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::VideoMerger::OwnersMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  QList<Group> G                                              ;
  QString      Q                                              ;
  Q = SC.sql.SelectFrom                                       (
        SC . Columns                                          (
          8                                                   ,
          "first"                                             ,
          "second"                                            ,
          "t1"                                                ,
          "t2"                                                ,
          "relation"                                          ,
          "membership"                                        ,
          "position"                                          ,
          "reversal"                                        ) ,
        PlanTable(Groups)                                     ,
        QString("where `second` = %1").arg(deletion)          ,
        SC.OrderByAsc("id")                                 ) ;
  SqlLoopNow ( SC , Q )                                       ;
    Group M                                                   ;
    M.first      = SC . Uuid   ( 0 )                          ;
    M.second     = SC . Uuid   ( 1 )                          ;
    M.t1         = SC . Int    ( 2 )                          ;
    M.t2         = SC . Int    ( 3 )                          ;
    M.relation   = SC . Int    ( 4 )                          ;
    M.membership = SC . Double ( 5 )                          ;
    M.position   = SC . Int    ( 6 )                          ;
    M.reversal   = SC . Int    ( 7 )                          ;
    G << M                                                    ;
  SqlLoopErr ( SC , Q )                                       ;
  SqlLoopEnd ( SC , Q )                                       ;
  if (G.count()<=0) return false                              ;
  GroupsRemove ( SC , G )                                     ;
  /////////////////////////////////////////////////////////////
  for (int i=0;i<G.count();i++) G[i].second = master          ;
  GroupsJoin ( SC , G )                                       ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::VideoMerger::MergeTriples(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `first` = %1").arg(deletion)   ,
        1                                             ,
        "first"                                     ) ;
  SC . Prepare ( Q                )                   ;
  SC . Bind    ( "first" , master )                   ;
  SC . Exec    (                  )                   ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `second` = %1").arg(deletion)  ,
        1                                             ,
        "second"                                    ) ;
  SC . Prepare ( Q                 )                  ;
  SC . Bind    ( "second" , master )                  ;
  SC . Exec    (                   )                  ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `third` = %1").arg(deletion)   ,
        1                                             ,
        "third"                                     ) ;
  SC . Prepare ( Q                )                   ;
  SC . Bind    ( "third" , master )                   ;
  SC . Exec    (                  )                   ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `first` = %1").arg(deletion))  ;
  SC.Query(Q)                                         ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `second` = %1").arg(deletion)) ;
  SC.Query(Q)                                         ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `third` = %1").arg(deletion))  ;
  SC.Query(Q)                                         ;
  /////////////////////////////////////////////////////
  return true                                         ;
}

bool N::VideoMerger::MergeFlags(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                            ;
  Q = SC.sql.Update(PlanTable(UFlags),SC.WhereUuid(deletion),1,"uuid") ;
  SC . Prepare ( Q               )                                     ;
  SC . Bind    ( "uuid" , master )                                     ;
  SC . Exec    (                 )                                     ;
  //////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(UFlags),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                     {
    PlanDebug ( 60 , tr("Remove %1 from uflags").arg(deletion)       ) ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  return true                                                          ;
}

bool N::VideoMerger::MergeVariables(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                               ;
  CUIDs   N                                                               ;
  CUIDs   R                                                               ;
  bool    correct                                                         ;
  /////////////////////////////////////////////////////////////////////////
  correct = SC.Cuids(N,PlanTable(Variables),"id",SC.WhereUuid(deletion))  ;
            SC.Cuids(R,PlanTable(Variables),"id",SC.WhereUuid(master  ))  ;
  if (!correct)                                                           {
    PlanDebug ( 60                                                        ,
                tr("%1 does not have variables").arg(deletion)          ) ;
    return true                                                           ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  if (!Execute)                                                           {
    PlanDebug ( 60                                                        ,
         tr("%1 has %2 variables").arg(deletion).arg(N.count())         ) ;
    return true                                                           ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  QStringList V                                                           ;
  QString     VN                                                          ;
  Q = SC . sql . SelectFrom                                               (
        "name"                                                            ,
        PlanTable(Variables)                                              ,
        SC.WhereUuid(deletion)                                            ,
        SC.OrderByAsc("id")                                             ) ;
  SqlLoopNow ( SC , Q )                                                   ;
    VN = SC . String ( 0 )                                                ;
    if (VN.length()>0 && !V.contains(VN)) V << VN                         ;
  SqlLoopErr ( SC , Q )                                                   ;
  SqlLoopEnd ( SC , Q )                                                   ;
  /////////////////////////////////////////////////////////////////////////
  foreach (VN,V)                                                          {
    Q = SC.sql.Update                                                     (
          PlanTable(Variables)                                            ,
          QString ( "where `uuid` = %1 and `name` = '%2'"                 )
          . arg ( deletion                                                )
          . arg ( VN                                                    ) ,
          1                                                               ,
          "uuid"                                                        ) ;
    SC . Prepare ( Q               )                                      ;
    SC . Bind    ( "uuid" , master )                                      ;
    SC . Exec    (                 )                                      ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(Variables),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                        {
    PlanDebug ( 60 , tr("Remove %1 from variables").arg(deletion)       ) ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  return true                                                             ;
}

bool N::VideoMerger::MergeIdentifier(SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                                 ;
  Q = SC.sql.Update(PlanTable(Identifiers),SC.WhereUuid(deletion),1,"uuid") ;
  SC . Prepare ( Q               )                                          ;
  SC . Bind    ( "uuid" , master )                                          ;
  SC . Exec    (                 )                                          ;
  ///////////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(Identifiers),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                          {
    PlanDebug ( 60 , tr("Remove %1 from identifiers").arg(deletion)       ) ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  return true                                                               ;
}

void N::VideoMerger::Merge(void)
{
  nDropOut ( topLevelItemCount() < 2 )     ;
  UUIDs Uuids = itemUuids(0)               ;
  nDropOut ( Uuids . count()     < 2 )     ;
  SUID  puid  = Uuids[0]                   ;
  SUID  uuid                               ;
  Uuids . takeAt      ( 0                ) ;
  //////////////////////////////////////////
  setEnabled          ( false            ) ;
  EnterSQL            ( SC , plan -> sql ) ;
    foreach           ( uuid , Uuids     ) {
      NameMerger      ( SC , puid , uuid ) ;
      NamesMerger     ( SC , puid , uuid ) ;
      GroupsMerger    ( SC , puid , uuid ) ;
      OwnersMerger    ( SC , puid , uuid ) ;
      MergeTriples    ( SC , puid , uuid ) ;
      MergeFlags      ( SC , puid , uuid ) ;
      MergeVariables  ( SC , puid , uuid ) ;
      MergeIdentifier ( SC , puid , uuid ) ;
      if ( Execute )                       {
        DeleteMerged  ( SC ,        uuid ) ;
      }                                    ;
    }                                      ;
  ErrorSQL            ( SC , plan -> sql ) ;
  LeaveSQL            ( SC , plan -> sql ) ;
  setEnabled          ( true             ) ;
  //////////////////////////////////////////
  TreeWidgetItems Items                    ;
  for (int i=1;i<topLevelItemCount();i++)  {
    Items << topLevelItem(i)               ;
  }                                        ;
  for (int i=0;i<Items.count();i++)        {
    takeItem(Items[i])                     ;
  }                                        ;
  //////////////////////////////////////////
  Alert ( Done  )                          ;
}

void N::VideoMerger::run(int T,ThreadData * data)
{ Q_UNUSED  ( data )   ;
  switch    ( T )      {
    case 10001         :
      startLoading ( ) ;
      Merge        ( ) ;
      stopLoading  ( ) ;
    break              ;
  }                    ;
}

bool N::VideoMerger::Menu(QPoint pos)
{
  N::MenuManager mm ( this )                                  ;
  QTreeWidgetItem * item = itemAt  ( pos )                    ;
  QAction       * aa                                          ;
  if (NotNull(item))                                          {
    mm . add ( 102 , LocalIcons [ "Delete" ] , tr("Delete") ) ;
  }                                                           ;
  if (topLevelItemCount()>1)                                  {
    mm . add ( 103 , LocalIcons [ "Merge"  ] , tr("Merge")  ) ;
  }                                                           ;
  mm   . add ( 104 , LocalIcons [ "Clear"  ] , tr("Clear")  ) ;
  mm   . addSeparator ( )                                     ;
  mm   . add ( 201 , tr("Deletion" ) , true , Deletion )      ;
  mm   . add ( 202 , tr("Execution") , true , Execute  )      ;
  mm . addSeparator (    )                                    ;
  DockingMenu       ( mm )                                    ;
  mm   . setFont(plan)                                        ;
  aa = mm.exec()                                              ;
  nKickOut ( IsNull(aa) , true )                              ;
  if ( RunDocking ( mm , aa ) ) return true                   ;
  switch ( mm [ aa ] )                                        {
    case 102                                                  :
      Delete (       )                                        ;
    break                                                     ;
    case 103                                                  :
      start  ( 10001 )                                        ;
    break                                                     ;
    case 104                                                  :
      clear  (       )                                        ;
    break                                                     ;
    case 201                                                  :
      Deletion = Deletion ? false : true                      ;
    break                                                     ;
    case 202                                                  :
      Execute  = Execute  ? false : true                      ;
    break                                                     ;
  }                                                           ;
  return true                                                 ;
}
