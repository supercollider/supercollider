#ifndef QC_SOUND_FILE_VIEW_H
#define QC_SOUND_FILE_VIEW_H

#include <sndfile.h>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QSlider>
#include <QPixmap>
#include <QThread>

class QcWaveform;
class SoundCacheStream;

struct PeakCache {
  PeakCache() : min(0), max(0) {};
  ~PeakCache() {
    delete [] min;
    delete [] max;
  }
  short *min;
  short *max;
};

class QcSoundFileView : public QWidget
{
  friend class QcWaveform;

  Q_OBJECT
  /* TODO:
  Q_PROPERTY( float readProgress READ loadProgress );
  Q_PROPERTY( int frames READ frames );
  Q_PROPERTY( int viewFrames READ viewFrames );
  Q_PROPERTY( int scrollPos READ scrollPos WRITE scrollTo );
  */

public:
  QcSoundFileView();
  Q_INVOKABLE void load( const QString& filename );
  /* TODO:
  float readProgress();
  int frames();
  int viewFrames();
  int scrollPos();
  */

public Q_SLOTS:
  /* TODO:
  void scrollTo( int frame );
  void scrollBy( int frames );
  void scrollToStart();
  void scrollToEnd();
  void zoomTo( float fraction );
  void zoomBy( float factor );
  void zoomAll();
  */
private Q_SLOTS:
  void onPosSliderChanged( int value );
  void onZoomSliderChanged( int value );
private Q_SLOTS:
  void updateTimeScrollBar();
  void updateZoomScrollBar();

private:
  QcWaveform *waveform;
  QVBoxLayout *layout;
  QScrollBar *timeScrollBar;
  QSlider *zoomScrollBar;

  double hScrollMultiplier;
};

class QcWaveform : public QWidget {

  Q_OBJECT

public:

  QcWaveform( QWidget * parent = 0 );
  ~QcWaveform();
  void load( const QString& filename );

  float zoom();
  quint64 viewStart() { return _beg; }
  quint64 viewWidth() { return _dur; }
  quint64 duration() { if( sf ) return sfInfo.frames; else return 0; }

  void setZoom( float );
  void setViewStart( quint64 startFrame );

Q_SIGNALS:

  void loadProgress( int );
  void loadingDone();

public Q_SLOTS:

  void redraw() {
    dirty = true;
    update();
  }

protected:

  virtual QSize sizeHint() const { return QSize( 400, 200 ); }
  virtual void resizeEvent( QResizeEvent * );
  virtual void paintEvent( QPaintEvent * );

private:

  void rebuildCache ( int maxFramesPerCache, int maxRawFrames );
  void draw ( QPixmap *, int x, int width, double beginning, double duration );

  SNDFILE *sf;
  SF_INFO sfInfo;

  SoundCacheStream *_cache;

  quint64 _beg;
  quint64 _dur;

  QPixmap *pixmap;
  bool dirty;
};

class SoundStream {
public:
  inline int channels() { return ch; }

  inline double beginning() { return beg; }

  inline double duration() { return dur; }

  virtual bool integrate( int channel, double offset, double duration,
                          short *minBuffer, short *maxBuffer, int bufferSize ) = 0;

  inline bool integrateAll( int channel, short *minBuffer, short *maxBuffer, int bufferSize ) {
    return integrate( channel, beginning(), duration(), minBuffer, maxBuffer, bufferSize );
  }

  virtual short *rawFrames( int channel, quint64 beginning, quint64 duration, bool *interleaved ) = 0;

protected:
  SoundStream()
  : ch( 0 ), beg( 0.0 ), dur( 0.0 ) {}
  SoundStream( int channels, double beginning, double duration ) :
    ch( channels ), beg( beginning ), dur( duration ) {}
  int ch;
  double beg;
  double dur;
};

class SoundFileStream : public SoundStream
{
public:
  SoundFileStream();
  SoundFileStream( SNDFILE *sf, const SF_INFO &sf_info, double beginning, double duration );
  ~SoundFileStream();
  void load( SNDFILE *sf, const SF_INFO &sf_info, double beginning, double duration );
  bool integrate( int channel, double offset, double duration,
                  short *minBuffer, short *maxBuffer, int bufferSize );
  short *rawFrames( int channel, quint64 beginning, quint64 duration, bool *interleaved );
private:
  short *_data;
  int _dataSize;
  quint64 _dataOffset;
};

class SoundCacheLoader;

class SoundCacheStream : public QObject, public SoundStream
{
  friend class SoundCacheLoader;

  Q_OBJECT

public:
  SoundCacheStream();
  ~SoundCacheStream();
  void load( SNDFILE *sf, const SF_INFO &info, int maxFramesPerUnit, int maxRawFrames );
  inline double fpu() { return _fpu; }
  inline int size() { return _cacheSize; }
  inline bool ready() { return _ready; }
  inline bool loading() { return _loading; }
  inline int loadProgress() { return _loadProgress; }
  bool integrate( int channel, double offset, double duration,
                  short *minBuffer, short *maxBuffer, int bufferSize );
  short *rawFrames( int channel, quint64 beginning, quint64 duration, bool *interleaved );

Q_SIGNALS:
  void loadProgress( int );
  void loadingDone();

private Q_SLOTS:
  void onLoadProgress( int );
  void onLoadingDone();

private:
  PeakCache *_caches;
  double _fpu; // soundfile frames per cache frame
  int _cacheSize;
  bool _ready;
  bool _loading;
  SoundCacheLoader *_loader;
  int _loadProgress;
};

class SoundCacheLoader : public QThread
{
  Q_OBJECT
public:
  SoundCacheLoader( SoundCacheStream *cache ) : QThread( cache ), _cache( cache ), _sf(0) {}
  void load( SNDFILE *sf, const SF_INFO &info );

Q_SIGNALS:
  void loadProgress( int );
  void loadingDone();
private:
  void run();

  SoundCacheStream *_cache;
  SNDFILE *_sf;
  SF_INFO _info;
};

#endif
