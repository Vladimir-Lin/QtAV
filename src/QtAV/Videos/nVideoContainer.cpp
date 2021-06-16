#include <qtav.h>

N::VideoContainer:: VideoContainer ( QWidget * parent , Plan * p )
                  : VideoView      (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoContainer::~VideoContainer (void)
{
}

void N::VideoContainer::Configure(void)
{
  setAccessibleName ( "N::VideoContainer" ) ;
  setObjectName     ( "N::VideoContainer" ) ;
}

bool N::VideoContainer::startup(void)
{
  return VideoView :: startup ( ) ;
}

bool N::VideoContainer::startup(UUIDs & Uuids,int sourceType)
{
  t1     = sourceType                   ;
  Albums = Uuids                        ;
  LimitValues [ 0 ] = 0                 ;
  LimitValues [ 1 ] = Albums . count( ) ;
  LimitValues [ 2 ] = Albums . count( ) ;
  View ( Albums )                       ;
  return true                           ;
}

bool N::VideoContainer::ActorMenu(QWidget *,QPoint pos)
{
  nScopedMenu ( mm , this )                                           ;
  QListWidgetItem * item = itemAt(pos)                                ;
  SUID              uuid = 0                                          ;
  QString           name = ""                                         ;
  QAction    * aa = NULL                                              ;
  QMenu      * me = NULL                                              ;
  QMenu      * mi = NULL                                              ;
  QMenu      * mn = NULL                                              ;
  if (IsNull(item))                                                   {
  } else                                                              {
    mm . add(102,LocalIcons [ "Play" ] , tr("Play"))                  ;
    uuid = item->data(Qt::UserRole).toULongLong()                     ;
    name = item->text()                                               ;
           mm.addSeparator()                                          ;
    me   = mm.addMenu(tr("Edit"))                                     ;
    mm . add(me,203,tr("%1 Details").arg(item->text()))               ;
    mm . add(me,201,tr("Edit %1"   ).arg(item->text()))               ;
    mm . add(me,202,tr("Rename %1" ).arg(item->text()))               ;
    mi   = mm.addMenu(tr("Information"))                              ;
    mm . add(mi,301,tr("Gallery"   )                                ) ;
    mm . add(mi,303,tr("Clips"     )                                ) ;
    mn   = mm.addMenu(tr("Network")                                 ) ;
    mm . add(mn,304,QIcon(":/images/bookmarks.png"),tr("Bookmarks" )) ;
    mm . add(mn,305,tr("Search"    )                                ) ;
  }                                                                   ;
  mm . addSeparator (         )                                       ;
  mm . add ( 501 , tr("Refresh")  )                                   ;
  mm . add ( 502 , LocalIcons [ "Update" ] , tr("Reload" ) )          ;
  DockingMenu      ( mm      )                                        ;
  mm . setFont     ( plan    )                                        ;
  aa = mm . exec   (         )                                        ;
  if (IsNull(aa)) return false                                        ;
  if (RunDocking(mm,aa)) return true                                  ;
  switch (mm[aa])                                                     {
    case 102                                                          :
      emit Play    ( uuid , name )                                    ;
    break                                                             ;
    case 201                                                          :
      emit Edit    ( uuid , name )                                    ;
    break                                                             ;
    case 202                                                          :
      Rename       ( item        )                                    ;
    break                                                             ;
    case 203                                                          :
      emit Details ( uuid  ,name )                                    ;
    break                                                             ;
    case 301                                                          :
      emit Gallery ( uuid , name )                                    ;
    break                                                             ;
    case 302                                                          :
      emit Actor   ( uuid , name )                                    ;
    break                                                             ;
    case 303                                                          :
      emit Clips   ( uuid , name )                                    ;
    break                                                             ;
    case 304                                                          :
      emit Bookmark( uuid , name )                                    ;
    break                                                             ;
    case 305                                                          :
      emit Search  ( uuid , name )                                    ;
    break                                                             ;
    case 501                                                          :
      Refresh      ( item        )                                    ;
    break                                                             ;
    case 502                                                          :
      startup      ( Albums      )                                    ;
    break                                                             ;
  }                                                                   ;
  return true                                                         ;
}

void N::VideoContainer::MoveAlbums(SUID atUuid,const UUIDs & Uuids)
{
  VideoView :: MoveAlbums ( atUuid , Uuids ) ;
}

void N::VideoContainer::JoinAlbums(SUID atUuid,UUIDs & Uuids)
{
  VideoView :: JoinAlbums ( atUuid , Uuids ) ;
}
