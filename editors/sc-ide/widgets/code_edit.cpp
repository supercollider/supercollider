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

#include "code_edit.hpp"
#include "../core/doc_manager.hpp"
#include "../core/sc_syntax_highlighter.hpp"
#include "../core/settings.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QKeyEvent>

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
    _lineIndicator( new LineIndicator(this) ),
    mDoc(0),
    mIndentWidth(4),
    mSpaceIndent(true),
    mShowWhitespace(false)
{
    QFont fnt(font());
    fnt.setFamily("monospace");
    fnt.setStyleHint(QFont::TypeWriter);
    setFont(fnt);

    _lineIndicator->move( contentsRect().topLeft() );

    connect( this, SIGNAL(blockCountChanged(int)),
             _lineIndicator, SLOT(setLineCount(int)) );

    connect( _lineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );

    connect( this, SIGNAL(cursorPositionChanged()),
             this, SLOT(matchBrackets()) );

    _lineIndicator->setLineCount(1);
}

void CodeEditor::setDocument( Document *doc )
{
    QTextDocument *tdoc = doc->textDocument();

    QFontMetricsF fm(font());

    QTextOption opt;
    opt.setTabStop( fm.width(' ') * mIndentWidth );
    if(mShowWhitespace)
        opt.setFlags( QTextOption::ShowTabsAndSpaces );

    tdoc->setDefaultTextOption(opt);
    tdoc->setDefaultFont(font());
    tdoc->setDocumentLayout( new QPlainTextDocumentLayout(tdoc) );

    QPlainTextEdit::setDocument(tdoc);

    _lineIndicator->setLineCount( tdoc->blockCount() );

    mDoc = doc;
}

void CodeEditor::setIndentWidth( int width )
{
    mIndentWidth = width;

    QTextDocument *tdoc = mDoc->textDocument();

    QFontMetricsF fm(font());

    QTextOption opt;
    opt.setTabStop( fm.width(' ') * mIndentWidth );
    if(mShowWhitespace)
        opt.setFlags( QTextOption::ShowTabsAndSpaces );

    tdoc->setDefaultTextOption(opt);
}

void CodeEditor::zoomIn(int steps)
{
    QFont f = font();
    qreal size = f.pointSize();
    if( size != -1 )
        f.setPointSizeF( size + steps );
    else
        f.setPixelSize( f.pixelSize() + steps );

    setFont(f);
}

void CodeEditor::zoomOut(int steps)
{
    QFont f = font();
    qreal size = f.pointSize();
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

void CodeEditor::applySettings( QSettings *s )
{
    s->beginGroup("IDE/editor");

    mSpaceIndent = s->value("spaceIndent", true).toBool();

    setIndentWidth( s->value("indentWidth", 4).toInt() );

    QPalette plt;
    QFont fnt(font());

    fnt.setFamily("monospace");
    fnt.setStyleHint(QFont::TypeWriter);

    s->beginGroup("colors");

    if (s->contains("background"))
        plt.setColor(QPalette::Base, s->value("background").value<QColor>());

    if (s->contains("text"))
        plt.setColor(QPalette::Text, s->value("text").value<QColor>());

    mBracketHighlight = s->value("matchingBrackets", Qt::gray).value<QColor>();

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

void CodeEditor::updateLayout()
{
    setViewportMargins( _lineIndicator->width(), 0, 0, 0 );
}

void CodeEditor::updateLineIndicator( QRect r, int dy )
{
    if (dy)
        _lineIndicator->scroll(0, dy);
    else
        _lineIndicator->update(0, r.y(), _lineIndicator->width(), r.height() );
}

void CodeEditor::matchBrackets()
{
    QList<QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);

    QTextCursor cursor(textCursor());
    QTextBlock block(cursor.block());

    BracketMatch match;
    matchBracket( block, cursor.position() - block.position(), match );

    if( match.pos != -1 && match.matchPos != -1 )
    {
        highlightBracket(match.pos);
        highlightBracket(match.matchPos);
    }
}

void CodeEditor::matchBracket( const QTextBlock & block, int pos, BracketMatch & match )
{
    SyntaxHighlighter::BlockData *data =
        static_cast<SyntaxHighlighter::BlockData*>(block.userData());

    Q_ASSERT(data);

    int n = data->brackets.size();
    for( int i = 0; i < n; ++i )
    {
        SyntaxHighlighter::BracketInfo const & bracket = data->brackets[i];
        if(bracket.position == pos - 1 || bracket.position == pos)
        {
            match.pos = bracket.position + block.position();

            static QString lbrackets("([{");
            static QString rbrackets(")]}");

            char c = bracket.character;
            int b;
            if((b = lbrackets.indexOf(c)) != -1)
                matchLeftBracket( c, rbrackets[b].toAscii(), block, i, match );
            else if((b = rbrackets.indexOf(c)) != -1)
                matchRightBracket( c, lbrackets[b].toAscii(), block, i, match );
            return;
        }
    }

    match.pos = -1;
    match.matchPos = -1;
}

void CodeEditor::matchRightBracket
( char c, char cc, const QTextBlock & block, int index, BracketMatch & match )
{
    int level = 1;
    bool first = true;
    QTextBlock b(block);
    int i = index - 1;

    while(b.isValid())
    {
        SyntaxHighlighter::BlockData *data =
            static_cast<SyntaxHighlighter::BlockData*>(b.userData());

        if(data)
        {
            if(!first)
                i = data->brackets.size() - 1;

            for(; i >= 0; --i)
            {
                SyntaxHighlighter::BracketInfo const & bracket = data->brackets[i];
                if(bracket.character == cc)
                    --level;
                else if(bracket.character == c)
                    ++level;
                if(level == 0) {
                    match.matchPos = bracket.position + b.position();
                    return;
                }
            }
        }

        first = false;
        b = b.previous();
    }

    match.matchPos = -1;
}

void CodeEditor::matchLeftBracket
( char c, char cc, const QTextBlock & block, int index, BracketMatch & match )
{
    int level = 1;
    QTextBlock b(block);
    int i = index + 1;

    while(b.isValid())
    {
        SyntaxHighlighter::BlockData *data =
            static_cast<SyntaxHighlighter::BlockData*>(b.userData());

        if(data)
        {
            int n = data->brackets.size();

            for(; i < n; ++i)
            {
                SyntaxHighlighter::BracketInfo const & bracket = data->brackets[i];
                if(bracket.character == cc)
                    --level;
                else if(bracket.character == c)
                    ++level;
                if(level == 0) {
                    match.matchPos = bracket.position + b.position();
                    return;
                }
            }
        }

        i = 0;
        b = b.next();
    }

    match.matchPos = -1;
}

void CodeEditor::highlightBracket( int pos )
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    QTextEdit::ExtraSelection selection;
    selection.cursor = cursor;
    selection.format.setFontWeight(QFont::Bold);
    selection.format.setBackground(mBracketHighlight);

    QList<QTextEdit::ExtraSelection> selections = extraSelections();
    selections.append(selection);
    setExtraSelections(selections);
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

void CodeEditor::resizeEvent( QResizeEvent *e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect cr = contentsRect();
    _lineIndicator->resize( _lineIndicator->width(), cr.height() );
}

void CodeEditor::paintLineIndicator( QPaintEvent *e )
{
    QPalette plt( palette() );
    QRect r( e->rect() );
    QPainter p( _lineIndicator );

    p.fillRect( r, plt.color( QPalette::Button ) );
    p.setPen( plt.color(QPalette::Text) );
    p.drawLine( r.topRight(), r.bottomRight() );

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            p.drawText(0, top, _lineIndicator->width() - 4, fontMetrics().height(),
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
