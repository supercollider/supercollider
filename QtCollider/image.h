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
        case Null:
            return 0;
        case ImageState:
            return m_image.width();
        case PixmapState:
            return m_pixmap.width();
        }
        return 0;
    }

    int height() const
    {
        switch (m_state) {
        case Null:
            return 0;
        case ImageState:
            return m_image.height();
        case PixmapState:
            return m_pixmap.height();
        }
        return 0;
    }

    bool isPainting() const { return m_painting; }

    void setPainting( bool painting ) { m_painting = painting; }

private:
    QImage m_image;
    QPixmap m_pixmap;
    State m_state;
    bool m_painting;
};

} // namespace QtCollider

#endif // QT_COLLIDER_IMAGE_INCLUDED
