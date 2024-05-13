/*
    SuperCollider Qt IDE
    Copyright (c) 2013 Jakob Leben & Tim Blechmann
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

#include <QWidget>

class QLabel;
class QSlider;

namespace ScIDE {

class VolumeWidget : public QWidget {
    Q_OBJECT
public:
    explicit VolumeWidget(QWidget* parent = 0);

public slots:
    void setVolumeRange(float min, float max);
    void setVolume(float volume);

signals:
    void volumeChangeRequested(float volume);

private slots:
    void onVolumeSliderAction();

private:
    int volumeToSlider(float volume);
    float volumeFromSlider(int slider);
    bool eventFilter(QObject*, QEvent*);

    QLabel* mVolumeLabel;
    QSlider* mVolumeSlider;

    float mVolumeMin = -10000, mVolumeMax = 10000;
};

} // namespace ScIDE
