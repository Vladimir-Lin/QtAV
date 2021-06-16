#include <qtav.h>

N::Universe * N::WaveCreator(void *,QObject * parent)
{
  return new N::WaveUniverse ( parent ) ;
}

N::Universe * N::CameraCreator(void *,QObject * parent)
{
  return new N::CameraBox ( parent ) ;
}

N::Universe * N::VideoCreator(void *,QObject * parent)
{
  return new N::VideoUniverse ( parent ) ;
}
