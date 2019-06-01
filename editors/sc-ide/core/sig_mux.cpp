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

#include "sig_mux.hpp"
#include <cstring>

namespace ScIDE {

SignalMultiplexer::SignalMultiplexer(QObject* parent): QObject(parent) {}

void SignalMultiplexer::connect(QObject* sender, const char* signal, const char* slot, ConnectionMode mode) {
    Q_ASSERT(strlen(signal) > 0);
    Q_ASSERT(strlen(slot) > 0);

    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.slot = slot;
    conn.mode = mode;

    mConnections << conn;
    connect(conn);
}

bool SignalMultiplexer::disconnect(QObject* sender) {
    bool result = false;
    QList<Connection>::Iterator it;
    for (it = mConnections.begin(); it != mConnections.end(); ++it) {
        Connection conn = *it;
        if ((QObject*)conn.sender == sender) {
            disconnect(conn);
            mConnections.erase(it);
            result = true;
        }
    }

    return result;
}

bool SignalMultiplexer::disconnect(QObject* sender, const char* signal, const char* slot) {
    QList<Connection>::Iterator it;
    for (it = mConnections.begin(); it != mConnections.end(); ++it) {
        Connection conn = *it;
        if ((QObject*)conn.sender == sender && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0) {
            disconnect(conn);
            mConnections.erase(it);
            return true;
        }
    }
    return false;
}

void SignalMultiplexer::connect(const char* signal, QObject* receiver, const char* slot, ConnectionMode mode) {
    Connection conn;
    conn.receiver = receiver;
    conn.signal = signal;
    conn.slot = slot;
    conn.mode = mode;

    mConnections << conn;
    connect(conn);
}

bool SignalMultiplexer::disconnect(const char* signal, QObject* receiver, const char* slot) {
    QList<Connection>::Iterator it;
    for (it = mConnections.begin(); it != mConnections.end(); ++it) {
        Connection conn = *it;
        if ((QObject*)conn.receiver == receiver && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0) {
            disconnect(conn);
            mConnections.erase(it);
            return true;
        }
    }
    return false;
}

void SignalMultiplexer::connect(const Connection& conn) {
    if (!mObject)
        return;
    if (!conn.sender && !conn.receiver)
        return;

    bool optional = conn.mode == ConnectionOptional;

    // HACK: for QMetaObject::indexOfSlot/Signal we have to skip first char
    // that's added by SLOT() and SIGNAL() macros

    if (conn.sender) {
        if (optional && (mObject->metaObject()->indexOfSlot(conn.slot + 1) == -1))
            return;
        QObject::connect((QObject*)conn.sender, conn.signal, (QObject*)mObject, conn.slot);
    } else {
        if (optional && (mObject->metaObject()->indexOfSignal(conn.signal + 1) == -1))
            return;
        QObject::connect((QObject*)mObject, conn.signal, (QObject*)conn.receiver, conn.slot);
    }
}

void SignalMultiplexer::disconnect(const Connection& conn) {
    if (!mObject)
        return;
    if (!conn.sender && !conn.receiver)
        return;

    bool optional = conn.mode == ConnectionOptional;

    // HACK: for QMetaObject::indexOfSlot/Signal we have to skip first char
    // that's added by SLOT() and SIGNAL() macros

    if (conn.sender) {
        if (optional && (mObject->metaObject()->indexOfSlot(conn.slot + 1) == -1))
            return;
        QObject::disconnect((QObject*)conn.sender, conn.signal, (QObject*)mObject, conn.slot);
    } else {
        if (optional && (mObject->metaObject()->indexOfSignal(conn.signal + 1) == -1))
            return;
        QObject::disconnect((QObject*)mObject, conn.signal, (QObject*)conn.receiver, conn.slot);
    }
}


void SignalMultiplexer::setCurrentObject(QObject* newObject) {
    if (newObject == mObject)
        return;

    QList<Connection>::ConstIterator it;
    for (it = mConnections.begin(); it != mConnections.end(); ++it)
        disconnect(*it);
    mObject = newObject;
    for (it = mConnections.begin(); it != mConnections.end(); ++it)
        connect(*it);
}

} // namespace ScIDE
