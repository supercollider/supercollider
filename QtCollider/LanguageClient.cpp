/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#include "LanguageClient.h"
#include "QcApplication.h"
#include "QtCollider.h"
#include "QObjectProxy.h"

#include <PyrKernel.h>
#include <PyrLexer.h>

#include <qmath.h>
#include <QWidget>

extern double elapsedTime();

using namespace QtCollider;

LangClient::LangClient(const char* name): SC_TerminalClient(name) {}

void LangClient::commandLoop() {
    int exit_code = QcApplication::instance()->exec();
    SC_TerminalClient::quit(exit_code);
}

void LangClient::daemonLoop() { commandLoop(); }

void LangClient::sendSignal(Signal sig) {
    if (sig == sig_sched) {
        QApplication::postEvent(this, new SCRequestEvent(Event_SCRequest_Tick));
    } else {
        SC_TerminalClient::sendSignal(sig);
        QApplication::postEvent(this, new SCRequestEvent(Event_SCRequest_Work));
    }
}

void LangClient::onQuit(int exitCode) {
    QApplication::postEvent(this, new SCRequestEvent(Event_SCRequest_Quit, exitCode));
}

void LangClient::onLibraryShutdown() {
    QWidgetList windows = QApplication::topLevelWidgets();
    Q_FOREACH (QWidget* w, windows)
        w->hide();
}

void LangClient::customEvent(QEvent* e) {
    int type = e->type();
    switch (type) {
    case Event_SCRequest_Tick:
        tick();

    case Event_SCRequest_Work:
        QApplication::removePostedEvents(this, Event_SCRequest_Work);
        mIoService.poll();
        break;
    case Event_SCRequest_Quit: {
        int code = static_cast<SCRequestEvent*>(e)->data.toInt();
        qcDebugMsg(1, QStringLiteral("Quit requested with code %1").arg(code));
        qApp->exit(code);
        break;
    }
    default:;
    }
}

void LangClient::tick() {
    double secs;
    lock();
    bool haveNext = tickLocked(&secs);
    unlock();

    flush();

    if (haveNext) {
        secs -= elapsedTime();
        secs *= 1000;
        int ti = qMax(0, qCeil(secs));
        qcDebugMsg(2, QStringLiteral("next at %1").arg(ti));
        appClockTimer.start(ti, this);
    } else
        appClockTimer.stop();
}

void LangClient::timerEvent(QTimerEvent* e) {
    if (e->timerId() == appClockTimer.timerId())
        tick();
}
