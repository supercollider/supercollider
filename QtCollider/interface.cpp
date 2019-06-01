/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QtCollider.h"
#include "QcApplication.h"
#include "Common.h"
#include "style/ProxyStyle.hpp"
#include "style/style.hpp"
#include "metatype.hpp"

#include <QTimer>
#include <QEventLoop>
#include <QDir>
#include <QWebEngineSettings>

#ifdef Q_WS_X11
#    include <X11/Xlib.h>
#endif

#include <clocale>

namespace QtCollider {
void loadFactories();
}

inline void initResources() { Q_INIT_RESOURCE(resources); }

static QPalette gSystemPalette;
QPalette QtCollider::systemPalette() { return gSystemPalette; }

void QtCollider::init() {
    if (!QApplication::instance()) {
        qcDebugMsg(1, "Initializing QtCollider");

        initResources();

        QtCollider::loadFactories();

        QtCollider::MetaType::initAll();

        QLocale::setDefault(QLocale::c());

#ifdef Q_WS_X11
        XInitThreads();
#endif

#ifdef Q_OS_MAC
        // TODO: this should not be necessary
        QApplication::setAttribute(Qt::AA_PluginApplication, true);
#endif

        static int qcArgc = 1;
        static char qcArg0[] = "SuperCollider";
        static char* qcArgv[1] = { qcArg0 };

        QcApplication* qcApp = new QcApplication(qcArgc, qcArgv);

        qcApp->setQuitOnLastWindowClosed(false);

        // qcApp->setStyle( new QtCollider::Style::StyleImpl( new QPlastiqueStyle ) );

        gSystemPalette = qcApp->palette();

        // Enable javascript localStorage for WebViews
        QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

        // NOTE: Qt may tamper with the C language locale, affecting POSIX number-string conversions.
        // Revert the locale to default:
        setlocale(LC_NUMERIC, "C");
    }
}

int QtCollider::exec(int argc, char** argv) {
    QtCollider::init();
    Q_ASSERT(qApp);
    return qApp->exec();
}
