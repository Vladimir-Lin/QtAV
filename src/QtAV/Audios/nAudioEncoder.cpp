#include <qtav.h>

bool N::Encoders::Convert(Plan * plan,int Codec,QString filename,QString audio,bool progress,QString format)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  QFileInfo FI                                                              ;
  FI . setFile ( filename )                                                 ;
  if (!FI.exists()) return false                                            ;
  ///////////////////////////////////////////////////////////////////////////
  int codecID                                                               ;
  if (!Plan->Encoders.contains(Codec)) return false                         ;
  codecID = Plan->Encoders[Codec]                                           ;
  ///////////////////////////////////////////////////////////////////////////
  nVideoManager VM(Plan)                                                    ;
  void * AVP = VM.AvPacket ()                                               ;
  ///////////////////////////////////////////////////////////////////////////
  if (!VM.OpenPacket(AVP,filename))                                         {
    VM.FreePacket ( AVP                                                   ) ;
    return false                                                            ;
  }                                                                         ;
  if (!VM.FindStream(AVP))                                                  {
    VM.FreePacket ( AVP                                                   ) ;
    return false                                                            ;
  }                                                                         ;
  if (!VM.FindVideo(AVP))                                                   {
    VM.ClosePacket ( AVP                                                  ) ;
    VM.FreePacket  ( AVP                                                  ) ;
    return false                                                            ;
  }                                                                         ;
  if (!VM.FindAudio(AVP))                                                   {
    VM.ClosePacket ( AVP                                                  ) ;
    VM.FreePacket  ( AVP                                                  ) ;
    return false                                                            ;
  }                                                                         ;
  if (!VM.OpenVideo(AVP))                                                   {
    VM.ClosePacket ( AVP                                                  ) ;
    VM.FreePacket  ( AVP                                                  ) ;
    return false                                                            ;
  }                                                                         ;
  if (!VM.OpenAudio(AVP))                                                   {
    VM.ClosePacket ( AVP                                                  ) ;
    VM.FreePacket  ( AVP                                                  ) ;
    return false                                                            ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  QFile         F  ( audio )                                                ;
  nAudioEncoder AE ( Plan  )                                                ;
  int  avSampleRate   = VM . AvSampleRate   (AVP)                           ;
  int  avChannels     = VM . Channels       (AVP)                           ;
  int  avSampleFormat = VM . AvSampleFormat (AVP)                           ;
  ///////////////////////////////////////////////////////////////////////////
  bool success        = false                                               ;
  if (AE.Open(codecID,avSampleRate,avChannels,avSampleFormat))              {
    if (AE.Compatible() && F.open(QIODevice::WriteOnly))                    {
      nScopedProgress * P = NULL                                            ;
      if (progress) P = new nScopedProgress(Plan,format,10)                 ;
      if (progress)                                                         {
        P -> setRange ( 0,VM.Duration(AVP)/1000000 )                        ;
        P -> show     (                            )                        ;
        P -> Start    (                            )                        ;
      }                                                                     ;
      ///////////////////////////////////////////////////////////////////////
      QByteArray AB                                                         ;
      AB.resize(VM.AudioBufferSize(AVP,5000))                               ;
      unsigned char * data    = (unsigned char *)AB.data()                  ;
      int             index   = 0                                           ;
      int             buffer  = 0                                           ;
      qint64          written = 0                                           ;
      int             fs      = AE.FrameSize()                              ;
      int             bpf     = VM.BytesPerFrame(AVP)                       ;
      int             fb      = fs * bpf                                    ;
      bool            decode  = true                                        ;
      ///////////////////////////////////////////////////////////////////////
      while (decode && VM.ReadFrame(AVP)==0)                                {
        if (progress)                                                       {
          long long timestamp = VM.Timestamp(AVP)                           ;
          if (timestamp!=AV_NOPTS_VALUE) P->setValue(timestamp/1000000)     ;
        }                                                                   ;
        if (VM.isAudio(AVP))                                                {
          int length = VM.DecodeAudio(AVP,&data[index])                     ;
          if (length>0)                                                     {
            buffer = index + length                                         ;
            if (buffer>=fb)                                                 {
              int compressed = AE.Encode(F,buffer,data,false)               ;
              F.flush()                                                     ;
              F.waitForBytesWritten(1000)                                   ;
              if (compressed> 0) written += compressed                      ;
              if (compressed<=0) decode = false ;                        else
              if (compressed<buffer)                                        {
                index  = AE.Collect(data,buffer,compressed)                 ;
                buffer = index                                              ;
              } else                                                        {
                buffer = 0                                                  ;
                index  = 0                                                  ;
              }                                                             ;
            } else                                                          {
              index = buffer                                                ;
            }                                                               ;
          } else                                                            {
//            if (progress) Plan->Status->setEnabled(false)                   ;
          }                                                                 ;
        }                                                                   ;
        VM.ReleasePacket(AVP)                                               ;
      }                                                                     ;
      ///////////////////////////////////////////////////////////////////////
      success = true                                                        ;
      if (progress) P->Finish()                                             ;
      Plan -> processEvents  ()                                             ;
      if (buffer>0) AE.Encode(F,buffer,data,true)                           ;
      if (progress) delete P                                                ;
      F.waitForBytesWritten(1000)                                           ;
      F.close()                                                             ;
      if (written<=0)                                                       {
        QFile::remove(audio)                                                ;
        success = false                                                     ;
      }                                                                     ;
    }                                                                       ;
    AE.Close()                                                              ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  VM.ClosePacket ( AVP )                                                    ;
  VM.FreePacket  ( AVP )                                                    ;
  Plan->Status->setEnabled(true)                                            ;
  return success                                                            ;
#endif
  return false ;
}

typedef QList<double> DList ;

bool N::Encoders::BuildAmplitude(Plan & plan,SUID uuid)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  QString Filename                                                          ;
  nSqlConnection SC(Plan.Sql)                                               ;
  if (SC.open("Acoustics","BuildAmplitude"))                                {
    QString Q                                                               ;
    Q = SC.Sql.SelectFrom                                                   (
          "suffix"                                                          ,
          Plan.Tables[Neutrino::Tables::AudioFiles]                         ,
          SC.WhereUuid(uuid))                                               ;
    if (SC.Fetch(Q))                                                        {
      QString Suffix = SC . String   ( 0 )                                  ;
      QString Name   = QString("%1.%2").arg(uuid).arg(Suffix)               ;
      Filename = Plan.AudioFile(Name)                                       ;
      if (Suffix=="raw") Filename = ""                                      ;
    }                                                                       ;
    SC.close()                                                              ;
  }                                                                         ;
  SC.remove()                                                               ;
  nKickOut ( Filename.length() <= 0 , false )                               ;
  ///////////////////////////////////////////////////////////////////////////
  nVideoManager VM(&Plan)                                                   ;
  void * AVP = VM.AvPacket ()                                               ;
  ///////////////////////////////////////////////////////////////////////////
  if (!VM.AudioStream(AVP,Filename)) return false                           ;
  ///////////////////////////////////////////////////////////////////////////
  int      avSampleRate   = VM . AvSampleRate   (AVP)                       ;
  int      avChannels     = VM . Channels       (AVP)                       ;
  int      avSampleFormat = VM . AvSampleFormat (AVP)                       ;
  int      Samples        = avSampleRate / 20                               ;
  int      PerSamples     = Samples * avChannels                            ;
  double   Factor         = 1.0                                             ;
  double * B1             = new double [avSampleRate]                       ;
  double * B2             = new double [avSampleRate]                       ;
  QMap<int,double *> Buffer                                                 ;
  Buffer[0] = B1                                                            ;
  Buffer[1] = B2                                                            ;
  ///////////////////////////////////////////////////////////////////////////
  for (int i=0;i<avSampleRate;i++) B1 [ i ] = 0                             ;
  for (int i=0;i<avSampleRate;i++) B2 [ i ] = 0                             ;
  ///////////////////////////////////////////////////////////////////////////
  switch (avSampleFormat)                                                   {
    case SAMPLE_FMT_U8                                                      :
      Factor  = 1                                                           ;
      Factor /= 127.0                                                       ;
    break                                                                   ;
    case SAMPLE_FMT_S16                                                     :
      Factor  = 1                                                           ;
      Factor /= 32767.0                                                     ;
    break                                                                   ;
    case SAMPLE_FMT_S32                                                     :
      Factor  = 1                                                           ;
      Factor /= 2147483647.0                                                ;
    break                                                                   ;
    case SAMPLE_FMT_FLT                                                     :
    break                                                                   ;
    case SAMPLE_FMT_DBL                                                     :
    break                                                                   ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  QByteArray AB                                                             ;
  AB.resize(VM.AudioBufferSize(AVP,5000))                                   ;
  unsigned char  * data    = (unsigned char *)AB.data()                     ;
  int              index   = 0                                              ;
  int              buffer  = 0                                              ;
  bool             decode  = true                                           ;
  nAbstractFourier NAF                                                      ;
  fftw_complex   * dout1   = NAF.array(avSampleRate)                        ;
  fftw_complex   * dout2   = NAF.array(avSampleRate)                        ;
  fftw_plan        plan1   = NAF.Frequency(Samples,B1,dout1)                ;
  fftw_plan        plan2   = NAF.Frequency(Samples,B2,dout2)                ;
  QMap<int,fftw_plan> plan                                                  ;
  QMap<int,fftw_complex *> dout                                             ;
  QMap<int,double> maximum                                                  ;
  QMap<int,double> minimum                                                  ;
  QMap<int,double> average                                                  ;
  QMap<int,DList> Amplitudes                                                ;
  plan[0] = plan1                                                           ;
  plan[1] = plan2                                                           ;
  dout[0] = dout1                                                           ;
  dout[1] = dout2                                                           ;
  maximum[0] = -100000000000000.0                                           ;
  minimum[0] =  100000000000000.0                                           ;
  average[0] =                0.0                                           ;
  maximum[1] = -100000000000000.0                                           ;
  minimum[1] =  100000000000000.0                                           ;
  average[1] =                0.0                                           ;
  Amplitudes[0] << 0 ; Amplitudes[0].clear()                                ;
  Amplitudes[1] << 0 ; Amplitudes[1].clear()                                ;
  ///////////////////////////////////////////////////////////////////////////
  while (decode && VM.ReadFrame(AVP)==0)                                    {
    if (VM.isAudio(AVP))                                                    {
      int length = VM.DecodeAudio(AVP,&data[index])                         ;
      if (length>0)                                                         {
        int ending   = 0                                                    ;
        int position = 0                                                    ;
        buffer = index + length                                             ;
        switch (avSampleFormat)                                             {
          case SAMPLE_FMT_U8                                                :
            while ((buffer-ending)>PerSamples)                              {
              unsigned char * v = data                                      ;
              for (int i=0;i<avSampleRate;i++) B1 [ i ] = 0                 ;
              for (int i=0;i<avSampleRate;i++) B2 [ i ] = 0                 ;
              for (int i=0;i<Samples;i++)                                   {
                for (int j=0;j<avChannels;j++)                              {
                  Buffer[j][i]  = v[position]                               ;
                  Buffer[j][i] -= 128                                       ;
                  Buffer[j][i] *= Factor                                    ;
                  position ++                                               ;
                }                                                           ;
                ending    += avChannels                                     ;
              }                                                             ;
              for (int i=0;i<avChannels;i++)                                {
                NAF.Execute(plan[i])                                        ;
                double dx                                                   ;
                dx = dout[i][0][0]                                          ;
                if (dx>maximum[i]) maximum[i] = dx                          ;
                if (dx<minimum[i]) minimum[i] = dx                          ;
                Amplitudes[i] << dx                                         ;
                if (dx<0) dx = -dx                                          ;
                average[i]  += dx                                           ;
              }                                                             ;
            }                                                               ;
          break                                                             ;
          case SAMPLE_FMT_S16                                               :
            while ((buffer-ending)>PerSamples)                              {
              short * v = (short *)data                                     ;
              for (int i=0;i<avSampleRate;i++) B1 [ i ] = 0                 ;
              for (int i=0;i<avSampleRate;i++) B2 [ i ] = 0                 ;
              for (int i=0;i<Samples;i++)                                   {
                for (int j=0;j<avChannels;j++)                              {
                  Buffer[j][i]  = v[position]                               ;
                  Buffer[j][i] *= Factor                                    ;
                  position ++                                               ;
                }                                                           ;
                ending    += avChannels * 2                                 ;
              }                                                             ;
              for (int i=0;i<avChannels;i++)                                {
                NAF.Execute(plan[i])                                        ;
                double dx                                                   ;
                dx = dout[i][0][0]                                          ;
                if (dx>maximum[i]) maximum[i] = dx                          ;
                if (dx<minimum[i]) minimum[i] = dx                          ;
                Amplitudes[i] << dx                                         ;
                if (dx<0) dx = -dx                                          ;
                average[i]  += dx                                           ;
              }                                                             ;
            }                                                               ;
          break                                                             ;
          case SAMPLE_FMT_S32                                               :
            while ((buffer-ending)>PerSamples)                              {
              int * v = (int *)data                                         ;
              for (int i=0;i<avSampleRate;i++) B1 [ i ] = 0                 ;
              for (int i=0;i<avSampleRate;i++) B2 [ i ] = 0                 ;
              for (int i=0;i<Samples;i++)                                   {
                for (int j=0;j<avChannels;j++)                              {
                  Buffer[j][i]  = v[position]                               ;
                  Buffer[j][i] *= Factor                                    ;
                  position ++                                               ;
                }                                                           ;
                ending    += avChannels * 4                                 ;
              }                                                             ;
              for (int i=0;i<avChannels;i++)                                {
                NAF.Execute(plan[i])                                        ;
                double dx                                                   ;
                dx = dout[i][0][0]                                          ;
                if (dx>maximum[i]) maximum[i] = dx                          ;
                if (dx<minimum[i]) minimum[i] = dx                          ;
                Amplitudes[i] << dx                                         ;
                if (dx<0) dx = -dx                                          ;
                average[i]  += dx                                           ;
              }                                                             ;
            }                                                               ;
          break                                                             ;
          case SAMPLE_FMT_FLT                                               :
            while ((buffer-ending)>PerSamples)                              {
              float * v = (float *)data                                     ;
              for (int i=0;i<avSampleRate;i++) B1 [ i ] = 0                 ;
              for (int i=0;i<avSampleRate;i++) B2 [ i ] = 0                 ;
              for (int i=0;i<Samples;i++)                                   {
                for (int j=0;j<avChannels;j++)                              {
                  Buffer[j][i]  = v[position]                               ;
                  Buffer[j][i] *= Factor                                    ;
                  position ++                                               ;
                }                                                           ;
                ending    += avChannels * 4                                 ;
              }                                                             ;
              for (int i=0;i<avChannels;i++)                                {
                NAF.Execute(plan[i])                                        ;
                double dx                                                   ;
                dx = dout[i][0][0]                                          ;
                if (dx>maximum[i]) maximum[i] = dx                          ;
                if (dx<minimum[i]) minimum[i] = dx                          ;
                Amplitudes[i] << dx                                         ;
                if (dx<0) dx = -dx                                          ;
                average[i]  += dx                                           ;
              }                                                             ;
            }                                                               ;
          break                                                             ;
          case SAMPLE_FMT_DBL                                               :
          break                                                             ;
        }                                                                   ;
        if ((buffer-ending)>0)                                              {
          index = buffer-ending                                             ;
          memcpy(data,data+ending,index)                                    ;
        } else index = 0                                                    ;
      }                                                                     ;
    }                                                                       ;
    VM.ReleasePacket(AVP)                                                   ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  VM.ClosePacket ( AVP )                                                    ;
  VM.FreePacket  ( AVP )                                                    ;
  ///////////////////////////////////////////////////////////////////////////
  nSqlConnection XC(Plan.Sql)                       ;
  if (XC.open("Acoustics","BuildAmplitude"))        {
    QString Q                                       ;
    Q = XC.Sql.DeleteFrom                           (
          Plan.Tables[Neutrino::Tables::Amplitude]  ,
          XC.WhereUuid(uuid)                      ) ;
    XC.Query(Q)                                     ;
    for (int i=0;i<avChannels;i++)                  {
      DList DL = Amplitudes[i]                      ;
      int   DC = DL.count()                         ;
      QByteArray AMP                                ;
      AMP.resize(DC*sizeof(double))                 ;
      average[i] /= DC                              ;
      double * v = (double *)AMP.data()             ;
      for (int j=0;j<DC;j++) v[j] = DL[j]           ;
      QByteArray ZC = nXzCompress(AMP,9)            ;
      int size      = ZC.size()                     ;
      int checksum  = qChecksum(ZC,size)            ;
      Q = XC.Sql.InsertInto                         (
          Plan.Tables[Neutrino::Tables::Amplitude]  ,
          11                                        ,
          "uuid"                                    ,
          "channel"                                 ,
          "length"                                  ,
          "samples"                                 ,
          "average"                                 ,
          "maximum"                                 ,
          "minimum"                                 ,
          "size"                                    ,
          "checksum"                                ,
          "decode"                                  ,
          "volume"                                ) ;
      XC . Prepare ( Q )                            ;
      XC . Bind    ( "uuid"     , uuid            ) ;
      XC . Bind    ( "channel"  , i               ) ;
      XC . Bind    ( "length"   , DC              ) ;
      XC . Bind    ( "samples"  , Samples         ) ;
      XC . Bind    ( "average"  , average[i]      ) ;
      XC . Bind    ( "maximum"  , maximum[i]      ) ;
      XC . Bind    ( "minimum"  , minimum[i]      ) ;
      XC . Bind    ( "size"     , ZC.size()       ) ;
      XC . Bind    ( "checksum" , checksum        ) ;
      XC . Bind    ( "decode"   , AMP.size()      ) ;
      XC . Bind    ( "volume"   , ZC              ) ;
      XC . Exec    (   )                            ;
    }                                               ;
    XC.close( )                                     ;
  }                                                 ;
  XC.remove()                                       ;
#endif
  return true                                       ;
}
