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
#include "../../core/main.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/settings/manager.hpp"

#include <QKeyEvent>
#include <QStack>

namespace ScIDE {

static void matchBracket( const TokenIterator & bracket, BracketPair & match );

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

    applySettings(Main::settings());
}

void ScCodeEditor::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/editor");

    mSpaceIndent = settings->value("spaceIndent").toBool();

    mBlinkDuration = settings->value("blinkDuration").toInt();

    QPalette palette;

    settings->beginGroup("colors");

    if (settings->contains("text")) {
        QTextCharFormat format = settings->value("text").value<QTextCharFormat>();
        QBrush bg = format.background();
        QBrush fg = format.foreground();
        if (bg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Base, bg);
        if (fg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Text, fg);
    }

    if (settings->contains("lineNumbers")) {
        QPalette lineNumPlt;
        QTextCharFormat format = settings->value("lineNumbers").value<QTextCharFormat>();
        QBrush bg = format.background();
        QBrush fg = format.foreground();
        if (bg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Button, bg);
        if (fg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::ButtonText, fg);
        mLineIndicator->setPalette(lineNumPlt);
    }

    mBracketHighlight = settings->value("matchingBrackets").value<QTextCharFormat>();

    settings->endGroup(); // colors

    setPalette(palette);

    settings->endGroup();
}

bool ScCodeEditor::event( QEvent *e )
{
    switch (e->type())
    {
        case QEvent::KeyPress:
        {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            int key = ke->key();
            switch (key)
            {
            case Qt::Key_Tab:
                indent();
                e->accept();
                return true;
            default:;
            }
            break;
    }
    default:;
    }
    return QPlainTextEdit::event(e);
}

void ScCodeEditor::keyPressEvent( QKeyEvent *e )
{
    switch (e->key()) {
    case Qt::Key_Home:
    {
        Qt::KeyboardModifiers mods(e->modifiers());
        if (mods && mods != Qt::ShiftModifier) {
            GenericCodeEditor::keyPressEvent(e);
            return;
        }

        hideMouseCursor();

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

        return;
    }

    case Qt::Key_Backtab:
    {
        hideMouseCursor();
        QTextCursor cursor = textCursor();
        cursor.insertText("\t");
        ensureCursorVisible();
        return;
    }

    default:;
    }

    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_BraceRight:
    case Qt::Key_BracketRight:
    case Qt::Key_ParenRight: {
        // Wrap superclass' implementation into an edit block,
        // so it can be joined with indentation later:

        QTextCursor cursor = textCursor();

        cursor.beginEditBlock();
        GenericCodeEditor::keyPressEvent(e);
        cursor.endEditBlock();

        cursor.joinPreviousEditBlock();
        indent();
        cursor.endEditBlock();

        cursor.setVerticalMovementX(-1);
        setTextCursor(cursor);

        break;
    }
    default:
        GenericCodeEditor::keyPressEvent(e);
    }

    mAutoCompleter->keyPress(e);
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
    QTextCursor cursor = cursorForPosition(e->pos());
    QTextCursor selection = blockAtCursor(cursor);

    if (!selection.isNull()) {
        setTextCursor(selection);
        return;
    }

    GenericCodeEditor::mouseDoubleClickEvent(e);
}

void ScCodeEditor::mouseMoveEvent( QMouseEvent *e )
{
    // Prevent initiating a text drag:
    if(!mMouseBracketMatch)
        GenericCodeEditor::mouseMoveEvent(e);
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

    BracketPair match;
    if( it.isValid() && it.block() == block)
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
        }
        else {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(Qt::red);
            cursor.setPosition(match.first.position());
            cursor.setPosition(match.second.position()+1, QTextCursor::KeepAnchor);
            selection.cursor = cursor;
            mBracketSelections.append(selection);
        }
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

void ScCodeEditor::indent()
{
    indent(textCursor());
}

void ScCodeEditor::indent( const QTextCursor & selection )
{
    if (selection.isNull())
        return;

    QTextCursor cursor(selection);

    cursor.beginEditBlock();

    QTextDocument *doc = QPlainTextEdit::document();
    int startBlockNum = doc->findBlock(cursor.selectionStart()).blockNumber();
    int endBlockNum = cursor.hasSelection() ?
        doc->findBlock(cursor.selectionEnd()).blockNumber() : startBlockNum;

    QStack<int> stack;
    int level = 0;
    int blockNum = 0;
    QTextBlock block = QPlainTextEdit::document()->begin();
    while (block.isValid())
    {
        if (level > 0) {
            stack.push(level);
            level = 0;
        }

        int initialStackSize = stack.size();

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

        if(blockNum >= startBlockNum) {
            int indentLevel;
            if (data && data->tokens.size() && data->tokens[0].type == Token::ClosingBracket)
                indentLevel = stack.size();
            else
                indentLevel = initialStackSize;
            block = indent(block, indentLevel);
        }

        if(blockNum == endBlockNum)
            break;

        block = block.next();
        ++blockNum;
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

static bool isSelectionComment(QString const & text)
{
    QString trimmed = text.trimmed();
    if ( trimmed.startsWith(QString("/*")) && trimmed.endsWith(QString("*/")) )
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

    QString commentString = makeIndentationString(indentation) + QString("// ")
                            + makeIndentationString(blockIndentationLevel - indentation);

    cursor.insertText(commentString);

    cursor.movePosition(QTextCursor::StartOfBlock);
}

void ScCodeEditor::removeSingleLineComment(QTextCursor cursor)
{
    QTextBlock currentBlock(cursor.block());
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + indentedStartOfLine(currentBlock) + 2, QTextCursor::KeepAnchor);

    if (!cursor.selectedText().endsWith(QString("//")))
        cursor.setPosition(cursor.anchor() + indentedStartOfLine(currentBlock), QTextCursor::KeepAnchor);

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
        QTextCursor selectionCursor(cursor);
        selectionCursor.setPosition(cursor.selectionStart());

        QTextBlock currentBlock = selectionCursor.block();
        bool isComment = isSingleLineComment(currentBlock);
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
    } else {
        QString selectionText = cursor.selectedText();
        QTextCursor selectionCursor(cursor);
        if (isSelectionComment(selectionText)) {
            selectionText = selectionText.trimmed().remove(0, 2);
            selectionText.chop(2);
            selectionCursor.insertText(selectionText);
        } else {
            selectionText = QString("/* ") + selectionText + QString(" */");
            selectionCursor.insertText(selectionText);

            int position = selectionCursor.position();
            cursor.setPosition(position - selectionText.size());
            cursor.setPosition(position, QTextCursor::KeepAnchor);
            setTextCursor(cursor);
        }
    }

    cursor.endEditBlock();
}

// taking nested brackets into account
static TokenIterator previousOpeningBracket(TokenIterator it)
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
static TokenIterator nextClosingBracket(TokenIterator it)
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

static void matchBracket( const TokenIterator & bracket, BracketPair & match )
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

static void nextBracketPair( const TokenIterator & startIt, BracketPair & bracketPair )
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

inline static bool bracketPairContainsPosition( const BracketPair & bracketPair, int position )
{
    bool result =
            bracketPair.first.isValid() && bracketPair.second.isValid()
            && bracketPair.first.position() < position
            && bracketPair.second.position() >= position;
    return result;
}

QTextCursor ScCodeEditor::blockAtCursor(const QTextCursor & cursor)
{
    TokenIterator it (cursor.block(), cursor.positionInBlock());

    if (it.isValid()) {
        switch (it->type) {
        case Token::OpeningBracket:
        case Token::ClosingBracket:
        {
            BracketPair match;
            matchBracket(it, match);

            if (match.first.isValid() && match.second.isValid()) {
                QTextCursor selection(textDocument());
                selection.setPosition(match.first.position());
                selection.setPosition(match.second.position() + 1, QTextCursor::KeepAnchor);
                return selection;
            }
            break;
        }

        default:
            break;
        }
    }

    return QTextCursor();
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

inline static bool tokenIsFirstAndOnlyInBlock( const TokenIterator & it )
{
    Q_ASSERT(it.isValid());
    bool result = it->positionInBlock == 0;
    result = result && static_cast<TextBlockData*>(it.block().userData())->tokens.size() == 1;
    return result;
}

static bool bracketPairDefinesRegion( const BracketPair & bracketPair )
{
    Q_ASSERT(bracketPair.first.isValid());
    Q_ASSERT(bracketPair.second.isValid());

    if ( bracketPair.first->character != '(' || bracketPair.second->character != ')' )
        return false;

    if (!tokenIsFirstAndOnlyInBlock(bracketPair.second) || !tokenIsFirstAndOnlyInBlock(bracketPair.second))
        return false;

    // check whether this is an Event
    TokenIterator it = bracketPair.first.next();
    if (it.isValid()) {
        if (it->type == Token::SymbolArg)
            return false;
        else {
            ++it;
            if (it.isValid() && it->character == ':')
                return false;
        }
    }

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
                regionCursor.setPosition(bracketPair.first.position() + 1);
                regionCursor.setPosition(bracketPair.second.position(), QTextCursor::KeepAnchor);
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

void ScCodeEditor::findReferences()
{
    Main::findReferences(symbolUnderCursor(), this);
}

QTextCursor ScCodeEditor::cursorAt(const TokenIterator it, int offset)
{
    Q_ASSERT(it.isValid());

    QTextCursor textCursor(textDocument());
    textCursor.setPosition(it.position() + offset);

    return textCursor;
}

} // namespace ScIDE
