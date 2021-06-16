#include <qtav.h>

N::AlbumView:: AlbumView ( QWidget * parent , Plan * p )
             : GroupView (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AlbumView::~AlbumView (void)
{
}

void N::AlbumView::Configure(void)
{
  type    = Types::Album                                  ;
  display = ThumbIcon                                     ;
  setAccessibleName            ( "N::AlbumView"        )  ;
  setObjectName                ( "N::AlbumView"        )  ;
  setDragDropMode              ( DragDrop              )  ;
  setViewMode                  ( IconMode              )  ;
  setIconSize                  ( QSize(128,128)        )  ;
  setGridSize                  ( QSize(144,192)        )  ;
  setMovement                  ( Snap                  )  ;
  setResizeMode                ( Adjust                )  ;
  setSelectionMode             ( ExtendedSelection     )  ;
  setWordWrap                  ( true                  )  ;
  setMouseTracking             ( true                  )  ;
  setWrapping                  ( true                  )  ;
  setTextElideMode             ( Qt::ElideNone         )  ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )  ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )  ;
  setMinimumSize               ( QSize(172,192)        )  ;
  setDropFlag                  ( DropText     , false  )  ;
  setDropFlag                  ( DropUrls     , false  )  ;
  setDropFlag                  ( DropImage    , true   )  ;
  setDropFlag                  ( DropHtml     , false  )  ;
  setDropFlag                  ( DropColor    , false  )  ;
  setDropFlag                  ( DropTag      , false  )  ;
  setDropFlag                  ( DropPicture  , true   )  ;
  setDropFlag                  ( DropPeople   , false  )  ;
  setDropFlag                  ( DropVideo    , false  )  ;
  setDropFlag                  ( DropAlbum    , true   )  ;
  setDropFlag                  ( DropGender   , false  )  ;
  setDropFlag                  ( DropDivision , true   )  ;
  plan -> setFont              ( this                  )  ;
  /////////////////////////////////////////////////////////
  LocalIcons [ "Refresh" ] = QIcon(":/images/update.png") ;
  LocalIcons [ "Add"     ] = QIcon(":/images/plus.png"  ) ;
  LocalIcons [ "Font"    ] = QIcon(":/images/font.png"  ) ;
}

UUIDs N::AlbumView::GroupUuids(SqlConnection & Connection)
{
  return Connection . Uuids                    (
           PlanTable(VideoGroups)              ,
           "uuid"                              ,
           Connection . OrderByDesc ( "id" ) ) ;
}

SUID N::AlbumView::AppendGroup(SqlConnection & Connection,QString name)
{
  SUID u = GroupItems::NewDivision(Connection,name,Types::Division) ;
  Connection . insertUuid ( PlanTable(VideoGroups) , u , "uuid" )   ;
  return u                                                          ;
}

QString N::AlbumView::ToolTipTotoal(int total)
{
  return tr("%1 albums in this group").arg(total) ;
}

int N::AlbumView::Counting(SqlConnection & Connection,SUID uuid)
{
  GroupItems GI ( plan )             ;
  GI . AutoMap    = true             ;
  GI . GroupTable = GI . LookTable   (
           Types  :: Division        ,
           type                      ,
           Groups :: Subordination ) ;
  return GI . Count                  (
           Connection                ,
           uuid                      ,
           Types  :: Division        ,
           type                      ,
           Groups :: Subordination ) ;
}

QString N::AlbumView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType            (
           mime                           ,
           "album/uuid;album/uuids;"
           "picture/uuid;picture/uuids;"
           "division/uuid;division/uuids"
         )                                ;
}

UUIDs N::AlbumView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                                           ;
  QByteArray data = mime->data(mimetype)                     ;
  if (data.size()<=0) return Uuids                           ;
  if (mimetype=="album/uuid"    ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="album/uuids"   ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="picture/uuid"  ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="picture/uuids" ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="division/uuid" ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="division/uuids") Uuids  = GetUuids ( data ) ;
  return Uuids                                               ;
}

void N::AlbumView::GroupPositions(RMAPs & Position,UUIDs & Uuids)
{
  int Id = Uuids . count() ;
  SUID uuid                ;
  foreach (uuid,Uuids)     {
    Position [ uuid ] = Id ;
    Id --                  ;
  }                        ;
}

bool N::AlbumView::MoveGroups(SUID atUuid,const UUIDs & Uuids)
{
  dropAction = true                                     ;
  SUID            u                                     ;
  UUIDs           ITEMs = ItemUuids ( )                 ;
  ListWidgetItems mItems                                ;
  foreach (u,Uuids)                                     {
    QListWidgetItem * it = uuidAt ( u )                 ;
    mItems << it                                        ;
    takeItem ( row ( it ) )                             ;
    ITEMs . takeAt ( ITEMs . indexOf (u) )              ;
  }                                                     ;
  QListWidgetItem * rit   = uuidAt ( atUuid )           ;
  int               rid   = row    ( rit    )           ;
  for (int i=0;i<mItems.count();i++,rid++)              {
    insertItem ( rid , mItems [ i ] )                   ;
  }                                                     ;
  rid = ITEMs . indexOf ( atUuid )                      ;
  if (rid<0) rid = 0                                    ;
  foreach (u,Uuids)                                     {
    ITEMs . insert ( rid , u )                          ;
    rid++                                               ;
  }                                                     ;
  RMAPs Position                                        ;
  GroupPositions(Position,ITEMs)                        ;
  ///////////////////////////////////////////////////////
  Bustle ( )                                            ;
  SqlConnection SC(plan->sql)                           ;
  if ( SC . open ( FunctionString ) )                   {
    QString Q                                           ;
    foreach (u,ITEMs)                                   {
      int Id = Position [ u ]                           ;
      Q = SC.sql.Update                                 (
            PlanTable(Divisions)                        ,
            QString("where uuid = %1 ").arg(u)          ,
            1,"position"                            )   ;
      SC . Prepare ( Q               )                  ;
      SC . Bind    ( "position" , Id )                  ;
      SC . Exec    (                 )                  ;
    }                                                   ;
    foreach (u,ITEMs)                                   {
      int Id = Position [ u ]                           ;
      Q = SC.sql.Update                                 (
            PlanTable(VideoGroups)                      ,
            QString("where uuid = %1 ").arg(u)          ,
            1,"id"                                  )   ;
      SC . Prepare ( Q          )                       ;
      SC . Bind    ( "id" , -Id )                       ;
      SC . Exec    (            )                       ;
    }                                                   ;
    foreach (u,ITEMs)                                   {
      int Id = Position [ u ]                           ;
      Q = SC.sql.Update                                 (
            PlanTable(VideoGroups)                      ,
            QString("where uuid = %1 ").arg(u)          ,
            1,"id"                                  )   ;
      SC . Prepare ( Q          )                       ;
      SC . Bind    ( "id" ,  Id )                       ;
      SC . Exec    (            )                       ;
    }                                                   ;
    SC.close()                                          ;
  }                                                     ;
  SC.remove()                                           ;
  ///////////////////////////////////////////////////////
  Vacancy  (      )                                     ;
  Alert    ( Done )                                     ;
  dropAction = false                                    ;
  return true                                           ;
}

bool N::AlbumView::JoinImage(SUID uuid,const QImage & image)
{
  bool result = setGroupIcon(uuid,image) ;
  QListWidgetItem * item = uuidAt(uuid)  ;
  Refresh ( item )                       ;
  return result                          ;
}

bool N::AlbumView::JoinPictures(SUID uuid,const UUIDs & Uuids)
{
  if (Uuids.count()<=0) return true            ;
  bool result = setGroupIcon( uuid , Uuids[0]) ;
  QListWidgetItem * item = uuidAt(uuid)        ;
  Refresh ( item )                             ;
  return result                                ;
}

void N::AlbumView::reportItems(void)
{
  QString m                                        ;
  m = tr ( "%1 album groups" ) . arg ( count ( ) ) ;
  emit assignToolTip                 ( m         ) ;
  plan -> Talk                       ( m         ) ;
}

bool N::AlbumView::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMenu           * me                                          ;
  QAction         * aa                                          ;
  QListWidgetItem * it                                          ;
  it = itemAt(pos)                                              ;
  mm . add ( 101 , LocalIcons [ "Add"     ] , tr("New album") ) ;
  if ( NotNull(it) ) mm . add ( 102 , tr("Rename") )            ;
  if ( NotNull(it) ) mm . add ( 103 , tr("Delete") )            ;
  mm . addSeparator ( )                                         ;
  mm . add ( 201 , LocalIcons [ "Refresh" ] , tr("Reflush")   ) ;
  if ( NotNull(it) ) mm . add ( 202 , tr("Reflush item")      ) ;
  mm . add ( 203 , LocalIcons [ "Font"    ] , tr("Set font")  ) ;
  mm . addSeparator ( )                                         ;
  mm . add ( 301 , tr("Copy to clipboard")                    ) ;
  mm . add ( 302 , tr("Clear selection"  )                    ) ;
  mm . add ( 303 , tr("Select all"       )                    ) ;
  me = mm . addMenu ( tr("Edit") )                              ;
  mm . add ( me , 211 , tr("Multilingual translations")       ) ;
  AdjustMenu     ( mm , me )                                    ;
  mm . setFont   ( plan    )                                    ;
  aa = mm . exec (         )                                    ;
  if ( IsNull        (      aa ) ) return true                  ;
  if ( RunAdjustment ( mm , aa ) ) return true                  ;
  UUIDs Tuids                                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      New             ( )                                       ;
    break                                                       ;
    case 102                                                    :
      Rename          ( )                                       ;
    break                                                       ;
    case 103                                                    :
      Delete          ( )                                       ;
    break                                                       ;
    case 201                                                    :
      startup         ( )                                       ;
    break                                                       ;
    case 202                                                    :
      Refresh         ( it )                                    ;
    break                                                       ;
    case 203                                                    :
      setFont         ( )                                       ;
    break                                                       ;
    case 211                                                    :
      Tuids = ItemUuids (                         )             ;
      emit Translations ( windowTitle ( ) , Tuids )             ;
    break                                                       ;
    case 301                                                    :
      CopyToClipboard ( )                                       ;
    break                                                       ;
    case 302                                                    :
      SelectNone      ( )                                       ;
    break                                                       ;
    case 303                                                    :
      SelectAll       ( )                                       ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}
