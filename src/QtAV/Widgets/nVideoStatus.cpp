#include <qtav.h>
#include "ui_nVideoStatus.h"

N::VideoStatus:: VideoStatus ( QWidget * parent , Plan * p )
               : Widget      (           parent ,        p )
               , ui          ( new Ui::nVideoStatus        )
               , stream      ( NULL                        )
               , vcf         ( NULL                        )
               , TimeId      ( -1                          )
               , TimeID      ( -1                          )
               , closeIt     ( false                       )
{
  WidgetClass            ;
  ui -> setupUi ( this ) ;
}

N::VideoStatus::~VideoStatus(void)
{
  delete ui ;
}

void N::VideoStatus::changeEvent(QEvent * e)
{
  QWidget::changeEvent ( e )       ;
  switch ( e -> type ( ) )         {
    case QEvent::LanguageChange    :
      ui -> retranslateUi ( this ) ;
    break                          ;
    default                        :
    break                          ;
  }
}

void N::VideoStatus::timerEvent(QTimerEvent * e)
{
  QWidget::timerEvent ( e )           ;
  if ( closeIt )                      {
    closeIt = false                   ;
    killTimer ( TimeID )              ;
    TimeID  = -1                      ;
    stream  = NULL                    ;
    vcf     = NULL                    ;
  } else                              {
    if ( e -> timerId ( ) == TimeId ) {
      Refresh ( )                     ;
    }                                 ;
  }                                   ;
}

bool N::VideoStatus::isStopped(void)
{
  return ( TimeID < 0 ) ;
}

void N::VideoStatus::Finalize(void)
{
  TimeId  = -1   ;
  closeIt = true ;
}

void N::VideoStatus::Start(void)
{
  TimeID = startTimer ( 200 ) ;
  TimeId = TimeID             ;
}

void N::VideoStatus::Close(void)
{
  if ( IsNull ( stream ) ) return          ;
  emit Close ( stream -> ThreadId , this ) ;
}

void N::VideoStatus::Refresh(void)
{
  if ( IsNull ( stream ) ) return                                            ;
  qint64 frames                                                              ;
  qint64 secs    = stream -> AudioBuffer . Seconds ( )                       ;
  qint64 msecs   = stream -> msecs ( )                                       ;
  qint64 vframes = stream -> VideoStream -> nb_frames                        ;
  qint64 va      = stream -> VideoAt                                         ;
  qint64 vn      = ( va + 1 ) % stream -> VideoFrames                        ;
  qint64 CT      = 0                                                         ;
  qint64 DT                                                                  ;
  qint64 ET                                                                  ;
  qint64 ST                                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  vframes *= 1000                                                            ;
  vframes /= msecs                                                           ;
  if ( stream -> VideoAt < 0 )                                               {
    frames = stream -> VideoPending                                          ;
  } else
  if ( stream -> VideoAt == stream -> VideoPending )                         {
    frames = stream -> VideoFrames                                           ;
  } else                                                                     {
    frames = stream -> VideoPending - stream -> VideoAt                      ;
    if ( stream -> VideoPending < stream -> VideoAt )                        {
      frames += stream -> VideoFrames                                        ;
    }                                                                        ;
  }                                                                          ;
  if ( va >= 0 )                                                             {
    DT = stream -> transmission -> Elapsed ( va )                            ;
    CT = stream -> transmission -> Elapsed ( vn )                            ;
    ST = CT - DT                                                             ;
    ET = stream -> transmission -> Elapsed (    )                            ;
  }                                                                          ;
  ui -> AudioBuffer  -> setValue ( secs                                    ) ;
  ui -> VideoBuffer  -> setRange ( 0 , stream -> VideoFrames               ) ;
  ui -> VideoBuffer  -> setValue ( frames                                  ) ;
  ui -> VideoIndex   -> setText  ( tr("At : %1"     ).arg( stream->VideoAt      ) ) ;
  ui -> VideoPending -> setText  ( tr("Pending : %1").arg( stream->VideoPending ) ) ;
  ui -> FPS          -> setText  ( tr("FPS : %1"    ).arg( vframes       ) ) ;
  ui -> Next         -> setText  ( tr("dT : %1"     ).arg( ST            ) ) ;
}
