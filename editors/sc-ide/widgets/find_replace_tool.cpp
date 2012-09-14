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
#include "code_editor/editor.hpp"

#include <QApplication>
#include <QTextBlock>

namespace ScIDE {

TextFindReplacePanel::TextFindReplacePanel( QWidget * parent ):
    QWidget(parent),
    mMode((Mode) 0), // a hack so that first setMode() works
    mEditor(0),
    mSearchPosition(-1)
{
    mFindField = new QLineEdit;
    mReplaceField = new QLineEdit;

    mNextBtn = new QPushButton(tr("Next"));
    mPrevBtn = new QPushButton(tr("Previous"));
    mFindAllBtn = new QPushButton(tr("Find All"));
    mReplaceBtn = new QPushButton(tr("Replace"));
    mReplaceAllBtn = new QPushButton(tr("Replace All"));

    mOptionsBtn = new QPushButton(tr("Options"));
    QMenu *optMenu = new QMenu(this);
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
    mGrid->setContentsMargins(2,2,2,2);
    mGrid->addWidget(mFindLabel, 0, 0);
    mGrid->addWidget(mFindField, 0, 1);
    mGrid->addWidget(mNextBtn, 0, 2);
    mGrid->addWidget(mPrevBtn, 0, 3);
    mGrid->addWidget(mFindAllBtn, 0, 4);
    mGrid->addWidget(mOptionsBtn, 0, 5);
    mGrid->addWidget(mReplaceLabel, 1, 0);
    mGrid->addWidget(mReplaceField, 1, 1);
    mGrid->addWidget(mReplaceBtn, 1, 2);
    mGrid->addWidget(mReplaceAllBtn, 1, 3);
    setLayout(mGrid);

    setMode(Find);

    setFocusProxy(mFindField);
    QWidget::setTabOrder(mFindField, mReplaceField);
    mFindField->installEventFilter(this);

    connect(mNextBtn, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(mPrevBtn, SIGNAL(clicked()), this, SLOT(findPrevious()));
    connect(mFindAllBtn, SIGNAL(clicked()), this, SLOT(findAll()));
    connect(mReplaceBtn, SIGNAL(clicked()), this, SLOT(replace()));
    connect(mReplaceAllBtn, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(mFindField, SIGNAL(returnPressed()), this, SLOT(onFindFieldReturn()));
    connect(mFindField, SIGNAL(textChanged(QString)), this, SLOT(onFindFieldTextChanged()));
    connect(mReplaceField, SIGNAL(returnPressed()), this, SLOT(replace()));
}

void TextFindReplacePanel::setMode( Mode mode )
{
    if (mode == mMode) return;

    mMode = mode;

    bool visible = mMode == Replace;
    mReplaceLabel->setVisible(visible);
    mReplaceField->setVisible(visible);
    mReplaceBtn->setVisible(visible);
    mReplaceAllBtn->setVisible(visible);
}

void TextFindReplacePanel::initiate()
{
    mSearchPosition = -1;

    if(mEditor)
    {
        QTextCursor c( mEditor->textCursor() );
        if(c.hasSelection() &&
           c.document()->findBlock(c.selectionStart()) ==
           c.document()->findBlock(c.selectionEnd()))
        {
            mFindField->setText(c.selectedText());
            mReplaceField->clear();
        }
    }

    mFindField->selectAll();
    findAll();
}

QRegExp TextFindReplacePanel::regexp()
{
    QRegExp expr(findString());
    expr.setPatternSyntax(asRegExp() ? QRegExp::RegExp : QRegExp::FixedString);
    expr.setCaseSensitivity(matchCase() ? Qt::CaseSensitive : Qt::CaseInsensitive);
    return expr;
}

QTextDocument::FindFlags TextFindReplacePanel::flags()
{
    QTextDocument::FindFlags f;
    if(wholeWords())
        f |= QTextDocument::FindWholeWords;
    return f;
}

void TextFindReplacePanel::findNext()
{
    find(false);
}

void TextFindReplacePanel::findPrevious()
{
    find(true);
}

void TextFindReplacePanel::onFindFieldReturn()
{
    find (QApplication::keyboardModifiers() & Qt::ShiftModifier);
}

void TextFindReplacePanel::onFindFieldTextChanged()
{
    // Incremental search

    if (!mEditor) return;

    QRegExp expr(regexp());
    QTextDocument::FindFlags flagz(flags());

    if (mSearchPosition == -1)
        mSearchPosition = mEditor->textCursor().selectionStart();

    int count = mEditor->findAll(expr, flagz);

    QTextCursor searchCursor(mEditor->textDocument());
    searchCursor.setPosition(mSearchPosition);

    if (expr.isEmpty()) {
        mEditor->setTextCursor(searchCursor);
    } else if (count) {
        mEditor->setTextCursor(searchCursor);
        mEditor->find(expr, flagz);
    }
}

bool TextFindReplacePanel::eventFilter (QObject *obj, QEvent *ev)
{
    if (obj == mFindField && ev->type() == QEvent::FocusOut)
        mSearchPosition = -1;

    return QWidget::eventFilter(obj,ev);
}

void TextFindReplacePanel::find (bool backwards)
{
    // Non incremental search!

    if (!mEditor) return;

    QRegExp expr = regexp();
    if (expr.isEmpty()) return;

    QTextDocument::FindFlags opt = flags();
    if (backwards)
        opt |= QTextDocument::FindBackward;

    mEditor->find(expr, opt);

    // This was not incremental search, so reset search position
    mSearchPosition = -1;
}

void TextFindReplacePanel::findAll()
{
    if (!mEditor) return;

    // NOTE: empty expression removes any search highlighting
    mEditor->findAll(regexp(), flags());
}

void TextFindReplacePanel::replace()
{
    if (!mEditor) return;

    QRegExp expr = regexp();
    if (expr.isEmpty()) return;

    mEditor->replace(expr, replaceString(), flags());

    mSearchPosition = -1;
}

void TextFindReplacePanel::replaceAll()
{
    if (!mEditor) return;

    QRegExp expr = regexp();
    if (expr.isEmpty()) return;

    mEditor->replaceAll(expr, replaceString(), flags());

    mSearchPosition = -1;
}

} // namespace ScIDE
