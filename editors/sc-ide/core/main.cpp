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
#include "settings/manager.hpp"
#include "session_manager.hpp"
#include "util/standard_dirs.hpp"
#include "../widgets/main_window.hpp"
#include "../widgets/help_browser.hpp"
#include "../widgets/lookup_dialog.hpp"
#include "../widgets/code_editor/highlighter.hpp"
#include "../../../QtCollider/hacks/hacks_mac.hpp"
#include "../primitives/localsocket_utils.hpp"

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QDataStream>
#include <QDir>
#include <QFileOpenEvent>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>

using namespace ScIDE;

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

    QStringList arguments (QApplication::arguments());
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
    translationLoaded = fallbackTranslator.load( "scide", ideTranslationPath );
    app.installTranslator(&fallbackTranslator);
    if (!translationLoaded)
        qWarning("scide warning: Failed to load fallback translation file.");

    // Load translator for locale
    QString ideTranslationFile = "scide_" + QLocale::system().name();
    QTranslator scideTranslator;
    scideTranslator.load( ideTranslationFile, ideTranslationPath );
    app.installTranslator(&scideTranslator);

    // Load settings so that we can copy editor color values into the UI colors,
    // which must be set before window is created.
    Main *main = Main::instance();

    // Palette must be set before style, for consistent application.
    main->setAppPaletteFromSettings();

   // Set up style
    app.setStyle( new ScIDE::Style(app.style()) );

    // Go...
    MainWindow *win = new MainWindow(main);

    app.setWindowIcon(QIcon("qrc:///icons/sc-ide-svg"));

    // NOTE: load session after GUI is created, so that GUI can respond
    SessionManager *sessions = main->sessionManager();

    // NOTE: window has to be shown before restoring its geometry,
    // or else restoring maximized state will fail, if it has ever before
    // been saved un-maximized.
    win->show();

    Settings::Manager *settings = main->settings();
    QString startSessionName = settings->value("IDE/startWithSession").toString();
    if (startSessionName == "last") {
        QString lastSession = sessions->lastSession();
        if (!lastSession.isEmpty()) {
            sessions->openSession(lastSession);
        }
    }
    else if (!startSessionName.isEmpty()) {
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

    return app.exec();
}


bool SingleInstanceGuard::tryConnect(QStringList const & arguments)
{
    const int maxNumberOfInstances = 128;
    if (!arguments.empty()) {
        for (int socketID = 0; socketID != maxNumberOfInstances; ++socketID) {
            QString serverName = QStringLiteral("SuperColliderIDE_Singleton_%1").arg(socketID);
            QSharedPointer<QLocalSocket> socket (new QLocalSocket(this));
            socket->connectToServer(serverName);

            QStringList canonicalArguments;
            foreach (QString path, arguments) {
                QFileInfo info(path);
                canonicalArguments << info.canonicalFilePath();
            }

            if (socket->waitForConnected(200)) {
                sendSelectorAndData(socket.data(), QStringLiteral("open"), canonicalArguments);
                if (!socket->waitForBytesWritten(300))
                    qWarning("SingleInstanceGuard: writing data to another IDE instance timed out");

                return true;
            }
        }
    }

    mIpcServer = new QLocalServer(this);
    for (int socketID = 0; socketID != maxNumberOfInstances; ++socketID) {
        QString serverName = QStringLiteral("SuperColliderIDE_Singleton_%1").arg(socketID);

        bool listening = mIpcServer->listen(serverName);
        if (listening) {
            connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));
            return false;
        }
    }
    return false;
}

void SingleInstanceGuard::onIpcData()
{
    mIpcData.append(mIpcSocket->readAll());

    // After we have put the data in the buffer, process it
    int avail = mIpcData.length();
    do{
        if (mReadSize == 0 && avail > 4){
            mReadSize = ArrayToInt(mIpcData.left(4));
            mIpcData.remove(0, 4);
            avail -= 4;
        }

        if (mReadSize > 0 && avail >= mReadSize){
            QByteArray baReceived(mIpcData.left(mReadSize));
            mIpcData.remove(0, mReadSize);
            mReadSize = 0;

            QDataStream in(baReceived);
            in.setVersion(QDataStream::Qt_4_6);
            QString selector;
            in >> selector;
            if (in.status() != QDataStream::Ok)
                return;

            QStringList message;
            in >> message;
            if (in.status() != QDataStream::Ok)
                return;

            if (selector == QStringLiteral("open")) {
                foreach(QString path, message)
                    Main::documentManager()->open(path);
            }
        }
    } while ((mReadSize == 0 && avail > 4) || (mReadSize > 0 && avail > mReadSize));
}


static inline QString getSettingsFile()
{
    return standardDirectory(ScConfigUserDir) + "/sc_ide_conf.yaml";
}

// NOTE: mSettings must be the first to initialize,
// because other members use it!

Main::Main(void) :
    mSettings( new Settings::Manager( getSettingsFile(), this ) ),
    mScProcess( new ScProcess(mSettings, this) ),
    mScServer( new ScServer(mScProcess, mSettings, this) ),
    mDocManager( new DocumentManager(this, mSettings) ),
    mSessionManager( new SessionManager(mDocManager, this) )
{
    new SyntaxHighlighterGlobals(this, mSettings);

#ifdef Q_OS_MAC
    QtCollider::Mac::DisableAutomaticWindowTabbing();
#endif

    connect(mScProcess, SIGNAL(response(QString,QString)),
            mDocManager, SLOT(handleScLangMessage(QString,QString)));

    qApp->installEventFilter(this);
    qApp->installNativeEventFilter(this);
}

void Main::quit() {
    mSessionManager->saveSession();
    storeSettings();
    mScProcess->stopLanguage();
    QApplication::quit();
}

void Main::setAppPaletteFromSettings() {
    const QTextCharFormat *format = &mSettings->getThemeVal("text");
    QBrush text_bg = format->background();
    QBrush text_fg = format->foreground();

    QBrush mid_bg, dark_bg;
    // If the text is darker than the background we we create contrast colors
    // that are darker in value than the background, otherwise we do the
    // opposite.
    if (text_fg.color().value() < text_bg.color().value()) {
        mid_bg = QBrush(text_bg.color().darker(200));
        dark_bg = QBrush(text_bg.color().darker(300));
    } else {
        // QtColor::lighter() multiplies the value of the color by the provided
        // percentage factor, so if the value is zero it has no effect. In that
        // case we darken the foreground color, since hue information is lost
        // in maximum black.
        if (text_bg.color().value() > 0) {
            mid_bg = QBrush(text_bg.color().lighter(200));
            dark_bg = QBrush(text_bg.color().lighter(300));
        } else {
            mid_bg = QBrush(text_fg.color().darker(500));
            dark_bg = QBrush(text_fg.color().darker(1000));
        }
    }

    QBrush clamp_fg = text_fg;

#ifdef Q_OS_MACOS
    // On OS X Qt uses Cocoa-style native buttons, which are currently rendered
    // with a hard-coded white background. If we are using a light text on dark
    // background themes this means the text can be washed out and not readable
    // on the controls. For this platform we therefore clamp the text color to
    // be at most half value.
    if (clamp_fg.color().value() > 127) {
        int hue = clamp_fg.color().hue();
        int sat = clamp_fg.color().saturation();
        clamp_fg = QBrush(QColor::fromHsv(hue, sat, 127));
    }
#endif

    qApp->setPalette(QPalette(
        clamp_fg,      // windowText - text color for active and inactive tab
                       //    bars, and most non-bold text in controls, including
                       //    the selector buttons at the top of the editor
                       //    settings tab.
        text_bg,       // button - background color of active tab.
        text_fg,       // light - no observed use in current ui.
        dark_bg,       // dark - color for dividers around tabs, the background
                       //     color around the Auto Scroll button, and selection
                       //     background color in the settings tab menu.
        mid_bg,        // mid - background color for the help and log dock bars
                       //     as well as inactive tabs.
        clamp_fg,      // text - text color for home and autoscroll dock bars,
                       //     tab selector names in settings, and most buttons.
        text_fg,       // bright_text - no observed use in current UI.
        text_bg,       // base - no observed use in current UI.
        mid_bg         // window - background color of settings window and the color
                       //     of the frame drawn around the editor widgets.
    ));
}

bool Main::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
    {
        // open the file dragged onto the application icon on Mac
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent*>(event);
        mDocManager->open(openEvent->file());
        return true;
    }

    case QEvent::MouseMove:
        QApplication::restoreOverrideCursor();
        break;

    default:
        break;
    }

    return QObject::eventFilter(object, event);
}

bool Main::nativeEventFilter(const QByteArray &, void * message, long *)
{
    bool result = false;

#ifdef Q_OS_MAC
    if (QtCollider::Mac::IsCmdPeriodKeyDown(reinterpret_cast<void *>(message)))
    {
//        QKeyEvent event(QEvent::KeyPress, Qt::Key_Period, Qt::ControlModifier, ".");
//        QApplication::sendEvent(this, &event);
        mScProcess->stopMain(); // we completely bypass the shortcut handling
        result = true;
    }
    else if (QtCollider::Mac::IsCmdPeriodKeyUp(reinterpret_cast<void *>(message)))
    {
        result = true;
    }
#endif

    return result;
}

bool Main::openDocumentation(const QString & string)
{
    QString symbol = string.trimmed();
    if (symbol.isEmpty())
        return false;

    HelpBrowserDocklet *helpDock = MainWindow::instance()->helpBrowserDocklet();
    helpDock->browser()->gotoHelpFor(symbol);
    helpDock->focus();
    return true;
}

bool Main::openDocumentationForMethod(const QString & className, const QString & methodName)
{
    HelpBrowserDocklet *helpDock = MainWindow::instance()->helpBrowserDocklet();
    helpDock->browser()->gotoHelpForMethod(className, methodName);
    helpDock->focus();
    return true;
}

void Main::openDefinition(const QString &string, QWidget * parent)
{
    QString definitionString = string.trimmed();

    LookupDialog dialog(parent);
    if (!definitionString.isEmpty())
        dialog.query(definitionString);
    dialog.exec();
}

void Main::openCommandLine(const QString &string)
{
    MainWindow::instance()->showCmdLine(string);
}

void Main::findReferences(const QString &string, QWidget * parent)
{
    QString definitionString = string.trimmed();

    ReferencesDialog dialog(parent);
    if (!definitionString.isEmpty())
        dialog.query(definitionString);
    dialog.exec();
}

