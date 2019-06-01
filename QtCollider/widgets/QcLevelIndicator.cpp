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

#include "QcLevelIndicator.h"
#include "../QcWidgetFactory.h"

#include <QPainter>

QC_DECLARE_QWIDGET_FACTORY(QcLevelIndicator);

QcLevelIndicator::QcLevelIndicator():
    QtCollider::Style::Client(this),
    _value(0.f),
    _warning(0.6),
    _critical(0.8),
    _peak(0.f),
    _drawPeak(false),
    _ticks(0),
    _majorTicks(0),
    _stepWidth(10),
    _style(LevelIndicatorStyle::Continuous),
    _clipped(false),
    _meterColor(0, 255, 0),
    _warningColor(255, 255, 0),
    _criticalColor(255, 100, 0) {
    _clipTimer = new QTimer(this);
    _clipTimer->setSingleShot(true);
    _clipTimer->setInterval(1000);
    connect(_clipTimer, SIGNAL(timeout()), this, SLOT(clipTimeout()));
}

const QColor QcLevelIndicator::valueColor(float value) {
    if (value > _critical)
        return _criticalColor;
    else if (value > _warning)
        return _warningColor;
    else
        return _meterColor;
}

void QcLevelIndicator::clipTimeout() {
    _clipped = false;
    update();
}

void QcLevelIndicator::paintEvent(QPaintEvent* e) {
    QPainter p(this);

    bool vertical = height() >= width();
    float groove = vertical ? width() : height();
    float length = vertical ? height() : width();

    if (!vertical) {
        p.scale(-1, 1);
        p.translate(0, 0);
        p.rotate(90);
    } else {
        p.scale(1, -1);
        p.translate(0, -height());
    }

    QPalette plt = palette();


    if (_ticks || _majorTicks)
        groove -= 6;

    float colorValue = _drawPeak ? _peak : _value;

    if (colorValue > _critical) {
        _clipTimer->stop();
        _clipped = true;
    } else if (_clipped && !_clipTimer->isActive()) {
        _clipTimer->start();
    }

    p.fillRect(QRectF(0, 0, groove, length), grooveColor());

    QRectF r;
    r.setWidth(groove);
    r.setY(0);

    p.setRenderHint(QPainter::Antialiasing, true);

    switch (_style) {
    case Continuous: {
        r.setHeight(_value * length);
        p.fillRect(r, valueColor(colorValue));
        if (_drawPeak && _peak > 0.f) {
            // compensate for border and peak line width
            float val = _peak * (length - 4) + 2;

            QPen pen(valueColor(_peak));
            pen.setWidth(2);
            pen.setCapStyle(Qt::FlatCap);
            p.setPen(pen);
            p.drawLine(0.f, val, groove, val);
        }

        break;
    }
    case LED: {
        float ledBaseline = 0;
        float spaceWidth = _stepWidth <= 3 ? 1 : 2;
        float cornerWidth = _stepWidth <= 3 ? 0 : 1.2;
        float stepSpacing = _stepWidth + spaceWidth;
        float adjustedLength = std::floor(length / stepSpacing) * stepSpacing;

        r.setHeight(_stepWidth);
        QPainterPath path;
        path.addRoundedRect(r, cornerWidth, cornerWidth);

        while (ledBaseline < adjustedLength * _value) {
            float normValue = (ledBaseline / adjustedLength); // 0..1 scaled value of the BOTTOM of the rect
            float nextValue = ((ledBaseline + _stepWidth + spaceWidth) / adjustedLength);

            QColor c = valueColor(normValue);
            if (nextValue > _value) { // last cell
                c = valueColor(_value);
                c.setAlphaF(c.alphaF() * (0.5 + 0.5 * ((_value - normValue) / (nextValue - normValue))));
            }

            p.fillPath(path, QBrush(c));
            ledBaseline += stepSpacing;
            path.translate(0, stepSpacing);
        }

        if (_drawPeak && _peak > 0.f) {
            float peak = std::floor(_peak * adjustedLength / stepSpacing) * stepSpacing;
            peak = std::min(peak, adjustedLength - stepSpacing); // For _peak == 1, don't run off edge of widget

            QPainterPath peakPath;
            peakPath.addRoundedRect(r, cornerWidth, cornerWidth);
            peakPath.translate(0, peak);

            p.fillPath(peakPath, QBrush(valueColor(_peak)));
        }

        break;
    }
    default:
        break;
    }

    p.setRenderHint(QPainter::Antialiasing, false);

    if (_ticks) {
        QPen pen(plt.color(QPalette::WindowText));
        pen.setCapStyle(Qt::FlatCap);
        p.setPen(pen);
        float dVal = (_ticks > 1) ? (length - 1) / (float)(_ticks - 1) : 0.f;
        float t = 0;
        while (t < _ticks) {
            float v = t * dVal;
            p.drawLine(groove, v, width(), v);
            t++;
        }
    }

    if (_majorTicks) {
        QPen pen(plt.color(QPalette::WindowText));
        pen.setCapStyle(Qt::FlatCap);
        pen.setWidth(3);
        p.setPen(pen);
        float dVal = (_majorTicks > 1) ? (length - 3) / (float)(_majorTicks - 1) : 0.f;
        float t = 0;
        while (t < _majorTicks) {
            float v = (int)(t * dVal) + 1;
            if (vertical)
                p.drawLine(groove, v, width(), v);
            else
                p.drawLine(v, groove, v, height());
            t++;
        }
    }
}
