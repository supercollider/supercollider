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

#include "overlay.hpp"
#include "editor.hpp"
#include "../../core/main.hpp"

#include <QTextDocument>
#include <QTextLayout>
#include <QTextBlock>
#include <QPainter>
#include <QPropertyAnimation>
#include <QtCore/qmath.h>
#include <QDebug>

namespace ScIDE {

void CodeEditor::blinkCode( const QTextCursor & c )
{
    if( !c.document() || !c.hasSelection() ) return;

    Settings::Manager *settings = Main::instance()->settings();
    settings->beginGroup("IDE/editor/colors");
    QColor fg = settings->value("evaluatedCodeText").value<QColor>();
    QColor bg = settings->value("evaluatedCodeBackground").value<QColor>();
    settings->endGroup();

    QTextDocument *doc = c.document();

    int startPos = c.selectionStart();
    int endPos = c.selectionEnd();
    QTextBlock startBlock = doc->findBlock(startPos);
    QTextBlock endBlock = doc->findBlock(endPos);
    startPos -= startBlock.position();
    endPos -= endBlock.position();

    // Get the bounds of visible blocks within the cursor's selection:

    QTextBlock b = firstVisibleBlock();
    int idx = b.blockNumber();
    int sidx = startBlock.blockNumber();

    QTextBlock firstBlock, lastBlock;
    firstBlock = lastBlock = b;

    QRectF geom = blockBoundingGeometry(b).translated(contentOffset());
    qreal top = geom.top();
    qreal bottom = top;
    qreal width=0;

    while(b.isValid() && bottom < viewport()->rect().height())
    {
        if(b.isVisible())
        {
            QTextLayout *l = b.layout();
            QRectF r = l->boundingRect();
            bottom += r.height();
            if(idx < sidx) {
                // Block not within the selection. Will skip it.
                top = bottom;
            }
            else {
                // Block within the selection.
                width = qMax(width, l->maximumWidth() + r.left());
            }
        }

        if(b == endBlock) break;

        b = b.next();
        ++idx;
        if(top == bottom)
            firstBlock = b;
    }

    lastBlock = b;

    if(bottom == top) {
        //qDebug("no visible block.");
        return;
    }

    // Construct a pixmap to render the code on:

    QPixmap pix( QSize(qCeil(width), qCeil(bottom - top)) );
    pix.fill(QColor(0,0,0,0));

    // Render the visible blocks:

    QPainter painter(&pix);
    QVector<QTextLayout::FormatRange> selections;
    b = firstBlock;
    int y=0;
    while(true)
    {
        QRectF r = b.layout()->boundingRect();

        // Use extra char formatting to hide code outside of selection
        // and modify the appearance of selected code:

        QTextLayout::FormatRange range;
        selections.clear();

        int start = 0;
        if(b == startBlock) {
            range.start = 0;
            range.length = startPos;
            range.format.setForeground(QColor(0,0,0,0));
            range.format.setBackground(Qt::NoBrush);
            selections.append(range);
            start = startPos;
        }

        range.start = start;
        range.length = (b == endBlock ? endPos : b.length() - 1) - range.start;
        range.format.setForeground(fg);
        range.format.setBackground(bg);
        selections.append(range);

        if(b == endBlock) {
            range.start = range.start + range.length;
            range.length = b.length() - 1 - range.start;
            range.format.setForeground(QColor(0,0,0,0));
            range.format.setBackground(Qt::NoBrush);
            selections.append(range);
        }

        b.layout()->draw(&painter, QPointF(0,y), selections);

        if(b == lastBlock) break;

        y += r.height();

        b = b.next();
    }

    // Create an overlay item to display the pixmap, and animate it:

    CodeFragmentOverlay *item = new CodeFragmentOverlay();
    item->setPixmap(pix);
    item->setPos(geom.left(), top);

    mOverlay->addItem(item);

    QPropertyAnimation *anim = new QPropertyAnimation(item, "opacity", item);
    anim->setDuration(mBlinkDuration);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve( QEasingCurve::InCubic );
    anim->start();

    connect(anim, SIGNAL(finished()), item, SLOT(deleteLater()));
}

} // namespace ScIDE
