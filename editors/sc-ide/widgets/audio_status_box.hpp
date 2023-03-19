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

#pragma once

#include "util/status_box.hpp"

namespace ScIDE {

class ScServer;

class AudioStatusBox : public StatusBox {
    Q_OBJECT
public:
    AudioStatusBox(ScServer*, QWidget* parent = 0);

private slots:
    void onServerRunningChanged(bool running, QString const& hostName, int port, bool unresponsive);

    void updateStatistics(int ugens, int synths, int groups, int synthDefs, float avgCPU, float peakCPU);

    void updateVolumeLabel(float volume);
    void updateMuteLabel(bool muted);
    void updateRecordLabel(bool recording);

signals:
    void increaseVolume();
    void decreaseVolume();

private:
    void wheelEvent(QWheelEvent*);

    StatusLabel* mStatisticsLabel;
    StatusLabel* mVolumeLabel;
    StatusLabel* mMuteLabel;
    StatusLabel* mRecordLabel;
};

} // namespace ScIDE
