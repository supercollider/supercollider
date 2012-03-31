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
    QLocalSocket * mSocket;

public:
    explicit SCIpcServer( QObject *parent = 0 ):
        QLocalServer( parent ),
        mIdeName("SCIde_" + QString::number(QCoreApplication::applicationPid()))
    {
        connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }

    QString const & ideName() const
    {
        return mIdeName;
    }

    void onSclangStart(void)
    {
        listen(mIdeName);
    }

private Q_SLOTS:
    void onNewConnection(void)
    {
        mSocket = nextPendingConnection();
        connect(mSocket, SIGNAL(disconnected()),
                mSocket, SLOT(deleteLater()));
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()));
    }

    void readDataFromSocket()
    {
        QByteArray data = mSocket->readAll();

        for (;;) {
            QDataStream in(&data, QIODevice::ReadOnly);
            in.setVersion(QDataStream::Qt_4_6);

            QString selector, message;
            in >> selector;
            in >> message;
            if (in.status() != QDataStream::Ok) {
                bool readSuccessful = mSocket->waitForReadyRead();
                if (!readSuccessful) {
                    qDebug("readDataFromSocket: problem reading large message");
                    return;
                }
                data.append(mSocket->readAll());
                continue;
            }
            qDebug("readDataFromSocket %s %s", selector.toStdString().c_str(), message.toStdString().c_str());
            return;
        }
    }
};

}

#endif
