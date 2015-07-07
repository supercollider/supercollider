/*
    SuperCollider Qt IDE
    Copyright (c) 2012 - 2013 Jakob Leben & Tim Blechmann
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

#include "audio_status_box.hpp"
#include "../core/sc_server.hpp"

#include <QHBoxLayout>
#include <QWheelEvent>

namespace ScIDE {

AudioStatusBox::AudioStatusBox(ScServer *server, QWidget *parent):
    StatusBox(parent),
    mServer(server),
    m_avg_cpu(0.f),
    m_peak_cpu(0.f),
    m_ugens(0),
    m_synths(0),
    m_groups(0),
    m_synthdefs(0)
{
    mStatisticsLabel = new StatusLabel;
    mVolumeLabel = new StatusLabel;
    mMuteLabel = new StatusLabel;
    mMuteLabel->setText("M");
    mRecordLabel = new StatusLabel;
    mRecordLabel->setText("R");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(mStatisticsLabel);
    layout->addWidget(mVolumeLabel);
    layout->addWidget(mMuteLabel);
    layout->addWidget(mRecordLabel);

    setLayout(layout);

    server->action(ScServer::Record)->setProperty("keep_menu_open", true);
    server->action(ScServer::VolumeRestore)->setProperty("keep_menu_open", true);
    server->action(ScServer::Mute)->setProperty("keep_menu_open", true);
    server->action(ScServer::DumpOSC)->setProperty("keep_menu_open", true);

    QAction *separator;
    addAction( server->action(ScServer::ToggleRunning) );
    addAction( server->action(ScServer::Reboot) );
    addAction( server->action(ScServer::KillAll) );
    addActionSeparator();
    addAction( server->action(ScServer::ShowMeters) );
    addAction( server->action(ScServer::ShowScope) );
    addAction( server->action(ScServer::ShowFreqScope) );
    addAction( server->action(ScServer::DumpNodeTree) );
    addAction( server->action(ScServer::DumpNodeTreeWithControls) );
    addAction( server->action(ScServer::PlotTree) );
    addAction( server->action(ScServer::DumpOSC) );
    addActionSeparator();
    addAction( server->action(ScServer::Record) );
    addActionSeparator();
    addAction( server->action(ScServer::VolumeRestore) );
    addAction( server->action(ScServer::Mute) );
    addAction( server->action(ScServer::Volume) );

    connect(server, SIGNAL(runningStateChange(bool,QString,int)),
            this, SLOT(onServerRunningChanged(bool,QString,int)));
    connect(server, SIGNAL(updateServerStatus(int,int,int,int,float,float)),
            this, SLOT(onServerStatusReply(int,int,int,int,float,float)));
    connect(server, SIGNAL(volumeChanged(float)), this, SLOT(updateVolumeLabel(float)));
    connect(server, SIGNAL(mutedChanged(bool)), this, SLOT(updateMuteLabel(bool)));
    connect(server, SIGNAL(recordingChanged(bool)), this, SLOT(updateRecordLabel(bool)));

    onServerRunningChanged(false, "", 0);
    updateVolumeLabel( mServer->volume() );
    updateMuteLabel( mServer->isMuted() );
    updateRecordLabel( mServer->isRecording() );
}

void AudioStatusBox::onServerStatusReply(int ugens, int synths, int groups, int synthDefs,
                                         float avgCPU, float peakCPU)
{
    m_avg_cpu = avgCPU;
    m_peak_cpu = peakCPU;
    m_ugens = ugens;
    m_synths = synths;
    m_groups = groups;
    m_synthdefs = synthDefs;

    updateStatistics();
}

void AudioStatusBox::onServerRunningChanged(bool running, const QString &, int)
{
    mStatisticsLabel->setTextColor( running ? Qt::green : Qt::white);
    mVolumeLabel->setTextColor( running ? Qt::green : Qt::white );

    if (!running)
        onServerStatusReply(0, 0, 0, 0, 0, 0);
}

void AudioStatusBox::wheelEvent(QWheelEvent * event)
{
    if (event->orientation() == Qt::Vertical) {
        if (event->delta() > 0)
            mServer->changeVolume(0.2);
        else
            mServer->changeVolume(-0.2);
    }
    StatusBox::wheelEvent(event);
}

void AudioStatusBox::updateStatistics()
{
    QString statusString =
            QStringLiteral("%1% %2% %3u %4s %5g %6d ")
            .arg(m_avg_cpu,  5, 'f', 2)
            .arg(m_peak_cpu, 5, 'f', 2)
            .arg(m_ugens,     4)
            .arg(m_synths,    4)
            .arg(m_groups,    4)
            .arg(m_synthdefs, 4);

    mStatisticsLabel->setText(statusString);
}

void AudioStatusBox::updateVolumeLabel( float volume )
{
    mVolumeLabel->setText( QStringLiteral("%1dB ").arg( volume, 5, 'f', 1) );
}

void AudioStatusBox::updateMuteLabel( bool muted )
{
    mMuteLabel->setTextColor( muted ? Qt::red : QColor(30,30,30) );
}

void AudioStatusBox::updateRecordLabel( bool recording )
{
    mRecordLabel->setTextColor( recording ? Qt::red : QColor(30,30,30) );
}

} // namespace ScIDE
