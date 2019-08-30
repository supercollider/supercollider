/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcCanvas.h"
#include "../painting.h"
#include "../Common.h"

#include <QPainter>
#include <QPaintEvent>
#include <QBackingStore>

QcCanvas::QcCanvas(QWidget* parent):
    QcCanvasBase(parent),
    _paint(false),
    _repaintNeeded(true),
    _clearOnRefresh(true),
    _clearOnce(false),
    _resize(false),
    _fps(60.f),
    _fpsActual(0.f),
    _timerId(0),
    _animating(false),
    _frameCount(0),
    _meterPeriod(1000),
    _meterFrames(0) {}

float QcCanvas::frameRate() const { return _fpsActual; }

void QcCanvas::setFrameRate(float rate) {
    if (rate != _fps) {
        _fps = rate;
        if (_animating && _fps > 0) {
            // restart animation timer with new frame rate
            killTimer(_timerId);
            _timerId = startTimer(1000.f / _fps);
        }
    }
}

void QcCanvas::setBackground(const QColor& c) {
    if (_bkg == c)
        return;

    _bkg = c;

    setAttribute(Qt::WA_OpaquePaintEvent, c.isValid() && c.alpha() == 255);
    setAttribute(Qt::WA_TranslucentBackground, c.isValid());

    if (!testAttribute(Qt::WA_WState_InPaintEvent))
        update();
}

void QcCanvas::setBackgroundImage(const QtCollider::SharedImage& image, const QRectF& rect, int tileMode,
                                  double opacity) {
    _bkg_image.setImage(image, rect, tileMode, opacity);

    if (!testAttribute(Qt::WA_WState_InPaintEvent))
        update();
}

void QcCanvas::refresh() {
    _repaintNeeded = true;
    update();
}

void QcCanvas::clear() { _clearOnce = true; }

void QcCanvas::animate(bool on) {
    if (on) {
        if (!_animating && _fps > 0) {
            _frameCount = 0;
            _animating = true;
            _meterTime.start();
            _timerId = startTimer(1000.f / _fps);
            _fpsTimer.start(_meterPeriod, this);
        }
    } else if (_animating) {
        killTimer(_timerId);
        _fpsTimer.stop();
        _animating = false;
    }
}

void QcCanvas::customEvent(QEvent* e) {
    if (e->type() == (QEvent::Type)QtCollider::Event_Refresh) {
        e->accept();
        refresh();
    }
}

void QcCanvas::changeEvent(QEvent* e) {
    if (e->type() == QEvent::PaletteChange)
        refresh();
}

void QcCanvas::resizeEvent(QResizeEvent*) {
    _resize = true;
    refresh();
}

void QcCanvas::paintEvent(QPaintEvent* e) {
    if (_paint && _repaintNeeded) {
        if (_resize) {
            qreal pixelRatio = backingStore()->window()->devicePixelRatio();
            _pixmap = QPixmap(size() * pixelRatio);
            _pixmap.setDevicePixelRatio(pixelRatio);
            _resize = false;
            _clearOnce = true;
        }

        if (_clearOnRefresh || _clearOnce) {
            _pixmap.fill(QColor(0, 0, 0, 0));
            _clearOnce = false;
        }

        bool opaque_before = testAttribute(Qt::WA_OpaquePaintEvent);

        QPainter pixPainter(&_pixmap);
        Q_EMIT(painting(&pixPainter));
        _repaintNeeded = false;

        bool opaque_after = testAttribute(Qt::WA_OpaquePaintEvent);
        if (opaque_before && !opaque_after) {
            repaint();
            return;
        }
    }

    QPainter painter(this);
    QPalette plt(palette());

    if (_bkg.isValid())
        painter.fillRect(e->rect(), _bkg);

    if (_bkg_image.isValid())
        _bkg_image.paint(&painter, rect());

    if (_paint) {
        qreal pixelRatio = round(backingStore()->window()->devicePixelRatio());
        auto sourceRect = e->rect().intersected(QRect(0, 0, size().width(), size().height()));
        sourceRect = QRect(sourceRect.topLeft() * pixelRatio, sourceRect.size() * pixelRatio);
        painter.drawPixmap(e->rect(), _pixmap, sourceRect);
    }
}

void QcCanvas::timerEvent(QTimerEvent* e) {
    if (e->timerId() == _timerId) {
        ++_frameCount;
        ++_meterFrames;
        _repaintNeeded = true;
        repaint();
    } else if (e->timerId() == _fpsTimer.timerId()) {
        // recalc actual fps
        float dTime = _meterTime.elapsed();
        _fpsActual = (dTime > 0) ? (_meterFrames * 1000.f / dTime) : 0.f;
        // reset fps meter
        _meterTime.restart();
        _meterFrames = 0;
    }
}
