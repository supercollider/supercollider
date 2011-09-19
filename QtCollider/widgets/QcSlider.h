/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_SLIDER
#define QC_SLIDER

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"

#include <QSlider>

class QcSlider : public QSlider, public QcHelper, public QcAbstractStepValue
{
  Q_OBJECT
  Q_PROPERTY( float shiftScale READ dummyFloat WRITE setShiftScale );
  Q_PROPERTY( float ctrlScale READ dummyFloat WRITE setCtrlScale );
  Q_PROPERTY( float altScale READ dummyFloat WRITE setAltScale );
  Q_PROPERTY( float step READ dummyFloat WRITE setStep )
  Q_PROPERTY( float value READ value WRITE setValue );

  public:
    QcSlider();
    float value() { return QSlider::value() * 0.0001f; }
    void setStep( float );
    void setValue( float val ) { QSlider::setValue( val * 10000 ); }
  public Q_SLOTS:
    void increment( double factor );
    void decrement( double factor );
  Q_SIGNALS:
    void action();
  private Q_SLOTS:
    void action( int val );
  private:
    int lastVal;
    bool bDoAction;
};

#endif
