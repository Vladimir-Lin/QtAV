#include <qtav.h>

#pragma message("AlbumClip requires some revise working")

N::AlbumClip:: AlbumClip    ( QWidget * parent , Plan * p     )
             : TreeDock     (           parent ,        p     )
             , Ownership    ( 0                , Types::Album )
             , VideoManager (                           p     )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AlbumClip::~AlbumClip(void)
{
}

bool N::AlbumClip::hasItem(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  return NotNull ( item )                  ;
}

bool N::AlbumClip::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::AlbumClip::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

QMimeData * N::AlbumClip::dragMime(void)
{
  QMimeData * mime = standardMime("video")                   ;
  nKickOut ( IsNull(mime) , NULL )                           ;
  QImage image = windowIcon().pixmap(QSize(32,32)).toImage() ;
  mime -> setImageData(image)                                ;
  return mime                                                ;
}

void N::AlbumClip::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::AlbumClip::finishDrag(QMouseEvent * event)
{
  return true ;
}

QString N::AlbumClip::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(mime,"video/uuid;video/uuids");
}

UUIDs N::AlbumClip::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="video/uuid")            {
    SUID * suid = (SUID *)data.data()    ;
    Uuids << (*suid)                     ;
  } else
  if (mimetype=="video/uuids")           {
    SUID * suid  = (SUID *)data.data()   ;
    int    total = (int)suid[0]          ;
    for (int i=0;i<total;i++)            {
      Uuids << suid[i+1]                 ;
      plan->processEvents()              ;
    }                                    ;
  }                                      ;
  return Uuids                           ;
}

bool N::AlbumClip::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler(mime) ;
}

bool N::AlbumClip::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::AlbumClip::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::AlbumClip::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::AlbumClip::dropVideos(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                        ;
  Q_UNUSED ( pos    )                        ;
  if ( Uuids . count ( ) <= 0 ) return false ;
  return DropClips ( Uuids )                 ;
}

bool N::AlbumClip::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( GoUp         , ItemUp      ( ) ) ;
  LinkAction        ( GoDown       , ItemDown    ( ) ) ;
  return true                                          ;
}

void N::AlbumClip::Configure(void)
{
  NewTreeWidgetItem       ( head                               ) ;
  head -> setIcon         ( 0,QIcon(":/images/videoclip.png" ) ) ;
  head -> setText         ( 0,tr("Clip name")                  ) ;
  ////////////////////////////////////////////////////////////////
  setAccessibleName       ( "N::AlbumClip"                     ) ;
  setObjectName           ( "N::AlbumClip"                     ) ;
  setColumnCount          ( 1                                  ) ;
  setRootIsDecorated      ( false                              ) ;
  setAlternatingRowColors ( true                               ) ;
  setDragDropMode         ( DragDrop                           ) ;
  setSelectionMode        ( ExtendedSelection                  ) ;
  setAllAlignments        ( head , Qt::AlignCenter             ) ;
  setFont                 ( head , Fonts::ListView             ) ;
  setHeaderItem           ( head                               ) ;
  setWindowTitle          ( tr("Edit album clips")             ) ;
  setAccessibleName       ( "nAlbumClip"                       ) ;
  plan->setFont           ( this                               ) ;
  setDropFlag             ( DropVideo , true                   ) ;
  ////////////////////////////////////////////////////////////////
  MountClicked            ( 1                                  ) ;
  MountClicked            ( 2                                  ) ;
  ////////////////////////////////////////////////////////////////
  setFunction             ( 16701 , true                       ) ;
  setFunction             ( 16702 , true                       ) ;
  setFunction             ( 16703 , true                       ) ;
}

void N::AlbumClip::run(int T,ThreadData * data)
{
  VarArgs V                ;
  V = data -> Arguments    ;
  switch ( T )             {
    case 10001             :
      startLoading   (   ) ;
      Reload         (   ) ;
      stopLoading    (   ) ;
    break                  ;
    case 10002             :
      startLoading   (   ) ;
      Remove         (   ) ;
      stopLoading    (   ) ;
    break                  ;
    case 10003             :
      startLoading   (   ) ;
      UpdatePosition (   ) ;
      stopLoading    (   ) ;
    break                  ;
    case 10004             :
      startLoading   (   ) ;
      AppendClips    ( V ) ;
      stopLoading    (   ) ;
    break                  ;
  }                        ;
}

bool N::AlbumClip::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::AlbumClip::Reload(void)
{
  emit assignEnabling ( false       )                  ;
  LockGui             (             )                  ;
  SqlConnection SC    ( plan -> sql )                  ;
  if ( SC . open ( FunctionString ) )                  {
    UUIDs Uuids = Clips(SC,ObjectUuid())               ;
    SUID  uuid                                         ;
    foreach (uuid,Uuids)                               {
      QStringList Names = VideoManager::Names(SC,uuid) ;
      NewTreeWidgetItem (IT)                           ;
      IT->setData(0,Qt::UserRole,uuid)                 ;
      if (Names.count()>0) IT->setText(0,Names[0])     ;
      addTopLevelItem ( IT          )                  ;
    }                                                  ;
    SC . close        (             )                  ;
  }                                                    ;
  SC   . remove       (             )                  ;
  UnlockGui           (             )                  ;
  emit assignEnabling ( true        )                  ;
  if ( isFunction ( 16703 ) )                          {
    Alert             ( Done        )                  ;
  }                                                    ;
}

bool N::AlbumClip::DropClips(const UUIDs & U)
{
  VarArgs V                ;
  toVariants ( U     , V ) ;
  start      ( 10004 , V ) ;
  return true              ;
}

bool N::AlbumClip::AppendClips(VarArgs & args)
{
  UUIDs Uuids                                               ;
  UUIDs X = itemUuids ( 0            )                      ;
  ///////////////////////////////////////////////////////////
  FromVariants        ( args , Uuids )                      ;
  emit assignEnabling ( false        )                      ;
  LockGui             (              )                      ;
  ///////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                          ;
  if ( SC . open ( FunctionString ) )                       {
    SUID  u                                                 ;
    foreach ( u , Uuids )                                   {
      if ( ( u > 0 ) && ( ! X . contains ( u ) ) )          {
        QStringList names = VideoManager::Names ( SC , u )  ;
        NewTreeWidgetItem (IT)                              ;
        IT->setData(0,Qt::UserRole,u)                       ;
        if (names.count()>0) IT->setText(0,names[0])        ;
        addTopLevelItem ( IT )                              ;
        VideoManager::AlbumClip ( SC , ObjectUuid ( ) , u ) ;
      }                                                     ;
    }                                                       ;
    SC . close        (              )                      ;
  }                                                         ;
  SC   . remove       (              )                      ;
  UnlockGui           (              )                      ;
  emit assignEnabling ( true         )                      ;
  return true                                               ;
}

UUIDs N::AlbumClip::UuidOrders(void)
{
  UUIDs Uuids                             ;
  SUID  uuid                              ;
  for (int i=0;i<topLevelItemCount();i++) {
    QTreeWidgetItem * it                  ;
    it = topLevelItem(i)                  ;
    uuid = nTreeUuid ( it , 0 )           ;
    if (uuid>0) Uuids << uuid             ;
  }                                       ;
  return Uuids                            ;
}

NAMEs N::AlbumClip::ClipNames(UUIDs & Uuids)
{
  NAMEs Clips                              ;
  LockGui          (             )         ;
  SqlConnection SC ( plan -> sql )         ;
  if ( SC . open ( FunctionString ) )      {
    Uuids = VideoManager::Clips     ( SC ) ;
    Clips = VideoManager::ClipLists ( SC ) ;
    SC . close  ( )                        ;
  }                                        ;
  SC   . remove ( )                        ;
  UnlockGui     ( )                        ;
  return Clips                             ;
}

QComboBox * N::AlbumClip::ClipSelection(void)
{
  UUIDs Uuids                                ;
  NAMEs Clips = ClipNames ( Uuids )          ;
  QComboBox * combo = new QComboBox ( this ) ;
  N::AddItems (*combo,Uuids,Clips)           ;
  return combo                               ;
}

void N::AlbumClip::Insert(void)
{
  removeOldItem ( )                       ;
  QComboBox * combo = ClipSelection ( )   ;
  NewTreeWidgetItem(IT)                   ;
  IT -> setData   (0,Qt::UserRole,0)      ;
  ItemEditing = IT                        ;
  ItemColumn  = 0                         ;
  ItemWidget  = combo                     ;
  addTopLevelItem (IT)                    ;
  DoProcessEvents                         ;
  scrollToItem    (IT)                    ;
  setCurrentItem  (IT)                    ;
  setItemWidget   (IT,0,combo)            ;
  DoProcessEvents                         ;
  connect(combo,SIGNAL(activated  (int))  ,
          this ,SLOT  (clipChanged(int))) ;
  combo -> setMaxVisibleItems ( 30 )      ;
  combo -> showPopup          (    )      ;
}

void N::AlbumClip::Delete(void)
{
  start ( 10002 ) ;
}

void N::AlbumClip::Remove(void)
{
  TreeWidgetItems Items = selectedItems()       ;
  UUIDs Uuids                                   ;
  SUID uuid                                     ;
  if (Items.count()<=0) return                  ;
  for (int i=0;i<Items.count();i++)             {
    int index                                   ;
    uuid = nTreeUuid(Items[i],0)                ;
    if (uuid>0) Uuids << uuid                   ;
    index = indexOfTopLevelItem(Items[i])       ;
    takeTopLevelItem(index)                     ;
  }                                             ;
  if (Uuids.count()<=0) return                  ;
  ///////////////////////////////////////////////
  LockGui          (             )              ;
  SqlConnection SC ( plan -> sql )              ;
  if ( SC . open ( FunctionString ) )           {
    foreach (uuid,Uuids)                        {
      RemoveClip ( SC , ObjectUuid() , uuid )   ;
    }                                           ;
    Uuids = Clips ( SC , ObjectUuid() )         ;
    AlbumOrders   ( SC , ObjectUuid() , Uuids ) ;
    SC . close  ( )                             ;
  }                                             ;
  SC   . remove ( )                             ;
  UnlockGui     ( )                             ;
}

void N::AlbumClip::UpdatePosition(void)
{
  UUIDs Uuids = UuidOrders ( )                  ;
  SqlConnection SC ( plan -> sql )              ;
  if ( SC . open ( FunctionString ) )           {
    AlbumOrders ( SC , ObjectUuid ( ) , Uuids ) ;
    SC . close  ( )                             ;
  }                                             ;
  SC   . remove ( )                             ;
}

void N::AlbumClip::ItemUp(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  int  index = indexOfTopLevelItem(item)   ;
  if (index<=0) return                     ;
  takeTopLevelItem   ( index            )  ;
  insertTopLevelItem ( index - 1 , item )  ;
  setCurrentItem     ( item             )  ;
  start              ( 10003            )  ;
}

void N::AlbumClip::ItemDown(void)
{
  QTreeWidgetItem * item = currentItem ( )   ;
  if (IsNull(item)) return                   ;
  int  index = indexOfTopLevelItem(item) + 1 ;
  if (index>=topLevelItemCount()) return     ;
  takeTopLevelItem   ( index - 1    )        ;
  insertTopLevelItem ( index , item )        ;
  setCurrentItem     ( item         )        ;
  start              ( 10003        )        ;
}

void N::AlbumClip::singleClicked(QTreeWidgetItem * item,int column)
{
  if (item==ItemEditing && column==ItemColumn) return ;
  removeOldItem ()                                    ;
  Alert ( Click )                                     ;
}

void N::AlbumClip::doubleClicked(QTreeWidgetItem * item,int)
{
  removeOldItem         (          )            ;
  SUID uuid = nTreeUuid ( item , 0 )            ;
  QComboBox * combo = ClipSelection()           ;
  setItemWidget   (item,0,combo)                ;
  DoProcessEvents                               ;
    combo  -> setMaxVisibleItems (30)           ;
  (*combo) <= (SUID)uuid                        ;
  ItemEditing = item                            ;
  ItemColumn  = 0                               ;
  ItemWidget  = combo                           ;
  nConnect ( combo , SIGNAL(activated  (int))   ,
             this  , SLOT  (clipChanged(int)) ) ;
  combo -> showPopup ( )                        ;
}

void N::AlbumClip::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                  ;
  SUID uuid    = nTreeUuid(ItemEditing,ItemColumn) ;
  QString name = ItemEditing->text(0)              ;
  removeItemWidget (ItemEditing,ItemColumn)        ;
  if (uuid<=0 && name.length()<=0)                 {
    int index = indexOfTopLevelItem(ItemEditing)   ;
    takeTopLevelItem(index)                        ;
  }                                                ;
  ItemEditing = NULL                               ;
  ItemWidget  = NULL                               ;
  ItemColumn  = -1                                 ;
}

void N::AlbumClip::clipChanged(int index)
{
  QComboBox * combo = qobject_cast<QComboBox *>(ItemWidget)     ;
  if (IsNull(combo)) return                                     ;
  SUID uuid = combo->itemData(index,Qt::UserRole).toULongLong() ;
  if (uuid>0)                                                   {
    bool duplicated = false                                     ;
    for (int i=0;!duplicated && i<topLevelItemCount();i++)      {
      QTreeWidgetItem * IT = topLevelItem(i)                    ;
      SUID nuid = IT -> data ( 0 , Qt::UserRole ).toULongLong() ;
      duplicated = (nuid==uuid)                                 ;
    }                                                           ;
    if (!duplicated)                                            {
      ItemEditing -> setData ( 0 , Qt::UserRole , uuid    )     ;
      ItemEditing -> setText ( 0 , combo -> currentText() )     ;
      AbstractGui::Mutex.lock()                                 ;
      SqlConnection SC(plan->sql)                               ;
      if (SC.open("nAlbumClip","clipChanged"))                  {
        VideoManager::AlbumClip(SC,ObjectUuid(),uuid)           ;
        SC.close()                                              ;
      }                                                         ;
      SC.remove()                                               ;
      AbstractGui::Mutex.unlock()                               ;
    }                                                           ;
  }                                                             ;
  removeOldItem ( )                                             ;
}

bool N::AlbumClip::Menu(QPoint pos)
{
  if ( ! isFunction ( 16701 ) ) return true                 ;
  ///////////////////////////////////////////////////////////
  nScopedMenu ( mm , this )                                 ;
  QTreeWidgetItem * it = itemAt(pos)                        ;
  QAction         * aa                                      ;
  ///////////////////////////////////////////////////////////
  if ( NotNull ( it ) )                                     {
    mm . add ( 101 , tr("Play") )                           ;
  }                                                         ;
  if ( isFunction ( 16701 ) )                               {
    DockingMenu ( mm )                                      ;
  }                                                         ;
  mm . setFont(plan)                                        ;
  aa = mm.exec()                                            ;
  if (IsNull(aa)) return true                               ;
  ///////////////////////////////////////////////////////////
  if ( isFunction ( 16701 ) )                               {
    if ( RunDocking ( mm , aa ) ) return true               ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                      {
    case 101                                                :
      emit Play ( nTreeUuid ( it , 0 ) , it -> text ( 0 ) ) ;
    break                                                   ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  return true                                               ;
}
