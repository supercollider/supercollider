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

#include "QcSoundFileView.h"
#include "../QcWidgetFactory.h"

#include <QGridLayout>
#include <QPainter>
#include <QApplication>
#include <QPaintEvent>

#include <climits>
#include <cmath>

static QcWidgetFactory<QcSoundFileView> sfViewFactory;
static QcWidgetFactory<QcWaveform> waveformFactory;

QcSoundFileView::QcSoundFileView() :
  hScrollMultiplier( 0.f )
{
  waveform = new QcWaveform();

  timeScrollBar = new QScrollBar( Qt::Horizontal );

  zoomScrollBar = new QSlider();
  zoomScrollBar->setRange( 0, 1000 );

  QGridLayout *l = new QGridLayout();
  l->addWidget( waveform , 0, 0 );
  l->addWidget( zoomScrollBar, 0, 1 );
  l->addWidget( timeScrollBar, 1, 0, 1, 2 );

  setLayout( l );

  connect( timeScrollBar, SIGNAL(valueChanged(int)), this, SLOT(onPosSliderChanged(int)) );
  connect( zoomScrollBar, SIGNAL(valueChanged(int)), this, SLOT(onZoomSliderChanged(int)) );
  connect( waveform, SIGNAL(loadingDone()), this, SLOT(updateTimeScrollBar()) );
  connect( waveform, SIGNAL(loadingDone()), this, SLOT(updateZoomScrollBar()) );

  updateTimeScrollBar();
}



void QcSoundFileView::load( const QString& filename )
{
  waveform->load( filename );
}


void QcSoundFileView::onPosSliderChanged( int value )
{
  waveform->scrollTo( value * hScrollMultiplier );
}

void QcSoundFileView::onZoomSliderChanged( int z )
{
  float frac = (float)(zoomScrollBar->maximum() - z) / zoomScrollBar->maximum();
  frac *= frac;
  frac *= frac;
  waveform->zoomTo( frac );
  updateTimeScrollBar();
}

void QcSoundFileView::updateTimeScrollBar()
{
  // FIXME:
  // view start moves weirdly when changing zoom. setSliderPosition() actually calls Waveform
  // again, and the resulting position is not the same as Waveform's original.

  quint64 max;
  max = waveform->frames() - waveform->viewFrames();
  if( max > 1000 ) {
    hScrollMultiplier = max / 1000.0;
    max = 1000;
  } else {
    hScrollMultiplier = 1.0;
  }
  timeScrollBar->setMaximum( max );
  //printf("before: %Li\n", waveform->scrollPos() );
  timeScrollBar->blockSignals( true );
  timeScrollBar->setSliderPosition( waveform->scrollPos() / hScrollMultiplier );
  timeScrollBar->blockSignals( false );
  //printf("after: %Li\n", waveform->scrollPos() );
  timeScrollBar->setEnabled( max > 0 );
}

void QcSoundFileView::updateZoomScrollBar()
{
  int max = zoomScrollBar->maximum();
  zoomScrollBar->setSliderPosition( max - (waveform->zoom() * max) );
}

QcWaveform::QcWaveform( QWidget * parent ) : QWidget( parent ),
  sf(0),
  _cache(0),
  _beg(0.0),
  _dur(0.0),
  _curSel(0),
  pixmap(0),
  dirty(false)
{
  memset( &sfInfo, 0, sizeof(SF_INFO) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

QcWaveform::~QcWaveform()
{
  delete _cache;
  delete pixmap;
  if( sf ) sf_close( sf );
}

void QcWaveform::load( const QString& filename )
{
  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );

  SNDFILE *new_sf = sf_open( filename.toStdString().c_str(), SFM_READ, &new_info );

  printf("bla\n");
  if( !new_sf ) {
    printf("Could not open soundfile!\n");
    return;
  }

  // NOTE we have to delete SoundCacheStream before closing the soundfile, as it might be still
  // loading it
  // TODO: should SoundCacheStream open the soundfile on its own?

  delete _cache;
  if( sf ) sf_close( sf );

  sf = new_sf;
  sfInfo = new_info;
  _beg = 0;
  _dur = sfInfo.frames;
  printf("frames %Li\n", sfInfo.frames);

  _cache = new SoundCacheStream();
  connect( _cache, SIGNAL(loadProgress(int)),
           this, SIGNAL(loadProgress(int)) );
  connect( _cache, SIGNAL(loadProgress(int)),
           this, SLOT(update()) );
  connect( _cache, SIGNAL(loadingDone()), this, SIGNAL(loadingDone()) );
  connect( _cache, SIGNAL(loadingDone()), this, SLOT(redraw()) );

  _cache->load( sf, sfInfo, 128, 300000 );

  redraw();
}

float QcWaveform::loadProgress()
{
  return _cache ? _cache->loadProgress() : 1.f;
}

float QcWaveform::zoom()
{
  return sfInfo.frames ? (double) _dur / sfInfo.frames : 0;
}

VariantList QcWaveform::selectionAt( int index )
{
  VariantList l;
  if( index < 0 || index >= 64 ) return l;
  const Selection &s = _selections[index];
  l.data << QVariant(static_cast<int>(s.start));
  l.data << QVariant(static_cast<int>(s.size));
  return l;
}

void QcWaveform::setSelectionAt( int index, VariantList l )
{
  if( index < 0 || index >= 64 || l.data.count() < 2 ) return;
  Selection& s = _selections[index];
  s.start = l.data[0].toInt();
  s.size = l.data[1].toInt();
  update();
}

void QcWaveform::setSelectionEditable( int index, bool editable )
{
  if( index < 0 || index >= 64 ) return;
  _selections[index].editable = editable;
  update();
}

void QcWaveform::zoomTo( float z )
{
  z = qMax( 0.f, qMin( 1.f, z ) );

  _dur = sfInfo.frames ? qMax( 1.f, sfInfo.frames * z ) : 0;

  //printf("dur: %Li view: %Li\n", sfInfo.frames, _dur);
  if( _beg + _dur > sfInfo.frames ) _beg = sfInfo.frames - _dur;

  redraw();
}

void QcWaveform::zoomBy( float factor )
{
  double z = zoom() * factor;
  zoomTo( z );
  redraw();
}

void QcWaveform::zoomAllOut()
{
  _beg = 0.0;
  _dur = sfInfo.frames;
  redraw();
}

void QcWaveform::scrollTo( quint64 startFrame )
{
  _beg = qMax( (quint64) 0, qMin( (quint64) sfInfo.frames - _dur, startFrame ) );
  redraw();
}

void QcWaveform::scrollBy( qint64 f )
{
  // NOTE take care of signedness
  quint64 dest;
  if( f < 0 ) {
    qint64 reversed = -f;
    dest = ( _beg > reversed ) ? ( _beg - reversed ) : 0;
  }
  else {
    dest = _beg + f;
  }
  scrollTo( dest );
}

void QcWaveform::scrollToStart()
{
  scrollTo( _beg );
}

void QcWaveform::scrollToEnd()
{
  scrollTo( sfInfo.frames - _dur );
}

void QcWaveform::resizeEvent( QResizeEvent * )
{
  delete pixmap;
  pixmap = new QPixmap( size() );
  redraw();
}

void QcWaveform::paintEvent( QPaintEvent *ev )
{
  Q_ASSERT( pixmap != 0 );

  QPainter p( this );

  if( _cache && _cache->loading() ) {
    QRect r( rect() );
    p.fillRect( r, QColor(70,70,70) );
    r.setRight( r.right() * _cache->loadProgress() / 100 );
    p.fillRect( r, QColor( 0, 0, 0 ) );
    p.setPen( QColor(255,255,255) );
    QTextOption opt;
    opt.setAlignment( Qt::AlignCenter );
    p.drawText( rect(), "loading...", opt );
    return;
  }

  if( dirty ) {
    draw( pixmap, 0, width(), _beg, _dur );
    dirty = false;
  }

  p.drawPixmap( ev->rect(), *pixmap, ev->rect() );

  p.scale( (double) width() / _dur, 1.0 );
  p.translate( _beg * -1.0, 0.0 );
  p.setPen( Qt::NoPen );

  int i;
  for( i = 0; i < 64; ++i ) {
    const Selection &s = _selections[i];
    if( s.size > 0 );
    QRectF r ( s.start, 0, s.size, height() );
    p.fillRect( r, i == _curSel ? QColor( 255, 0, 0, 100 ) : QColor( 255, 255, 255, 100 ) );
  }
}

void QcWaveform::rebuildCache ( int maxFPU, int maxRawFrames )
{
}

void QcWaveform::draw( QPixmap *pix, int x, int width, double f_beg, double f_dur )
{
  // FIXME anomaly: when fpp reaching 1.0 rms can go outside min-max!

  QPainter p( pix );
  p.fillRect( pix->rect(), QColor( 0, 0, 0 ) );

  if( !sf || !_cache || !_cache->ready() ) return;

  // check for sane situation:
  if( f_beg < 0 || f_beg + f_dur > sfInfo.frames ) return;

  // data indexes
  quint64 d_beg = floor(f_beg); // data beginning;
  int d_count = ceil( f_beg + f_dur ) - d_beg; // data count;
  bool haveOneMore;
  if( sfInfo.frames - (f_beg + f_dur) >= 1 ) {
    ++d_count;
    haveOneMore = true;
  }
  else {
    haveOneMore = false;
  }

  // data source - choose according to horiz. zoom (data-display resolution)
  SoundStream *soundStream;
  SoundFileStream sfStream;

  double fpp = f_dur / width;
  if( fpp > 1.0 ? (fpp < _cache->fpu()) : _cache->fpu() > 1.0 ) {
    printf("use file\n");
    soundStream = &sfStream;
    sfStream.load( sf, sfInfo, d_beg, d_count );
  }
  else {
    printf("use cache\n");
    soundStream = _cache;
  }

  // geometry
  float chHeight = pix->height() / (float) sfInfo.channels;
  float yscale = -chHeight / 65535.f;
  float spacing = 0.f;

  // initial painter setup
  QPen minMaxPen( QColor(180,180,0) );
  QPen rmsPen( QColor(255,255,0) );
  p.scale( 1.f, yscale );
  p.translate( (float) x, -32767.f );


  int ch;
  for( ch = 0; ch < soundStream->channels(); ++ch ) {
    p.setPen( QColor(255,255,255) );
    p.drawLine( x, 0, x + width, 0 );
    p.setPen( QColor(100,100,100) );
    p.drawLine( x, SHRT_MIN, x+width, SHRT_MIN );

    if( fpp > 1.0 ) {

      // draw min-max regions and RMS

      short minBuffer[width];
      short maxBuffer[width];
      short minRMS[width];
      short maxRMS[width];

      bool ok = soundStream->displayData( ch, f_beg, f_dur,
                                        minBuffer, maxBuffer,
                                        minRMS, maxRMS,
                                        width );
//    printf("integration ok: %i\n", ok);
      Q_ASSERT( ok );

      int i;
      for( i = 0; i < width; ++i ) {
        short min = minBuffer[i];
        short max = maxBuffer[i];
        if( max != min ) {
          p.setPen( minMaxPen );
          p.drawLine( x + i, min, x + i, max );
        }
        else {
          p.setPen( minMaxPen );
          p.drawPoint( x + i, min );
        }
        p.setPen( rmsPen );
        p.drawLine( x + i, minRMS[i], x + i, maxRMS[i] );
      }
    }
    else {

      // draw lines between actual values

      qreal ppf = 1.0 / fpp;
      qreal dx = (d_beg - f_beg) * ppf;

      bool interleaved = false;
      short *data = soundStream->rawFrames( ch, d_beg, d_count, &interleaved );
      //printf("got raw frames ok: %i\n", data != 0 );
      Q_ASSERT( data != 0 );
      int step = interleaved ? soundStream->channels() : 1;

      QPainterPath path;

      if( d_count ) {
        QPointF pt( dx, (qreal) *data );
        path.moveTo( pt );
      }
      int f; // frame
      for( f = 1; f < d_count; ++f ) {
        data += step;
        QPointF pt( f * ppf + dx, (qreal) *data );
        path.lineTo( pt );
      }
      if( d_count && !haveOneMore ) {
        path.lineTo( QPointF( f * ppf + dx, (qreal)*data ) );
      }

      p.setPen( rmsPen );
      p.drawPath( path );
    }

    p.translate( 0.f, -65535.f + spacing );
  }

}


SoundFileStream::SoundFileStream() : _data(0), _dataSize(0), _dataOffset(0)
{}

SoundFileStream::SoundFileStream( SNDFILE *sf, const SF_INFO &info, double b, double d )
: _data(0)
{
  load( sf, info, b, d );
}

SoundFileStream::~SoundFileStream()
{
  delete[] _data;
}

void SoundFileStream::load( SNDFILE *sf, const SF_INFO &info, double b, double d )
{
  delete[] _data;

  _dataOffset = floor(b);
  _dataSize = ceil(b + d) - _dataOffset;

  _data = new short [_dataSize * info.channels];
  sf_seek( sf, _dataOffset, SEEK_SET);
  _dataSize = sf_readf_short( sf, _data, _dataSize );

  ch = info.channels;
  beg = _dataOffset;
  dur = _dataSize;
}

bool SoundFileStream::integrate
( int ch, double off, double dur,
  short *minBuffer, short *maxBuffer, float *sumBuf, float *sum2Buf, int bufferSize )
{
  bool ok = _data != 0
            && ch < channels()
            && ( off >= beginning() )
            && ( off + dur <= beginning() + duration() );
  if( !ok ) return false;

  double fpu = dur / bufferSize;
  double f_pos = off - beginning();

  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-proof:
    // f_pos = (double)(i+1) / width() * dur + off;

    // the following is a faster variant, but floating point operations are fallible,
    // so we need to make sure we stay within the constraints of f_dur;
    double f_pos1 = f_pos + fpu;
    if( f_pos1 > dur ) f_pos1 = dur;

    int frame_count = ceil(f_pos1) - data_pos;

    float frac0 = data_pos + 1.f - f_pos;
    float frac1 = f_pos1 + 1.f - ceil(f_pos1);

    // get min, max and sum
    short *samples = _data + (data_pos * channels()) + ch;
    short min = SHRT_MAX;
    short max = SHRT_MIN;
    float sum = 0.f;
    float sum2 = 0.f;
    int f; // frame
    for( f = 0; f < frame_count; ++f, samples += channels() ){
    // TODO should we overlap min-max or not here?
      float sample = *samples;
      float frac;
      if( f == 0 ) frac = frac0;
      else if( f == frame_count - 1 ) frac = frac1;
      else frac = 1.0;

      if( sample < min ) min = sample;
      if( sample > max ) max = sample;

      sum += sample * frac;
      sum2 += sample * sample * frac;
    }

    minBuffer[i] = min;
    maxBuffer[i] = max;
    sumBuf[i] = sum;
    sum2Buf[i] = sum2;

    f_pos = f_pos1;
  }

  return true;
}

bool SoundFileStream::displayData
( int ch, double off, double dur,
  short *minBuffer, short *maxBuffer, short *minRMS, short *maxRMS, int bufferSize )
{
  bool ok = _data != 0
            && ch < channels()
            && ( off >= beginning() )
            && ( off + dur <= beginning() + duration() );
  if( !ok ) return false;

  double fpu = dur / bufferSize;
  double f_pos = off - beginning();

  short min = SHRT_MAX;
  short max = SHRT_MIN;
  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-proof:
    // f_pos = (double)(i+1) / width() * dur + off;

    // the following is a faster variant, but floating point operations are fallible,
    // so we need to make sure we stay within the constraints of f_dur;
    double f_pos1 = f_pos + fpu;
    if( f_pos1 > dur ) f_pos1 = dur;

    int frame_count = ceil(f_pos1) - data_pos;

    float frac0 = data_pos + 1.f - f_pos;
    float frac1 = f_pos1 + 1.f - ceil(f_pos1);

    // get min, max and sum
    short *samples = _data + (data_pos * channels()) + ch;

    float sum = 0.f;
    float sum2 = 0.f;
    int f; // frame
    for( f = 0; f < frame_count; ++f, samples += channels() ){
      // TODO should we overlap min-max or not here?
      float sample = *samples;
      float frac;
      if( f == 0 ) frac = frac0;
      else if( f == frame_count - 1 ) frac = frac1;
      else frac = 1.0;

      if( sample < min ) min = sample;
      if( sample > max ) max = sample;

      sum += sample * frac;
      sum2 += sample * sample * frac;
    }

    double n = fpu;
    double avg = sum / n;
    double stdDev = sqrt( abs((sum2 - (sum*avg) ) / n) );

    minBuffer[i] = min;
    maxBuffer[i] = max;
    minRMS[i] = avg - stdDev;
    maxRMS[i] = avg + stdDev;

    f_pos = f_pos1;
    min = maxBuffer[i];
    max = minBuffer[i];
  }

  return true;
}

short *SoundFileStream::rawFrames( int ch, quint64 b, quint64 d, bool *interleaved )
{
  if( ch > channels() || b < _dataOffset || b + d > _dataOffset + _dataSize ) return 0;
  *interleaved = true;
  int offset = (b - _dataOffset) * channels() + ch;
  return ( _data + offset );
}


SoundCacheStream::SoundCacheStream()
: SoundStream ( 0, 0.0, 0.0 ),
  _caches(0),
  _fpu(0.0),
  _cacheSize(0),
  _ready(false),
  _loading(false),
  _loadProgress(0)
{
  _loader = new SoundCacheLoader( this );
  connect( _loader, SIGNAL(loadProgress(int)),
           this, SLOT(onLoadProgress(int)),
           Qt::QueuedConnection );
  connect( _loader, SIGNAL(loadingDone()), this, SLOT(onLoadingDone()), Qt::QueuedConnection );
}

void SoundCacheStream::load( SNDFILE *sf, const SF_INFO &info,
                             int maxFramesPerUnit, int maxRawFrames )
{
  _ready = false;
  _loadProgress = 0;

  if( _loader->isRunning() ) {
    _loader->terminate();
    _loader->wait();
  }
  delete [] _caches;

  ch = info.channels;
  beg = 0.0;
  dur = (double) info.frames;

  if( info.frames <= maxRawFrames ) {
    _cacheSize = info.frames;
    _fpu = 1.0;
  }
  else {
    _cacheSize = maxRawFrames;
    _fpu = (double) info.frames / _cacheSize;
    if( _fpu > maxFramesPerUnit ) {
      _fpu = maxFramesPerUnit;
      _cacheSize = info.frames / _fpu;
    }
  }

  _caches = new SoundCache [info.channels];
  int ch;
  for( ch = 0; ch < info.channels; ++ch ) {
    _caches[ch].min = new short [_cacheSize];
    _caches[ch].max = new short [_cacheSize];
    _caches[ch].sum = new float [_cacheSize];
    _caches[ch].sum2 = new float [_cacheSize];
  }

  _loading = true;
  _loader->load( sf, info );
}

SoundCacheStream::~SoundCacheStream()
{
  if( _loader->isRunning() ) {
    _loader->terminate();
    _loader->wait();
  }
  delete [] _caches;
}

bool SoundCacheStream::displayData
( int ch, double off, double dur,
  short *minBuffer, short *maxBuffer, short *minRMS, short *maxRMS, int bufferSize )
{
  // we assume that beginning() == 0

  bool ok = _ready
            && ch < channels()
            && ( off >= beginning() )
            && ( off + dur <= beginning() + duration() )
            && bufferSize <= dur * _fpu;
  if( !ok ) return false;

  double ratio = dur / _fpu / bufferSize;
  double cache_pos = off / _fpu;

  short min = SHRT_MAX;
  short max = SHRT_MIN;
  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int f = floor(cache_pos); // first frame
    bool no_overlap = f == ceil(cache_pos);
    float frac0 = f + 1.f - cache_pos;

    cache_pos += ratio;
    // Due to possibility of floating point operation failures.
    if( cache_pos > _cacheSize ) cache_pos = _cacheSize;
    int frame_count = ceil(cache_pos) - f ;
    float frac1 = cache_pos + 1.f - ceil(cache_pos);

    //short min = SHRT_MAX;
    //short max = SHRT_MIN;
    double sum = 0.0;
    double sum2 = 0.0;

    short *p_min = _caches[ch].min;
    short *p_max = _caches[ch].max;
    float *p_sum = _caches[ch].sum;
    float *p_sum2 = _caches[ch].sum2;
    int countdown = frame_count;
    while( countdown-- ) {

      // NOTE for min-max, behave as if first frame was ceil(cache_pos) instead of floor(),
      // to not smudge too much at large scale
      if( countdown < frame_count - 1 || no_overlap ) {
        if( p_min[f] < min ) min = p_min[f];
        if( p_max[f] > max ) max = p_max[f];
      }

      float frac;
      if( countdown == frame_count - 1 ) frac = frac0;
      else if( countdown == 0 ) frac = frac1;
      else frac = 1.0;

      sum += p_sum[f] * frac;
      sum2 += p_sum2[f] * frac;

      ++f;
    }

    double n = dur / bufferSize;
    double avg = sum / n;
    double stdDev = sqrt( abs((sum2 - (sum*avg) ) / n) );

    minBuffer[i] = min;
    maxBuffer[i] = max;
    minRMS[i] = avg - stdDev;
    maxRMS[i] = avg + stdDev;

    // assure continuity from pixel to pixel
    min = maxBuffer[i];
    max = minBuffer[i];
  }

  return true;
}

short *SoundCacheStream::rawFrames( int ch, quint64 b, quint64 d, bool *interleaved )
{
  if( !_ready || _fpu > 1.0 || ch > channels() || b + d > _cacheSize ) return 0;
  *interleaved = false;
  return _caches[ch].min + b;
}

/*SoundCacheLoader::SoundCacheLoader( SNDFILE *sf, const SF_INFO &info,
                                    int maxFPU, int maxRawFrames )
: _sf( sf ),
  _sfInfo( info ),
  _maxFPU( maxFPU ),
  maxRawFrames( maxRawFrames )
{}*/

void SoundCacheStream::onLoadProgress( int progress )
{
  _loadProgress = progress;
  Q_EMIT( loadProgress(progress) );
}

void SoundCacheStream::onLoadingDone()
{
  // FIXME what if the signal is received just after starting another load?
  _ready = true;
  _loading = false;
  Q_EMIT( loadingDone() );
}

void SoundCacheLoader::load( SNDFILE *sf, const SF_INFO &info )
{
  Q_ASSERT( !isRunning() );
  _sf = sf;
  _info = info;
  start();
}

void SoundCacheLoader::run()
{
  Q_ASSERT( _sf );

  int channels = _cache->channels();
  double fpu = _cache->_fpu;
  int cacheSize = _cache->_cacheSize;
  SoundCache *chanCaches = _cache->_caches;

  int i = 0;
  while( i < cacheSize ) {
    int chunkSize = qMin( 1000, cacheSize - i );

    SoundFileStream sfStream( _sf, _info, i * fpu, chunkSize * fpu );

    int ch;
    for( ch = 0; ch < channels; ++ch ) {
      sfStream.integrate( ch, i * fpu, chunkSize * fpu,
                          chanCaches[ch].min + i, chanCaches[ch].max + i,
                          chanCaches[ch].sum + i, chanCaches[ch].sum2 + i,
                          chunkSize );
    }

    i += chunkSize;

    Q_EMIT( loadProgress( i * 100 / cacheSize ) );
  }

  Q_EMIT( loadingDone() );
}
