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

#include "QcGraph.h"
#include "../QcWidgetFactory.h"
#include "../style/routines.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QtCore/qmath.h>

#include <cmath>

QC_DECLARE_QWIDGET_FACTORY(QcGraph);

void QcGraphModel::append(QcGraphElement* e) {
    if (_elems.count()) {
        QcGraphElement* prev = _elems.last();
        prev->_next = e;
        e->_prev = prev;
    }
    _elems.append(e);
    Q_EMIT(appended(e));
}

void QcGraphModel::removeAt(int i) {
    QcGraphElement* e = _elems[i];
    int ci = _conns.count();
    while (ci--) {
        Connection c = _conns[ci];
        if (c.a == e || c.b == e)
            _conns.removeAt(ci);
    }
    if (e->_prev)
        e->_prev->_next = e->_next;
    if (e->_next)
        e->_next->_prev = e->_prev;
    _elems.removeAt(i);
    Q_EMIT(removed(e));
    delete e;
}


QcGraph::QcGraph():
    QtCollider::Style::Client(this),
    _defaultThumbSize(QSize(18, 18)),
    _gridOn(false),
    _style(DotElements),
    _drawLines(true),
    _drawRects(true),
    _editable(true),
    _step(0.01),
    _selectionForm(ElasticSelection),
    _xOrder(NoOrder),
    _geometryDirty(false),
    _lastIndex(-1) {
    QPalette plt(palette());

    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_AcceptTouchEvents);

    connect(&_model, SIGNAL(removed(QcGraphElement*)), this, SLOT(onElementRemoved(QcGraphElement*)));
}

QVariantList QcGraph::value() const {
    QVariantList x;
    QVariantList y;
    QList<QcGraphElement*> elems = _model.elements();
    Q_FOREACH (QcGraphElement* e, elems) {
        QPointF val = e->value;
        x.append(val.x());
        y.append(val.y());
    }

    QVariantList values;
    values.append(QVariant(x));
    values.append(QVariant(y));

    return values;
}

QcGraphElement* QcGraph::currentElement() const { return _selection.count() ? _selection.elems.first().elem : 0; }

int QcGraph::index() const {
    QcGraphElement* e = currentElement();
    return e ? _model.elements().indexOf(e) : -1;
}

QVariantList QcGraph::selectionIndexes() const {
    QVariantList result;
    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        if (e->selected)
            result << i;
    }
    return result;
}

float QcGraph::currentX() const {
    QcGraphElement* e = currentElement();
    return e ? e->value.x() : 0.f;
}

float QcGraph::currentY() const {
    QcGraphElement* e = currentElement();
    return e ? e->value.y() : 0.f;
}

void QcGraph::setValue(const QVariantList& list) {
    if (list.count() != 2)
        return;
    QVariantList xList = list[0].toList();
    QVariantList yList = list[1].toList();

    int newc = qMin(xList.count(), yList.count());
    if (!newc)
        return;

    int c = _model.elementCount();
    while (c > newc) {
        --c;
        _model.removeAt(c);
    }

    int i;
    for (i = 0; i < newc; ++i) {
        QPointF val(xList[i].toFloat(), yList[i].toFloat());
        if (i < c) {
            QcGraphElement* e = _model.elementAt(i);
            setValue(e, val);
        } else {
            QcGraphElement* e = new QcGraphElement(_defaultThumbSize);
            setValue(e, val);
            _model.append(e);
        }
    }

    if (newc)
        ensureOrder();

    _geometryDirty = true;

    update();
}

void QcGraph::setStrings(const QVariantList& list) {
    int strc = list.count();
    int c = _model.elementCount();
    int i;
    for (i = 0; i < c && i < strc; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->text = list[i].toString();
    }
    update();
}

void QcGraph::setCurves(const QVariantList& curves) {
    for (int i = 0; i < curves.size() && i < _model.elementCount(); ++i) {
        const QVariant& data = curves[i];
        QcGraphElement::CurveType type;
        double curvature;
        if (data.type() == QVariant::Int) {
            type = (QcGraphElement::CurveType)data.toInt();
            curvature = 0.0;
        } else {
            type = QcGraphElement::Curvature;
            curvature = data.toDouble();
        }
        _model.elementAt(i)->setCurveType(type, curvature);
    }
    update();
}

void QcGraph::setCurves(double curvature) {
    Q_FOREACH (QcGraphElement* e, _model.elements())
        e->setCurveType(QcGraphElement::Curvature, curvature);
    update();
}

void QcGraph::setCurves(int typeId) {
    QcGraphElement::CurveType type = (QcGraphElement::CurveType)typeId;
    Q_FOREACH (QcGraphElement* e, _model.elements())
        e->setCurveType(type);
    update();
}

void QcGraph::setStringAt(int i, const QString& str) {
    int c = _model.elementCount();
    if (i >= 0 && i < c) {
        QcGraphElement* e = _model.elementAt(i);
        e->text = str;
        update();
    }
}

void QcGraph::connectElements(int src, QVariantList targets) {
    int c = _model.elementCount();
    if (src < 0 || src >= c)
        return;

    Q_FOREACH (const QVariant& var, targets) {
        int trg = var.toInt();
        if (trg < 0 || trg >= c)
            continue;
        _model.connect(src, trg);
    }

    update();
}

void QcGraph::setIndex(int i) { select(i); }

void QcGraph::select(int i, bool exclusive) {
    if (i >= 0 && i < _model.elementCount()) {
        if (exclusive)
            setAllDeselected();
        setIndexSelected(i, true);
        update();
    }
}

void QcGraph::deselect(int i) {
    if (i >= 0 && i < _model.elementCount()) {
        setIndexSelected(i, false);
        update();
    }
}

void QcGraph::deselectAll() { setAllDeselected(); }

void QcGraph::setCurrentX(float f) {
    QcGraphElement* e = currentElement();
    if (!e)
        return;

    QPointF val = e->value;
    val.setX(f);
    if (_xOrder != NoOrder)
        orderRestrictValue(e, val, true);
    else
        restrictValue(val);
    e->value = val;

    update();
}

void QcGraph::setCurrentY(float f) {
    QcGraphElement* e = currentElement();
    if (!e)
        return;

    QPointF val = e->value;
    val.setY(f);
    if (_xOrder != NoOrder)
        orderRestrictValue(e, val, true);
    else
        restrictValue(val);
    e->value = val;

    update();
}

void QcGraph::setThumbSize(int s) {
    QSize size(s, s);

    _defaultThumbSize = size;

    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->size = size;
    }

    _largestThumbSize = size;
    _geometryDirty = false;

    update();
}

void QcGraph::setThumbWidth(int w) {
    _defaultThumbSize.setWidth(w);

    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->size.setWidth(w);
    }

    // For backward compatibility, switch to style that supports
    // different thumb width and height:
    _style = RectElements;
    _largestThumbSize.setWidth(w);

    update();
}

void QcGraph::setThumbHeight(int h) {
    _defaultThumbSize.setHeight(h);

    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->size.setHeight(h);
    }

    // For backward compatibility, switch to style that supports
    // different thumb width and height:
    _style = RectElements;
    _largestThumbSize.setHeight(h);

    update();
}

void QcGraph::setThumbSizeAt(int i, int s) {
    if (i < 0 || i >= _model.elementCount())
        return;
    _model.elementAt(i)->size = QSize(s, s);
    _geometryDirty = true;
    update();
}

void QcGraph::setThumbWidthAt(int i, int w) {
    if (i < 0 || i >= _model.elementCount())
        return;
    _model.elementAt(i)->size.setWidth(w);
    // For backward compatibility, switch to style that supports
    // different thumb width and height:
    _style = RectElements;
    _geometryDirty = true;
    update();
}

void QcGraph::setThumbHeightAt(int i, int h) {
    if (i < 0 || i >= _model.elementCount())
        return;
    _model.elementAt(i)->size.setHeight(h);
    // For backward compatibility, switch to style that supports
    // different thumb width and height:
    _style = RectElements;
    _geometryDirty = true;
    update();
}

void QcGraph::setFillColor(const QColor& color) {
    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->fillColor = color;
    }
    update();
}

void QcGraph::setFillColorAt(int i, const QColor& color) {
    int c = _model.elementCount();
    if (i >= 0 && i < c) {
        QcGraphElement* e = _model.elementAt(i);
        e->fillColor = color;
        update();
    }
}

void QcGraph::setEditableAt(int i, bool b) {
    int c = _model.elementCount();
    if (i >= 0 && i < c) {
        QcGraphElement* e = _model.elementAt(i);
        e->editable = b;
    }
}

void QcGraph::setStep(double step) {
    _step = qMax(0.0, step);

    if (_model.elementCount()) {
        ensureOrder();
        update();
    };
}

void QcGraph::setHorizontalOrder(int i) {
    _xOrder = (Order)i;
    if (_xOrder != NoOrder) {
        ensureOrder();
        update();
    }
}

void QcGraph::onElementRemoved(QcGraphElement* e) { _selection.elems.removeAll(SelectedElement(e)); }

void QcGraph::setAllDeselected() {
    int c = _model.elementCount();
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        e->selected = false;
    }
    _selection.elems.clear();
}

void QcGraph::setIndexSelected(int index, bool select) {
    Q_ASSERT(index >= 0 && index < _model.elementCount());

    // exit early if the element's selection status is already correct
    QcGraphElement* e = _model.elementAt(index);
    if (e->selected == select)
        return;

    if (select) {
        e->selected = true;

        // insert this node into the list of selected elements after selected
        // nodes with lower indices. Maintains that the list is sorted.
        int numSelectedNodes = 0;
        for (int i = 0; i < index; ++i) {
            if (_model.elementAt(i)->selected)
                ++numSelectedNodes;
        }

        _selection.elems.insert(numSelectedNodes, SelectedElement(e));

        // mark this as the last index selected
        _lastIndex = index;
    } else {
        e->selected = false;
        _selection.elems.removeAll(SelectedElement(e));
    }

    update();
}

inline static void qc_graph_round(double& val, double& step, bool& grid) {
    if (val < 0.0) {
        val = 0.0;
    } else if (grid) {
        double ratio = (val + (step * 0.5) > 1.0) ? qFloor(1.0 / step) : qRound(val / step);
        val = ratio * step;
    } else if (val > 1.0) {
        val = 1.0;
    }
}

inline void QcGraph::restrictValue(QPointF& val) {
    double x = val.x();
    double y = val.y();
    bool grid = _step > 0.0;
    qc_graph_round(x, _step, grid);
    qc_graph_round(y, _step, grid);
    val.setX(x);
    val.setY(y);
}

void QcGraph::orderRestrictValue(QcGraphElement* e, QPointF& val, bool selected) {
    restrictValue(val);

    double x0 = e->value.x();
    double x = val.x();

    if (x == x0) {
        return;
    } else if (x < x0) {
        // new x is smaller, check if not too small;
        QcGraphElement* prev = e->prev();
        if (prev && (selected || !prev->selected) && x < prev->value.x())
            val.setX(prev->value.x());
    } else {
        // new x is larger, check if not too large;
        QcGraphElement* next = e->next();
        if (next && (selected || !next->selected) && x > next->value.x())
            val.setX(next->value.x());
    }
}

inline void QcGraph::setValue(QcGraphElement* e, const QPointF& pt) {
    QPointF val(pt);
    restrictValue(val);
    e->value = val;
}

void QcGraph::ensureOrder() {
    int c = _model.elementCount();
    double x_min = 0.0;
    for (int i = 0; i < c; ++i) {
        QcGraphElement* e = _model.elementAt(i);
        QPointF val = e->value;
        if (_xOrder != NoOrder && val.x() < x_min)
            val.setX(x_min);
        setValue(e, val);
        x_min = e->value.x();
    }
}

void QcGraph::moveFree(QcGraphElement* e, const QPointF& val) {
    if (!e->editable)
        return;
    setValue(e, val);
}

void QcGraph::moveOrderRestricted(QcGraphElement* e, const QPointF& val) {
    if (!e->editable)
        return;
    QPointF v(val);
    orderRestrictValue(e, v, true);
    e->value = v;
}

void QcGraph::moveSelected(const QPointF& dif, SelectionForm form, bool cached) {
    int c = _selection.count();

    switch (form) {
    case ElasticSelection: {
        switch (_xOrder) {
        case NoOrder:

            for (int i = 0; i < c; ++i) {
                SelectedElement& se = _selection.elems[i];
                moveFree(se.elem, (cached ? se.moveOrigin : se.elem->value) + dif);
            }

            break;

        case RigidOrder:

            if (dif.x() <= 0) {
                for (int i = 0; i < c; ++i) {
                    SelectedElement& se = _selection.elems[i];
                    moveOrderRestricted(se.elem, (cached ? se.moveOrigin : se.elem->value) + dif);
                }
            } else {
                for (int i = _selection.count() - 1; i >= 0; --i) {
                    SelectedElement& se = _selection.elems[i];
                    moveOrderRestricted(se.elem, (cached ? se.moveOrigin : se.elem->value) + dif);
                }
            }

            break;
        }

        break;
    }

    case RigidSelection: {
        // reduce dif until acceptable by all nodes
        QPointF d(dif);
        for (int i = 0; i < c; ++i) {
            SelectedElement& se = _selection.elems[i];
            // if any node in selection is not editable, abort, since
            // we want to keep the selection form!
            if (!se.elem->editable)
                return;
            QPointF val0 = (cached ? se.moveOrigin : se.elem->value);
            QPointF val = val0 + d;
            if (_xOrder == NoOrder) {
                restrictValue(val);
            } else {
                orderRestrictValue(se.elem, val, false);
            }
            d = val - val0;
        }

        // if no dif left, do not bother moving
        if (d.isNull())
            return;

        // move all with the new dif
        for (int i = 0; i < c; ++i) {
            SelectedElement& se = _selection.elems[i];
            if (!se.elem->editable)
                continue;
            se.elem->value = (cached ? se.moveOrigin : se.elem->value) + d;
        }

        break;
    }
    }
}

void QcGraph::addCurve(QPainterPath& path, QcGraphElement* e1, QcGraphElement* e2) {
    QcGraphElement::CurveType type = e1->curveType;

    const QPointF& pt1 = e1->value;
    const QPointF& pt2 = e2->value;

    // coefficients for control points of cubic curve
    // approximating first quarter of sinusoid
    // technically: y = sin(pi*x/2) over x = [0,1]
    static const float ax = 1.0 / 3.0;
    static const float ay = 0.52359877f; // pi/6
    static const float bx = 2.0 / 3.0;
    static const float by = 1.0;

    switch (type) {
    case QcGraphElement::Step:
        path.moveTo(pt1);
        path.lineTo(pt1.x(), pt2.y());
        path.lineTo(pt2);
        break;
    case QcGraphElement::Hold:
        path.moveTo(pt1);
        path.lineTo(pt1.y(), pt2.y());
        path.lineTo(pt2);
        break;
    case QcGraphElement::Linear:
        path.moveTo(pt1);
        path.lineTo(pt2);
        break;

    case QcGraphElement::Quadratic: {
        path.moveTo(pt1);

        const qreal x1 = pt1.x();
        const qreal x2 = pt2.x();
        const qreal y1 = std::sqrt(pt1.y());
        const qreal y2 = std::sqrt(pt2.y());
        static const int n = 100;

        const qreal dx = x2 - x1;
        const qreal dy = y2 - y1;
        const qreal k = dx != 0.0 ? dy / dx : 0.0;
        const qreal a = y1 - x1 * k;
        const qreal kx = dx / n;

        for (int i = 1; i < n; ++i) {
            qreal x = i * kx + x1;
            qreal y = k * x + a;
            path.lineTo(x, y * y);
        }

        path.lineTo(pt2);

        break;
    }

    case QcGraphElement::Cubic: {
        path.moveTo(pt1);

        const qreal x1 = pt1.x();
        const qreal x2 = pt2.x();
        const qreal y1 = std::pow(pt1.y(), qreal(1 / 3.0));
        const qreal y2 = std::pow(pt2.y(), qreal(1 / 3.0));
        static const int n = 100;

        const qreal dx = x2 - x1;
        const qreal dy = y2 - y1;
        const qreal k = dx != 0.0 ? dy / dx : 0.0;
        const qreal a = y1 - x1 * k;
        const qreal kx = dx / n;

        for (int i = 1; i < n; ++i) {
            qreal x = i * kx + x1;
            qreal y = k * x + a;
            path.lineTo(x, y * y * y);
        }

        path.lineTo(pt2);

        break;
    }

    case QcGraphElement::Sine: {
        // half of difference between end points
        float dx = (pt2.x() - pt1.x()) * 0.5f;
        float dy = (pt2.y() - pt1.y()) * 0.5f;

        // middle point
        QPointF mid = pt1 + QPointF(dx, dy);

        path.moveTo(pt1);
        path.cubicTo(pt1 + QPointF(dx * (1 - bx), dy * (1 - by)), pt1 + QPointF(dx * (1 - ax), dy * (1 - ay)), mid);
        path.cubicTo(mid + QPointF(dx * ax, dy * ay), mid + QPointF(dx * bx, dy * by), pt2);

        break;
    }
    case QcGraphElement::Welch: {
        // difference between points
        float dx = (pt2.x() - pt1.x());
        float dy = (pt2.y() - pt1.y());

        path.moveTo(pt1);
        if (dy > 0)
            path.cubicTo(pt1 + QPointF(dx * ax, dy * ay), pt1 + QPointF(dx * bx, dy * by), pt2);
        else
            path.cubicTo(pt1 + QPointF(dx * (1 - bx), dy * (1 - by)), pt1 + QPointF(dx * (1 - ax), dy * (1 - ay)), pt2);

        break;
    }
    case QcGraphElement::Exponential: {
        // FIXME: find a Bezier curve approximation

        path.moveTo(pt1);

        float dy = (pt2.y() - pt1.y());

        // prevent NaN, optimize
        if (pt1.y() <= 0.f || pt2.y() <= 0.f) {
            path.lineTo(dy < 0 ? QPointF(pt1.x(), pt2.y()) : QPointF(pt2.x(), pt1.y()));
            path.lineTo(pt2);
        } else {
            static const int n = 100;
            const qreal x1 = pt1.x();
            const qreal x2 = pt2.x();
            const qreal y1 = pt1.y();
            const qreal y2 = pt2.y();

            const qreal kx = (x2 - x1) / n;
            const double ky = y1 != 0.0 ? std::pow(y2 / y1, 1.0 / n) : 1.f;
            double y = y1;

            for (int i = 1; i < n; ++i) {
                qreal x = i * kx + x1;
                y = y * ky;
                path.lineTo(x, y);
            }

            path.lineTo(pt2);
        }

        break;
    }
    case QcGraphElement::Curvature:

        // FIXME: find a Bezier curve approximation

        path.moveTo(pt1);

        // prevent NaN
        double curve = qBound(-100.0, e1->curvature, 100.0);

        if (std::abs(curve) < 0.0001) {
            path.lineTo(pt2);
        } else {
            float dx = (pt2.x() - pt1.x());
            float dy = (pt2.y() - pt1.y());
            double denom = 1.0 - exp(curve);
            const float n = 100.f;
            for (float ph = 1 / n; ph <= (1 - 1 / n); ph += 1 / n) {
                double numer = 1.0 - exp(ph * curve);
                qreal y = pt1.y() + dy * (numer / denom);
                path.lineTo(pt1.x() + (dx * ph), y);
            }
            path.lineTo(pt2);
        }
        break;
    }
}

QSize QcGraph::drawnElementSize(QcGraphElement* e) {
    if (_style == DotElements) {
        int s = qMin(e->size.width(), e->size.height());
        return QSize(s, s);
    } else
        return e->size;
}

QRect QcGraph::valueRect() {
    using namespace QtCollider::Style;

    if (_geometryDirty) {
        int w = 0;
        int h = 0;
        int c = _model.elementCount();
        if (_style == RectElements) {
            for (int i = 0; i < c; ++i) {
                QSize s = _model.elementAt(i)->size;
                w = qMax(w, s.width());
                h = qMax(h, s.height());
            }
        } else {
            for (int i = 0; i < c; ++i) {
                QSize s = _model.elementAt(i)->size;
                w = qMax(w, qMin(s.width(), s.height()));
            }
            h = w;
        }
        _largestThumbSize = QSize(w, h);
        _geometryDirty = false;
    }

    return marginsRect(sunkenContentsRect(rect()), _largestThumbSize).adjusted(1, 1, -1, -1);
}

QRectF QcGraph::labelRect(QcGraphElement* e, const QPointF& pt, const QRect& bounds, const QFontMetrics& fm) {
    QRectF textRect(fm.boundingRect(e->text));
    QSize sz(drawnElementSize(e));
    qreal hnd_w_2 = sz.width() * 0.5;
    qreal hnd_h_2 = sz.height() * 0.5;
    textRect.moveBottomLeft(pt + QPointF(hnd_w_2, -hnd_h_2));
    if (textRect.y() < bounds.y())
        textRect.moveTop(pt.y() + hnd_h_2);
    if (textRect.right() > bounds.right())
        textRect.moveRight(pt.x() - hnd_w_2);
    return textRect;
}

void QcGraph::paintEvent(QPaintEvent*) {
    using namespace QtCollider::Style;
    using QtCollider::Style::Ellipse;
    using QtCollider::Style::RoundRect;

    QPainter p(this);
    const QPalette& plt = palette();

    p.setRenderHint(QPainter::Antialiasing, true);
    RoundRect frame(rect(), 2);
    drawSunken(&p, plt, frame, background(), hasFocus() ? focusColor() : QColor());
    p.setRenderHint(QPainter::Antialiasing, false);

    QRect contentsRect(valueRect());

    // draw grid;

    p.setPen(gridColor());
    p.setBrush(Qt::NoBrush);
    p.drawRect(contentsRect);

    if (_gridOn) {
        float dx = _gridMetrics.x();
        float dy = _gridMetrics.y();
        float cl = contentsRect.left();
        float cr = contentsRect.right();
        float ct = contentsRect.top();
        float cb = contentsRect.bottom();

        if (contentsRect.width() > 0.f && dx > 0.f && dx < 1.f) {
            dx *= contentsRect.width();
            int i = 1;
            float x;
            while ((x = i * dx + cl) < cr) {
                p.drawLine(x, ct, x, cb);
                ++i;
            }
        }

        if (contentsRect.height() > 0.f && dy > 0.f && dy < 1.f) {
            dy *= contentsRect.height();
            int i = 1;
            float y;
            while ((y = cb - i * dy) > ct) {
                p.drawLine(cl, y, cr, y);
                ++i;
            }
        }
    }

    QList<QcGraphElement*> elems = _model.elements();

    int c = elems.count();
    if (!c)
        return;

    const QColor& strokeClr = strokeColor();
    QPen pen;
    pen.setColor(strokeClr);
    pen.setWidth(0);
    p.setPen(pen);

    // draw lines;
    if (_drawLines) {
        QPainterPath lines;
        QList<QcGraphModel::Connection> conns = _model.connections();

        if (conns.count()) {
            Q_FOREACH (QcGraphModel::Connection c, conns) { addCurve(lines, c.a, c.b); }

        } else {
            QcGraphElement* e1 = elems[0];
            int i;
            for (i = 1; i < c; ++i) {
                QcGraphElement* e2 = elems[i];
                addCurve(lines, e1, e2);
                e1 = e2;
            }
        }

        p.save();
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setBrush(Qt::NoBrush);
        p.translate(contentsRect.x(), contentsRect.y() + contentsRect.height());
        p.scale(contentsRect.width(), -contentsRect.height());
        p.drawPath(lines);
        p.restore();
    }

    // draw rects and strings
    if (_drawRects) {
        p.setRenderHint(QPainter::Antialiasing, true);

        QFontMetrics fm(font());
        QColor rectColor = plt.color(QPalette::Button).lighter(105);
        QColor circleColor = rectColor;
        circleColor.setAlpha(70);
        QColor dotColor = plt.color(QPalette::Text);
        const QColor& selectClr = selectionColor();
        QRectF rect;
        QPointF pt;
        int i;

        for (i = 0; i < c; ++i) {
            QcGraphElement* e = elems[i];

            rect.setSize(drawnElementSize(e));

            pt = Style::pos(e->value, contentsRect);
            rect.moveCenter(pt.toPoint());

            if (_style == DotElements)
                drawDotElement(e, rect, contentsRect, dotColor, circleColor, strokeClr, selectClr, plt, fm, &p);
            else
                drawRectElement(e, rect, rectColor, strokeClr, selectClr, plt, &p);
        }
    }
}

void QcGraph::drawDotElement(QcGraphElement* e, const QRectF& rect, const QRect& bounds, const QColor& dotColor,
                             const QColor& circleColor, const QColor& textColor, const QColor& selectColor,
                             const QPalette& plt, const QFontMetrics& fm, QPainter* p) {
    using namespace QtCollider::Style;
    using QtCollider::Style::Ellipse;

    // base
    Ellipse thumb(rect);
    drawRaised(p, plt, thumb, circleColor);

    // marker
    QRectF r(thumb._rect);
    qreal wdif = r.width() * 0.3;
    qreal hdif = r.height() * 0.3;
    p->setPen(Qt::NoPen);
    p->setBrush(e->fillColor.isValid() ? e->fillColor : dotColor);
    p->drawEllipse(r.adjusted(wdif, hdif, -wdif, -hdif));

    // selection indicator
    if (e->selected) {
        p->setBrush(Qt::NoBrush);
        p->setPen(selectColor);
        p->drawEllipse(thumb._rect.adjusted(1, 1, -1, -1));
    }

    // label
    p->setPen(textColor);
    QString text = e->text;
    if (!text.isEmpty()) {
        QRectF lblRect(labelRect(e, rect.center(), bounds, fm));
        p->drawText(lblRect, 0, text);
    }
}

void QcGraph::drawRectElement(QcGraphElement* e, const QRectF& rect, const QColor& fillColor, const QColor& textColor,
                              const QColor& selectColor, const QPalette& plt, QPainter* p) {
    using namespace QtCollider::Style;
    using QtCollider::Style::RoundRect;

    // base
    RoundRect base(rect, 5);
    drawRaised(p, plt, base, e->fillColor.isValid() ? e->fillColor : fillColor);

    // selection indicator
    if (e->selected) {
        p->setBrush(Qt::NoBrush);
        p->setPen(selectColor);
        p->drawRoundedRect(base._rect, 5, 5);
    }

    // label
    p->setPen(textColor);
    QString text = e->text;
    if (!text.isEmpty()) {
        p->drawText(rect, Qt::AlignCenter, text);
    }
}

void QcGraph::mousePressEvent(QMouseEvent* ev) {
    using namespace QtCollider::Style;

    QList<QcGraphElement*> elems = _model.elements();
    int c = elems.count();
    if (!c)
        return;

    QPointF mpos = ev->pos();
    QRectF valueRect(this->valueRect());

    QRectF r;

    int i;
    for (i = 0; i < c; ++i) {
        QcGraphElement* e = elems[i];
        r.setSize(drawnElementSize(e));
        QPointF pt(Style::pos(e->value, valueRect));
        r.moveCenter(pt);
        if (r.contains(mpos)) {
            if (ev->modifiers() & Qt::ShiftModifier) {
                setIndexSelected(i, !e->selected);
            } else {
                if (!e->selected) {
                    setAllDeselected();
                    setIndexSelected(i, true);
                }
            }

            _selection.cached = false;
            if (e->selected) {
                // if the element that was hit ended up selected
                // prepare for moving
                _selection.shallMove = true;
                _selection.moveOrigin = Style::value(mpos, valueRect);
            } else {
                _selection.shallMove = false;
            }

            update();
            return;
        }
    }

    _selection.shallMove = false;

    if (!(ev->modifiers() & Qt::ShiftModifier)) {
        setAllDeselected();
    }

    update();
}

void QcGraph::mouseMoveEvent(QMouseEvent* ev) {
    using namespace QtCollider::Style;

    if (!ev->buttons())
        return;

    if (!_editable || !_selection.shallMove || !_selection.size())
        return;

    if (!_selection.cached) {
        int c = _selection.count();
        for (int i = 0; i < c; ++i) {
            SelectedElement& se = _selection.elems[i];
            se.moveOrigin = se.elem->value;
        }
        _selection.cached = true;
    }

    QRectF valueRect(this->valueRect());
    QPointF dValue(Style::value(ev->pos(), valueRect));
    dValue = dValue - _selection.moveOrigin;

    moveSelected(dValue, _selectionForm, true);

    update();
    doAction(ev->modifiers());
}

void QcGraph::keyPressEvent(QKeyEvent* event) {
    int mods = event->modifiers();
    if (mods & Qt::AltModifier || mods & Qt::ShiftModifier) {
        // selection mode

        int c = _model.elementCount();
        if (!c)
            return;

        switch (event->key()) {
        case Qt::Key_Right: {
            // select the index after the last selected one (wrapping)
            // or extend selection to the right

            int i = -1;
            if (_selection.count()) {
                for (i = c - 1; i >= 0; --i) {
                    if (_model.elementAt(i)->selected)
                        break;
                }
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                i = qMin(i + 1, c - 1);
                setIndexSelected(i, true);
            } else {
                ++i;
                if (i >= c)
                    i = 0;
                setAllDeselected();
                setIndexSelected(i, true);
            }

            break;
        }
        case Qt::Key_Left: {
            // select the index before the first selected one (wrapping)
            // or extend selection to the left
            int i = c;
            if (_selection.count()) {
                for (i = 0; i < c; ++i) {
                    if (_model.elementAt(i)->selected)
                        break;
                }
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                i = qMax(i - 1, 0);
                setIndexSelected(i, true);
            } else {
                --i;
                if (i < 0)
                    i = c - 1;
                setAllDeselected();
                setIndexSelected(i, true);
            }

            break;
        }
        default:
            break;
        }
    } else {
        // editing mode

        if (!_editable || !_selection.size())
            return;

        QPointF dValue;

        switch (event->key()) {
        case Qt::Key_Up:
            dValue.setY(_step);
            break;
        case Qt::Key_Down:
            dValue.setY(-_step);
            break;
        case Qt::Key_Right:
            dValue.setX(_step);
            break;
        case Qt::Key_Left:
            dValue.setX(-_step);
            break;
        default:
            return;
        }

        moveSelected(dValue, _selectionForm, false);

        update();
        doAction(event->modifiers());
    }
}

void QcGraph::doAction(Qt::KeyboardModifiers mods) {
    Qt::KeyboardModifier ctrlMod =
#ifdef Q_OS_MAC
        Qt::MetaModifier;
#else
        Qt::ControlModifier;
#endif

    if (mods & ctrlMod)
        Q_EMIT(metaAction());
    else
        Q_EMIT(action());
}
