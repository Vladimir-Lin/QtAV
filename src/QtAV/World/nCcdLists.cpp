#include <qtav.h>

N::CcdLists:: CcdLists   ( QWidget * parent , Plan * p )
            : TreeWidget (           parent ,        p )
{
  Configure ( ) ;
}

N::CcdLists::~CcdLists(void)
{
}

void N::CcdLists::Configure(void)
{
  NewTreeWidgetItem       ( head                                ) ;
  /////////////////////////////////////////////////////////////////
  setWindowTitle          ( tr("Optical/CCD camera paraments")  ) ;
  setAccessibleName       ( "nCcdLists"                         ) ;
  /////////////////////////////////////////////////////////////////
  setColumnCount          ( 7                                   ) ;
  setRootIsDecorated      ( false                               ) ;
  setAlternatingRowColors ( true                                ) ;
  setSelectionMode        ( ExtendedSelection                   ) ;
  /////////////////////////////////////////////////////////////////
  head -> setText         ( 0 , tr( "Name"                    ) ) ;
  head -> setText         ( 1 , tr( "Identifier"              ) ) ;
  head -> setText         ( 2 , tr( "Width"                   ) ) ;
  head -> setText         ( 3 , tr( "Height"                  ) ) ;
  head -> setText         ( 4 , tr( "Field of View"           ) ) ;
  head -> setText         ( 5 , tr( "Focus point"             ) ) ;
  head -> setText         ( 6 , tr( "Equation"                ) ) ;
  for (int i=0;i<columnCount();i++)                               {
    head->setTextAlignment( i , Qt::AlignCenter                 ) ;
  }                                                               ;
  setHeaderItem           ( head                                ) ;
  setAlignment            ( 2 , Qt::AlignRight|Qt::AlignVCenter ) ;
  setAlignment            ( 3 , Qt::AlignRight|Qt::AlignVCenter ) ;
  setAlignment            ( 4 , Qt::AlignRight|Qt::AlignVCenter ) ;
  setAlignment            ( 5 , Qt::AlignRight|Qt::AlignVCenter ) ;
  /////////////////////////////////////////////////////////////////
  plan -> setFont         ( this                                ) ;
  setDragDropMode         ( NoDragDrop                          ) ;
  /////////////////////////////////////////////////////////////////
  connect (this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
           this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::CcdLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Save         , Save        ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  return true                                          ;
}

void N::CcdLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit      * le = NULL                    ;
  QSpinBox       * sb = NULL                    ;
  QDoubleSpinBox * ds = NULL                    ;
  switch (column)                               {
    case 0                                      :
      le  = setLineEdit                         (
              item                              ,
              column                            ,
              SIGNAL(editingFinished())         ,
              SLOT  (nameFinished   ())       ) ;
      le -> setFocus ( Qt::TabFocusReason     ) ;
    break                                       ;
    case 1                                      :
      le  = setLineEdit                         (
              item                              ,
              column                            ,
              SIGNAL(editingFinished())         ,
              SLOT  (identFinished  ())       ) ;
      le -> setFocus ( Qt::TabFocusReason     ) ;
    break                                       ;
    case 2                                      :
      sb  = setSpinBox                          (
              item                              ,
              column                            ,
              0                                 ,
              100000                            ,
              SIGNAL(editingFinished())         ,
              SLOT  (widthFinished  ())       ) ;
    break                                       ;
    case 3                                      :
      sb  = setSpinBox                          (
              item                              ,
              column                            ,
              0                                 ,
              100000                            ,
              SIGNAL(editingFinished())         ,
              SLOT  (heightFinished ())       ) ;
    break                                       ;
    case 4                                      :
      ds  = setDoubleSpin                       (
              item                              ,
              column                            ,
              0                                 ,
              360                               ,
              SIGNAL(editingFinished())         ,
              SLOT  (fovFinished    ())       ) ;
      ds -> setDecimals ( 6 )                   ;
      ds -> setValue (item->text(4).toDouble()) ;
    break                                       ;
    case 5                                      :
      ds  = setDoubleSpin                       (
              item                              ,
              column                            ,
              0                                 ,
              10000000000.0                     ,
              SIGNAL(editingFinished())         ,
              SLOT  (focusFinished  ())       ) ;
      ds -> setDecimals ( 6 )                   ;
      ds -> setValue (item->text(5).toDouble()) ;
    break                                       ;
    case 6                                      :
      le  = setLineEdit                         (
              item                              ,
              column                            ,
              SIGNAL(editingFinished ())        ,
              SLOT  (equationFinished())      ) ;
      le -> setFocus ( Qt::TabFocusReason     ) ;
    break                                       ;
    default                                     :
      removeOldItem ( )                         ;
    break                                       ;
  }                                             ;
}

void N::CcdLists::run(int Type,ThreadData * data)
{
  switch (Type) {
    case 10001  :
      List ( )  ;
    break       ;
  }             ;
}

bool N::CcdLists::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::CcdLists::List(void)
{
  clear    (                )                                  ;
  EnterSQL ( SC , plan->sql )                                  ;
    QString Q                                                  ;
    UUIDs   Uuids                                              ;
    SUID    uuid                                               ;
    Uuids = SC.Uuids                                           (
              PlanTable(CCD)                                   ,
              "uuid"                                           ,
              SC.OrderByAsc("id")                            ) ;
    foreach (uuid,Uuids)                                       {
      Q = SC.sql.SelectFrom                                    (
            "identifier,flags,width,height,fov,focus,equation" ,
            PlanTable(CCD)                                     ,
            SC.WhereUuid(uuid)                               ) ;
      if (SC.Fetch(Q))                                         {
        QString identifier = SC . String (0)                   ;
        int     flags      = SC . Int    (1)                   ;
        int     width      = SC . Int    (2)                   ;
        int     height     = SC . Int    (3)                   ;
        double  fov        = SC . Value  (4) . toDouble ( )    ;
        double  focus      = SC . Value  (5) . toDouble ( )    ;
        QString equation   = SC . String (6)                   ;
        QString name                                           ;
        name = SC.getName                                      (
                 PlanTable(Names)                              ,
                 "uuid"                                        ,
                 vLanguageId                                   ,
                 uuid                                        ) ;
        NewTreeWidgetItem ( IT )                               ;
        setAlignments     ( IT )                               ;
        IT->setData ( 0 , Qt::UserRole , uuid             )    ;
        IT->setData ( 1 , Qt::UserRole , flags            )    ;
        IT->setText ( 0 , name                            )    ;
        IT->setText ( 1 , identifier                      )    ;
        IT->setText ( 2 , QString::number(width )         )    ;
        IT->setText ( 3 , QString::number(height)         )    ;
        IT->setText ( 4 , QString("%1").arg(fov  ,0,'f',6))    ;
        IT->setText ( 5 , QString("%1").arg(focus,0,'f',6))    ;
        IT->setText ( 6 , equation                        )    ;
        addTopLevelItem(IT)                                    ;
      }                                                        ;
    }                                                          ;
  LeaveSQL ( SC , plan->sql )                                  ;
  emit AutoFit ( )                                             ;
  Alert ( Done )                                               ;
}

void N::CcdLists::nameFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing,0          )  ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget )  ;
  //////////////////////////////////////////////////////////
  if (IsNull(name))                                        {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QString cn = name->text()                                ;
  if (cn.length()>0)                                       {
    EnterSQL ( SC , plan->sql )                            ;
      QString Q                                            ;
      if (uuid<=0)                                         {
        uuid = SC.Unique(PlanTable(MajorUuid),"uuid",3399) ;
        SC.insertUuid                                      (
          PlanTable(MajorUuid)                             ,
          uuid                                             ,
          Types::CCD                           ) ;
        SC.insertUuid(PlanTable(CCD),uuid,"uuid")          ;
        ItemEditing->setData(0,Qt::UserRole,uuid)          ;
        ItemEditing->setText(2,"0"  )                      ;
        ItemEditing->setText(3,"0"  )                      ;
        ItemEditing->setText(4,"0.0")                      ;
        ItemEditing->setText(5,"0.0")                      ;
      }                                                    ;
      SC.assureName(PlanTable(Names),uuid,vLanguageId,cn)  ;
      ItemEditing->setText(0,cn               )            ;
    LeaveSQL ( SC , plan->sql )                            ;
  }                                                        ;
  removeOldItem (      )                                   ;
  Alert         ( Done )                                   ;
}

void N::CcdLists::identFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing,0          )  ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget )  ;
  //////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                      {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QString cn = name->text()                                ;
  if (cn.length()>0)                                       {
    EnterSQL ( SC , plan->sql )                            ;
      QString Q                                            ;
      Q = SC.sql.Update                                    (
            PlanTable(CCD)                                 ,
            SC.sql.Where(1,"uuid")                         ,
            1,"identifier"                               ) ;
      SC . Prepare ( Q                          )          ;
      SC . Bind    ( "uuid"       , uuid        )          ;
      SC . Bind    ( "identifier" , cn.toUtf8() )          ;
      SC . Exec    (                            )          ;
      ItemEditing->setText ( 1 , cn             )          ;
    LeaveSQL ( SC , plan->sql )                            ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  removeOldItem (      )                                   ;
  Alert         ( Done )                                   ;
}

void N::CcdLists::equationFinished(void)
{
  SUID        uuid = nTreeUuid ( ItemEditing,0          )  ;
  QLineEdit * name = Casting   ( QLineEdit  ,ItemWidget )  ;
  //////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                      {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QString cn = name->text()                                ;
  if (cn.length()>0)                                       {
    EnterSQL ( SC , plan->sql )                            ;
      QString Q                                            ;
      Q = SC.sql.Update                                    (
            PlanTable(CCD)                                 ,
            SC.sql.Where(1,"uuid")                         ,
            1,"equation"                                 ) ;
      SC . Prepare ( Q                        )            ;
      SC . Bind    ( "uuid"     , uuid        )            ;
      SC . Bind    ( "equation" , cn.toUtf8() )            ;
      SC . Exec    (                          )            ;
      ItemEditing->setText ( 6 , cn           )            ;
    LeaveSQL ( SC , plan->sql )                            ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  removeOldItem (      )                                   ;
  Alert         ( Done )                                   ;
}

void N::CcdLists::widthFinished(void)
{
  SUID       uuid = nTreeUuid ( ItemEditing,0         )    ;
  QSpinBox * name = Casting   ( QSpinBox  ,ItemWidget )    ;
  //////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                      {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  int width = name->value()                                ;
  EnterSQL ( SC , plan->sql )                              ;
    QString Q                                              ;
    Q = SC.sql.Update                                      (
          PlanTable(CCD)                                   ,
          SC.sql.Where(1,"uuid")                           ,
          1,"width"                                      ) ;
    SC . Prepare ( Q                )                      ;
    SC . Bind    ( "uuid"  , uuid   )                      ;
    SC . Bind    ( "width" , width  )                      ;
    SC . Exec    (                  )                      ;
    ItemEditing->setText ( 2 , QString::number(width) )    ;
  LeaveSQL ( SC , plan->sql )                              ;
  //////////////////////////////////////////////////////////
  removeOldItem (      )                                   ;
  Alert         ( Done )                                   ;
}

void N::CcdLists::heightFinished(void)
{
  SUID       uuid = nTreeUuid ( ItemEditing,0         )    ;
  QSpinBox * name = Casting   ( QSpinBox  ,ItemWidget )    ;
  //////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                      {
    removeOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  int height = name->value()                               ;
  EnterSQL ( SC , plan->sql )                              ;
    QString Q                                              ;
    Q = SC.sql.Update                                      (
          PlanTable(CCD)                                   ,
          SC.sql.Where(1,"uuid")                           ,
          1,"height"                                     ) ;
    SC . Prepare ( Q                 )                     ;
    SC . Bind    ( "uuid"   , uuid   )                     ;
    SC . Bind    ( "height" , height )                     ;
    SC . Exec    (                   )                     ;
    ItemEditing->setText ( 3 , QString::number(height) )   ;
  LeaveSQL ( SC , plan->sql )                              ;
  //////////////////////////////////////////////////////////
  removeOldItem (      )                                   ;
  Alert         ( Done )                                   ;
}

void N::CcdLists::fovFinished(void)
{
  SUID             uuid = nTreeUuid ( ItemEditing,0               ) ;
  QDoubleSpinBox * name = Casting   ( QDoubleSpinBox  ,ItemWidget ) ;
  ///////////////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                               {
    removeOldItem ( )                                               ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  double fov = name->value()                                        ;
  EnterSQL ( SC , plan->sql )                                       ;
    QString Q                                                       ;
    Q = SC.sql.Update                                               (
          PlanTable(CCD)                                            ,
          SC.sql.Where(1,"uuid")                                    ,
          1,"fov"                                                 ) ;
    SC . Prepare ( Q             )                                  ;
    SC . Bind    ( "uuid" , uuid )                                  ;
    SC . Bind    ( "fov"  , fov  )                                  ;
    SC . Exec    (               )                                  ;
    ItemEditing->setText ( 4 , QString("%1").arg(fov,0,'f',6) )     ;
    int    w  = ItemEditing->text(2).toInt   ()                     ;
    int    h  = ItemEditing->text(3).toInt   ()                     ;
    double fp = ItemEditing->text(5).toDouble()                     ;
    if (w>0 && h>0 && fov>0 && fp<=0.0001)                          {
      fp = w                                                        ;
      if (w<h) fp = h                                               ;
      fp  /= 2                                                      ;
      fp  /= ::tan( ( fov * M_PI ) / 360.0 )                        ;
      Q = SC.sql.Update                                             (
            PlanTable(CCD)                                          ,
            SC.sql.Where(1,"uuid")                                  ,
            1,"focus"                                             ) ;
      SC . Prepare ( Q               )                              ;
      SC . Bind    ( "uuid"  , uuid  )                              ;
      SC . Bind    ( "focus" , fp    )                              ;
      SC . Exec    (                 )                              ;
      ItemEditing->setText ( 5 , QString("%1").arg(fp,0,'f',6) )    ;
    }                                                               ;
  LeaveSQL ( SC , plan->sql )                                       ;
  ///////////////////////////////////////////////////////////////////
  removeOldItem (      )                                            ;
  Alert         ( Done )                                            ;
}

void N::CcdLists::focusFinished(void)
{
  SUID             uuid = nTreeUuid ( ItemEditing,0               ) ;
  QDoubleSpinBox * name = Casting   ( QDoubleSpinBox  ,ItemWidget ) ;
  ///////////////////////////////////////////////////////////////////
  if (IsNull(name) || ( uuid <= 0 ) )                               {
    removeOldItem ( )                                               ;
    return                                                          ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  double focus = name->value()                                      ;
  EnterSQL ( SC , plan->sql )                                       ;
    QString Q                                                       ;
    Q = SC.sql.Update                                               (
          PlanTable(CCD)                                            ,
          SC.sql.Where(1,"uuid")                                    ,
          1,"focus"                                               ) ;
    SC . Prepare ( Q               )                                ;
    SC . Bind    ( "uuid"  , uuid  )                                ;
    SC . Bind    ( "focus" , focus )                                ;
    SC . Exec    (                 )                                ;
    ItemEditing->setText ( 5 , QString("%1").arg(focus,0,'f',6) )   ;
  LeaveSQL ( SC , plan->sql )                                       ;
  ///////////////////////////////////////////////////////////////////
  removeOldItem (      )                                            ;
  Alert         ( Done )                                            ;
}

void N::CcdLists::Insert(void)
{
  removeOldItem     (                       ) ;
  NewTreeWidgetItem ( IT                    ) ;
  setAlignments     ( IT                    ) ;
  addTopLevelItem   ( IT                    ) ;
  scrollToItem      ( IT                    ) ;
  IT->setData       ( 0  , Qt::UserRole , 0 ) ;
  doubleClicked     ( IT , 0                ) ;
}

void N::CcdLists::Save(void)
{
  QString filename                                        ;
  filename = QFileDialog::getSaveFileName                 (
               this                                       ,
               tr("Save Optical/CCD paraments into file") ,
               plan->Temporary("")                        ,
               "*.txt\n*.*"                             ) ;
  if (filename.length()<=0) return                        ;
  QStringList L                                           ;
  for (int i=0;i<topLevelItemCount();i++)                 {
    QTreeWidgetItem * item = topLevelItem(i)              ;
    QStringList I                                         ;
    for (int j=0;j<columnCount();j++) I << item->text(j)  ;
    L << I.join("\t")                                     ;
  }                                                       ;
  QString    M = L.join("\r\n")                           ;
  QByteArray B = M.toUtf8()                               ;
  File::toFile(filename,B)                      ;
  Alert ( Done )                                          ;
}

void N::CcdLists::Export(void)
{
  QString filename                                            ;
  filename = QFileDialog::getSaveFileName                     (
               this                                           ,
               tr("Save Optical/CCD paraments into database") ,
               plan->Temporary("")                            ,
               "*.ndb\n*.*"                                 ) ;
  if (filename.length()<=0) return                            ;
  Sql LITE                                                    ;
  LITE.SqlMode = "SQLITE"                                     ;
  LITE.dbName  = filename                                     ;
  EnterSQL ( SC , plan->sql )                                 ;
    UUIDs   Uuids                                             ;
    Uuids = SC.Uuids                                          (
              PlanTable(CCD)                                  ,
              "uuid"                                          ,
              SC.OrderByAsc("id")                           ) ;
    SqlConnection LC(LITE)                                    ;
    if (LC.open("nCcdLists","SQLITE"))                        {
      QString Q                                               ;
      SUID    uuid                                            ;
      Q = "create table ccd ("
          "id integer primary key autoincrement,"
          "uuid bigint unsigned not null,"
          "identifier blob default null,"
          "width integer default 0,"
          "height integer default 0,"
          "fov double default 0,"
          "focus double default 0,"
          "equation blob default null"
          ");"                                                ;
      LC . Query ( Q )                                        ;
      foreach (uuid,Uuids)                                    {
        Q = SC.sql.SelectFrom                                 (
              "identifier,width,height,fov,focus,equation"    ,
              PlanTable(CCD)                                  ,
              SC.WhereUuid(uuid)                            ) ;
        if (SC.Fetch(Q))                                      {
          QString identifier = SC.String(0)                   ;
          int     width      = SC.Int   (1)                   ;
          int     height     = SC.Int   (2)                   ;
          double  fov        = SC.Value (3).toDouble()        ;
          double  focus      = SC.Value (4).toDouble()        ;
          QString equation   = SC.String(5)                   ;
          Q = LC.sql.InsertInto                               (
                "ccd"                                         ,
                7                                             ,
               "uuid"                                         ,
                 "identifier"                                 ,
                "width"                                       ,
                "height"                                      ,
                "fov"                                         ,
                "focus"                                       ,
                "equation"                                  ) ;
          LC . Prepare ( Q                                  ) ;
          LC . Bind    ( "uuid"       , uuid                ) ;
          LC . Bind    ( "identifier" , identifier.toUtf8() ) ;
          LC . Bind    ( "width"      , width               ) ;
          LC . Bind    ( "height"     , height              ) ;
          LC . Bind    ( "fov"        , fov                 ) ;
          LC . Bind    ( "focus"      , focus               ) ;
          LC . Bind    ( "equation"   , equation.toUtf8()   ) ;
          LC . Exec    (                                    ) ;
        }                                                     ;
      }                                                       ;
      LC.close()                                              ;
    }                                                         ;
    LC.remove()                                               ;
  LeaveSQL ( SC , plan->sql )                                 ;
  Alert ( Done )                                              ;
}

bool N::CcdLists::Menu(QPoint pos)
{
  return true ;
}
