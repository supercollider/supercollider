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

#pragma once

#include <QObject>
#include <QPointer>
#include <QList>

namespace ScIDE {

class SignalMultiplexer : public QObject {
    Q_OBJECT

public:
    enum ConnectionMode { ConnectionNecessary = 0, ConnectionOptional = 1 };

    SignalMultiplexer(QObject* parent = 0);

    void connect(QObject* sender, const char* signal, const char* slot, ConnectionMode = ConnectionNecessary);
    bool disconnect(QObject* sender, const char* signal, const char* slot);
    void connect(const char* signal, QObject* receiver, const char* slot, ConnectionMode = ConnectionNecessary);
    bool disconnect(const char* signal, QObject* receiver, const char* slot);
    bool disconnect(QObject* sender);

    QObject* currentObject() const { return mObject; }

public slots:
    void setCurrentObject(QObject* newObject);

private:
    struct Connection {
        QPointer<QObject> sender;
        QPointer<QObject> receiver;
        const char* signal;
        const char* slot;
        ConnectionMode mode;
    };

    void connect(const Connection& conn);
    void disconnect(const Connection& conn);

    QPointer<QObject> mObject;
    QList<Connection> mConnections;
};

} // namespace ScIDE
