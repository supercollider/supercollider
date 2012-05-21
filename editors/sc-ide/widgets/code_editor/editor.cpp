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
#include "../../core/doc_manager.hpp"
#include "../../core/settings/manager.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QKeyEvent>
#include <QScrollBar>

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
    mOverlay( new QGraphicsScene(this) )
{
    mLineIndicator->move( contentsRect().topLeft() );

    connect( this, SIGNAL(blockCountChanged(int)),
             mLineIndicator, SLOT(setLineCount(int)) );

    connect( mLineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );

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

    if(!cursor.isNull())
    {
        setTextCursor(cursor);
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
    selection.format.setBackground(Qt::yellow);

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
    QTextBlock b(c.block());

    int pos = c.position() - b.position();
    BracketIterator start;
    BracketIterator end;
    int topLevel = 0;
    int level = 0;

    // search unmatched opening bracket
    BracketIterator it = BracketIterator::leftOf( b, pos );
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
    it = BracketIterator::rightOf( b, pos );
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
    QFont f = font();
    qreal size = f.pointSizeF();
    if( size != -1 )
        f.setPointSizeF( size + steps );
    else
        f.setPixelSize( f.pixelSize() + steps );

    setFont(f);
}

void CodeEditor::zoomOut(int steps)
{
    QFont f = font();
    qreal size = f.pointSizeF();
    if( size != -1 )
        f.setPointSizeF( qMax(1.0, size - steps) );
    else
        f.setPixelSize( qMax(1, f.pixelSize() - steps) );

    setFont(f);
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

void CodeEditor::applySettings( Settings::Manager *s )
{
    s->beginGroup("IDE/editor");

    mSpaceIndent = s->value("spaceIndent").toBool();

    setIndentWidth( s->value("indentWidth").toInt() );

    QPalette plt;

    QFont fnt;
    fnt.fromString( s->value("font").toString() );

    s->beginGroup("colors");

    if (s->contains("background"))
        plt.setColor(QPalette::Base, s->value("background").value<QColor>());

    if (s->contains("text"))
        plt.setColor(QPalette::Text, s->value("text").value<QColor>());

    QPalette lineNumPlt;
    if (s->contains("lineNumbersBackground"))
        lineNumPlt.setColor(QPalette::Button, s->value("lineNumbersBackground").value<QColor>());
    if (s->contains("lineNumbers"))
        lineNumPlt.setColor(QPalette::ButtonText, s->value("lineNumbers").value<QColor>());
    mLineIndicator->setPalette(lineNumPlt);

    mBracketHighlight = s->value("matchingBrackets").value<QColor>();

    s->endGroup(); // colors

    setPalette(plt);
    setFont(fnt);

    s->endGroup();
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
                case Qt::Key_Backtab:
                    indent( key == Qt::Key_Backtab );
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
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QTextCursor c(textCursor());
        QTextBlock b(c.block());
        int indent = indentedStartOfLine(b);
        c.insertBlock();
        if(indent > 0) {
            QString str(b.text());
            str.truncate(indent);
            c.insertText(str);
        }

        return;
    }
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
    default:;
    }

    QPlainTextEdit::keyPressEvent(e);
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
    QTextBlock b(c.block());

    BracketMatch m;
    matchBracket( b, c.position() - b.position(), m );
    if(m.pos > -1 && m.matchPos > -1)
    {
        c.setPosition(qMin(m.pos, m.matchPos));
        c.setPosition(qMax(m.pos, m.matchPos) + 1, QTextCursor::KeepAnchor);
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
    QTextBlock block(cursor.block());

    BracketMatch match;
    matchBracket( block, cursor.position() - block.position(), match );

    if( match.pos != -1 && match.matchPos != -1 )
    {
        QTextEdit::ExtraSelection selection;
        selection.format.setFontWeight(QFont::Bold);
        selection.format.setBackground(mBracketHighlight);

        cursor.setPosition(match.pos);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        selection.cursor = cursor;
        mBracketSelections.append(selection);

        cursor.setPosition(match.matchPos);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        selection.cursor = cursor;
        mBracketSelections.append(selection);
    }

    updateExtraSelections();
}

void CodeEditor::matchBracket( const QTextBlock & block, int pos, BracketMatch & match )
{
    BracketIterator it = BracketIterator::around( block, pos );

    if( !it.isValid() )
    {
        match.pos = match.matchPos = -1;
        return;
    }

    char c = it.character();
    match.pos = it.position();

    static QString lbrackets("([{");
    static QString rbrackets(")]}");

    int b;
    if((b = lbrackets.indexOf(c)) != -1)
    {
        char cc = rbrackets[b].toAscii();
        int level = 1;
        while((++it).isValid())
        {
            char chr = it.character();
            if(chr == cc)
                --level;
            else if(chr == c)
                ++level;
            if(level == 0) {
                match.matchPos = it.position();
                return;
            }
        }
    }
    else if((b = rbrackets.indexOf(c)) != -1)
    {
        char cc = lbrackets[b].toAscii();
        int level = 1;
        while((--it).isValid())
        {
            char chr = it.character();
            if(chr == cc)
                --level;
            else if(chr == c)
                ++level;
            if(level == 0) {
                match.matchPos = it.position();
                return;
            }
        }
    }

    match.pos = match.matchPos = -1;
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

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            p.drawText(0, top, mLineIndicator->width() - 4, fontMetrics().height(),
                            Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::indent( bool less )
{
    QTextDocument *doc = QPlainTextEdit::document();
    QTextCursor c(textCursor());
    int pos = c.position();
    int anc = c.anchor();
    QTextBlock block = doc->findBlock(qMin(pos,anc));
    QTextBlock endBlock = anc != pos ? doc->findBlock(qMax(pos,anc)) : block;
    int indent = mIndentWidth;

    c.beginEditBlock();

    while (true) {
        QTextCursor c( block );
        QString text( block.text() );

        int pos = 0;
        int spaces = 0;
        while (true)
        {
            QChar chr(text[pos]);
            if( chr == ' ' )
                ++spaces;
            else if( chr == '\t' )
                spaces += indent;
            else
                break;
            ++pos;
        }

        int tabs = spaces / indent;
        if (less) {
            if(tabs * indent == spaces && tabs > 0)
                --tabs;
        }
        else {
            ++tabs;
        }

        c.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, pos );

        if( tabs == 0 )
        {
            c.removeSelectedText();
        }
        else if( mSpaceIndent )
        {
            spaces = tabs * indent;
            while(spaces--)
                c.insertText(" ");
        }
        else
        {
            while(tabs--)
                c.insertText("\t");
        }

        if(block != endBlock)
            block = block.next();
        else
            break;
    }

    c.endEditBlock();
}

} // namespace ScIDE
