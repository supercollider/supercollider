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

#include "find_replace_tool.hpp"
#include "main_window.hpp"
#include "code_editor/editor.hpp"
#include "../core/main.hpp"
#include "../core/settings/manager.hpp"

#include <QApplication>
#include <QTextBlock>
#include <QToolButton>
#include <QStyle>
#include <QHBoxLayout>

namespace ScIDE {

TextFindReplacePanel::TextFindReplacePanel(QWidget* parent):
    QWidget(parent),
    mMode((Mode)0), // a hack so that first setMode() works
    mEditor(0),
    mSearchPosition(-1) {
    mFindField = new QLineEdit;
    mReplaceField = new QLineEdit;

    mNextBtn = new QToolButton();
    mNextBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    mNextBtn->setToolTip(tr("Find Next"));

    mPrevBtn = new QToolButton();
    mPrevBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    mPrevBtn->setToolTip(tr("Find Previous"));

    mReplaceBtn = new QToolButton();
    mReplaceBtn->setText(tr("Replace"));
    mReplaceAllBtn = new QToolButton();
    mReplaceAllBtn->setText(tr("Replace All"));

    mOptionsBtn = new QToolButton();
    mOptionsBtn->setText(tr("Options"));
    mOptionsBtn->setIcon(QIcon::fromTheme("preferences-other"));
    mOptionsBtn->setPopupMode(QToolButton::InstantPopup);

    QMenu* optMenu = new QMenu(this);
    mMatchCaseAction = optMenu->addAction(tr("Match Case"));
    mMatchCaseAction->setCheckable(true);
    mRegExpAction = optMenu->addAction(tr("Regular Expression"));
    mRegExpAction->setCheckable(true);
    mWholeWordAction = optMenu->addAction(tr("Whole Words"));
    mWholeWordAction->setCheckable(true);
    mOptionsBtn->setMenu(optMenu);

    mFindLabel = new QLabel(tr("Find:"));
    mFindLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mReplaceLabel = new QLabel(tr("Replace:"));
    mReplaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mGrid = new QGridLayout();
    mGrid->setContentsMargins(0, 0, 0, 0);
    mGrid->setSpacing(2);

    QHBoxLayout* findBtnLayout = new QHBoxLayout();
    findBtnLayout->setContentsMargins(0, 0, 0, 0);
    findBtnLayout->setSpacing(1);
    findBtnLayout->addWidget(mPrevBtn);
    findBtnLayout->addWidget(mNextBtn);
    findBtnLayout->addStretch(0);
    findBtnLayout->addWidget(mOptionsBtn);

    mGrid->addWidget(mFindLabel, 0, 0);
    mGrid->addWidget(mFindField, 0, 1);
    mGrid->addLayout(findBtnLayout, 0, 2);

    QHBoxLayout* replaceBtnLayout = new QHBoxLayout();
    replaceBtnLayout->setContentsMargins(0, 0, 0, 0);
    replaceBtnLayout->setSpacing(1);
    replaceBtnLayout->addWidget(mReplaceBtn);
    replaceBtnLayout->addWidget(mReplaceAllBtn);
    replaceBtnLayout->addStretch(0);

    mGrid->addWidget(mReplaceLabel, 1, 0);
    mGrid->addWidget(mReplaceField, 1, 1);
    mGrid->addLayout(replaceBtnLayout, 1, 2);

    mGrid->setColumnStretch(1, 1);

    setLayout(mGrid);

    setMode(Find);

    setFocusProxy(mFindField);
    QWidget::setTabOrder(mFindField, mReplaceField);
    mFindField->installEventFilter(this);

    connect(mNextBtn, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(mPrevBtn, SIGNAL(clicked()), this, SLOT(findPrevious()));
    connect(mReplaceBtn, SIGNAL(clicked()), this, SLOT(replace()));
    connect(mReplaceAllBtn, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(mFindField, SIGNAL(returnPressed()), this, SLOT(onFindFieldReturn()));
    connect(mFindField, SIGNAL(textChanged(QString)), this, SLOT(onFindFieldTextChanged()));
    connect(mReplaceField, SIGNAL(returnPressed()), this, SLOT(replace()));
    // Update search results when options change:
    connect(optMenu, SIGNAL(triggered(QAction*)), this, SLOT(findAll()));

    Settings::Manager* settings = Main::settings();
    QAction* action;

    action = mActions[FindNext] = new QAction(tr("Find Next"), this);
    action->setShortcut(tr("Ctrl+G", "Find Next"));
    connect(action, SIGNAL(triggered()), this, SLOT(findNext()));
    settings->addAction(action, "editor-find-next", tr("Text Editor"));

    action = mActions[FindPrevious] = new QAction(tr("Find Previous"), this);
    action->setShortcut(tr("Ctrl+Shift+G", "Find Previous"));
    connect(action, SIGNAL(triggered()), this, SLOT(findPrevious()));
    settings->addAction(action, "editor-find-previous", tr("Text Editor"));
}

void TextFindReplacePanel::setMode(Mode mode) {
    if (mode == mMode)
        return;

    mMode = mode;

    bool visible = mMode == Replace;
    mReplaceLabel->setVisible(visible);
    mReplaceField->setVisible(visible);
    mReplaceBtn->setVisible(visible);
    mReplaceAllBtn->setVisible(visible);
}

void TextFindReplacePanel::initiate() {
    mSearchPosition = -1;

    if (mEditor) {
        QTextCursor c(mEditor->textCursor());
        if (c.hasSelection()
            && c.document()->findBlock(c.selectionStart()) == c.document()->findBlock(c.selectionEnd())) {
            mFindField->setText(c.selectedText());
            mReplaceField->clear();
        }
    }

    mFindField->selectAll();
    findAll();
}

QRegExp TextFindReplacePanel::regexp() {
    QRegExp expr(findString());
    expr.setPatternSyntax(asRegExp() ? QRegExp::RegExp : QRegExp::FixedString);
    expr.setCaseSensitivity(matchCase() ? Qt::CaseSensitive : Qt::CaseInsensitive);
    return expr;
}

QTextDocument::FindFlags TextFindReplacePanel::flags() {
    QTextDocument::FindFlags f;
    if (wholeWords())
        f |= QTextDocument::FindWholeWords;
    return f;
}

void TextFindReplacePanel::findNext() { find(false); }

void TextFindReplacePanel::findPrevious() { find(true); }

void TextFindReplacePanel::onFindFieldReturn() { find(QApplication::keyboardModifiers() & Qt::ShiftModifier); }

void TextFindReplacePanel::onFindFieldTextChanged() {
    // Incremental search

    if (!mEditor)
        return;

    QRegExp expr(regexp());
    QTextDocument::FindFlags flagz(flags());

    if (mSearchPosition == -1)
        mSearchPosition = mEditor->textCursor().selectionStart();

    int count = mEditor->findAll(expr, flagz);
    if (!expr.isEmpty())
        reportFoundOccurrencies(count);

    QTextCursor searchCursor(mEditor->textDocument());
    searchCursor.setPosition(mSearchPosition);

    if (expr.isEmpty()) {
        mEditor->setTextCursor(searchCursor);
    } else if (count) {
        mEditor->setTextCursor(searchCursor);
        mEditor->find(expr, flagz);
    }
}

bool TextFindReplacePanel::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == mFindField && ev->type() == QEvent::FocusOut)
        mSearchPosition = -1;

    return QWidget::eventFilter(obj, ev);
}

void TextFindReplacePanel::find(bool backwards) {
    // Non incremental search!

    if (!mEditor)
        return;

    QRegExp expr = regexp();
    if (expr.isEmpty())
        return;

    QTextDocument::FindFlags opt = flags();
    if (backwards)
        opt |= QTextDocument::FindBackward;

    mEditor->find(expr, opt);

    // This was not incremental search, so reset search position
    mSearchPosition = -1;
}

void TextFindReplacePanel::findAll() {
    if (!mEditor)
        return;

    QRegExp expr = regexp();

    // NOTE: empty expression removes any search highlighting
    int count = mEditor->findAll(expr, flags());

    if (!expr.isEmpty())
        reportFoundOccurrencies(count);
}

void TextFindReplacePanel::replace() {
    if (!mEditor)
        return;

    QRegExp expr = regexp();
    if (expr.isEmpty())
        return;

    mEditor->replace(expr, replaceString(), flags());

    mSearchPosition = -1;
}

void TextFindReplacePanel::replaceAll() {
    if (!mEditor)
        return;

    QRegExp expr = regexp();
    if (expr.isEmpty())
        return;

    QTextDocument::FindFlags opt = flags();

    int count = mEditor->replaceAll(expr, replaceString(), opt);

    reportReplacedOccurrencies(count);

    mSearchPosition = -1;
}

void TextFindReplacePanel::reportFoundOccurrencies(int count) {
    QString message = tr("%n matches found.", "Find text in document...", count);
    MainWindow::instance()->showStatusMessage(message);
}

void TextFindReplacePanel::reportReplacedOccurrencies(int count) {
    QString message = tr("%n matches replaced.", "Find/replace text in document...", count);
    MainWindow::instance()->showStatusMessage(message);
}

} // namespace ScIDE
