#include <qtav.h>

N::VideoBrowser:: VideoBrowser (QWidget * parent,Plan * p)
                : FileView     (          parent,       p)
{
  WidgetClass ;
}

N::VideoBrowser::~VideoBrowser (void)
{
}

QStringList N::VideoBrowser::Filters(void)
{
  QStringList vs ;
  vs << "*.avi"  ;
  vs << "*.AVI"  ;
  vs << "*.wmv"  ;
  vs << "*.WMV"  ;
  vs << "*.rmvb" ;
  vs << "*.RMVB" ;
  vs << "*.rm"   ;
  vs << "*.RM"   ;
  vs << "*.ram"  ;
  vs << "*.RAM"  ;
  vs << "*.dat"  ;
  vs << "*.DAT"  ;
  vs << "*.mp4"  ;
  vs << "*.MP4"  ;
  vs << "*.mpg"  ;
  vs << "*.MPG"  ;
  vs << "*.mpeg" ;
  vs << "*.MPEG" ;
  vs << "*.mkv"  ;
  vs << "*.MKV"  ;
  return vs      ;
}

QListWidgetItem * N::VideoBrowser::Load (QFileInfo & info)
{
  if (!info.exists()) return NULL                   ;
  QString Fn = info.absoluteFilePath()              ;
  QString su = info.suffix()                        ;
  QIcon   icon = QIcon(":/images/videos.png")       ;
  su = su.toLower()                                 ;
  if (su=="avi" ) icon = QIcon(":/images/avi.png" ) ;
  if (su=="rmvb") icon = QIcon(":/images/rmvb.png") ;
  if (su=="rm"  ) icon = QIcon(":/images/rmvb.png") ;
  if (su=="ram" ) icon = QIcon(":/images/rmvb.png") ;
  if (su=="wmv" ) icon = QIcon(":/images/wmv.png" ) ;
  if (su=="mpg" ) icon = QIcon(":/images/mpeg.png") ;
  if (su=="mpeg") icon = QIcon(":/images/mpeg.png") ;
  if (su=="mp4" ) icon = QIcon(":/images/mp4.png" ) ;
  if (su=="mkv" ) icon = QIcon(":/images/mkv.png" ) ;
  NewListWidgetItem (LWI)                           ;
  LWI->setIcon      (icon           )               ;
  LWI->setText      (info.fileName())               ;
  LWI->setToolTip   (info.fileName())               ;
  LWI->setData(Qt::UserRole,Fn.toUtf8())            ;
  return LWI                                        ;
}
