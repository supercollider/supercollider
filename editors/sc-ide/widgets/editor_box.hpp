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


#ifndef SCIDE_WIDGET_CODE_EDITOR_EDITOR_BOX_HPP
#define SCIDE_WIDGET_CODE_EDITOR_EDITOR_BOX_HPP

#include <QWidget>
#include <QStackedLayout>
#include <QPointer>
#include <QComboBox>
#include <QBoxLayout>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "../core/doc_manager.hpp"
#include "../core/main.hpp"
#include "util/multi_splitter.hpp"

namespace ScIDE {

class Document;
class GenericCodeEditor;
class MultiSplitter;

/*
A CodeEditorBox represents an IDE document split view: it contains a stack
(history) of documents displayed in that split, and an editor widget for each
of those documents. Using the document tabs, or the "Switch Document" action
one is effectively changing which of those editors is displayed, and thus
reordering the viewing history of the CodeEditorBox (the last displayed always
moves on top of the stack).

A CodeEditorBox becomes "active" when any of its editors receives focus, but
it does *not* become inactive when its editors loose focus: it only becomes
inactive when another EditorBox becomes active. That allows a CodeEditorBox to
be active even if no editor widget in the IDE has focus.

The purpose of the "active" CodeEditorBox is for the IDE to direct certain
actions to it: e.g. it is the one where newly created and opened documents will
be displayed.
*/

class CodeEditorBox : public QWidget
{
    Q_OBJECT

public:
    typedef QList< GenericCodeEditor * > History;

    CodeEditorBox(MultiSplitter *splitter, QWidget *parent = 0);

    void setDocument(Document *, int pos = -1, int selectionLength = 0);

    GenericCodeEditor *currentEditor();
    Document *currentDocument();

    const History & history() { return mHistory; }

    void setActive() {
        if (isActive())
            return;

        CodeEditorBox *lastActiveBox = gActiveBox;
        gActiveBox = this;

        if (lastActiveBox)
            lastActiveBox->update();

        update();

        if (lastActiveBox)
          emit lastActiveBox->activeChanged(false);
        emit activated(this);
        emit activeChanged(true);
    }

    bool isActive() { return gActiveBox == this; }

    QSize minimumSizeHint() const { return QSize(100, 100); }
    QSize sizeHint() const { return QSize(100, 100); }

    void showComboBox( bool );

signals:
    void currentChanged(GenericCodeEditor*);
    void activated( CodeEditorBox *me );
    void activeChanged(bool active);

public slots:
    void applySettings( Settings::Manager * );
    void comboBoxWhenSplitting();
    void tabsWhenRemovingSplits();

private slots:
    void onDocumentClosed(Document*);
    void onDocumentSaved(Document*);
    void onComboSelectionChanged(int index);

private:
    int historyIndexOf(Document*);
    GenericCodeEditor *editorForDocument(Document*);
    bool eventFilter( QObject *, QEvent * );
    void focusInEvent( QFocusEvent * );
    void paintEvent( QPaintEvent * );

    QStackedLayout *mLayout;
    History mHistory;
    static QPointer<CodeEditorBox> gActiveBox;
    QBoxLayout *mTopLayout;
    QComboBox *mDocComboBox;
    MultiSplitter *mSplitter;
    QSortFilterProxyModel *mProxyModel;
};

} // namespace ScIDE

#endif // SCIDE_WIDGET_CODE_EDITOR_EDITOR_BOX_HPP
