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

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextDocument>
#include <QAction>
#include <QMenu>
#include <QToolButton>

namespace ScIDE {

class GenericCodeEditor;

class TextFindReplacePanel : public QWidget {
    Q_OBJECT

public:
    enum Mode { Find = 1, Replace };

    enum ActionRole {
        FindNext,
        FindPrevious,

        ActionCount
    };

public:
    TextFindReplacePanel(QWidget* parent = 0);

    void setEditor(GenericCodeEditor* editor) { mEditor = editor; }

    Mode mode() const { return mMode; }
    void setMode(Mode);
    void initiate();

    QString findString() const { return mFindField->text(); }
    QString replaceString() const { return mReplaceField->text(); }
    bool matchCase() const { return mMatchCaseAction->isChecked(); }
    bool asRegExp() const { return mRegExpAction->isChecked(); }
    bool wholeWords() const { return mWholeWordAction->isChecked(); }
    QRegExp regexp();
    QTextDocument::FindFlags flags();

    QAction* action(ActionRole role) { return mActions[role]; }

public Q_SLOTS:
    void findNext();
    void findPrevious();
    void findAll();
    void replace();
    void replaceAll();

private Q_SLOTS:
    void onFindFieldReturn();
    void onFindFieldTextChanged();

private:
    void find(bool backwards);
    bool eventFilter(QObject*, QEvent*);
    static void reportFoundOccurrencies(int count);
    static void reportReplacedOccurrencies(int count);

    QLineEdit* mFindField;
    QLabel* mFindLabel;
    QLineEdit* mReplaceField;
    QLabel* mReplaceLabel;
    QToolButton* mNextBtn;
    QToolButton* mPrevBtn;
    QToolButton* mReplaceBtn;
    QToolButton* mReplaceAllBtn;
    QToolButton* mOptionsBtn;
    QAction* mMatchCaseAction;
    QAction* mRegExpAction;
    QAction* mWholeWordAction;
    QAction* mActions[ActionCount];

    QGridLayout* mGrid;

    Mode mMode;

    GenericCodeEditor* mEditor;
    int mSearchPosition;
};

} // namespace ScIDE
