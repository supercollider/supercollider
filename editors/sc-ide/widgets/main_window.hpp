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
class StatusLabel;
class StatusClockLabel;

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
        DocSaveAll,
        DocClose,
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
        LookupDocumentationForCursor,
        LookupDocumentation,
        ShowAbout,
        ShowAboutQT,

        ActionCount
    };

    explicit MainWindow(Main *);

    QAction *action( ActionRole );

    bool quit();

    void saveWindowState();
    void restoreWindowState();
    void focusCodeEditor();
    bool promptSaveDocs();

    HelpBrowserDocklet * helpBrowserDocklet() { return mHelpBrowserDocklet; }

    static MainWindow *instance() { return mInstance; }

    static bool close( Document * );
    static bool save( Document *, bool forceChoose = false );
    static bool reload( Document * );

public Q_SLOTS:
    void newSession();
    void saveCurrentSessionAs();
    void openSessionsDialog();

    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void saveAllDocuments();
    void reloadDocument();
    void closeDocument();
    void closeAllDocuments();

    void showCmdLine();
    void showFindTool();
    void showReplaceTool();
    void showGoToLineTool();
    void hideToolBox();

    void showSettings();

signals:
    void evaluateCode( const QString &, bool silent = true );

public Q_SLOTS:
    void showStatusMessage( QString const & string );

private Q_SLOTS:
    void openStartupFile();
    void openUserSupportDirectory();

    void switchSession( Session *session );
    void saveSession( Session *session );
    void onInterpreterStateChanged( QProcess::ProcessState );
    void onServerStatusReply(int ugens, int synths, int groups, int synthDefs, float avgCPU, float peakCPU);
    void onServerRunningChanged( bool running, QString const & hostName, int port );
    void onQuit();
    void onCurrentDocumentChanged( Document * );
    void onDocumentChangedExternally( Document * );
    void onDocDialogFinished();
    void updateRecentDocsMenu();
    void onRecentDocAction( QAction * );
    void onOpenSessionAction( QAction * );
    void updateWindowTitle();
    void toggleFullScreen();
    void lookupImplementation();
    void lookupImplementationForCursor();
    void lookupReferences();
    void lookupReferencesForCursor();
    void openHelp();
    void openHelpAboutIDE();
    void lookupDocumentationForCursor();
    void lookupDocumentation();
    void applySettings( Settings::Manager * );
    void storeSettings( Settings::Manager * );
    void showSwitchSessionDialog();
    void showAbout();
    void showAboutQT();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void dragEnterEvent( QDragEnterEvent * );
    virtual void dropEvent( QDropEvent * );
    virtual bool eventFilter( QObject *, QEvent * );

private:
    void createActions();
    void createMenus();
    template <class T> void saveWindowState(T * settings);
    template <class T> void restoreWindowState(T * settings);
    void updateSessionsMenu();
    void updateClockWidget( bool isFullScreen );
    void openSession( QString const & sessionName );
    bool checkFileExtension( const QString & fpath );
    void toggleInterpreterActions( bool enabled);
    QString documentOpenPath() const;
    QString documentSavePath( Document * ) const;

    Main *mMain;

    QAction * mActions[ActionCount];
    QMenu * mRecentDocsMenu;
    QMenu * mSessionsMenu;

    MultiEditor *mEditors;

    // Tools
    ToolBox *mToolBox;
    CmdLine *mCmdLine;
    GoToLineTool *mGoToLineTool;
    TextFindReplacePanel *mFindReplaceTool;

    // Status bar
    QStatusBar  *mStatusBar;
    StatusLabel *mLangStatus;
    StatusLabel *mServerStatus;
    StatusClockLabel *mClockLabel;

    // Docks
    PostDocklet * mPostDocklet;
    DocumentsDocklet *mDocumentsDocklet;
    HelpBrowserDocklet *mHelpBrowserDocklet;

    QSignalMapper mCodeEvalMapper;
    DocumentsDialog * mDocDialog;

    QString mLastDocumentSavePath;

    static MainWindow *mInstance;
};

class StatusLabel : public QLabel
{
public:
    StatusLabel(QWidget *parent = 0);
    void setBackground(const QBrush &);
    void setTextColor(const QColor &);
protected:
    void showContextMenu();
    virtual void mousePressEvent( QMouseEvent * );
};

class StatusClockLabel : public StatusLabel
{
public:
    StatusClockLabel (QWidget * parent = 0);
    ~StatusClockLabel();

private:
    void timerEvent(QTimerEvent *);
    void updateTime();
    int mTimerId;
};

} // namespace ScIDE

#endif
