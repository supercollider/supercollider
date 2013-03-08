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

#ifndef SCIDE_WIDGETS_UTIL_VOLUME_WIDGET_INCLUDED
#define SCIDE_WIDGETS_UTIL_VOLUME_WIDGET_INCLUDED

#include <QWidget>
class QLabel;
class QSlider;

namespace ScIDE {

class VolumeWidget : public QWidget
{
    Q_OBJECT
public:
    VolumeWidget( QWidget * parent = 0 );
    float volume() const { return mVolume; }
    float setVolume( float volume );
    void setRange( float min, float max );
signals:
    void volumeChanged( float volumet );
private slots:
    void onVolumeSliderAction();
private:
    void updateVolumeLabel( float volume );
    int volumeToSlider( float volume );
    float volumeFromSlider( int slider );
    bool eventFilter( QObject *, QEvent * );

    QLabel *mVolumeLabel;
    QSlider *mVolumeSlider;
    float mVolume;
    float mVolumeMin;
    float mVolumeMax;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_VOLUME_WIDGET_INCLUDED
