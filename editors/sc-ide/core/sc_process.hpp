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

namespace ScIDE {

class Main;

class SCProcess:
    public QProcess
{
Q_OBJECT
public:
    SCProcess( Main *main );

    enum SCProcessActionRole {
        StartSCLang = 0,
        RecompileClassLibrary,
        StopSCLang,
        RunMain,
        StopMain,

        SCProcessActionCount
    };

signals:
    void scPost(QString const &);
    void scStarted();

public slots:
    void start (void);

    void recompileClassLibrary (void)
    {
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
        closeWriteChannel();
    }

    void onReadyRead(void)
    {
        QByteArray out = QProcess::readAll();
        QString postString(out);
        if (postString.endsWith( '\n' ))
            postString.chop(1);
        emit scPost(postString);
    }

    void evaluateCode(QString const & commandString, bool silent = false)
    {
        QByteArray asciiCommandString = commandString.toAscii();
        size_t writtenBytes = write(asciiCommandString);
        if (writtenBytes != asciiCommandString.size()) {
            QString errorMessage ("error when passing data to sclang");
            emit scPost(errorMessage);
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
    void prepareActions(void)
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
        connect(action, SIGNAL(triggered()), this, SLOT(stopMain()));
    }

    Main * mMain;
    QAction * mActions[SCProcessActionCount];
};

}

#endif
