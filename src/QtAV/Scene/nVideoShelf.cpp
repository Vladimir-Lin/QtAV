#include <qtav.h>

#ifdef QT_OPENGL_LIB

N::VideoShelf:: VideoShelf ( QObject * parent , Plan * p )
              : ViewShelf  (           parent ,        p )
{
  Configure ( ) ;
}

N::VideoShelf:: VideoShelf ( QObject * parent , Plan * p , SceneDestructor * des )
              : ViewShelf  (           parent ,        p ,                   des )
{
  Configure ( ) ;
}

N::VideoShelf::~VideoShelf (void)
{
}

void N::VideoShelf::Configure(void)
{
  QObject::setProperty ( "Type"             , "VideoShelf"              ) ;
  setVariable          ( "Type"             , "VideoShelf"              ) ;
  setVariable          ( "Menu"             , true                      ) ;
  /////////////////////////////////////////////////////////////////////////
  setVariable          ( "Access"           , "First"                   ) ;
  setVariable          ( "T1"               , Types::Division           ) ;
  setVariable          ( "T2"               , Types::Album              ) ;
  setVariable          ( "T3"               , Types::Division           ) ;
  setVariable          ( "T4"               , Types::Division           ) ;
  setVariable          ( "T5"               , Types::Album              ) ;
  setVariable          ( "Sorting"          , Qt::DescendingOrder       ) ;
  setVariable          ( "Items"            , 0                         ) ;
  setVariable          ( "ByItem"           , "id"                      ) ;
  setVariable          ( "AutoRefresh"      , true                      ) ;
  setVariable          ( "ReplaceIcon"      , ":/images/videogroup.png" ) ;
  setVariable          ( "ArrangeBy"        , "Language"                ) ;
  setVariable          ( "ScrollBy"         , "Start"                   ) ;
  /////////////////////////////////////////////////////////////////////////
  setVariable          ( "Editable"         , true                      ) ;
//  setVariable          ( "FollowWindow"     , true                      ) ;
  /////////////////////////////////////////////////////////////////////////
  setDropFlag          ( DropText           , true                      ) ;
  setDropFlag          ( DropUrls           , true                      ) ;
  setDropFlag          ( DropImage          , false                     ) ;
  setDropFlag          ( DropHtml           , false                     ) ;
  setDropFlag          ( DropColor          , false                     ) ;
  setDropFlag          ( DropTag            , true                      ) ;
  setDropFlag          ( DropPicture        , true                      ) ;
  setDropFlag          ( DropPeople         , true                      ) ;
  setDropFlag          ( DropVideo          , true                      ) ;
  setDropFlag          ( DropAlbum          , true                      ) ;
  setDropFlag          ( DropGender         , false                     ) ;
  setDropFlag          ( DropDivision       , false                     ) ;
}

bool N::VideoShelf::FocusIn(void)
{
  if ( IsNull ( plan ) ) return false    ;
  return true                            ;
}

// PickView::ItemRunner     => 10001 , 10002 , 10003 , 10004 , 10011, 73002, 98011, 98012, 851226, 851227, 851228, 851229, 851230, 851231, 851232
// GalleryShelf::ItemRunner => 73007 , 73008
bool N::VideoShelf::ItemRunner(int type,ThreadData * data)
{
  if ( PickView::ItemRunner ( type , data ) ) return true                    ;
  switch ( type )                                                            {
    case CiosMenuId   ( 89 , 44 , 14      )                                  :
    return true                                                              ;
    case CiosMenuId   ( 89 , 44 , 15      )                                  :
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

void N::VideoShelf::SelectNothing(void)
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

void N::VideoShelf::SelectAll(void)
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

int N::VideoShelf::Indexing(RangeBar * rbu,int direction,QVariants & v)
{
//  qint64 s = v [ 0 ] . toLongLong ( ) ; // start
//  qint64 e = v [ 1 ] . toLongLong ( ) ; // end
//  qint64 p = v [ 2 ] . toLongLong ( ) ; // page
  qint64 i = v [ 3 ] . toLongLong ( ) ; // index
  setVariable ( "StartId" , i )       ;
  start       ( 10011         )       ;
  return 0                            ;
}

bool N::VideoShelf::Relocation(QRectF viewport)
{
  return CentralRelocation ( viewport ) ;
}

void N::VideoShelf::PlayAlbum(SelectItem * item)
{
  if ( IsNull ( item ) ) return                           ;
  SUID album = item -> Value ( "UUID" ) . toULongLong ( ) ;
  if ( album <= 0      ) return                           ;
  emit PlayAlbum ( album )                                ;
  /////////////////////////////////////////////////////////
  VarArgs args                                            ;
  args << CiosMenuId ( 99 , 40 , 11 )                     ;
  args << album                                           ;
  SceneWidget::CallBinding ( args )                       ;
}

void N::VideoShelf::AppendAlbum(void)
{
  SUID guid = FirstUuid ( )           ;
  if ( guid <= 0      ) return        ;
  emit NewAlbum ( guid )              ;
  /////////////////////////////////////
  VarArgs args                        ;
  args << CiosMenuId ( 99 , 40 , 12 ) ;
  args << guid                        ;
  SceneWidget::CallBinding ( args )   ;
}

void N::VideoShelf::NewDetailedAlbum(void)
{
  SUID guid = FirstUuid ( )           ;
  if ( guid <= 0      ) return        ;
  emit DetailAlbum ( guid )           ;
  /////////////////////////////////////
  VarArgs args                        ;
  args << CiosMenuId ( 99 , 40 , 13 ) ;
  args << guid                        ;
  SceneWidget::CallBinding ( args )   ;
}

void N::VideoShelf::AlbumMenu(MenuManager & mm,QMenu * me,SelectItem * item)
{
  if ( IsNull ( me ) ) return                                                ;
  QString title = Value ( "WindowTitle" ) . toString ( )                     ;
  QString name                                                               ;
  if ( NotNull ( item ) )                                                    {
    MountUuid           ( mm , me , item )                                   ;
    mm   . addSeparator ( me             )                                   ;
    name = item -> Value ( "ItemName" ) . toString ( )                       ;
    mm   . add ( me , CiosMenuId(89,44,41) , tr("Refresh %1").arg(name)    ) ;
    mm   . add ( me , CiosMenuId(89,44,42) , tr("%1 Details").arg(name)    ) ;
    mm   . add ( me , CiosMenuId(89,44,43) , tr("Edit %1"   ).arg(name)    ) ;
    mm   . add ( me , CiosMenuId(89,44,44) , tr("Rating %1" ).arg(name)    ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( "First" == Value ( "Access" ) . toString ( ) )                        {
    mm . addSeparator ( me )                                                 ;
    mm . add          ( me                                                   ,
                        CiosMenuId(89,44,51)                                 ,
                        tr("Set video album positions")                    ) ;
    if ( FirstUuid ( ) > 0 )                                                 {
      UUIDs U = SelectedItems ( )                                            ;
      mm   . addSeparator     ( me                                         ) ;
      mm   . add              ( me                                           ,
                                CiosMenuId(89,44,52)                         ,
                                tr("Remove from %1").arg(title)            ) ;
      if ( U . count ( ) > 0 )                                               {
        mm . add              ( me                                           ,
                                CiosMenuId(89,44,53)                         ,
                                tr("Remove selections")                    ) ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
}

void N::VideoShelf::InformationMenu(MenuManager & mm,QMenu * mi,SelectItem * item)
{
  if ( IsNull ( mi ) ) return                                                ;
  mm . add          ( mi , CiosMenuId(89,44,61) , tr("Gallery")            ) ;
  mm . add          ( mi , CiosMenuId(89,44,62) , tr("Actors")             ) ;
  mm . add          ( mi , CiosMenuId(89,44,63) , tr("Clips")              ) ;
  mm . add          ( mi , CiosMenuId(89,44,64) , tr("Torrents")           ) ;
  mm . addSeparator ( mi                                                   ) ;
  mm . add          ( mi , CiosMenuId(89,44,65) , tr("All related items")  ) ;
}

void N::VideoShelf::NetworkMenu(MenuManager & mm,QMenu * mn,SelectItem * item)
{
  if ( IsNull ( mn ) ) return                                                ;
  mm . add ( mn                                                              ,
             CiosMenuId(89,44,71)                                            ,
             plan->Icon(Types::URL,1,0)                                      ,
             tr("Bookmarks" )                                              ) ;
  mm . add ( mn , CiosMenuId(89,44,72) , tr("Search")                      ) ;
}

bool N::VideoShelf::AlbumProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  switch ( mm [ aa ] )               {
    case CiosMenuId ( 89 , 44 , 41 ) : // Refresh album
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 42 ) : // album details
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 43 ) : // Edit album
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 44 ) : // Rating album
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 51 ) : // Set video album positions
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 52 ) : // Remove from this group
      if ( NotNull ( item ) )        {
      }                              ;
    return true                      ;
    case CiosMenuId ( 89 , 44 , 53 ) : // Remove selections
      SelectNothing (              ) ;
    return true                      ;
  }                                  ;
  return false                       ;
}

bool N::VideoShelf::InformationProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                     ;
  switch ( mm [ aa ] )                                             {
    case CiosMenuId ( 89 , 44 , 61 )                               : // Gallery
      if ( NotNull ( item ) )                                      {
        emit AlbumGallery ( item )                                 ;
        PushCommand ( CiosMenuId ( 89 , 44 , 61 ) , item )         ;
      }                                                            ;
    return true                                                    ;
    case CiosMenuId ( 89 , 44 , 62 )                               : // Actors
      if ( NotNull ( item ) )                                      {
        QString n = item -> Value ( "ItemName" ) . toString    ( ) ;
        SUID    g = item -> Value ( "UUID"     ) . toULongLong ( ) ;
        emit AlbumActors ( item )                                  ;
        args << CiosMenuId ( 89 , 44 , 62 )                        ;
        args << g                                                  ;
        args << n                                                  ;
        args << GroupType    ( 2 )                                 ;
        args << GroupType    ( 3 )                                 ;
        args << GroupType    ( 5 )                                 ; // Fix the problem with actors
        args << GroupType    ( 5 )                                 ;
        args << FirstUuid    (   )                                 ;
        args << SecondUuid   (   )                                 ;
        args << RelationType (   )                                 ;
        args << node                                               ;
        args << UniverseUuid (   )                                 ;
        args << Value ( "Suicide" ) . toBool ( )                   ;
        CallRequests ( args )                                      ;
      }                                                            ;
    return true                                                    ;
    case CiosMenuId ( 89 , 44 , 63 )                               : // Clips
      if ( NotNull ( item ) )                                      {
        emit AlbumClips ( item )                                   ;
        PushCommand ( CiosMenuId ( 89 , 44 , 63 ) , item )         ;
      }                                                            ;
    return true                                                    ;
    case CiosMenuId ( 89 , 44 , 64 )                               : // Torrents
      if ( NotNull ( item ) )                                      {
        emit AlbumTorrents ( item )                                ;
        PushCommand ( CiosMenuId ( 89 , 44 , 64 ) , item )         ;
      }                                                            ;
    return true                                                    ;
    case CiosMenuId ( 89 , 44 , 65 )                               : // All related items
      if ( NotNull ( item ) )                                      {
        emit AlbumRelated ( item )                                 ;
        PushCommand ( CiosMenuId ( 89 , 44 , 65 ) , item )         ;
      }                                                            ;
    return true                                                    ;
  }                                                                ;
  return false                                                     ;
}

bool N::VideoShelf::NetworkProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  switch ( mm [ aa ] )                                     {
    case CiosMenuId ( 89 , 44 , 71 )                       : // Bookmarks
      if ( NotNull ( item ) )                              {
        emit AlbumRelated ( item )                         ;
        PushCommand ( CiosMenuId ( 89 , 44 , 71 ) , item ) ;
      }                                                    ;
    return true                                            ;
    case CiosMenuId ( 89 , 44 , 72 )                       : // Search
      if ( NotNull ( item ) )                              {
        emit AlbumRelated ( item )                         ;
        PushCommand ( CiosMenuId ( 89 , 44 , 72 ) , item ) ;
      }                                                    ;
    return true                                            ;
  }                                                        ;
  return false                                             ;
}

bool N::VideoShelf::Menu     (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  AllowPickItemMenu                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager  mm ( widget )                                                 ;
  QAction    * aa   = NULL                                                   ;
  QMenu      * ma   = NULL                                                   ;
  QMenu      * ms   = NULL                                                   ;
  QMenu      * mi   = NULL                                                   ;
  QMenu      * me   = NULL                                                   ;
  QMenu      * mn   = NULL                                                   ;
  SelectItem * item = NearestItem ( From , To )                              ;
  QLineEdit  * line = NULL                                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull  ( item ) ) item = CurrentItem ( )                             ;
  NameEditor        ( mm , NULL , item , &line )                             ;
  mm . addSeparator (                          )                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Editable" ) )                                          {
    mm . add          ( CiosMenuId(89,44,12) , tr("Play album")            ) ;
    mm . addSeparator (                                                    ) ;
    mm . add          ( CiosMenuId(89,44,13) , tr("New album")             ) ;
    mm . add          ( CiosMenuId(89,44,14) , tr("New detailed album")    ) ;
    mm . add          ( CiosMenuId(89,44,15) , tr("Reload")                ) ;
    mm . addSeparator (                                                    ) ;
    me = mm . addMenu ( tr("Edit")                                         ) ;
    mi = mm . addMenu ( tr("Information")                                  ) ;
    mn = mm . addMenu ( tr("Network")                                      ) ;
    mm . addSeparator (                                                    ) ;
    AlbumMenu         ( mm , me , item                                     ) ;
    InformationMenu   ( mm , mi , item                                     ) ;
    NetworkMenu       ( mm , mn , item                                     ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ma   = mm . addMenu ( tr("Adjustments")                                  ) ;
  ms   = mm . addMenu ( tr("Sorting")                                      ) ;
  AdjustMenu          ( mm , ma                                            ) ;
  SortingMenu         ( mm , ms                                            ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator   (                                                    ) ;
  mm . add            ( CiosMenuId(89,44,99)                                 ,
                        tr("Close album explorer")                         ) ;
  ////////////////////////////////////////////////////////////////////////////
  RunPickItemMenu                                                            ;
  if ( IsNull ( aa ) )                                                       {
    ModifyName ( item , line )                                               ;
    return true                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( AdjustProcessor      ( mm , aa        ) ) return true                 ;
  if ( SortingProcessor     ( mm , aa        ) ) return true                 ;
  if ( AlbumProcessor       ( mm , aa , item ) ) return true                 ;
  if ( InformationProcessor ( mm , aa , item ) ) return true                 ;
  if ( NetworkProcessor     ( mm , aa , item ) ) return true                 ;
  if ( 0 != CallToMenu      ( mm , aa , true ) ) return true                 ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId    ( 89 , 44 , 99 )                                      : // Close this gallery explorer
      emit CloseObject ( node         )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 12 )                                      : // Play album
      PlayAlbum        ( item         )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 13 )                                      : // New album
      AppendAlbum      (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 14 )                                      : // New detailed album
      NewDetailedAlbum (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 15 )                                      : // Refresh
      startup          (              )                                      ;
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}

#endif
