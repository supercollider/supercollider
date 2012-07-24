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
#include <QBuffer>

namespace ScIDE {

class SCIpcServer:
    public QLocalServer
{
    Q_OBJECT
    QString mIdeName;
    QLocalSocket * mSocket;
    QByteArray mReceivedData;

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
        if(!isListening()) // avoid a warning on stderr
            listen(mIdeName);
    }

Q_SIGNALS:
    void message(QString const &);
    void command(const QString & selector, const QString & data);

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
        mReceivedData.append(mSocket->readAll());
        tryEvaluateReceivedData();
    }

private:
    void tryEvaluateReceivedData(void);
};

}

#endif
