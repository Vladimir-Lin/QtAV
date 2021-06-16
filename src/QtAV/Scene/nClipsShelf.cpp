#include <qtav.h>

#ifdef QT_OPENGL_LIB

N::ClipsShelf:: ClipsShelf ( QObject * parent , Plan * p )
              : ListShelf  (           parent ,        p )
{
  Configure ( ) ;
}

N::ClipsShelf:: ClipsShelf ( QObject * parent , Plan * p , SceneDestructor * des )
              : ListShelf  (           parent ,        p ,                   des )
{
  Configure ( ) ;
}

N::ClipsShelf::~ClipsShelf (void)
{
}

void N::ClipsShelf::Configure(void)
{
  QSize TextRect       ( 512           , 16                  ) ;
  QObject::setProperty ( "Type"        , "ClipsShelf"        ) ;
  setVariable          ( "Type"        , "ClipsShelf"        ) ;
  setVariable          ( "Menu"        , true                ) ;
  setVariable          ( "Suicide"     , true                ) ;
  //////////////////////////////////////////////////////////////
  setVariable          ( "Access"      , "First"             ) ;
  setVariable          ( "Table"       , PlanTable(Videos)   ) ;
  setVariable          ( "T1"          , Types::Album        ) ;
  setVariable          ( "T2"          , Types::Video        ) ;
  setVariable          ( "Sorting"     , Qt::AscendingOrder  ) ;
  setVariable          ( "ByItem"      , "position"          ) ;
  setVariable          ( "AutoRefresh" , true                ) ;
  setVariable          ( "ArrangeBy"   , "Names"             ) ;
  setVariable          ( "ScrollBy"    , "Start"             ) ;
  //////////////////////////////////////////////////////////////
  setVariable          ( "TextRect"    , TextRect            ) ;
  setVariable          ( "TextPixels"  , 15                  ) ;
  setVariable          ( "TextLength"  , 80                  ) ;
  //////////////////////////////////////////////////////////////
  setVariable          ( "NameMaps"    , PlanTable(ClipMaps) ) ;
  setVariable          ( "NameTable"   , PlanTable(Names)    ) ;
}

void N::ClipsShelf::atDoubleClicked(SelectItem * item)
{
  QString n = item -> Value ( "ItemName" ) . toString    ( ) ;
  SUID    g = item -> Value ( "UUID"     ) . toULongLong ( ) ;
  VarArgs args                                               ;
  args << CiosMenuId  ( 89 , 45 , 94 )                       ;
  args << g                                                  ;
  args << n                                                  ;
  args << GroupType    ( 2 )                                 ;
  args << GroupType    ( 3 )                                 ;
  args << GroupType    ( 4 )                                 ;
  args << GroupType    ( 5 )                                 ;
  args << FirstUuid    (   )                                 ;
  args << SecondUuid   (   )                                 ;
  args << RelationType (   )                                 ;
  args << node                                               ;
  args << UniverseUuid (   )                                 ;
  args << Value ( "Suicide" ) . toBool ( )                   ;
  CallRequests ( args )                                      ;
}

int N::ClipsShelf::Indexing(RangeBar * rbu,int direction,QVariants & v)
{
//  qint64 s = v [ 0 ] . toLongLong ( ) ; // start
//  qint64 e = v [ 1 ] . toLongLong ( ) ; // end
//  qint64 p = v [ 2 ] . toLongLong ( ) ; // page
  qint64 i = v [ 3 ] . toLongLong ( ) ; // index
  setVariable ( "StartId" , i )       ;
  start       ( 10011         )       ;
  return 0                            ;
}

bool N::ClipsShelf::Relocation(QRectF viewport)
{
  return CentralRelocation ( viewport ) ;
}

bool N::ClipsShelf::Menu   (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  AllowPickItemMenu                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager mm ( widget )                                                  ;
  QMenu     * ma                                                             ;
  QAction   * aa                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  ma  = mm . addMenu   ( tr("Adjustments")    )                              ;
  AdjustMenu           ( mm , ma              )                              ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator    (                      )                              ;
  mm . add             ( 901 , tr("Close")    )                              ;
  ////////////////////////////////////////////////////////////////////////////
  RunPickItemMenu                                                            ;
  nKickOut             ( IsNull ( aa ) , true )                              ;
  ////////////////////////////////////////////////////////////////////////////
  if ( AdjustProcessor ( mm , aa        ) ) return true                      ;
  if ( 0 != CallToMenu ( mm , aa , true ) ) return true                      ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 901                                                                 :
      emit CloseObject ( node )                                              ;
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}

#endif
