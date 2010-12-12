#ifndef QC_SOUND_FILE_VIEW_H
#define QC_SOUND_FILE_VIEW_H

#include <sndfile.h>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QSlider>
#include <QProgressBar>
#include <QPixmap>

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
public:
  QcSoundFileView();
  Q_INVOKABLE void load( const QString& filename );
private Q_SLOTS:
  void setBeginning( int centiseconds );
  void setZoom( int percent );
private Q_SLOTS:
  void updateTimeScrollBar();
  void updateZoomScrollBar();

private:
  QcWaveform *waveform;
  QVBoxLayout *layout;
  QScrollBar *timeScrollBar;
  QSlider *zoomScrollBar;
  QProgressBar *progressBar;

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

protected:

  virtual QSize sizeHint() const { return QSize( 400, 200 ); }
  virtual void resizeEvent( QResizeEvent * );
  virtual void paintEvent( QPaintEvent * );

private:


  inline void redraw() {
    dirty = true;
    update();
  }

  void rebuildCache ( int maxFramesPerCache, int maxRawFrames );
  void draw ( QPixmap *, int x, int width, double beginning, double duration );

  inline void integrate( short *srcMin, short *srcMax,
                         int stepSize, int steps, short &p_min, short &p_max );

  SNDFILE *sf;
  SF_INFO sfInfo;

  SoundCacheStream *_cache;
  PeakCache *peakCache;
  double _fpcache; // soundfile frames per cache frame
  int _cacheSize;

  quint64 _beg;
  quint64 _dur;

  QPixmap *pixmap;
  bool dirty;
};

#if 0
class Integrator
{
  virtual void load( float startFrame, float endFrame, int steps ) = 0;
  virtual void integralAt( int channel, int index, short *p_min, short *p_max );
protected:
  void integrate( short *data, int step, int steps, short *p_min, short *p_max );
}

class SoundFile : public Integrator
{
public:
  void load( float startFrame, float endFrame, int steps );
private:
  short *buffer;
  int bufFrames;
  float stepSize;
}
#endif

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

  virtual quint64 dataFrameCount() { return 0; }

  virtual double dataResolution() { return 0.0; }

  virtual quint64 dataOffset() { return 0; }

  virtual bool interleaved() = 0;

  virtual short *dataMin( int channel ) { return 0; }
  virtual short *dataMax( int channel ) { return 0; }

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
  bool interleaved () { return true; }
  bool integrate( int channel, double offset, double duration,
                  short *minBuffer, short *maxBuffer, int bufferSize );
private:
  short *_data;
  int _dataSize;
  quint64 _dataOffset;
};

class SoundCacheStream : public QObject, public SoundStream
{
  friend class QcWaveform;

  Q_OBJECT

public:
  SoundCacheStream( SNDFILE *sf, const SF_INFO &info, int maxFramesPerUnit, int maxRawFrames );
  ~SoundCacheStream();
  inline bool interleaved () { return false; }
  inline double fpu() { return _fpu; }
  bool integrate( int channel, double offset, double duration,
                  short *minBuffer, short *maxBuffer, int bufferSize );
Q_SIGNALS:
  void loadProgress( int );
  void loadingDone();

private:

  PeakCache *_caches;
  double _fpu; // soundfile frames per cache frame
  int _cacheSize;
};

#endif
