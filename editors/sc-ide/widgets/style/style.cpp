/*
    SuperCollider Qt IDE
    Copyright (c) 2010-2012 Jakob Leben
    Copyright (c) 2012 Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "style.hpp"
#include "../../core/util/color.hpp"

#include <cmath>

#include <QPainter>
#include <QStyleOption>
#include <QDockWidget>
#include <QStyleOptionTabV3>
#include <QTabBar>
#include <QToolButton>
#include <QLayout>
#include <QDebug>
#include <QWebEngineView>

namespace ScIDE {

inline bool Style::shouldNotHandle(const QWidget* widget) const {
    return widget ? widget->window()->windowType() == Qt::Dialog : false;
}

void Style::polish(QWidget* widget) {
    if (shouldNotHandle(widget)) {
        QProxyStyle::polish(widget);
        widget->update();
        widget->updateGeometry();
        if (widget->layout())
            widget->layout()->invalidate();
        return;
    }

    if (qobject_cast<QDockWidget*>(widget)) {
        return;
    }
    if ((qobject_cast<QTabBar*>(widget)) || (qobject_cast<QToolButton*>(widget))) {
        widget->setAttribute(Qt::WA_Hover, true);
    } else
        QProxyStyle::polish(widget);
}

void Style::unpolish(QWidget* widget) { QProxyStyle::unpolish(widget); }

void Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter,
                               const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    switch (control) {
    // FIXME: this is a workaround for the WebKit bug #104116 (or a variation on it).
    case QStyle::CC_ScrollBar: {
        if (qobject_cast<const QWebEngineView*>(widget) != 0 && option->type == QStyleOption::SO_Slider) {
            // WebKit tries to hide scrollbars, but mistakenly hides QWebView - NULL-ify styleObject to prevent.
            const QStyleOptionSlider* optSlider = static_cast<const QStyleOptionSlider*>(option);
            QStyleOptionSlider opt2(*optSlider);
            opt2.styleObject = NULL;

            QProxyStyle::drawComplexControl(control, &opt2, painter, widget);
            return;
        }
    }
    case QStyle::CC_ToolButton: {
        // TODO: We only draw either text, or icon, or arrow

        const QToolButton* toolBtn = qobject_cast<const QToolButton*>(widget);
        if (!toolBtn)
            break;

        Q_ASSERT(toolBtn);

        const QStyleOptionToolButton* toolOption = static_cast<const QStyleOptionToolButton*>(option);

        painter->save();

        QRect r = option->rect;

        if (option->state & QStyle::State_On) {
            painter->setBrush(option->palette.color(QPalette::Dark));
            painter->setPen(option->palette.color(QPalette::Shadow));
            painter->drawRect(r.adjusted(0, 0, -1, -1));
        } else {
            bool highlight = option->state & QStyle::State_MouseOver;

            if (highlight) {
                QColor fill = option->palette.color(QPalette::Button);
                painter->setBrush(fill);
                painter->setPen(Qt::NoPen);
                painter->drawRect(r.adjusted(0, 0, 0, -1));
            }

            if (qobject_cast<QTabBar*>(toolBtn->parent())) {
                if (!highlight) {
                    QColor fill = option->palette.color(QPalette::Mid);
                    painter->setBrush(fill);
                    painter->setPen(Qt::NoPen);
                    painter->drawRect(r.adjusted(0, 0, 0, -1));
                }
                if (toolBtn->arrowType() == Qt::LeftArrow) {
                    painter->setPen(option->palette.color(QPalette::Shadow));
                    painter->drawLine(option->rect.topLeft(), option->rect.bottomLeft());
                }
            }
        }

        painter->restore();

        QIcon icon = toolOption->icon;
        if (!icon.isNull()) {
            QIcon::Mode iconMode = option->state & QStyle::State_Enabled
                ? (option->state & QStyle::State_MouseOver ? QIcon::Active : QIcon::Normal)
                : QIcon::Disabled;

            QIcon::State iconState = option->state & QStyle::State_Selected ? QIcon::On : QIcon::Off;

            QPixmap pixmap = icon.pixmap(toolOption->iconSize, iconMode, iconState);
            QRect pixRect = pixmap.rect();
            pixRect.moveCenter(option->rect.center());
            painter->drawPixmap(pixRect.topLeft(), pixmap);
        } else {
            QStyle::PrimitiveElement elem = Style::PE_CustomBase;
            switch (toolBtn->arrowType()) {
            case Qt::LeftArrow:
                elem = PE_IndicatorArrowLeft;
                break;
            case Qt::RightArrow:
                elem = PE_IndicatorArrowRight;
                break;
            case Qt::DownArrow:
                elem = PE_IndicatorArrowDown;
                break;
            case Qt::UpArrow:
                elem = PE_IndicatorArrowUp;
                break;
            default:
                break;
            }
            if (elem != Style::PE_CustomBase) {
                drawPrimitive(elem, option, painter, widget);
            } else if (!toolOption->text.isEmpty()) {
                painter->drawText(toolOption->rect, Qt::AlignCenter | Qt::TextShowMnemonic, toolOption->text);
            }
        }
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void Style::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                        const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        QProxyStyle::drawControl(element, option, painter, widget);
        return;
    }

    switch (element) {
    case QStyle::CE_TabBarTab: {
        const QStyleOptionTab* tabOption = static_cast<const QStyleOptionTab*>(option);

        painter->save();

        bool selected = tabOption->state & QStyle::State_Selected;
        bool mouseOver = tabOption->state & QStyle::State_MouseOver;

        QColor background_color;
        if (selected) {
            background_color = option->palette.color(QPalette::Window);
        } else {
            background_color = option->palette.color(QPalette::Mid);
            if (mouseOver) {
                background_color = color::lighten(background_color, 10);
            }
        }

        QColor text_color = option->palette.color(QPalette::WindowText);
        if (!selected) {
            text_color = color::interpolate(text_color, background_color, kDeselectedTabBlend);
        }

        // Draw tab rectangle.
        QRect rect = tabOption->rect;
        painter->setBrush(background_color);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        // For inactive tabs, add a thin dark line to the right side for visual
        // separation.
        if (!selected && !(tabOption->selectedPosition == QStyleOptionTab::NextIsSelected)
            && !(tabOption->position == QStyleOptionTab::End)
            && !(tabOption->position == QStyleOptionTab::OnlyOneTab)) {
            QPen pen(option->palette.color(QPalette::Shadow), 1);
            painter->setPen(pen);
            painter->drawLine(rect.topRight(), rect.bottomRight());
        }

        // If the contrast between active and inactive tabs is stupidly low,
        // then use an overline to distinguish them. This happens when the user
        // sets a really dark background color.
        int window_mid_value_difference =
            abs(option->palette.color(QPalette::Window).value() - option->palette.color(QPalette::Mid).value());

        if (selected && window_mid_value_difference < 20) {
            painter->setBrush(text_color);
            painter->drawRect(rect.left(), rect.top(), rect.width(), 2);
        }

        painter->restore();

        // Draw icon.

        if (!tabOption->icon.isNull()) {
            QPixmap pixmap = tabOption->icon.pixmap(tabOption->iconSize);
            QRect iconRect = pixmap.rect();
            iconRect.moveCenter(tabOption->rect.center());
            int lmargin = 5;
            if (tabOption->leftButtonSize.width() > 0)
                lmargin += tabOption->leftButtonSize.width() + 4;
            iconRect.moveLeft(tabOption->rect.left() + lmargin);
            painter->drawPixmap(iconRect, pixmap);
        }

        // Draw text.

        QRect textRect = subElementRect(QStyle::SE_TabBarTabText, option, widget);

        painter->save();
        if (selected) {
            QFont font;
            font.setBold(true);
            painter->setFont(font);
        }
        painter->setPen(text_color);
        painter->drawText(textRect, Qt::AlignCenter | Qt::TextShowMnemonic, tabOption->text);
        painter->restore();

        return;
    }
    case CE_Splitter:
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(option->palette.color(QPalette::Shadow));
        painter->drawRect(option->rect);
        painter->restore();
        return;
    default:
        break;
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                          const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        return;
    }

    switch (element) {
    case QStyle::PE_IndicatorTabTear:
        return;
    case QStyle::PE_FrameTabBarBase: {
        const QTabBar* tabBar = qobject_cast<const QTabBar*>(widget);

        painter->save();

        painter->setPen(Qt::NoPen);

        if (tabBar) {
            painter->setBrush(option->palette.color(QPalette::Mid));
            painter->drawRect(tabBar->rect());
        }

        painter->setBrush(option->palette.color(QPalette::Shadow));
        painter->drawRect(option->rect);

        painter->restore();
        return;
    }
    case PE_IndicatorDockWidgetResizeHandle:
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(option->palette.color(QPalette::Shadow));
        painter->drawRect(option->rect);
        painter->restore();
        return;
    case PE_IndicatorTabClose: {
        QPoint center = option->rect.center();
        int rect_width = option->rect.width();
        // a = half of the width of the X.
        int a = rect_width * 0.5f * 0.4f;
        float cx = center.x() + 0.5f;
        float cy = center.y() + 0.5f;

        QColor x_color = option->palette.color(QPalette::WindowText);
        if (!(option->state & QStyle::State_Selected)) {
            QColor background = option->palette.color(QPalette::Mid);
            x_color = color::interpolate(x_color, background, kDeselectedTabBlend);
        }
        if ((option->state & State_Enabled) && (option->state & State_MouseOver)) {
            x_color = color::lighten(x_color, 40);
        }

        float thickness = 1.5f;
        // Sometimes this needs adjusting for symmetrical results on different
        // line thicknesses.
        float extend = 0.0f;

        QPen pen(x_color, thickness);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(pen);
        painter->drawLine(QLineF(cx - a, cy - a, cx + a + extend, cy + a + extend));
        painter->drawLine(QLineF(cx - a, cy + a, cx + a + extend, cy - a - extend));
        painter->restore();
        return;
    }
    default:
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}

QRect Style::subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        return QProxyStyle::subElementRect(element, option, widget);
    }

    switch (element) {
    // NOTE: Assuming horizontal tab bar direction
    case QStyle::SE_TabBarTabRightButton: {
        QRect r;
        r.setSize(QSize(16, 16));
        r.moveCenter(option->rect.center());
        r.moveRight(option->rect.right() + 1 - 5);
        return r;
    }
    case QStyle::SE_TabBarTabLeftButton: {
        QRect r;
        r.setSize(QSize(16, 16));
        r.moveCenter(option->rect.center());
        r.moveLeft(option->rect.left() + 5);
        return r;
    }
    case QStyle::SE_TabBarTabText: {
        const auto* tabOption = static_cast<const QStyleOptionTab*>(option);

        int lMargin = 5;
        if (tabOption->leftButtonSize.width() > 0)
            lMargin += tabOption->leftButtonSize.width() + 4;
        if (!tabOption->icon.isNull())
            lMargin += tabOption->iconSize.width() + 4;

        int rMargin = 5;
        if (tabOption->rightButtonSize.width() > 0)
            rMargin += tabOption->rightButtonSize.width() + 4;

        QRect r = option->rect;
        r.adjust(lMargin, 0, -rMargin, 0);
        return r;
    }
    default:
        return QProxyStyle::subElementRect(element, option, widget);
    }
}

QSize Style::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize,
                              const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
    }

    switch (type) {
    case QStyle::CT_TabBarTab:
        return contentsSize; //+ QSize(10, 10);
    case QStyle::CT_ToolButton:
        return contentsSize + QSize(10, 10);
    default:
        return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
    }
}

int Style::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const {
    if (shouldNotHandle(widget)) {
        return QProxyStyle::pixelMetric(metric, option, widget);
    }

    switch (metric) {
    case QStyle::PM_DockWidgetFrameWidth:
        return 2;
    case QStyle::PM_DockWidgetSeparatorExtent:
    case QStyle::PM_SplitterWidth:
        return 1;
    case QStyle::PM_TabBarBaseHeight:
        return 0;
    case QStyle::PM_TabBarBaseOverlap:
        return 1;
    case QStyle::PM_TabBarTabShiftHorizontal:
    case QStyle::PM_TabBarTabShiftVertical:
        return 0;
    case QStyle::PM_TabBarTabHSpace:
    case QStyle::PM_TabBarTabVSpace:
        return 10;
    case QStyle::PM_TabBarTabOverlap:
        return 0;
    case QStyle::PM_TabBarIconSize:
        return 16;
    case PM_TabCloseIndicatorHeight:
    case PM_TabCloseIndicatorWidth:
        return 16;
    case PM_TabBarScrollButtonWidth:
        return 24;
    case PM_TabBar_ScrollButtonOverlap:
        return 1;
    case PM_MenuButtonIndicator:
        return 0;
    default:
        break;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int Style::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget,
                     QStyleHintReturn* returnData) const {
    if (shouldNotHandle(widget)) {
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }

    switch (hint) {
    case SH_TabBar_PreferNoArrows:
        return 0;
    case SH_TabBar_ElideMode:
        return Qt::ElideNone;
    default:
        break;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

} // namespace ScIDE
