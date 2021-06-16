#include <qtav.h>

#define hasMenu isFunction(1001)
#define ALERT(ID) AnotherALERT(N::Command::ID)

N::MusicAlbums:: MusicAlbums ( QWidget * parent , Plan * p                 )
               : SnapView    (           parent ,        p                 )
               , Group       ( 0 , Types::Division , Groups::Subordination )
               , dropAction  ( false                                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::MusicAlbums::~MusicAlbums (void)
{
}

void N::MusicAlbums::closeEvent(QCloseEvent * e)
{
  StopIcons                (   ) ;
  ListWidget :: closeEvent ( e ) ;
}

QMimeData * N::MusicAlbums::dragMime(void)
{
  PickedUuids . clear ( )                                        ;
  if (Items.count()<=0) return NULL                              ;
  QMimeData       * mime    = new QMimeData ( )                  ;
  QListWidgetItem * current = currentItem   ( )                  ;
  if (Items.count()==1)                                          {
    SUID        type = nListUuid(Items[0])                       ;
    QByteArray  data = CreateByteArray(type)                     ;
    mime -> setData ("music/uuid",data)                          ;
  } else                                                         {
    UUIDs       Uuids = Selected        (       )                ;
    QByteArray  data  = CreateByteArray ( Uuids )                ;
    mime -> setData ("music/uuids",data)                         ;
    PickedUuids = Uuids                                          ;
  }                                                              ;
  if (NotNull(current))                                          {
    QIcon  icon  = current->icon()                               ;
    if (!icon.isNull())                                          {
      QSize  s     = iconSize()                                  ;
      QImage image = icon . pixmap(s) . toImage()                ;
      if (!image.isNull())                                       {
        mime -> setImageData(image)                              ;
      }                                                          ;
    }                                                            ;
  }                                                              ;
  QString tt                                                     ;
  tt = tr("%1 music album selected") . arg ( Items . count ( ) ) ;
  QToolTip :: showText ( QCursor::pos ( ) , tt                 ) ;
  return mime                                                    ;
}

QString N::MusicAlbums::MimeType(const QMimeData * mime)
{
  return AbstractGui :: MimeType       (
           mime                        ,
           "music/uuid;music/uuids;"
           "album/uuid;album/uuids;"
           "video/uuid;video/uuids;"
           "picture/uuid;picture/uuids;"
           "people/uuid;people/uuids") ;
}

UUIDs N::MusicAlbums::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      U                                ;
  QByteArray data = mime -> data ( mimetype ) ;
  nKickOut ( data . size ( ) <= 0 , U       ) ;
  if (mimetype=="music/uuid")                 {
    U << GetUuid  ( data )                    ;
  } else
  if (mimetype=="music/uuids")                {
    U  = GetUuids ( data )                    ;
  } else
  if (mimetype=="album/uuid")                 {
    U << GetUuid  ( data )                    ;
  } else
  if (mimetype=="album/uuids")                {
    U  = GetUuids ( data )                    ;
  } else
  if (mimetype=="video/uuid")                 {
    U << GetUuid(data)                        ;
  } else
  if (mimetype=="video/uuids")                {
    U  = GetUuids ( data )                    ;
  } else
  if (mimetype=="picture/uuid")               {
    U << GetUuid(data)                        ;
  } else
  if (mimetype=="picture/uuids")              {
    U  = GetUuids ( data )                    ;
  } else
  if (mimetype=="people/uuid")                {
    U << GetUuid(data)                        ;
  } else
  if (mimetype=="people/uuids")               {
    U  = GetUuids ( data )                    ;
  }                                           ;
  return U                                    ;
}

bool N::MusicAlbums::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::MusicAlbums::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction , false )    ;
  QString    mtype                   ;
  UUIDs      Uuids                   ;
  alert ( "Click" , "" )             ;
  if (source==this)                  {
    mtype = MimeType  (mime      )   ;
    Uuids = MimeUuids (mime,mtype)   ;
    plan->showMessage                (
      tr("Start moving %1 album"     )
                .arg(Uuids.count())) ;
    plan->processEvents()            ;
  } else                             {
    if (mime->hasUrls())             {
    } else                           {
      mtype = MimeType  (mime      ) ;
      Uuids = MimeUuids (mime,mtype) ;
      if (mtype=="album/uuid"       ||
          mtype=="album/uuids"       )
      plan->showMessage              (
        tr("Copy %1 album from %2"   )
        .arg(Uuids.count()           )
        .arg(source->windowTitle())) ;
      if (mtype=="picture/uuid"     ||
          mtype=="picture/uuids"     )
      plan->showMessage              (
        tr("Copy %1 pictures from %2")
        .arg(Uuids.count()           )
        .arg(source->windowTitle())) ;
      if (mtype=="people/uuid"      ||
          mtype=="people/uuids"      )
      plan->showMessage              (
        tr("Copy %1 people from %2"  )
        .arg(Uuids.count()           )
        .arg(source->windowTitle())) ;
      if (mtype=="video/uuid"       ||
          mtype=="video/uuids"       )
      plan->showMessage              (
        tr("Copy %1 video from %2"   )
        .arg(Uuids.count()           )
        .arg(source->windowTitle())) ;
      plan->processEvents()          ;
    }                                ;
  }                                  ;
  return true                        ;
}

bool N::MusicAlbums::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction                 , false       ) ;
  nKickOut ( NotEqual ( source , this ) , true        ) ;
  QListWidgetItem * atItem = itemAt(pos)                ;
  nKickOut ( IsNull (atItem)            , false       ) ;
  if ( NotNull ( atItem ) && atItem -> isSelected ( ) ) {
    return false                                        ;
  }                                                     ;
  return true                                           ;
}

bool N::MusicAlbums::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut         ( dropAction , false  ) ;
  return dropItems ( source , mime , pos ) ;
}

bool N::MusicAlbums::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Delete       , Remove          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( GoHome       , PageHome        ( ) ) ;
  LinkAction        ( GoEnd        , PageEnd         ( ) ) ;
  LinkAction        ( PreviousPage , PageUp          ( ) ) ;
  LinkAction        ( NextPage     , PageDown        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  ALERT             ( Click                              ) ;
  return true                                              ;
}

void N::MusicAlbums::Configure(void)
{
  StackSize                    = 0                                 ;
  Reservation                  = false                             ;
  t2                           = Types::MusicAlbum                 ;
  AutoMap                      = true                              ;
  //////////////////////////////////////////////////////////////////
  LimitValues [  0 ]           =   0                               ;
  LimitValues [  1 ]           =  48                               ;
  LimitValues [  2 ]           =   0                               ;
  LimitValues [  9 ]           =  40                               ;
  LimitValues [ 11 ]           = (int)Qt::DescendingOrder          ;
  LimitValues [ 75 ]           =   0                               ;
  LimitValues [ 76 ]           = 100                               ;
  LimitValues [ 77 ]           = 500                               ;
  LimitValues [ 81 ]           =   1                               ;
  //////////////////////////////////////////////////////////////////
  LocalIcons  [ "Stop"      ]  = QIcon(":/images/stoprecord.png" ) ;
  LocalIcons  [ "Edit"      ]  = QIcon(":/images/edit.png"       ) ;
  LocalIcons  [ "Play"      ]  = QIcon(":/images/PlayerPlay.png" ) ;
  LocalIcons  [ "Plus"      ]  = QIcon(":/images/plus.png"       ) ;
  LocalIcons  [ "Update"    ]  = QIcon(":/images/update.png"     ) ;
  LocalIcons  [ "Music"     ]  = QIcon(":/images/music.png"      ) ;
  LocalIcons  [ "Details"   ]  = QIcon(":/images/checklist.png"  ) ;
  LocalIcons  [ "People"    ]  = QIcon(":/images/people.png"     ) ;
  LocalIcons  [ "Gallery"   ]  = QIcon(":/images/gallery.png"    ) ;
  LocalIcons  [ "Documents" ]  = QIcon(":/images/documents.png"  ) ;
  //////////////////////////////////////////////////////////////////
  dockingOrientation           = 0                                 ;
  dockingPlace                 = Qt::BottomDockWidgetArea          ;
  //////////////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Music albums")              ) ;
  setWindowIcon                ( QIcon ( ":/images/music.png" )  ) ;
  setViewMode                  ( IconMode                        ) ;
  setIconSize                  ( QSize ( 128 , 128 )             ) ;
  setGridSize                  ( QSize ( 140 , 192 )             ) ;
  setMovement                  ( Snap                            ) ;
  setResizeMode                ( Adjust                          ) ;
  setSelectionMode             ( ExtendedSelection               ) ;
  setWordWrap                  ( true                            ) ;
  setMouseTracking             ( true                            ) ;
  setWrapping                  ( true                            ) ;
  setSortingEnabled            ( false                           ) ;
  setTextElideMode             ( Qt::ElideNone                   ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded           ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded           ) ;
  setMinimumSize               ( QSize ( 144 , 192 )             ) ;
  setDragDropMode              ( DragDrop                        ) ;
  setAccessibleName            ( "N::MusicAlbums"                ) ;
  setObjectName                ( "N::MusicAlbums"                ) ;
  //////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropText     , true             ) ;
  setDropFlag                  ( DropUrls     , true             ) ;
  setDropFlag                  ( DropImage    , true             ) ;
  setDropFlag                  ( DropHtml     , true             ) ;
  setDropFlag                  ( DropColor    , false            ) ;
  setDropFlag                  ( DropTag      , true             ) ;
  setDropFlag                  ( DropPicture  , true             ) ;
  setDropFlag                  ( DropPeople   , true             ) ;
  setDropFlag                  ( DropVideo    , false            ) ;
  setDropFlag                  ( DropAlbum    , false            ) ;
  setDropFlag                  ( DropGender   , false            ) ;
  setDropFlag                  ( DropDivision , false            ) ;
  setDropFlag                  ( DropTorrent  , true             ) ;
  setDropFlag                  ( DropAudio    , true             ) ;
  setDropFlag                  ( DropMusic    , true             ) ;
  //////////////////////////////////////////////////////////////////
  setFunction                  ( 1001         , true             ) ;
  setFunction                  ( 1002         , true             ) ;
  setFunction                  ( 1003         , true             ) ;
  setFunction                  ( 1004         , true             ) ;
  setFunction                  ( 3101         , true             ) ;
  //////////////////////////////////////////////////////////////////
  plan -> setFont              ( this                            ) ;
  MountCommands                (                                 ) ;
}

bool N::MusicAlbums::dropText(QWidget * s,QPointF p,const QString & text)
{
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  SUID              album  = 0               ;
  ////////////////////////////////////////////
  if ( NotNull ( atItem ) )                  {
    album = nListUuid ( atItem )             ;
  }                                          ;
  ////////////////////////////////////////////
  dropAction = true                          ;
  ////////////////////////////////////////////
  VarArgs args                               ;
  args << album                              ;
  args << text                               ;
  ////////////////////////////////////////////
  dropAction = false                         ;
  ////////////////////////////////////////////
  start ( 81001 , args )                     ;
  ////////////////////////////////////////////
  return true                                ;
}

bool N::MusicAlbums::dropUrls(QWidget * s,QPointF p,const QList<QUrl> & urls)
{
  QPoint            pos    = p . toPoint ( )    ;
  QListWidgetItem * atItem = itemAt ( pos )     ;
  SUID              album  = 0                  ;
  ///////////////////////////////////////////////
  if ( NotNull ( atItem ) )                     {
    album = nListUuid ( atItem )                ;
  }                                             ;
  ///////////////////////////////////////////////
  dropAction = true                             ;
  ///////////////////////////////////////////////
  VarArgs args                                  ;
  args << album                                 ;
  for (int i = 0 ; i < urls . count ( ) ; i++ ) {
    args << urls [ i ]                          ;
  }                                             ;
  ///////////////////////////////////////////////
  dropAction = false                            ;
  ///////////////////////////////////////////////
  start ( 81002 , args )                        ;
  ///////////////////////////////////////////////
  return true                                   ;
}

bool N::MusicAlbums::dropImage(QWidget * s,QPointF p,const QImage & image)
{
  if ( s == this ) return true               ;
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  SUID              album  = 0               ;
  ////////////////////////////////////////////
  if ( NotNull ( atItem ) )                  {
    album = nListUuid ( atItem )             ;
  }                                          ;
  ////////////////////////////////////////////
  dropAction = true                          ;
  ////////////////////////////////////////////
  VarArgs args                               ;
  args << album                              ;
  args << image                              ;
  ////////////////////////////////////////////
  dropAction = false                         ;
  ////////////////////////////////////////////
  start ( 81003 , args )                     ;
  ////////////////////////////////////////////
  return true                                ;
}

bool N::MusicAlbums::dropHtml(QWidget * s,QPointF p,const QString & html)
{
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  SUID              album  = 0               ;
  ////////////////////////////////////////////
  if ( NotNull ( atItem ) )                  {
    album = nListUuid ( atItem )             ;
  }                                          ;
  ////////////////////////////////////////////
  dropAction = true                          ;
  ////////////////////////////////////////////
  VarArgs args                               ;
  args << album                              ;
  args << html                               ;
  ////////////////////////////////////////////
  dropAction = false                         ;
  ////////////////////////////////////////////
  start ( 81004 , args )                     ;
  ////////////////////////////////////////////
  return true                                ;
}

bool N::MusicAlbums::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if ( source == this    ) return true      ;
  QPoint pos = psf.toPoint()                ;
  QListWidgetItem * atItem = itemAt ( pos ) ;
  if ( IsNull ( atItem ) ) return true      ;
  SUID album = nListUuid ( atItem )         ;
  if ( album <= 0        ) return true      ;
  ///////////////////////////////////////////
  dropAction = true                         ;
  ///////////////////////////////////////////
  VarArgs args                              ;
  args << album                             ;
  toVariants ( Uuids , args )               ;
  ///////////////////////////////////////////
  dropAction = false                        ;
  ///////////////////////////////////////////
  start ( 80003 , args )                    ;
  ///////////////////////////////////////////
  return true                               ;
}

bool N::MusicAlbums::dropPictures(QWidget * s,QPointF p,const UUIDs & Uuids)
{
  if ( s == this         ) return true       ;
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  if ( IsNull ( atItem ) ) return true       ;
  ////////////////////////////////////////////
  SUID album = nListUuid ( atItem )          ;
  dropAction = true                          ;
  ////////////////////////////////////////////
  N::VarArgs V                               ;
  N::Group   G                               ;
  UUIDs      U    = Uuids                    ;
  G  . first      = album                    ;
  G  . second     = 0                        ;
  G  . t1         = Types  :: MusicAlbum     ;
  G  . t2         = Types  :: Picture        ;
  G  . relation   = Groups :: Subordination  ;
  G  . position   = 0                        ;
  G  . membership = 1.0                      ;
  V << 10011                                 ;
  G  . addParaments ( V     )                ;
  V << U . count    (       )                ;
  toVariants        ( U , V )                ;
  addSequence       ( V     )                ;
  ////////////////////////////////////////////
  dropAction = false                         ;
  emit TriggerCommand ( )                    ;
  return true                                ;
}

bool N::MusicAlbums::dropPeople(QWidget * s,QPointF p,const UUIDs & Uuids)
{
  if ( s == this         ) return true       ;
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  if ( IsNull ( atItem ) ) return true       ;
  ////////////////////////////////////////////
  SUID album = nListUuid ( atItem )          ;
  dropAction = true                          ;
  ////////////////////////////////////////////
  N::VarArgs V                               ;
  N::Group   G                               ;
  UUIDs      U    = Uuids                    ;
  G  . first      = album                    ;
  G  . second     = 0                        ;
  G  . t1         = Types  :: MusicAlbum     ;
  G  . t2         = Types  :: People         ;
  G  . relation   = Groups :: Subordination  ;
  G  . position   = 0                        ;
  G  . membership = 1.0                      ;
  V << 10011                                 ;
  G  . addParaments   ( V     )              ;
  V << U . count      (       )              ;
  toVariants          ( U , V )              ;
  addSequence         ( V     )              ;
  ////////////////////////////////////////////
  dropAction = false                         ;
  emit TriggerCommand (       )              ;
  return true                                ;
}

bool N::MusicAlbums::dropAudios(QWidget * s,QPointF p,const UUIDs & U)
{
  if ( s == this            ) return true ;
  QPoint pos = p . toPoint ( )            ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  if ( IsNull ( atItem )    ) return true ;
  /////////////////////////////////////////
  SUID album = nListUuid(atItem)          ;
  if ( album <= 0          ) return true  ;
  /////////////////////////////////////////
  dropAction = true                       ;
  /////////////////////////////////////////
  VarArgs args                            ;
  args << album                           ;
  toVariants ( U , args )                 ;
  /////////////////////////////////////////
  dropAction = false                      ;
  /////////////////////////////////////////
  start ( 80001 , args )                  ;
  /////////////////////////////////////////
  return true                             ;
}

bool N::MusicAlbums::dropMusic(QWidget * s,QPointF p,const UUIDs & U)
{
  QPoint            pos    = p . toPoint ( ) ;
  QListWidgetItem * atItem = itemAt ( pos )  ;
  SUID              album  = 0               ;
  if ( NotNull ( atItem ) )                  {
    album = nListUuid ( atItem )             ;
  }                                          ;
  ////////////////////////////////////////////
  VarArgs args                               ;
  dropAction = true                          ;
  args << album                              ;
  toVariants ( U , args )                    ;
  dropAction = false                         ;
  if ( s == this )                           {
    start ( 11001 , args )                   ;
  } else                                     {
    start ( 80002 , args )                   ;
  }                                          ;
  ////////////////////////////////////////////
  return true                                ;
}

bool N::MusicAlbums::contains(SUID u)
{
  for (int i=0;i<count();i++)         {
    QListWidgetItem * it = item ( i ) ;
    SUID s = ItemUuid ( it )          ;
    if ( s == u ) return true         ;
  }                                   ;
  return false                        ;
}

SUID N::MusicAlbums::appendAlbum(SqlConnection & SC,QString name)
{
  int     position = 0                                ;
  QString Q                                           ;
  SUID    uuid                                        ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid",25979) ;
  SC.assureUuid                                       (
    PlanTable(MajorUuid)                              ,
    uuid                                              ,
    Types::MusicAlbum                               ) ;
  SC . assureUuid                                     (
    PlanTable(Divisions)                              ,
    uuid                                              ,
    Types::MusicAlbum                               ) ;
  SC . assureName                                     (
    PlanTable(Names)                                  ,
    uuid                                              ,
    vLanguageId                                       ,
    name                                            ) ;
  /////////////////////////////////////////////////////
  Q = SC.sql.SelectFrom                               (
        "position"                                    ,
        PlanTable(Divisions)                          ,
        QString("where type = %1 "
                "order by position desc "
                "limit 0,1"                           )
        .arg(Types::MusicAlbum                    ) ) ;
  if (SC.Fetch(Q)) position = SC.Int(0)               ;
  position++                                          ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Divisions)                          ,
        SC.sql.Where(1,"uuid")                        ,
        1                                             ,
        "position"                                  ) ;
  SC . Prepare ( Q                                  ) ;
  SC . Bind    ( "uuid"     , uuid                  ) ;
  SC . Bind    ( "position" , position              ) ;
  SC . Exec    (                                    ) ;
  /////////////////////////////////////////////////////
  if ( uuid > 0 )                                     {
    UUIDs Juids                                       ;
    if ( isFirst  ( ) )                               {
      Juids << uuid                                   ;
      GroupItems::Join                                (
        SC                                            ,
        first                                         ,
        t1                                            ,
        t2                                            ,
        relation                                      ,
        0                                             ,
        Juids                                       ) ;
    } else
    if ( isSecond ( ) )                               {
      Juids << second                                 ;
      GroupItems::Join                                (
        SC                                            ,
        uuid                                          ,
        t1                                            ,
        t2                                            ,
        relation                                      ,
        0                                             ,
        Juids                                       ) ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return uuid                                         ;
}

bool N::MusicAlbums::setUuid (
       SqlConnection   & SC  ,
       SUID              u   ,
       QListWidgetItem * it  )
{
  PictureManager PM ( plan )                        ;
  GroupItems     GI ( plan )                        ;
  GI . AutoMap    = true                            ;
  GI . GroupTable = GI . LookTable                  (
                    Types  :: MusicAlbum            ,
                    Types  :: Picture               ,
                    Groups :: Icon                ) ;
  QSize is ( 128 , 128 )                            ;
  QString  name = SC . getName                      (
                    PlanTable(Names)                ,
                    "uuid"                          ,
                    vLanguageId                     ,
                    u                             ) ;
  SUID     puid = GI . FindSecond                   (
                  SC                                ,
                  u                                 ,
                  Types  :: MusicAlbum              ,
                  Types  :: Picture                 ,
                  Groups :: Icon                    ,
                  SC . OrderByAsc  ( "position" )   ,
                  SC . sql . Limit ( 0 , 1      ) ) ;
  QImage * image   = PM . Thumb ( SC , u )          ;
  QString  tooltip = name                           ;
  bool     exists  = false                          ;
  if ( NotNull ( image ) )                          {
    QIcon icon = PM . Icon ( image , is )           ;
    delete image                                    ;
    if ( IsNull ( it ) )                            {
      addItem ( u , icon , name , tooltip )         ;
    } else                                          {
      it -> setText    ( name    )                  ;
      it -> setIcon    ( icon    )                  ;
      it -> setToolTip ( tooltip )                  ;
    }                                               ;
    exists = true                                   ;
  } else                                            {
    if ( IsNull ( it ) )                            {
      addItem ( u                                   ,
                LocalIcons [ "Music" ]              ,
                name                                ,
                tooltip                           ) ;
    } else                                          {
      it -> setText    ( name                     ) ;
      it -> setIcon    ( LocalIcons [ "Music" ]   ) ;
      it -> setToolTip ( tooltip                  ) ;
    }                                               ;
  }                                                 ;
  return exists                                     ;
}

bool N::MusicAlbums::addUuid(SqlConnection & Connection,SUID uuid)
{
  return setUuid ( Connection , uuid , NULL ) ;
}

QListWidgetItem * N::MusicAlbums::newUuid(SqlConnection & SC,SUID u)
{
  QString name = SC . getName                                 (
                   PlanTable(Names)                           ,
                   "uuid"                                     ,
                   vLanguageId                                ,
                   u                                        ) ;
  return addItem ( u , LocalIcons [ "Music" ] , name , name ) ;
}

void N::MusicAlbums::FetchTOTALs(void)
{
  int total = 0                                 ;
  GroupItems GI ( plan )                        ;
  GI . AutoMap    = true                        ;
  GI . GroupTable = GI . LookTable              (
                    t1                          ,
                    t2                          ,
                    relation                  ) ;
  EnterSQL ( SC , plan -> sql )                 ;
    QString Q                                   ;
    if ( isFirst  ( ) )                         {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            GI . GroupTable                     ,
            FirstItem (first ,t1,t2,relation) ) ;
    } else
    if ( isSecond ( ) )                         {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            GI.GroupTable                       ,
            SecondItem(second,t1,t2,relation) ) ;
    } else                                      {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            PlanTable(Divisions)                ,
            QString ( "where `type` = %1"       )
            . arg   ( Types::MusicAlbum )     ) ;
    }                                           ;
    if ( SC . Fetch ( Q ) )                     {
      total = SC . Int ( 0 )                    ;
    }                                           ;
  LeaveSQL ( SC , plan -> sql )                 ;
  LimitValues [ 2 ] = total                     ;
}

void N::MusicAlbums::FetchUUIDs(void)
{
  UUIDs         Uuids                                                     ;
  int           startId =                 LimitValues [  0 ]              ;
  int           page    =                 LimitValues [  1 ]              ;
  int           total   =                 LimitValues [  2 ]              ;
  Qt::SortOrder order   = (Qt::SortOrder) LimitValues [ 11 ]              ;
  if ( page <= 0 ) page = total                                           ;
  UuidMaps . take ( 10001 )                                               ;
  GroupItems     GI ( plan )                                              ;
  GI . AutoMap    = true                                                  ;
  GI . GroupTable = GI . LookTable ( t1 , t2 , relation                 ) ;
  EnterSQL ( SC , plan -> sql )                                           ;
    if ( isFirst ( ) )                                                    {
      Uuids = GI . Subordination                                          (
                SC                                                        ,
                first                                                     ,
                t1                                                        ,
                t2                                                        ,
                relation                                                  ,
                SC . OrderBy     ( "position" , order                   ) ,
                SC . sql . Limit ( startId    , page                  ) ) ;
    } else
    if ( isSecond ( ) )                                                   {
      Uuids = GI . GetOwners                                              (
                SC                                                        ,
                second                                                    ,
                t1                                                        ,
                t2                                                        ,
                relation                                                  ,
                SC . OrderBy     ( "reversal" , order                   ) ,
                SC . sql . Limit ( startId    , page                  ) ) ;
    } else                                                                {
      QString WT                                                          ;
      WT    = QString ( "where `type` = %1" ) . arg ( Types::MusicAlbum ) ;
      Uuids = SC . Uuids                                                  (
                PlanTable(Divisions)                                      ,
                "uuid"                                                    ,
                QString ( "%1 %2 %3"                                      )
                . arg   ( WT                                              )
                . arg   ( SC . OrderBy     ( "position" , order )         )
                . arg   ( SC . sql . Limit ( startId    , page  )     ) ) ;
    }                                                                     ;
  LeaveSQL ( SC , plan -> sql )                                           ;
  UuidMaps [ 10001 ] = Uuids                                              ;
}

bool N::MusicAlbums::CountAudios(SqlConnection & SC,QListWidgetItem * it)
{
  nKickOut ( IsNull ( it ) , false )            ;
  SUID u = nListUuid ( it )                     ;
  nKickOut ( u <= 0        , false )            ;
  ///////////////////////////////////////////////
  int total = 0                                 ;
  GroupItems GI ( plan )                        ;
  GI . AutoMap    = true                        ;
  GI . GroupTable = GI . LookTable              (
                      Types  :: MusicAlbum      ,
                      Types  :: Audio           ,
                      Groups :: Subordination ) ;
  ///////////////////////////////////////////////
  total = GI . Count                            (
            SC                                  ,
            u                                   ,
            Types  :: MusicAlbum                ,
            Types  :: Audio                     ,
            Groups :: Subordination           ) ;
  ///////////////////////////////////////////////
  if ( total > 0 )                              {
    QString m                                   ;
    m   = tr("%1 audios").arg(total)            ;
    it -> setToolTip ( m )                      ;
  }                                             ;
  ///////////////////////////////////////////////
  return true                                   ;
}

bool N::MusicAlbums::ReportDetails(SqlConnection & SC,QListWidgetItem * it,int flags)
{
  switch ( flags )                 {
    case 3                         :
    return CountAudios ( SC , it ) ;
  }                                ;
  return false                     ;
}

void N::MusicAlbums::DetachAlbums(SqlConnection & Connection,UUIDs & Uuids)
{
  if ( ! isOwner ( ) ) return      ;
  GroupItems GI ( plan )           ;
  GI . AutoMap    = true           ;
  GI . GroupTable = GI . LookTable (
                    t1             ,
                    t2             ,
                    relation     ) ;
  GI . Detach                      (
    Connection                     ,
    first                          ,
    t1                             ,
    t2                             ,
    relation                       ,
    Uuids                        ) ;
}

void N::MusicAlbums::JoinMusic(SqlConnection & SC,SUID uuid)
{
  if ( ! isOwner ( ) ) return ;
  UUIDs U                     ;
  U << uuid                   ;
  if ( isFirst  ( ) )         {
    GroupItems :: Join        (
      SC                      ,
      first                   ,
      t1                      ,
      t2                      ,
      relation                ,
      0                       ,
      U                       ,
      false                 ) ;
  } else
  if ( isSecond ( ) )         {
    U << second               ;
    GroupItems :: JoinOwner   (
      SC                      ,
      second                  ,
      t1                      ,
      t2                      ,
      relation                ,
      U                       ,
      false                 ) ;
  }                           ;
}

void N::MusicAlbums::JoinMusic(SqlConnection & SC,SUID uuid,QListWidgetItem * Item)
{
  if ( ! isOwner ( ) ) return             ;
  Item -> setData ( Qt::UserRole , uuid ) ;
  JoinMusic       ( SC , uuid           ) ;
  setUuid         ( SC , uuid , Item    ) ;
}

void N::MusicAlbums::View(UUIDs & Uuids)
{
  emit clearItems       (                ) ;
  emit assignEnabling   ( false          ) ;
  emit assignWaitCursor (                ) ;
  DoProcessEvents                          ;
  LockGui               (                ) ;
  blockSignals          ( true           ) ;
  //////////////////////////////////////////
  int             PPL = LimitValues [ 76 ] ;
  int             PRC = LimitValues [ 77 ] ;
  LoopInt         LIP ( 0 , 0 , PPL )      ;
  QIcon           ICON                     ;
  ListWidgetItems ITEMs                    ;
  bool            GUI                      ;
  //////////////////////////////////////////
  ICON  = LocalIcons [ "Music" ]           ;
  ITEMs = SnapView :: New ( Uuids )        ;
  GUI   = ( ITEMs . count ( ) > PRC  )     ;
  for (int i=0;i<ITEMs.count();i++)        {
    ITEMs [ i ] -> setIcon ( ICON        ) ;
    ITEMs [ i ] -> setText ( " "         ) ;
    QListWidget :: addItem ( ITEMs [ i ] ) ;
    if ( GUI )                             {
      ++LIP                                ;
      if ( 0 == LIP.V )                    {
        DoProcessEvents                    ;
      }                                    ;
    }                                      ;
  }                                        ;
  //////////////////////////////////////////
  blockSignals           ( false         ) ;
  UnlockGui              (               ) ;
  emit assignArrowCursor (               ) ;
  emit assignEnabling    ( true          ) ;
  SnapView::Relocation   (               ) ;
  //////////////////////////////////////////
  QString TT                               ;
  QString MM                               ;
  LimitValues [ 75 ] ++                    ;
  TT = tr("Total %1 music albums, displayed %2 start at %3") ;
  MM = QString ( TT                        )
      . arg    ( LimitValues[2]            )
      . arg    ( ITEMs.count( )            )
      . arg    ( LimitValues[0]          ) ;
  DoProcessEvents                          ;
  emit assignToolTip    ( MM             ) ;
  //////////////////////////////////////////
  Group   G                                ;
  VarArgs V                                ;
  //////////////////////////////////////////
  G . first      = 0                       ;
  G . second     = 0                       ;
  G . t1         = Types  :: MusicAlbum    ;
  G . t2         = Types  :: Picture       ;
  G . relation   = Groups :: Icon          ;
  G . position   = 0                       ;
  G . membership = 1.0                     ;
  //////////////////////////////////////////
  addSequence           ( 20002          ) ;
  //////////////////////////////////////////
  V  . clear            (                ) ;
  V << 30002                               ;
  V << 3                                   ;
  addSequence           ( V              ) ;
  //////////////////////////////////////////
  V  . clear            (                ) ;
  V << 40001                               ;
  G . addParaments      ( V              ) ;
  addSequence           ( V              ) ;
  //////////////////////////////////////////
  emit TriggerCommand   (                ) ;
  DoProcessEvents                          ;
}

void N::MusicAlbums::StopIcons(void)
{
  stopForcely ( )                            ;
  while ( detection ( ) ) Time::skip ( 100 ) ;
  emit Leave  ( this )                       ;
}

void N::MusicAlbums::JoinText(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return           ;
  SUID    album = d -> Arguments [ 0 ] . toULongLong ( ) ;
  QString text  = d -> Arguments [ 1 ] . toString    ( ) ;
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  ALERT         ( Done )                                 ;
}

void N::MusicAlbums::JoinURLs(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return               ;
  SUID        album = d -> Arguments [ 0 ] . toULongLong ( ) ;
  QList<QUrl> URLs                                           ;
  ////////////////////////////////////////////////////////////
  for (int i = 1 ; i < d -> Arguments . count ( ) ; i++ )    {
    URLs << d -> Arguments [ i ] . toUrl ( )                 ;
  }                                                          ;
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  ALERT         ( Done )                                     ;
}

void N::MusicAlbums::JoinImage(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return              ;
  SUID   album   = d -> Arguments [ 0 ] . toULongLong   ( ) ;
  QImage I       = d -> Arguments [ 1 ] . value<QImage> ( ) ;
  SUID   puid    = 0                                        ;
  bool   success = false                                    ;
  ///////////////////////////////////////////////////////////
  PictureManager PM ( plan )                                ;
  success = PM . Import ( I , puid )                        ;
  if      ( puid <= 0 )                                     {
    ALERT ( Error     )                                     ;
    return                                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  UUIDs U                                                   ;
  U << puid                                                 ;
  ///////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                          ;
  GroupItems    GI ( plan        )                          ;
  if ( SC . open ( FunctionString ) )                       {
    GI . AutoMap    = true                                  ;
    GI . GroupTable = GI . LookTable                        (
      Types  :: MusicAlbum                                  ,
      Types  :: Picture                                     ,
      Groups :: Subordination                             ) ;
    GI . Join                                               (
      SC                                                    ,
      album                                                 ,
      Types::MusicAlbum                                     ,
      Types::Picture                                        ,
      Groups::Subordination                                 ,
      0                                                     ,
      U                                                   ) ;
    ALERT       ( Done  )                                   ;
    SC . close  (       )                                   ;
  } else                                                    {
    ALERT       ( Error )                                   ;
  }                                                         ;
  SC   . remove (       )                                   ;
  ///////////////////////////////////////////////////////////
}

void N::MusicAlbums::JoinHtml(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return           ;
  SUID    album = d -> Arguments [ 0 ] . toULongLong ( ) ;
  QString html  = d -> Arguments [ 1 ] . toString    ( ) ;
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  ALERT         ( Done )                                 ;
}

void N::MusicAlbums::JoinTags(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return          ;
  SUID  album = d -> Arguments [ 0 ] . toULongLong ( )  ;
  UUIDs U                                               ;
  FromVariants ( 1 , d -> Arguments , U )               ;
  ///////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                      ;
  GroupItems    GI ( plan        )                      ;
  ///////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                   {
    GI . AutoMap    = true                              ;
    GI . GroupTable = GI . LookTable                    (
                        Types  :: Tag                   ,
                        Types  :: MusicAlbum            ,
                        Groups :: Subordination       ) ;
    GI . JoinOwner ( SC                                 ,
                     album                              ,
                     Types::Tag                         ,
                     Types::MusicAlbum                  ,
                     Groups::Subordination              ,
                     U                                ) ;
    SC . close  (      )                                ;
  }                                                     ;
  SC   . remove (      )                                ;
  ///////////////////////////////////////////////////////
  ALERT         ( Done )                                ;
}

void N::MusicAlbums::JoinAudios(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return         ;
  SUID  album = d -> Arguments [ 0 ] . toULongLong ( ) ;
  UUIDs U                                              ;
  FromVariants ( 1 , d -> Arguments , U )              ;
  //////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                     ;
  GroupItems    GI ( plan        )                     ;
  //////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                  {
    GI . AutoMap    = true                             ;
    GI . GroupTable = GI . LookTable                   (
                        Types  :: MusicAlbum           ,
                        Types  :: Audio                ,
                        Groups :: Subordination      ) ;
    GI . Join                                          (
      SC                                               ,
      album                                            ,
      Types::MusicAlbum                                ,
      Types::Audio                                     ,
      Groups::Subordination                            ,
      0                                                ,
      U                                              ) ;
    SC . close  (      )                               ;
  }                                                    ;
  SC   . remove (      )                               ;
  //////////////////////////////////////////////////////
  ALERT         ( Done )                               ;
}

void N::MusicAlbums::JoinAlbums(ThreadData * d)
{
  if      ( ! isOwner ( ) )                             {
    ALERT ( Error         )                             ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if ( d -> Arguments . count ( ) < 2 ) return          ;
  SUID  album = d -> Arguments [ 0 ] . toULongLong ( )  ;
  UUIDs U                                               ;
  FromVariants ( 1 , d -> Arguments , U )               ;
  ///////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                      ;
  GroupItems    GI ( plan        )                      ;
  ///////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                   {
    GI . AutoMap    = true                              ;
    GI . GroupTable = GI . LookTable                    (
                        t1                              ,
                        t2                              ,
                        relation                      ) ;
    if               ( isFirst  ( )                   ) {
      GI . Join      ( SC                               ,
                       first                            ,
                       t1                               ,
                       t2                               ,
                       relation                         ,
                       album                            ,
                       U                              ) ;
      ALERT         ( Done )                            ;
    } else
    if               ( isSecond ( )                   ) {
      GI . JoinOwner ( SC                               ,
                       second                           ,
                       t1                               ,
                       t2                               ,
                       relation                         ,
                       U                              ) ;
      ALERT         ( Done )                            ;
    }                                                   ;
    SC . close  (      )                                ;
  }                                                     ;
  SC   . remove (      )                                ;
  ///////////////////////////////////////////////////////
}

void N::MusicAlbums::AppendMusic(ThreadData * d)
{
  if ( d -> Arguments . count ( ) <= 0 ) {
    ALERT ( Error )                      ;
    return                               ;
  }                                      ;
  ////////////////////////////////////////
  QStringList S                          ;
  QString     T                          ;
  S << d -> Arguments                    ;
  ////////////////////////////////////////
  EnterSQL        ( SC , plan -> sql   ) ;
    foreach       ( T  , S             ) {
      appendAlbum ( SC , T             ) ;
    }                                    ;
    ALERT         ( Done               ) ;
  ErrorSQL        ( SC , plan -> sql   ) ;
  ALERT           ( Error              ) ;
  LeaveSQL        ( SC , plan -> sql   ) ;
  ////////////////////////////////////////
  pendingStartup  (                    ) ;
}

void N::MusicAlbums::Move(SUID atUuid,const UUIDs & Uuids)
{
  #pragma message("Move albums require another kind of implementation")
  ALERT ( Error )                                        ;
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  SUID            uuid                                   ;
  UUIDs           ITEMs = ItemUuids ( )                  ;
  ListWidgetItems mItems                                 ;
  foreach (uuid,Uuids)                                   {
    QListWidgetItem * it = uuidAt ( uuid )               ;
    mItems << it                                         ;
    takeItem ( row ( it ) )                              ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )            ;
  }                                                      ;
  QListWidgetItem * rit   = uuidAt ( atUuid )            ;
  int               rid   = row    ( rit    )            ;
  for (int i=0;i<mItems.count();i++,rid++)               {
    insertItem ( rid , mItems [ i ] )                    ;
  }                                                      ;
  rid = ITEMs . indexOf ( atUuid )                       ;
  if (rid<0) rid = 0                                     ;
  foreach (uuid,Uuids)                                   {
    ITEMs . insert ( rid , uuid )                        ;
    rid++                                                ;
  }                                                      ;
  RMAPs Position                                         ;
  int xId = 1                                            ;
  foreach (uuid,ITEMs)                                   {
    Position [ uuid ] = xId                              ;
    xId ++                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  Bustle ( )                                             ;
  SqlConnection SC ( plan->sql )                         ;
  GroupItems GI ( plan )                                 ;
  GI.AutoMap    = true                                   ;
  GI.GroupTable = GI . LookTable ( t1,t2,relation      ) ;
  if (SC.open("PeopleView","MovePeople"))                {
    QString Q                                            ;
    if ( isFirst ( ) )                                   {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "position"                               ) ;
        SC . Prepare ( Q               )                 ;
        SC . Bind    ( "position" , Id )                 ;
        SC . Exec    (                 )                 ;
      }                                                  ;
    } else
    if ( isSecond ( ))                                   {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "reversal"                               ) ;
        SC . Prepare ( Q               )                 ;
        SC . Bind    ( "reversal" , Id )                 ;
        SC . Exec    (                 )                 ;
      }                                                  ;
    } else                                               {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Id += 1000000000                                 ;
        Q = SC.sql.Update                                (
              PlanTable(PeopleUuid)                      ,
              SC.WhereUuid(uuid)                         ,
              1                                          ,
              "id"                                     ) ;
        SC . Prepare ( Q         )                       ;
        SC . Bind    ( "id" , Id )                       ;
        SC . Exec    (           )                       ;
      }                                                  ;
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              PlanTable(PeopleUuid)                      ,
              SC.WhereUuid(uuid)                         ,
              1                                          ,
              "id"                                     ) ;
        SC . Prepare ( Q         )                       ;
        SC . Bind    ( "id" , Id )                       ;
        SC . Exec    (           )                       ;
      }                                                  ;
      Q = QString("alter table %1 auto_increment = %2 ;" )
          .arg(PlanTable(PeopleUuid)                     )
          .arg(ITEMs.count()+1                         ) ;
      SC . Query ( Q )                                   ;
    }                                                    ;
    SC.close()                                           ;
  }                                                      ;
  SC.remove()                                            ;
  ////////////////////////////////////////////////////////
  Vacancy  ( )                                           ;
  ALERT ( Done )                                         ;
#endif
}

void N::MusicAlbums::setPositions(ThreadData * d)
{
  UUIDs U = ItemUuids ( )                                                   ;
  nDropOut ( U . count ( ) <= 0 )                                           ;
  ///////////////////////////////////////////////////////////////////////////
  int    ID                                                                 ;
  qint64 Value = 0                                                          ;
  bool   Go    = true                                                       ;
  ///////////////////////////////////////////////////////////////////////////
  GroupItems GI ( plan             )                                        ;
  EnterSQL      ( SC , plan -> sql )                                        ;
    QString Q                                                               ;
    SUID    u                                                               ;
    int     position = 0                                                    ;
    /////////////////////////////////////////////////////////////////////////
    GI . AutoMap     = true                                                 ;
    GI . GroupTable  = GI . LookTable ( t1 , t2 , relation )                ;
    /////////////////////////////////////////////////////////////////////////
    ID    = plan -> Progress ( windowTitle ( ) , tr("%v/%m music albums") ) ;
    plan -> Start    ( ID , &Value , &Go           )                        ;
    plan -> setRange ( ID , 0      , U . count ( ) )                        ;
    /////////////////////////////////////////////////////////////////////////
    if ( isFirst  ( ) )                                                     {
      foreach ( u , U )                                                     {
        ++Value                                                             ;
        ++position                                                          ;
        Q = SC . sql . Update                                               (
              GI . GroupTable                                               ,
              ExactItem ( first , u , t1 , t2 , relation )                  ,
              1                                                             ,
              "position"                                                  ) ;
        SC . Prepare ( Q                                                  ) ;
        SC . Bind    ( "position" , position                              ) ;
        SC . Exec    (                                                    ) ;
      }                                                                     ;
    } else
    if ( isSecond ( ) )                                                     {
      foreach ( u , U )                                                     {
        ++Value                                                             ;
        ++position                                                          ;
        Q = SC . sql . Update                                               (
              GI . GroupTable                                               ,
              ExactItem ( first , u , t1 , t2 , relation )                  ,
              1                                                             ,
              "reversal"                                                  ) ;
        SC . Prepare ( Q                                                  ) ;
        SC . Bind    ( "reversal" , position                              ) ;
        SC . Exec    (                                                    ) ;
      }                                                                     ;
    } else                                                                  {
      foreach ( u , U )                                                     {
        ++Value                                                             ;
        ++position                                                          ;
        Q = SC . sql . Update                                               (
              PlanTable(Divisions)                                          ,
              SC . sql . Where                                              (
                1                                                           ,
                "uuid"                                                    ) ,
                1                                                           ,
                "position"                                                ) ;
        SC . Prepare ( Q                                                  ) ;
        SC . Bind    ( "position" , position                              ) ;
        SC . Bind    ( "uuid"     , u                                     ) ;
        SC . Exec    (                                                    ) ;
      }                                                                     ;
    }                                                                       ;
    /////////////////////////////////////////////////////////////////////////
    plan -> Finish ( ID               )                                     ;
    /////////////////////////////////////////////////////////////////////////
    ALERT          ( Done             )                                     ;
  ErrorSQL         ( SC , plan -> sql )                                     ;
    ALERT          ( Error            )                                     ;
  LeaveSQL         ( SC , plan -> sql )                                     ;
}

void N::MusicAlbums::run(int T,ThreadData * d)
{
  ItemRunner ( T , d ) ;
}

bool N::MusicAlbums::ItemRunner(int T,ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return false         ;
  VarArgs  V      = d -> Arguments               ;
  bool     loaded = false                        ;
  QString  TT                                    ;
  N::Group group                                 ;
  UUIDs    U                                     ;
  SUID     u                                     ;
  int      t                                     ;
  startLoading              (                  ) ;
  switch                    ( T                ) {
    case 10002                                   :
      TT = tr("Counting music albums...")        ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchTOTALs           (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 10003            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 10003                                   :
      TT = tr("Loading music albums...")         ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchUUIDs            (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      if   ( LimitValues[1] > 0                ) {
        if ( UuidMaps . contains ( 10001 )     ) {
          UUIDs U = UuidMaps [ 10001 ]           ;
          if (U.count()>0)                       {
            V  . clear          (              ) ;
            V << 10004                           ;
            V << U.count        (              ) ;
            toVariants          ( U , V        ) ;
            addSequence         ( V            ) ;
            emit TriggerCommand (              ) ;
            loaded = true                        ;
          }                                      ;
        }                                        ;
      }                                          ;
      ////////////////////////////////////////////
      if                    ( ! loaded         ) {
        TT = tr("No music album found")          ;
        emit assignToolTip  ( TT               ) ;
      }                                          ;
    break                                        ;
    case 10011                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if                    ( group.first > 0  ) {
        JoinGroupThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10012                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if                    ( group.first > 0  ) {
        JoinGroupThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
      ////////////////////////////////////////////
      pendingStartup        (                  ) ;
    break                                        ;
    case 10013                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if                    ( group.first > 0  ) {
        JoinOwnerThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10014                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if                    ( group.first > 0  ) {
        JoinOwnerThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
      ////////////////////////////////////////////
      pendingStartup        (                  ) ;
    break                                        ;
    case 11001                                   :
      u = V [ 0 ] . toULongLong (              ) ;
      ArgsToUuids           ( 1 , V , U        ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      Move                  ( u , U            ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 12001            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 13001                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      ResumeLanguage        (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 10002            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 20002                                   :
      emit OnBusy           (                  ) ;
      NameThreads           ( 20101            ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20003                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      setPositions          ( d                ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20004                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchTOTALs           (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20101                                   :
      emit OnBusy           (                  ) ;
      NameThread            ( V                ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 30001                                   :
      ArgsToGroup           ( 0 , V , group    ) ;
      emit OnBusy           (                  ) ;
      SnapView::IconsThread ( group            ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 30002                                   :
      if (V.count()>0)                           {
        t = V [ 0 ] . toInt (                  ) ;
        emit OnBusy         (                  ) ;
        DetailsThread       ( t                ) ;
        IsCompleted         (                  ) ;
        emit GoRelax        (                  ) ;
      }                                          ;
    break                                        ;
    case 30003                                   :
      emit OnBusy           (                  ) ;
      SnapView::IconsThread ( V                ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80001                                   :
      emit OnBusy           (                  ) ;
      JoinAudios            ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80002                                   :
      emit OnBusy           (                  ) ;
      JoinAlbums            ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80003                                   :
      emit OnBusy           (                  ) ;
      JoinTags              ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 81001                                   :
      emit OnBusy           (                  ) ;
      JoinText              ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 81002                                   :
      emit OnBusy           (                  ) ;
      JoinURLs              ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 81003                                   :
      emit OnBusy           (                  ) ;
      JoinImage             ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 81004                                   :
      emit OnBusy           (                  ) ;
      JoinHtml              ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 82001                                   :
      emit OnBusy           (                  ) ;
      AppendMusic           ( d                ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
  }                                              ;
  stopLoading               (                  ) ;
  return false                                   ;
}

bool N::MusicAlbums::RunCommand(VarArgs & arguments)
{
  if (arguments.count()<1) return false  ;
  VarArgs  V = arguments                 ;
  N::Group group                         ;
  UUIDs    U                             ;
  int      c = V [ 0 ] . toInt ( )       ;
  switch         ( c     )               {
    case 10001                           :
      startup    (       )               ;
    break                                ;
    case 10003                           :
    case 50003                           :
      start      ( c     )               ;
    break                                ;
    case 10004                           :
      ArgsToUuids ( 1 , V , U )          ;
      DoProcessEvents                    ;
      if ( U . count ( ) > 0 )           {
        View     ( U                   ) ;
        DoProcessEvents                  ;
        setFocus ( Qt::TabFocusReason  ) ;
      }                                  ;
    break                                ;
    case 10002                           :
    case 10011                           :
    case 10012                           :
    case 10013                           :
    case 10014                           :
    case 11001                           :
    case 13001                           :
    case 20002                           :
    case 20003                           :
    case 20004                           :
    case 20101                           :
    case 30001                           :
    case 30002                           :
    case 30003                           :
    case 50002                           :
      V . takeAt     ( 0               ) ;
      start          ( c , V           ) ;
    break                                ;
    case 40001                           :
      ArgsToGroup    ( 1 , V , group   ) ;
      GoIconsThreads ( 30003 , group   ) ;
    break                                ;
    case 12001                           :
      ArgsToUuids    ( 1 , V , U       ) ;
      DoProcessEvents                    ;
      setSelections  ( U               ) ;
    break                                ;
    default                              :
    return false                         ;
  }                                      ;
  return true                            ;
}

void N::MusicAlbums::FolderLanguage(void)
{
  if ( ! isFirst ( ) ) return            ;
  int OL = vLanguageId                   ;
  AbstractGui :: assignLanguage ( this ) ;
  if ( OL == vLanguageId ) return        ;
  EnterSQL ( SC , plan->sql )            ;
    GroupItems :: assignLanguage         (
      SC                                 ,
      first                              ,
      t1                                 ,
      vLanguageId                      ) ;
  ErrorSQL ( SC , plan->sql )            ;
  LeaveSQL ( SC , plan->sql )            ;
}

void N::MusicAlbums::ResumeLanguage(void)
{
  if ( isFirst() && ( LimitValues [ 81 ] > 0 ) ) {
    EnterSQL ( SC , plan->sql )                  ;
      int L                                      ;
      L = usingLanguage(SC,first,t1,vLanguageId) ;
      if (L>0) vLanguageId = L                   ;
      LimitValues [ 81 ] --                      ;
    ErrorSQL ( SC , plan->sql )                  ;
    LeaveSQL ( SC , plan->sql )                  ;
  }                                              ;
}

bool N::MusicAlbums::startup(void)
{
  if ( !isStopped ( ) ) return false ;
  clear               (       )      ;
  addSequence         ( 13001 )      ;
  emit TriggerCommand (       )      ;
  return true                        ;
}

bool N::MusicAlbums::canDelete(void)
{
  return ( isOwner ( ) ) ;
}

bool N::MusicAlbums::isOwner(void)
{
  if ( isFirst  ( ) ) return true ;
  if ( isSecond ( ) ) return true ;
  return false                    ;
}

void N::MusicAlbums::doTranslations(void)
{
  UUIDs   u = ItemUuids   (       ) ;
  QString t = windowTitle (       ) ;
  emit Translations       ( t , u ) ;
}

void N::MusicAlbums::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::MusicAlbums::Paste(QString text)
{
  QStringList S = text . split ( "\n" ) ;
  QString     T                         ;
  S = File :: PurifyLines ( S )         ;
  if ( S . count ( ) <= 0 ) return      ;
  VarArgs args                          ;
  args << S                             ;
  start ( 82001 , args )                ;
}

void N::MusicAlbums::Insert(void)
{
  NewListWidgetItem ( LWI                                ) ;
  //////////////////////////////////////////////////////////
  LWI -> setIcon ( LocalIcons [ "Music" ]                ) ;
  LWI -> setData ( Qt::UserRole , 0                      ) ;
  if ( ( (int)Qt::AscendingOrder ) == LimitValues [ 11 ] ) {
    QListWidget::addItem    (     LWI                    ) ;
  } else                                                   {
    QListWidget::insertItem ( 0 , LWI                    ) ;
  }                                                        ;
  scrollToItem        ( LWI                              ) ;
  setCurrentItem      ( LWI                              ) ;
  plan->processEvents (                                  ) ;
  //////////////////////////////////////////////////////////
  ItemEditing   = LWI                                      ;
  QRect       R = visualItemRect(LWI)                      ;
  LineEdit * l = new LineEdit(this,plan)                   ;
  QFont       f = font()                                   ;
  //////////////////////////////////////////////////////////
  QRect L (R.left(),R.bottom(),R.width(),f.pixelSize()+2)  ;
  setItemWidget(ItemEditing,l)                             ;
  l -> setGeometry ( L )                                   ;
  l -> setFont     ( f )                                   ;
  ItemWidget = l                                           ;
  //////////////////////////////////////////////////////////
  nConnect ( l    , SIGNAL ( editingFinished ( ) )         ,
             this , SLOT   ( editingFinished ( ) )       ) ;
  //////////////////////////////////////////////////////////
  l -> setFocus ( Qt::TabFocusReason )                     ;
}

void N::MusicAlbums::editingFinished(void)
{
  LineEdit * l = Casting(LineEdit,ItemWidget)                      ;
  nDropOut ( IsNull(l) )                                           ;
  QString name = l->text()                                         ;
  removeItemWidget(ItemEditing)                                    ;
  if (name.length()<=0)                                            {
    takeItem(row(ItemEditing))                                     ;
    ItemEditing = NULL                                             ;
    ItemWidget  = NULL                                             ;
    return                                                         ;
  }                                                                ;
  ItemEditing->setText(name)                                       ;
  //////////////////////////////////////////////////////////////////
  LockGui   ( )                                                    ;
  SqlConnection SC(plan->sql)                                      ;
  if ( SC . open ( FunctionString ) )                              {
    SUID u = appendAlbum ( SC , name )                             ;
    SC . close()                                                   ;
    ItemEditing -> setData ( Qt::UserRole , u )                    ;
    LimitValues [ 2 ] ++                                           ;
  } else                                                           {
    takeItem(row(ItemEditing))                                     ;
  }                                                                ;
  SC.remove ( )                                                    ;
  UnlockGui ( )                                                    ;
  //////////////////////////////////////////////////////////////////
  ItemEditing = NULL                                               ;
  ItemWidget  = NULL                                               ;
  //////////////////////////////////////////////////////////////////
  emit assignToolTip ( tr("%1 music albums") . arg ( count ( ) ) ) ;
  ALERT ( Done )                                                   ;
}

void N::MusicAlbums::Refresh(QListWidgetItem * item)
{
  LockGui           (               ) ;
  SqlConnection  SC ( plan->sql     ) ;
  if ( SC . open ( FunctionString ) ) {
    SUID uuid = nListUuid ( item    ) ;
    setUuid    ( SC , uuid , item   ) ;
    SC . close (                    ) ;
  }                                   ;
  SC . remove  (                    ) ;
  UnlockGui    (                    ) ;
}

void N::MusicAlbums::Remove(void)
{
  if ( !isOwner ( ) ) return            ;
  UUIDs Uuids                           ;
  for (int i=0;i<count();i++)           {
    QListWidgetItem * it = item ( i )   ;
    SUID uuid = ItemUuid ( it )         ;
    if (it->isSelected()) Uuids << uuid ;
  }                                     ;
  if (Uuids.count()<=0) return          ;
  LockGui          (            )       ;
  SqlConnection SC ( plan->sql  )       ;
  if ( SC . open ( FunctionString ) )   {
    DetachAlbums   ( SC , Uuids )       ;
    SC . close     (            )       ;
  }                                     ;
  SC . remove      (            )       ;
  UnlockGui        (            )       ;
  pendingStartup   (            )       ;
}

void N::MusicAlbums::singleClicked(QListWidgetItem * item)
{
  ListWidget :: singleClicked ( item )    ;
  ALERT ( Click )                         ;
  plan -> disableAction ( Menus::Delete ) ;
  if ( canDelete ( ) )                    {
    plan -> connectAction                 (
      Menus::Delete                       ,
      this                                ,
      SLOT ( Remove ( ) )                 ,
      true                              ) ;
  }                                       ;
  if ( item == ItemEditing ) return       ;
  RemoveOldItem ( )                       ;
}

void N::MusicAlbums::RemoveOldItem(void)
{
  nDropOut               ( IsNull ( ItemEditing ) ) ;
  nDropOut               ( IsNull ( ItemWidget  ) ) ;
  SUID u = ItemUuid      ( ItemEditing            ) ;
  removeItemWidget       ( ItemEditing            ) ;
  if ( u <= 0 ) takeItem ( row    ( ItemEditing ) ) ;
  ItemWidget  = NULL                                ;
  ItemEditing = NULL                                ;
}

bool N::MusicAlbums::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                              ;
  UUIDs             Uuids                                                    ;
  SUID              uuid     = 0                                             ;
  QString           name     = ""                                            ;
  QAction         * aa       = NULL                                          ;
  QListWidgetItem * it       = itemAt ( pos )                                ;
  QMenu           * md       = NULL                                          ;
  QMenu           * ms       = NULL                                          ;
  QMenu           * mp       = NULL                                          ;
  QMenu           * mx       = NULL                                          ;
  int               StartId  = 0                                             ;
  int               PageSize = 0                                             ;
  Uuids                      = Selected()                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( it ) )                                                      {
    uuid = nListUuid  ( it )                                                 ;
    name = it -> text (    )                                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( it ) )                                                      {
    aa  = mm . add     ( 101 , it -> text ( ) )                              ;
    aa -> setEnabled   ( false                )                              ;
    mm  . addSeparator (                      )                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( isFunction ( 3101 ) )                                                 {
    mp = PageMenu ( mm,tr("Total %1 music albums"),StartId,PageSize )        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mm . add ( 401 , LocalIcons [ "Plus" ] , tr("New music album") )           ;
  if ( isLoading ( ) )                                                       {
    mm . add ( 603 , tr("Stop loading icons" ) )                             ;
  } else                                                                     {
    if ( isStopped ( ) && isFunction ( 3101 ) )                              {
      mm . add ( 602 , LocalIcons [ "Update" ] , tr("Reload" ) )             ;
    }                                                                        ;
  }                                                                          ;
  mm   . addSeparator (                                                    ) ;
  mm   . addSeparator (                                                    ) ;
  if                  ( NotNull ( it )                                     ) {
    if ( canDelete ( ) ) mm . add ( 402 , tr("Remove this music album")    ) ;
    mm . addSeparator (                                                    ) ;
    mm . add          ( 201 , LocalIcons [ "Edit" ] , tr("Edit")           ) ;
    mm . add          ( 202 , tr("Edit names")                             ) ;
    mm . addSeparator (                                                    ) ;
    md = mm . addMenu ( tr("Details")                                      ) ;
    mm . addSeparator (                                                    ) ;
    mm . add          ( 301 , tr("Refresh music album")                    ) ;
    mm . addSeparator (                                                    ) ;
    //////////////////////////////////////////////////////////////////////////
    mm . add          ( md                                                   ,
                        701                                                  ,
                        LocalIcons [ "Details" ]                             ,
                        tr("Audio details")                                ) ;
    mm . add          ( md                                                   ,
                        702                                                  ,
                        LocalIcons [ "People" ]                              ,
                        tr("People")                                       ) ;
    mm . add          ( md                                                   ,
                        703                                                  ,
                        LocalIcons [ "Gallery" ]                             ,
                        tr("Gallery")                                      ) ;
    mm . add          ( md                                                   ,
                        704                                                  ,
                        LocalIcons [ "Documents" ]                           ,
                        tr("Documents")                                    ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ms   = mm . addMenu ( tr("Adjustments")                                  ) ;
  mx   = mm . addMenu ( tr("Selections")                                   ) ;
  mm   . add          ( ms                                                   ,
                        501                                                  ,
                        tr("Ascending Sort")                                 ,
                        true                                                 ,
                        Qt::AscendingOrder  == LimitValues [ 11 ]          ) ;
  mm   . add          ( ms                                                   ,
                        502                                                  ,
                        tr("Descending Sort")                                ,
                        true                                                 ,
                        Qt::DescendingOrder == LimitValues [ 11 ]          ) ;
  mm   . addSeparator ( ms                                                 ) ;
  mm   . add          ( ms , 604 , tr("Multilingual translations")         ) ;
  if                  ( LimitValues [ 2 ] > 0                              ) {
    if              ( ( PageSize >= LimitValues [ 2 ]                    )  ||
                      ( PageSize == 0                                    ) ) {
      mm . add        ( ms , 605 , tr("Set music album positions")         ) ;
    }                                                                        ;
  }                                                                          ;
  if                  ( isFirst ( )                                        ) {
    mm . addSeparator ( ms                                                 ) ;
    mm . add          ( ms , 606 , tr("Assign folder language")            ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AdjustMenu          ( mm , ms                                            ) ;
  SelectionsMenu      ( mm , mx                                            ) ;
  DockingMenu         ( mm                                                 ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm . setFont        ( plan                                               ) ;
  aa = mm . exec      (                                                    ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull ( aa ) )                                                       {
    if ( isFunction ( 3101 ) )                                               {
      PageChanged ( StartId , PageSize )                                     ;
    }                                                                        ;
    return true                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( RunDocking    ( mm , aa ) ) return true                               ;
  if ( RunAdjustment ( mm , aa ) ) return true                               ;
  if ( RunSelections ( mm , aa ) ) return true                               ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 201                                                                 :
      if ( uuid > 0 ) emit Command     ( uuid               )                ;
    return true                                                              ;
    case 202                                                                 :
      if ( uuid > 0 ) emit EditNames   ( this , uuid , name )                ;
    return true                                                              ;
    case 301                                                                 :
      if ( NotNull ( it ) ) Refresh ( it )                                   ;
    return true                                                              ;
    case 401                                                                 :
      Insert         (                                 )                     ;
    return true                                                              ;
    case 402                                                                 :
      Remove         (                                 )                     ;
    return true                                                              ;
    case 501                                                                 :
      LimitValues [ 11 ] = Qt::AscendingOrder                                ;
      sortItems      ( Qt::AscendingOrder              )                     ;
    return true                                                              ;
    case 502                                                                 :
      LimitValues [ 11 ] = Qt::DescendingOrder                               ;
      sortItems      ( Qt::DescendingOrder             )                     ;
    return true                                                              ;
    case 602                                                                 :
      startup        (                                 )                     ;
    return true                                                              ;
    case 603                                                                 :
      StopIcons      (                                 )                     ;
    return true                                                              ;
    case 604                                                                 :
      doTranslations (                                 )                     ;
    return true                                                              ;
    case 605                                                                 :
      start          ( 20003                           )                     ;
    return true                                                              ;
    case 606                                                                 :
      FolderLanguage (                                 )                     ;
    return true                                                              ;
    case 701                                                                 :
      emit Details   ( Types::MusicAlbum , uuid , name )                     ;
    return true                                                              ;
    case 702                                                                 :
      emit Details   ( Types::People     , uuid , name )                     ;
    return true                                                              ;
    case 703                                                                 :
      emit Details   ( Types::Picture    , uuid , name )                     ;
    return true                                                              ;
    case 704                                                                 :
      emit Details   ( Types::Document   , uuid , name )                     ;
    return true                                                              ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( isFunction ( 3101 ) )                                                 {
    RunPageMenu ( mm , aa , StartId , PageSize )                             ;
    PageChanged ( StartId , PageSize           )                             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}
