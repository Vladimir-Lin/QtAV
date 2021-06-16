#include <AV>

//#include "SDL.h"
//#include "SDL_audio.h"
//#include "SDL_mutex.h"

//#include "portaudio.h"

//////////////////
// SDL Supports
//////////////////

#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
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

void nMediaPlayer::CreateDevices(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (PrivateDevice!=NULL) delete PrivateDevice;
  localSDL * sdl = new localSDL() ;
  PrivateDevice  = (void *)sdl    ;
  sdl->Screen    = NULL           ;
  sdl->YUV       = NULL           ;
  sdl->Audio     = NULL           ;
  sdl->Spec      = NULL           ;
  sdl->Mutex     = NULL           ;
  sdl->stream    = NULL           ;
#endif
}

void nMediaPlayer::DestroyDevices(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  SDL_Quit();
  if (PrivateDevice!=NULL) delete PrivateDevice;
  PrivateDevice = NULL;
#endif
}

void nMediaPlayer::Surface(QSize screen)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  Screen = screen ;
  setAttribute(Qt::WA_PaintOnScreen);
  localSDL * sdl = (localSDL *)PrivateDevice;
  char variable[256];
  sprintf(variable,"SDL_WINDOWID=0x%lx", winId());
  putenv(variable);
  ::SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) ;
  ::SDL_Init         (SDL_INIT_VIDEO                       ) ;
  sdl->Screen = ::SDL_SetVideoMode                           (
                  screen . width  ()                         ,
                  screen . height ()                         ,
                  32                                         ,
                  SDL_HWSURFACE                              |
                  SDL_ASYNCBLIT                              |
                  SDL_DOUBLEBUF                            ) ;
#endif
}

void nMediaPlayer::CreateDisplay(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  localSDL * sdl = (localSDL *)PrivateDevice;
  sdl->YUV = ::SDL_CreateYUVOverlay(
                 Screen . width () ,
                 Screen . height() ,
                 SDL_YV12_OVERLAY  ,
                 sdl->Screen     ) ;
  if (Scaling) {
    sdl->Rect.x  = width () - Screen.width () ;
    sdl->Rect.y  = height() - Screen.height() ;
    sdl->Rect.x /= 2;
    sdl->Rect.y /= 2;
    sdl->Rect.w  = Screen.width () ;
    sdl->Rect.h  = Screen.height() ;
  } else {
    sdl->Rect.x  = 0 ;
    sdl->Rect.y  = 0 ;
    sdl->Rect.w  = Screen.width () ;
    sdl->Rect.h  = Screen.height() ;
  };

  unsigned char * data     [4] ;
  int             linesize [4] ;

  data    [0] = sdl->YUV->pixels  [0] ;
  data    [1] = sdl->YUV->pixels  [2] ;
  data    [2] = sdl->YUV->pixels  [1] ;
  data    [3] = NULL                  ;
  linesize[0] = sdl->YUV->pitches [0] ;
  linesize[1] = sdl->YUV->pitches [2] ;
  linesize[2] = sdl->YUV->pitches [1] ;
  linesize[3] = 0                     ;

  setVideoOutput(Screen,data,linesize);
#endif
}

void nMediaPlayer::prepareDisplay(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  localSDL * sdl = (localSDL *)PrivateDevice ;
  SDL_LockYUVOverlay(sdl->YUV)               ;
#endif
}

void nMediaPlayer::display(void)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  localSDL * sdl = (localSDL *)PrivateDevice  ;
  SDL_UnlockYUVOverlay  (sdl->YUV           ) ;
  SDL_DisplayYUVOverlay (sdl->YUV,&sdl->Rect) ;
#endif
}
