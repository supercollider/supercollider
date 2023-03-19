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
#include "../core/main.hpp"

#include <QPainter>
#include <QScrollBar>
#include <QModelIndex>

namespace ScIDE {

QPointer<CodeEditorBox> CodeEditorBox::gActiveBox;

CodeEditorBox::CodeEditorBox(MultiSplitter* splitter, QWidget* parent): QWidget(parent), mSplitter(splitter) {
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mTopLayout = new QBoxLayout(QBoxLayout::BottomToTop);
    mTopLayout->setSpacing(1);
    mTopLayout->setContentsMargins(0, 0, 0, 0);

    mLayout = new QStackedLayout();
    mTopLayout->addLayout(mLayout);
    setLayout(mTopLayout);

    mDocComboBox = new QComboBox();
    mDocComboBox->setFocusPolicy(Qt::NoFocus);
    mTopLayout->addWidget(mDocComboBox);

    mProxyModel = new QSortFilterProxyModel();
    mProxyModel->setSourceModel(Main::documentManager()->docModel());
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    mProxyModel->sort(0);
    mDocComboBox->setModel(mProxyModel);

    connect(mDocComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboSelectionChanged(int)),
            Qt::QueuedConnection);

    connect(Main::documentManager(), SIGNAL(closed(Document*)), this, SLOT(onDocumentClosed(Document*)));
    connect(Main::documentManager(), SIGNAL(saved(Document*)), this, SLOT(onDocumentSaved(Document*)));
    connect(Main::instance(), SIGNAL(applySettingsRequest(Settings::Manager*)), this,
            SLOT(applySettings(Settings::Manager*)));

    connect(mSplitter->editor(), SIGNAL(splitViewActivated()), this, SLOT(comboBoxWhenSplitting()));
    connect(mSplitter->editor(), SIGNAL(splitViewDeactivated()), this, SLOT(tabsWhenRemovingSplits()));

    applySettings(Main::settings());
}

void CodeEditorBox::applySettings(Settings::Manager* settings) {
    bool comboBoxActive = settings->value("IDE/editor/useComboBox").toBool();
    showComboBox(comboBoxActive);

    comboBoxWhenSplitting();
}

void CodeEditorBox::comboBoxWhenSplitting() {
    if (mSplitter->count() > 1) {
        bool comboBoxInUse = Main::settings()->value("IDE/editor/useComboBox").toBool();
        if (!comboBoxInUse) {
            bool comboBoxIsInUse = Main::settings()->value("IDE/editor/useComboBoxWhenSplitting").toBool();
            showComboBox(comboBoxIsInUse);
        }
    }
}

void CodeEditorBox::tabsWhenRemovingSplits() {
    if (mSplitter->count() < 2) {
        bool comboBoxInUse = Main::settings()->value("IDE/editor/useComboBox").toBool();
        showComboBox(comboBoxInUse);
    }
}

void CodeEditorBox::showComboBox(bool condition) {
    if (condition)
        mDocComboBox->show();
    else
        mDocComboBox->hide();
}

void CodeEditorBox::onComboSelectionChanged(int index) {
    QModelIndex proxyIndex = mProxyModel->index(index, 0);
    QModelIndex mRow = mProxyModel->mapToSource(proxyIndex);
    int mIndex = mRow.row();

    if (mIndex >= 0 && index >= 0 && mDocComboBox) {
        setDocument(Main::documentManager()->docModel()->item(mIndex)->data().value<Document*>(), -1, 0);
    }
}

void CodeEditorBox::setDocument(Document* doc, int pos, int selectionLength) {
    if (!doc)
        return;

    GenericCodeEditor* editor = currentEditor();
    bool switchEditor = !editor || editor->document() != doc;

    if (switchEditor) {
        editor = editorForDocument(doc);
        if (!editor) {
            editor = doc->isPlainText() ? new GenericCodeEditor(doc) : new ScCodeEditor(doc);
            editor->installEventFilter(this);
            mHistory.prepend(editor);
            mLayout->addWidget(editor);
            connect(this, SIGNAL(activeChanged(bool)), editor, SLOT(setActiveAppearance(bool)));
        } else {
            mHistory.removeOne(editor);
            mHistory.prepend(editor);
        }
        editor->setActiveAppearance(this->isActive());
        editor->setReadOnly(!doc->editable());
        mLayout->setCurrentWidget(editor);
        setFocusProxy(editor);
        int modelIndex = doc->modelItem()->index().row();
        if (mDocComboBox) {
            mDocComboBox->blockSignals(true);
            QModelIndex mIndex = Main::documentManager()->docModel()->index(modelIndex, 0, QModelIndex());
            QModelIndex proxyRow = mProxyModel->mapFromSource(mIndex);
            int proxyIndex = proxyRow.row();
            mDocComboBox->setCurrentIndex(proxyIndex);
            mDocComboBox->setCurrentText(doc->title());
            mDocComboBox->blockSignals(false);
        }
    }

    if (pos != -1)
        editor->showPosition(pos, selectionLength);

    if (switchEditor)
        emit currentChanged(editor);
}

void CodeEditorBox::onDocumentClosed(Document* doc) {
    GenericCodeEditor* editor = editorForDocument(doc);
    if (editor) {
        bool wasCurrent = editor == currentEditor();
        mHistory.removeAll(editor);
        editor->deleteLater();
        if (wasCurrent) {
            editor = currentEditor();
            if (editor)
                mLayout->setCurrentWidget(editor);
            setFocusProxy(editor);
            emit currentChanged(editor);
        }
    }
}

void CodeEditorBox::onDocumentSaved(Document* doc) {
    // Check whether the document type still matches the editor type.
    // If not, replace the editor.

    int history_idx = historyIndexOf(doc);
    if (history_idx == -1)
        return;

    GenericCodeEditor* editor = mHistory[history_idx];
    if (doc->isPlainText() == (qobject_cast<ScCodeEditor*>(editor) == 0))
        return;

    bool was_current = editor == currentEditor();
    bool was_focused = editor->window()->focusWidget() == editor;
    int cursor_position = editor->textCursor().position();
    int scroll_position = editor->verticalScrollBar()->value();

    mHistory.removeAt(history_idx);
    delete editor;

    editor = doc->isPlainText() ? new GenericCodeEditor(doc) : new ScCodeEditor(doc);
    editor->installEventFilter(this);
    mHistory.insert(history_idx, editor);
    mLayout->addWidget(editor);

    QTextCursor cursor(editor->textDocument());
    cursor.setPosition(cursor_position);
    editor->setTextCursor(cursor);

    editor->verticalScrollBar()->setValue(scroll_position);

    if (was_current) {
        mLayout->setCurrentWidget(editor);
        setFocusProxy(editor);
    }

    if (was_focused)
        editor->setFocus(Qt::OtherFocusReason);

    emit currentChanged(editor);
}

GenericCodeEditor* CodeEditorBox::currentEditor() {
    if (mHistory.count())
        return mHistory.first();
    else
        return 0;
}

int CodeEditorBox::historyIndexOf(Document* doc) {
    int count = mHistory.count();
    for (int idx = 0; idx < count; ++idx)
        if (mHistory[idx]->document() == doc)
            return idx;
    return -1;
}

GenericCodeEditor* CodeEditorBox::editorForDocument(Document* doc) {
    foreach (GenericCodeEditor* editor, mHistory)
        if (editor->document() == doc)
            return editor;
    return 0;
}

bool CodeEditorBox::eventFilter(QObject* object, QEvent* event) {
    switch (event->type()) {
    case QEvent::FocusIn:
        setActive();
    default:;
    }

    return QWidget::eventFilter(object, event);
}

void CodeEditorBox::focusInEvent(QFocusEvent*) { setActive(); }

Document* CodeEditorBox::currentDocument() {
    GenericCodeEditor* editor = currentEditor();
    return editor ? editor->document() : 0;
}

void CodeEditorBox::paintEvent(QPaintEvent*) {
    if (mLayout->currentWidget() == 0) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int colorRatio = isActive() ? 160 : 125;
        painter.setBrush(palette().color(QPalette::Window).darker(colorRatio));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect().adjusted(4, 4, -4, -4), 4, 4);
    }
}

} // namesapce ScIDE
