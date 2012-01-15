/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#include "QcSlider.h"
#include "../QcWidgetFactory.h"

#include <QApplication>

QC_DECLARE_QWIDGET_FACTORY(QcSlider);

QcSlider::QcSlider()
: lastVal(0), bDoAction( false )
{
  setRange(0, 10000);
  setStep( 0.01 );
  lastVal = sliderPosition();

  connect( this, SIGNAL(actionTriggered( int )),
           this, SLOT(action( int )));
}

void  QcSlider::increment( double factor )
{
  QSlider::setValue( QSlider::singleStep() * factor + QSlider::value() );
}

void  QcSlider::decrement( double factor )
{
  QSlider::setValue( QSlider::singleStep() * (-factor) + QSlider::value() );
}

void QcSlider::action( int act )
{
  if( sliderPosition() != lastVal )
  {
      if( act < 5 ) {
        float step = singleStep();
        bool modified = modifyStep( &step );

        if( modified ) {
          if( act == QAbstractSlider::SliderSingleStepAdd ||
              act == QAbstractSlider::SliderPageStepAdd )
            setSliderPosition( lastVal + step );
          else
            setSliderPosition( lastVal - step );
        }
      }
      lastVal = sliderPosition();
      Q_EMIT( action() );
  }
}

void QcSlider::setStep( float fStep )
{
  int iStep = fStep * 10000;
  setSingleStep( iStep );
  setPageStep( iStep );
}
