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

#include "editor.hpp"
#include "tokens.hpp"
#include "../core/settings/theme.hpp"

#include <QTextBlock>
#include <utility>

namespace ScIDE {

typedef std::pair<TokenIterator, TokenIterator> BracketPair;

class ScCodeEditor : public GenericCodeEditor {
    Q_OBJECT

public:
    /** \brief Used to indicate whether an edit action should start a new edit
     * block, which affects undo/redo operation grouping.
     */
    enum EditBlockMode {
        NewEditBlock, ///< Start a new edit block.
        JoinEditBlock ///< Join this edit to the previous one.
    };

public:
    static TokenIterator previousOpeningBracket(TokenIterator it);
    static TokenIterator nextClosingBracket(TokenIterator it);
    static void matchBracket(const TokenIterator& bracket, BracketPair& match);
    static void nextBracketPair(const TokenIterator& startIt, BracketPair& bracketPair);

public:
    ScCodeEditor(Document*, QWidget* parent = 0);
    QTextCursor currentRegion();
    void blinkCode(const QTextCursor& c);
    bool spaceIndent() { return mSpaceIndent; }

public slots:
    void applySettings(Settings::Manager*);
    void setSpaceIndent(bool on) { mSpaceIndent = on; }

    /// Indents the currently-selected text.
    void indent(EditBlockMode = NewEditBlock);

    void triggerAutoCompletion();
    void triggerMethodCallAid();
    void toggleComment();
    void gotoPreviousBlock();
    void gotoNextBlock();
    void selectBlockAroundCursor();
    void selectCurrentRegion();
    void gotoNextRegion();
    void gotoPreviousRegion();
    bool openDocumentation();
    void openDefinition();
    void openCommandLine();
    void findReferences();
    void evaluateLine();
    void evaluateRegion();
    void evaluateDocument();

protected:
    virtual bool event(QEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual bool canInsertFromMimeData(const QMimeData* data) const;
    virtual void insertFromMimeData(const QMimeData* data);

private slots:
    void matchBrackets();

private:
    QTextCursor cursorAt(const TokenIterator, int offset = 0);
    QTextCursor selectionForPosition(int position);
    QTextCursor regionAroundCursor(const QTextCursor&);
    QTextCursor blockAroundCursor(const QTextCursor&);
    void moveToNextToken(QTextCursor&, QTextCursor::MoveMode);
    void moveToPreviousToken(QTextCursor&, QTextCursor::MoveMode);

    void updateExtraSelections();
    void indentCurrentRegion();

    void toggleCommentSingleLine(QTextCursor);
    void toggleCommentSingleLine();
    void toggleCommentSelection();
    void addSingleLineComment(QTextCursor, int indentationLevel);
    void removeSingleLineComment(QTextCursor);

    /** \brief Returns the index of the first non-whitespace character in
     * \c block, or <tt>block.text().size()</tt> if the block is all whitespace.
     */
    int indentedStartOfLine(const QTextBlock& block);

    /// Indents the text in \c selection according to editor preferences.
    void indent(const QTextCursor& selection, EditBlockMode = NewEditBlock);

    /** \brief Indents a line to a chosen level of indentation.
     *
     * \param block The line to indent.
     * \param level The level of indentation.
     * \return A new \c QTextBlock indented to the requested level.
     */
    QTextBlock indent(const QTextBlock& block, int level);

    /** \brief Creates a string of indentation characters acccording to
     * editor preferences.
     *
     * \param level The indentation level.
     */
    QString makeIndentationString(int level);

    /** \brief Get the indentation level for the cursor's selection start.
     *
     * \param cursor A cursor in the document.
     * \return The number of levels of indentation there should be for
     * the line where the \c cursor's selection starts, or -1 on failure.
     */
    int indentationLevel(const QTextCursor& cursor);

    /** \brief Inserts enough whitespace at \c cursor to reach the next
     * tab stop according to editor preferences.
     */
    void insertSpaceToNextTabStop(QTextCursor& cursor);

    bool insertMatchingTokens(const QString& token);
    bool removeMatchingTokens();

    // Data members

    /// If \c true, use spaces to indent.
    bool mSpaceIndent;

    bool mStepForwardEvaluation;
    int mBlinkDuration;
    bool mInsertMatchingTokens;
    QTextCharFormat mBracketHighlight;
    QTextCharFormat mBracketMismatchFormat;
    bool mHighlightBracketContents;

    QList<QTextEdit::ExtraSelection> mBracketSelections;
    bool mMouseBracketMatch;

    class AutoCompleter* mAutoCompleter;
};

} // namespace ScIDE
