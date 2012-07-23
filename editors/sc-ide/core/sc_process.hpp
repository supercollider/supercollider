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
#include <QDebug>

#include "sc_ipc.hpp"

namespace ScIDE {

class Main;

class SCProcess:
    public QProcess
{
Q_OBJECT
    SCIpcServer * mIPC;

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

Q_SIGNALS:
    void scPost(QString const &);
    void statusMessage(const QString &);

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
        QString commandString = QString("ScIDE.getClassDefinitions(%1)").arg(classname);
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

private:
    void onSclangStart()
    {
        mIPC->onSclangStart();
        QString command = QString("ScIDE.connect(\"%1\")").arg(mIPC->ideName());
        evaluateCode ( command, true );
    }

    void prepareActions(Main *);

    QAction * mActions[SCProcessActionCount];
};

}

#endif
