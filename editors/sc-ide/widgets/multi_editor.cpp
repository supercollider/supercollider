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

#include "multi_editor.hpp"
#include "main_window.hpp"
#include "code_editor/editor.hpp"
#include "../core/doc_manager.hpp"
#include "../core/sig_mux.hpp"
#include "../core/main.hpp"
#include "../core/sc_process.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QApplication>
#include <QStyle>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QMenu>
#include <QDebug>

#include <QTreeWidget>
#include <QHeaderView>
#include <QFileInfo>
#include <QDialog>

namespace ScIDE {

class OpenDefinitionDialog : public QDialog
{
public:
    OpenDefinitionDialog( QWidget * parent = 0 ):
        QDialog(parent)
    {
        mTreeWidget = new QTreeWidget();
        mTreeWidget->setRootIsDecorated(false);
        mTreeWidget->setAllColumnsShowFocus(true);
        //mTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        mTreeWidget->setHeaderHidden(true);
        mTreeWidget->header()->setStretchLastSection(false);

        QHeaderView *header = mTreeWidget->header();
        header->setResizeMode( QHeaderView::ResizeToContents );

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(mTreeWidget);
        layout->setContentsMargins(1,1,1,1);

        connect(mTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(accept()));
        connect(mTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(accept()));

        mTreeWidget->setFocus(Qt::OtherFocusReason);

        resize(400, 300);
    }

    QTreeWidget *treeWidget() { return mTreeWidget; }

private:
    QTreeWidget *mTreeWidget;
};

MultiEditor::MultiEditor( Main *main, QWidget * parent ) :
    QWidget(parent),
    mDocManager(main->documentManager()),
    mSigMux(new SignalMultiplexer(this)),
    mTabs(new QTabWidget),
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) ),
    mScRequest( new ScRequest(main->scProcess(), this) )
{
    mTabs->setDocumentMode(true);
    mTabs->setTabsClosable(true);
    mTabs->setMovable(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->addWidget(mTabs);
    setLayout(l);

    connect(mDocManager, SIGNAL(opened(Document*, int)),
            this, SLOT(onOpen(Document*, int)));
    connect(mDocManager, SIGNAL(closed(Document*)),
            this, SLOT(onClose(Document*)));
    connect(mDocManager, SIGNAL(saved(Document*)),
            this, SLOT(update(Document*)));
    connect(mDocManager, SIGNAL(showRequest(Document*,int)),
            this, SLOT(show(Document*,int))),

    connect(mTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentChanged(int)));
    connect(mTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));

    connect(&mModificationMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(onModificationChanged(QWidget*)));

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));

    connect(mScRequest, SIGNAL(response(QString,QString)),
            this, SLOT(onScResponse(QString,QString)));

    createActions();
    updateActions();
    applySettings(main->settings());
}

void MultiEditor::createActions()
{
    Settings::Manager *s = Main::instance()->settings();
    s->beginGroup("IDE/shortcuts");

    QAction * act;

    // Edit

    mActions[Undo] = act = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    act->setShortcut(tr("Ctrl+Z", "Undo"));
    act->setStatusTip(tr("Undo last editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(undo()));
    mSigMux->connect(SIGNAL(undoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Redo] = act = new QAction(
        QIcon::fromTheme("edit-redo"), tr("Re&do"), this);
    act->setShortcut(tr("Ctrl+Shift+Z", "Redo"));
    act->setStatusTip(tr("Redo next editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));
    mSigMux->connect(SIGNAL(redoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
    act->setShortcut(tr("Ctrl+X", "Cut"));
    act->setStatusTip(tr("Cut text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C", "Copy"));
    act->setStatusTip(tr("Copy text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcut(tr("Ctrl+V", "Paste"));
    act->setStatusTip(tr("Paste text from clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    mActions[IndentMore] = act = new QAction(
        QIcon::fromTheme("format-indent-more"), tr("Indent &More"), this);
    act->setStatusTip(tr("Increase indentation of selected lines"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indentMore()));

    mActions[IndentLess] = act = new QAction(
        QIcon::fromTheme("format-indent-less"), tr("Indent &Less"), this);
    act->setStatusTip(tr("Decrease indentation of selected lines"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indentLess()));

    // View

    mActions[EnlargeFont] = act = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    act->setShortcut(tr("Ctrl++", "Enlarge font"));
    act->setStatusTip(tr("Increase displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomIn()));

    mActions[ShrinkFont] = act = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    act->setShortcut( tr("Ctrl+-", "Shrink font"));
    act->setStatusTip(tr("Decrease displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomOut()));

    mActions[ShowWhitespace] = act = new QAction(tr("Show Spaces and Tabs"), this);
    act->setCheckable(true);
    mSigMux->connect(act, SIGNAL(triggered(bool)), SLOT(setShowWhitespace(bool)));

    // Browse
    mActions[OpenDefinition] = act = new QAction(tr("Open Class/Method Definition"), this);
    act->setShortcut(tr("Ctrl+D", "Open definition of selected class or method"));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(openDefinition()));

    s->endGroup(); // IDE/shortcuts

    for (int i = 0; i < ActionRoleCount; ++i)
        s->addAction( mActions[i] );
}

void MultiEditor::updateActions()
{
    CodeEditor *editor = currentEditor();
    QTextDocument *doc = editor ? editor->document()->textDocument() : 0;

    mActions[Undo]->setEnabled( doc && doc->isUndoAvailable() );
    mActions[Redo]->setEnabled( doc && doc->isRedoAvailable() );
    mActions[Copy]->setEnabled( editor && editor->textCursor().hasSelection() );
    mActions[Cut]->setEnabled( mActions[Copy]->isEnabled() );
    mActions[Paste]->setEnabled( editor );
    mActions[IndentMore]->setEnabled( editor );
    mActions[IndentLess]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
    mActions[ShowWhitespace]->setEnabled( editor );
    mActions[ShowWhitespace]->setChecked( editor && editor->showWhitespace() );
}

void MultiEditor::applySettings( Settings::Manager *s )
{
    s->beginGroup("IDE/editor");
    mStepForwardEvaluation = s->value("stepForwardEvaluation").toBool();
    s->endGroup();

    int c = editorCount();
    for( int i = 0; i < c; ++i )
    {
        CodeEditor *editor = editorForTab(i);
        if(!editor) continue;
        editor->applySettings(s);
    }
}

void MultiEditor::setCurrent( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    if(editor)
        mTabs->setCurrentWidget(editor);
}

void MultiEditor::onOpen( Document *doc, int pos )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);
    editor->applySettings(Main::instance()->settings());

    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = mDocModifiedIcon;

    mTabs->addTab( editor, icon, doc->title() );
    mTabs->setCurrentIndex( mTabs->count() - 1 );

    mModificationMapper.setMapping(tdoc, editor);
    connect(tdoc, SIGNAL(modificationChanged(bool)),
            &mModificationMapper, SLOT(map()));

    if (pos > 0)
        editor->showPosition(pos);
}

void MultiEditor::onClose( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    delete editor;
}

void MultiEditor::show( Document *doc, int pos )
{
    CodeEditor *editor = editorForDocument( doc );
    mTabs->setCurrentWidget(editor);
    editor->showPosition(pos);
}

void MultiEditor::update( Document *doc )
{
    int c = editorCount();
    for(int i=0; i<c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if(editor && editor->document() == doc)
            mTabs->setTabText(i, doc->title());
    }
}

void MultiEditor::onCloseRequest( int index )
{
    CodeEditor *editor = editorForTab( index );
    if(editor)
        MainWindow::close(editor->document());
}

void MultiEditor::onCurrentChanged( int index )
{
    CodeEditor *editor = editorForTab(index);

    if(editor) {
        mSigMux->setCurrentObject(editor);
        editor->setFocus(Qt::OtherFocusReason);
    }

    updateActions();

    Q_EMIT( currentChanged( editor ? editor->document() : 0 ) );
}

void MultiEditor::onModificationChanged( QWidget *w )
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(w);
    if(!editor) return;

    int i = mTabs->indexOf(editor);
    if( i == -1 ) return;

    QIcon icon;
    if(editor->document()->textDocument()->isModified())
        icon = mDocModifiedIcon;
    mTabs->setTabIcon( i, icon );
}

CodeEditor * MultiEditor::editorForTab( int index )
{
    return qobject_cast<CodeEditor*>( mTabs->widget(index) );
}

CodeEditor * MultiEditor::editorForDocument( Document *doc )
{
    int c = editorCount();
    for(int i = 0; i < c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if( editor && editor->document() == doc)
            return editor;
    }
    return 0;
}

void MultiEditor::openDefinition()
{
    CodeEditor * editor = currentEditor();
    QString selectedText = editor->textCursor().selectedText();
    if (selectedText.isEmpty())
        return;

    QString command ( selectedText[0].isUpper() ? "sendClassDefinitions" : "sendMethodDefinitions");

    mScRequest->send( command, selectedText );
}

void MultiEditor::onScResponse( const QString & command, const QString & data )
{
    static QString classDefinitions("sendClassDefinitions");
    static QString methodDefinitions("sendMethodDefinitions");

    if (command == classDefinitions)
        handleClassDefinitions(data);
    else if(command == methodDefinitions)
        handleMethodDefinitions(data);
}

void MultiEditor::handleClassDefinitions( const QString & yamlString )
{
    std::stringstream stream;
    stream << yamlString.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        assert(doc.Type() == YAML::NodeType::Sequence);

        switch (doc.size()) {
        case 0:
        {
            //emit message("Class not defined!");
            qDebug("Class not defined!");
            return;
        }

        case 1:
        {
            YAML::Iterator it = doc.begin();
            YAML::Node const & entry = *it;
            assert(entry.Type() == YAML::NodeType::Sequence);

            // name, path, character position
            std::string path = entry[1].to<std::string>();
            int charPosition = entry[2].to<int>() - 1; // position is one off
            Main::instance()->documentManager()->open(QString(path.c_str()), charPosition);
            return;
        }

        default:
        {
            QPointer<OpenDefinitionDialog> dialog = new OpenDefinitionDialog(this);
            dialog->setWindowTitle("Open Class Definition");

            QTreeWidget *tree = dialog->treeWidget();
            tree->setColumnCount(2);

            for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it) {
                YAML::Node const & entry = *it;
                assert(entry.Type() == YAML::NodeType::Sequence);

                std::string path = entry[1].to<std::string>();
                int charPosition = entry[2].to<int>() - 1; // position is one off

                QTreeWidgetItem *item = new QTreeWidgetItem (
                    tree,
                    QStringList()
                    << QFileInfo(path.c_str()).fileName()
                    << path.c_str()
                );
                item->setData( 0, Qt::UserRole, charPosition );
            }

            tree->setCurrentItem(tree->topLevelItem(0));

            if (dialog->exec())
            {
                QTreeWidgetItem *item = tree->currentItem();
                if (item) {
                    QString fileName = item->text(1);
                    int pos = item->data(0, Qt::UserRole).toInt();
                    Main::instance()->documentManager()->open(fileName, pos);
                }
            }

            delete dialog;
        }
        }
    }
}

static QString constructMethodSignature(const YAML::Node & node)
{
    assert(node.Type() == YAML::NodeType::Sequence);

    QString sig( node[0].to<std::string>().c_str() );
    sig.append(": ");

    QString method( node[1].to<std::string>().c_str() );
    sig.append(method);

    int count = node.size();
    if (count > 2) {
        sig.append(" (");
        for (int i = 2; i < count; ++i) {
            if (i > 2) sig.append(", ");
            sig.append( node[i].to<std::string>().c_str() );
        }
        sig.append(")");
    }
    else if (method.endsWith('_')) {
        sig.append(" (value)");
    }

    return sig;
}

void MultiEditor::handleMethodDefinitions( const QString & yamlString )
{
    std::stringstream stream;
    stream << yamlString.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        assert(doc.Type() == YAML::NodeType::Sequence);

        switch (doc.size()) {
        case 0:
        {
            //emit message("Method not defined!");
            qDebug("Method not defined!");
            return;
        }
        case 1:
        {
            YAML::Node const & entry = doc[0];
            QString sig = constructMethodSignature( entry[0] );
            YAML::Node const & location = entry[1];
            assert(location.Type() == YAML::NodeType::Sequence);
            QString fileName( location[0].to<std::string>().c_str() );
            int pos = location[1].to<int>();
            Main::instance()->documentManager()->open(fileName, pos);
            return;
        }
        default:
        {
            QPointer<OpenDefinitionDialog> dialog = new OpenDefinitionDialog(this);
            dialog->setWindowTitle("Open Method Definition");
            QTreeWidget *tree = dialog->treeWidget();
            tree->setColumnCount(2);

            for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
            {
                YAML::Node const & entry = *it;
                assert(entry.Type() == YAML::NodeType::Sequence);

                QString signature = constructMethodSignature( entry[0] );

                YAML::Node const & location = entry[1];
                assert(location.Type() == YAML::NodeType::Sequence);
                QString path = location[0].to<std::string>().c_str();
                int pos = location[1].to<int>();

                QTreeWidgetItem *item = new QTreeWidgetItem (
                    tree,
                    QStringList()
                    << signature
                    << path
                );
                item->setData( 0, Qt::UserRole, pos );
            }

            tree->setCurrentItem(tree->topLevelItem(0));

            if (dialog->exec())
            {
                QTreeWidgetItem *item = tree->currentItem();
                if (item) {
                    QString fileName = item->text(1);
                    int pos = item->data(0, Qt::UserRole).toInt();
                    Main::instance()->documentManager()->open(fileName, pos);
                }
            }

            delete dialog;
        }
        }
    }
}

} // namespace ScIDE
