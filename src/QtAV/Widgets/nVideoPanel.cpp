#include <qtav.h>
#include "ui_nVideoPanel.h"

N::VideoPanel:: VideoPanel     (QWidget * parent,Plan * p)
              : Widget         (          parent,       p)
              , PlayController (          this           )
              , ui             (new Ui::nVideoPanel      )
              , Total          (0                        )
              , Current        (0                        )
              , Trigger        (true                     )
{
  WidgetClass            ;
  ui -> setupUi ( this ) ;
  connectTo     ( this ) ;
}

N::VideoPanel::~VideoPanel (void)
{
  delete ui;
}

UUIDs N::VideoPanel::PlayingUuids(void)
{
  UUIDs uuids                       ;
  CUIDs k = Uuids.keys()            ;
  for (int i=0;i<Uuids.count();i++) {
    int n = k[i]                    ;
    uuids << Uuids[n]               ;
  }                                 ;
  return uuids                      ;
}

void N::VideoPanel::Progress(QStringList & paraments)
{
  if (paraments.count()<=0) return                    ;
  if (paraments[0]!="A:"  ) return                    ;
  double f = paraments[1].toDouble()                  ;
  TUID   t = (TUID)(f * 1000)                         ;
  bool   same = false                                 ;
  same = ( LastTime == QDateTime::currentDateTime() ) ;
  if (!Seeking && !same)                              {
    ui -> Time -> blockSignals (true )                ;
    ui -> Time -> setValue     (t    )                ;
    ui -> Time -> blockSignals (false)                ;
    LastTime = QDateTime::currentDateTime()           ;
  }                                                   ;
  Current = t                                         ;
  if (!same)                                          {
    QTime T(0,0,0,0)                                  ;
    QTime C(0,0,0,0)                                  ;
    T = T.addMSecs(Total  )                           ;
    C = C.addMSecs(Current)                           ;
    QString M                                         ;
    M = QString("%1/%2")
        .arg(C.toString("hh:mm:ss"))
        .arg(T.toString("hh:mm:ss"))                  ;
    ui -> Time -> setToolTip (M)                      ;
  }                                                   ;
}

void N::VideoPanel::finished(int exitCode,QProcess::ExitStatus exitStatus)
{
  ui -> FullScreen -> setEnabled   (false) ;
  ui -> Time       -> setEnabled   (false) ;
  ui -> Back       -> setEnabled   (false) ;
  ui -> Normal     -> setEnabled   (false) ;
  ui -> Fast       -> setEnabled   (false) ;
  ui -> Stop       -> setEnabled   (false) ;
  ui -> Play       -> blockSignals (true ) ;
  ui -> Play       -> setChecked   (false) ;
  ui -> Play       -> blockSignals (false) ;
  if (Trigger) Next ()                     ;
}

void N::VideoPanel::readyReadStandardOutput(void)
{
  readStandardOutput();
}

void N::VideoPanel::started(void)
{
  ui -> FullScreen -> setEnabled (true ) ;
  ui -> Time       -> setEnabled (true ) ;
  ui -> Back       -> setEnabled (true ) ;
  ui -> Normal     -> setEnabled (true ) ;
  ui -> Fast       -> setEnabled (true ) ;
  ui -> Stop       -> setEnabled (true ) ;
}

void N::VideoPanel::dragEnterEvent(QDragEnterEvent * event)
{
  if (dragEnter(event)) event->acceptProposedAction() ;
  else QWidget::dragEnterEvent(event);
}

void N::VideoPanel::dragLeaveEvent(QDragLeaveEvent * event)
{
  if (removeDrop()) event->accept() ;
  else QWidget::dragLeaveEvent(event);
}

void N::VideoPanel::dragMoveEvent(QDragMoveEvent * event)
{
  if (dragMove(event)) event->acceptProposedAction() ;
  else QWidget::dragMoveEvent(event);
}

void N::VideoPanel::dropEvent(QDropEvent * event)
{
  if (drop(event)) event->acceptProposedAction() ;
  else QWidget::dropEvent(event);
}

QString N::VideoPanel::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(mime,"album/uuid;album/uuids");
}

UUIDs N::VideoPanel::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="album/uuid")            {
    SUID * suid = (SUID *)data.data()    ;
    Uuids << (*suid)                     ;
  } else
  if (mimetype=="album/uuids")           {
    SUID * suid  = (SUID *)data.data()   ;
    int    total = (int)suid[0]          ;
    for (int i=0;i<total;i++)            {
      Uuids << suid[i+1]                 ;
      plan->processEvents()              ;
    }                                    ;
  }                                      ;
  return Uuids                           ;
}

bool N::VideoPanel::acceptDrop(QWidget * source,const QMimeData * mime)
{
  if (mime->hasUrls()) return true ;
  QString mt = MimeType(mime);
  if (mt.length()<=0) return false ;
  return true ;
}

bool N::VideoPanel::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (mime->hasUrls()) return true ;
  QString mt = MimeType(mime);
  if (mt.length()<=0) return false ;
  return true ;
}

bool N::VideoPanel::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (mime->hasUrls()) return true ;
  QString mt = MimeType(mime);
  if (mt.length()<=0) return false ;
  return true ;
}

bool N::VideoPanel::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (mime->hasUrls())                     {
    URLs urls = mime->urls()               ;
    appendUrls(urls)                       ;
    return true                            ;
  } else                                   {
    QString mtype = MimeType  (mime      ) ;
    UUIDs   Uuids = MimeUuids (mime,mtype) ;
    if (Uuids.count()>0)                   {
      appendUuids(Uuids)                   ;
      return true                          ;
    }                                      ;
  }                                        ;
  return true                              ;
}

TUID N::VideoPanel::Length(void)
{
  return Total ;
}

TUID N::VideoPanel::Now(void)
{
  return Current ;
}

bool N::VideoPanel::Start(QString filename,QString name)
{
  VideoManager VM(plan)                             ;
  TUID duration = VM.Duration(filename)             ;
  if ( duration <= 0 )                              {
    QTimer::singleShot(1000,this,SLOT(Next()))      ;
    return false                                    ;
  }                                                 ;
  duration /= 1000                                  ;
  Total     = (int)duration                         ;
  Current   = 0                                     ;
  Seeking   = false                                 ;
  ui -> Play       -> blockSignals (true )          ;
  ui -> Play       -> setChecked   (true )          ;
  ui -> Play       -> blockSignals (false)          ;
  ui -> Play       -> setEnabled   (true )          ;
  ui -> Time -> setRange (0,(int)duration)          ;
  LastTime = QDateTime::currentDateTime()           ;
  plan -> Talk (name)                               ;
  bool existing = false                             ;
  QDateTime DX = QDateTime::currentDateTime()       ;
  while (!existing && Trigger)                      {
    QFile F(filename)                               ;
    if (F.exists()) existing = true; else           {
      Time::skip(1000)                             ;
    }                                               ;
    if (DX.secsTo(QDateTime::currentDateTime())>10) {
      DX = QDateTime::currentDateTime()             ;
      plan->Talk(tr("File access abnormal"))        ;
    }                                               ;
  }                                                 ;
  Play(filename)                                    ;
  return true                                       ;
}

bool N::VideoPanel::Stop(bool trigger)
{
  Trigger = trigger             ;
  if (!isPlaying()) return true ;
  Command("q")                  ;
  Process.terminate()           ;
  return true                   ;
}

bool N::VideoPanel::isPlaying(void)
{
  return ( Process.state() == QProcess::Running ) ;
}

void N::VideoPanel::TimeMoved(int ms)
{
  if (ms<0) return                    ;
  KeyMutex.  lock ()                  ;
  ui -> Time -> blockSignals (true )  ;
  ui -> Time -> setEnabled   (false)  ;
  Seeking = true                      ;
  if (ms>Current)                     {
    int ds = ms - Current             ;
    ds /= 10000                       ;
    for (int i=0;i<ds;i++)            {
      Command("@")                    ;
      plan->processEvents()           ;
      Time::msleep(100)              ;
    }                                 ;
  } else                              {
    int ds = Current - ms             ;
    ds /= 10000                       ;
    for (int i=0;i<ds;i++)            {
      Command("$")                    ;
      plan->processEvents()           ;
      Time::msleep(100)              ;
    }                                 ;
  }                                   ;
  ui -> Time -> setEnabled   (true )  ;
  ui -> Time -> blockSignals (false)  ;
  Seeking = false                     ;
  KeyMutex.unlock ()                  ;
}

void N::VideoPanel::State(bool enable)
{
  Pause () ;
}

void N::VideoPanel::FullScreen(void)
{
  Command("f") ;
}

void N::VideoPanel::Fast(void)
{
  Command("]") ;
}

void N::VideoPanel::Back(void)
{
  Command("[") ;
}

void N::VideoPanel::Normal(void)
{
  Command("^") ;
}

void N::VideoPanel::Pause(void)
{
  Command("p") ;
}

void N::VideoPanel::appendUuids(UUIDs & Uuids)
{
  UUIDs clips                               ;
  SUID  uuid                                ;
  Sql   sql = plan->sql                     ;
  SqlConnection SC(sql)                     ;
  VideoManager VM(plan)                     ;
  if (SC.open("nVideoPanel","appendUuids")) {
    foreach (uuid,Uuids)                    {
      UUIDs Xuids = VM.Clips(SC,uuid)       ;
      SUID  xuid                            ;
      foreach (xuid,Xuids) clips << xuid    ;
    }                                       ;
    SC.close()                              ;
  }                                         ;
  SC.remove()                               ;
  foreach (uuid,clips) appendEvent(0,uuid)  ;
  ui -> Videos -> blockSignals (false)      ;
  ui -> Videos -> clear ()                  ;
  ui -> Videos -> addItems (Names)          ;
  ui -> Videos -> setEnabled (true)         ;
  ui -> Videos -> blockSignals (true )      ;
  if (isPlaying()) return                   ;
  ui -> Play      -> setEnabled (true)      ;
  ui -> Clear     -> setEnabled (true)      ;
  ui -> Delete    -> setEnabled (true)      ;
  ui -> Next      -> setEnabled (true)      ;
  ui -> Export    -> setEnabled (true)      ;
  ui -> TimeTable -> setEnabled (true)      ;
  ui -> Database  -> setEnabled (true)      ;
  ui -> Analysis  -> setEnabled (true)      ;
  ui -> User      -> setEnabled (true)      ;
  Start (Files[0],Names[0])                 ;
}

void N::VideoPanel::appendClips(UUIDs & clips)
{
  SUID  uuid                                ;
  foreach (uuid,clips) appendEvent(0,uuid)  ;
  ui -> Videos -> blockSignals (false)      ;
  ui -> Videos -> clear ()                  ;
  ui -> Videos -> addItems (Names)          ;
  ui -> Videos -> setEnabled (true)         ;
  ui -> Videos -> blockSignals (true )      ;
  if (isPlaying()) return                   ;
  ui -> Play      -> setEnabled (true)      ;
  ui -> Clear     -> setEnabled (true)      ;
  ui -> Delete    -> setEnabled (true)      ;
  ui -> Next      -> setEnabled (true)      ;
  ui -> Export    -> setEnabled (true)      ;
  ui -> TimeTable -> setEnabled (true)      ;
  ui -> Database  -> setEnabled (true)      ;
  ui -> Analysis  -> setEnabled (true)      ;
  ui -> User      -> setEnabled (true)      ;
  Start (Files[0],Names[0])                 ;
}

void N::VideoPanel::appendUrls(QList<QUrl> & Urls)
{
  QStringList files                ;
  for (int i=0;i<Urls.count();i++) {
    files << Urls[i].toLocalFile() ;
  }                                ;
  appendFiles (files)              ;
}

void N::VideoPanel::Browse(void)
{
  QStringList Depots                                  ;
  Depots = plan->settings.getStringList("VideoDepot") ;
  QString path = plan->Path("Temp")                   ;
  if (Depots.count()>0) path = Depots[0]              ;
  QStringList files = QFileDialog::getOpenFileNames   (
    this,tr("Play video files"),path                ) ;
  if (files.count()<=0) return                        ;
  appendFiles (files)                                 ;
}

void N::VideoPanel::appendFiles(QStringList & files)
{
  if (files.count()<=0) return         ;
  for (int i=0;i<files.count();i++)    {
    int id = Files.count()             ;
    Files << files [i]                 ;
    Names << files [i]                 ;
    Uuids      [id] = 0                ;
    TimeTables [id] = 0                ;
    Events     [id] = 0                ;
  }                                    ;
  ui -> Videos -> blockSignals (false) ;
  ui -> Videos -> clear ()             ;
  ui -> Videos -> addItems (Names)     ;
  ui -> Videos -> setEnabled (true)    ;
  ui -> Videos -> blockSignals (true ) ;
  if (isPlaying()) return              ;
  ui -> Play      -> setEnabled (true) ;
  ui -> Clear     -> setEnabled (true) ;
  ui -> Delete    -> setEnabled (true) ;
  ui -> Next      -> setEnabled (true) ;
  ui -> Export    -> setEnabled (true) ;
  ui -> TimeTable -> setEnabled (true) ;
  ui -> Database  -> setEnabled (true) ;
  ui -> Analysis  -> setEnabled (true) ;
  ui -> User      -> setEnabled (true) ;
  Start (Files[0],Names[0])            ;
}

void N::VideoPanel::playEvent(SUID event,SUID clip)
{
  appendEvent(event,clip)              ;
  ui -> Videos -> blockSignals (false) ;
  ui -> Videos -> clear ()             ;
  ui -> Videos -> addItems (Names)     ;
  ui -> Videos -> setEnabled (true)    ;
  ui -> Videos -> blockSignals (true ) ;
  if (isPlaying()) return              ;
  if (Names.count()<=0) return         ;
  ui -> Play      -> setEnabled (true) ;
  ui -> Clear     -> setEnabled (true) ;
  ui -> Delete    -> setEnabled (true) ;
  ui -> Next      -> setEnabled (true) ;
  ui -> Export    -> setEnabled (true) ;
  ui -> TimeTable -> setEnabled (true) ;
  ui -> Database  -> setEnabled (true) ;
  ui -> Analysis  -> setEnabled (true) ;
  ui -> User      -> setEnabled (true) ;
  Start (Files[0],Names[0])            ;
}

void N::VideoPanel::appendEvent(SUID event,SUID clip)
{
  QStringList names                                                      ;
  QString     filename = ""                                              ;
  Sql sql = plan->sql                                                  ;
  SqlConnection SC(sql)                                                 ;
  VideoManager VM(plan)                                                 ;
  if (SC.open("nVideoPanel","appendEvent"))                              {
    QString Q                                                            ;
    names = VM.Names(SC,clip)                                            ;
    Q = sql.SelectFrom("filename",plan->Tables[Tables::Videos] ,
            QString("where uuid = %1").arg(clip)                       ) ;
    if (SC.Fetch(Q)) filename = SC.String(0)                             ;
    SC.close()                                                           ;
  }                                                                      ;
  SC.remove()                                                            ;
  if (names   .count ()<=0) return                                       ;
  if (filename.length()<=0) return                                       ;
  QString path = plan->VideoFile(filename)                               ;
  if (path.length()<=0) return                                           ;
  int index = Files.count ()                                             ;
  Files << path                                                          ;
  Names << names [0]                                                     ;
  Uuids      [index] = clip                                              ;
  TimeTables [index] = 0                                                 ;
  Events     [index] = event                                             ;
}

void N::VideoPanel::Clear(void)
{
  Files            . clear      (     ) ;
  Names            . clear      (     ) ;
  Uuids            . clear      (     ) ;
  TimeTables       . clear      (     ) ;
  Events           . clear      (     ) ;
  ui -> Videos    -> clear      (     ) ;
  ui -> Play      -> setEnabled (false) ;
  ui -> Clear     -> setEnabled (false) ;
  ui -> Delete    -> setEnabled (false) ;
  ui -> Next      -> setEnabled (false) ;
  ui -> Export    -> setEnabled (false) ;
  ui -> TimeTable -> setEnabled (false) ;
  ui -> Database  -> setEnabled (false) ;
  ui -> Analysis  -> setEnabled (false) ;
  ui -> User      -> setEnabled (false) ;
  Stop                          (     ) ;
}

void N::VideoPanel::Database(void)
{
  emit Append ( Files ) ;
}

void N::VideoPanel::Analysis(void)
{
  emit Analysis ( Files ) ;
}

void N::VideoPanel::User(void)
{
  emit User ( Files ) ;
}

void N::VideoPanel::Playlist(void)
{
  QStringList Depots                                           ;
  Depots = plan->settings.getStringList("VideoDepot")          ;
  QString path = plan->Path("Temp")                            ;
  if (Depots.count()>0) path = Depots[0]                       ;
  QString filename = QFileDialog::getOpenFileName              (
                       this,tr("Import video from play lists") ,
                       path,"*.m3u"                          ) ;
  if (filename.length()<=0) return                             ;
  VideoManager VM(plan)                                        ;
  QStringList M3U = VM.M3U(filename)                           ;
  appendFiles ( M3U )                                          ;
}

void N::VideoPanel::Next(void)
{
  // Finish current video
  if (isPlaying())                     {
    Stop()                             ;
    return                             ;
  }                                    ;
  // Remove current video
  int total = Files.count()            ;
  if (total<=0) return                 ;
  for (int i=1;i<total;i++)            {
    Uuids      [i-1] = Uuids      [i]  ;
    TimeTables [i-1] = TimeTables [i]  ;
    Events     [i-1] = Events     [i]  ;
  }                                    ;
  Files      . takeAt ( 0       )      ;
  Names      . takeAt ( 0       )      ;
  Uuids      . remove ( total-1 )      ;
  TimeTables . remove ( total-1 )      ;
  Events     . remove ( total-1 )      ;
  ui -> Play      -> setEnabled (false);
  ui -> Clear     -> setEnabled (false);
  ui -> Delete    -> setEnabled (false);
  ui -> Next      -> setEnabled (false);
  ui -> Export    -> setEnabled (false);
  ui -> TimeTable -> setEnabled (false);
  ui -> Database  -> setEnabled (false);
  ui -> Analysis  -> setEnabled (false);
  ui -> User      -> setEnabled (false);
  //////////////////////////////////////
  ui -> Videos -> blockSignals (false) ;
  ui -> Videos -> clear ()             ;
  ui -> Videos -> addItems (Names)     ;
  ui -> Videos -> blockSignals (true ) ;
  if (Files.count()<=0) return         ;
  ui -> Videos -> setEnabled (true)    ;
  //////////////////////////////////////
  if (isPlaying()) return              ;
  ui -> Play      -> setEnabled (true) ;
  ui -> Clear     -> setEnabled (true) ;
  ui -> Delete    -> setEnabled (true) ;
  ui -> Next      -> setEnabled (true) ;
  ui -> Export    -> setEnabled (true) ;
  ui -> TimeTable -> setEnabled (true) ;
  ui -> Database  -> setEnabled (true) ;
  ui -> Analysis  -> setEnabled (true) ;
  ui -> User      -> setEnabled (true) ;
  Start ( Files[0] , Names[0] )        ;
}

void N::VideoPanel::Export(void)
{
  if (Files.count()<=0) return                      ;
  QString filename = QFileDialog::getSaveFileName   (
                       this,tr("Save play lists"  ) ,
                       plan->Path("Temp")           ,
                       "*.m3u"                    ) ;
  if (filename.length()<=0) return                  ;
  QString head = "#EXTM3U"                          ;
  QStringList m3u                                   ;
  m3u << head                                       ;
  for (int i=0;i<Files.count();i++)                 {
    m3u << QDir::toNativeSeparators ( Files [i] )   ;
  }                                                 ;
  QFile F(filename)                                 ;
  if (!F.open(QIODevice::WriteOnly)) return         ;
  for (int i=0;i<m3u.count();i++)                   {
    QString l = m3u [i]                             ;
    l = l + "\r\n"                                  ;
    F.write(l.toUtf8())                             ;
  }                                                 ;
  F . close ()                                      ;
}

void N::VideoPanel::Delete(void)
{
  int index = ui->Videos->currentIndex() ;
  if (index<0) return                    ;
  int total = Files . count()            ;
  Files.takeAt(index)                    ;
  Names.takeAt(index)                    ;
  for (int i=index+1;i<total;i++)        {
    Uuids      [i-1] = Uuids      [i]    ;
    TimeTables [i-1] = TimeTables [i]    ;
    Events     [i-1] = Events     [i]    ;
  }                                      ;
  Uuids      . remove ( total-1 )        ;
  TimeTables . remove ( total-1 )        ;
  Events     . remove ( total-1 )        ;
  ui -> Videos -> blockSignals (false)   ;
  ui -> Videos -> clear        (     )   ;
  ui -> Videos -> addItems     (Names)   ;
  ui -> Videos -> blockSignals (true )   ;
}

void N::VideoPanel::TimeTable(void)
{
}
