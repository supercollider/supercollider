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

namespace ScIDE {

class Document;
class GenericCodeEditor;

class CodeEditorBox : public QWidget
{
    Q_OBJECT

public:
    typedef QList< GenericCodeEditor * > History;

    CodeEditorBox(QWidget *parent = 0);

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

        emit activated(this);
    }

    bool isActive() { return gActiveBox == this; }

    QSize minimumSizeHint() const { return QSize(100, 100); }
    QSize sizeHint() const { return QSize(100, 100); }

signals:
    void currentChanged(GenericCodeEditor*);
    void activated( CodeEditorBox *me );

private slots:
    void onDocumentClosed(Document*);

private:
    int historyIndexOf(Document*);
    GenericCodeEditor *editorForDocument(Document*);
    bool eventFilter( QObject *, QEvent * );
    void focusInEvent( QFocusEvent * );
    void paintEvent( QPaintEvent * );

    QStackedLayout *mLayout;
    History mHistory;
    static QPointer<CodeEditorBox> gActiveBox;
};

} // namespace ScIDE

#endif // SCIDE_WIDGET_CODE_EDITOR_EDITOR_BOX_HPP
