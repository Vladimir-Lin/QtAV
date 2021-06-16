#include <AV>
#include <windows.h>

#ifdef LINUX
typedef struct {
  pthread_t Thread;
  int       Priority;
  int       Status;
  int       running;
  void *  (*func)(void * arg);
} thread_data;
#endif

#ifdef MACOSX
typedef struct {
  pthread_t Thread;
  int       Priority;
  int       Status;
  int       running;
  void *  (*func)(void * arg);
} thread_data;
#endif

#ifdef WIN32
typedef struct {
  HANDLE    Thread     ;
  int       Priority   ;
  int       Status     ;
  int       running    ;
  DWORD   (*func)(LPVOID arg);
  DWORD     dwThreadID ;
} thread_data;
#endif

#ifdef LINUX
static void * mediaplayer_run(void * arg)
{
  MoviePlayer * play = (MoviePlayer *)arg;
  play->run();
  pthread_exit(play);
  return NULL;
}
#endif

#ifdef MACOSX
static void * mediaplayer_run(void * arg)
{
  MoviePlayer * play = (MoviePlayer *)arg;
  play->run();
  pthread_exit(play);
  return NULL;
}
#endif

#ifdef WIN32
static DWORD mediaplayer_run(LPVOID arg)
{
  nMediaPlayer * play = (nMediaPlayer *)arg ;
  play->run()                               ;
  return 0                                  ;
}
#endif

void nMediaPlayer::CreateThread(void)
{
  if (PrivateThread!=NULL) delete PrivateThread;
  PrivateThread = (void *)new thread_data();
  memset(PrivateThread,0,sizeof(thread_data));
}

void nMediaPlayer::DestroyThread(void)
{
  thread_data * ThreadData = (thread_data *)PrivateThread;
  delete ThreadData;
  PrivateThread = NULL;
}

void nMediaPlayer::start(void)
{
  thread_data * ThreadData = (thread_data *)PrivateThread;
  #ifdef LINUX
  int th_retcode;

  ThreadData->func    = mediaplayer_run;
  do {
    th_retcode = pthread_create(
                   &ThreadData->Thread ,
                   NULL                ,
                   ThreadData->func    ,
                   (void *)this
                 );
    if (th_retcode!=0) usleep(10*1000);
  } while (th_retcode!=0);
  ThreadData->Status = th_retcode;
  #endif

  #ifdef MACOSX
  int th_retcode;

  ThreadData->func    = mediaplayer_run;
  do {
    th_retcode = pthread_create(
                   &ThreadData->Thread ,
                   NULL               ,
                   ThreadData->func    ,
                   (void *)this
                 );
    if (th_retcode!=0) usleep(10*1000);
  } while (th_retcode!=0);
  ThreadData->Status = th_retcode;
  #endif

  #ifdef WIN32
  ThreadData->func    = mediaplayer_run;
  do {
    ThreadData->Thread =
      ::CreateThread                             (
        NULL                                     ,
        0                                        ,
        (LPTHREAD_START_ROUTINE)ThreadData->func ,
        (LPVOID)this                             ,
        0                                        ,
        &ThreadData->dwThreadID                ) ;
    if (ThreadData->Thread==NULL) ::Sleep(10);
  } while (ThreadData->Thread==NULL);
  ThreadData->Status = 0;
//  SetThreadPriority(ThreadData->Thread,THREAD_PRIORITY_TIME_CRITICAL);
  #endif
}
