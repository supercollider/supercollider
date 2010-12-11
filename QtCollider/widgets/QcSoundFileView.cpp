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

  timeScrollBar = new QScrollBar();

  zoomScrollBar = new QScrollBar();
  zoomScrollBar->setRange( 0, 1000 );

  progressBar = new QProgressBar();
  progressBar->setRange( 0, 100 );

  QGridLayout *l = new QGridLayout();
  l->addWidget( waveform , 0, 0 );
  l->addWidget( timeScrollBar, 0, 1 );
  l->addWidget( zoomScrollBar, 0, 2 );
  l->addWidget( progressBar, 1, 0, 1, 3 );

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
  //timeScrollBar->setSliderPosition( waveform->viewStart() / hScrollMultiplier );
  timeScrollBar->setEnabled( max > 0 );
}

void QcSoundFileView::updateZoomScrollBar()
{
  int max = zoomScrollBar->maximum();
  zoomScrollBar->setSliderPosition( max - (waveform->zoom() * max) );
}

QcWaveform::QcWaveform( QWidget * parent ) : QWidget( parent ),
  sf(0),
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
  delete [] peakCache;
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

  rebuildCache( 128 );

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

void QcWaveform::rebuildCache ( int resolution )
{
  delete [] peakCache;
  peakCache = 0;

  _cacheSize = (sfInfo.frames / resolution);
  // recalculate frames-per-cache after cache size rounded down
  _fpcache = (double) sfInfo.frames / _cacheSize;

  PeakCache *cache = new PeakCache [sfInfo.channels];
  int ch;
  for( ch = 0; ch < sfInfo.channels; ++ch ) {
    cache[ch].min = new short [_cacheSize];
    cache[ch].max = new short [_cacheSize];
  }

  double f_pos = 0;
  int i;

  for( i = 0; i < _cacheSize; ++i ) {

    // there has to be one frame of overlap with the previous pixel's frames,
    // to avoid discontinuity
    int sf_pos = floor(f_pos);

    f_pos += _fpcache;
    if( f_pos > sfInfo.frames ) f_pos = sfInfo.frames;

    int frame_count = ceil(f_pos) - sf_pos;

    short buf[frame_count * sfInfo.channels];
    sf_seek( sf, sf_pos, SEEK_SET );
    int read_count = sf_readf_short( sf, buf, frame_count );

    short *data = buf;
    for( ch = 0; ch < sfInfo.channels; ++ch, ++data ) {
      short min,max;
      integrate( data, data, sfInfo.channels, read_count, min, max );
      cache[ch].min[i] = min;
      cache[ch].max[i] = max;
    }

    Q_EMIT( loadProgress( i * 100 / _cacheSize ) );
    QApplication::processEvents();
  }

  peakCache = cache;

  Q_EMIT( loadingDone() );
}


void QcWaveform::draw( QPixmap *pix, int x, int width, double f_beg, double f_dur )

{
  QPainter p( pix );
  p.fillRect( pix->rect(), QColor( 255,255,255 ) );

  if( !sf || !peakCache ) return;

  // Check for sane situation:
  if( f_beg < 0 || f_beg + f_dur > sfInfo.frames ) return;

  double fpp = f_dur / width;
  float chHeight = (float) pix->height() / sfInfo.channels;

  p.scale( 1.f, chHeight / 65535.f ); 

  if( fpp >= _fpcache ) {

    // use cached peaks
    printf("peaks from cache\n");

    p.translate( (float)x, 32767.f );

    double cachePerPixel = fpp / _fpcache;

    int ch;
    for( ch = 0; ch < sfInfo.channels; ++ch ) {

      double cache_pos = f_beg / _fpcache;
      int i_beg, i_count;

      int i;
      for( i = 0; i < width; ++i ) {
        int i_cache_pos = ceil(cache_pos);
        cache_pos += cachePerPixel;
        // Due to possibility of floating point operation failures. See same issue below.
        if( cache_pos > _cacheSize ) cache_pos = _cacheSize;
        int frame_count = ceil(cache_pos) - i_cache_pos ;

        short *minData = peakCache[ch].min + i_cache_pos;
        short *maxData = peakCache[ch].max + i_cache_pos;

        short min, max;
        integrate( minData, maxData, 1, frame_count, min, max );

        p.drawLine( x + i, min, x + i, max );
      }

      p.translate( 0.f, 65535.f );
    }
  }
  else {

    // we have to read from file

    QPainterPath paths[sfInfo.channels];

    int ch;

    if( fpp > 1.0 ) {

      // we are drawing peaks
      printf("peaks from file\n");

      quint64 buf_offset = floor(f_beg);
      int buf_size = ceil(f_beg + f_dur) - buf_offset;

      short buf[buf_size * sfInfo.channels];
      sf_seek( sf, buf_offset, SEEK_SET);
      int i_read = sf_readf_short( sf, buf, buf_size );

      double f_pos = 0;
      int i;

      for( i = 0; i < width; ++i ) {

        // there has to be one frame of overlap with the previous pixel's frames,
        // to avoid discontinuity
        //quint64 buf_pos = floor(f_pos) - buf_offset;
        int buf_pos = floor(f_pos);

        // increment position
        // slower, but error-prone:
        // f_pos = (double)(i+1) / width() * f_dur + f_beg;
        // the following is a faster variant, but floating point operations are fallible,
        // so we need to make sure we stay within the constraints of f_dur;
        f_pos += fpp;
        if( f_pos > f_dur ) f_pos = f_dur;

        int frame_count = ceil(f_pos) - buf_pos;

        Q_ASSERT( buf_pos + frame_count <= buf_size );

        short *data = buf + (buf_pos * sfInfo.channels);

        for( ch = 0; ch < sfInfo.channels; ++ch, ++data ) {
          short min,max;
          integrate( data, data, sfInfo.channels, frame_count, min, max );

          // QPainter doesn't wanto to draw 0 length lines!
          if( max != min ) {
            p.drawLine( i + x, min + ( (ch + 0.5f ) * 65535.f ),
                        i + x, max + ( (ch + 0.5f ) * 65535.f ) );
          }
          else {
            p.drawPoint( i + x, min + ( (ch + 0.5f ) * 65535.f ) );
          }
        }
      }
    }

    else {

      // we are drawing lines between actual values
      printf("straight data from file\n");

      int i_beg = f_beg;
      int i_count = (int)(f_beg + f_dur) - i_beg + 1 ;

      short buf[ i_count * sfInfo.channels];
      sf_seek( sf, i_beg, SEEK_SET);
      int i_read = sf_readf_short( sf, buf, i_count );

      float ppf = 1 / fpp;
      float dx = ((int) f_beg - f_beg) * ppf;

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

      p.translate( (float) x, 32767.f );

      for( ch = 0; ch < sfInfo.channels; ++ch ) {
        p.drawPath( paths[ch] );
        p.translate( 0.f, 65535.f );
      }
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
