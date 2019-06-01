/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
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

#pragma once

#include <QListWidget>
#include <QStyledItemDelegate>
#include <QPainter>

namespace ScIDE {

class IconListWidget : public QListWidget {
public:
    class ItemDelegate : public QStyledItemDelegate {
    public:
        ItemDelegate(QObject* parent): QStyledItemDelegate(parent) {}

        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
            QIcon icon(index.data(Qt::DecorationRole).value<QIcon>());
            QSize iconSize = icon.actualSize(option.decorationSize);

            QString text = index.data(Qt::DisplayRole).toString();
            QFontMetrics fm(option.font);

            int fontWidth = fm.width(text);

            QSize requiredSize(qMax(fontWidth, iconSize.width()), fm.height() + iconSize.height());

            return requiredSize + QSize(5, 5);
        }

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
            painter->save();

            QStyle* style = QApplication::style();

            style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

            QRect r(option.rect.adjusted(0, 5, 0, -5));

            QIcon icon(index.data(Qt::DecorationRole).value<QIcon>());
            if (!icon.isNull())
                style->drawItemPixmap(painter, r, Qt::AlignTop | Qt::AlignHCenter, icon.pixmap(option.decorationSize));

            QString text = index.data(Qt::DisplayRole).toString();
            if (!text.isEmpty()) {
                int alignment = !icon.isNull() ? Qt::AlignBottom | Qt::AlignHCenter : Qt::AlignCenter;
                painter->setFont(option.font);
                style->drawItemText(painter, r, alignment, option.palette, true, text);
            }

            painter->restore();
        }
    };

public:
    IconListWidget(QWidget* parent = 0): QListWidget(parent) { setItemDelegate(new ItemDelegate(this)); }

    virtual QStyleOptionViewItem viewOptions() const {
        QStyleOptionViewItem opt(QListWidget::viewOptions());
        opt.displayAlignment = Qt::AlignCenter;
        opt.decorationAlignment = Qt::AlignCenter;
        opt.decorationPosition = QStyleOptionViewItem::Top;
        opt.font.setBold(true);
        return opt;
    }
};

} // namespace ScIDE
