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

#include "../QcHelper.h"

#include <SC_Types.h>
#include <SC_SndBuf.h>

#include <QWidget>

class QcScope : public QWidget, QcHelper {
    Q_OBJECT
    Q_PROPERTY(int bufferNumber READ dummyInt WRITE setBufferNumber);
    Q_PROPERTY(float xOffset READ dummyFloat WRITE setXOffset);
    Q_PROPERTY(float yOffset READ dummyFloat WRITE setYOffset);
    Q_PROPERTY(float xZoom READ dummyFloat WRITE setXZoom);
    Q_PROPERTY(float yZoom READ dummyFloat WRITE setYZoom);
    Q_PROPERTY(int style READ dummyInt WRITE setStyle);
    Q_PROPERTY(QVariantList waveColors READ dummyVariantList WRITE setWaveColors);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval);

public:
    QcScope();
    ~QcScope();
    void setBufferNumber(int);
    void setXOffset(float f) { xOffset = f; }
    void setYOffset(float f) { yOffset = f; }
    void setXZoom(float f) { xZoom = f; }
    void setYZoom(float f) { yZoom = f; }
    void setStyle(int i) { style = i; }
    void setWaveColors(const QVariantList& colors);
    QColor background() const { return _bkg; }
    void setBackground(const QColor& c) {
        _bkg = c;
        update();
    }
    int updateInterval() const;
    void setUpdateInterval(int i);
    QSize sizeHint() const { return QSize(500, 300); }
    QSize minimumSizeHint() const { return QSize(50, 50); }
private Q_SLOTS:
    void updateScope();

private:
    void paintEvent(QPaintEvent* event);
    void setPoint(QPointF& pt, float frame, float data, float xRatio, float yRatio, int xStart, int yStart);
    void paint1D(bool overlapped, QPainter&);
    void paint2D(QPainter&);

    int bufNum;
    SndBuf buffer;
    QTimer* timer;
    float xOffset;
    float yOffset;
    float xZoom;
    float yZoom;
    int style;
    QList<QColor> colors;
    QColor _bkg;
};
