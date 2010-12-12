#include "QcSoundFileView.h"
#include "../QcWidgetFactory.h"

#include <QGridLayout>
#include <QPainter>
#include <QApplication>
#include <QPaintEvent>

#include <climits>
#include <cmath>

class SoundFileViewFactory : public QcWidgetFactory<QcSoundFileView>
{
  void initialize( QcSoundFileView *sfv, QList<QVariant> & args ) {
    if( args.size() < 1 ) return;
    QString filename = args[0].toString();
    if( filename.isEmpty() ) return;
    sfv->load( filename );
  };
};

static SoundFileViewFactory factory;

QcSoundFileView::QcSoundFileView() :
  hScrollMultiplier( 0.f )
{
  waveform = new QcWaveform();

  timeScrollBar = new QScrollBar( Qt::Horizontal );

  zoomScrollBar = new QSlider();
  zoomScrollBar->setRange( 0, 1000 );

  progressBar = new QProgressBar();
  progressBar->setRange( 0, 100 );

  QGridLayout *l = new QGridLayout();
  l->addWidget( waveform , 0, 0 );
  l->addWidget( zoomScrollBar, 0, 1 );
  l->addWidget( timeScrollBar, 1, 0, 1, 2 );
  l->addWidget( progressBar, 1, 0, 1, 2 );

  progressBar->hide();

  setLayout( l );

  connect( timeScrollBar, SIGNAL(valueChanged(int)), this, SLOT(setBeginning(int)) );
  connect( zoomScrollBar, SIGNAL(valueChanged(int)), this, SLOT(setZoom(int)) );
  connect( waveform, SIGNAL(loadProgress(int)), progressBar, SLOT(show()) );
  connect( waveform, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)) );
  connect( waveform, SIGNAL(loadingDone()), progressBar, SLOT(hide()) );
  connect( waveform, SIGNAL(loadingDone()), this, SLOT(updateTimeScrollBar()) );
  connect( waveform, SIGNAL(loadingDone()), this, SLOT(updateZoomScrollBar()) );

  updateTimeScrollBar();
}



void QcSoundFileView::load( const QString& filename )
{
  waveform->load( filename );
}


void QcSoundFileView::setBeginning( int value )
{
  waveform->setViewStart( value * hScrollMultiplier );
}

void QcSoundFileView::setZoom( int z )
{
  waveform->setZoom( (zoomScrollBar->maximum() - z) / 1000.f );
  updateTimeScrollBar();
}

void QcSoundFileView::updateTimeScrollBar()
{
  // FIXME:
  // view start moves weirdly when changing zoom. setSliderPosition() actually calls Waveform
  // again, and the resulting position is not the same as Waveform's original.

  quint64 max;
  max = waveform->duration() - waveform->viewWidth();
  if( max > 1000 ) {
    hScrollMultiplier = max / 1000.0;
    max = 1000;
  } else {
    hScrollMultiplier = 1.0;
  }
  timeScrollBar->setMaximum( max );
  //printf("before: %Li\n", waveform->viewStart() );
  timeScrollBar->blockSignals( true );
  timeScrollBar->setSliderPosition( waveform->viewStart() / hScrollMultiplier );
  timeScrollBar->blockSignals( false );
  //printf("after: %Li\n", waveform->viewStart() );
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
  peakCache(0),
  _beg(0.0),
  _dur(0.0),
  pixmap(0),
  dirty(false)
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

QcWaveform::~QcWaveform()
{
  if( sf ) sf_close( sf );
  delete pixmap;
}

void QcWaveform::load( const QString& filename )
{
  if( sf ) sf_close( sf );

  sfInfo.format = 0;

  sf = sf_open( filename.toStdString().c_str(), SFM_READ, &sfInfo );
  if( !sf ) {
    printf("Could not open soundfile!\n");
    _beg = 0;
    _dur = 0;
    return;
  }

  _beg = 0;
  _dur = sfInfo.frames;

  rebuildCache( 128, 10000 );

  redraw();
}

float QcWaveform::zoom()
{
  return (float) (_dur - 1) / (sfInfo.frames - 1);
}

void QcWaveform::setZoom( float z )
{
  if( !sf ) return;

  z = qMax( 0.f, qMin( 1.f, z ) );
  // take it to the power of 4, for a quasi-logarithmic curve
  z *= z;
  z *= z;
  _dur = (sfInfo.frames - 1) * z + 1;
  //printf("dur: %Li view: %Li\n", sfInfo.frames, _dur);
  if( _beg + _dur > sfInfo.frames ) _beg = sfInfo.frames - _dur;

  redraw();
}

void QcWaveform::setViewStart( quint64 startFrame )
{
  _beg = qMax( (quint64) 0, qMin( (quint64) sfInfo.frames - _dur - 1, startFrame ) );
  redraw();
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

  if( dirty ) {
    draw( pixmap, 0, width(), _beg, _dur );
    dirty = false;
  }

  QPainter p( this );
  p.drawPixmap( ev->rect(), *pixmap, ev->rect() );
}

void QcWaveform::rebuildCache ( int maxFPU, int maxRawFrames )
{
  delete _cache;
  _cache = 0;
  peakCache = 0;

  SoundCacheStream *cache = new SoundCacheStream( sf, sfInfo, maxFPU, maxRawFrames );

  _cache = cache;

  peakCache = cache->_caches;
  _fpcache = cache->_fpu;
  _cacheSize = cache->_cacheSize;

  Q_EMIT( loadingDone() );
}

void QcWaveform::draw( QPixmap *pix, int x, int width, double f_beg, double f_dur )

{
  QPainter p( pix );
  p.fillRect( pix->rect(), QColor( 255,255,255 ) );

  if( !sf || !_cache || !peakCache ) return;

  // Check for sane situation:
  if( f_beg < 0 || f_beg + f_dur > sfInfo.frames ) return;

  double fpp = f_dur / width;
  float chHeight = (float) pix->height() / sfInfo.channels;
  SoundFileStream sfStream;

  p.scale( 1.f, chHeight / 65535.f );
  p.translate( (float) x, 32767.f );

  if( fpp > 1.0 ) {
    SoundStream *soundStream;

    if( fpp >= _cache->fpu() ) {
      soundStream = _cache;
    }
    else {
      sfStream.load( sf, sfInfo, f_beg, f_dur );
      soundStream = &sfStream;
    }
    
    short minBuffer[width];
    short maxBuffer[width];

    int ch;
    for( ch = 0; ch < soundStream->channels(); ++ch ) {
      bool ok = soundStream->integrate( ch, f_beg, f_dur, minBuffer, maxBuffer, width );
//    printf("integrated ok: %i\n", ok);
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
    // we are drawing lines between actual values
    //printf("straight data from file\n");

    QPainterPath paths[sfInfo.channels];

    int i_beg = f_beg;
    int i_count = (int)(f_beg + f_dur) - i_beg + 1 ;

    short buf[ i_count * sfInfo.channels];
    sf_seek( sf, i_beg, SEEK_SET);
    int i_read = sf_readf_short( sf, buf, i_count );

    float ppf = 1 / fpp;
    float dx = ((int) f_beg - f_beg) * ppf;

    int ch;
    for( ch = 0; ch < sfInfo.channels; ++ch ) {
      int f;
      for( f = 0; f < i_read; ++f ) {
        short s = buf[ f*sfInfo.channels + ch ];
        float x = f * ppf + dx;
        QPointF pt( x, (float) s );
        if( f == 0 ) paths[ch].moveTo( pt );
        else paths[ch].lineTo( pt );
      }
    }

    for( ch = 0; ch < sfInfo.channels; ++ch ) {
      p.drawPath( paths[ch] );
      p.translate( 0.f, 65535.f );
    }
  }

}


inline void QcWaveform::integrate
  ( short *p_srcMin, short *p_srcMax, int stepSize, int steps, short &min, short &max )
{
  min = SHRT_MAX;
  max = SHRT_MIN;
  int i;
  int count = steps * stepSize;
  for( i = 0; i < count; i += stepSize ){
    short srcMin = p_srcMin[i];
    short srcMax = p_srcMax[i];
    if( srcMin < min ) min = srcMin;
    if( srcMax > max ) max = srcMax;
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

  ch = info.channels;
  beg = b;
  dur = d;

  _dataOffset = floor(b);
  _dataSize = ceil(b + d) - _dataOffset;

  _data = new short [_dataSize * channels()];
  sf_seek( sf, _dataOffset, SEEK_SET);
  _dataSize = sf_readf_short( sf, _data, _dataSize );
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

  int i;
  for( i = 0; i < bufferSize; ++i ) {

    // there has to be one frame of overlap with the previous unit's frames,
    // to avoid discontinuity

    //quint64 data_pos = floor(f_pos) - _dataOffset;
    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-prone:
    // f_pos = (double)(i+1) / width() * f_dur + f_beg;

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

SoundCacheStream::SoundCacheStream
( SNDFILE *sf, const SF_INFO &info, int maxFramesPerUnit, int maxRawFrames )
: SoundStream ( info.channels, 0.0, (double) info.frames ),
  _caches( 0 )
{
  /*if( sfInfo.frames <= maxRawFrames ) {
    _cacheSize = sfInfo.frames;
    _fpu = 1.0;
  }
  else {
    _cacheSize = maxRawFrames;
    _fpu = (double) sfInfo.frames / _cacheSize;
    if( _fpu > maxFramesPerUnit ) {
      _cacheSize = sfInfo.frames / maxFramesPerUnit;
      _fpu = (double) sfInfo.frames / _cacheSize;
    }
  }*/

  _cacheSize = (info.frames / maxFramesPerUnit);
  // recalculate frames-per-cache after cache size rounded down
  _fpu = (double) info.frames / _cacheSize;


  _caches = new PeakCache [info.channels];
  int ch;
  for( ch = 0; ch < info.channels; ++ch ) {
    _caches[ch].min = new short [_cacheSize];
    _caches[ch].max = new short [_cacheSize];
  }


  int i = 0;
  while( i < _cacheSize ) {
    int chunkSize = qMin( 100, _cacheSize - i );

    SoundFileStream sfStream( sf, info, i * _fpu, chunkSize * _fpu );

    for( ch = 0; ch < channels(); ++ch ) {
      sfStream.integrateAll( ch, _caches[ch].min + i, _caches[ch].max + i, chunkSize );
    }

    i += chunkSize;

    Q_EMIT( loadProgress( i * 100 / _cacheSize ) );
    QApplication::processEvents();
  }

  Q_EMIT( loadingDone() );
}

SoundCacheStream::~SoundCacheStream()
{
  delete [] _caches;
}

bool SoundCacheStream::integrate
( int ch, double off, double dur, short *minBuffer, short *maxBuffer, int bufferSize )
{
  // we assume that beginning() == 0

  bool ok = ch < channels()
            && ( off >= beginning() )
            && ( off + dur <= beginning() + duration() )
            && bufferSize <= dur * _fpu;
  if( !ok ) return false;

  double ratio = dur / _fpu / bufferSize;
  double cache_pos = off / _fpu;
  int i_beg, i_count;

  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int i_cache_pos = ceil(cache_pos);
    cache_pos += ratio;
    // Due to possibility of floating point operation failures.
    if( cache_pos > _cacheSize ) cache_pos = _cacheSize;
    int frame_count = ceil(cache_pos) - i_cache_pos ;

    short min = SHRT_MAX;
    short max = SHRT_MIN;
    short *p_srcMin = _caches[ch].min + i_cache_pos;
    short *p_srcMax = _caches[ch].max + i_cache_pos;
    int f; // frame
    for( f = 0; f < frame_count; ++f, ++p_srcMin, ++p_srcMax ) {
      if( *p_srcMin < min ) min = *p_srcMin;
      if( *p_srcMax > max ) max = *p_srcMax;
    }

    minBuffer[i] = min;
    maxBuffer[i] = max;
  }

  return true;
}
