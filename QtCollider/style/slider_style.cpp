#include "style.hpp"
#include "routines.hpp"
#include "../widgets/QcSlider.h"

#include <QPainter>

namespace QtCollider { namespace Style {

QRect Slider::sc_groove(const QStyleOption* opt, const QcSlider*) { return sunkenContentsRect(opt->rect); }

QRect Slider::sc_handle(const QStyleOptionSlider* opt, const QcSlider* s) {
    QRect contRect(sunkenContentsRect(opt->rect));

    double valRange = opt->maximum - opt->minimum;
    double val = valRange > 0.0 ? (opt->sliderPosition - opt->minimum) / valRange : 0.0;

    int len = s->handleLength();

    QRect r;
    if (opt->orientation == Qt::Horizontal) {
        int pos = val * (contRect.width() - len);
        r.setX(pos + contRect.left());
        r.setY(contRect.top());
        r.setSize(QSize(len, contRect.height()));
    } else {
        int pos = val * (contRect.height() - len);
        r.setX(contRect.left());
        r.setY(contRect.bottom() - pos - len + 1);
        r.setSize(QSize(contRect.width(), len));
    }
    return r;
}

void Slider::cc_draw(const QStyleOptionSlider* opt, QPainter* p, const QcSlider* slider) {
    using QtCollider::Style::Ellipse;
    using QtCollider::Style::RoundRect;

    const QPalette& plt = opt->palette;

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    QRect rGroove = opt->rect;

    // draw groove
    RoundRect shGroove(rGroove, 2);

    QColor baseColor(slider->grooveColor());
    QColor focusColor;
    if (opt->state & QStyle::State_HasFocus)
        focusColor = slider->focusColor();

    drawSunken(p, plt, shGroove, baseColor, focusColor);

    // geometry
    QRect rHandle(sc_handle(opt, slider));

    // draw handle
    RoundRect shHandle(rHandle, 2);
    drawRaised(p, plt, shHandle, plt.color(QPalette::Button).lighter(105));

    p->restore();

    // draw marker
    QPen pen(plt.color(QPalette::ButtonText));
    pen.setWidth(2);
    p->setPen(pen);
    if (opt->orientation == Qt::Horizontal) {
        qreal center = rHandle.center().x() + 1;
        QLine line(center, rHandle.top() + 3, center, rHandle.bottom() - 2);
        p->drawLine(line);
        pen.setColor(plt.color(QPalette::Light));
    } else {
        qreal center = rHandle.center().y() + 1;
        QLine line(rHandle.left() + 3, center, rHandle.right() - 2, center);
        p->drawLine(line);
        pen.setColor(plt.color(QPalette::Light));
    }
}

} // namespace Style
} // namespace QtCollider
