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

#ifndef SCIDE_WIDGETS_FIND_REPLACE_TOOL_HPP_INCLUDED
#define SCIDE_WIDGETS_FIND_REPLACE_TOOL_HPP_INCLUDED

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextDocument>
#include <QAction>
#include <QMenu>

namespace ScIDE {

class GenericCodeEditor;

class TextFindReplacePanel : public QWidget
{
    Q_OBJECT

public:
    enum Mode
    {
        Find = 1,
        Replace
    };

public:
    TextFindReplacePanel( QWidget * parent = 0 );

    void setEditor( GenericCodeEditor *editor ) { mEditor = editor; }

    Mode mode () const { return mMode; }
    void setMode( Mode );
    void initiate();

    QString findString() const { return mFindField->text(); }
    QString replaceString() const { return mReplaceField->text(); }
    bool matchCase() const { return mMatchCaseAction->isChecked(); }
    bool asRegExp() const { return mRegExpAction->isChecked(); }
    bool wholeWords() const { return mWholeWordAction->isChecked(); }
    QRegExp regexp();
    QTextDocument::FindFlags flags();

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
    void find (bool backwards);
    bool eventFilter (QObject *, QEvent *);

    QLineEdit *mFindField;
    QLabel *mFindLabel;
    QLineEdit *mReplaceField;
    QLabel *mReplaceLabel;
    QPushButton *mNextBtn;
    QPushButton *mPrevBtn;
    QPushButton *mFindAllBtn;
    QPushButton *mReplaceBtn;
    QPushButton *mReplaceAllBtn;
    QPushButton *mOptionsBtn;
    QAction *mMatchCaseAction;
    QAction *mRegExpAction;
    QAction *mWholeWordAction;

    QGridLayout *mGrid;

    Mode mMode;

    GenericCodeEditor *mEditor;
    int mSearchPosition;
};

} // namespace ScIDE

#endif
