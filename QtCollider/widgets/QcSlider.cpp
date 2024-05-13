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

#include "QcSlider.h"
#include "../QcWidgetFactory.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

QC_DECLARE_QWIDGET_FACTORY(QcSlider);

QcSlider::QcSlider(): QtCollider::Style::Client(this), _value(0.0), _step(0.0), _hndLen(20) {
    setFocusPolicy(Qt::StrongFocus);
    setOrientation(Qt::Vertical);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

double QcSlider::pixelStep() {
    using namespace QtCollider::Style;

    QRect contRect(sunkenContentsRect(rect()));
    int range = (_ort == Qt::Horizontal) ? contRect.width() : contRect.height();
    range -= _hndLen;

    if (range > 0)
        return 1.0 / range;
    else
        return 0.0;
}

void QcSlider::setValue(double val) {
    double step = _step;
    modifyStep(&step);
    if (step)
        val = qRound(val / step) * step;

    _value = qBound(0.0, val, 1.0);

    update();
}

void QcSlider::increment(double factor) {
    double step = qMax(_step, pixelStep());
    setValue(step * factor + _value);
    update();
}

void QcSlider::decrement(double factor) {
    double step = qMax(_step, pixelStep());
    setValue(-step * factor + _value);
    update();
}

void QcSlider::setOrientation(int i) {
    _ort = (Qt::Orientation)i;

    if (_ort == Qt::Horizontal)
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    else
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    updateGeometry();
    update();
}

QSize QcSlider::sizeHint() const {
    if (orientation() == Qt::Horizontal)
        return QSize(qMax(_hndLen + 10, 150), 20);
    else
        return QSize(20, qMax(_hndLen + 10, 150));
}

QSize QcSlider::minimumSizeHint() const {
    if (orientation() == Qt::Horizontal)
        return QSize(_hndLen + 10, 20);
    else
        return QSize(20, _hndLen + 10);
}

void QcSlider::mousePressEvent(QMouseEvent* e) {
    setValue(valueAt(e->pos()));
    update();
    Q_EMIT(action());
}

void QcSlider::mouseMoveEvent(QMouseEvent* e) {
    if (!e->buttons())
        return;

    setValue(valueAt(e->pos()));
    update();
    Q_EMIT(action());
}

void QcSlider::wheelEvent(QWheelEvent* e) {
    double step = qMax(_step, pixelStep());
    modifyStep(&step);
    double dval = e->delta() / 120.0 * step;
    setValue(_value + dval);
    Q_EMIT(action());
}

void QcSlider::paintEvent(QPaintEvent* e) {
    using namespace QtCollider::Style;
    using QtCollider::Style::RoundRect;

    QPainter p(this);
    const QPalette& plt = palette();

    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);

    QRect rGroove = rect();

    // draw groove
    RoundRect shGroove(rGroove, 2);
    drawSunken(&p, plt, shGroove, grooveColor(), hasFocus() ? focusColor() : QColor());

    // geometry
    QRect rHandle(thumbRect());

    // draw handle
    RoundRect shHandle(rHandle, 2);
    drawRaised(&p, plt, shHandle, plt.color(QPalette::Button).lighter(105));

    p.restore();

    // draw marker
    QPen pen(knobColor());
    pen.setWidth(2);
    p.setPen(pen);
    if (_ort == Qt::Horizontal) {
        qreal center = rHandle.center().x() + 1;
        QLine line(center, rHandle.top() + 3, center, rHandle.bottom() - 2);
        p.drawLine(line);
        pen.setColor(plt.color(QPalette::Light));
    } else {
        qreal center = rHandle.center().y() + 1;
        QLine line(rHandle.left() + 3, center, rHandle.right() - 2, center);
        p.drawLine(line);
        pen.setColor(plt.color(QPalette::Light));
    }
}

QRect QcSlider::thumbRect() {
    using namespace QtCollider::Style;

    QRect contRect(sunkenContentsRect(rect()));

    QRect r;
    if (_ort == Qt::Horizontal) {
        int pos = _value * (contRect.width() - _hndLen);
        r.setX(pos + contRect.left());
        r.setY(contRect.top());
        r.setSize(QSize(_hndLen, contRect.height()));
    } else {
        int pos = _value * (contRect.height() - _hndLen);
        r.setX(contRect.left());
        r.setY(contRect.bottom() - pos - _hndLen + 1);
        r.setSize(QSize(contRect.width(), _hndLen));
    }
    return r;
}

double QcSlider::valueAt(const QPoint& pos) {
    using namespace QtCollider::Style;

    QRect contRect(sunkenContentsRect(rect()));
    if (_ort == Qt::Horizontal)
        return xValue(pos.x(), contRect, QSize(_hndLen, 0));
    else
        return yValue(pos.y(), contRect, QSize(0, _hndLen));
}
