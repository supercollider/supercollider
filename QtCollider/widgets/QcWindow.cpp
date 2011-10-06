/************************************************************************
*
* Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcWindow.h"
#include "../QcWidgetFactory.h"
#include "../QWidgetProxy.h"

#include <QShortcut>
#include <QApplication>
#include <QDesktopWidget>

static QWidgetProxy *qcInitWindow( QWidget *window, PyrObject *po, QList<QVariant> & arguments )
{
  if( arguments.count() < 4 ) return 0;

  // window title

  QString name = arguments[0].toString();
  window->setWindowTitle( name );

  // size, resizability

  QRect geom = arguments[1].value<QRect>();

  if( geom.isEmpty() ) {
    geom = QApplication::desktop()->availableGeometry();
    geom.setSize( window->sizeHint() );
  }

  bool resizable = arguments[2].value<bool>();
  if( resizable ) {
    window->setGeometry( geom );
  } else {
    window->move( geom.topLeft() );
    window->setFixedSize( geom.size() );
  }

  // frameless?

  bool border = arguments[3].value<bool>();
  if( !border )
    window->setWindowFlags( window->windowFlags() | Qt::FramelessWindowHint );

  // Ctrl+W shortcut: close the window

  QShortcut *closeShortcut =
    new QShortcut( QKeySequence( Qt::CTRL | Qt::Key_W ), window );
  QObject::connect( closeShortcut, SIGNAL(activated()),
                    window, SLOT(close()) );

  // make the proxy

  QWidgetProxy *proxy = new QWidgetProxy( window, po );
  return proxy;
}

class QcWindowFactory : public QcWidgetFactory<QcWindow>
{
  public:
  virtual QObjectProxy *newInstance( PyrObject *pyrobj, QList<QVariant> & args )
  {
    QcWindow *w = new QcWindow;
    QObjectProxy *proxy = qcInitWindow( w, pyrobj, args );
    if( !proxy ) {
      delete w;
      return 0;
    }
    QObject::connect( w, SIGNAL(painting(QPainter*)),
                      proxy, SLOT(customPaint(QPainter*)) );
    return proxy;
  }
};

class QcScrollWindowFactory : public QcWidgetFactory<QcScrollWindow>
{
  // NOTE: painting will be performed by QcScrollWidget and its factory
  public:
  virtual QObjectProxy *newInstance( PyrObject *pyrobj, QList<QVariant> & args )
  {
    QcScrollWindow *w = new QcScrollWindow;
    QObjectProxy *proxy = qcInitWindow( w, pyrobj, args );
    if( !proxy ) delete w;
    return proxy;
  }
};

static QcWindowFactory winFactory;
static QcScrollWindowFactory scrollWinFactory;
