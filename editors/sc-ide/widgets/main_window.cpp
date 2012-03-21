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

#include "main_window.hpp"
#include "../core/main.hpp"
#include "../core/doc_manager.hpp"
#include "code_edit.hpp"
#include "multi_editor.hpp"
#include "cmd_line.hpp"
#include "doc_list.hpp"
#include "post_window.hpp"
#include "settings/dialog.hpp"

#include <QAction>
#include <QShortcut>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>

namespace ScIDE {

MainWindow::MainWindow(Main * main) :
    mMain(main)
{
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );

    mEditors = new MultiEditor(main);
    mDocListDock = new DocumentsDock(main->documentManager(), this);
    mPostDock = new PostDock(this);

    // use a layout for tool widgets, to provide for separate margin control
    QVBoxLayout *tool_box = new QVBoxLayout;
    tool_box->addWidget(cmdLine());
    tool_box->setContentsMargins(5,2,5,2);

    QVBoxLayout *center_box = new QVBoxLayout;
    center_box->setContentsMargins(0,0,0,0);
    center_box->setSpacing(0);
    center_box->addWidget(mEditors);
    center_box->addLayout(tool_box);
    QWidget *central = new QWidget;
    central->setLayout(center_box);
    setCentralWidget(central);

    addDockWidget(Qt::LeftDockWidgetArea, mDocListDock);
    addDockWidget(Qt::BottomDockWidgetArea, mPostDock);

    connect(main->scProcess(), SIGNAL( scPost(QString) ),
            mPostDock->mPostWindow, SLOT( append(QString) ) );
    connect(mDocListDock->list(), SIGNAL(clicked(Document*)),
            mEditors, SLOT(setCurrent(Document*)));
    connect(mEditors, SIGNAL(currentChanged(Document*)),
            mDocListDock->list(), SLOT(setCurrent(Document*)),
            Qt::QueuedConnection);

    createMenus();

    QIcon icon;
    icon.addFile(":/icons/sc-cube-128");
    icon.addFile(":/icons/sc-cube-48");
    icon.addFile(":/icons/sc-cube-32");
    icon.addFile(":/icons/sc-cube-16");
    QApplication::setWindowIcon(icon);
}

void MainWindow::createMenus()
{
    new QShortcut( QKeySequence("Ctrl+Tab"), this, SLOT(toggleComandLineFocus()) );

    QAction *act;

    // File

    mActions[Quit] = act = new QAction(
        QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
    act->setShortcuts(QKeySequence::Quit);
    act->setStatusTip(tr("Quit SuperCollider IDE"));
    QObject::connect( act, SIGNAL(triggered()), this, SLOT(onQuit()) );

    // View
    mActions[ShowDocList] = act = new QAction(tr("&Documents"), this);
    act->setStatusTip(tr("Show/Hide the Documents dock"));
    act->setCheckable(true);
    connect(act, SIGNAL(triggered(bool)), mDocListDock, SLOT(setVisible(bool)));
    connect(mDocListDock, SIGNAL(visibilityChanged(bool)), act, SLOT(setChecked(bool)));

    // Language

    mActions[EvaluateCurrentFile] = act = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("Evaluate &File"), this);
    act->setStatusTip(tr("Evaluate current File"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateCurrentFile()));

    mActions[EvaluateSelectedRegion] = act = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("&Evaluate Region"), this);
    act->setShortcut(QKeySequence(tr("Ctrl+Return", "Language|Evaluate Region")));
    act->setStatusTip(tr("Evaluate selection or current line"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateCurrentRegion()));

    mMain->scProcess()->action(ScIDE::SCProcess::StopMain)
        ->setShortcut(QKeySequence(tr("Ctrl+.", "Language|Stop Main")));

    // Settings

    mActions[ShowSettings] = act = new QAction(tr("&Configure IDE..."), this);
    act->setStatusTip(tr("Show configuration dialog"));
    connect(act, SIGNAL(triggered()), this, SLOT(showSettings()));


    QMenu *menu;
    QMenu *submenu;

    menu = new QMenu(tr("&File"), this);
    menu->addAction( mEditors->action(MultiEditor::DocNew) );
    menu->addAction( mEditors->action(MultiEditor::DocOpen) );
    menu->addAction( mEditors->action(MultiEditor::DocSave) );
    menu->addAction( mEditors->action(MultiEditor::DocSaveAs) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::DocClose) );
    menu->addSeparator();
    menu->addAction( mActions[Quit] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Edit"), this);
    menu->addAction( mEditors->action(MultiEditor::Undo) );
    menu->addAction( mEditors->action(MultiEditor::Redo) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Cut) );
    menu->addAction( mEditors->action(MultiEditor::Copy) );
    menu->addAction( mEditors->action(MultiEditor::Paste) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::IndentMore) );
    menu->addAction( mEditors->action(MultiEditor::IndentLess) );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&View"), this);
    submenu = new QMenu(tr("&Docks"), this);
    submenu->addAction( mActions[ShowDocList] );
    menu->addMenu(submenu);
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EnlargeFont) );
    menu->addAction( mEditors->action(MultiEditor::ShrinkFont) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowWhitespace) );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(SCProcess::StartSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::RecompileClassLibrary) );
    menu->addAction( mMain->scProcess()->action(SCProcess::StopSCLang) );
    menu->addSeparator();
    menu->addAction( mActions[EvaluateCurrentFile] );
    menu->addAction( mActions[EvaluateSelectedRegion] );
    menu->addSeparator();
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::RunMain) );
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::StopMain) );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Settings"), this);
    menu->addAction( mActions[ShowSettings] );

    menuBar()->addMenu(menu);
}

QAction *MainWindow::action( ActionRole role )
{
    Q_ASSERT( role < ActionCount );
    return mActions[role];
}

bool MainWindow::quit()
{
    bool ok = true;

    QList<Document*> docs = mMain->documentManager()->documents();
    Q_FOREACH(Document *doc, docs) {
        if(doc->textDocument()->isModified()) {
            ok = false;
            break;
        }
    }

    if(!ok) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(
            NULL,
            tr("SuperCollider IDE"),
            tr("There are unsaved changes to documents.\n"
                "Do you want to quit without saving?"),
            QMessageBox::Ok | QMessageBox::Cancel
        );
        if( ret == QMessageBox::Cancel )
            return false;
    }

    mMain->storeSettings();

    QApplication::quit();

    return true;
}

void MainWindow::onQuit()
{
    quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!quit()) event->ignore();
}

void MainWindow::toggleComandLineFocus()
{
    QWidget *cmd = cmdLine();
    if(cmd->hasFocus()) {
        QWidget *editor = mEditors->currentEditor();
        if(editor) editor->setFocus(Qt::OtherFocusReason);
    }
    else
        cmd->setFocus(Qt::OtherFocusReason);
}

void MainWindow::showSettings()
{
    Settings::Dialog dialog(mMain->settings());
    int result = dialog.exec();
    if( result == QDialog::Accepted )
        mMain->applySettings();
}

QWidget *MainWindow::cmdLine()
{
    static QWidget *widget = 0;
    if(!widget) {
        CmdLine *w = new CmdLine("Command line:");
        connect(w, SIGNAL(invoked(QString, bool)),
                mMain->scProcess(), SLOT(evaluateCode(QString, bool)));
        widget = w;
    }
    return widget;
}

void MainWindow::evaluateCurrentRegion()
{
    // Evaluate selection, if any, otherwise current line

    CodeEditor *editor = mEditors->currentEditor();
    if (!editor)
        return;

    QString text;
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection())
        text = cursor.selectedText();
    else {
        text = cursor.block().text();
        if( mEditors->stepForwardEvaluation() ) {
            cursor.movePosition(QTextCursor::NextBlock);
            cursor.movePosition(QTextCursor::EndOfBlock);
            editor->setTextCursor(cursor);
        }
    }

    if (text.isEmpty()) return;

    // NOTE: QTextDocument contains unicode paragraph separators U+2029
    text.replace( QChar( 0x2029 ), QChar( '\n' ) );

    Main::instance()->scProcess()->evaluateCode(text);
}

void MainWindow::evaluateCurrentFile()
{
    CodeEditor *editor = mEditors->currentEditor();
    if (!editor)
        return;

    QString documentText = editor->document()->textDocument()->toPlainText();
    Main::instance()->scProcess()->evaluateCode(documentText);
}

} // namespace ScIDE
