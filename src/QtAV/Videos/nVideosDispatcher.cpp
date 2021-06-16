#include <qtav.h>

typedef struct     {
  N::Plan * plan   ;
  QWidget * widget ;
} ExtraPacket      ;

N::VideosDispatcher:: VideosDispatcher ( QObject * parent )
                    : QObject          (           parent )
                    , SyntaxDispatcher (                  )
                    , UuidSyntax       (                  )
                    , plan             ( NULL             )
{
  nConnect ( this , SIGNAL ( EmitListClips  ( ) )   ,
             this , SLOT   ( SendListClips  ( ) ) ) ;
  nConnect ( this , SIGNAL ( EmitClipsGroup ( ) )   ,
             this , SLOT   ( SendClipsGroup ( ) ) ) ;
  nConnect ( this , SIGNAL ( EmitClips      ( ) )   ,
             this , SLOT   ( SendClips      ( ) ) ) ;
  nConnect ( this , SIGNAL ( EmitVideos     ( ) )   ,
             this , SLOT   ( SendVideos     ( ) ) ) ;
  nConnect ( this , SIGNAL ( EmitEditings   ( ) )   ,
             this , SLOT   ( SendEditings   ( ) ) ) ;
}

N::VideosDispatcher::~VideosDispatcher (void)
{
}

int N::VideosDispatcher::type(void) const
{
  return CiosMenuId ( 79 , 0 , 1 ) ;
}

void N::VideosDispatcher::Help(QStringList & help)
{
  help << tr("===== Videos dispatcher commands =====") ;
  help << QString("set video variable name value : %1").arg(tr("set variable into name")) ;
  help << QString("show video variables : %1").arg(tr("show variables")) ;
  help << QString("list clips (N): %1").arg(tr("open up clip list")) ;
  help << QString("search clips by name ... syntax ... : %1").arg(tr("search clips")) ;
  help << QString("search clips by files ... items ... : %1").arg(tr("search clips")) ;
  help << QString("search clips by id ... items ... : %1").arg(tr("search clips")) ;
  help << QString("search clips in list by name ... syntax ... : %1").arg(tr("search clips and open in list")) ;
  help << QString("search albums by id ... items ... : %1").arg(tr("search album by identifiers exactly")) ;
  help << QString("search albums by identifiers ... syntax ... : %1").arg(tr("search album by identifiers with bracket syntax")) ;
  help << QString("search albums by name ... syntax ... : %1").arg(tr("search album with bracket syntax")) ;
  help << QString("search albums by tags ... syntax ... : %1").arg(tr("search album by tags")) ;
  help << QString("search albums by uuid ... items ... : %1").arg(tr("search albums in uuid format")) ;
  help << QString("search albums by files ... items ... : %1").arg(tr("search albums by file size")) ;
  help << QString("search albums by pictures ... items ... : %1").arg(tr("search albums by picture files")) ;
  help << QString("edit albums by id ... items ... : %1").arg(tr("edit album by identifiers exactly")) ;
  help << QString("catalog albums identical ( with/without progress ) : %1").arg(tr("catalog identical albums into files")) ;
  help << QString("catalog albums by organizations : %1").arg(tr("catalog albums depend on its organization")) ;
  help << QString("catalog albums by products : %1").arg(tr("catalog albums depend on its product identifiers")) ;
  help << QString("catalog albums by orders : %1").arg(tr("catalog albums depend on its video clips")) ;
  help << QString("catalog albums by commodity : %1").arg(tr("catalog albums depend on its commodity")) ;
}

int N::VideosDispatcher::matching(QString cmd)
{
  QStringList C = CommandTokens ( cmd )                                      ;
  ////////////////////////////////////////////////////////////////////////////
  command    = cmd                                                           ;
  arguments  . clear ( )                                                     ;
  arguments <= C                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) == 3 ) && ( "list" == C [ 0 ] . toLower ( ) )       ) {
    if ( "clips" == C [ 1 ] . toLower ( )                                  ) {
      return CiosMenuId ( 79 , 1 , 1 )                                       ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) > 4 ) && ( "search" == C [ 0 ] . toLower ( ) ) )      {
    if ( "clips" == C [ 1 ] . toLower ( ) )                                  {
      ////////////////////////////////////////////////////////////////////////
      if ( "by" == C [ 2 ] . toLower ( ) )                                   {
        if ( "name"  == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 79 , 1 ,  2 )                                  ;
        }                                                                    ;
        if ( "files" == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 79 , 1 ,  3 )                                  ;
        }                                                                    ;
        if ( "id"    == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 79 , 1 ,  4 )                                  ;
        }                                                                    ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( ( C . count ( ) > 6 ) )                                           {
        if ( "in" == C [ 2 ] . toLower ( ) )                                 {
          if ( "list" == C [ 3 ] . toLower ( ) )                             {
            if ( "by" == C [ 4 ] . toLower ( ) )                             {
              if ( "name" == C [ 5 ] . toLower ( ) )                         {
                return CiosMenuId ( 79 , 1 ,  5 )                            ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) > 4 ) && ( "search" == C [ 0 ] . toLower ( ) ) )      {
    if ( "albums" == C [ 1 ] . toLower ( ) )                                 {
      if ( "by" == C [ 2 ] . toLower ( ) )                                   {
        if ( "id" == C [ 3 ] . toLower ( ) )                                 {
          return CiosMenuId ( 79 , 3 ,  1 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "identifiers" == C [ 3 ] . toLower ( ) )                        {
          return CiosMenuId ( 79 , 3 ,  2 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "name" == C [ 3 ] . toLower ( ) )                               {
          return CiosMenuId ( 79 , 3 ,  3 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "uuid" == C [ 3 ] . toLower ( ) )                               {
          return CiosMenuId ( 79 , 3 ,  4 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "files" == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 79 , 3 ,  5 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "pictures" == C [ 3 ] . toLower ( ) )                           {
          return CiosMenuId ( 79 , 3 ,  6 )                                  ;
        }                                                                    ;
        //////////////////////////////////////////////////////////////////////
        if ( "tags"     == C [ 3 ] . toLower ( ) )                           {
          return CiosMenuId ( 79 , 3 ,  7 )                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) > 1 ) && "catalog" == C [ 0 ] . toLower ( ) )         {
    if ( "albums" == C [ 1 ] . toLower ( ) )                                 {
      if ( ( C . count ( ) > 2 ) && "identical" == C [ 2 ] . toLower ( ) )   {
        bool c = true                                                        ;
        if ( ! Variables . contains ( "OutputList" ) )                       {
          Report ( tr("You need to set up variable OutputList") )            ;
          Report ( "\n"                                         )            ;
          c = false                                                          ;
        }                                                                    ;
        if ( ! Variables . contains ( "OutputUuid" ) )                       {
          Report ( tr("You need to set up variable OutputUuid") )            ;
          Report ( "\n"                                         )            ;
          c = false                                                          ;
        }                                                                    ;
        if ( ! c ) return 0                                                  ;
        return CiosMenuId ( 79 , 4 ,  1 )                                    ;
      }                                                                      ;
      if ( ( C . count ( ) > 3 ) && "by" == C [ 2 ] . toLower ( ) )          {
        if ( "organizations" == C [ 3 ] . toLower ( ) )                      {
          return CiosMenuId ( 79 , 4 ,  2 )                                  ;
        }                                                                    ;
        if ( "products"      == C [ 3 ] . toLower ( ) )                      {
          return CiosMenuId ( 79 , 4 ,  3 )                                  ;
        }                                                                    ;
        if ( "orders"        == C [ 3 ] . toLower ( ) )                      {
          return CiosMenuId ( 79 , 4 ,  4 )                                  ;
        }                                                                    ;
        if ( "commodity"     == C [ 3 ] . toLower ( ) )                      {
          return CiosMenuId ( 79 , 4 ,  5 )                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) > 4 ) && ( "edit" == C [ 0 ] . toLower ( ) ) )        {
    if ( "albums" == C [ 1 ] . toLower ( ) )                                 {
      if ( "by" == C [ 2 ] . toLower ( ) )                                   {
        if ( "id" == C [ 3 ] . toLower ( ) )                                 {
          return CiosMenuId ( 79 , 5 ,  1 )                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) == 5 ) && ( "set" == C [ 0 ] . toLower ( ) )        ) {
    if ( "video" == C [ 1 ] . toLower ( )                                  ) {
      if ( ( "variable" == C [ 2 ] . toLower ( ) )  )                        {
        return CiosMenuId ( 79 , 2 , 1 )                                     ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) == 3 ) && ( "show" == C [ 0 ] . toLower ( ) )       ) {
    if ( "video" == C [ 1 ] . toLower ( )                                  ) {
      if ( ( "variables" == C [ 2 ] . toLower ( ) )  )                       {
        return CiosMenuId ( 79 , 2 , 2 )                                     ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return 0                                                                   ;
}

bool N::VideosDispatcher::execute(int id)
{
  switch ( id )                          {
    case CiosMenuId     ( 79 ,  1 ,  1 ) :
      ListClips         (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  1 ,  2 ) :
      SearchClips       (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  1 ,  3 ) :
      SearchFiles       (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  1 ,  4 ) :
      ClipsId           (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  1 ,  5 ) :
      SearchClip        (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  2 ,  1 ) :
      setVariable       (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  2 ,  2 ) :
      showVariables     (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  1 ) :
      SearchId          (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  2 ) :
      SearchIdentifiers (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  3 ) :
      SearchAlbums      (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  4 ) :
      SearchUUIDs       (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  5 ) :
      AlbumFiles        (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  6 ) :
      AlbumPictures     (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  3 ,  7 ) :
      SearchTags        (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  4 ,  1 ) :
      MatchIdenticals   (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  4 ,  2 ) :
      CatalogAlbums     (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  4 ,  3 ) :
      CatalogProducts   (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  4 ,  4 ) :
      CatalogOrders     (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  4 ,  5 ) :
      CatalogCommodity  (              ) ;
    return true                          ;
    case CiosMenuId     ( 79 ,  5 ,  1 ) :
      EditId            (              ) ;
    return true                          ;
  }                                      ;
  return false                           ;
}

void N::VideosDispatcher::setExtras(void * data)
{
  ExtraPacket * ep = (ExtraPacket *) data                   ;
  QWidget     * ww = ep -> widget                           ;
  plan = ep -> plan                                         ;
  nConnect ( this , SIGNAL ( ListClips (int)            )   ,
             ww   , SLOT   ( ListClips (int)            ) ) ;
  nConnect ( this , SIGNAL ( ListClips (QString,UUIDs&) )   ,
             ww   , SLOT   ( ListClips (QString,UUIDs&) ) ) ;
  nConnect ( this , SIGNAL ( ListClip  (QString,UUIDs&) )   ,
             ww   , SLOT   ( ListClip  (QString,UUIDs&) ) ) ;
  nConnect ( this , SIGNAL ( ListVideo (QString,UUIDs&) )   ,
             ww   , SLOT   ( ListVideo (QString,UUIDs&) ) ) ;
  nConnect ( this , SIGNAL ( AlbumDetails(SUID,QString) )   ,
             ww   , SLOT   ( AlbumDetails(SUID,QString) ) ) ;
  Variables [ "Language" ] = plan -> LanguageId             ;
}

bool N::VideosDispatcher::FilterType(SqlConnection & SC,int Type,UUIDs & U,UUIDs & P)
{
  return UuidSyntax::FilterType ( SC , PlanTable(MajorUuid) , Type , U , P ) ;
}

bool N::VideosDispatcher::NamesOwners(SqlConnection & SC,UUIDs & U,UUIDs & P)
{
  return UuidSyntax::NamesOwners ( SC , PlanTable(NameMaps) , U , P ) ;
}



void N::VideosDispatcher::setVariable(void)
{
  if ( arguments . count ( ) != 5 ) return       ;
  QString n = arguments [ 3 ] . toString ( )     ;
  QString v = arguments [ 4 ] . toString ( )     ;
  Variables [ n ] = arguments [ 4 ]              ;
  Report ( QString("%1 => %2\n").arg(n).arg(v) ) ;
}

void N::VideosDispatcher::showVariables(void)
{
  QStringList K = Variables . keys ( )             ;
  QString     k                                    ;
  QString     v                                    ;
  foreach ( k , K )                                {
    v = Variables [ k ] . toString ( )             ;
    Report ( QString("%1 => %2\n").arg(k).arg(v) ) ;
  }                                                ;
}

void N::VideosDispatcher::SendListClips(void)
{
  while ( Clips . count ( ) > 0 ) {
    int c = Clips . first (   )   ;
    Clips . takeFirst     (   )   ;
    emit ListClips        ( c )   ;
  }                               ;
}

void N::VideosDispatcher::SendClipsGroup(void)
{
  while ( ClipsPaddings . count ( ) > 0 ) {
    QString p = ClipsPaddings . first ( ) ;
    UUIDs   U = ClipsQueues [ p ]         ;
    ClipsPaddings . takeFirst (       )   ;
    ClipsQueues   . remove    ( p     )   ;
    emit ListClips            ( p , U )   ;
  }                                       ;
}

void N::VideosDispatcher::SendClips(void)
{
  while ( ClipPaddings . count ( ) > 0 ) {
    QString p = ClipPaddings . first ( ) ;
    UUIDs   U = ClipQueues [ p ]         ;
    ClipPaddings . takeFirst (       )   ;
    ClipQueues   . remove    ( p     )   ;
    emit ListClip            ( p , U )   ;
  }                                      ;
}

void N::VideosDispatcher::SendVideos(void)
{
  while ( VideoPaddings . count ( ) > 0 ) {
    QString p = VideoPaddings . first ( ) ;
    UUIDs   U = VideoQueues [ p ]         ;
    VideoPaddings . takeFirst (       )   ;
    VideoQueues   . remove    ( p     )   ;
    emit ListVideo            ( p , U )   ;
  }                                       ;
}

void N::VideosDispatcher::SendEditings(void)
{
  while ( EditPaddings . count ( ) > 0 ) {
    QString p = EditPaddings . first ( ) ;
    UUIDs   U = EditQueues [ p ]         ;
    SUID    u                            ;
    EditPaddings . takeFirst  (       )  ;
    EditQueues   . remove     ( p     )  ;
    foreach ( u , U )                    {
      emit AlbumDetails       ( u , p )  ;
    }                                    ;
  }                                      ;
}

void N::VideosDispatcher::ListClips(void)
{
  if ( arguments . count ( ) != 3 ) return ;
  int c = arguments [ 2 ] . toInt ( )      ;
  Clips << c                               ;
  emit EmitListClips ( )                   ;
}

bool N::VideosDispatcher::ExactId(SqlConnection & SC,UUIDs & U,QString K)
{
  FinanceManager FM                      ( plan   ) ;
  SUID           u = FM . FindIdentifier ( SC , K ) ;
  if ( u > 0 )                                      {
    if ( ! U . contains ( u ) ) U << u              ;
  } else                                            {
    Report ( tr("Found nothing for `%1`").arg(K)  ) ;
    Report ( "\n"                                 ) ;
  }                                                 ;
  return ( U . count ( ) > 0 )                      ;
}

bool N::VideosDispatcher::LikeId(SqlConnection & SC,UUIDs & U,QString K)
{
  QString W                                                         ;
  QString Q                                                         ;
  SUID    u                                                         ;
  ///////////////////////////////////////////////////////////////////
  W = QString("where `name` like '%1' order by `id` asc").arg(K)    ;
  Q = SC . sql . SelectFrom ( "uuid" , PlanTable(Identifiers) , W ) ;
  SqlLoopNow ( SC , Q )                                             ;
    u = SC . Uuid ( 0 )                                             ;
    if ( u > 0 )                                                    {
      if ( ! U.contains ( u ) ) U << u                              ;
    } else                                                          {
      Report ( tr("Found nothing for `%1`").arg(K) )                ;
      Report ( "\n"                                )                ;
    }                                                               ;
  SqlLoopErr ( SC , Q )                                             ;
    Report   ( tr("Found nothing for `%1`").arg(K) )                ;
    Report   ( "\n"                                )                ;
  SqlLoopEnd ( SC , Q )                                             ;
  ///////////////////////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                                      ;
}

bool N::VideosDispatcher::RegexpId(SqlConnection & SC,UUIDs & U,QString K)
{
  QString W                                                         ;
  QString Q                                                         ;
  SUID    u                                                         ;
  ///////////////////////////////////////////////////////////////////
  W = QString("where `name` regexp '%1' order by `id` asc").arg(K)  ;
  Q = SC . sql . SelectFrom ( "uuid" , PlanTable(Identifiers) , W ) ;
  SqlLoopNow ( SC , Q )                                             ;
    u = SC . Uuid ( 0 )                                             ;
    if ( u > 0 )                                                    {
      if ( ! U.contains ( u ) ) U << u                              ;
    } else                                                          {
      Report ( tr("Found nothing for `%1`").arg(K) )                ;
      Report ( "\n"                                )                ;
    }                                                               ;
  SqlLoopErr ( SC , Q )                                             ;
    Report   ( tr("Found nothing for `%1`").arg(K) )                ;
    Report   ( "\n"                                )                ;
  SqlLoopEnd ( SC , Q )                                             ;
  ///////////////////////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                                      ;
}

bool N::VideosDispatcher::MatchId(SqlConnection & SC,UUIDs & U,QString K)
{
  if ( K . contains ( "%" )      ) {
    return LikeId   ( SC , U , K ) ;
  }                                ;
  if ( K . contains ( "*" )      ) {
    return RegexpId ( SC , U , K ) ;
  }                                ;
  return   ExactId  ( SC , U , K ) ;
}

bool N::VideosDispatcher::ReportNames(SqlConnection & SC,UUIDs & U)
{
  GroupItems GI ( plan )                          ;
  SUID       u                                    ;
  /////////////////////////////////////////////////
  foreach ( u , U )                               {
    QStringList NS = GI . Names ( SC , u )        ;
    Report ( tr("Commodity %1").arg(u)   )        ;
    Report ( "\n"                        )        ;
    if ( NS . count ( ) > 0 )                     {
      for (int i = 0 ; i < NS . count ( ) ; i++ ) {
        Report ( "=> " )                          ;
        Report ( NS[i] )                          ;
        Report ( "\n"  )                          ;
      }                                           ;
    }                                             ;
  }                                               ;
  /////////////////////////////////////////////////
  return true                                     ;
}

bool N::VideosDispatcher::FetchAlbums(SqlConnection & SC,UUIDs & U,UUIDs & P)
{
  GroupItems GI ( plan )                    ;
  SUID       u                              ;
  SUID       z                              ;
  ///////////////////////////////////////////
  GI . AutoMap    = true                    ;
  GI . GroupTable = GI . LookTable          (
    Types  :: Commodity                     ,
    Types  :: Album                         ,
    Groups :: Subordination               ) ;
  ///////////////////////////////////////////
  foreach ( u , U )                         {
    UUIDs XU                                ;
    XU = GI . Subordination                 (
           SC                               ,
           u                                ,
           Types  :: Commodity              ,
           Types  :: Album                  ,
           Groups :: Subordination          ,
           SC . OrderByAsc ( "position" ) ) ;
    foreach ( z , XU )                      {
      if ( ! P . contains ( z ) ) P << z    ;
    }                                       ;
  }                                         ;
  ///////////////////////////////////////////
  return ( P . count ( ) > 0 )              ;
}

bool N::VideosDispatcher::FetchNames(SqlConnection & SC,UUIDs & U,QString K)
{
  if ( K . contains ( "%" ) )                     {
    FetchLike   ( SC , PlanTable(Names) , U , K ) ;
  } else
  if ( K . contains ( "*" ) )                     {
    FetchRegexp ( SC , PlanTable(Names) , U , K ) ;
  } else                                          {
    K = "%" + K + "%"                             ;
    FetchLike   ( SC , PlanTable(Names) , U , K ) ;
  }                                               ;
  /////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                    ;
}

bool N::VideosDispatcher::FetchByTag(SqlConnection & SC,UUIDs & U,QString Key)
{
  TagsManager TM ( plan )                         ;
  GroupItems  GI ( plan )                         ;
  UUIDs       T                                   ;
  UUIDs       P                                   ;
  UUIDs       L                                   ;
  SUID        u                                   ;
  /////////////////////////////////////////////////
  TM . GetTags ( SC , T , Key )                   ;
  if ( T . count ( ) <= 0 ) return false          ;
  /////////////////////////////////////////////////
  GI . AutoMap    = true                          ;
  GI . GroupTable = GI . LookTable                (
                      Types::Tag                  ,
                      Types::Album                ,
                      Groups::Subordination     ) ;
  foreach ( u , T )                               {
    P = GI . Subordination                        (
          SC                                      ,
          u                                       ,
          Types::Tag                              ,
          Types::Album                            ,
          Groups::Subordination                   ,
          SC . OrderByAsc ( "position" )        ) ;
    L << P                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  P . clear      (   )                            ;
  P = Uniqueness ( L )                            ;
  foreach ( u , P )                               {
    if ( ! U . contains ( u ) ) U << u            ;
  }                                               ;
  /////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                    ;
}

void N::VideosDispatcher::SearchClips(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "clips"  != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "name"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    UUIDs P                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchNames     ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    if ( Operate ( U , t , UM ) )                       {
      if ( NamesOwners ( SC , U , P ) )                 {
        FilterType ( SC , Types::Video , P , X )        ;
      }                                                 ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 clips") . arg ( X . count ( ) )    ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    ClipsPaddings  << l                                 ;
    ClipsQueues [ l ] = X                               ;
    emit EmitClipsGroup ( )                             ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchClip(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "clips"  != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "in"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "list"   != l ) c = false                        ;
  l = s [ 4 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 5 ] . toLower ( )                             ;
  if ( "name"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<6;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    UUIDs P                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchNames     ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    if ( Operate ( U , t , UM ) )                       {
      if ( NamesOwners ( SC , U , P ) )                 {
        FilterType ( SC , Types::Video , P , X )        ;
      }                                                 ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 clips") . arg ( X . count ( ) )    ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    ClipPaddings  << l                                  ;
    ClipQueues [ l ] = X                                ;
    emit EmitClips ( )                                  ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchId(void)
{
  QStringList s                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  s <= arguments                                        ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums"  != l ) c = false                       ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "id"     != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  if ( s . count ( ) <= 0 ) c = false                   ;
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  UUIDs X                                               ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs C                                             ;
    /////////////////////////////////////////////////////
    foreach ( l , s ) ExactId ( SC , C , l )            ;
    if ( C . count ( ) > 0 )                            {
      ReportNames ( SC , C     )                        ;
      FetchAlbums ( SC , C , X )                        ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = s . join ( " " )                                ;
    VideoPaddings  << l                                 ;
    VideoQueues [ l ] = X                               ;
    emit EmitVideos ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchIdentifiers(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search"      != l ) c = false                   ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums"      != l ) c = false                   ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"          != l ) c = false                   ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "identifiers" != l ) c = false                   ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    UUIDs C                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      MatchId        ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    if ( Operate ( C , t , UM ) )                       {
      if ( C . count ( ) > 0 )                          {
        ReportNames ( SC , C     )                      ;
        FetchAlbums ( SC , C , X )                      ;
      }                                                 ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = s . join ( " " )                                ;
    VideoPaddings  << l                                 ;
    VideoQueues [ l ] = X                               ;
    emit EmitVideos ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchAlbums(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums" != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "name"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchNames     ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    if ( Operate ( U , t , UM ) )                       {
      FilterType ( SC , Types::Album , U , X )          ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = s . join ( " " )                                ;
    VideoPaddings  << l                                 ;
    VideoQueues [ l ] = X                               ;
    emit EmitVideos ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchUUIDs(void)
{
  QStringList s                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  s <= arguments                                        ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums"  != l ) c = false                       ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "uuid"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  if ( s . count ( ) <= 0 ) c = false                   ;
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  UUIDs X                                               ;
  foreach ( l , s )                                     {
    SUID u = l . toULongLong ( )                        ;
    if ( ! X . contains ( u ) ) X << u                  ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = s . join ( " " )                                ;
    VideoPaddings  << l                                 ;
    VideoQueues [ l ] = X                               ;
    emit EmitVideos ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::MatchIdenticals(void)
{
  QStringList s                                              ;
  QString     l                                              ;
  QString     OutputList                                     ;
  QString     OutputUuid                                     ;
  bool        c = true                                       ;
  bool        p = true                                       ;
  int         language = Variables [ "Language" ] . toInt () ;
  ////////////////////////////////////////////////////////////
  s <= arguments                                             ;
  if ( ! Variables . contains ( "OutputList" ) )             {
    Report ( tr("You need to set up variable OutputList")  ) ;
    Report ( "\n"                                          ) ;
    c = false                                                ;
  }                                                          ;
  if ( ! Variables . contains ( "OutputUuid" ) )             {
    Report ( tr("You need to set up variable OutputUuid")  ) ;
    Report ( "\n"                                          ) ;
    c = false                                                ;
  }                                                          ;
  OutputList = Variables [ "OutputList" ] . toString ( )     ;
  OutputUuid = Variables [ "OutputUuid" ] . toString ( )     ;
  ////////////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                                  ;
  if ( "catalog"   != l ) c = false                          ;
  l = s [ 1 ] . toLower ( )                                  ;
  if ( "albums"    != l ) c = false                          ;
  l = s [ 2 ] . toLower ( )                                  ;
  if ( "identical" != l ) c = false                          ;
  ////////////////////////////////////////////////////////////
  if ( ! c )                                                 {
    Report ( tr("Syntax error") )                            ;
    Report ( "\n"               )                            ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if ( s . count ( ) > 4 )                                   {
    if ( "progress" == s [ 4 ] . toLower ( ) )               {
      if ( "without" == s [ 4 ] . toLower ( ) )              {
        p = false                                            ;
      }                                                      ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  NAMEs         S                                            ;
  UUIDs         U                                            ;
  SqlConnection SC ( plan -> sql )                           ;
  ////////////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                        {
    QString Q                                                ;
    QString N                                                ;
    SUID    u                                                ;
    qint64  total                                            ;
    qint64  index = 0                                        ;
    bool    go    = true                                     ;
    int     id    = -1                                       ;
    //////////////////////////////////////////////////////////
    Q = QString("select `uuid` from `divisions`"
                " where `type` = %1 ;").arg(Types::Album)    ;
    SqlLoopNow ( SC , Q )                                    ;
      U << SC . Uuid ( 0 )                                   ;
    SqlLoopErr ( SC , Q )                                    ;
    SqlLoopEnd ( SC , Q )                                    ;
    //////////////////////////////////////////////////////////
    total = U . count ( )                                    ;
    if ( total > 0 )                                         {
      id = plan -> Progress ( tr("Catalog albums")           ,
                              tr("%v/%m albums")           ) ;
      if ( id >= 0 )                                         {
        plan -> Start    ( id , &index , &go   )             ;
        plan -> setRange ( id , 0      , total )             ;
      }                                                      ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    while ( go && ( index < total ) )                        {
      u = U [ index ]                                        ;
      N = SC . getName                                       (
            PlanTable(Names)                                 ,
            "uuid"                                           ,
            language                                         ,
            u                                              ) ;
      if ( N . length ( ) > 0 )                              {
        S [ u ] = N                                          ;
      }                                                      ;
      index++                                                ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    if ( id >= 0 ) plan -> Finish ( id )                     ;
    //////////////////////////////////////////////////////////
    SC . close  ( )                                          ;
  }                                                          ;
  SC   . remove ( )                                          ;
  ////////////////////////////////////////////////////////////
  U . clear ( )                                              ;
  if ( SortedUuids ( S , U ) )                               {
    QStringList L                                            ;
    QStringList G                                            ;
    QString     LS                                           ;
    QString     GS                                           ;
    QByteArray  LL                                           ;
    QByteArray  GG                                           ;
    SUID        z                                            ;
    toStrings ( S , U , L )                                  ;
    foreach   ( z , U     )                                  {
      G << QString("%1 => %2") . arg ( z ) . arg ( S [ z ] ) ;
    }                                                        ;
    LS = L  . join   ( "\n"            )                     ;
    GS = G  . join   ( "\n"            )                     ;
    LL = LS . toUtf8 (                 )                     ;
    GG = GS . toUtf8 (                 )                     ;
    File::toFile     ( OutputList , LL )                     ;
    File::toFile     ( OutputUuid , GG )                     ;
  }                                                          ;
  Report ( tr("Catalog identical albums completed") )        ;
  Report ( "\n"                                     )        ;
  Alert  ( Done                                     )        ;
}

SUID N::VideosDispatcher::MapToCompany(QString ID)
{
  SUID u                                      ;
  foreach ( u , Matchings )                   {
    QString S = MatchingScripts [ u ]         ;
    if ( Scripts::StringMatching ( S , ID ) ) {
      return ToCompanies [ u ]                ;
    }                                         ;
  }                                           ;
  return 0                                    ;
}

void N::VideosDispatcher::CatalogAlbums(void)
{
  int lines = 0                                                           ;
  GroupItems    GI ( plan        )                                        ;
  SqlConnection SC ( plan -> sql )                                        ;
  if ( SC . open ( FunctionString ) )                                     {
    UUIDs Ouids                                                           ;
    SUID  u                                                               ;
    SUID  c                                                               ;
    SUID  x                                                               ;
    Ouids = SC.Uuids(PlanTable(Organizations),"uuid",SC.OrderByAsc("id")) ;
    foreach ( u , Ouids )                                                 {
      QStringList names                                                   ;
      QString     on                                                      ;
      UUIDs       Auids                                                   ;
      UUIDs       Cuids                                                   ;
      UUIDs       Xuids                                                   ;
      names = GI . Names ( SC , u )                                       ;
      if (names.count()>0) on = names[0]                                  ;
      Auids = GI . Subordination                                          (
                SC                                                        ,
                u                                                         ,
                Types  :: Organization                                    ,
                Types  :: Album                                           ,
                Groups :: Subordination                                   ,
                SC      . OrderByAsc ( "position" )                     ) ;
      Cuids = GI . Subordination                                          (
                SC                                                        ,
                u                                                         ,
                Types  :: Organization                                    ,
                Types  :: Commodity                                       ,
                Groups :: Subordination                                   ,
                SC      . OrderByAsc ( "position" )                     ) ;
      Report ( QString("<%1> has (%2,%3)\n" ).arg(on).arg(Auids.count()).arg(Cuids.count()) ) ;
      lines++                                                             ;
//      if ( 0 == ( lines % 100 ) ) Clear ( )                               ;
      foreach ( c , Cuids )                                               {
        Xuids = GI . Subordination                                        (
                  SC                                                      ,
                  c                                                       ,
                  Types  :: Commodity                                     ,
                  Types  :: Album                                         ,
                  Groups :: Subordination                                 ,
                  SC.OrderByAsc("position")                             ) ;
        foreach ( x , Xuids )                                             {
          if ( !Auids.contains(x) ) Auids << x                            ;
        }                                                                 ;
      }                                                                   ;
      if (Auids.count()>0)                                                {
        Report ( QString("Append <%1> into <%2>\n" ).arg(Auids.count()).arg(on) ) ;
        lines++                                                           ;
//        if ( 0 == ( lines % 100 ) ) Clear ( )                             ;
        GI . FullDetach                                                   (
          SC                                                              ,
          u                                                               ,
          Types  :: Organization                                          ,
          Types  :: Album                                                 ,
          Groups :: Subordination                                       ) ;
        GI . Join                                                         (
          SC                                                              ,
          u                                                               ,
          Types  :: Organization                                          ,
          Types  :: Album                                                 ,
          Groups :: Subordination                                         ,
          0                                                               ,
          Auids                                                         ) ;
      }                                                                   ;
    }                                                                     ;
    SC . close  ( )                                                       ;
  }                                                                       ;
  SC   . remove ( )                                                       ;
  Alert ( Done )                                                          ;
}

void N::VideosDispatcher::CatalogProducts(void)
{
  Matchings       . clear ( )               ;
  Companies       . clear ( )               ;
  ToCompanies     . clear ( )               ;
  MatchingScripts . clear ( )               ;
  CompanyNames    . clear ( )               ;
  GroupItems    GI ( plan        )          ;
  SqlConnection SC ( plan -> sql )          ;
  if (SC.open("VideoCLI","Catalog"))        {
    QString Q                               ;
    QString N                               ;
    UUIDs   U                               ;
    SUID    u                               ;
    int     Lines = 0                       ;
    /////////////////////////////////////////
    Q = SC.sql.SelectFrom                   (
          "script,company"                  ,
          "dvdmatching"                     ,
          "order by id asc"               ) ;
    SqlLoopNow ( SC , Q )                   ;
      SUID SCRIPT  = SC.Uuid(0)             ;
      SUID COMPANY = SC.Uuid(1)             ;
      Matchings << SCRIPT                   ;
      Companies << COMPANY                  ;
      ToCompanies [ SCRIPT ] = COMPANY      ;
    SqlLoopErr ( SC , Q )                   ;
    SqlLoopEnd ( SC , Q )                   ;
    /////////////////////////////////////////
    foreach (u,Matchings)                   {
      Q = SC.sql.SelectFrom                 (
            "script"                        ,
            PlanTable(Scripts)              ,
            SC.WhereUuid(u)               ) ;
      if (SC.Fetch(Q))                      {
        QString XX = SC.String(0)           ;
        MatchingScripts [ u ] = XX          ;
      }                                     ;
    }                                       ;
    /////////////////////////////////////////
    foreach (u,Companies)                   {
      QStringList L = GI.Names(SC,u)        ;
      if (L.count()>0)                      {
        CompanyNames[u] = L[0]              ;
      }                                     ;
    }                                       ;
    /////////////////////////////////////////
    U = SC . Uuids                          (
          PlanTable(Commodities)            ,
          "uuid"                            ,
          SC.OrderByAsc("id")             ) ;
    foreach (u,U)                           {
      Q = SC.sql.SelectFrom                 (
            "name"                          ,
            PlanTable(Identifiers)          ,
            SC.WhereUuid(u)               ) ;
      if (SC.Fetch(Q))                      {
        N = SC.String(0)                    ;
        if (N.length()>0)                   {
          N = N . toUpper ( )               ;
          SUID x = MapToCompany(N)          ;
          if (x>0)                          {
            UUIDs CU                        ;
            CU << u                         ;
            Report ( QString("%1 => %2\n").arg(N).arg(CompanyNames[x]) ) ;
            Lines++                         ;
            if (Lines>=100)                 {
              if ( NULL != History )        {
                History -> clear ( )        ;
              }                             ;
              Lines = 0                     ;
            }                               ;
            GI . Join                       (
              SC                            ,
              x                             ,
              Types  :: Organization        ,
              Types  :: Commodity           ,
              Groups :: Subordination       ,
              0                             ,
              CU                            ,
              false                       ) ;
          }                                 ;
        }                                   ;
      }                                     ;
      plan -> processEvents ( )             ;
    }                                       ;
    SC.close()                              ;
  }                                         ;
  SC.remove()                               ;
  Report ( tr("Completed") )                ;
  Report ( "\n"            )                ;
  Alert  ( Done            )                ;
}

void N::VideosDispatcher::CatalogOrders(void)
{
  int           total = 0                ;
  ZMAPs         NtoV                     ;
  QStringList   VCL                      ;
  UUIDs         CUL                      ;
  GroupItems    GI ( plan        )       ;
//  VideoManager  VM ( plan        )       ;
  SqlConnection SC ( plan -> sql )       ;
  if ( SC . open ( FunctionString ) )    {
    QStringList N                        ;
    QString     Q                        ;
    QString     V                        ;
    QString     S                        ;
    UUIDs       U                        ;
    SUID        u                        ;
    SUID        z                        ;
    int         lines = 0                ;
    Report ( QString("Starting\n") )     ;
    //////////////////////////////////////
    U = SC . Uuids                       (
          PlanTable(Videos)              ,
          "uuid"                         ,
          SC.OrderByAsc("id")          ) ;
    foreach (u,U)                        {
      z = 0                              ;
      Q = QString("select first from groups where t1 = %1 and t2 = %2 and relation = %3 and second = %4 ;")
          . arg (N::Types ::Album        )
          . arg (N::Types ::Video        )
          . arg (N::Groups::Subordination)
          . arg (u                     ) ;
      if (SC.Fetch(Q)) z = SC.Uuid(0)    ;
      if (z<=0)                          {
        N = GI.Names(SC,u)               ;
        if (N.length()>0)                {
          QString X = N[0]               ;
          N = X.split('/')               ;
          if (N.count()>2)               {
            if (!VCL.contains(X))        {
              VCL << X                   ;
              NtoV [ X ] = u             ;
            }                            ;
          }                              ;
        }                                ;
      }                                  ;
    }                                    ;
    //////////////////////////////////////
    Report ( QString("Completed\n") )    ;
    VCL . sort       (      )            ;
    S   = VCL . join ( "\n" )            ;
    QByteArray BS = S.toUtf8()           ;
    N::File::toFile("G:/Temp/EurAV.txt",BS) ;
    foreach ( V , VCL )                  {
      u = NtoV [ V ]                     ;
      CUL << u                           ;
//      S = QString("%1 => %2").arg(u).arg(V) ;
//      Report ( S    )                    ;
//      Report ( "\n" )                    ;
//      lines++                            ;
//      total++                            ;
//      if (lines>200)                     {
//        Clear ( )                        ;
//        lines = 0                        ;
//      }                                  ;
    }                                    ;
    //////////////////////////////////////
    SC.close()                           ;
  }                                      ;
  SC.remove()                            ;
  Report ( QString("%1 Completed\n").arg(total) ) ;
//  emit ListClip ( "European" , CUL ) ;
  Alert  ( Done            )                ;
}

void N::VideosDispatcher::CatalogCommodity(void)
{
  QStringList   Used                        ;
  QStringList   All                         ;
  int           language                    ;
  SqlConnection SC ( plan -> sql )          ;
  language = Variables["Language"].toInt()  ;
  if ( SC . open ( FunctionString ) )       {
    QString Q                               ;
    QString N                               ;
    UUIDs   U                               ;
    SUID    u                               ;
    /////////////////////////////////////////
    Q = SC.sql.SelectFrom                   (
          "script"                          ,
          "dvdmatching"                     ,
          "order by id asc"               ) ;
    SqlLoopNow ( SC , Q )                   ;
      U << SC . Uuid ( 0 )                  ;
    SqlLoopErr ( SC , Q )                   ;
    SqlLoopEnd ( SC , Q )                   ;
    /////////////////////////////////////////
    foreach (u,U)                           {
      N = SC . getName                      (
            PlanTable(Names)                ,
            "uuid"                          ,
            language                        ,
            u                             ) ;
      if (N.length()>0) Used << N           ;
    }                                       ;
    /////////////////////////////////////////
    U = SC . Uuids                          (
          PlanTable(Commodities)            ,
          "uuid"                            ,
          SC.OrderByAsc("id")             ) ;
    foreach (u,U)                           {
      Q = SC.sql.SelectFrom                 (
            "name"                          ,
            PlanTable(Identifiers)          ,
            SC.WhereUuid(u)               ) ;
      if (SC.Fetch(Q))                      {
        N = SC.String(0)                    ;
        if (N.length()>0)                   {
          int xid                           ;
          N   = N . toUpper (     )         ;
          xid = N . indexOf ( '-' )         ;
          if (xid>0)                        {
            QString n = N.left(xid+1)       ;
            if (!Used.contains(n))          {
              if (!All.contains(n))         {
                bool correct = true         ;
                if (n.contains(' '))        {
                  correct = false           ;
                }                           ;
                if (!(n.at(0)>='A' && n.at(0)<='Z')) {
                  correct = false           ;
                }                           ;
                if (correct) All << n       ;
              }                             ;
            }                               ;
          }                                 ;
        }                                   ;
      }                                     ;
      plan -> processEvents ( )             ;
    }                                       ;
    /////////////////////////////////////////
    All . sort ( )                          ;
    foreach (N,All)                         {
      QString SS                            ;
      int     tt = Scripts::Matching        ;
      SS = QString("function matching(ss)\n"
                   "{\n"
                   "  return ( ss . indexOf ( \"%1\" ) == 0 ) ;\n"
                   "}\n"           ).arg(N) ;
      u = SC.Unique                         (
            PlanTable(MajorUuid)            ,
            "uuid"                          ,
            7789120                       ) ;
      SC . assureUuid                       (
        PlanTable(MajorUuid)                ,
        u                                   ,
        Types::Script                     ) ;
      SC . assureUuid                       (
        PlanTable(Scripts  )                ,
        u                                   ,
        tt                                ) ;
      SC . assureName                       (
        PlanTable(Names)                    ,
        u                                   ,
        language                            ,
        N                                 ) ;
      Q = SC.sql.Update                     (
            PlanTable(Scripts)              ,
            "where uuid = :UUID"            ,
              2,"script","type"           ) ;
      SC.Prepare ( Q                      ) ;
      SC.Bind    ( "uuid"   , u           ) ;
      SC.Bind    ( "type"   , tt          ) ;
      SC.Bind    ( "script" , SS.toUtf8() ) ;
      SC.Exec    (                        ) ;
    }                                       ;
    /////////////////////////////////////////
    SC.close()                              ;
  }                                         ;
  SC.remove()                               ;
  Report ( tr("Completed") )                ;
  Report ( "\n"            )                ;
  Alert  ( Done            )                ;
}

void N::VideosDispatcher::EditId(void)
{
  QStringList s                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  s <= arguments                                        ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "edit"    != l ) c = false                       ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums"  != l ) c = false                       ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "id"     != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  if ( s . count ( ) <= 0 ) c = false                   ;
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  UUIDs X                                               ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs C                                             ;
    /////////////////////////////////////////////////////
    foreach ( l , s ) ExactId ( SC , C , l )            ;
    if ( C . count ( ) > 0 )                            {
      ReportNames ( SC , C     )                        ;
      FetchAlbums ( SC , C , X )                        ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = s . join ( " " )                                ;
    EditPaddings  << l                                  ;
    EditQueues [ l ] = X                                ;
    emit EmitEditings ( )                               ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::VideosDispatcher::SearchFiles(void)
{
  static char * cmds [ ] = { "search" , "clips" , "by" , "files" , NULL }    ;
  QStringList s = N::CommandTokens ( command )                               ;
  QString     l                                                              ;
  bool        c = compare ( 4 , s , cmds )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! c )                                                                 {
    Report ( tr("Syntax error") )                                            ;
    Report ( "\n"               )                                            ;
    return                                                                   ;
  }                                                                          ;
  for (int i=0;i<4;i++) s . takeFirst ( )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  UUIDs         X                                                            ;
  SqlConnection SC   ( plan -> sql         )                                 ;
  VideoManager  VM   ( plan                )                                 ;
  if ( SC . open     ( FunctionString )    )                                 {
    QString f                                                                ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( f , s ) VM . ClipsFilename ( SC , f , X )                      ;
    //////////////////////////////////////////////////////////////////////////
    SC . close       (                     )                                 ;
  }                                                                          ;
  SC   . remove      (                     )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                                                   {
    QString m                                                                ;
    m = tr("Found %1 clips") . arg ( X . count ( ) )                         ;
    Report ( m    )                                                          ;
    Report ( "\n" )                                                          ;
    l = s . join ( " " )                                                     ;
    ClipsPaddings  << l                                                      ;
    ClipsQueues [ l ] = X                                                    ;
    emit EmitClipsGroup ( )                                                  ;
  } else                                                                     {
    Report ( tr("Found nothing") )                                           ;
    Report ( "\n"                )                                           ;
  }                                                                          ;
}

void N::VideosDispatcher::ClipsId(void)
{
  static char * cmds [ ] = { "search" , "clips" , "by" , "id" , NULL }       ;
  QStringList s = N::CommandTokens ( command )                               ;
  QString     l                                                              ;
  bool        c = compare ( 4 , s , cmds )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! c )                                                                 {
    Report ( tr("Syntax error") )                                            ;
    Report ( "\n"               )                                            ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )                                    ;
  if ( s . count ( ) <= 0 ) c = false                                        ;
  if ( ! c )                                                                 {
    Report ( tr("Syntax error") )                                            ;
    Report ( "\n"               )                                            ;
    return                                                                   ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  UUIDs         X                                                            ;
  SqlConnection SC   ( plan -> sql         )                                 ;
  GroupItems    GI   ( plan                )                                 ;
  if ( SC . open     ( FunctionString )    )                                 {
    UUIDs C                                                                  ;
    UUIDs A                                                                  ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( l , s ) ExactId ( SC , C , l )                                 ;
    if ( C . count ( ) > 0 )                                                 {
      ReportNames ( SC , C     )                                             ;
      FetchAlbums ( SC , C , A )                                             ;
      if ( A . count ( ) > 0 )                                               {
        SUID u                                                               ;
        GI . AutoMap    = true                                               ;
        GI . GroupTable = GI . LookTable                                     (
                            Types::Album                                     ,
                            Types::Video                                     ,
                            Groups::Subordination                          ) ;
        foreach ( u , A )                                                    {
          UUIDs V = GI . Subordination                                       (
                      SC                                                     ,
                      u                                                      ,
                      Types::Album                                           ,
                      Types::Video                                           ,
                      Groups::Subordination                                  ,
                      SC . OrderByAsc ( "position" )                       ) ;
          SUID  v                                                            ;
          foreach ( v , V )                                                  {
            if ( ! X . contains ( v ) ) X << v                               ;
          }                                                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SC . close       (                     )                                 ;
  }                                                                          ;
  SC   . remove      (                     )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                                                   {
    QString m                                                                ;
    m = tr("Found %1 clips") . arg ( X . count ( ) )                         ;
    Report ( m    )                                                          ;
    Report ( "\n" )                                                          ;
    l = s . join ( " " )                                                     ;
    ClipsPaddings  << l                                                      ;
    ClipsQueues [ l ] = X                                                    ;
    emit EmitClipsGroup ( )                                                  ;
  } else                                                                     {
    Report ( tr("Found nothing") )                                           ;
    Report ( "\n"                )                                           ;
  }                                                                          ;
}

void N::VideosDispatcher::AlbumFiles(void)
{
  static char * cmds [ ] = { "search" , "albums" , "by" , "files" , NULL }   ;
  QStringList s = N::CommandTokens ( command )                               ;
  QString     l                                                              ;
  bool        c = compare ( 4 , s , cmds )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! c )                                                                 {
    Report ( tr("Syntax error") )                                            ;
    Report ( "\n"               )                                            ;
    return                                                                   ;
  }                                                                          ;
  for (int i=0;i<4;i++) s . takeFirst ( )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  UUIDs         X                                                            ;
  SqlConnection SC   ( plan -> sql         )                                 ;
  VideoManager  VM   ( plan                )                                 ;
  if ( SC . open     ( FunctionString )    )                                 {
    QString f                                                                ;
    UUIDs   C                                                                ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( f , s )        VM . ClipsFilename ( SC , f , C )               ;
    if ( C . count ( ) > 0 ) VM . ClipsToAlbums ( SC , C , X )               ;
    //////////////////////////////////////////////////////////////////////////
    SC . close       (                     )                                 ;
  }                                                                          ;
  SC   . remove      (                     )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                                                   {
    QString m                                                                ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )                        ;
    Report ( m    )                                                          ;
    Report ( "\n" )                                                          ;
    l = s . join ( " " )                                                     ;
    VideoPaddings  << l                                                      ;
    VideoQueues [ l ] = X                                                    ;
    emit EmitVideos ( )                                                      ;
  } else                                                                     {
    Report ( tr("Found nothing") )                                           ;
    Report ( "\n"                )                                           ;
  }                                                                          ;
}

void N::VideosDispatcher::AlbumPictures(void)
{
  static char * cmds [ ] = { "search","albums","by","pictures",NULL }        ;
  QStringList s = N::CommandTokens ( command )                               ;
  QString     l                                                              ;
  bool        c = compare ( 4 , s , cmds )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! c )                                                                 {
    Report ( tr("Syntax error") )                                            ;
    Report ( "\n"               )                                            ;
    return                                                                   ;
  }                                                                          ;
  for (int i=0;i<4;i++) s . takeFirst ( )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  UUIDs          X                                                           ;
  SqlConnection  SC  ( plan -> sql         )                                 ;
  PictureManager PM  ( plan                )                                 ;
  VideoManager   VM  ( plan                )                                 ;
  if ( SC . open     ( FunctionString )    )                                 {
    QString f                                                                ;
    UUIDs   I                                                                ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( f , s )                                                        {
      SUID puid = 0                                                          ;
      PM . Import ( SC , f , puid )                                          ;
      if ( ( puid > 0 ) && ( ! I . contains ( puid ) ) ) I << puid           ;
    }                                                                        ;
    if ( I . count ( ) > 0 ) VM . ImagesToAlbums ( SC, I , X )               ;
    //////////////////////////////////////////////////////////////////////////
    SC . close       (                     )                                 ;
  }                                                                          ;
  SC   . remove      (                     )                                 ;
  ////////////////////////////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                                                   {
    QString m                                                                ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )                        ;
    Report ( m    )                                                          ;
    Report ( "\n" )                                                          ;
    l = s . join ( " " )                                                     ;
    VideoPaddings  << l                                                      ;
    VideoQueues [ l ] = X                                                    ;
    emit EmitVideos ( )                                                      ;
  } else                                                                     {
    Report ( tr("Found nothing") )                                           ;
    Report ( "\n"                )                                           ;
  }                                                                          ;
}

void N::VideosDispatcher::SearchTags(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search"   != l ) c = false                      ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "albums" != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"       != l ) c = false                      ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "tags"     != l ) c = false                      ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchByTag     ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    Operate ( U , t , UM )                              ;
    X = Uniqueness ( U )                                ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 albums") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    VideoPaddings  << l                                 ;
    VideoQueues [ l ] = X                               ;
    emit EmitVideos ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}
