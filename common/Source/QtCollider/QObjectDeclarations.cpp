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

#define DECLARE_QT_CLASS( name, class ) \
  QObject *create_##name ( const QVariant & arguments ) \
    { Q_UNUSED( arguments ); return new class (); } \
  QcObjectFactory name ##_factory ( #name, create_##name );

/*
* Declaration for a simple QObject goes like this:
*
* DECLARE_QT_CLASS( identifier, CppClass )
*
* ...where CppClass is a subclass of Qt's QObject class.
* And then in supercollider you can do:
*
* x = QObject.new( "identifier" )
*/

/*
* If you are creating a GUI class, you should use the DECLARE_WIDGET_CLASS
* macro.
*/

/*
* In case you really need to use (extra) construction arguments and/or you do
* not want to provide a default constructor for your C++ class, you have to
* create your own factory function and your own QcObjectFactory instance.
*
* If this the case, and you are creating a GUI class. it is strongly recommended
* to call initWidget() in your factory function to do important initializations,
* common to all GUI classes. initWidget() expects a VariantList with a QRect
* for bounds and a QObjectProxy* for parent as arguments.
*/

#include "QObjectProxy.h"
#include "widgets/QcScrollArea.h"
#include <QLayout>

void initWidget( QWidget * widget, const QVariant & args )
{
  VariantList argList = args.value<VariantList>();
  if( argList.data.count() < 2 ) return;

  QRect r = argList.data[1].value<QRect>();
  if( r.size().isEmpty() ) r.setSize( widget->sizeHint() );

  widget->setGeometry( r );

  // Do we have a parent?

  QObjectProxy *parentProxy = argList.data[0].value<QObjectProxy*>();
  if( !parentProxy ) return;

  QWidget *parentWidget = qobject_cast<QWidget*>( parentProxy->object() );
  if( !parentWidget ) return;

  // Ok, we have the parent, so stuff the child in

  QcScrollArea *scrollArea = qobject_cast<QcScrollArea*>( parentWidget );

  if( scrollArea )
      scrollArea->addWidget( widget );
  else if( parentWidget->layout() )
      parentWidget->layout()->addWidget( widget );
  else
    widget->setParent( parentWidget );

  widget->show();
}

#define DECLARE_WIDGET_CLASS( name, class ) \
  QObject *create_##name ( const QVariant & arguments ) \
  { \
    QWidget *w = new class (); \
    initWidget( w, arguments ); \
    return w; \
  } \
    \
  QcObjectFactory name ##_factory ( #name, create_##name );

#include <QPushButton>
#include "widgets/BasicWidgets.h"
#include "widgets/QcNumberBox.h"
#include "widgets/QcSlider.h"
#include "widgets/QcSlider2D.h"
#include "widgets/QcRangeSlider.h"
#include "widgets/QcMultiSlider.h"
#include "widgets/QcTextEdit.h"
#include "widgets/QcScope.h"

DECLARE_WIDGET_CLASS( QView, QcDefaultWidget );
DECLARE_WIDGET_CLASS( QHLayout, QcHLayoutWidget );
DECLARE_WIDGET_CLASS( QVLayout, QcVLayoutWidget );
DECLARE_WIDGET_CLASS( QScrollArea, QcScrollArea );
DECLARE_WIDGET_CLASS( QStaticText, QLabel );
DECLARE_WIDGET_CLASS( QTextField, QcTextField );
DECLARE_WIDGET_CLASS( QListView, QcListWidget );
DECLARE_WIDGET_CLASS( QPopUpMenu, QcPopUpMenu );
DECLARE_WIDGET_CLASS( QButton, QcButton );
DECLARE_WIDGET_CLASS( QNumberBox, QcNumberBox );
DECLARE_WIDGET_CLASS( QSlider, QcSlider );
DECLARE_WIDGET_CLASS( QSlider2D, QcSlider2D );
DECLARE_WIDGET_CLASS( QRangeSlider, QcRangeSlider );
DECLARE_WIDGET_CLASS( QMultiSliderView, QcMultiSlider );
DECLARE_WIDGET_CLASS( QTextView, QcTextEdit );
DECLARE_WIDGET_CLASS( QScopeView, QcScope );
DECLARE_WIDGET_CLASS( QFreqScope, QcScope );
DECLARE_WIDGET_CLASS( QUserView, QcCustomPainted );

#include <QShortcut>

QObject *create_QWindow ( const QVariant & arguments ) {

  VariantList argList = arguments.value<VariantList>();
  if( argList.data.count() < 5 ) return 0;

  bool scroll = argList.data[4].value<bool>();

  QWidget *window;
  if( scroll ) window = new QcScrollArea();
  else window = new QcCustomPainted();

  QString name = argList.data[0].toString();
  window->setWindowTitle( name );

  QRect geom = argList.data[1].value<QRect>();
  bool resizable = argList.data[2].value<bool>();
  if( resizable ) {
    window->setGeometry( geom );
  } else {
    window->move( geom.topLeft() );
    window->setFixedSize( geom.size() );
  }

  bool border = argList.data[3].value<bool>();
  if( !border )
    window->setWindowFlags( window->windowFlags() | Qt::FramelessWindowHint );

  QShortcut *closeShortcut =
    new QShortcut( QKeySequence( Qt::CTRL | Qt::Key_W ), window );
  QObject::connect( closeShortcut, SIGNAL(activated()),
                    window, SLOT(close()) );

  return window;
}

QcObjectFactory QWindow_factory ( "QWindow", create_QWindow );
