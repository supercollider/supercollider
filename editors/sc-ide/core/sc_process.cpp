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

namespace ScIDE {

SCProcess::SCProcess( Main *main ):
    QProcess( main ),
    mMain( main )
{
    prepareActions();

    connect(this, SIGNAL( readyRead() ),
            this, SLOT( onReadyRead() ));
}

void SCProcess::start (void)
{
    QSettings *settings = mMain->settings();
    settings->beginGroup("IDE/interpreter");

    QString sclangProgramName = settings->value("command", "sclang").toString();
    QString workingDirectory = settings->value("runtimeDir").toString();
    QString configFile = settings->value("configFile").toString();

    settings->endGroup();

    QStringList sclangArguments;
    if(!configFile.isEmpty())
        sclangArguments << "-l" << configFile;
    sclangArguments << "-i" << "scqt";

    if(!workingDirectory.isEmpty())
        setWorkingDirectory(workingDirectory);

    QProcess::start(sclangProgramName, sclangArguments);
    bool processStarted = QProcess::waitForStarted();
    if (!processStarted) {
        QString errorMessage ("cannot start sclang process");
        emit scPost(errorMessage);
    } else
        emit scStarted();
}

} // namespace ScIDE
