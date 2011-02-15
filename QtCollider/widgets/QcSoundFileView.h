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
  Q_PROPERTY( float yZoom READ yZoom WRITE setYZoom );
  Q_PROPERTY( float xZoom READ xZoom WRITE setXZoom );
  Q_PROPERTY( bool cursorVisible READ cursorVisible WRITE setCursorVisible );
  Q_PROPERTY( bool cursorEditable READ dummyBool WRITE setCursorEditable );
  Q_PROPERTY( int cursorPosition READ cursorPosition WRITE setCursorPosition );
  Q_PROPERTY( QColor cursorColor READ dummyColor WRITE setCursorColor );
  Q_PROPERTY( bool gridVisible READ dummyFloat WRITE setGridVisible );
  Q_PROPERTY( float gridOffset READ dummyFloat WRITE setGridOffset );
  Q_PROPERTY( float gridResolution READ dummyFloat WRITE setGridResolution );
  Q_PROPERTY( QColor gridColor READ dummyColor WRITE setGridColor );
  Q_PROPERTY( bool drawsWaveform READ drawsWaveform WRITE setDrawsWaveform );

public:

  struct Selection {
    Selection() : start(0), size(0), editable(true), color(QColor(0,0,150)) {}
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
  float zoom(); //visible fraction
  float xZoom(); //visible seconds
  float yZoom(); //factor

  int currentSelection() const { return _curSel; }
  void setCurrentSelection( int i );
  Q_INVOKABLE VariantList selection( int index );
  void setSelection( int i, quint64 a, quint64 b );
  Q_INVOKABLE void setSelection( int index, VariantList data );
  Q_INVOKABLE void setSelectionStart( int i, quint64 frame );
  Q_INVOKABLE void setSelectionEnd( int i, quint64 frame );
  Q_INVOKABLE void setSelectionEditable( int index, bool editable );
  Q_INVOKABLE void setSelectionColor( int index, const QColor &clr );

  bool cursorVisible() { return _showCursor; }
  void setCursorVisible( bool b ) { _showCursor = b; update(); }
  int cursorPosition() { return _cursorPos; }
  void setCursorPosition( int pos ) { _cursorPos = pos; update(); }
  void setCursorColor( const QColor &c ) { _cursorColor = c; update(); }
  void setCursorEditable( bool b ) { _cursorEditable = b; }

  void setGridVisible( bool b ) { _showGrid = b; update(); }
  void setGridOffset( float f ) { _gridOffset = f; update(); }
  void setGridResolution( float f ) { _gridResolution = f; update(); }
  void setGridColor( const QColor &c ) { _gridColor = c; update(); }

  void setPeakColor( const QColor &clr ) { _peakColor = clr; redraw(); }
  void setRmsColor( const QColor &clr ) { _rmsColor = clr; redraw(); }

  bool drawsWaveform() const { return _drawWaveform; }
  bool setDrawsWaveform( bool b ) { _drawWaveform = b; update(); }

  QSize sizeHint() const { return QSize( 400, 200 ); }
  QSize minimumSizeHint() const { return QSize( 100, 20 ); }

public Q_SLOTS:
  void zoomTo( float fraction );
  //void zoomTo( float fraction, quint64 frame );
  void zoomBy( float factor );
  void zoomAllOut();
  void scrollTo( quint64 frame );
  void scrollBy( qint64 frames );
  void scrollToStart();
  void scrollToEnd();
  void setYZoom( float factor );
  void setXZoom( float seconds );
  void zoomSelection( int selectionIndex );

Q_SIGNALS:

  void loadProgress( int );
  void loadingDone();

  void action();

public Q_SLOTS:

  void redraw() {
    dirty = true;
    update();
  }

protected:

  virtual void resizeEvent( QResizeEvent * );
  virtual void paintEvent( QPaintEvent * );
  virtual void mousePressEvent( QMouseEvent * );
  virtual void mouseDoubleClickEvent ( QMouseEvent * );
  virtual void mouseMoveEvent( QMouseEvent * );

private:

  inline void updateFPP() { _fpp = width() ? (double) _dur / width() : 0.0; }
  void rebuildCache ( int maxFramesPerCache, int maxRawFrames );
  void draw ( QPixmap *, int x, int width, double beginning, double duration );

  // data
  SNDFILE *sf;
  SF_INFO sfInfo;

  SoundCacheStream *_cache;

   // selections
  Selection _selections[64];
  int _curSel;

  // cursor
  bool _showCursor;
  quint64 _cursorPos;
  QColor _cursorColor;
  bool _cursorEditable;

  //grid
  bool _showGrid;
  float _gridResolution;
  float _gridOffset;
  QColor _gridColor;

  // view
  quint64 _beg;
  quint64 _dur;
  double _fpp;
  float _yZoom;

  // painting
  QPixmap *pixmap;
  QColor _peakColor;
  QColor _rmsColor;
  bool dirty;
  bool _drawWaveform;

  // interaction
  enum DragAction {
    NoDragAction,
    Scroll,
    Zoom,
    Select,
    MoveSelection,
    MoveCursor
  };
  DragAction _dragAction;
  QPointF _dragPoint;
  quint64 _dragFrame;
  double _dragData;
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
