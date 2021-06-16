#include <qtav.h>
#include "ui_PadRecorder.h"

N::PadRecorder:: PadRecorder (QWidget      * parent,Plan * p)
               : Splitter    (Qt::Vertical , parent,       p)
               , ui          (new Ui::PadRecorder           )
               , display     (NULL                          )
               , writer      (NULL                          )
               , wacceptor   (NULL                          )
{
  Configure ( ) ;
}

N::PadRecorder::~PadRecorder(void)
{
  delete ui ;
}

QSize N::PadRecorder::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize ( 1280 ,  900 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::PadRecorder::Configure(void)
{
  ui -> setupUi                     ( this               )       ;
  ui -> Label -> setVisible         ( false              )       ;
  display = new AudioDisplay        ( ui->Editing , plan )       ;
  ui -> Editing -> addWidget        ( display            )       ;
  ui -> Editing -> setCurrentWidget ( ui->Format         )       ;
  ////////////////////////////////////////////////////////////////
  addAction (  0 , tr("Back"  ) , QIcon(":/icons/cdup.png"    )) ;
  addAction (  1 , tr("Play"  ) , QIcon(":/images/audio.png"  )) ;
  addAction (  2 , tr("Record") , QIcon(":/icons/recorder.png")) ;
  addAction (  3 , tr("Stop"  ) , QIcon(":/icons/stop.png"    )) ;
  addAction (  4 , tr("Next"  ) , QIcon(":/icons/next.png"    )) ;
  ////////////////////////////////////////////////////////////////
  Displays [ 0 ] [  0 ] = true                                   ;
  Displays [ 0 ] [  1 ] = true                                   ;
  Displays [ 0 ] [  2 ] = true                                   ;
  Displays [ 0 ] [  3 ] = false                                  ;
  Displays [ 0 ] [  4 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  Displays [ 1 ] [  0 ] = false                                  ;
  Displays [ 1 ] [  1 ] = false                                  ;
  Displays [ 1 ] [  2 ] = false                                  ;
  Displays [ 1 ] [  3 ] = true                                   ;
  Displays [ 1 ] [  4 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  Displays [ 2 ] [  0 ] = false                                  ;
  Displays [ 2 ] [  1 ] = false                                  ;
  Displays [ 2 ] [  2 ] = false                                  ;
  Displays [ 2 ] [  3 ] = false                                  ;
  Displays [ 2 ] [  4 ] = true                                   ;
  ////////////////////////////////////////////////////////////////
  nConnect ( Actions [  0 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SIGNAL ( Back       ( ) )        ) ;
  nConnect ( Actions [  1 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Play       ( ) )        ) ;
  nConnect ( Actions [  2 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Record     ( ) )        ) ;
  nConnect ( Actions [  3 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Stop       ( ) )        ) ;
  nConnect ( Actions [  4 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Next       ( ) )        ) ;
  ////////////////////////////////////////////////////////////////
  plan    -> setFont ( ui -> Message )                           ;
  plan    -> setFont ( ui -> Format  )                           ;
  nConnect  ( &Timer , SIGNAL ( timeout     ( ) )                ,
              this   , SLOT   ( TimeElapsed ( ) )              ) ;
  Display   ( 0                                                ) ;
}

void N::PadRecorder::addAction(int Id,QString text,QIcon icon)
{
  Actions [ Id ] = new QAction ( icon , text , ui->Tools ) ;
  ui -> Tools    -> addAction  ( Actions [ Id ]          ) ;
  Actions [ Id ] -> setFont    ( ui -> Tools -> font ( ) ) ;
}

void N::PadRecorder::Display(int conf)
{
  BMAPs D   = Displays [ conf ]               ;
  CUIDs IDs = D . keys (      )               ;
  int   id                                    ;
  foreach (id,IDs)                            {
    Actions [ id ] -> setVisible ( D [ id ] ) ;
  }                                           ;
}

void N::PadRecorder::setPlay(bool enable)
{
  Displays [ 0 ] [  1 ] = enable ;
  Display  ( 0 )                 ;
}

void N::PadRecorder::Detect(void)
{
  NAMEs D = Acoustics :: Devices   ( Acoustics::Input )       ;
  UUIDs U = D . keys               (                  )       ;
  SUID  u                                                     ;
  ui   -> Devices -> blockSignals  ( true             )       ;
  ui   -> Devices -> clear         (                  )       ;
  foreach (u,U)                                               {
    ui -> Devices -> addItem       ( D[u]             )       ;
  }                                                           ;
  ui   -> Devices -> blockSignals  ( false            )       ;
  bool available = ( ui->Devices->count() > 0 )               ;
  ui   -> Filename -> setEnabled   ( available        )       ;
  ui   -> Formats  -> setEnabled   ( available        )       ;
  ui   -> Devices  -> setEnabled   ( available        )       ;
  ui   -> Samples  -> setEnabled   ( available        )       ;
  ui   -> Time     -> setEnabled   ( available        )       ;
  if (!available) return                                      ;
  QString adn = ""                                            ;
  if (plan->Equipments.contains(PurePlan::DefaultAudioInput)) {
    adn = plan->Equipments[PurePlan::DefaultAudioInput]       ;
  }                                                           ;
  if (adn.length()>0)                                         {
    int index                                                 ;
    index = ui -> Devices -> findText ( adn )                 ;
    if (index>=0)                                             {
      ui -> Devices -> blockSignals    ( true               ) ;
      ui -> Devices -> setCurrentIndex ( index              ) ;
      ui -> Devices -> blockSignals    ( true               ) ;
    }                                                         ;
  }                                                           ;
  ListSamples  ( 0 )                                          ;
  ListEncoders ( 0 )                                          ;
}

void N::PadRecorder::DeviceChanged(int)
{
  ListSamples  ( 0 ) ;
  ListEncoders ( 0 ) ;
}

void N::PadRecorder::ListSamples(int)
{
  int     s   = 0                                             ;
  QString adn = ""                                            ;
  adn = ui -> Devices -> currentText ( )                      ;
  QList<QAudioDeviceInfo> X                                   ;
  QAudioDeviceInfo        A                                   ;
  bool                    m = false                           ;
  X = QAudioDeviceInfo::availableDevices(QAudio::AudioInput ) ;
  for (int i=0;!m && i<X.count();i++)                         {
    if (X[i].deviceName()==adn)                               {
      A = X [ i ]                                             ;
      m = true                                                ;
    }                                                         ;
  }                                                           ;
  if (!m) return                                              ;
  ui -> Samples -> blockSignals ( true  )                     ;
  ui -> Samples -> clear        (       )                     ;
  /////////////////////////////////////////////////////////////
  while (Acoustics::SamplingRates[s]>0)                       {
    QAudioFormat FMT                                          ;
    FMT . setChannelCount ( 1                           )     ;
    FMT . setCodec        ( "audio/pcm"                 )     ;
    FMT . setSampleRate   ( Acoustics::SamplingRates[s] )     ;
    FMT . setSampleSize   ( 16                          )     ;
    FMT . setSampleType   ( QAudioFormat::SignedInt     )     ;
    FMT . setByteOrder    ( QAudioFormat::LittleEndian  )     ;
    if (A.isFormatSupported(FMT))                             {
      QString n                                               ;
      n = QString::number(Acoustics::SamplingRates[s])        ;
      ui -> Samples -> addItem ( n )                          ;
    }                                                         ;
    s++                                                       ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  s = ui -> Samples -> findText ( "48000" )                   ;
  if ( s >= 0 )                                               {
    ui -> Samples -> setCurrentIndex ( s )                    ;
  } else                                                      {
    s = ui -> Samples -> findText ( "44100" )                 ;
    if ( s >= 0 ) ui -> Samples -> setCurrentIndex ( s )      ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  ui -> Samples -> blockSignals ( false )                     ;
}

void N::PadRecorder::ListEncoders(int)
{
  int rate = ui -> Samples -> currentText ( ) . toInt ( ) ;
  if (rate<=0) return                                     ;
  AppendItems ( rate )                                    ;
}

void N::PadRecorder::AppendItems(int rate)
{
  ui -> Formats -> blockSignals ( true  ) ;
  ui -> Formats -> clear        (       ) ;
  #define ADDX(ID) AddItem(ID,rate)
  ADDX(AV_CODEC_ID_MPEG1VIDEO);
  ADDX(AV_CODEC_ID_MPEG2VIDEO);
  ADDX(AV_CODEC_ID_H261);
  ADDX(AV_CODEC_ID_H263);
  ADDX(AV_CODEC_ID_RV10);
  ADDX(AV_CODEC_ID_RV20);
  ADDX(AV_CODEC_ID_MJPEG);
  ADDX(AV_CODEC_ID_MJPEGB);
  ADDX(AV_CODEC_ID_LJPEG);
  ADDX(AV_CODEC_ID_SP5X);
  ADDX(AV_CODEC_ID_JPEGLS);
  ADDX(AV_CODEC_ID_MPEG4);
  ADDX(AV_CODEC_ID_RAWVIDEO);
  ADDX(AV_CODEC_ID_MSMPEG4V1);
  ADDX(AV_CODEC_ID_MSMPEG4V2);
  ADDX(AV_CODEC_ID_MSMPEG4V3);
  ADDX(AV_CODEC_ID_WMV1);
  ADDX(AV_CODEC_ID_WMV2);
  ADDX(AV_CODEC_ID_H263P);
  ADDX(AV_CODEC_ID_H263I);
  ADDX(AV_CODEC_ID_FLV1);
  ADDX(AV_CODEC_ID_SVQ1);
  ADDX(AV_CODEC_ID_SVQ3);
  ADDX(AV_CODEC_ID_DVVIDEO);
  ADDX(AV_CODEC_ID_HUFFYUV);
  ADDX(AV_CODEC_ID_CYUV);
  ADDX(AV_CODEC_ID_H264);
  ADDX(AV_CODEC_ID_INDEO3);
  ADDX(AV_CODEC_ID_VP3);
  ADDX(AV_CODEC_ID_THEORA);
  ADDX(AV_CODEC_ID_ASV1);
  ADDX(AV_CODEC_ID_ASV2);
  ADDX(AV_CODEC_ID_FFV1);
  ADDX(AV_CODEC_ID_4XM);
  ADDX(AV_CODEC_ID_VCR1);
  ADDX(AV_CODEC_ID_CLJR);
  ADDX(AV_CODEC_ID_MDEC);
  ADDX(AV_CODEC_ID_ROQ);
  ADDX(AV_CODEC_ID_INTERPLAY_VIDEO);
  ADDX(AV_CODEC_ID_XAN_WC3);
  ADDX(AV_CODEC_ID_XAN_WC4);
  ADDX(AV_CODEC_ID_RPZA);
  ADDX(AV_CODEC_ID_CINEPAK);
  ADDX(AV_CODEC_ID_WS_VQA);
  ADDX(AV_CODEC_ID_MSRLE);
  ADDX(AV_CODEC_ID_MSVIDEO1);
  ADDX(AV_CODEC_ID_IDCIN);
  ADDX(AV_CODEC_ID_8BPS);
  ADDX(AV_CODEC_ID_SMC);
  ADDX(AV_CODEC_ID_FLIC);
  ADDX(AV_CODEC_ID_TRUEMOTION1);
  ADDX(AV_CODEC_ID_VMDVIDEO);
  ADDX(AV_CODEC_ID_MSZH);
  ADDX(AV_CODEC_ID_ZLIB);
  ADDX(AV_CODEC_ID_QTRLE);
  ADDX(AV_CODEC_ID_TSCC);
  ADDX(AV_CODEC_ID_ULTI);
  ADDX(AV_CODEC_ID_QDRAW);
  ADDX(AV_CODEC_ID_VIXL);
  ADDX(AV_CODEC_ID_QPEG);
  ADDX(AV_CODEC_ID_PNG);
  ADDX(AV_CODEC_ID_PPM);
  ADDX(AV_CODEC_ID_PBM);
  ADDX(AV_CODEC_ID_PGM);
  ADDX(AV_CODEC_ID_PGMYUV);
  ADDX(AV_CODEC_ID_PAM);
  ADDX(AV_CODEC_ID_FFVHUFF);
  ADDX(AV_CODEC_ID_RV30);
  ADDX(AV_CODEC_ID_RV40);
  ADDX(AV_CODEC_ID_VC1);
  ADDX(AV_CODEC_ID_WMV3);
  ADDX(AV_CODEC_ID_LOCO);
  ADDX(AV_CODEC_ID_WNV1);
  ADDX(AV_CODEC_ID_AASC);
  ADDX(AV_CODEC_ID_INDEO2);
  ADDX(AV_CODEC_ID_FRAPS);
  ADDX(AV_CODEC_ID_TRUEMOTION2);
  ADDX(AV_CODEC_ID_BMP);
  ADDX(AV_CODEC_ID_CSCD);
  ADDX(AV_CODEC_ID_MMVIDEO);
  ADDX(AV_CODEC_ID_ZMBV);
  ADDX(AV_CODEC_ID_AVS);
  ADDX(AV_CODEC_ID_SMACKVIDEO);
  ADDX(AV_CODEC_ID_NUV);
  ADDX(AV_CODEC_ID_KMVC);
  ADDX(AV_CODEC_ID_FLASHSV);
  ADDX(AV_CODEC_ID_CAVS);
  ADDX(AV_CODEC_ID_JPEG2000);
  ADDX(AV_CODEC_ID_VMNC);
  ADDX(AV_CODEC_ID_VP5);
  ADDX(AV_CODEC_ID_VP6);
  ADDX(AV_CODEC_ID_VP6F);
  ADDX(AV_CODEC_ID_TARGA);
  ADDX(AV_CODEC_ID_DSICINVIDEO);
  ADDX(AV_CODEC_ID_TIERTEXSEQVIDEO);
  ADDX(AV_CODEC_ID_TIFF);
  ADDX(AV_CODEC_ID_GIF);
  ADDX(AV_CODEC_ID_DXA);
  ADDX(AV_CODEC_ID_DNXHD);
  ADDX(AV_CODEC_ID_THP);
  ADDX(AV_CODEC_ID_SGI);
  ADDX(AV_CODEC_ID_C93);
  ADDX(AV_CODEC_ID_BETHSOFTVID);
  ADDX(AV_CODEC_ID_PTX);
  ADDX(AV_CODEC_ID_TXD);
  ADDX(AV_CODEC_ID_VP6A);
  ADDX(AV_CODEC_ID_AMV);
  ADDX(AV_CODEC_ID_VB);
  ADDX(AV_CODEC_ID_PCX);
  ADDX(AV_CODEC_ID_SUNRAST);
  ADDX(AV_CODEC_ID_INDEO4);
  ADDX(AV_CODEC_ID_INDEO5);
  ADDX(AV_CODEC_ID_MIMIC);
  ADDX(AV_CODEC_ID_RL2);
  ADDX(AV_CODEC_ID_ESCAPE124);
  ADDX(AV_CODEC_ID_DIRAC);
  ADDX(AV_CODEC_ID_BFI);
  ADDX(AV_CODEC_ID_CMV);
  ADDX(AV_CODEC_ID_MOTIONPIXELS);
  ADDX(AV_CODEC_ID_TGV);
  ADDX(AV_CODEC_ID_TGQ);
  ADDX(AV_CODEC_ID_TQI);
  ADDX(AV_CODEC_ID_AURA);
  ADDX(AV_CODEC_ID_AURA2);
  ADDX(AV_CODEC_ID_V210X);
  ADDX(AV_CODEC_ID_TMV);
  ADDX(AV_CODEC_ID_V210);
  ADDX(AV_CODEC_ID_DPX);
  ADDX(AV_CODEC_ID_MAD);
  ADDX(AV_CODEC_ID_FRWU);
  ADDX(AV_CODEC_ID_FLASHSV2);
  ADDX(AV_CODEC_ID_CDGRAPHICS);
  ADDX(AV_CODEC_ID_R210);
  ADDX(AV_CODEC_ID_ANM);
  ADDX(AV_CODEC_ID_BINKVIDEO);
  ADDX(AV_CODEC_ID_IFF_ILBM);
  ADDX(AV_CODEC_ID_IFF_BYTERUN1);
  ADDX(AV_CODEC_ID_KGV1);
  ADDX(AV_CODEC_ID_YOP);
  ADDX(AV_CODEC_ID_VP8);
  ADDX(AV_CODEC_ID_PICTOR);
  ADDX(AV_CODEC_ID_ANSI);
  ADDX(AV_CODEC_ID_A64_MULTI);
  ADDX(AV_CODEC_ID_A64_MULTI5);
  ADDX(AV_CODEC_ID_R10K);
  ADDX(AV_CODEC_ID_MXPEG);
  ADDX(AV_CODEC_ID_LAGARITH);
  ADDX(AV_CODEC_ID_PRORES);
  ADDX(AV_CODEC_ID_JV);
  ADDX(AV_CODEC_ID_DFA);
  ADDX(AV_CODEC_ID_WMV3IMAGE);
  ADDX(AV_CODEC_ID_VC1IMAGE);
  ADDX(AV_CODEC_ID_UTVIDEO);
  ADDX(AV_CODEC_ID_BMV_VIDEO);
  ADDX(AV_CODEC_ID_VBLE);
  ADDX(AV_CODEC_ID_DXTORY);
  ADDX(AV_CODEC_ID_V410);
  ADDX(AV_CODEC_ID_XWD);
  ADDX(AV_CODEC_ID_CDXL);
  ADDX(AV_CODEC_ID_XBM);
  ADDX(AV_CODEC_ID_ZEROCODEC);
  ADDX(AV_CODEC_ID_MSS1);
  ADDX(AV_CODEC_ID_MSA1);
  ADDX(AV_CODEC_ID_TSCC2);
  ADDX(AV_CODEC_ID_MTS2);
  ADDX(AV_CODEC_ID_CLLC);
  ADDX(AV_CODEC_ID_MSS2);
  ADDX(AV_CODEC_ID_VP9);
  ADDX(AV_CODEC_ID_AIC);
  #ifdef Q_OS_WIN
  ADDX(AV_CODEC_ID_HNM4_VIDEO);
  #endif
  ADDX(AV_CODEC_ID_BRENDER_PIX);
  ADDX(AV_CODEC_ID_Y41P);
  ADDX(AV_CODEC_ID_ESCAPE130);
  ADDX(AV_CODEC_ID_EXR);
  ADDX(AV_CODEC_ID_AVRP);
  ADDX(AV_CODEC_ID_012V);
  ADDX(AV_CODEC_ID_G2M);
  ADDX(AV_CODEC_ID_AVUI);
  ADDX(AV_CODEC_ID_AYUV);
  ADDX(AV_CODEC_ID_TARGA_Y216);
  ADDX(AV_CODEC_ID_V308);
  ADDX(AV_CODEC_ID_V408);
  ADDX(AV_CODEC_ID_YUV4);
  ADDX(AV_CODEC_ID_SANM);
  ADDX(AV_CODEC_ID_PAF_VIDEO);
  ADDX(AV_CODEC_ID_AVRN);
  ADDX(AV_CODEC_ID_CPIA);
  ADDX(AV_CODEC_ID_XFACE);
  ADDX(AV_CODEC_ID_SGIRLE);
  ADDX(AV_CODEC_ID_MVC1);
  ADDX(AV_CODEC_ID_MVC2);
  ADDX(AV_CODEC_ID_SNOW);
  ADDX(AV_CODEC_ID_WEBP);
  ADDX(AV_CODEC_ID_SMVJPEG);
  ADDX(AV_CODEC_ID_HEVC);
  ADDX(AV_CODEC_ID_FIRST_AUDIO);
  ADDX(AV_CODEC_ID_PCM_S16LE);
  ADDX(AV_CODEC_ID_PCM_S16BE);
  ADDX(AV_CODEC_ID_PCM_U16LE);
  ADDX(AV_CODEC_ID_PCM_U16BE);
  ADDX(AV_CODEC_ID_PCM_S8);
  ADDX(AV_CODEC_ID_PCM_U8);
  ADDX(AV_CODEC_ID_PCM_MULAW);
  ADDX(AV_CODEC_ID_PCM_ALAW);
  ADDX(AV_CODEC_ID_PCM_S32LE);
  ADDX(AV_CODEC_ID_PCM_S32BE);
  ADDX(AV_CODEC_ID_PCM_U32LE);
  ADDX(AV_CODEC_ID_PCM_U32BE);
  ADDX(AV_CODEC_ID_PCM_S24LE);
  ADDX(AV_CODEC_ID_PCM_S24BE);
  ADDX(AV_CODEC_ID_PCM_U24LE);
  ADDX(AV_CODEC_ID_PCM_U24BE);
  ADDX(AV_CODEC_ID_PCM_S24DAUD);
  ADDX(AV_CODEC_ID_PCM_ZORK);
  ADDX(AV_CODEC_ID_PCM_S16LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_DVD);
  ADDX(AV_CODEC_ID_PCM_F32BE);
  ADDX(AV_CODEC_ID_PCM_F32LE);
  ADDX(AV_CODEC_ID_PCM_F64BE);
  ADDX(AV_CODEC_ID_PCM_F64LE);
  ADDX(AV_CODEC_ID_PCM_BLURAY);
  ADDX(AV_CODEC_ID_PCM_LXF);
  ADDX(AV_CODEC_ID_S302M);
  ADDX(AV_CODEC_ID_PCM_S8_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S24LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S32LE_PLANAR);
  ADDX(AV_CODEC_ID_PCM_S16BE_PLANAR);
  ADDX(AV_CODEC_ID_ADPCM_IMA_QT);
  ADDX(AV_CODEC_ID_ADPCM_IMA_WAV);
  ADDX(AV_CODEC_ID_ADPCM_IMA_DK3);
  ADDX(AV_CODEC_ID_ADPCM_IMA_DK4);
  ADDX(AV_CODEC_ID_ADPCM_IMA_WS);
  ADDX(AV_CODEC_ID_ADPCM_IMA_SMJPEG);
  ADDX(AV_CODEC_ID_ADPCM_MS);
  ADDX(AV_CODEC_ID_ADPCM_4XM);
  ADDX(AV_CODEC_ID_ADPCM_XA);
  ADDX(AV_CODEC_ID_ADPCM_ADX);
  ADDX(AV_CODEC_ID_ADPCM_EA);
  ADDX(AV_CODEC_ID_ADPCM_G726);
  ADDX(AV_CODEC_ID_ADPCM_CT);
  ADDX(AV_CODEC_ID_ADPCM_SWF);
  ADDX(AV_CODEC_ID_ADPCM_YAMAHA);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_4);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_3);
  ADDX(AV_CODEC_ID_ADPCM_SBPRO_2);
  ADDX(AV_CODEC_ID_ADPCM_THP);
  ADDX(AV_CODEC_ID_ADPCM_IMA_AMV);
  ADDX(AV_CODEC_ID_ADPCM_EA_R1);
  ADDX(AV_CODEC_ID_ADPCM_EA_R3);
  ADDX(AV_CODEC_ID_ADPCM_EA_R2);
  ADDX(AV_CODEC_ID_ADPCM_IMA_EA_SEAD);
  ADDX(AV_CODEC_ID_ADPCM_IMA_EA_EACS);
  ADDX(AV_CODEC_ID_ADPCM_EA_XAS);
  ADDX(AV_CODEC_ID_ADPCM_EA_MAXIS_XA);
  ADDX(AV_CODEC_ID_ADPCM_IMA_ISS);
  ADDX(AV_CODEC_ID_ADPCM_G722);
  ADDX(AV_CODEC_ID_ADPCM_IMA_APC);
  ADDX(AV_CODEC_ID_VIMA);
  ADDX(AV_CODEC_ID_ADPCM_AFC);
  ADDX(AV_CODEC_ID_ADPCM_IMA_OKI);
  ADDX(AV_CODEC_ID_ADPCM_DTK);
  ADDX(AV_CODEC_ID_ADPCM_IMA_RAD);
  ADDX(AV_CODEC_ID_ADPCM_G726LE);
  ADDX(AV_CODEC_ID_AMR_NB);
  ADDX(AV_CODEC_ID_AMR_WB);
  ADDX(AV_CODEC_ID_RA_144);
  ADDX(AV_CODEC_ID_RA_288);
  ADDX(AV_CODEC_ID_ROQ_DPCM);
  ADDX(AV_CODEC_ID_INTERPLAY_DPCM);
  ADDX(AV_CODEC_ID_XAN_DPCM);
  ADDX(AV_CODEC_ID_SOL_DPCM);
  ADDX(AV_CODEC_ID_MP2);
  ADDX(AV_CODEC_ID_MP3);
  ADDX(AV_CODEC_ID_AAC);
  ADDX(AV_CODEC_ID_AC3);
  ADDX(AV_CODEC_ID_DTS);
  ADDX(AV_CODEC_ID_VORBIS);
  ADDX(AV_CODEC_ID_DVAUDIO);
  ADDX(AV_CODEC_ID_WMAV1);
  ADDX(AV_CODEC_ID_WMAV2);
  ADDX(AV_CODEC_ID_MACE3);
  ADDX(AV_CODEC_ID_MACE6);
  ADDX(AV_CODEC_ID_VMDAUDIO);
  ADDX(AV_CODEC_ID_FLAC);
  ADDX(AV_CODEC_ID_MP3ADU);
  ADDX(AV_CODEC_ID_MP3ON4);
  ADDX(AV_CODEC_ID_SHORTEN);
  ADDX(AV_CODEC_ID_ALAC);
  ADDX(AV_CODEC_ID_WESTWOOD_SND1);
  ADDX(AV_CODEC_ID_GSM);
  ADDX(AV_CODEC_ID_QDM2);
  ADDX(AV_CODEC_ID_COOK);
  ADDX(AV_CODEC_ID_TRUESPEECH);
  ADDX(AV_CODEC_ID_TTA);
  ADDX(AV_CODEC_ID_SMACKAUDIO);
  ADDX(AV_CODEC_ID_QCELP);
  ADDX(AV_CODEC_ID_WAVPACK);
  ADDX(AV_CODEC_ID_DSICINAUDIO);
  ADDX(AV_CODEC_ID_IMC);
  ADDX(AV_CODEC_ID_MUSEPACK7);
  ADDX(AV_CODEC_ID_MLP);
  ADDX(AV_CODEC_ID_GSM_MS);
  ADDX(AV_CODEC_ID_ATRAC3);
  ADDX(AV_CODEC_ID_APE);
  ADDX(AV_CODEC_ID_NELLYMOSER);
  ADDX(AV_CODEC_ID_MUSEPACK8);
  ADDX(AV_CODEC_ID_SPEEX);
  ADDX(AV_CODEC_ID_WMAVOICE);
  ADDX(AV_CODEC_ID_WMAPRO);
  ADDX(AV_CODEC_ID_WMALOSSLESS);
  ADDX(AV_CODEC_ID_ATRAC3P);
  ADDX(AV_CODEC_ID_EAC3);
  ADDX(AV_CODEC_ID_SIPR);
  ADDX(AV_CODEC_ID_MP1);
  ADDX(AV_CODEC_ID_TWINVQ);
  ADDX(AV_CODEC_ID_TRUEHD);
  ADDX(AV_CODEC_ID_MP4ALS);
  ADDX(AV_CODEC_ID_ATRAC1);
  ADDX(AV_CODEC_ID_BINKAUDIO_RDFT);
  ADDX(AV_CODEC_ID_BINKAUDIO_DCT);
  ADDX(AV_CODEC_ID_AAC_LATM);
  ADDX(AV_CODEC_ID_QDMC);
  ADDX(AV_CODEC_ID_CELT);
  ADDX(AV_CODEC_ID_G723_1);
  ADDX(AV_CODEC_ID_G729);
  ADDX(AV_CODEC_ID_8SVX_EXP);
  ADDX(AV_CODEC_ID_8SVX_FIB);
  ADDX(AV_CODEC_ID_BMV_AUDIO);
  ADDX(AV_CODEC_ID_RALF);
  ADDX(AV_CODEC_ID_IAC);
  ADDX(AV_CODEC_ID_ILBC);
  ADDX(AV_CODEC_ID_METASOUND);
  ADDX(AV_CODEC_ID_FFWAVESYNTH);
  ADDX(AV_CODEC_ID_SONIC);
  ADDX(AV_CODEC_ID_SONIC_LS);
  ADDX(AV_CODEC_ID_PAF_AUDIO);
  ADDX(AV_CODEC_ID_OPUS);
  ADDX(AV_CODEC_ID_TAK);
  ADDX(AV_CODEC_ID_EVRC);
  ADDX(AV_CODEC_ID_SMV);
  ADDX(AV_CODEC_ID_FIRST_SUBTITLE);
  ADDX(AV_CODEC_ID_DVD_SUBTITLE);
  ADDX(AV_CODEC_ID_DVB_SUBTITLE);
  ADDX(AV_CODEC_ID_TEXT);
  ADDX(AV_CODEC_ID_XSUB);
  ADDX(AV_CODEC_ID_SSA);
  ADDX(AV_CODEC_ID_MOV_TEXT);
  ADDX(AV_CODEC_ID_HDMV_PGS_SUBTITLE);
  ADDX(AV_CODEC_ID_DVB_TELETEXT);
  ADDX(AV_CODEC_ID_SRT);
  ADDX(AV_CODEC_ID_MICRODVD);
  ADDX(AV_CODEC_ID_EIA_608);
  ADDX(AV_CODEC_ID_JACOSUB);
  ADDX(AV_CODEC_ID_SAMI);
  ADDX(AV_CODEC_ID_REALTEXT);
  ADDX(AV_CODEC_ID_SUBVIEWER1);
  ADDX(AV_CODEC_ID_SUBVIEWER);
  ADDX(AV_CODEC_ID_SUBRIP);
  ADDX(AV_CODEC_ID_WEBVTT);
  ADDX(AV_CODEC_ID_MPL2);
  ADDX(AV_CODEC_ID_VPLAYER);
  ADDX(AV_CODEC_ID_PJS);
  ADDX(AV_CODEC_ID_ASS);
  ADDX(AV_CODEC_ID_FIRST_UNKNOWN);
  ADDX(AV_CODEC_ID_TTF);
  ADDX(AV_CODEC_ID_BINTEXT);
  ADDX(AV_CODEC_ID_XBIN);
  ADDX(AV_CODEC_ID_IDF);
  ADDX(AV_CODEC_ID_OTF);
  ADDX(AV_CODEC_ID_SMPTE_KLV);
  ADDX(AV_CODEC_ID_DVD_NAV);
  #undef  ADDX
  int index = -1                                        ;
  for (int i=0;index<0 && i<ui->Formats->count();i++)   {
    QString n = ui->Formats->itemText(i)                ;
    if (n.contains("MP3")) index = i                    ;
  }                                                     ;
  if (index<0)                                          {
    for (int i=0;index<0 && i<ui->Formats->count();i++) {
      QString n = ui->Formats->itemText(i)              ;
      if (n.contains("MP2")) index = i                  ;
    }                                                   ;
  }                                                     ;
  if (index>=0) ui->Formats->setCurrentIndex(index)     ;
  ui -> Formats -> blockSignals ( false )               ;
}

void N::PadRecorder::AddItem(int ID,int rate)
{
  AVCodec * Encoder = NULL                                     ;
  //////////////////////////////////////////////////////////////
  Encoder = ::avcodec_find_encoder ( (enum AVCodecID) ID    )  ;
  nDropOut            ( IsNull(Encoder)                     )  ;
  nDropOut            ( Encoder->type != AVMEDIA_TYPE_AUDIO )  ;
  AVCodecContext * Context                                     ;
  int              fmt     = AV_SAMPLE_FMT_S16                 ;
  bool             correct = true                              ;
  Context                  = ::avcodec_alloc_context3(Encoder) ;
  Context -> sample_rate   = rate                              ;
  Context -> channels      = 1                                 ;
  Context -> sample_fmt    = (AVSampleFormat)fmt               ;
  //////////////////////////////////////////////////////////////
  if (::avcodec_open2(Context,Encoder,NULL)<0) correct = false ;
  if (correct && Context->frame_size<=0      ) correct = false ;
  if (!correct) return                                         ;
  ::avcodec_close ( Context )                                  ;
  QString name = Encoder -> long_name                          ;
  if (name.contains("MP3"))                                    {
    name = "MP3 (MPEG audio layer 3)"                          ;
  } else
  if (name.contains("Opus"))                                   {
    name = "Opus"                                              ;
  }
  ui -> Formats -> addItem ( name , ID )                       ;
}

void N::PadRecorder::Play(void)
{
}

void N::PadRecorder::Record(void)
{
  QString filename                                                         ;
  filename = ui->Filename->text()                                          ;
  if (filename.length()<=0)                                                {
    Alert ( Click )                                                        ;
    return                                                                 ;
  }                                                                        ;
  filename = Current . absoluteFilePath ( filename )                       ;
  //////////////////////////////////////////////////////////////////////////
  QString rp = Root  . absoluteFilePath ( ""       )                       ;
  if (!filename.contains(rp))                                              {
    Alert ( Click )                                                        ;
    return                                                                 ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Label   -> setVisible       ( true         )                       ;
  ui -> Label   -> setCurrentWidget ( ui->Progress )                       ;
  ui -> Editing -> setCurrentWidget ( display      )                       ;
  Display                           ( 1            )                       ;
  plan -> processEvents             (              )                       ;
  plan -> Rehabilitate              (              )                       ;
  //////////////////////////////////////////////////////////////////////////
  int     codec  = ui->Formats->currentIndex ( )                           ;
  QString device = ui->Devices->currentText  ( )                           ;
  int     rate   = ui->Samples->currentText  ( ).toInt()                   ;
  int     MID    = RequestInput(device,rate,16,1,(int)N::Acoustics::Int16) ;
  codec = ui->Formats->itemData(codec).toInt()                             ;
  //////////////////////////////////////////////////////////////////////////
  Junction  * junc                                                         ;
  AudioJack * jack                                                         ;
  jack      = (AudioJack *)plan->AudioChannels[MID]                        ;
  //////////////////////////////////////////////////////////////////////////
  if (NotNull(display->Audio)) delete display->Audio                       ;
  junc      = MountChannel ( MID )                                         ;
  display  -> junction = junc                                              ;
  display  -> Audio    = new AudioIO()                                     ;
  display  -> acceptor = new AudioAcceptor( this,display->Audio )          ;
  display  -> channel  = (AudioChannel *)jack                              ;
  jack     -> assign     ( *(display->Audio)   )                           ;
  //////////////////////////////////////////////////////////////////////////
  QFile * fout                                                             ;
  if (NotNull(writer))                                                     {
    if (NotNull(writer->Output)) delete writer->Output                     ;
    delete writer                                                          ;
  }                                                                        ;
  if (NotNull(wacceptor))                                                  {
    wacceptor -> audio = NULL                                              ;
    wacceptor -> deleteLater    (               )                          ;
  }                                                                        ;
  fout      = new QFile         ( filename      )                          ;
  writer    = new AudioWriter   ( this          )                          ;
  wacceptor = new AudioAcceptor ( this , writer )                          ;
  writer   -> Filename = filename                                          ;
  writer   -> Output   = fout                                              ;
  writer   -> codec    = codec                                             ;
  jack     -> assign ( *writer )                                           ;
  writer   -> setName ( filename )                                         ;
  nConnect ( writer , SIGNAL ( Finished       ( ) )                        ,
             this   , SLOT   ( RecordFinished ( ) )                      ) ;
  //////////////////////////////////////////////////////////////////////////
  junc     -> addConduit ( display -> acceptor )                           ;
  junc     -> addConduit ( wacceptor           )                           ;
  plan     -> Dredge     ( true                )                           ;
  //////////////////////////////////////////////////////////////////////////
  int ds = ui -> Delay -> value ( )                                        ;
  ui -> Progress -> setRange ( 0 , ui->Time->value() )                     ;
  Time :: skip ( ds * 1000 )                                               ;
  //////////////////////////////////////////////////////////////////////////
  jack    -> play       (                     )                            ;
  while ( display->Audio->Milliseconds() < 600 ) N::Time::skip(100)        ;
  display -> startup    (                     )                            ;
  writer  -> start      (                     )                            ;
  Elapsed  . restart    (                     )                            ;
  Timer    . start      ( 500                 )                            ;
}

void N::PadRecorder::TimeElapsed(void)
{
  int elapsed = Elapsed . elapsed ( )                 ;
  int limited = ui -> Time -> value() * 1000          ;
  ui -> Progress -> setValue ( elapsed / 1000 )       ;
  plan -> processEvents ( )                           ;
  if ( elapsed >= limited )                           {
    Timer . stop       (                            ) ;
    QTimer::singleShot ( 300 , this , SLOT (Stop()) ) ;
  }                                                   ;
}

void N::PadRecorder::Stop(void)
{
  AudioJack * jack                                  ;
  jack = (AudioJack *) display -> channel           ;
  Display                     ( 2     )             ;
  Actions [ 4 ] -> setEnabled ( false )             ;
  plan     -> processEvents   (       )             ;
  jack     -> abort           (       )             ;
  display  -> stop            (       )             ;
  Timer     . stop            (       )             ;
  ///////////////////////////////////////////////////
  Conduit   * conduit                               ;
  conduit   = display -> junction -> conduits [ 0 ] ;
  display -> junction -> takeConduit ( conduit )    ;
}

void N::PadRecorder::RecordFinished(void)
{
  Time :: skip ( 1500 )                                       ;
  /////////////////////////////////////////////////////////////
  CUIDs       IDs  = plan -> AudioChannels . keys ( )         ;
  AudioJack * jack = (AudioJack *) display -> channel         ;
  int         MID  = -1                                       ;
  /////////////////////////////////////////////////////////////
  display -> junction -> takeConduit ( display  -> acceptor ) ;
  display -> junction -> takeConduit ( wacceptor            ) ;
  display -> junction = NULL                                  ;
  display -> channel  = NULL                                  ;
  /////////////////////////////////////////////////////////////
  if (IDs.count()>0)                                          {
    for (int i=0;MID<0 && i<IDs.count();i++)                  {
      if (jack==plan->AudioChannels[i]) MID = i               ;
    }                                                         ;
    if (MID>=0)                                               {
      plan->AudioChannels.remove(MID)                         ;
      delete jack                                             ;
    }                                                         ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  Actions [ 4 ] -> setEnabled ( true )                        ;
}

void N::PadRecorder::Next(void)
{
  ui -> Label   -> setVisible       ( false      ) ;
  ui -> Editing -> setCurrentWidget ( ui->Format ) ;
  Display                           ( 0          ) ;
}

int N::PadRecorder::RequestInput(QString device,int rate,int bits,int channels,int format)
{
  CUIDs       IDs = plan->AudioChannels.keys()    ;
  int         MID = 0                             ;
  AudioJack * jack                                ;
  jack  = new AudioJack (        )                ;
  jack -> setParent     ( this   )                ;
  jack -> setDevice     ( device )                ;
  jack -> set                                     (
    rate                                          ,
    bits                                          ,
    channels                                      ,
    (N::Acoustics::SampleFormat)format          ) ;
  if (IDs.count()>0)                              {
    for (int i=0;i<IDs.count();i++)               {
      if (IDs[i]>MID) MID = IDs[i]                ;
    }                                             ;
  }                                               ;
  MID++                                           ;
  plan->AudioChannels[MID] = (AudioChannel *)jack ;
  return MID                                      ;
}

N::Junction * N::PadRecorder::MountChannel(int MID)
{
  nKickOut ( !plan->AudioChannels.contains(MID) , NULL )   ;
  AudioConduit * acot                                      ;
  acot = new AudioConduit (this,plan->AudioChannels[MID] ) ;
  Junction * junc = plan -> GrabJunction ( this )          ;
  junc->interval = 90                                      ;
  junc->addConduit ( acot )                                ;
  return junc                                              ;
}
