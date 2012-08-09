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

#ifndef SCIDE_SC_PROCESS_HPP_INCLUDED
#define SCIDE_SC_PROCESS_HPP_INCLUDED

#include <QAction>
#include <QProcess>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QByteArray>
#include <QUuid>
#include <QDebug>

namespace ScIDE {

class Main;

class SCProcess:
    public QProcess
{
Q_OBJECT

public:
    SCProcess( Main * );

    enum SCProcessActionRole {
        StartSCLang = 0,
        RecompileClassLibrary,
        StopSCLang,
        RunMain,
        StopMain,

        SCProcessActionCount
    };

    void sendRequest( const QString &id, const QString &command, const QString &data )
    {
        QString cmd = QString("ScIDE.request(\"%1\",'%2',\"%3\")")
            .arg(id)
            .arg(command)
            .arg(data);

        evaluateCode(cmd, true);
    }

Q_SIGNALS:
    void scPost(QString const &);
    void statusMessage(const QString &);
    void response(const QString & id, const QString & data);

public slots:
    void start (void);

    void recompileClassLibrary (void)
    {
        if(state() != QProcess::Running) {
            emit statusMessage("Interpreter is not running!");
            return;
        }

        write("\x18");
    }

    void runMain(void)
    {
        evaluateCode("thisProcess.run", false);
    }

    void stopMain(void)
    {
        evaluateCode("thisProcess.stop", false);
    }

    void stopLanguage (void)
    {
        if(state() != QProcess::Running) {
            emit statusMessage("Interpreter is not running!");
            return;
        }

        closeWriteChannel();
    }

    void getClassDefinitions(QString const & classname)
    {
        QString commandString = QString("ScIDE.sendClassDefinitions('%1')").arg(classname);
        evaluateCode(commandString, true);
    }

    void getMethodDefinitions(QString const & methodName)
    {
        evaluateCode(QString("ScIDE.sendMethodDefinitions(\"%1\")").arg(methodName), true);
    }

    void onReadyRead(void)
    {
        QByteArray out = QProcess::readAll();
        QString postString = QString::fromUtf8(out);
        if (postString.endsWith( '\n' ))
            postString.chop(1);
        emit scPost(postString);
    }

    void evaluateCode(QString const & commandString, bool silent = false)
    {
        if(state() != QProcess::Running) {
            emit statusMessage("Interpreter is not running!");
            return;
        }

        QByteArray bytesToWrite = commandString.toUtf8();
        size_t writtenBytes = write(bytesToWrite);
        if (writtenBytes != bytesToWrite.size()) {
            emit statusMessage("Error when passing data to interpreter!");
            return;
        }

        char commandChar = silent ? '\x1b' : '\x0c';

        write( &commandChar, 1 );
    }

    QAction *action(SCProcessActionRole role)
    {
        return mActions[role];
    }

private slots:
    void onNewIpcConnection()
    {
        mIpcSocket = mIpcServer->nextPendingConnection();
        connect(mIpcSocket, SIGNAL(disconnected()), mIpcSocket, SLOT(deleteLater()));
        connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcData()));
    }

    void onIpcData();

private:
    void onSclangStart()
    {
        if(!mIpcServer->isListening()) // avoid a warning on stderr
            mIpcServer->listen(mIpcServerName);

        QString command = QString("ScIDE.connect(\"%1\")").arg(mIpcServerName);
        evaluateCode ( command, true );
    }

    void prepareActions(Main *);

    QAction * mActions[SCProcessActionCount];

    QLocalServer *mIpcServer;
    QLocalSocket *mIpcSocket;
    QString mIpcServerName;
    QByteArray mIpcData;
};

class ScRequest : public QObject
{
    Q_OBJECT
public:
    ScRequest( SCProcess *sc, QObject * parent = 0 ):
        QObject(parent),
        mSc(sc)
    {
        connect(mSc, SIGNAL(response(QString,QString)),
                this, SLOT(onResponse(QString,QString)));
    }

    void send( const QString & command, const QString & data )
    {
        mId = QUuid::createUuid();
        mCommand = command;
        mSc->sendRequest(mId.toString(), command, data);
    }

    void cancel()
    {
        mId = QUuid();
    }

signals:
    void response( const QString & command, const QString & data );

private slots:
    void onResponse( const QString & responseId, const QString & responseData )
    {
        if (responseId == mId.toString()) {
            emit response(mCommand, responseData);
        }
    }

private:
    QString mCommand;
    QUuid mId;
    SCProcess *mSc;
};

}

#endif
