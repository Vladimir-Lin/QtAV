/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001 ~ 2016 Neutrino International Inc.                    *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_AV_H
#define QT_AV_H

#include <QtModels>
#include <QtPhotography>
#include <DataWidgets>
#include <MathWidgets>
#include <QtTasks>
#include <QtHuman>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_QTAV_LIB)
#      define Q_AV_EXPORT Q_DECL_EXPORT
#    else
#      define Q_AV_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_AV_EXPORT
#endif

namespace Ui                        {
class Q_AV_EXPORT nVideoPanel       ;
class Q_AV_EXPORT PadRecorder       ;
class Q_AV_EXPORT nCameraEditor     ;
class Q_AV_EXPORT AddNewAlbum       ;
class Q_AV_EXPORT nVideoRatio       ;
class Q_AV_EXPORT nStreamController ;
class Q_AV_EXPORT nVideoStatus      ;
}

namespace N
{

class Q_AV_EXPORT UnifiedResources     ;
class Q_AV_EXPORT WaveUniverse         ;
class Q_AV_EXPORT VideoUniverse        ;
class Q_AV_EXPORT EditableUniverse     ;
class Q_AV_EXPORT GroupUniverse        ;
class Q_AV_EXPORT CameraBox            ;
class Q_AV_EXPORT CcdLists             ;
class Q_AV_EXPORT CameraView           ;
class Q_AV_EXPORT CameraEditor         ;
class Q_AV_EXPORT CameraLists          ;
class Q_AV_EXPORT VideoTransmission    ;
class Q_AV_EXPORT VideoData            ;
class Q_AV_EXPORT VideoReader          ;
class Q_AV_EXPORT VideoChannel         ;
class Q_AV_EXPORT VideoConduit         ;
class Q_AV_EXPORT VideoAcceptor        ;
class Q_AV_EXPORT VideoJunction        ;
class Q_AV_EXPORT VideoFeeder          ;
class Q_AV_EXPORT PickAudio            ;
class Q_AV_EXPORT AudioDepot           ;
class Q_AV_EXPORT CodecSupports        ;
class Q_AV_EXPORT AudioEncoderSupports ;
class Q_AV_EXPORT VideoBrowser         ;
class Q_AV_EXPORT MusicAlbums          ;
class Q_AV_EXPORT VideoView            ;
class Q_AV_EXPORT AlbumView            ;
class Q_AV_EXPORT VideoContainer       ;
class Q_AV_EXPORT VideoTags            ;
class Q_AV_EXPORT AlbumClip            ;
class Q_AV_EXPORT ClipList             ;
class Q_AV_EXPORT VideoLists           ;
class Q_AV_EXPORT PlayList             ;
class Q_AV_EXPORT PlayController       ;
class Q_AV_EXPORT AudioDisplay         ;
class Q_AV_EXPORT PadRecorder          ;
class Q_AV_EXPORT AudioPanel           ;
class Q_AV_EXPORT VideoEditorCLI       ;
class Q_AV_EXPORT AddNewAlbum          ;
class Q_AV_EXPORT VideoRatio           ;
class Q_AV_EXPORT VcfVideo             ;
class Q_AV_EXPORT StreamInterface      ;
class Q_AV_EXPORT StreamController     ;
class Q_AV_EXPORT VideoStatus          ;
class Q_AV_EXPORT VideosDispatcher     ;
class Q_AV_EXPORT VideoMerger          ;
class Q_AV_EXPORT VideoStreaming       ;
class Q_AV_EXPORT AudioStreaming       ;
class Q_AV_EXPORT ClipsShelf           ;
class Q_AV_EXPORT AlbumShelf           ;
class Q_AV_EXPORT VideoShelf           ;
class Q_AV_EXPORT EarIn                ;
class Q_AV_EXPORT VideoEditor          ;

namespace Acoustics
{

Q_AV_EXPORT QActionGroup * DeviceMenu (Plan * plan,Equipment equipment,QMenu * menu) ;

}

namespace Encoders
{

Q_AV_EXPORT bool Convert        (Plan * plan,int Codec,QString filename,QString audio,bool progress,QString format) ;
Q_AV_EXPORT bool BuildAmplitude (Plan & plan,SUID uuid) ;

}

Q_AV_EXPORT N::Universe * WaveCreator   (void *,QObject * parent) ;
Q_AV_EXPORT N::Universe * CameraCreator (void *,QObject * parent) ;
Q_AV_EXPORT N::Universe * VideoCreator  (void *,QObject * parent) ;

class Q_AV_EXPORT WaveUniverse : public QObject
                               , public Universe
                               , public BlobAcceptor
{
  Q_OBJECT
  public:

    QDateTime  ThrottleTime   ;
    QByteArray Throttle       ;
    bool       BypassThrottle ;

    explicit WaveUniverse              (QObject * parent = NULL) ;
    virtual ~WaveUniverse              (void) ;

    virtual int  EatType               (void) const { return 20001 ; }

    virtual bool Animation             (void) ;

    virtual void setWaveCamera         (int    id         ,
                                        int    mode       ,
                                        int    CameraOp   ,
                                        QPoint pos        ,
                                        QSize  s          ,
                                        int    Samples    ,
                                        double minValue   ,
                                        double maxValue ) ;
    SceneObject * addPlane             (int    id         ,
                                        int    SampleRate ,
                                        double minValue   ,
                                        double maxValue   ,
                                        double z          ,
                                        QColor border     ,
                                        QColor inside   ) ;
    SceneObject * addWave              (int    SampleRate ,
                                        int    SampleFmt  ,
                                        double z          ,
                                        QColor color    ) ;

    virtual void setConfigure          (int    SampleRate   ,
                                        int    SampleFmt    ,
                                        QPoint pos          ,
                                        QSize  camera       ,
                                        double plane        ,
                                        double line         ,
                                        double wave         ,
                                        QColor planeColor   ,
                                        QColor lineColor    ,
                                        QColor waveColor  ) ;
    virtual void Relocation            (QRectF viewport) ;

    bool SampleRange                   (int      sampleFmt  ,
                                        double & minValue   ,
                                        double & maxValue ) ;
    int  SampleType                    (int sampleFmt) ;

    virtual bool mouseDoubleClickEvent (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseMoveEvent        (QWidget * widget,QMouseEvent * event) ;
    virtual bool mousePressEvent       (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseReleaseEvent     (QWidget * widget,QMouseEvent * event) ;

    virtual bool Menu                  (QWidget * widget,QPointF pos) ;

    virtual bool Demolish              (void) ;
    virtual bool Feeding               (QByteArray & data) ;

  protected:

    QMutex ThrottleMutex  ;
    int    EachSample     ;
    int    EachRate       ;

    int    AudioSampleSize             (int sampleFmt) ;

  private:

  public slots:

    void ChangeBorder                  (QWidget * widget) ;
    void ChangeBackground              (QWidget * widget) ;
    void ChangeWaveColor               (QWidget * widget) ;

  protected slots:

  private slots:

  signals:

    void RequestPanel                  (WaveUniverse * wave,bool enabled) ;
    void Movable                       (WaveUniverse * wave,bool enabled) ;
    void DisableMenu                   (void) ;

} ;

class Q_AV_EXPORT CameraBox : public QObject
                            , public Universe
{
  Q_OBJECT
  public:

    enum TextureDirection {
      XL = 0              ,
      YU = 1              ,
      XR = 2              ,
      YD = 3              ,
      ZF = 4              ,
      ZB = 5            } ;

    double MovingFactor ;

    explicit CameraBox                 (QObject * parent = NULL) ;
    virtual ~CameraBox                 (void) ;

    virtual void setLength             (double length,QFont & font) ;
    virtual void setBox                (SceneObject * box,double length) ;
    virtual void setBox                (double length) ;
    virtual void setImageColor         (QColor color) ;
    virtual void assignCamera          (int index) ;
    virtual void assignCamera          (int index,Camera * camera) ;

    virtual bool mouseDoubleClickEvent (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseMoveEvent        (QWidget * widget,QMouseEvent * event) ;
    virtual bool mousePressEvent       (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseReleaseEvent     (QWidget * widget,QMouseEvent * event) ;
    virtual bool wheelEvent            (QWidget * widget,QWheelEvent * event) ;

    virtual bool Menu                  (QWidget * widget,QPointF pos) ;
    virtual bool Moving                (QPointF & difference) ;
    virtual void Apply                 (QPointF & difference) ;
    virtual void Preview               (QPointF & difference) ;

    virtual void PaintBox              (double length,QFont & font) ;

    virtual bool Animation             (void) ;

  protected:

    int  UsingCamera  ;
    bool Modification ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

    void Close (CameraBox * camera) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                                World objects                              *
 *                                                                           *
 *****************************************************************************/

class Q_AV_EXPORT CcdLists : public TreeWidget
{
  Q_OBJECT
  public:

    explicit CcdLists             (StandardConstructor) ;
    virtual ~CcdLists             (void) ;

  protected:

    virtual bool FocusIn          (void) ;
    virtual void Configure        (void) ;

    virtual void run              (int Type,ThreadData * Data) ;

  private:

  public slots:

    virtual bool startup          (void) ;
    virtual void List             (void) ;
    virtual void Insert           (void) ;
    virtual void Save             (void) ;
    virtual void Export           (void) ;

  protected slots:

    virtual bool Menu             (QPoint pos) ;
    virtual void doubleClicked    (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished     (void) ;
    virtual void identFinished    (void) ;
    virtual void equationFinished (void) ;
    virtual void widthFinished    (void) ;
    virtual void heightFinished   (void) ;
    virtual void fovFinished      (void) ;
    virtual void focusFinished    (void) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT CameraView : public GLWidget
{
  Q_OBJECT
  public:

    Camera camera ;

    explicit CameraView  (StandardConstructor) ;
    virtual ~CameraView  (void);

  protected:

    int           lights     ;
    BMAPs         properties ;
    Camera        viewpoint  ;
    Lights        lightings  ;
    ControlPoints rotates    ;
    SceneObject   root       ;
    SceneObjects  objects    ;

    virtual void Configure    (void) ;
    virtual void initializeGL (void);
    virtual void paintGL      (void);
    virtual void resizeGL     (int width,int height);

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT CameraEditor : public Widget
                               , public AttachDock
                               , public SceneManager
{
  Q_OBJECT
  public:

    Camera * camera ;

    explicit CameraEditor    (StandardConstructor) ;
    virtual ~CameraEditor    (void);

  protected:

    MatrixEditor      * matrix ;
    QGLWidget         * GL     ;
    Ui::nCameraEditor * ui     ;

    virtual void Configure   (void) ;

  private:

    double doubleAt          (int row,int column) ;

  public slots:

    void Save                (void) ;
    void Relocation          (void) ;

    virtual void Docking     (QMainWindow * Main,QString title,Qt::DockWidgetArea area,Qt::DockWidgetAreas areas);
    virtual void DockIn      (bool shown) ;

    virtual void Attach      (QGLWidget * gl) ;
    virtual void Attach      (Camera * camera) ;
    virtual void Detach      (void) ;

    virtual void Reflush     (void) ;

  protected slots:

    void cameraState         (double) ;
    void cameraCell          (int,int) ;
    void cameraChanged       (void) ;
    void nameChanged         (QString text) ;
    void Visible             (bool visible) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT CameraLists : public ListDock
                              , public SceneManager
{
  Q_OBJECT
  public:

    explicit CameraLists           (StandardConstructor) ;
    virtual ~CameraLists           (void);

  protected:

    QPoint dragPoint ;

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

  private:

  public slots:

    virtual void List              (void) ;
    virtual void Insert            (void) ;
    virtual void Copy              (void) ;

  protected slots:

    bool         Menu              (QPoint pos) ;

  private slots:

  signals:

    void         Append            (QWidget * widget) ;

} ;

class Q_AV_EXPORT VideoTransmission
{
  public:

    QMap<int,QImage  *> Images         ;
    QMap<int,bool    *> Displays       ;
    QMap<int,QDateTime> FrameTimes     ;
    QMap<int,bool     > Status         ;
    QDateTime           BaseTime       ;
    QDateTime           LastTime       ;
    QDateTime           CurrentTime    ;
    QSize               Dimension      ;
    QImage::Format      Format         ;
    TUID                TotalAudioSize ;
    TUID                AudioFlushed   ;
    int                 ThrottleSize   ;
    VarArgLists         Commands       ;
    QMutex              mutex          ;

    explicit VideoTransmission (void) ;
    virtual ~VideoTransmission (void) ;

    void     Lock              (void) ;
    void     Unlock            (void) ;
    void     setStatus         (int id,bool status) ;
    void     setTime           (int total,QDateTime now) ;
    void     SituationChanged  (QSize dimension,QMap<SUID,QImage *> & images,XMAPs & pictures) ;
    bool     Modified          (QSize dimension) ;
    bool     hasCommand        (VarArgs & command) ;
    TUID     Elapsed           (void) ;
    TUID     Elapsed           (int At) ;

  protected:

  private:

} ;

class Q_AV_EXPORT VideoData
{
  public:

    QString   Filename      ;
    QString   Title         ;
    QSize     Dimension     ;
    long long Duration      ;
    bool      isAudio       ;
    bool      isVideo       ;
    int       Frames        ;
    int       AvPixelFormat ;
    AudioData AudioBuffer   ;
    AudioData AudioCache    ;
    int       VideoFrames   ;
    int       VideoPending  ;
    int       VideoAt       ;
    int       Drift         ;

    explicit VideoData      (void) ;
    virtual ~VideoData      (void) ;

    int      setPixelFormat (QImage::Format format) ;

  protected:

  private:

} ;

class Q_AV_EXPORT VideoReader : public VideoData
                              , public AvDecoder
{
  public:

    explicit         VideoReader    (void) ;
    virtual         ~VideoReader    (void) ;

    QString          VideoPath      (void) const ;
    virtual void     setFilename    (QString filename) ;

    virtual bool     Peep           (void) ;
    virtual bool     prepare        (void) ;
    virtual bool     decode         (void) ;
    virtual bool     cease          (void) ;
    virtual bool     fresh          (void) ;
    virtual bool     demolish       (void) ;

    virtual bool     LockAudio      (void) ;
    virtual bool     UnlockAudio    (void) ;
    virtual bool     LockVideo      (void) ;
    virtual bool     UnlockVideo    (void) ;

  protected:

    QMutex   AudioMutex  ;
    QMutex   VideoMutex  ;
    bool     going       ;
    AVPacket Packet      ;

    virtual void     Error          (int error,QString message) ;
    virtual void     started        (void) ;
    virtual bool     proceed        (void) ;
    virtual void     finished       (void) ;
    virtual void     progress       (int current,int total) ;

    virtual void     FetchAudio     (void) ;
    virtual void     FetchImage     (void) ;
    virtual void     FetchSubtitle  (void) ;
    virtual void     PacketError    (void) ;
    virtual void     StreamError    (int Error) ;

    virtual void     AssignImage    (QImage * image) ;
    virtual QImage * ObtainImage    (void) ;
    virtual void     AudioAppended  (int newSize) ;
    virtual TUID     toMilliseconds (TUID timestamp) ;

  private:

} ;

class Q_AV_EXPORT VideoChannel
{
  public:

    VarArgs             VideoParaments ;
    VideoTransmission * Transmission   ;

    explicit            VideoChannel    (void) ;
    virtual            ~VideoChannel    (void) ;

    VideoTransmission * NewTransmission (void) ;
    virtual int         channelType     (void) const ;
    virtual QString     channelName     (void)   = 0 ;
    virtual QString     channelSource   (void)   = 0 ;
    virtual bool        channelHasData  (void)   = 0 ;
    virtual TUID        channelLength   (void)   = 0 ;
    virtual bool        canSeekChannel  (void)   = 0 ;
    virtual bool        channelPlay     (void)   = 0 ;
    virtual bool        channelPause    (void)   = 0 ;
    virtual bool        channelSeek     (TUID timestamp) = 0 ;
    virtual bool        channelResume   (void)   = 0 ;
    virtual bool        channelAbort    (void)   = 0 ;

  protected:

  private:

} ;

class Q_AV_EXPORT VideoConduit : public Conduit
{ // Audio Input Feeder
  Q_OBJECT
  public:

    VideoChannel * channel  ;

    explicit VideoConduit  (QObject * parent = NULL) ;
    explicit VideoConduit  (QObject * parent,VideoChannel * video) ;
    virtual ~VideoConduit  (void) ;

    virtual int  type      (void) const { return 2001 ; }
    virtual bool Outflow   (int junctionType) ;
    virtual bool Into      (int junctionType) ;
    virtual bool Throught  (Conduit * PreviousConduit) ;
    virtual bool Has       (void) ;
    virtual bool Transit   (void) ;

  protected:

  private:

  public slots:

    virtual void assign    (VideoChannel * channel) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoAcceptor : public Conduit
{
  Q_OBJECT
  public:

    SyncAcceptor * acceptor ;

    explicit VideoAcceptor  (QObject * parent = NULL) ;
    explicit VideoAcceptor  (QObject * parent,SyncAcceptor * renderer) ;
    virtual ~VideoAcceptor  (void) ;

    virtual int  type      (void) const { return 2002 ; }
    virtual bool Into      (int junctionType) ;
    virtual bool Throught  (Conduit * PreviousConduit) ;
    virtual bool Demolish  (Conduit * Faucet) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoJunction : public Junction
{
  Q_OBJECT
  public:

    explicit VideoJunction (QObject * parent = NULL) ;
    virtual ~VideoJunction (void) ;

    virtual int  type      (void) const { return 2001 ; }

    VideoChannel * Supply  (void) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoUniverse : public QObject
                                , public RasterUniverse
                                , public SyncAcceptor
{
  Q_OBJECT
  public:

    QRectF ScreenRange ;
    QSizeF Rectangle   ;

    explicit     VideoUniverse         (QObject * parent = NULL) ;
    explicit     VideoUniverse         (QObject * parent,SceneDestructor * destructor) ;
    virtual     ~VideoUniverse         (void) ;

    virtual int  SyncType              (void) const { return 30001 ; }

    virtual bool Demolish              (void) ;
    virtual bool Sync                  (void * data) ;
    virtual bool setTransmission       (VideoTransmission * transmission) ;

    virtual bool contains              (QPointF pos) ;
    virtual void Relocation            (QRectF viewport) ;
    virtual bool Menu                  (QWidget * widget,QPointF pos) ;

    virtual bool mouseDoubleClickEvent (QWidget * widget,QMouseEvent * event) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

    void Finished      (VideoUniverse * universe) ;
    void VideoStop     (VideoUniverse * universe) ;
    void DisplayRatio  (VideoUniverse * universe) ;
    void DoubleClicked (VideoUniverse * universe) ;
    void RequestAudio  (VideoUniverse * universe,bool enabled) ;
    void RequestPanel  (VideoUniverse * universe,bool enabled) ;
    void Movable       (VideoUniverse * universe,bool enabled) ;
    void DisableMenu   (void) ;

} ;

class Q_AV_EXPORT VideoFeeder : public QObject
                              , public Object
                              , public VideoReader
                              , public VideoChannel
                              , public AudioChannel
                              , public Thread
{
  Q_OBJECT
  public:

    VarArgLists * PostSequences ;

    explicit          VideoFeeder     (QObject * parent = NULL) ;
    virtual          ~VideoFeeder     (void) ;

    // Audio Channel
    virtual int       type            (void) const ;
    virtual QString   name            (void) ;
    virtual QString   source          (void) ;
    virtual bool      hasData         (void) ;
    virtual TUID      length          (void) ;
    virtual TUID      bytes           (void) ;
    virtual int       rate            (void) ;
    virtual int       channels        (void) ;
    virtual bool      hasChannel      (int id) ;
    virtual QString   channel         (int id) ;
    virtual bool      canSeek         (void) ;
    virtual bool      assign          (AudioData & data) ;
    virtual bool      obtain          (AudioData & data,int bytes) ;
    virtual bool      play            (void) ;
    virtual bool      Pause           (void) ;
    virtual bool      seek            (TUID timestamp) ;
    virtual bool      Resume          (void) ;
    virtual bool      abort           (void) ;

    // Video Channel
    virtual int       channelType     (void) const ;
    virtual QString   channelName     (void) ;
    virtual QString   channelSource   (void) ;
    virtual bool      channelHasData  (void) ;
    virtual TUID      channelLength   (void) ;
    virtual bool      canSeekChannel  (void) ;
    virtual bool      channelPlay     (void) ;
    virtual bool      channelPause    (void) ;
    virtual bool      channelSeek     (TUID timestamp) ;
    virtual bool      channelResume   (void) ;
    virtual bool      channelAbort    (void) ;

    virtual int       Interval        (void) ;
    virtual void      setInterval     (int interval) ;
    virtual int       BufferTime      (void) ;
    virtual void      setBufferTime   (int buffer) ;
    virtual bool      Waiting         (void) ;
    virtual void      setCompletion   (bool wait) ;

    virtual bool      Halt            (void) ;
    virtual bool      Peep            (void) ;
    virtual bool      prepare         (void) ;

  protected:

    int  bufferTime     ;
    int  interval       ;
    bool waitCompletion ;
    bool keepWaiting    ;

    virtual void      run             (int Type,ThreadData * data) ;
    virtual void      Completion      (void) ;
    virtual void      setTransmission (void) ;
    virtual QImage *  ObtainImage     (void) ;
    virtual void      AudioAppended   (int newSize) ;
    virtual void      RollVideo       (void) ;
    virtual bool      hasVideo        (void) ;
    virtual bool      isVideoFull     (void) ;

    virtual QDateTime GuessTime       (void) ;
    virtual QDateTime AudioTime       (void) ;

    virtual void      finished        (void) ;

  private:

    TUID              AudioPending    (void) ;
    TUID              AudioFlushed    (void) ;

  public slots:

    virtual void      Loading         (void) ;
    virtual void      Watching        (void) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT SceneView : public GLWidget
                            , public SceneManager
{
  Q_OBJECT
  Q_PROPERTY(int Interval READ RenderingInterval WRITE setRenderingInterval)
  public:

    enum RunFunctionIDs        {
      RunRendering        =  0 ,
      RunLoading          =  1 ,
      RunAnimation        =  2 ,
      RunInputAudio       =  3 ,
      RunOutputAudio      =  4 ,
      RunNetInputAudio    =  5 ,
      RunNetOutputAudio   =  6 ,
      RunFeedAudio        =  7 ,
      RunFeedVideo        =  8 ,
      RunFeedInterface    =  9 ,
      RunAcceptInterface  = 10 ,
      RunFeedCommand      = 11 ,
      RunAcceptCommand    = 12 ,
      RunExecuteCommand   = 13 ,
      RunSemanticAnalysis = 14 ,
      RunDownload         = 15 ,
      RunUpload           = 16 ,
      RunFeedObjects      = 17 ,
      RunAcceptObjects    = 18 ,
      RunCapturing        = 19 ,
      RunCustom           = 99
    }                          ;

    SceneDestructor * destructor ;

    explicit      SceneView            (StandardConstructor) ;
    virtual      ~SceneView            (void) ;

    virtual QSize sizeHint             (void) const ;

    void          ForceRendering       (int ms) ;
    int           ForcingInterval      (void) ;
    int           RenderingInterval    (void) ;
    int           setRenderingInterval (int interval) ;

    SUID          RequestUniverse      (SUID startUuid) ;
    UUIDs         Multiverse           (void) ;
    Universe *    GetUniverse          (SUID uuid) ;
    Universe *    removeUniverse       (SUID uuid) ;
    bool          JoinDestructor       (SceneNode * node) ;

    double        ReflushRate          (void) ;

    CameraBox *   addCameraControl     (SUID cameraUuid,double length) ;
    Universe  *   addColorDistribution (SUID uuid,QByteArray & colors) ;

  protected:

    QTimer              * Timer            ;
    QDateTime             StartReflush     ;
    QDateTime             FlushTime        ;
    SUID                  PaintGLs         ;
    SUID                  PaintCosts       ;
    bool                  Alive            ;
    bool                  Stopped          ;
    bool                  Modified         ;
    bool                  Painting         ;
    bool                  Finalize         ;
    bool                  Adaptive         ;
    bool                  Moment           ;
    int                   Interval         ;
    int                   MotionInterval   ;
    int                   Lights           ;
    ControlPoint          background       ;
    BMAPs                 Properties       ;
    QMap<SUID,Universe *> Universes        ;
    QMap<SUID,QImage   *> ImageDepot       ;
    IMAPs                 Intervals        ;
    IMAPs                 Behaviors        ;
    UUIDs                 AudioQueues      ;
    NAMEs                 AudioPathes      ;
    NAMEs                 AudioNames       ;
    UUIDs                 VideoQueues      ;
    NAMEs                 VideoPathes      ;
    NAMEs                 VideoNames       ;
    UUIDs                 AlbumQueues      ;
    NAMEs                 AlbumNames       ;
    SMAPs                 VideoAlbumMaps   ;
    NAMEs                 DeviceProperties ;
    VarArgLists           GuiSequences     ;

    virtual void resizeEvent           (QResizeEvent      * event) ;
    virtual void showEvent             (QShowEvent        * event) ;
    virtual void closeEvent            (QCloseEvent       * event) ;
    virtual void focusInEvent          (QFocusEvent       * event) ;
    virtual void focusOutEvent         (QFocusEvent       * event) ;
    virtual void actionEvent           (QActionEvent      * event) ;
    virtual void changeEvent           (QEvent            * event) ;
    virtual void contextMenuEvent      (QContextMenuEvent * event) ;
    virtual void dragEnterEvent        (QDragEnterEvent   * event) ;
    virtual void dragLeaveEvent        (QDragLeaveEvent   * event) ;
    virtual void dragMoveEvent         (QDragMoveEvent    * event) ;
    virtual void dropEvent             (QDropEvent        * event) ;
    virtual void enterEvent            (QEvent            * event) ;
    virtual void hideEvent             (QHideEvent        * event) ;
    virtual void inputMethodEvent      (QInputMethodEvent * event) ;
    virtual void keyPressEvent         (QKeyEvent         * event) ;
    virtual void keyReleaseEvent       (QKeyEvent         * event) ;
    virtual void leaveEvent            (QEvent            * event) ;
    virtual void mouseDoubleClickEvent (QMouseEvent       * event) ;
    virtual void mouseMoveEvent        (QMouseEvent       * event) ;
    virtual void mousePressEvent       (QMouseEvent       * event) ;
    virtual void mouseReleaseEvent     (QMouseEvent       * event) ;
    virtual void moveEvent             (QMoveEvent        * event) ;
    virtual void paintEvent            (QPaintEvent       * event) ;
    virtual void tabletEvent           (QTabletEvent      * event) ;
    virtual void wheelEvent            (QWheelEvent       * event) ;

    virtual bool acceptTap             (QTapGesture        * gesture) ;
    virtual bool acceptTapHold         (QTapAndHoldGesture * gesture) ;
    virtual bool acceptPan             (QPanGesture        * gesture) ;
    virtual bool acceptPinch           (QPinchGesture      * gesture) ;
    virtual bool acceptSwipe           (QSwipeGesture      * gesture) ;
    virtual bool acceptCustom          (QGesture           * gesture) ;

    virtual void Configure             (void) ;

    virtual bool FocusIn               (void) ;
    virtual bool FocusOut              (void) ;

    virtual void initializeGL          (void);
    virtual void initializeOverlayGL   (void);
    virtual void paintGL               (void);
    virtual void paintOverlayGL        (void);
    virtual void resizeGL              (int width,int height);
    virtual void resizeOverlayGL       (int width,int height) ;

    virtual bool diehardPainter        (void) ;

    virtual bool acceptDrop            (nDeclWidget,const QMimeData * mime);
    virtual bool dropNew               (nDeclWidget,const QMimeData * mime,QPoint pos);
    virtual bool dropMoving            (nDeclWidget,const QMimeData * mime,QPoint pos);
    virtual bool dropAppend            (nDeclWidget,const QMimeData * mime,QPoint pos);

    virtual bool dropCameras           (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;
    virtual bool dropAudios            (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;
    virtual bool dropVideos            (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;
    virtual bool dropAlbums            (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;
    virtual bool dropPeople            (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;
    virtual bool dropPictures          (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;

    virtual void ThreadEvent           (void) ;
    virtual void run                   (int Type,ThreadData * data) ;

    virtual void DispatchCommands      (void) ;

    virtual Junction      * MountChannel  (int MID) ;
    virtual AudioAcceptor * SpeakerOutlet (AudioFeeder * feeder) ;
    virtual AudioAcceptor * WaveDisplay   (Junction * junc,AudioChannel * feeder) ;
    virtual WaveUniverse  * AllocateWave  (void) ;
    virtual AudioAcceptor * SpeakerOutlet (VideoFeeder * feeder) ;
    virtual Junction      * MountVideo    (int MID) ;
    virtual VideoAcceptor * VideoDisplay  (Junction * junc,VideoChannel * feeder) ;
    virtual VideoUniverse * AllocateVideo (void) ;

    virtual int  RequestChannel        (QString filename) ;
    virtual int  RequestVideo          (QString filename) ;
    virtual int  RequestInput          (QString device   ,
                                        int     rate     ,
                                        int     bits     ,
                                        int     channels ,
                                        int     format ) ;
    virtual int  JunctionCount         (void) ;
    virtual int  ConduitCount          (void) ;

    virtual void PrepareAudioInput     (QActionGroup * group) ;
    virtual void PrepareAudioOutput    (QActionGroup * group) ;
    virtual void PrepareCameraMenu     (QActionGroup * group) ;

    virtual bool AudioInputMenu        (QActionGroup * group,QAction * aa) ;
    virtual bool AudioOutputMenu       (QActionGroup * group,QAction * aa) ;
    virtual bool CameraInputMenu       (QActionGroup * group,QAction * aa) ;

    virtual void AudioFeederClosed     (VarArgs args) ;
    virtual void ChannelClosed         (AudioFeeder * feeder) ;
    virtual void AudioFailure          (SUID uuid,QString filename) ;
    virtual void UnmountAudioData      (AudioData * audio) ;

    virtual void VideoFeederClosed     (VarArgs args) ;
    virtual void VideoClosed           (VideoFeeder * feeder) ;
    virtual void VideoFailure          (SUID uuid,QString filename) ;
    virtual void UnmountSyncData       (SyncAcceptor * sync) ;

    QMap<SUID,QImage *> FetchImages    (SqlConnection & Connection,UUIDs & Uuids) ;
    QMap<SUID,QImage *> FetchImages    (UUIDs & Uuids) ;
    bool         RemoveImage           (SUID uuid) ;

  private:

    void         EnableWaveThrottle    (AudioAcceptor * waveu) ;

  public slots:

    virtual void InstallDecisions      (void) ;
    virtual bool startup               (void) ;
    virtual bool shutdown              (void) ;
    virtual bool shutup                (void) ;

    virtual void Relocation            (void) ;
    virtual void CopyToClipboard       (void) ;

    void         setBackground         (QColor & bgColor) ;
    void         assignBackground      (void) ;

    virtual void Prepare               (void) ;
    virtual void doUniverses           (void) ;
    virtual void Flush                 (void) ;
    virtual void Motion                (void) ;

    int          addUniverse           (SUID uuid,Universe * universe) ;
    bool         moveUniverse          (SUID from,SUID to) ;
    virtual bool DeleteUniverse        (Universe * universe) ;

    virtual void GoFor                 (int Type) ;
    virtual void GoFor                 (int Type,VarArgs arguments) ;
    virtual void Run                   (int Type,int Id) ;
    virtual void Rendering             (int Id) ;
    virtual void Loading               (int Id) ;
    virtual void Animation             (int Id) ;

    virtual void InputAudio            (int Id) ;
    virtual void OutputAudio           (int Id) ;
    virtual void NetInputAudio         (int Id) ;
    virtual void NetOutputAudio        (int Id) ;
    virtual void FeedAudio             (int Id) ;
    virtual void FeedVideo             (int Id) ;

    virtual void FeedInterface         (int Id) ;
    virtual void AcceptInterface       (int Id) ;

    virtual void FeedCommand           (int Id) ;
    virtual void AcceptCommand         (int Id) ;
    virtual void ExecuteCommand        (int Id) ;

    virtual void SemanticAnalysis      (int Id) ;
    virtual void Download              (int Id) ;
    virtual void Upload                (int Id) ;
    virtual void FeedObjects           (int Id) ;
    virtual void AcceptObjects         (int Id) ;
    virtual void Capturing             (int Id) ;
    virtual void Custom                (int Id) ;

    virtual void PostSequence          (VarArgs args) ;
    virtual void RunCommand            (VarArgs & arguments) ;

    virtual void CloseController       (CameraBox * camera) ;

    virtual void QueueAlbum            (SUID uuid,QString name) ;
    virtual void QueueVideo            (SUID uuid,QString name) ;
    virtual void QueueAudio            (SUID uuid,QString name) ;
    virtual void QueuePeople           (SUID uuid,QString name,QPointF pos = QPointF(-1,-1)) ;
    virtual void QueuePictures         (SUID uuid,QString name,QPointF pos = QPointF(-1,-1)) ;

    virtual void dropAlbum             (VarArgs & arguments) ;
    virtual void dropVideo             (VarArgs & arguments) ;
    virtual void dropAudio             (VarArgs & arguments) ;
    virtual void dropPeople            (VarArgs & arguments) ;
    virtual void dropCamera            (VarArgs & arguments) ;
    virtual void dropPicture           (VarArgs & arguments) ;
    virtual void ShutAudio             (VarArgs & arguments) ;
    virtual void ShutVideo             (VarArgs & arguments) ;
    virtual void AddRecorder           (VarArgs & arguments) ;
    virtual void playAudio             (VarArgs & arguments) ;
    virtual void playVideo             (VarArgs & arguments) ;

    virtual void dropPeopleAt          (QPointF pos,UUIDs & Uuids) ;
    virtual void dropPictureAt         (QPointF pos,UUIDs & Uuids) ;
    virtual void dropCameraAt          (QPointF pos,UUIDs & Uuids) ;

    virtual bool CallForAlbum          (void) ;
    virtual bool CallForVideo          (void) ;
    virtual bool CallForAudio          (void) ;

  protected slots:

    virtual bool Menu                  (QPoint pos) ;
    virtual bool UniverseMenu          (QPoint pos) ;
    virtual bool ReflushMenu           (MenuManager & menu,QPoint pos) ;
    virtual bool ThreadsMenu           (MenuManager & menu,QPoint pos) ;
    virtual bool ThreadsProcessor      (MenuManager & menu,QAction * action) ;
    virtual void ThreadController      (void) ;
    virtual bool BridgeMenu            (MenuManager & menu,QPoint pos) ;
    virtual bool BridgeProcessor       (MenuManager & menu,QAction * action) ;
    virtual bool ContentMenu           (MenuManager & menu,QPoint pos) ;
    virtual bool ContentProcessor      (MenuManager & menu,QAction * action) ;
    virtual bool MultimediaMenu        (MenuManager & menu,QPoint pos) ;
    virtual bool AskForAudioInput      (MenuManager & menu,QMenu * m,QActionGroup * group) ;
    virtual bool MultimediaProcessor   (MenuManager & menu,QAction * action) ;
    virtual bool AudioInputProcessor   (MenuManager & menu,QAction * action) ;

    virtual bool assureAlbums          (UUIDs & Uuids) ;
    virtual bool assureVideos          (UUIDs & Uuids) ;
    virtual void tossVideos            (void) ;
    virtual void ExamVideo             (SUID uuid) ;
    virtual void LaunchVideo           (VarArgs arguments) ;

    virtual bool assureAudios          (UUIDs & Uuids) ;
    virtual void tossAudios            (void) ;
    virtual void ExamAudio             (SUID uuid) ;
    virtual void LaunchAudio           (VarArgs arguments) ;
    virtual void LaunchRecorder        (VarArgs arguments) ;

    virtual void GuiDispatch           (void) ;

  private slots:

  signals:

    void Go                            (int Type,int Id) ;
    void Going                         (int Type) ;
    void Going                         (int Type,VarArgs arguments) ;
    void Reflush                       (void) ;
    void audioAccepted                 (void) ;
    void videoAccepted                 (void) ;
    void audioStarting                 (VarArgs arguments) ;
    void videoStarting                 (VarArgs arguments) ;
    void recorderStarting              (VarArgs arguments) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                             Multimedia widgets                            *
 *                                                                           *
 *****************************************************************************/

class Q_AV_EXPORT PickAudio : public TreeWidget
{
  Q_OBJECT
  public:

    explicit     PickAudio     (StandardConstructor) ;
    virtual     ~PickAudio     (void) ;

    QString      deviceName    (void) ;

  protected:

    int             DefaultId ;
    SUID            Notify    ;
    QMap<int,QIcon> Icons     ;

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual void List          (Acoustics::Equipment equipment) ;
    virtual void setDefault    (int deviceId) ;
    virtual void setClickable  (int click) ;
    virtual void setIcon       (bool enable,QIcon icon) ;
    virtual void setNotify     (SUID notify) ;

  protected slots:

    virtual void deviceClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void DeviceChanged         (int deviceId) ;

} ;

class Q_AV_EXPORT AudioDepot : public TreeWidget
                             , public Ownership
{
  Q_OBJECT
  public:

    explicit AudioDepot               (StandardConstructor) ;
    virtual ~AudioDepot               (void);

    virtual QStringList defaultHeader (void);

  protected:

    QTreeWidgetItem * ItemEdit   ;
    int               ItemColumn ;
    QLineEdit       * TextEdit   ;
    QString           LastPath   ;

    virtual bool FocusIn              (void);
    virtual void Configure            (void);

    virtual bool acceptDrop           (QWidget * source,const QMimeData * mime);
    virtual bool dropAppend           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropUrls             (QWidget * source,QPointF pos,const URLs & urls) ;

    virtual void run                  (int Type,ThreadData * Data) ;

  private:

    void         AppendFiles          (QStringList files) ;

  public slots:

    virtual bool startup              (void) ;

    void         List                 (void) ;
    void         Insert               (void) ;
    void         Verify               (void) ;
    void         Delete               (void) ;
    void         Play                 (void) ;
    void         Play                 (SUID uuid) ;
    void         Append               (QString filename) ;

  protected slots:

    virtual bool Menu                 (QPoint pos);
    void         doubleClicked        (QTreeWidgetItem * item,int column);
    void         editingFinished      (void) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT CodecSupports : public TreeWidget
{
  Q_OBJECT
  public:

    explicit CodecSupports  (StandardConstructor) ;
    virtual ~CodecSupports  (void);

    QString TypeName        (int codectype);

  protected:

    virtual void Configure  (void) ;

    void AddItem            (QString name,int ID);

  private:

  public slots:

    void Detect             (void);

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT AudioEncoderSupports : public TreeWidget
{
  Q_OBJECT
  public:

    explicit AudioEncoderSupports (StandardConstructor) ;
    virtual ~AudioEncoderSupports (void);

    virtual QSize sizeHint        (void) const ;

  protected:

    virtual void Configure        (void) ;

    void AddItem                  (QString name,int ID) ;

  private:

  public slots:

    void Detect                   (void);

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoBrowser : public FileView
{
  Q_OBJECT
  public:

    explicit VideoBrowser          (StandardConstructor) ;
    virtual ~VideoBrowser          (void);

  protected:

    virtual QStringList Filters    (void);
    virtual QListWidgetItem * Load (QFileInfo & info);

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT MusicAlbums : public SnapView
                              , public Group
{
  Q_OBJECT
  public:

    explicit            MusicAlbums     (StandardConstructor) ;
    virtual            ~MusicAlbums     (void) ;

    virtual bool        contains        (SUID uuid) ;

  protected:

    bool dropAction ;

    virtual void        closeEvent      (QCloseEvent * e) ;

    virtual bool        FocusIn         (void);
    virtual void        Configure       (void) ;

    virtual QMimeData * dragMime        (void);

    virtual bool        acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType        (const QMimeData * mime);
    virtual UUIDs       MimeUuids       (const QMimeData * mime,QString mimetype) ;

    virtual bool        dropText        (nDeclDrops,const QString     & text  ) ;
    virtual bool        dropUrls        (nDeclDrops,const QList<QUrl> & urls  ) ;
    virtual bool        dropImage       (nDeclDrops,const QImage      & image ) ;
    virtual bool        dropHtml        (nDeclDrops,const QString     & html  ) ;
    virtual bool        dropTags        (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPictures    (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople      (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropAudios      (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropMusic       (nDeclDrops,const UUIDs       & Uuids ) ;

    virtual void        run             (int Type,ThreadData * data) ;
    virtual bool        ItemRunner      (int Type,ThreadData * data) ;

    virtual void        JoinText        (ThreadData * data) ;
    virtual void        JoinURLs        (ThreadData * data) ;
    virtual void        JoinImage       (ThreadData * data) ;
    virtual void        JoinHtml        (ThreadData * data) ;
    virtual void        JoinTags        (ThreadData * data) ;
    virtual void        JoinAudios      (ThreadData * data) ;
    virtual void        JoinAlbums      (ThreadData * data) ;
    virtual void        AppendMusic     (ThreadData * data) ;
    virtual void        setPositions    (ThreadData * data) ;

    virtual bool        canDelete       (void) ;
    virtual bool        isOwner         (void) ;
    virtual void        StopIcons       (void) ;

    virtual SUID        appendAlbum     (SqlConnection   & SC         ,
                                         QString           name     ) ;
    virtual bool        setUuid         (SqlConnection   & Connection ,
                                         SUID              uuid       ,
                                         QListWidgetItem * item     ) ;
    virtual bool        addUuid         (SqlConnection   & Connection ,
                                         SUID              uuid     ) ;
    virtual void        DetachAlbums    (SqlConnection   & Connection ,
                                         UUIDs           & Uuids    ) ;
    virtual void        JoinMusic       (SqlConnection   & Connection ,
                                         SUID              uuid     ) ;
    virtual void        JoinMusic       (SqlConnection   & Connection ,
                                         SUID              uuid       ,
                                         QListWidgetItem * Item     ) ;

    QListWidgetItem   * newUuid         (SqlConnection   & Connection ,
                                         SUID              uuid     ) ;
    virtual bool        ReportDetails   (SqlConnection   & Connection ,
                                         QListWidgetItem * item       ,
                                         int               flags    ) ;
    virtual bool        CountAudios     (SqlConnection   & SC         ,
                                         QListWidgetItem * it       ) ;

    virtual void        FetchUUIDs      (void) ;
    virtual void        FetchTOTALs     (void) ;

  private:

  public slots:

    virtual void        View            (UUIDs & Uuids) ;

    virtual bool        startup         (void) ;
    virtual void        FolderLanguage  (void) ;
    virtual void        ResumeLanguage  (void) ;

    virtual bool        RunCommand      (VarArgs & arguments) ;

    virtual void        Insert          (void) ;
    virtual void        Remove          (void) ;
    virtual void        Refresh         (QListWidgetItem * item) ;
    virtual void        Paste           (void) ;
    virtual void        Paste           (QString text) ;
    virtual void        Move            (SUID before,const UUIDs & Uuids) ;

  protected slots:

    virtual void        singleClicked   (QListWidgetItem * item) ;

    virtual bool        Menu            (QPoint pos) ;

    virtual void        RemoveOldItem   (void) ;
    virtual void        editingFinished (void) ;
    virtual void        doTranslations  (void) ;

  private slots:

  signals:

    void Translations                   (QString title,UUIDs & Uuids) ;
    void EditNames                      (QWidget * w,SUID u,QString name) ;
    void Details                        (int type,SUID u,QString name) ;

} ;

class Q_AV_EXPORT VideoView : public SnapView
                            , public Group
{
  Q_OBJECT
  public:

    explicit            VideoView       (StandardConstructor) ;
    virtual            ~VideoView       (void);

  protected:

    bool        dropAction ;
    QStringList ItemNames  ;
    ComboBox  * Keywords   ;

    virtual bool        FocusIn         (void);
    virtual void        Configure       (void);

    virtual void        closeEvent      (QCloseEvent * event);

    virtual QMimeData * dragMime        (void);

    virtual bool        acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType        (const QMimeData * mime);
    virtual UUIDs       MimeUuids       (const QMimeData * mime,QString mimetype) ;

    bool                MatchString     (QString sentence,QStringList keywords) ;

    virtual bool        dropText        (nDeclDrops,const QString & text) ;
    virtual bool        dropUrls        (nDeclDrops,const QList<QUrl> & urls) ;
    virtual bool        dropImage       (nDeclDrops,const QImage & image) ;
    virtual bool        dropHtml        (nDeclDrops,const QString & html) ;
    virtual bool        dropTags        (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropPictures    (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropPeople      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropVideos      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropAlbums      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropTorrents    (nDeclDrops,const UUIDs & Uuids) ;

    virtual void        StopIcons       (void) ;
    virtual void        run             (int Type,ThreadData * data) ;

    virtual bool        isOwner         (void) ;

    virtual bool        AlbumDetails    (SqlConnection & Connection,QListWidgetItem * item) ;
    virtual bool        ReportDetails   (SqlConnection & Connection,QListWidgetItem * item,int flags) ;
    virtual void        FetchUUIDs      (void) ;
    virtual void        FetchTOTALs     (void) ;

    virtual SUID        appendAlbum     (SqlConnection & Connection,QString name) ;

    virtual bool        Delete          (ThreadData * data) ;

  private:

    void                drawRating      (QImage          & image,int rate) ;
    void                drawRating      (QListWidgetItem * item ,int rate) ;

  public slots:

    virtual bool        startup         (void) ;
    virtual void        FolderLanguage  (void) ;
    virtual void        ResumeLanguage  (void) ;
    virtual void        Paste           (void) ;
    virtual void        Paste           (QString text) ;
    virtual void        addAlbum        (SqlConnection & Connection,QString name) ;
    virtual void        View            (UUIDs & Uuids) ;
    virtual void        JoinUuids       (QString mime,QPoint pos,SUID before,UUIDs & Uuids) ;
    virtual void        MoveUuids       (QString mime,QPoint pos,SUID before,UUIDs & Uuids) ;

    virtual void        Search          (void) ;
    virtual void        New             (void) ;
    virtual void        Rating          (void) ;
    virtual void        Delete          (void) ;
    virtual void        setPositions    (void) ;

    virtual void        Remove          (SUID uuid) ;
    virtual void        Remove          (UUIDs & Uuids) ;

    virtual void        Rating          (QListWidgetItem * item) ;
    virtual void        Rename          (QListWidgetItem * item) ;
    virtual void        Refresh         (QListWidgetItem * item);

    virtual void        MoveAlbums      (SUID atUuid,const UUIDs & Uuids) ;
    virtual void        JoinAlbums      (SUID atUuid,      UUIDs & Uuids) ;

    virtual void        DefaultMenu     (void) ;

    virtual void        RelatedItems    (SUID uuid,QString title) ;

    virtual void        IconsThread     (Group group,int start,int end) ;

    virtual bool        RunCommand      (VarArgs & arguments) ;

  protected slots:

    virtual bool        Menu            (QPoint pos) ;
    virtual bool        VideoMenu       (QWidget * widget,QPoint pos) ;

    virtual void        doTranslations  (void) ;

    virtual void        renameFinished  (void) ;
    virtual void        editingFinished (void) ;
    virtual void        rateFinished    (void) ;
    virtual void        SearchFinished  (void) ;
    virtual void        SearchChanged   (const QString & text) ;

  private slots:

  signals:

    void                NewAlbum        (void) ;
    void                ShowMenu        (QWidget * widget,QPoint pos) ;
    void                FullNames       (SUID album) ;
    void                Edit            (SUID album,QString name) ;
    void                Gallery         (SUID album,QString name) ;
    void                Actor           (SUID album,QString name) ;
    void                Details         (SUID album,QString name) ;
    void                Clips           (SUID album,QString name) ;
    void                Torrents        (SUID album,QString name) ;
    void                Bookmark        (SUID album,QString name) ;
    void                Search          (SUID album,QString name) ;
    void                Play            (SUID album,QString name) ;
    void                UuidItems       (UUIDs & Uuids,RMAPs & Types,NAMEs & Names,QString title) ;
    void                Translations    (QString title,UUIDs & Uuids) ;

} ;

class Q_AV_EXPORT VideoContainer : public VideoView
{
  Q_OBJECT
  public:

    UUIDs Albums ;

    explicit VideoContainer           (StandardConstructor) ;
    virtual ~VideoContainer           (void);

  protected:

    virtual void        Configure     (void);

  private:

  public slots:

    virtual bool        startup       (void) ;
    virtual bool        startup       (UUIDs & Uuids,int sourceType = 0);

    virtual void        MoveAlbums    (SUID atUuid,const UUIDs & Uuids) ;
    virtual void        JoinAlbums    (SUID atUuid,UUIDs & Uuids) ;

  protected slots:

    virtual bool        ActorMenu     (QWidget * widget,QPoint pos) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT AlbumView : public GroupView
{
  Q_OBJECT
  public:

    explicit AlbumView             (StandardConstructor) ;
    virtual ~AlbumView             (void);

  protected:

    virtual void    Configure      (void) ;

    virtual QString MimeType       (const QMimeData * mime);
    virtual UUIDs   MimeUuids      (const QMimeData * mime,QString mimetype) ;

    virtual UUIDs   GroupUuids     (SqlConnection & Connection) ;
    virtual QString ToolTipTotoal  (int total) ;
    virtual int     Counting       (SqlConnection & Connection,SUID uuid) ;
    virtual SUID    AppendGroup    (SqlConnection & Connection,QString name) ;

    virtual void    reportItems    (void) ;

  private:

  public slots:

    virtual void    GroupPositions (RMAPs & Position,UUIDs & Uuids) ;
    virtual bool    MoveGroups     (SUID uuid,const UUIDs  & Uuids) ;
    virtual bool    JoinImage      (SUID uuid,const QImage & image) ;
    virtual bool    JoinPictures   (SUID uuid,const UUIDs  & Uuids) ;

  protected slots:

    virtual bool    Menu           (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoTags : public TagsEditor
{
  Q_OBJECT
  public:

    explicit     VideoTags        (StandardConstructor) ;
    virtual     ~VideoTags        (void) ;

  protected:

    QLineEdit  * SetsOp ;

    virtual void Configure        (void) ;

    virtual bool addMenuItems     (MenuManager & menu,QPoint pos) ;

    SUID         TagUuid          (SqlConnection & Connection,int type,QString name) ;

    virtual void TagsUnion        (ThreadData * data) ;
    virtual void TagsIntersection (ThreadData * data) ;

  private:

  public slots:

    virtual bool RunMenuResult    (QAction * action,MenuManager & menu,QPoint pos) ;
    virtual bool SearchSets       (void) ;

  protected slots:

    virtual void setsFinished     (void) ;
    virtual void SendPending      (void) ;

  private slots:

  signals:

    void TagSelected              (QString name,SUID    tag  ) ;
    void ListVideo                (QString name,UUIDs & Uuids) ;

} ;

class Q_AV_EXPORT AlbumClip : public TreeDock
                            , public Ownership
                            , public VideoManager
{
  Q_OBJECT
  public:

    explicit AlbumClip             (StandardConstructor) ;
    virtual ~AlbumClip             (void);

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool        acceptDrop (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType   (const QMimeData * mime);
    virtual UUIDs       MimeUuids  (const QMimeData * mime,QString mimetype) ;

    virtual bool        dropVideos (nDeclDrops,const UUIDs & Uuids) ;

    virtual void run               (int type,ThreadData * data) ;

    virtual bool DropClips         (const UUIDs & Uuids) ;
    virtual void Remove            (void) ;
    virtual void UpdatePosition    (void) ;

    virtual bool AppendClips       (VarArgs & args) ;

    virtual void Reload            (void) ;

  private:

    UUIDs       UuidOrders         (void) ;
    NAMEs       ClipNames          (UUIDs & Uuids) ;
    QComboBox * ClipSelection      (void) ;

  public slots:

    virtual bool startup           (void) ;

    virtual void Insert            (void) ;
    virtual void Delete            (void) ;
    virtual void ItemUp            (void) ;
    virtual void ItemDown          (void) ;

  protected slots:

    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void removeOldItem             (void) ;
    void clipChanged               (int index) ;

    virtual bool Menu              (QPoint pos);

  private slots:

  signals:

    void Play       (SUID uuid,QString name) ;

} ;

class Q_AV_EXPORT ClipList : public TreeWidget
                           , public VideoManager
{
  Q_OBJECT
  public:

    explicit ClipList             (StandardConstructor) ;
    virtual ~ClipList             (void);

  protected:

    QPoint      dragPoint   ;
    QStringList WaitingList ;
    bool        Adding      ;
    NAMEs       ScriptID    ;

    virtual void closeEvent       (QCloseEvent * e) ;

    virtual bool FocusIn          (void) ;
    virtual void Configure        (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool        acceptDrop (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropUrls   (nDeclDrops,const QList<QUrl> & urls) ;

    virtual QString     MimeType   (const QMimeData * mime);
    virtual UUIDs       MimeUuids  (const QMimeData * mime,QString mimetype) ;

    virtual void        run        (int type,ThreadData * data) ;

  private:

    void LoadScripts               (SqlConnection & Connection) ;
    void LoadClip                  (SqlConnection & Connection,SUID uuid) ;
    QString ScriptContent          (SqlConnection & Connection,SUID uuid) ;

    virtual bool AppendNext        (void) ;
    virtual void Appending         (void) ;

  public slots:

    virtual bool startup           (void) ;
    virtual bool startup           (UUIDs & Uuids) ;

    virtual void Reload            (void) ;
    virtual void Empty             (void) ;
    virtual void List              (UUIDs & Uuids) ;
    virtual void Add               (QFileInfo & info) ;
    virtual void Add               (QStringList & files) ;
    virtual void Pending           (void) ;
    virtual void Insert            (void) ;
    virtual void AddFiles          (void) ;

    virtual bool Integrity                 (QString name,QString filename) ;
    virtual bool AddClip                   (QString name,QString filename,QTreeWidgetItem * item) ;

  protected slots:

    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void         removeOldItem     (void) ;
    void         editingFinished   (void) ;

    virtual bool Menu              (QPoint pos);

  private slots:

  signals:

    void ClipNames                 (SUID clip,QString name) ;
    void AnalyzeClip               (SUID clip,QString name) ;
    void ClipInformation           (SUID clip,QString name) ;

} ;

class Q_AV_EXPORT VideoLists : public TreeDock
                             , public VideoManager
{
  Q_OBJECT
  public:

    explicit VideoLists            (StandardConstructor) ;
    virtual ~VideoLists            (void) ;

  protected:

    QMutex     Mutex        ;
    QPoint     dragPoint    ;
    LineEdit * SearchEditor ;
    bool       KeepGo       ;

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool  acceptDrop       (QWidget * source,const QMimeData * mime           ) ;
    virtual bool  dropNew          (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool  dropMoving       (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool  dropAppend       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual bool  dropUrls         (nDeclDrops,const QList<QUrl> & urls ) ;
    virtual bool  dropVideos       (nDeclDrops,const UUIDs       & Uuids) ;
    virtual bool  dropAlbums       (nDeclDrops,const UUIDs       & Uuids) ;

    virtual bool  Load             (SqlConnection   & Connection ,
                                    QStringList     & Depots     ,
                                    QTreeWidgetItem * item       ,
                                    SUID              uuid     ) ;

    qint64        checkClip        (QStringList & Depots,QTreeWidgetItem * item) ;
    qint64        verifyClip       (QStringList & Depots,QTreeWidgetItem * item) ;
    SUID          AppendAudio      (QString filename,SUID vuid,QString name) ;
    virtual bool  Integrity        (QString name,QString filename) ;
    virtual bool  ReplaceClip      (SUID u,QString filename,QTreeWidgetItem * item) ;

    void          Dump             (QTreeWidgetItem * item) ;
    void          Verify           (QTreeWidgetItem * item) ;

    virtual bool  RemoveClip       (SqlConnection & SC,SUID clip) ;

    virtual void  run              (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool Lists             (void) ;
    virtual bool Lists             (int total) ;
    virtual bool Lists             (UUIDs & U) ;

    virtual void ListAll           (void) ;
    virtual void List              (int total) ;
    virtual void List              (VarArgs & args) ;

    virtual void Delete            (void) ;
    virtual void Search            (void) ;
    virtual void Rename            (void) ;
    virtual void Stop              (void) ;
    virtual void Clear             (void) ;
    virtual void Replace           (QTreeWidgetItem * item) ;
    virtual void Replace           (VarArgs & args) ;
    virtual void DistillAll        (void) ;
    virtual void Distill           (QTreeWidgetItem * item) ;

    virtual void VerifyAll         (void) ;
    virtual void DumpAll           (void) ;

    virtual void ClearFlags        (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void         searchFinished    (void) ;
    void         removeEditor      (void) ;
    void         nameFinished      (void) ;

  private slots:

  signals:

    void Play                      (SUID uuid,QString name) ;
    void ClipNames                 (SUID uuid,QString name) ;

} ;

class Q_AV_EXPORT PlayList : public TreeDock
                           , public VideoManager
{
  Q_OBJECT
  public:

    explicit     PlayList          (StandardConstructor) ;
    virtual     ~PlayList          (void) ;

    UUIDs        WaitingUUIDs      (void) ;

  protected:

    QMutex   Mutex     ;
    QPoint   dragPoint ;
    int      Mode      ;

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool  acceptDrop       (QWidget * source,const QMimeData * mime           ) ;
    virtual bool  dropNew          (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool  dropMoving       (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool  dropAppend       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual bool  dropUrls         (nDeclDrops,const QList<QUrl> & urls ) ;
    virtual bool  dropVideos       (nDeclDrops,const UUIDs       & Uuids) ;
    virtual bool  dropAlbums       (nDeclDrops,const UUIDs       & Uuids) ;

    void          addClip          (SqlConnection & Connection,QStringList & Depots,SUID clip) ;

    virtual void  run              (int Type,ThreadData * data) ;
    virtual bool  AddVideos        (ThreadData * data) ;
    virtual bool  AddAlbums        (ThreadData * data) ;

  private:

  public slots:

    virtual bool  startup          (void) ;
    virtual bool  Lists            (void) ;
    virtual bool  New              (void) ;
    virtual bool  Remove           (void) ;
    virtual void  Export           (void) ;
    virtual void  Submit           (void) ;
    virtual void  Arrange          (void) ;
    virtual void  FileList         (void) ;
    virtual void  OpList           (void) ;

    virtual void  RemoveAll        (void) ;
    virtual void  ClearPlayed      (void) ;

    virtual void  SaveTo           (QString filename) ;
    virtual void  LoadFrom         (QString filename) ;
    virtual void  Play             (QString filename) ;

    virtual void  CheckNext        (void) ;
    virtual void  CheckTotalTime   (void) ;

    virtual void  AcceptShell      (void) ;
    virtual void  AcceptPlaying    (SUID u) ;
    virtual void  AcceptStopped    (SUID u) ;

  protected slots:

    virtual bool  Menu             (QPoint pos) ;

    virtual void  singleClicked    (QTreeWidgetItem * item,int column) ;
    virtual void  doubleClicked    (QTreeWidgetItem * item,int column) ;

    void          clipChanged      (int index) ;
    void          selectClips      (void) ;
    void          timeChanged      (void) ;
    void          setPlayTime      (void) ;

  private slots:

  signals:

    void Play                      (SUID uuid,QString name) ;

} ;

class Q_AV_EXPORT PlayController
{
  public:

    QProcess    Process   ;
    QFileInfo   Player    ;
    QStringList Paraments ;

    explicit PlayController  (QObject * parent = NULL) ;
    virtual ~PlayController  (void) ;

    void setPlayer           (QString player) ;
    void Play                (QString filename) ;
    bool connectTo           (QObject * parent) ;
    void waitFinished        (void) ;
    void Command             (QString cmd);

  protected:

    bool       startReport ;
    int        lastIndex   ;
    QByteArray StdOut      ;
    QMutex     PlayMutex   ;

    void AppendStandOutput   (QByteArray & SOUT) ;
    bool canReadLine         (void) ;
    QByteArray readLine      (void) ;

    void readStandardOutput  (void);
    virtual bool Parse       (QByteArray & Response) ;
    virtual void Progress    (QByteArray Line) ;
    virtual void Progress    (QStringList & parements) ;

  private:

} ;

class Q_AV_EXPORT AudioDisplay : public PaintWidget
{
  Q_OBJECT
  Q_PROPERTY(int interval READ Interval WRITE setInterval)
  public:

    Junction        * junction  ;
    AudioAcceptor   * acceptor  ;
    AudioChannel    * channel   ;
    AudioIO         * Audio     ;
    RangeInt          Range     ;
    RangeDouble       Bounds    ;
    QMap<int,Vector > Vectors   ;
    QMap<int,QPointF> Shifts    ;
    QMap<int,QSizeF > Scales    ;
    QMap<int,QPen   > PENs      ;
    double            Thickness ;

    explicit AudioDisplay   (StandardConstructor) ;
    virtual ~AudioDisplay   (void) ;

    virtual QSize sizeHint  (void) const ;

    int     Interval        (void) ;
    void    setInterval     (int interval) ;

  protected:

    int interval ;

    virtual void closeEvent (QCloseEvent * event) ;

    virtual void Configure  (void) ;

    virtual void run        (int type,ThreadData * data) ;

    void         DrawWave   (void) ;

    virtual void Place      (int channel,int channels,QSize size) ;

  private:

  public slots:

    virtual void startup    (void) ;
    virtual void reading    (void) ;
    virtual void stop       (void) ;
    virtual void relocation (void) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT PadRecorder : public Splitter
{
  Q_OBJECT
  public:

    QDir Root    ;
    QDir Current ;

    explicit PadRecorder    (StandardConstructor) ;
    virtual ~PadRecorder    (void) ;

    virtual QSize sizeHint  (void) const ;

  protected:

    Ui::PadRecorder  *  ui        ;
    AudioDisplay     *  display   ;
    AudioWriter      *  writer    ;
    AudioAcceptor    *  wacceptor ;
    QMap<int,QAction *> Actions   ;
    QMap<int,BMAPs    > Displays  ;
    QTimer              Timer     ;
    QTime               Elapsed   ;

    virtual void Configure  (void) ;

  private:

    void addAction          (int Id,QString text,QIcon icon) ;
    void AddItem            (int ID,int rate) ;
    void AppendItems        (int rate) ;

    int  RequestInput       (QString device   ,
                             int     rate     ,
                             int     bits     ,
                             int     channels ,
                             int     format ) ;
    Junction * MountChannel (int MID) ;

  public slots:

    void Display            (int conf) ;
    void setPlay            (bool enable) ;
    void TimeElapsed        (void) ;
    void Detect             (void) ;
    void DeviceChanged      (int) ;
    void ListSamples        (int) ;
    void ListEncoders       (int) ;

    void Play               (void) ;
    void Record             (void) ;
    void Stop               (void) ;
    void Next               (void) ;

  protected slots:

    void RecordFinished     (void) ;

  private slots:

  signals:

    void Back               (void) ;

} ;

class Q_AV_EXPORT VideoPanel : public Widget
                             , public PlayController
{
  Q_OBJECT
  public:

    explicit VideoPanel          (StandardConstructor) ;
    virtual ~VideoPanel          (void);

    bool  isPlaying              (void) ;
    TUID  Length                 (void) ;
    TUID  Now                    (void) ;
    UUIDs PlayingUuids           (void) ;

  protected:

    Ui::nVideoPanel * ui          ;
    QMutex            KeyMutex    ;
    QDateTime         LastTime    ;
    TUID              Total       ;
    TUID              Current     ;
    bool              Seeking     ;
    bool              Trigger     ;
    QStringList       Files       ;
    QStringList       Names       ;
    CMAPs             Uuids       ;
    QMap<int,TUID>    TimeTables  ;
    CMAPs             Events      ;

    virtual void dragEnterEvent  (QDragEnterEvent * event) ;
    virtual void dragLeaveEvent  (QDragLeaveEvent * event) ;
    virtual void dragMoveEvent   (QDragMoveEvent  * event) ;
    virtual void dropEvent       (QDropEvent      * event) ;

    virtual bool acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString MimeType     (const QMimeData * mime) ;
    virtual UUIDs   MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual void Progress        (QStringList & parements) ;

  private:

  public slots:

    void Clear                   (void);
    void Delete                  (void);
    void Browse                  (void);
    void Next                    (void);
    void Export                  (void);
    void TimeTable               (void);
    void Database                (void);
    void Playlist                (void);
    void Analysis                (void);
    void User                    (void);
    bool Start                   (QString filename,QString name) ;
    bool Stop                    (bool trigger = true) ;
    void FullScreen              (void) ;
    void Fast                    (void) ;
    void Back                    (void) ;
    void Normal                  (void) ;
    void Pause                   (void) ;
    void State                   (bool enable) ;
    void TimeMoved               (int ms) ;
    void appendFiles             (QStringList & files) ;
    void appendUrls              (QList<QUrl> & Urls) ;
    void appendUuids             (UUIDs & Uuids) ;
    void appendClips             (UUIDs & Uuids) ;
    void appendEvent             (SUID event,SUID clip);
    void playEvent               (SUID event,SUID clip);

  protected slots:

    void finished                (int exitCode,QProcess::ExitStatus exitStatus);
    void readyReadStandardOutput (void);
    void started                 (void);

  private slots:

  signals:

    void Append                  (QStringList & files) ;
    void Analysis                (QStringList & files) ;
    void User                    (QStringList & files) ;

} ;

class Q_AV_EXPORT AudioPanel : public StackedWidget
                             , public AvDecoder
                             , public CiosAudio::Conduit
{
  Q_OBJECT
  Q_PROPERTY(int Freq READ Frequency WRITE setFrequency)
  Q_PROPERTY(double FreqFactor READ Factor WRITE setFactor)
  public:

    typedef enum   {
      Label    = 0 ,
      Display  = 1 ,
      List     = 2 ,
      Progress = 3 ,
      Volume   = 4 ,
      Buttons  = 5 }
      WidgetType   ;

    explicit       AudioPanel   (StandardConstructor) ;
    virtual       ~AudioPanel   (void) ;

    virtual QSize  sizeHint     (void) const ;

    QWidget *      widget       (WidgetType type) ;

    virtual int    obtain       (void) ;
    virtual int    put          (void) ;
    virtual void   finish       (ConduitDirection direction = NoDirection ,
                                 FinishCondition  condition = Correct   ) ;

    virtual int    Frequency    (void) const ;
    virtual int    setFrequency (int freq) ;
    virtual double Factor       (void) const ;
    virtual double setFactor    (double factor) ;

    virtual int    audioVolume  (void) ;

  protected:

    CiosAudio::Core      *  core        ;
    CiosAudio::Stream    *  stream      ;
    QSplitter            *  vertical    ;
    QSplitter            *  horizontal  ;
    QLabel               *  label       ;
    ToolWrapper          *  wrapper     ;
    StackedWidget        *  stack       ;
    StackedWidget        *  panel       ;
    PaintWidget          *  display     ;
    TreeWidget           *  list        ;
    QSlider              *  progress    ;
    QSlider              *  volume      ;
    Widget               *  buttons     ;
    Widget               *  files       ;
    Widget               *  database    ;
    AudioLists           *  depot       ;
    QMap<int,QToolButton *> functions   ;
    BMAPs                   Booleans    ;
    QTimer                  Timer       ;
    QString                 lastPath    ;
    long long               totalOutput ;
    long long               duration    ;
    long long               currentPTS  ;
    int                     MaxFreq     ;
    int                     Freq        ;
    double                  FreqFactor  ;
    RangeInt                Range       ;
    RangeDouble             Bounds      ;
    QMap<int,Vector >       Vectors     ;
    QMap<int,QPointF>       Shifts      ;
    QMap<int,QSizeF >       Scales      ;
    QMap<int,QPen   >       PENs        ;
    double                  Thickness   ;
    AudioData               audioData   ;
    AudioData               waveData    ;

    virtual void closeEvent     (QCloseEvent * event) ;

    virtual void Configure      (void) ;

    virtual void run            (int type,ThreadData * data) ;

    virtual void Monitor        (void) ;
    virtual void Play           (ThreadData * data,QString filename) ;
    virtual void OpenDevice     (void) ;
    virtual void DisplayWave    (void) ;
    virtual void PaintWave      (AudioData & data) ;

    virtual void Place          (int channel,int channels,QSize s) ;

  private:

    bool         shutdown       (void) ;

  public slots:

    virtual void setWrapper     (ToolWrapper * w) ;
    virtual void Appear         (void) ;
    virtual void Disappear      (void) ;
    virtual void Eradicate      (void) ;

    virtual void showStyle      (int style) ;

    virtual void AddChannel     (QString name,SUID uuid,QString filename) ;
    virtual void Add            (QString name,SUID uuid,QString filename) ;
    virtual void Add            (QString filename) ;
    virtual void Add            (SUID uuid) ;
    virtual void Browse         (void) ;

    virtual void Pad            (void) ;
    virtual void Files          (void) ;
    virtual void Depot          (void) ;
    virtual void Renew          (void) ;

    virtual void PLAY           (void) ;
    virtual void PAUSE          (void) ;
    virtual void STOP           (void) ;
    virtual void NEXT           (void) ;

    virtual void REMOVE         (void) ;
    virtual void UP             (void) ;
    virtual void DOWN           (void) ;

    virtual void setAudioVolume (int value) ;

  protected slots:

    virtual void Updating       (void) ;
    virtual void currentTime    (int value) ;
    virtual void setVolume      (int value) ;

    virtual void itemClicked    (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void         Finished       (void) ;
    void         volumeChanged  (int value) ;

} ;

class Q_AV_EXPORT VideoEditorCLI : public InteractiveEditor
{
  Q_OBJECT
  public:

    VcfInterface * vcf    ;
    ScrollDock   * scroll ;

    explicit      VideoEditorCLI     (StandardConstructor) ;
    virtual      ~VideoEditorCLI     (void) ;

    virtual QSize sizeHint           (void) const ;
    virtual bool  isCommanding       (void) ;

  protected:

    QStringList HistoryCommands ;
    QStringList CurrentCommands ;
    QString     CurrentCommand  ;

    virtual void Configure           (void) ;
    virtual bool Interpret           (QString command) ;
    virtual void run                 (int type,ThreadData * data) ;

  private:

  public slots:

    virtual void Enter               (void) ;
    virtual void Load                (QString filename) ;
    virtual void RunCommandsFromFile (QString filename) ;
    virtual void SaveTo              (QString filename) ;
    virtual void SaveCommandsToFile  (QString filename) ;
    virtual void AcceptMessage       (QString message) ;

  protected slots:

  private slots:

    void SendCommand                 (void) ;

  signals:

    void SafeCommand                 (void) ;
    void Commanding                  (QString     command) ;
    void Commanding                  (QStringList commands) ;

} ;

class Q_AV_EXPORT AddNewAlbum : public Widget
{
  Q_OBJECT
  public:

    VcfInterface * vcf    ;
    UUIDs          videos ;

    explicit        AddNewAlbum (StandardConstructor) ;
    virtual        ~AddNewAlbum (void) ;

    virtual QString AlbumName   (void) ;
    virtual int     Language    (void) ;

  protected:

    Ui::AddNewAlbum * ui ;

    virtual void    changeEvent (QEvent * e) ;

    virtual void    Configure   (void) ;

  private:

  public slots:

    virtual void    GainFocus   (void) ;
    virtual void    AddAlbum    (void) ;
    virtual void    CancalAdd   (void) ;
    virtual void    EditDetails (void) ;
    virtual void    NameChanged (QString text) ;

  protected slots:

  private slots:

  signals:

    void Add                    (AddNewAlbum * widget) ;
    void Cancel                 (AddNewAlbum * widget) ;
    void Details                (AddNewAlbum * widget) ;

} ;

class Q_AV_EXPORT VideoRatio : public Widget
{
  Q_OBJECT
  public:

    int            ThreadId ;
    VcfInterface * vcf      ;

    explicit     VideoRatio  (StandardConstructor) ;
    virtual     ~VideoRatio  (void);

  protected:

    Ui::nVideoRatio * ui      ;
    QSize             video   ;
    QSize             window  ;
    QRect             current ;

    virtual void changeEvent (QEvent * e) ;
    virtual void Configure   (void) ;

  private:

  public slots:

    virtual void setRange    (QSize video,QSize window,QRect rect) ;
    virtual void FreeRatio   (void) ;
    virtual void Ratio       (int value) ;
    virtual void Changed     (int value) ;
    virtual void Width       (int value) ;
    virtual void Height      (int value) ;
    virtual void Close       (void) ;

  protected slots:

    virtual void Send        (void) ;

  private slots:

  signals:

    void Changed             (int Id,QRect rect) ;
    void Close               (int Id,VideoRatio * vr) ;

} ;

class Q_AV_EXPORT UnifiedResources
{
  public:

    explicit UnifiedResources (void) ;
    virtual ~UnifiedResources (void) ;

    virtual SUID UniverseAdd    (QString key,Universe * universe) = 0 ;
    virtual SUID UniverseAdd    (Universe * universe) = 0 ;
    virtual bool UniverseMove   (SUID from,SUID to  ) = 0 ;
    virtual bool UniverseDelete (Universe * universe) = 0 ;

    virtual SUID CameraAdd      (Universe * universe) = 0 ;
    virtual bool CameraMove     (SUID from,SUID to  ) = 0 ;
    virtual bool CameraDelete   (Universe * universe) = 0 ;

  protected:

    ZMAPs ROIDs ;

  private:

} ;

class Q_AV_EXPORT EditableUniverse : public UniverseWidget
                                   , public Enabler
{
  Q_OBJECT
  public:

    UnifiedResources       *  unified ;
    QList<QWidget          *> widgets ;
    QList<VcfInterface     *> VCFs    ;
    SMAPs                     nMaps   ;
    QMap<int,QScriptEngine *> Engines ;

    explicit     EditableUniverse      (QObject * parent,Plan * plan) ;
    explicit     EditableUniverse      (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~EditableUniverse      (void) ;

    virtual void clear                 (void) ;
    virtual void Flush                 (void) ;
    virtual bool Animation             (void) ;
    virtual SUID Lastest               (void) ;

    Camera *     FlatCamera            (void) ;
    Camera *     CreateCamera          (int style) ;

    virtual void setAlign              (int align) ;
    virtual void setArea               (QRect Port,QRectF Region) ;

    virtual void Relocation            (QRectF viewport) ;

    virtual bool mouseDoubleClickEvent (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseMoveEvent        (QWidget * widget,QMouseEvent * event) ;
    virtual bool mousePressEvent       (QWidget * widget,QMouseEvent * event) ;
    virtual bool mouseReleaseEvent     (QWidget * widget,QMouseEvent * event) ;

    virtual bool Menu                  (QWidget * widget,QPointF pos) ;

    virtual void RemovePerformers      (QString TypeName) ;

  protected:

    Plan         * plan  ;
    GlyphManager * glyph ;

    static  int  RequestingFor         (void * data,void * dispatch,QVariants & values) ;

    virtual void Configure             (void) ;
    virtual bool Moving                (ControlPoint & from,ControlPoint & to) ;

    virtual void run                   (int type,ThreadData * data) ;
    virtual bool ItemRunner            (int type,ThreadData * data) ;

  private:

  public slots:

    virtual void Clean                 (void) ;

    virtual void addWidget             (QWidget * widget) ;
    virtual void takeWidget            (QWidget * widget) ;

    virtual void addVcf                (VcfInterface * vcf) ;
    virtual void takeVcf               (VcfInterface * vcf) ;

    virtual void addAxis               (double length,ControlPoint at,bool edit = false) ;
    virtual void addAxis               (bool edit = false) ;

    virtual bool LoadPicture           (SqlConnection & SC,SUID uuid) ;
    virtual bool LoadThumb             (SqlConnection & SC,SUID uuid) ;

    virtual void addBookcase           (SUID uuid) ;

    virtual void AreaChanged           (QRectF area) ;

    virtual SUID CreateCalendar        (QRectF  & view      ,
                                        QDateTime start     ,
                                        QDateTime final     ,
                                        QDateTime separator ,
                                        qint64    gap     ) ;
    virtual SUID CreateCalendar        (QRectF  & view) ;

    virtual void CloseChild            (SUID u) ;

    virtual void BuildCameras          (int startId) ;
    virtual void SelectCamera          (QString identifier,QRect geometry) ;
    virtual void ChooseCamera          (void) ;
    virtual int  MenuBinder            (MenuManager & mm           ,
                                        QAction     * action       ,
                                        QVariants   & v            ,
                                        void        * dispatcher ) ;
    virtual int  RequestsFor           (QVariants & args) ;
    virtual int  CallBinding           (SceneObject * s,QVariants & args) ;
    virtual void BindRequest           (SceneObject * s) ;

    virtual void addEar                (void) ;

  protected slots:

    virtual void ActualCloseChild      (VarArgs & args) ;

    virtual void doubleClicked         (QWidget * widget,QPointF pos,UUIDs & U) ;

    virtual bool ObjectsMenu           (MenuManager & menu       ,
                                        QMenu       * objects  ) ;
    virtual bool ControlMenu           (MenuManager & menu       ,
                                        QMenu       * objects  ) ;
    virtual bool InterfaceMenu         (MenuManager & menu       ,
                                        QMenu       * objects  ) ;

    virtual void SeeGalleries          (VarArgs & args) ;
    virtual void SeeAlbums             (VarArgs & args) ;
    virtual void SeeCrowds             (VarArgs & args) ;
    virtual void LaunchPictures        (VarArgs & args) ;
    virtual void EditPicture           (VarArgs & args) ;
    virtual void LaunchRoles           (VarArgs & args) ;
    virtual void LaunchAlbums          (VarArgs & args) ;
    virtual void LaunchClips           (VarArgs & args) ;
    virtual void LaunchSubgroup        (VarArgs & args) ;
    virtual void CameraSelected        (VarArgs & args) ;
    virtual void PlayClip              (VarArgs & args) ;

  private slots:

    void addText (void) ;

  signals:

    void Close                         (Universe * universe) ;
    void EditPoint                     (QString title,EditableUniverse * universe,ControlPoint * point) ;
    void RenderSequences               (QString title,SceneObject * obj,SMAPs * maps) ;
    void Albums                        (SUID uuid) ;
    void People                        (EditableUniverse * universe,SUID uuid) ;
    void Picture                       (EditableUniverse * universe,SUID uuid) ;
    void EditCamera                    (EditableUniverse * universe,Camera * camera) ;

} ;

class Q_AV_EXPORT GroupUniverse : public EditableUniverse
{
  Q_OBJECT
  public:

    explicit GroupUniverse (QObject * parent,Plan * plan) ;
    explicit GroupUniverse (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual ~GroupUniverse (void) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_AV_EXPORT VcfVideo : public VcfCanvas
{
  Q_OBJECT
  public:

    enum VcfVideoMode {
      Label   =  3    ,
      Attache =  4    ,
      Capture =  5  } ;

    explicit     VcfVideo         (QObject       * parent       ,
                                   QGraphicsItem * item         ,
                                   Plan          * plan = NULL) ;
    virtual     ~VcfVideo         (void) ;

  protected:

    virtual void contextMenuEvent (QGraphicsSceneContextMenuEvent * e) ;

  private:

  public slots:

    virtual void Paint            (QPainter * painter,QRectF Region,bool clip,bool color) ;

  protected slots:

  private slots:

  signals:

    void         Menu             (VcfVideo * video,QPointF pos) ;

} ;

class Q_AV_EXPORT VideoStreaming : public VideoReader
{
  public:

    int                            ThreadId     ;
    int                            step         ;
    int                            LastStep     ;
    int                            CacheMs      ;
    int                            Roll         ;
    SUID                           video        ;
    SUID                           album        ;
    SUID                           description  ;
    TUID                           position     ;
    LMAPs                          Booleans     ;
    CiosAudio::Core             *  core         ;
    CiosAudio::Conduit          *  conduit      ;
    CiosAudio::Stream           *  stream       ;
    VideoTransmission           *  transmission ;
    VideoUniverse               *  universe     ;
    StreamInterface             *  controller   ;
    VideoRatio                  *  ratio        ;
    VcfVideo                    *  canvas       ;
    VideoStatus                 *  status       ;
    void                        *  extras       ;
    QList < WaveUniverse *       > waves        ;
    QMap  < WaveUniverse * , int > waveChannels ;
    QMap  < int , QByteArray     > ChannelBlobs ;
    QMap  < int , AVPacket       > AvPackets    ;
    Mutexz                         mutex        ;
    int                            vds          ;

    explicit          VideoStreaming   (void) ;
    virtual          ~VideoStreaming   (void) ;

    virtual  void     setTransmission  (void) ;
    virtual  bool     hasVideo         (void) ;
    virtual  bool     hasAudio         (void) ;
    virtual  bool     hasStream        (void) ;
    virtual  bool     Having           (void) ;
    virtual  bool     ObtainAudio      (void * buffer,int total) ;
    virtual  bool     isFull           (void) ;
    virtual  bool     CacheFull        (void) ;
    virtual  bool     isVideoFull      (void) ;
    virtual  void     RollVideo        (void) ;
    virtual  TUID     AudioPending     (void) ;
    virtual  TUID     AudioFlushed     (void) ;
    virtual  TUID     msecs            (void) ;
    virtual  TUID     current          (void) ;
    virtual  void     setCache         (int ms) ;
    virtual  void     Precache         (void) ;

    virtual  void     Locate           (void) ;
    virtual  void     Seeking          (TUID pts) ;
    virtual  void     Halt             (void) ;
    virtual  void     Resume           (void) ;
    virtual  void     Stop             (void) ;
    virtual  void     Previous         (void) ;
    virtual  void     Next             (void) ;
    virtual  void     Forward          (void) ;
    virtual  void     Backward         (void) ;

    virtual  bool     hasPrevious      (void) ;
    virtual  bool     hasNext          (void) ;
    virtual  bool     hasForward       (void) ;
    virtual  bool     hasBackward      (void) ;

    virtual  bool     hasWave          (void) ;
    virtual  void     FeedWave         (void) ;
    virtual  void     addWave          (int channel,WaveUniverse * wave) ;
    virtual  void     removeWave       (WaveUniverse * wave) ;

    virtual  void     Preparation      (void) ;

  protected:

    virtual QImage  * ObtainImage      (void) ;
    virtual QDateTime GuessTime        (void) ;
    virtual QDateTime AudioTime        (void) ;
    virtual void      AudioAppended    (int newSize) ;
    virtual void      Parking          (TUID pts) ;
    virtual void      FetchImage       (void) ;
    virtual void      ProcessImage     (TUID pts,QImage * image) ;

  private:

} ;

class Q_AV_EXPORT AudioStreaming : public CiosAudio::Conduit
{
  public:

    VideoStreaming * stream ;

    explicit AudioStreaming (void) ;
    virtual ~AudioStreaming (void) ;

    virtual int  obtain     (void) ;
    virtual int  put        (void) ;
    virtual void finish     (ConduitDirection direction = NoDirection ,
                             FinishCondition  condition = Correct   ) ;

  protected:

  private:

} ;

class Q_AV_EXPORT StreamInterface
{
  public:

    VideoStreaming * stream ;
    VcfInterface   * vcf    ;

    explicit     StreamInterface (void) ;
    virtual     ~StreamInterface (void) ;

    virtual void Restart         (void) = 0 ;
    virtual bool isStopped       (void) = 0 ;
    virtual void Finalize        (void) = 0 ;
    virtual void LoadStyleSheet  (void) = 0 ;
    virtual void SaveStyleSheet  (void) = 0 ;

  protected:

  private:

} ;

class Q_AV_EXPORT StreamController : public Widget
                                   , public StreamInterface
{
  Q_OBJECT
  public:

    explicit StreamController   (StandardConstructor) ;
    virtual ~StreamController   (void) ;

    virtual void Restart        (void) ;
    virtual bool isStopped      (void) ;
    virtual void Finalize       (void) ;
    virtual void LoadStyleSheet (void) ;
    virtual void SaveStyleSheet (void) ;

  protected:

    Ui::nStreamController * ui      ;
    bool                    moving  ;
    int                     TimeId  ;
    int                     TimeID  ;
    bool                    closeIt ;

    virtual void changeEvent    (QEvent       * e) ;
    virtual void resizeEvent    (QResizeEvent * e) ;
    virtual void timerEvent     (QTimerEvent  * e) ;

    virtual void KeepPath       (QString path) ;
    virtual QString GetPath     (void) ;

  private:

    QString      TimeLength     (TUID t) ;

  public slots:

    virtual void Sync           (void) ;
    virtual void Refresh        (void) ;
    virtual void Play           (bool enabled) ;
    virtual void Stop           (void) ;
    virtual void Previous       (void) ;
    virtual void Next           (void) ;
    virtual void Forward        (void) ;
    virtual void Backward       (void) ;

  protected slots:

    virtual void leftChanged    (int value) ;
    virtual void rightChanged   (int value) ;
    virtual void opacityChanged (int value) ;
    virtual void videoChanged   (int value) ;
    virtual void videoReleased  (void) ;

    virtual void SyncButtons    (void) ;
    virtual void leftMsg        (void) ;
    virtual void rightMsg       (void) ;
    virtual void opacityMsg     (void) ;

    virtual void startMenu      (void) ;

  private slots:

  signals:

    void         PanelMenu      (int ThreadId,QPoint pos) ;

} ;

class Q_AV_EXPORT VideoStatus : public Widget
{
  Q_OBJECT
  public:

    VideoStreaming * stream ;
    VcfInterface   * vcf    ;

    explicit     VideoStatus (StandardConstructor) ;
    virtual     ~VideoStatus (void) ;

    virtual bool isStopped   (void) ;
    virtual void Finalize    (void) ;

  protected:

    Ui::nVideoStatus * ui      ;
    int                TimeId  ;
    int                TimeID  ;
    bool               closeIt ;

    virtual void changeEvent (QEvent      * e) ;
    virtual void timerEvent  (QTimerEvent * e) ;

  private:

  public slots:

    virtual void Start       (void) ;
    virtual void Refresh     (void) ;
    virtual void Close       (void) ;

  protected slots:

  private slots:

  signals:

    void Close               (int ThreadId,VideoStatus * vs) ;

} ;

class Q_AV_EXPORT VideosDispatcher : public QObject
                                   , public SyntaxDispatcher
                                   , public UuidSyntax
{
  Q_OBJECT
  public:

    explicit     VideosDispatcher  (QObject * parent) ;
    virtual     ~VideosDispatcher  (void) ;

    virtual int  type              (void) const ; // CiosMenuId ( 79 , 0 , 1 )
    virtual int  matching          (QString command) ;
    virtual bool execute           (int id) ;
    virtual void setExtras         (void * data) ;
    virtual void Help              (QStringList & help) ;

  protected:

    Plan              * plan            ;
    WMAPs               Variables       ;
    UUIDs               Matchings       ;
    UUIDs               Companies       ;
    SMAPs               ToCompanies     ;
    NAMEs               MatchingScripts ;
    NAMEs               CompanyNames    ;
    CUIDs               Clips           ;
    QStringList         ClipsPaddings   ;
    QMap<QString,UUIDs> ClipsQueues     ;
    QStringList         ClipPaddings    ;
    QMap<QString,UUIDs> ClipQueues      ;
    QStringList         VideoPaddings   ;
    QMap<QString,UUIDs> VideoQueues     ;
    QStringList         EditPaddings    ;
    QMap<QString,UUIDs> EditQueues      ;

    virtual bool FetchByTag        (SqlConnection & SC,UUIDs & U,QString Key) ;
    virtual bool FilterType        (SqlConnection & SC,int type,UUIDs & U,UUIDs & P) ;
    virtual bool NamesOwners       (SqlConnection & SC,UUIDs & U,UUIDs & P) ;
    virtual bool FetchNames        (SqlConnection & SC,UUIDs & U,QString K) ;
    virtual bool ExactId           (SqlConnection & SC,UUIDs & U,QString K) ;
    virtual bool LikeId            (SqlConnection & SC,UUIDs & U,QString K) ;
    virtual bool RegexpId          (SqlConnection & SC,UUIDs & U,QString K) ;
    virtual bool MatchId           (SqlConnection & SC,UUIDs & U,QString K) ;
    virtual bool ReportNames       (SqlConnection & SC,UUIDs & U) ;
    virtual bool FetchAlbums       (SqlConnection & SC,UUIDs & U,UUIDs & P) ;
    virtual SUID MapToCompany      (QString ID) ;

  private:

  public slots:

    virtual void setVariable       (void) ;
    virtual void showVariables     (void) ;
    virtual void ListClips         (void) ;
    virtual void SearchClips       (void) ;
    virtual void SearchClip        (void) ;
    virtual void SearchId          (void) ;
    virtual void SearchIdentifiers (void) ;
    virtual void SearchAlbums      (void) ;
    virtual void SearchUUIDs       (void) ;
    virtual void MatchIdenticals   (void) ;
    virtual void CatalogAlbums     (void) ;
    virtual void CatalogProducts   (void) ;
    virtual void CatalogOrders     (void) ;
    virtual void CatalogCommodity  (void) ;
    virtual void EditId            (void) ;
    virtual void SearchFiles       (void) ;
    virtual void ClipsId           (void) ;
    virtual void AlbumFiles        (void) ;
    virtual void AlbumPictures     (void) ;
    virtual void SearchTags        (void) ;

  protected slots:

  private slots:

    void         SendListClips     (void) ;
    void         SendClipsGroup    (void) ;
    void         SendClips         (void) ;
    void         SendVideos        (void) ;
    void         SendEditings      (void) ;

  signals:

    void EmitListClips             (void) ;
    void EmitClipsGroup            (void) ;
    void EmitClips                 (void) ;
    void EmitVideos                (void) ;
    void EmitEditings              (void) ;
    void ListClips                 (int total) ;
    void ListClips                 (QString name,UUIDs & Uuids) ;
    void ListClip                  (QString name,UUIDs & Uuids) ;
    void ListVideo                 (QString name,UUIDs & Uuids) ;
    void AlbumDetails              (SUID album,QString name) ;

} ;

class Q_AV_EXPORT VideoMerger : public TreeDock
                              , public GroupItems
{
  Q_OBJECT
  public:

    explicit     VideoMerger (StandardConstructor) ;
    virtual     ~VideoMerger (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    bool Deletion ;
    bool Execute  ;

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;

    virtual bool acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropAlbums      (nDeclDrops,const UUIDs & Uuids) ;

    virtual bool NameMerger      (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool NamesMerger     (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool GroupsRemove    (SqlConnection & SC,QList<Group> & groups) ;
    virtual bool GroupsJoin      (SqlConnection & SC,QList<Group> & groups) ;
    virtual bool GroupsMerger    (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool OwnersMerger    (SqlConnection & SC,SUID master,SUID deletion) ;

    virtual void run             (int Type,ThreadData * data) ;

  private:

    virtual bool MergeTriples    (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeFlags      (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeVariables  (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeIdentifier (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool DeleteMerged    (SqlConnection & SC,SUID deletion) ;

  public slots:

    virtual void Delete          (void) ;
    virtual void Merge           (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT ClipsShelf : public ListShelf
{
  Q_OBJECT
  public:

    explicit     ClipsShelf        (QObject * parent,Plan * plan) ;
    explicit     ClipsShelf        (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~ClipsShelf        (void) ;

    virtual bool Menu              (QWidget      * widget ,
                                    QPointF        pos    ,
                                    ControlPoint & From   ,
                                    ControlPoint & To   ) ;

    virtual bool Relocation        (QRectF viewport) ;

  protected:

    virtual void Configure         (void) ;

  private:

  public slots:

    virtual int  Indexing          (RangeBar * rbu,int direction,QVariants & v) ;

  protected slots:

    virtual void atDoubleClicked   (SelectItem * item) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT AlbumShelf : public ViewShelf
{
  Q_OBJECT
  public:

    explicit     AlbumShelf        (QObject * parent,Plan * plan) ;
    explicit     AlbumShelf        (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~AlbumShelf        (void) ;

    virtual bool Menu              (QWidget      * widget ,
                                    QPointF        pos    ,
                                    ControlPoint & From   ,
                                    ControlPoint & To   ) ;

    virtual bool Relocation        (QRectF viewport) ;

  protected:

    virtual void Configure         (void) ;

    virtual bool FocusIn           (void) ;
    virtual bool ItemRunner        (int type,ThreadData * data) ;

    virtual bool hasItem           (ControlPoint & From ,
                                    ControlPoint & To   ,
                                    ControlPoint & At ) ;
    virtual QMimeData * dragMime   (ControlPoint & From ,
                                    ControlPoint & To   ,
                                    ControlPoint & At ) ;
    virtual bool dropAlbums        (QWidget     * widget  ,
                                    ControlPoint  pos     ,
                                    const UUIDs & Uuids ) ;
    virtual bool dropPictures      (QWidget     * widget  ,
                                    ControlPoint  pos     ,
                                    const UUIDs & Uuids ) ;
    virtual bool dropDivisions     (QWidget     * widget  ,
                                    ControlPoint  pos     ,
                                    const UUIDs & Uuids ) ;

    virtual void AppendAlbum       (VarArgs & args) ;
    virtual void MoveAlbums        (VarArgs & args) ;
    virtual void AlbumConstraints  (VarArgs & args) ;
    virtual void AlbumRules        (VarArgs & args) ;

    virtual void AllAlbums         (void) ;

  private:

  public slots:

    virtual void SelectNothing     (void) ;
    virtual void SelectAll         (void) ;
    virtual void NewAlbum          (void) ;

    virtual int  Indexing          (RangeBar * rbu,int direction,QVariants & v) ;

  protected slots:

    virtual void atDoubleClicked   (SelectItem * item) ;

  private slots:

  signals:

    void         ShowAll           (void) ;

} ;

class Q_AV_EXPORT VideoShelf : public ViewShelf
{
  Q_OBJECT
  public:

    explicit     VideoShelf           (QObject * parent,Plan * plan) ;
    explicit     VideoShelf           (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~VideoShelf           (void) ;

    virtual bool Menu                 (QWidget      * widget ,
                                       QPointF        pos    ,
                                       ControlPoint & From   ,
                                       ControlPoint & To   ) ;

    virtual bool Relocation           (QRectF viewport) ;

  protected:

    virtual void Configure            (void) ;

    virtual bool FocusIn              (void) ;
    virtual bool ItemRunner           (int type,ThreadData * data) ;

  private:

  public slots:

    virtual void PlayAlbum            (SelectItem * item) ;
    virtual void AppendAlbum          (void) ;
    virtual void NewDetailedAlbum     (void) ;
    virtual void SelectNothing        (void) ;
    virtual void SelectAll            (void) ;

    virtual int  Indexing             (RangeBar * rbu,int direction,QVariants & v) ;

  protected slots:

    virtual void AlbumMenu            (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void InformationMenu      (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void NetworkMenu          (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual bool AlbumProcessor       (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool InformationProcessor (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool NetworkProcessor     (MenuManager & mm,QAction * aa,SelectItem * item) ;

  private slots:

  signals:

    void         ShowAll              (void) ;
    void         PlayAlbum            (SUID uuid) ;
    void         NewAlbum             (SUID uuid) ;
    void         DetailAlbum          (SUID uuid) ;
    void         AlbumGallery         (SelectItem * item) ;
    void         AlbumActors          (SelectItem * item) ;
    void         AlbumClips           (SelectItem * item) ;
    void         AlbumTorrents        (SelectItem * item) ;
    void         AlbumRelated         (SelectItem * item) ;

} ;

class Q_AV_EXPORT EarIn : public SceneWidget
                        , public CiosAudio::Conduit
{
  Q_OBJECT
  public:

    explicit      EarIn          (QObject * parent,Plan * plan) ;
    explicit      EarIn          (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual      ~EarIn          (void) ;

    virtual int   obtain         (void) ;
    virtual int   put            (void) ;
    virtual void  finish         (ConduitDirection direction = NoDirection ,
                                  FinishCondition  condition = Correct   ) ;

    virtual void  render         (Camera & camera,QMap<int,Matrix> & matrices) ;
    virtual void  renderSegments (void) ;
    virtual void  renderBox      (void) ;

    virtual bool  FocusIn        (void) ;
    virtual bool  FocusOut       (void) ;

    virtual bool  MouseClicked   (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  BMAPs        & buttons ) ;
    virtual bool  MouseVector    (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  BMAPs        & buttons ) ;
    virtual bool  MouseHover     (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  BMAPs        & buttons ) ;
    virtual bool  MouseMove      (ControlPoint & From1     ,
                                  ControlPoint & To1       ,
                                  ControlPoint & From2     ,
                                  ControlPoint & To2       ,
                                  BMAPs        & buttons ) ;
    virtual bool  Spot           (ControlPoint & From ,
                                  ControlPoint & To   ,
                                  ControlPoint & At ) ;
    virtual int   AtRegion       (ControlPoint & At) ;

    virtual bool  Menu           (QWidget      * widget ,
                                  QPointF        pos    ,
                                  ControlPoint & From   ,
                                  ControlPoint & To   ) ;
  protected:

    Camera         * UsingCamera   ;
    QMap<int,Matrix> UsingMatrix   ;
    QByteArray       InputBuffer   ;
    QByteArray       DataBuffer    ;
    QByteArray       RenderBuffer  ;

    virtual void  Configure      (void) ;

    virtual void  run            (int type,ThreadData * data) ;

    virtual bool  LocalClicked   (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  ControlPoint & At        ,
                                  BMAPs        & buttons   ,
                                  bool           after     ,
                                  bool           overlap ) ;
    virtual bool  LocalVector    (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  ControlPoint & At        ,
                                  BMAPs        & buttons   ,
                                  bool           after     ,
                                  bool           overlap ) ;
    virtual bool  LocalHover     (ControlPoint & From      ,
                                  ControlPoint & To        ,
                                  ControlPoint & At        ,
                                  BMAPs        & buttons   ,
                                  bool           after     ,
                                  bool           overlap ) ;

    virtual void  Recording      (VarArgs & args) ;
    virtual void  Saving         (VarArgs & args) ;

    virtual void  Processing     (QByteArray & extras) ;
    virtual bool  Encode         (AvEncoder & encoder ,
                                  QIODevice & IO      ,
                                  bool        flush ) ;

  private:

    bool          KeepRecording  (void) ;

  public slots:

    virtual bool  prepare        (void) ;
    virtual bool  startup        (void) ;
    virtual void  SaveAudio      (void) ;
    virtual void  StartRecord    (void) ;
    virtual void  StopRecord     (void) ;

    virtual void  SelectColor    (QString key) ;

  protected slots:

    virtual void  DisplayMenu        (MenuManager & menu,QMenu * display) ;
    virtual QActionGroup * RateMenu  (MenuManager & menu,QMenu * rate) ;
    virtual void PrepareAudioInput   (QActionGroup * group) ;

    virtual void  InputDevice        (QAction * action) ;
    virtual void  GetSampleRate      (QAction * action) ;

    virtual bool  DisplayProcessor   (MenuManager & menu,QAction * action) ;

    virtual QActionGroup * CodecMenu (MenuManager & menu,QMenu * encoder) ;
    virtual void  addCodec           (MenuManager  & menu    ,
                                      QMenu        * encoder ,
                                      QActionGroup * group   ,
                                      int            codec ) ;
    virtual void  GetCodec           (QAction * action) ;

  private slots:

  signals:

} ;

class Q_AV_EXPORT VideoEditor : public WorldView
                              , public UnifiedResources
{
  Q_OBJECT
  public:

    explicit VideoEditor                  (StandardConstructor,QString IPC = "VideoEditor") ;
    virtual ~VideoEditor                  (void) ;

    // inherit UnifiedResources
    virtual SUID   UniverseAdd            (QString key,Universe * universe) ;
    virtual SUID   UniverseAdd            (Universe * universe) ;
    virtual bool   UniverseMove           (SUID from,SUID to  ) ;
    virtual bool   UniverseDelete         (Universe * universe) ;

    virtual SUID   CameraAdd              (Universe * universe) ;
    virtual bool   CameraMove             (SUID from,SUID to  ) ;
    virtual bool   CameraDelete           (Universe * universe) ;

    virtual int    VideosCount            (void) ;

  protected:

    QTimer                              *  VideoTimer         ;
    Rpc                                    RPC                ;
    VideoEditorCLI                      *  cli                ;
    GlyphManager                        *  glyph              ;
    UUIDs                                  VideoQueues        ;
    NAMEs                                  VideoPathes        ;
    NAMEs                                  VideoNames         ;
    UUIDs                                  AlbumQueues        ;
    NAMEs                                  AlbumNames         ;
    SMAPs                                  VideoAlbumMaps     ;
    QMap<VideoUniverse *,VideoChannel   *> UniverseToChannels ;
    QMap<VideoUniverse *,Junction       *> UniverseToJunction ;
    QMap<WaveUniverse  *,AudioChannel   *> WaveToChannels     ;
    QMap<Universe      *,Junction       *> WaveToJunction     ;
    QMap<WaveUniverse  *,VcfVideo       *> WaveCanvas         ;
    QMap<VcfVideo      *,WaveUniverse   *> CanvasWave         ;
    QMap<int            ,VideoStreaming *> Streamings         ;
    QMap<VideoUniverse *,VideoStreaming *> StreamVideos       ;
    QMap<VideoUniverse *,VcfVideo       *> UniverseCanvas     ;
    QMap<VcfVideo      *,VideoUniverse  *> CanvasUniverse     ;

    static int   CalendarConnector        (void * data,void * dispatcher,QVariants & values) ;
    static int   PeriodConnector          (void * data,void * dispatcher,QVariants & values) ;
    static int   EventConnector           (void * data,void * dispatcher,QVariants & values) ;
    static int   PeriodActions            (void * data,void * dispatcher,QVariants & values) ;
    static int   AttacheMenu              (void * data,void * dispatcher,QVariants & values) ;
    static int   MenuBindings             (void * data,void * dispatcher,QVariants & values) ;
    static int   BindFunctions            (void * data,void * dispatcher,QVariants & values) ;

    virtual void Configure                (void) ;
    virtual void AutoDeletion             (void) ;

    virtual bool FocusIn                  (void) ;

    virtual bool dropObjects              (int id,QPointF pos,const UUIDs & uuids) ;

    virtual bool ItemRunner               (int type,ThreadData * data) ;

    virtual void DispatchCommands         (void) ;

    QMap<SUID,QImage *> FetchImages       (SqlConnection & Connection,UUIDs & Uuids) ;
    QMap<SUID,QImage *> FetchImages       (UUIDs & Uuids) ;
    bool         RemoveImage              (SUID uuid) ;

    virtual void PrepareAudioInput        (QActionGroup * group) ;
    virtual void PrepareAudioOutput       (QActionGroup * group) ;
    virtual void PrepareCameraMenu        (QActionGroup * group) ;

    virtual bool AudioInputMenu           (QActionGroup * group,QAction * aa) ;
    virtual bool AudioOutputMenu          (QActionGroup * group,QAction * aa) ;
    virtual bool CameraInputMenu          (QActionGroup * group,QAction * aa) ;

    virtual Junction      * MountChannel  (int MID) ;
    virtual Junction      * MountVideo    (int MID) ;

    virtual AudioAcceptor * SpeakerOutlet (AudioFeeder * feeder) ;
    virtual AudioAcceptor * SpeakerOutlet (VideoFeeder * feeder) ;

    virtual AudioAcceptor * WaveDisplay   (Junction * junc,AudioChannel * feeder) ;
    virtual VideoAcceptor * VideoDisplay  (Junction * junc,VideoChannel * feeder) ;

    virtual Universe      * CreateWorld   (QString name) ;
    virtual WaveUniverse  * AllocateWave  (void) ;
    virtual VideoUniverse * AllocateVideo (void) ;

    virtual void AudioFeederClosed        (VarArgs args) ;
    virtual void ChannelClosed            (AudioFeeder * feeder) ;
    virtual void AudioFailure             (SUID uuid,QString filename) ;
    virtual void UnmountAudioData         (AudioData * audio) ;

    virtual void VideoFeederClosed        (VarArgs args) ;
    virtual void VideoClosed              (VideoFeeder * feeder) ;
    virtual void VideoFailure             (SUID uuid,QString filename) ;
    virtual void UnmountSyncData          (SyncAcceptor * sync) ;

    virtual int  RequestChannel           (QString filename) ;
    virtual int  RequestVideo             (QString filename) ;
    virtual int  RequestInput             (QString device    ,
                                           int     rate      ,
                                           int     bits      ,
                                           int     channels  ,
                                           int     format  ) ;

    virtual void StayAudio                (int Id,VarArgs args) ;
    virtual void RollVideo                (int Id,VarArgs args) ;
    virtual void RollAudio                (int Id,VarArgs args) ;
    virtual void AccessVideo              (int Id,VarArgs args) ;
    virtual void AccessAlbum              (int Id,VarArgs args) ;
    virtual void DescribeAlbum            (int Id,VarArgs args) ;
    virtual void DescribeVideo            (int Id,VarArgs args) ;
    virtual void VideoEvent               (int Id,VarArgs args) ;
    virtual void PlayEvent                (int Id,VarArgs args) ;
    virtual void DeleteVideo              (int Id) ;

    virtual QString ObtainVideoFile       (SUID uuid) ;
    virtual void    KeepPath              (QString path) ;
    virtual QString GetPath               (void) ;

    virtual VideoStreaming * NewVideoStreaming (void) ;
    virtual AudioStreaming * NewAudioStreaming (void) ;

    virtual void NewAlbum                 (int Id,VarArgs args) ;

    virtual void Download                 (int Id,VarArgs args) ;
    virtual void Copy                     (int Id,VarArgs args) ;
    virtual void Compare                  (int Id,VarArgs args) ;
    virtual void Verify                   (int Id,VarArgs args) ;

    virtual GroupUniverse * RequestWorld  (void) ;

  private:

    void    EnableWaveThrottle            (AudioAcceptor * waveu) ;

  public slots:

    virtual bool startup                  (void) ;
    virtual bool startup                  (int type,SUID uuid) ;

    virtual void Estimate                 (int step) ;

    virtual void PicturesDropIn           (QPointF pos,UUIDs & uuids) ;
    virtual void PeopleDropIn             (QPointF pos,UUIDs & uuids) ;
    virtual void AlbumsDropIn             (QPointF pos,UUIDs & uuids) ;
    virtual void VideosDropIn             (QPointF pos,UUIDs & uuids) ;
    virtual void AudiosDropIn             (QPointF pos,UUIDs & uuids) ;
    virtual void CamerasDropIn            (QPointF pos,UUIDs & uuids) ;

    virtual void Copy                     (void) ;
    virtual void Delete                   (void) ;
    virtual void Refresh                  (void) ;

    virtual void RunCommand               (VarArgs & arguments) ;

    virtual void ShutAudio                (VarArgs & arguments) ;
    virtual void ShutVideo                (VarArgs & arguments) ;
    virtual void AddRecorder              (VarArgs & arguments) ;
    virtual void playVideo                (VarArgs & arguments) ;

    virtual void Play                     (QString filename) ;
    virtual void PlayVideo                (SUID uuid) ;
    virtual void PlayVideo                (void) ;

    virtual void ListAlbums               (void) ;
    virtual void OpenVideo                (void) ;
    virtual void PendingVideo             (QString filename) ;
    virtual void SavePTS                  (VideoStreaming * vs) ;
    virtual void WritePTS                 (QString video,QString output) ;
    virtual void ExportPTS                (VarArgs & args) ;

    virtual void AttacheCLI               (void) ;
    virtual bool PendingCommands          (QStringList commands) ;
    virtual void Speak                    (QString message) ;

    virtual void ListUniverses            (void) ;
    virtual void AddBoard                 (void) ;
    virtual void PlayList                 (void) ;
    virtual void NewAlbum                 (void) ;
    virtual void NewAlbum                 (UUIDs videos) ;
    virtual void Add                      (AddNewAlbum * widget) ;
    virtual void Cancel                   (AddNewAlbum * widget) ;
    virtual void Details                  (AddNewAlbum * widget) ;
    virtual void ListCameras              (void) ;

    virtual bool Copy                     (QString from,QString to,int size = 64 * 1024) ;
    virtual bool Compare                  (QString from,QString to,int size = 64 * 1024) ;
    virtual bool Verify                   (QString filename,bool strict = false) ;

    virtual void EditRatio                (VideoStreaming * vs) ;
    virtual void UpdateRatio              (VideoUniverse  * vu) ;
    virtual void DisplayRatio             (VideoUniverse  * vu) ;
    virtual void DoubleClicked            (VideoUniverse  * vu) ;

    virtual void ShowStatus               (VideoStreaming * vs) ;
    virtual void StatusClose              (int ThreadId,VideoStatus * vs) ;

    virtual void SchedulePrevious         (void) ;
    virtual void ScheduleNext             (void) ;
    virtual void Schedule                 (VideoStreaming * vs) ;

    virtual void ImportAlbums             (SUID uuid) ;

    virtual void StartGroup               (int id) ;

    virtual void ShowAlbums               (void) ;
    virtual void ShowCrowds               (void) ;
    virtual void ShowGalleries            (void) ;
    virtual void ShowScheduler            (void) ;

    virtual void Close                    (ContainEditor * cpe) ;
    virtual void EditPoint                (QString title,EditableUniverse * universe,ControlPoint * point) ;
    virtual void RenderSequences          (QString title,SceneObject * obj,SMAPs * maps) ;
    virtual SUID addCamera                (SUID startUuid,Camera * camera) ;
    virtual void EditCamera               (EditableUniverse * universe,Camera * camera) ;
    virtual void Close                    (CameraBox * camera) ;

    virtual void AudioProcessor           (void) ;

    virtual void EditFTP                  (int evType,SUID u,bool panel = false) ;

    virtual int  MenuBinder               (MenuManager & mm           ,
                                           QAction     * action       ,
                                           QVariants   & v            ,
                                           void        * dispatcher ) ;
    virtual void BindingMenu              (SceneObject * node) ;
    virtual void BindingMenu              (Universe    * node) ;

    virtual int  CallFunctions            (void * dispatcher,QVariants & values) ;
    virtual void CorpFunctions            (Universe * universe) ;
    virtual void CorpFunctions            (SceneObject * scene) ;

  protected slots:

    virtual void PicturesDropIn           (VarArgs & args) ;
    virtual void PeopleDropIn             (VarArgs & args) ;
    virtual void AlbumsDropIn             (VarArgs & args) ;
    virtual void VideosDropIn             (VarArgs & args) ;
    virtual void AudiosDropIn             (VarArgs & args) ;
    virtual void CamerasDropIn            (VarArgs & args) ;

    virtual bool Menu                     (QPoint pos) ;
    virtual bool RenderingMenu            (MenuManager & menu       ,
                                           QMenu       * rendering  ,
                                           int         & interval   ,
                                           int         & motion   ) ;
    virtual bool ResourceMenu             (MenuManager & menu       ,
                                           QMenu       * resource ) ;
    virtual bool ThreadsMenu              (MenuManager & menu       ,
                                           QMenu       * threads  ) ;
    virtual bool BridgeMenu               (MenuManager & menu       ,
                                           QMenu       * bridge   ) ;
    virtual bool WindowMenu               (MenuManager & menu     ) ;
    virtual bool ObjectsMenu              (MenuManager & menu       ,
                                           QMenu       * objects  ) ;

    virtual bool ResourceProcessor        (MenuManager & menu,QAction * action) ;
    virtual bool MultimediaProcessor      (MenuManager & menu,QAction * action) ;
    virtual bool WindowProcessor          (MenuManager & menu,QAction * action) ;

    virtual void VcfVideoMenu             (VcfVideo * canvas,QPointF pos) ;

    virtual bool assureObjectName         (SUID    uuid ) ;
    virtual bool assureObjectNames        (SUID    uuid ) ;
    virtual bool assureVideos             (UUIDs & Uuids) ;
    virtual bool assureAlbums             (UUIDs & Uuids) ;

    virtual void ExamVideo                (SUID uuid) ;
    virtual void LaunchVideo              (SUID uuid,int MID) ;

    virtual void AttachWaveJunction       (VideoFeeder   * feeder,Junction * junc) ;
    virtual void Finished                 (VideoUniverse * universe) ;
    virtual void VideoStop                (VideoUniverse * universe) ;
    virtual void RequestAudio             (VideoUniverse * universe,bool enabled) ;
    virtual void RequestPanel             (VideoUniverse * universe,bool enabled) ;
    virtual void VideoMovable             (VideoUniverse * universe,bool enabled) ;
    virtual void PlaceCanvas              (VideoUniverse * universe) ;
    virtual void AssignCanvas             (VideoUniverse * universe) ;
    virtual void RemoveCanvas             (VideoUniverse * universe) ;
    virtual void VideoChanged             (VcfItem       * item) ;
    virtual void PanelMenu                (int ThreadId,QPoint pos) ;
    virtual void RatioChanged             (int Id,QRect rect) ;
    virtual void RatioClose               (int Id,VideoRatio * vr) ;

    virtual void ExamAudio                (SUID uuid) ;
    virtual void LaunchAudio              (SUID uuid,int MID) ;

    virtual void RequestPanel             (WaveUniverse  * universe,bool enabled) ;
    virtual void AudioMovable             (WaveUniverse  * universe,bool enabled) ;
    virtual void PlaceCanvas              (WaveUniverse  * universe) ;
    virtual void AssignCanvas             (WaveUniverse  * universe) ;
    virtual void RemoveCanvas             (WaveUniverse  * universe) ;
    virtual void AudioChanged             (VcfItem       * item) ;

    virtual void LaunchRecorder           (VarArgs arguments) ;

    virtual void Existing                 (void) ;
    virtual void Login                    (QString login,QString hostname) ;

    virtual void EditEvent                (QObject * calendar ,
                                           SUID      uuid     ,
                                           QDateTime start    ,
                                           QDateTime end      ,
                                           bool      panel  ) ;

    virtual void EventHandler             (int ServiceId,SUID u) ;

  private slots:

    void         EmitEmptyShell           (void) ;
    void         EmitPlaying              (SUID u) ;
    void         EmitStopped              (SUID u) ;
    void         PrivateSayPlaying        (SUID u) ;
    void         PrivateSayStopped        (SUID u) ;

  signals:

    void         PrivateEmptyShell        (void) ;
    void         PrivateEmitPlaying       (SUID u) ;
    void         PrivateEmitStopped       (SUID u) ;

    void         SendStatus               (QString message) ;
    void         OpenBackup               (QString BackupINI) ;
    void         EmptyShell               (void) ;
    void         SayPlaying               (SUID u) ;
    void         SayStopped               (SUID u) ;

} ;

Q_AV_EXPORT Universe * EditableCreator (void * p,QObject * parent) ;
Q_AV_EXPORT Universe * GroupCreator    (void * p,QObject * parent) ;

}

QT_END_NAMESPACE

#endif
