#include <qtav.h>

typedef struct   {
  QString Name   ;
  UUIDs   Uuids  ;
} PendingSignals ;

N::VideoTags:: VideoTags  ( QWidget * parent , Plan * p )
             : TagsEditor (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoTags::~VideoTags(void)
{
}

void N::VideoTags::Configure(void)
{
  SetsOp        = NULL                         ;
  Trigger       = NULL                         ;
  Scope         = "VideoTags"                  ;
  TagsCheckable = true                         ;
  ItemType      = Types::Video                 ;
  //////////////////////////////////////////////
  NewTreeWidgetItem ( head                   ) ;
  setWindowTitle    ( tr("Tags")             ) ;
  head->setText     ( 0 , tr("Video tags")   ) ;
  setAccessibleName ( "N::VideoTags"         ) ;
  setObjectName     ( "N::VideoTags"         ) ;
  setAllAlignments  ( head , Qt::AlignCenter ) ;
  setFont           ( head , Fonts::ListView ) ;
  setHeaderItem     ( head                   ) ;
  //////////////////////////////////////////////
  setDropFlag       ( DropText    , true     ) ;
  setDropFlag       ( DropUrls    , false    ) ;
  setDropFlag       ( DropImage   , false    ) ;
  setDropFlag       ( DropHtml    , false    ) ;
  setDropFlag       ( DropColor   , false    ) ;
  setDropFlag       ( DropTag     , true     ) ;
  setDropFlag       ( DropPicture , false    ) ;
  setDropFlag       ( DropPeople  , false    ) ;
  setDropFlag       ( DropVideo   , true     ) ;
  setDropFlag       ( DropAlbum   , true     ) ;
  setDropFlag       ( DropGender  , false    ) ;
}

bool N::VideoTags::SearchSets(void)
{
  if (NotNull(SetsOp)) SetsOp->deleteLater()                           ;
  SetsOp  = headerEditor(SIGNAL(returnPressed()),SLOT(setsFinished())) ;
  SetsOp -> setPlaceholderText(tr("Sets operations"))                  ;
  return true                                                          ;
}

SUID N::VideoTags::TagUuid(N::SqlConnection & SC,int type,QString name)
{
  return N::TagsManager::TagUuid(SC,type,name,vLanguageId) ;
}

void N::VideoTags::setsFinished(void)
{
  nDropOut   ( IsNull(SetsOp)     )                 ;
  QString text = SetsOp->text()                     ;
  SetsOp -> deleteLater()                           ;
  SetsOp  = NULL                                    ;
  nDropOut   ( text.length() <= 0 )                 ;
  N::SetsAlgebra NSA                                ;
  nDropOut   ( !NSA.Decode(text)  )                 ;
  nDropOut   ( NSA.Total()<=0     )                 ;
  UUIDs uuids                                       ;
  setEnabled ( false              )                 ;
  EnterSQL   ( SC , plan->sql     )                 ;
    QString Q                                       ;
    SUID    uuid                                    ;
    int     i                                       ;
    bool    correct = true                          ;
    ZMAPs   TagUuids                                ;
    if (NSA.Inputs.count()>0)                       {
      for (i=0;correct && i<NSA.Inputs.count();i++) {
        Q    = NSA.Inputs[i]                        ;
        uuid = TagUuid ( SC,ItemType,Q )            ;
        if (uuid>0) TagUuids[Q] = uuid              ;
               else correct = false                 ;
      }                                             ;
    }                                               ;
    if (correct)                                    {
      for (i=0;i<NSA.Inputs.count();i++)            {
        QString N                                   ;
        N    = NSA.Inputs[i]                        ;
        uuid = TagUuids  [N]                        ;
        uuids . clear   (       )                   ;
        Q = SC.sql.SelectFrom                       (
            "second",PlanTable(Groups)              ,
            QString("where first = %1 "
                    "and t1 = %2 "
                    "and t2 = %3 "
                    "and relation = %4"             )
            .arg(uuid                               )
            .arg(Types::Tag                         )
            .arg(Types::Album                       )
            .arg(Groups::Subordination          ) ) ;
        SqlLoopNow ( SC , Q )                       ;
          uuids  <<  SC . Uuid(0)                   ;
        SqlLoopErr ( SC , Q )                       ;
        SqlLoopEnd ( SC , Q )                       ;
        NSA.Uuids[N] = uuids                        ;
      }                                             ;
      uuids . clear   (       )                     ;
      NSA   . Execute ( uuids )                     ;
    }                                               ;
  LeaveSQL   ( SC , plan->sql     )                 ;
  setEnabled ( true               )                 ;
  if (uuids.count() <= 0)                           {
    Alert          ( Error        )                 ;
  } else                                            {
    emit ListVideo ( text , uuids )                 ;
  }                                                 ;
}

bool N::VideoTags::addMenuItems(MenuManager & menu,QPoint pos)
{ Q_UNUSED ( pos )                                  ;
  QTreeWidgetItem * item = currentItem ( )          ;
  UUIDs Uuids                                       ;
  ///////////////////////////////////////////////////
  if ( NotNull ( SetsOp ) )                         {
    SetsOp -> deleteLater ( )                       ;
    SetsOp  = NULL                                  ;
  }                                                 ;
  ///////////////////////////////////////////////////
  nFullLoop(i,topLevelItemCount())                  {
    QTreeWidgetItem * item = topLevelItem(i)        ;
    if (item->checkState(0)==Qt::Checked)           {
      Uuids << nTreeUuid    ( item , 0 )            ;
    }                                               ;
  }                                                 ;
  if (Uuids.count()<=0 && IsNull(item)) return true ;
  if (Uuids.count()==0 && NotNull(item))            {
    menu.add(501,tr("Selected video albums"     ))  ;
  } else
  if (Uuids.count()==1)                             {
    menu.add(501,tr("Checked video albums"      ))  ;
  } else
  if (Uuids.count()>1)                              {
    menu.add(501,tr("Tags selected video albums"))  ;
    menu.add(502,tr("Tags union video albums"   ))  ;
  }                                                 ;
  menu.add(503,tr("Search tags by sets operation")) ;
  menu.addSeparator()                               ;
  return true                                       ;
}

bool N::VideoTags::RunMenuResult(QAction * action,MenuManager & menu,QPoint pos)
{ Q_UNUSED ( pos )                                         ;
  UUIDs       Uuids                                        ;
  VarArgs     args                                         ;
  int         id = menu [ action ]                         ;
  //////////////////////////////////////////////////////////
  if ( id == 503 ) return SearchSets ( )                   ;
  //////////////////////////////////////////////////////////
  QTreeWidgetItem * item = currentItem ( )                 ;
  nFullLoop(i,topLevelItemCount())                         {
    QTreeWidgetItem * it = topLevelItem ( i )              ;
    if ( it -> checkState ( 0 ) == Qt::Checked )           {
      Uuids << nTreeUuid    ( it , 0 )                     ;
    }                                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if ( Uuids . count ( ) <= 0 )                            {
    if ( IsNull ( item ) )                                 {
      Alert ( Error )                                      ;
      return true                                          ;
    }                                                      ;
    Uuids << nTreeUuid ( item , 0 )                        ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if ( ( Uuids . count ( ) == 1 ) && NotNull ( item ) )    {
    emit TagSelected ( item->text(0) , Uuids[0] )          ;
    return true                                            ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  toVariants ( Uuids , args )                              ;
  switch ( id )                                            {
    case 501                                               :
      start ( 10102 , args )                               ;
    break                                                  ;
    case 502                                               :
      start ( 10101 , args )                               ;
    break                                                  ;
  }                                                        ;
  return true                                              ;
}

void N::VideoTags::SendPending(void)
{
  QList<PendingSignals *> PSs                                         ;
  mutex [ "Pending" ] . lock   ( )                                    ;
  while ( PendingItems . count ( ) > 0 )                              {
    PendingSignals * ps = (PendingSignals *) PendingItems . first ( ) ;
    PendingItems . takeFirst ( )                                      ;
    emit ListVideo        ( ps -> Name , ps -> Uuids )                ;
    plan -> processEvents (                          )                ;
    PSs << ps                                                         ;
  }                                                                   ;
  mutex [ "Pending" ] . unlock ( )                                    ;
  for (int i = 0 ; i < PSs . count ( ) ; i++ ) delete PSs [ i ]       ;
  PSs . clear ( )                                                     ;
}

void N::VideoTags::TagsUnion(ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return                           ;
  UUIDs U                                                    ;
  FromVariants ( d -> Arguments , U )                        ;
  if ( U . count ( ) <= 0 ) return                           ;
  bool              correct = true                           ;
  UUIDs             Z                                        ;
  QStringList       HL                                       ;
  QTreeWidgetItem * it                                       ;
  EnterSQL ( SC , plan -> sql )                              ;
    UUIDs V                                                  ;
    UUIDs X                                                  ;
    SUID  u = U . first ( )                                  ;
    U . takeFirst ( )                                        ;
    it = uuidAt ( u )                                        ;
    if ( NotNull ( it ) ) HL << it -> text ( 0 )             ;
    GetSecondUuids ( SC                                      ,
                     Z                                       ,
                     u                                       ,
                     Types::Tag                              ,
                     Types::Album                            ,
                     Groups::Subordination                 ) ;
    V = Uniqueness ( Z )                                     ;
    while ( U . count ( ) > 0 )                              {
      u = U . first  ( )                                     ;
      U . takeFirst  ( )                                     ;
      it = uuidAt ( u )                                      ;
      if ( NotNull ( it ) ) HL << it -> text ( 0 )           ;
      X . clear      ( )                                     ;
      GetSecondUuids ( SC                                    ,
                       X                                     ,
                       u                                     ,
                       Types::Tag                            ,
                       Types::Album                          ,
                       Groups::Subordination               ) ;
      Z = V + Uniqueness ( X )                               ;
      V = Z                                                  ;
    }                                                        ;
    Z = Uniqueness ( V )                                     ;
  ErrorSQL ( SC , plan -> sql )                              ;
    correct = false                                          ;
  LeaveSQL ( SC , plan -> sql )                              ;
  ////////////////////////////////////////////////////////////
  if ( Z . count ( ) <= 0 )                                  {
    Alert ( Error )                                          ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  PendingSignals * ps = new PendingSignals ( )               ;
  ps -> Name  = HL . join ( " + " )                          ;
  ps -> Uuids = Z                                            ;
  mutex [ "Pending" ] . lock   ( )                           ;
  PendingItems << ps                                         ;
  mutex [ "Pending" ] . unlock ( )                           ;
  emit DispatchPending ( )                                   ;
}

void N::VideoTags::TagsIntersection (ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return                           ;
  UUIDs U                                                    ;
  FromVariants ( d -> Arguments , U )                        ;
  if ( U . count ( ) <= 0 ) return                           ;
  bool              correct = true                           ;
  UUIDs             Z                                        ;
  QStringList       HL                                       ;
  QTreeWidgetItem * it                                       ;
  EnterSQL ( SC , plan -> sql )                              ;
    UUIDs V                                                  ;
    UUIDs X                                                  ;
    SUID  u = U . first ( )                                  ;
    U . takeFirst ( )                                        ;
    it = uuidAt ( u )                                        ;
    if ( NotNull ( it ) ) HL << it -> text ( 0 )             ;
    GetSecondUuids ( SC                                      ,
                     Z                                       ,
                     u                                       ,
                     Types::Tag                              ,
                     Types::Album                            ,
                     Groups::Subordination                 ) ;
    V = Uniqueness ( Z )                                     ;
    while ( ( Z . count ( ) > 0 ) && ( U . count ( ) > 0 ) ) {
      u = U . first  ( )                                     ;
      U . takeFirst  ( )                                     ;
      it = uuidAt ( u )                                      ;
      if ( NotNull ( it ) ) HL << it -> text ( 0 )           ;
      X . clear      ( )                                     ;
      GetSecondUuids ( SC                                    ,
                       X                                     ,
                       u                                     ,
                       Types::Tag                            ,
                       Types::Album                          ,
                       Groups::Subordination               ) ;
      Z = V & Uniqueness ( X )                               ;
      V = Z                                                  ;
    }                                                        ;
    Z = Uniqueness ( V )                                     ;
  ErrorSQL ( SC , plan -> sql )                              ;
    correct = false                                          ;
  LeaveSQL ( SC , plan -> sql )                              ;
  ////////////////////////////////////////////////////////////
  if ( Z . count ( ) <= 0 )                                  {
    Alert ( Error )                                          ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  PendingSignals * ps = new PendingSignals ( )               ;
  ps -> Name  = HL . join ( " & " )                          ;
  ps -> Uuids = Z                                            ;
  mutex [ "Pending" ] . lock   ( )                           ;
  PendingItems << ps                                         ;
  mutex [ "Pending" ] . unlock ( )                           ;
  emit DispatchPending ( )                                   ;
}
