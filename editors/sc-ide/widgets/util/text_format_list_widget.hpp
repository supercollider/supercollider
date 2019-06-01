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

#include <QTreeView>
#include <QStyledItemDelegate>
#include <QTextCharFormat>
#include <QStandardItemModel>

namespace ScIDE {

class TextFormatListWidget : public QTreeView {
    Q_OBJECT

public:
    class ItemDelegate : public QStyledItemDelegate {
    public:
        ItemDelegate(QObject* parent);

        virtual QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;

        virtual void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const;
    };

public:
    TextFormatListWidget(QWidget* parent);

    void addFormat(const QString& name, const QTextCharFormat& format);

    void setFormat(int index, const QTextCharFormat& format);

    inline void removeFormat(int index) { standardModel()->removeRow(index); }

    int currentIndex() {
        QModelIndex index(QAbstractItemView::currentIndex());
        if (index.isValid())
            return index.row();
        else
            return -1;
    }

    QString name(int index);

    QTextCharFormat format(int index);

    inline int count() { return standardModel()->rowCount(); }

Q_SIGNALS:
    void clicked(int index);

private Q_SLOTS:
    void onDoubleClicked(const QModelIndex&);

private:
    QStandardItem* makeBrushItem(const QBrush& brush);

    QStandardItem* makeBoolItem(bool b);

    inline QStandardItemModel* standardModel() { return qobject_cast<QStandardItemModel*>(model()); }
};

} // namespace ScIDE
