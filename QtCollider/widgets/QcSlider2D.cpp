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

#include "QcSlider2D.h"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

QC_DECLARE_QWIDGET_FACTORY(QcSlider2D);

QcSlider2D::QcSlider2D(): QtCollider::Style::Client(this), _x(0.0), _y(0.0), _thumbSize(QSize(20, 20)), _step(0.01) {
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

void QcSlider2D::incrementX(double factor) { setValue(QPointF(_step * factor + _x, _y)); }

void QcSlider2D::decrementX(double factor) { setValue(QPointF(-_step * factor + _x, _y)); }

void QcSlider2D::incrementY(double factor) { setValue(QPointF(_x, _step * factor + _y)); }

void QcSlider2D::decrementY(double factor) { setValue(QPointF(_x, -_step * factor + _y)); }

QRect QcSlider2D::thumbRect() {
    using namespace QtCollider::Style;
    return QtCollider::Style::rect(QPointF(_x, _y), sunkenContentsRect(rect()), _thumbSize);
}

void QcSlider2D::setValue(const QPointF val, bool doAction) {
    double x = qMax(0.0, qMin(1.0, (double)val.x()));
    double y = qMax(0.0, qMin(1.0, (double)val.y()));
    if (x != _x || y != _y) {
        _x = x;
        _y = y;
        update();
        if (doAction)
            Q_EMIT(action());
    }
}

void QcSlider2D::mouseMoveEvent(QMouseEvent* ev) {
    using namespace QtCollider::Style;

    if (!ev->buttons())
        return;

    QPointF val = QtCollider::Style::value(QPointF(ev->pos()), sunkenContentsRect(rect()), _thumbSize);
    setValue(val);
}

void QcSlider2D::mousePressEvent(QMouseEvent* ev) {
    using namespace QtCollider::Style;
    QPointF val = QtCollider::Style::value(QPointF(ev->pos()), sunkenContentsRect(rect()), _thumbSize);
    setValue(val);
}

void QcSlider2D::keyPressEvent(QKeyEvent* e) {
    double step = _step;
    switch (e->key()) {
    case Qt::Key_Up:
        modifyStep(&step);
        setValue(QPointF(_x, _y + step));
        break;
    case Qt::Key_Down:
        modifyStep(&step);
        setValue(QPointF(_x, _y - step));
        break;
    case Qt::Key_Right:
        modifyStep(&step);
        setValue(QPointF(_x + step, _y));
        break;
    case Qt::Key_Left:
        modifyStep(&step);
        setValue(QPointF(_x - step, _y));
        break;
    case Qt::Key_N:
        setValue(QPointF(0.0, 0.0));
        break;
    case Qt::Key_X:
        setValue(QPointF(1.0, 1.0));
        break;
    case Qt::Key_C:
        setValue(QPointF(0.5, 0.5));
        break;
    case Qt::Key_R:
        Q_EMIT(randomize());
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}

void QcSlider2D::setBackgroundImage(const QtCollider::SharedImage& image, const QRectF& rect, int tileMode,
                                    double opacity) {
    _backgroundImage.setImage(image, rect, tileMode, opacity);
}

void QcSlider2D::paintEvent(QPaintEvent* e) {
    using namespace QtCollider::Style;
    using QtCollider::Style::Ellipse;
    using QtCollider::Style::RoundRect;

    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPalette plt = palette();

    RoundRect frame(rect(), 2);
    drawSunken(&p, plt, frame, grooveColor(), hasFocus() ? focusColor() : QColor());

    if (_backgroundImage.isValid())
        _backgroundImage.paint(&p, rect());

    Ellipse thumb(thumbRect());
    drawRaised(&p, plt, thumb, plt.color(QPalette::Button).lighter(105));

    QRectF r(thumb._rect);
    qreal wdif = r.width() * 0.3;
    qreal hdif = r.height() * 0.3;
    p.setPen(Qt::NoPen);
    p.setBrush(knobColor());
    p.drawEllipse(r.adjusted(wdif, hdif, -wdif, -hdif));
}
