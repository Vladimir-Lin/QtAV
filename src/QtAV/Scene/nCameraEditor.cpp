#include <qtav.h>
#include "ui_nCameraEditor.h"

#define TABLES(ID) ScenePlan->Tables[Tables::ID]

N::CameraEditor:: CameraEditor ( QWidget * parent , Plan * p )
                : Widget       (           parent ,        p )
                , AttachDock   (                           p )
                , SceneManager (                           p )
                , ui           ( new Ui::nCameraEditor       )
                , camera       ( NULL                        )
                , GL           ( NULL                        )
                , matrix       ( NULL                        )
{
  Configure ( ) ;
}

N::CameraEditor::~CameraEditor(void)
{
  delete ui;
}

void N::CameraEditor::Configure(void)
{
  ui -> setupUi ( this )                                          ;
  /////////////////////////////////////////////////////////////////
  ui -> Type -> addItem(tr("Reality"     ),Camera::Reality     ) ;
  ui -> Type -> addItem(tr("Perfect"     ),Camera::Perfect     ) ;
  ui -> Type -> addItem(tr("Perspective" ),Camera::Perspective ) ;
  ui -> Type -> addItem(tr("Orthographic"),Camera::Orthographic) ;
  ui -> Type -> addItem(tr("Frustum"     ),Camera::Frustum     ) ;
  ui -> Type -> addItem(tr("Matrix"      ),Camera::NodeMatrix  ) ;
  /////////////////////////////////////////////////////////////////
  matrix = new MatrixEditor (this,plan)                          ;
  Relocation                (         )                          ;
}

void N::CameraEditor::Docking(QMainWindow * Main,QString title,Qt::DockWidgetArea area,Qt::DockWidgetAreas areas)
{
  AttachDock::Docking(Main,this,title,area,areas) ;
  QObject::connect(Dock,SIGNAL(visibilityChanged(bool)),this,SLOT(Visible(bool)));
}

void N::CameraEditor::Visible(bool visible)
{
  Visiblity(visible) ;
}

void N::CameraEditor::DockIn(bool shown)
{
  Show(shown);
}

void N::CameraEditor::Relocation(void)
{
  if (IsNull(matrix)) return ;
  matrix  ->move   (  0,280) ;
  matrix  ->resize (256,256) ;
}

void N::CameraEditor::Attach(QGLWidget * gl)
{
  GL = gl             ;
  setEnabled ( true ) ;
  matrix->matrix = &camera->matrix ;
  Reflush() ;
}

void N::CameraEditor::Attach(Camera * c)
{
  camera = c;
}

void N::CameraEditor::Detach(void)
{
  setEnabled ( false ) ;
}

void N::CameraEditor::Reflush(void)
{
  QTableWidgetItem * item                       ;
  matrix   -> startup  (                      ) ;
  ///////////////////////////////////////////////
  ui -> Name       -> blockSignals ( true  )    ;
  ui -> Identifier -> blockSignals ( true  )    ;
  ui -> Vectors    -> blockSignals ( true  )    ;
  ui -> FOV        -> blockSignals ( true  )    ;
  ui -> Near       -> blockSignals ( true  )    ;
  ui -> Far        -> blockSignals ( true  )    ;
  ui -> Type       -> blockSignals ( true  )    ;
  ///////////////////////////////////////////////
  ui->Identifier->setText ( ""             )    ;
  ui->Name -> setText  ( camera->name      )    ;
  ui->FOV  -> setValue ( camera->FOV       )    ;
  ui->Near -> setValue ( camera->minimum.z )    ;
  ui->Far  -> setValue ( camera->maximum.z )    ;
  *(ui->Type) <= camera->mode                   ;
  ///////////////////////////////////////////////
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->eye.x)) ;
  ui->Vectors->setItem(0,0,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->eye.y)) ;
  ui->Vectors->setItem(0,1,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->eye.z)) ;
  ui->Vectors->setItem(0,2,item)                ;
  ///////////////////////////////////////////////
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->center.x)) ;
  ui->Vectors->setItem(1,0,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->center.y)) ;
  ui->Vectors->setItem(1,1,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->center.z)) ;
  ui->Vectors->setItem(1,2,item)                ;
  ///////////////////////////////////////////////
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->up.x))  ;
  ui->Vectors->setItem(2,0,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->up.y))  ;
  ui->Vectors->setItem(2,1,item)                ;
  item = new QTableWidgetItem()                 ;
  item->setTextAlignment(Qt::AlignRight         |
                         Qt::AlignVCenter     ) ;
  item->setText(QString::number(camera->up.z))  ;
  ui->Vectors->setItem(2,2,item)                ;
  ///////////////////////////////////////////////
  ui -> Name       -> blockSignals ( false )    ;
  ui -> Identifier -> blockSignals ( false )    ;
  ui -> Vectors    -> blockSignals ( false )    ;
  ui -> FOV        -> blockSignals ( false )    ;
  ui -> Near       -> blockSignals ( false )    ;
  ui -> Far        -> blockSignals ( false )    ;
  ui -> Type       -> blockSignals ( false )    ;
}

void N::CameraEditor::cameraState(double)
{
  cameraChanged       ( ) ;
  plan->processEvents ( ) ;
  Reflush             ( ) ;
}

void N::CameraEditor::cameraCell(int,int)
{
  cameraChanged       ( ) ;
  plan->processEvents ( ) ;
  Reflush             ( ) ;
}

double N::CameraEditor::doubleAt(int row,int column)
{
  QTableWidgetItem * item = ui->Vectors->item(row,column) ;
  QString v = item->text() ;
  return v.toDouble() ;
}

void N::CameraEditor::cameraChanged(void)
{
  camera->mode      = ui->Type->itemData(ui->Type->currentIndex()).toInt() ;
  camera->FOV       = ui->FOV ->value() ;
  camera->minimum.z = ui->Near->value() ;
  camera->maximum.z = ui->Far ->value() ;
  ///////////////////////////////////////
  camera->eye   .x = doubleAt(0,0)      ;
  camera->eye   .y = doubleAt(0,1)      ;
  camera->eye   .z = doubleAt(0,2)      ;
  camera->center.x = doubleAt(1,0)      ;
  camera->center.y = doubleAt(1,1)      ;
  camera->center.z = doubleAt(1,2)      ;
  camera->up    .x = doubleAt(2,0)      ;
  camera->up    .y = doubleAt(2,1)      ;
  camera->up    .z = doubleAt(2,2)      ;
  ///////////////////////////////////////
  GL->updateGL()                        ;
}

void N::CameraEditor::nameChanged(QString text)
{
  bool    saveit = true                   ;
  QString n      = ui->Name      ->text() ;
  QString i      = ui->Identifier->text() ;
  saveit = (n.length()>0)                 ;
  ui -> Save -> setEnabled ( saveit )     ;
  camera->name = n                        ;
  GL->setWindowTitle(n)                   ;
}

void N::CameraEditor::Save(void)
{
  QImage  I = GL->grabFrameBuffer  ( false ) ;
  QString i = ui->Identifier->text (       ) ;
  EnterSQL         ( SC , plan->sql        ) ;
    updateCamera   ( SC , *camera , I , i  ) ;
  LeaveSQL         ( SC , plan->sql        ) ;
  Alert            ( Done                  ) ;
}
