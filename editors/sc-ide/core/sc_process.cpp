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

#include <QBuffer>
#include <QCoreApplication>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QTextDocumentFragment>

#include <QtConcurrent>

#include "main.hpp"
#include "main_window.hpp"
#include "sc_introspection.hpp"
#include "sc_process.hpp"
#include "sc_server.hpp"
#include "settings/manager.hpp"
#include "util/standard_dirs.hpp"
#include "../primitives/localsocket_utils.hpp"

#include "../widgets/help_browser.hpp"

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>

namespace ScIDE {

ScProcess::ScProcess( Settings::Manager * settings, QObject * parent ):
    QProcess( parent ),
    mIpcServer( new QLocalServer(this) ),
    mIpcSocket(NULL),
    mIpcServerName("SCIde_" + QString::number(QCoreApplication::applicationPid())),
    mTerminationRequested(false),
    mCompiled(false)
{
    prepareActions(settings);

    connect(this, SIGNAL( readyRead() ),
            this, SLOT( onReadyRead() ));
    connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));
    connect(this, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
}

void ScProcess::prepareActions(Settings::Manager * settings)
{
    QAction * action;

    const QString interpreterCategory(tr("Interpreter"));

    mActions[ToggleRunning] = action = new QAction(tr("Boot or Quit Interpreter"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleRunning()) );
    //settings->addAction( action, "interpreter-toggle-running", interpreterCategory);

    mActions[Start] = action =
        new QAction(QIcon::fromTheme("system-run"), tr("Boot Interpreter"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(startLanguage()) );
    settings->addAction( action, "interpreter-start", interpreterCategory);

    mActions[Stop] = action =
        new QAction(QIcon::fromTheme("system-shutdown"), tr("Quit Interpreter"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(stopLanguage()) );
    settings->addAction( action, "interpreter-stop", interpreterCategory);

    mActions[Restart] = action = new QAction(
        QIcon::fromTheme("system-reboot"), tr("Reboot Interpreter"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(restartLanguage()) );
    settings->addAction( action, "interpreter-restart", interpreterCategory);

    mActions[RecompileClassLibrary] = action = new QAction(
        QIcon::fromTheme("system-reboot"), tr("Recompile Class Library"), this);
    action->setShortcut(tr("Ctrl+Shift+l", "Recompile Class Library)"));
    connect(action, SIGNAL(triggered()), this, SLOT(recompileClassLibrary()) );
    settings->addAction( action, "interpreter-recompile-lib", interpreterCategory);

    mActions[StopMain] = action = new QAction(
        QIcon::fromTheme("media-playback-stop"), tr("Stop"), this);
    action->setShortcut(tr("Ctrl+.", "Stop (a.k.a. cmd-period)"));
    action->setShortcutContext(Qt::ApplicationShortcut);
    connect(action, SIGNAL(triggered()), this, SLOT(stopMain()));
    settings->addAction( action, "interpreter-main-stop", interpreterCategory);
    
    mActions[ShowQuarks] = action = new QAction(tr("Quarks"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(showQuarks()));
    settings->addAction( action, "interpreter-show-quarks-gui", interpreterCategory);

    connect( mActions[Start], SIGNAL(changed()), this, SLOT(updateToggleRunningAction()) );
    connect( mActions[Stop], SIGNAL(changed()), this, SLOT(updateToggleRunningAction()) );

    onProcessStateChanged(QProcess::NotRunning);
}

void ScProcess::updateToggleRunningAction()
{
    QAction *targetAction = state() == QProcess::NotRunning ? mActions[Start] : mActions[Stop];

    mActions[ToggleRunning]->setText( targetAction->text() );
    mActions[ToggleRunning]->setIcon( targetAction->icon() );
    mActions[ToggleRunning]->setShortcut( targetAction->shortcut() );
}

void ScProcess::toggleRunning()
{
    switch(state()) {
    case NotRunning:
        startLanguage();
        break;
    default:
        stopLanguage();
    }
}

void ScProcess::startLanguage (void)
{
    if (state() != QProcess::NotRunning) {
        statusMessage(tr("Interpreter is already running."));
        return;
    }

    Settings::Manager *settings = Main::settings();
    settings->beginGroup("IDE/interpreter");

    QString workingDirectory = settings->value("runtimeDir").toString();
    QString configFile = settings->value("configFile").toString();
    bool standalone = settings->value("standalone").toBool();

    settings->endGroup();

    QString sclangCommand;
#ifdef Q_OS_MAC
    sclangCommand = standardDirectory(ScResourceDir) + "/../MacOS/sclang";
#else
    sclangCommand = "sclang";
#endif

    QStringList sclangArguments;
    if(!configFile.isEmpty())
        sclangArguments << "-l" << configFile;
    sclangArguments << "-i" << "scqt";
    if(standalone)
        sclangArguments << "-a";

    if(!workingDirectory.isEmpty())
        setWorkingDirectory(workingDirectory);

    QProcess::start(sclangCommand, sclangArguments);
    bool processStarted = QProcess::waitForStarted();
    if (!processStarted)
        emit statusMessage(tr("Failed to start interpreter!"));
}

void ScProcess::recompileClassLibrary (void)
{
    if(state() != QProcess::Running) {
        emit statusMessage(tr("Interpreter is not running!"));
        return;
    }
    mCompiled = false;
    write("\x18");
}


void ScProcess::stopLanguage (void)
{
    if(state() != QProcess::Running) {
        emit statusMessage(tr("Interpreter is not running!"));
        return;
    }

    evaluateCode("0.exit", true);    
    mCompiled = false;
    mTerminationRequested   = true;
    mTerminationRequestTime = QDateTime::currentDateTimeUtc();

    bool finished = waitForFinished(1000);
    if ( !finished && (state() != QProcess::NotRunning) ) {
        terminate();
        bool reallyFinished = waitForFinished(200);
        if (!reallyFinished)
            emit statusMessage(tr("Failed to stop interpreter!"));
    }
    closeWriteChannel();
    mTerminationRequested = false;
}

void ScProcess::restartLanguage()
{
    mCompiled = false;
    stopLanguage();
    startLanguage();
}

void ScProcess::stopMain(void)
{
    evaluateCode("thisProcess.stop", true);
}

void ScProcess::showQuarks(void)
{
    evaluateCode("Quarks.gui",true);
}


void ScProcess::onReadyRead(void)
{
    if (mTerminationRequested) {
        // when stopping the language, we don't want to post for longer than 200 ms to prevent the UI to freeze
        if (QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - mTerminationRequestTime.toMSecsSinceEpoch() > 200)
            return;
    }

    QByteArray out = QProcess::readAll();
    QString postString = QString::fromUtf8(out);
    emit scPost(postString);
}

void ScProcess::evaluateCode(QString const & commandString, bool silent)
{
    if(state() != QProcess::Running) {
        emit statusMessage(tr("Interpreter is not running!"));
        return;
    }

    QByteArray bytesToWrite = commandString.toUtf8();
    size_t writtenBytes = write(bytesToWrite);
    if (writtenBytes != bytesToWrite.size()) {
        emit statusMessage(tr("Error when passing data to interpreter!"));
        return;
    }

    char commandChar = silent ? '\x1b' : '\x0c';

    write( &commandChar, 1 );
}

void ScProcess::onNewIpcConnection()
{
    if (mIpcSocket)
        // we can handle only one ipc connection at a time
        mIpcSocket->disconnect();

    mIpcSocket = mIpcServer->nextPendingConnection();
    connect(mIpcSocket, SIGNAL(disconnected()), this, SLOT(finalizeConnection()));
    connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcData()));
}

void ScProcess::finalizeConnection()
{
    mIpcData.clear();
    mIpcSocket->deleteLater();
    mIpcSocket = NULL;
}

void ScProcess::onProcessStateChanged(QProcess::ProcessState state)
{
    switch (state) {
    case QProcess::Starting:
        mActions[Start]->setEnabled(false);
        mActions[Stop]->setEnabled(true);
        mActions[Restart]->setEnabled(true);
        updateToggleRunningAction();

        break;

    case QProcess::Running:
        mActions[StopMain]->setEnabled(true);
        mActions[ShowQuarks]->setEnabled(true);
        mActions[RecompileClassLibrary]->setEnabled(true);

        onStart();

        break;

    case QProcess::NotRunning:
        mActions[Start]->setEnabled(true);
        mActions[Stop]->setEnabled(false);
        mActions[Restart]->setEnabled(false);
        mActions[StopMain]->setEnabled(false);
        mActions[ShowQuarks]->setEnabled(false);
        mActions[RecompileClassLibrary]->setEnabled(false);
        updateToggleRunningAction();
        postQuitNotification();
        mCompiled = false;
        break;
    }
}

void ScProcess::postQuitNotification()
{
    QString message;
    switch (exitStatus()) {
    case QProcess::CrashExit:
        message = tr("Interpreter has crashed or stopped forcefully. [Exit code: %1]\n").arg(exitCode());
        break;
    default:
        message = tr("Interpreter has quit. [Exit code: %1]\n").arg(exitCode());
    }
    emit scPost(message);
}


void ScProcess::onIpcData()
{
    mIpcData.append(mIpcSocket->readAll());
    // After we have put the data in the buffer, process it    
    int avail = mIpcData.length();
    do {
        if (mReadSize == 0 && avail > 4){
            mReadSize = ArrayToInt(mIpcData.left(4));
            mIpcData.remove(0, 4);
            avail -= 4;
        }

        if (mReadSize > 0 && avail >= mReadSize){
            QByteArray baReceived(mIpcData.left(mReadSize));
            mIpcData.remove(0, mReadSize);
            mReadSize = 0;
            avail -= mReadSize;

            QDataStream in(baReceived);
            in.setVersion(QDataStream::Qt_4_6);
            QString selector, message;
            in >> selector;
            if (in.status() != QDataStream::Ok)
                return;

            in >> message;
            if (in.status() != QDataStream::Ok)
                return;

            onResponse(selector, message);
            emit response(selector, message);
        }
    } while ((mReadSize == 0 && avail > 4) || (mReadSize > 0 && avail > mReadSize));
}

void ScProcess::onResponse( const QString & selector, const QString & data )
{
    if (selector == QStringLiteral("introspection")) {
        using ScLanguage::Introspection;

        auto watcher = new QFutureWatcher<Introspection>(this);
        connect( watcher, &QFutureWatcher<Introspection>::finished, [=]{
            try {
                Introspection newIntrospection = watcher->result();
                mIntrospection = std::move(newIntrospection);
                emit introspectionChanged();
            } catch (std::exception & e) {
                MainWindow::instance()->showStatusMessage(e.what());
            }
            watcher->deleteLater();
        });

        // Start the computation.
        QFuture<Introspection> future = QtConcurrent::run( [] (QString data) {
            return ScLanguage::Introspection(data);
        }, data );
        watcher->setFuture(future);
    }

    else if (selector == QStringLiteral("classLibraryRecompiled")){
        mCompiled = true;
        emit classLibraryRecompiled();
    }

    else if (selector == QStringLiteral("requestCurrentPath"))
        Main::documentManager()->sendActiveDocument();
}

void ScProcess::onStart()
{
    if(!mIpcServer->isListening()) // avoid a warning on stderr
        mIpcServer->listen(mIpcServerName);

    QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);
    evaluateCode ( command, true );
    Main::documentManager()->sendActiveDocument();
}

    
void ScProcess::updateTextMirrorForDocument ( Document * doc, int position, int charsRemoved, int charsAdded )
{
    if (!mIpcSocket)
        return;

    if (mIpcSocket->state() != QLocalSocket::ConnectedState)
        return;

    QVariantList argList;
    argList.append(QVariant(doc->id()));
    argList.append(QVariant(position));
    argList.append(QVariant(charsRemoved));

    QTextCursor cursor = QTextCursor(doc->textDocument());
    cursor.setPosition(position, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
    argList.append(QVariant(cursor.selection().toPlainText()));

    try {
        sendSelectorAndData(mIpcSocket, QStringLiteral("updateDocText"), argList);
    } catch (std::exception const & e) {
        scPost(QStringLiteral("Exception during ScIDE_Send: %1\n").arg(e.what()));
    }
}
    
void ScProcess::updateSelectionMirrorForDocument ( Document * doc, int start, int range )
{
    if (!mIpcSocket)
        return;

    if (mIpcSocket->state() != QLocalSocket::ConnectedState)
        return;

    QVariantList argList;
    argList.append(QVariant(doc->id()));
    argList.append(QVariant(start));
    argList.append(QVariant(range));


    try {
        sendSelectorAndData(mIpcSocket, QStringLiteral("updateDocSelection"), argList);
    } catch (std::exception const & e) {
        scPost(QStringLiteral("Exception during ScIDE_Send: %1\n").arg(e.what()));
    }
}

} // namespace ScIDE
