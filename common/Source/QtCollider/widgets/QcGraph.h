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

#ifndef QC_GRAPH_H
#define QC_GRAPH_H

#include "Common.h"
#include "../QcHelper.h"

#include <QWidget>

class QcGraph : public QWidget, QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList value READ value WRITE setValue )
  Q_PROPERTY( VariantList strings READ dummyVariantList WRITE setStrings );
  Q_PROPERTY( int index READ index WRITE setIndex );
  Q_PROPERTY( float thumbSize READ dummyFloat WRITE setThumbSize );
  Q_PROPERTY( float thumbWidth READ dummyFloat WRITE setThumbWidth );
  Q_PROPERTY( float thumbHeight READ dummyFloat WRITE setThumbHeight );
  Q_PROPERTY( QColor strokeColor READ dummyColor WRITE setStrokeColor );
  Q_PROPERTY( QColor fillColor READ dummyColor WRITE setFillColor );
  Q_PROPERTY( QColor selectionColor READ dummyColor WRITE setSelectionColor );
  Q_PROPERTY( QColor gridColor READ dummyColor WRITE setGridColor );
  Q_PROPERTY( bool drawLines READ dummyBool WRITE setDrawLines );
  Q_PROPERTY( bool drawRects READ dummyBool WRITE setDrawRects );
  Q_PROPERTY( bool editable READ dummyBool WRITE setEditable );
  Q_PROPERTY( float step READ dummyFloat WRITE setStep );
  Q_PROPERTY( float x READ currentX WRITE setCurrentX );
  Q_PROPERTY( float y READ currentY WRITE setCurrentY );
  Q_PROPERTY( QPointF grid READ grid WRITE setGrid );
  Q_PROPERTY( bool gridOn READ dummyBool WRITE setGridOn );

  struct Element {
    Element() : editable( true ) {};
    QPointF value;
    QString text;
    bool editable;
  };

  public:
    QcGraph();

    VariantList value() const;
    int index() const { return selIndex; }
    float currentX() const;
    float currentY() const;
    QPointF grid() const { return _gridMetrics; }

    void setValue( const VariantList & );
    void setStrings( const VariantList &list );
    void setIndex( int i ) {
      if( i>=0 && i<_elems.count() ) {
        selIndex = i;
        update();
      }
    }
    void setCurrentX( float );
    void setCurrentY( float );
    void setThumbSize( float f ) { _thumbSize = QSize(f,f); update(); }
    void setThumbWidth( float f ) { _thumbSize.setWidth(f); update(); }
    void setThumbHeight( float f ) { _thumbSize.setHeight(f); update(); }
    void setStrokeColor( const QColor & c ) { _strokeColor = c; update(); }
    void setFillColor( const QColor & c ) { _fillColor = c; update(); }
    void setSelectionColor( const QColor & c ) { _selColor = c; update(); }
    void setGridColor( const QColor & c ) { _gridColor = c; update(); }
    void setDrawLines( bool b ) { _drawLines = b; update(); }
    void setDrawRects( bool b ) { _drawRects = b; update(); }
    void setEditable( bool b ) { _editable = b; update(); }
    void setStep( float f );
    void setGrid( const QPointF &pt ) { _gridMetrics = pt; update(); }
    void setGridOn( bool b ) { _gridOn = b; update(); }

  private:
    inline void setValue( Element &, const QPointF & );
    QPointF pos( const QPointF & value );
    QPointF value( const QPointF & pos );
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void keyPressEvent( QKeyEvent * );
    QSize sizeHint() const { return QSize( 200,200 ); }
    void doAction( Qt::KeyboardModifiers );

    QList<Element> _elems;

    QSize _thumbSize;
    QColor _strokeColor;
    QColor _fillColor;
    QColor _selColor;
    QColor _gridColor;
    QPointF _gridMetrics;
    bool _gridOn;

    bool _drawLines;
    bool _drawRects;

    bool _editable;
    float _step;

    int selIndex;
    int dragIndex;
    QPointF dragDelta;
};

#endif
