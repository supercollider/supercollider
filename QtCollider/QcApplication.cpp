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

#include "QcApplication.h"
#include "widgets/QcTreeWidget.h"

#include <PyrLexer.h>
#include <VMGlobals.h>
#include <PyrKernel.h>
#include <PyrSlot.h>
#include <GC.h>
#include "SC_Filesystem.hpp"

#include <QThread>
#include <QFileOpenEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QMenuBar>
#include <QSharedPointer>

#include "hacks/hacks_mac.hpp"

#ifdef Q_OS_MAC
#    include "../../common/SC_Apple.hpp"
#endif

extern bool compiledOK;

QcApplication* QcApplication::_instance = 0;
QMutex QcApplication::_mutex;

#ifdef Q_WS_X11
#    include <X11/Xlib.h>
#endif


/* on x11, we need to check, if we can actually connect to the X server */
static bool QtColliderUseGui(void) {
#ifdef Q_WS_X11
    Display* dpy = XOpenDisplay(NULL);
    if (!dpy)
        return false;
    XCloseDisplay(dpy);
    return true;
#else
    return true;
#endif
}

// undefine some interfering X11 definitions
#undef KeyPress

bool QcApplication::_systemHasMouseWheel = false;

QcApplication::QcApplication(int& argc, char** argv): QApplication(argc, argv, QtColliderUseGui()) {
    _mutex.lock();
    _instance = this;
    _mutex.unlock();
    this->setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef Q_OS_MAC
    QtCollider::Mac::DisableAutomaticWindowTabbing();
#endif

    if (QtColliderUseGui()) { // avoid a crash on linux, if x is not available
        QIcon icon;
        icon.addFile(":/icons/sc-cube-128");
        icon.addFile(":/icons/sc-cube-48");
        icon.addFile(":/icons/sc-cube-32");
        icon.addFile(":/icons/sc-cube-16");
        setWindowIcon(icon);
        _mainMenu = QSharedPointer<QMenuBar>(new QMenuBar(0));
    }

#ifdef Q_OS_MAC
    // On Mac, we may need to disable "App Nap", so we aren't put to sleep unexpectedly
    SC::Apple::disableAppNap();
#endif

    _handleCmdPeriod = SC_Filesystem::instance().getIdeName() != "scapp";
}

QcApplication::~QcApplication() {
    _mutex.lock();
    _instance = 0;
    _mutex.unlock();
}

bool QcApplication::compareThread() { return gMainVMGlobals->canCallOS; }

void QcApplication::interpret(const QString& str, bool print) {
    QtCollider::lockLang();
    if (compiledOK) {
        VMGlobals* g = gMainVMGlobals;

        PyrString* strObj = newPyrString(g->gc, str.toStdString().c_str(), 0, true);

        SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strObj);
        g->gc->GCWriteNew(slotRawObject(&g->process->interpreter),
                          strObj); // we know strObj is white so we can use GCWriteNew

        runLibrary(print ? SC_SYM(interpretPrintCmdLine) : SC_SYM(interpretCmdLine));
    }
    QtCollider::unlockLang();
}

bool QcApplication::event(QEvent* event) {
    switch (event->type()) {
    case QEvent::FileOpen: {
        // open the file dragged onto the application icon on Mac
        QFileOpenEvent* fe = static_cast<QFileOpenEvent*>(event);
        interpret(QStringLiteral("Document.open(\"%1\")").arg(fe->file()), false);
        event->accept();
        return true;
    }
    default:
        break;
    }

    return QApplication::event(event);
}

bool QcApplication::notify(QObject* object, QEvent* event) {
    switch (event->type()) {
    case QEvent::KeyPress: {
        QKeyEvent* kevent = static_cast<QKeyEvent*>(event);
        if (_handleCmdPeriod && (kevent->key() == Qt::Key_Period) && (kevent->modifiers() & Qt::ControlModifier)) {
            static QString cmdPeriodCommand("CmdPeriod.run");
            interpret(cmdPeriodCommand, false);
        }
        break;
    }
    case QEvent::Wheel: {
        _systemHasMouseWheel = true;
        break;
    }
    default:
        break;
    }

    bool result = QApplication::notify(object, event);

#ifdef Q_OS_MAC
    // XXX Explicitly accept all handled events so they don't propagate outside the application.
    // This is a hack; for a not-fully-understood reason Qt past 5.7 sends these events to the
    // native window if they aren't accepted here. This caused issue #4058. Accepting them here
    // seems to solve the problem, but might cause other issues since it is a heavy-handed way
    // of doing this. TODO - solve more elegantly
    if (result)
        event->accept();
#endif
    return result;
}
