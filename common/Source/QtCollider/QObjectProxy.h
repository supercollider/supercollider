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

#ifndef QC_QOBJECT_PROXY_H
#define QC_QOBJECT_PROXY_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QEvent>

#include <PyrObject.h>
#include <PyrSlot.h>
#include <PyrSymbol.h>

struct VariantList;
struct ScMethodCallEvent;

class QObjectProxy;

typedef void (QObjectProxy::*InterpretEventFn) ( QEvent *, QList<QVariant> & );

class QObjectProxy : public QObject
{
  Q_OBJECT

    enum RequestType {
      SetProperty,
      GetProperty,
      SetEventHandler
    };

  public:

    struct PropertyData
    {
      QString name;
      QVariant value;
      PyrSlot *slot;
    };

    struct EventHandlerData
    {
      EventHandlerData() : type( QEvent::None ) {}
      int type;
      PyrSymbol *method;
      bool direct;
      InterpretEventFn interpretFn;
    };

    QObjectProxy( QObject *qObject, PyrObject *scObject );

    virtual ~QObjectProxy();

    int setProperty( const char *property, PyrSlot *arg, bool direct = false );

    int getProperty( const char *property, PyrSlot *ret, PyrSlot *retExtra );

    void setEventHandler( int eventType, PyrSymbol *method, bool direct );

    bool invokeMethod( const char *method, PyrSlot *arg );

    QObject *object() { return qObject; }

  private Q_SLOTS:
    void objectDestroyed();

  private:
    void invokeScMethod
      ( PyrSymbol *method, const QList<QVariant> & args = QList<QVariant>(),
        PyrSlot *result = 0, bool locked = false );

    void syncRequest( int type, const QVariant& data = QVariant(),
                      QVariant *ret = 0 );
    void asyncRequest( int type, const QVariant& data = QVariant(),
                       QVariant *ret = 0 );

    void doSetProperty( const QString &property, const QVariant& data );
    int doGetProperty( const QString &property, PyrSlot *slot );
    void doSetEventHandler( const EventHandlerData & );

    void customEvent( QEvent * );
    bool eventFilter( QObject * watched, QEvent * event );

    inline void scMethodCallEvent( ScMethodCallEvent * );

    void interpretMouseEvent( QEvent *e, QList<QVariant> &args );
    void interpretKeyEvent( QEvent *e, QList<QVariant> &args );

    QObject *qObject;
    PyrObject *scObject;
    QMap<int,EventHandlerData> eventHandlers;
};

Q_DECLARE_METATYPE( QObjectProxy::PropertyData );
Q_DECLARE_METATYPE( QObjectProxy::EventHandlerData );
Q_DECLARE_METATYPE( QObjectProxy * );

#endif //QC_QOBJECT_PROXY_H