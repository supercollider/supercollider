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

#ifndef SCIDE_WIDGETS_MAIN_WINDOW_HPP_INCLUDED
#define SCIDE_WIDGETS_MAIN_WINDOW_HPP_INCLUDED

#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QSignalMapper>
#include <QStatusBar>
#include <QList>

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>

#include "util/status_box.hpp"
#include "multi_editor.hpp"
#include "util/docklet.hpp"
#include "doc_list.hpp"
#include "../core/main.hpp"
#include "editor_box.hpp"


namespace ScIDE {

class Main;
class MultiEditor;
class PostDocklet;
class DocumentsDocklet;
class HelpBrowserDocklet;
class Document;
class DocumentsDialog;
struct Session;
class ClockStatusBox;
class ScServer;
class ScProcess;
class GenericCodeEditor;
class DocumentListWidget;
class SubWindow;

namespace Settings { class Manager; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum ActionRole {
        // File
        Quit = 0,
        DocNew,
        DocOpen,
        DocOpenStartup,
        DocOpenSupportDir,
        DocSave,
        DocSaveAs,
        DocSaveAsExtension,
        DocSaveAll,
        DocCloseAll,
        DocReload,
        ClearRecentDocs,

        // Sessions
        NewSession,
        SaveSessionAs,
        ManageSessions,
        OpenSessionSwitchDialog,

        // Edit
        Find,
        Replace,

        // View
        ShowCmdLine,
        CmdLineForCursor,
        ShowGoToLineTool,
        CloseToolBox,
        ShowFullScreen,
        FocusPostWindow,

        // Settings
        ShowSettings,

        // Language
        LookupImplementation,
        LookupImplementationForCursor,
        LookupReferences,
        LookupReferencesForCursor,

        // Help
        Help,
        HelpAboutIDE,
        ReportABug,
        LookupDocumentationForCursor,
        LookupDocumentation,
        ShowAbout,
        ShowAboutQT,

        ActionCount
    };

    typedef QList< MultiEditor * > History;

    explicit MainWindow(Main *);

    QAction *action( ActionRole );

    bool quit();

    void saveWindowState();
    void restoreWindowState();
    void focusCodeEditor();
    bool promptSaveDocs();

    HelpBrowserDocklet * helpBrowserDocklet() { return mHelpBrowserDocklet; }
    PostDocklet * postDocklet() { return mPostDocklet; }

    static MainWindow *instance() { return mInstance; }
    Settings::Manager *setting();

    static bool close( Document * );
    static bool save( Document *, bool forceChoose = false, bool saveInExtensionFolder = false );
    static bool reload( Document * );

    void restoreDocuments();
    MultiEditor * currentMultiEditor();
    QMenuBar *createMenus();

    DocumentsDocklet *docDocklet() { return mDocumentsDocklet; }    
    int docDockletWidth() { return mDocumentsDocklet->list()->width(); }

public Q_SLOTS:
    void newSession();
    void saveCurrentSessionAs();
    void openSessionsDialog();

    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void saveDocumentAsExtension();
    void saveAllDocuments();
    void reloadDocument();
    void closeDocument();
    void closeAllDocuments();

    void showCmdLine();
    void showCmdLine( const QString & );
    void showFindTool();
    void showReplaceTool();
    void showGoToLineTool();
    void hideToolBox();

    void showSettings();

signals:
    void evaluateCode( const QString &, bool silent = true );
    void reloadDocumentLists( QList<Document*> );
    void tabsOrderChanged(int, int);
    void documentDockletUndocked();
    void currentEditorChanged( Document * );

public Q_SLOTS:
    void showStatusMessage( QString const & string );
    void newWindow();
    void setCurrentEditor(MultiEditor *);

private Q_SLOTS:
    void openStartupFile();
    void openUserSupportDirectory();

    void switchSession( Session *session );
    void saveSession( Session *session );
    void onInterpreterStateChanged( QProcess::ProcessState );
    void onQuit();
    void onCurrentDocumentChanged( Document * );
    void onDocumentChangedExternally( Document * );
    void onDocDialogFinished();
    void updateRecentDocsMenu();
    void onOpenRecentDocument( QAction * );
    void onOpenSessionAction( QAction * );
    void updateWindowTitle();
    void toggleFullScreen();
    void lookupImplementation();
    void lookupImplementationForCursor();
    void lookupReferences();
    void lookupReferencesForCursor();
    void openHelp();
    void openHelpAboutIDE();
    void doBugReport();
    void lookupDocumentationForCursor();
    void lookupDocumentation();
    void applySettings( Settings::Manager * );
    void storeSettings( Settings::Manager * );
    void showSwitchSessionDialog();
    void showAbout();
    void showAboutQT();
    void cmdLineForCursor();
    void reloadAllLists( QList<Document*> );
    void reloadAllLists( int, int );
    void onDocumentDockletUndocked( bool );
    void setCurrent(Document *);
    
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void dragEnterEvent( QDragEnterEvent * );
    virtual void dropEvent( QDropEvent * );
    virtual bool eventFilter( QObject *, QEvent * );

private:
    void createActions();
    template <class T> void saveWindowState(T * settings);
    template <class T> void restoreWindowState(T * settings);
    void updateSessionsMenu();
    void updateClockWidget( bool isFullScreen );
    void openSession( QString const & sessionName );
    bool checkFileExtension( const QString & fpath );
    void toggleInterpreterActions( bool enabled);
    void applyCursorBlinkingSettings( Settings::Manager * );
    QString documentOpenPath();
    QString documentSavePath( Document * ) const;
    void createDocumentConnections();

    Main *mMain;

    QAction * mActions[ActionCount];
    QMenu * mRecentDocsMenu;
    QMenu * mSessionsMenu;

    MultiEditor *mEditors;
    History mEditorList;

    // Status bar
    QStatusBar  *mStatusBar;
    QMenuBar *mMenu;
    StatusBox *mLangStatus;
    StatusBox *mServerStatus;
    ClockStatusBox *mClockLabel;

    // Docks
    PostDocklet * mPostDocklet;
    DocumentsDocklet *mDocumentsDocklet;
    HelpBrowserDocklet *mHelpBrowserDocklet;

    QSignalMapper mCodeEvalMapper;
    DocumentsDialog * mDocDialog;

    QString mLastDocumentSavePath;

    static MainWindow *mInstance;
};

class ClockStatusBox : public StatusLabel
{
public:
    ClockStatusBox (QWidget * parent = 0);
    ~ClockStatusBox();

private:
    void timerEvent(QTimerEvent *);
    void updateTime();
    int mTimerId;
};

class SubWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindow( QWidget * parent = 0 ):
    QWidget(parent),
    sEditors(new MultiEditor(Main::instance()))
    {
        window = new QWidget;
        window->resize(640, 480);
        window->show();
        window->setWindowTitle(
            QApplication::translate("toplevel", "Code Editor")
        );

        main = MainWindow::instance();

        QVBoxLayout *windowLayout = new QVBoxLayout;
        windowLayout->setContentsMargins(0,0,0,0);
        windowLayout->setSpacing(0);

#ifndef Q_OS_MAC
        QMenuBar *newMenu = main->createMenus();
        newMenu->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Fixed );
        windowLayout->addWidget(newMenu);
#endif

        QVBoxLayout *center_box = new QVBoxLayout;
        center_box->setContentsMargins(0,0,0,0);
        center_box->setSpacing(0);
        center_box->addWidget(sEditors);

        sEditorsDocklet = new QWidget;
        sEditorsDocklet->setLayout(center_box);

        documentsDocklet();

        QSplitter * editors_layout = new QSplitter;
        editors_layout->setOrientation(Qt::Horizontal);
        editors_layout->addWidget(sDocumentsDocklet);
        editors_layout->addWidget(sEditorsDocklet);
        QList<int> layoutInitialWidths;
        layoutInitialWidths.append(main->docDockletWidth());
        int editorWidth = editors_layout->size().width() - main->docDockletWidth();
        layoutInitialWidths.append(editorWidth);
        editors_layout->setSizes(layoutInitialWidths);

        windowLayout->addWidget(editors_layout);
        window->setLayout(windowLayout);

        createDocumentConnections();

        connect(main, SIGNAL(documentDockletUndocked()),
                this, SLOT(onDocumentDockletUndocked()));

        // loadDocuments
        sDocumentListWidget->populateList(main->docDocklet()->list()->listDocuments());
        sEditors->updateTabsOrder(sDocumentListWidget->listDocuments());

        sDocumentsDocklet->setVisible(main->docDocklet()->isVisible());
        onDocumentDockletUndocked();
    }

    MultiEditor * editor() { return sEditors; }
    
public Q_SLOTS:

    void onDocumentDockletUndocked()
    {
        setDocDockletVisibility();
        if( main->docDocklet()->isDetached() || main->docDocklet()->dockWidget()->isFloating() ) {
            connect(sEditors, SIGNAL(currentDocumentChanged(Document*)),
                    main->docDocklet()->list(), SLOT(setCurrent(Document*)));
            connect(main, SIGNAL(currentEditorChanged(Document*)),
                    main->docDocklet()->list(), SLOT(setCurrent(Document*)));

        } else {
            disconnect(sEditors, SIGNAL(currentDocumentChanged(Document*)),
                    main->docDocklet()->list(), SLOT(setCurrent(Document*)));
            disconnect(main, SIGNAL(currentEditorChanged(Document*)),
                    main->docDocklet()->list(), SLOT(setCurrent(Document*)));
        }
    }

    void setDocDockletVisibility()
    {
        if (main->docDocklet()->isDetached() || main->docDocklet()->dockWidget()->isFloating()) {
            sDocumentsDocklet->hide();
        } else {
            sDocumentsDocklet->setVisible(main->docDocklet()->isVisible());
        }
    }

private:

    void documentsDocklet()
    {
        DockletToolBar * documentsToolBar = new DockletToolBar("Documents");
        sDocumentListWidget = new DocumentListWidget(Main::instance()->documentManager(), window);

        QVBoxLayout * documentsDocklet_layout = new QVBoxLayout;
        documentsDocklet_layout->setContentsMargins(0,0,0,0);
        documentsDocklet_layout->setSpacing(0);
        documentsDocklet_layout->addWidget(documentsToolBar);
        documentsDocklet_layout->addWidget(sDocumentListWidget);
        
        sDocumentsDocklet = new QWidget;
        sDocumentsDocklet->setLayout(documentsDocklet_layout);

        QMenu *optionsMenu = documentsToolBar->optionsMenu();
        optionsMenu->clear();

        QAction *action;
        action = optionsMenu->addAction(tr("Hide"));
        connect(action, SIGNAL(triggered(bool)), 
                sDocumentsDocklet, SLOT(setVisible(bool)) );
        
        action = optionsMenu->addAction(tr("Close all"));
        connect(action, SIGNAL(triggered(bool)), 
                main->docDocklet()->dockWidget(), SLOT(hide()) );

        connect(main->docDocklet()->dockWidget(), SIGNAL(visibilityChanged(bool)), 
                this, SLOT(setDocDockletVisibility()) );        
    }

    void createDocumentConnections()
    {
        // Editor-Document list interaction
        connect(sDocumentListWidget, SIGNAL(clicked(Document*)),
                sEditors, SLOT(setCurrent(Document*)));
        connect(sEditors, SIGNAL(currentDocumentChanged(Document*)),
                sDocumentListWidget, SLOT(setCurrent(Document*)),
                Qt::QueuedConnection);

        // General Document lists syncronization
        connect(sDocumentListWidget, SIGNAL(reloadAllLists(QList<Document*>)),
                main, SLOT(reloadAllLists(QList<Document*>)));
        connect(sEditors, SIGNAL(tabsOrderChanged(int, int)),
                main, SLOT(reloadAllLists(int, int)));
    }

    MultiEditor * sEditors;
    QWidget * window;
    QWidget * sDocumentsDocklet;
    QWidget * sEditorsDocklet;
    DocumentListWidget * sDocumentListWidget;
    MainWindow * main;
    bool docDockletVisibility;
};

} // namespace ScIDE

#endif
