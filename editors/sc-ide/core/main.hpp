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

#ifndef SCIDE_MAIN_HPP_INCLUDED
#define SCIDE_MAIN_HPP_INCLUDED

#include <QAction>
#include <QObject>

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

#include "sc_process.hpp"
#include "sc_server.hpp"
#include "doc_manager.hpp"
#include "settings/manager.hpp"

namespace ScIDE {

class SessionManager;

// scide instances have a LocalServer. when called with an argument, it will try to reconnect
// to the instance with the lowest number.
class SingleInstanceGuard:
    public QObject
{
    Q_OBJECT

public:
    bool tryConnect(QStringList const & arguments);

public Q_SLOTS:
    void onNewIpcConnection()
    {
        mIpcSocket = mIpcServer->nextPendingConnection();
        connect(mIpcSocket, SIGNAL(disconnected()), mIpcSocket, SLOT(deleteLater()));
        connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcData()));
    }

    void onIpcData();

private:
    QLocalServer * mIpcServer;
    QLocalSocket * mIpcSocket;
};

class Main:
    public QObject
{
    Q_OBJECT

public:
    static Main * instance(void)
    {
        static Main * singleton = new Main;
        return singleton;
    }

    static void evaluateCode(QString const & text, bool silent = false)
    {
        instance()->scProcess()->evaluateCode(text, silent);
    }

    Settings::Manager *settings()       { return mSettings;    }
    DocumentManager * documentManager() { return mDocManager;  }
    SessionManager * sessionManager()   { return mSessionManager; }
    SCProcess * scProcess()             { return mSCProcess;   }
    ScServer  * scServer()              { return mSCServer;    }
    ScResponder  * scResponder()        { return mSCResponder; }

public Q_SLOTS:
    void storeSettings() {
        Q_EMIT(storeSettingsRequest(mSettings));
        mSettings->sync();
    }

    void applySettings() {
        Q_EMIT(applySettingsRequest(mSettings));
    }

    void quit();

Q_SIGNALS:
    void storeSettingsRequest(Settings::Manager *);
    void applySettingsRequest(Settings::Manager *);

private:
    Main(void);
    bool eventFilter(QObject *obj, QEvent *event);

    Settings::Manager *mSettings;
    ScResponder * mSCResponder;
    SCProcess * mSCProcess;
    ScServer * mSCServer;
    DocumentManager *mDocManager;
    SessionManager *mSessionManager;
};

}

#endif
