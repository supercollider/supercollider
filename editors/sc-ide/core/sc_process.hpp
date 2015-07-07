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

#include <QAction>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QUuid>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

namespace ScIDE {

namespace Settings { class Manager; }
class ScIntrospectionParser;

class ScProcess:
    public QProcess
{
    Q_OBJECT

public:
    ScProcess( Settings::Manager *, QObject * parent );

    enum ActionRole {
        ToggleRunning = 0,
        Start,
        Stop,
        Restart,
        RecompileClassLibrary,
        StopMain,

        ActionCount
    };

    const ScLanguage::Introspection & introspection() { return mIntrospection; }

    void sendRequest( const QString &id, const QString &command, const QString &data )
    {
        QString cmd = QStringLiteral("ScIDE.request(\"%1\",'%2',\"%3\")")
            .arg(id)
            .arg(command)
            .arg(data);

        evaluateCode(cmd, true);
    }

    QAction *action(ActionRole role)
    {
        return mActions[role];
    }
    
    bool compiled() { return mCompiled; }
    
    void post(QString & text) { scPost(text); }
    void updateTextMirrorForDocument ( class Document * doc, int position, int charsRemoved, int charsAdded );
    void updateSelectionMirrorForDocument ( class Document * doc, int start, int range);

public slots:
    void toggleRunning();
    void startLanguage (void);
    void stopLanguage (void);
    void restartLanguage (void);
    void recompileClassLibrary (void);
    void stopMain(void);
    void evaluateCode(QString const & commandString, bool silent = false);

signals:
    void scPost(QString const &);
    void statusMessage(const QString &);
    void response(const QString & selector, const QString & data);
    void classLibraryRecompiled();
    void introspectionAboutToSwap();

private slots:
    void swapIntrospection (ScLanguage::Introspection *newIntrospection)
    {
        emit introspectionAboutToSwap();
        // LATER: use c++11/std::move
        mIntrospection = *newIntrospection;
        delete newIntrospection;
    }
    void onNewIpcConnection();
    void onIpcData();
    void finalizeConnection();
    void onProcessStateChanged( QProcess::ProcessState state);
    void onReadyRead(void);
    void updateToggleRunningAction();

private:
    void onStart();
    void onResponse( const QString & selector, const QString & data );

    void prepareActions(Settings::Manager * settings);
    void postQuitNotification();

    QAction * mActions[ActionCount];

    ScLanguage::Introspection mIntrospection;
    ScIntrospectionParser *mIntrospectionParser;

    QLocalServer *mIpcServer;
    QLocalSocket *mIpcSocket;
    QString mIpcServerName;
    QByteArray mIpcData;

    bool mTerminationRequested;
    QDateTime mTerminationRequestTime;
    bool mCompiled;
};

class ScRequest : public QObject
{
    Q_OBJECT
public:
    ScRequest( ScProcess *sc, QObject * parent = 0 ):
        QObject(parent),
        mSc(sc)
    {
        connect(mSc, SIGNAL(response(QString,QString)),
                this, SLOT(onResponse(QString,QString)));

        connect(mSc, SIGNAL(classLibraryRecompiled()),
                this, SLOT(cancel()));
    }

    void send( const QString & command, const QString & data )
    {
        mId = QUuid::createUuid();
        mCommand = command;
        mSc->sendRequest(mId.toString(), command, data);
    }

public slots:
    void cancel()
    {
        mId = QUuid();
        emit cancelled();
    }

signals:
    void response( const QString & command, const QString & data );
    void cancelled();

private slots:
    void onResponse( const QString & responseId, const QString & responseData )
    {
        if (responseId == mId.toString())
            emit response(mCommand, responseData);
    }

private:
    QString mCommand;
    QUuid mId;
    ScProcess *mSc;
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
    ScIntrospectionParser( QObject * parent = 0 ):
        QThread(parent)
    {
        connect(this, SIGNAL(newIntrospectionData(QString)),
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

    void process( const QString & introspectionData )
    {
        emit newIntrospectionData(introspectionData);
    }

signals:
    void newIntrospectionData( const QString & data );
    void done( ScLanguage::Introspection * );

private:
    ScIntrospectionParserWorker mWorker;
};

}

#endif
