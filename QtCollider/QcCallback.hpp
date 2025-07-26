/*
    SuperCollider Language
    Copyright (c) 2018 SuperCollider Team
    https://supercollider.github.io/

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

#pragma once

#include <QObject>
#include <QPointer>

namespace QtCollider {

class QcCallback;

class QcCallbackWeakFunctor {
public:
    QcCallbackWeakFunctor(QPointer<QcCallback> cb): _cb(cb) {}

    template <typename RESULT> void operator()(RESULT r) const;

private:
    QPointer<QcCallback> _cb;
};

class QcCallback : public QObject {
    Q_OBJECT

public:
    QcCallback() {}

    template <typename CallbackT> void call(const CallbackT& result) { Q_EMIT(onCalled(result)); }

    QcCallbackWeakFunctor asFunctor() { return QcCallbackWeakFunctor(QPointer<QcCallback>(this)); }

Q_SIGNALS:
    void onCalled(bool);
    void onCalled(const QString&);
    void onCalled(const QVariant&);
    void onCalled(const QUrl&);
};

template <typename RESULT> void QcCallbackWeakFunctor::operator()(RESULT r) const {
    if (_cb) {
        _cb->call(r);
    }
}

} // namespace QtCollider

Q_DECLARE_METATYPE(QtCollider::QcCallback*);
