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

#ifndef SCIDE_SC_IPC_HPP_INCLUDED
#define SCIDE_SC_IPC_HPP_INCLUDED

#include <QCoreApplication>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

namespace ScIDE {

class SCIpcServer:
    public QLocalServer
{
    Q_OBJECT
    QString mIdeName;

public:
    explicit SCIpcServer( QObject *parent = 0 ):
        QLocalServer( parent ),
        mIdeName("SCIde_" + QString::number(QCoreApplication::applicationPid()))
    {
        connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }

private Q_SLOTS:
    void onSclangStart(void)
    {
        listen(mIdeName);
    }

    void onNewConnection(void)
    {
        QLocalSocket * socket = nextPendingConnection();
        connect(socket, SIGNAL(disconnected()),
                socket, SLOT(deleteLater()));

        QByteArray dataFromClient = socket->readAll();
        // TODO: dispatch messages from sclang, possibly respond
    }
};

}

#endif
