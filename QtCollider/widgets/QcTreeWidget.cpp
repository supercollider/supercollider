/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcTreeWidget.h"
#include "../QcWidgetFactory.h"

#include <PyrKernel.h>

#include <QKeyEvent>

class QcTreeWidgetFactory : public QcWidgetFactory<QcTreeWidget> {
    void initialize(QWidgetProxy* p, QcTreeWidget* w) { p->setMouseEventWidget(w->viewport()); }
};

QC_DECLARE_FACTORY(QcTreeWidget, QcTreeWidgetFactory);

QcTreeWidget::QcTreeWidget() {
    setAttribute(Qt::WA_AcceptTouchEvents);

    // Forward signals to argument-less versions connectable from SC.
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SIGNAL(action()));
    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SIGNAL(itemPressedAction()));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SIGNAL(currentItemChanged()));

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onCollapsed(QTreeWidgetItem*)));
}

QVariantList QcTreeWidget::columns() const {
    QVariantList varList;
    QTreeWidgetItem* header = headerItem();
    if (header) {
        for (int i = 0; i < header->columnCount(); ++i) {
            varList << header->text(i);
        }
    }
    return varList;
}

void QcTreeWidget::setColumns(const QVariantList& varList) {
    int count = varList.size();
    setColumnCount(count);
    QStringList labels;
    Q_FOREACH (const QVariant& var, varList)
        labels << var.toString();
    setHeaderLabels(labels);
}

void QcTreeWidget::onExpanded(QTreeWidgetItem* item) { Q_EMIT(expanded(QcTreeWidget::Item::safePtr(item))); }

void QcTreeWidget::onCollapsed(QTreeWidgetItem* item) { Q_EMIT(collapsed(QcTreeWidget::Item::safePtr(item))); }

QcTreeWidget::ItemPtr QcTreeWidget::currentItem() const {
    QTreeWidgetItem* item = QTreeWidget::currentItem();
    return QcTreeWidget::Item::safePtr(item);
}

void QcTreeWidget::setCurrentItem(const ItemPtr& item) {
    _emitAction = false;
    QTreeWidget::setCurrentItem(item);
    _emitAction = true;
}

QcTreeWidget::ItemPtr QcTreeWidget::item(const QcTreeWidget::ItemPtr& parent, int index) {
    QTreeWidgetItem* item = parent ? parent->child(index) : QTreeWidget::topLevelItem(index);

    return QcTreeWidget::Item::safePtr(item);
}

QcTreeWidget::ItemPtr QcTreeWidget::parentItem(const QcTreeWidget::ItemPtr& item) {
    if (!item)
        return QcTreeWidget::ItemPtr();
    return QcTreeWidget::Item::safePtr(item->parent());
}

int QcTreeWidget::indexOfItem(const QcTreeWidget::ItemPtr& item) {
    if (!item)
        return -1;
    QTreeWidgetItem* parent = item->parent();
    if (parent)
        return parent->indexOfChild(item);
    else
        return indexOfTopLevelItem(item);
}

QcTreeWidget::ItemPtr QcTreeWidget::addItem(const QcTreeWidget::ItemPtr& parent, const QVariantList& varList) {
    QStringList strings;
    for (int i = 0; i < varList.count(); ++i)
        strings << varList[i].toString();

    Item* item = new Item(strings);
    if (!parent)
        addTopLevelItem(item);
    else
        parent->addChild(item);

    return item->safePtr();
}

QcTreeWidget::ItemPtr QcTreeWidget::insertItem(const QcTreeWidget::ItemPtr& parent, int index,
                                               const QVariantList& varList) {
    int itemCount = topLevelItemCount();
    if (index < 0 || index > itemCount)
        return ItemPtr();

    QStringList strings;
    for (int i = 0; i < varList.count(); ++i)
        strings << varList[i].toString();

    Item* item = new Item(strings);
    if (!parent)
        insertTopLevelItem(index, item);
    else
        parent->insertChild(index, item);

    if (!item->treeWidget()) {
        delete item;
        return ItemPtr();
    }

    return item->safePtr();
}

void QcTreeWidget::removeItem(const QcTreeWidget::ItemPtr& item) { delete item.ptr(); }

QVariantList QcTreeWidget::strings(const QcTreeWidget::ItemPtr& item) {
    QVariantList varList;
    if (!item)
        return varList;
    for (int i = 0; i < item->columnCount(); ++i) {
        varList << item->text(i);
    }
    return varList;
}

void QcTreeWidget::setText(const QcTreeWidget::ItemPtr& item, int column, const QString& text) {
    if (item)
        item->setText(column, text);
}

void QcTreeWidget::setColor(const QcTreeWidget::ItemPtr& item, int column, const QColor& color) {
    if (item)
        item->setBackground(column, color);
}

void QcTreeWidget::setTextColor(const QcTreeWidget::ItemPtr& item, int column, const QColor& color) {
    if (item)
        item->setData(column, Qt::ForegroundRole, color);
}

QWidget* QcTreeWidget::itemWidget(const QcTreeWidget::ItemPtr& item, int column) {
    return item ? QTreeWidget::itemWidget(item, column) : 0;
}

void QcTreeWidget::setItemWidget(const QcTreeWidget::ItemPtr& item, int column, QWidget* o) {
    if (!item)
        return;

    if (!o)
        return;

    QTreeWidget::setItemWidget(item, column, o);
}

void QcTreeWidget::removeItemWidget(const QcTreeWidget::ItemPtr& item, int column) {
    if (item)
        QTreeWidget::removeItemWidget(item, column);
}

void QcTreeWidget::sort(int column, bool descending) {
    sortItems(column, descending ? Qt::DescendingOrder : Qt::AscendingOrder);
}

int QcTreeWidget::columnWidth(int column) {
    if (column < 0 || column >= columnCount())
        return -1;
    else
        return QTreeWidget::columnWidth(column);
}

void QcTreeWidget::setColumnWidth(int column, int width) { QTreeWidget::setColumnWidth(column, width); }

void QcTreeWidget::keyPressEvent(QKeyEvent* e) {
    QTreeWidget::keyPressEvent(e);

    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Home:
    case Qt::Key_End:
        // Prevent propagating to parent when scroller reaches minimum or maximum:
        e->accept();
    default:
        break;
    }
}

//////////////////////////////// Item //////////////////////////////////////

QcTreeWidget::ItemPtr QcTreeWidget::Item::safePtr(QTreeWidgetItem* item) {
    if (item && item->type() == Item::Type)
        return static_cast<Item*>(item)->safePtr();
    else
        return ItemPtr();
}

void QcTreeWidget::Item::initialize(VMGlobals* g, PyrObject* obj, const QcTreeWidget::ItemPtr& ptr) {
    Q_ASSERT(isKindOf(obj, SC_CLASS(TreeViewItem)));
    if (ptr.id()) {
        // store the SafePtr
        QcTreeWidget::ItemPtr* newPtr = new QcTreeWidget::ItemPtr(ptr);
        SetPtr(obj->slots + 0, newPtr);
        // store the id for equality comparison
        SetPtr(obj->slots + 1, ptr.id());
        // install finalizer
    } else {
        SetNil(obj->slots + 0);
        SetNil(obj->slots + 1);
    }
    InstallFinalizer(g, obj, 2, &QcTreeWidget::Item::finalize);
}

int QcTreeWidget::Item::finalize(VMGlobals* g, PyrObject* obj) {
    qcDebugMsg(1, "finalizing QTreeViewItem!");
    if (IsPtr(obj->slots + 0)) {
        QcTreeWidget::ItemPtr* ptr = static_cast<QcTreeWidget::ItemPtr*>(slotRawPtr(obj->slots + 0));
        delete ptr;
    }
    return errNone;
}

bool QcTreeWidget::Item::operator<(const QTreeWidgetItem& other) const {
    int column = treeWidget()->sortColumn();
    return text(column).toLower() < other.text(column).toLower();
}
