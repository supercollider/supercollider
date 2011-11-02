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

#ifndef QC_WIDGET_PROXY_H
#define QC_WIDGET_PROXY_H

#include "QObjectProxy.h"

#include <QWidget>
#include <QAtomicInt>
#include <QMimeData>

namespace QtCollider {
  struct SetFocusEvent;
  struct SetAlwaysOnTopEvent;
  struct StartDragEvent;
}

class QWidgetProxy : public QObjectProxy
{
  Q_OBJECT

public:

  enum GlobalEvent {
    KeyPress = 0x001,
    KeyRelease = 0x002
  };

public:

  static void setGlobalEventEnabled ( GlobalEvent ev, bool b ) {
    int mask = _globalEventMask;
    if(b)
      mask |= ev;
    else
      mask &= ~ev;
    _globalEventMask = mask;
  }

public:

  QWidgetProxy( QWidget *w, PyrObject *po );

  void setKeyEventWidget( QWidget * );

  void setMouseEventWidget( QWidget * );

  bool alwaysOnTop();

  void refresh();

  void setLayout ( QObjectProxy *layoutProxy );

  virtual bool setParent( QObjectProxy *parent );

  inline QWidget *widget() { return static_cast<QWidget*>( object() ); }

protected:

  virtual void customEvent( QEvent * );

  virtual bool filterEvent( QObject *, QEvent *, EventHandlerData &, QList<QVariant> & args );

private Q_SLOTS:

  void customPaint( QPainter * );

private:
  bool interpretMouseEvent( QObject *, QEvent *, QList<QVariant> &args );
  bool interpretMouseWheelEvent( QObject *, QEvent *, QList<QVariant> &args );
  bool interpretKeyEvent( QObject *, QEvent *, QList<QVariant> &args );
  bool interpretDragEvent( QObject *, QEvent *, QList<QVariant> &args );

  void bringFrontEvent();
  void setFocusEvent( QtCollider::SetFocusEvent * );
  void setAlwaysOnTopEvent( QtCollider::SetAlwaysOnTopEvent * );
  void startDragEvent( QtCollider::StartDragEvent * );

  static void sendRefreshEventRecursive( QWidget *w );

  QWidget *_keyEventWidget;
  QWidget *_mouseEventWidget;
  static QAtomicInt _globalEventMask;
};

namespace QtCollider {

struct SetFocusEvent : public QEvent
{
  SetFocusEvent( bool b )
  : QEvent( (QEvent::Type) QtCollider::Event_Proxy_SetFocus ),
    focus(b)
  {}
  bool focus;
};

struct SetAlwaysOnTopEvent  : public QEvent
{
  SetAlwaysOnTopEvent( bool b )
  : QEvent( (QEvent::Type) QtCollider::Event_Proxy_SetAlwaysOnTop ),
    alwaysOnTop(b)
  {}
  bool alwaysOnTop;
};

struct StartDragEvent : public QEvent
{
  StartDragEvent( const QString &label_, QMimeData *data_ )
  : QEvent( (QEvent::Type) QtCollider::Event_Proxy_StartDrag ),
    label( label_ ), data( data_ )
  {}
  ~StartDragEvent() { delete data; }
  QString label;
  QMimeData *data;
};

}

#endif //QC_WIDGET_PROXY_H
