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

#include "type_codec.hpp"

#include <PyrSlot.h>

#include <QMetaType>
#include <QVariant>

namespace QtCollider {

struct MetaType {
public:
    static void initAll();

    static MetaType* find(PyrSlot*);
    static MetaType* find(int id);

    // Generic algorithms

    template <typename T> static void* read(PyrSlot* slot, void* ptr) {
        if (slot)
            return new (ptr) T(TypeCodec<T>::read(slot));
        else
            return new (ptr) T();
    }

    template <typename T> static QVariant read(PyrSlot* slot) {
        return QVariant::fromValue<T>(TypeCodec<T>::read(slot));
    }

    template <typename T> static void write(PyrSlot* slot, void* ptr) {
        TypeCodec<T>::write(slot, *reinterpret_cast<T*>(ptr));
    }

    template <typename T> static void write(PyrSlot* slot, const QVariant& var) {
        TypeCodec<T>::write(slot, var.value<T>());
    }

    template <typename T> static void destruct(void* ptr) {
        T* typed_ptr = reinterpret_cast<T*>(ptr);
        typed_ptr->~T();
    }

    // Abstract access to generic algorithms

    virtual size_t size() = 0;
    virtual void* read(PyrSlot*, void* ptr) = 0;
    virtual QVariant read(PyrSlot*) = 0;
    virtual void write(PyrSlot*, void* ptr) = 0;
    virtual void write(PyrSlot*, const QVariant&) = 0;
    virtual void destruct(void* ptr) = 0;

    const int& id() const { return mId; }

protected:
    MetaType(int a_id): mId(a_id) {}
    int mId;
};

template <typename T> struct MetaTypeImpl : MetaType {
    MetaTypeImpl(): MetaType(qMetaTypeId<T>()) {}

    size_t size() { return sizeof(T); }

    void* read(PyrSlot* slot, void* ptr) { return MetaType::read<T>(slot, ptr); }

    QVariant read(PyrSlot* slot) { return MetaType::read<T>(slot); }

    void write(PyrSlot* slot, void* ptr) { MetaType::write<T>(slot, ptr); }

    void write(PyrSlot* slot, const QVariant& var) { MetaType::write<T>(slot, var); }

    void destruct(void* ptr) { MetaType::destruct<T>(ptr); }

    /////////////

    static MetaTypeImpl<T>* instance;
};

class MetaValue {
public:
    MetaValue(): mType(0), mData(0) {}

    ~MetaValue() {
        if (mType)
            mType->destruct(mData);
    }

    MetaType* type() const { return mType; }

    void read(void* mem, MetaType* type, PyrSlot* slot) {
        mData = mem;
        mType = type;
        mType->read(slot, mData);
    }

    void write(PyrSlot* slot) {
        if (mType)
            mType->write(slot, mData);
    }

    template <typename T> T value() {
        if (mType && qMetaTypeId<T>() == mType->id())
            return *reinterpret_cast<T*>(mData);
        else
            return T();
    }

    QGenericArgument toGenericArgument() {
        if (mType)
            return QGenericArgument(QMetaType::typeName(mType->id()), mData);
        else
            return QGenericArgument();
    }

    QGenericReturnArgument toGenericReturnArgument() {
        if (mType)
            return QGenericReturnArgument(QMetaType::typeName(mType->id()), mData);
        else
            return QGenericReturnArgument();
    }

private:
    MetaType* mType;
    void* mData;
};


template <typename T> inline MetaType* metaType() { return MetaTypeImpl<T>::instance; }

template <> struct TypeCodec<QVariant> {
    static QVariant safeRead(PyrSlot* slot) {
        MetaType* mt = MetaType::find(slot);
        return mt ? mt->read(slot) : QVariant();
    }
};

inline bool set(PyrSlot* s, const QVariant& var) {
    MetaType* mt = MetaType::find(var.userType());
    if (!mt)
        return false;
    mt->write(s, var);
    return true;
}

} // namespace QtCollider
