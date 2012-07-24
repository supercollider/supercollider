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

#include "sc_process.hpp"
#include "main.hpp"
#include "settings/manager.hpp"

namespace ScIDE {

SCProcess::SCProcess( Main *parent ):
    QProcess( parent ), mIPC( new SCIpcServer(this) )
{
    prepareActions(parent);

    connect(this, SIGNAL( readyRead() ),
            this, SLOT( onReadyRead() ));
    connect(mIPC, SIGNAL(message(QString)), this, SIGNAL(statusMessage(QString)));
    connect(mIPC, SIGNAL(command(QString,QString)), this, SIGNAL(scCommand(QString,QString)));
}

void SCProcess::start (void)
{
    if (state() != QProcess::NotRunning) {
        statusMessage("Interpreter is already running.");
        return;
    }

    Settings::Manager *settings = Main::instance()->settings();
    settings->beginGroup("IDE/interpreter");

    QString sclangCommand = settings->value("command").toString();
    QString workingDirectory = settings->value("runtimeDir").toString();
    QString configFile = settings->value("configFile").toString();

    settings->endGroup();

    if(sclangCommand.isEmpty()) sclangCommand = "sclang";

    QStringList sclangArguments;
    if(!configFile.isEmpty())
        sclangArguments << "-l" << configFile;
    sclangArguments << "-i" << "scqt";

    if(!workingDirectory.isEmpty())
        setWorkingDirectory(workingDirectory);

    QProcess::start(sclangCommand, sclangArguments);
    bool processStarted = QProcess::waitForStarted();
    if (!processStarted) {
        emit statusMessage("Could not start interpreter!");
    } else
        onSclangStart();
}

void SCProcess::prepareActions(Main * main)
{
    QAction * action;
    mActions[StartSCLang] = action = new QAction(
        QIcon::fromTheme("system-run"), tr("Start SCLang"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(start()) );

    mActions[RecompileClassLibrary] = action = new QAction(
        QIcon::fromTheme("system-reboot"), tr("Recompile Class Library"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(recompileClassLibrary()) );

    mActions[StopSCLang] = action = new QAction(
        QIcon::fromTheme("system-shutdown"), tr("Stop SCLang"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(stopLanguage()) );

    mActions[RunMain] = action = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("Run Main"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(runMain()));

    mActions[StopMain] = action = new QAction(
        QIcon::fromTheme("process-stop"), tr("Stop Main"), this);
    action->setShortcut(tr("Ctrl+.", "Stop Main (a.k.a. cmd-period)"));
    connect(action, SIGNAL(triggered()), this, SLOT(stopMain()));

    Settings::Manager *settings = main->settings();
    for (int i = 0; i < SCProcessActionCount; ++i)
        settings->addAction( mActions[i] );
}

} // namespace ScIDE
