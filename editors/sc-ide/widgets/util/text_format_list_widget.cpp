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

#include "text_format_list_widget.hpp"

#include <QPainter>
#include <QApplication>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QColorDialog>

namespace ScIDE {

TextFormatListWidget::ItemDelegate::ItemDelegate(QObject* parent): QStyledItemDelegate(parent) {}

QSize TextFormatListWidget::ItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
    switch (index.column()) {
    case 1:
        return QSize(30, 20);
    default:
        return QStyledItemDelegate::sizeHint(opt, index);
    }
}

void TextFormatListWidget::ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& opt,
                                               const QModelIndex& index) const {
    int col = index.column();
    if (col < 1 || col > 2) {
        QStyledItemDelegate::paint(painter, opt, index);
        return;
    }

    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

    QBrush brush = index.data(Qt::DisplayRole).value<QBrush>();
    if (brush.style() == Qt::NoBrush) {
        brush.setColor(opt.palette.color(QPalette::Text));
        brush.setStyle(Qt::BDiagPattern);
    }

    QSize sz(30, 14);
    QRect r;
    r.setSize(sz);
    r.moveCenter(opt.rect.center());

    painter->save();

    painter->setBrush(brush);
    painter->setPen(opt.palette.color(QPalette::Text));
    painter->drawRect(r.adjusted(0, 0, -1, -1));

    painter->restore();
}


TextFormatListWidget::TextFormatListWidget(QWidget* parent): QTreeView(parent) {
    QStandardItemModel* m = new QStandardItemModel(this);
    m->setHorizontalHeaderLabels(QStringList() << tr("Role") << tr("Color") << tr("Background") << "B"
                                               << "I"
                                               << "U");

    QItemSelectionModel* old_sm = selectionModel();
    QAbstractItemModel* old_m = model();
    setModel(m);
    delete old_m;
    delete old_sm;

    QAbstractItemDelegate* old_d = itemDelegate();
    setItemDelegate(new ItemDelegate(this));
    delete old_d;

    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClicked(const QModelIndex&)));
}

void TextFormatListWidget::addFormat(const QString& name, const QTextCharFormat& format) {
    QList<QStandardItem*> items;

    QStandardItem* nameItem = new QStandardItem(name);
    nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    items << nameItem;

    items << makeBrushItem(format.foreground());

    items << makeBrushItem(format.background());

    items << makeBoolItem(format.fontWeight() == QFont::Bold);

    items << makeBoolItem(format.fontItalic());

    items << makeBoolItem(format.fontUnderline());

    standardModel()->appendRow(items);
}

void TextFormatListWidget::setFormat(int index, const QTextCharFormat& format) {
    QStandardItemModel* m = standardModel();

    if (index < 0 || index >= m->rowCount())
        return;

    m->item(index, 1)->setData(format.foreground(), Qt::DisplayRole);
    m->item(index, 2)->setData(format.background(), Qt::DisplayRole);
    m->item(index, 3)->setCheckState(format.fontWeight() == QFont::Bold ? Qt::Checked : Qt::Unchecked);
    m->item(index, 4)->setCheckState(format.fontItalic() ? Qt::Checked : Qt::Unchecked);
    m->item(index, 5)->setCheckState(format.fontUnderline() ? Qt::Checked : Qt::Unchecked);
}

QString TextFormatListWidget::name(int index) {
    QStandardItemModel* m = standardModel();
    QString str;

    if (index < 0 || index >= m->rowCount())
        return str;

    str = m->item(index, 0)->text();

    return str;
}

QTextCharFormat TextFormatListWidget::format(int index) {
    QStandardItemModel* m = standardModel();
    QTextCharFormat fm;

    if (index < 0 || index >= m->rowCount())
        return fm;

    QBrush br = m->item(index, 1)->data(Qt::DisplayRole).value<QBrush>();
    if (br.style() != Qt::NoBrush)
        fm.setForeground(br);

    br = m->item(index, 2)->data(Qt::DisplayRole).value<QBrush>();
    if (br.style() != Qt::NoBrush)
        fm.setBackground(br);

    bool b = m->item(index, 3)->checkState() == Qt::Checked;
    if (b)
        fm.setFontWeight(QFont::Bold);

    b = m->item(index, 4)->checkState() == Qt::Checked;
    if (b)
        fm.setFontItalic(true);

    b = m->item(index, 5)->checkState() == Qt::Checked;
    if (b)
        fm.setFontUnderline(true);

    return fm;
}

void TextFormatListWidget::onDoubleClicked(const QModelIndex& index) {
    int column = index.column();
    if (column < 1 || column > 2)
        return;

    QStandardItem* item = standardModel()->item(index.row(), column);
    QBrush br = item->data(Qt::DisplayRole).value<QBrush>();

    QColor color = QColorDialog::getColor(br.color(), this);
    if (color.isValid()) {
        item->setData(QBrush(color), Qt::DisplayRole);
    }
}

QStandardItem* TextFormatListWidget::makeBrushItem(const QBrush& brush) {
    QStandardItem* item = new QStandardItem();
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setData(brush, Qt::DisplayRole);
    return item;
}

QStandardItem* TextFormatListWidget::makeBoolItem(bool b) {
    QStandardItem* item = new QStandardItem();
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setCheckable(true);
    if (b)
        item->setCheckState(Qt::Checked);
    return item;
}

} // namespace ScIDE
