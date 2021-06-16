#include <qtav.h>

#define TABLES(ID) ScenePlan->Tables[Tables::ID]

N::CameraLists:: CameraLists  ( QWidget * parent,Plan * p )
               : ListDock     (           parent,       p )
               , SceneManager (                         p )
{
  Configure ( ) ;
}

N::CameraLists::~CameraLists(void)
{
}

void N::CameraLists::Configure(void)
{
  setWindowTitle               ( tr("Camera lists")   ) ;
  setViewMode                  (IconMode              ) ;
  setIconSize                  (QSize(128,128)        ) ;
  setGridSize                  (QSize(160,192)        ) ;
  setMovement                  (Snap                  ) ;
  setResizeMode                (Adjust                ) ;
  setSelectionMode             (SingleSelection       ) ;
  setWordWrap                  (true                  ) ;
  setMouseTracking             (true                  ) ;
  setWrapping                  (true                  ) ;
  setTextElideMode             (Qt::ElideNone         ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOff) ;
  setMinimumSize               (QSize(128,200)        ) ;
  setDragDropMode              (DragOnly              ) ;
  plan -> setFont              (this                  ) ;
  MountCommands                (                      ) ;
}

bool N::CameraLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , List        ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  return true                                          ;
}

QMimeData * N::CameraLists::dragMime(void)
{
  QListWidgetItem * IT = currentItem()            ;
  if (IsNull(IT)) return NULL                     ;
  SUID  uuid  = nListUuid(IT)                     ;
  QMimeData * mime = new QMimeData()              ;
  setMime ( mime , "camera/uuid"  , uuid )        ;
  if (NotNull(IT))                                {
    QIcon icon  = IT->icon()                      ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
  }                                               ;
  return mime                                     ;
}

bool N::CameraLists::hasItem(void)
{
  QListWidgetItem * item = currentItem() ;
  return NotNull ( item )                ;
}

bool N::CameraLists::startDrag(QMouseEvent * event)
{
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return false                                               ;
}

bool N::CameraLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::CameraLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::CameraLists::finishDrag(QMouseEvent * event)
{
  return true   ;
}

void N::CameraLists::List(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  clear ( )                                          ;
  PictureManager PM(plan)                            ;
  EnterSQL ( SC , plan->sql )                        ;
    QString Q                                        ;
    SUID    uuid                                     ;
    UUIDs   Uuids = Cameras(SC)                      ;
    foreach (uuid,Uuids)                             {
      QImage  image                                  ;
      QIcon   icon                                   ;
      QString name = Name(SC,uuid,Plan->LanguageId)  ;
      QString identifier                             ;
      Q = SC.sql.SelectFrom("snap,name"              ,
            TABLES(Cameras) , SC.WhereUuid(uuid)   ) ;
      if (SC.Fetch(Q))                               {
        identifier      = SC.String(1)               ;
        QByteArray cpng = SC.ByteArray(0)            ;
        QByteArray png  = qUncompress(cpng)          ;
        image = QImage::fromData(png,"PNG")          ;
        icon  = PM.Icon(&image,QSize(128,128))       ;
      }                                              ;
      addItem ( uuid , icon , name , identifier )    ;
    }                                                ;
  LeaveSQL ( SC , plan->sql )                        ;
#endif
  Alert    ( Done           )                        ;
}

void N::CameraLists::Insert(void)
{
  emit Append ( this ) ;
}

void N::CameraLists::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

bool N::CameraLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                    ;
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())     ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())     ;
  QPoint          global = mapToGlobal(pos)                    ;
  QAction       * a                                            ;
  mm.add(101,tr("Reflush"))                                    ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  a = mm.exec(global)                                          ;
  if (IsNull(a)) return true                                   ;
  switch (mm[a])                                               {
    case 101                                                   :
      List()                                                   ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi (this,Qt::Vertical)                       ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::LeftDockWidgetArea                                 ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea       ) ;
    break                                                      ;
    default                                                    :
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
