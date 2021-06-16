#include <qtav.h>

N::AudioDepot:: AudioDepot ( QWidget * parent , Plan * p             )
              : TreeWidget (           parent ,        p             )
              , Ownership  ( 0 , Types::None , Groups::Subordination )
              , ItemEdit   ( NULL                                    )
              , ItemColumn ( -1                                      )
              , TextEdit   ( NULL                                    )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AudioDepot::~AudioDepot(void)
{
}

QStringList N::AudioDepot::defaultHeader(void)
{
  QStringList s         ;
  s << tr("Name"      ) ;
  s << tr("Identifier") ;
  s << tr("UUID"      ) ;
  s << tr("Format"    ) ;
  s << tr("File size" ) ;
  s << tr("Length"    ) ;
  s << tr("Operation" ) ;
  return s              ;
}

bool N::AudioDepot::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  return true                                          ;
}

void N::AudioDepot::Configure(void)
{
  QStringList headers                                         ;
  /////////////////////////////////////////////////////////////
  LimitValues [ 0 ] =  0                                      ;
  LimitValues [ 1 ] = 40                                      ;
  LimitValues [ 2 ] =  0                                      ;
  LastPath          = plan -> Temporary ( "" )                ;
  /////////////////////////////////////////////////////////////
  headers           = defaultHeader (                       ) ;
  /////////////////////////////////////////////////////////////
  setAcceptDrops                    ( true                  ) ;
  setDragDropMode                   ( DropOnly              ) ;
  setRootIsDecorated                ( false                 ) ;
  setAlternatingRowColors           ( true                  ) ;
  setHorizontalScrollBarPolicy      ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy        ( Qt::ScrollBarAsNeeded ) ;
  setColumnCount                    ( headers.count()       ) ;
  assignHeaderItems                 ( headers               ) ;
  setWindowTitle                    ( tr("Audio depot")     ) ;
  setAccessibleName                 ( "N::AudioDepot"       ) ;
  plan -> setFont                   ( this                  ) ;
  MountClicked                      ( 2                     ) ;
  /////////////////////////////////////////////////////////////
  setDropFlag                       ( DropUrls , true       ) ;
}

void N::AudioDepot::run(int T,ThreadData * d)
{
  switch ( T )                              {
    case 10001                              :
      startLoading ( )                      ;
      List         ( )                      ;
      stopLoading  ( )                      ;
    break                                   ;
    case 10002                              :
      if ( d -> Arguments . count ( ) > 0 ) {
        QStringList files                   ;
        files << d -> Arguments             ;
        AppendFiles ( files )               ;
        List        (       )               ;
      }                                     ;
    break                                   ;
  }                                         ;
}

bool N::AudioDepot::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::AudioDepot::List(void)
{
  plan -> Talk     ( windowTitle ( ) )                                       ;
  emit setEnabling ( false           )                                       ;
  GroupItems GI ( plan )                                                     ;
  EnterSQL(SC,plan->sql)                                                     ;
    QString Q                                                                ;
    UUIDs   Uuids                                                            ;
    int     total   = 0                                                      ;
    int     startId = LimitValues [ 0 ]                                      ;
    int     amount  = LimitValues [ 1 ]                                      ;
    if ( isStandby ( ) )                                                     {
      Q = SC . sql . SelectFrom                                              (
            "count(*)"                                                       ,
            PlanTable(AudioFiles)                                            ,
            ";"                                                            ) ;
      if ( SC . Fetch ( Q ) ) total = SC . Int ( 0 )                         ;
      LimitValues [ 2 ] = total                                              ;
      if ( amount <= 0 ) amount = total                                      ;
      plan -> Talk ( tr("Loading %1 audio records from depot").arg(total)  ) ;
      Uuids = SC . Uuids                                                     (
                PlanTable(AudioFiles)                                        ,
                "uuid"                                                       ,
                QString ( "%1 %2"                                            )
                . arg   ( SC . OrderByAsc  ( "id"               )            )
                . arg   ( SC . sql . Limit ( startId , amount ) )          ) ;
    } else                                                                   {
      GI . AutoMap    = true                                                 ;
      GI . GroupTable = GI . LookTable                                       (
                          ObjectType ( )                                     ,
                          Types::Audio                                       ,
                          Connexion ( )                                    ) ;
      total           = GI . Count                                           (
                          SC                                                 ,
                          ObjectUuid ( )                                     ,
                          ObjectType ( )                                     ,
                          Types::Audio                                       ,
                          Connexion  ( )                                   ) ;
      LimitValues [ 2 ] = total                                              ;
      if ( amount <= 0 ) amount = total                                      ;
      Uuids = GI . Subordination                                             (
                SC                                                           ,
                ObjectUuid ( )                                               ,
                ObjectType ( )                                               ,
                Types::Audio                                                 ,
                Connexion  ( )                                               ,
                QString ( "%1 %2"                                            )
                . arg   ( SC . OrderByAsc  ( "position" )                    )
                . arg   ( SC . sql . Limit ( startId , amount ) )          ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SUID  uuid                                                               ;
    foreach ( uuid , Uuids )                                                 {
      QString name = ""                                                      ;
      name = SC.getName                                                      (
                  plan->Tables[Tables::Names]                                ,
                  "uuid",plan->LanguageId,uuid                             ) ;
      Q = SC.sql.SelectFrom                                                  (
             "suffix,identifier,seconds,filesize"                            ,
             plan->Tables[Tables::AudioFiles]                                ,
             QString("where uuid = %1").arg(uuid)                          ) ;
      if (SC.Fetch(Q))                                                       {
        QString suffix     = SC.String(0)                                    ;
        QString identifier = SC.String(1)                                    ;
        int     seconds    = SC.Value (2).toInt()                            ;
        int     filesize   = SC.Value (3).toInt()                            ;
        QTime   T(0,0,0,0)                                                   ;
        NewTreeWidgetItem(IT)                                                ;
        T   = T . addMSecs     ( seconds                             )       ;
        IT -> setData          ( 0 , Qt::UserRole , uuid             )       ;
        IT -> setText          ( 0 , name                            )       ;
        IT -> setText          ( 1 , identifier                      )       ;
        IT -> setText          ( 2 , QString::number(uuid)           )       ;
        IT -> setTextAlignment ( 2 , Qt::AlignRight|Qt::AlignVCenter )       ;
        IT -> setText          ( 3 , suffix                          )       ;
        IT -> setText          ( 4 , QString::number(filesize)       )       ;
        IT -> setTextAlignment ( 4 , Qt::AlignRight|Qt::AlignVCenter )       ;
        IT -> setText          ( 5 , T.toString("hh:mm:ss.zzz")      )       ;
        IT -> setTextAlignment ( 5 , Qt::AlignRight|Qt::AlignVCenter )       ;
        addTopLevelItem(IT)                                                  ;
      }                                                                      ;
    }                                                                        ;
  LeaveSQL(SC,plan->sql)                                                     ;
  plan -> Talk ( tr("%1 audio records loaded") . arg(topLevelItemCount()) )  ;
  emit setEnabling ( true )                                                  ;
  emit AutoFit     (      )                                                  ;
  Alert            ( Done )                                                  ;
}

void N::AudioDepot::Insert(void)
{
  QStringList files                           ;
  files = QFileDialog::getOpenFileNames       (
                        this                  ,
                        tr("Add audio files") ,
                        LastPath            ) ;
  if (files.count()<=0) return                ;
  N::VarArgs args                             ;
  args << files                               ;
  start ( 10002 , args )                      ;
}

void N::AudioDepot::Verify(void)
{
  ScopedProgress P (plan,tr("Verify %v/%m"),10)                ;
  plan->Talk(tr("Verify all audio information"))               ;
  setEnabled (false          )                                 ;
  P . setRange ( 0 , topLevelItemCount() )                     ;
  P . show     (                         )                     ;
  P . Start    (                         )                     ;
  Sql sql = plan->sql                                          ;
  for (int i=0;i<topLevelItemCount();i++)                      {
    QTreeWidgetItem * it = topLevelItem(i)                     ;
    SUID uuid = RetrieveUuid(it,0)                             ;
    AudioData Music                                            ;
    ++P                                                        ;
    plan->showMessage(tr("Verify %1").arg(it->text(0)))        ;
    plan->processEvents()                                      ;
    if (Acoustics::Load(*plan,uuid,Music))                     {
      SqlConnection SC(sql)                                    ;
      if (SC.open("nAudioDepot","Verify"))                     {
        int ms = Music.Milliseconds()                          ;
        QString Q                                              ;
        Q = sql.Update                                         (
              plan->Tables[Tables::AudioFiles]                 ,
              SC.WhereUuid(uuid)                               ,
              1,"seconds"                             )        ;
        SC.Prepare(Q)                                          ;
        SC.Bind("seconds",ms)                                  ;
        SC.Exec()                                              ;
        SC.close()                                             ;
        QTime T(0,0,0,0)                                       ;
        T = T.addMSecs(ms)                                     ;
        it->setText(5,T.toString("hh:mm:ss.zzz"))              ;
        it->setText(6,QString::number(Music.AudioSize()))      ;
        scrollToItem(it)                                       ;
        DoProcessEvents                                        ;
      }                                                        ;
      SC.remove()                                              ;
    }                                                          ;
  }                                                            ;
  P . Finish ( )                                               ;
  plan->Talk(tr("%1 audio verified").arg(topLevelItemCount())) ;
  setEnabled ( true )                                          ;
}

void N::AudioDepot::Delete(void)
{
}

void N::AudioDepot::Play(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  SUID uuid = RetrieveUuid(item,0)         ;
  if (uuid<=0) return                      ;
  Play (uuid)                              ;
}

void N::AudioDepot::Play(SUID uuid)
{
  AudioData Music                                               ;
  if (!Acoustics::Load(*plan,uuid,Music)) return                ;
  QString T = nTimeNow . toString ( "yyyy/MM/ss-hh:mm:ss.zzz" ) ;
  plan -> Audio . Append ( T , Music )                          ;
  plan -> Audio . Play   ( T         )                          ;
}

bool N::AudioDepot::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                         ;
  ///////////////////////////////////////////////////////////////////
  QAction         * aa       = NULL                                 ;
  QMenu           * ms       = NULL                                 ;
  QMenu           * ma       = NULL                                 ;
  QMenu           * mp       = NULL                                 ;
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
    tms      = tr("Total %1 audio files").arg(LimitValues[2])       ;
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
  mm . add ( 101 , QIcon(":/images/PlayerPlay.png"),tr("Play")    ) ;
  mm . add ( 102 , tr("Insert"           )                        ) ;
  mm . add ( 103 , tr("Delete"           )                        ) ;
  mm . add ( 111 , tr("Refresh"          )                        ) ;
  mm . add ( 112 , tr("Verify all audios")                        ) ;
  ///////////////////////////////////////////////////////////////////
  mm . addSeparator (                                  )            ;
  ma = mm . addMenu ( tr("Adjustments")                )            ;
  mp = mm . addMenu ( tr("Selection"  )                )            ;
  ms = mm . addMenu ( tr("Sorting"    )                )            ;
  AdjustMenu        ( mm , ma                          )            ;
  SelectionsMenu    ( mm , mp                          )            ;
  SortingMenu       ( mm , ms                          )            ;
  ///////////////////////////////////////////////////////////////////
  mm . setFont ( plan )                                             ;
  aa = mm . exec ( )                                                ;
  if ( ! mm . contains ( aa ) )                                     {
    if ( ( StartId  != LimitValues[0] )                            ||
         ( PageSize != LimitValues[1] )                           ) {
      LimitValues [ 0 ] = StartId                                   ;
      LimitValues [ 1 ] = PageSize                                  ;
      startup ( )                                                   ;
    }                                                               ;
    return true                                                     ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if ( RunSorting    ( mm , aa ) ) return true                      ;
  if ( RunAdjustment ( mm , aa ) ) return true                      ;
  if ( RunSelections ( mm , aa ) ) return true                      ;
  ///////////////////////////////////////////////////////////////////
  int Id = mm [ aa ]                                                ;
  switch (Id)                                                       {
    case 101                                                        :
      Play    ( )                                                   ;
    return true                                                     ;
    case 102                                                        :
      Insert  ( )                                                   ;
    return true                                                     ;
    case 103                                                        :
      Delete  ( )                                                   ;
    return true                                                     ;
    case 111                                                        :
      startup ( )                                                   ;
    return true                                                     ;
    case 112                                                        :
      Verify  ( )                                                   ;
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
    default                                                         :
    return true                                                     ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  if ( ( StartId  != LimitValues[0] )                              ||
       ( PageSize != LimitValues[1] )                             ) {
    LimitValues [ 0 ] = StartId                                     ;
    LimitValues [ 1 ] = PageSize                                    ;
    startup ( )                                                     ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  return true                                                       ;
}

void N::AudioDepot::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column >= 2 )                      ;
  if (NotNull(TextEdit))                        {
    removeItemWidget(ItemEdit,ItemColumn)       ;
  }                                             ;
  ItemEdit   = item                             ;
  ItemColumn = column                           ;
  TextEdit   = new QLineEdit(this)              ;
  TextEdit->setText(ItemEdit->text(ItemColumn)) ;
  setItemWidget(ItemEdit,ItemColumn,TextEdit)   ;
  nMountSlot(TextEdit,this,editingFinished())   ;
  Alert ( Click )                               ;
}

void N::AudioDepot::editingFinished(void)
{
  nDropOut ( IsNull(TextEdit) )             ;
  SUID    uuid = nTreeUuid(ItemEdit,0)      ;
  QString name = TextEdit->text()           ;
  EnterSQL ( SC , plan->sql )               ;
    QString Q                               ;
    switch (ItemColumn)                     {
      case 0                                :
        if (name.length()>0)                {
          ItemEdit->setText(0,name)         ;
          SC.assureName(PlanTable(Names)    ,
                        uuid                ,
                        vLanguageId         ,
                        name              ) ;
        }                                   ;
      break                                 ;
      case 1                                :
        ItemEdit->setText(1,name)           ;
        Q = SC.sql.Update                   (
              PlanTable(AudioFiles)         ,
              "where uuid = :UUID"          ,
              1,"identifier"              ) ;
        SC.Prepare(Q)                       ;
        SC.Bind("uuid",uuid)                ;
        SC.Bind("identifier",name.toUtf8()) ;
        SC.Exec()                           ;
      break                                 ;
    }                                       ;
  LeaveSQL ( SC , plan->sql )               ;
  ///////////////////////////////////////////
  removeItemWidget(ItemEdit,ItemColumn)     ;
  ItemEdit   = NULL                         ;
  ItemColumn = -1                           ;
  TextEdit   = NULL                         ;
}

bool N::AudioDepot::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::AudioDepot::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::AudioDepot::dropUrls(QWidget * source,QPointF pos,const QList<QUrl> & urls)
{ Q_UNUSED ( pos                         )     ;
  nKickOut ( nEqual(source,this) , false )     ;
  for (int i=0;i<urls.count();i++)             {
    Append(urls[i].toLocalFile())              ;
  }                                            ;
  QTimer::singleShot(500,this,SLOT(startup())) ;
  return true                                  ;
}

void N::AudioDepot::Append(QString filename)
{
  AudioData Music                                        ;
  plan->showMessage (filename)                           ;
  plan->processEvents (        )                         ;
  nDropOut ( plan->AudioDepots.count() <= 0 )            ;
  if (!Acoustics::Load(filename,Music)) return           ;
  int seconds = Music.Milliseconds()                     ;
  nDropOut ( seconds <= 0 )                              ;
  ////////////////////////////////////////////////////////
  QByteArray Data                                        ;
  QFileInfo FI(filename)                                 ;
  QFile File(filename)                                   ;
  QString suffix = FI.suffix()                           ;
  nDropOut ( !File.open(QIODevice::ReadOnly) )           ;
  Data = File.readAll()                                  ;
  File.close()                                           ;
  nDropOut ( Data.size() <= 0 )                          ;
  ////////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                            ;
    QString Q                                            ;
    UUIDs   Uuids                                        ;
    bool    append = true                                ;
    //////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                                (
          "uuid"                                         ,
          PlanTable(AudioFiles)                          ,
          QString("where suffix = '%1' and filesize = %2")
            .arg(suffix).arg(Data.size()                 )
        )                                                ;
    SqlLoopNow(SC,Q)                                     ;
      Uuids << SC.Uuid(0)                                ;
    SqlLoopErr(SC,Q)                                     ;
    SqlLoopEnd(SC,Q)                                     ;
    //////////////////////////////////////////////////////
    if (Uuids.count()>0)                                 {
      for (int i=0;append && i<Uuids.count();i++)        {
        SUID uuid = Uuids[i]                             ;
        Q = SC.sql.SelectFrom                            (
          "suffix"                                       ,
          PlanTable(AudioFiles)                          ,
          SC.WhereUuid(uuid)                           ) ;
        if (SC.Fetch(Q))                                 {
          QString suff = SC.String(0)                    ;
          QString f                                      ;
          f = QString("%1.%2").arg(uuid).arg(suff)       ;
          f = plan->AudioFile(f)                         ;
          QFileInfo FF(f)                                ;
          if (FF.exists())                               {
            QByteArray V                                 ;
            if (File::toByteArray(f,V))        {
              if (V==Data) append = false                ;
            }                                            ;
          }                                              ;
        }                                                ;
      }                                                  ;
    }                                                    ;
    //////////////////////////////////////////////////////
    if (append)                                          {
      int checksum = qChecksum(Data.data(),Data.size())  ;
      QString name = FI.fileName()                       ;
      SUID uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
      SC.insertUuid(PlanTable(MajorUuid),uuid            ,
                    Types::Audio             ) ;
      SC.insertUuid(PlanTable(Audios),uuid,"uuid"      ) ;
      SC.assureName(PlanTable(Names)                     ,
                    uuid                                 ,
                    plan->LanguageId                     ,
                    name                               ) ;
      Q = SC.sql.InsertInto(PlanTable(AudioFiles),5      ,
            "uuid"                                       ,
            "suffix"                                     ,
            "seconds"                                    ,
            "checksum"                                   ,
            "filesize"                                 ) ;
      SC.Prepare(Q)                                      ;
      SC.Bind("uuid"    ,uuid           )                ;
      SC.Bind("suffix"  ,suffix.toUtf8())                ;
      SC.Bind("seconds" ,seconds        )                ;
      SC.Bind("checksum",checksum       )                ;
      SC.Bind("filesize",Data.size()    )                ;
      SC.Exec()                                          ;
      if (plan->AudioDepots.count()>0)                   {
        QDir D(plan->AudioDepots[0])                     ;
        QString FX                                       ;
        FX = QString("%1.%2").arg(uuid).arg(suffix)      ;
        FX = D.absoluteFilePath(FX)                      ;
        File::toFile(FX,Data)                            ;
      }                                                  ;
    }                                                    ;
  LeaveSQL ( SC , plan->sql )                            ;
}

void N::AudioDepot::AppendFiles(QStringList files)
{
  QString filename                    ;
  foreach ( filename , files )        {
    QFileInfo F ( filename )          ;
    if ( F . exists ( ) )             {
      Append ( filename )             ;
      LastPath = F . absolutePath ( ) ;
    }                                 ;
  }                                   ;
}
