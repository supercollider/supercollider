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

#include <QAbstractNativeEventFilter>
#include <QAction>
#include <QObject>

#include "sc_process.hpp"
#include "sc_server.hpp"
#include "doc_manager.hpp"
#include "settings/manager.hpp"

namespace ScIDE {

class SessionManager;

// scide instances have a LocalServer. when called with an argument, it will try to reconnect
// to the instance with the lowest number.
class SingleInstanceGuard:
    public QObject, public IIpcHandler
{
    Q_OBJECT

public:
    static const int Port = 6770;
    bool tryConnect(QStringList const & arguments);
    void onIpcLog(const QString & message) override;
    void onIpcMessage(const QString &selector, const QVariantList &data) override;

public Q_SLOTS:
    void onNewIpcConnection();
    void onIpcData();

private:
    IIpcHandler * mHandler;
    ScIpcChannel * mIpcChannel;
        QTcpServer * mIpcServer;
        QTcpSocket * mIpcSocket;
};

class Main:
    public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    static Main * instance(void)
    {
        static Main * singleton = new Main;
        return singleton;
    }

    static ScProcess * scProcess()             { return instance()->mScProcess;      }
    static ScServer  * scServer()              { return instance()->mScServer;       }
    static SessionManager * sessionManager()   { return instance()->mSessionManager; }
    static DocumentManager * documentManager() { return instance()->mDocManager;     }
    static Settings::Manager *settings()       { return instance()->mSettings;       }

    static void evaluateCode(QString const & text, bool silent = false)
    {
        instance()->scProcess()->evaluateCode(text, silent);
    }

    static void evaluateCodeIfCompiled(QString const & text, bool silent = false)
    {
        if(instance()->scProcess()->compiled())
            evaluateCode(text, silent);
    }

    static bool openDocumentation(const QString & string);
    static bool openDocumentationForMethod(const QString & className, const QString & methodName);
    static void openDefinition(const QString &string, QWidget * parent);
    static void openCommandLine(const QString &string);
    static void findReferences(const QString &string, QWidget * parent);

    void onIpcLog(const QString & message);

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
    bool nativeEventFilter(const QByteArray&, void* message, long*);

    Settings::Manager *mSettings;
    ScProcess * mScProcess;
    ScServer * mScServer;
    DocumentManager *mDocManager;
    SessionManager *mSessionManager;
};

}

#endif
