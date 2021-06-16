#include <qtav.h>

N::VideoJunction:: VideoJunction (QObject * parent)
                 : Junction      (          parent)
{
}

N::VideoJunction::~VideoJunction (void)
{
}

N::VideoChannel * N::VideoJunction::Supply(void)
{
  if (conduits.count()<=0) return NULL               ;
  VideoConduit * vc = NULL                           ;
  for (int i=0;IsNull(vc) && i<conduits.count();i++) {
    if ( 2001 == conduits[i]->type() )               {
      vc = (VideoConduit *)conduits[i]               ;
    }                                                ;
  }                                                  ;
  if (IsNull(vc)) return NULL                        ;
  return vc->channel                                 ;
}
