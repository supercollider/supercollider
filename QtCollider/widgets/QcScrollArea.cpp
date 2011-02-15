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
#include "../QcWidgetFactory.h"

#include <QLayout>
#include <QScrollBar>
#include <QEvent>
#include <QChildEvent>
#include <QApplication>

class QcScrollAreaFactory : public QcWidgetFactory<QcScrollArea>
{
protected:
  virtual QObjectProxy *newInstance( PyrObject *scObject, QList<QVariant> & arguments )
  {
    QObjectProxy *proxy =
        QcWidgetFactory<QcScrollArea>::newInstance( scObject, arguments );

    if( proxy ) {
      QcScrollArea *scrollArea = static_cast<QcScrollArea*>( proxy->object() );
      QObject::connect( scrollArea->widget(), SIGNAL(painting(QPainter*)),
                        proxy, SLOT(customPaint(QPainter*)) );
    }

    return proxy;
  }
};

static QcScrollAreaFactory factory;

QcScrollWidget::QcScrollWidget( QWidget *parent ) : QcCanvas( parent )
{
  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

QSize QcScrollWidget::sizeHint() const
{
  QRect r = childrenRect();
  QSize sz( r.x() + r.width(), r.y() + r.height() );
  return sz;
}

bool QcScrollWidget::event ( QEvent * e ) {
  int t = e->type();
  if( t == QEvent::ChildAdded ) {
    QChildEvent *ce = static_cast<QChildEvent*>(e);
    ce->child()->installEventFilter( this );
  }
  else if( t == QEvent::ChildRemoved ) {
    updateGeometry();
  }

  return QWidget::event( e );
}

bool QcScrollWidget::eventFilter ( QObject * watched, QEvent * event ) {
  Q_UNUSED( watched );

  switch( event->type() ) {
    case QEvent::Resize:
    case QEvent::Move:
    case QEvent::Show:
    case QEvent::Hide:
      updateGeometry();
      break;
    default:
      return false;
  }

  return false;
}

QcScrollArea::QcScrollArea()
{
  scrollWidget = new QcScrollWidget( viewport() );
  setWidget( scrollWidget );
  setWidgetResizable( true );
}

void QcScrollArea::setHasBorder( bool b ) {
  if( b )
    QFrame::setFrameShape( QFrame::StyledPanel );
  else
    QFrame::setFrameShape( QFrame::NoFrame );
}

QPointF QcScrollArea::visibleOrigin() const
{
  return scrollWidget->mapFromParent( QPoint(0,0) );
}

void QcScrollArea::setVisibleOrigin( const QPointF &pt )
{
  horizontalScrollBar()->setValue( pt.x() );
  verticalScrollBar()->setValue( pt.y() );
}
