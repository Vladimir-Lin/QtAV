#include <qtav.h>

#pragma message("Require a major fix up to newer version")

N::AudioPanel:: AudioPanel         ( QWidget * parent , Plan * p )
              : StackedWidget      (           parent ,        p )
              , AvDecoder          (                             )
              , CiosAudio::Conduit (                             )
              , core               ( NULL                        )
              , stream             ( NULL                        )
              , vertical           ( NULL                        )
              , horizontal         ( NULL                        )
              , label              ( NULL                        )
              , wrapper            ( NULL                        )
              , stack              ( NULL                        )
              , panel              ( NULL                        )
              , display            ( NULL                        )
              , list               ( NULL                        )
              , progress           ( NULL                        )
              , volume             ( NULL                        )
              , buttons            ( NULL                        )
              , files              ( NULL                        )
              , database           ( NULL                        )
              , depot              ( NULL                        )
              , totalOutput        ( 0                           )
              , duration           ( -1                          )
              , currentPTS         ( -1                          )
              , MaxFreq            ( 6000                        )
              , Freq               ( 6000                        )
              , FreqFactor         ( 0.5                         )
              , Range              ( 0 , 1000                    )
              , Thickness          ( 0.08                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AudioPanel::~AudioPanel (void)
{
  shutdown (      )       ;
  nIfSafe  ( core )       {
    core -> Terminate ( ) ;
    delete core           ;
    core   = NULL         ;
  }                       ;
}

void N::AudioPanel::closeEvent (QCloseEvent * event)
{
  shutdown             (       ) ;
  QWidget:: closeEvent ( event ) ;
}

QSize N::AudioPanel::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 384 , 200 ) ) ;
}

QWidget * N::AudioPanel::widget(WidgetType type)
{
  switch ( type )   {
    case Label      :
    return label    ;
    case Display    :
    return display  ;
    case List       :
    return list     ;
    case Progress   :
    return progress ;
    case Volume     :
    return volume   ;
    case Buttons    :
    return buttons  ;
  }                 ;
  return NULL       ;
}

int N::AudioPanel::Frequency(void) const
{
  return Freq ;
}

int N::AudioPanel::setFrequency(int freq)
{
  Freq = freq ;
  return Freq ;
}

double N::AudioPanel::Factor(void) const
{
  return FreqFactor ;
}

double N::AudioPanel::setFactor(double factor)
{
  FreqFactor = factor ;
  return FreqFactor   ;
}

void N::AudioPanel::Configure(void)
{
  setWindowTitle                  ( tr("Audio panel")           )         ;
  /////////////////////////////////////////////////////////////////////////
  core = new CiosAudio :: Core    (                             )         ;
  core -> Initialize              (                             )         ;
  /////////////////////////////////////////////////////////////////////////
  vertical    = new QSplitter     ( Qt::Vertical   , this       )         ;
  label       = new QLabel        ( vertical                    )         ;
  stack       = new StackedWidget ( vertical       , plan       )         ;
  display     = new PaintWidget   ( stack          , plan       )         ;
  list        = new TreeWidget    ( stack          , plan       )         ;
  depot       = new AudioLists    ( stack          , plan       )         ;
  progress    = new QSlider       ( Qt::Horizontal , vertical   )         ;
  horizontal  = new QSplitter     ( Qt::Horizontal , vertical   )         ;
  panel       = new StackedWidget ( horizontal     , plan       )         ;
  volume      = new QSlider       ( Qt::Horizontal , horizontal )         ;
  buttons     = new Widget        ( panel          , plan       )         ;
  files       = new Widget        ( panel          , plan       )         ;
  database    = new Widget        ( panel          , plan       )         ;
  /////////////////////////////////////////////////////////////////////////
  addWidget                       ( vertical                    )         ;
  /////////////////////////////////////////////////////////////////////////
  vertical   -> addWidget         ( label                       )         ;
  vertical   -> addWidget         ( stack                       )         ;
  vertical   -> addWidget         ( progress                    )         ;
  vertical   -> addWidget         ( horizontal                  )         ;
  /////////////////////////////////////////////////////////////////////////
  stack      -> addWidget         ( display                     )         ;
  stack      -> addWidget         ( list                        )         ;
  stack      -> addWidget         ( depot                       )         ;
  /////////////////////////////////////////////////////////////////////////
  horizontal -> addWidget         ( panel                       )         ;
  horizontal -> addWidget         ( volume                      )         ;
  /////////////////////////////////////////////////////////////////////////
  panel      -> addWidget         ( buttons                     )         ;
  panel      -> addWidget         ( files                       )         ;
  panel      -> addWidget         ( database                    )         ;
  /////////////////////////////////////////////////////////////////////////
  vertical   -> setHandleWidth    ( 0                           )         ;
  horizontal -> setHandleWidth    ( 0                           )         ;
  /////////////////////////////////////////////////////////////////////////
  label      -> setMinimumHeight  ( 24                          )         ;
  label      -> setMaximumHeight  ( 24                          )         ;
  progress   -> setMinimumHeight  ( 20                          )         ;
  progress   -> setMaximumHeight  ( 20                          )         ;
  horizontal -> setMinimumHeight  ( 28                          )         ;
  horizontal -> setMaximumHeight  ( 28                          )         ;
  panel      -> setMinimumHeight  ( 28                          )         ;
  panel      -> setMaximumHeight  ( 28                          )         ;
  panel      -> setMinimumWidth   ( 168                         )         ;
  panel      -> setMaximumWidth   ( 168                         )         ;
  display    -> setMinimumHeight  ( 32                          )         ;
  /////////////////////////////////////////////////////////////////////////
  progress -> setTickInterval ( 30                              )         ;
  progress -> setTickPosition ( QSlider::TicksAbove             )         ;
  progress -> setRange        ( 0 , 300                         )         ;
  progress -> setEnabled      ( false                           )         ;
  /////////////////////////////////////////////////////////////////////////
  volume   -> setTickInterval ( 1000                            )         ;
  volume   -> setTickPosition ( QSlider::TicksBelow             )         ;
  volume   -> setRange        ( 0 , 10000                       )         ;
  volume   -> setValue        ( 10000                           )         ;
  /////////////////////////////////////////////////////////////////////////
  for (int i =  0 ; i <  6 ; i++ )                                        {
    QRect         g ( i * 28 , 0 , 28 , 28 )                              ;
    QToolButton * tb = new QToolButton ( buttons  )                       ;
    functions [ i ] = tb                                                  ;
    tb -> setAutoRaise ( true         )                                   ;
    tb -> setGeometry  ( g            )                                   ;
    tb -> setIconSize  ( QSize(28,28) )                                   ;
  }                                                                       ;
  for (int i = 0 ; i < 6 ; i++ )                                          {
    QRect         g ( i * 28 , 0 , 28 , 28 )                              ;
    QToolButton * tb = new QToolButton ( files    )                       ;
    functions [ i + 10 ] = tb                                             ;
    tb -> setAutoRaise ( true         )                                   ;
    tb -> setGeometry  ( g            )                                   ;
    tb -> setIconSize  ( QSize(28,28) )                                   ;
  }                                                                       ;
  for (int i = 0 ; i < 3 ; i++ )                                          {
    QRect         g ( i * 28 , 0 , 28 , 28 )                              ;
    QToolButton * tb = new QToolButton ( database )                       ;
    functions [ i + 20 ] = tb                                             ;
    tb -> setAutoRaise ( true         )                                   ;
    tb -> setGeometry  ( g            )                                   ;
    tb -> setIconSize  ( QSize(28,28) )                                   ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  QString ss                                                              ;
  ss = "QLabel { background-color: rgb(255, 255, 255); }"                 ;
  label -> setStyleSheet ( ss )                                           ;
  /////////////////////////////////////////////////////////////////////////
  functions [  0 ] -> setToolTip ( tr("Play list") )                      ;
  functions [  1 ] -> setToolTip ( tr("Database" ) )                      ;
  functions [  2 ] -> setToolTip ( tr("Play"     ) )                      ;
  functions [  3 ] -> setToolTip ( tr("Pause"    ) )                      ;
  functions [  4 ] -> setToolTip ( tr("Stop"     ) )                      ;
  functions [  5 ] -> setToolTip ( tr("Next"     ) )                      ;
  /////////////////////////////////////////////////////////////////////////
  functions [ 10 ] -> setToolTip ( tr("Panel"    ) )                      ;
  functions [ 11 ] -> setToolTip ( tr("Database" ) )                      ;
  functions [ 12 ] -> setToolTip ( tr("Add"      ) )                      ;
  functions [ 13 ] -> setToolTip ( tr("Remove"   ) )                      ;
  functions [ 14 ] -> setToolTip ( tr("Up"       ) )                      ;
  functions [ 15 ] -> setToolTip ( tr("Down"     ) )                      ;
  /////////////////////////////////////////////////////////////////////////
  functions [ 20 ] -> setToolTip ( tr("Panel"    ) )                      ;
  functions [ 21 ] -> setToolTip ( tr("Play list") )                      ;
  functions [ 22 ] -> setToolTip ( tr("Refresh"  ) )                      ;
  /////////////////////////////////////////////////////////////////////////
  functions [  0 ] -> setIcon    ( QIcon(":/images/documentitems.png" ) ) ;
  functions [  1 ] -> setIcon    ( QIcon(":/images/odbc.png"          ) ) ;
  functions [  2 ] -> setIcon    ( QIcon(":/images/PlayerPlay.png"    ) ) ;
  functions [  3 ] -> setIcon    ( QIcon(":/images/PlayPause.png"     ) ) ;
  functions [  4 ] -> setIcon    ( QIcon(":/images/StopPlay.png"      ) ) ;
  functions [  5 ] -> setIcon    ( QIcon(":/images/PlayNext.png"      ) ) ;
  /////////////////////////////////////////////////////////////////////////
  functions [ 10 ] -> setIcon    ( QIcon(":/images/control.png"       ) ) ;
  functions [ 11 ] -> setIcon    ( QIcon(":/images/odbc.png"          ) ) ;
  functions [ 12 ] -> setIcon    ( QIcon(":/images/zoomin.png"        ) ) ;
  functions [ 13 ] -> setIcon    ( QIcon(":/images/zoomout.png"       ) ) ;
  functions [ 14 ] -> setIcon    ( QIcon(":/images/up.png"            ) ) ;
  functions [ 15 ] -> setIcon    ( QIcon(":/images/down.png"          ) ) ;
  /////////////////////////////////////////////////////////////////////////
  functions [ 20 ] -> setIcon    ( QIcon(":/images/control.png"       ) ) ;
  functions [ 21 ] -> setIcon    ( QIcon(":/images/documentitems.png" ) ) ;
  functions [ 22 ] -> setIcon    ( QIcon(":/images/update.png"        ) ) ;
  /////////////////////////////////////////////////////////////////////////
  QStringList h                                                           ;
  h << tr("Name")                                                         ;
  h << tr("Time")                                                         ;
  h << tr("File")                                                         ;
  list -> setColumnCount          ( 3        )                            ;
  list -> setRootIsDecorated      ( false    )                            ;
  list -> setAlternatingRowColors ( true     )                            ;
  list -> assignHeaderItems       ( h        )                            ;
  list -> setColumnHidden         ( 1 , true )                            ;
  list -> setColumnHidden         ( 2 , true )                            ;
  /////////////////////////////////////////////////////////////////////////
  nConnect ( functions [  0 ],SIGNAL(clicked()),this,SLOT(Files ()) )     ;
  nConnect ( functions [  1 ],SIGNAL(clicked()),this,SLOT(Depot ()) )     ;
  nConnect ( functions [  2 ],SIGNAL(clicked()),this,SLOT(PLAY  ()) )     ;
  nConnect ( functions [  3 ],SIGNAL(clicked()),this,SLOT(PAUSE ()) )     ;
  nConnect ( functions [  4 ],SIGNAL(clicked()),this,SLOT(STOP  ()) )     ;
  nConnect ( functions [  5 ],SIGNAL(clicked()),this,SLOT(NEXT  ()) )     ;
  nConnect ( functions [ 10 ],SIGNAL(clicked()),this,SLOT(Pad   ()) )     ;
  nConnect ( functions [ 11 ],SIGNAL(clicked()),this,SLOT(Depot ()) )     ;
  nConnect ( functions [ 12 ],SIGNAL(clicked()),this,SLOT(Browse()) )     ;
  nConnect ( functions [ 13 ],SIGNAL(clicked()),this,SLOT(REMOVE()) )     ;
  nConnect ( functions [ 14 ],SIGNAL(clicked()),this,SLOT(UP    ()) )     ;
  nConnect ( functions [ 15 ],SIGNAL(clicked()),this,SLOT(DOWN  ()) )     ;
  nConnect ( functions [ 20 ],SIGNAL(clicked()),this,SLOT(Pad   ()) )     ;
  nConnect ( functions [ 21 ],SIGNAL(clicked()),this,SLOT(Files ()) )     ;
  nConnect ( functions [ 22 ],SIGNAL(clicked()),this,SLOT(Renew ()) )     ;
  /////////////////////////////////////////////////////////////////////////
  functions [  0 ] -> setEnabled ( true                                 ) ;
  functions [  1 ] -> setEnabled ( true                                 ) ;
  functions [  2 ] -> setEnabled ( false                                ) ;
  functions [  3 ] -> setEnabled ( false                                ) ;
  functions [  4 ] -> setEnabled ( false                                ) ;
  functions [  5 ] -> setEnabled ( false                                ) ;
  functions [ 10 ] -> setEnabled ( true                                 ) ;
  functions [ 11 ] -> setEnabled ( true                                 ) ;
  functions [ 12 ] -> setEnabled ( true                                 ) ;
  functions [ 13 ] -> setEnabled ( false                                ) ;
  functions [ 14 ] -> setEnabled ( false                                ) ;
  functions [ 15 ] -> setEnabled ( false                                ) ;
  functions [ 20 ] -> setEnabled ( true                                 ) ;
  functions [ 21 ] -> setEnabled ( true                                 ) ;
  functions [ 22 ] -> setEnabled ( true                                 ) ;
  /////////////////////////////////////////////////////////////////////////
  Booleans  [  0 ]  = true                                                ;
  Booleans  [  1 ]  = true                                                ;
  Booleans  [  2 ]  = false                                               ;
  Booleans  [  3 ]  = false                                               ;
  Booleans  [  4 ]  = false                                               ;
  Booleans  [  5 ]  = false                                               ;
  Booleans  [  6 ]  = false                                               ;
  Booleans  [  7 ]  = false                                               ;
  Booleans  [  8 ]  = false                                               ;
  Booleans  [  9 ]  = false                                               ;
  Booleans  [ 51 ]  = false                                               ;
  start ( 10001 )                                                         ;
  /////////////////////////////////////////////////////////////////////////
  nConnect ( list     , SIGNAL(itemClicked(QTreeWidgetItem*,int))         ,
             this     , SLOT  (itemClicked(QTreeWidgetItem*,int))       ) ;
  nConnect ( progress , SIGNAL(valueChanged(int))                         ,
             this     , SLOT  (currentTime (int))                       ) ;
  nConnect ( volume   , SIGNAL(valueChanged(int))                         ,
             this     , SLOT  (setVolume   (int))                       ) ;
  /////////////////////////////////////////////////////////////////////////
  Timer . setParent ( this )                                              ;
  nConnect ( &Timer , SIGNAL(timeout()) , this , SLOT(Updating()) )       ;
  Timer . start     ( 100  )                                              ;
  /////////////////////////////////////////////////////////////////////////
  lastPath = plan->Path(N::Directory::Sounds).absoluteFilePath("")        ;
  /////////////////////////////////////////////////////////////////////////
  volume -> setToolTip  ( tr("Volume %1").arg(100) )                      ;
  plan   -> setFont     ( this                     )                      ;
  /////////////////////////////////////////////////////////////////////////
  nConnect ( depot , SIGNAL (AddChannel (QString,SUID,QString) )          ,
             this  , SLOT   (AddChannel (QString,SUID,QString) )        ) ;
  start ( 10005 )                                                         ;
//  depot -> setFunction ( 101 , true )                                     ;
}

void N::AudioPanel::run(int type,ThreadData * data)
{
  switch ( type )                                         {
    case 10001                                            :
      startLoading     ( )                                ;
      Monitor          ( )                                ;
      stopLoading      ( )                                ;
    break                                                 ;
    case 10002                                            :
      startLoading     ( )                                ;
      if ( data -> Arguments . count ( ) > 0 )            {
        QString filename                                  ;
        filename = data -> Arguments [ 0 ] . toString ( ) ;
        Play ( data , filename )                          ;
      }                                                   ;
      stopLoading      ( )                                ;
    break                                                 ;
    case 10003                                            :
      startLoading     ( )                                ;
      OpenDevice       ( )                                ;
      stopLoading      ( )                                ;
    break                                                 ;
    case 10004                                            :
      startLoading     ( )                                ;
      DisplayWave      ( )                                ;
      stopLoading      ( )                                ;
    break                                                 ;
    case 10005                                            :
      startLoading     ( )                                ;
      depot -> startup ( )                                ;
      stopLoading      ( )                                ;
    break                                                 ;
  }                                                       ;
}

void N::AudioPanel::setWrapper(ToolWrapper * w)
{
  wrapper = w ;
}

void N::AudioPanel::Appear(void)
{
  QWidget * w = this                 ;
  if ( NULL != wrapper ) w = wrapper ;
  nDropOut ( w -> isVisible ( ) )    ;
  FadeIn   ( w , 25             )    ;
}

void N::AudioPanel::Disappear(void)
{
  QWidget * w = this                 ;
  if ( NULL != wrapper ) w = wrapper ;
  w -> hide ( )                      ;
}

void N::AudioPanel::Eradicate(void)
{
  QWidget * w = this                 ;
  if ( NULL != wrapper ) w = wrapper ;
  w -> deleteLater ( )               ;
}

void N::AudioPanel::showStyle(int style)
{
  switch ( style )           {
    case 0                   :
      label      -> show ( ) ;
      progress   -> show ( ) ;
      horizontal -> show ( ) ;
    break                    ;
    case 1                   :
      label      -> hide ( ) ;
      progress   -> hide ( ) ;
      horizontal -> hide ( ) ;
    break                    ;
  }
}

void N::AudioPanel::PLAY(void)
{
  if ( Booleans [ 1 ] ) return                  ;
  bool more = ( list->topLevelItemCount() > 0 ) ;
  Booleans [ 0 ]  = true                        ;
  Booleans [ 1 ]  = true                        ;
  start ( 10001 )                               ;
  functions [ 2 ] ->setEnabled ( false )        ;
  functions [ 3 ] ->setEnabled ( true  )        ;
  functions [ 4 ] ->setEnabled ( true  )        ;
  functions [ 5 ] ->setEnabled ( more  )        ;
}

void N::AudioPanel::PAUSE(void)
{
  if ( Booleans [ 51 ] )             {
    Booleans [ 51 ] = false          ;
    progress -> setEnabled ( true  ) ;
  } else                             {
    Booleans [ 51 ] = true           ;
    progress -> setEnabled ( false ) ;
  }                                  ;
}

void N::AudioPanel::STOP(void)
{
  Booleans  [ 0 ] = false                ;
  Booleans  [ 2 ] = false                ;
  Booleans  [ 5 ] = false                ;
  functions [ 2 ] ->setEnabled ( true  ) ;
  functions [ 3 ] ->setEnabled ( false ) ;
  functions [ 4 ] ->setEnabled ( false ) ;
  functions [ 5 ] ->setEnabled ( false ) ;
}

void N::AudioPanel::NEXT(void)
{
  Booleans  [ 2 ] = false ;
  Booleans  [ 5 ] = false ;
}

void N::AudioPanel::itemClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column )                                     ;
  int index = list -> indexOfTopLevelItem ( item )        ;
  int cnt   = list -> topLevelItemCount   (      )        ;
  Alert ( Action )                                        ;
  if ( 0 == index )                                       {
    int rid = nTreeInt ( item , 0 )                       ;
    if ( 1 == rid )                                       {
      functions [ 13 ] -> setEnabled ( false )            ;
      functions [ 14 ] -> setEnabled ( false )            ;
      functions [ 15 ] -> setEnabled ( false )            ;
      return                                              ;
    }                                                     ;
  }                                                       ;
  if ( 1 == index )                                       {
    QTreeWidgetItem * it                                  ;
    it = list -> topLevelItem ( 0 )                       ;
    if ( NotNull(it) )                                    {
      functions [ 13 ] -> setEnabled ( true    )          ;
      functions [ 14 ] -> setEnabled ( false   )          ;
      functions [ 15 ] -> setEnabled ( cnt > 2 )          ;
    }                                                     ;
  }                                                       ;
  functions [ 13 ] -> setEnabled ( true                 ) ;
  functions [ 14 ] -> setEnabled ( 0 != index           ) ;
  functions [ 15 ] -> setEnabled ( ( index + 1 ) != cnt ) ;
}

void N::AudioPanel::REMOVE(void)
{
  QTreeWidgetItem * it                     ;
  int               index                  ;
  it = list->currentItem()                 ;
  if ( IsNull(it) ) return                 ;
  index = list->indexOfTopLevelItem(it)    ;
  if ( index < 0 ) return                  ;
  if ( 0 == index )                        {
    int rid = nTreeInt ( it , 0 )          ;
    if ( 1 == rid ) return                 ;
  }                                        ;
  list -> takeTopLevelItem ( index )       ;
  if ( index < list->topLevelItemCount() ) {
    it  = list->topLevelItem(index)        ;
    if ( NotNull(it) )                     {
      list -> setCurrentItem ( it )        ;
    }                                      ;
  }                                        ;
  Alert ( Action )                         ;
}

void N::AudioPanel::UP(void)
{
  QTreeWidgetItem * it                          ;
  int               index                       ;
  it = list->currentItem()                      ;
  if ( IsNull(it) ) return                      ;
  index = list->indexOfTopLevelItem(it)         ;
  if ( index < 0 ) return                       ;
  if ( 0 == index ) return                      ;
  if ( 1 == index )                             {
    QTreeWidgetItem * zt                        ;
    zt = list->topLevelItem(0)                  ;
    int rid = nTreeInt ( zt , 0 )               ;
    if ( 1 == rid ) return                      ;
  }                                             ;
  list -> takeTopLevelItem   ( index          ) ;
  list -> insertTopLevelItem ( index - 1 , it ) ;
  list -> setCurrentItem     ( it             ) ;
  Alert ( Action )                              ;
}

void N::AudioPanel::DOWN(void)
{
  QTreeWidgetItem * it                                 ;
  int               index                              ;
  it = list->currentItem()                             ;
  if ( IsNull(it) ) return                             ;
  index = list->indexOfTopLevelItem(it)                ;
  if ( index < 0 ) return                              ;
  if ( list->topLevelItemCount() <= (index+1) ) return ;
  if ( 0 == index )                                    {
    int rid = nTreeInt ( it , 0 )                      ;
    if ( 1 == rid ) return                             ;
  }                                                    ;
  list -> takeTopLevelItem   ( index          )        ;
  list -> insertTopLevelItem ( index + 1 , it )        ;
  list -> setCurrentItem     ( it             )        ;
  Alert ( Action )                                     ;
}

void N::AudioPanel::AddChannel(QString name,SUID uuid,QString filename)
{
  stack -> setCurrentWidget ( list    ) ;
  panel -> setCurrentWidget ( files   ) ;
  Add ( name , uuid , filename )        ;
}

void N::AudioPanel::Add(QString name,SUID uuid,QString filename)
{
  NewTreeWidgetItem       ( IT                      ) ;
  IT   -> setText         ( 0 , name                ) ;
  IT   -> setText         ( 2 , filename            ) ;
  /////////////////////////////////////////////////////
  IT   -> setData         ( 0 , Qt::UserRole , 0    ) ;
  IT   -> setData         ( 1 , Qt::UserRole , uuid ) ;
  /////////////////////////////////////////////////////
  AvDecoder decoder                                   ;
  if ( ! decoder . StartAudio ( filename ) )          {
    decoder . Close ( )                               ;
    delete IT                                         ;
    return                                            ;
  }                                                   ;
  /////////////////////////////////////////////////////
  TUID length = decoder . Duration ( )                ;
  decoder . Close ( )                                 ;
  /////////////////////////////////////////////////////
  QTime T(0,0,0,0)                                    ;
  T   = T . addMSecs ( length / 1000 )                ;
  IT -> setText ( 1 , T.toString("hh:mm:ss.zzz") )    ;
  /////////////////////////////////////////////////////
  list -> addTopLevelItem ( IT                      ) ;
}

void N::AudioPanel::Add(QString filename)
{
  QFileInfo F ( filename )            ;
  Add ( F.fileName() , 0 , filename ) ;
}

void N::AudioPanel::Add(SUID uuid)
{
  if ( uuid <= 0 ) return            ;
  QString name                       ;
  EnterSQL ( SC , plan->sql )        ;
    name = SC . getName              (
             PlanTable(Names)        ,
             "uuid"                  ,
             vLanguageId             ,
             uuid                  ) ;
  LeaveSQL ( SC , plan->sql )        ;
  if ( name.length() <= 0 ) return   ;
  VideoManager VM(plan)              ;
  QString      filename              ;
  filename = VM.Location(*plan,uuid) ;
  if (filename.length()<=0) return   ;
  Add ( name , uuid , filename )     ;
}

bool N::AudioPanel::shutdown(void)
{
  if ( ! Booleans [ 1 ] ) return true        ;
  if ( ! Booleans [ 0 ] ) return false       ;
  Timer . stop ( )                           ;
  Booleans [ 0 ] = false                     ;
  while ( Booleans [ 1 ] ) Time :: skip(100) ;
  return true                                ;
}

void N::AudioPanel::Browse(void)
{
  QStringList filters                           ;
  QString     filename                          ;
  QString     filter                            ;
  filters  = N::Acoustics::ExtensionFilters (0) ;
  filter   . append ( "*.* ("                 ) ;
  filter   . append ( filters.join(" ")       ) ;
  filter   . append ( ")\n"                   ) ;
  filter   . append ( filters.join("\n")      ) ;
  filename = QFileDialog::getOpenFileName       (
               this                             ,
               tr("Add audio into play list")   ,
               lastPath                         ,
               filter                         ) ;
  if ( filename . length ( ) <= 0 ) return      ;
  QFileInfo F ( filename )                      ;
  if ( ! F.exists() ) return                    ;
  lastPath = F . absolutePath ( )               ;
  Add ( filename ) ;
}

void N::AudioPanel::Pad(void)
{
  stack -> setCurrentWidget ( display ) ;
  panel -> setCurrentWidget ( buttons ) ;
}

void N::AudioPanel::Files(void)
{
  stack -> setCurrentWidget ( list    ) ;
  panel -> setCurrentWidget ( files   ) ;
}

void N::AudioPanel::Depot(void)
{
  stack -> setCurrentWidget ( depot    ) ;
  panel -> setCurrentWidget ( database ) ;
}

void N::AudioPanel::Renew(void)
{
  depot -> clear ( ) ;
  start ( 10005 )    ;
}

void N::AudioPanel::currentTime(int value)
{
  if ( IsNull(stream)   ) return                                 ;
  if (   Booleans [ 6 ] ) return                                 ;
  if ( ! Booleans [ 5 ] ) return                                 ;
  Booleans [ 6 ] = true                                          ;
  while ( Booleans [ 0 ] && Booleans [ 6 ] && ! Booleans [ 7 ] ) {
    Time :: skip ( 50 )                                          ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  TUID msecs = value * 1000                                      ;
  if ( ( msecs >= 0 ) && RawSeek ( msecs ) )                     {
    msecs *= 1000                                                ;
    totalOutput  = msecs                                         ;
    totalOutput *= audioData . BytesPerSecond ( )                ;
    totalOutput /= 1000000                                       ;
    currentPTS   = msecs                                         ;
    msecs        = audioData . AudioSize      ( )                ;
    audioData    . takeAt ( 0 , msecs )                          ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  Booleans [ 6 ] = false                                         ;
  Booleans [ 7 ] = false                                         ;
}

void N::AudioPanel::setVolume(int value)
{
  emit volumeChanged ( value )                 ;
  if ( IsNull ( stream )         ) return      ;
  if ( ! stream -> hasVolume ( ) ) return      ;
  stream -> setVolume ( value )                ;
  QString m                                    ;
  m = tr ( "Volume %1" ) . arg ( value / 100 ) ;
  volume -> setToolTip  ( m           )        ;
}

int N::AudioPanel::audioVolume(void)
{
  return volume -> value ( ) ;
}

void N::AudioPanel::setAudioVolume(int value)
{
  volume -> setValue ( value ) ;
}

int N::AudioPanel::obtain(void)
{
  using namespace CiosAudio                             ;
  if ( Booleans [ 51 ]                                 ||
       Booleans [  6 ]                                 ||
       Booleans [  7 ]                                ) {
    ::memset ( Output . Buffer , 0 , Output.Total()   ) ;
    return Output . Result                              ;
  }                                                     ;
  int as = audioData.AudioSize()                        ;
  if ( as <= 0 ) return Output . Result                 ;
  int ts = Output . Total ( )                           ;
  if ( ts > as ) ts = as                                ;
  ///////////////////////////////////////////////////////
  ::memcpy ( Output.Buffer , audioData.index(0) , ts  ) ;
  LockGui             (                               ) ;
  waveData  . add     ( audioData . index(0) , ts     ) ;
  audioData . takeAt  ( 0 , ts                        ) ;
  as = waveData.Frames() - waveData.Rate()              ;
  if ( as > 0 )                                         {
    as *= waveData . BytesPerFrame ( )                  ;
    waveData . takeAt ( 0 , as )                        ;
  }                                                     ;
  UnlockGui           (                               ) ;
  ///////////////////////////////////////////////////////
  long long z                                           ;
  totalOutput += ts                                     ;
  z            = totalOutput                            ;
  z           *= 1000000                                ;
  z           /= waveData . BytesPerSecond ( )          ;
  currentPTS   = z                                      ;
  ///////////////////////////////////////////////////////
  return Continue                                       ;
}

int N::AudioPanel::put(void)
{
  using namespace CiosAudio ;
  return Complete           ;
}

void N::AudioPanel::finish        (
       ConduitDirection direction ,
       FinishCondition  condition )
{ Q_UNUSED ( condition )                     ;
  using namespace CiosAudio                  ;
  if ( OutputDirection != direction ) return ;
  Booleans [ 2 ] = false                     ;
}

void N::AudioPanel::Monitor(void)
{
  QTreeWidgetItem * it                ;
  /////////////////////////////////////
  while ( Booleans [ 0 ] )            {
    if ( list->topLevelItemCount()>0) {
      it = list -> topLevelItem ( 0 ) ;
      int rid = nTreeInt(it,0)        ;
      if ( rid == 0 )                 {
        QString filename              ;
        VarArgs args                  ;
        it->setData(0,Qt::UserRole,1) ;
        filename = it->text(2)        ;
        args << filename              ;
        label -> setText(it->text(0)) ;
        start ( 10002 , args )        ;
      }                               ;
    }                                 ;
    Time :: msleep ( 100 )            ;
  }                                   ;
  /////////////////////////////////////
  while ( Booleans [ 1 ] )            {
    Time :: msleep ( 10 )             ;
    if ( Booleans [ 2 ] ) continue    ;
    if ( Booleans [ 3 ] ) continue    ;
    if ( Booleans [ 4 ] ) continue    ;
    if ( Booleans [ 5 ] ) continue    ;
    if ( Booleans [ 6 ] ) continue    ;
    if ( Booleans [ 7 ] ) continue    ;
    if ( Booleans [ 8 ] ) continue    ;
    if ( Booleans [ 9 ] ) continue    ;
    Booleans [ 1 ] = false            ;
  }                                   ;
}

void N::AudioPanel::OpenDevice(void)
{
  int                       deviceId                                         ;
  int                       frameCount                                       ;
  int                       BPS                                              ;
  bool                      correct = true                                   ;
  CiosAudio::CaSampleFormat sampleFormat                                     ;
  CiosAudio::CaError        rt                                               ;
  ////////////////////////////////////////////////////////////////////////////
  Booleans [ 2 ]  = true                                                     ;
  Booleans [ 3 ]  = true                                                     ;
  Output . Result = CiosAudio::Conduit::Continue                             ;
  deviceId        = plan -> EquipmentIDs [ N::Plan::DefaultAudioOutput ]     ;
  sampleFormat    = Acoustics::toAudioCore ( audioData . Format ( ) )        ;
  frameCount      = audioData.Rate ( ) / 10                                  ;
  BPS             = audioData . BytesPerSecond( )                            ;
  ////////////////////////////////////////////////////////////////////////////
  CiosAudio::StreamParameters ONSP ( deviceId                                ,
                                     audioData . Channels ( )                ,
                                     sampleFormat                          ) ;
  ONSP . suggestedLatency = core -> GetDeviceInfo(deviceId)->defaultLowOutputLatency ;
  ////////////////////////////////////////////////////////////////////////////
  rt = core -> Open ( &stream                                                ,
                      NULL                                                   ,
                      &ONSP                                                  ,
                      audioData . Rate ( )                                   ,
                      frameCount                                             ,
                      0                                                      ,
                      this                                                 ) ;
  if ( ( CiosAudio::NoError != rt ) || ( NULL == stream ) ) correct = false  ;
  ////////////////////////////////////////////////////////////////////////////
  if ( correct )                                                             {
    rt = core -> Start ( stream )                                            ;
    if ( CiosAudio::NoError != rt ) correct = false                          ;
    //////////////////////////////////////////////////////////////////////////
    if ( correct && stream -> hasVolume ( ) )                                {
      stream -> setVolume ( volume->value() )                                ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    while ( Booleans [ 2 ] &&
            correct                                                         &&
            ( 1 == core -> IsActive  ( stream ) )                          ) {
      Time :: msleep ( 100 )                                                 ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    if ( correct && ( 0 != core -> IsStopped ( stream ) ) )                  {
      core -> Stop ( stream )                                                ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  core -> Close ( stream )                                                   ;
  stream = NULL                                                              ;
  ////////////////////////////////////////////////////////////////////////////
  Booleans [ 2 ]  = false                                                    ;
  Booleans [ 3 ]  = false                                                    ;
  duration        = -1                                                       ;
  currentPTS      = -1                                                       ;
  totalOutput     =  0                                                       ;
  Output . Result = CiosAudio::Conduit::Complete                             ;
  ////////////////////////////////////////////////////////////////////////////
  list  -> takeTopLevelItem ( 0  )                                           ;
  label -> setText          ( "" )                                           ;
  bool more = ( list->topLevelItemCount() > 0 )                              ;
  functions [ 5 ] ->setEnabled ( more  )                                     ;
  emit Finished ( )                                                          ;
}

void N::AudioPanel::Updating(void)
{
  if ( duration < 0 )                         {
    progress -> blockSignals ( true    )      ;
    progress -> setRange     ( 0 , 300 )      ;
    progress -> setValue     ( 0       )      ;
    progress -> setEnabled   ( false   )      ;
    progress -> blockSignals ( false   )      ;
  } else                                      {
    int s = duration   / 1000000              ;
    int t = currentPTS / 1000000              ;
    progress -> blockSignals ( true    )      ;
    progress -> setRange     ( 0 , s   )      ;
    progress -> setEnabled   ( true    )      ;
    if ( currentPTS >= 0 )                    {
      QTime   T ( 0 , 0 , 0 , 0 )             ;
      QTime   Z ( 0 , 0 , 0 , 0 )             ;
      QString m                               ;
      progress -> setValue ( t       )        ;
      T  = T . addMSecs ( currentPTS / 1000 ) ;
      Z  = Z . addMSecs ( duration   / 1000 ) ;
      m  = T . toString ( "hh:mm:ss.zzz"    ) ;
      m += " / "                              ;
      m += Z . toString ( "hh:mm:ss.zzz"    ) ;
      progress -> setToolTip ( m )            ;
    }                                         ;
    progress -> blockSignals ( false )        ;
  }                                           ;
  if ( Booleans [ 2 ] || Booleans [ 5 ] )     {
    if ( display->isVisible() )               {
      display -> update ( )                   ;
    }                                         ;
  }                                           ;
}

void N::AudioPanel::Place(int channel,int channels,QSize s)
{
  int    rate = waveData . Rate ( )                                    ;
  double sx   = s . width  ( )                                         ;
  double fx   = s . width  ( )                                         ;
  double sy   = s . height ( )                                         ;
  if ( channels == 1 )                                                 {
    sx /= rate                                                         ;
    fx /= MaxFreq                                                      ;
    sy /= Bounds . Distance ( )                                        ;
    QSizeF  SS ( sx , -sy            )                                 ;
    QSizeF  SX ( fx , -FreqFactor    )                                 ;
    QPointF SP ( 0  , s.height() / 2 )                                 ;
    QPointF XP ( 0  , s.height()     )                                 ;
    Shifts [   0 ] = SP                                                ;
    Scales [   0 ] = SS                                                ;
    Shifts [ 100 ] = XP                                                ;
    Scales [ 100 ] = SX                                                ;
  } else                                                               {
    double H = s.height()                                              ;
    double h = H                                                       ;
    sx /= rate                                                         ;
    fx /= MaxFreq                                                      ;
    sy /= Bounds . Distance ( )                                        ;
    sy /= channels                                                     ;
    H  /= 2                                                            ;
    H  *= ( channel + 1 )                                              ;
    h  /= 4                                                            ;
    h  *= ((channel*2)+1)                                              ;
    QSizeF  SS ( sx , -sy         )                                    ;
    QSizeF  SX ( fx , -FreqFactor )                                    ;
    QPointF SP ( 0  , h           )                                    ;
    QPointF XP ( 0  , H           )                                    ;
    Shifts [ channel       ] = SP                                      ;
    Scales [ channel       ] = SS                                      ;
    Shifts [ channel + 100 ] = XP                                      ;
    Scales [ channel + 100 ] = SX                                      ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  Thickness  = sx * 4                                                  ;
  switch (channel)                                                     {
    case 0                                                             :
      PENs [   0           ] = QPen      ( QColor(   0 ,   0 , 255 ) ) ;
      PENs [   0           ] . setWidthF ( Thickness                 ) ;
      PENs [ 100           ] = QPen      ( QColor(   0 , 128 ,   0 ) ) ;
      PENs [ 100           ] . setWidthF ( Thickness                 ) ;
    break                                                              ;
    case 1                                                             :
      PENs [   1           ] = QPen      ( QColor( 255 ,   0 ,   0 ) ) ;
      PENs [   1           ] . setWidthF ( Thickness                 ) ;
      PENs [ 101           ] = QPen      ( QColor( 128 ,   0 , 128 ) ) ;
      PENs [ 101           ] . setWidthF ( Thickness                 ) ;
    break                                                              ;
    default                                                            :
      PENs [       channel ] = QPen      ( QColor(   0 ,   0 ,   0 ) ) ;
      PENs [       channel ] . setWidthF ( Thickness                 ) ;
      PENs [ 100 + channel ] = QPen      ( QColor( 128 , 128 ,  64 ) ) ;
      PENs [ 100 + channel ] . setWidthF ( Thickness                 ) ;
    break                                                              ;
  }                                                                    ;
}

void N::AudioPanel::PaintWave(AudioData & data)
{
  QImage Drawing                                     ;
  QImage Background                                  ;
  QImage Painting                                    ;
  display    -> LockImage   ( )                      ;
  Drawing     = display -> Drawing                   ;
  Background  = display -> Background                ;
  Painting    = display -> Painting                  ;
  display    -> UnlockImage ( )                      ;
  ////////////////////////////////////////////////////
  QPainter d                                         ;
  QPainter x                                         ;
  int      channels = data . Channels ( )            ;
  d . begin         ( &Drawing           )           ;
  d . drawImage     ( 0 , 0 , Background )           ;
  ////////////////////////////////////////////////////
  for (int i=0;i<channels;i++)                       {
    d . setPen  ( PENs    [ i ]                    ) ;
    PaintVector ( Scales  [ i ]                      ,
                  Shifts  [ i ]                      ,
                  Vectors [ i ]                      ,
                  d                                ) ;
  }                                                  ;
  for (int i=0;i<channels;i++)                       {
    d . setPen  ( PENs    [ i + 100 ]              ) ;
    PaintVector ( Scales  [ i + 100 ]                ,
                  Shifts  [ i + 100 ]                ,
                  Vectors [ i + 100 ]                ,
                  d                                ) ;
  }                                                  ;
  ////////////////////////////////////////////////////
  d . end        (                 )                 ;
  ////////////////////////////////////////////////////
  x . begin      ( &Painting       )                 ;
  x . drawImage  ( 0 , 0 , Drawing )                 ;
  x . end        (                 )                 ;
  ////////////////////////////////////////////////////
  display -> LockImage   ( )                         ;
  display -> Painting   = Painting                   ;
  display -> UnlockImage ( )                         ;
}

void N::AudioPanel::DisplayWave(void)
{
  bool           correct  = true                    ;
  bool           enabled  = false                   ;
  int            channels = waveData . Channels ( ) ;
  int            rate     = waveData . Rate     ( ) ;
  int            r2       = rate / 4                ;
  qint64         dt       = 0                       ;
  QSize          SIZE     = display -> size     ( ) ;
  QSize          NS                                 ;
  AudioData      WD                                 ;
  QByteArray     normalize                          ;
  QByteArray     complex                            ;
  FFTW           fftw                               ;
  fftw_plan      fftplan                            ;
  double       * v        = NULL                    ;
  double       * c        = NULL                    ;
  fftw_complex * cplex    = NULL                    ;
  double         BD                                 ;
  int            fftlen                             ;
  MaxFreq = Freq                                    ;
  if ( r2 < Freq ) MaxFreq = r2                     ;
  r2 = MaxFreq                                      ;
  ///////////////////////////////////////////////////
  Booleans [ 8 ] = true                             ;
  for (int i=0;i<channels;i++)                      {
    Vectors[i    ] = Vector( Cpp :: Double , 0    ) ;
    Vectors[i+100] = Vector( Cpp :: Double , r2   ) ;
  }                                                 ;
  ///////////////////////////////////////////////////
  Bounds . L = waveData . lowerBound ( )            ;
  Bounds . R = waveData . upperBound ( )            ;
  BD         = Bounds   . Distance   ( )            ;
  BD         = 1        / BD                        ;
  ///////////////////////////////////////////////////
  fftlen    = sizeof(double) * rate                 ;
  normalize . resize ( fftlen                     ) ;
  complex   . resize ( fftlen * 2                 ) ;
  v         = (double *) normalize . data ( )       ;
  c         = (double *) complex   . data ( )       ;
  cplex     = (fftw_complex *) c                    ;
  fftplan   = fftw . Frequency                      (
                rate                                ,
                v                                   ,
                cplex                               ,
                FFTW_MEASURE                      ) ;
  ///////////////////////////////////////////////////
  for (int i=0;i<channels;i++)                      {
    Place ( i , channels , SIZE )                   ;
  }                                                 ;
  ///////////////////////////////////////////////////
  while ( correct && Booleans [ 0 ] )               {
    if ( ! enabled )                                {
      enabled = true                                ;
      if ( ! Booleans [ 2 ] ) enabled = false       ;
      if ( ! Booleans [ 5 ] ) enabled = false       ;
    } else                                          {
      if ( ! Booleans [ 2 ] ) correct = false       ;
      if ( ! Booleans [ 5 ] ) correct = false       ;
    }                                               ;
    if ( correct && ! Booleans [ 51 ] )             {
      ///////////////////////////////////////////////
      LockGui           (        )                  ;
      WD = waveData                                 ;
      UnlockGui         (        )                  ;
      dt = WD . AudioSize ( )                       ;
      if ( dt > WD . BytesPerSecond() )             {
        dt  -= WD . BytesPerSecond ( )              ;
        WD . takeAt ( 0 , dt )                      ;
      }                                             ;
      NS = display -> size ( )                      ;
      if ( SIZE != NS )                             {
        SIZE = NS                                   ;
        for (int i=0;i<channels;i++)                {
          Place ( i , channels , SIZE )             ;
        }                                           ;
      }                                             ;
      for (int i=0;i<channels;i++)                  {
        int      f                                  ;
        int      rp                                 ;
        int      cp                                 ;
        double   dr                                 ;
        double   dc                                 ;
        double * pp                                 ;
        WD   . toVector ( Vectors[i] , i )          ;
        ::memcpy ( v,Vectors[i].array(),fftlen )    ;
        N::Math::FaPb ( rate , v , BD , -Bounds.L ) ;
        N::Math::AddValues ( rate , v , -0.5      ) ;
//        N::Math::AddValues ( rate , v , Bounds.L  ) ;
//        N::Math::Multiply  ( rate , v , BD        ) ;
        fftw . Execute  ( fftplan        )          ;
        pp = (double *)Vectors [ i + 100 ] .array() ;
        for ( f = 0 , rp = 0 , cp = 1               ;
              f < r2                                ;
              f++ , rp += 2 , cp += 2             ) {
          dr = c [ rp ]                             ;
          dc = c [ cp ]                             ;
          pp [ f ] = ::sqrt( ( dr*dr )+( dc*dc )  ) ;
        }                                           ;
      }                                             ;
      PaintWave ( WD )                              ;
    }                                               ;
    Time :: msleep ( 100 )                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  fftw . Destroy ( fftplan )                        ;
  ///////////////////////////////////////////////////
  Booleans [ 8 ] = false                            ;
}

void N::AudioPanel::Play(ThreadData * d,QString filename)
{
  Booleans [ 4 ] = true                                                ;
  Booleans [ 5 ] = true                                                ;
  Booleans [ 6 ] = false                                               ;
  Booleans [ 7 ] = false                                               ;
  if ( ! StartAudio ( filename ) )                                     {
    Booleans [ 4 ] = false                                             ;
    Booleans [ 5 ] = false                                             ;
    AvDecoder :: Close()                                               ;
    return                                                             ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  char * buffer = NULL                                                 ;
  int    channels = Channels       ( )                                 ;
  int    sample   = AvSampleFormat ( )                                 ;
  int    bits     = N :: Acoustics :: AvBits   ( sample )              ;
  int    format   = N :: Acoustics :: AvFormat ( sample )              ;
  //////////////////////////////////////////////////////////////////////
  LockGui           ( )                                                ;
  audioData . reset ( )                                                ;
  waveData  . reset ( )                                                ;
  audioData . set   ( AvSampleRate                               ( )   ,
                      bits                                             ,
                      channels                                         ,
                      (N::Acoustics::SampleFormat) format            ) ;
  waveData  . set   ( AvSampleRate                               ( )   ,
                      bits                                             ,
                      channels                                         ,
                      (N::Acoustics::SampleFormat) format            ) ;
  buffer = new char [ waveData.BytesPerSecond() ]                      ;
  ::memset ( buffer ,  0 , waveData.BytesPerSecond() )                 ;
  waveData . add    ( buffer , waveData . BytesPerSecond ( )         ) ;
  UnlockGui         ( )                                                ;
  delete [] buffer                                                     ;
  //////////////////////////////////////////////////////////////////////
  AVPacket Packet                                                      ;
  int      Ret                                                         ;
  bool     bootup = false                                              ;
  Initialize ( &Packet )                                               ;
  duration = Duration ( )                                              ;
  //////////////////////////////////////////////////////////////////////
  totalOutput = 0                                                      ;
  while ( Booleans [ 0 ] && Booleans [ 4 ] && Booleans [ 5 ] )         {
    if ( ! IsContinue ( d ) )                                          {
      Booleans [ 0 ] = false                                           ;
      continue                                                         ;
    }                                                                  ;
    if ( Booleans [ 6 ] )                                              {
      Booleans [ 7 ] = true                                            ;
      Time :: msleep ( 50 )                                            ;
      continue                                                         ;
    }                                                                  ;
    if ( audioData . Milliseconds ( ) >= 5000 )                        {
      Time :: msleep ( 50 )                                            ;
      continue                                                         ;
    }                                                                  ;
    Ret = ReadPacket(Packet)                                           ;
    switch (Ret)                                                       {
      case AVERROR_BUFFER_TOO_SMALL                                    :
      case AVERROR_EXTERNAL                                            :
      case AVERROR_BUG2                                                :
      case AVERROR_BSF_NOT_FOUND                                       :
      case AVERROR_BUG                                                 :
      case AVERROR_DECODER_NOT_FOUND                                   :
      case AVERROR_DEMUXER_NOT_FOUND                                   :
      case AVERROR_ENCODER_NOT_FOUND                                   :
      case AVERROR_EXIT                                                :
      case AVERROR_FILTER_NOT_FOUND                                    :
      case AVERROR_INVALIDDATA                                         :
      case AVERROR_MUXER_NOT_FOUND                                     :
      case AVERROR_OPTION_NOT_FOUND                                    :
      case AVERROR_PATCHWELCOME                                        :
      case AVERROR_PROTOCOL_NOT_FOUND                                  :
      case AVERROR_STREAM_NOT_FOUND                                    :
      case AVERROR_UNKNOWN                                             :
      case AVERROR_EXPERIMENTAL                                        :
      case AVERROR_EOF                                                 :
        Booleans [ 5 ] = false                                         ;
      break                                                            ;
      default                                                          :
        if (Ret<0) Booleans [ 5 ] = false ;                         else
        if ( isAudioChannel ( Packet ) )                               {
          LockGui     (                    )                           ;
          DecodeAudio ( Packet , audioData )                           ;
          UnlockGui   (                    )                           ;
          if ( ! bootup                                               &&
               audioData.AudioSize() > audioData.BytesPerSecond() )    {
            bool more = ( list->topLevelItemCount() > 1 )              ;
            bootup = true                                              ;
            start ( 10003 )                                            ;
            start ( 10004 )                                            ;
            functions [ 2 ] ->setEnabled ( false )                     ;
            functions [ 3 ] ->setEnabled ( true  )                     ;
            functions [ 4 ] ->setEnabled ( true  )                     ;
            functions [ 5 ] ->setEnabled ( more  )                     ;
          }                                                            ;
        }                                                              ;
      break                                                            ;
    }                                                                  ;
    Release ( &Packet )                                                ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  AvDecoder :: Close()                                                 ;
  Booleans [ 4  ] = false                                              ;
  Booleans [ 5  ] = false                                              ;
  Booleans [ 6  ] = false                                              ;
  Booleans [ 7  ] = false                                              ;
  Booleans [ 51 ] = false                                              ;
  Output . Result = CiosAudio::Conduit::Complete                       ;
}
