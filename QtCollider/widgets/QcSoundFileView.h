/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#ifndef QC_SOUND_FILE_VIEW_H
#define QC_SOUND_FILE_VIEW_H

#include "../Common.h"
#include "../QcHelper.h"

#include <sndfile.h>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QSlider>
#include <QPixmap>
#include <QThread>

class QcWaveform;
class SoundCacheStream;

struct SoundCache {
  SoundCache() : min(0), max(0), sum(0), sum2(0) {};
  ~SoundCache() {
    delete [] min;
    delete [] max;
    delete [] sum;
    delete [] sum2;
  }
  short *min;
  short *max;
  float *sum;
  float *sum2;
};

class QcSoundFileView : public QWidget
{
  friend class QcWaveform;

  Q_OBJECT


public:
  QcSoundFileView();
  Q_INVOKABLE void load( const QString& filename );

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

class QcWaveform : public QWidget, public QcHelper {

  Q_OBJECT

  Q_PROPERTY( float readProgress READ loadProgress );
  Q_PROPERTY( int frames READ frames );
  Q_PROPERTY( int viewFrames READ viewFrames );
  Q_PROPERTY( int scrollPos READ scrollPos WRITE scrollTo );
  Q_PROPERTY( int currentSelection READ currentSelection WRITE setCurrentSelection );
  Q_PROPERTY( QColor peakColor READ dummyColor WRITE setPeakColor );
  Q_PROPERTY( QColor rmsColor READ dummyColor WRITE setRmsColor );
public:

  struct Selection {
    Selection() : start(0), size(0), editable(true), color(QColor(0,0,200)) {}
    quint64 start;
    quint64 size;
    bool editable;
    QColor color;
  };

  QcWaveform( QWidget * parent = 0 );
  ~QcWaveform();

  Q_INVOKABLE void load( const QString& filename );
  float loadProgress();
  quint64 frames() { return sfInfo.frames; }
  quint64 viewFrames() { return _dur; }
  quint64 scrollPos() { return _beg; }
  float zoom();

  int currentSelection() const { return _curSel; }
  void setCurrentSelection( int index ) { _curSel = index; update(); }
  Q_INVOKABLE VariantList selection( int index );
  Q_INVOKABLE void setSelection( int index, VariantList data );
  Q_INVOKABLE void setSelectionEditable( int index, bool editable );
  Q_INVOKABLE void setSelectionColor( int index, const QColor &clr );

  void setPeakColor( const QColor &clr ) { _peakColor = clr; redraw(); }
  void setRmsColor( const QColor &clr ) { _rmsColor = clr; redraw(); }

public Q_SLOTS:
  void zoomTo( float fraction );
  void zoomBy( float factor );
  void zoomAllOut();
  void scrollTo( quint64 frame );
  void scrollBy( qint64 frames );
  void scrollToStart();
  void scrollToEnd();

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

  Selection _selections[64];
  int _curSel;

  QPixmap *pixmap;
  QColor _peakColor;
  QColor _rmsColor;
  bool dirty;
};

class SoundStream {
public:
  inline int channels() { return ch; }

  inline double beginning() { return beg; }

  inline double duration() { return dur; }

  virtual bool displayData( int channel, double offset, double duration,
                            short *minBuffer,
                            short *maxBuffer,
                            short *minRMS,
                            short *maxRMS,
                            int bufferSize ) = 0;

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
                  short *minBuffer,
                  short *maxBuffer,
                  float *sumBuffer,
                  float *sum2Buffer,
                  int bufferSize );
  bool displayData( int channel, double offset, double duration,
                    short *minBuffer,
                    short *maxBuffer,
                    short *minRMS,
                    short *maxRMS,
                    int bufferSize );
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
  bool displayData( int channel, double offset, double duration,
                    short *minBuffer,
                    short *maxBuffer,
                    short *minRMS,
                    short *maxRMS,
                    int bufferSize );
  short *rawFrames( int channel, quint64 beginning, quint64 duration, bool *interleaved );

Q_SIGNALS:
  void loadProgress( int );
  void loadingDone();

private Q_SLOTS:
  void onLoadProgress( int );
  void onLoadingDone();

private:
  SoundCache *_caches;
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
