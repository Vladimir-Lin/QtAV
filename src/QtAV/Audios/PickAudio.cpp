#include <qtav.h>

//#@ dependency : N::TreeWidget , CiosAudio::Core

N::PickAudio:: PickAudio  ( QWidget * parent , Plan * p )
             : TreeWidget (           parent ,        p )
             , DefaultId  ( -1                          )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PickAudio::~PickAudio(void)
{
}

void N::PickAudio::Configure(void)
{
  setRootIsDecorated      ( false )        ;
  setAlternatingRowColors ( true  )        ;
  Icons [ 0 ] = QIcon(":/icons/empty.png") ;
  Icons [ 1 ] = QIcon(":/icons/pick.png" ) ;
  Notify      = (SUID)N::Command::Click    ;
}

void N::PickAudio::List(Acoustics::Equipment equipment)
{
  NAMEs names = Acoustics::Devices(equipment) ;
  UUIDs uuids = names . keys ( )              ;
  SUID  suid                                  ;
  /////////////////////////////////////////////
  clear ( )                                   ;
  foreach (suid,uuids)                        {
    NewTreeWidgetItem ( IT )                  ;
    IT -> setData ( 0 , Qt::UserRole , suid ) ;
    IT -> setText ( 0 , names[suid]         ) ;
    if ( suid == (SUID)DefaultId )            {
      IT -> setIcon ( 0 , Icons[1] )          ;
    } else                                    {
      IT -> setIcon ( 0 , Icons[0] )          ;
    }                                         ;
    addTopLevelItem ( IT           )          ;
  }                                           ;
  /////////////////////////////////////////////
  QStringList L                               ;
  switch ( equipment )                        {
    case Acoustics::Input                     :
      L << tr ( "Audio input device"  )       ;
    break                                     ;
    case Acoustics::Output                    :
      L << tr ( "Audio output device" )       ;
    break                                     ;
    case Acoustics::Camera                    :
      L << tr ( "Camera input device" )       ;
    break                                     ;
  }                                           ;
  if ( L . count ( ) > 0 )                    {
    assignHeaderItems ( L       )             ;
    setWindowTitle    ( L [ 0 ] )             ;
  }                                           ;
}

void N::PickAudio::setDefault(int deviceId)
{
  DefaultId = deviceId ;
}

void N::PickAudio::setClickable(int click)
{
  disconnect ( this , SIGNAL(itemClicked      (QTreeWidgetItem*,int) )       ,
               NULL , NULL                                           )       ;
  disconnect ( this , SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int) )       ,
               NULL , NULL                                           )       ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( click )                                                           {
    case 1                                                                   :
      connect ( this , SIGNAL(itemClicked      (QTreeWidgetItem*,int))       ,
                this , SLOT  (deviceClicked    (QTreeWidgetItem*,int))     ) ;
    break                                                                    ;
    case 2                                                                   :
      connect ( this , SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))       ,
                this , SLOT  (deviceClicked    (QTreeWidgetItem*,int))     ) ;
    break                                                                    ;
  }
}

void N::PickAudio::setIcon(bool enable,QIcon icon)
{
  int id = enable ? 1 :  0 ;
  Icons [ id ] = icon      ;
}

void N::PickAudio::setNotify(SUID notify)
{
  Notify = notify ;
}

void N::PickAudio::deviceClicked (QTreeWidgetItem * item,int column)
{
  if ( DefaultId >= 0 )                                  {
    bool got = false                                     ;
    for (int i=0;!got && i < topLevelItemCount() ; i++ ) {
      QTreeWidgetItem * it = topLevelItem ( i      )     ;
      SUID              uu = nTreeUuid    ( it , 0 )     ;
      if ( uu == (SUID)DefaultId )                       {
        got = true                                       ;
        it -> setIcon ( 0 , Icons [ 0 ] )                ;
      }                                                  ;
    }                                                    ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  DefaultId = (int) nTreeUuid ( item , 0 )               ;
  item -> setIcon ( 0 , Icons[1] )                       ;
  ////////////////////////////////////////////////////////
  emit DeviceChanged ( DefaultId )                       ;
  nIfSafe ( plan ) plan -> Play ( Notify )               ;
}

QString N::PickAudio::deviceName(void)
{
  QString name = ""                                      ;
  if ( DefaultId >= 0 )                                  {
    bool got = false                                     ;
    for (int i=0;!got && i < topLevelItemCount() ; i++ ) {
      QTreeWidgetItem * it = topLevelItem ( i      )     ;
      SUID              uu = nTreeUuid    ( it , 0 )     ;
      if ( uu == (SUID)DefaultId )                       {
        got  = true                                      ;
        name = it -> text ( 0 )                          ;
      }                                                  ;
    }                                                    ;
  }                                                      ;
  return name                                            ;
}
