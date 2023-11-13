/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "main.hpp"
#include "session_manager.hpp"
#include "util/standard_dirs.hpp"
#include "../widgets/main_window.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <QStyleFactory>

#include "util/HelpBrowserWebSocketServices.hpp"

#include <iostream>
#include <string>

using namespace ScIDE;

static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)> [file1.scd file2.scd ...]\n"
              << "Options:\n"
              << "\t-h, --help\tShow this help message\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    // In order to scale the UI properly on Windows with display scaling like 125% or 150%
    // we need to disable scale factor rounding
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);

    QStringList arguments(QApplication::arguments());
    arguments.pop_front(); // application path

    // Pass files to existing instance and quit

    SingleInstanceGuard guard;
    if (guard.tryConnect(arguments))
        return 0;

    // Set up translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QString ideTranslationPath = standardDirectory(ScResourceDir) + "/translations";

    bool translationLoaded;

    // Load fallback translator that only handles plural forms in English
    QTranslator fallbackTranslator;
    translationLoaded = fallbackTranslator.load("scide", ideTranslationPath);
    app.installTranslator(&fallbackTranslator);
    if (!translationLoaded)
        qWarning("scide warning: Failed to load fallback translation file.");

    // Load translator for locale
    const QLocale locale;
    QTranslator scideTranslator;
    scideTranslator.load(locale, "scide", "_", ideTranslationPath);
    app.installTranslator(&scideTranslator);

    // Force Fusion style to appear consistently on all platforms.
    app.setStyle(QStyleFactory::create("Fusion"));

    // Palette must be set before style, for consistent application.
    Main* main = Main::instance();
    main->setAppPaletteFromSettings();

    // Install style proxy.
    app.setStyle(new ScIDE::Style(app.style()));

    // Go...
    MainWindow* win = new MainWindow(main);

    // NOTE: load session after GUI is created, so that GUI can respond
    Settings::Manager* settings = main->settings();
    SessionManager* sessions = main->sessionManager();

    // NOTE: window has to be shown before restoring its geometry,
    // or else restoring maximized state will fail, if it has ever before
    // been saved un-maximized.
    win->show();

    QString startSessionName = settings->value("IDE/startWithSession").toString();
    if (startSessionName == "last") {
        QString lastSession = sessions->lastSession();
        if (!lastSession.isEmpty()) {
            sessions->openSession(lastSession);
        }
    } else if (!startSessionName.isEmpty()) {
        sessions->openSession(startSessionName);
    }

    if (!sessions->currentSession()) {
        win->restoreWindowState();
        sessions->newSession();
    }

    foreach (QString argument, arguments) {
        main->documentManager()->open(argument);
    }

    win->restoreDocuments();

    bool startInterpreter = settings->value("IDE/interpreter/autoStart").toBool();
    if (startInterpreter)
        main->scProcess()->startLanguage();

#ifdef SC_USE_QTWEBENGINE
    HelpBrowserWebSocketServices hbServices(win->helpBrowserDocklet()->browser());
#endif
    app.exec();
}
