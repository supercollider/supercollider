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

#include "../QcObjectFactory.h"
#include "../QWidgetProxy.h"
#include "BasicWidgets.h"
#include "QcScrollArea.h"

#include <QShortcut>

class QcWindowFactory : public QcAbstractFactory
{
public:
  QcWindowFactory() : QcAbstractFactory( "QcWindow" ) {}

  virtual QObjectProxy *newInstance( PyrObject *po, QList<QVariant> & arguments ){

    if( arguments.count() < 5 ) return 0;

    bool scroll = arguments[4].value<bool>();

    QWidget *window;
    QWidget *canvas;
    if( scroll ) {
      QcScrollArea *scroll = new QcScrollArea();
      window = scroll;
      canvas = scroll->widget();
    }
    else {
      window = canvas = new QcCustomPainted();
    }

    QString name = arguments[0].toString();
    window->setWindowTitle( name );

    QRect geom = arguments[1].value<QRect>();
    bool resizable = arguments[2].value<bool>();
    if( resizable ) {
      window->setGeometry( geom );
    } else {
      window->move( geom.topLeft() );
      window->setFixedSize( geom.size() );
    }

    bool border = arguments[3].value<bool>();
    if( !border )
      window->setWindowFlags( window->windowFlags() | Qt::FramelessWindowHint );

    QShortcut *closeShortcut =
      new QShortcut( QKeySequence( Qt::CTRL | Qt::Key_W ), window );
    QObject::connect( closeShortcut, SIGNAL(activated()),
                      window, SLOT(close()) );

    QWidgetProxy *proxy = new QWidgetProxy( window, po );

    QObject::connect( canvas, SIGNAL(painting(QPainter*)),
                      proxy, SLOT(customPaint(QPainter*)) );

    return proxy;
  }
};

static QcWindowFactory f;
