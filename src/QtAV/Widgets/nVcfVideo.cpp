#include <qtav.h>

N::VcfVideo:: VcfVideo  ( QObject * parent , QGraphicsItem * item , Plan * plan )
            : VcfCanvas (           parent ,                 item ,        plan )
{
}

N::VcfVideo::~VcfVideo (void)
{
}

void N::VcfVideo::contextMenuEvent(QGraphicsSceneContextMenuEvent * e)
{
  if (   isFunction ( 32001 ) )           {
    QGraphicsItem::contextMenuEvent ( e ) ;
  } else                                  {
    if ( isFunction ( 32002 ) )           {
      emit Menu   ( this , e -> pos ( ) ) ;
    }                                     ;
    e -> accept (                       ) ;
  }                                       ;
}

void N::VcfVideo::Paint(QPainter * p,QRectF,bool,bool)
{
  pushPainters             ( p                          ) ;
  switch (Mode)                                           {
    case Empty                                            :
    break                                                 ;
    case Border                                           :
      Painter . drawBorder ( p , "Default" , ScreenRect ) ;
    break                                                 ;
    case Board                                            :
      Painter . drawRect   ( p , "Default" , ScreenRect ) ;
    break                                                 ;
    case Label                                            :
      Painter . drawRect   ( p , "Default" , ScreenRect ) ;
    break                                                 ;
    case Attache                                          :
      Painter . drawRect   ( p , "Default" , ScreenRect ) ;
    break                                                 ;
    case Capture                                          :
      Painter . drawRect   ( p , "Default" , ScreenRect ) ;
    break                                                 ;
  }                                                       ;
  popPainters              ( p                          ) ;
}
