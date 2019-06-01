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
    Q_PROPERTY(QcTreeWidget::ItemPtr currentItem READ currentItem WRITE setCurrentItem);

public:
    Q_INVOKABLE QcTreeWidget::ItemPtr item(const QcTreeWidget::ItemPtr& parent, int index);
    Q_INVOKABLE QcTreeWidget::ItemPtr parentItem(const QcTreeWidget::ItemPtr&);
    Q_INVOKABLE int indexOfItem(const QcTreeWidget::ItemPtr&);

    Q_INVOKABLE QcTreeWidget::ItemPtr addItem(const QcTreeWidget::ItemPtr& parent, const QVariantList& data);

    Q_INVOKABLE QcTreeWidget::ItemPtr insertItem(const QcTreeWidget::ItemPtr& parent, int index,
                                                 const QVariantList& data);

    Q_INVOKABLE void removeItem(const QcTreeWidget::ItemPtr&);

    Q_INVOKABLE QVariantList strings(const QcTreeWidget::ItemPtr&);
    Q_INVOKABLE void setText(const QcTreeWidget::ItemPtr&, int column, const QString&);
    Q_INVOKABLE void setColor(const QcTreeWidget::ItemPtr&, int column, const QColor&);
    Q_INVOKABLE void setTextColor(const QcTreeWidget::ItemPtr&, int column, const QColor&);

    Q_INVOKABLE QWidget* itemWidget(const QcTreeWidget::ItemPtr&, int column);
    Q_INVOKABLE void setItemWidget(const QcTreeWidget::ItemPtr&, int column, QWidget*);
    Q_INVOKABLE void removeItemWidget(const QcTreeWidget::ItemPtr&, int column);

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

Q_DECLARE_METATYPE(QcTreeWidget::ItemPtr);
