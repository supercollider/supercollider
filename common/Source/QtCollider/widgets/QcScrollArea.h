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

#include "../QcHelper.h"
#include "../QcPen.h"

#include <QAbstractScrollArea>

class QcScrollWidget : public QWidget
{
  public:
    QcScrollWidget( QWidget *parent = 0 ) : QWidget( parent )
    {}
    void updateSize();
    bool event ( QEvent * );
    bool eventFilter ( QObject *, QEvent * );
};

class QcScrollArea : public QAbstractScrollArea, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( bool hasBorder READ dummyBool WRITE setHasBorder );
  Q_PROPERTY( QColor background READ dummyColor WRITE setBackground );
  Q_PROPERTY( bool paint READ dummyBool WRITE setPaint );
  public:
    QcScrollArea();
    void addWidget( QWidget* );
    void setBackground ( const QColor &color );
    void setPaint( bool b ) { paint = b; }
    void setHasBorder( bool b );
    Q_INVOKABLE void rebuildPen();
  public Q_SLOTS:
    void update() { viewport()->update(); }
  private:
    void updateScrollBars();
    void updateWidgetPosition();
    void scrollContentsBy ( int dx, int dy );
    void resizeEvent( QResizeEvent * );
    bool eventFilter ( QObject *, QEvent * );
    void paintEvent( QPaintEvent * );

    QcScrollWidget *scrollWidget;
    bool paint;
    bool painting;
    QColor bkg;

    QtCollider::Pen pen;
};
