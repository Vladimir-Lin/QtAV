#include <qtav.h>
#include <libavutil/error.h>

N::ClipList:: ClipList     ( QWidget * parent , Plan * p )
            : TreeWidget   (           parent ,        p )
            , VideoManager (                           p )
            , Adding       ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ClipList::~ClipList(void)
{
}

void N::ClipList::closeEvent(QCloseEvent * e)
{
  TreeWidget::closeEvent ( e ) ;
}

bool N::ClipList::hasItem(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  return NotNull ( item )                  ;
}

bool N::ClipList::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::ClipList::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

QMimeData * N::ClipList::dragMime(void)
{
  QMimeData * mime = standardMime("video")                   ;
  nKickOut ( IsNull(mime) , NULL )                           ;
  QImage image = windowIcon().pixmap(QSize(32,32)).toImage() ;
  mime -> setImageData(image)                                ;
  return mime                                                ;
}

void N::ClipList::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::ClipList::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::ClipList::acceptDrop(QWidget * source,const QMimeData * mime)
{
  if (source==this) return false ;
  return dropHandler(mime)       ;
}

bool N::ClipList::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::ClipList::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return true ;
}

bool N::ClipList::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

QString N::ClipList::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(mime,"video/uuid;video/uuids");
}

UUIDs N::ClipList::MimeUuids(const QMimeData * mime,QString mimetype)
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

bool N::ClipList::dropUrls(QWidget * source,QPointF pos,const QList<QUrl> & urls)
{ Q_UNUSED ( source )              ;
  Q_UNUSED ( pos    )              ;
  QStringList files                ;
  for (int i=0;i<urls.count();i++) {
    files << urls[i].toLocalFile() ;
  }                                ;
  Add ( files )                    ;
  return true                      ;
}

void N::ClipList::Configure(void)
{
  /////////////////////////////////////////////////////////////
  LimitValues [ 0 ] =  0                                      ;
  LimitValues [ 1 ] = 48                                      ;
  LimitValues [ 2 ] =  0                                      ;
  LocalIcons [ "Yes"    ] = QIcon(":/images/yes.png"        ) ;
  LocalIcons [ "DB"     ] = QIcon(":/images/odbc.png"       ) ;
  LocalIcons [ "Close"  ] = QIcon(":/images/close.png"      ) ;
  LocalIcons [ "Plus"   ] = QIcon(":/images/shopcartadd.png") ;
  LocalIcons [ "Join"   ] = QIcon(":/images/projectup.png"  ) ;
  LocalIcons [ "Update" ] = QIcon(":/images/update.png"     ) ;
  /////////////////////////////////////////////////////////////
  NewTreeWidgetItem       ( head                            ) ;
  head -> setText         ( 2 , tr("Length")                ) ;
  head -> setText         ( 3 , tr("Name"  )                ) ;
  setAccessibleName       ( "N::ClipList"                   ) ;
  setObjectName           ( "N::ClipList"                   ) ;
  setColumnCount          ( 4                               ) ;
  setRootIsDecorated      ( false                           ) ;
  setAlternatingRowColors ( true                            ) ;
  setDragDropMode         ( DragDrop                        ) ;
  setSelectionMode        ( ExtendedSelection               ) ;
  setAllAlignments        ( head , Qt::AlignCenter          ) ;
  setFont                 ( head , Fonts::ListView          ) ;
  head -> setIcon         ( 0 , LocalIcons [ "Yes"   ]      ) ;
  head -> setIcon         ( 1 , LocalIcons [ "DB"    ]      ) ;
  setHeaderItem           ( head                            ) ;
  setWindowTitle          ( tr("Edit video clips")          ) ;
  setWindowIcon           ( QIcon(":/images/videoclip.png") ) ;
  resizeColumnToContents  ( 0                               ) ;
  resizeColumnToContents  ( 1                               ) ;
  setAccessibleName       ( "N::ClipList"                   ) ;
  MountClicked            ( 1                               ) ;
  MountClicked            ( 2                               ) ;
  plan -> setFont         ( this                            ) ;
  /////////////////////////////////////////////////////////////
  setDropFlag             ( DropText    , false             ) ;
  setDropFlag             ( DropUrls    , true              ) ;
  setDropFlag             ( DropImage   , false             ) ;
  setDropFlag             ( DropHtml    , false             ) ;
  setDropFlag             ( DropColor   , false             ) ;
  setDropFlag             ( DropTag     , false             ) ;
  setDropFlag             ( DropPicture , false             ) ;
  setDropFlag             ( DropPeople  , false             ) ;
  setDropFlag             ( DropVideo   , false             ) ;
  setDropFlag             ( DropAlbum   , false             ) ;
  setDropFlag             ( DropGender  , false             ) ;
  /////////////////////////////////////////////////////////////
  setFunction             ( 18501       , true              ) ;
  setFunction             ( 18502       , true              ) ;
  setFunction             ( 18503       , true              ) ;
  setFunction             ( 18504       , true              ) ;
  setFunction             ( 18505       , true              ) ;
  /////////////////////////////////////////////////////////////
  setFunction             ( 19701       , false             ) ;
}

bool N::ClipList::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  return true                                          ;
}

void N::ClipList::run(int T,ThreadData * data)
{
  VarArgs V = data -> Arguments        ;
  UUIDs   U                            ;
  switch ( T )                         {
    case 10001                         :
      startLoading (   )               ;
      Reload       (   )               ;
      stopLoading  (   )               ;
    break                              ;
    case 10002                         :
      startLoading (   )               ;
      for (int i=0;i<V.count();i++)    {
        U << V [ i ] . toULongLong ( ) ;
      }                                ;
      List         ( U )               ;
      stopLoading  (   )               ;
    break                              ;
    case 10003                         :
      startLoading (   )               ;
      Appending    (   )               ;
      stopLoading  (   )               ;
    break                              ;
  }                                    ;
}

bool N::ClipList::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

bool N::ClipList::startup(UUIDs & U)
{
  VarArgs args           ;
  SUID    u              ;
  foreach ( u , U )      {
    args << u            ;
  }                      ;
  clear (              ) ;
  start ( 10002 , args ) ;
  return true            ;
}

void N::ClipList::LoadScripts(SqlConnection & SC)
{
  UUIDs Uuids                                      ;
  SUID  zuid                                       ;
  ScriptID . clear ( )                             ;
  Uuids = SC . Uuids                               (
    PlanTable(Scripts)                             ,
    "uuid"                                         ,
    QString("where `type` = %1 order by `id` desc" )
   .arg(Scripts::Mapper)                         ) ;
  foreach ( zuid , Uuids )                         {
    QString nnn = SC . getName                     (
      PlanTable(Names)                             ,
      "uuid"                                       ,
      plan -> LanguageId                           ,
      zuid                                       ) ;
    ScriptID [ zuid ] = nnn                        ;
  }                                                ;
}

void N::ClipList::LoadClip(SqlConnection & SC,SUID uuid)
{
  NewTreeWidgetItem(IT)                                ;
  QStringList names                                    ;
  QString     Q                                        ;
  IT -> setData    (0,Qt::UserRole,uuid )              ;
  IT -> setToolTip (1,tr("%1").arg(uuid))              ;
  names = VideoManager::Names ( SC , uuid )            ;
  if (names.count()>0) IT -> setText(3,names[0])       ;
  Q = SC . sql . SelectFrom                            (
        SC . Columns                                   (
          5                                            ,
          "suffix"                                     ,
          "filesize"                                   ,
          "filename"                                   ,
          "duration"                                   ,
          "actual"                                   ) ,
        PlanTable(Videos)                              ,
        SC . WhereUuid ( uuid )                      ) ;
  if (SC.Fetch(Q))                                     {
    QTime T (0,0,0,0)                                  ;
    qint64  filesize = SC.Tuid  (1)                    ;
    QString filename = SC.String(2)                    ;
    TUID    actual   = SC.Tuid  (4)                    ;
    QString filepath = ""                              ;
    T = T . addMSecs ( actual / 1000 )                 ;
    IT->setText         ( 2 , T.toString("hh:mm:ss") ) ;
    IT->setTextAlignment( 2 , Qt::AlignRight         ) ;
    filepath = Find                                    (
                 plan->VideoDepots                     ,
                 filename                              ,
                 filesize                            ) ;
    if (filepath.length()>0)                           {
      IT -> setIcon ( 0 , LocalIcons [ "Yes" ]       ) ;
      IT -> setIcon ( 1 , LocalIcons [ "DB"  ]       ) ;
      IT -> setData ( 1 , Qt::UserRole, 1            ) ;
      addTopLevelItem (IT)                             ;
    }                                                  ;
  }                                                    ;
}

void N::ClipList::Empty(void)
{
  emit assignEnabling ( false          ) ;
  emit clearItems     (                ) ;
  EnterSQL            ( SC , plan->sql ) ;
    LoadScripts       ( SC             ) ;
  LeaveSQL            ( SC , plan->sql ) ;
  emit assignEnabling ( true           ) ;
  Alert               ( Done           ) ;
}

void N::ClipList::Reload(void)
{
  emit assignEnabling ( false          )                          ;
  emit OnBusy         (                )                          ;
  EnterSQL            ( SC , plan->sql )                          ;
    LoadScripts       ( SC             )                          ;
    UUIDs Uuids                                                   ;
    Uuids = SC.Uuids                                              (
      PlanTable(Videos)                                           ,
      "uuid"                                                      ,
      SC.OrderByDesc("id")                                      ) ;
    FixUuids ( Uuids )                                            ;
    ///////////////////////////////////////////////////////////////
    if ( Uuids . count ( ) > 0 )                                  {
      int    pid                                                  ;
      qint64 value = 0                                            ;
      bool   go    = true                                         ;
      QDateTime T = nTimeNow                                      ;
      pid = plan->Progress(tr("Loading clips"),tr("%v/%m clips")) ;
      plan -> Start    ( pid , &value , &go           )           ;
      plan -> setRange ( pid , 0      , Uuids.count() )           ;
      for (int i=0;i<Uuids.count();i++)                           {
        LoadClip ( SC , Uuids[i] )                                ;
        value ++                                                  ;
      }                                                           ;
      if ( T.msecsTo(nTimeNow) < 1500 ) Time::skip(1500)          ;
      plan -> Finish ( pid )                                      ;
    }                                                             ;
  LeaveSQL            ( SC , plan->sql )                          ;
  emit GoRelax        (                )                          ;
  emit assignEnabling ( true           )                          ;
  /////////////////////////////////////////////////////////////////
  int     total = topLevelItemCount ( )                           ;
  QString m                                                       ;
  m = tr("Total %1 clips").arg(total)                             ;
  plan -> Talk ( m )                                              ;
  /////////////////////////////////////////////////////////////////
  emit AutoFit (      )                                           ;
  Alert        ( Done )                                           ;
}

void N::ClipList::List(UUIDs & Uuids)
{
  emit assignEnabling ( false          )                          ;
  emit OnBusy         (                )                          ;
  EnterSQL            ( SC , plan->sql )                          ;
    LoadScripts       ( SC             )                          ;
    if ( Uuids.count() > 0 )                                      {
      int    pid                                                  ;
      qint64 value = 0                                            ;
      bool   go    = true                                         ;
      QDateTime T = nTimeNow                                      ;
      pid = plan->Progress(tr("Loading clips"),tr("%v/%m clips")) ;
      plan -> Start    ( pid , &value , &go           )           ;
      plan -> setRange ( pid , 0      , Uuids.count() )           ;
      for (int i=0;i<Uuids.count();i++)                           {
        LoadClip ( SC , Uuids[i] )                                ;
        value++                                                   ;
      }                                                           ;
      if ( T.msecsTo(nTimeNow) < 1500 ) Time::skip(1500)          ;
      plan -> Finish ( pid )                                      ;
    }                                                             ;
  LeaveSQL            ( SC , plan->sql )                          ;
  emit GoRelax        (                )                          ;
  emit assignEnabling ( true           )                          ;
  /////////////////////////////////////////////////////////////////
  int     total = topLevelItemCount ( )                           ;
  QString m                                                       ;
  m = tr("Total %1 clips").arg(total)                             ;
  plan -> Talk ( m )                                              ;
  /////////////////////////////////////////////////////////////////
  emit AutoFit (      )                                           ;
  Alert        ( Done )                                           ;
}

void N::ClipList::Add(QStringList & files)
{
  for (int i=0;i<files.count();i++) WaitingList << files[i] ;
  if ( Adding ) return                                      ;
  QTimer::singleShot ( 300 , this , SLOT( Pending() )     ) ;
}

void N::ClipList::Pending(void)
{
  if ( WaitingList . count () <= 0 )                    {
    Alert ( Done )                                      ;
    return                                              ;
  }                                                     ;
  QFileInfo info ( WaitingList [0] )                    ;
  Add ( info )                                          ;
  WaitingList . takeAt ( 0 )                            ;
  QTimer::singleShot ( 300 , this , SLOT( Pending() ) ) ;
}

void N::ClipList::Add(QFileInfo & info)
{
  LockGui ( )                                           ;
  Adding = true                                         ;
  if (info.exists())                                    {
    QString filename = info . absoluteFilePath ()       ;
    TUID duration = Duration ( filename )               ;
    NewTreeWidgetItem (IT)                              ;
    IT->setData( 0 , Qt::UserRole , 0                 ) ;
    IT->setText( 3 , filename                         ) ;
    IT->setData( 3 , Qt::UserRole , filename.toUtf8() ) ;
    IT->setTextAlignment( 2 , Qt::AlignRight          ) ;
    if (duration>0)                                     {
      QTime T(0,0,0,0)                                  ;
      T = T . addMSecs ( duration / 1000 )              ;
      IT->setIcon( 0 , LocalIcons [ "Yes"   ]         ) ;
      IT->setData( 1 , Qt::UserRole , 0               ) ;
      IT->setText( 2 , T.toString("hh:mm:ss")         ) ;
    } else                                              {
      IT->setIcon( 0 , LocalIcons [ "Close" ]         ) ;
      IT->setData( 1 , Qt::UserRole , -1              ) ;
      IT->setText( 2 , "--:--:--"                     ) ;
    }                                                   ;
    insertTopLevelItem    ( 0 , IT )                    ;
    DoProcessEvents                                     ;
  }                                                     ;
  Adding = false                                        ;
  UnlockGui ( )                                         ;
}

void N::ClipList::singleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut      ( ItemEditing==item && column==ItemColumn ) ;
  removeOldItem (                                         ) ;
  Alert         ( Click                                   ) ;
}

void N::ClipList::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column != 3 )             ;
  SUID uuid = nTreeUuid(item,0)        ;
  nDropOut ( uuid   > 0  )             ;
  int status = nTreeInt(item,1)        ;
  nDropOut ( status < 0  )             ;
  QLineEdit * l                        ;
  l = new QLineEdit(this)              ;
  l-> setText(item->text(3))           ;
  ItemEditing = item                   ;
  ItemColumn  = column                 ;
  ItemWidget  = l                      ;
  setItemWidget(item,column,l)         ;
  nMountSlot(l,this,editingFinished()) ;
}

void N::ClipList::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::ClipList::editingFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget) ;
  ItemEditing->setText ( 3 , l->text() )        ;
  removeOldItem        (               )        ;
  Alert                ( Selected      )        ;
}

void N::ClipList::Insert(void)
{
  if ( isFunction ( 19701 ) ) return ;
  start ( 10003 )                    ;
}

void N::ClipList::Appending(void)
{
  setFunction ( 19701 , true )      ;
  while ( isLoading ( ) )           {
    if ( ! AppendNext ( ) )         {
      setFunction ( 19701 , false ) ;
      return                        ;
    }                               ;
  }                                 ;
  setFunction ( 19701 , false )     ;
}

bool N::ClipList::AppendNext(void)
{
  int total = topLevelItemCount()                                        ;
  int index = total - 1                                                  ;
  if ( index < 0 ) return false                                          ;
  ////////////////////////////////////////////////////////////////////////
  QTreeWidgetItem * item = NULL                                          ;
  while (IsNull(item) && index>=0)                                       {
    QTreeWidgetItem * it = topLevelItem(index)                           ;
    SUID uuid = nTreeUuid(it,0)                                          ;
    int  flag = it->data(1,Qt::UserRole).toInt()                         ;
    if (uuid==0 && flag==0) item = it                                    ;
    index--                                                              ;
  }                                                                      ;
  if ( IsNull ( item ) ) return false                                    ;
  ////////////////////////////////////////////////////////////////////////
  QString name     = item->text(3)                                       ;
  QString filename                                                       ;
  filename = QString::fromUtf8(item->data(3,Qt::UserRole).toByteArray()) ;
  plan->showMessage(QString("%1 %2").arg(name).arg(filename))            ;
  LockGui   ( )                                                          ;
  if (Integrity(name,filename)) AddClip(name,filename,item)              ;
  UnlockGui ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////
  return true                                                            ;
}

bool N::ClipList::Integrity(QString name,QString filename)
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

bool N::ClipList::AddClip(QString name,QString filename,QTreeWidgetItem * item)
{
  if ( filename . length ( ) <= 0 ) return false                            ;
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
  duration     = AVX.Duration ()                                            ;
  ActualLength = duration                                                   ;
  Bits         = AVX.AvBits   ()                                            ;
  ///////////////////////////////////////////////////////////////////////////
  QDir    DDD ( Depot )                                                     ;
  bool    success = false                                                   ;
  QString TFile                                                             ;
  ///////////////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                          ;
  if ( SC . open ( FunctionString ) )                                       {
    QString Q                                                               ;
    SUID nuid                                                               ;
    SUID uuid = SC.Unique(plan->Tables[Tables::MajorUuid],"uuid")           ;
    TFile = QString("%1.%2").arg(uuid).arg(FI.suffix())                     ;
    int MimeId = -1                                                         ;
    Q = SC.sql.SelectFrom                                                   (
          "mimeid",plan->Tables[Tables::Extensions]                         ,
          "where name = :NAME"                                            ) ;
    SC . Prepare ( Q )                                                      ;
    SC . Bind    ( "name" , FI.suffix().toLower().toUtf8()                ) ;
    if (SC.Exec() && SC.Next()) MimeId = SC.Int(0)                          ;
    if (MimeId>0)                                                           {
      SC.assureUuid                                                         (
        plan->Tables[Tables::MajorUuid]                                     ,
        uuid,Types::Video                                                 ) ;
      Q = SC.sql.InsertInto                                                 (
            plan->Tables[Tables::Videos]                                    ,
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
      if (SC.Exec())                                                        {
        nuid = SC.Unique(plan->Tables[Tables::MajorUuid],"uuid" )           ;
        SC.assureUuid                                                       (
          plan->Tables[Tables::MajorUuid]                                   ,
          nuid,Types::Name                                                ) ;
        Q = SC.sql.InsertInto                                               (
              plan->Tables[Tables::Names]                                   ,
              3,"uuid","language","name"                                  ) ;
        SC.insertName(Q,nuid,plan->LanguageId,name)                         ;
        Q = SC.sql.InsertInto                                               (
              plan->Tables[Tables::NameMaps]                                ,
              3,"uuid","name","prefer"                                    ) ;
        SC.Prepare ( Q               )                                      ;
        SC.Bind    ( "uuid"   , uuid )                                      ;
        SC.Bind    ( "name"   , nuid )                                      ;
        SC.Bind    ( "prefer" , 1    )                                      ;
        SC.Exec    (                 )                                      ;
        success = true                                                      ;
      }                                                                     ;
      if (success)                                                          {
        item -> setData ( 0 , Qt::UserRole , uuid )                         ;
      }                                                                     ;
    }                                                                       ;
    SC.close()                                                              ;
  }                                                                         ;
  SC.remove()                                                               ;
  ///////////////////////////////////////////////////////////////////////////
  AVX.Close()                                                               ;
  Alert ( Done )                                                            ;
  plan -> Talk          ( tr("Video clip added into database")            ) ;
  DoProcessEvents                                                           ;
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
    item -> setIcon ( 1 , LocalIcons [ "DB"    ]                          ) ;
    item -> setData ( 1 , Qt::UserRole,1                                  ) ;
    Alert ( Done  )                                                         ;
  } else                                                                    {
    item -> setIcon ( 1 , LocalIcons [ "Close" ]                          ) ;
    item -> setData ( 1 , Qt::UserRole,-1                                 ) ;
    Alert ( Error )                                                         ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  return true                                                               ;
}

QString N::ClipList::ScriptContent(SqlConnection & SC,SUID u)
{
  QString script = ""          ;
  QString Q                    ;
  Q = SC . sql . SelectFrom    (
        "script"               ,
        PlanTable(Scripts)     ,
        SC . WhereUuid ( u ) ) ;
  if (SC.Fetch(Q))             {
    script = SC . String ( 0 ) ;
  }                            ;
  return script                ;
}

void N::ClipList::AddFiles(void)
{
  QStringList files                                        ;
  QString     path = plan->Temporary("")                   ;
  if ( plan -> Variables . contains ( "ClipPath" ) )       {
    path = plan -> Variables [ "ClipPath" ] . toString ( ) ;
  }                                                        ;
  files = QFileDialog::getOpenFileNames                    (
                        this                               ,
                        tr("Add video files")              ,
                        path                             ) ;
  if ( files . count ( ) > 0 )                             {
    QFileInfo FXI ( files [ 0 ] )                          ;
    path  = FXI . absoluteDir ( ) . absolutePath ( )       ;
    plan -> Variables [ "ClipPath" ] = path                ;
    Add ( files )                                          ;
  }                                                        ;
}

bool N::ClipList::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                         ;
  QAction          * aa = NULL                                      ;
  QTreeWidgetItem  * item = itemAt(pos)                             ;
  SUID               uuid = 0                                       ;
  SUID               zuid                                           ;
  CMAPs              MIDs                                           ;
  QMenu            * ms                                             ;
  QMenu            * ma                                             ;
  QMenu            * mp                                             ;
  QStringList        Depots = plan->VideoDepots                     ;
  QString            path   = plan->Temporary("")                   ;
  UUIDs              Zuids = ScriptID.keys()                        ;
  if (NotNull(item)) uuid  = nTreeUuid(item,0)                      ;
  if (Depots.count()>0) path = Depots[0]                            ;
  ///////////////////////////////////////////////////////////////////
  QMenu           * mpu      = NULL                                 ;
  SpinBox         * spb      = NULL                                 ;
  SpinBox         * spp      = NULL                                 ;
  int               StartId  = 0                                    ;
  int               PageSize = 0                                    ;
  ///////////////////////////////////////////////////////////////////
  if (LimitValues[2]>0)                                             {
    QString tms                                                     ;
    StartId  = LimitValues[0]                                       ;
    PageSize = LimitValues[1]                                       ;
    tms      = tr("Total %1 organizations").arg(LimitValues[2])     ;
    spb      = new SpinBox  ( NULL , plan                     )     ;
    spp      = new SpinBox  ( NULL , plan                     )     ;
    spb     -> setRange     ( 0    , LimitValues[2]           )     ;
    spp     -> setRange     ( 0    , LimitValues[2]           )     ;
    spb     -> setValue     ( StartId                         )     ;
    spp     -> setValue     ( PageSize                        )     ;
    spb     -> External = &StartId                                  ;
    spp     -> External = &PageSize                                 ;
    mpu      = mm . addMenu ( tr("Page")                      )     ;
    aa       = mm . add     ( mpu, 551 , tms                  )     ;
    aa      -> setEnabled   ( false                           )     ;
    mm       . addSeparator ( mpu                             )     ;
    mm       . add          ( mpu , 561 , tr("Home"         ) )     ;
    mm       . add          ( mpu , 562 , tr("Previous page") )     ;
    mm       . add          ( mpu , 563 , tr("Next page"    ) )     ;
    mm       . add          ( mpu , 564 , tr("End page"     ) )     ;
    mm       . addSeparator ( mpu                             )     ;
    mm       . add          ( mpu , 571 , spb                 )     ;
    mm       . add          ( mpu , 572 , spp                 )     ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if (uuid>0)                                                       {
    QMenu * ms                                                      ;
    mm.add(101,tr("Edit clip names" ))                              ;
    ms = mm.addMenu(tr("Conversion scripts"))                       ;
    mm.addSeparator()                                               ;
    mm.add(102,tr("Analyze clip"    ))                              ;
    mm.add(103,tr("Clip information"))                              ;
    mm.addSeparator()                                               ;
    int index = 100001                                              ;
    foreach (zuid,Zuids)                                            {
      mm.add(ms,index,ScriptID[zuid])                               ;
      MIDs[index] = zuid                                            ;
      index++                                                       ;
    }                                                               ;
    mm.addSeparator()                                               ;
  }                                                                 ;
  mm . add          ( 901 , LocalIcons [ "Join"   ]                 ,
                            tr("Insert pending clips") )            ;
  mm . add          ( 902 , LocalIcons [ "Update" ]                 ,
                            tr("Refresh"             ) )            ;
  mm . addSeparator (                                  )            ;
  mm . add          ( 911 , LocalIcons [ "Plus"   ]                 ,
                            tr("Add video files"     ) )            ;
  mm . addSeparator (                                  )            ;
  ma = mm . addMenu ( tr("Adjustments")                )            ;
  mp = mm . addMenu ( tr("Selection"  )                )            ;
  ms = mm . addMenu ( tr("Sorting"    )                )            ;
  AdjustMenu        ( mm , ma                          )            ;
  SelectionsMenu    ( mm , mp                          )            ;
  SortingMenu       ( mm , ms                          )            ;
  mm . setFont      ( plan                             )            ;
  aa = mm . exec    (                                  )            ;
  if (IsNull(aa))                                                   {
    if ( ( LimitValues [ 2 ]  > 0       )                          &&
         ( ( StartId  != LimitValues[0] )                          ||
           ( PageSize != LimitValues[1] )                       ) ) {
      LimitValues [ 0 ] = StartId                                   ;
      LimitValues [ 1 ] = PageSize                                  ;
      startup ( )                                                   ;
    }                                                               ;
    return false                                                    ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if ( RunSorting    ( mm , aa ) ) return true                      ;
  if ( RunAdjustment ( mm , aa ) ) return true                      ;
  if ( RunSelections ( mm , aa ) ) return true                      ;
  ///////////////////////////////////////////////////////////////////
  switch (mm[aa])                                                   {
    case 101                                                        :
      emit ClipNames       ( uuid , item->text(3) )                 ;
    return true                                                     ;
    case 102                                                        :
      emit AnalyzeClip     ( uuid , item->text(3) )                 ;
    return true                                                     ;
    case 103                                                        :
      emit ClipInformation ( uuid , item->text(3) )                 ;
    return true                                                     ;
    case 561                                                        :
      StartId = 0                                                   ;
    break                                                           ;
    case 562                                                        :
      StartId -= PageSize                                           ;
      if (StartId<0) StartId = 0                                    ;
    break                                                           ;
    case 563                                                        :
      StartId += PageSize                                           ;
      if (StartId>=LimitValues[2]) StartId = LimitValues[2] - 1     ;
    break                                                           ;
    case 564                                                        :
      StartId  = LimitValues [ 2 ]                                  ;
      StartId -= PageSize                                           ;
      if (StartId<0) StartId = 0                                    ;
    break                                                           ;
    case 901                                                        :
      Insert  ( )                                                   ;
    return true                                                     ;
    case 902                                                        :
      startup ( )                                                   ;
    return true                                                     ;
    case 911                                                        :
      AddFiles ( )                                                  ;
    return true                                                     ;
    default                                                         :
      zuid = MIDs[mm[aa]]                                           ;
      if (zuid>0 && uuid>0)                                         {
        QString script = ""                                         ;
        EnterSQL(SC,plan->sql)                                      ;
          QString Q                                                 ;
          script = ScriptContent(SC,zuid)                           ;
          if (script.length()>0)                                    {
            QString source = item->text(3)                          ;
            QString result                                          ;
            result = Scripts::StringMapper(script,source) ;
            if (result.length()>0)                                  {
              SUID nuid = 0                                         ;
              Q = SC.sql.SelectFrom                                 (
                  "name"                                            ,
                  plan->Tables[Tables::NameMaps]                    ,
                  QString("where uuid = %1 order by prefer asc limit 0,1").arg(uuid)) ;
              if (SC.Fetch(Q)) nuid = SC.Uuid(0)                    ;
              if (nuid>0)                                           {
                SC.assureName                                       (
                  plan->Tables[Tables::Names]                       ,
                  nuid,plan->LanguageId,result                    ) ;
                item->setText(3,result)                             ;
              }                                                     ;
            }                                                       ;
          }                                                         ;
        LeaveSQL(SC,plan->sql)                                      ;
      }                                                             ;
    return true                                                     ;
  }                                                                 ;
    if ( ( LimitValues [ 2 ]  > 0       )                          &&
         ( ( StartId  != LimitValues[0] )                          ||
           ( PageSize != LimitValues[1] )                       ) ) {
      LimitValues [ 0 ] = StartId                                   ;
      LimitValues [ 1 ] = PageSize                                  ;
      startup ( )                                                   ;
    }                                                               ;
  return true                                                       ;
}
