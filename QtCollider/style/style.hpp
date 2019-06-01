#pragma once

#include "ProxyStyle.hpp"
#include "routines.hpp"

#include <QStyleOptionComplex>

class QcSlider;

namespace QtCollider { namespace Style {

enum ColorRole {
    Focus = 0,
    Groove,

    ColorRoleCount
};

class StyleImpl : public ProxyStyle {
    Q_OBJECT

public:
    StyleImpl(QStyle* s = 0): ProxyStyle(s) {}

    virtual void polish(QPalette&);

    QColor color(ColorRole role) {
        Q_ASSERT(role < ColorRoleCount);
        return _colors[role];
    }

private:
    QColor _colors[ColorRoleCount];
};

struct Client {
    Client(QWidget* w): _widget(w) {}

    QColor focusColor() const { return color(Focus); }
    void setFocusColor(const QColor& c) { setColor(Focus, c); }

    QColor grooveColor() const { return color(Groove); }
    void setGrooveColor(const QColor& c) { setColor(Groove, c); }

    QColor color(ColorRole role) const {
        Q_ASSERT(role < ColorRoleCount);
        QColor c(_colors[role]);
        if (c.isValid())
            return c;
        else {
            StyleImpl* style = qobject_cast<StyleImpl*>(_widget->style());
            return style ? style->color(role) : QColor();
        }
    }

    void setColor(ColorRole role, const QColor& color) {
        Q_ASSERT(role < ColorRoleCount);
        _colors[role] = color;
        _widget->update();
    }

private:
    QColor _colors[ColorRoleCount];
    QWidget* _widget;
};

} // namespace QtCollider
} // namespace Style
