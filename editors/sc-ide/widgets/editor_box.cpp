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

#include "editor_box.hpp"
#include "code_editor/sc_editor.hpp"
#include "../../core/main.hpp"

#include <QPainter>

namespace ScIDE {

QPointer<CodeEditorBox> CodeEditorBox::gActiveBox;

CodeEditorBox::CodeEditorBox(QWidget *parent) :
    QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mLayout = new QStackedLayout();
    setLayout(mLayout);

    connect(Main::documentManager(), SIGNAL(closed(Document*)),
            this, SLOT(onDocumentClosed(Document*)));
}

void CodeEditorBox::setDocument(Document *doc, int pos, int selectionLength)
{
    if (!doc)
        return;

    GenericCodeEditor *editor = currentEditor();
    bool switchEditor = !editor || editor->document() != doc;

    if (switchEditor) {
        editor = editorForDocument(doc);
        if (!editor) {
            editor = doc->isPlainText() ? new GenericCodeEditor(doc)
                                        : new ScCodeEditor(doc);
            editor->installEventFilter(this);
            mHistory.prepend(editor);
            mLayout->addWidget(editor);
        }
        else {
            mHistory.removeOne(editor);
            mHistory.prepend(editor);
        }
        mLayout->setCurrentWidget(editor);
        setFocusProxy(editor);
    }

    if (pos != -1)
        editor->showPosition(pos, selectionLength);

    if (switchEditor)
        emit currentChanged(editor);
}

void CodeEditorBox::onDocumentClosed(Document *doc)
{
    GenericCodeEditor * editor = editorForDocument(doc);
    if (editor) {
        bool wasCurrent = editor == currentEditor();
        mHistory.removeAll(editor);
        delete editor;
        if (wasCurrent) {
            editor = currentEditor();
            if (editor)
                mLayout->setCurrentWidget(editor);
            setFocusProxy(editor);
            emit currentChanged(editor);
        }
    }
}

GenericCodeEditor *CodeEditorBox::currentEditor()
{
    if (mHistory.count())
        return mHistory.first();
    else
        return 0;
}

int CodeEditorBox::historyIndexOf(Document *doc)
{
    int count = mHistory.count();
    for(int idx = 0; idx < count; ++idx)
        if (mHistory[idx]->document() == doc)
            return idx;
    return -1;
}

GenericCodeEditor *CodeEditorBox::editorForDocument(Document* doc)
{
    foreach(GenericCodeEditor *editor, mHistory)
        if (editor->document() == doc)
            return editor;
    return 0;
}

bool CodeEditorBox::eventFilter( QObject *object, QEvent *event )
{
    switch(event->type()) {
    case QEvent::FocusIn:
        setActive();
    default:;
    }

    return QWidget::eventFilter(object, event);
}

void CodeEditorBox::focusInEvent( QFocusEvent * )
{
    setActive();
}

Document * CodeEditorBox::currentDocument()
{
    GenericCodeEditor *editor = currentEditor();
    return editor ? editor->document() : 0;
}

void CodeEditorBox::paintEvent( QPaintEvent * )
{
    if (mLayout->currentWidget() == 0) {
        QPainter painter(this);
        painter.setRenderHint( QPainter::Antialiasing, true );
        int colorRatio = isActive() ? 160 : 125;
        painter.setBrush( palette().color(QPalette::Window).darker(colorRatio) );
        painter.setPen( Qt::NoPen );
        painter.drawRoundedRect( rect().adjusted(4, 4, -4, -4), 4, 4 );
    }
}

} // namesapce ScIDE
