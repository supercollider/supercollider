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

#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QSignalMapper>
#include <QStatusBar>

#include "util/status_box.hpp"

namespace ScIDE {

class Main;
class MultiEditor;
class ToolBox;
class TextFindReplacePanel;
class GoToLineTool;
class PostDocklet;
class DocumentsDocklet;
class HelpBrowserDocklet;
class CmdLine;
class Document;
class DocumentsDialog;
struct Session;
class ClockStatusBox;
class ScServer;
class ScProcess;
class GenericCodeEditor;

namespace Settings {
class Manager;
}

class MainWindow : public QMainWindow {
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

    explicit MainWindow(Main*);

    QAction* action(ActionRole);

    bool quit();

    void saveWindowState();
    void restoreWindowState();
    void focusCodeEditor();
    bool promptSaveDocs();

    HelpBrowserDocklet* helpBrowserDocklet() { return mHelpBrowserDocklet; }
    PostDocklet* postDocklet() { return mPostDocklet; }

    static MainWindow* instance() { return mInstance; }
    Settings::Manager* setting();

    static bool close(Document*);
    static bool save(Document*, bool forceChoose = false, bool saveInExtensionFolder = false);
    static bool reload(Document*);

    void restoreDocuments();

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
    void showCmdLine(const QString&);
    void showFindTool();
    void showReplaceTool();
    void showGoToLineTool();
    void hideToolBox();

    void showSettings();

signals:
    void evaluateCode(const QString&, bool silent = true);

public Q_SLOTS:
    void showStatusMessage(QString const& string);

private Q_SLOTS:
    void openStartupFile();
    void openUserSupportDirectory();

    void switchSession(Session* session);
    void saveSession(Session* session);
    void onInterpreterStateChanged(QProcess::ProcessState);
    void onQuit();
    void onCurrentDocumentChanged(Document*);
    void onDocumentChangedExternally(Document*);
    void onDocDialogFinished();
    void updateRecentDocsMenu();
    void onOpenRecentDocument(QAction*);
    void onOpenSessionAction(QAction*);
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
    void applySettings(Settings::Manager*);
    void storeSettings(Settings::Manager*);
    void showSwitchSessionDialog();
    void showAbout();
    void showAboutQT();
    void cmdLineForCursor();

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dropEvent(QDropEvent*);
    virtual bool eventFilter(QObject*, QEvent*);

private:
    void createActions();
    void createMenus();
    template <class T> void saveWindowState(T* settings);
    template <class T> void restoreWindowState(T* settings);
    void updateSessionsMenu();
    void updateClockWidget(bool isFullScreen);
    void openSession(QString const& sessionName);
    bool checkFileExtension(const QString& fpath);
    void toggleInterpreterActions(bool enabled);
    void applyCursorBlinkingSettings(Settings::Manager*);
    QString documentOpenPath() const;
    QString documentSavePath(Document*) const;

    Main* mMain;

    QAction* mActions[ActionCount];
    QMenu* mRecentDocsMenu;
    QMenu* mSessionsMenu;

    MultiEditor* mEditors;

    // Tools
    ToolBox* mToolBox;
    CmdLine* mCmdLine;
    GoToLineTool* mGoToLineTool;
    TextFindReplacePanel* mFindReplaceTool;

    // Status bar
    QStatusBar* mStatusBar;
    StatusBox* mLangStatus;
    StatusBox* mServerStatus;
    ClockStatusBox* mClockLabel;

    // Docks
    PostDocklet* mPostDocklet;
    DocumentsDocklet* mDocumentsDocklet;
    HelpBrowserDocklet* mHelpBrowserDocklet;

    QSignalMapper mCodeEvalMapper;
    DocumentsDialog* mDocDialog;

    QString mLastDocumentSavePath;

    static MainWindow* mInstance;
};

class ClockStatusBox : public StatusLabel {
public:
    ClockStatusBox(QWidget* parent = 0);
    ~ClockStatusBox();

private:
    void timerEvent(QTimerEvent*);
    void updateTime();
    int mTimerId;
};

} // namespace ScIDE
