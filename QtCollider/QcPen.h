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

#ifndef QC_PEN_H
#define QC_PEN_H

#include <QPainter>
#include <QPainterPath>
#include <QStack>

namespace QtCollider {

class Pen
{
  friend void beginPen( Pen * );
  friend void endPen();

  public:

    enum EnvironmentFlag {
      HasTransform = 0x01,
      HasPen = 0x02,
      HasBrush = 0x04,
      HasFont = 0x08,
      HasAntiAliasing = 0x10
    };

    enum Action {
      FillAndStroke = 1,
      Fill,
      Stroke,
      Clip,
      StringAtPoint,
      StringInRect,
      Save,
      Restore
    };

    struct Environment {
      Environment()
      : antiAliasing( false ),
        flags( 0 )
      {}
      inline void setBrush( const QBrush & );
      inline void setPen( const QPen & );
      inline void setFont( const QFont &f );
      inline void setAntiAliasing( bool aa );
      inline void translate( float x, float y );
      inline void scale( float x, float y );
      inline void shear( float x, float y );
      inline void rotate( float angle, float x, float y );
      inline void setTransform( const QTransform & t );

      QTransform transform;
      QPen pen;
      QBrush brush;
      QFont font;
      bool antiAliasing;
      int flags;
    };

    Pen() : score(0)
    {}

    ~Pen()
    {
      delete score;
    }

    void reset();

    void save();

    void restore();

    void draw( int style );

    void clip();

    void clear()
    {
      path = QPainterPath();
    }

    inline void setFillColor( const QColor &c )
      { env.setBrush( c ); }
    inline void setStrokeColor( const QColor &c ) {
      env.pen.setColor( c );
      env.setPen( env.pen );
    }
    inline void setWidth( float w ) {
      env.pen.setWidth( w );
      env.setPen( env.pen );
    }
    inline void setFont( const QFont &f )
      { env.setFont( f ); }
    inline void setAntiAliasing( bool aa )
      { env.setAntiAliasing( aa ); }

    inline void translate( float x, float y )
      { env.translate(x,y); }
    inline void scale( float x, float y )
      { env.scale(x,y); }
    inline void shear( float x, float y )
      { env.shear(x,y); }
    inline void rotate( float angle, float x, float y )
      { env.rotate( angle, x, y ); }
    inline void setTransform( const QTransform &t )
      { env.setTransform( t ); }

    inline void moveTo( const QPointF &pt ) {
      path.moveTo( pt );
    }
    inline void lineTo( const QPointF &pt ) {
      path.lineTo( pt );
    }
    inline void cubicTo( const QPointF &pt1, const QPointF &pt2, const QPointF &pt3 ) {
      path.cubicTo( pt1, pt2, pt3 );
    }
    inline void quadTo( const QPointF &pt1, const QPointF &pt2 ) {
      path.quadTo( pt1, pt2 );
    }
    inline void addRect( const QRectF &r ) {
      path.addRect( r );
    }
    inline void addRoundedRect( const QRectF &r, float radiusX, float radiusY ) {
      path.addRoundedRect( r, radiusX, radiusY );
    }
    inline void addEllipse( const QRectF &r ) {
      path.addEllipse( r );
    }
    void addWedge( const QPointF &, float, float, float );
    void addAnnularWedge( const QPointF &, float, float, float, float );
    void stringAtPoint( const QPointF &, const QString & );
    void stringInRect( const QRectF &, const QString & );

    inline void setBaseTransform( const QTransform &t ) {
      baseTransform = t;
    }

    void paint( QPainter *p );

  private:
    void applyEnvironment( QPainter *, const Environment & );
    Environment env;
    QPainterPath path;
    QStack<Environment> envs;

    QByteArray scoreData;
    QDataStream *score;
    QTransform baseTransform;
};

void beginPen( Pen * );
void endPen();

inline void Pen::Environment::setBrush( const QBrush &b )
{
  brush = b;
  flags |= HasBrush;
}

inline void Pen::Environment::setPen( const QPen &p )
{
  pen = p;
  flags |= HasPen;
}

inline void Pen::Environment::setFont( const QFont &f )
{
  font = f;
  flags |= HasFont;
}

inline void Pen::Environment::setAntiAliasing( bool aa )
{
  antiAliasing = aa;
  flags |= HasAntiAliasing;
}


inline void Pen::Environment::translate( float x, float y )
{
  transform.translate( x, y );
  flags |= HasTransform;
}

inline void Pen::Environment::scale( float x, float y )
{
  transform.scale( x, y );
  flags |= HasTransform;
}

inline void Pen::Environment::shear( float x, float y )
{
  transform.shear( x, y );
  flags |= HasTransform;
}

inline void Pen::Environment::rotate( float angle, float x, float y )
{
  transform.translate( x, y );
  transform.rotate( angle );
  transform.translate( -x, -y );
  flags |= HasTransform;
}

inline void Pen::Environment::setTransform( const QTransform & t )
{
  transform = t;
  flags |= HasTransform;
}

} //namespace

QDataStream & operator<< ( QDataStream &, QtCollider::Pen::Environment & );
QDataStream & operator>> ( QDataStream &, QtCollider::Pen::Environment & );

#endif //QC_PEN_H
