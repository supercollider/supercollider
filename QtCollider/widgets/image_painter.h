/************************************************************************
*
* Copyright 2013 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#ifndef QT_COLLIDER_IMAGE_PAINTER_INCLUDED
#define QT_COLLIDER_IMAGE_PAINTER_INCLUDED

#include <QPainter>
#include <QPixmap>

namespace QtCollider {

struct ImagePainter
{
    enum TileMode {
        DefaultMode = 0
        // TODO
    };

    QPixmap pixmap;
    QRectF sourceRect;
    TileMode tileMode;
    qreal opacity;

    bool isValid() const { return !pixmap.isNull(); }

    void setPixmap( const QPixmap & pixmap, const QRectF & rect = QRectF(),
                    TileMode tileMode = DefaultMode, qreal opacity = 1.0 )
    {
        this->pixmap = pixmap;
        this->sourceRect = rect.isNull() ? pixmap.rect() : rect;
        this->tileMode = tileMode;
        this->opacity = opacity;
    }

    void paint ( QPainter * painter )
    {
        qreal old_opacity = painter->opacity();
        painter->setOpacity(opacity);

        // TODO: tiling modes
        painter->drawPixmap( QPointF(0,0), pixmap, sourceRect);

        painter->setOpacity(old_opacity);
    }
};

} // namespace QtCollider

#endif // QT_COLLIDER_IMAGE_PAINTER_INCLUDED
