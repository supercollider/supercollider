/************************************************************************
*
* This file is part of SuperCollider Qt GUI.
*
* Copyright 2013 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QT_COLLIDER_IMAGE_INCLUDED
#define QT_COLLIDER_IMAGE_INCLUDED

#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QSharedPointer>
#include <QMetaType>
#include <cassert>

namespace QtCollider {

class Image
{
    enum State {
        Null,
        ImageState,
        PixmapState
    };

public:
    Image():
        transformationMode(Qt::FastTransformation),
        m_state(Null),
        m_painting(false)
    {

    }

    void setImage( const QImage & image )
    {
        assert(!m_painting);
        m_pixmap = QPixmap();
        m_image = image;
        m_state = ImageState;
    }

    void setPixmap( const QPixmap & pixmap )
    {
        assert(!m_painting);
        m_image = QImage();
        m_pixmap = pixmap;
        m_state = PixmapState;
    }

    QImage & image()
    {
        if (m_state == ImageState)
            return m_image;
        assert(!m_painting);
        if (m_state == PixmapState) {
            m_image = m_pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
            m_pixmap = QPixmap();
        }
        m_state = ImageState;
        return m_image;
    }

    QPixmap & pixmap()
    {
        assert(!m_painting);
        if (m_state == PixmapState)
            return m_pixmap;
        if (m_state == ImageState) {
            m_pixmap = QPixmap::fromImage(m_image);
            m_image = QImage();
        }
        m_state = PixmapState;
        return m_pixmap;
    }

    void clear()
    {
        assert(!m_painting);
        m_image = QImage();
        m_pixmap = QPixmap();
        m_state = Null;
    }

    bool isNull() const
    {
        return m_state == Null;
    }

    int width() const
    {
        switch (m_state) {
        case ImageState:
            return m_image.width();
        case PixmapState:
            return m_pixmap.width();
        default:
            return 0;
        }
    }

    int height() const
    {
        switch (m_state) {
        case ImageState:
            return m_image.height();
        case PixmapState:
            return m_pixmap.height();
        default:
            return 0;
        }
    }

    QRect rect() const
    {
        switch (m_state) {
        case ImageState:
            return m_image.rect();
        case PixmapState:
            return m_pixmap.rect();
        default:
            return QRect();
        }
    }

    void resize( const QSize &new_size, int resize_mode )
    {
        assert(!m_painting);

        if (m_state == Null)
            return;

        switch (resize_mode) {
        case 0:
        {
            if (m_state == ImageState) {
                QImage new_image( new_size, QImage::Format_ARGB32_Premultiplied );
                new_image.fill( Qt::transparent );
                QPainter painter(&new_image);
                painter.drawImage( QPointF(0,0), m_image );
                painter.end();
                m_image = new_image;
            }
            else {
                QPixmap new_pixmap( new_size );
                new_pixmap.fill( Qt::transparent );
                QPainter painter(&new_pixmap);
                painter.drawPixmap( QPointF(0,0), m_pixmap );
                painter.end();
                m_pixmap = new_pixmap;
            }
            break;
        }
        case 1:
        case 2:
        case 3:
        {
            Qt::AspectRatioMode aspectRatioMode = (Qt::AspectRatioMode)(resize_mode - 1);
            if (m_state == ImageState) {
                m_image = m_image.scaled( new_size, aspectRatioMode, transformationMode );
            }
            else {
                m_pixmap = m_pixmap.scaled( new_size, aspectRatioMode, transformationMode );
            }
            break;
        }
        default:
            break;
        }
    }

    bool isPainting() const { return m_painting; }
    void setPainting( bool painting ) { m_painting = painting; }

    Qt::TransformationMode transformationMode;

private:
    QImage m_image;
    QPixmap m_pixmap;
    State m_state;
    bool m_painting;
};

typedef QSharedPointer<QtCollider::Image> SharedImage;

} // namespace QtCollider

Q_DECLARE_METATYPE( QtCollider::SharedImage );

#endif // QT_COLLIDER_IMAGE_INCLUDED
