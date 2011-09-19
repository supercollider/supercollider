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

#ifndef QC_SLIDER_2D
#define QC_SLIDER_2D

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"

#include <QWidget>

class QcSlider2D : public QWidget, public QcHelper, public QcAbstractStepValue
{
  Q_OBJECT
  Q_PROPERTY( float xValue READ xValue WRITE setXValue )
  Q_PROPERTY( float yValue READ yValue WRITE setYValue )
  Q_PROPERTY( float shiftScale READ dummyFloat WRITE setShiftScale );
  Q_PROPERTY( float ctrlScale READ dummyFloat WRITE setCtrlScale );
  Q_PROPERTY( float altScale READ dummyFloat WRITE setAltScale );
  Q_PROPERTY( float step READ dummyFloat WRITE setStep )

  public:
    QcSlider2D();
    float xValue() const { return _x; }
    float yValue() const { return _y; }
    void setXValue( float x ) { setValue( QPointF( x, _y ), false ); }
    void setYValue( float y ) { setValue( QPointF( _x, y ), false ); }
    void setStep( float f ) { _step = f;}
    QSize sizeHint() const { return QSize(150,150); }
    QSize minimumSizeHint() const { return QSize(30,30); }
  public Q_SLOTS:
    void incrementX( double factor = 1.f );
    void decrementX( double factor = 1.f );
    void incrementY( double factor = 1.f );
    void decrementY( double factor = 1.f );
  Q_SIGNALS:
    void action();
    void randomize();
  private:
    QRect thumbRect();
    QPointF valueFromPos( const QPoint pos );
    void setValue( const QPointF val, bool doAction = true );
    void mouseMoveEvent ( QMouseEvent * );
    void mousePressEvent ( QMouseEvent * );
    void keyPressEvent ( QKeyEvent * );
    void paintEvent ( QPaintEvent * );

    float _x;
    float _y;
    QSize _thumbSize;
    float _step;
};

#endif
