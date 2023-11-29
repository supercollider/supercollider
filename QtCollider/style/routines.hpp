#pragma once

#include <QPainter>
#include <QPalette>

namespace QtCollider { namespace Style {
#if 0
namespace ReliefType {
  struct Hover;
  struct Raised;
  struct Flat;
  struct Sunken;
};
using namespace ReliefType;

namespace ShapeType {
  struct Rect;
  struct RoundRect;
  struct LeftRoundRect;
  struct RightRoundRect;
  struct TopRoundRect;
  struct BottomRoundRect;
  struct Ellipse;
};
using namespace ShapeType;

// drawShape //

template<typename Shape>
void drawShape( QPainter *, const QRect & );

template<typename Shape>
void drawShape( QPainter *, const QRect &, qreal param1 );

template<>
void drawShape <Rect> ( QPainter *p, const QRect &r )
{
  p->drawRect(r);
}

template<>
void drawShape <Ellipse> ( QPainter *p, const QRect &r )
{
  p->drawEllipse(r);
}

template<>
void drawShape <RoundRect> ( QPainter *p, const QRect &r, qreal radius )
{
  p->drawRoundedRect( r, radius, radius, Qt::RelativeSize );
}

// drawReliefShape //

template<typename ShapeT> struct Shape
{
  template<typename RectT> void draw();
};
#endif

struct Rect {
    Rect(QRectF rect): _rect(rect) {}
    template <typename RectT> inline void draw(QPainter* p, const RectT& r) const { p->drawRect(r); }
    QRectF _rect;
};

struct RoundRect {
    RoundRect(QRectF rect, qreal radius): _rect(rect), _radius(radius) {}
    template <typename RectT> inline void draw(QPainter* p, const RectT& r) const {
        p->drawRoundedRect(r, _radius, _radius, Qt::AbsoluteSize);
    }
    QRectF _rect;
    qreal _radius;
};

struct Ellipse {
    Ellipse(QRectF rect): _rect(rect) {}
    template <typename RectT> inline void draw(QPainter* p, const RectT& r) const { p->drawEllipse(r); }
    QRectF _rect;
};

template <typename ShapeT>
void drawRaised(QPainter* p, const QPalette& plt, const ShapeT& shape, const QColor& color,
                const QColor& focusColor = QColor()) {
    p->save();

    bool focus = focusColor.isValid();

    QRectF r(shape._rect);

    p->setBrush(Qt::NoBrush);

    // focus indication

    QPen pen;
    pen.setWidth(2);
    if (focus) {
        pen.setColor(focusColor);
        p->setPen(pen);
        shape.draw(p, r.adjusted(1, 1, -1, -1));
        r.adjust(2, 2, -2, -2);
    } else {
        r.adjust(1, 1, -1, -1);
    }

    // edge

    QColor edgeClr(focus ? plt.color(QPalette::Light) : plt.color(QPalette::Dark));
    if (focus)
        edgeClr.setAlpha(100);
    else
        edgeClr.setAlpha(color.alpha());
    pen.setColor(edgeClr);
    p->setPen(pen);
    shape.draw(p, r);

    // center

    double pos = shape._rect.height();
    pos = (pos - 3) / pos;

    QLinearGradient lgrad(shape._rect.topLeft(), shape._rect.bottomLeft());
    lgrad.setColorAt(0, color.lighter(110));
    lgrad.setColorAt(pos, color.darker(105));
    lgrad.setColorAt(1, color.darker(115));

    p->setPen(Qt::NoPen);
    p->setBrush(QBrush(lgrad));
    shape.draw(p, r);

    p->restore();
}

template <typename ShapeT>
void drawSunken(QPainter* p, const QPalette& plt, const ShapeT& shape, const QColor& color,
                const QColor& focusColor = QColor()) {
    p->save();

    QPen pen;
    pen.setWidth(2);

    // outer border

    QColor c = focusColor.isValid() ? focusColor : QColor(255, 255, 255, 20);

    pen.setColor(c);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    shape.draw(p, shape._rect.adjusted(1, 1, -1, -1));

    // inner border

    QColor c1(plt.color(QPalette::Shadow));
    c1.setAlpha(180);
    pen.setColor(c1);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    shape.draw(p, shape._rect.adjusted(2, 2, -2, -2));

    // shadow

    QColor c2 = color;
    c2.setAlpha(150);
    p->setPen(Qt::NoPen);
    p->setBrush(c2);
    shape.draw(p, shape._rect.adjusted(2, 2, -2, -2));

    // background

    p->setBrush(color);
    shape.draw(p, shape._rect.adjusted(2, 3, -2, -2));

    p->restore();
}

template <typename RectT> RectT sunkenContentsRect(const RectT& r) { return r.adjusted(1, 1, -1, -1); }

template <typename RectT> qreal xValue(qreal x, const RectT& bounds) { return (x - bounds.x()) / bounds.width(); }

template <typename RectT> qreal yValue(qreal y, const RectT& bounds) {
    return (bounds.y() + bounds.height() - y) / bounds.height();
}

template <typename RectT, typename SizeT> qreal xValue(qreal x, const RectT& bounds, const SizeT& margin) {
    return margin.width() < bounds.width()
        ? (x - bounds.x() - (margin.width() * 0.5)) / (bounds.width() - margin.width())
        : 0.0;
}

template <typename RectT, typename SizeT> qreal yValue(qreal y, const RectT& bounds, const SizeT& margin) {
    return margin.height() < bounds.height()
        ? (bounds.y() + bounds.height() - y - (margin.height() * 0.5)) / (bounds.height() - margin.height())
        : 0.0;
}

template <typename RectT, typename PointT> QPointF value(const PointT& pos, const RectT& bounds) {
    return QPointF(xValue(pos.x(), bounds), yValue(pos.y(), bounds));
}

template <typename RectT, typename SizeT, typename PointT>
QPointF value(const PointT& pos, const RectT& bounds, const SizeT& margin) {
    return QPointF(xValue(pos.x(), bounds, margin), yValue(pos.y(), bounds, margin));
}

template <typename RectT, typename SizeT> RectT marginsRect(const RectT& bounds, const SizeT& margin) {
    RectT r;
    r.setSize(SizeT(qMax((qreal)bounds.width() - margin.width(), qreal(0.0)),
                    qMax((qreal)bounds.height() - margin.height(), qreal(0.0))));
    r.moveCenter(bounds.center());
    return r;
}

template <typename RectT, typename SizeT> RectT rect(const QPointF& value, const RectT& bounds, const SizeT& size) {
    qreal x = value.x() * (bounds.width() - size.width()) + bounds.left();
    qreal y = bounds.top() + bounds.height() - size.height() - value.y() * (bounds.height() - size.height());
    return RectT(x, y, size.width(), size.height());
}

template <typename RectT, typename SizeT> QPointF pos(const QPointF& value, const RectT& bounds, const SizeT& margin) {
    qreal x = value.x() * (bounds.width() - margin.width()) + bounds.left() + margin.x() * 0.5;
    qreal y = -value.y() * (bounds.height() - margin.height()) + bounds.top() + bounds.height() - margin.y() * 0.5;
    return QPointF(x, y);
}

template <typename RectT> QPointF pos(const QPointF& value, const RectT& bounds) {
    qreal x = value.x() * bounds.width() + bounds.left();
    qreal y = -value.y() * bounds.height() + bounds.top() + bounds.height();
    return QPointF(x, y);
}

} // namespace QtCollider
} // namespace Style
