#include <qtav.h>

N::VideoAcceptor:: VideoAcceptor ( QObject * parent )
                 : Conduit       (           parent )
                 , acceptor      ( NULL             )
{
}

N::VideoAcceptor:: VideoAcceptor ( QObject * parent , SyncAcceptor * renderer )
                 : Conduit       (           parent                           )
                 , acceptor      (                                   renderer )
{
}

N::VideoAcceptor::~VideoAcceptor(void)
{
}

bool N::VideoAcceptor::Into(int junctionType)
{
  nKickOut ( junctionType!=2001 , false ) ;
  return true                             ;
}

bool N::VideoAcceptor::Throught(Conduit * conduit)
{
  nKickOut ( IsNull(conduit)       , false )  ;
  nKickOut ( conduit->type()!=2001 , false )  ;
  nKickOut ( IsNull(acceptor)      , false )  ;
  VideoConduit * vc = (VideoConduit *)conduit ;
  nKickOut ( IsNull(vc->channel)   , false )  ;
  acceptor -> Sync ( vc -> channel )          ;
  return true                                 ;
}

bool N::VideoAcceptor::Demolish(Conduit * Faucet)
{
  nKickOut ( Faucet->type()!=2001 , false ) ;
  nKickOut ( IsNull(acceptor)     , false ) ;
  acceptor -> Demolish ( )                  ;
  acceptor  = NULL                          ;
  return true                               ;
}
