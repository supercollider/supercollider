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

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

namespace ScIDE {

ScServer::ScServer(ScProcess *scLang, Settings::Manager *settings, QObject *parent):
    QObject(parent), mPort(0)
{
    createActions(settings);

    mUdpSocket = new QUdpSocket(this);
    for (int port = 57140; port != 57150; ++port) {
        bool success = mUdpSocket->bind(port);
        if (success)
            break;
    }
    startTimer(333);

    connect(scLang, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(onScLangStateChanged(QProcess::ProcessState)));
    connect(scLang, SIGNAL(response(QString,QString)),
            this, SLOT(onScLangReponse(QString,QString)));

    onRunningStateChanged(false, QString(), 0); // initialize ToggleRunning action
}

void ScServer::createActions(Settings::Manager * settings)
{
    const QString synthServerCategory(tr("Sound Synthesis Server"));
    QAction *action;

    mActions[ToggleRunning] = action = new QAction(tr("Boot or quit server"), this);
    action->setShortcut(tr("Ctrl+B", "Boot or quit default server"));
    connect(action, SIGNAL(triggered()), this, SLOT(toggleRunning()));
    settings->addAction( action, "synth-server-toggle-running", synthServerCategory);

    mActions[Reboot] = action = new QAction(tr("Reboot server"), this);
    action->setShortcut(tr("Ctrl+Shift+B", "Reboot default server"));
    connect(action, SIGNAL(triggered()), this, SLOT(reboot()));
    settings->addAction( action, "synth-server-reboot", synthServerCategory);

    mActions[ShowMeters] = action = new QAction(tr("Show server meter"), this);
    action->setShortcut(tr("Ctrl+M", "Show server meter"));
    connect(action, SIGNAL(triggered()), this, SLOT(showMeters()));
    settings->addAction( action, "synth-server-meter", synthServerCategory);

    mActions[DumpNodeTree] = action = new QAction(tr("Dump node tree"), this);
    action->setShortcut(tr("Ctrl+T", "Dump node tree"));
    connect(action, SIGNAL(triggered()), this, SLOT(dumpNodeTree()));
    settings->addAction( action, "synth-server-dump-nodes", synthServerCategory);

    mActions[DumpNodeTreeWithControls] = action = new QAction(tr("Dump node tree with controls"), this);
    action->setShortcut(tr("Ctrl+Shift+T", "Dump node tree with controls"));
    connect(action, SIGNAL(triggered()), this, SLOT(dumpNodeTreeWithControls()));
    settings->addAction( action, "synth-server-dump-nodes-with-controls", synthServerCategory);
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

void ScServer::toggleRunning()
{
    if (isRunning())
        quit();
    else
        boot();
}

void ScServer::showMeters()
{
    Main::evaluateCode("ScIDE.defaultServer.meter", true);
}

void ScServer::dumpNodeTree()
{
    queryAllNodes(false);
}

void ScServer::dumpNodeTreeWithControls()
{
    queryAllNodes(true);
}

void ScServer::queryAllNodes(bool dumpControls)
{
    QString arg = dumpControls ? QString("true") : QString("false");

    Main::scProcess()->evaluateCode( QString("ScIDE.defaultServer.queryAllNodes(%1)").arg(arg), true );
}

void ScServer::onScLangStateChanged( QProcess::ProcessState state )
{
    bool langIsRunning = state == QProcess::Running;
    mActions[ToggleRunning]->setEnabled(langIsRunning);
    mActions[Reboot]->setEnabled(langIsRunning);
    mActions[ShowMeters]->setEnabled(langIsRunning);
    mActions[DumpNodeTree]->setEnabled(langIsRunning);
    mActions[DumpNodeTreeWithControls]->setEnabled(langIsRunning);
}

void ScServer::onScLangReponse( const QString & selector, const QString & data )
{
    static QString defaultServerRunningChangedSelector("defaultServerRunningChanged");

    if (selector != defaultServerRunningChangedSelector)
        return;

    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    bool serverRunningState;
    std::string hostName;
    int port;

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        assert(doc.Type() == YAML::NodeType::Sequence);

        bool success = doc[0].Read(serverRunningState);
        if (!success) return; // LATER: report error?

        success = doc[1].Read(hostName);
        if (!success) return; // LATER: report error?

        success = doc[2].Read(port);
        if (!success) return; // LATER: report error?
    }

    QString qstrHostName( hostName.c_str() );

    onRunningStateChanged( serverRunningState, qstrHostName, port );

    emit runningStateChange( serverRunningState, qstrHostName, port );
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

void ScServer::onRunningStateChanged( bool running, QString const & hostName, int port )
{
    if (running) {
        mServerAddress = QHostAddress(hostName);
        mPort = port;

        mActions[ToggleRunning]->setText( tr("&Quit Server") );
        mActions[ToggleRunning]->setStatusTip(tr("Quit sound synthesis server"));
    } else {
        mServerAddress.clear();
        mPort = 0;

        mActions[ToggleRunning]->setText( tr("&Boot Server") );
        mActions[ToggleRunning]->setStatusTip(tr("Boot sound synthesis server"));
    }
}

}
