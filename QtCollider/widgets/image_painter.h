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

#include "../image.h"
#include "../debug.h"

#include <QPainter>

namespace QtCollider {

struct ImagePainter
{
    enum HorizontalMode {
        AlignLeft,
        AlignHCenter,
        AlignRight,
        TileHorizontally,
        StretchHorizontally,
    };

    enum VerticalMode {
        AlignTop,
        AlignVCenter,
        AlignBottom,
        TileVertically,
        StretchVertically
    };

    SharedImage image;
    QRectF sourceRect;
    HorizontalMode horizontalMode;
    VerticalMode verticalMode;
    bool scaleToFit;
    qreal opacity;

    ImagePainter():
        horizontalMode(AlignLeft),
        verticalMode(AlignTop),
        scaleToFit(false),
        opacity(1.0)
    {}

    bool isValid() const { return !image.isNull(); }

    void setImage( const SharedImage & image, const QRectF & rect = QRectF(),
                    int tileMode = 1, qreal opacity = 1.0 )
    {
        this->image = image;
        this->sourceRect = rect;
        this->opacity = opacity;
        setTileMode(tileMode);
    }

    void clear() { image.clear(); }

    void paint ( QPainter * painter, const QRectF & targetRect )
    {
        if (!image)
            return;

        if (image->isPainting()) {
            qcErrorMsg("Can not draw image while being painted.");
            return;
        }

        const QPixmap & pixmap = image->pixmap();

        if (sourceRect.isNull() || targetRect.isNull())
            return;

        painter->save();
        painter->setOpacity(opacity);
        painter->setRenderHint( QPainter::SmoothPixmapTransform,
                                image->transformationMode == Qt::SmoothTransformation );

        QRectF rect = sourceRect;

        if (horizontalMode == StretchHorizontally) {
            rect.moveLeft(targetRect.left());
            rect.setWidth(targetRect.width());
        }
        if (verticalMode == StretchVertically) {
            rect.moveTop(targetRect.top());
            rect.setHeight(targetRect.height());
        }
        if ( horizontalMode != StretchHorizontally &&
             verticalMode != StretchVertically &&
             scaleToFit )
        {
            float aspect_ratio = rect.width() / rect.height();
            rect.setHeight( targetRect.height() );
            rect.setWidth( rect.height() * aspect_ratio );
            if (rect.width() > targetRect.width()) {
                rect.setWidth( targetRect.width() );
                rect.setHeight( rect.width() / aspect_ratio );
            }
        }

        switch (horizontalMode) {
        case AlignLeft:
        case TileHorizontally:
            rect.moveLeft(targetRect.left()); break;
        case AlignHCenter:
            rect.moveLeft(targetRect.left() + targetRect.width() / 2 - rect.width() / 2); break;
        case AlignRight:
            rect.moveRight(targetRect.right()); break;
        default:
            break;
        };

        switch (verticalMode) {
        case AlignTop:
        case TileVertically:
            rect.moveTop(targetRect.top()); break;
        case AlignVCenter:
            rect.moveTop(targetRect.top() + targetRect.height() / 2  - rect.height() / 2); break;
        case AlignRight:
            rect.moveBottom(targetRect.bottom()); break;
        default:
            break;
        };

        bool tileVertically = verticalMode == TileVertically;
        bool tileHorizontally = horizontalMode == TileHorizontally;

        qreal y_origin = rect.top();
        do {
            do {
                painter->drawPixmap( rect, pixmap, sourceRect);

                if (tileVertically)
                    rect.moveTop( rect.top() + rect.height() );
                else
                    break;
            } while( rect.top() <= targetRect.bottom() );

            if (tileHorizontally) {
                rect.moveTop( y_origin );
                rect.moveLeft( rect.left() + rect.width() );
            }
            else
                break;
        } while (rect.left() <= targetRect.right());

        painter->restore();
    }

    void setTileMode( const int mode )
    {
        /*
            modes :
            1 - fixed to left, fixed to top
            2 - horizontally tile, fixed to top
            3 - fixed to right, fixed to top
            4 - fixed to left, vertically tile
            5 - horizontally tile, vertically tile
            6 - fixed to right, vertically tile
            7 - fixed to left, fixed to bottom
            8 - horizontally tile, fixed to bottom
            9 - fixed to right, fixed to bottom
            10 - fit
            11 - center, center (scale)
            12 - center , fixed to top
            13 - center , fixed to bottom
            14 - fixed to left, center
            15 - fixed to right, center
            16 - center, center (no scale)
        */

        int mode_map_index = mode - 1;
        if (mode_map_index < 0 || mode_map_index >= 16)
            return;

        static int mode_map[16][2] = {
            { AlignLeft, AlignTop },
            { TileHorizontally, AlignTop },
            { AlignRight, AlignTop },
            { AlignLeft, TileVertically },
            { TileHorizontally, TileVertically },
            { AlignRight, TileVertically },
            { AlignLeft, AlignBottom },
            { TileHorizontally, AlignBottom },
            { AlignRight, AlignBottom },
            { StretchHorizontally, StretchVertically },
            { AlignHCenter, AlignVCenter },
            { AlignHCenter, AlignTop },
            { AlignHCenter, AlignBottom },
            { AlignLeft, AlignVCenter },
            { AlignRight, AlignVCenter },
            { AlignHCenter, AlignVCenter }
        };

        horizontalMode = (HorizontalMode) mode_map[mode_map_index][0];
        verticalMode = (VerticalMode) mode_map[mode_map_index][1];
        scaleToFit = (mode == 11);
    }
};

} // namespace QtCollider

#endif // QT_COLLIDER_IMAGE_PAINTER_INCLUDED
