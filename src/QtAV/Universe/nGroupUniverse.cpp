#include <qtav.h>

N::Universe * N::GroupCreator(void * p,QObject * parent)
{
  return new N::GroupUniverse ( parent , (Plan *) p ) ;
}

N::GroupUniverse:: GroupUniverse    ( QObject * parent , Plan * p )
                 : EditableUniverse (           parent ,        p )
{
  name = "Group"    ;
  addType ( 53002 ) ;
}

N::GroupUniverse:: GroupUniverse    ( QObject         * parent ,
                                      Plan            * p      ,
                                      SceneDestructor * des    )
                 : EditableUniverse ( parent , p , des         )
{
  name = "Group"    ;
  addType ( 53002 ) ;
}

N::GroupUniverse::~GroupUniverse (void)
{
}
