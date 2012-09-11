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

#include "sc_introspection.hpp"
#include "settings/manager.hpp"

#include <QAction>
#include <QByteArray>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QUuid>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

namespace ScIDE {

class Main;
class ScIntrospectionParser;

class SCProcess:
    public QProcess
{
    Q_OBJECT

public:
    SCProcess( Main *, class ScResponder * responder, Settings::Manager * );

    enum SCProcessActionRole {
        StartSCLang = 0,
        RecompileClassLibrary,
        StopSCLang,
        RestartSCLang,
        RunMain,
        StopMain,

        SCProcessActionCount
    };

    const ScLanguage::Introspection & introspection() { return mIntrospection; }

    void sendRequest( const QString &id, const QString &command, const QString &data )
    {
        QString cmd = QString("ScIDE.request(\"%1\",'%2',\"%3\")")
            .arg(id)
            .arg(command)
            .arg(data);

        evaluateCode(cmd, true);
    }

    void setActiveDocument(class Document *);
    void sendActiveDocument();

    QAction *action(SCProcessActionRole role)
    {
        return mActions[role];
    }

    void emitClassLibraryRecompiled()
    {
        emit classLibraryRecompiled();
    }

Q_SIGNALS:
    void scPost(QString const &);
    void statusMessage(const QString &);
    void response(const QString & id, const QString & data);
    void classLibraryRecompiled();

public slots:
    void recompileClassLibrary (void);
    void runMain(void)  { evaluateCode("thisProcess.run", false); }
    void stopMain(void) { evaluateCode("thisProcess.stop", false); }
    void startLanguage (void);
    void stopLanguage (void);
    void restartLanguage (void);
    void onReadyRead(void);
    void evaluateCode(QString const & commandString, bool silent = false);

    void swapIntrospection (ScLanguage::Introspection *newIntrospection)
    {
        // LATER: use c++11/std::move
        mIntrospection = *newIntrospection;
        delete newIntrospection;
    }

private slots:
    void onNewIpcConnection();
    void onIpcData();
    void finalizeConnection();

private:
    void onSclangStart();

    void prepareActions(Settings::Manager * settings);

    QAction * mActions[SCProcessActionCount];

    ScLanguage::Introspection mIntrospection;
    ScIntrospectionParser *mIntrospectionParser;

    QLocalServer *mIpcServer;
    QLocalSocket *mIpcSocket;
    QString mIpcServerName;
    QByteArray mIpcData;

    QString mCurrentDocumentPath;
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

        connect(mSc, SIGNAL(classLibraryRecompiled()),
                this, SLOT(onCancelRequest()));
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
    void requestCanceled();

private slots:
    void onCancelRequest()
    {
        cancel();
        emit requestCanceled();
        deleteLater();
    }

    void onResponse( const QString & responseId, const QString & responseData )
    {
        if (responseId == mId.toString())
            emit response(mCommand, responseData);
    }

private:
    QString mCommand;
    QUuid mId;
    SCProcess *mSc;
};

class ScResponder : public QObject
{
    Q_OBJECT

public:
    ScResponder( QObject * parent = 0):
        QObject(parent)
    {}

Q_SIGNALS:
    void serverRunningChanged( bool serverRunning, const QString & hostName, int port );
    void newIntrospectionData( const QString & yaml );

private Q_SLOTS:
    void onResponse( const QString & selector, const QString & data );

private:
    void handleOpenFile( const QString & data ) const;
    void handleServerRunningChanged( const QString & data );
};

class ScIntrospectionParserWorker : public QObject
{
    Q_OBJECT
signals:
    void done( ScLanguage::Introspection * output );
private slots:
    void process( const QString & input );

    void quit()
    {
        thread()->quit();
    }
};

class ScIntrospectionParser : public QThread
{
    Q_OBJECT
public:
    ScIntrospectionParser( ScResponder * responder, QObject * parent = 0 ):
        QThread(parent)
    {
        connect(responder, SIGNAL(newIntrospectionData(QString)),
                &mWorker, SLOT(process(QString)), Qt::QueuedConnection);
        connect(&mWorker, SIGNAL(done(ScLanguage::Introspection*)),
                this, SIGNAL(done(ScLanguage::Introspection*)), Qt::QueuedConnection);
        mWorker.moveToThread(this);
    }
    ~ScIntrospectionParser()
    {
        QMetaObject::invokeMethod(&mWorker, "quit");
        wait();
    }

signals:
    void done( ScLanguage::Introspection * );

private:
    ScIntrospectionParserWorker mWorker;
};

}

#endif
