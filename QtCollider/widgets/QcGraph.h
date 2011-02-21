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

struct QcGraphElement {
  enum CurveType {
    Step,
    Linear,
    Exponential,
    Sine,
    Welch,
    Curvature
  };
  QcGraphElement() : fillColor( QColor(0,0,0) ), curvature( 0.f ), editable( true ) {};
  void setCurveType( QVariant data ) {
    if( data.type() == QVariant::String ) {
      QString curveName = data.toString();
      //printf("curve name: %s\n",curveName.toStdString().c_str());
      if( curveName == "step" ) curveType = Step;
      else if( curveName == "linear" ) curveType = Linear;
      else if( curveName == "exponential" ) curveType = Exponential;
      else if( curveName == "sine" ) curveType = Sine;
      else if( curveName == "welch" ) curveType = Welch;
    }
    else {
      //printf("curvature: %f\n", data.value<float>());
      curveType = Curvature;
      curvature = data.value<float>();
    }
  }
  QPointF value;
  QString text;
  QColor fillColor;
  CurveType curveType;
  float curvature;
  bool editable;
};

class QcGraphModel
{
  public:

    struct Connection {
      Connection( QcGraphElement * a_ = 0, QcGraphElement * b_ = 0 ) : a(a_), b(b_) {}
      QcGraphElement * a;
      QcGraphElement * b;
    };

    inline QList<QcGraphElement*> elements() const { return _elems; }

    inline QcGraphElement *elementAt( int i ) const { return _elems[i]; }

    inline int elementCount() const { return _elems.count(); }

    inline void append( QcGraphElement * e ) { _elems.append(e); }

    inline QList<Connection> connections() const { return _conns; }

    void removeAt( int i ) {
      QcGraphElement *e = _elems[i];
      int ci = _conns.count();
      while( ci-- ) {
        Connection c = _conns[ci];
        if( c.a == e || c.b == e ) _conns.removeAt(ci);
      }
      _elems.removeAt(i);
    }

    void connect( int xi, int yi ) {
      QcGraphElement *x = _elems[xi];
      QcGraphElement *y = _elems[yi];
      int i;
      for( i = 0; i < _conns.count(); ++i ) {
        Connection c = _conns[i];
        if( (c.a==x && c.b==y) || (c.a==y && c.b==x) ) return;
      }
      _conns.append( Connection(x,y) );
    }

  private:

    QList<Connection> _conns;
    QList<QcGraphElement*> _elems;
};

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

  public:
    QcGraph();

    VariantList value() const;
    int index() const { return selIndex; }
    float currentX() const;
    float currentY() const;
    QPointF grid() const { return _gridMetrics; }

    void setValue( const VariantList & );
    void setStrings( const VariantList &list );
    Q_INVOKABLE void setCurves( const VariantList & curves );
    Q_INVOKABLE void setStringAt( int, const QString & );
    Q_INVOKABLE void connectElements( int, VariantList );
    void setIndex( int i ) {
      if( i>=0 && i<_model.elementCount() ) {
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
    void setFillColor( const QColor & c );
    Q_INVOKABLE void setFillColorAt( int, const QColor & );
    void setSelectionColor( const QColor & c ) { _selColor = c; update(); }
    void setGridColor( const QColor & c ) { _gridColor = c; update(); }
    void setDrawLines( bool b ) { _drawLines = b; update(); }
    void setDrawRects( bool b ) { _drawRects = b; update(); }
    void setEditable( bool b ) { _editable = b; update(); }
    Q_INVOKABLE void setEditableAt( int, bool );
    void setStep( float f );
    void setGrid( const QPointF &pt ) { _gridMetrics = pt; update(); }
    void setGridOn( bool b ) { _gridOn = b; update(); }
    QSize sizeHint() const { return QSize( 200,200 ); }
    QSize minimumSizeHint() const { return QSize( 50,50 ); }

  Q_SIGNALS:
    void action();
    void metaAction();

  private:
    inline void setValue( QcGraphElement *, const QPointF & );
    QPointF pos( const QPointF & value );
    QPointF value( const QPointF & pos );
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void keyPressEvent( QKeyEvent * );
    void doAction( Qt::KeyboardModifiers );

    QcGraphModel _model;

    QSize _thumbSize;
    QColor _strokeColor;
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
