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

#include <QDebug>

#include "sc_server.hpp"
#include "sc_process.hpp"
#include "main.hpp"

#include "scsynthsend.h"
#include "sc_msg_iter.h"

namespace ScIDE {

ScServer::ScServer(QObject * parent):
    QObject(parent), mPort(0)
{
    mUdpSocket = new QUdpSocket(this);
    for (int port = 57140; port != 57150; ++port) {
        bool success = mUdpSocket->bind(port);
        if (success)
            break;
    }
    startTimer(333);
}

void ScServer::boot()
{
    if (isRunning())
        return;

    Main::scProcess()->evaluateCode( "ScIDE.defaultServer.boot", true );
}

void ScServer::quit()
{
    if (!isRunning())
        return;

    Main::scProcess()->evaluateCode( "ScIDE.defaultServer.quit", true );
}

void ScServer::reboot()
{
    Main::scProcess()->evaluateCode( "ScIDE.defaultServer.reboot", true );
}

void ScServer::queryAllNodes(bool dumpControls)
{
    QString arg = dumpControls ? QString("true") : QString("false");

    Main::scProcess()->evaluateCode( QString("ScIDE.defaultServer.queryAllNodes(%1)").arg(arg), true );
}

void ScServer::timerEvent(QTimerEvent * event)
{
    if (mUdpSocket->hasPendingDatagrams()) {
        size_t datagramSize = mUdpSocket->pendingDatagramSize();
        QByteArray array(datagramSize, 0);
        mUdpSocket->readDatagram(array.data(), datagramSize);

        if (!mPort)
            return;

        if (array[0]) {
            char *addr = array.data();
            const char * data = OSCstrskip(array.data());
            int size = datagramSize - (data - addr);

            if (strcmp(addr, "/status.reply") == 0) {
                sc_msg_iter reply(size, data);
                int	unused     = reply.geti();
                int	ugenCount  = reply.geti();
                int	synthCount = reply.geti();
                int	groupCount = reply.geti();
                int	defCount   = reply.geti();
                float avgCPU   = reply.getf();
                float peakCPU  = reply.getf();
                double srNom   = reply.getd();
                double srAct   = reply.getd();

                emit updateServerStatus(ugenCount, synthCount, groupCount, defCount, avgCPU, peakCPU);
            }
        }
    }

    if (mPort) {
        small_scpacket packet;
        packet.BeginMsg();
        packet.adds_slpre("status");
        packet.maketags(1);
        packet.addtag(',');
        packet.EndMsg();

        mUdpSocket->writeDatagram(packet.data(), packet.size(), mServerAddress, mPort);
    }
}

}
