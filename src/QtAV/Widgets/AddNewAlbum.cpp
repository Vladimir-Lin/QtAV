#include <qtav.h>
#include "ui_AddNewAlbum.h"

N::AddNewAlbum:: AddNewAlbum ( QWidget * parent , Plan * p )
               : Widget      (           parent ,        p )
               , ui          ( new Ui::AddNewAlbum         )
               , vcf         ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AddNewAlbum::~AddNewAlbum(void)
{
  delete ui ;
}

void N::AddNewAlbum::changeEvent(QEvent * e)
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

void N::AddNewAlbum::Configure(void)
{
  ui -> setupUi ( this )                     ;
  ui -> Languages << plan -> languages       ;
  setAttribute ( Qt::WA_InputMethodEnabled ) ;
}

QString N::AddNewAlbum::AlbumName(void)
{
  return ui -> Name -> text ( ) ;
}

int N::AddNewAlbum::Language(void)
{
  return (int)N::GetUuid(ui->Languages) ;
}

void N::AddNewAlbum::GainFocus(void)
{
  ui -> Name -> setFocus ( Qt::TabFocusReason ) ;
}

void N::AddNewAlbum::NameChanged(QString text)
{
  bool c = ( text . length ( ) > 0 ) ;
  ui -> Add     -> setEnabled ( c  ) ;
  ui -> Details -> setEnabled ( c  ) ;
}

void N::AddNewAlbum::AddAlbum(void)
{
  emit Add ( this ) ;
}

void N::AddNewAlbum::CancalAdd(void)
{
  emit Cancel ( this ) ;
}

void N::AddNewAlbum::EditDetails(void)
{
  emit Details ( this ) ;
}
