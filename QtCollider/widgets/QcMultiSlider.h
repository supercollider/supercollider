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

#ifndef QC_MULTI_SLIDER
#define QC_MULTI_SLIDER

#include "../QcHelper.h"

#include <QWidget>

class QcMultiSlider : public QWidget, QcHelper
{
  // TODO setting selection with mouse
  Q_OBJECT
  Q_PROPERTY( VariantList values READ values WRITE setValues );
  Q_PROPERTY( VariantList reference READ dummyVariantList WRITE setReference );
  Q_PROPERTY( float value READ value WRITE setValue );
  Q_PROPERTY( float stepSize READ dummyFloat WRITE setStepSize );
  Q_PROPERTY( int index READ index WRITE setIndex );
  Q_PROPERTY( int selectionSize READ selectionSize WRITE setSelectionSize );
  Q_PROPERTY( Qt::Orientation orientation
              READ orientation WRITE setOrientation );
  Q_PROPERTY( bool elastic READ dummyBool WRITE setElastic );
  Q_PROPERTY( int indexThumbSize READ dummyFloat WRITE setIndexThumbSize );
  Q_PROPERTY( float valueThumbSize READ dummyFloat WRITE setValueThumbSize );
  Q_PROPERTY( int gap READ dummyInt WRITE setGap );
  Q_PROPERTY( bool drawLines READ dummyBool WRITE setDrawLines );
  Q_PROPERTY( bool drawRects READ dummyBool WRITE setDrawRects );
  Q_PROPERTY( bool isFilled READ dummyBool WRITE setIsFilled );
  Q_PROPERTY( bool highlight READ dummyBool WRITE setHighlight );
  Q_PROPERTY( QColor fillColor READ dummyColor WRITE setFillColor );
  Q_PROPERTY( QColor strokeColor READ dummyColor WRITE setStrokeColor );
  Q_PROPERTY( bool editable READ dummyBool WRITE setEditable );
  Q_PROPERTY( int startIndex READ dummyInt WRITE setStartIndex );

  public:
    QcMultiSlider();
    QSize sizeHint() const { return QSize( 500,300 ); }
    QSize minimumSizeHint() const { return QSize( 50, 50 ); }
  Q_SIGNALS:
    void modified();
    void interacted();
    void action();
    void metaAction();
  public Q_SLOTS:
    void doAction();
  private:
    Qt::Orientation orientation() const { return ort; }
    VariantList values() const;
    float value() const;
    int index() const { return _currentIndex; }
    int selectionSize() const { return _selectionSize; }

    void setSize( int size );
    void setValues( const VariantList & );
    void setReference( const VariantList & );
    void setValue( float f );
    void setStepSize( float f );
    void setIndex( int i ) {
      if( i >= 0 && i < _values.count() ) { _currentIndex = i; update(); }
    }
    void setSelectionSize( int i ) { _selectionSize = i; update(); }
    void setOrientation( Qt::Orientation o ) { ort = o; update(); }
    void setElastic( bool b ) { elastic = b; update(); }
    void setIndexThumbSize( float f ) { thumbSize.setWidth(f); update(); }
    void setValueThumbSize( float f ) { thumbSize.setHeight(f); update(); }
    void setGap( int i ) { gap = i; update(); }
    void setDrawLines( bool b ) { drawLines = b; update(); }
    void setDrawRects( bool b ) { drawRects = b; update(); }
    void setIsFilled( bool b ) { isFilled = b; update(); }
    void setHighlight( bool b ) { highlight = b; update(); }
    void setFillColor( const QColor& c ) { _fillColor = c; update(); }
    void setStrokeColor( const QColor& c ) { _strokeColor = c; update(); }
    void setEditable( bool b ) { editable = b; }
    void setStartIndex( int i ) { startIndex = i; update(); }

    inline float valueFromPos( float pos, float range );
    inline void setValue( int index, float value );
    void mousePressEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void paintEvent( QPaintEvent * );

    // values
    QList<float> _values;
    QList<float> _ref;
    int _currentIndex;
    int _selectionSize;

    // functional properties
    float roundStep;
    bool editable;

    // visual properties
    Qt::Orientation ort;
    bool elastic;
    QSizeF thumbSize;
    int gap;
    bool drawRects;
    bool drawLines;
    bool isFilled;
    bool highlight;
    QColor _fillColor;
    QColor _strokeColor;
    int startIndex;

    // temporary
    QPoint moveOrigin;
};

#endif
