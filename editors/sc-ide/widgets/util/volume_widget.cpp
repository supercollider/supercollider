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

#include "volume_widget.hpp"

#include "SC_BoundsMacros.h"
#include "SC_InlineUnaryOp.h"

#include <QtGlobal>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QKeyEvent>

namespace ScIDE {

VolumeWidget::VolumeWidget( QWidget *parent ):
    QWidget(parent)
{
    mVolumeLabel = new QLabel;

    mVolumeSlider = new QSlider;
    mVolumeSlider->setOrientation( Qt::Horizontal );
    mVolumeSlider->setMinimum(0);
    mVolumeSlider->setMaximum(1000);
    mVolumeSlider->setMinimumWidth(120);
    mVolumeSlider->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget( new QLabel("Volume:") );
    layout->addWidget( mVolumeLabel );
    layout->addWidget( mVolumeSlider );

    setLayout( layout );

    connect( mVolumeSlider, &QSlider::actionTriggered, [=] {
        onVolumeSliderAction();
    });
}

void VolumeWidget::setVolume( float volume )
{
    mVolumeSlider->setValue( volumeToSlider( volume ) );
    mVolumeLabel->setText( QString::number(volume, 'f', 1) + " dB" );
}

void VolumeWidget::setVolumeRange(float min, float max)
{
    mVolumeMin = min;
    mVolumeMax = max;
}

void VolumeWidget::onVolumeSliderAction()
{
    emit volumeChangeRequested( volumeFromSlider( mVolumeSlider->sliderPosition() ) );
}

int VolumeWidget::volumeToSlider( float volume )
{
    float ampMax = sc_dbamp(mVolumeMax);
    float ampMin = sc_dbamp(mVolumeMin);
    float ampRange = ampMax - ampMin;
    float slider = (ampRange >= 0) ? std::sqrt( (sc_dbamp(volume) - ampMin) / ampRange )
                                   : 1 - std::sqrt( 1 - ((sc_dbamp(volume) - ampMin) / ampRange) );
    return slider * 1000.f;
}

float VolumeWidget::volumeFromSlider( int sliderValue )
{
    float slider = sliderValue / 1000.f;
    float ampMax = sc_dbamp(mVolumeMax);
    float ampMin = sc_dbamp(mVolumeMin);
    float ampRange = ampMax - ampMin;
    if (ampRange >= 0)
        return sc_ampdb( slider * slider * ampRange + ampMin );
    else {
        return sc_ampdb( (1.f - (1.f-slider)*(1.f-slider)) * ampRange + ampMin );
    }
}

bool VolumeWidget::eventFilter( QObject *object, QEvent *event )
{
    if (object != mVolumeSlider)
        return false;

    switch (event->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch(keyEvent->key())
        {
        case Qt::Key_Home:
            emit volumeChangeRequested( 0.f );
            event->accept();
            return true;
        case Qt::Key_End:
            emit volumeChangeRequested( -9999 );
            event->accept();
            return true;
        default:
            break;
        }
    }
    default:
        break;
    }

    return QWidget::eventFilter(object, event);
}

} // namespace ScIDE
