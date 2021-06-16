#include <qtav.h>

N::VideoConduit:: VideoConduit ( QObject * parent )
                : Conduit      (           parent )
                , channel      ( NULL             )
{
}

N::VideoConduit:: VideoConduit ( QObject * parent , VideoChannel * video )
                : Conduit      (           parent                        )
                , channel      ( NULL                                    )
{
  assign ( video ) ;
}

N::VideoConduit::~VideoConduit(void)
{
}

bool N::VideoConduit::Outflow(int junctionType)
{
  nKickOut ( junctionType!=2001 , false ) ;
  return true                             ;
}

bool N::VideoConduit::Into(int junctionType)
{ Q_UNUSED ( junctionType ) ;
  return false              ;
}

bool N::VideoConduit::Throught(Conduit * PreviousConduit)
{ Q_UNUSED ( PreviousConduit ) ;
  return false                 ;
}

bool N::VideoConduit::Has(void)
{
  nKickOut ( IsNull(channel)            , false ) ;
  nKickOut ( !channel->channelHasData() , false ) ;
  return true                                     ;
}

bool N::VideoConduit::Transit(void)
{
  return true ;
}

void N::VideoConduit::assign(VideoChannel * video)
{
  channel = video              ;
  nDropOut ( IsNull(channel) ) ;
}
