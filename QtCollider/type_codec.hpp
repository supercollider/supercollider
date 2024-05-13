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

#pragma once

#include "widgets/QcTreeWidget.h"
#include "widgets/QcMenu.h"
#include "image.h"

#include <PyrSlot.h>
#include <PyrKernel.h>

#include <QDebug>
#include <QChar>
#include <QString>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QWidget>
#include <QLayout>
#include <QVector>
#include <QUrl>
#include <QVariantList>

class QObjectProxy;

namespace QtCollider {

template <typename T, typename EnabledT = void> struct TypeCodec {};

// Forwarding from QtCollider namespace to TypeCodec

template <typename T> inline T read(PyrSlot* slot) { return TypeCodec<T>::read(slot); }

template <typename T> inline void write(PyrSlot* slot, const T& val) { return TypeCodec<T>::write(slot, val); }

// Lazy conversion, allows automatic codec deduction

struct DecodableSlot {
    PyrSlot* _slot;
    DecodableSlot(PyrSlot* slot): _slot(slot) {}
    template <typename T> operator T() { return TypeCodec<T>::safeRead(_slot); }
};

inline DecodableSlot get(PyrSlot* slot) { return DecodableSlot(slot); }

template <typename T> inline T get(PyrSlot* slot) { return TypeCodec<T>::safeRead(slot); }

template <typename T> inline void set(PyrSlot* slot, const T& val) {
    // write is always type-safe
    TypeCodec<T>::write(slot, val);
}

template <typename IteratorT> static void setObjectList(PyrSlot* slot, int size, IteratorT iter, IteratorT end) {
    typedef typename IteratorT::value_type ValueT;
    VMGlobals* g = gMainVMGlobals;

    PyrObject* array = newPyrArray(g->gc, size, 0, true);
    SetObject(slot, array);

    PyrSlot* s = array->slots;
    for (; iter != end; ++iter) {
        if (size > 0) {
            TypeCodec<ValueT>::write(s, *iter);
            ++array->size;
            ++s;
            --size;
        }
    }
}


// TypeCodecs

template <> struct TypeCodec<bool> {
    static bool read(PyrSlot* slot) { return IsTrue(slot); }

    static bool safeRead(PyrSlot* slot) { return read(slot); }

    static void write(PyrSlot* slot, const bool val) {
        if (val)
            SetTrue(slot);
        else
            SetFalse(slot);
    }
};

template <> struct TypeCodec<int> {
    static int read(PyrSlot* slot) { return slotRawInt(slot); }

    static int safeRead(PyrSlot* slot) {
        int val;
        if (slotIntVal(slot, &val))
            return 0;
        return val;
    }

    static void write(PyrSlot* slot, const int val) { SetInt(slot, val); }
};

template <> struct TypeCodec<float> {
    static float read(PyrSlot* slot) { return slotRawFloat(slot); }

    static float safeRead(PyrSlot* slot) {
        float val;
        if (slotFloatVal(slot, &val))
            return 0.f;
        return val;
    }

    static void write(PyrSlot* slot, const float val) { SetFloat(slot, val); }
};

template <> struct TypeCodec<double> {
    static double read(PyrSlot* slot) {
        double d;
        slotVal(slot, &d);
        return d;
    }

    static double safeRead(PyrSlot* slot) {
        double val;
        if (slotDoubleVal(slot, &val))
            return 0.0;
        return val;
    }

    static void write(PyrSlot* slot, const double val) {
        // NOTE: the signature actually reads SetFloat(PyrSlot*, double):
        SetFloat(slot, val);
    }
};

template <> struct TypeCodec<QChar> {
    static QChar read(PyrSlot* slot) { return QChar(slotRawChar(slot)); }

    static QChar safeRead(PyrSlot* slot) {
        if (GetTag(slot) == tagChar)
            return QChar(slotRawChar(slot));
        else
            return QChar();
    }

    static void write(PyrSlot* slot, const QChar& val) {
        // FIXME: Should add support for unicode in PyrSlot!
        SetChar(slot, val.toLatin1());
    }
};

template <> struct TypeCodec<QString> {
    static QString read(PyrSlot* slot);

    static QString safeRead(PyrSlot* slot) { return read(slot); }

    static void write(PyrSlot* slot, const QString& val);
};

template <> struct TypeCodec<QUrl> {
    static QUrl read(PyrSlot* slot);

    static QUrl safeRead(PyrSlot* slot) { return read(slot); }

    static void write(PyrSlot* slot, const QUrl& val);
};

template <> struct TypeCodec<QPointF> {
    static QPointF read(PyrSlot* slot);

    static QPointF safeRead(PyrSlot* slot);

    static void write(PyrSlot* slot, const QPointF& pt);
};

template <> struct TypeCodec<QPoint> {
    static QPoint read(PyrSlot* slot) { return TypeCodec<QPointF>::read(slot).toPoint(); }

    static QPoint safeRead(PyrSlot* slot) { return TypeCodec<QPointF>::safeRead(slot).toPoint(); }

    static void write(PyrSlot* slot, const QPoint& pt) { TypeCodec<QPointF>::write(slot, pt); }
};

template <> struct TypeCodec<QRectF> {
    static QRectF read(PyrSlot* slot);

    static QRectF safeRead(PyrSlot* slot);

    static void write(PyrSlot* slot, const QRectF& r);
};

template <> struct TypeCodec<QRect> {
    static QRect read(PyrSlot* slot) { return TypeCodec<QRectF>::read(slot).toRect(); }

    static QRect safeRead(PyrSlot* slot) { return TypeCodec<QRectF>::safeRead(slot).toRect(); }

    static void write(PyrSlot* slot, const QRect& rect) { TypeCodec<QRectF>::write(slot, rect); }
};

template <> struct TypeCodec<QSizeF> {
    static QSizeF read(PyrSlot* slot);

    static QSizeF safeRead(PyrSlot* slot);

    static void write(PyrSlot* slot, const QSizeF& sz);
};

template <> struct TypeCodec<QSize> {
    static QSize read(PyrSlot* slot) { return TypeCodec<QSizeF>::read(slot).toSize(); }

    static QSize safeRead(PyrSlot* slot) { return TypeCodec<QSizeF>::safeRead(slot).toSize(); }

    static void write(PyrSlot* slot, const QSize& size) { TypeCodec<QSizeF>::write(slot, size); }
};

template <> struct TypeCodec<QColor> {
    static QColor read(PyrSlot* slot);

    static QColor safeRead(PyrSlot* slot) {
        if (IsObj(slot))
            return read(slot);
        return QColor();
    }

    static void write(PyrSlot* slot, const QColor&);
};

template <> struct TypeCodec<QFont> {
    static QFont read(PyrSlot* slot);

    static QFont safeRead(PyrSlot* slot);

    static void write(PyrSlot* slot, const QFont&) {
        qWarning("WARNING: QtCollider: writing QFont to PyrSlot not supported.");
    }
};

template <> struct TypeCodec<QPalette> {
    static QPalette read(PyrSlot* slot);

    static QPalette safeRead(PyrSlot* slot);

    static void write(PyrSlot* slot, const QPalette&);
};

template <> struct TypeCodec<QObjectProxy*> {
    static QObjectProxy* read(PyrSlot* slot) {
        PyrSlot* proxySlot = slotRawObject(slot)->slots;
        if (IsPtr(proxySlot))
            return (QObjectProxy*)slotRawPtr(proxySlot);
        else
            return 0;
    }

    static QObjectProxy* safeRead(PyrSlot* slot);

    static void write(PyrSlot*, QObjectProxy*) {
        qWarning("WARNING: QtCollider: writing QObjectProxy* to PyrSlot not supported.");
    }
};

template <> struct TypeCodec<QObject*> {
    static QObject* read(PyrSlot*) {
        qWarning("WARNING: QtCollider: reading QObject* from PyrSlot not supported.");
        return 0;
    }

    static void write(PyrSlot*, QObject*);
};


#define TYPE_IS_QOBJECT(type) std::is_convertible<QObjectT, QObject*>::value
template <> struct TypeCodec<PyrObject*> {
    static PyrObject* read(PyrSlot*) {
        qWarning("WARNING: TypeCodec<PyrObject*>::read(PyrSlot*) = NO-OP");
        return 0;
    }

    static void write(PyrSlot* slot, PyrObject* object) { SetObject(slot, object); }
};

template <> struct TypeCodec<QcTreeWidget::ItemPtr> {
    static QcTreeWidget::ItemPtr read(PyrSlot* slot);

    static void write(PyrSlot* slot, const QcTreeWidget::ItemPtr&);
};

template <> struct TypeCodec<SharedImage> {
    static SharedImage read(PyrSlot* slot);
    static SharedImage safeRead(PyrSlot* slot);
    static void write(PyrSlot* slot, SharedImage image);
};

template <> struct TypeCodec<QVector<int>> {
    static QVector<int> read(PyrSlot* slot);

    static void write(PyrSlot* slot, const QVector<int>&);
};

template <> struct TypeCodec<QVector<double>> {
    static QVector<double> read(PyrSlot* slot);

    static void write(PyrSlot* slot, const QVector<double>&);
};

template <typename ContainedT> struct TypeCodec<QVector<ContainedT>> {
    static QVector<ContainedT> read(PyrSlot* slot) {
        qWarning("WARNING: TypeCodec<PyrObject*>::read(PyrSlot*) = NO-OP");
        return QVector<ContainedT>();
    }

    static void write(PyrSlot* slot, const QVector<ContainedT>& vec) {
        setObjectList(slot, vec.size(), vec.begin(), vec.end());
    }
};

template <typename ContainedT> struct TypeCodec<QList<ContainedT>> {
    static QList<ContainedT> read(PyrSlot* slot) {
        qWarning("WARNING: TypeCodec<PyrObject*>::read(PyrSlot*) = NO-OP");
        return QList<ContainedT>();
    }

    static void write(PyrSlot* slot, const QList<ContainedT>& vec) {
        setObjectList(slot, vec.size(), vec.begin(), vec.end());
    }
};

template <> struct TypeCodec<QVariantList> {
    static QVariantList read(PyrSlot* slot);

    static QVariantList safeRead(PyrSlot* slot) { return read(slot); }

    static void write(PyrSlot* slot, const QVariantList&);
};

template <typename QObjectT> struct TypeCodec<QObjectT, void> {
    static QObjectT read(PyrSlot* slot) { return safeRead(slot); }

    static QObjectT safeRead(PyrSlot* slot) {
        auto proxy = TypeCodec<QObjectProxy*>::safeRead(slot);

        if (proxy) {
            auto action = qobject_cast<QObjectT>(proxy->object());
            return action;
        } else {
            return 0;
        }
    }

    static void write(PyrSlot* slot, QObjectT object) {
        auto qobject = qobject_cast<QObject*>(object);
        TypeCodec<QObject*>::write(slot, qobject);
    }
};

} // namespace QtCollider
