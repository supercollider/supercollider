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

#pragma once

#include "SC_TerminalClient.h"
#include "Common.h"

#include <QObject>
#include <QBasicTimer>
#include <QEvent>

namespace QtCollider {

struct SCRequestEvent : public QEvent {
    SCRequestEvent(QtCollider::EventType type, const QVariant& d = QVariant()): QEvent((QEvent::Type)type), data(d) {}

    QVariant data;
};

class LangClient : public QObject, public SC_TerminalClient {
    Q_OBJECT
public:
    LangClient(const char* name);
    virtual ~LangClient() {}
    virtual void sendSignal(Signal);

protected:
    virtual void commandLoop();
    virtual void daemonLoop();

    virtual void onQuit(int exitCode);
    virtual void onLibraryShutdown();

    virtual void customEvent(QEvent*);
    virtual void timerEvent(QTimerEvent*);

private:
    void tick();
    QBasicTimer appClockTimer;
};

} // namespace QtCollider
