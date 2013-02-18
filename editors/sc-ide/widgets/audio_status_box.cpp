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

namespace ScIDE {

AudioStatusBox::AudioStatusBox(ScServer *server, QWidget *parent):
    StatusBox(parent)
{
    QAction *separator;
    addAction( server->action(ScServer::ToggleRunning) );
    addAction( server->action(ScServer::Reboot) );
    addActionSeparator();
    addAction( server->action(ScServer::ShowMeters) );
    addAction( server->action(ScServer::DumpNodeTree) );
    addAction( server->action(ScServer::DumpNodeTreeWithControls) );
    addActionSeparator();
    addAction( server->action(ScServer::VolumeRestore) );
    addAction( server->action(ScServer::Mute) );
    addAction( server->action(ScServer::Volume) );

    connect(server, SIGNAL(runningStateChange(bool,QString,int)),
            this, SLOT(onServerRunningChanged(bool,QString,int)));
    connect(server, SIGNAL(updateServerStatus(int,int,int,int,float,float)),
            this, SLOT(onServerStatusReply(int,int,int,int,float,float)));

    onServerRunningChanged(false, "", 0);
}

void AudioStatusBox::onServerStatusReply(int ugens, int synths, int groups, int synthDefs,
                                         float avgCPU, float peakCPU)
{
    QString statusString =
            QString("%1% %2% %3u %4s %5g %6d")
            .arg(avgCPU,  5, 'f', 2)
            .arg(peakCPU, 5, 'f', 2)
            .arg(ugens,     4)
            .arg(synths,    4)
            .arg(groups,    4)
            .arg(synthDefs, 4);

    setText(statusString);
}

void AudioStatusBox::onServerRunningChanged(bool running, const QString &, int)
{
    setTextColor( running ? Qt::green : Qt::white);
    if (!running) {
        onServerStatusReply(0, 0, 0, 0, 0, 0);
    }
}

} // namespace ScIDE
