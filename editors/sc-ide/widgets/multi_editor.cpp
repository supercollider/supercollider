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
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) )
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

    mActions[IndentLineOrRegion] = act = new QAction(
        QIcon::fromTheme("format-indent-line"), tr("Indent Line or Region"), this);
    act->setStatusTip(tr("Indent Line or Region"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indent()));

    mActions[TriggerAutoCompletion] = act = new QAction(tr("Trigger Autocompletion"), this);
    act->setStatusTip(tr("Suggest possible completions of text at cursor"));
    act->setShortcut(tr("Ctrl+Space", "Trigger Autocompletion"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(triggerAutoCompletion()));

    mActions[TriggerMethodCallAid] = act = new QAction(tr("Trigger Method Call Aid"), this);
    act->setStatusTip(tr("Show arguments for currently typed method call"));
    act->setShortcut(tr("Alt+Space", "Trigger Method Call Aid"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(triggerMethodCallAid()));

    // View

    mActions[EnlargeFont] = act = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    act->setShortcut(tr("Ctrl++", "Enlarge font"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Increase displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomIn()));

    mActions[ShrinkFont] = act = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    act->setShortcut( tr("Ctrl+-", "Shrink font"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Decrease displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomOut()));

    mActions[ResetFontSize] = act = new QAction(
        QIcon::fromTheme("zoom-reset"), tr("&Reset Font Size"), this);
    act->setShortcut( tr("Ctrl+0", "Reset font"));
    act->setStatusTip(tr("Reset displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(resetFontSize()));

    mActions[ShowWhitespace] = act = new QAction(tr("Show Spaces and Tabs"), this);
    act->setCheckable(true);
    mSigMux->connect(act, SIGNAL(triggered(bool)), SLOT(setShowWhitespace(bool)));

    mActions[NextDocument] = act = new QAction(tr("Next Document"), this);
    act->setShortcut( tr("Alt+Right", "Next Document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showNextDocument()));

    mActions[PreviousDocument] = act = new QAction(tr("Previous Document"), this);
    act->setShortcut( tr("Alt+Left", "Next Document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showPreviousDocument()));

    // Browse
    mActions[OpenDefinition] = act = new QAction(tr("Open Class/Method Definition"), this);
    act->setShortcut(tr("Ctrl+D", "Open definition of selected class or method"));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(openDefinition()));

    s->endGroup(); // IDE/shortcuts

    for (int i = 0; i < ActionRoleCount; ++i)
        s->addAction( mActions[i] );

    // These actions are not added to any menu, so they have to be added
    // at least to this widget, in order for the shortcuts to always respond:
    addAction(mActions[TriggerAutoCompletion]);
    addAction(mActions[TriggerMethodCallAid]);
    // These actions have to be added because to the widget because they have
    // Qt::WidgetWithChildrenShortcut context:
    addAction(mActions[EnlargeFont]);
    addAction(mActions[ShrinkFont]);
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
    mActions[IndentLineOrRegion]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
    mActions[ResetFontSize]->setEnabled( editor );
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

void MultiEditor::showNextDocument()
{
    int currentIndex = mTabs->currentIndex();
    mTabs->setCurrentIndex( qMin(currentIndex + 1, mTabs->count() - 1) );
}

void MultiEditor::showPreviousDocument()
{
    int currentIndex = mTabs->currentIndex();
    mTabs->setCurrentIndex( qMax(0, currentIndex - 1) );
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
    QTextCursor textCursor = editor->textCursor();

    if (!textCursor.hasSelection())
        textCursor.select(QTextCursor::WordUnderCursor);

    QString selectedText = textCursor.selectedText();
    if (selectedText.isEmpty())
        return;

    if (selectedText[0].isUpper())
        openClassDefinition(selectedText);
    else
        openMethodDefinition(selectedText);
}

void MultiEditor::openClassDefinition( const QString & className )
{
    using namespace ScLanguage;

    const Introspection & introspection = Main::instance()->scProcess()->introspection();
    const ClassMap & classes = introspection.classMap();

    ClassMap::const_iterator klass_it = classes.find(className);
    if (klass_it == classes.end()) {
        qWarning("Class not defined!");
        return;
    }
    Class *klass = klass_it->second;
    QString const & classLibPath = introspection.classLibraryPath();

    typedef QMap< QString, QList<Method*>* > MethodMap;
    MethodMap methodMap;

    foreach (Method *method, klass->metaClass->methods) {
        QList<Method*>* list = methodMap.value(method->definition.path);
        if (!list) {
            list = new QList<Method*>;
            methodMap.insert(method->definition.path, list);
        }
        list->append(method);
    }

    foreach (Method *method, klass->methods) {
        QList<Method*>* list = methodMap.value(method->definition.path);
        if (!list) {
            list = new QList<Method*>;
            methodMap.insert(method->definition.path, list);
        }
        list->append(method);
    }

    QPointer<OpenDefinitionDialog> dialog = new OpenDefinitionDialog(this);
    dialog->setWindowTitle(tr("Open Definition of Class '%1'").arg(klass->name));

    QTreeWidget *tree = dialog->treeWidget();
    tree->setColumnCount(1);
    tree->setRootIsDecorated(true);

    for (MethodMap::Iterator it = methodMap.begin(); it != methodMap.end(); ++it) {
        QString path = it.key();
        QList<Method*> *methods = it.value();

        QString displayPath = path.startsWith(classLibPath) ? path.mid(classLibPath.length())
                                                            : path;

        QTreeWidgetItem *pathItem = new QTreeWidgetItem (
            tree,
            QStringList()
            << displayPath
        );

        pathItem->setData( 0, Qt::UserRole, path );

        foreach( Method *method, *methods ) {
            QString name = method->name;
            if (method->ownerClass == klass->metaClass)
                name.prepend('*');
            QTreeWidgetItem *methodItem = new QTreeWidgetItem (
                pathItem,
                QStringList()
                << name
            );

            methodItem->setData( 0, Qt::UserRole, method->definition.position );
        }

        delete methods;
    }

    tree->setCurrentItem(tree->topLevelItem(0));

    if (dialog->exec())
    {
        QTreeWidgetItem *item = tree->currentItem();
        if (item && item->parent() && item->parent() != tree->invisibleRootItem()) {
            QString path = item->parent()->data(0, Qt::UserRole).toString();
            int pos = item->data(0, Qt::UserRole).toInt();
            Main::instance()->documentManager()->open(path, pos);
        }
    }

    delete dialog;
}

static QString signature(ScLanguage::Method *method, bool arguments = false)
{
    using namespace ScLanguage;

    QString sig = method->ownerClass->name;
    if (sig.startsWith("Meta_")) {
        sig.remove(0, 5);
        sig.append(": *");
    }
    else
        sig.append(": ");

    sig.append(method->name);

    if (!arguments)
        return sig;

    int argc = method->arguments.count();
    if (argc) {
        sig.append(" (");
        for( int i = 0; i < argc; ++i )
        {
            const Argument & arg = method->arguments[i];
            if (i > 0)
                sig.append(", ");
            sig.append(arg.name);
            if (!arg.defaultValue.get().isEmpty()) {
                sig.append(" = ");
                sig.append(arg.defaultValue);
            }
        }
        sig.append(")");
    }
    else if (method->name.get().endsWith('_'))
        sig.append(" (value)");

    return sig;
}

void MultiEditor::openMethodDefinition( const QString & methodName )
{
    using namespace ScLanguage;
    using std::pair;

    const Introspection & introspection = Main::instance()->scProcess()->introspection();
    const MethodMap & methods = introspection.methodMap();
    const ClassMap & classes = introspection.classMap();

    pair<MethodMap::const_iterator, MethodMap::const_iterator> matchingMethods =
        methods.equal_range(methodName);

    if (matchingMethods.first == matchingMethods.second) {
        qWarning("Method not defined!");
        return;
    }

    QString const & classLibPath = introspection.classLibraryPath();

    QPointer<OpenDefinitionDialog> dialog = new OpenDefinitionDialog(this);
    dialog->setWindowTitle(tr("Open Definition of Method '%1'").arg(methodName));
    QTreeWidget *tree = dialog->treeWidget();
    tree->setColumnCount(2);

    for (MethodMap::const_iterator it = matchingMethods.first;
        it != matchingMethods.second;
        ++it)
    {
        Method *method = it->second;

        const QString & path = method->definition.path;

        QString displayPath;
        if (path.startsWith(classLibPath))
            displayPath = path.mid(classLibPath.length());
        else
            displayPath = path;

        QTreeWidgetItem *item = new QTreeWidgetItem (
            tree,
            QStringList()
            << signature(method)
            << displayPath
        );

        item->setData( 0, Qt::UserRole, path );
        item->setData( 0, Qt::UserRole + 1, method->definition.position );
    }

    tree->model()->sort(0);
    tree->setCurrentItem(tree->topLevelItem(0));


    if (dialog->exec())
    {
        QTreeWidgetItem *item = tree->currentItem();
        if (item) {
            QString path = item->data(0, Qt::UserRole).toString();
            int pos = item->data(0, Qt::UserRole + 1).toInt();
            Main::instance()->documentManager()->open(path, pos);
        }
    }

    delete dialog;
}

} // namespace ScIDE
