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

#include "sc_editor.hpp"
#include "autocompleter.hpp"
#include "line_indicator.hpp"
#include "../util/gui_utilities.hpp"
#include "../../core/main.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/settings/theme.hpp"

#include "QtCollider/hacks/hacks_qt.hpp"

#include <QKeyEvent>
#include <QStack>
#include <QMimeData>
#include <QUrl>

namespace ScIDE {

ScCodeEditor::ScCodeEditor( Document *doc, QWidget *parent ) :
    GenericCodeEditor( doc, parent ),
    mSpaceIndent(true),
    mBlinkDuration(600),
    mMouseBracketMatch(false),
    mAutoCompleter( new AutoCompleter(this) )
{
    Q_ASSERT(mDoc != 0);

    connect( this, SIGNAL(cursorPositionChanged()),
             this, SLOT(matchBrackets()) );

    connect( Main::instance(), SIGNAL(applySettingsRequest(Settings::Manager*)),
             this, SLOT(applySettings(Settings::Manager*)) );

    mAutoCompleter->documentChanged(textDocument());

    applySettings( Main::settings() );
}

void ScCodeEditor::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/editor");

    mSpaceIndent = settings->value("spaceIndent").toBool();
    mBlinkDuration = settings->value("blinkDuration").toInt();
    mBracketHighlight = settings->getThemeVal("matchingBrackets");
    mBracketMismatchFormat = settings->getThemeVal("mismatchedBrackets");
    mStepForwardEvaluation = settings->value("stepForwardEvaluation").toBool();
    mInsertMatchingTokens = settings->value("insertMatchingTokens").toBool();
    mHighlightBracketContents = settings->value("highlightBracketContents").toBool();

    settings->endGroup();

    // update bracket match highlighting:
    matchBrackets();
}

bool ScCodeEditor::event( QEvent *e )
{
    switch (e->type()) {
    case QEvent::KeyPress:
    {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        switch (ke->key()) {
        case Qt::Key_Tab:
            if (!tabChangesFocus()) {
                indent();
                e->accept();
                return true;
            }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    return GenericCodeEditor::event(e);
}

void ScCodeEditor::keyPressEvent( QKeyEvent *e )
{
    hideMouseCursor(e);
    bool actionInSuper = false;

    QTextCursor cursor( textCursor() );
    bool cursorMoved = true;

    if (e == QKeySequence::MoveToNextWord)
        moveToNextToken( cursor, QTextCursor::MoveAnchor );
    else if (e == QKeySequence::MoveToPreviousWord)
        moveToPreviousToken( cursor, QTextCursor::MoveAnchor );
    else if (e == QKeySequence::SelectNextWord)
        moveToNextToken( cursor, QTextCursor::KeepAnchor );
    else if (e == QKeySequence::SelectPreviousWord)
        moveToPreviousToken( cursor, QTextCursor::KeepAnchor );
    else
        cursorMoved = false;

    if (cursorMoved) {
        setTextCursor( cursor );
        doKeyAction(e);
        return;
    }

    switch (e->key()) {
    case Qt::Key_Home:
    {
        Qt::KeyboardModifiers mods(e->modifiers());
        if (mods && mods != Qt::ShiftModifier) {
            GenericCodeEditor::keyPressEvent(e);
            return;
        }

        QTextCursor::MoveMode mode =
            mods & Qt::ShiftModifier ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;

        QTextCursor c(textCursor());
        QTextBlock b(c.block());

        int pos = indentedStartOfLine(b);
        pos += b.position();

        if (c.position() == pos)
            c.movePosition(QTextCursor::StartOfLine, mode);
        else
            c.setPosition(pos, mode);

        setTextCursor(c);
        doKeyAction(e);
        return;
    }

    case Qt::Key_Backtab:
    {
        QTextCursor cursor = textCursor();
        insertSpaceToNextTabStop( cursor );
        ensureCursorVisible();
        doKeyAction(e);
        return;
    }
    case Qt::Key_Backspace:
        if (mInsertMatchingTokens && !overwriteMode() && e->modifiers() == 0)
            if (removeMatchingTokens())
                break;
        GenericCodeEditor::keyPressEvent(e);
        actionInSuper = true;
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QTextBlock cursorBlock = cursor.block();
        int cursorPosInBlock = cursor.position() - cursorBlock.position();
        
        TokenIterator prevToken = TokenIterator::leftOf(cursorBlock, cursorPosInBlock);
        TokenIterator nextToken = TokenIterator::rightOf(cursorBlock, cursorPosInBlock);
        
        if (   nextToken.block() == cursorBlock
            && nextToken.type() == Token::ClosingBracket
            && prevToken.type() != Token::ClosingBracket // no double-newline if cursor is between closing brackets, i.e. ])
            && !(prevToken.block().firstLineNumber() < nextToken.block().firstLineNumber()) // no double-nl if only whitespace to the left
        )
        {
            cursor.beginEditBlock();
            cursor.insertBlock();
            cursor.insertBlock();
            cursor.endEditBlock();
            cursor.movePosition( QTextCursor::PreviousBlock, QTextCursor::KeepAnchor );
            indent(cursor, JoinEditBlock);
            cursor.movePosition( QTextCursor::EndOfBlock );
        }
        else {
            cursor.beginEditBlock();
            cursor.insertBlock();
            cursor.endEditBlock();
            indent(cursor, JoinEditBlock);
        }
        cursor.setVerticalMovementX(-1);
        setTextCursor(cursor);
        break;
    }
    default:
        if (!overwriteMode() && insertMatchingTokens(e->text()))
            break;
        GenericCodeEditor::keyPressEvent(e);
        actionInSuper = true;
    }

    mAutoCompleter->keyPress(e);
    if(!actionInSuper) doKeyAction(e);
}

void ScCodeEditor::mouseReleaseEvent ( QMouseEvent *e )
{
    // Prevent deselection of bracket match:
    if(!mMouseBracketMatch)
        GenericCodeEditor::mouseReleaseEvent(e);

    mMouseBracketMatch = false;
}

void ScCodeEditor::mouseDoubleClickEvent( QMouseEvent * e )
{
    // Always pass to superclass so as to handle line selection on triple click
    GenericCodeEditor::mouseDoubleClickEvent(e);

    if (e->button() == Qt::LeftButton) {
        QTextCursor cursor = cursorForPosition(e->pos());
        QTextCursor selection = selectionForPosition( cursor.position() );

        if (!selection.isNull()) {
            mMouseBracketMatch = true;
            setTextCursor(selection);
            return;
        }
    }
}

void ScCodeEditor::mouseMoveEvent( QMouseEvent *e )
{
    // Prevent initiating a text drag:
    if(!mMouseBracketMatch)
        GenericCodeEditor::mouseMoveEvent(e);
}

void ScCodeEditor::dragEnterEvent( QDragEnterEvent * event )
{
    // The purpose is only to bypass GenericCodeEditor::dragEnterEvent:
    QPlainTextEdit::dragEnterEvent(event);
}

bool ScCodeEditor::canInsertFromMimeData ( const QMimeData * data ) const
{
    if (data->hasUrls())
        return true;

    return QPlainTextEdit::canInsertFromMimeData(data);
}

void ScCodeEditor::insertFromMimeData ( const QMimeData * data )
{
    if (data->hasUrls()) {
        QTextCursor cursor = textCursor();
        QList<QUrl> urls = data->urls();
        bool multiple = urls.size() > 1;
        if (multiple) {
            cursor.insertText("[");
            cursor.insertBlock();
        }
        for (int i = 0; i < urls.size(); ++ i) {
            QUrl url = urls[i];
            cursor.insertText("\"");
            if ( QURL_IS_LOCAL_FILE(url) )
                cursor.insertText(url.toLocalFile());
            else
                cursor.insertText(url.toString());
            cursor.insertText("\"");
            if (i < urls.size() - 1) {
                cursor.insertText(",");
                cursor.insertBlock();
            }
        }
        if (multiple) {
            cursor.insertBlock();
            cursor.insertText("]");
        }
    }
    else
        QPlainTextEdit::insertFromMimeData(data);
}

void ScCodeEditor::moveToNextToken( QTextCursor & cursor, QTextCursor::MoveMode mode )
{
    if (cursor.atBlockEnd()) {
        cursor.movePosition( QTextCursor::NextCharacter, mode );
        return;
    }

    QTextBlock block( cursor.block() );
    QString blockText = block.text();
    int positionInBlock = cursor.position() - block.position();

    // go to end of token or end of word
    TokenIterator tokenIt( block, positionInBlock );
    if (tokenIt.isValid())
        positionInBlock = tokenIt->positionInBlock + tokenIt->length;
    else {
        int pos = positionInBlock;
        if (blockText[pos].isLetterOrNumber()) {
            ++pos;
            while (pos < blockText.size() && blockText[pos].isLetterOrNumber())
                ++pos;
        } else {
            ++pos;
        }
        positionInBlock = pos;
    }

    // skip whitespace
    while (positionInBlock < blockText.size() && blockText[positionInBlock].isSpace())
        ++positionInBlock;

    cursor.setPosition( positionInBlock + block.position(), mode );
}

void ScCodeEditor::moveToPreviousToken( QTextCursor & cursor, QTextCursor::MoveMode mode )
{
    if (cursor.atBlockStart()) {
        cursor.movePosition( QTextCursor::PreviousCharacter, mode );
        return;
    }

    QTextBlock block( cursor.block() );
    QString blockText = block.text();
    int positionInBlock = cursor.position() - block.position() - 1;

    // skip whitespace
    while (positionInBlock > 0 && blockText[positionInBlock].isSpace())
        --positionInBlock;

    cursor.setPosition(positionInBlock + block.position(), mode);
    if (positionInBlock == 0)
        return;

    // go to beginning of token or beginning of word
    TokenIterator tokenIt( block, positionInBlock );
    if (tokenIt.isValid()) {
        cursor.setPosition( tokenIt.position(), mode );
    } else {
        int pos = positionInBlock;
        if (blockText[pos].isLetterOrNumber()) {
            while (pos > 0 && blockText[pos-1].isLetterOrNumber())
                --pos;
        }
        cursor.setPosition( pos + block.position(), mode );
    }
}

bool ScCodeEditor::insertMatchingTokens( const QString & text )
{
    if (text.isEmpty())
        return false;

    QTextCursor cursor = textCursor();
    QTextDocument *document = cursor.document();
    int cursorPosition = cursor.position();

    QChar token = text[0];
    QChar matchingToken;

    static QString openingTokens("([{'\"");
    static QString closingTokens(")]}'\"");

    bool isOpeningToken, isClosingToken;

    int idx;
    if ( (idx = openingTokens.indexOf(token)) != -1 ) {
        matchingToken = closingTokens[idx];
        isOpeningToken = true;
        isClosingToken = token == matchingToken;
    }
    else if ( (idx = closingTokens.indexOf(token)) != -1 ) {
        matchingToken = openingTokens[idx];
        isClosingToken = true;
        isOpeningToken = token == matchingToken;
    }
    else
        return false;

    cursor.beginEditBlock();

    if (mInsertMatchingTokens)
    {
        if (cursor.hasSelection()) {
            if (isOpeningToken) {
                int start = cursor.selectionStart();
                int end = cursor.selectionEnd();
                cursor.setPosition(start);
                cursor.insertText(token);
                cursor.setPosition(end + 1);
                cursor.insertText(matchingToken);
            }
            else
                cursor.insertText(token);
        }
        else {
            if (isClosingToken && document->characterAt(cursorPosition) == token) {
                cursor.movePosition(QTextCursor::NextCharacter);
            }
            else if (isOpeningToken) {
                cursor.insertText(token);
                cursor.insertText(matchingToken);
                cursor.movePosition(QTextCursor::PreviousCharacter);
            }
            else
                cursor.insertText(token);
        }
    }
    else
        cursor.insertText(token);

    cursor.endEditBlock();

    cursor.setVerticalMovementX(-1);
    setTextCursor(cursor);

    if (isClosingToken)
        indent(cursor, JoinEditBlock);

    ensureCursorVisible();

    return true;
}

bool ScCodeEditor::removeMatchingTokens()
{
    QTextCursor cursor = textCursor();
    QTextDocument *document = cursor.document();
    int cursorPosition = cursor.position();
    if (cursorPosition == 0)
        return false;

    QChar previousChar = document->characterAt(cursorPosition-1);
    QChar nextChar;
    if (previousChar == '{')
        nextChar = '}';
    else if (previousChar == '[')
        nextChar = ']';
    else if (previousChar == '(')
        nextChar = ')';
    else if (previousChar == '\'' || previousChar == '"')
        nextChar = previousChar;
    else
        return false;

    if (document->characterAt(cursorPosition) != nextChar)
        return false;

    cursor.beginEditBlock();
    cursor.deletePreviousChar();
    cursor.deleteChar();
    cursor.endEditBlock();

    setTextCursor(cursor);
    return true;
}

QTextCursor ScCodeEditor::selectionForPosition( int position )
{
    QTextBlock block( textDocument()->findBlock(position) );
    if (!block.isValid())
        return QTextCursor();

    int positionInBlock = position - block.position();

    TokenIterator it = TokenIterator( block, positionInBlock );
    if (it.type() == Token::Unknown) {
        // Token is invalid, or Token::Unknown (i.e. punctuations).
        // Prefer token at previous position.
        TokenIterator alternativeIt = TokenIterator( block, positionInBlock - 1 );
        if (alternativeIt.isValid())
            it = alternativeIt;
    }

    if (it.isValid()) {
        switch (it->type) {
        case Token::OpeningBracket:
        case Token::ClosingBracket:
        {
            BracketPair match;
            matchBracket(it, match);
            if (match.first.isValid() && match.second.isValid()) {
                int start = match.first.position();
                int end = match.second.position() + 1;
                QTextCursor selection(textDocument());
                if (it == match.second) {
                    selection.setPosition(start);
                    selection.setPosition(end, QTextCursor::KeepAnchor);
                } else {
                    selection.setPosition(end);
                    selection.setPosition(start, QTextCursor::KeepAnchor);
                }
                return selection;
            }
            break;
        }

        default:
            QTextCursor selection( textDocument() );
            selection.setPosition( it.position() );
            selection.setPosition( selection.position() + it->length, QTextCursor::KeepAnchor );
            return selection;
        }
    }

    return QTextCursor();
}

void ScCodeEditor::matchBrackets()
{
    mBracketSelections.clear();

    QTextCursor cursor(textCursor());
    QTextBlock block( cursor.block() );
    int posInBlock = cursor.positionInBlock();
    TokenIterator it(block);
    while (it.isValid() && it.block() == block)
    {
        const Token & token = *it;
        if (token.positionInBlock > posInBlock) {
            it = TokenIterator();
            break;
        } else if (
            (token.positionInBlock == posInBlock && token.type == Token::OpeningBracket) ||
            (token.positionInBlock == posInBlock - 1 && token.type == Token::ClosingBracket)
        )
            break;
        ++it;
    }

    if (!it.isValid() || it.block() != block) {
        updateExtraSelections();
        return;
    }

    BracketPair match;
    matchBracket( it, match );

    if( match.first.isValid() && match.second.isValid() )
    {
        const Token & tok1 = *match.first;
        const Token & tok2 = *match.second;

        if (
            (tok1.character == '(' && tok2.character == ')')
            || (tok1.character == '[' && tok2.character == ']')
            || (tok1.character == '{' && tok2.character == '}')
        ){
            QTextEdit::ExtraSelection selection;
            selection.format = mBracketHighlight;
            cursor.setPosition(match.first.position());
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            selection.cursor = cursor;
            mBracketSelections.append(selection);

            cursor.setPosition(match.second.position());
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            selection.cursor = cursor;
            mBracketSelections.append(selection);

            if (mHighlightBracketContents) {
                QTextCharFormat format;
                format.setBackground( mBracketHighlight.background() );
                selection.format = format;
                cursor.setPosition(match.first.position()+1);
                cursor.setPosition(match.second.position(), QTextCursor::KeepAnchor);
                selection.cursor = cursor;
                mBracketSelections.append(selection);
            }
        }
        else {
            QTextEdit::ExtraSelection selection;
            selection.format = mBracketMismatchFormat;
            cursor.setPosition(match.first.position());
            cursor.setPosition(match.second.position()+1, QTextCursor::KeepAnchor);
            selection.cursor = cursor;
            mBracketSelections.append(selection);
        }
    }
    else
    {
        if ( it.type() == Token::OpeningBracket ) {
            cursor.setPosition(it.position());
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        }
        else {
            cursor.setPosition(it.position() + 1);
            cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
        }
        QTextEdit::ExtraSelection selection;
        selection.format = mBracketMismatchFormat;
        selection.cursor = cursor;
        mBracketSelections.append(selection);
    }

    updateExtraSelections();
}

int ScCodeEditor::indentedStartOfLine( const QTextBlock &b )
{
    QString t(b.text());
    int n = t.size();
    int i = 0;
    while (i < n) {
        QChar c(t[i]);
        if (c != ' ' && c != '\t')
            break;
        ++i;
    }

    return i;
}

void ScCodeEditor::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections;
    selections.append(mBracketSelections);
    selections.append(mSearchSelections);
    setExtraSelections(selections);
}

void ScCodeEditor::indentCurrentRegion()
{
    indent(currentRegion());
}

void ScCodeEditor::indent( EditBlockMode editBlockMode )
{
    indent(textCursor(), editBlockMode);
}

void ScCodeEditor::indent( const QTextCursor & selection, EditBlockMode editBlockMode )
{
    if (selection.isNull())
        return;

    QTextCursor cursor(selection);

    if (editBlockMode == NewEditBlock)
        cursor.beginEditBlock();
    else
        cursor.joinPreviousEditBlock();

    QTextDocument *doc = QPlainTextEdit::document();
    int startBlockNum = doc->findBlock(cursor.selectionStart()).blockNumber();
    int endBlockNum = cursor.hasSelection() ?
        doc->findBlock(cursor.selectionEnd()).blockNumber() : startBlockNum;

    QStack<int> stack;
    int global_level = 0;
    int blockNum = 0;
    bool in_string = false;
    QTextBlock block = QPlainTextEdit::document()->begin();
    while (block.isValid())
    {
        int initialStackSize = stack.size();
        int level = 0;
        bool block_start_in_string = in_string;

        TextBlockData *data = static_cast<TextBlockData*>(block.userData());
        if (data)
        {
            int count = data->tokens.size();
            for (int idx = 0; idx < count; ++idx)
            {
                const Token & token = data->tokens[idx];
                switch (token.type)
                {
                case Token::OpeningBracket:
                    if (token.character != '(' || stack.size() || token.positionInBlock)
                        ++level;
                    break;

                case Token::ClosingBracket:
                    if (level)
                        --level;
                    else if (global_level) {
                        --global_level;
                        if (!stack.isEmpty() && global_level < stack.top())
                            stack.pop();
                    }
                    break;

                case Token::StringMark:
                    in_string = !in_string;
                    break;

                default:
                    break;
                }
            }
        }

        if(blockNum >= startBlockNum) {
            int indentLevel;
            if (data && data->tokens.size() && data->tokens[0].type == Token::ClosingBracket)
                indentLevel = stack.size();
            else if (!block_start_in_string)
                indentLevel = initialStackSize;
            else
                indentLevel = 0;
            block = indent(block, indentLevel);
        }

        if(blockNum == endBlockNum)
            break;

        block = block.next();
        ++blockNum;

        if (level) {
            global_level += level;
            stack.push(global_level);
        }
    }

    cursor.endEditBlock();
}

QString ScCodeEditor::makeIndentationString(int level)
{
    if (level <= 0)
        return QString();

    if ( mSpaceIndent ) {
        const int spaces = mDoc->indentWidth() * level;
        QString indentationString (spaces, QChar(' '));
        return indentationString;
    } else {
        const int tabs = level;
        QString indentationString (tabs, QChar('\t'));
        return indentationString;
    }
}

QTextBlock ScCodeEditor::indent( const QTextBlock & block, int level )
{
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + indentedStartOfLine(block), QTextCursor::KeepAnchor);

    cursor.insertText(makeIndentationString(level));

    // modification has invalidated the block, so return a new one
    return cursor.block();
}

int ScCodeEditor::indentationLevel(const QTextCursor & cursor)
{
    QTextDocument *doc = QPlainTextEdit::document();
    int startBlockNum = doc->findBlock(cursor.selectionStart()).blockNumber();

    QStack<int> stack;
    int level = 0;
    int blockNum = 0;
    QTextBlock block = QPlainTextEdit::document()->begin();
    while (block.isValid()) {
        if (level > 0) {
            stack.push(level);
            level = 0;
        }

        TextBlockData *data = static_cast<TextBlockData*>(block.userData());
        if (data) {
            int count = data->tokens.size();
            for (int idx = 0; idx < count; ++idx) {
                const Token & token = data->tokens[idx];
                switch (token.type) {
                case Token::OpeningBracket:
                    if (token.character != '(' || stack.size() || token.positionInBlock)
                        level += 1;
                    break;

                case Token::ClosingBracket:
                    if (level)
                        level -= 1;
                    else if(!stack.isEmpty()) {
                        stack.top() -= 1;
                        if (stack.top() <= 0)
                            stack.pop();
                    }
                    break;

                default:
                    ;
                }
            }
        }

        if (blockNum == startBlockNum)
            return stack.size();

        block = block.next();
        ++blockNum;
    }

    return -1;
}

void ScCodeEditor::insertSpaceToNextTabStop( QTextCursor &cursor )
{
    if ( mSpaceIndent ) {
        const int indentWidth = mDoc->indentWidth();
        if (indentWidth < 1) return;
        const int spaces = indentWidth - (cursor.positionInBlock() % indentWidth);
        QString spaceString (spaces, QChar(' '));
        cursor.insertText( spaceString );
    } else {
        cursor.insertText("\t");
    }
}

void ScCodeEditor::triggerAutoCompletion()
{
    mAutoCompleter->triggerCompletion();
}

void ScCodeEditor::triggerMethodCallAid()
{
    mAutoCompleter->triggerMethodCallAid();
}

static bool isSingleLineComment(QTextBlock const & block)
{
    static QRegExp commentRegex("^\\s*//.*");
    return commentRegex.exactMatch(block.text());
}

static bool isSingleLineComment(QTextCursor const & selection)
{
    QTextCursor cursor(selection);
    cursor.setPosition(selection.selectionStart());
    QTextBlock startBlock = cursor.block();

    cursor.setPosition(selection.selectionEnd() - 1);
    QTextBlock endBlock = cursor.block();

    for (QTextBlock block = startBlock; block != endBlock.next(); block = block.next()) {
        if (!isSingleLineComment(block))
            return false;
    }

    return true;
}

static bool isSelectionComment(QString const & text)
{
    QString trimmed = text.trimmed();
    if ( trimmed.startsWith(QStringLiteral("/*")) && trimmed.endsWith(QStringLiteral("*/")) )
        return true;
    else
        return false;
}

void ScCodeEditor::toggleComment()
{
    QTextCursor cursor = textCursor();

    if (cursor.hasSelection())
        toggleCommentSelection();
    else
        toggleCommentSingleLine();
}

void ScCodeEditor::toggleCommentSingleLine()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    toggleCommentSingleLine( cursor );

    cursor.endEditBlock();
}

void ScCodeEditor::addSingleLineComment(QTextCursor cursor, int indentation)
{
    QTextBlock currentBlock(cursor.block());
    int blockIndentationLevel = indentationLevel(cursor);

    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + indentedStartOfLine(currentBlock), QTextCursor::KeepAnchor);

    QString commentString = makeIndentationString(indentation) + QStringLiteral("// ")
                            + makeIndentationString(blockIndentationLevel - indentation);

    cursor.insertText(commentString);

    cursor.movePosition(QTextCursor::StartOfBlock);
}

void ScCodeEditor::removeSingleLineComment(QTextCursor cursor)
{
    QTextBlock currentBlock(cursor.block());
    cursor.movePosition(QTextCursor::StartOfBlock);
    const int startPosition = cursor.position();
    const int indentPosition = indentedStartOfLine(currentBlock);
    const int commentStartPosition = startPosition + indentPosition;

    cursor.setPosition(commentStartPosition);
    cursor.setPosition(commentStartPosition + 3, QTextCursor::KeepAnchor);

    if (!cursor.selectedText().endsWith(QStringLiteral("// "))) {
        cursor.setPosition(commentStartPosition + 2, QTextCursor::KeepAnchor);
        if (!cursor.selectedText().endsWith(QStringLiteral("//")))
            return;
    }

    cursor.insertText("");
}

void ScCodeEditor::toggleCommentSingleLine(QTextCursor cursor)
{
    QTextBlock currentBlock(cursor.block());

    cursor.beginEditBlock();

    if (!isSingleLineComment(currentBlock)) {
        int blockIndentation = indentationLevel(cursor);
        addSingleLineComment(cursor, blockIndentation);
    } else
        removeSingleLineComment(cursor);

    cursor.endEditBlock();
    indent(cursor);
}

static bool isBlockOnlySelection(QTextCursor cursor)
{
    Q_ASSERT(cursor.hasSelection());

    QTextCursor begin(cursor);
    begin.setPosition(begin.anchor());

    if (begin.atBlockStart() && (cursor.atBlockStart() || cursor.atBlockEnd()))
        return true;
    else
        return false;
}

void ScCodeEditor::toggleCommentSelection()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    if (isBlockOnlySelection(cursor)) {
        const bool isComment = isSingleLineComment(cursor);

        QTextCursor selectionCursor(cursor);
        selectionCursor.setPosition(cursor.selectionStart());

        QTextBlock currentBlock = selectionCursor.block();
        int firstBlockIndentation = isComment ? 0
                                              : indentationLevel(selectionCursor);

        do {
            QTextCursor blockCursor(currentBlock);
            if (!isComment)
                addSingleLineComment(blockCursor, firstBlockIndentation);
            else
                removeSingleLineComment(blockCursor);
            currentBlock = currentBlock.next();
        } while (currentBlock.isValid() && currentBlock.position() < cursor.selectionEnd());

        if (!isComment) {
            // fix up selection
            QTextCursor newSelection(cursor);
            if (cursor.anchor() < cursor.position()) {
                newSelection.setPosition(newSelection.selectionStart());
                newSelection.movePosition(QTextCursor::StartOfBlock);
                newSelection.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
            } else {
                newSelection.setPosition(newSelection.selectionEnd());
                newSelection.setPosition(cursor.selectionStart(), QTextCursor::KeepAnchor);
                newSelection.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            }
            setTextCursor(newSelection);
        }
    } else {
        QString selectionText = cursor.selectedText();
        QTextCursor selectionCursor(cursor);
        if (isSelectionComment(selectionText)) {
            selectionText = selectionText.trimmed().remove(0, 2);
            selectionText.chop(2);
            selectionCursor.insertText(selectionText);
        } else {
            selectionText = QStringLiteral("/*") + selectionText + QStringLiteral("*/");
            selectionCursor.insertText(selectionText);
        }

        // fix up selection
        const int position = selectionCursor.position();
        const int anchor   = selectionCursor.anchor();
        if (position > anchor) {
            cursor.setPosition(position - selectionText.size());
            cursor.setPosition(position, QTextCursor::KeepAnchor);
        } else {
            cursor.setPosition(position);
            cursor.setPosition(position - selectionText.size(), QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }

    cursor.endEditBlock();
}

// taking nested brackets into account
TokenIterator ScCodeEditor::previousOpeningBracket(TokenIterator it)
{
    int level = 0;
    while (it.isValid()) {
        switch (it->type) {
        case Token::OpeningBracket:
            if (level == 0)
                return it;
            --level;
            break;

        case Token::ClosingBracket:
            ++level;

        default:
            break;
        }
        --it;
    }
    return it;
}

// taking nested brackets into account
TokenIterator ScCodeEditor::nextClosingBracket(TokenIterator it)
{
    int level = 0;
    while (it.isValid()) {
        switch (it->type) {
        case Token::ClosingBracket:
            if (level == 0)
                return it;
            --level;
            break;

        case Token::OpeningBracket:
            ++level;

        default:
            break;
        }
        ++it;
    }
    return it;
}

void ScCodeEditor::matchBracket( const TokenIterator & bracket, BracketPair & match )
{
    Q_ASSERT(bracket.isValid());

    switch(bracket->type) {
    case Token::OpeningBracket:
        match.first = bracket;
        match.second = nextClosingBracket(bracket.next());
        break;
    case Token::ClosingBracket:
        match.second = bracket;
        match.first = previousOpeningBracket(bracket.previous());
        break;
    default:
        match.first = TokenIterator();
        match.second = TokenIterator();
    }
}

void ScCodeEditor::nextBracketPair( const TokenIterator & startIt, BracketPair & bracketPair )
{
    TokenIterator it( startIt );
    while (it.isValid()) {
        if (it->type == Token::OpeningBracket) {
            matchBracket( it, bracketPair );
            return;
        }
        ++it;
    }
    bracketPair = BracketPair();
}

void ScCodeEditor::gotoNextBlock()
{
    QTextCursor cursor = textCursor();

    TokenIterator tokenIt = TokenIterator::rightOf( cursor.block(), cursor.positionInBlock() );
    if (tokenIt.type() == Token::OpeningBracket
            && tokenIt.block() == cursor.block()
            && tokenIt->positionInBlock == cursor.positionInBlock())
        ++tokenIt;

    tokenIt = nextClosingBracket( tokenIt );

    if (tokenIt.isValid())
        setTextCursor( cursorAt(tokenIt, 1) );
    else {
        cursor.movePosition( QTextCursor::End );
        setTextCursor( cursor );
    }
}

void ScCodeEditor::gotoPreviousBlock()
{
    QTextCursor cursor = textCursor();

    TokenIterator tokenIt = TokenIterator::leftOf(cursor.block(), cursor.positionInBlock());
    if (tokenIt.type() == Token::ClosingBracket
            && tokenIt.block() == cursor.block()
            && tokenIt->positionInBlock == cursor.positionInBlock() - 1)
        --tokenIt;


    tokenIt = previousOpeningBracket( tokenIt );

    if (tokenIt.isValid())
        setTextCursor( cursorAt(tokenIt) );
    else {
        cursor.movePosition( QTextCursor::Start );
        setTextCursor( cursor );
    }
}

QTextCursor ScCodeEditor::blockAroundCursor( const QTextCursor & cursor )
{
    TokenIterator left_bracket =
            previousOpeningBracket(
                TokenIterator::leftOf(cursor.block(), cursor.positionInBlock()) );
    if (!left_bracket.isValid())
        return QTextCursor();
    TokenIterator right_bracket =
            nextClosingBracket(
                TokenIterator::rightOf(cursor.block(), cursor.positionInBlock()) );
    if (!right_bracket.isValid())
        return QTextCursor();

    QTextCursor block_cursor = cursor;
    block_cursor.setPosition( left_bracket.position() );
    block_cursor.setPosition( right_bracket.position() + 1, QTextCursor::KeepAnchor );
    return block_cursor;
}

void ScCodeEditor::selectBlockAroundCursor()
{
    QTextCursor block_cursor = blockAroundCursor(textCursor());
    if (!block_cursor.isNull())
        setTextCursor(block_cursor);
}

inline static bool tokenMaybeRegionStart( const TokenIterator & it )
{
    Q_ASSERT(it.isValid());
    return ( it->character == '(' && it->positionInBlock == 0 );
}

inline static bool tokenMaybeRegionEnd( const TokenIterator & it )
{
    Q_ASSERT(it.isValid());
    if (it->character != ')')
        return false;
    TokenIterator next_it = it.next();
    return (!next_it.isValid() ||
            next_it.block() != it.block() ||
            next_it->character == ';');
}

static bool bracketPairDefinesRegion( const BracketPair & bracketPair )
{
    Q_ASSERT(bracketPair.first.isValid());
    Q_ASSERT(bracketPair.second.isValid());

    if (!tokenMaybeRegionStart(bracketPair.first) || !tokenMaybeRegionEnd(bracketPair.second))
        return false;

    return true;
}

QTextCursor ScCodeEditor::regionAroundCursor(const QTextCursor & cursor)
{
    int cursorPosition = cursor.position();

    BracketPair bracketPair;
    TokenIterator it = TokenIterator::rightOf( textDocument()->begin(), 0 );
    while (it.isValid()) {
        nextBracketPair(it, bracketPair);
        if (bracketPair.first.isValid() && bracketPair.first.position() < cursorPosition)
        {
            if ( bracketPair.second.isValid() && bracketPair.second.position() >= cursorPosition
                 && bracketPairDefinesRegion(bracketPair) )
            {
                QTextCursor regionCursor(QPlainTextEdit::document());
                regionCursor.setPosition(bracketPair.first.position());
                regionCursor.setPosition(bracketPair.second.position() + 1, QTextCursor::KeepAnchor);
                return regionCursor;
            }
        } else {
            break;
        }

        it = bracketPair.second;
    }

    return QTextCursor();
}

QTextCursor ScCodeEditor::currentRegion()
{
    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    int positionInBlock = cursor.positionInBlock();

    if (TokenIterator(block, positionInBlock - 1).type() == Token::ClosingBracket)
        cursor.movePosition( QTextCursor::PreviousCharacter );
    else if (TokenIterator(block, positionInBlock).type() == Token::OpeningBracket)
        cursor.movePosition( QTextCursor::NextCharacter );

    return regionAroundCursor( cursor );
}

void ScCodeEditor::selectCurrentRegion()
{
    QTextCursor selectedRegionCursor = currentRegion();
    if (!selectedRegionCursor.isNull() && selectedRegionCursor.hasSelection())
        setTextCursor(selectedRegionCursor);
}

void ScCodeEditor::gotoNextRegion()
{
    QTextCursor cursor = textCursor();
    int cursorPosition = cursor.position();

    BracketPair bracketPair;
    TokenIterator it = TokenIterator::rightOf( textDocument()->begin(), 0 );
    while (it.isValid()) {
        nextBracketPair(it, bracketPair);
        if ( bracketPair.first.isValid() && bracketPair.second.isValid()
             && bracketPair.first.position() > cursorPosition
             && bracketPairDefinesRegion(bracketPair))
        {
            setTextCursor( cursorAt(bracketPair.first) );
            return;
        }
        it = bracketPair.second;
    }

    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void ScCodeEditor::gotoPreviousRegion()
{
    QTextCursor cursor = textCursor();
    int cursorPosition = cursor.position();

    BracketPair prevBracketPair;
    BracketPair bracketPair;
    TokenIterator it = TokenIterator::rightOf( textDocument()->begin(), 0 );
    while(it.isValid()) {
        nextBracketPair(it, bracketPair);
        if ( !bracketPair.second.isValid()
             || bracketPair.second.position() >= cursorPosition - 1 )
        {
            break;
        }
        if (bracketPairDefinesRegion(bracketPair)) {
            prevBracketPair = bracketPair;
        }
        it = bracketPair.second;
    }

    if ( prevBracketPair.first.isValid() && prevBracketPair.second.isValid() )
    {
        setTextCursor( cursorAt(prevBracketPair.second, 1) );
        return;
    }

    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);
}

bool ScCodeEditor::openDocumentation()
{
    return Main::openDocumentation(symbolUnderCursor());
}

void ScCodeEditor::openDefinition()
{
    Main::openDefinition(symbolUnderCursor(), this);
}

void ScCodeEditor::openCommandLine()
{
    Main::openCommandLine(symbolUnderCursor());
}

void ScCodeEditor::findReferences()
{
    Main::findReferences(symbolUnderCursor(), this);
}

void ScCodeEditor::evaluateLine()
{
    QString text;

    // Try current selection
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        text = cursor.selectedText();
    else {
        text = cursor.block().text();

        if( mStepForwardEvaluation ) {
            QTextCursor newCursor = cursor;
            newCursor.movePosition(QTextCursor::NextBlock);
            newCursor.setVerticalMovementX( cursor.verticalMovementX() );
            setTextCursor(newCursor);
        }

        // Adjust cursor for code blinking:
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }

    if (text.isEmpty())
        return;

    text.replace( QChar( 0x2029 ), QChar( '\n' ) );

    Main::evaluateCode(text);

    blinkCode( cursor );
}

void ScCodeEditor::evaluateRegion()
{
    QString text;

    // Try current selection
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        text = cursor.selectedText();
    else {
        // If no selection, try current region
        cursor = currentRegion();
        if (!cursor.isNull()) {
            text = cursor.selectedText();
        } else {
            // If no current region, try current line
            cursor = textCursor();
            text = cursor.block().text();
            if( mStepForwardEvaluation ) {
                QTextCursor newCursor = cursor;
                newCursor.movePosition(QTextCursor::NextBlock);
                newCursor.setVerticalMovementX( cursor.verticalMovementX() );
                setTextCursor(newCursor);
            }
            // Adjust cursor for code blinking:
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
    }

    if (text.isEmpty())
        return;

    text.replace( QChar( 0x2029 ), QChar( '\n' ) );

    Main::evaluateCode(text);

    blinkCode( cursor );
}

void ScCodeEditor::evaluateDocument()
{
    QString documentText = textDocument()->toPlainText();
    Main::evaluateCode(documentText);
}

QTextCursor ScCodeEditor::cursorAt(const TokenIterator it, int offset)
{
    Q_ASSERT(it.isValid());

    QTextCursor textCursor(textDocument());
    textCursor.setPosition(it.position() + offset);

    return textCursor;
}

} // namespace ScIDE
