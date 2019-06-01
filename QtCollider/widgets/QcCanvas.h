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

#pragma once

#include "image_painter.h"

#include <QWidget>
#include <QPixmap>
#include <QBasicTimer>
#include <QTime>
#include <QPalette>
#include <QGLWidget>

// using QcCanvasBase = QGLWidget;
using QcCanvasBase = QWidget;

class QcCanvas : public QcCanvasBase {
    Q_PROPERTY(bool clearOnRefresh READ clearOnRefresh WRITE setClearOnRefresh);
    Q_PROPERTY(bool drawingEnabled READ drawingEnabled WRITE setDrawingEnabled);
    Q_PROPERTY(float frameRate READ frameRate WRITE setFrameRate);
    Q_PROPERTY(int frameCount READ frameCount);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_OBJECT
public:
    QcCanvas(QWidget* parent = 0);
    bool drawingEnabled() const { return _paint; }
    void setDrawingEnabled(bool b) { _paint = b; }
    bool clearOnRefresh() const { return _clearOnRefresh; }
    void setClearOnRefresh(bool b) { _clearOnRefresh = b; }
    float frameRate() const;
    void setFrameRate(float rate);
    int frameCount() const { return _frameCount; }
    QColor background() const { return _bkg; }
    void setBackground(const QColor& c);
    Q_INVOKABLE
    void setBackgroundImage(const QtCollider::SharedImage& image, const QRectF& rect, int tileMode, double opacity);
    Q_INVOKABLE
    void removeBackgroundImage() {
        _bkg_image.clear();
        update();
    }

public Q_SLOTS:
    void refresh();
    void clear();
    void animate(bool toggle);
Q_SIGNALS:
    void painting(QPainter*);

protected:
    virtual void customEvent(QEvent*);
    virtual void changeEvent(QEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void timerEvent(QTimerEvent*);

private:
    QPixmap _pixmap;
    bool _paint;
    bool _repaintNeeded;
    bool _clearOnRefresh;
    bool _clearOnce;
    bool _resize;
    float _fps;
    float _fpsActual;
    int _timerId;
    bool _animating;
    int _frameCount; // SC has no idea of unsigned integers

    QBasicTimer _fpsTimer;
    int _meterPeriod; // msecs between actual fps recalculation
    QTime _meterTime;
    int _meterFrames;

    QColor _bkg;
    QtCollider::ImagePainter _bkg_image;
};
