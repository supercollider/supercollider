/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcScope.h"
#include "../QcWidgetFactory.h"

#include <QPainter>
#include <QTimer>

QC_DECLARE_QWIDGET_FACTORY(QcScope);

QcScope::QcScope(): bufNum(0), xOffset(0.f), yOffset(0.f), xZoom(1.f), yZoom(1.f), style(0), _bkg(QColor(0, 0, 0)) {
    memset(&buffer, 0, sizeof(SndBuf));
    timer = new QTimer(this);
    timer->setInterval(15);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateScope()));
}

QcScope::~QcScope() { free(buffer.data); }

void QcScope::setBufferNumber(int n) {
    bufNum = n;
    timer->start();
}

void QcScope::setWaveColors(const QVariantList& newColors) {
    colors.clear();
    Q_FOREACH (const QVariant& var, newColors) {
        QColor color = var.value<QColor>();
        if (!color.isValid())
            colors.append(QColor(0, 0, 0));
        else
            colors.append(color);
    }
}

int QcScope::updateInterval() const { return timer->interval(); }

void QcScope::setUpdateInterval(int interval) { timer->setInterval(qMax(0, interval)); }

int getScopeBuf(uint32 index, SndBuf* buf, bool& changed);

void QcScope::updateScope() {
#ifndef NO_INTERNAL_SERVER
    // printf("update\n");
    bool changed;
    getScopeBuf(bufNum, &buffer, changed);
    if (changed) {
        // printf("channels: %i\n", buffer.channels);
        update();
    }
#endif
}

inline void QcScope::setPoint(QPointF& pt, float x, float y, float xRatio, float yRatio, int xStart, int yStart) {
    pt.setX(((x + xOffset) * xRatio) + xStart);
    pt.setY(yStart - ((y + yOffset) * yRatio));
}

void QcScope::paint1D(bool overlapped, QPainter& p) {
    QRect area = rect();
    float xRatio = xZoom * area.width() / (float)buffer.frames;
    float yRatio = yZoom * area.height() * 0.5;
    if (!overlapped)
        yRatio /= buffer.channels;

    int c, f, s;
    QPointF pt1;
    QPointF pt2;
    QPen pen;
    pen.setWidth(0); // width==0 means width 1 regardless of transformations

    for (c = 0; c < buffer.channels; c++) {
        pen.setColor(c < colors.count() ? colors[c] : QColor(255, 255, 255));
        p.setPen(pen);

        float yCenter = area.height() * 0.5;
        if (!overlapped) {
            yCenter *= (c * 2 + 1);
            yCenter /= buffer.channels;
        }
        yCenter += area.y();

        setPoint(pt1, 0, buffer.data[c], xRatio, yRatio, area.x(), yCenter);

        for (f = 1, s = c + buffer.channels; f < buffer.frames; f++, s += buffer.channels) {
            setPoint(pt2, f, buffer.data[s], xRatio, yRatio, area.x(), yCenter);
            p.drawLine(pt1, pt2);
            pt1 = pt2;
        }
    }
}

void QcScope::paint2D(QPainter& p) {
    QPen pen;
    pen.setWidth(0); // width==0 means width 1 regardless of transformations
    pen.setColor(colors.count() ? colors[0] : QColor(255, 255, 255));

    QRect area = rect();
    int minSize = qMin(area.width(), area.height());
    float xRatio = xZoom * minSize * 0.5;
    float yRatio = yZoom * minSize * 0.5;
    QPoint center = area.center();
    int centerY = center.y();
    int centerX = center.x();

    QPointF pt1;
    QPointF pt2;
    float* data = buffer.data;
    float y = buffer.channels > 1 ? data[1] : 0.f;

    setPoint(pt1, data[0], y, xRatio, yRatio, centerX, centerY);
    data += buffer.channels;

    int f;
    for (f = 1; f < buffer.frames; f++, data += buffer.channels) {
        if (buffer.channels > 1)
            y = data[1];
        setPoint(pt2, data[0], y, xRatio, yRatio, centerX, centerY);
        p.drawLine(pt1, pt2);
        pt1 = pt2;
    }
}

void QcScope::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter p(this);
    QRect area = rect();
    p.fillRect(area, _bkg);

    if (buffer.frames == 0)
        return;

    if (style == 0)
        paint1D(false, p);
    else if (style == 1)
        paint1D(true, p);
    else if (style == 2)
        paint2D(p);
}
