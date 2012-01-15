/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

QC_DECLARE_QWIDGET_FACTORY(QcSoundFileView);
QC_DECLARE_QWIDGET_FACTORY(QcWaveform);

const int kMaxRawFrames = 300000;
const int kMaxFramesPerCacheUnit = 128;

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

  _rangeBeg(0),
  _rangeDur(0),
  _rangeEnd(0),

  _cache(0),

  _curSel(0),

  _showCursor(false),
  _cursorEditable(true),
  _cursorPos(0),
  _cursorColor(QColor(255,0,0)),

  _showGrid(true),
  _gridResolution(1.0),
  _gridOffset(0.0),
  _gridColor(QColor(100,100,200)),

  _beg(0.0),
  _dur(0.0),
  _yZoom(1.f),

  pixmap(0),
  _peakColor( QColor(242,178,0) ),
  _rmsColor( QColor(255,255,0) ),
  dirty(false),
  _drawWaveform(true)
{
  memset( &sfInfo, 0, sizeof(SF_INFO) );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setAttribute( Qt::WA_OpaquePaintEvent, true );
}

QcWaveform::~QcWaveform()
{
  delete _cache;
  delete pixmap;
  if( sf ) sf_close( sf );
}

void QcWaveform::load( const QString& filename )
{
  qcDebugMsg( 1, "QcWaveform::load()" );

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );

  SNDFILE *new_sf = sf_open( filename.toStdString().c_str(), SFM_READ, &new_info );

  if( !new_sf ) {
    qcErrorMsg("Could not open soundfile.");
    return;
  }

  doLoad( new_sf, new_info, 0, new_info.frames );
}

void QcWaveform::load( const QString& filename, int beg, int dur )
{
  qcDebugMsg( 1, "QcWaveform::load( beg, dur )" );

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );

  SNDFILE *new_sf = sf_open( filename.toStdString().c_str(), SFM_READ, &new_info );

  if( !new_sf ) {
    qcErrorMsg("Could not open soundfile.");
    return;
  }

  doLoad( new_sf, new_info, beg, dur );
}

void QcWaveform::doLoad( SNDFILE *new_sf, const SF_INFO &new_info, sf_count_t beg, sf_count_t dur )
{
  // set up soundfile to scale data in range [-1,1] to int range
  // when reading floating point data as int
  sf_command( new_sf, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE );

  // check beginning and duration validity

  if( beg < 0 || dur < 1 || beg + dur > new_info.frames ) {
    qcErrorMsg("Invalid beginning and/or duration.");
    sf_close( new_sf );
    return;
  }

  // cleanup previous state

  // NOTE we have to delete SoundCacheStream before closing the soundfile, as it might be still
  // loading it
  // TODO: should SoundCacheStream open the soundfile on its own?

  delete _cache;
  if( sf ) sf_close( sf );

  sf = new_sf;
  sfInfo = new_info;
  _beg = _rangeBeg = beg;
  _dur = _rangeDur = dur;
  _rangeEnd = _rangeBeg + _rangeDur;

  updateFPP();

  _cache = new SoundCacheStream();
  connect( _cache, SIGNAL(loadProgress(int)),
           this, SIGNAL(loadProgress(int)) );
  connect( _cache, SIGNAL(loadProgress(int)),
           this, SLOT(update()) );
  connect( _cache, SIGNAL(loadingDone()), this, SIGNAL(loadingDone()) );
  connect( _cache, SIGNAL(loadingDone()), this, SLOT(redraw()) );

  _cache->load( sf, sfInfo, beg, dur, kMaxFramesPerCacheUnit, kMaxRawFrames );

  redraw();
}

float QcWaveform::loadProgress()
{
  return _cache ? _cache->loadProgress() : 1.f;
}

float QcWaveform::zoom()
{
  // NOTE We have limited _rangeDur to 1 minimum.
  return _dur / _rangeDur;
}

float QcWaveform::xZoom()
{
  return ( sfInfo.samplerate ? _dur / sfInfo.samplerate : 0 );
}

float QcWaveform::yZoom()
{
  return _yZoom;
}

VariantList QcWaveform::selections() const
{
  VariantList slist;
  for( int i = 0; i < 64; ++i ) {
    slist.data << QVariant::fromValue<VariantList>( selection(i) );
  }
  return slist;
}

void QcWaveform::setCurrentSelection( int i ) {
  if( i < 0 || i > 63 ) return;
  _curSel = i;
  update();
}

VariantList QcWaveform::selection( int i ) const
{
  VariantList l;
  if( i < 0 || i > 63 ) return l;
  const Selection &s = _selections[i];
  l.data << QVariant(static_cast<int>(s.start - _rangeBeg));
  l.data << QVariant(static_cast<int>(s.size));
  return l;
}

void QcWaveform::setSelection( int i, sf_count_t a, sf_count_t b )
{
  if( i < 0 || i > 63 ) return;
  Selection& s = _selections[i];
  s.start = qMin( a, b );
  s.size = qMax( a, b ) - s.start;
  update();
}

void QcWaveform::setSelection( int i, VariantList l )
{
  if( l.data.count() < 2 ) return;
  sf_count_t start = l.data[0].toInt() + _rangeBeg;
  sf_count_t end = start + l.data[1].toInt();
  setSelection( i, start, end );
}

void QcWaveform::setSelectionStart( int i, sf_count_t frame )
{
  if( i < 0 || i > 63 ) return;
  Selection& s = _selections[i];
  sf_count_t frame2 = s.start + s.size;
  s.start = qMin( frame, frame2 );
  s.size = qMax( frame, frame2 ) - s.start;
  update();
}

void QcWaveform::setSelectionEnd( int i, sf_count_t frame )
{
  if( i < 0 || i > 63 ) return;
  Selection& s = _selections[i];
  sf_count_t frame2 = s.start;
  s.start = qMin( frame, frame2 );
  s.size = qMax( frame, frame2 ) - s.start;
  update();
}

void QcWaveform::setSelectionEditable( int i, bool editable )
{
  if( i < 0 || i > 63 ) return;
  _selections[i].editable = editable;
  update();
}

void QcWaveform::setSelectionColor( int i, const QColor &c )
{
  if( i < 0 || i > 63 ) return;
  _selections[i].color = c;
  update();
}

VariantList QcWaveform::waveColors() const
{
  VariantList clist;
  Q_FOREACH( QColor clr, _waveColors )
    clist.data << QVariant(clr);
  return clist;
}

void QcWaveform::setWaveColors( const VariantList &list )
{
  _waveColors.clear();
  Q_FOREACH( QVariant var, list.data )
    _waveColors << var.value<QColor>();
  redraw();
}

void QcWaveform::zoomTo( double z )
{
  z = qMax( 0.0, qMin( 1.0, z ) );

  _dur = qMax( _rangeDur * z, 1.0 );

  //printf("dur: %Li view: %Li\n", sfInfo.frames, _dur);
  if( _beg + _dur > _rangeEnd ) _beg = _rangeEnd - _dur;

  updateFPP();
  redraw();
}

void QcWaveform::zoomBy( double factor )
{
  zoomTo( zoom() * factor );
}

void QcWaveform::zoomAllOut()
{
  _beg = _rangeBeg;
  _dur = _rangeDur;
  updateFPP();
  redraw();
}

void QcWaveform::scrollTo( double startFrame )
{
  _beg = qBound( (double)_rangeBeg, startFrame, _rangeEnd - _dur );
  redraw();
}

void QcWaveform::scrollBy( double f )
{
  scrollTo( _beg + f );
}

float QcWaveform::scrollPos()
{
  double scrollRange = _rangeDur - _dur;
  return scrollRange > 0.0 ? (_beg - _rangeBeg) / scrollRange : 0.f;
}
void QcWaveform::setScrollPos( double fraction )
{
  scrollTo( fraction * (_rangeDur - _dur) + _rangeBeg );
}

void QcWaveform::scrollToStart()
{
  scrollTo( _rangeBeg );
}

void QcWaveform::scrollToEnd()
{
  scrollTo( _rangeEnd - _dur );
}

void QcWaveform::setXZoom( double seconds )
{
  // NOTE We have limited _rangeDur to 1 minimum.
  double frac = seconds * sfInfo.samplerate / _rangeDur;
  zoomTo( frac );
}

void QcWaveform::setYZoom( double factor )
{
  _yZoom = factor;
  redraw();
}

void QcWaveform::zoomSelection( int i )
{
  if( i < 0 || i > 63 ) return;

  Selection &s = _selections[i];

  if( s.start >= _rangeEnd || s.size < 1 || s.start + s.size <= _rangeBeg )
    return;

  _beg = qMax( s.start, _rangeBeg );
  double end = qMin( s.start + s.size, _rangeEnd );
  _dur = end - _beg;

  // clear the selection
  s.size = 0;

  updateFPP();
  redraw();
}

void QcWaveform::resizeEvent( QResizeEvent * )
{
  delete pixmap;
  pixmap = new QPixmap( size() );
  updateFPP();
  redraw();
}

void QcWaveform::paintEvent( QPaintEvent *ev )
{
  Q_ASSERT( pixmap != 0 );

  QPainter p( this );

  // if loading draw progress
  if( _cache && _cache->loading() ) {
    QRect r( rect() );
    p.fillRect( r, QColor(100,100,100) );
    r.setRight( r.right() * _cache->loadProgress() / 100 );
    p.fillRect( r, QColor( 0, 0, 0 ) );
    p.setPen( QColor(255,255,255) );
    QTextOption opt;
    opt.setAlignment( Qt::AlignCenter );
    p.drawText( rect(), "loading...", opt );
    return;
  }

  // draw waveform on pixmap

  if( _drawWaveform && dirty ) {
    draw( pixmap, 0, width(), _beg, _dur );
    dirty = false;
  }

  // clear background

  p.fillRect( rect(), QColor( 0, 0, 0 ) );

  // draw selections

  p.save();

  p.scale( (double) width() / _dur, 1.0 );
  p.translate( _beg * -1.0, 0.0 );
  p.setPen( Qt::NoPen );

  int i;
  for( i = 0; i < 64; ++i ) {
    const Selection &s = _selections[i];
    if( s.size > 0 ) {
      QRectF r ( s.start, 0, s.size, height() );
      p.setBrush( s.color );
      p.drawRect( r );
    }
  }

  p.restore();

  // draw time grid

  if( _showGrid && sfInfo.samplerate > 0 && _gridResolution > 0.f ) {
    p.save();

    double durSecs = (double) _dur / sfInfo.samplerate;
    double begSecs = (double) _beg / sfInfo.samplerate;

    p.scale( width() / durSecs, 1.0 );
    p.setPen( _gridColor );

    double offset = _gridOffset - begSecs;
    offset -= ( floor( offset / _gridResolution ) * _gridResolution );
    while( offset <  durSecs ) {
      p.drawLine( QLineF( offset, 0.0, offset, height() ) );
      offset += _gridResolution;
    }

    p.restore();
  }

  // paste the waveform pixmap on screen

  if( _drawWaveform )
    p.drawPixmap( ev->rect(), *pixmap, ev->rect() );

  // draw cursor

  if( _showCursor && _cursorPos >= _beg && _cursorPos < _beg + _dur ) {
    double cursorX = (_cursorPos - _beg) / _fpp;
    p.setPen( _cursorColor );
    p.drawLine( QLineF( cursorX, 0, cursorX, height() ) );
  }
}

void QcWaveform::mousePressEvent( QMouseEvent *ev )
{
  _dragAction = NoDragAction;
  _dragPoint = ev->pos();
  _dragFrame = ev->pos().x() * _fpp + _beg;

  Qt::KeyboardModifiers mods = ev->modifiers();
  Qt::MouseButton btn = ev->button();
#ifdef Q_OS_MAC
  Qt::KeyboardModifier CTRL = Qt::MetaModifier;
#else
  Qt::KeyboardModifier CTRL = Qt::ControlModifier;
#endif

  if( btn == Qt::LeftButton ) {

    if( (mods & Qt::ShiftModifier) && ( mods & CTRL ) ) {
      _dragFrame = _selections[_curSel].start;
      _dragAction = MoveSelection;
    }
    else if( mods & Qt::ShiftModifier ) {
      _dragAction = Select;
      const Selection &s = _selections[_curSel];
      if( _dragFrame < s.start + (s.size*0.5) ) {
        setSelectionStart( _curSel, _dragFrame );
        _dragFrame = s.start + s.size;
      }
      else {
        setSelectionEnd( _curSel, _dragFrame );
        _dragFrame = s.start;
      }
      Q_EMIT( action() );
    }
    else {
      if( !(mods & CTRL) ) {
        _dragAction = Select;
        _selections[_curSel].start = _dragFrame;
        _selections[_curSel].size = 0;
        update();
        Q_EMIT( action() );
      }
      if( _cursorEditable ) {
        _cursorPos = _dragFrame;
        if( mods & CTRL )  _dragAction = MoveCursor;
        update();
        Q_EMIT( metaAction() );
      }
    }

  }
  else if( btn == Qt::RightButton ) {

    if( mods & Qt::ShiftModifier ) {
      _dragAction = Zoom;
      _dragData = zoom();
      _dragData2 = ev->pos().x() * _fpp + _beg;
    }
    else {
      _dragAction = Scroll;
      _dragData = _beg;
    }

  }
}

void QcWaveform::mouseDoubleClickEvent ( QMouseEvent * )
{
  setSelection( _curSel, _rangeBeg, _rangeEnd );
  Q_EMIT( action() );
}

void QcWaveform::mouseMoveEvent( QMouseEvent *ev )
{
  if( _dragAction == Scroll ) {
    double dpos = _dragPoint.x() - ev->pos().x();
    scrollTo( dpos * _fpp + _dragData );
  }
  else if( _dragAction == Zoom ) {
    double factor = pow( 2, (ev->pos().y() - _dragPoint.y()) * 0.008 );
    double zoom_0 = _dragData;
    zoomTo( zoom_0 * factor );
    scrollTo( _dragData2 - (_dragPoint.x() * _fpp) );
  }
  else if( _dragAction == Select ) {
    sf_count_t frame = qBound( 0, ev->pos().x(), width() ) * _fpp + _beg;
    setSelection( _curSel, _dragFrame, frame );
    update();
    Q_EMIT( action() );
  }
  else if( _dragAction == MoveSelection ) {
    double dpos = ev->pos().x() - _dragPoint.x();
    Selection &s = _selections[_curSel];
    s.start = _dragFrame + (dpos * _fpp);
    s.start = qBound( _rangeBeg, s.start, _rangeEnd - s.size );
    update();
    Q_EMIT( action() );
  }
  else if( _dragAction == MoveCursor ) {
    _cursorPos = qBound( 0, ev->pos().x(), width() ) * _fpp + _beg;
    update();
    Q_EMIT( metaAction() );
  }
}

void QcWaveform::rebuildCache ( int maxFPU, int maxRawFrames )
{
}

void QcWaveform::draw( QPixmap *pix, int x, int width, double f_beg, double f_dur )
{
  // FIXME anomaly: when _fpp reaching 1.0 rms can go outside min-max!

  pix->fill( QColor( 0, 0, 0, 0 ) );

  QPainter p( pix );

  if( !sf || !_cache || !_cache->ready() ) return;

  // check for sane situation:
  if( f_beg < _rangeBeg || f_beg + f_dur > _rangeEnd ) return;

  // data indexes
  sf_count_t i_beg = floor(f_beg); // data beginning;
  sf_count_t i_count = ceil( f_beg + f_dur ) - i_beg; // data count;
  bool haveOneMore;
  if( i_beg + i_count < _rangeEnd ) {
    ++i_count;
    haveOneMore = true;
  }
  else {
    haveOneMore = false;
  }

  // data source - choose according to horiz. zoom (data-display resolution)
  SoundStream *soundStream;
  SoundFileStream sfStream;

  if( _fpp > 1.0 ? (_fpp < _cache->fpu()) : _cache->fpu() > 1.0 ) {
    qcDebugMsg( 1, QString("use file") );
    soundStream = &sfStream;
    sfStream.load( sf, sfInfo, i_beg, i_count );
  }
  else {
    qcDebugMsg( 1, QString("use cache") );
    soundStream = _cache;
  }

  // geometry
  float spacing = pix->height() * 0.15f / (sfInfo.channels + 1);
  float chHeight = pix->height() * 0.85f / (float) sfInfo.channels;
  float yScale = -chHeight / 65535.f * _yZoom;
  //spacing /= yscale;

  // initial painter setup
  QPen minMaxPen;
  QPen rmsPen;

  float halfChH = chHeight * 0.5;
  p.translate( 0.f, halfChH + spacing );

  int waveColorN = _waveColors.count();
  int ch;
  for( ch = 0; ch < soundStream->channels(); ++ch ) {

    if( ch < waveColorN && _waveColors[ch].isValid() ) {
      QColor clr( _waveColors[ch] );
      rmsPen.setColor( clr );
      minMaxPen.setColor( clr.darker( 140 ) );
    }
    else {
      rmsPen.setColor( _rmsColor );
      minMaxPen.setColor( _peakColor );
    }

    // draw center line
    p.setPen( QColor(90,90,90) );
    p.drawLine( x, 0, x + width, 0 );

    // draw bounding lines
    p.setPen( QColor(100,100,100) );
    p.drawLine( x, halfChH, x+width, halfChH );
    p.drawLine( x, -halfChH, x+width, -halfChH );

    p.save();

    p.setClipping(true);
    p.setClipRect( x, -halfChH, x+width, chHeight );
    p.scale( 1.f, yScale );

    if( _fpp > 1.0 ) {

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

      qreal ppf = 1.0 / _fpp;
      qreal dx = (i_beg - f_beg) * ppf;

      bool interleaved = false;
      short *data = soundStream->rawFrames( ch, i_beg, i_count, &interleaved );
      //printf("got raw frames ok: %i\n", data != 0 );
      Q_ASSERT( data != 0 );
      int step = interleaved ? soundStream->channels() : 1;

      QPainterPath path;

      if( i_count ) {
        QPointF pt( dx, (qreal) *data );
        path.moveTo( pt );
      }
      int f; // frame
      for( f = 1; f < i_count; ++f ) {
        data += step;
        QPointF pt( f * ppf + dx, (qreal) *data );
        path.lineTo( pt );
      }
      if( i_count && !haveOneMore ) {
        path.lineTo( QPointF( f * ppf + dx, (qreal)*data ) );
      }

      p.setPen( rmsPen );
      p.drawPath( path );
    }

    p.restore();

    p.translate( 0.f, chHeight + spacing );
  }
}

SoundFileStream::SoundFileStream() : _data(0), _dataSize(0), _dataOffset(0)
{}

SoundFileStream::SoundFileStream( SNDFILE *sf, const SF_INFO &info, sf_count_t b, sf_count_t d )
: _data(0)
{
  load( sf, info, b, d );
}

SoundFileStream::~SoundFileStream()
{
  delete[] _data;
}

void SoundFileStream::load( SNDFILE *sf, const SF_INFO &info, sf_count_t beg, sf_count_t dur )
{
  delete[] _data;

  _dataOffset = beg;
  _dataSize = dur;

  _data = new short [_dataSize * info.channels];
  sf_seek( sf, _dataOffset, SEEK_SET);
  _dataSize = sf_readf_short( sf, _data, _dataSize );

  _ch = info.channels;
  _beg = _dataOffset;
  _dur = _dataSize;
}

bool SoundFileStream::integrate
( int ch, double f_beg, double f_dur,
  short *minBuffer, short *maxBuffer, float *sumBuf, float *sum2Buf, int bufferSize )
{
  bool ok = _data != 0
            && ch < channels()
            && ( f_beg >= beginning() )
            && ( f_beg + f_dur <= beginning() + duration() );
  if( !ok ) return false;

  double fpu = f_dur / bufferSize;
  double f_pos = f_beg - _dataOffset;
  double f_pos_max = _dataSize;

  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-proof:
    // f_pos = (double)(i+1) / width() * f_dur + f_beg;

    // the following is a faster variant, but floating point operations are fallible,
    // so we need to make sure we stay within the constraints of f_dur;
    double f_pos1 = f_pos + fpu;
    if( f_pos1 > f_pos_max ) f_pos1 = f_pos_max;

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
( int ch, double f_beg, double f_dur,
  short *minBuffer, short *maxBuffer, short *minRMS, short *maxRMS, int bufferSize )
{
  bool ok = _data != 0
            && ch < channels()
            && ( f_beg >= beginning() )
            && ( f_beg + f_dur <= beginning() + duration() );
  if( !ok ) return false;

  double fpu = f_dur / bufferSize;
  double f_pos = f_beg - _dataOffset;
  double f_pos_max = _dataSize;

  short min = SHRT_MAX;
  short max = SHRT_MIN;
  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int data_pos = floor(f_pos);

    // increment position

    // slower, but error-proof:
    // f_pos = (double)(i+1) / width() * f_dur + f_beg;

    // the following is a faster variant, but floating point operations are fallible,
    // so we need to make sure we stay within the constraints of f_dur;
    double f_pos1 = f_pos + fpu;
    if( f_pos1 > f_pos_max ) f_pos1 = f_pos_max;

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

short *SoundFileStream::rawFrames( int ch, sf_count_t b, sf_count_t d, bool *interleaved )
{
  if( ch > channels() || b < _dataOffset || b + d > _dataOffset + _dataSize ) return 0;
  *interleaved = true;
  sf_count_t offset = (b - _dataOffset) * channels() + ch;
  return ( _data + offset );
}


SoundCacheStream::SoundCacheStream()
: SoundStream ( 0, 0.0, 0.0 ),
  _caches(0),
  _fpu(0.0),
  _dataOffset(0),
  _dataSize(0),
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

void SoundCacheStream::load( SNDFILE *sf, const SF_INFO &info, sf_count_t beg, sf_count_t dur,
                             int maxFramesPerUnit, int maxRawFrames )
{
  Q_ASSERT( maxRawFrames > 0 && maxFramesPerUnit > 0 );

  _ready = false;
  _loadProgress = 0;

  if( _loader->isRunning() ) {
    _loader->terminate();
    _loader->wait();
  }
  delete [] _caches;

  _ch = info.channels;
  _beg = _dataOffset = beg;
  _dur = dur;

  // adjust data size for data amount limit
  if( _dur <= maxRawFrames ) {
     // ok, not crossing the limit
    _dataSize = _dur;
    _fpu = 1.0;
  }
  else {
    _dataSize = maxRawFrames;
    _fpu = (double) _dur / _dataSize;
    // re-adjust for data resolution limit
    if( _fpu > maxFramesPerUnit ) {
      _dataSize = (double) _dur / maxFramesPerUnit;
      _fpu = (double) _dur / _dataSize;
    }
  }

  _caches = new SoundCache [info.channels];
  int ch;
  for( ch = 0; ch < info.channels; ++ch ) {
    _caches[ch].min = new short [_dataSize];
    _caches[ch].max = new short [_dataSize];
    _caches[ch].sum = new float [_dataSize];
    _caches[ch].sum2 = new float [_dataSize];
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
( int ch, double f_beg, double f_dur,
  short *minBuffer, short *maxBuffer, short *minRMS, short *maxRMS, int bufferSize )
{
  bool ok = _ready
            && ch < channels()
            && ( f_beg >= beginning() )
            && ( f_beg + f_dur <= beginning() + duration() )
            && bufferSize <= f_dur * _fpu;
  if( !ok ) return false;

  double ratio = f_dur / _fpu / bufferSize;
  double cache_pos = (f_beg - _dataOffset) / _fpu ;

  short min = SHRT_MAX;
  short max = SHRT_MIN;
  int i;
  for( i = 0; i < bufferSize; ++i ) {
    int f = floor(cache_pos); // first frame
    bool no_overlap = f == ceil(cache_pos);
    float frac0 = f + 1.f - cache_pos;

    cache_pos += ratio;
    // Due to possibility of floating point operation failures.
    if( cache_pos > _dataSize ) cache_pos = _dataSize;
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

    double n = f_dur / bufferSize;
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

short *SoundCacheStream::rawFrames( int ch, sf_count_t b, sf_count_t d, bool *interleaved )
{
  if( !_ready || _fpu != 1.0 || ch > channels() ||
      b < _dataOffset || b + d > _dataOffset + _dataSize )
    return 0;

  *interleaved = false;
  return _caches[ch].min + b - _dataOffset;
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
  int size = _cache->_dataSize;
  double offset = _cache->_dataOffset;
  SoundCache *chanCaches = _cache->_caches;

  int i = 0;
  while( i < size ) {
    int chunkSize = qMin( 1000, size - i );

    double beg = i * fpu + offset;
    double dur = chunkSize * fpu;

    sf_count_t i_beg = floor(beg);
    sf_count_t i_dur = ceil(beg+dur) - i_beg;

    SoundFileStream sfStream( _sf, _info, i_beg, i_dur );

    int ch;
    for( ch = 0; ch < channels; ++ch ) {
      sfStream.integrate( ch, beg, dur,
                          chanCaches[ch].min + i, chanCaches[ch].max + i,
                          chanCaches[ch].sum + i, chanCaches[ch].sum2 + i,
                          chunkSize );
    }

    i += chunkSize;

    Q_EMIT( loadProgress( i * 100 / size ) );
  }

  Q_EMIT( loadingDone() );
}
