#include <qtav.h>

N::VideoChannel:: VideoChannel (void)
                : Transmission (NULL)
{
}

N::VideoChannel::~VideoChannel(void)
{
}

N::VideoTransmission * N::VideoChannel::NewTransmission(void)
{
  return new VideoTransmission() ;
}

int N::VideoChannel::channelType(void) const
{
  return 0 ;
}
