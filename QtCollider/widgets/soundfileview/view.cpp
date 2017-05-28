/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
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

#include "view.hpp"
#include "../../QcWidgetFactory.h"

#include <QGridLayout>
#include <QPainter>
#include <QApplication>
#include <QPaintEvent>
#include <QCursor>

#include <climits>
#include <cmath>
#include <cstring>

QC_DECLARE_QWIDGET_FACTORY(QcWaveform);

const int kMaxRawFrames = 300000;
const int kMaxFramesPerCacheUnit = 128;

QcWaveform::QcWaveform( QWidget * parent ) : QWidget( parent ),
  sf(0),

  _rangeBeg(0),
  _rangeDur(0),
  _rangeEnd(0),

  _cache(0),

  _curSel(0),

  _showCursor(false),
  _cursorPos(0),
  _cursorEditable(true),

  _showGrid(true),
  _gridResolution(1.0),
  _gridOffset(0.0),

  _beg(0.0),
  _dur(0.0),
  _yZoom(1.f),

  pixmap(0),
  _bkgColor( QColor(0,0,0) ),
  _peakColor( QColor(242,178,0) ),
  _rmsColor( QColor(255,255,0) ),
  _cursorColor( QColor(255,0,0) ),
  _gridColor( QColor(100,100,200) ),
  dirty(false),
  _drawWaveform(true)
{
  memset( &sfInfo, 0, sizeof(SF_INFO) );

  setFocusPolicy( Qt::StrongFocus );
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
  qcDebugMsg( 1, "QcWaveform::load( filename )" );

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );

  SNDFILE *new_sf = sf_open( filename.toStdString().c_str(), SFM_READ, &new_info );

  if( !new_sf ) {
    qcErrorMsg(QStringLiteral("Could not open soundfile: ") + filename);
    return;
  }

  doLoad( new_sf, new_info, 0, new_info.frames );
}

void QcWaveform::load( const QString& filename, int beg, int dur )
{
  qcDebugMsg( 1, "QcWaveform::load( filename, beg, dur )" );

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );

  SNDFILE *new_sf = sf_open( filename.toStdString().c_str(), SFM_READ, &new_info );

  if( !new_sf ) {
    qcErrorMsg(QStringLiteral("Could not open soundfile: ") + filename);
    return;
  }

  doLoad( new_sf, new_info, beg, dur );
}

void QcWaveform::load( const QVector<double> & data, int offset, int ch, int sr )
{
  qcDebugMsg( 1, "QcWaveform::load( data, offset, channels )" );

  if( ch < 1 ) {
    qcWarningMsg( "QSoundFileView: invalid number of channels!" );
    return;
  }

  int ns = data.count();
  int nf = ns / ch;

  if( nf * ch != ns ) {
    qcWarningMsg( "QSoundFileView: size of data not a multiple of channel count!" );
    return;
  }

  if( offset < 0 || nf - offset < 1 ) {
    qcWarningMsg( "QSoundFileView: invalid range of data!" );
    return;
  }

  delete _cache;
  if( sf ) sf_close( sf );
  sf = 0;

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );
  new_info.channels = ch;
  new_info.samplerate = sr;
  sfInfo = new_info;

  _beg = _rangeBeg = 0;
  _dur = _rangeDur = _rangeEnd = nf - offset;

  updateFPP();

  _cache = new SoundCacheStream();
  connect( _cache, SIGNAL(loadingDone()), this, SIGNAL(loadingDone()) );
  connect( _cache, SIGNAL(loadingDone()), this, SLOT(redraw()) );

  _cache->load( data, _rangeDur, offset, ch );
}

void QcWaveform::allocate ( int frames, int ch, int sr )
{
  if( ch < 1 ) {
    qcWarningMsg( "QSoundFileView: invalid number of channels!" );
    return;
  }

  delete _cache;
  if( sf ) sf_close( sf );
  sf = 0;

  SF_INFO new_info;
  memset( &new_info, 0, sizeof(SF_INFO) );
  new_info.channels = ch;
  new_info.samplerate = sr;
  sfInfo = new_info;

  _beg = _rangeBeg = 0;
  _dur = _rangeDur = _rangeEnd = frames;

  updateFPP();

  _cache = new SoundCacheStream();
  _cache->allocate( frames, ch );

  redraw();
}

void QcWaveform::write( const QVector<double> & data, int offset )
{
  if( sf ) {
    qcWarningMsg( "QSoundFileView: can not write data while displaying a sound file!" );
    return;
  }

  if( !_cache || !_cache->ready() ) {
    qcWarningMsg( "QSoundFileView: can not write data; memory has not been allocated yet!" );
    return;
  }

  int ch = sfInfo.channels;
  int ns = data.size();
  int nf = ns / ch;

  if( nf * ch != ns ) {
    qcWarningMsg( "QSoundFileView: can not write data; size not a multiple of channels!" );
    return;
  }

  if( offset < 0 || offset + nf > _rangeEnd ) {
    qcWarningMsg( "QSoundFileView: can not write data; either offset is wrong or data size is too large." );
    return;
  }

  _cache->write( data, offset, nf );

  redraw();
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

QVariantList QcWaveform::selections() const
{
  QVariantList slist;
  for( int i = 0; i < 64; ++i ) {
    slist << QVariant( selection(i) );
  }
  return slist;
}

void QcWaveform::setCurrentSelection( int i ) {
  if( i < 0 || i > 63 ) return;
  _curSel = i;
  update();
}

QVariantList QcWaveform::selection( int i ) const
{
  QVariantList list;
  if( i < 0 || i > 63 ) return list;
  const Selection &s = _selections[i];
  list << QVariant(static_cast<int>(s.start - _rangeBeg));
  list << QVariant(static_cast<int>(s.size));
  return list;
}

void QcWaveform::setSelection( int i, sf_count_t a, sf_count_t b )
{
  if( i < 0 || i > 63 ) return;
  Selection& s = _selections[i];
  s.start = qMin( a, b );
  s.size = qMax( a, b ) - s.start;
  update();
}

void QcWaveform::setSelection( int i, QVariantList list )
{
  if( list.count() < 2 ) return;
  sf_count_t start = list[0].toInt() + _rangeBeg;
  sf_count_t end = start + list[1].toInt();
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

QVariantList QcWaveform::waveColors() const
{
  QVariantList clist;
  Q_FOREACH( QColor clr, _waveColors )
    clist << QVariant(clr);
  return clist;
}

void QcWaveform::setWaveColors( const QVariantList &list )
{
  _waveColors.clear();
  Q_FOREACH( const QVariant & var, list )
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

  p.fillRect( rect(), _bkgColor );

  // draw waveform on pixmap

  if( _drawWaveform && dirty ) {
    draw( pixmap, 0, width(), _beg, _dur );
    dirty = false;
  }

  // draw time grid

  if( _showGrid && sfInfo.samplerate > 0 && _gridResolution > 0.f ) {
    p.save();

    // Since _gridResolution and _gridOffset are in seconds, scale using
    // duration in seconds.
    double dur_in_secs = _dur / sfInfo.samplerate;

    p.setPen( Qt::NoPen );
    p.setBrush( _gridColor );
    p.scale( (double) width() / dur_in_secs, 1.0 );

    // Wrap the offset to (-100%, 100%) of the grid resolution. Subtract by
    // beg_in_secs to account for zoom+timeshift.
    double beg_in_secs = _beg / sfInfo.samplerate;
    double offset = std::fmod(_gridOffset - beg_in_secs, _gridResolution * 2);

    // Catch the case where the grid is [50% - 100%) offset from the start
    // in order to draw the portion off the left side of the view.
    if (offset > _gridResolution)
      offset -= _gridResolution * 2;

    while( offset < dur_in_secs ) {
      QRectF r( offset, 0, _gridResolution, height() );
      p.drawRect( r );
      offset += _gridResolution * 2;
    }

    p.restore();
  }

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

void QcWaveform::keyPressEvent( QKeyEvent *ev )
{
  if( ev->key() == Qt::Key_Shift && _dragAction == Navigate )
  {
    _dragPoint = mapFromGlobal(QCursor::pos());
    _dragData2 = zoom();
  }
  else
    ev->ignore();
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
    _dragAction = Navigate;
    _dragData = ev->pos().x() * _fpp + _beg;
    _dragData2 = zoom();
  }
}

void QcWaveform::mouseDoubleClickEvent ( QMouseEvent * )
{
  setSelection( _curSel, _rangeBeg, _rangeEnd );
  Q_EMIT( action() );
}

void QcWaveform::mouseMoveEvent( QMouseEvent *ev )
{
  if( !ev->buttons() ) return;

  if( _dragAction == Navigate ) {
    Qt::KeyboardModifiers mods = ev->modifiers();
    if( mods & Qt::ShiftModifier ) {
      double factor = pow( 2, (ev->pos().y() - _dragPoint.y()) * 0.008 );
      // zoom to the initial zoom times the factor based on distance from initial position
      zoomTo( _dragData2 * factor );
    }
    // scroll to the clicked frame minus the current mouse position in frames
    // _fpp has been adjusted by zooming
    scrollTo( _dragData - (ev->pos().x() * _fpp) );
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

  if( !_cache || !_cache->ready() ) return;

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
  bool canUseCache = _fpp < 1.0 ? _cache->fpu() == 1.0 : _fpp >= _cache->fpu();

  SoundStream *soundStream;
  SoundFileStream sfStream;

  if( canUseCache ) {
    qcDebugMsg( 2, QStringLiteral("using cache") );
    soundStream = _cache;
  }
  else if( sf ) {
    qcDebugMsg( 2, QStringLiteral("using file") );
    soundStream = &sfStream;
    sfStream.load( sf, sfInfo, i_beg, i_count );
  }
  else {
    qcWarningMsg( "QSoundFileView: can't paint waveform: view resolution exceeds data cache resolution,"
                  " and soundfile is not given." );
    return;
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
      short * minBuffer = (short*)alloca(width * sizeof(short));
      short * maxBuffer = (short*)alloca(width * sizeof(short));;
      short * minRMS = (short*)alloca(width * sizeof(short));;
      short * maxRMS = (short*)alloca(width * sizeof(short));;

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
