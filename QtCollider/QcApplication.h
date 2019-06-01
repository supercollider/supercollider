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

#include "Common.h"

#include <QApplication>
#include <QEventLoop>
#include <QMutex>
#include <QMenuBar>

namespace QtCollider {

class EventProcessor : public QObject {
public:
    void work() {
        QApplication::postEvent(this, new QEvent(QEvent::User));
        _loop.exec();
    }
    void customEvent(QEvent* e) { _loop.exit(); }

private:
    QEventLoop _loop;
};

} // namespace QtCollider

class QcApplication : public QApplication {
    Q_OBJECT

public:
    QcApplication(int& argc, char** argv);
    virtual ~QcApplication();
    static bool compareThread(); // NOTE: language must be locked
    static void processEvents() {
        if (_instance)
            _instance->_eventProc.work();
    }
    static inline bool SystemHasMouseWheel() { return _systemHasMouseWheel; }
    static QMenuBar* getMainMenu() { return _instance->_mainMenu.data(); }

public Q_SLOTS:
    void interpret(const QString& code, bool printResult = true);

protected:
    virtual bool event(QEvent*);
    virtual bool notify(QObject*, QEvent*);

private:
    QSharedPointer<QMenuBar> _mainMenu;

    QtCollider::EventProcessor _eventProc;

    static QMutex _mutex;
    static QcApplication* _instance;
    static bool _systemHasMouseWheel;

    bool _handleCmdPeriod;
};
