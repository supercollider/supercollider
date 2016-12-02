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
#include <QWidgetAction>

#include "sc_server.hpp"
#include "sc_process.hpp"
#include "main.hpp"
#include "../widgets/util/volume_widget.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <sstream>
#include <iomanip>
#include <boost/chrono/chrono_io.hpp>
#include <osc/OscReceivedElements.h>
#include <osc/OscOutboundPacketStream.h>

using namespace std;
using namespace boost::chrono;

namespace ScIDE {

ScServer::ScServer(ScProcess *scLang, Settings::Manager *settings, QObject *parent):
    QObject(parent),
    mLang(scLang),
    mPort(0),
    mIsRecording(false)
{
    createActions(settings);

    mUdpSocket = new QUdpSocket(this);
    startTimer(333);

    connect(scLang, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(onScLangStateChanged(QProcess::ProcessState)));
    connect(scLang, SIGNAL(response(QString,QString)),
            this, SLOT(onScLangReponse(QString,QString)));
    connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(onServerDataArrived()));
}

void ScServer::createActions(Settings::Manager * settings)
{
    const QString synthServerCategory(tr("Sound Synthesis Server"));
    QAction *action;
    QWidgetAction *widgetAction;

    mActions[ToggleRunning] = action = new QAction(tr("Boot or quit default server"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(toggleRunning()));
    //settings->addAction( action, "synth-server-toggle-running", synthServerCategory);

    mActions[Boot] = action =
        new QAction(QIcon::fromTheme("system-run"), tr("&Boot Server"), this);
    action->setShortcut(tr("Ctrl+B", "Boot default server"));
    connect(action, SIGNAL(triggered()), this, SLOT(boot()));
    settings->addAction( action, "synth-server-boot", synthServerCategory);

    mActions[Quit] = action =
        new QAction(QIcon::fromTheme("system-shutdown"), tr("&Quit Server"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(quit()));
    settings->addAction( action, "synth-server-quit", synthServerCategory);

    mActions[KillAll] = action =
    new QAction(QIcon::fromTheme("system-killall"), tr("&Kill All Servers"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(killAll()));
    settings->addAction( action, "synth-server-killall", synthServerCategory);

    mActions[Reboot] = action =
        new QAction( QIcon::fromTheme("system-reboot"), tr("&Reboot Server"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(reboot()));
    settings->addAction( action, "synth-server-reboot", synthServerCategory);

    mActions[ShowMeters] = action = new QAction(tr("Show Server Meter"), this);
    action->setShortcut(tr("Ctrl+M", "Show server meter"));
    connect(action, SIGNAL(triggered()), this, SLOT(showMeters()));
    settings->addAction( action, "synth-server-meter", synthServerCategory);

    mActions[ShowScope] = action = new QAction(tr("Show Scope"), this);
    action->setShortcut(tr("Ctrl+Shift+M", "Show scope"));
    connect(action, SIGNAL(triggered()), this, SLOT(showScope()));
    settings->addAction( action, "synth-server-scope", synthServerCategory);

    mActions[ShowFreqScope] = action = new QAction(tr("Show Freqscope"), this);
    action->setShortcut(tr("Ctrl+Alt+M", "Show freqscope"));
    connect(action, SIGNAL(triggered()), this, SLOT(showFreqScope()));
    settings->addAction( action, "synth-server-freqscope", synthServerCategory);

    mActions[DumpNodeTree] = action = new QAction(tr("Dump Node Tree"), this);
    action->setShortcut(tr("Ctrl+T", "Dump node tree"));
    connect(action, SIGNAL(triggered()), this, SLOT(dumpNodeTree()));
    settings->addAction( action, "synth-server-dump-nodes", synthServerCategory);

    mActions[DumpNodeTreeWithControls] = action = new QAction(tr("Dump Node Tree with Controls"), this);
    action->setShortcut(tr("Ctrl+Shift+T", "Dump node tree with controls"));
    connect(action, SIGNAL(triggered()), this, SLOT(dumpNodeTreeWithControls()));
    settings->addAction( action, "synth-server-dump-nodes-with-controls", synthServerCategory);

    mActions[PlotTree] = action = new QAction(tr("Show Node Tree"), this);
    action->setShortcut(tr("Ctrl+Alt+T", "Show node tree"));
    connect(action, SIGNAL(triggered()), this, SLOT(plotTree()));
    settings->addAction( action, "synth-server-plot-tree", synthServerCategory);

    mActions[DumpOSC] = action = new QAction(tr("Server Dump OSC"), this);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sendDumpingOSC(bool)));
    settings->addAction( action, "synth-server-dumpOSC", synthServerCategory);

    mActions[Mute] = action = new QAction(tr("Mute"), this);
    action->setShortcut(tr("Ctrl+Alt+End", "Mute sound output."));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(sendMuted(bool)));
    connect(action, SIGNAL(toggled(bool)), this, SIGNAL(mutedChanged(bool)));
    settings->addAction( action, "synth-server-mute", synthServerCategory);

    mVolumeWidget = new VolumeWidget;

    mActions[Volume] = widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget( mVolumeWidget );

    connect( mVolumeWidget, &VolumeWidget::volumeChangeRequested, [this](float newValue) {
        setVolume( newValue );
    });
    connect( this, SIGNAL(volumeChanged(float)),            mVolumeWidget, SLOT(setVolume(float))            );
    connect( this, SIGNAL(volumeRangeChanged(float,float)), mVolumeWidget, SLOT(setVolumeRange(float,float)) );
    emit volumeChanged( mVolume );
    emit volumeRangeChanged( mVolumeMin, mVolumeMax );

    mActions[VolumeUp] = action = new QAction(tr("Increase Volume"), this);
    action->setShortcut(tr("Ctrl+Alt+PgUp", "Increase volume"));
    connect(action, SIGNAL(triggered()), this, SLOT(increaseVolume()));
    settings->addAction( action, "synth-server-volume-up", synthServerCategory);

    mActions[VolumeDown] = action = new QAction(tr("Decrease Volume"), this);
    action->setShortcut(tr("Ctrl+Alt+PgDown", "Decrease volume"));
    connect(action, SIGNAL(triggered()), this, SLOT(decreaseVolume()));
    settings->addAction( action, "synth-server-volume-down", synthServerCategory);

    mActions[VolumeRestore] = action = new QAction(tr("Restore Volume to 0 dB"), this);
    action->setShortcut(tr("Ctrl+Alt+Home", "Restore volume"));
    connect(action, SIGNAL(triggered()), this, SLOT(restoreVolume()));
    settings->addAction( action, "synth-server-volume-restore", synthServerCategory);

    mActions[Record] = action = new QAction(tr("Recording"), this);
    action->setCheckable(true);
    connect( action, SIGNAL(triggered(bool)), this, SLOT(sendRecording(bool)) );
    connect( action, SIGNAL(toggled(bool)), this, SIGNAL(recordingChanged(bool)) );
    settings->addAction( action, "synth-server-record", synthServerCategory);

    mActions[PauseRecord] = action = new QAction(tr("Pause Recording"), this);
    action->setCheckable(true);
    connect( action, SIGNAL(triggered(bool)), this, SLOT(pauseRecording(bool)) );
    connect( action, SIGNAL(toggled(bool)), this, SLOT(pauseChanged(bool)) );
    settings->addAction( action, "synth-server-pause-recording", synthServerCategory);


    connect( mActions[Boot], SIGNAL(changed()), this, SLOT(updateToggleRunningAction()) );
    connect( mActions[Quit], SIGNAL(changed()), this, SLOT(updateToggleRunningAction()) );

    updateToggleRunningAction();
    updateRecordingAction();
    updateEnabledActions();
}

void ScServer::updateToggleRunningAction()
{
    QAction *targetAction = isRunning() ? mActions[Quit] : mActions[Boot];
    mActions[ToggleRunning]->setText( targetAction->text() );
    mActions[ToggleRunning]->setIcon( targetAction->icon() );
    mActions[ToggleRunning]->setShortcut( targetAction->shortcut() );
}

void ScServer::boot()
{
    if (isRunning())
        return;

    mLang->evaluateCode( "ScIDE.defaultServer.boot", true );
}

void ScServer::quit()
{
    if (!isRunning())
        return;

    mLang->evaluateCode( "ScIDE.defaultServer.quit", true );
}

void ScServer::killAll()
{
    mLang->evaluateCode( "Server.killAll", true );
}

void ScServer::reboot()
{
    mLang->evaluateCode( "ScIDE.defaultServer.reboot", true );
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
   mLang->evaluateCode("ScIDE.defaultServer.meter", true);
}

void ScServer::showScope()
{
   mLang->evaluateCode("ScIDE.defaultServer.scope(ScIDE.defaultServer.options.numOutputBusChannels)", true);
}

void ScServer::showFreqScope()
{
   mLang->evaluateCode("ScIDE.defaultServer.freqscope", true);
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
    QString arg = dumpControls ? QStringLiteral("true") : QStringLiteral("false");

    mLang->evaluateCode( QStringLiteral("ScIDE.defaultServer.queryAllNodes(%1)").arg(arg), true );
}

void ScServer::plotTree()
{
    mLang->evaluateCode("ScIDE.defaultServer.plotTree", true );
}

bool ScServer::isMuted() const { return mActions[Mute]->isChecked(); }

void ScServer::setMuted( bool muted )
{
    mActions[Mute]->setChecked(muted);
    sendMuted(muted);
}

bool ScServer::isDumpingOSC() const { return mActions[DumpOSC]->isChecked(); }

void ScServer::setDumpingOSC( bool dumping )
{
    mActions[DumpOSC]->setChecked(dumping);
    sendDumpingOSC(dumping);
}

float ScServer::volume() const { return mVolume; }

void ScServer::setVolume( float volume )
{
    volume = qBound( mVolumeMin, volume, mVolumeMax );

    if( volume != mVolume ) {
        mVolume = volume;
        sendVolume( volume );
        emit volumeChanged( volume );
    }
}


void ScServer::setVolumeRange(float min, float max)
{
    mVolumeMin = min;
    mVolumeMax = max;
    emit volumeRangeChanged( min, max );
}

void ScServer::increaseVolume()
{
    changeVolume( +1.5 );
}

void ScServer::decreaseVolume()
{
    changeVolume( -1.5 );
}

void ScServer::changeVolume(float difference)
{
    setVolume( volume() + difference);
}

void ScServer::restoreVolume()
{
    setVolume( 0.0f );
    unmute();
}

void ScServer::sendMuted(bool muted)
{
    static const QString muteCommand("ScIDE.defaultServer.mute");
    static const QString unmuteCommand("ScIDE.defaultServer.unmute");

    mLang->evaluateCode( muted ? muteCommand : unmuteCommand, true );
}

void ScServer::sendDumpingOSC(bool dumping)
{
    static const QString dumpCommand("ScIDE.defaultServer.dumpOSC(true)");
    static const QString stopDumpCommand("ScIDE.defaultServer.dumpOSC(false)");

    mLang->evaluateCode( dumping ? dumpCommand : stopDumpCommand, true );
}

void ScServer::sendVolume( float volume )
{
    mLang->evaluateCode( QStringLiteral("ScIDE.setServerVolume(%1)").arg(volume), true );
}

bool ScServer::isRecording() const { return mIsRecording; }
bool ScServer::isPaused() const { return mIsRecordingPaused; }



void ScServer::setRecording( bool doRecord )
    {
        if (!isRunning())
            return;

        mIsRecording = doRecord;
        mIsRecordingPaused = false;
        updateRecordingAction();
}

void ScServer::pauseRecording( bool flag )
    {
        if(mIsRecordingPaused != flag) {
            mIsRecordingPaused = flag;
            if(flag) {
                mLang->evaluateCode( QStringLiteral("ScIDE.defaultServer.pauseRecording"), true );
            } else {
                if(mIsRecording) {
                setRecording(true);
                sendRecording(true);
               }
            }
            updateRecordingAction();
        }
}


void ScServer::sendRecording( bool doRecord )
{
    static const QString startRecordingCommand("ScIDE.defaultServer.record");
    static const QString stopRecordingCommand("ScIDE.defaultServer.stopRecording");
    setRecording(doRecord);
    mLang->evaluateCode(doRecord ? startRecordingCommand : stopRecordingCommand);
}


void ScServer::updateRecordingAction()
{
    if (isRecording()) {
        int s = mRecordingSeconds % 60;
        int m = mRecordingSeconds / 60 % 60;
        int h = mRecordingSeconds / 3600;
        ostringstream msg;
        msg << "Recording: ";
        msg << setw(2) << setfill('0') << h << ':';
        msg << setw(2) << setfill('0') << m << ':';
        msg << setw(2) << setfill('0') << s;
        mActions[Record]->setText( msg.str().c_str() );
    }
    else {
        mRecordingSeconds = 0;
        mActions[Record]->setText( "Start Recording" );
        mIsRecordingPaused = false;
    }
    mActions[Record]->setChecked( isRecording() );
    mActions[PauseRecord]->setChecked( mIsRecordingPaused );
}

void ScServer::onScLangStateChanged( QProcess::ProcessState )
{
    updateEnabledActions();
}

void ScServer::onScLangReponse( const QString & selector, const QString & data )
{
    static QString defaultServerRunningChangedSelector("defaultServerRunningChanged");
    static QString mutedSelector("serverMuted");
    static QString unmutedSelector("serverUnmuted");
    static QString ampSelector("serverAmp");
    static QString ampRangeSelector("serverAmpRange");
    static QString startDumpOSCSelector("dumpOSCStarted");
    static QString stopDumpOSCSelector("dumpOSCStopped");
    static QString startRecordingSelector("recordingStarted");
    static QString pauseRecordingSelector("recordingPaused");
    static QString stopRecordingSelector("recordingStopped");
    static QString recordingDurationSelector("recordingDuration");

    if (selector == defaultServerRunningChangedSelector)
        handleRuningStateChangedMsg(data);
    else if(selector == mutedSelector) {
        mActions[Mute]->setChecked(true);
    } else if (selector == unmutedSelector) {
        mActions[Mute]->setChecked(false);
    }
    else if (selector == startDumpOSCSelector) {
        mActions[DumpOSC]->setChecked(true);
    }
    else if (selector == stopDumpOSCSelector) {
        mActions[DumpOSC]->setChecked(false);
    }
	else if (selector == recordingDurationSelector) {
        bool ok;
        float duration = data.mid(1, data.size() - 2).toFloat(&ok);
        if (ok) {
            mRecordingSeconds = (int) duration;
            updateRecordingAction();
        }
    }
    else if (selector == startRecordingSelector) {
        setRecording(true);
    }
    else if (selector == startRecordingSelector) {
        setRecording(true);
    }
    else if (selector == pauseRecordingSelector) {
        pauseRecording(true);
    }
    else if (selector == stopRecordingSelector) {
        setRecording(false);
    }
    else if (selector == ampSelector) {
        bool ok;
        float volume = data.mid(1, data.size() - 2).toFloat(&ok);
        if (ok) {
            mVolume = volume;
            emit volumeChanged( volume );
        }
    }
    else if (selector == ampRangeSelector) {
        bool ok;
        QStringList dataList = data.mid(1, data.size() - 2).split(',');
        if (dataList.size() < 2)
            return;
        float min = dataList[0].toFloat(&ok);
        if (!ok) return;
        float max = dataList[1].toFloat(&ok);
        if (!ok) return;
        mVolumeMin = min;
        mVolumeMax = max;
        setVolumeRange( min, max );
    }
}

void ScServer::handleRuningStateChangedMsg( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    bool serverRunningState, serverUnresponsive;

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

        success = doc[3].Read(serverUnresponsive);
        if (!success) return; // LATER: report error?
    }

    QString qstrHostName( hostName.c_str() );

    onRunningStateChanged( serverRunningState, qstrHostName, port);

    emit runningStateChanged( serverRunningState, qstrHostName, port, serverUnresponsive );
}

void ScServer::timerEvent(QTimerEvent * event)
{
    if (mPort)
    {
        char buffer[512];
        osc::OutboundPacketStream stream(buffer, 512);
        stream << osc::BeginMessage("/status");
        stream << osc::MessageTerminator();

        qint64 sentSize = mUdpSocket->write(stream.Data(), stream.Size());
        if (sentSize == -1)
            qCritical() << "Failed to send server status request:" << mUdpSocket->errorString();
    }
}

void ScServer::onRunningStateChanged( bool running, QString const & hostName, int port )
{
    if (running) {
        mServerAddress = QHostAddress(hostName);
        mPort = port;
        mUdpSocket->connectToHost(mServerAddress, mPort);
    } else {
        mServerAddress.clear();
        mPort = 0;
        mIsRecording = false;
        mUdpSocket->disconnectFromHost();
    }

    updateToggleRunningAction();
    updateRecordingAction();
    updateEnabledActions();
}

void ScServer::onServerDataArrived()
{
    while (mUdpSocket->hasPendingDatagrams())
    {
        size_t datagramSize = mUdpSocket->pendingDatagramSize();
        QByteArray array(datagramSize, 0);
        qint64 readSize = mUdpSocket->readDatagram(array.data(), datagramSize);
        if (readSize == -1)
            continue;

        processOscPacket( osc::ReceivedPacket(array.data(), datagramSize) );
    }
}

void ScServer::processOscMessage( const osc::ReceivedMessage & message )
{
    if (strcmp(message.AddressPattern(), "/status.reply") == 0)
    {
        processServerStatusMessage(message);
    }
}

void ScServer::processServerStatusMessage(const osc::ReceivedMessage &message )
{
    if (!isRunning())
        return;

    osc::int32 unused;
    osc::int32 ugenCount;
    osc::int32 synthCount;
    osc::int32 groupCount;
    osc::int32 defCount;
    float avgCPU;
    float peakCPU;

    auto args = message.ArgumentStream();

    try
    {
        args >> unused
             >> ugenCount
             >> synthCount
             >> groupCount
             >> defCount
             >> avgCPU
             >> peakCPU;
    }
    catch (osc::MissingArgumentException)
    {
        qCritical("Misformatted server status message.");
        return;
    }

    emit updateServerStatus(ugenCount, synthCount,
                            groupCount, defCount,
                            avgCPU, peakCPU);
}

void ScServer::updateEnabledActions()
{
    bool langRunning = mLang->state() == QProcess::Running;
    bool langAndServerRunning = langRunning && isRunning();

    mActions[ToggleRunning]->setEnabled(langRunning);
    mActions[KillAll]->setEnabled(langRunning);
    mActions[Reboot]->setEnabled(langRunning);
    mActions[ShowMeters]->setEnabled(langRunning);
    mActions[ShowScope]->setEnabled(langRunning);
    mActions[ShowFreqScope]->setEnabled(langRunning);
    mActions[DumpNodeTree]->setEnabled(langAndServerRunning);
    mActions[DumpNodeTreeWithControls]->setEnabled(langAndServerRunning);
    mActions[PlotTree]->setEnabled(langAndServerRunning);
    mActions[Mute]->setEnabled(langAndServerRunning);
    mActions[VolumeUp]->setEnabled(langAndServerRunning);
    mActions[VolumeDown]->setEnabled(langAndServerRunning);
    mActions[Volume]->setEnabled(langAndServerRunning);
    mActions[VolumeRestore]->setEnabled(langAndServerRunning);
    mActions[Record]->setEnabled(langAndServerRunning);
    mActions[PauseRecord]->setEnabled(langAndServerRunning);
    mActions[DumpOSC]->setEnabled(langAndServerRunning);
}

}
