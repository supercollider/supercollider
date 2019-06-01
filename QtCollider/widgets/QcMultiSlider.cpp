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

#include "QcMultiSlider.h"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include <cmath>

QC_DECLARE_QWIDGET_FACTORY(QcMultiSlider);

QcMultiSlider::QcMultiSlider():
    QtCollider::Style::Client(this),
    _currentIndex(0),
    _selectionSize(1),
    roundStep(0.f),
    editable(true),
    ort(Qt::Vertical),
    elastic(false),
    thumbSize(QSizeF(12.f, 12.f)),
    gap(1),
    drawRects(true),
    drawLines(false),
    isFilled(false),
    highlight(false),
    startIndex(0) {
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_AcceptTouchEvents);

    connect(this, SIGNAL(modified()), this, SLOT(update()));
    connect(this, SIGNAL(interacted()), this, SLOT(doAction()));
}

void QcMultiSlider::setSliderCount(int newSize) {
    while (newSize > _values.size())
        _values.append(0.f);
    while (newSize < _values.size())
        _values.removeLast();
    update();
}

QVector<double> QcMultiSlider::values() const { return QVector<double>::fromList(_values); }

void QcMultiSlider::setValues(const QVector<double>& vec) {
    _values.clear();

    Q_FOREACH (double value, vec)
        _values << qBound(0.0, rounded(value), 1.0);

    update();
}


double QcMultiSlider::value() const {
    if (_currentIndex >= 0 && _currentIndex < _values.count())
        return _values[_currentIndex];
    else
        return 0.0;
}

void QcMultiSlider::setValue(double val) {
    if (_currentIndex >= 0 && _currentIndex < _values.count())
        setValue(_currentIndex, val);

    update();
}

QVector<double> QcMultiSlider::reference() const { return QVector<double>::fromList(_ref); }

void QcMultiSlider::setReference(const QVector<double>& vec) {
    _ref.clear();

    Q_FOREACH (double value, vec)
        _ref << qBound(0.0, value, 1.0);

    update();
}

void QcMultiSlider::setStep(double step) {
    if (roundStep == step)
        return;
    roundStep = step;
    if (step == 0.0)
        return;
    int c = _values.count();
    int i;
    for (i = 0; i < c; ++i)
        setValue(i, _values[i]);
    update();
}

void QcMultiSlider::setIndex(int i) {
    if (i >= 0 && i < _values.count()) {
        _currentIndex = i;
        _selectionSize = 1;
        update();
    }
}

void QcMultiSlider::setSelectionSize(int i) {
    _selectionSize = qMin(i, _values.count() - _currentIndex);
    update();
}

inline void QcMultiSlider::setValue(int index, double value) { _values[index] = qBound(0.0, rounded(value), 1.0); }

inline double QcMultiSlider::rounded(double value) {
    return roundStep > 0.0 ? qRound(value / roundStep) * roundStep : value;
}

QRect QcMultiSlider::contentsRect() { return QtCollider::Style::sunkenContentsRect(rect()).adjusted(2, 2, -2, -2); }

QRect QcMultiSlider::valueRect(int count, qreal& spacing) {
    QRect r(contentsRect());

    bool horiz = ort == Qt::Horizontal;

    spacing = elastic ? (qreal)(horiz ? r.height() : r.width()) / count : thumbSize.width() + gap;

    if (!isFilled) {
        int hnd = thumbSize.height();
        if (horiz) {
            r.setWidth(r.width() - hnd);
            r.moveLeft(r.left() + hnd * 0.5);
        } else {
            r.setHeight(r.height() - hnd);
            r.moveTop(r.top() + hnd * 0.5);
        }
    }

    return r;
}

void QcMultiSlider::mousePressEvent(QMouseEvent* e) {
    using namespace QtCollider::Style;

    moveOrigin = e->pos();
    int c = _values.count();

    if (!c)
        return;

    bool horiz = ort == Qt::Horizontal;
    qreal spacing;

    QRect r(valueRect(c - startIndex, spacing));

    int i;
    float val;

    if (horiz) {
        i = spacing > 0.f ? floor((float)(moveOrigin.y() - r.y()) / spacing) : 0;
        val = xValue((qreal)moveOrigin.x(), r);
    } else {
        i = spacing > 0.f ? floor((float)(moveOrigin.x() - r.x()) / spacing) : 0;
        val = yValue((qreal)moveOrigin.y(), r);
    }

    i += startIndex;

    if (i >= startIndex && i < c) {
        _currentIndex = i;
        _selectionSize = 1;

        if (editable)
            setValue(i, val);
        if (editable || highlight)
            Q_EMIT(modified());

        Q_EMIT(interacted());
    }
}

void QcMultiSlider::mouseMoveEvent(QMouseEvent* e) {
    using namespace QtCollider::Style;

    if (!e->buttons())
        return;

    int c = _values.count();

    QPoint pos = e->pos();

    if (!c) {
        moveOrigin = pos;
        return;
    }

    qreal xStep;

    QRect r(valueRect(c - startIndex, xStep));

    float xOrig, yOrig, xDest, yDest;
    int xOffset, xRange;
    int iOrig, iDest, iMax;

    if (ort == Qt::Vertical) {
        xOffset = r.left();
        xRange = r.width();
        xOrig = moveOrigin.x();
        yOrig = yValue(moveOrigin.y(), r);
        xDest = pos.x();
        yDest = yValue(pos.y(), r);
    } else {
        xOffset = r.top();
        xRange = r.height();
        xOrig = moveOrigin.y();
        yOrig = xValue(moveOrigin.x(), r);
        xDest = pos.y();
        yDest = xValue(pos.x(), r);
    }

    if (xStep > 0.0) {
        iOrig = floor((xOrig - xOffset) / xStep);
        iOrig += startIndex;
        iDest = floor((xDest - xOffset) / xStep);
        iDest += startIndex;
        iMax = elastic ? c : qMin(c, int((float)xRange / xStep) + startIndex + 1);
    } else {
        iOrig = iDest = iMax = startIndex;
    }

    if (editable && (iOrig != iDest)) {
        float k = (yDest - yOrig) / (xDest - xOrig);
        float n = yOrig - (xOrig - xOffset) * k;
        int i = iOrig - startIndex;

        while (iOrig < iDest) {
            ++i;
            if (iOrig >= startIndex && iOrig < iMax) {
                float y = (i * xStep) * k + n;
                setValue(iOrig, y);
            }
            ++iOrig;
        }
        while (iOrig > iDest) {
            if (iOrig >= startIndex && iOrig < iMax) {
                float y = (i * xStep) * k + n;
                setValue(iOrig, y);
            }
            --iOrig;
            --i;
        }
    }

    if (iDest >= startIndex && iDest < iMax) {
        if (editable)
            setValue(iDest, yDest);

        _currentIndex = iDest;
        _selectionSize = 1;
    }

    moveOrigin = pos;

    Q_EMIT(modified());
    Q_EMIT(interacted());
}

void QcMultiSlider::paintEvent(QPaintEvent* e) {
    using namespace QtCollider::Style;
    using QtCollider::Style::Ellipse;
    using QtCollider::Style::RoundRect;

    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    RoundRect frame(rect(), 2);
    drawSunken(&p, palette(), frame, background(), hasFocus() ? focusColor() : QColor());

    if (!_values.count())
        return;

    p.setRenderHint(QPainter::Antialiasing, false);

    bool horiz = ort == Qt::Horizontal;

    QRect bounds(contentsRect());

    p.setClipRect(bounds);

    if (horiz) {
        p.translate(bounds.topLeft());
        p.rotate(90);
        p.scale(1.0, -1.0);
        bounds.setSize(QSize(bounds.height(), bounds.width()));
    } else {
        p.translate(bounds.left(), bounds.top() + bounds.height());
        p.scale(1.0, -1.0);
    }

    int count = _values.count() - startIndex;
    qreal spacing, width, yscale;

    spacing = elastic ? (qreal)bounds.width() / count : thumbSize.width() + gap;
    width = elastic ? qMin(spacing, (qreal)thumbSize.width()) : thumbSize.width();
    yscale = bounds.height();
    if (!isFilled)
        yscale -= thumbSize.height();

    const QColor& fillClr = fillColor();

    // selection

    if (highlight) {
        int i = _currentIndex - startIndex;
        int c = qMin(count - i, _selectionSize);
        if (c) {
            QRect r;
            r.setHeight(bounds.height());
            r.setWidth(c * spacing);
            r.moveLeft(i * spacing);

            QColor hlColor = fillClr;
            hlColor.setAlpha(70);
            p.fillRect(r, hlColor);
        }
    }

    QPen pen;
    pen.setColor(strokeColor());
    pen.setWidth(0);
    p.setPen(pen);

    // lines

    if (drawLines) {
        bool fill = isFilled & !drawRects;

        p.save();

        p.setRenderHint(QPainter::Antialiasing, true);
        p.translate(spacing * 0.5, isFilled ? 0.0 : thumbSize.height() * 0.5);
        p.scale(1.0, (qreal)yscale);
        if (fill)
            p.setBrush(fillClr);

        QPainterPath path;

        // value line

        path.moveTo(0, _values[startIndex]);
        for (int i = 1; i < count; ++i)
            path.lineTo((qreal)i * spacing, _values[i + startIndex]);

        // reference line

        int refcount = _ref.count() - startIndex;
        if (refcount > 0 || fill) {
            qreal x, y;
            int i = count - 1;

            x = i * spacing;
            y = i < refcount ? _ref[i + startIndex] : 0.f;
            if (fill)
                path.lineTo(x, y);
            else
                path.moveTo(x, y);

            while (--i >= 0) {
                x = i * spacing;
                y = i < refcount ? _ref[i + startIndex] : 0.f;
                path.lineTo(x, y);
            }

            if (fill)
                path.closeSubpath();
        }

        p.drawPath(path);

        p.restore();
    }

    // rects

    if (drawRects) {
        p.setRenderHint(QPainter::Antialiasing, false);
        p.translate((spacing - width) * 0.5, 0);
        p.setBrush(fillClr);

        QRectF r;
        r.setWidth(width);

        if (isFilled) {
            int refcount = _ref.count() - startIndex;
            for (int i = 0; i < count; ++i) {
                int ref = (i < refcount ? _ref[i + startIndex] : 0.f) * yscale;
                int val = _values[i + startIndex] * yscale;
                r.moveLeft(i * spacing);
                r.moveTop(ref);
                r.setHeight(val - ref);
                if (horiz)
                    p.drawRect(r.normalized().adjusted(0, 0, -1, -1));
                else
                    p.drawRect(r.normalized().adjusted(0, 1, -1, 0));
            }
        } else {
            r.setHeight(thumbSize.height());
            for (int i = 0; i < count; ++i) {
                r.moveLeft(i * spacing);
                r.moveTop(_values[i + startIndex] * yscale);
                if (horiz)
                    p.drawRect(r.adjusted(0, 0, -1, -1));
                else
                    p.drawRect(r.adjusted(0, 1, -1, 0));
            }
        }
    }
}

void QcMultiSlider::doAction() {
    Qt::KeyboardModifier ctrlMod =
#ifdef Q_OS_MAC
        Qt::MetaModifier;
#else
        Qt::ControlModifier;
#endif

    if (QApplication::keyboardModifiers() & ctrlMod)
        Q_EMIT(metaAction());
    else
        Q_EMIT(action());
}
