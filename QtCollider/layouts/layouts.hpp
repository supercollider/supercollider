/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#include "stack_layout.hpp"
#include "../Common.h"
#include "../QcObjectFactory.h"
#include "../QObjectProxy.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>

template <class LAYOUT> struct QcLayout : public LAYOUT {
public:
    QVariantList margins() const { return QVariantList(); }

    void setMargins(const QVariantList& list) {
        if (list.size() < 4)
            return;
        int m[4];
        for (int i = 0; i < 4; ++i)
            m[i] = list[i].toInt();
        LAYOUT::setContentsMargins(m[0], m[1], m[2], m[3]);
    }
};

template <class BOXLAYOUT> class QcBoxLayout : public QcLayout<BOXLAYOUT> {
public:
    QcBoxLayout() {}

    QcBoxLayout(const QVariantList& items) {
        Q_FOREACH (const QVariant& var, items) {
            QVariantList item = var.toList();
            addItem(item);
        }
    }

    void addItem(const QVariantList& item) {
        if (item.size() < 3)
            return;

        int stretch = item[1].toInt();
        Qt::Alignment alignment = (Qt::Alignment)item[2].toInt();

        const QVariant& varObject = item[0];

        if (!varObject.isValid()) {
            BOXLAYOUT::addStretch(stretch);
            return;
        }

        if (varObject.canConvert<int>()) {
            int size = varObject.toInt();
            BOXLAYOUT::addSpacing(size);
            return;
        }

        QWidget* w = varObject.value<QWidget*>();
        if (w) {
            BOXLAYOUT::addWidget(w, stretch, alignment);
            return;
        }

        QLayout* l2 = varObject.value<QLayout*>();
        if (l2) {
            BOXLAYOUT::addLayout(l2, stretch);
            return;
        }
    }

    void insertItem(const QVariantList& item) {
        if (item.size() < 4)
            return;

        int index = item[1].toInt();
        int stretch = item[2].toInt();
        Qt::Alignment alignment = (Qt::Alignment)item[3].toInt();

        const QVariant& varObject = item[0];

        if (!varObject.isValid()) {
            BOXLAYOUT::insertStretch(index, stretch);
            return;
        }

        if (varObject.canConvert<int>()) {
            int size = varObject.toInt();
            BOXLAYOUT::insertSpacing(index, size);
            return;
        }

        QWidget* w = varObject.value<QWidget*>();
        if (w) {
            BOXLAYOUT::insertWidget(index, w, stretch, alignment);
            return;
        }

        QLayout* l2 = varObject.value<QLayout*>();
        if (l2) {
            BOXLAYOUT::insertLayout(index, l2, stretch);
            return;
        }
    }

    void setStretch(QWidget* w, int stretch) { BOXLAYOUT::setStretchFactor(w, stretch); }

    void setStretch(QLayout* l, int stretch) { BOXLAYOUT::setStretchFactor(l, stretch); }

    void setAlignment(QWidget* w, Qt::Alignment alignment) { BOXLAYOUT::setAlignment(w, alignment); }

    void setAlignment(QLayout* l, Qt::Alignment alignment) { BOXLAYOUT::setAlignment(l, alignment); }
};

class QcHBoxLayout : public QcBoxLayout<QHBoxLayout> {
    Q_OBJECT
    Q_PROPERTY(QVariantList margins READ margins WRITE setMargins)
public:
    QcHBoxLayout() {}
    Q_INVOKABLE QcHBoxLayout(const QVariantList& items): QcBoxLayout<QHBoxLayout>(items) {}
    Q_INVOKABLE void addItem(const QVariantList& data) { QcBoxLayout<QHBoxLayout>::addItem(data); }
    Q_INVOKABLE void insertItem(const QVariantList& data) { QcBoxLayout<QHBoxLayout>::insertItem(data); }
    Q_INVOKABLE void setStretch(int index, int stretch) { QBoxLayout::setStretch(index, stretch); }
    Q_INVOKABLE void setStretch(QWidget* w, int stretch) { QcBoxLayout<QHBoxLayout>::setStretch(w, stretch); }
    Q_INVOKABLE void setStretch(QLayout* l, int stretch) { QcBoxLayout<QHBoxLayout>::setStretch(l, stretch); }
    Q_INVOKABLE void setAlignment(int i, int a) {
        itemAt(i)->setAlignment((Qt::Alignment)a);
        update();
    }
    Q_INVOKABLE void setAlignment(QWidget* w, int a) { QcBoxLayout<QHBoxLayout>::setAlignment(w, (Qt::Alignment)a); }
    Q_INVOKABLE void setAlignment(QLayout* l, int a) { QcBoxLayout<QHBoxLayout>::setAlignment(l, (Qt::Alignment)a); }
};

class QcVBoxLayout : public QcBoxLayout<QVBoxLayout> {
    Q_OBJECT
    Q_PROPERTY(QVariantList margins READ margins WRITE setMargins)
public:
    QcVBoxLayout() {}
    Q_INVOKABLE QcVBoxLayout(const QVariantList& items): QcBoxLayout<QVBoxLayout>(items) {}
    Q_INVOKABLE void addItem(const QVariantList& data) { QcBoxLayout<QVBoxLayout>::addItem(data); }
    Q_INVOKABLE void insertItem(const QVariantList& data) { QcBoxLayout<QVBoxLayout>::insertItem(data); }
    Q_INVOKABLE void setStretch(int index, int stretch) { QBoxLayout::setStretch(index, stretch); }
    Q_INVOKABLE void setStretch(QWidget* w, int stretch) { QcBoxLayout<QVBoxLayout>::setStretch(w, stretch); }
    Q_INVOKABLE void setStretch(QLayout* l, int stretch) { QcBoxLayout<QVBoxLayout>::setStretch(l, stretch); }
    Q_INVOKABLE void setAlignment(int i, int a) {
        itemAt(i)->setAlignment((Qt::Alignment)a);
        update();
    }
    Q_INVOKABLE void setAlignment(QWidget* w, int a) { QcBoxLayout<QVBoxLayout>::setAlignment(w, (Qt::Alignment)a); }
    Q_INVOKABLE void setAlignment(QLayout* l, int a) { QcBoxLayout<QVBoxLayout>::setAlignment(l, (Qt::Alignment)a); }
};

class QcGridLayout : public QcLayout<QGridLayout> {
    Q_OBJECT
    Q_PROPERTY(QVariantList margins READ margins WRITE setMargins)
    Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
    Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
public:
    Q_INVOKABLE void addItem(const QVariantList& dataList);
    Q_INVOKABLE void setRowStretch(int row, int factor) { QcLayout<QGridLayout>::setRowStretch(row, factor); }
    Q_INVOKABLE void setColumnStretch(int column, int factor) {
        QcLayout<QGridLayout>::setColumnStretch(column, factor);
    }
    Q_INVOKABLE void setAlignment(int r, int c, int a) {
        QLayoutItem* item = itemAtPosition(r, c);
        if (item) {
            item->setAlignment((Qt::Alignment)a);
            update();
        }
    }
    Q_INVOKABLE void setAlignment(QWidget* w, int a) { QLayout::setAlignment(w, (Qt::Alignment)a); }
    Q_INVOKABLE void setAlignment(QLayout* l, int a) { QLayout::setAlignment(l, (Qt::Alignment)a); }
    Q_INVOKABLE int minRowHeight(int row) { return (row >= 0 && row < rowCount()) ? rowMinimumHeight(row) : 0; }
    Q_INVOKABLE int minColumnWidth(int col) { return (col >= 0 && col < columnCount()) ? columnMinimumWidth(col) : 0; }
    Q_INVOKABLE void setMinRowHeight(int row, int h) { setRowMinimumHeight(row, h); }
    Q_INVOKABLE void setMinColumnWidth(int col, int w) { setColumnMinimumWidth(col, w); }
};

class QcStackLayout : public QcLayout<QtCollider::StackLayout> {
    Q_OBJECT
    Q_PROPERTY(QVariantList margins READ margins WRITE setMargins)

public:
    QcStackLayout() {}

    Q_INVOKABLE QcStackLayout(const QVariantList& items) {
        Q_FOREACH (const QVariant& var, items) {
            QWidget* w = var.value<QWidget*>();
            if (w)
                addWidget(w);
        }
    }

    Q_INVOKABLE void insertWidget(int index, QWidget* w) { QtCollider::StackLayout::insertWidget(index, w); }
};
