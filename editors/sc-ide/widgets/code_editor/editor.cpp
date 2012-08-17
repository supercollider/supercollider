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

#include "editor.hpp"
#include "highlighter.hpp"
#include "autocompleter.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/main.hpp"
#include "../../core/settings/manager.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>

namespace ScIDE
{

LineIndicator::LineIndicator( CodeEditor *editor )
: QWidget( editor ), mEditor(editor)
{}

void LineIndicator::setLineCount( int count )
{
    mLineCount = count;
    setFixedWidth( widthForLineCount(count) );
    Q_EMIT( widthChanged() );
}

void LineIndicator::changeEvent( QEvent *e )
{
    if( e->type() == QEvent::FontChange ) {
        setFixedWidth( widthForLineCount(mLineCount) );
        Q_EMIT( widthChanged() );
    }
    else
        QWidget::changeEvent(e);
}

void LineIndicator::paintEvent( QPaintEvent *e )
{ mEditor->paintLineIndicator(e); }

void LineIndicator::mousePressEvent( QMouseEvent *e )
{
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    if(cursor.isNull()) {
        mLastCursorPos = -1;
        return;
    }
    mEditor->setTextCursor(cursor);
    mLastCursorPos = cursor.position();
}

void LineIndicator::mouseMoveEvent( QMouseEvent *e )
{
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    if(cursor.isNull() || cursor.position() == mLastCursorPos)
        return;
    QTextCursor origCursor = mEditor->textCursor();
    origCursor.setPosition( cursor.position(), QTextCursor::KeepAnchor );
    mEditor->setTextCursor(origCursor);
    mLastCursorPos = cursor.position();
    // The selectionChanged() signal of the editor does not always fire here!
    // Qt bug?
    update();
}

void LineIndicator::mouseDoubleClickEvent( QMouseEvent *e )
{
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
    mEditor->setTextCursor(cursor);
}

int LineIndicator::widthForLineCount( int lineCount )
{
    int digits = 2;
    while( lineCount >= 100 ) {
        lineCount /= 10;
        ++digits;
    }

    return 6 + fontMetrics().width('9') * digits;
}

CodeEditor::CodeEditor( QWidget *parent ) :
    QPlainTextEdit( parent ),
    mLineIndicator( new LineIndicator(this) ),
    mDoc(0),
    mIndentWidth(4),
    mSpaceIndent(true),
    mShowWhitespace(false),
    mMouseBracketMatch(false),
    mOverlay( new QGraphicsScene(this) ),
    mAutoCompleter( new AutoCompleter(this) )
{
    mLineIndicator->move( contentsRect().topLeft() );

    connect( this, SIGNAL(blockCountChanged(int)),
             mLineIndicator, SLOT(setLineCount(int)) );

    connect( mLineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );
    connect( this, SIGNAL(selectionChanged()),
             mLineIndicator, SLOT(update()) );

    connect( this, SIGNAL(cursorPositionChanged()),
             this, SLOT(matchBrackets()) );

    connect( mOverlay, SIGNAL(changed(const QList<QRectF>&)),
             this, SLOT(onOverlayChanged(const QList<QRectF>&)) );

    mLineIndicator->setLineCount(1);
}

void CodeEditor::setDocument( Document *doc )
{
    QTextDocument *tdoc = doc->textDocument();
    new SyntaxHighlighter(tdoc);

    QFontMetricsF fm(font());

    QTextOption opt;
    opt.setTabStop( fm.width(' ') * mIndentWidth );
    if(mShowWhitespace)
        opt.setFlags( QTextOption::ShowTabsAndSpaces );

    tdoc->setDefaultTextOption(opt);
    tdoc->setDefaultFont(font());
    tdoc->setDocumentLayout( new QPlainTextDocumentLayout(tdoc) );

    QPlainTextEdit::setDocument(tdoc);

    mLineIndicator->setLineCount( tdoc->blockCount() );

    mDoc = doc;

    mAutoCompleter->documentChanged(tdoc);
}

void CodeEditor::setIndentWidth( int width )
{
    mIndentWidth = width;

    QTextDocument *tdoc = QPlainTextEdit::document();

    QFontMetricsF fm(font());

    QTextOption opt;
    opt.setTabStop( fm.width(' ') * mIndentWidth );
    if(mShowWhitespace)
        opt.setFlags( QTextOption::ShowTabsAndSpaces );

    tdoc->setDefaultTextOption(opt);
}

static bool findInBlock(QTextDocument *doc, const QTextBlock &block, const QRegExp &expr, int offset,
                        QTextDocument::FindFlags options, QTextCursor &cursor)
{
    QString text = block.text();
    if(options & QTextDocument::FindBackward)
        text.truncate(offset);
    text.replace(QChar::Nbsp, QLatin1Char(' '));

    int idx = -1;
    while (offset >=0 && offset <= text.length()) {
        idx = (options & QTextDocument::FindBackward) ?
               expr.lastIndexIn(text, offset) : expr.indexIn(text, offset);
        if (idx == -1)
            return false;

        if (options & QTextDocument::FindWholeWords) {
            const int start = idx;
            const int end = start + expr.matchedLength();
            if ((start != 0 && text.at(start - 1).isLetterOrNumber())
                || (end != text.length() && text.at(end).isLetterOrNumber())) {
                //if this is not a whole word, continue the search in the string
                offset = (options & QTextDocument::FindBackward) ? idx-1 : end+1;
                idx = -1;
                continue;
            }
        }
        //we have a hit, return the cursor for that.
        break;
    }

    if (idx == -1)
        return false;

    cursor = QTextCursor(doc);
    cursor.setPosition(block.position() + idx);
    cursor.setPosition(cursor.position() + expr.matchedLength(), QTextCursor::KeepAnchor);
    return true;
}

bool CodeEditor::find( const QRegExp &expr, QTextDocument::FindFlags options )
{
    // Although QTextDocument provides a find() method, we implement
    // our own, because the former one is not adequate.

    if(expr.isEmpty()) return true;

    bool backwards = options & QTextDocument::FindBackward;

    QTextCursor c( textCursor() );
    int pos;
    if (c.hasSelection())
    {
        bool matching = expr.exactMatch(c.selectedText());

        if( backwards == matching )
            pos = c.selectionStart();
        else
            pos = c.selectionEnd();
    }
    else
        pos = c.position();

    QTextDocument *doc = QPlainTextEdit::document();
    QTextBlock startBlock = doc->findBlock(pos);
    int startBlockOffset = pos - startBlock.position();

    QTextCursor cursor;

    if (!backwards) {
        int blockOffset = startBlockOffset;
        QTextBlock block = startBlock;
        while (block.isValid()) {
            if (findInBlock(doc, block, expr, blockOffset, options, cursor))
                break;
            blockOffset = 0;
            block = block.next();
        }
        if(cursor.isNull())
        {
            blockOffset = 0;
            block = doc->begin();
            while(true) {
                if (findInBlock(doc, block, expr, blockOffset, options, cursor)
                    || block == startBlock)
                    break;
                block = block.next();
            }
        }
    } else {
        int blockOffset = startBlockOffset;
        QTextBlock block = startBlock;
        while (block.isValid()) {
            if (findInBlock(doc, block, expr, blockOffset, options, cursor))
                break;
            block = block.previous();
            blockOffset = block.length() - 1;
        }
        if(cursor.isNull())
        {
            block = doc->end();
            while(true) {
                blockOffset = block.length() - 1;
                if (findInBlock(doc, block, expr, blockOffset, options, cursor)
                    || block == startBlock)
                    break;
                block = block.previous();
            }
        }
    }

    if(!cursor.isNull()) {
        QTextEdit::ExtraSelection extraSelection;
        extraSelection.cursor = cursor;
        extraSelection.format.setBackground(Qt::yellow);

        cursor.setPosition(cursor.selectionEnd());
        setTextCursor(cursor);

        QList<QTextEdit::ExtraSelection> selections = extraSelections();
        selections.append(extraSelection);
        setExtraSelections(selections);

        return true;
    }
    else
        return false;
}

int CodeEditor::findAll( const QRegExp &expr, QTextDocument::FindFlags options )
{
    mSearchSelections.clear();

    if(expr.isEmpty()) {
        updateExtraSelections();
        return 0;
    }

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(Qt::darkYellow);

    QTextDocument *doc = QPlainTextEdit::document();
    QTextBlock block = doc->begin();
    QTextCursor cursor;

    while (block.isValid()) {
        int blockPos = block.position();
        int offset = 0;
        while(findInBlock(doc, block, expr, offset, options, cursor))
        {
            offset = cursor.selectionEnd() - blockPos;
            selection.cursor = cursor;
            mSearchSelections.append(selection);
        }
        block = block.next();
    }

    updateExtraSelections();

    return mSearchSelections.count();
}

//#define CSTR(QSTR) QSTR.toStdString().c_str()

static QString resolvedReplacement( const QString &replacement, const QRegExp &expr )
{
    //qDebug("START");
    static const QRegExp rexpr("(\\\\\\\\)|(\\\\[0-9]+)");
    QString str( replacement );
    int i=0;
    while(i < str.size() && ((i = rexpr.indexIn(str, i)) != -1))
    {
        int len = rexpr.matchedLength();
        if(rexpr.pos(1) != -1)
        {
            //qDebug("%i (%s): escape", i, CSTR(rexpr.cap(1)));
            str.replace(i, len, "\\");
            i += 1;
        }
        else if(rexpr.pos(2) != -1)
        {
            QString num_str = rexpr.cap(2);
            num_str.remove(0, 1);
            int num = num_str.toInt();
            //qDebug("%i (%s): backref = %i", i, CSTR(rexpr.cap(2)), num);
            if(num <= expr.captureCount())
            {
                QString cap = expr.cap(num);
                //qDebug("resolving ref to: %s", CSTR(cap));
                str.replace(i, len, cap);
                i += cap.size();
            }
            else
            {
                //qDebug("ref out of range", i, num);
                str.remove(i, len);
            }
        }
        else
        {
            //qDebug("%i (%s): unknown match", i, CSTR(rexpr.cap(0)));
            str.remove(i, len);
        }
        //qDebug(">> [%s] %i", CSTR(str), i);
    }
    //qDebug("END");
    return str;
}

bool CodeEditor::replace( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options )
{
    if(expr.isEmpty()) return true;

    QTextCursor cursor = textCursor();
    if (cursor.hasSelection() && expr.exactMatch(cursor.selectedText()))
    {
        QString rstr = replacement;
        if(expr.patternSyntax() != QRegExp::FixedString)
            rstr = resolvedReplacement(rstr, expr);
        cursor.insertText(rstr);
    }

    return find(expr, options);
}

int CodeEditor::replaceAll( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options )
{
    mSearchSelections.clear();
    updateExtraSelections();

    if(expr.isEmpty()) return 0;

    int replacements = 0;
    bool caps = expr.patternSyntax() != QRegExp::FixedString;

    QTextDocument *doc = QPlainTextEdit::document();
    QTextBlock block = doc->begin();
    QTextCursor cursor;

    QTextCursor(doc).beginEditBlock();

    while (block.isValid())
    {
        int blockPos = block.position();
        int offset = 0;
        while(findInBlock(doc, block, expr, offset, options, cursor))
        {
            QString rstr = replacement;
            if(caps)
                rstr = resolvedReplacement(rstr, expr);
            cursor.insertText(rstr);
            ++replacements;
            offset = cursor.selectionEnd() - blockPos;
        }
        block = block.next();
    }

    QTextCursor(doc).endEditBlock();

    return replacements;
}

QTextCursor CodeEditor::currentRegion()
{
    QTextCursor c(textCursor());
    QTextBlock block(c.block());

    int positionInBlock = c.position() - block.position();
    TokenIterator start;
    TokenIterator end;
    int topLevel = 0;
    int level = 0;

    // search unmatched opening bracket
    TokenIterator it = TokenIterator::leftOf( block, positionInBlock );
    while(it.isValid())
    {
        char chr = it->character;
        if(chr == '(') {
            ++level;
            if(level > topLevel) {
                topLevel = level;
                start = it;
            }
        }
        else if(chr == ')') {
            --level;
        }
        --it;
    }

    if(topLevel < 1)
        // no unmatched opening bracket
        return QTextCursor();

    // match the found opening bracket
    it = TokenIterator::rightOf( block, positionInBlock );
    while(it.isValid())
    {
        char chr = it->character;
        if(chr == '(')
            ++topLevel;
        else if(chr == ')')
        {
            --topLevel;
            if(topLevel == 0)
            {
                end = it;
                break;
            }
        }
        ++it;
    }

    if(start.isValid() && end.isValid())
    {
        // only care about brackets at beginning of a line
        if(start->positionInBlock != 0)
            return QTextCursor();

        // check whether the bracket makes part of an event
        it = start.next();
        if (it.isValid()) {
            if (it->type == Token::SymbolArg)
                return QTextCursor();
            else {
                ++it;
                if (it.isValid() && it->character == ':')
                    return QTextCursor();
            }
        }

        // ok, this is is a real top-level region
        QTextCursor c(QPlainTextEdit::document());
        c.setPosition(start.position() + 1);
        c.setPosition(end.position(), QTextCursor::KeepAnchor);
        return c;
    }

    return QTextCursor();
}

void CodeEditor::showPosition( int pos )
{
    if (pos < 0) return;

    QTextDocument *doc = QPlainTextEdit::document();
    if (!doc) return;

    int lineNumber = doc->findBlock(pos).firstLineNumber();
    verticalScrollBar()->setValue(lineNumber);

    QTextCursor cursor(doc);
    cursor.setPosition(pos);
    setTextCursor(cursor);
}

void CodeEditor::clearSearchHighlighting()
{
    mSearchSelections.clear();
    updateExtraSelections();
}

void CodeEditor::zoomIn(int steps)
{
    zoomFont(steps);
}

void CodeEditor::zoomOut(int steps)
{
    zoomFont(-steps);
}

void CodeEditor::resetFontSize()
{
    Settings::Manager *settings = Main::instance()->settings();

    settings->beginGroup("IDE/editor");
    QFont fnt;
    fnt.fromString( settings->value("font").toString() );
    setFont(fnt);
    settings->endGroup();
}

void CodeEditor::zoomFont(int steps)
{
    QFont currentFont = font();
    const int newSize = currentFont.pointSize() + steps;
    if (newSize <= 0)
        return;
    currentFont.setPointSize(newSize);
    setFont(currentFont);
}

void CodeEditor::setShowWhitespace(bool show)
{
    mShowWhitespace = show;

    QTextDocument *doc = QPlainTextEdit::document();
    QTextOption opt( doc->defaultTextOption() );
    if( show )
        opt.setFlags( opt.flags() | QTextOption::ShowTabsAndSpaces );
    else
        opt.setFlags( opt.flags() & ~QTextOption::ShowTabsAndSpaces );
    doc->setDefaultTextOption(opt);
}

void CodeEditor::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/editor");

    mSpaceIndent = settings->value("spaceIndent").toBool();

    setIndentWidth( settings->value("indentWidth").toInt() );

    QPalette plt;

    QFont fnt;
    fnt.fromString( settings->value("font").toString() );

    settings->beginGroup("colors");

    if (settings->contains("background"))
        plt.setColor(QPalette::Base, settings->value("background").value<QColor>());

    if (settings->contains("text"))
        plt.setColor(QPalette::Text, settings->value("text").value<QColor>());

    QPalette lineNumPlt;
    if (settings->contains("lineNumbersBackground"))
        lineNumPlt.setColor(QPalette::Button, settings->value("lineNumbersBackground").value<QColor>());
    if (settings->contains("lineNumbers"))
        lineNumPlt.setColor(QPalette::ButtonText, settings->value("lineNumbers").value<QColor>());
    mLineIndicator->setPalette(lineNumPlt);

    mBracketHighlight = settings->value("matchingBrackets").value<QColor>();

    settings->endGroup(); // colors

    setPalette(plt);
    setFont(fnt);

    settings->endGroup();
}

void CodeEditor::deleteTrailingSpaces()
{
    document()->deleteTrailingSpaces();
}

bool CodeEditor::event( QEvent *e )
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

void CodeEditor::changeEvent( QEvent *e )
{
    if( e->type() == QEvent::FontChange ) {
        // adjust tab stop to match mIndentWidth * width of space
        QTextDocument *doc = QPlainTextEdit::document();
        QFontMetricsF fm(font());
        QTextOption opt( doc->defaultTextOption() );
        opt.setTabStop( fm.width(' ') * mIndentWidth );
        doc->setDefaultTextOption(opt);
    }

    QPlainTextEdit::changeEvent(e);
}

void CodeEditor::keyPressEvent( QKeyEvent *e )
{
    switch (e->key()) {
    case Qt::Key_Home:
    {
        Qt::KeyboardModifiers mods(e->modifiers());
        if (mods && mods != Qt::ShiftModifier) break;

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
        QTextCursor cursor = textCursor();
        cursor.insertText("\t");
        return;
    }

    default:;
    }

    QPlainTextEdit::keyPressEvent(e);

    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_BraceRight:
    case Qt::Key_BracketRight:
    case Qt::Key_ParenRight:
        indent();
        break;

    default:;
    }

    mAutoCompleter->keyPress(e);
}

void CodeEditor::mouseReleaseEvent ( QMouseEvent *e )
{
    // Prevent deselection of bracket match:
    if(!mMouseBracketMatch)
        QPlainTextEdit::mouseReleaseEvent(e);

    mMouseBracketMatch = false;
}

void CodeEditor::mouseDoubleClickEvent ( QMouseEvent *e )
{
    QTextCursor c(textCursor());

    BracketMatch m;
    matchBracket( c.position(), m );
    if(m.first.isValid() && m.second.isValid())
    {
        c.setPosition(m.first.position());
        c.setPosition(m.second.position() + 1, QTextCursor::KeepAnchor);
        setTextCursor(c);
        mMouseBracketMatch = true;
    }
    else
    {
        QPlainTextEdit::mouseDoubleClickEvent(e);
    }
}

void CodeEditor::mouseMoveEvent( QMouseEvent *e )
{
    // Prevent initiating a text drag:
    if(!mMouseBracketMatch)
        QPlainTextEdit::mouseMoveEvent(e);
}

void CodeEditor::wheelEvent( QWheelEvent * e )
{
    if (e->modifiers() == Qt::ControlModifier) {
        if (e->delta() > 0)
            zoomIn();
        else
            zoomOut();
        return;
    }

    QPlainTextEdit::wheelEvent(e);
}

void CodeEditor::onOverlayChanged ( const QList<QRectF> & region )
{
    foreach(QRectF r, region)
    {
        viewport()->update(r.toRect());
    }
}

void CodeEditor::paintEvent( QPaintEvent *e )
{
    QPlainTextEdit::paintEvent(e);

    QPainter p(viewport());
    mOverlay->render(&p, e->rect(), e->rect());
}

void CodeEditor::updateLayout()
{
    setViewportMargins( mLineIndicator->width(), 0, 0, 0 );
}

void CodeEditor::updateLineIndicator( QRect r, int dy )
{
    if (dy)
        mLineIndicator->scroll(0, dy);
    else
        mLineIndicator->update(0, r.y(), mLineIndicator->width(), r.height() );
}

void CodeEditor::matchBrackets()
{
    mBracketSelections.clear();

    QTextCursor cursor(textCursor());

    BracketMatch match;
    matchBracket( cursor.position(), match );

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
            selection.format.setFontWeight(QFont::Bold);
            selection.format.setBackground(mBracketHighlight);

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

void CodeEditor::matchBracket( int pos, BracketMatch & match )
{
    QTextBlock block( textDocument()->findBlock(pos) );
    if (!block.isValid())
        return;

    int posInBlock = pos - block.position();
    TokenIterator it(block);

    while (it.isValid() && it.block() == block)
    {
        const Token & token = *it;
        if (token.positionInBlock > posInBlock)
            return;
        else if (
            (token.positionInBlock == posInBlock && token.type == Token::OpeningBracket) ||
            (token.positionInBlock == posInBlock - 1 && token.type == Token::ClosingBracket)
        )
            break;
        ++it;
    }

    if( !it.isValid() || it.block() != block)
        return;

    if(it->type == Token::OpeningBracket)
    {
        match.first = it;
        int level = 1;
        while((++it).isValid())
        {
            Token::Type type = it->type;
            if(type == Token::ClosingBracket)
                --level;
            else if(type == Token::OpeningBracket)
                ++level;
            if(level == 0) {
                match.second = it;
                return;
            }
        }
    }
    else if(it->type == Token::ClosingBracket)
    {
        match.second = it;
        int level = 1;
        while((--it).isValid())
        {
            Token::Type type = it->type;
            if(type == Token::OpeningBracket)
                --level;
            else if(type == Token::ClosingBracket)
                ++level;
            if(level == 0) {
                match.first = it;
                return;
            }
        }
    }
}

int CodeEditor::indentedStartOfLine( const QTextBlock &b )
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

void CodeEditor::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections;
    selections.append(mBracketSelections);
    selections.append(mSearchSelections);
    setExtraSelections(selections);
}

void CodeEditor::resizeEvent( QResizeEvent *e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect cr = contentsRect();
    mLineIndicator->resize( mLineIndicator->width(), cr.height() );
}

void CodeEditor::paintLineIndicator( QPaintEvent *e )
{
    QPalette plt( mLineIndicator->palette() );
    QRect r( e->rect() );
    QPainter p( mLineIndicator );

    p.fillRect( r, plt.color( QPalette::Button ) );
    p.setPen( plt.color(QPalette::ButtonText) );
    p.drawLine( r.topRight(), r.bottomRight() );

    QTextDocument *doc = QPlainTextEdit::document();
    QTextCursor cursor(textCursor());
    int selStartBlock, selEndBlock;
    if (cursor.hasSelection()) {
        selStartBlock = doc->findBlock(cursor.selectionStart()).blockNumber();
        selEndBlock = doc->findBlock(cursor.selectionEnd()).blockNumber();
    }
    else
        selStartBlock = selEndBlock = -1;

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            p.save();

            QRect numRect( 0, top, mLineIndicator->width() - 1, bottom - top );

            int num = blockNumber;
            if (num >= selStartBlock && num <= selEndBlock) {
                num -= selStartBlock;
                p.setPen(Qt::NoPen);
                p.setBrush(plt.color(QPalette::Highlight));
                p.drawRect(numRect);
                p.setPen(plt.color(QPalette::HighlightedText));
            }

            QString number = QString::number(num + 1);
            p.drawText(0, top, mLineIndicator->width() - 4, fontMetrics().height(),
                            Qt::AlignRight, number);

            p.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::indent()
{
    indent(textCursor());
}

void CodeEditor::indent( const QTextCursor & selection )
{
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

        if(blockNum >= startBlockNum)
            block = indent(block, stack.size());

        if(blockNum == endBlockNum)
            break;

        block = block.next();
        ++blockNum;
    }

    cursor.endEditBlock();
}

QString CodeEditor::makeIndentationString(int level)
{
    if (level <= 0)
        return QString();

    if ( mSpaceIndent ) {
        const int spaces = mIndentWidth * level;
        QString indentationString (spaces, QChar(' '));
        return indentationString;
    } else {
        const int tabs = level;
        QString indentationString (tabs, QChar('\t'));
        return indentationString;
    }
}

QTextBlock CodeEditor::indent( const QTextBlock & block, int level )
{
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + indentedStartOfLine(block), QTextCursor::KeepAnchor);

    cursor.insertText(makeIndentationString(level));

    // modification has invalidated the block, so return a new one
    return cursor.block();
}

int CodeEditor::indentationLevel(const QTextCursor & cursor)
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

void CodeEditor::triggerAutoCompletion()
{
    mAutoCompleter->triggerCompletion();
}

void CodeEditor::triggerMethodCallAid()
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

void CodeEditor::toggleComment()
{
    QTextCursor cursor = textCursor();

    if (cursor.hasSelection())
        toggleCommentSelection();
    else
        toggleCommentSingleLine();
}

void CodeEditor::toggleCommentSingleLine()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    toggleCommentSingleLine( cursor );

    cursor.endEditBlock();
}

void CodeEditor::addSingleLineComment(QTextCursor cursor, int indentation)
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

void CodeEditor::removeSingleLineComment(QTextCursor cursor)
{
    QTextBlock currentBlock(cursor.block());
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + indentedStartOfLine(currentBlock) + 2, QTextCursor::KeepAnchor);

    if (!cursor.selectedText().endsWith(QString("//")))
        cursor.setPosition(cursor.anchor() + indentedStartOfLine(currentBlock), QTextCursor::KeepAnchor);

    cursor.insertText("");
}

void CodeEditor::toggleCommentSingleLine(QTextCursor cursor)
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

static bool isBlockOnlySelection(QTextCursor)
{
    return true;
}

void CodeEditor::toggleCommentSelection()
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

        // TODO: keep indentation level of first block
        do {
            QTextCursor blockCursor(currentBlock);
            if (!isComment)
                addSingleLineComment(blockCursor, firstBlockIndentation);
            else
                removeSingleLineComment(blockCursor);
            currentBlock = currentBlock.next();
        } while (currentBlock.isValid() && currentBlock.position() <= cursor.selectionEnd());
    } else {
        QString selectionText = cursor.selectedText();
        QTextCursor selectionCursor(cursor);

        if (isSelectionComment(selectionText)) {
            selectionText = selectionText.trimmed().remove(2);
            selectionText.chop(2);
            selectionCursor.insertText(selectionText);
        } else {
            selectionText = QString("/* ") + selectionText + QString(" */");
            selectionCursor.insertText(selectionText);
        }
    }

    cursor.endEditBlock();
    indent(currentRegion());
}

void CodeEditor::copyUpDown(bool up)
{
    // directly taken from qtcreator
    // Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
    // GNU Lesser General Public License
    QTextCursor cursor = textCursor();
    QTextCursor move = cursor;
    move.beginEditBlock();

    bool hasSelection = cursor.hasSelection();

    if (hasSelection) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
        move.movePosition(move.atBlockStart() ? QTextCursor::Left: QTextCursor::EndOfBlock,
                          QTextCursor::KeepAnchor);
    } else {
        move.movePosition(QTextCursor::StartOfBlock);
        move.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }

    QString text = move.selectedText();

    if (up) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.insertBlock();
        move.movePosition(QTextCursor::Left);
    } else {
        move.movePosition(QTextCursor::EndOfBlock);
        if (move.atBlockStart()) {
            move.movePosition(QTextCursor::NextBlock);
            move.insertBlock();
            move.movePosition(QTextCursor::Left);
        } else {
            move.insertBlock();
        }
    }

    int start = move.position();
    move.clearSelection();
    move.insertText(text);
    int end = move.position();

    move.setPosition(start);
    move.setPosition(end, QTextCursor::KeepAnchor);

    move.endEditBlock();
    indent(currentRegion());

    setTextCursor(move);
}

void CodeEditor::copyLineDown()
{
    copyUpDown(false);
}

void CodeEditor::copyLineUp()
{
    copyUpDown(true);
}

void CodeEditor::moveLineUpDown(bool up)
{
    // directly taken from qtcreator
    // Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
    // GNU Lesser General Public License
    QTextCursor cursor = textCursor();
    QTextCursor move = cursor;

    move.setVisualNavigation(false); // this opens folded items instead of destroying them

    move.beginEditBlock();

    bool hasSelection = cursor.hasSelection();

    if (cursor.hasSelection()) {
        move.setPosition(cursor.selectionStart());
        move.movePosition(QTextCursor::StartOfBlock);
        move.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
        move.movePosition(move.atBlockStart() ? QTextCursor::Left: QTextCursor::EndOfBlock,
                          QTextCursor::KeepAnchor);
    } else {
        move.movePosition(QTextCursor::StartOfBlock);
        move.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }
    QString text = move.selectedText();

    move.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    move.removeSelectedText();

    if (up) {
        move.movePosition(QTextCursor::PreviousBlock);
        move.insertBlock();
        move.movePosition(QTextCursor::Left);
    } else {
        move.movePosition(QTextCursor::EndOfBlock);
        if (move.atBlockStart()) { // empty block
            move.movePosition(QTextCursor::NextBlock);
            move.insertBlock();
            move.movePosition(QTextCursor::Left);
        } else {
            move.insertBlock();
        }
    }

    int start = move.position();
    move.clearSelection();
    move.insertText(text);
    int end = move.position();

    if (hasSelection) {
        move.setPosition(start);
        move.setPosition(end, QTextCursor::KeepAnchor);
    }

    move.endEditBlock();
    indent(currentRegion());

    setTextCursor(move);
}

void CodeEditor::moveLineUp()
{
    moveLineUpDown(true);
}

void CodeEditor::moveLineDown()
{
    moveLineUpDown(false);
}

} // namespace ScIDE
