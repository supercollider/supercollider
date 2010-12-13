#include "QcSoundFileView.h"
#include "../QcWidgetFactory.h"

#include <QGridLayout>
#include <QPainter>
#include <QApplication>
#include <QPaintEvent>

#include <climits>
#include <cmath>

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
  pixmap(0),
  dirty(false)
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  memset( &sfInfo, 0, sizeof(SF_INFO) );
}

QcWaveform::~QcWaveform()
{
  delete _cache;
  delete pixmap;
  if( sf ) sf_close( sf );
}

void QcWaveform::load( const QString& filename )
{
  // NOTE we have to delete SoundCacheStream before closing the soundfile, as it might be still
  // loading it
  // TODO: make SoundCacheStream open the soundfile on its own

  delete _cache;
  _cache = 0;

  if( sf ) sf_close( sf );
  memset( &sfInfo, 0, sizeof(SF_INFO) );

  _beg = 0;
  _dur = 0;

  sf = sf_open( filename.toStdString().c_str(), SFM_READ, &sfInfo );
  if( !sf ) {
    printf("Could not open soundfile!\n");
    return;
  }

  _beg = 0;
  _dur = sfInfo.frames;

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
    p.fillRect( r, QColor(50,50,150) );
    r.setRight( r.right() * _cache->loadProgress() / 100 );
    p.fillRect( r, QColor( 0, 0, 100 ) );
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
}

void QcWaveform::rebuildCache ( int maxFPU, int maxRawFrames )
{
}

void QcWaveform::draw( QPixmap *pix, int x, int width, double f_beg, double f_dur )
{
  QPainter p( pix );
  p.fillRect( pix->rect(), QColor( 0, 0, 100 ) );

  if( !sf || !_cache || !_cache->ready() ) return;

  // Check for sane situation:
  if( f_beg < 0 || f_beg + f_dur > sfInfo.frames ) return;

  // Some variables
  double fpp = f_dur / width;
  float chHeight = (float) pix->height() / sfInfo.channels;
  SoundStream *soundStream;
  SoundFileStream sfStream;

  // determine with data source to use according to horiz. zoom (data-display resolution)
  if( fpp > 1.0 ? (fpp < _cache->fpu()) : _cache->fpu() > 1.0 ) {
    printf("use file\n");
    soundStream = &sfStream;
  }
  else {
    printf("use cache\n");
    soundStream = _cache;
  }

  // initial painter setup
  p.setPen( QColor(255,255,0) );
  p.scale( 1.f, chHeight / 65535.f );
  p.translate( (float) x, 32767.f );

  if( fpp > 1.0 ) {

    // display data accumulated into min-max ranges per pixel
    printf("drawing min-max ranges\n");

    if( soundStream == &sfStream ) {
      sfStream.load( sf, sfInfo, f_beg, f_dur );
    }

    short minBuffer[width];
    short maxBuffer[width];

    int ch;
    for( ch = 0; ch < soundStream->channels(); ++ch ) {
      bool ok = soundStream->integrate( ch, f_beg, f_dur, minBuffer, maxBuffer, width );
//    printf("integration ok: %i\n", ok);
      Q_ASSERT( ok );

      int i;
      for( i = 0; i < width; ++i ) {
        short min = minBuffer[i];
        short max = maxBuffer[i];
        if( max != min ) {
          p.drawLine( x + i, min, x + i, max );
        }
        else {
          p.drawPoint( x + i, min );
        }
      }

      p.translate( 0.f, 65535.f );
    }
  }
  else {

    // draw lines between actual values
    printf("drawing lines\n");

    quint64 beg = floor(f_beg);
    int count = ceil( f_beg + f_dur ) - beg;
    bool haveOneMore;
    if( sfInfo.frames - (f_beg + f_dur) >= 1 ) {
      ++count;
      haveOneMore = true;
    }
    else {
      haveOneMore = false;
    }

    if( soundStream == &sfStream ) {
      sfStream.load( sf, sfInfo, beg, count );
    }

    qreal ppf = 1.0 / fpp;
    qreal dx = (beg - f_beg) * ppf;

    p.translate( dx, 0.0 );

    int ch;
    for( ch = 0; ch < sfInfo.channels; ++ch ) {
      bool interleaved = false;
      short *data = soundStream->rawFrames( ch, beg, count, &interleaved );
      //printf("got raw frames ok: %i\n", data != 0 );
      Q_ASSERT( data != 0 );
      int step = interleaved ? soundStream->channels() : 1;

      QPainterPath path;

      if( count ) {
        QPointF pt( 0, (qreal) *data );
        path.moveTo( pt );
      }
      int f; // frame
      for( f = 1; f < count; ++f ) {
        data += step;
        dx = f * ppf;
        QPointF pt( dx, (qreal) *data );
        if( f == 0 ) path.moveTo( pt );
        else path.lineTo( pt );
      }
      if( count && !haveOneMore ) {
        path.lineTo( QPointF( f * ppf, (qreal)*data ) );
      }

      p.drawPath( path );
      p.translate( 0.f, 65535.f );
    }
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
( int ch, double off, double dur, short *minBuffer, short *maxBuffer, int bufferSize )
{
  bool ok = _data != 0
            && ch < channels()
            && ( off >= beginning() )
            && ( off + dur <= beginning() + duration() );
  if( !ok ) return false;

  double fpu = dur / bufferSize;
  double f_pos = off - beginning();

  // TODO consider using the min-max approach in SoundCacheStream here as well, but then
  // protection needs to be implemented in case fpu == 1

  int i;
  for( i = 0; i < bufferSize; ++i ) {

    // there has to be one frame of overlap with the previous unit's frames,
    // to avoid discontinuity

    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-proof:
    // f_pos = (double)(i+1) / width() * dur + off;

    // the following is a faster variant, but floating point operations are fallible,
    // so we need to make sure we stay within the constraints of f_dur;
    f_pos += fpu;
    if( f_pos > dur ) f_pos = dur;

    int frame_count = ceil(f_pos) - data_pos;

    Q_ASSERT( data_pos + frame_count <= _dataSize );

    short *samples = _data + (data_pos * channels()) + ch;
    short min = SHRT_MAX;
    short max = SHRT_MIN;
    int f; // frame
    for( f = 0; f < frame_count; ++f, samples += channels() ){
      short sample = *samples;
      if( sample < min ) min = sample;
      if( sample > max ) max = sample;
    }

    //printf("_dataSize %i, data_pos %i, frame_count %i, min %i, max %i\n",
    //       _dataSize, data_pos, frame_count, min, max );
    minBuffer[i] = min;
    maxBuffer[i] = max;
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

  _caches = new PeakCache [info.channels];
  int ch;
  for( ch = 0; ch < info.channels; ++ch ) {
    _caches[ch].min = new short [_cacheSize];
    _caches[ch].max = new short [_cacheSize];
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

bool SoundCacheStream::integrate
( int ch, double off, double dur, short *minBuffer, short *maxBuffer, int bufferSize )
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
  int i_beg, i_count;

  short min = SHRT_MAX;
  short max = SHRT_MIN;
  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int i_cache_pos = ceil(cache_pos);
    cache_pos += ratio;
    // Due to possibility of floating point operation failures.
    if( cache_pos > _cacheSize ) cache_pos = _cacheSize;
    int frame_count = ceil(cache_pos) - i_cache_pos ;

    short *p_srcMin = _caches[ch].min + i_cache_pos;
    short *p_srcMax = _caches[ch].max + i_cache_pos;
    int f; // frame
    for( f = 0; f < frame_count; ++f, ++p_srcMin, ++p_srcMax ) {
      if( *p_srcMin < min ) min = *p_srcMin;
      if( *p_srcMax > max ) max = *p_srcMax;
    }

    minBuffer[i] = min;
    maxBuffer[i] = max;
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
  PeakCache *chanCaches = _cache->_caches;

  int i = 0;
  while( i < cacheSize ) {
    int chunkSize = qMin( 1000, cacheSize - i );

    SoundFileStream sfStream( _sf, _info, i * fpu, chunkSize * fpu );

    int ch;
    for( ch = 0; ch < channels; ++ch ) {
      sfStream.integrateAll( ch, chanCaches[ch].min + i, chanCaches[ch].max + i, chunkSize );
    }

    i += chunkSize;

    Q_EMIT( loadProgress( i * 100 / cacheSize ) );
  }

  Q_EMIT( loadingDone() );
}
