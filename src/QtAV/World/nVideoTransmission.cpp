#include <qtav.h>

N::VideoTransmission:: VideoTransmission (void)
{
  Format         = QImage::Format_RGB888 ;
  ThrottleSize   = 150                   ;
  TotalAudioSize =   0                   ;
  AudioFlushed   =   0                   ;
}

N::VideoTransmission::~VideoTransmission(void)
{
}

void N::VideoTransmission::Lock(void)
{
  mutex . lock ( ) ;
}

void N::VideoTransmission::Unlock(void)
{
  mutex . unlock ( ) ;
}

void N::VideoTransmission::setStatus(int id,bool status)
{
  Status [ id ] = status ;
}

void N::VideoTransmission::setTime(int total,QDateTime now)
{
  for (int i=0;i<total;i++) {
    FrameTimes [ i ] = now  ;
  }                         ;
}

void N::VideoTransmission::SituationChanged (
       QSize                 dimension      ,
       QMap<SUID,QImage *> & images         ,
       XMAPs               & pictures       )
{
  Lock   ( )                               ;
  UUIDs U = images.keys()                  ;
  SUID  u                                  ;
  foreach (u,U)                            {
    Images   [ (int) u ] = images    [ u ] ;
    Displays [ (int) u ] = &pictures [ u ] ;
  }                                        ;
  Dimension = dimension                    ;
  Unlock ( )                               ;
}

bool N::VideoTransmission::Modified(QSize dimension)
{
  if (dimension.width ()!=Dimension.width ()) return true ;
  if (dimension.height()!=Dimension.height()) return true ;
  return false                                            ;
}

bool N::VideoTransmission::hasCommand(VarArgs & command)
{
  if (Commands.count()<=0) return false ;
  Lock   ( )                            ;
  command  = Commands [ 0 ]             ;
  Commands . takeAt   ( 0 )             ;
  Unlock ( )                            ;
  return true                           ;
}

TUID N::VideoTransmission::Elapsed(void)
{
  CurrentTime = nTimeNow                    ;
  return BaseTime . msecsTo ( CurrentTime ) ;
}

TUID N::VideoTransmission::Elapsed(int At)
{
  QDateTime T = nTimeNow          ;
  if (FrameTimes.contains(At))    {
    T = FrameTimes [ At ]         ;
  }                               ;
  return BaseTime . msecsTo ( T ) ;
}
