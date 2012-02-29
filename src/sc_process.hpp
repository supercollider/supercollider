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

#include <QProcess>

namespace ScIDE {

class SCProcess:
    public QProcess
{
Q_OBJECT
public:
    SCProcess( QObject *parent = 0 ):
        QProcess( parent )
    {
        connect(this, SIGNAL( readyRead() ),
                this, SLOT( onReadyRead() ));
    }

signals:
    void scPost(QString const &);

public slots:
    void start (void)
    {
        QString sclangProgramName = "sclang";
        QStringList sclangArguments;
        sclangArguments << "-i" << "scqt";
        QProcess::start(sclangProgramName, sclangArguments);
        bool processStarted = QProcess::waitForStarted();
        if (!processStarted) {
            QString errorMessage ("cannot start sclang process");
            emit scPost(errorMessage);
        }
    }

    void recompileClassLibrary (void)
    {
        write("\x18");
    }

    void stopLanguage (void)
    {
        closeWriteChannel();
    }

    void onReadyRead(void)
    {
        QByteArray out = QProcess::readAll();
        QString postString(out);
        emit scPost(postString);
    }
};

}

#endif
