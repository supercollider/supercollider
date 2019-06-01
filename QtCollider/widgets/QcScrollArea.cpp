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

#include "QcScrollArea.h"
#include "../QcWidgetFactory.h"

#include <QLayout>
#include <QScrollBar>
#include <QEvent>
#include <QChildEvent>
#include <QApplication>

class QcScrollWidgetFactory : public QcWidgetFactory<QcScrollWidget> {
protected:
    virtual void initialize(QWidgetProxy* p, QcScrollWidget* w) {
        QObject::connect(w, SIGNAL(painting(QPainter*)), p, SLOT(customPaint(QPainter*)));
    }
};

QC_DECLARE_FACTORY(QcScrollWidget, QcScrollWidgetFactory);

QcScrollWidget::QcScrollWidget(QWidget* parent): QcCanvas(parent) {
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

QSize QcScrollWidget::sizeHint() const {
    QRect r = childrenRect();
    QSize sz(r.x() + r.width(), r.y() + r.height());
    return sz;
}

bool QcScrollWidget::event(QEvent* e) {
    int t = e->type();
    if (t == QEvent::ChildAdded) {
        QChildEvent* ce = static_cast<QChildEvent*>(e);
        ce->child()->installEventFilter(this);
    } else if (t == QEvent::ChildRemoved) {
        updateGeometry();
    }

    return QWidget::event(e);
}

bool QcScrollWidget::eventFilter(QObject* watched, QEvent* event) {
    Q_UNUSED(watched);

    switch (event->type()) {
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


QC_DECLARE_QWIDGET_FACTORY(QcScrollArea);

QcScrollArea::QcScrollArea() {
    connect(horizontalScrollBar(), SIGNAL(actionTriggered(int)), this, SIGNAL(scrolled()));
    connect(verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SIGNAL(scrolled()));
}

void QcScrollArea::setWidget(QWidget* widget) {
    QScrollArea::setWidget(widget);
    setWidgetResizable(true);
}

void QcScrollArea::addChild(QWidget* w) {
    if (widget())
        w->setParent(widget());
}

QRectF QcScrollArea::innerBounds() const {
    QSize vs = viewport()->size();
    if (!widget())
        return QRectF(0, 0, vs.width(), vs.height());
    QSize cs = widget()->size();
    return QRectF(0, 0, qMax(vs.width(), cs.width()), qMax(vs.height(), cs.height()));
}

void QcScrollArea::setHasBorder(bool b) {
    if (b)
        QFrame::setFrameShape(QFrame::StyledPanel);
    else
        QFrame::setFrameShape(QFrame::NoFrame);
}

QPointF QcScrollArea::visibleOrigin() const {
    QWidget* w = widget();
    return (w != 0 ? widget()->mapFromParent(QPoint(0, 0)) : QPoint(0, 0));
}

void QcScrollArea::setVisibleOrigin(const QPointF& pt) {
    if (horizontalScrollBar()->maximum() < pt.x())
        horizontalScrollBar()->setMaximum(pt.x());
    if (verticalScrollBar()->maximum() < pt.y())
        verticalScrollBar()->setMaximum(pt.y());

    horizontalScrollBar()->setValue(pt.x());
    verticalScrollBar()->setValue(pt.y());
}
