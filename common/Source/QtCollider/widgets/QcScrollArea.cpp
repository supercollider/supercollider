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

#include "QcScrollArea.h"
#include "../Painting.h"

#include <QLayout>
#include <QScrollBar>
#include <QEvent>
#include <QChildEvent>
#include <QPainter>
#include <QApplication>

void QcScrollWidget::updateSize() {
  QRect r = childrenRect();
  QSize sz( r.x() + r.width(), r.y() + r.height() );
  resize( sz );
}
bool QcScrollWidget::event ( QEvent * e ) {
  if( e->type() == QEvent::ChildAdded ) {
    QChildEvent *ce = static_cast<QChildEvent*>(e);
    ce->child()->installEventFilter( this );
  }
  return QWidget::event( e );
}
bool QcScrollWidget::eventFilter ( QObject * watched, QEvent * event ) {
  Q_UNUSED( watched );
  if( event->type() == QEvent::Resize ) {
    updateSize();
  }
  return false;
}


QcScrollArea::QcScrollArea() :
  paint(false),
  painting(false)
{
  setViewport( new QWidget() );
  scrollWidget = new QcScrollWidget( viewport() );
  scrollWidget->installEventFilter( this );
  updateScrollBars();
}

void QcScrollArea::setHasBorder( bool b ) {
  if( b )
    QFrame::setFrameShape( QFrame::NoFrame );
  else
    QFrame::setFrameShape( QFrame::StyledPanel );
}

void QcScrollArea::addWidget( QWidget *w ) {
  w->setParent( scrollWidget );
  w->show();
}

void QcScrollArea::updateScrollBars() {
  QSize areaSize = viewport()->size();
  QSize  widgetSize = scrollWidget->size();

  verticalScrollBar()->setPageStep(widgetSize.height());
  horizontalScrollBar()->setPageStep(widgetSize.width());
  verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
  horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
  updateWidgetPosition();
}

void QcScrollArea::updateWidgetPosition() {
  int hvalue = horizontalScrollBar()->value();
  int vvalue = verticalScrollBar()->value();
  QPoint topLeft = viewport()->rect().topLeft();
  scrollWidget->move(topLeft.x() - hvalue, topLeft.y() - vvalue);
  viewport()->update();
}

void QcScrollArea::scrollContentsBy ( int dx, int dy ) {
  updateWidgetPosition();
}

void QcScrollArea::resizeEvent( QResizeEvent * ) {
  updateScrollBars();
}

bool QcScrollArea::eventFilter ( QObject * watched, QEvent * event ) {
  if( watched == scrollWidget && event->type() == QEvent::Resize ) {
    updateScrollBars();
  }
  return false;
}

void QcScrollArea::setBackground ( const QColor &color )
{
  if( !color.isValid() ) return;
  bkg = color;
  viewport()->setAttribute( Qt::WA_OpaquePaintEvent, true );
  viewport()->update();
}

void QcScrollArea::rebuildPen()
{
  QtCollider::lockLang();

  QtCollider::beginPen( &pen );
  QApplication::sendEvent( this, new ScMethodCallEvent( "draw", QList<QVariant>(), true ) );
  QtCollider::endPen();

  QtCollider::unlockLang();

  update();
}

void QcScrollArea::paintEvent( QPaintEvent *e )
{
  Q_UNUSED(e);

  QPainter p(viewport());

  if( bkg.isValid() ) {
    p.fillRect( viewport()->rect(), bkg );
  }

  if( !paint ) return;

  int hOffset = horizontalScrollBar()->value();
  int vOffset = verticalScrollBar()->value();
  pen.setBaseTransform( QTransform::fromTranslate( -hOffset, -vOffset ) );
  pen.paint( &p );
}
