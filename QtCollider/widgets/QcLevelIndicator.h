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

#ifndef QC_LEVEL_INDICATOR_H
#define QC_LEVEL_INDICATOR_H

#include "../QcHelper.h"
#include "../style/style.hpp"

#include <QWidget>
#include <QTimer>

class QcLevelIndicator : public QWidget, QcHelper, QtCollider::Style::Client
{
  Q_OBJECT
  Q_PROPERTY( float value READ dummyFloat WRITE setValue );
  Q_PROPERTY( float warning READ dummyFloat WRITE setWarning );
  Q_PROPERTY( float critical READ dummyFloat WRITE setCritical );
  Q_PROPERTY( float peak READ dummyFloat WRITE setPeak );
  Q_PROPERTY( bool drawPeak READ dummyBool WRITE setDrawPeak );
  Q_PROPERTY( int ticks READ dummyInt WRITE setTicks );
  Q_PROPERTY( int majorTicks READ dummyInt WRITE setMajorTicks );
  Q_PROPERTY( QColor grooveColor READ grooveColor WRITE setGrooveColor );

public:
  QcLevelIndicator();
  void setValue( float f ) { _value = f; update(); }
  void setWarning( float f ) { _warning = f; update(); }
  void setCritical( float f ) { _critical = f; update(); }
  void setPeak( float f ) { _peak = f; update(); }
  void setDrawPeak( bool b ) { _drawPeak = b; update(); }
  void setTicks( int i ) { _ticks = qMax(i,0); update(); }
  void setMajorTicks( int i ) { _majorTicks = qMax(i,0); update(); }
  virtual QSize sizeHint() const { return QSize( 25, 150 ); }
private Q_SLOTS:
  void clipTimeout();
private:
  void paintEvent( QPaintEvent *e );
  float _value;
  float _warning;
  float _critical;
  float _peak;
  bool _drawPeak;
  int _ticks;
  int _majorTicks;
  bool _clipped;
  QTimer *_clipTimer;
};

#endif
