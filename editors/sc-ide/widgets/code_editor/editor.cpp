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
#include "line_indicator.hpp"
#include "../util/gui_utilities.hpp"
#include "../../core/main.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/settings/manager.hpp"

#include <QApplication>
#include <QDebug>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QUrl>
#include <QMimeData>
#include <QScrollBar>

#ifdef Q_WS_X11
# include <QX11Info>
# include <X11/Xlib.h>
// X11 defines the following, clashing with QEvent::Type enum
# undef KeyPress
# undef KeyRelease
#endif

namespace ScIDE {

GenericCodeEditor::GenericCodeEditor( Document *doc, QWidget *parent ):
    QPlainTextEdit( parent ),
    mDoc(doc),
    mLastCursorBlock(-1)
{
    Q_ASSERT(mDoc != 0);

    setFrameShape( QFrame::NoFrame );

    viewport()->setAttribute( Qt::WA_MacNoClickThrough, true );

    mLineIndicator = new LineIndicator(this);
    mLineIndicator->move( contentsRect().topLeft() );

    mOverlay = new QGraphicsScene(this);

    QGraphicsView *overlayView = new QGraphicsView(mOverlay, this);
    overlayView->setFrameShape( QFrame::NoFrame );
    overlayView->setBackgroundBrush( Qt::NoBrush );
    overlayView->setStyleSheet("background: transparent");
    overlayView->setFocusPolicy( Qt::NoFocus );
    overlayView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    overlayView->setSceneRect(QRectF(0,0,1,1));
    overlayView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    mOverlayWidget = overlayView;

    connect( mDoc, SIGNAL(defaultFontChanged()), this, SLOT(onDocumentFontChanged()) );

    connect( this, SIGNAL(blockCountChanged(int)),
             mLineIndicator, SLOT(setLineCount(int)) );

    connect( mLineIndicator, SIGNAL( widthChanged() ),
             this, SLOT( updateLayout() ) );

    connect( this, SIGNAL(updateRequest(QRect,int)),
             this, SLOT(updateLineIndicator(QRect,int)) );

    connect( this, SIGNAL(selectionChanged()),
             mLineIndicator, SLOT(update()) );
    connect( this, SIGNAL(cursorPositionChanged()),
             this, SLOT(onCursorPositionChanged()) );

    connect( Main::instance(), SIGNAL(applySettingsRequest(Settings::Manager*)),
             this, SLOT(applySettings(Settings::Manager*)) );

    QTextDocument *tdoc = doc->textDocument();
    QPlainTextEdit::setDocument(tdoc);
    onDocumentFontChanged();
    mLineIndicator->setLineCount(blockCount());

    applySettings( Main::settings() );
}

void GenericCodeEditor::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/editor");

    bool lineWrap = settings->value("lineWrap").toBool();
    bool showWhitespace = settings->value("showWhitespace").toBool();

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
        // NOTE: the line number widget will inherit the palette from the editor
        QTextCharFormat format = settings->value("lineNumbers").value<QTextCharFormat>();
        QBrush bg = format.background();
        QBrush fg = format.foreground();
        if (bg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Mid, bg);
        if (fg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::ButtonText, fg);
    }

    if (settings->contains("selection")) {
        QTextCharFormat format = settings->value("selection").value<QTextCharFormat>();
        QBrush bg = format.background();
        QBrush fg = format.foreground();
        if (bg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Highlight, bg);
        if (fg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::HighlightedText, fg);
    }

    mCurrentLineTextFormat = settings->value("currentLine").value<QTextCharFormat>();
    mSearchResultTextFormat = settings->value("searchResult").value<QTextCharFormat>();

    settings->endGroup(); // colors

    mHighlightCurrentLine = settings->value("highlightCurrentLine").toBool();
    updateCurrentLineHighlighting();

    settings->endGroup(); // IDE/editor

    setLineWrapMode( lineWrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap );
    setShowWhitespace( showWhitespace );
    setPalette(palette);
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
    selection.format = mSearchResultTextFormat;

    QTextDocument *doc = QPlainTextEdit::document();
    QTextBlock block = doc->begin();
    QTextCursor cursor;

    while (block.isValid()) {
        int blockPos = block.position();
        int offset = 0;
        while(findInBlock(doc, block, expr, offset, options, cursor)) {
            offset = cursor.selectionEnd() - blockPos;

            if (cursor.hasSelection()) {
                selection.cursor = cursor;
                mSearchSelections.append(selection);
            } else
                offset += 1;
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

void GenericCodeEditor::showPosition( int pos, int selectionLength )
{
    if (pos < 0) return;

    QTextDocument *doc = QPlainTextEdit::document();
    if (!doc) return;

    int lineNumber = doc->findBlock(pos).firstLineNumber();
    verticalScrollBar()->setValue(lineNumber);

    QTextCursor cursor(doc);
    cursor.setPosition(pos);
    if (selectionLength)
        cursor.setPosition(pos + selectionLength, QTextCursor::KeepAnchor);

    setTextCursor(cursor);
}

QString GenericCodeEditor::symbolUnderCursor()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        return cursor.selectedText();
    else
    {
        QString blockString = cursor.block().text();
        int position = cursor.positionInBlock();
        return wordInStringAt( position, blockString );
    }
}

bool GenericCodeEditor::event( QEvent * event )
{
    switch (event->type()) {
    case QEvent::ShortcutOverride: {
        // Prevent overriding certain shortcuts,
        // as they are useful in our application.
        QKeyEvent* kevent = static_cast<QKeyEvent*>(event);
        switch (kevent->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            event->ignore();
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QPlainTextEdit::event(event);
}

void GenericCodeEditor::keyPressEvent(QKeyEvent * e)
{
    hideMouseCursor(e);

    QTextCursor cursor( textCursor() );

    bool updateCursor = false;

    if (e == QKeySequence::InsertLineSeparator) {
        // override to avoid entering a "soft" new line
        cursor.insertBlock();
        updateCursor = true;
    } else {
        switch (e->key()) {

        case Qt::Key_Delete:
            if (e->modifiers() & Qt::META) {
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            } else
                QPlainTextEdit::keyPressEvent(e);
            break;

        case Qt::Key_Backspace:
            if (e->modifiers() & Qt::META) {
                cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            } else {
                if ( !overwriteMode()
                     || (cursor.positionInBlock() == 0)
                     || cursor.hasSelection() ) {
                    QPlainTextEdit::keyPressEvent(e);
                } else {
                    // in overwrite mode, backspace should insert a space
                    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                    QString selectedText = cursor.selectedText();
                    if (selectedText == QString(" ") ||
                        selectedText == QString("\t") ) {
                        cursor.clearSelection();
                    } else {
                        cursor.insertText(QString(QChar(' ')));
                        cursor.movePosition(QTextCursor::PreviousCharacter);
                    }
                }
                updateCursor = true;
            }
            break;

        case Qt::Key_Down:
        {
            if (cursor.block() == textDocument()->lastBlock()) {
                QTextCursor::MoveMode moveMode = e->modifiers() & Qt::SHIFT ? QTextCursor::KeepAnchor
                                                                            : QTextCursor::MoveAnchor;

                cursor.movePosition(QTextCursor::EndOfBlock, moveMode);
                setTextCursor(cursor);
            } else
                QPlainTextEdit::keyPressEvent(e);
            break;
        }

        case Qt::Key_Up:
        {
            if (cursor.block() == textDocument()->firstBlock()) {
                QTextCursor::MoveMode moveMode = e->modifiers() & Qt::SHIFT ? QTextCursor::KeepAnchor
                                                                            : QTextCursor::MoveAnchor;

                cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
                setTextCursor(cursor);
            } else
                QPlainTextEdit::keyPressEvent(e);
            break;
        }

        default:
            QPlainTextEdit::keyPressEvent(e);

        }
    } // else...

    if (updateCursor) {
        cursor.setVerticalMovementX(-1);
        setTextCursor( cursor );
        ensureCursorVisible();
    }
    doKeyAction(e);
}
    
void GenericCodeEditor::keyReleaseEvent(QKeyEvent * e)
{
    doKeyAction(e);
}
    
void GenericCodeEditor::doKeyAction( QKeyEvent * ke )
{
    int key = ke->key();
    
    int mods = ke->modifiers();
    
    QChar character;
    
#ifdef Q_WS_MAC
    bool isLetter = key >= Qt::Key_A && key <= Qt::Key_Z;
    if (mods & Qt::MetaModifier && isLetter)
    {
        character = QChar(key - Qt::Key_A + 1);
    }
    else if(mods & Qt::AltModifier && isLetter)
    {
        character = (mods & Qt::ShiftModifier) ? QChar(key) : QChar(key - Qt::Key_A + 97 );
    }
    else
#endif
    {
        QString text( ke->text() );
        if (text.count()){
            character = text[0];
        } else {
            character = QChar(QChar::ReplacementCharacter);
        }
    }
    
    int unicode = character.unicode();
    
#ifdef Q_WS_X11
    KeySym sym = ke->nativeVirtualKey();
    int keycode = XKeysymToKeycode( QX11Info::display(), sym );
#else
    // FIXME: On Mac OS X, this does not work for modifier keys
    int keycode = ke->nativeVirtualKey();
#endif
    QString type;
    
    if(ke->type() == QEvent::KeyPress)
    {
        type = QString("keyDown");
    } else {
        type = QString("keyUp");
    }

    Main::evaluateCodeIfCompiled(QString("ScIDEDocument.findByQUuid(\'%1\').%2(%3, %4, %5, %6)").arg(mDoc->id().constData()).arg(type).arg(mods).arg(unicode).arg(keycode).arg(key), true);
    
}
    
void GenericCodeEditor::mousePressEvent(QMouseEvent * e)
{
    int button;
    
    switch( e->button() ) {
        case Qt::LeftButton:
            button = 0; break;
        case Qt::RightButton:
            button = 1; break;
        case Qt::MidButton:
            button = 2; break;
        default:
            button = -1;
    }
    
    Main::evaluateCodeIfCompiled(QString("ScIDEDocument.findByQUuid(\'%1\').mouseDown(%2, %3, %4, %5, 1)").arg(mDoc->id().constData()).arg(e->x()).arg(e->y()).arg(e->modifiers()).arg(button), true);
    QPlainTextEdit::mousePressEvent(e);
}
    
void GenericCodeEditor::mouseDoubleClickEvent(QMouseEvent * e)
{
    int button;
    
    switch( e->button() ) {
        case Qt::LeftButton:
            button = 0; break;
        case Qt::RightButton:
            button = 1; break;
        case Qt::MidButton:
            button = 2; break;
        default:
            button = -1;
    }
    
    Main::evaluateCodeIfCompiled(QString("ScIDEDocument.findByQUuid(\'%1\').mouseDown(%2, %3, %4, %5, 2)").arg(mDoc->id().constData()).arg(e->x()).arg(e->y()).arg(e->modifiers()).arg(button), true);
    QPlainTextEdit::mouseDoubleClickEvent(e);
}
    
void GenericCodeEditor::mouseReleaseEvent(QMouseEvent * e)
{
    int button;
    
    switch( e->button() ) {
        case Qt::LeftButton:
            button = 0; break;
        case Qt::RightButton:
            button = 1; break;
        case Qt::MidButton:
            button = 2; break;
        default:
            button = -1;
    }
    
    Main::evaluateCodeIfCompiled(QString("ScIDEDocument.findByQUuid(\'%1\').mouseUp(%2, %3, %4, %5)").arg(mDoc->id().constData()).arg(e->x()).arg(e->y()).arg(e->modifiers()).arg(button), true);
    QPlainTextEdit::mouseReleaseEvent(e);
}

void GenericCodeEditor::wheelEvent( QWheelEvent * e )
{
    // FIXME: Disable zooming for now, to avoid nasty effect when Ctrl
    // is unintentionally pressed while inertial scrolling is going on.

    // Moreover, Ctrl|Shift + Wheel scrolls by pages, which is also
    // rather annoying.

    // So rather just forward the event without modifiers.

    QWheelEvent modifiedEvent( e->pos(), e->globalPos(), e->delta(),
                               e->buttons(), 0, e->orientation() );
    QPlainTextEdit::wheelEvent( &modifiedEvent );
    return;

#if 0
    if (e->modifiers() == Qt::ControlModifier) {
        if (e->delta() > 0)
            zoomIn();
        else
            zoomOut();
        return;
    }

    QPlainTextEdit::wheelEvent(e);
#endif
}

void GenericCodeEditor::dragEnterEvent( QDragEnterEvent * event )
{
    const QMimeData * data = event->mimeData();
    if (data->hasUrls()) {
        // Propagate event to parent.
        // URL drops are ultimately handled by MainWindow.
        event->ignore();
        return;
    }

    QPlainTextEdit::dragEnterEvent(event);
}

void GenericCodeEditor::hideMouseCursor(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
    case Qt::Key_Hyper_L:
    case Qt::Key_Hyper_R:
    case Qt::Key_Menu:
        return;

    default:
        hideMouseCursor();
    }
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
    mOverlayWidget->setGeometry( viewport()->geometry() );
}

void GenericCodeEditor::updateLineIndicator( QRect r, int dy )
{
    if (dy)
        mLineIndicator->scroll(0, dy);
    else
        mLineIndicator->update(0, r.y(), mLineIndicator->width(), r.height() );
}

void GenericCodeEditor::onCursorPositionChanged()
{
    if (mHighlightCurrentLine)
        updateCurrentLineHighlighting();
    mLastCursorBlock = textCursor().blockNumber();
}

void GenericCodeEditor::updateCurrentLineHighlighting()
{
    int currentCursorBlock = textCursor().blockNumber();
    int first_block_num = qMin(mLastCursorBlock, currentCursorBlock);
    int second_block_num = qMax(mLastCursorBlock, currentCursorBlock);

    QRegion region(0,0,0,0);

    QTextBlock block = firstVisibleBlock();
    int block_num = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal max_top = viewport()->rect().bottom();
    while (block.isValid() && block_num <= second_block_num && top <= max_top)
    {
        QRectF block_rect = blockBoundingRect(block);
        if (block_num == first_block_num || block_num == second_block_num) {
            region += block_rect.translated(0, top).toRect();
        }
        top += block_rect.height();
        block = block.next();
        ++block_num;
    }

    viewport()->update( region );
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

    mOverlayWidget->setGeometry( viewport()->geometry() );
}

void GenericCodeEditor::paintLineIndicator( QPaintEvent *e )
{
    QPalette plt( mLineIndicator->palette() );
    QRect r( e->rect() );
    QPainter p( mLineIndicator );

    p.fillRect( r, plt.color( QPalette::Mid ) );
    p.setPen( plt.color(QPalette::Dark) );
    p.drawLine( r.topRight(), r.bottomRight() );

    p.setPen( plt.color(QPalette::ButtonText) );

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
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top + blockBoundingRect(block).height();

    while (block.isValid() && top <= e->rect().bottom()) {
        if (block.isVisible() && bottom >= e->rect().top()) {
            p.save();

            QRectF numRect( 0, top, mLineIndicator->width() - 1, bottom - top );

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
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void GenericCodeEditor::paintEvent( QPaintEvent *event )
{
    if (mHighlightCurrentLine)
    {
        int cursor_block_num = textCursor().blockNumber();
        QTextBlock block = firstVisibleBlock();
        int block_num = block.blockNumber();
        qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
        qreal max_top = event->rect().bottom();
        while (block.isValid() && block_num <= cursor_block_num && top <= max_top)
        {
            QRectF block_rect = blockBoundingRect(block);
            if (block_num == cursor_block_num) {
                QPainter painter(viewport());
                painter.fillRect(
                            block_rect.translated(0, top),
                            mCurrentLineTextFormat.background().color() );
                painter.end();
                break;
            }
            top += block_rect.height();
            block = block.next();
            ++block_num;
        }
    }

    QPlainTextEdit::paintEvent(event);
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
}


void GenericCodeEditor::toggleOverwriteMode()
{
    setOverwriteMode(!overwriteMode());

    // FIXME: reset cursor to the same position in to force a repaint
    //        there might be a nicer solution for this issue, though
    setTextCursor(textCursor());
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
#ifdef Q_OS_MAC
    return; // LATER: for some reason this crashes on osx. we should try to figure out why
#endif
    QCursor * overrideCursor = QApplication::overrideCursor();
    if (!overrideCursor || overrideCursor->shape() != Qt::BlankCursor)
        QApplication::setOverrideCursor( Qt::BlankCursor );
}

QMimeData *GenericCodeEditor::createMimeDataFromSelection() const
{
    // Here, we bundle up just the plaintext (not HTML, as is the default) of
    // the editor's selected contents.
    QMimeData *data = new QMimeData;
    data->setText(textCursor().selection().toPlainText());
    return data;
}

} // namespace ScIDE
