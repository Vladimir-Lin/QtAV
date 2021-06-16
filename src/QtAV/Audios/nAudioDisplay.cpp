#include <qtav.h>

N::AudioDisplay:: AudioDisplay (QWidget * parent,Plan * p)
                : PaintWidget  (          parent,       p)
                , junction     (NULL                     )
                , acceptor     (NULL                     )
                , channel      (NULL                     )
                , Audio        (NULL                     )
                , Range        (0,1000                   )
                , interval     (100                      )
                , Thickness    (0.08                     )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::AudioDisplay::~AudioDisplay (void)
{
}

QSize N::AudioDisplay::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 320 , 320 ) ) ;
}

int N::AudioDisplay::Interval(void)
{
  return interval ;
}

void N::AudioDisplay::setInterval(int value)
{
  interval = value ;
}

void N::AudioDisplay::closeEvent (QCloseEvent * event)
{
  stop                 (       ) ;
  QWidget:: closeEvent ( event ) ;
}

void N::AudioDisplay::Configure(void)
{
}

void N::AudioDisplay::relocation(void)
{
  QColor   w = QColor ( 255 , 255 , 255           ) ;
  QSize    s = size   (                           ) ;
  LockGui             (                           ) ;
  Painting   = QImage ( s , QImage::Format_ARGB32 ) ;
  Background = QImage ( s , QImage::Format_ARGB32 ) ;
  Drawing    = QImage ( s , QImage::Format_ARGB32 ) ;
  Painting   . fill   ( w                         ) ;
  Background . fill   ( w                         ) ;
  Drawing    . fill   ( w                         ) ;
  if (NotNull(Audio))                               {
    for (int i=0;i<Audio->Channels();i++)           {
      Place ( i , Audio->Channels() , s )           ;
    }                                               ;
  }                                                 ;
  UnlockGui           (                           ) ;
}

void N::AudioDisplay::Place(int channel,int channels,QSize s)
{
  int    rate = Audio -> Rate ( )       ;
  double sx   = s . width  ( )          ;
  double sy   = s . height ( )          ;
  if ( channels == 1 )                  {
    sx /= rate                          ;
    sy /= Bounds . Distance ( )         ;
    QSizeF  SS(sx,-sy)                  ;
    QPointF SP(0,s.height()/2)          ;
    Shifts [ 0 ] = SP                   ;
    Scales [ 0 ] = SS                   ;
  } else                                {
    double h = s.height()               ;
    sx /= rate                          ;
    sy /= Bounds . Distance ( )         ;
    sy /= channels                      ;
    h  /= 4                             ;
    h  *= ((channel*2)+1)               ;
    QSizeF  SS(sx,-sy)                  ;
    QPointF SP(0,h)                     ;
    Shifts [ channel ] = SP             ;
    Scales [ channel ] = SS             ;
  }                                     ;
  ///////////////////////////////////////
  Thickness  = sx * 4                   ;
  switch (channel)                      {
    case 0                              :
      PENs[0] = QPen(QColor(  0,0,255)) ;
      PENs[0] . setWidthF ( Thickness ) ;
    break                               ;
    case 1                              :
      PENs[1] = QPen(QColor(255,0,  0)) ;
      PENs[1] . setWidthF ( Thickness ) ;
    break                               ;
  }                                     ;
}

void N::AudioDisplay::DrawWave(void)
{
  LockGui              (                           ) ;
  ////////////////////////////////////////////////////
  QPainter d                                         ;
  QPainter x                                         ;
  int      channels = Audio -> Channels ( )          ;
  d . begin         ( &Drawing           )           ;
  d . drawImage     ( 0 , 0 , Background )           ;
  ////////////////////////////////////////////////////
  for (int i=0;i<channels;i++)                       {
    d . setPen  ( PENs[i]                          ) ;
    PaintVector ( Scales[i],Shifts[i],Vectors[i],d ) ;
  }                                                  ;
  ////////////////////////////////////////////////////
  d . end        (                    )              ;
  ////////////////////////////////////////////////////
  x . begin      ( &Painting          )              ;
  x . drawImage  ( 0 , 0 , Drawing    )              ;
  x . end        (                    )              ;
  ////////////////////////////////////////////////////
  UnlockGui      (                    )              ;
  emit reflush   (                    )              ;
}

void N::AudioDisplay::run(int type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( type ) {
    case 1001       :
      reading ( )   ;
    break           ;
  }                 ;
}

void N::AudioDisplay::startup(void)
{
  start ( 1001 ) ;
}

void N::AudioDisplay::reading(void)
{
  nDropOut ( IsNull(Audio) )                       ;
  AudioData DATA                                   ;
  QDateTime T                                      ;
  QDateTime C                                      ;
  int       rate     = Audio -> Rate     ( )       ;
  int       bits     = Audio -> Bits     ( )       ;
  int       channels = Audio -> Channels ( )       ;
  int       dt       = 0                           ;
  int       bpf                                    ;
  char    * src                                    ;
  //////////////////////////////////////////////////
  DATA . set                                       (
    rate                                           ,
    bits                                           ,
    channels                                       ,
    (Acoustics::SampleFormat)Audio->Format())      ;
  DATA . buffer ( 1                         )      ;
  src = DATA.index(0)                              ;
  memset ( src , 0 , DATA.BytesPerSecond()  )      ;
  bpf = DATA.BytesPerFrame()                       ;
  //////////////////////////////////////////////////
  for (int i=0;i<channels;i++)                     {
    Vectors[i] = Vector ( Cpp :: Double , 0 )      ;
  }                                                ;
  //////////////////////////////////////////////////
  Bounds . L = Audio -> lowerBound ( )             ;
  Bounds . R = Audio -> upperBound ( )             ;
  //////////////////////////////////////////////////
  for (int i=0;i<Audio->Channels();i++)            {
    Place ( i , Audio->Channels() , size() )       ;
  }                                                ;
  //////////////////////////////////////////////////
  T = QDateTime::currentDateTime ( )               ;
  while ( NotNull(Audio) && Audio->AudioSize()>0 ) {
    C  = QDateTime::currentDateTime ( )            ;
    dt = T . msecsTo ( C )                         ;
    if (dt>0)                                      {
      if (dt>1000) dt = 1000                       ;
      int ds = DATA.BytesPerSecond()               ;
      ds *= dt                                     ;
      ds /= 1000                                   ;
      if (ds>Audio->AudioSize())                   {
        ds  = Audio->AudioSize()                   ;
      }                                            ;
      ds /= bpf                                    ;
      ds *= bpf                                    ;
      if (ds>0)                                    {
        Audio       -> Lock   (          )         ;
        src = Audio -> index  ( 0        )         ;
        DATA         . add    ( src , ds )         ;
        Audio       -> takeAt ( 0   , ds )         ;
        Audio       -> Unlock (          )         ;
      }                                            ;
    }                                              ;
    ////////////////////////////////////////////////
    dt = DATA . AudioSize ( )                      ;
    if (dt>DATA.BytesPerSecond())                  {
      dt  -= DATA.BytesPerSecond()                 ;
      DATA . takeAt ( 0 , dt )                     ;
    }                                              ;
    for (int i=0;i<channels;i++)                   {
      DATA . toVector ( Vectors[i] , i )           ;
    }                                              ;
    DrawWave ( )                                   ;
    ////////////////////////////////////////////////
    Time :: msleep ( interval )                    ;
    T = C                                          ;
  }                                                ;
}

void N::AudioDisplay::stop(void)
{
  Audio = NULL ;
}
