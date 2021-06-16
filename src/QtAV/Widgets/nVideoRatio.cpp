#include <qtav.h>
#include "ui_nVideoRatio.h"

N::VideoRatio:: VideoRatio ( QWidget * parent , Plan * p )
              : Widget     (           parent ,        p )
              , ui         ( new Ui::nVideoRatio         )
              , ThreadId   ( -1                          )
              , vcf        ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoRatio::~VideoRatio(void)
{
  delete ui ;
}

void N::VideoRatio::changeEvent(QEvent * e)
{
  QWidget::changeEvent ( e )       ;
  switch ( e -> type ( ) )         {
    case QEvent::LanguageChange    :
      ui -> retranslateUi ( this ) ;
    break                          ;
    default                        :
    break                          ;
  }
}

void N::VideoRatio::Configure(void)
{
  ui -> setupUi              ( this             )                     ;
  /////////////////////////////////////////////////////////////////////
  ui -> R -> blockSignals    ( true             )                     ;
  /////////////////////////////////////////////////////////////////////
  ui -> R -> addItem         ( tr("25%" ) ,  25 )                     ;
  ui -> R -> addItem         ( tr("50%" ) ,  50 )                     ;
  ui -> R -> addItem         ( tr("75%" ) ,  75 )                     ;
  ui -> R -> addItem         ( tr("100%") , 100 )                     ;
  ui -> R -> addItem         ( tr("125%") , 125 )                     ;
  ui -> R -> addItem         ( tr("150%") , 150 )                     ;
  ui -> R -> addItem         ( tr("175%") , 175 )                     ;
  ui -> R -> addItem         ( tr("200%") , 200 )                     ;
  ui -> R -> addItem         ( tr("250%") , 250 )                     ;
  ui -> R -> addItem         ( tr("300%") , 300 )                     ;
  ui -> R -> addItem         ( tr("400%") , 400 )                     ;
  /////////////////////////////////////////////////////////////////////
  ui -> R -> setCurrentIndex ( 3                )                     ;
  ui -> R -> blockSignals    ( false            )                     ;
  /////////////////////////////////////////////////////////////////////
  nConnect ( ui -> R -> lineEdit ( ) , SIGNAL ( returnPressed ( )   ) ,
             this                    , SLOT   ( FreeRatio     ( ) ) ) ;
}

void N::VideoRatio::setRange(QSize v,QSize w,QRect r)
{
  int ww  = w . width  ( ) / 2                         ;
  int wh  = w . height ( ) / 2                         ;
  video   = v                                          ;
  window  = w                                          ;
  current = r                                          ;
  ui -> X -> blockSignals ( true                     ) ;
  ui -> Y -> blockSignals ( true                     ) ;
  ui -> W -> blockSignals ( true                     ) ;
  ui -> H -> blockSignals ( true                     ) ;
  ui -> X -> setRange     ( -ww , ww                 ) ;
  ui -> X -> setValue     ( r . left   ( )           ) ;
  ui -> Y -> setRange     ( -wh , wh                 ) ;
  ui -> Y -> setValue     ( r . top    ( )           ) ;
  ui -> W -> setRange     ( 32 , v . width  ( ) * 32 ) ;
  ui -> W -> setValue     ( r . width  ( )           ) ;
  ui -> H -> setRange     ( 32 , v . height ( ) * 32 ) ;
  ui -> H -> setValue     ( r . height ( )           ) ;
  ui -> X -> blockSignals ( false                    ) ;
  ui -> Y -> blockSignals ( false                    ) ;
  ui -> W -> blockSignals ( false                    ) ;
  ui -> H -> blockSignals ( false                    ) ;
}

void N::VideoRatio::FreeRatio(void)
{
  if ( IsNull ( ui -> R -> lineEdit ( ) ) ) return ;
  QString r = ui -> R -> lineEdit ( ) -> text ( )  ;
  //////////////////////////////////////////////////
  if ( r . length ( ) <= 0 ) return                ;
  r = r . replace ( "%" , "" )                     ;
  r = r . replace ( " " , "" )                     ;
  bool  okay = false                               ;
  double v   = 0                                   ;
  v = r . toDouble ( &okay )                       ;
  if ( ! okay ) return                             ;
  //////////////////////////////////////////////////
  int w = ( video . width  ( ) * v ) / 100         ;
  int h = ( video . height ( ) * v ) / 100         ;
  ui -> W -> blockSignals ( true  )                ;
  ui -> H -> blockSignals ( true  )                ;
  ui -> W -> setValue     ( w     )                ;
  ui -> H -> setValue     ( h     )                ;
  ui -> W -> blockSignals ( false )                ;
  ui -> H -> blockSignals ( false )                ;
  //////////////////////////////////////////////////
  Send ( )                                         ;
}

void N::VideoRatio::Ratio(int)
{
  int r = ui -> R -> currentIndex ( )       ;
  if ( r < 0 ) return                       ;
  ///////////////////////////////////////////
  r = ui -> R -> itemData ( r ) . toInt ( ) ;
  int w = ( video . width  ( ) * r ) / 100  ;
  int h = ( video . height ( ) * r ) / 100  ;
  ui -> W -> blockSignals ( true  )         ;
  ui -> H -> blockSignals ( true  )         ;
  ui -> W -> setValue     ( w     )         ;
  ui -> H -> setValue     ( h     )         ;
  ui -> W -> blockSignals ( false )         ;
  ui -> H -> blockSignals ( false )         ;
  ///////////////////////////////////////////
  Send ( )                                  ;
}

void N::VideoRatio::Changed(int)
{
  Send ( ) ;
}

void N::VideoRatio::Width(int)
{
  if ( ui -> Aspect -> isChecked ( ) ) {
    double w = ui -> W -> value ( )    ;
    w /= video . width  ( )            ;
    w *= video . height ( )            ;
    ui -> H -> blockSignals ( true   ) ;
    ui -> H -> setValue     ( (int)w ) ;
    ui -> H -> blockSignals ( false  ) ;
  }                                    ;
  Send ( )                             ;
}

void N::VideoRatio::Height(int)
{
  if ( ui -> Aspect -> isChecked ( ) ) {
    double h = ui -> H -> value ( )    ;
    h /= video . height ( )            ;
    h *= video . width  ( )            ;
    ui -> W -> blockSignals ( true   ) ;
    ui -> W -> setValue     ( (int)h ) ;
    ui -> W -> blockSignals ( false  ) ;
  }                                    ;
  Send ( )                             ;
}

void N::VideoRatio::Close(void)
{
  emit Close ( ThreadId , this ) ;
}

void N::VideoRatio::Send(void)
{
  QRect R                                ;
  R . setX      ( ui -> X -> value ( ) ) ;
  R . setY      ( ui -> Y -> value ( ) ) ;
  R . setWidth  ( ui -> W -> value ( ) ) ;
  R . setHeight ( ui -> H -> value ( ) ) ;
  emit Changed  ( ThreadId , R         ) ;
}
