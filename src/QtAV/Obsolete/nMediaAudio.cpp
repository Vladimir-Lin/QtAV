#include <AV>

//#include "SDL.h"
//#include "SDL_audio.h"
//#include "SDL_mutex.h"

//#include "portaudio.h"

//////////////////
// SDL Supports
//////////////////

#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

typedef struct                        {
  SDL_Surface      * Screen           ;
  SDL_Overlay      * YUV              ;
  SDL_Rect           Rect             ;
  SDL_AudioSpec    * Audio            ;
  SDL_AudioSpec    * Spec             ;
  SDL_mutex        * Mutex            ;
  PaStreamParameters outputParameters ;
  PaStream         * stream           ;
} localSDL                            ;
#endif

static int paCallback( const void                     * inputBuffer     ,
                       void                           * outputBuffer    ,
                       unsigned long                    framesPerBuffer ,
                       const PaStreamCallbackTimeInfo * timeInfo        ,
                             PaStreamCallbackFlags      statusFlags     ,
                       void                           * userData        )
{
  nMediaPlayer * play = (nMediaPlayer *)userData          ;
  long long len = framesPerBuffer                         ;
  len *= play->Channels()                                 ;
  len *= play->PlayBits()                                 ;
  play->Feed((unsigned char *)outputBuffer,(int)len)      ;
  return paContinue                                       ;
}

void nMediaPlayer::CreateAudio(int BufferSize)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  localSDL * sdl = (localSDL *)PrivateDevice;

  PaError              err        ;

  sdl->stream = NULL;
  err = Pa_Initialize();
  if (err!=paNoError ) return;

  sdl->outputParameters.device = Pa_GetDefaultOutputDevice() ;

  if (Plan->Equipments.contains(nPlan::DefaultAudioOutput))  {
    QTextCodec * codec = QTextCodec::codecForLocale()        ;
    const   PaDeviceInfo * deviceInfo                        ;
    int     ds = Pa_GetDeviceCount ()                        ;
    QString AD = Plan->Equipments[nPlan::DefaultAudioOutput] ;
    for (int ii=0;ii<ds;ii++)                                {
      deviceInfo = Pa_GetDeviceInfo(ii)                      ;
      if (deviceInfo->maxOutputChannels>0)                   {
        QByteArray B(deviceInfo->name)                       ;
        if (B.size()>0)                                      {
          QString Name = codec->toUnicode(deviceInfo->name)  ;
          if (Name==AD) sdl->outputParameters.device = ii    ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  }                                                          ;

  if (sdl->outputParameters.device <  0         ) return;
  if (sdl->outputParameters.device == paNoDevice) return;

  sdl->outputParameters.channelCount              = Channels()           ;
  sdl->outputParameters.suggestedLatency          = Pa_GetDeviceInfo(sdl->outputParameters.device)->defaultLowOutputLatency;
  sdl->outputParameters.hostApiSpecificStreamInfo = NULL;

  switch (SampleBits())                                {
    case  8                                            :
      sdl->outputParameters.sampleFormat = paUInt8     ;
    break                                              ;
    case 16                                            :
      sdl->outputParameters.sampleFormat = paInt16     ;
    break                                              ;
    case 32                                            :
      if (isFloat())
        sdl->outputParameters.sampleFormat = paFloat32 ;
      else
        sdl->outputParameters.sampleFormat = paInt32   ;
    break                                              ;
    default                                            :
      sdl->outputParameters.sampleFormat = paInt32     ;
    break                                              ;
  }                                                    ;

  err = Pa_OpenStream(
          &sdl->stream           ,
          NULL                   ,
          &sdl->outputParameters ,
          SampleRate()           ,
          BufferSize             ,
          paClipOff              ,
          paCallback             ,
          this
        );
  if (err!=paNoError) return;
  err = Pa_StartStream(sdl->stream);
  if (err!=paNoError) return;
#endif

}

void nMediaPlayer::stopAudio(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  localSDL * sdl = (localSDL *)PrivateDevice;

  PaError err;
  if (sdl->stream==NULL) return;
  err = Pa_StopStream(sdl->stream);
  if (err!=paNoError) return;
  err = Pa_CloseStream(sdl->stream);
  if (err!=paNoError) return;
  Pa_Terminate();
#endif
}

void nMediaPlayer::lockAudio(void)
{
//  AudioMutex.lock();
}

void nMediaPlayer::unlockAudio(void)
{
//  AudioMutex.unlock();
}

bool nMediaPlayer::Feed(unsigned char * stream,int length)
{
  if (Halting) return false                    ;
  return FeedAudio(stream,length)              ;
}
