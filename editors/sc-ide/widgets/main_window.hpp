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

#include <QMainWindow>
#include <QLabel>
#include <QProcess>
#include <QSignalMapper>

namespace ScIDE
{

class Main;
class MultiEditor;
class PostDock;
class DocumentsDock;
class StatusLabel;
class Document;
class DocumentsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum ActionRole {
        // File
        Quit = 0,
        DocNew,
        DocOpen,
        DocSave,
        DocSaveAs,
        DocClose,
        DocReload,

        // View
        ShowDocList,

        // Language
        EvaluateCurrentFile,
        EvaluateSelection,
        EvaluateRegion,

        // Settings
        ShowSettings,

        // Help
        BrowseHelp,
        SearchHelp,
        HelpForSelection,

        ActionCount
    };

    MainWindow(Main *);

    QAction *action( ActionRole );

    bool quit();

    static MainWindow *instance() { return mInstance; }

    static bool close( Document * );
    static bool save( Document *, bool forceChoose = false );
    static bool reload( Document * );

public Q_SLOTS:
    void toggleComandLineFocus();
    void showSettings();
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void reloadDocument();
    void closeDocument();

signals:
    void evaluateCode( const QString &, bool silent = true );

private Q_SLOTS:
    void evaluateSelection(); // either selection, or current line
    void evaluateRegion();
    void evaluateCurrentFile();
    void helpForSelection();
    void onInterpreterStateChanged( QProcess::ProcessState );
    void onQuit();
    void onCurrentDocumentChanged( Document * );
    void onDocumentChangedExternally( Document * );
    void onDocDialogFinished();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void createMenus();
    QWidget *cmdLine();

    Main *mMain;

    QAction * mActions[ActionCount];

    MultiEditor *mEditors;

    // Status bar
    StatusLabel *mLangStatus;
    StatusLabel *mSynthStatus;

    // Docks
    PostDock * mPostDock;
    DocumentsDock *mDocListDock;

    QSignalMapper mCodeEvalMapper;
    DocumentsDialog * mDocDialog;

    static MainWindow *mInstance;
};

class StatusLabel : public QLabel
{
public:
    StatusLabel(QWidget *parent = 0);
    void setBackground(const QBrush &);
    void setTextColor(const QColor &);
};

} // namespace ScIDE

#endif
