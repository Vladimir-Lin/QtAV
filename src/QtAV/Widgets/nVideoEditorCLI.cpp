#include <qtav.h>

N::VideoEditorCLI:: VideoEditorCLI    ( QWidget * parent , Plan * p )
                  : InteractiveEditor (           parent ,        p )
                  , vcf               ( NULL                        )
                  , scroll            ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VideoEditorCLI::~VideoEditorCLI(void)
{
}

QSize N::VideoEditorCLI::sizeHint(void) const
{
  return SizeSuggestion ( QSize ( 640 , 240 ) ) ;
}

void N::VideoEditorCLI::Configure(void)
{
  Prompt = "> "                                  ;
  nConnect ( this , SIGNAL ( SafeCommand ( ) )   ,
             this , SLOT   ( SendCommand ( ) ) ) ;
}

void N::VideoEditorCLI::SendCommand(void)
{
  if ( CurrentCommand . length ( ) > 0 ) {
    emit Commanding ( CurrentCommand )   ;
  }                                      ;
  if ( CurrentCommands . count ( ) > 0 ) {
    emit Commanding ( CurrentCommands )  ;
  }                                      ;
}

void N::VideoEditorCLI::run(int type,ThreadData * data)
{
  switch ( type )                                                      {
    case 10001                                                         :
      if ( data -> Arguments . count ( ) > 0 )                         {
        RunCommandsFromFile ( data -> Arguments [ 0 ] . toString ( ) ) ;
      }                                                                ;
    break                                                              ;
    case 10002                                                         :
      if ( data -> Arguments . count ( ) > 0 )                         {
        SaveCommandsToFile  ( data -> Arguments [ 0 ] . toString ( ) ) ;
      }                                                                ;
    break                                                              ;
  }                                                                    ;
}

void N::VideoEditorCLI::RunCommandsFromFile(QString filename)
{
  QStringList s                               ;
  s = File :: StringList  ( filename , "\n" ) ;
  s = File :: PurifyLines ( s               ) ;
  if ( s . count ( ) <= 0 ) return            ;
  QString c                                   ;
  foreach ( c , s )                           {
    Commands = c                              ;
    Enter ( )                                 ;
  }                                           ;
}

void N::VideoEditorCLI::SaveCommandsToFile(QString filename)
{
  if ( filename        . length ( ) <= 0 ) return ;
  if ( HistoryCommands . count  ( ) <= 0 ) return ;
  QString s = HistoryCommands . join ( "\r\n" )   ;
  if ( s . length ( ) <= 0 ) return               ;
  QByteArray B = s . toUtf8 ( )                   ;
  File::toFile ( filename , B )                   ;
  Alert        ( Done         )                   ;
}

void N::VideoEditorCLI::Load(QString filename)
{
  VarArgs args                                               ;
  if ( filename . length ( ) <= 0 )                          {
    filename = plan->Path(Directory::Scripts).absolutePath() ;
    filename = QFileDialog::getOpenFileName                  (
                 this                                        ,
                 tr("Load command from file")                ,
                 filename                                    ,
                 tr("Plain text (*.txt)")                  ) ;
    if ( filename . length ( ) <= 0 ) return                 ;
  }                                                          ;
  args << filename                                           ;
  start ( 10001 , args )                                     ;
}

void N::VideoEditorCLI::SaveTo(QString filename)
{
  VarArgs args                                               ;
  if ( filename . length ( ) <= 0 )                          {
    filename = plan->Path(Directory::Scripts).absolutePath() ;
    filename = QFileDialog::getSaveFileName                  (
                 this                                        ,
                 tr("Save history to file")                  ,
                 filename                                    ,
                 tr("Plain text (*.txt)")                  ) ;
    if ( filename . length ( ) <= 0 ) return                 ;
  }                                                          ;
  args << filename                                           ;
  start ( 10002 , args )                                     ;
}

void N::VideoEditorCLI::AcceptMessage(QString message)
{
  Report ( message ) ;
  Report ( "\n"    ) ;
}

void N::VideoEditorCLI::Enter(void)
{
  if (Commands.length()<=0) return ;
  Report ( Commands )              ;
  Report ( "\n"     )              ;
  if (Interpret(Commands))         {
    Queues  << Commands            ;
  }                                ;
  Commands = ""                    ;
}

bool N::VideoEditorCLI::Interpret(QString command)
{
  if ( command . length ( ) <= 0 ) return false            ;
  //////////////////////////////////////////////////////////
  QStringList C = N::CommandTokens ( command )             ;
  CurrentCommand  = command                                ;
  CurrentCommands = C                                      ;
  //////////////////////////////////////////////////////////
  if ( CurrentCommand . length ( ) > 0 )                   {
    HistoryCommands << CurrentCommand                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if ( C . count ( ) > 0 )                                 {
    ////////////////////////////////////////////////////////
    if ( "clear" == C [ 0 ] . toLower ( ) )                {
      if ( C . count ( ) > 1 )                             {
        if ( "message" == C [ 1 ] . toLower ( ) )          {
          Clear ( )                                        ;
          return true                                      ;
        }                                                  ;
      } else                                               {
        Clear ( )                                          ;
        return true                                        ;
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
    if ( "hide" == C [ 0 ] . toLower ( ) )                 {
      if ( C . count ( ) > 1 )                             {
      } else                                               {
        vcf -> hide ( )                                    ;
        return true                                        ;
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
    if ( "set" == C [ 0 ] . toLower ( ) )                  {
      if ( C . count ( ) > 1 )                             {
        if ( "interface" == C [ 1 ] . toLower ( ) )        {
          if ( C . count ( ) > 2 )                         {
            if ( "opacity" == C [ 2 ] .toLower ( ) )       {
              if ( C . count ( ) > 3 )                     {
                bool   okay = false                        ;
                double v    = C [ 3 ] . toDouble ( &okay ) ;
                if ( okay )                                {
                  if ( ( v > 0 ) && ( v <= 1.0 ) )         {
                    if ( NotNull ( vcf ) )                 {
                      vcf -> setOpacity ( v )              ;
                      return true                          ;
                    }                                      ;
                  }                                        ;
                }                                          ;
              }                                            ;
            }                                              ;
          }                                                ;
        }                                                  ;
        ////////////////////////////////////////////////////
        if ( "prompt" == C [ 1 ] . toLower ( ) )           {
          if ( C . count ( ) > 2 )                         {
            if ( "as" == C [ 2 ] .toLower ( ) )            {
              if ( C . count ( ) > 3 )                     {
                Prompt = C [ 3 ]                           ;
                return true                                ;
              }                                            ;
            }                                              ;
          }                                                ;
        }                                                  ;
        ////////////////////////////////////////////////////
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
    if ( "list" == C [ 0 ] . toLower ( ) )                 {
      if ( C . count ( ) > 1 )                             {
        if ( "history" == C [ 1 ] . toLower ( ) )          {
          if ( C . count ( ) > 2 )                         {
          } else                                           {
            QString s                                      ;
            foreach ( s , HistoryCommands )                {
              Report ( s + "\n" )                          ;
            }                                              ;
            return true                                    ;
          }                                                ;
        }                                                  ;
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
    if ( "load" == C [ 0 ] . toLower ( ) )                 {
      if ( C . count ( ) > 1 )                             {
        if ( "command" == C [ 1 ] . toLower ( ) )          {
          if ( C . count ( ) > 2 )                         {
            if ( "from" == C [ 2 ] . toLower ( ) )         {
              QString filename = ""                        ;
              if ( C . count ( ) > 3 )                     {
                filename = C [ 3 ]                         ;
              }                                            ;
              Load ( filename )                            ;
              return true                                  ;
            }                                              ;
          }                                                ;
        }                                                  ;
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
    if ( "save" == C [ 0 ] . toLower ( ) )                 {
      if ( C . count ( ) > 1 )                             {
        if ( "history" == C [ 1 ] . toLower ( ) )          {
          if ( C . count ( ) > 2 )                         {
            if ( "to" == C [ 2 ] . toLower ( ) )           {
              QString filename = ""                        ;
              int     cix      = HistoryCommands.count()   ;
              if ( C . count ( ) > 3 )                     {
                filename = C [ 3 ]                         ;
              }                                            ;
              cix = cix - 1                                ;
              if ( cix >= 0 ) HistoryCommands.takeAt(cix)  ;
              SaveTo ( filename )                          ;
              return true                                  ;
            }                                              ;
          }                                                ;
        }                                                  ;
      }                                                    ;
    }                                                      ;
    ////////////////////////////////////////////////////////
  }                                                        ;
  //////////////////////////////////////////////////////////
  emit SafeCommand ( )                                     ;
  return true                                              ;
}

bool N::VideoEditorCLI::isCommanding(void)
{
  if ( IsNull(vcf) ) return false ;
  return vcf -> isVisible ( )     ;
}
