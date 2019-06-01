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

#include "type_codec.hpp"
#include "metatype.hpp"

#include "QObjectProxy.h"
#include "widgets/QcTreeWidget.h"
#include "primitives/prim_QPalette.hpp"
#include "image.h"

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <VMGlobals.h>

#include <qmath.h>

namespace QtCollider {


QString TypeCodec<QString>::read(PyrSlot* slot) {
    if (IsSym(slot)) {
        return QString::fromUtf8(slotRawSymbol(slot)->name);
    } else if (isKindOfSlot(slot, class_string)) {
        int len = slotRawObject(slot)->size;
        return QString::fromUtf8(slotRawString(slot)->s, len);
    }
    return QString();
}


void TypeCodec<QString>::write(PyrSlot* slot, const QString& val) {
    PyrString* str = newPyrString(gMainVMGlobals->gc, val.toUtf8().constData(), 0, true);
    SetObject(slot, str);
}


QUrl TypeCodec<QUrl>::read(PyrSlot* slot) {
    if (IsSym(slot)) {
        return QUrl(QString::fromUtf8(slotRawSymbol(slot)->name));
    } else if (isKindOfSlot(slot, class_string)) {
        int len = slotRawObject(slot)->size;
        return QUrl(QString::fromUtf8(slotRawString(slot)->s, len));
    } else {
        return QUrl();
    }
}


void TypeCodec<QUrl>::write(PyrSlot* slot, const QUrl& val) {
    PyrString* str = newPyrString(gMainVMGlobals->gc, val.toString(QUrl::None).toUtf8().constData(), 0, true);
    SetObject(slot, str);
}

QPointF TypeCodec<QPointF>::read(PyrSlot* slot) {
    PyrSlot* slots = slotRawObject(slot)->slots;
    float x, y;
    int err;
    err = slotFloatVal(slots + 0, &x);
    if (err)
        return QPointF();
    err = slotFloatVal(slots + 1, &y);
    if (err)
        return QPointF();
    return QPointF(x, y);
}


QPointF TypeCodec<QPointF>::safeRead(PyrSlot* slot) {
    if (isKindOfSlot(slot, SC_CLASS(Point)))
        return read(slot);
    else
        return QPointF();
}


void TypeCodec<QPointF>::write(PyrSlot* slot, const QPointF& pt) {
    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, SC_CLASS(Point), 0, true, true);
    SetObject(slot, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, pt.x());
    SetFloat(slots + 1, pt.y());
}


QRectF TypeCodec<QRectF>::read(PyrSlot* slot) {
    PyrSlot* slots = slotRawObject(slot)->slots;
    float bounds[4];
    for (int i = 0; i < 4; ++i) {
        int err = slotFloatVal(slots + i, &bounds[i]);
        if (err)
            return QRectF();
    }

    return QRectF(bounds[0], bounds[1], bounds[2], bounds[3]);
}


QRectF TypeCodec<QRectF>::safeRead(PyrSlot* slot) {
    if (isKindOfSlot(slot, SC_CLASS(Rect)))
        return read(slot);
    else
        return QRectF();
}


void TypeCodec<QRectF>::write(PyrSlot* slot, const QRectF& r) {
    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, SC_CLASS(Rect), 0, true, true);
    SetObject(slot, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, r.x());
    SetFloat(slots + 1, r.y());
    SetFloat(slots + 2, r.width());
    SetFloat(slots + 3, r.height());
}


QSizeF TypeCodec<QSizeF>::read(PyrSlot* slot) {
    PyrSlot* slots = slotRawObject(slot)->slots;
    float w = 0.f, h = 0.f;
    slotFloatVal(slots + 0, &w);
    slotFloatVal(slots + 1, &h);

    return QSizeF(w, h);
}


QSizeF TypeCodec<QSizeF>::safeRead(PyrSlot* slot) {
    if (isKindOfSlot(slot, SC_CLASS(Size)))
        return read(slot);
    else
        return QSizeF();
}


void TypeCodec<QSizeF>::write(PyrSlot* slot, const QSizeF& sz) {
    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, SC_CLASS(Size), 0, true, true);
    SetObject(slot, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, sz.width());
    SetFloat(slots + 1, sz.height());
}

inline QColor asColor(PyrObject* obj) {
    PyrSlot* slots = obj->slots;

    float r, g, b, a;
    r = g = b = a = 0.f;
    slotFloatVal(slots + 0, &r);
    slotFloatVal(slots + 1, &g);
    slotFloatVal(slots + 2, &b);
    slotFloatVal(slots + 3, &a);
    return QColor(r * 255, g * 255, b * 255, a * 255);
}


QColor TypeCodec<QColor>::read(PyrSlot* slot) {
    PyrObject* obj = slotRawObject(slot);
    PyrClass* klass = obj->classptr;

    if (klass == SC_CLASS(Color))
        return asColor(obj);

    if (klass == SC_CLASS(Gradient) || klass == SC_CLASS(HiliteGradient)) {
        qcWarningMsg("WARNING: Gradient and HiliteGradient are not supported yet."
                     " Using the average gradient color instead.");

        QColor c1(safeRead(obj->slots + 0));
        QColor c2(safeRead(obj->slots + 1));
        QColor mix((c1.red() + c2.red()) / 2, (c1.green() + c2.green()) / 2, (c1.blue() + c2.blue()) / 2);
        return mix;
    }

    return QColor();
}


void TypeCodec<QColor>::write(PyrSlot* slot, const QColor& c) {
    if (!c.isValid()) {
        SetNil(slot);
        return;
    }

    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, SC_CLASS(Color), 0, true, true);
    SetObject(slot, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, c.red() / 255.0);
    SetFloat(slots + 1, c.green() / 255.0);
    SetFloat(slots + 2, c.blue() / 255.0);
    SetFloat(slots + 3, c.alpha() / 255.0);
}


QFont TypeCodec<QFont>::read(PyrSlot* slot) {
    PyrSlot* slots = slotRawObject(slot)->slots;

    QString family = TypeCodec<QString>::safeRead(slots + 0);
    float fSize = TypeCodec<float>::safeRead(slots + 1);
    bool bold = TypeCodec<bool>::safeRead(slots + 2);
    bool italic = TypeCodec<bool>::safeRead(slots + 3);
    bool isPtSize = TypeCodec<bool>::safeRead(slots + 4);

    QFont f;

    if (!family.isEmpty())
        f.setFamily(family);

    if (fSize > 0.f) {
        if (isPtSize) {
            f.setPointSizeF(fSize);
        } else {
            int pixSize = (fSize > 1.f ? qRound(fSize) : 1);
            f.setPixelSize(pixSize);
        }
    }

    f.setBold(bold);

    f.setItalic(italic);

    return f;
}


QFont TypeCodec<QFont>::safeRead(PyrSlot* slot) {
    if (isKindOfSlot(slot, SC_CLASS(Font)))
        return TypeCodec<QFont>::read(slot);
    else
        return QFont();
}

QPalette TypeCodec<QPalette>::read(PyrSlot* slot) {
    QPalette* p = QPALETTE_FROM_OBJECT(slotRawObject(slot));
    return *p;
}


QPalette TypeCodec<QPalette>::safeRead(PyrSlot* slot) {
    if (isKindOfSlot(slot, SC_CLASS(QPalette)))
        return TypeCodec<QPalette>::read(slot);
    else
        return QPalette();
}


void TypeCodec<QPalette>::write(PyrSlot* slot, const QPalette& plt) {
    PyrGC* gc = gMainVMGlobals->gc;
    PyrObject* obj = instantiateObject(gc, SC_CLASS(QPalette), 0, true, true);
    SetObject(slot, obj);

    QPalette_Init(gMainVMGlobals, obj, plt);
}


QObjectProxy* TypeCodec<QObjectProxy*>::safeRead(PyrSlot* slot) {
    if (!isKindOfSlot(slot, SC_CLASS(QObject)))
        return 0;
    return read(slot);
}

void TypeCodec<QObject*>::write(PyrSlot* slot, QObject* obj) {
    QObjectProxy* proxy = QObjectProxy::fromObject(obj);

    if (proxy && proxy->scObject())
        SetObject(slot, proxy->scObject());
    else
        SetNil(slot);
}


QVariantList TypeCodec<QVariantList>::read(PyrSlot* slot) {
    if (isKindOfSlot(slot, class_array)) {
        PyrObject* obj = slotRawObject(slot);
        PyrSlot* slots = obj->slots;
        int size = obj->size;
        QVariantList list;
        for (int i = 0; i < size; ++i, ++slots) {
            list << QtCollider::get<QVariant>(slots);
        }
        return list;
    } else if (isKindOfSlot(slot, class_symbolarray)) {
        PyrSymbolArray* symarray = slotRawSymbolArray(slot);
        PyrSymbol** symbols = symarray->symbols;
        int size = symarray->size;
        QVariantList list;
        for (int i = 0; i < size; ++i, ++symbols)
            list << QVariant(QString((*symbols)->name));
        return list;
    }

    return QVariantList();
}


void TypeCodec<QVariantList>::write(PyrSlot* slot, const QVariantList& varList) {
    VMGlobals* g = gMainVMGlobals;

    int count = varList.count();

    PyrObject* array = newPyrArray(g->gc, count, 0, true);
    SetObject(slot, array);

    int i;
    PyrSlot* s = array->slots;
    for (i = 0; i < count; ++i, ++s) {
        if (!QtCollider::set(s, varList[i])) {
            qcDebugMsg(1, "WARNING: Could not set one slot of array");
        }
        array->size++;
        g->gc->GCWrite(array, s);
    }
}


#define WRONG_OBJECT_FORMAT false

template <typename DEST, typename ORIG> inline static void copy(QVector<DEST>& dest, PyrSlot* orig, int size) {
    ORIG* array = (ORIG*)orig;
    for (int i = 0; i < size; ++i)
        dest << DEST(array[i]);
}

template <typename numeric_type> static QVector<numeric_type> toNumericVector(PyrObject* obj) {
    int size = obj->size;
    PyrSlot* slots = obj->slots;

    QVector<numeric_type> vector;
    vector.reserve(size);

    switch (obj->obj_format) {
    case obj_double:
        copy<numeric_type, double>(vector, slots, size);
        break;
    case obj_float:
        copy<numeric_type, float>(vector, slots, size);
        break;
    case obj_int32:
        copy<numeric_type, int32>(vector, slots, size);
        break;
    case obj_int16:
        copy<numeric_type, int16>(vector, slots, size);
        break;
    case obj_int8:
        copy<numeric_type, int8>(vector, slots, size);
        break;
    default:
        Q_ASSERT(WRONG_OBJECT_FORMAT);
    }

    return vector;
}

template <typename numeric_type> static void setNumeric(PyrSlot*, numeric_type);


template <> inline void setNumeric<double>(PyrSlot* s, double val) { SetFloat(s, val); }

template <> inline void setNumeric<int>(PyrSlot* s, int val) { SetInt(s, val); }

template <typename numeric_type> static void setNumericVector(PyrSlot* slot, const QVector<numeric_type>& vec) {
    VMGlobals* g = gMainVMGlobals;

    int count = vec.count();

    PyrObject* array = newPyrArray(g->gc, count, 0, true);
    SetObject(slot, array);

    PyrSlot* s = array->slots;
    Q_FOREACH (numeric_type val, vec) {
        setNumeric<numeric_type>(s, val);
        ++array->size;
        ++s;
    }
}

QVector<double> TypeCodec<QVector<double>>::read(PyrSlot* slot) { return toNumericVector<double>(slotRawObject(slot)); }


void TypeCodec<QVector<double>>::write(PyrSlot* slot, const QVector<double>& vec) {
    setNumericVector<double>(slot, vec);
}


QVector<int> TypeCodec<QVector<int>>::read(PyrSlot* slot) { return toNumericVector<int>(slotRawObject(slot)); }


void TypeCodec<QVector<int>>::write(PyrSlot* slot, const QVector<int>& vec) { setNumericVector<int>(slot, vec); }

QcTreeWidget::ItemPtr TypeCodec<QcTreeWidget::ItemPtr>::read(PyrSlot* slot) {
    PyrSlot* ptrSlot = slotRawObject(slot)->slots + 0;
    if (IsPtr(ptrSlot)) {
        QcTreeWidget::ItemPtr* safePtr = static_cast<QcTreeWidget::ItemPtr*>(slotRawPtr(ptrSlot));
        return *safePtr;
    } else {
        return QcTreeWidget::ItemPtr();
    }
}


void TypeCodec<QcTreeWidget::ItemPtr>::write(PyrSlot* slot, const QcTreeWidget::ItemPtr& item) {
    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, SC_CLASS(TreeViewItem), 0, true, true);
    QcTreeWidget::Item::initialize(gMainVMGlobals, obj, item);
    SetObject(slot, obj);
}

SharedImage TypeCodec<SharedImage>::read(PyrSlot* slot) {
    SharedImage* ptr = reinterpret_cast<SharedImage*>(slotRawPtr(slotRawObject(slot)->slots + 0));
    return *ptr;
}

SharedImage TypeCodec<SharedImage>::safeRead(PyrSlot* slot) {
    if (!isKindOfSlot(slot, SC_CLASS(Image)))
        return SharedImage();
    else
        return read(slot);
}

void TypeCodec<SharedImage>::write(PyrSlot* slot, SharedImage image) {
    qWarning("WARNING: QtCollider: writing SharedImage to PyrSlot not supported.");
}

}

// namespace QtCollider
