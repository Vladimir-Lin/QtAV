#include <qtav.h>

#ifdef QT_OPENGL_LIB

N::AlbumShelf:: AlbumShelf ( QObject * parent , Plan * p )
              : ViewShelf  (           parent ,        p )
{
  Configure ( ) ;
}

N::AlbumShelf:: AlbumShelf ( QObject * parent , Plan * p , SceneDestructor * des )
              : ViewShelf  (           parent ,        p ,                   des )
{
  Configure ( ) ;
}

N::AlbumShelf::~AlbumShelf (void)
{
}

void N::AlbumShelf::Configure(void)
{
  QObject::setProperty ( "Type"             , "AlbumShelf"           ) ;
  setVariable          ( "Type"             , "AlbumShelf"           ) ;
  setVariable          ( "Menu"             , true                   ) ;
  //////////////////////////////////////////////////////////////////////
  setVariable          ( "Access"           , "Full"                 ) ;
  setVariable          ( "T1"               , Types::Album           ) ;
  setVariable          ( "T2"               , Types::Album           ) ;
  setVariable          ( "T3"               , Types::Album           ) ;
  setVariable          ( "T4"               , Types::Division        ) ;
  setVariable          ( "T5"               , Types::Album           ) ;
  setVariable          ( "Sorting"          , Qt::DescendingOrder    ) ;
  setVariable          ( "ByItem"           , "id"                   ) ;
  setVariable          ( "Table"            , PlanTable(VideoGroups) ) ;
  setVariable          ( "AutoRefresh"      , false                  ) ;
  setVariable          ( "ReplaceIcon"      , ":/images/videos.png"  ) ;
  setVariable          ( "ArrangeBy"        , "Icon"                 ) ;
  setVariable          ( "ScrollBy"         , "Current"              ) ;
  //////////////////////////////////////////////////////////////////////
  setVariable          ( "Editable"         , true                   ) ;
//  setVariable          ( "FollowWindow"     , true                   ) ;
  //////////////////////////////////////////////////////////////////////
  setVariable          ( "NewGroup"         , tr("New album group")  ) ;
  setVariable          ( "ItemCount"        , tr("%1 albums")        ) ;
  //////////////////////////////////////////////////////////////////////
  setDropFlag          ( DropText           , false                  ) ;
  setDropFlag          ( DropUrls           , false                  ) ;
  setDropFlag          ( DropImage          , false                  ) ;
  setDropFlag          ( DropHtml           , false                  ) ;
  setDropFlag          ( DropColor          , false                  ) ;
  setDropFlag          ( DropTag            , false                  ) ;
  setDropFlag          ( DropPicture        , true                   ) ;
  setDropFlag          ( DropPeople         , false                  ) ;
  setDropFlag          ( DropVideo          , true                   ) ;
  setDropFlag          ( DropAlbum          , false                  ) ;
  setDropFlag          ( DropGender         , false                  ) ;
  setDropFlag          ( DropDivision       , true                   ) ;
}

bool N::AlbumShelf::FocusIn(void)
{
  if ( IsNull ( plan ) ) return false  ;
  LinkAction ( Insert , NewAlbum ( ) ) ;
  return true                          ;
}

bool N::AlbumShelf::hasItem (
       ControlPoint & From    ,
       ControlPoint & To      ,
       ControlPoint & At      )
{
  UUIDs U = SelectedItems ( )  ;
  return ( U . count ( ) > 0 ) ;
}

QMimeData * N::AlbumShelf::dragMime (
              ControlPoint & From     ,
              ControlPoint & To       ,
              ControlPoint & At       )
{
  UUIDs        U    = SelectedItems ( )           ;
  QMimeData  * mime = new QMimeData ( )           ;
  if ( 1 == U . count ( ) )                       {
    SUID       t    = U [ 0 ]                     ;
    QByteArray data = CreateByteArray ( t )       ;
    mime -> setData ( "division/uuid"  , data )   ;
  } else                                          {
    QByteArray data = CreateByteArray ( U )       ;
    mime -> setData ( "division/uuids" , data )   ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr ( "%1 albums selected"             )
            .arg ( U . count ( )                ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

bool N::AlbumShelf::dropAlbums (
       QWidget     * widget    ,
       ControlPoint  pos       ,
       const UUIDs & Uuids     )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 31 ) , args )       ;
  return true                                        ;
}

bool N::AlbumShelf::dropPictures (
       QWidget     * widget      ,
       ControlPoint  pos         ,
       const UUIDs & Uuids       )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 25 ) , args )       ;
  return true                                        ;
}

bool N::AlbumShelf::dropDivisions (
       QWidget     * source       ,
       ControlPoint  psf          ,
       const UUIDs & Uuids        )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 32 ) , args )       ;
  return true                                        ;
}

// PickView::ItemRunner    => 10001, 10002,  10003, 10004, 10011, 73002, 98011, 98012, 851226, 851227, 851228, 851229, 851230, 851231, 851232
// CrowdsShelf::ItemRunner => 73007, 73008, 851234, 851235
bool N::AlbumShelf::ItemRunner(int type,ThreadData * data)
{
  if ( PickView::ItemRunner ( type , data ) ) return true ;
  switch ( type )                                         {
    case 73007                                            :
      AlbumConstraints   ( data -> Arguments )            ;
    return true                                           ;
    case 73008                                            :
      AlbumRules         ( data -> Arguments )            ;
    return true                                           ;
    case CiosMenuId      ( 85 , 12 , 34      )            :
      AppendAlbum        ( data -> Arguments )            ;
    return true                                           ;
    case CiosMenuId      ( 85 , 12 , 35      )            :
      MoveAlbums         ( data -> Arguments )            ;
    return true                                           ;
  }                                                       ;
  return false                                            ;
}

void N::AlbumShelf::AppendAlbum(VarArgs & args)
{
  GroupItems    GI ( plan        )                                ;
  SqlConnection SC ( plan -> sql )                                ;
  QString       name = Value ( "NewGroup" ) . toString ( )        ;
  SUID          u    = 0                                          ;
  if ( SC . open ( FunctionString ) )                             {
    u = GI . NewDivision ( SC                                     ,
                           name                                   ,
                           (Types::ObjectTypes) GroupType ( 3 ) ) ;
    SC . insertUuid ( PlanTable(VideoGroups) , u , "uuid" )       ;
    SC . close  ( )                                               ;
  }                                                               ;
  SC   . remove ( )                                               ;
  /////////////////////////////////////////////////////////////////
  if ( u <= 0 )                                                   {
    Alert ( Error )                                               ;
    return                                                        ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  start ( 10011 )                                                 ;
}

void N::AlbumShelf::MoveAlbums(VarArgs & args)
{
  UUIDs U                                       ;
  SUID  g = args . first ( ) . toULongLong ( )  ;
  args . takeFirst ( )                          ;
  for (int i = 0 ; i < args . count ( ) ; i++ ) {
    U << args [ i ] . toULongLong ( )           ;
  }                                             ;
  if ( U . count ( ) <= 0 )                     {
    Alert ( Error )                             ;
    return                                      ;
  }                                             ;
  Alert ( Action )                              ;
  ///////////////////////////////////////////////
  UUIDs X = RealItems                           ;
  UUIDs J                                       ;
  RMAPs P                                       ;
  SUID  u                                       ;
  foreach ( u , U )                             {
    if ( X . contains ( u ) )                   {
      int ix = X . indexOf ( u )                ;
      if ( ix >= 0 )                            {
        X . takeAt ( ix )                       ;
        J << u                                  ;
      }                                         ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  if ( g <= 0 )                                 {
    X << J                                      ;
  } else                                        {
    int ix = X . indexOf ( g )                  ;
    if ( ix < 0 ) ix = 0                        ;
    foreach ( u , J )                           {
      X . insert ( ix , u )                     ;
      ix ++                                     ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  if ( Qt::AscendingOrder  == SortingType ( ) ) {
    int Id = 1                                  ;
    foreach ( u , X )                           {
      P [ u ] = Id                              ;
      Id ++                                     ;
    }                                           ;
  } else
  if ( Qt::DescendingOrder == SortingType ( ) ) {
    int Id = X .count ( )                       ;
    foreach ( u , X )                           {
      P [ u ] = Id                              ;
      Id --                                     ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )              ;
  if ( SC . open ( FunctionString )           ) {
    QString Q                                   ;
    foreach ( u , X )                           {
      int Id = P [ u ]                          ;
      Q = SC . sql . Update                     (
            PlanTable ( Divisions )             ,
            SC . WhereUuid ( u )                ,
            1                                   ,
            "position"                        ) ;
      SC . Prepare ( Q               )          ;
      SC . Bind    ( "position" , Id )          ;
      SC . Exec    (                 )          ;
    }                                           ;
    foreach ( u , X )                           {
      int Id = P [ u ]                          ;
      Q = SC . sql . Update                     (
            PlanTable(VideoGroups)              ,
            SC . WhereUuid ( u )                ,
            1                                   ,
            "id"                              ) ;
      SC . Prepare ( Q          )               ;
      SC . Bind    ( "id" , -Id )               ;
      SC . Exec    (            )               ;
    }                                           ;
    foreach ( u , X )                           {
      int Id = P [ u ]                          ;
      Q = SC . sql . Update                     (
            PlanTable(VideoGroups)              ,
            SC . WhereUuid ( u )                ,
            1                                   ,
            "id"                              ) ;
      SC . Prepare ( Q          )               ;
      SC . Bind    ( "id" ,  Id )               ;
      SC . Exec    (            )               ;
    }                                           ;
    SC . close   ( )                            ;
  }                                             ;
  SC   . remove  ( )                            ;
  ///////////////////////////////////////////////
  RealItems = X                                 ;
  J . clear ( )                                 ;
  foreach ( u , X ) J << RealToView [ u ]       ;
  LockAt   ( "Operate" )                        ;
  CurrentItems = J                              ;
  UnlockAt ( "Operate" )                        ;
  ///////////////////////////////////////////////
  BoxChanged ( true )                           ;
  Alert      ( Done )                           ;
}

void N::AlbumShelf::AlbumConstraints(VarArgs & args)
{
  if ( args . count ( ) <  2 ) return         ;
  /////////////////////////////////////////////
  SUID nuid = args [ 0 ] . toULongLong ( )    ;
  SUID uuid = args [ 1 ] . toULongLong ( )    ;
  /////////////////////////////////////////////
  if ( nuid             <= 0 ) return         ;
  if ( uuid             <= 0 ) return         ;
  /////////////////////////////////////////////
#pragma message("Album constraints are required to be completed")
}

void N::AlbumShelf::AlbumRules(VarArgs & args)
{
  if ( args . count ( ) <  2 ) return         ;
  /////////////////////////////////////////////
  SUID nuid = args [ 0 ] . toULongLong ( )    ;
  SUID uuid = args [ 1 ] . toULongLong ( )    ;
  /////////////////////////////////////////////
  if ( nuid             <= 0 ) return         ;
  if ( uuid             <= 0 ) return         ;
  /////////////////////////////////////////////
#pragma message("Album rule tables are required to be completed")
}

void N::AlbumShelf::atDoubleClicked(SelectItem * item)
{
  PushCommand ( CiosMenuId ( 89 , 44 , 67 ) , item ) ;
}

int N::AlbumShelf::Indexing(RangeBar * rbu,int direction,QVariants & v)
{
//  qint64 s = v [ 0 ] . toLongLong ( )     ; // start
//  qint64 e = v [ 1 ] . toLongLong ( )     ; // end
//  qint64 p = v [ 2 ] . toLongLong ( )     ; // page
  qint64 i = v [ 3 ] . toLongLong ( )     ; // index
  UUIDs  U = CurrentItems                 ;
  setVariable  ( "CurrentPageIndex" , i ) ;
  PickItem::ArrangeItems ( i , U )        ;
  return 0                                ;
}

bool N::AlbumShelf::Relocation(QRectF viewport)
{
  return CentralRelocation ( viewport ) ;
}

void N::AlbumShelf::AllAlbums(void)
{
  emit ShowAll ( ) ;
}

void N::AlbumShelf::NewAlbum(void)
{
  start ( CiosMenuId ( 85 , 12 , 34 ) ) ;
}

void N::AlbumShelf::SelectNothing(void)
{
  UUIDs U = CurrentItems                          ;
  SUID  u                                         ;
  foreach ( u , U )                               {
    SelectItem * item = ChildOf<SelectItem> ( u ) ;
    if ( NotNull ( item ) )                       {
      item -> setChecked ( false )                ;
    }                                             ;
  }                                               ;
  setVariable ( "CurrentNode" , 0 )               ;
}

void N::AlbumShelf::SelectAll(void)
{
  UUIDs U = CurrentItems                          ;
  SUID  u                                         ;
  foreach ( u , U )                               {
    SelectItem * item = ChildOf<SelectItem> ( u ) ;
    if ( NotNull ( item ) )                       {
      item -> setChecked ( true )                 ;
    }                                             ;
  }                                               ;
}

bool N::AlbumShelf::Menu   (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  AllowPickItemMenu                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager  mm ( widget )                                                 ;
  QAction    * aa                                                            ;
  QMenu      * ma                                                            ;
  QMenu      * ms                                                            ;
  QMenu      * md                                                            ;
  QMenu      * me                                                            ;
  QMenu      * mc                                                            ;
  SelectItem * item = NearestItem ( From , To )                              ;
  QLineEdit  * line = NULL                                                   ;
  QLineEdit  * utem = NULL                                                   ;
  VarArgs      args                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull ( item ) ) item = CurrentItem ( )                              ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Editable" ) )                                          {
    mm      . add          ( CiosMenuId(89,44,14) , tr("Open album group") ) ;
    mm      . add          ( CiosMenuId(89,44,11) , tr("All albums"     )  ) ;
    mm      . add          ( CiosMenuId(89,44,12) , tr("Refresh"        )  ) ;
    mm      . addSeparator (                                               ) ;
    mm      . add          ( CiosMenuId(89,44,13) , tr("New album group")  ) ;
    mm      . addSeparator (                                               ) ;
    me = mm . addMenu      ( tr("Edit")                                    ) ;
    mm      . add          ( me                                              ,
                             CiosMenuId(89,44,41)                            ,
                             tr("Count albums in all groups")              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Selectable" ) && NotNull ( item ) )                    {
    md = mm . addMenu      ( tr("Details")                                 ) ;
    mc = mm . addMenu      ( tr("Categorize")                              ) ;
    NameEditor             ( mm , md , item , &line                        ) ;
    MountUuid              ( mm , md , item                                ) ;
    mm      . addSeparator ( md                                            ) ;
    mm      . addSeparator (                                               ) ;
    //////////////////////////////////////////////////////////////////////////
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,51)                            ,
                             tr("Count albums in this group")              ) ;
    mm      . addSeparator ( md                                            ) ;
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,71)                            ,
                             tr("Delete this group")                       ) ;
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,72)                            ,
                             tr("Delete all albums in this group")         ) ;
    //////////////////////////////////////////////////////////////////////////
    mm      . add          ( mc , CiosMenuId(89,44,81) , tr("Constraints") ) ;
    mm      . add          ( mc , CiosMenuId(89,44,82) , tr("Rule tables") ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ma   = mm . addMenu  ( tr("Adjustments")    )                              ;
  ms   = mm . addMenu  ( tr("Sorting")        )                              ;
  AdjustMenu           ( mm , ma              )                              ;
  SortingMenu          ( mm , ms              )                              ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator    (                      )                              ;
  mm . add             ( CiosMenuId(89,44,99)                                ,
                         tr("Close album groups explorer")                 ) ;
  ////////////////////////////////////////////////////////////////////////////
  RunPickItemMenu                                                            ;
  if ( IsNull ( aa ) )                                                       {
    ModifyName ( item , line )                                               ;
    return true                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( AdjustProcessor  ( mm , aa        ) ) return true                     ;
  if ( SortingProcessor ( mm , aa        ) ) return true                     ;
  if ( 0 != CallToMenu  ( mm , aa , true ) ) return true                     ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId    ( 89 , 44 , 99 )                                      : // Close this album groups explorer
      emit CloseObject ( node         )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 11 )                                      : // View all albums
      AllAlbums        (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 12 )                                      : // Refresh
      startup          (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 13 )                                      : // New album group
      start            ( CiosMenuId ( 85 , 12 , 34 ) )                       ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 14 )                                      : // Open album group
      if ( NotNull ( item ) )                                                {
        emit DoubleClicked ( item )                                          ;
        atDoubleClicked    ( item )                                          ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 41 )                                      : // Count albums in all groups
      start            ( CiosMenuId ( 85 , 12 , 26 ) )                       ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 51 )                                      : // Count albums in this group
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 27 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 71 )                                      : // Delete this album group
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 29 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 72 )                                      : // Delete all albums in this album group
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 30 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 81 )                                      : // Album group categorize constraints
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( 73007 , args )                                      ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 82 )                                      : // Album group categorize rule tables
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( 73008 , args )                                      ;
      }                                                                      ;
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

#endif
