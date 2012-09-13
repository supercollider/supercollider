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

#ifndef SCIDE_SC_SERVER_HPP_INCLUDED
#define SCIDE_SC_SERVER_HPP_INCLUDED

#include <QObject>
#include <QtNetwork/QUdpSocket>

namespace ScIDE {

class ScServer : public QObject
{
    Q_OBJECT

public:
    ScServer(QObject * parent);
    void timerEvent(QTimerEvent * event);

    bool isRunning() { return mPort != 0; }

public Q_SLOTS:

    void boot();
    void reboot();
    void quit();
    void queryAllNodes(bool dumpControls);

    void onServerRunningChanged( bool running, QString const & hostName, int port )
    {
        if (running) {
            mServerAddress = QHostAddress(hostName);
            mPort = port;
        } else {
            mServerAddress.clear();
            mPort = 0;
        }
    }

Q_SIGNALS:
    void updateServerStatus (int ugenCount, int synthCount, int groupCount, int defCount, float avgCPU, float peakCPU);

private:
    QUdpSocket * mUdpSocket;
    QHostAddress mServerAddress;
    int mPort;
};

}

#endif // SCIDE_SC_SERVER_HPP_INCLUDED
