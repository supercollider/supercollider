/************************************************************************
 *
 * Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of Qt GUI for SuperCollider.
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

#include "../QcHelper.h"

#include <SC_Types.h>

#include <QWidget>

// FIXME: Due to Qt bug #22829, moc can not process headers that include
// boost/type_traits/detail/has_binary_operator.hp from boost 1.48, so
// we have to wrap the shm interface into a separate class.

namespace QtCollider {
class ScopeShm;
}

using QtCollider::ScopeShm;

class QcScopeShm : public QWidget, QcHelper {
    Q_OBJECT
    Q_PROPERTY(int serverPort READ serverPort WRITE setServerPort);
    Q_PROPERTY(int bufferNumber READ dummyInt WRITE setBufferNumber);
    Q_PROPERTY(float xOffset READ dummyFloat WRITE setXOffset);
    Q_PROPERTY(float yOffset READ dummyFloat WRITE setYOffset);
    Q_PROPERTY(float xZoom READ dummyFloat WRITE setXZoom);
    Q_PROPERTY(float yZoom READ dummyFloat WRITE setYZoom);
    Q_PROPERTY(int style READ style WRITE setStyle);
    Q_PROPERTY(QVariantList waveColors READ dummyVariantList WRITE setWaveColors);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(bool fill READ fill WRITE setFill);
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval);
    Q_PROPERTY(bool running READ running);

public:
    QcScopeShm();
    ~QcScopeShm();
    int serverPort() const { return _srvPort; }
    void setServerPort(int);
    void setBufferNumber(int);
    void setXOffset(float f) { xOffset = f; }
    void setYOffset(float f) { yOffset = f; }
    void setXZoom(float f) { xZoom = f; }
    void setYZoom(float f) { yZoom = f; }
    int style() const { return _style; }
    void setStyle(int i) { _style = i; }
    void setWaveColors(const QVariantList& colors);
    QColor background() const { return _bkg; }
    void setBackground(const QColor& c) {
        _bkg = c;
        update();
    }
    bool fill() { return _fill; };
    void setFill(bool b) { _fill = b; };
    int updateInterval() const;
    void setUpdateInterval(int i);
    bool running() const { return _running; }
    QSize sizeHint() const { return QSize(500, 300); }
    QSize minimumSizeHint() const { return QSize(50, 50); }
public Q_SLOTS:
    void start();
    void stop();

protected:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);
private Q_SLOTS:
    void updateScope();

private:
    void connectSharedMemory(int port);
    void initScopeReader(ScopeShm*, int index);
    void paint1D(bool overlapped, int channels, int maxFrames, int frames, const QRect& area, QPainter&);
    void paint2D(int channels, int maxFrames, int frames, const QRect& area, QPainter&);

    int _srvPort;
    int _scopeIndex;

    ScopeShm* _shm;

    bool _running;
    float* _data;
    uint _availableFrames;

    QTimer* timer;
    float xOffset;
    float yOffset;
    float xZoom;
    float yZoom;
    int _style;
    QList<QColor> colors;
    QColor _bkg;
    bool _fill;

    QPixmap _pixmap;
};
