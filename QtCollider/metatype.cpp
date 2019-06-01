/************************************************************************
 *
 * Copyright 2010 - 2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "metatype.hpp"
#include "type_codec.hpp"
#include "Common.h" // Make sure PyrObject* is declared to QMetaType
#include <PyrKernel.h>

#include "widgets/QcWebView.h"

namespace QtCollider {

static QVector<MetaType*> gMetaTypes;

template <typename T> MetaTypeImpl<T>* MetaTypeImpl<T>::instance = 0;

template <typename T> void qc_init_metatype() {
    MetaTypeImpl<T>::instance = new MetaTypeImpl<T>();
    gMetaTypes.append(MetaTypeImpl<T>::instance);
}

void MetaType::initAll() {
    qRegisterMetaType<PyrObject*>();
    qRegisterMetaType<QObjectProxy*>();
    qRegisterMetaType<QcTreeWidget::ItemPtr>();
    qRegisterMetaType<QVector<double>>();
    qRegisterMetaType<QVector<int>>();
    qRegisterMetaType<SharedImage>();
    qRegisterMetaType<QcAction*>();
    qRegisterMetaType<QcWidgetAction*>();
    qRegisterMetaType<QcMenu*>();
    qRegisterMetaType<QcToolBar*>();
    qRegisterMetaType<QList<QAction*>>();

    gMetaTypes.reserve(30);

    qc_init_metatype<bool>();
    qc_init_metatype<int>();
    qc_init_metatype<float>();
    qc_init_metatype<double>();
    qc_init_metatype<QChar>();
    qc_init_metatype<QString>();
    qc_init_metatype<QPointF>();
    qc_init_metatype<QPoint>();
    qc_init_metatype<QSizeF>();
    qc_init_metatype<QSize>();
    qc_init_metatype<QRectF>();
    qc_init_metatype<QRect>();
    qc_init_metatype<QColor>();
    qc_init_metatype<QFont>();
    qc_init_metatype<QPalette>();
    qc_init_metatype<QObjectProxy*>();
    qc_init_metatype<QObject*>();
    qc_init_metatype<QWidget*>();
    qc_init_metatype<QLayout*>();
    qc_init_metatype<PyrObject*>();
    qc_init_metatype<QcTreeWidget::ItemPtr>();
    qc_init_metatype<SharedImage>();
    qc_init_metatype<QMenu*>();
    qc_init_metatype<QAction*>();
    qc_init_metatype<QList<QAction*>>();
    qc_init_metatype<QVector<double>>();
    qc_init_metatype<QVector<int>>();
    qc_init_metatype<QVariantList>();
    qc_init_metatype<QUrl>();
    qc_init_metatype<QcCallback*>();
}

MetaType* MetaType::find(PyrSlot* slot) {
    switch (GetTag(slot)) {
    case tagNil:
        return 0;
    case tagInt:
        return metaType<int>();
    case tagSym:
        return metaType<QString>();
    case tagChar:
        return metaType<QChar>();
    case tagFalse:
    case tagTrue:
        return metaType<bool>();
    case tagObj: {
        PyrObject* obj = slotRawObject(slot);
        PyrClass* klass = obj->classptr;
        unsigned char format = obj->obj_format;

        if (format == obj_double || format == obj_float)
            return metaType<QVector<double>>();

        else if (format == obj_int32 || format == obj_int16 || format == obj_int8)
            return metaType<QVector<int>>();

        else if (isKindOfSlot(slot, class_string)) {
            return metaType<QString>();
        } else if (isKindOfSlot(slot, SC_CLASS(Point))) {
            return metaType<QPointF>();
        } else if (isKindOfSlot(slot, SC_CLASS(Rect))) {
            return metaType<QRectF>();
        } else if (isKindOfSlot(slot, SC_CLASS(Size))) {
            return metaType<QSizeF>();
        } else if (klass == SC_CLASS(Color) || klass == SC_CLASS(Gradient) || klass == SC_CLASS(HiliteGradient)) {
            return metaType<QColor>();
        } else if (isKindOfSlot(slot, SC_CLASS(Font))) {
            return metaType<QFont>();
        } else if (isKindOfSlot(slot, SC_CLASS(QPalette))) {
            return metaType<QPalette>();
        } else if (isKindOfSlot(slot, SC_CLASS(QCallback))) {
            return metaType<QcCallback*>();
        } else if (isKindOfSlot(slot, SC_CLASS(AbstractMenuAction))) {
            return metaType<QAction*>();
        } else if (isKindOfSlot(slot, SC_CLASS(Menu))) {
            return metaType<QMenu*>();
        } else if (isKindOfSlot(slot, SC_CLASS(View)) || isKindOfSlot(slot, SC_CLASS(ScrollCanvas))) {
            return metaType<QWidget*>();
        } else if (isKindOfSlot(slot, SC_CLASS(Layout))) {
            return metaType<QLayout*>();
        } else if (isKindOfSlot(slot, SC_CLASS(QObject))) {
            return metaType<QObjectProxy*>();
        } else if (isKindOfSlot(slot, class_array) || isKindOfSlot(slot, class_symbolarray)) {
            return metaType<QVariantList>();
        } else if (isKindOfSlot(slot, SC_CLASS(TreeViewItem))) {
            return metaType<QcTreeWidget::ItemPtr>();
        } else if (isKindOfSlot(slot, SC_CLASS(Image))) {
            return metaType<SharedImage>();
        } else {
            QString className = TypeCodec<QString>::read(&slotRawObject(slot)->classptr->name);
            qcWarningMsg(QStringLiteral("WARNING: Do not know how to use an instance of class '%1'").arg(className));
            return 0;
        }
    }
    default:
        return metaType<double>();
    }
}

MetaType* MetaType::find(int id) {
    int n = gMetaTypes.count();
    MetaType* const* d = gMetaTypes.constData();

    int i;
    for (i = 0; i < n; ++i) {
        if ((*d)->mId == id)
            return *d;
        ++d;
    }

    return 0;
}

} // namespace QtCollider
