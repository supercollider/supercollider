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

#include "../Common.h"
#include "../QcHelper.h"

#include <QWidget>

struct QcGraphElement {

  friend class QcGraphModel;

  enum CurveType {
    Step = 0,
    Linear,
    Sine,
    Welch,
    Exponential,
    Quadratic,
    Cubic,
    Curvature
  };

  QcGraphElement() :
    curveType( Linear ),
    curvature( 0.0 ),
    editable( true ),
    selected( false ),
    _prev(0),
    _next(0)
  {};

  void setCurveType( CurveType type, double curve = 0.0 ) {
    curveType = type;
    curvature = curve;
  }

  QcGraphElement *prev() { return _prev; }
  QcGraphElement *next() { return _next; }

  QPointF value;
  QString text;
  QColor fillColor;
  CurveType curveType;
  double curvature;
  bool editable;
  bool selected;

private:
  QcGraphElement *_prev;
  QcGraphElement *_next;
};

class QcGraphModel : public QObject
{
  Q_OBJECT

  public:

    QcGraphModel( QObject * parent = 0 ) : QObject(parent) {}

    struct Connection {
      Connection( QcGraphElement * a_ = 0, QcGraphElement * b_ = 0 ) : a(a_), b(b_) {}
      QcGraphElement * a;
      QcGraphElement * b;
    };

    inline QList<QcGraphElement*> elements() const { return _elems; }

    inline QcGraphElement *elementAt( int i ) const { return _elems[i]; }

    inline int elementCount() const { return _elems.count(); }

    void append( QcGraphElement * e );

    void removeAt( int i );

    inline QList<Connection> connections() const { return _conns; }

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

  Q_SIGNALS:
    void appended( QcGraphElement * );
    void removed( QcGraphElement * );

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
  Q_PROPERTY( double step READ step WRITE setStep );
  Q_PROPERTY( int selectionForm READ selectionForm WRITE setSelectionForm );
  Q_PROPERTY( int horizontalOrder READ horizontalOrder WRITE setHorizontalOrder );
  Q_PROPERTY( float x READ currentX WRITE setCurrentX );
  Q_PROPERTY( float y READ currentY WRITE setCurrentY );
  Q_PROPERTY( QPointF grid READ grid WRITE setGrid );
  Q_PROPERTY( bool gridOn READ dummyBool WRITE setGridOn );

  public:
    Q_INVOKABLE void connectElements( int, VariantList );
    Q_INVOKABLE void setStringAt( int, const QString & );
    Q_INVOKABLE void setFillColorAt( int, const QColor & );
    Q_INVOKABLE void setEditableAt( int, bool );
    Q_INVOKABLE void setCurves( double curvature );
    Q_INVOKABLE void setCurves( int type );
    Q_INVOKABLE void setCurves( const VariantList & curves );

  public Q_SLOTS:
    Q_INVOKABLE void select( int index, bool exclusive = true );
    Q_INVOKABLE void deselect( int index );
    Q_INVOKABLE void deselectAll();

  Q_SIGNALS:
    void action();
    void metaAction();

  public:
    enum SelectionForm {
      ElasticSelection,
      RigidSelection
    };

    enum Order {
      NoOrder,
      RigidOrder
    };

  public:
    QcGraph();

    VariantList value() const;
    int index() const { return _curIndex; }
    float currentX() const;
    float currentY() const;
    QPointF grid() const { return _gridMetrics; }

    void setValue( const VariantList & );
    void setStrings( const VariantList &list );

    void setIndex( int i );
    void setCurrentX( float );
    void setCurrentY( float );
    void setThumbSize( float f ) { _thumbSize = QSize(f,f); update(); }
    void setThumbWidth( float f ) { _thumbSize.setWidth(f); update(); }
    void setThumbHeight( float f ) { _thumbSize.setHeight(f); update(); }
    void setStrokeColor( const QColor & c ) { _strokeColor = c; update(); }
    void setFillColor( const QColor & c );

    void setSelectionColor( const QColor & c ) { _selColor = c; update(); }
    void setGridColor( const QColor & c ) { _gridColor = c; update(); }
    void setDrawLines( bool b ) { _drawLines = b; update(); }
    void setDrawRects( bool b ) { _drawRects = b; update(); }
    void setEditable( bool b ) { _editable = b; update(); }

    double step() const { return _step; }
    void setStep( double );
    int selectionForm() const { return (int)_selectionForm; }
    void setSelectionForm( int i ) { _selectionForm = (SelectionForm) i; }
    int horizontalOrder() const { return (int)_xOrder; }
    void setHorizontalOrder( int i );
    void setGrid( const QPointF &pt ) { _gridMetrics = pt; update(); }
    void setGridOn( bool b ) { _gridOn = b; update(); }
    QSize sizeHint() const { return QSize( 200,200 ); }
    QSize minimumSizeHint() const { return QSize( 50,50 ); }

  private Q_SLOTS:
    void onElementRemoved( QcGraphElement *e );

  private:
    struct SelectedElement {
      SelectedElement( QcGraphElement *e ) : elem(e) {}
      bool operator == (const SelectedElement & other) {
        return elem == other.elem;
      }

      QcGraphElement *elem;
      QPointF moveOrigin; // in data domain
    };

  private:
    void setAllDeselected();
    void setIndexSelected( int index, bool selected );
    void restrictValue( QPointF & );
    void orderRestrictValue( QcGraphElement *, QPointF &, bool selected );
    void setValue( QcGraphElement *, const QPointF & );
    void ensureOrder();
    void moveFree( QcGraphElement *, const QPointF & );
    void moveOrderRestricted( QcGraphElement *, const QPointF & );
    void moveSelected( const QPointF & dValue, SelectionForm, bool fromCache );
    QPointF pos( const QPointF & value );
    QPointF value( const QPointF & pos );
    void addCurve( QPainterPath &, QcGraphElement *e1, QcGraphElement *e2 );
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
    double _step;
    SelectionForm _selectionForm;
    Order _xOrder;

    int _curIndex;

    struct Selection {
      Selection () : cached(false), shallMove(false) {}
      int size() { return elems.size(); }
      int count() { return elems.count(); }

      QList<SelectedElement> elems;
      bool cached;
      bool shallMove;
      QPointF moveOrigin; // in data domain
    } _selection;
};

#endif
