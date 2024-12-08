/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#pragma once

#include "../Common.h"
#include "../QObjectProxy.h"
#include "../safeptr.hpp"

#include <VMGlobals.h>
#include <PyrObject.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>

using namespace QtCollider;

class QcTreeWidget : public QTreeWidget {
public:
    class Item : public QTreeWidgetItem {
    public:
        enum { Type = QTreeWidgetItem::UserType };
        Item(): QTreeWidgetItem(Item::Type), _safePtr(this) {}
        Item(const QStringList& strings): QTreeWidgetItem(strings, Item::Type), _safePtr(this) {}
        ~Item() { _safePtr.invalidate(); }
        SafePtr<Item> safePtr() const { return _safePtr; }
        static SafePtr<Item> safePtr(QTreeWidgetItem*);
        static void initialize(VMGlobals*, PyrObject*, const SafePtr<Item>&);
        static int finalize(VMGlobals*, PyrObject*);
        bool operator<(const QTreeWidgetItem& other) const;

    private:
        SafePtr<Item> _safePtr;
    };

    typedef SafePtr<Item> ItemPtr;

    Q_OBJECT
    Q_PROPERTY(QVariantList columns READ columns WRITE setColumns)
    Q_PROPERTY(QtCollider::SafePtr<QcTreeWidget::Item> currentItem READ currentItem WRITE setCurrentItem);

public:
    Q_INVOKABLE QtCollider::SafePtr<QcTreeWidget::Item> item(const QtCollider::SafePtr<QcTreeWidget::Item>& parent,
                                                             int index);
    Q_INVOKABLE QtCollider::SafePtr<QcTreeWidget::Item> parentItem(const QtCollider::SafePtr<QcTreeWidget::Item>&);
    Q_INVOKABLE int indexOfItem(const QtCollider::SafePtr<QcTreeWidget::Item>&);

    Q_INVOKABLE QtCollider::SafePtr<QcTreeWidget::Item> addItem(const QtCollider::SafePtr<QcTreeWidget::Item>& parent,
                                                                const QVariantList& data);

    Q_INVOKABLE QtCollider::SafePtr<QcTreeWidget::Item>
    insertItem(const QtCollider::SafePtr<QcTreeWidget::Item>& parent, int index, const QVariantList& data);

    Q_INVOKABLE void removeItem(const QtCollider::SafePtr<QcTreeWidget::Item>&);

    Q_INVOKABLE QVariantList strings(const QtCollider::SafePtr<QcTreeWidget::Item>&);
    Q_INVOKABLE void setText(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column, const QString&);
    Q_INVOKABLE void setColor(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column, const QColor&);
    Q_INVOKABLE void setTextColor(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column, const QColor&);

    Q_INVOKABLE QWidget* itemWidget(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column);
    Q_INVOKABLE void setItemWidget(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column, QWidget*);
    Q_INVOKABLE void removeItemWidget(const QtCollider::SafePtr<QcTreeWidget::Item>&, int column);

    Q_INVOKABLE void sort(int column, bool descending);

    Q_INVOKABLE int columnWidth(int column);
    Q_INVOKABLE void setColumnWidth(int column, int width);

Q_SIGNALS:

    void action();
    void itemPressedAction();
    void currentItemChanged();
    void expanded(QcTreeWidget::ItemPtr);
    void collapsed(QcTreeWidget::ItemPtr);

public:
    QcTreeWidget();

    ItemPtr currentItem() const;
    void setCurrentItem(const ItemPtr&);

    QVariantList columns() const;
    void setColumns(const QVariantList&);

public Q_SLOTS:
    void onExpanded(QTreeWidgetItem*);
    void onCollapsed(QTreeWidgetItem*);

protected:
    virtual void keyPressEvent(QKeyEvent*);

private:
    QTreeWidgetItem* _itemOnPress;
    bool _emitAction;
};

Q_DECLARE_METATYPE(QtCollider::SafePtr<QcTreeWidget::Item>);
