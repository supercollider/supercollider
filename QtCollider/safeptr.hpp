/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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

#ifndef QC_SAFE_PTR_H
#define QC_SAFE_PTR_H

#include "Common.h"

#include <QApplication>
#include <QAtomicPointer>
#include <QAtomicInt>
#include <QMutex>

namespace QtCollider {

template<typename T> class SafePtr
{
public:
  SafePtr() : _ptr(0), _refCount(0), _mutex(0) {}

  SafePtr( const SafePtr & other )
  : _ptr( other._ptr ), _refCount( other._refCount ), _mutex( other._mutex )
  {
    ref();
  }

  SafePtr( T* ptr )
  : _ptr(new QAtomicPointer<T>(ptr)),
    _refCount(new QAtomicInt(1)),
    _mutex(new QMutex())
  {}

  SafePtr & operator= ( const SafePtr & other ) {
    deref();
    _ptr = other._ptr;
    _refCount = other._refCount;
    _mutex = other._mutex;
    ref();
    return *this;
  }

  ~SafePtr() {
    deref();
  }

  T * operator-> () const { return *_ptr; }

  T & operator* () const { return **_ptr; }

  operator T* () const { return _ptr ? *_ptr : 0; }

  T *ptr() const {
    if( _ptr ) return *_ptr;
    else return 0;
  }

  void *id() const { return (void*) _ptr; } // useful for checking internal pointer identity

  void lock() { if(_mutex) _mutex->lock(); }

  bool tryLock() { return _mutex ? _mutex->tryLock() : true; }

  void unlock() { if(_mutex) _mutex->unlock(); }

  void invalidate() { qcDebugMsg(2,"SafePtr: invalidating"); if(_ptr) *_ptr = 0; }

private:
  void ref() {
    if(_refCount) {
      _refCount->ref();
      qcDebugMsg(2,QString("SafePtr: +refcount = %1").arg(*_refCount));
    }
  }
  void deref() {
    if( _refCount ) {
      bool ref = _refCount->deref();
      qcDebugMsg(2,QString("SafePtr: -refcount = %1").arg(*_refCount));
      if( !ref ) {
        qcDebugMsg(2,"SafePtr: unreferenced!");
        delete _refCount;
        delete _ptr;
        delete _mutex;
      }
    }
  }
  QAtomicPointer<T> *_ptr;
  QAtomicInt * _refCount;
  QMutex * _mutex;
};

} // namespace QtCollider

#endif
