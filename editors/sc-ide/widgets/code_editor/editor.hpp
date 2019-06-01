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

#include <QPlainTextEdit>
#include <QGraphicsScene>
#include <QList>

namespace ScIDE {

namespace Settings {
class Manager;
}

class Document;
class OverlayAnimator;

class GenericCodeEditor : public QPlainTextEdit {
    Q_OBJECT

    friend class LineIndicator;
    friend class OverlayAnimator;

public:
    GenericCodeEditor(Document*, QWidget* parent = NULL);

    Document* document() { return mDoc; }
    QTextDocument* textDocument() { return QPlainTextEdit::document(); }
    void setDocument(Document*);
    bool showWhitespace();
    bool showLinenumber();
    bool find(const QRegExp& expr, QTextDocument::FindFlags options = 0);
    bool replace(const QRegExp& expr, const QString& replacement, QTextDocument::FindFlags options = 0);
    int findAll(const QRegExp& expr, QTextDocument::FindFlags options = 0);
    int replaceAll(const QRegExp& expr, const QString& replacement, QTextDocument::FindFlags options = 0);

    void showPosition(int charPosition, int selectionLength = 0);
    QString symbolUnderCursor();
    int inactiveFadeAlpha() { return mInactiveFadeAlpha; }

protected:
    virtual bool event(QEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);
    void doKeyAction(QKeyEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void wheelEvent(QWheelEvent*);
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    void hideMouseCursor(QKeyEvent*);
    virtual QMimeData* createMimeDataFromSelection() const;

public slots:
    void applySettings(Settings::Manager*);
    void closeDocument();
    void zoomIn(int steps = 1);
    void zoomOut(int steps = 1);
    void resetFontSize();
    void setShowWhitespace(bool);
    void setShowLinenumber(bool);
    void clearSearchHighlighting();
    void toggleOverwriteMode();
    void copyLineUp();
    void copyLineDown();
    void moveLineUp();
    void moveLineDown();
    void deleteWord();
    void gotoPreviousEmptyLine();
    void gotoNextEmptyLine();
    void setActiveAppearance(bool active);
    void updateDocLastSelection();

protected slots:
    void updateLayout();
    void updateLineIndicator(QRect, int);
    void onDocumentFontChanged();
    void onCursorPositionChanged();

protected:
    void resizeEvent(QResizeEvent*);
    void paintLineIndicator(QPaintEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void updateExtraSelections();
    void updateCurrentLineHighlighting();
    virtual void indentCurrentRegion() {}

    void zoomFont(int steps);

    void copyUpDown(bool up);
    void moveLineUpDown(bool up);
    void gotoEmptyLineUpDown(bool up);

    void hideMouseCursor();

    void updateFocusRect();

    class LineIndicator* mLineIndicator;
    QGraphicsScene* mOverlay;
    QWidget* mOverlayWidget;
    OverlayAnimator* mOverlayAnimator;

    Document* mDoc;

    bool mHighlightCurrentLine;
    bool mEditorBoxIsActive;
    int mLastCursorBlock;
    QTextCharFormat mCurrentLineTextFormat;

    QTextCharFormat mSearchResultTextFormat;

    QList<QTextEdit::ExtraSelection> mSearchSelections;

    QGraphicsRectItem* mFocusRect = 0;

    int mInactiveFadeAlpha = 0;

private:
    void handleKeyDown(QKeyEvent* event, QTextCursor& textCursor);
    void handleKeyUp(QKeyEvent* event, QTextCursor& textCursor);
    void handleKeyDelete(QKeyEvent* event, QTextCursor& textCursor);
    void handleKeyRightParentheses(QKeyEvent* event, QTextCursor& textCursor);
    void handleKeyBackspace(QKeyEvent* event, QTextCursor& textCursor, bool& updateCursor);
};

} // namespace ScIDE
