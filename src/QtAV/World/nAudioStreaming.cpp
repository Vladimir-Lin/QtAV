#include <qtav.h>

N::AudioStreaming:: AudioStreaming     (void)
                  : CiosAudio::Conduit (    )
                  , stream             (NULL)
{
  Output . Result = CiosAudio::Conduit::Continue ;
}

N::AudioStreaming::~AudioStreaming (void)
{
}

int N::AudioStreaming::obtain(void)
{
  if ( CiosAudio::Conduit::Complete == Output . Result )                  {
    return Output . Result                                                ;
  }                                                                       ;
  if ( NULL == stream ) return Output . Result                            ;
  if ( ! stream -> ObtainAudio ( Output . Buffer , Output . Total ( ) ) ) {
    return CiosAudio::Conduit::Complete                                   ;
  }                                                                       ;
  return Output . Result                                                  ;
}

int N::AudioStreaming::put(void)
{
  return CiosAudio::Conduit::Complete ;
}

void N::AudioStreaming::finish    (
       ConduitDirection direction ,
       FinishCondition  condition )
{
  if ( CiosAudio::Conduit::OutputDirection == direction ) {
    Output . Situation = CiosAudio::StreamIO::Completed   ;
  }                                                       ;
  if ( CiosAudio::Conduit::InputDirection  == direction ) {
    Input  . Situation = CiosAudio::StreamIO::Completed   ;
  }                                                       ;
}
