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

#include "autocompleter.hpp"
#include "editor.hpp"
#include "highlighter.hpp"
#include "line_indicator.hpp"
#include "../sc_symbol_references.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/main.hpp"
#include "../../core/settings/manager.hpp"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QUrl>

namespace ScIDE {

GenericCodeEditor::GenericCodeEditor( Document *doc, QWidget *parent ):
    QPlainTextEdit( parent ),
    mLineIndicator( new LineIndicator(this) ),
    mDoc(doc)
{
    Q_ASSERT(mDoc != 0);

    setFrameShape( QFrame::NoFrame );

    mLineIndicator->move( contentsRect().topLeft() );

    connect( mDoc, SIGNAL(defaultFontChanged()), this, SLOT(onDocumentFontChanged()) );

    connect( this, SIGNAL(blockCountChanged(int)),
             mLineIndicator, SLOT(setLineCount(int)) );

    connect( mLineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );

    connect( this, SIGNAL(selectionChanged()),
             mLineIndicator, SLOT(update()) );

    QTextDocument *tdoc = doc->textDocument();
    QPlainTextEdit::setDocument(tdoc);
    onDocumentFontChanged();
    mLineIndicator->setLineCount(blockCount());
}


bool GenericCodeEditor::showWhitespace()
{
    QTextOption options( textDocument()->defaultTextOption() );
    return options.flags().testFlag( QTextOption::ShowTabsAndSpaces );
}

void GenericCodeEditor::setShowWhitespace(bool show)
{
    QTextDocument *doc = textDocument();
    QTextOption opt( doc->defaultTextOption() );
    if( show )
        opt.setFlags( opt.flags() | QTextOption::ShowTabsAndSpaces );
    else
        opt.setFlags( opt.flags() & ~QTextOption::ShowTabsAndSpaces );
    doc->setDefaultTextOption(opt);
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

bool GenericCodeEditor::find( const QRegExp &expr, QTextDocument::FindFlags options )
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
        setTextCursor(cursor);
        return true;
    }
    else
        return false;
}

int GenericCodeEditor::findAll( const QRegExp &expr, QTextDocument::FindFlags options )
{
    mSearchSelections.clear();

    if(expr.isEmpty()) {
        this->updateExtraSelections();
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

    this->updateExtraSelections();

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

bool GenericCodeEditor::replace( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options )
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

int GenericCodeEditor::replaceAll( const QRegExp &expr, const QString &replacement, QTextDocument::FindFlags options )
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

void GenericCodeEditor::showPosition( int pos )
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

QString GenericCodeEditor::symbolUnderCursor()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void GenericCodeEditor::keyPressEvent(QKeyEvent * e)
{
    hideMouseCursor();

    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        // override to avoid entering a "soft" new line when certain modifier is held
        textCursor().insertBlock();
        ensureCursorVisible();
        break;

    default:
        QPlainTextEdit::keyPressEvent(e);
    }
}

void GenericCodeEditor::wheelEvent( QWheelEvent * e )
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


void GenericCodeEditor::dragEnterEvent( QDragEnterEvent * event )
{
    const QMimeData * data = event->mimeData();
    if (data->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            if (url.scheme() == QString("file")) { // LATER: use isLocalFile
                // LATER: check mime type ?
                event->acceptProposedAction();
                return;
            }
        }
        return;
    }

    static QString textPlain("text/plain");
    if (data->formats().contains(textPlain))
        event->acceptProposedAction();
}

void GenericCodeEditor::dropEvent( QDropEvent * event )
{
    const QMimeData * data = event->mimeData();
    if (data->hasUrls()) {
        foreach (QUrl url, data->urls()) {
            if (url.scheme() == QString("file")) // LATER: use isLocalFile
                Main::documentManager()->open(url.toLocalFile());
        }
        return;
    }

    QTextCursor cursor = cursorForPosition(event->pos());
    cursor.insertText(data->text());
}


void GenericCodeEditor::clearSearchHighlighting()
{
    mSearchSelections.clear();
    this->updateExtraSelections();
}

void GenericCodeEditor::zoomIn(int steps)
{
    zoomFont(steps);
}

void GenericCodeEditor::zoomOut(int steps)
{
    zoomFont(-steps);
}

void GenericCodeEditor::resetFontSize()
{
    mDoc->resetDefaultFont();
}

void GenericCodeEditor::zoomFont(int steps)
{
    QFont currentFont = mDoc->defaultFont();
    const int newSize = currentFont.pointSize() + steps;
    if (newSize <= 0)
        return;
    currentFont.setPointSize(newSize);
    mDoc->setDefaultFont(currentFont);
}

void GenericCodeEditor::onDocumentFontChanged()
{
    QFont font = mDoc->defaultFont();
    setFont(font);
}

void GenericCodeEditor::updateLayout()
{
    setViewportMargins( mLineIndicator->width(), 0, 0, 0 );
}

void GenericCodeEditor::updateLineIndicator( QRect r, int dy )
{
    if (dy)
        mLineIndicator->scroll(0, dy);
    else
        mLineIndicator->update(0, r.y(), mLineIndicator->width(), r.height() );
}

void GenericCodeEditor::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections;
    selections.append(mSearchSelections);
    setExtraSelections(selections);
}

void GenericCodeEditor::resizeEvent( QResizeEvent *e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect cr = contentsRect();
    mLineIndicator->resize( mLineIndicator->width(), cr.height() );
}

void GenericCodeEditor::paintLineIndicator( QPaintEvent *e )
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
            p.drawText(0, top, mLineIndicator->width() - 4, bottom - top,
                       Qt::AlignRight, number);

            p.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void GenericCodeEditor::copyUpDown(bool up)
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

    setTextCursor(move);

    this->indentCurrentRegion();
}


void GenericCodeEditor::toggleOverwriteMode()
{
    setOverwriteMode(!overwriteMode());
}


void GenericCodeEditor::copyLineDown()
{
    copyUpDown(false);
}

void GenericCodeEditor::copyLineUp()
{
    copyUpDown(true);
}

void GenericCodeEditor::moveLineUpDown(bool up)
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

    setTextCursor(move);

    this->indentCurrentRegion();
}

void GenericCodeEditor::moveLineUp()
{
    moveLineUpDown(true);
}

void GenericCodeEditor::moveLineDown()
{
    moveLineUpDown(false);
}

void GenericCodeEditor::gotoPreviousEmptyLine()
{
    gotoEmptyLineUpDown(true);
}

void GenericCodeEditor::gotoNextEmptyLine()
{
    gotoEmptyLineUpDown(false);
}

void GenericCodeEditor::gotoEmptyLineUpDown(bool up)
{
    static const QRegExp whiteSpaceLine("^\\s*$");

    const QTextCursor::MoveOperation direction = up ? QTextCursor::PreviousBlock
                                                    : QTextCursor::NextBlock;

    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    bool cursorMoved = false;

    // find first non-whitespace line
    while ( cursor.movePosition(direction) ) {
        if ( !whiteSpaceLine.exactMatch(cursor.block().text()) )
            break;
    }

    // find first whitespace line
    while ( cursor.movePosition(direction) ) {
        if ( whiteSpaceLine.exactMatch(cursor.block().text()) ) {
            setTextCursor(cursor);
            cursorMoved = true;
            break;
        }
    }

    if (!cursorMoved) {
        const QTextCursor::MoveOperation startOrEnd = up ? QTextCursor::Start
                                                         : QTextCursor::End;

        cursor.movePosition(startOrEnd);
        setTextCursor(cursor);
    }

    cursor.endEditBlock();
}

void GenericCodeEditor::hideMouseCursor()
{
    QCursor * overrideCursor = QApplication::overrideCursor();
    if (!overrideCursor || overrideCursor->shape() != Qt::BlankCursor)
        QApplication::setOverrideCursor( Qt::BlankCursor );
}



CodeEditor::CodeEditor( Document *doc, QWidget *parent ) :
    GenericCodeEditor( doc, parent ),
    mSpaceIndent(true),
    mBlinkDuration(600),
    mMouseBracketMatch(false),
    mOverlay( new QGraphicsScene(this) ),
    mAutoCompleter( new AutoCompleter(this) )
{
    Q_ASSERT(mDoc != 0);

    setFrameShape( QFrame::NoFrame );

    mLineIndicator->move( contentsRect().topLeft() );

    connect( this, SIGNAL(cursorPositionChanged()),
             this, SLOT(matchBrackets()) );

    connect( mOverlay, SIGNAL(changed(const QList<QRectF>&)),
             this, SLOT(onOverlayChanged(const QList<QRectF>&)) );

    connect( Main::instance(), SIGNAL(applySettingsRequest(Settings::Manager*)),
             this, SLOT(applySettings(Settings::Manager*)) );

    QTextDocument *tdoc = doc->textDocument();
    mAutoCompleter->documentChanged(tdoc);
    mLineIndicator->setLineCount(blockCount());

    applySettings(Main::settings());
}


QTextCursor CodeEditor::currentRegion()
{
    return regionAtCursor(textCursor());
}


QTextCursor CodeEditor::regionAtCursor(const QTextCursor & cursor)
{
    QTextBlock block(cursor.block());
    int positionInBlock = cursor.positionInBlock();

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

QTextCursor CodeEditor::blockAtCursor(const QTextCursor & cursor)
{
    TokenIterator it (cursor.block(), cursor.positionInBlock());

    if (it.isValid()) {
        switch (it->type) {
        case Token::OpeningBracket:
        case Token::ClosingBracket:
        {
            BracketMatch match;
            matchBracket(it, match);

            if (match.first.isValid()) {
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


void CodeEditor::applySettings( Settings::Manager *settings )
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

void CodeEditor::keyPressEvent( QKeyEvent *e )
{
    switch (e->key()) {
    case Qt::Key_Home:
    {
        hideMouseCursor();
        Qt::KeyboardModifiers mods(e->modifiers());
        if (mods && mods != Qt::ShiftModifier) {
            QPlainTextEdit::keyPressEvent(e);
            break;
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

    default:
        GenericCodeEditor::keyPressEvent(e);
    }

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
        GenericCodeEditor::mouseReleaseEvent(e);

    mMouseBracketMatch = false;
}

void CodeEditor::mouseDoubleClickEvent( QMouseEvent * e )
{
    QTextCursor cursor = cursorForPosition(e->pos());
    QTextCursor selection = blockAtCursor(cursor);

    if (!selection.isNull()) {
        setTextCursor(selection);
        return;
    }

    GenericCodeEditor::mouseDoubleClickEvent(e);
}

void CodeEditor::mouseMoveEvent( QMouseEvent *e )
{
    // Prevent initiating a text drag:
    if(!mMouseBracketMatch)
        GenericCodeEditor::mouseMoveEvent(e);
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
    GenericCodeEditor::paintEvent(e);

    QPainter p(viewport());
    mOverlay->render(&p, e->rect(), e->rect());
}

void CodeEditor::matchBrackets()
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

    BracketMatch match;
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

void CodeEditor::matchBracket( const TokenIterator & bracket , BracketMatch & match )
{
    TokenIterator it(bracket);

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

void CodeEditor::indentCurrentRegion()
{
    indent(currentRegion());
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

QString CodeEditor::makeIndentationString(int level)
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
            selectionText = selectionText.trimmed().remove(2);
            selectionText.chop(2);
            selectionCursor.insertText(selectionText);
        } else {
            selectionText = QString("/* ") + selectionText + QString(" */");
            selectionCursor.insertText(selectionText);
        }
    }

    cursor.endEditBlock();
    cursor.beginEditBlock();
    indentCurrentRegion();
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


void CodeEditor::gotoNextBlock()
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

void CodeEditor::gotoPreviousBlock()
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

void CodeEditor::selectCurrentRegion()
{
    QTextCursor selectedRegionCursor = currentRegion();
    if (!selectedRegionCursor.isNull() && selectedRegionCursor.hasSelection())
        setTextCursor(selectedRegionCursor);
}

void CodeEditor::gotoNextRegion()
{
    QTextCursor cursor = textCursor();

    QTextCursor regionCursor = regionAtCursor(cursor);
    if (!regionCursor.isNull()) {
        cursor = regionCursor;
        // regionCursor does not include region's closing bracket, so skip it
        cursor.movePosition(QTextCursor::NextCharacter);
    }

    // Skip potential opening bracket immediately right of cursor:
    cursor.movePosition(QTextCursor::NextCharacter);

    TokenIterator it = TokenIterator::rightOf(cursor.block(), cursor.positionInBlock());

    while (it.isValid()) {
        if ( (it->type == Token::OpeningBracket) && (it->character == '(') &&
             (it->positionInBlock == 0) ) {
            setTextCursor( cursorAt(it, 1) );
            return;
        }
        ++it;
    }

    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void CodeEditor::gotoPreviousRegion()
{
    QTextCursor cursor = textCursor();

    QTextCursor regionCursor = regionAtCursor(cursor);

    if (!regionCursor.isNull()) {
        // regionCursor does not include region's opening bracket, so skip it:
        cursor.setPosition( regionCursor.selectionStart() - 1 );
    }

    // Skip potential closing bracket immediately left of cursor:
    cursor.movePosition( QTextCursor::PreviousCharacter );

    TokenIterator it = TokenIterator::leftOf(cursor.block(), cursor.positionInBlock());

    while (it.isValid()) {
        if ( (it->type == Token::ClosingBracket) && (it->character == ')') &&
             (it->positionInBlock == 0) ) {
            setTextCursor( cursorAt(it) );
            return;
        }
        --it;
    }

    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);
}

bool CodeEditor::openDocumentation()
{
    return Main::openDocumentation(symbolUnderCursor());
}

void CodeEditor::openDefinition()
{
    Main::openDefinition(symbolUnderCursor(), this);
}

void CodeEditor::findReferences()
{
    Main::findReferences(symbolUnderCursor(), this);
}

QTextCursor CodeEditor::cursorAt(const TokenIterator it, int offset)
{
    Q_ASSERT(it.isValid());

    QTextCursor textCursor(textDocument());
    textCursor.setPosition(it.position() + offset);

    return textCursor;
}

} // namespace ScIDE
