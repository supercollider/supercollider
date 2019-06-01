/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#pragma once

#include "debug.h"

#include <QList>
#include <QVariant>
#include <QEvent>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>

#include <SCBase.h>
#include <PyrSymbol.h>
#include <PyrObject.h>
#include <PyrSched.h>

Q_DECLARE_METATYPE(PyrObject*);
Q_DECLARE_METATYPE(QVector<double>);
Q_DECLARE_METATYPE(QVector<int>);

namespace QtCollider {

enum EventType {
    Event_SCRequest_Work = QEvent::User,
    Event_SCRequest_Tick,
    Event_SCRequest_Quit,
    Event_ScMethodCall,
    Event_Refresh,
    Event_Proxy_SetProperty,
    Event_Proxy_Destroy,
    Event_Proxy_BringFront,
    Event_Proxy_SetFocus,
    Event_Proxy_SetAlwaysOnTop,
    Event_Proxy_Release
};

enum Synchronicity { Synchronous, Asynchronous };

inline void lockLang() {
    qcDebugMsg(2, "locking lang!");
    gLangMutex.lock();
    qcDebugMsg(2, "locked");
}

inline void unlockLang() {
    gLangMutex.unlock();
    qcDebugMsg(2, "unlocked");
}

void runLang(PyrObjectHdr* receiver, PyrSymbol* method, const QList<QVariant>& args = QList<QVariant>(),
             PyrSlot* result = 0);

int wrongThreadError();

QPalette systemPalette();

#define QC_DO_SYMBOLS                                                                                                  \
    QC_DO_SYMBOL(interpretCmdLine);                                                                                    \
    QC_DO_SYMBOL(interpretPrintCmdLine);                                                                               \
    QC_DO_SYMBOL(doFunction);                                                                                          \
    QC_DO_SYMBOL(doDrawFunc);                                                                                          \
    QC_DO_SYMBOL(prRelease);                                                                                           \
    QC_DO_SYMBOL(Rect);                                                                                                \
    QC_DO_SYMBOL(Point);                                                                                               \
    QC_DO_SYMBOL(Color);                                                                                               \
    QC_DO_SYMBOL(Size);                                                                                                \
    QC_DO_SYMBOL(QPalette);                                                                                            \
    QC_DO_SYMBOL(Font);                                                                                                \
    QC_DO_SYMBOL(QCallback);                                                                                           \
    QC_DO_SYMBOL(WebPage);                                                                                             \
    QC_DO_SYMBOL(QObject);                                                                                             \
    QC_DO_SYMBOL(Layout);                                                                                              \
    QC_DO_SYMBOL(ScrollCanvas);                                                                                        \
    QC_DO_SYMBOL(TreeViewItem);                                                                                        \
    QC_DO_SYMBOL(Gradient);                                                                                            \
    QC_DO_SYMBOL(HiliteGradient);                                                                                      \
    QC_DO_SYMBOL(AbstractMenuAction);                                                                                  \
    QC_DO_SYMBOL(Menu);                                                                                                \
    QC_DO_SYMBOL(View);                                                                                                \
    QC_DO_SYMBOL(Image);

#define QC_DO_SYMBOL(SYM) extern PyrSymbol* sym_##SYM
QC_DO_SYMBOLS
#undef QC_DO_SYMBOL

#define SC_SYM(SYM) QtCollider::sym_##SYM
#define SC_CLASS(SYM) SC_SYM(SYM)->u.classobj

}
