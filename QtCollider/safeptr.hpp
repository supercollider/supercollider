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

#include "Common.h"

#include <QApplication>
#include <QAtomicPointer>
#include <QAtomicInt>
#include <QMutex>

namespace QtCollider {

template <typename T> class SafePtr {
public:
    SafePtr(): d(0) {}

    SafePtr(const SafePtr& other): d(other.d) { ref(); }

    SafePtr(T* ptr): d(new Data(ptr)) {}

    SafePtr& operator=(const SafePtr& other) {
        deref();
        d = other.d;
        ref();
        return *this;
    }

    ~SafePtr() { deref(); }

    T* operator->() const { return d->ptr.load(); }

    T& operator*() const { return *d->ptr.load(); }

    operator T*() const { return (d ? d->ptr.load() : 0); }

    T* ptr() const { return (d ? d->ptr.load() : 0); }

    void* id() const { return (void*)d; } // useful for checking internal pointer identity

    void invalidate() {
        qcDebugMsg(2, "SafePtr: invalidating");
        if (d)
            d->ptr = 0;
    }

private:
    struct Data {
        Data(T* ptr_): ptr(ptr_), refCount(1) {}
        QAtomicPointer<T> ptr;
        QAtomicInt refCount;
    };

    void ref() {
        if (d) {
            d->refCount.ref();
            qcDebugMsg(2, QString("SafePtr: +refcount = %1").arg(d->refCount.load()));
        }
    }
    void deref() {
        if (d) {
            bool ref = d->refCount.deref();
            qcDebugMsg(2, QString("SafePtr: -refcount = %1").arg(d->refCount.load()));
            if (!ref) {
                qcDebugMsg(2, "SafePtr: unreferenced!");
                delete d;
            }
        }
    }

    Data* d;
};

} // namespace QtCollider
