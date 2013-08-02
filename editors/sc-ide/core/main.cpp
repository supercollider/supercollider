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
#include "../widgets/main_window.hpp"
#include "../widgets/help_browser.hpp"
#include "../widgets/lookup_dialog.hpp"
#include "../widgets/code_editor/highlighter.hpp"
#include "../widgets/style/style.hpp"

#include "SC_DirUtils.h"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

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

    char resourcePath[PATH_MAX];
    sc_GetResourceDirectory(resourcePath, PATH_MAX);
    QString ideTranslationPath = QString(resourcePath) + "/translations";

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

    // Set up style

    app.setStyle( new ScIDE::Style(app.style()) );

    // Go...

    Main * main = Main::instance();

    MainWindow *win = new MainWindow(main);

    // NOTE: load session after GUI is created, so that GUI can respond
    Settings::Manager *settings = main->settings();
    SessionManager *sessions = main->sessionManager();

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
            QString serverName = QString("SuperColliderIDE_Singleton_%1").arg(socketID);
            QSharedPointer<QLocalSocket> socket (new QLocalSocket(this));
            socket->connectToServer(serverName);

            QStringList canonicalArguments;
            foreach (QString path, arguments) {
                QFileInfo info(path);
                canonicalArguments << info.canonicalFilePath();
            }

            if (socket->waitForConnected(200)) {
                QDataStream stream(socket.data());
                stream.setVersion(QDataStream::Qt_4_6);

                stream << QString("open");
                stream << canonicalArguments;
                if (!socket->waitForBytesWritten(300))
                    qWarning("SingleInstanceGuard: writing data to another IDE instance timed out");

                return true;
            }
        }
    }

    mIpcServer = new QLocalServer(this);
    for (int socketID = 0; socketID != maxNumberOfInstances; ++socketID) {
        QString serverName = QString("SuperColliderIDE_Singleton_%1").arg(socketID);

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
    QByteArray ipcData = mIpcSocket->readAll();

    QBuffer receivedData ( &ipcData );
    receivedData.open ( QIODevice::ReadOnly );

    QDataStream in ( &receivedData );
    in.setVersion ( QDataStream::Qt_4_6 );
    QString id;
    in >> id;
    if ( in.status() != QDataStream::Ok )
        return;

    QStringList message;
    in >> message;
    if ( in.status() != QDataStream::Ok )
        return;

    if (id == QString("open")) {
        foreach (QString path, message)
            Main::documentManager()->open(path);
    }
}


static QString getSettingsFile()
{
    char config_dir[PATH_MAX];
    sc_GetUserConfigDirectory(config_dir, PATH_MAX);
    return QString(config_dir) + SC_PATH_DELIMITER + "sc_ide_conf.yaml";
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

    connect(mScProcess, SIGNAL(response(QString,QString)), this, SLOT(onScLangResponse(QString,QString)));
    connect(mDocManager, SIGNAL(closed(Document*)), this, SLOT(onClose(Document*)));
    connect(mDocManager, SIGNAL(opened(Document*, int, int)), this, SLOT(onOpen(Document*, int, int)));

    qApp->installEventFilter(this);
}

void Main::quit() {
    mSessionManager->saveSession();
    storeSettings();
    QApplication::quit();
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

void Main::onOpen(Document* doc, int cursorPosition, int selectionLength)
{
    QString command = QString("ScIDEDocument.syncFromIDE(\'%1\', \'%2\', %3, %4)").arg(doc->id().constData()).arg(doc->title()).arg(doc->textAsSCArrayOfCharCodes(0, -1)).arg(doc->isModified());
    mScProcess->evaluateCode ( command, true );
}

void Main::onClose(Document* doc)
{
    QString command = QString("ScIDEDocument.findByQUuid(\'%1\').closed").arg(doc->id().constData());
    mScProcess->evaluateCode ( command, true );
}

void Main::onScLangResponse( const QString & selector, const QString & data )
{
    static QString openFileSelector("openFile");
    static QString requestDocListSelector("requestDocumentList");
	static QString newDocSelector("newDocument");
    static QString getDocTextSelector("getDocumentText");
    static QString setDocTextSelector("setDocumentText");
    static QString setCurrentDocSelector("setCurrentDocument");
    static QString closeDocSelector("closeDocument");
    static QString setDocTitleSelector("setDocumentTitle");
	
    if (selector == openFileSelector)
        handleOpenFileScRequest(data);
    
    if (selector == requestDocListSelector)
        handleDocListScRequest();
	
	if (selector == newDocSelector)
        handleNewDocScRequest(data);
    
    if (selector == getDocTextSelector)
        handleGetDocTextScRequest(data);
    
    if (selector == setDocTextSelector)
        handleSetDocTextScRequest(data);
    
    if (selector == setCurrentDocSelector)
        handleSetCurrentDocScRequest(data);
    
    if (selector == closeDocSelector)
        handleCloseDocScRequest(data);
    
    if (selector == setDocTitleSelector)
        handleSetDocTitleScRequest(data);
}

void Main::handleOpenFileScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;

        std::string path;
        bool success = doc[0].Read(path);
        if (!success)
            return;

        int position = 0;
        doc[1].Read(position);

        int selectionLength = 0;
        doc[2].Read(selectionLength);

        mDocManager->open(QString(path.c_str()), position, selectionLength);
    }
}

void Main::handleDocListScRequest()
{
    QList<Document*> docs = mDocManager->documents();
    QList<Document*>::Iterator it;
    QString command = QString("ScIDEDocument.syncDocs([");
    for (it = docs.begin(); it != docs.end(); ++it) {
        Document * doc = *it;
        QString docData = QString("[\'%1\', \'%2\', %3, %4],").arg(doc->id().constData()).arg(doc->title()).arg(doc->textAsSCArrayOfCharCodes(0, -1)).arg(doc->isModified());
        command = command.append(docData);
    }
    command = command.append("]);");
    mScProcess->evaluateCode ( command, true );
}

void Main::handleNewDocScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string title;
        bool success = doc[0].Read(title);
        if (!success)
            return;
		
		std::string initString;
        success = doc[1].Read(initString);
        if (!success)
            return;
		
		std::string quuid;
        success = doc[2].Read(quuid);
        if (!success)
            return;
		
        mDocManager->create(QByteArray(quuid.c_str()), QString(title.c_str()), QString(initString.c_str()));
    }
}

void Main::handleGetDocTextScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string quuid;
        bool success = doc[0].Read(quuid);
        if (!success)
            return;
        
        std::string funcID;
        success = doc[1].Read(funcID);
        if (!success)
            return;
        
        int start;
        success = doc[2].Read(start);
        if (!success)
            return;
        
        int range;
        success = doc[3].Read(range);
        if (!success)
            return;
        
        QByteArray docID = QByteArray(quuid.c_str());
        
        Document *document = mDocManager->getDocByID(docID);
        if(document){
            QString docText = document->textAsSCArrayOfCharCodes(start, range);
            
            QString command = QString("ScIDEDocument.executeAsyncResponse(\'%1\', %2.asAscii)").arg(QString(funcID.c_str()), docText);
            mScProcess->evaluateCode ( command, true );
        }
        
    }
}

void Main::handleSetDocTextScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string quuid;
        bool success = doc[0].Read(quuid);
        if (!success)
            return;
        
        std::string funcID;
        success = doc[1].Read(funcID);
        if (!success)
            return;
        
        std::string text;
        success = doc[2].Read(text);
        if (!success)
            return;
        
        int start;
        success = doc[3].Read(start);
        if (!success)
            return;
        
        int range;
        success = doc[4].Read(range);
        if (!success)
            return;
        
        QByteArray docID = QByteArray(quuid.c_str());
        
        Document *document = mDocManager->getDocByID(docID);
        if(document){
            document->setTextInRange(QString(text.c_str()), start, range);
        
            QString command = QString("ScIDEDocument.executeAsyncResponse(\'%1\')").arg(QString(funcID.c_str()));
            mScProcess->evaluateCode ( command, true );
        } 
        
    }
}

void Main::handleSetCurrentDocScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string quuid;
        bool success = doc[0].Read(quuid);
        if (!success)
            return;
        
        QByteArray docID = QByteArray(quuid.c_str());
        
        Document *document = mDocManager->getDocByID(docID);
        if(document){
            mDocManager->showDocument(document);
        }
    }

}

void Main::handleCloseDocScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string quuid;
        bool success = doc[0].Read(quuid);
        if (!success)
            return;
        
        QByteArray docID = QByteArray(quuid.c_str());
        
        Document *document = mDocManager->getDocByID(docID);
        if(document){
            mDocManager->close(document);
        }
    }
}

void Main::handleSetDocTitleScRequest( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);
	
    YAML::Node doc;
    if (parser.GetNextDocument(doc)) {
        if (doc.Type() != YAML::NodeType::Sequence)
            return;
		
        std::string quuid;
        bool success = doc[0].Read(quuid);
        if (!success)
            return;
        
        std::string title;
        success = doc[1].Read(title);
        if (!success)
            return;
        
        QByteArray docID = QByteArray(quuid.c_str());
        
        Document *document = mDocManager->getDocByID(docID);
        if(document){
            mDocManager->changeDocumentTitle(document, QString(title.c_str()));
        }
        
    }

}