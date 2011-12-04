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

#ifndef _SC_QT_COMMON_H
#define _SC_QT_COMMON_H

#include "debug.h"

#include <QList>
#include <QVariant>
#include <QEvent>
#include <QMutex>
#include <QWaitCondition>

#include <PyrSymbol.h>
#include <PyrObject.h>

#include <pthread.h>

extern pthread_mutex_t gLangMutex;

struct VariantList {
  QList<QVariant> data;
};

Q_DECLARE_METATYPE( VariantList );
Q_DECLARE_METATYPE( PyrObject * );

namespace QtCollider {

  enum MetaType {
    Type_VariantList,
    Type_Count
  };

  enum EventType {
    Event_SCRequest_Input = QEvent::User,
    Event_SCRequest_Sched,
    Event_SCRequest_Quit,
    Event_SCRequest_Recompile,
    Event_ScMethodCall,
    Event_Refresh,
    Event_Proxy_SetProperty,
    Event_Proxy_Destroy,
    Event_Proxy_BringFront,
    Event_Proxy_SetFocus,
    Event_Proxy_SetAlwaysOnTop,
    Event_Proxy_StartDrag
  };

  enum Synchronicity {
    Synchronous,
    Asynchronous
  };

  inline void lockLang()
  {
    qcDebugMsg(2,"locking lang!");
    pthread_mutex_lock (&gLangMutex);
    qcDebugMsg(2,"locked");
  }

  inline void unlockLang()
  {
    pthread_mutex_unlock(&gLangMutex);
    qcDebugMsg(2,"unlocked");
  }

  void runLang (
    PyrObjectHdr *receiver,
    PyrSymbol *method,
    const QList<QVariant> & args = QList<QVariant>(),
    PyrSlot *result = 0 );

  int wrongThreadError ();

  extern PyrSymbol *s_interpretCmdLine;
  extern PyrSymbol *s_interpretPrintCmdLine;
  extern PyrSymbol *s_doFunction;
  extern PyrSymbol *s_doDrawFunc;
  extern PyrSymbol *s_Rect;
  extern PyrSymbol *s_Point;
  extern PyrSymbol *s_Color;
  extern PyrSymbol *s_Size;
  extern PyrSymbol *s_Array;
  extern PyrSymbol *s_FloatArray;
  extern PyrSymbol *s_SymbolArray;
  extern PyrSymbol *s_String;
  extern PyrSymbol *s_QPalette;
  extern PyrSymbol *s_QFont;
  extern PyrSymbol *s_QObject;
  extern PyrSymbol *s_QLayout;
  extern PyrSymbol *s_QTreeViewItem;

#define class_Rect s_Rect->u.classobj
#define class_Point s_Point->u.classobj
#define class_Color s_Color->u.classobj
#define class_Size s_Size->u.classobj
#define class_Array s_Array->u.classobj
#define class_FloatArray s_FloatArray->u.classobj
#define class_SymbolArray s_SymbolArray->u.classobj
#define class_String s_String->u.classobj
#define class_QPalette s_QPalette->u.classobj
#define class_QFont s_QFont->u.classobj
#define class_QObject s_QObject->u.classobj
#define class_QLayout s_QLayout->u.classobj
#define class_QTreeViewItem s_QTreeViewItem->u.classobj

}

#endif //_SC_QT_COMMON_H
