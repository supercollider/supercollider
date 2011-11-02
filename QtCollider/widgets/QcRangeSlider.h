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

#ifndef QC_RANGE_SLIDER
#define QC_RANGE_SLIDER

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"

#include <QWidget>

class QcRangeSlider :
  public QWidget, public QcHelper, public QcAbstractStepValue
{
  Q_OBJECT
  Q_PROPERTY( Qt::Orientation orientation
              READ orientation WRITE setOrientation );
  Q_PROPERTY( float loValue READ loValue WRITE setLoValue )
  Q_PROPERTY( float hiValue READ hiValue WRITE setHiValue )
  Q_PROPERTY( float shiftScale READ dummyFloat WRITE setShiftScale );
  Q_PROPERTY( float ctrlScale READ dummyFloat WRITE setCtrlScale );
  Q_PROPERTY( float altScale READ dummyFloat WRITE setAltScale );
  Q_PROPERTY( float step READ dummyFloat WRITE setStep )

  public:
    enum MouseMode {
      None = 0,
      Drag,
      SetLo,
      SetHi
    };

    QcRangeSlider();
    Qt::Orientation orientation() const { return _ort; }
    void setOrientation( Qt::Orientation o );
    float loValue() const { return _lo; }
    void setLoValue( float );
    float hiValue() const { return _hi; }
    void setHiValue( float );
    void setStep( float f ) { _step = f; }
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

  public Q_SLOTS:
    void increment( double factor );
    void decrement( double factor );

  Q_SIGNALS:
    void action();

  private:
    QRect thumbRect();
    float valueFromPos( const QPoint& pos );
    void moveBy( float );
    void increment();
    void decrement();
    void mouseMoveEvent ( QMouseEvent * );
    void mousePressEvent ( QMouseEvent * );
    void mouseReleaseEvent ( QMouseEvent * );
    void keyPressEvent ( QKeyEvent * );
    void paintEvent ( QPaintEvent * );

    Qt::Orientation _ort;
    float _lo;
    float _hi;
    float _step;
    QPoint dragOffset;
    MouseMode mouseMode;
};

#endif
