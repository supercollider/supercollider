#ifndef QC_SOUND_FILE_VIEW_H
#define QC_SOUND_FILE_VIEW_H

#include <sndfile.h>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QProgressBar>
#include <QPixmap>

class QcWaveform;

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
  QScrollBar *zoomScrollBar;
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

  void rebuildCache ( int resolution );
  void draw ( QPixmap *, int x, int width, double beginning, double duration );

  struct PeakCache {
    PeakCache() : min(0), max(0) {};
    ~PeakCache() {
      delete [] min;
      delete [] max;
    }
    short *min;
    short *max;
  };

  inline void integrate( short *srcMin, short *srcMax,
                         int stepSize, int steps, short &p_min, short &p_max );

  SNDFILE *sf;
  SF_INFO sfInfo;

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

#endif
