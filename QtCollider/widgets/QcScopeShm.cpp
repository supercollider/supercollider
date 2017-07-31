/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of Qt GUI for SuperCollider.
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

#include "QcScopeShm.h"
#include "scope_shm_interface.hpp"
#include "../QcWidgetFactory.h"
#include "../debug.h"

#include <QPainter>
#include <QTimer>
#include <QResizeEvent>
#include <QWindow>

QC_DECLARE_QWIDGET_FACTORY(QcScopeShm);

QcScopeShm::QcScopeShm() :
  _srvPort(-1),
  _scopeIndex(-1),
  _shm(new ScopeShm(this)),
  _running(false),
  _data(0),
  _availableFrames(0),
  xOffset( 0.f ),
  yOffset( 0.f ),
  xZoom( 1.f ),
  yZoom( 1.f ),
  _style( 0 ),
  _bkg( QColor(0,0,0) ),
  _fill( true )
{
  setAttribute( Qt::WA_OpaquePaintEvent, true );
  setAutoFillBackground(false);

  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  
  timer = new QTimer( this );
  timer->setInterval( 50 );
  connect( timer, SIGNAL( timeout() ), this, SLOT( updateScope() ) );
}

QcScopeShm::~QcScopeShm()
{
  stop();
}

void QcScopeShm::setServerPort( int port )
{
  if( _running ) {
    qcWarningMsg( "QScope: Can not change server port while running!" );
    return;
  }

  _srvPort = port;
}

void QcScopeShm::setBufferNumber( int n )
{
  if( _running ) {
    // TODO: release used reader?
    initScopeReader( _shm, n );
  }
  _scopeIndex = n;
}

void QcScopeShm::setWaveColors( const QVariantList & newColors )
{
  colors.clear();
  Q_FOREACH( const QVariant & var, newColors ) {
    QColor color = var.value<QColor>();
    if( !color.isValid() )
      colors.append( QColor( 0,0,0 ) );
    else
      colors.append( color );
  }
}

int QcScopeShm::updateInterval() const {
  return timer->interval();
}

void QcScopeShm::setUpdateInterval( int interval ) {
  timer->setInterval( qMax(0, interval) );
}

void QcScopeShm::start()
{
  if( _running ) return;
  if( _srvPort < 0 || _scopeIndex < 0 ) return;

  connectSharedMemory( _srvPort );
  if( !_shm->client ) {
    stop();
    return;
  }

  initScopeReader( _shm, _scopeIndex );

  timer->start();

  _running = true;
}

void QcScopeShm::stop()
{
  // TODO: release used reader?

  delete _shm->client;
  _shm->client = 0;

  timer->stop();

  _running = false;
}

void QcScopeShm::updateScope()
{
  bool valid = _shm->reader.valid();
  //qcDebugMsg(1, tr("valid = %1").arg(valid));
  if(!valid) return;

  bool ok = _shm->reader.pull( _availableFrames );
  //qcDebugMsg(1, tr("Got %1 frames").arg(_availableFrames) );
  if(ok) {
    _data = _shm->reader.data();
    update();
  }
}

void QcScopeShm::resizeEvent ( QResizeEvent * ev )
{
  _pixmap = QPixmap(ev->size());
}

void QcScopeShm::paintEvent ( QPaintEvent * event )
{
  Q_UNUSED( event );

  QPainter p;

  _pixmap.fill( _bkg );

  if( _running && _availableFrames ) {

    int chanCount = _shm->reader.channels();
    int maxFrames = _shm->reader.max_frames();
    QRect area (_pixmap.rect());
    p.begin(&_pixmap);

    switch (_style) {
      case 0:
        paint1D( false, chanCount, maxFrames, _availableFrames, area, p );
        break;
      case 1:
        paint1D( true, chanCount, maxFrames, _availableFrames, area, p );
        break;
      case 2:
        paint2D( chanCount, maxFrames, _availableFrames, area, p );
        break;
    }

    p.end();
  }

  p.begin(this);
  p.drawPixmap(0, 0, _pixmap);
}

void QcScopeShm::paint1D( bool overlapped, int chanCount, int maxFrames, int frameCount,
                          const QRect &area, QPainter & painter )
{
  //qcDebugMsg( 0, tr("Drawing: data %1 / channels %2 / max-size %3").arg(_data!=0).arg(chanCount).arg(maxFrames) );

  if( frameCount < 2 || area.width() < 1 || area.height() < 1 ) return;

  float yRatio = - yZoom * area.height() * 0.5;
  if( !overlapped ) yRatio /= chanCount;
  float yHeight = area.height();
  if( !overlapped ) yHeight /= chanCount;
  QPen pen;
  pen.setWidth(0);  // width==0 means width 1 regardless of transformations
  pen.setCapStyle( Qt::FlatCap );

  if( frameCount < area.width() )
  {
    float xRatio = xZoom * area.width() / (frameCount-1);

    for( int ch = 0; ch < chanCount; ch++ ) {
      float *frameData = _data + (ch * maxFrames); //frame vector
      float yOrigin = yHeight * (overlapped ? 0.5 : ch + 0.5);
      QColor strokeColor = ch < colors.count() ? colors[ch] : QColor(255,255,255);
      QColor fillColor(strokeColor); fillColor.setAlpha(0.65 * 255);
      pen.setColor( strokeColor );

      painter.save();
      painter.translate( area.x(), area.y() + yOrigin );
      painter.scale( xRatio, yRatio );
      painter.setPen(pen);

      QPainterPath path;
      path.moveTo( xOffset, frameData[0] );
      for( int f = 1; f < frameCount; ++f )
        path.lineTo( xOffset + f, frameData[f] );

      if (_fill) {
        path.lineTo(xOffset + frameCount, 0);
        path.lineTo(0, 0);
        path.lineTo(xOffset, frameData[0]);
        
        painter.fillPath(path, QBrush(fillColor));
      }
      
      painter.drawPath(path);

      painter.restore();
    }
  }
  else
  {
    int w = area.width();
    float fpp = frameCount / (float) w; // frames per x pixel

    for( int ch = 0; ch < chanCount; ch++ )
    {
      float *frameData = _data + (ch * maxFrames); //frame vector
      float yOrigin = yHeight * (overlapped ? 0.5 : ch + 0.5);
      QColor strokeColor = ch < colors.count() ? colors[ch] : QColor(255,255,255);
      QColor fillColor(strokeColor); fillColor.setAlpha(0.65 * 255);

      painter.save();
      painter.translate( area.x(), area.y() + yOrigin );
      painter.setPen(pen);

      QPainterPath pathLine;
      QPainterPath pathFill;

      int p=0, f=1; // pixel, frame
      float min, max;
      min = max = frameData[0];

      while( p++ < w )
      {
        int f_max = fpp * p;

        for(; f < f_max; ++f)
        {
          float d = frameData[f];
          if( d < min ) min = d;
          if( d > max ) max = d;
        }

        qreal x = p-1;
        float y = max * yRatio;
        pathLine.moveTo( x, y );
        y = qMax( min * yRatio, y+1 );
        pathLine.lineTo( x, y );
        
        if (_fill) {
          pathFill.moveTo( x, y );
          pathFill.lineTo( x, 0 );
        }
        
        // flip min/max to ensure continuity
        float val = min;
        min = max;
        max = val;
      }
      
      pen.setColor(strokeColor);
      painter.strokePath(pathLine, pen);
      
      if (_fill) {
        pen.setColor(fillColor);
        painter.strokePath(pathFill, pen);
      }

      painter.restore();
    }
  }
}

void QcScopeShm::paint2D( int chanCount, int maxFrames, int frameCount,
                          const QRect &area, QPainter & painter )
{
  int minSize = qMin( area.width(), area.height() );
  // NOTE: use yZoom for both axis, since both represent value, as opposed to index
  float xRatio = yZoom * minSize * 0.5;
  float yRatio = -yZoom * minSize * 0.5;
  QPoint center = area.center();
  QPen pen;
  pen.setWidth(0);  // width==0 means width 1 regardless of transformations
  pen.setCapStyle( Qt::FlatCap );
  pen.setColor(colors.count() ? colors[0] : QColor(255,255,255));
    
  painter.setPen(pen);
  painter.translate( center.x(), center.y() );
  painter.scale( xRatio, yRatio );

  QPainterPath path;

  if( chanCount >= 2 )
  {
    float *data1 = _data;
    float *data2 = _data + maxFrames;

    path.moveTo( data1[0], data2[0] );
    for( int f = 1; f < frameCount; ++f )
      path.lineTo( data1[f], data2[f] );
  }
  else
  {
    float *data1 = _data;
    path.moveTo( data1[0], 0.f );
    for( int f = 1; f < frameCount; ++f )
      path.lineTo( data1[f], 0.f );
  }

  painter.drawPath(path);
}

void QcScopeShm::connectSharedMemory( int port )
{
  try {
      server_shared_memory_client * client = new server_shared_memory_client(port);
      _shm->client = client;
      qcDebugMsg(1,"Shared memory connected");
  } catch (std::exception & e) {
      _shm->client = 0;
      qcErrorMsg(QStringLiteral("Cannot connect to shared memory: %1").arg(e.what()) );
  }
}

void QcScopeShm::initScopeReader( ScopeShm *shm, int index )
{
  shm->reader = shm->client->get_scope_buffer_reader( index );
  qcDebugMsg(1,QStringLiteral("Initialized scope buffer reader for index %1.").arg(index));
}
