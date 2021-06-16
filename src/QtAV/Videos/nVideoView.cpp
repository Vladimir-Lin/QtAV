#include <qtav.h>

#define hasMenu isFunction(1001)
#define ALERT(ID) AnotherALERT(N::Command::ID)

N::VideoView:: VideoView  ( QWidget * parent , Plan * p                 )
             : SnapView   (           parent ,        p                 )
             , Group      ( 0 , Types::Division , Groups::Subordination )
             , dropAction ( false                                       )
             , Keywords   ( NULL                                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoView::~VideoView (void)
{
}

void N::VideoView::closeEvent(QCloseEvent * event)
{
  StopIcons                (       ) ;
  ListWidget :: closeEvent ( event ) ;
}

QMimeData * N::VideoView::dragMime(void)
{
  PickedUuids . clear ( )                         ;
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  QListWidgetItem * current = currentItem   ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("album/uuid",data)           ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("album/uuids",data)          ;
    PickedUuids = Uuids                           ;
  }                                               ;
  if (NotNull(current))                           {
    QIcon  icon  = current->icon()                ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr("%1 album selected"                )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

QString N::VideoView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType         (
           mime                        ,
           "album/uuid;album/uuids;"
           "video/uuid;video/uuids;"
           "picture/uuid;picture/uuids;"
           "people/uuid;people/uuids") ;
}

UUIDs N::VideoView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  nKickOut ( data.size() <= 0 , Uuids )  ;
  if (mimetype=="album/uuid")            {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="album/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="video/uuid")            {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="video/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="picture/uuid")          {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="picture/uuids")         {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="people/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="people/uuids")          {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::VideoView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::VideoView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction , false )    ;
  QString    mtype                   ;
  UUIDs      Uuids                   ;
  ALERT ( Click )                    ;
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

bool N::VideoView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction            , false )   ;
  nKickOut ( NotEqual(source,this) , true  )   ;
  QListWidgetItem * atItem = itemAt(pos)       ;
  nKickOut ( IsNull (atItem)       , false )   ;
  if (NotNull(atItem) && atItem->isSelected()) {
    return false                               ;
  }                                            ;
  return true                                  ;
}

bool N::VideoView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut         ( dropAction , false  ) ;
  return dropItems ( source , mime , pos ) ;
}

void N::VideoView::drawRating(QImage & image,int rate)
{
  nDropOut           ( rate <=0   )                    ;
  //////////////////////////////////////////////////////
  PictureManager PM  ( plan       )                    ;
  QImage          I                                    ;
  if (image.width()==128 && image.height()==128)       {
    I = image                                          ;
  } else                                               {
    if (image.width()>128 || image.height()>128)       {
      I = PM.Icon(image,QSize(128,128))                ;
    } else                                             {
      I = image                                        ;
    }                                                  ;
  }                                                    ;
  QImage W(QSize(128,128),QImage::Format_ARGB32)       ;
  memset(W.bits(),0,W.byteCount())                     ;
  int x = 128 - I . width  ()                          ;
  int y = 128 - I . height ()                          ;
  //////////////////////////////////////////////////////
  rate *= 127 ; rate /= 100 ; if (rate>127) rate = 127 ;
  QRect R(0,119,rate,8)                                ;
  QRect X(0,119,127 ,8)                                ;
  //////////////////////////////////////////////////////
  QLinearGradient LG ( QPointF(0,120),QPointF(0,128) ) ;
  LG . setColorAt    ( 0.00 , QColor(255,  0,128)    ) ;
  LG . setColorAt    ( 0.25 , QColor(255,219, 88)    ) ;
  LG . setColorAt    ( 1.00 , QColor(255,  0,128)    ) ;
  QBrush          LB ( LG                            ) ;
  //////////////////////////////////////////////////////
  QPainter p                                           ;
  p . begin      ( &W                  )               ;
  p . drawImage  ( x/2 , y/2 , I       )               ;
  p . setOpacity ( 0.50                )               ;
  p . setPen     ( QColor( 64,  0,255) )               ;
  p . drawRect   ( X                   )               ;
  p . setBrush   ( LB                  )               ;
  p . drawRect   ( R                   )               ;
  p . end        (                     )               ;
  //////////////////////////////////////////////////////
  image = W                                            ;
}

void N::VideoView::drawRating(QListWidgetItem * item,int rate)
{
  QImage * image  = new QImage      (                ) ;
  QIcon    icon   = item  -> icon   (                ) ;
  QPixmap  pixmap = icon   . pixmap ( QSize(128,128) ) ;
  //////////////////////////////////////////////////////
  *image  = pixmap . toImage        (                ) ;
  drawRating                        ( *image , rate  ) ;
  emit setIcon                      ( image  , item  ) ;
}

void N::VideoView::Configure(void)
{
  StackSize                    = 0                                 ;
  Reservation                  = false                             ;
  t2                           = Types::Album                      ;
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
  LocalIcons  [ "Stop"   ]     = QIcon(":/images/stoprecord.png" ) ;
  LocalIcons  [ "Edit"   ]     = QIcon(":/images/edit.png"       ) ;
  LocalIcons  [ "Play"   ]     = QIcon(":/images/PlayerPlay.png" ) ;
  LocalIcons  [ "Plus"   ]     = QIcon(":/images/plus.png"       ) ;
  LocalIcons  [ "Update" ]     = QIcon(":/images/update.png"     ) ;
  //////////////////////////////////////////////////////////////////
  dockingOrientation           = 0                                 ;
  dockingPlace                 = Qt::BottomDockWidgetArea          ;
  //////////////////////////////////////////////////////////////////
  setViewMode                  ( IconMode              )           ;
  setIconSize                  ( QSize(128,128)        )           ;
  setGridSize                  ( QSize(140,192)        )           ;
  setMovement                  ( Snap                  )           ;
  setResizeMode                ( Adjust                )           ;
  setSelectionMode             ( ExtendedSelection     )           ;
  setWordWrap                  ( true                  )           ;
  setMouseTracking             ( true                  )           ;
  setWrapping                  ( true                  )           ;
  setSortingEnabled            ( false                 )           ;
  setTextElideMode             ( Qt::ElideNone         )           ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )           ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )           ;
  setMinimumSize               ( QSize(144,192)        )           ;
  setDragDropMode              ( DragDrop              )           ;
  setAccessibleName            ( "N::VideoView"        )           ;
  setObjectName                ( "N::VideoView"        )           ;
  //////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropText     , true   )           ;
  setDropFlag                  ( DropUrls     , true   )           ;
  setDropFlag                  ( DropImage    , true   )           ;
  setDropFlag                  ( DropHtml     , true   )           ;
  setDropFlag                  ( DropColor    , false  )           ;
  setDropFlag                  ( DropTag      , true   )           ;
  setDropFlag                  ( DropPicture  , true   )           ;
  setDropFlag                  ( DropPeople   , true   )           ;
  setDropFlag                  ( DropVideo    , true   )           ;
  setDropFlag                  ( DropAlbum    , true   )           ;
  setDropFlag                  ( DropGender   , false  )           ;
  setDropFlag                  ( DropDivision , false  )           ;
  setDropFlag                  ( DropTorrent  , true   )           ;
  //////////////////////////////////////////////////////////////////
  setFunction                  ( 1001         , true   )           ;
  setFunction                  ( 1002         , true   )           ;
  setFunction                  ( 1003         , true   )           ;
  setFunction                  ( 1004         , true   )           ;
  //////////////////////////////////////////////////////////////////
  plan->setFont                ( this                  )           ;
}

void N::VideoView::FetchUUIDs(void)
{
  UUIDs         Uuids                                                ;
  int           startId =                 LimitValues [  0 ]         ;
  int           page    =                 LimitValues [  1 ]         ;
  int           total   =                 LimitValues [  2 ]         ;
  Qt::SortOrder order   = (Qt::SortOrder) LimitValues [ 11 ]         ;
  if (page<=0) page = total                                          ;
  UuidMaps . take ( 10001 )                                          ;
  EnterSQL ( SC , plan->sql )                                        ;
    if (isFirst())                                                   {
      Uuids = Subordination                                          (
                SC                                                   ,
                first                                                ,
                t1                                                   ,
                t2                                                   ,
                relation                                             ,
                SC . OrderBy   ( "position" , order                ) ,
                SC . sql.Limit ( startId    , page               ) ) ;
    } else
    if (isSecond())                                                  {
      Uuids = GetOwners                                              (
                SC                                                   ,
                second                                               ,
                t1                                                   ,
                t2                                                   ,
                relation                                             ,
                SC . OrderBy   ( "reversal" , order                ) ,
                SC . sql.Limit ( startId    , page               ) ) ;
    } else                                                           {
      Uuids = SC.Uuids                                               (
                PlanTable(Divisions)                                 ,
                "uuid"                                               ,
                QString ( "%1 %2 %3"                                 )
                . arg   ( "where `type` = %1" ) . arg ( Types::Album )
                . arg   ( SC.OrderBy   ( "id"    , order )           )
                . arg   ( SC.sql.Limit ( startId , page  )       ) ) ;
    }                                                                ;
  LeaveSQL ( SC , plan->sql )                                        ;
  UuidMaps [ 10001 ] = Uuids                                         ;
}

void N::VideoView::FetchTOTALs(void)
{
  int total = 0                                 ;
  EnterSQL ( SC , plan->sql )                   ;
    QString Q                                   ;
    if (isFirst())                              {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            PlanTable(Groups)                   ,
            FirstItem (first ,t1,t2,relation) ) ;
    } else
    if (isSecond())                             {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            PlanTable(Groups)                   ,
            SecondItem(second,t1,t2,relation) ) ;
    } else                                      {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            PlanTable(Divisions)                ,
            QString ( "where `type` = %1"       )
            . arg   ( Types::Album          ) ) ;
    }                                           ;
    if (SC.Fetch(Q))                            {
      total = SC.Int(0)                         ;
    }                                           ;
  LeaveSQL ( SC , plan->sql )                   ;
  LimitValues [ 2 ] = total                     ;
}

bool N::VideoView::AlbumDetails(SqlConnection & SC,QListWidgetItem * item)
{
  nKickOut           ( IsNull(item) , false )       ;
  SUID u = nListUuid (        item          )       ;
  nKickOut           ( ( u <= 0 )   , false )       ;
  ///////////////////////////////////////////////////
  int        total                                  ;
  QString    tooltip                                ;
  GroupItems GI ( plan )                            ;
  GI . AutoMap    = true                            ;
  GI . GroupTable = GI . LookTable                  (
    Types::Album                                    ,
    Types::Video                                    ,
    Groups::Subordination                         ) ;
  ///////////////////////////////////////////////////
  total   = GI . Count                              (
              SC                                    ,
              u                                     ,
              Types::Album                          ,
              Types::Video                          ,
              Groups::Subordination               ) ;
  tooltip = tr("%1 clips in this album").arg(total) ;
  item -> setToolTip ( tooltip )                    ;
  return true                                       ;
}

bool N::VideoView::ReportDetails(SqlConnection & Connection,QListWidgetItem * item,int flags)
{
  switch ( flags )                            {
    case 3                                    :
    return AlbumDetails ( Connection , item ) ;
  }                                           ;
  return false                                ;
}

void N::VideoView::IconsThread(Group group,int start,int end)
{
  if (start>=count()) return               ;
  if (start<=0      ) start = 0            ;
  if (end  >=count()) end   = count ( )    ;
  PictureManager PM ( plan        )        ;
  GroupItems     GI ( plan        )        ;
  SqlConnection  SC ( plan -> sql )        ;
  GI.GroupTable = PlanTable(IconGroups)    ;
  if (SC.open("VideoView","IconsThread"))  {
    QString L                              ;
    QString O                              ;
    QString Q                              ;
    L  = SC . sql.Limit  ( 0 , 1       )   ;
    O  = SC . OrderByAsc ( "position"  )   ;
    startLoading ( )                       ;
    while (isLoading() && start<end)       {
      QListWidgetItem * it                 ;
      SUID              u = 0              ;
      it = item        ( start )           ;
      if (NotNull(it))                     {
        u  = nListUuid ( it    )           ;
      }                                    ;
      start++                              ;
      if (u>0)                             {
        int      rate    = 0               ;
        SUID     puid                      ;
        QImage * image   = NULL            ;
        puid = GI . FindSecond             (
                 SC                        ,
                 u                         ,
                 group.t1                  ,
                 group.t2                  ,
                 group.relation            ,
                 O                         ,
                 L                     )   ;
        if (puid>0)                        {
          image = PM . Thumb ( SC , puid ) ;
        }                                  ;
        ////////////////////////////////////
        if ( ! isFunction(1003) )          {
          if (NotNull(image))              {
            emit setIcon ( image , it )    ;
          }                                ;
        } else                             {
          Q = SC.sql.SelectFrom            (
                "value"                    ,
                PlanTable(Rating)          ,
                QString("where `uuid` = %1 and `name` = 'Album'").arg(u) ) ;
          if (NotNull(image))              {
            if (SC.Fetch(Q))               {
              rate = SC.Int(0)             ;
              drawRating ( *image , rate ) ;
            }                              ;
            emit setIcon ( image , it )    ;
          } else                           {
            if (SC.Fetch(Q))               {
              rate = SC.Int(0)             ;
              drawRating ( it , rate )     ;
            }                              ;
          }                                ;
        }                                  ;
      }                                    ;
    }                                      ;
    stopLoading  ( )                       ;
    SC . close   ( )                       ;
  }                                        ;
  SC   . remove  ( )                       ;
}

bool N::VideoView::isOwner(void)
{
  if ( isFirst  ( ) ) return true ;
  if ( isSecond ( ) ) return true ;
  return false                    ;
}

void N::VideoView::StopIcons(void)
{
  stopForcely ( )                         ;
  while ( detection ( ) ) Time::skip(100) ;
  emit Leave  ( this )                    ;
}

void N::VideoView::FolderLanguage(void)
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

void N::VideoView::ResumeLanguage(void)
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

bool N::VideoView::startup(void)
{
  if ( !isStopped ( ) ) return false ;
  ResumeLanguage      (       )      ;
  clear               (       )      ;
  addSequence         ( 10002 )      ;
  emit TriggerCommand (       )      ;
  return true                        ;
}

void N::VideoView::View(UUIDs & Uuids)
{
  clear               (                 )  ;
  setEnabled          ( false           )  ;
  setCursor           ( Qt::WaitCursor  )  ;
  plan->processEvents (                 )  ;
  LockGui             (                 )  ;
  blockSignals        ( true            )  ;
  //////////////////////////////////////////
  int             PPL = LimitValues [ 76 ] ;
  int             PRC = LimitValues [ 77 ] ;
  LoopInt         LIP ( 0 , 0 , PPL )      ;
  QIcon           ICON                     ;
  ListWidgetItems ITEMs                    ;
  bool            GUI                      ;
  //////////////////////////////////////////
  ICON  = plan->Icon(Types::Album,1,0)     ;
  ITEMs = SnapView :: New ( Uuids )        ;
  GUI   = ( ITEMs.count() > PRC  )         ;
  for (int i=0;i<ITEMs.count();i++)        {
    ITEMs [ i ] -> setIcon ( ICON        ) ;
    ITEMs [ i ] -> setText ( " "         ) ;
    QListWidget :: addItem ( ITEMs [ i ] ) ;
    if ( GUI )                             {
      ++LIP                                ;
      if ( 0 == LIP.V )                    {
        plan -> processEvents ( )          ;
      }                                    ;
    }                                      ;
  }                                        ;
  //////////////////////////////////////////
  blockSignals         ( false           ) ;
  UnlockGui            (                 ) ;
  setCursor            ( Qt::ArrowCursor ) ;
  setEnabled           ( true            ) ;
  SnapView::Relocation (                 ) ;
  //////////////////////////////////////////
  QString TT                               ;
  QString MM                               ;
  LimitValues [ 75 ] ++                    ;
  TT = tr("Total %1 albums, displayed %2 start at %3") ;
  MM = QString ( TT                        )
      .arg     ( LimitValues[2]            )
      .arg     ( ITEMs.count( )            )
      .arg     ( LimitValues[0]          ) ;
  plan -> processEvents (                ) ;
  emit assignToolTip    ( MM             ) ;
  //////////////////////////////////////////
  Group   G                                ;
  VarArgs V                                ;
  //////////////////////////////////////////
  G.first      = 0                         ;
  G.second     = 0                         ;
  G.t1         = Types  :: Album           ;
  G.t2         = Types  :: Picture         ;
  G.relation   = Groups :: Icon            ;
  G.position   = 0                         ;
  G.membership = 1.0                       ;
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
  plan -> processEvents (                ) ;
}

bool N::VideoView::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Insert       , New             ( ) ) ;
  LinkAction        ( Search       , Search          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( GoHome       , PageHome        ( ) ) ;
  LinkAction        ( GoEnd        , PageEnd         ( ) ) ;
  LinkAction        ( PreviousPage , PageUp          ( ) ) ;
  LinkAction        ( NextPage     , PageDown        ( ) ) ;
  if ( isOwner ( ) )                                       {
    LinkAction      ( Delete       , Delete          ( ) ) ;
  }                                                        ;
  ALERT             ( Action                             ) ;
  return true                                              ;
}

void N::VideoView::Search(void)
{
  if (NotNull(Keywords)) Keywords->deleteLater()                ;
  Keywords   = (ComboBox *)plan->ComboBox(plan->status)         ;
  QFont font = plan->status->font()                             ;
  QSize s    = plan->status->size()                             ;
  QSize m(s.width()/6,s.height()-2)                             ;
  Keywords->setEditable(true)                                   ;
  plan->status->addPermanentWidget(Keywords)                    ;
  if (font.pixelSize()>0)                                       {
    font.setPixelSize(font.pixelSize())                         ;
    Keywords->setFont(font)                                     ;
    Keywords->lineEdit()->setFont(font)                         ;
  }                                                             ;
  Keywords -> setMinimumSize ( m )                              ;
  Keywords -> setMaximumSize ( m )                              ;
  QLineEdit * le = Keywords->lineEdit ()                        ;
  QSize ml(m.width()-30,m.height()-2)                           ;
  le     -> setMinimumSize ( ml )                               ;
  le     -> setMaximumSize ( ml )                               ;
  ///////////////////////////////////////////////////////////////
  ItemNames . clear ()                                          ;
  for (int i=0;i<count();i++)                                   {
    QListWidgetItem * it = item ( i )                           ;
    ItemNames << it->text ()                                    ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  ((QComboBox *)Keywords) -> addItems ( ItemNames )             ;
  le     -> setText ("")                                        ;
  connect(Keywords->lineEdit(),SIGNAL(returnPressed (       ))  ,
          this                ,SLOT  (SearchFinished(       ))) ;
  connect(Keywords->lineEdit(),SIGNAL(textChanged   (QString))  ,
          this                ,SLOT  (SearchChanged (QString))) ;
  Keywords->lineEdit()->setFocus(Qt::TabFocusReason)            ;
}

void N::VideoView::SearchFinished(void)
{
  QComboBox * comb = Casting(QComboBox,Keywords)             ;
  nDropOut ( IsNull(comb) )                                  ;
  QLineEdit * line = comb -> lineEdit ( )                    ;
  nDropOut ( IsNull(line) )                                  ;
  QString text = line -> text ( )                            ;
  QStringList K = text.split(' ')                            ;
  if (K.count()<=0)                                          {
    Keywords -> hide        ()                               ;
    Keywords -> deleteLater ()                               ;
    Keywords  = NULL                                         ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  QString Match = ""                                         ;
  for (int i=0;Match.length()<=0 && i<ItemNames.count();i++) {
    if (MatchString(ItemNames[i],K))                         {
      Match = ItemNames [ i ]                                ;
    }                                                        ;
  }                                                          ;
  if (Match.length()>0)                                      {
    QListWidgetItem * it = NULL                              ;
    for (int i=0;it==NULL && i<count();i++)                  {
      QListWidgetItem * xt = item ( i )                      ;
      if (Match==xt->text()) it = xt                         ;
    }                                                        ;
    if (NotNull(it))                                         {
      scrollToItem   ( it )                                  ;
      setCurrentItem ( it )                                  ;
      plan->processEvents()                                  ;
      QRect  vw = visualItemRect (it)                        ;
      QPoint vc = vw.center      (  )                        ;
      QCursor::setPos(mapToGlobal(vc))                       ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  Keywords -> hide        ()                                 ;
  Keywords -> deleteLater ()                                 ;
  Keywords  = NULL                                           ;
}

void N::VideoView::SearchChanged(const QString & text)
{
  QComboBox * comb = Casting(QComboBox,Keywords)  ;
  nDropOut ( IsNull(comb) )                       ;
  QStringList K = text.split(' ')                 ;
  if (K.count()<=0)                               {
    Keywords -> setToolTip ( "" )                 ;
    return                                        ;
  }                                               ;
  QStringList Matches                             ;
  for (int i=0;i<ItemNames.count();i++)           {
    if (MatchString(ItemNames[i],K))              {
      Matches << ItemNames[i]                     ;
    }                                             ;
  }                                               ;
  int total = Matches . count ()                  ;
  if (total>10) total = 10                        ;
  if (total<=0)                                   {
    Keywords -> setToolTip ( "" )                 ;
  } else                                          {
    QStringList Final                             ;
    for (int i=0;i<total;i++) Final << Matches[i] ;
    QString tooltip = Final.join("\n")            ;
    Keywords -> setToolTip ( tooltip )            ;
    QToolTip::showText(QCursor::pos(),tooltip)    ;
  }                                               ;
}

bool N::VideoView::MatchString(QString sentence,QStringList keywords)
{
  for (int i=0;i<keywords.count();i++)                {
    if (!sentence.contains(keywords[i])) return false ;
  }                                                   ;
  return true                                         ;
}

void N::VideoView::New(void)
{
  NewListWidgetItem ( LWI                                ) ;
  //////////////////////////////////////////////////////////
  LWI -> setIcon ( plan->Icon(Types::Album,1,0) )          ;
  LWI -> setData ( Qt::UserRole                       ,0 ) ;
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

SUID N::VideoView::appendAlbum(SqlConnection & SC,QString name)
{
  int     position = 0                                ;
  QString Q                                           ;
  SUID    uuid                                        ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid",51718) ;
  SC.assureUuid                                       (
    PlanTable(MajorUuid)                              ,
    uuid                                              ,
    Types::Album                                    ) ;
  SC.assureUuid                                       (
    PlanTable(Divisions)                              ,
    uuid                                              ,
    Types::Album                                    ) ;
  SC.assureName                                       (
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
        .arg(Types::Album                         ) ) ;
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

void N::VideoView::editingFinished(void)
{
  LineEdit * l = Casting(LineEdit,ItemWidget)           ;
  nDropOut ( IsNull(l) )                                ;
  QString name = l->text()                              ;
  removeItemWidget(ItemEditing)                         ;
  if (name.length()<=0)                                 {
    takeItem(row(ItemEditing))                          ;
    ItemEditing = NULL                                  ;
    ItemWidget  = NULL                                  ;
    return                                              ;
  }                                                     ;
  ItemEditing->setText(name)                            ;
  ///////////////////////////////////////////////////////
  LockGui   ( )                                         ;
  SqlConnection SC(plan->sql)                           ;
  if (SC.open("VideoView","editingFinished"))           {
    SUID uuid = appendAlbum ( SC , name )               ;
    SC . close()                                        ;
    ItemEditing->setData(Qt::UserRole,uuid)             ;
    LimitValues [ 2 ] ++                                ;
  } else                                                {
    takeItem(row(ItemEditing))                          ;
  }                                                     ;
  SC.remove ( )                                         ;
  UnlockGui ( )                                         ;
  ItemEditing = NULL                                    ;
  ItemWidget  = NULL                                    ;
  emit assignToolTip (tr("%1 albums").arg(count()))     ;
}

void N::VideoView::Rating(void)
{
  QListWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item) )              ;
  Rating   ( item         )              ;
}

void N::VideoView::Rating(QListWidgetItem * item)
{
  SUID uuid = nListUuid(item)                           ;
  int  rate = 0                                         ;
  nDropOut ( uuid <= 0      )                           ;
  ///////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                           ;
    QString Q                                           ;
    Q = SC.sql.SelectFrom                               (
          "value",PlanTable(Rating)                     ,
          QString("where uuid = %1 and name = 'Album'"  )
          .arg(uuid)                                  ) ;
    if (SC.Fetch(Q)) rate = SC.Int(0)                   ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ItemEditing   = item                                  ;
  QRect       R = visualItemRect ( item )               ;
  QSpinBox  * l = new QSpinBox   ( this )               ;
  QFont       f = font()                                ;
  ///////////////////////////////////////////////////////
  QRect L (R.left (),R.bottom   ()                      ,
           R.width(),f.pixelSize()+6                  ) ;
  setItemWidget(ItemEditing,l)                          ;
  l->setGeometry(L)                                     ;
  ItemWidget = l                                        ;
  ///////////////////////////////////////////////////////
  connect(l   ,SIGNAL(editingFinished())                ,
          this,SLOT  (rateFinished   ())              ) ;
  ///////////////////////////////////////////////////////
  l -> setRange     ( 0 , 100                         ) ;
  l -> setValue     ( rate                            ) ;
  l -> setAlignment ( Qt::AlignRight|Qt::AlignVCenter ) ;
  l -> setFocus     ( Qt::TabFocusReason              ) ;
}

void N::VideoView::rateFinished(void)
{
  QSpinBox * l = Casting(QSpinBox,ItemWidget) ;
  nDropOut ( IsNull(l) )                      ;
  int  rate    = l -> value (             )   ;
  SUID uuid    = nListUuid  ( ItemEditing )   ;
  nDropOut ( uuid <= 0 )                      ;
  /////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                 ;
    QString Q                                 ;
    Q = SC.sql.DeleteFrom                     (
          PlanTable(Rating)                   ,
          QString("where uuid = %1 "
                  "and name = 'Album'"        )
          .arg(uuid)                        ) ;
    SC . Query ( Q )                          ;
    Q  = SC.sql.InsertInto                    (
           PlanTable(Rating)                  ,
           3                                  ,
           "uuid"                             ,
           "name"                             ,
           "value"                   )        ;
    SC . Prepare ( Q                 )        ;
    SC  .Bind    ( "uuid"  , uuid    )        ;
    SC  .Bind    ( "name"  , "Album" )        ;
    SC . Bind    ( "value" , rate    )        ;
    SC . Exec    (                   )        ;
  LeaveSQL ( SC , plan->sql )                 ;
  /////////////////////////////////////////////
  drawRating       ( ItemEditing , rate )     ;
  removeItemWidget ( ItemEditing        )     ;
  ItemEditing = NULL                          ;
  ItemWidget  = NULL                          ;
}

void N::VideoView::Refresh(QListWidgetItem * item)
{
  if ( IsNull(item) ) return                                   ;
  PictureManager PM  ( plan           )                        ;
  EnterSQL           ( SC , plan->sql )                        ;
    QString  Q                                                 ;
    SUID     uuid    = nListUuid  ( item )                     ;
    QString  name    = SC . getName                            (
                         PlanTable(Names)                      ,
                         "uuid"                                ,
                         vLanguageId                           ,
                         uuid                                ) ;
    SUID     puid    = FindSecond                              (
                         SC                                    ,
                         uuid                                  ,
                         Types::Album                          ,
                         Types::Picture                        ,
                         Groups::Icon                          ,
                         "order by position asc limit 0,1"   ) ;
    int      total   = Count                                   (
                         SC                                    ,
                         uuid                                  ,
                         Types::Album                          ,
                         Types::Video                          ,
                         Groups::Subordination               ) ;
    QImage * image   = PM.Thumb (SC,puid)                      ;
    QString  tooltip = tr("%1 clips in this album").arg(total) ;
    int      rate    = 0                                       ;
    Q = SC.sql.SelectFrom                                      (
          "value"                                              ,
          PlanTable(Rating)                                    ,
          QString("where uuid = %1 and name = 'Album'"         )
          .arg(uuid)                                         ) ;
    if (NotNull(image))                                        {
      if (SC.Fetch(Q))                                         {
        rate = SC.Int(0)                                       ;
        drawRating ( *image , rate )                           ;
      }                                                        ;
      QSize is ( 128 , 128 )                                   ;
      QIcon icon = PM . Icon (image,is)                        ;
      delete image                                             ;
      item->setIcon(icon)                                      ;
    } else                                                     {
      QIcon icon = plan->Icon(Types::Album,1,0)                ;
      item->setIcon(icon)                                      ;
      if (SC.Fetch(Q))                                         {
        rate = SC.Int(0)                                       ;
        drawRating ( item , rate )                             ;
      }                                                        ;
    }                                                          ;
    item->setText(name)                                        ;
    emit assignToolTip ( tooltip )                             ;
  LeaveSQL(SC,plan->sql)                                       ;
}

void N::VideoView::Rename(QListWidgetItem * item)
{
  nDropOut ( IsNull(item) )                     ;
  setCurrentItem (  item  )                     ;
  ItemEditing =     item                        ;
  ///////////////////////////////////////////////
  QRect      R = visualItemRect ( item )        ;
  LineEdit * l = new LineEdit   ( this )        ;
  QFont      f = font           (      )        ;
  QRect L ( R.left () , R.bottom   ()           ,
            R.width() , f.pixelSize()+2       ) ;
  ///////////////////////////////////////////////
  setItemWidget(ItemEditing,l)                  ;
  l->setGeometry(L)                             ;
  l->setFont    (f)                             ;
  ItemWidget = l                                ;
  l->setText(item->text())                      ;
  ///////////////////////////////////////////////
  nConnect ( l    , SIGNAL(editingFinished())   ,
             this , SLOT  (renameFinished ()) ) ;
  ///////////////////////////////////////////////
  l -> setFocus ( Qt::TabFocusReason )          ;
}

void N::VideoView::renameFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget) ;
  nDropOut ( IsNull(l) )                        ;
  ///////////////////////////////////////////////
  QString name = l->text()                      ;
  SUID    uuid = nListUuid(ItemEditing)         ;
  removeItemWidget(ItemEditing)                 ;
  if (name.length()<=0)                         {
    ItemEditing = NULL                          ;
    ItemWidget  = NULL                          ;
    return                                      ;
  }                                             ;
  ItemEditing->setText(name)                    ;
  ///////////////////////////////////////////////
  nDropOut ( uuid <= 0 )                        ;
  ///////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                   ;
    SC.assureName                               (
      PlanTable(Names)                          ,
      uuid                                      ,
      vLanguageId                               ,
      name                                    ) ;
  LeaveSQL ( SC , plan->sql )                   ;
  ///////////////////////////////////////////////
  ItemEditing = NULL                            ;
  ItemWidget  = NULL                            ;
}

void N::VideoView::Remove(SUID uuid)
{
  nDropOut ( ! isOwner ( ) ) ;
  UUIDs    Uuids             ;
  Uuids << uuid              ;
  Remove(Uuids)              ;
}

bool N::VideoView::Delete(ThreadData * d)
{
  nKickOut     ( ! IsContinue ( d ) , false )       ;
  UUIDs U                                           ;
  SUID  u                                           ;
  FromVariants ( d -> Arguments     , U     )       ;
  nKickOut     ( U . count ( ) <= 0 , false )       ;
  nKickOut     ( ! isOwner ( )      , false )       ;
  ///////////////////////////////////////////////////
  QMap<SUID,QListWidgetItem *> itmap                ;
  foreach ( u , U )                                 {
    QListWidgetItem * it = uuidAt ( u )             ;
    if ( NULL != it ) itmap [ u ] = it              ;
  }                                                 ;
  ///////////////////////////////////////////////////
  emit assignEnabling ( false               )       ;
  EnterSQL            ( SC , plan -> sql    )       ;
    foreach ( u , U ) if ( itmap . contains ( u ) ) {
      emit takeListItem ( itmap [ u ] )             ;
      if ( isFirst  ( ) )                           {
        GroupItems::ExactDetach                     (
          SC                                        ,
          first                                     ,
          u                                         ,
          t1                                        ,
          t2                                        ,
          relation                                ) ;
      } else
      if ( isSecond ( ) )                           {
        GroupItems::ExactDetach                     (
          SC                                        ,
          u                                         ,
          second                                    ,
          t1                                        ,
          t2                                        ,
          relation                                ) ;
      }                                             ;
    }                                               ;
  LeaveSQL            ( SC , plan -> sql    )       ;
  ///////////////////////////////////////////////////
  emit assignEnabling ( true                )       ;
  ALERT               ( Done                )       ;
  return true                                       ;
}

void N::VideoView::Remove(UUIDs & Uuids)
{
  nDropOut ( ! isOwner ( ) )  ;
  SUID u                      ;
  foreach ( u , Uuids )       {
    QListWidgetItem * item    ;
    item = uuidAt ( u )       ;
    takeItem(row(item))       ;
  }                           ;
  /////////////////////////////
  EnterSQL ( SC , plan->sql ) ;
    if ( isFirst ( ) )        {
      GroupItems :: Detach    (
        SC                    ,
        first                 ,
        t1                    ,
        t2                    ,
        relation              ,
        Uuids               ) ;
    }                         ;
  LeaveSQL ( SC , plan->sql ) ;
  /////////////////////////////
  ALERT    ( Done           ) ;
}

void N::VideoView::Delete(void)
{
  UUIDs U = Selected ( )           ;
  if ( U . count ( ) <= 0 ) return ;
  VarArgs V                        ;
  toVariants ( U     , V )         ;
  start      ( 40001 , V )         ;
}

void N::VideoView::setPositions(void)
{
  nDropOut ( ! isOwner ( ) )                              ;
  if ( ( ! isFirst() ) && ( ! isSecond ( ) ) ) return     ;
  UUIDs Uuids = ItemUuids ( )                             ;
  nDropOut ( Uuids.count() <= 0 )                         ;
  EnterSQL ( SC , plan->sql )                             ;
    QString T                                             ;
    QString Q                                             ;
    SUID    u                                             ;
    int     position = 0                                  ;
    int     id       = 0                                  ;
    int     order    = LimitValues [ 11 ]                 ;
    T = LookTable ( t1 , t2 , relation )                  ;
    foreach ( u , Uuids )                                 {
      position++                                          ;
      switch ( order )                                    {
        case Qt::DescendingOrder                          :
          id = Uuids.count() - position + 1               ;
        break                                             ;
        case Qt::AscendingOrder                           :
        default                                           :
          id = position                                   ;
        break                                             ;
      }                                                   ;
      /////////////////////////////////////////////////////
      if ( isFirst  ( ) )                                 {
        Q = SC . sql . Update ( T )                       +
            QString("`position` = %1 ") . arg ( id )      +
            ExactItem ( first , u  , t1 , t2 , relation ) ;
      }                                                   ;
      if ( isSecond ( ) )                                 {
        Q = SC . sql . Update ( T )                       +
            QString("`reversal` = %1 ") . arg ( id )      +
            ExactItem ( u , second , t1 , t2 , relation ) ;
      }                                                   ;
      if ( Q . length ( ) > 0 ) SC . Query ( Q )          ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

bool N::VideoView::dropText(QWidget * source,QPointF psf,const QString & text)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::VideoView::dropUrls(QWidget * source,QPointF psf,const QList<QUrl> & urls)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::VideoView::dropImage(QWidget * source,QPointF psf,const QImage & image)
{
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  /////////////////////////////////////////
  PictureManager PM              ( plan ) ;
  SUID puid    = 0                        ;
  bool success = false                    ;
  QImage I = image                        ;
  success = PM . Import ( I , puid )      ;
  if (puid<=0 ) return true               ;
  dropAction = true                       ;
  UUIDs Uuids                             ;
  Uuids << puid                           ;
  /////////////////////////////////////////
  EnterSQL ( SC , plan->sql )             ;
    GroupItems :: Join                    (
      SC                                  ,
      album                               ,
      Types::Album                        ,
      Types::Picture                      ,
      Groups::Subordination               ,
      0                                   ,
      Uuids                             ) ;
  LeaveSQL ( SC , plan->sql )             ;
  /////////////////////////////////////////
  ALERT    ( Done           )             ;
  dropAction = false                      ;
  return true                             ;
}

bool N::VideoView::dropHtml(QWidget * source,QPointF psf,const QString & html)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::VideoView::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  /////////////////////////////////////////
  dropAction = true                       ;
  UUIDs Puids                             ;
  Puids << album                          ;
  /////////////////////////////////////////
  EnterSQL ( SC , plan->sql )             ;
    SUID uuid                             ;
    foreach (uuid,Uuids)                  {
      GroupItems::Join                    (
        SC                                ,
        uuid                              ,
        Types::Tag                        ,
        Types::Album                      ,
        Groups::Subordination             ,
        0                                 ,
        Puids                           ) ;
    }                                     ;
  LeaveSQL ( SC , plan->sql )             ;
  /////////////////////////////////////////
  ALERT    ( Done           )             ;
  dropAction = false                      ;
  return true                             ;
}

bool N::VideoView::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  dropAction = true                       ;
  /////////////////////////////////////////
  UUIDs      U = Uuids                    ;
  N::VarArgs V                            ;
  N::Group   G                            ;
  G  . first      = album                 ;
  G  . second     = 0                     ;
  G  . t1         = Types ::Album         ;
  G  . t2         = Types ::Picture       ;
  G  . relation   = Groups::Subordination ;
  G  . position   = 0                     ;
  G  . membership = 1.0                   ;
  V << 10011                              ;
  G  . addParaments ( V     )             ;
  V << U.count      (       )             ;
  toVariants        ( U , V )             ;
  addSequence       ( V     )             ;
  /////////////////////////////////////////
  dropAction = false                      ;
  emit TriggerCommand ( )                 ;
  return true                             ;
}

bool N::VideoView::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{ FunctionDebug ;
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  dropAction = true                       ;
  /////////////////////////////////////////
  UUIDs      U = Uuids                    ;
  N::VarArgs V                            ;
  N::Group   G                            ;
  G  . first      = album                 ;
  G  . second     = 0                     ;
  G  . t1         = Types ::Album         ;
  G  . t2         = Types ::People        ;
  G  . relation   = Groups::Subordination ;
  G  . position   = 0                     ;
  G  . membership = 1.0                   ;
  V << 10011                              ;
  G  . addParaments ( V     )             ;
  V << U.count      (       )             ;
  toVariants        ( U , V )             ;
  addSequence       ( V     )             ;
  /////////////////////////////////////////
  dropAction = false                      ;
  emit TriggerCommand ( )                 ;
  return true                             ;
}

bool N::VideoView::dropVideos(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  /////////////////////////////////////////
  dropAction = true                       ;
  UUIDs Vuids = Uuids                     ;
  /////////////////////////////////////////
  EnterSQL ( SC , plan->sql )             ;
    GroupItems::Join                      (
      SC                                  ,
      album                               ,
      Types::Album                        ,
      Types::Video                        ,
      Groups::Subordination               ,
      0                                   ,
      Vuids                             ) ;
  LeaveSQL ( SC , plan->sql )             ;
  /////////////////////////////////////////
  ALERT    ( Done           )             ;
  dropAction = false                      ;
  return true                             ;
}

bool N::VideoView::dropAlbums(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QListWidgetItem * atItem = itemAt(psf.toPoint()) ;
  if ( source == this )                            {
    if (IsNull(atItem)) return true                ;
    UUIDs   Puid = Uuids                           ;
    SUID    auid = nListUuid(atItem)               ;
    VarArgs V                                      ;
    dropAction = true                              ;
    V << 11001                                     ;
    V << auid                                      ;
    V << Puid.count()                              ;
    toVariants ( Puid , V )                        ;
    dropAction = false                             ;
    addSequence         ( V )                      ;
    emit TriggerCommand (   )                      ;
  } else                                           {
    if ( ! isOwner ( ) ) return true               ;
    SUID  auid = 0                                 ;
    UUIDs ITEMs = ItemUuids()                      ;
    UUIDs AUIDs                                    ;
    SUID  uuid                                     ;
    if (NotNull(atItem)) auid = nListUuid(atItem)  ;
    foreach (uuid,Uuids)                           {
      if (!ITEMs.contains(uuid)) AUIDs << uuid     ;
    }                                              ;
    if (AUIDs.count()<=0) return true              ;
    JoinAlbums ( auid , AUIDs )                    ;
  }                                                ;
  return true                                      ;
}

bool N::VideoView::dropTorrents(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( nEqual(source,this) , true ) ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  nKickOut ( IsNull(atItem     ) , true ) ;
  SUID album = nListUuid(atItem)          ;
  if (album<=0) return true               ;
  /////////////////////////////////////////
  nKickOut ( album <= 0          , true ) ;
  dropAction = true                       ;
  /////////////////////////////////////////
  UUIDs      U = Uuids                    ;
  N::VarArgs V                            ;
  N::Group   G                            ;
  G  . first      = album                 ;
  G  . second     = 0                     ;
  G  . t1         = Types::Album          ;
  G  . t2         = Types::File           ;
  G  . relation   = Groups::Subordination ;
  G  . position   = 0                     ;
  G  . membership = 1.0                   ;
  V << 10011                              ;
  G  . addParaments ( V     )             ;
  V << U.count      (       )             ;
  toVariants        ( U , V )             ;
  addSequence       ( V     )             ;
  /////////////////////////////////////////
  dropAction = false                      ;
  emit TriggerCommand ( )                 ;
  return true                             ;
}

void N::VideoView::JoinUuids(QString mime,QPoint pos,SUID before,UUIDs & Uuids)
{
}

void N::VideoView::MoveUuids(QString mime,QPoint pos,SUID before,UUIDs & Uuids)
{
}

void N::VideoView::MoveAlbums(SUID atUuid,const UUIDs & Uuids)
{
  SUID            uuid                                  ;
  UUIDs           ITEMs = ItemUuids ( )                 ;
  ListWidgetItems mItems                                ;
  foreach (uuid,Uuids)                                  {
    QListWidgetItem * it = uuidAt ( uuid )              ;
    mItems << it                                        ;
    takeItem ( row ( it ) )                             ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )           ;
  }                                                     ;
  QListWidgetItem * rit   = uuidAt ( atUuid )           ;
  int               rid   = row    ( rit    )           ;
  for (int i=0;i<mItems.count();i++,rid++)              {
    insertItem ( rid , mItems [ i ] )                   ;
  }                                                     ;
  rid = ITEMs . indexOf ( atUuid )                      ;
  if (rid<0) rid = 0                                    ;
  foreach (uuid,Uuids)                                  {
    ITEMs . insert ( rid , uuid )                       ;
    rid++                                               ;
  }                                                     ;
  RMAPs Position                                        ;
  if ( isOwner ( ) )                                    {
    int Id = 1                                          ;
    foreach (uuid,ITEMs)                                {
      if ( Qt::DescendingOrder == LimitValues [ 11 ]  ) {
        Position [ uuid ] = ITEMs.count() - Id + 1      ;
      } else                                            {
        Position [ uuid ] = Id                          ;
      }                                                 ;
      Id ++                                             ;
    }                                                   ;
  } else                                                {
    int Id = ITEMs . count()                            ;
    foreach (uuid,ITEMs)                                {
      if ( Qt::DescendingOrder == LimitValues [ 11 ]  ) {
        Position [ uuid ] = ITEMs.count() - Id + 1      ;
      } else                                            {
        Position [ uuid ] = Id                          ;
      }                                                 ;
      Id --                                             ;
    }                                                   ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                           ;
    QString Q                                           ;
    QString T = LookTable ( t1 , t2 , relation )        ;
    if ( isFirst ( ) )                                  {
      foreach (uuid,ITEMs)                              {
        int Id = Position [ uuid ]                      ;
        Q = SC . sql . Update ( T )                     +
            QString("`position` = %1 ").arg(Id)         +
            ExactItem(first,uuid,t1,t2,relation)        ;
        SC . Query ( Q )                                ;
      }                                                 ;
    } else
    if ( isSecond ( ) )                                 {
      foreach (uuid,ITEMs)                              {
        int Id = Position [ uuid ]                      ;
        Q = SC . sql . Update ( T )                     +
            QString("`reversal` = %1 ").arg(Id)         +
            ExactItem(first,uuid,t1,t2,relation)        ;
        SC . Query ( Q )                                ;
      }                                                 ;
    } else                                              {
      foreach (uuid,ITEMs)                              {
        int Id = Position [ uuid ]                      ;
        Q = SC.sql.Update                               (
              PlanTable(Divisions)                      ,
              SC.WhereUuid(uuid)                        ,
              1                                         ,
              "position"                              ) ;
        SC . Prepare ( Q               )                ;
        SC . Bind    ( "position" , Id )                ;
        SC . Exec    (                 )                ;
      }                                                 ;
    }                                                   ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ALERT    ( Done           )                           ;
}

void N::VideoView::JoinAlbums(SUID atUuid,UUIDs & Uuids)
{ Q_UNUSED ( atUuid        )                ;
  nDropOut ( ! isOwner ( ) )                ;
  dropAction = true                         ;
  ///////////////////////////////////////////
  UUIDs      U = Uuids                      ;
  N::VarArgs V                              ;
  N::Group   G                              ;
  if ( isFirst ( ) )                        {
    G  . first      = first                 ;
    G  . second     = 0                     ;
    G  . t1         = t1                    ;
    G  . t2         = t2                    ;
    G  . relation   = relation              ;
    G  . position   = 0                     ;
    G  . membership = 1.0                   ;
    V << 10012                              ;
    G  . addParaments ( V     )             ;
    V << U.count      (       )             ;
    toVariants        ( U , V )             ;
    addSequence       ( V     )             ;
    dropAction = false                      ;
    emit TriggerCommand ( )                 ;
  } else
  if ( isSecond ( ) )                       {
    G  . first      = 0                     ;
    G  . second     = second                ;
    G  . t1         = t1                    ;
    G  . t2         = t2                    ;
    G  . relation   = relation              ;
    G  . position   = 0                     ;
    G  . membership = 1.0                   ;
    V << 10014                              ;
    G  . addParaments ( V     )             ;
    V << U . count    (       )             ;
    toVariants        ( U , V )             ;
    addSequence       ( V     )             ;
    dropAction = false                      ;
    emit TriggerCommand ( )                 ;
  }
}

void N::VideoView::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::VideoView::Paste(QString text)
{
  QStringList S = text.split("\n") ;
  QString     T                    ;
  S = File :: PurifyLines ( S )    ;
  if (S.count()<=0) return         ;
  EnterSQL ( SC , plan->sql )      ;
    foreach ( T , S )              {
      addAlbum ( SC , T )          ;
    }                              ;
  LeaveSQL ( SC , plan->sql )      ;
  ALERT    ( Done           )      ;
}

void N::VideoView::addAlbum(SqlConnection & SC,QString name)
{
  SUID u = appendAlbum      ( SC  , name                   ) ;
  if (u<=0) return                                           ;
  LimitValues [ 2 ] ++                                       ;
  NewListWidgetItem         ( it                           ) ;
  it -> setData             ( Qt::UserRole , u             ) ;
  it -> setText             ( name                         ) ;
  it -> setIcon             ( plan->Icon(Types::Album,1,0) ) ;
  if ( Qt::DescendingOrder == LimitValues [ 11 ]           ) {
    QListWidget::insertItem ( 0 , it                       ) ;
  } else                                                     {
    QListWidget::addItem    ( it                           ) ;
  }                                                          ;
  plan->processEvents       (                              ) ;
}

void N::VideoView::RelatedItems(SUID uuid,QString title)
{
  UUIDs   ResultUuids                                    ;
  RMAPs   ResultTypes                                    ;
  NAMEs   ResultNames                                    ;
  int     ptype = Types::Album                           ;
  setEnabled ( false          )                          ;
  EnterSQL   ( SC , plan->sql )                          ;
    QString Q                                            ;
    SUID    puid                                         ;
    Q = SC.sql.SelectFrom                                (
          "name"                                         ,
          PlanTable(NameMaps)                            ,
          SC.WhereUuid(uuid)                           ) ;
    SqlLoopNow ( SC , Q )                                ;
      SUID wuid = SC.Uuid(0)                             ;
      if (!ResultUuids.contains(wuid))                   {
        ResultUuids << wuid                              ;
      }                                                  ;
    SqlLoopErr ( SC , Q )                                ;
    SqlLoopEnd ( SC , Q )                                ;
    Q = SC.sql.SelectFrom                                (
          "second"                                       ,
          PlanTable(Groups)                              ,
          QString("where `first` = %1 and `t1` = %2"     )
          .arg(uuid).arg(ptype)                        ) ;
    SqlLoopNow ( SC , Q )                                ;
      SUID vuid = SC.Uuid(0)                             ;
      if (!ResultUuids.contains(vuid))                   {
        ResultUuids << vuid                              ;
      }                                                  ;
    SqlLoopErr ( SC , Q )                                ;
    SqlLoopEnd ( SC , Q )                                ;
    Q = SC.sql.SelectFrom                                (
          "first"                                        ,
          PlanTable(Groups)                              ,
          QString("where `second` = %1 and `t2` = %2"    )
          .arg(uuid).arg(ptype)                        ) ;
    SqlLoopNow ( SC , Q )                                ;
      SUID kuid = SC.Uuid(0)                             ;
      if (!ResultUuids.contains(kuid))                   {
        ResultUuids << kuid                              ;
      }                                                  ;
    SqlLoopErr ( SC , Q )                                ;
    SqlLoopEnd ( SC , Q )                                ;
    foreach (puid,ResultUuids)                           {
      int t = 0                                          ;
      Q = SC.sql.SelectFrom                              (
            "type"                                       ,
            PlanTable(MajorUuid)                         ,
            SC.WhereUuid(puid)                         ) ;
      if (SC.Fetch(Q)) t = SC.Int(0)                     ;
      ResultTypes[puid] = t                              ;
      if (t==Types::Name)                                {
        Q = SC.sql.SelectFrom                            (
              "name"                                     ,
              PlanTable(Names)                           ,
              SC.WhereUuid(puid)                       ) ;
        if (SC.Fetch(Q))                                 {
          ResultNames[puid]=SC.String(0)                 ;
        }                                                ;
      } else                                             {
        Q = SC.sql.SelectFrom                            (
              "id"                                       ,
              PlanTable(Names)                           ,
              QString("where uuid = %1 and language = %2")
              .arg(puid).arg(vLanguageId)              ) ;
        if (!SC.Fetch(Q))                                {
          QStringList Ns                                 ;
          Ns = GroupItems::Names(SC,puid)                ;
          if (Ns.count()>0)                              {
            ResultNames[puid] = Ns[0]                    ;
          }                                              ;
        } else                                           {
          ResultNames[puid] = SC.getName                 (
            PlanTable(Names)                             ,
            "uuid"                                       ,
            vLanguageId                                  ,
            puid                                       ) ;
        }                                                ;
      }                                                  ;
    }                                                    ;
  LeaveSQL   ( SC , plan->sql )                          ;
  setEnabled ( true           )                          ;
  if (ResultUuids.count()<=0)                            {
    ALERT ( Error )                                      ;
  } else                                                 {
    ALERT ( Done  )                                      ;
    emit UuidItems                                       (
           ResultUuids                                   ,
           ResultTypes                                   ,
           ResultNames                                   ,
           title                                       ) ;
  }                                                      ;
}

void N::VideoView::doTranslations(void)
{
  UUIDs   Uuids = ItemUuids   ( )      ;
  QString title = windowTitle ( )      ;
  emit Translations ( title , Uuids )  ;
}

void N::VideoView::run(int Type,ThreadData * data)
{
  if ( ! IsContinue ( data ) ) return            ;
  VarArgs  V      = data -> Arguments            ;
  bool     loaded = false                        ;
  QString  TT                                    ;
  N::Group group                                 ;
  UUIDs    U                                     ;
  SUID     u                                     ;
  int      t                                     ;
  switch                    ( Type             ) {
    case 10002                                   :
      TT = tr("Counting albums...")              ;
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
      TT = tr("Loading albums...")               ;
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
      if ( ! loaded )                            {
        TT = tr("No album found")                ;
        emit assignToolTip ( TT )                ;
      }                                          ;
    break                                        ;
    case 10011                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        startLoading        (                  ) ;
        JoinGroupThread     ( group , U        ) ;
        stopLoading         (                  ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10012                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        startLoading        (                  ) ;
        JoinGroupThread     ( group , U        ) ;
        stopLoading         (                  ) ;
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
      if ( group.first > 0 )                     {
        startLoading        (                  ) ;
        JoinOwnerThread     ( group , U        ) ;
        stopLoading         (                  ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10014                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group . second > 0 )                  {
        startLoading        (                  ) ;
        JoinOwnerThread     ( group , U        ) ;
        stopLoading         (                  ) ;
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
      MoveAlbums            ( u , U            ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 12001            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 20002                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      NameThread            (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20003                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      setPositions          (                  ) ;
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
    case 30001                                   :
      ArgsToGroup           ( 0 , V , group    ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      SnapView::IconsThread ( group            ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 30002                                   :
      if (V.count()>0)                           {
        t = V [ 0 ] . toInt (                  ) ;
        emit OnBusy         (                  ) ;
        startLoading        (                  ) ;
        DetailsThread       ( t                ) ;
        stopLoading         (                  ) ;
        IsCompleted         (                  ) ;
        emit GoRelax        (                  ) ;
      }                                          ;
    break                                        ;
    case 30003                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      SnapView::IconsThread ( V                ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 40001                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      Delete                ( data             ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
  }                                              ;
}

bool N::VideoView::RunCommand(VarArgs & arguments)
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
      start      ( c     )               ;
    break                                ;
    case 10004                           :
      ArgsToUuids ( 1 , V , U )          ;
      plan -> processEvents ( )          ;
      if (U.count()>0)                   {
        View     ( U                   ) ;
        plan -> processEvents (        ) ;
        setFocus ( Qt::TabFocusReason  ) ;
      }                                  ;
    break                                ;
    case 10002                           :
    case 10011                           :
    case 10012                           :
    case 10013                           :
    case 10014                           :
    case 11001                           :
    case 20002                           :
    case 20003                           :
    case 20004                           :
    case 30001                           :
    case 30002                           :
    case 30003                           :
      V . takeAt ( 0     )               ;
      start      ( c , V )               ;
    break                                ;
    case 40001                           :
      ArgsToGroup    ( 1 , V , group  )  ;
      GoIconsThreads ( 30003 , group  )  ;
    break                                ;
    case 12001                           :
      ArgsToUuids ( 1 , V , U )          ;
      plan -> processEvents ( )          ;
      setSelections ( U )                ;
    break                                ;
    default                              :
    return false                         ;
  }                                      ;
  return true                            ;
}

void N::VideoView::DefaultMenu(void)
{
  nConnect ( this , SIGNAL ( ShowMenu  (QWidget*,QPoint) )   ,
             this , SLOT   ( VideoMenu (QWidget*,QPoint) ) ) ;
}

bool N::VideoView::Menu(QPoint pos)
{
  emit ShowMenu ( this , pos ) ;
  return true                  ;
}

bool N::VideoView::VideoMenu(QWidget *,QPoint pos)
{
  nScopedMenu ( mm , this )                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  QListWidgetItem * item     = itemAt(pos)                                   ;
  SUID              uuid     = 0                                             ;
  QString           name     = ""                                            ;
  QAction         * aa       = NULL                                          ;
  QMenu           * ma       = NULL                                          ;
  QMenu           * me       = NULL                                          ;
  QMenu           * mi       = NULL                                          ;
  QMenu           * mn       = NULL                                          ;
  QMenu           * mp       = NULL                                          ;
  QMenu           * ms       = NULL                                          ;
  QMenu           * mx       = NULL                                          ;
  int               StartId  = 0                                             ;
  int               PageSize = 0                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if (NotNull(item))                                                         {
    uuid = nListUuid   ( item )                                              ;
    name = item->text  (      )                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if (NotNull(item))                                                         {
    aa  = mm . add     ( 101 , item -> text ( ) )                            ;
    aa -> setEnabled   ( false                  )                            ;
    mm  . addSeparator (                        )                            ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mp = PageMenu ( mm , tr("Total %1 albums") , StartId , PageSize )          ;
  ////////////////////////////////////////////////////////////////////////////
  mm . add ( 101 , LocalIcons [ "Plus" ] , tr("New album")                 ) ;
  mm . add ( 103 , LocalIcons [ "Edit" ] , tr("New detailed album")        ) ;
  if ( isLoading ( ) )                                                       {
    mm . add ( 503 , LocalIcons [ "Stop"   ] , tr("Stop loading icons" )   ) ;
  } else                                                                     {
    mm . addSeparator()                                                      ;
    if (NotNull(item)) mm . add ( 501 , tr("Refresh")  )                     ;
    mm . add ( 502 , LocalIcons [ "Update" ] , tr("Reload" )  )              ;
  }                                                                          ;
  if ( NotNull ( item ) )                                                    {
    mm . addSeparator ( )                                                    ;
    mm . add ( 102 , LocalIcons [ "Play" ] , tr("Play")            )         ;
    mm . add ( 104 , LocalIcons [ "Note" ] , tr("Edit full names") )         ;
    mm . addSeparator ( )                                                    ;
    me   = mm.addMenu(tr("Edit"))                                            ;
    mm . add ( me,203,tr("%1 Details").arg(item->text()))                    ;
    mm . add ( me,201,tr("Edit %1"   ).arg(item->text()))                    ;
    mm . add ( me,202,tr("Rename %1" ).arg(item->text()))                    ;
    mm . add ( me,204,tr("Rating %1" ).arg(item->text()))                    ;
    mi   = mm.addMenu(tr("Information"))                                     ;
    mm . add ( mi,301,tr("Gallery"   )                  )                    ;
    mm . add ( mi,302,tr("Actors"    )                  )                    ;
    mm . add ( mi,303,tr("Clips"     )                  )                    ;
    mm . add ( mi,306,tr("Torrents"  )                  )                    ;
    mm . addSeparator(mi)                                                    ;
    mm . add ( mi,711,tr("All related items"))                               ;
    mn   = mm.addMenu(tr("Network")                 )                        ;
    mm . add ( mn,304,plan->Icon(Types::URL,1,0)                             ,
                  tr("Bookmarks" )                                         ) ;
    mm . add ( mn,305,tr("Search"    )                                     ) ;
    if ( isOwner ( ) )                                                       {
      UUIDs Uuids = Selected()                                               ;
      mm.addSeparator()                                                      ;
      mm.add(401,tr("Remove from %1").arg(windowTitle()))                    ;
      if (Uuids.count()>0)                                                   {
        mm.add(402,tr("Remove selections"))                                  ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( isStopped ( ) )                                                       {
    mm . addSeparator (                    )                                 ;
    ms = mm . addMenu ( tr("Arrangements") )                                 ;
    aa = mm . add     ( ms                                                   ,
                        601                                                  ,
                        tr("Enable sorting")                                 ,
                        true                                                 ,
                        isSortingEnabled ( )                               ) ;
    mm . add ( ms                                                            ,
               602                                                           ,
               tr("Ascend sorting")                                          ,
               true                                                          ,
               Qt::AscendingOrder  == LimitValues [ 11 ]                   ) ;
    mm . add ( ms                                                            ,
               603                                                           ,
               tr("Descend sorting")                                         ,
               true                                                          ,
               Qt::DescendingOrder == LimitValues [ 11 ]                   ) ;
    if ( isFirst ( ) || isSecond ( ) )                                       {
      mm . addSeparator ( ms )                                               ;
      mm . add (ms,604,tr("Set video album positions"))                      ;
    }                                                                        ;
    mm . addSeparator ( ms )                                                 ;
    mm . add ( ms , 504 , tr("Multilingual translations") )                  ;
  }                                                                          ;
  ma = mm . addMenu ( tr("Adjustments")    )                                 ;
  mx = mm . addMenu ( tr("Selections" )    )                                 ;
  if ( isStopped() && isFirst ( ) )                                          {
    mm . addSeparator ( ma )                                                 ;
    mm . add   ( ms , 606 , tr("Assign folder language"   ) )                ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AdjustMenu     ( mm , ma )                                                 ;
  SelectionsMenu ( mm , mx )                                                 ;
  DockingMenu    ( mm      )                                                 ;
  mm . setFont   ( plan    )                                                 ;
  aa = mm . exec (         )                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull ( aa ) )                                                       {
    PageChanged ( StartId , PageSize )                                       ;
    return true                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( RunDocking     ( mm , aa ) ) return true                              ;
  if ( RunAdjustment  ( mm , aa ) ) return true                              ;
  if ( RunSelections  ( mm , aa ) ) return true                              ;
  ////////////////////////////////////////////////////////////////////////////
  switch (mm[aa])                                                            {
    case 101                                                                 :
      New               (                     )                              ;
    return true                                                              ;
    case 102                                                                 :
      emit Play         ( uuid , name         )                              ;
    return true                                                              ;
    case 103                                                                 :
      emit NewAlbum     (                     )                              ;
    return true                                                              ;
    case 104                                                                 :
      emit FullNames    ( uuid                )                              ;
    return true                                                              ;
    case 201                                                                 :
      emit Edit         ( uuid , name         )                              ;
    return true                                                              ;
    case 202                                                                 :
      Rename            ( item                )                              ;
    return true                                                              ;
    case 203                                                                 :
      emit Details      ( uuid  ,name         )                              ;
    return true                                                              ;
    case 204                                                                 :
      Rating            ( item                )                              ;
    return true                                                              ;
    case 301                                                                 :
      emit Gallery      ( uuid , name         )                              ;
    return true                                                              ;
    case 302                                                                 :
      emit Actor        ( uuid , name         )                              ;
    return true                                                              ;
    case 303                                                                 :
      emit Clips        ( uuid , name         )                              ;
    return true                                                              ;
    case 304                                                                 :
      emit Bookmark     ( uuid , name         )                              ;
    return true                                                              ;
    case 305                                                                 :
      emit Search       ( uuid , name         )                              ;
    return true                                                              ;
    case 306                                                                 :
      emit Torrents     ( uuid , name         )                              ;
    return true                                                              ;
    case 401                                                                 :
      Remove            ( uuid                )                              ;
    return true                                                              ;
    case 402                                                                 :
      Delete            (                     )                              ;
    return true                                                              ;
    case 501                                                                 :
      Refresh           ( item                )                              ;
    return true                                                              ;
    case 502                                                                 :
      startup           (                     )                              ;
    return true                                                              ;
    case 503                                                                 :
      StopIcons         (                     )                              ;
    return true                                                              ;
    case 504                                                                 :
      doTranslations    (                     )                              ;
    return true                                                              ;
    case 601                                                                 :
      setSortingEnabled ( aa->isChecked()     )                              ;
    return true                                                              ;
    case 602                                                                 :
      sortItems         ( Qt::AscendingOrder  )                              ;
      LimitValues [ 11 ] = (int)Qt::AscendingOrder                           ;
    return true                                                              ;
    case 603                                                                 :
      sortItems         ( Qt::DescendingOrder )                              ;
      LimitValues [ 11 ] = (int)Qt::DescendingOrder                          ;
    return true                                                              ;
    case 604                                                                 :
      start             ( 20003               )                              ;
    return true                                                              ;
    case 606                                                                 :
      FolderLanguage    (                     )                              ;
    return true                                                              ;
    case 711                                                                 :
      if (uuid>0) RelatedItems(uuid,item->text())                            ;
    return true                                                              ;
  }                                                                          ;
  RunPageMenu ( mm , aa , StartId , PageSize )                               ;
  PageChanged ( StartId , PageSize           )                               ;
  return true                                                                ;
}
