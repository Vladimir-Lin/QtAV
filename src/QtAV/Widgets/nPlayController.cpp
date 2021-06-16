#include <qtav.h>

N::PlayController:: PlayController (QObject * parent)
{
  Paraments << "-ontop"          ;
  Paraments << "-fs"             ;
  Paraments << "-vo"             ;
  Paraments << "gl2"             ;
  Process    . setParent(parent) ;
}

N::PlayController::~PlayController ()
{
}

void N::PlayController::setPlayer(QString player)
{
  Player = QFileInfo(player) ;
}

bool N::PlayController::connectTo(QObject * parent)
{
  QObject::connect(&Process,SIGNAL(finished(int,QProcess::ExitStatus)),parent,SLOT(finished(int,QProcess::ExitStatus)));
  QObject::connect(&Process,SIGNAL(readyReadStandardOutput()),parent,SLOT(readyReadStandardOutput()));
  QObject::connect(&Process,SIGNAL(started()),parent,SLOT(started()));
  return true ;
}

void N::PlayController::Play(QString filename)
{
  QStringList fs = Paraments                                  ;
  if (Process.state()==QProcess::Running) Process.terminate() ;
  Process . setWorkingDirectory ( Player . absolutePath ()  ) ;
  fs << QDir::toNativeSeparators( filename                  ) ;
  startReport = false ;
  lastIndex   = 0                                               ;
  StdOut  . clear (                                         ) ;
  Process . start (Player.absoluteFilePath(),fs             ) ;
}

void N::PlayController::Command(QString cmd)
{
  if (Process.state()!=QProcess::Running) return;
  Process.write(cmd.toUtf8());
  Process.waitForBytesWritten(1000);
}

void N::PlayController::waitFinished(void)
{
  while (Process.state()==QProcess::Running) {
    qApp->processEvents()                    ;
    Time::msleep(100)                        ;
  }                                          ;
}

void N::PlayController::readStandardOutput(void)
{
  if (Process.bytesAvailable()<=0) return ;
  QByteArray C = Process.readAll()        ;
  Parse(C)                                ;
}

void N::PlayController::AppendStandOutput(QByteArray & STDOUT)
{
  if (STDOUT.size()==0) return                   ;
  int OINDEX = StdOut.size()                     ;
  StdOut.resize(OINDEX+STDOUT.size())            ;
  char * s = (char *)(const char *)StdOut.data() ;
  char * f = (char *)(const char *)STDOUT.data() ;
  memcpy(&s[OINDEX],f,STDOUT.size())             ;
}

bool N::PlayController::canReadLine(void)
{
  char * s  = (char *)(const char *)StdOut.data()    ;
  int TOTAL = StdOut . size ()                       ;
  if (TOTAL==lastIndex                ) return false ;
  if (strstr(&s[lastIndex],"\r")!=NULL) return true  ;
  if (strstr(&s[lastIndex],"\n")!=NULL) return true  ;
  return false                                       ;
}

QByteArray N::PlayController::readLine(void)
{
  QByteArray  L;
  int    slen = 0;
  int    tlen = StdOut.size();
  char * s    = (char *)(const char *)StdOut.data();
  char * d    = strstr(&s[lastIndex],"\r");
  char * t;
  char * k;
  if (d==NULL) d = strstr(&s[lastIndex],"\n");
  if (d==NULL) return L;
  if (lastIndex<tlen) {
    k = &s[lastIndex];
    while (k!=d) { slen++; k++; }
    if ((*k)=='\r' || (*k)=='\n') slen--;
  };
  if (slen>0) {
    L.resize(slen);
    t = (char *)(const char *)L.data();
    k = &s[lastIndex];
    for (int i=0;i<slen;i++) t[i] = k[i];
  };
  s = &s[lastIndex];
  while (s!=d) {
    lastIndex++;
    s++;
  };
  while (lastIndex<tlen &&
         ((*s)=='\r' || (*s)=='\n')) {
    lastIndex++;
    s++;
  };
  return L;
}

void N::PlayController::Progress(QByteArray Line)
{
  QString     s = QString::fromUtf8(Line).simplified() ;
  QStringList p = s.split(' ')                         ;
  Progress   (p)                                       ;
}

void N::PlayController::Progress(QStringList & paraments)
{
}

bool N::PlayController::Parse(QByteArray & Response)
{
  PlayMutex.lock() ;
  AppendStandOutput (Response) ;
  while (canReadLine()) {
    QByteArray L = readLine();
    if (L.size()>0) {
      char * l = (char *)(const char *)L.data();
      if (strncmp(l,"Starting",8)==0) {
        startReport = true;
      } else
      if (strncmp(l,"Exiting",7)==0) {
        startReport = false;
      } else
      if (startReport && strncmp(l,"A:",2)==0) {
        Progress (L) ;
      };
    };
  };
  PlayMutex.unlock() ;
  return true ;
}
