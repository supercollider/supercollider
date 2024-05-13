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

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"
#include "../style/style.hpp"
#include "image_painter.h"

#include <QWidget>

class QcSlider2D : public QWidget, QcHelper, QcAbstractStepValue, QtCollider::Style::Client {
    Q_OBJECT
    Q_PROPERTY(double xValue READ xValue WRITE setXValue)
    Q_PROPERTY(double yValue READ yValue WRITE setYValue)
    Q_PROPERTY(double shiftScale READ dummyFloat WRITE setShiftScale);
    Q_PROPERTY(double ctrlScale READ dummyFloat WRITE setCtrlScale);
    Q_PROPERTY(double altScale READ dummyFloat WRITE setAltScale);
    Q_PROPERTY(double step READ dummyFloat WRITE setStep)
    Q_PROPERTY(QColor grooveColor READ grooveColor WRITE setGrooveColor);
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);
    Q_PROPERTY(QColor knobColor READ knobColor WRITE setKnobColor);

public:
    QcSlider2D();
    double xValue() const { return _x; }
    double yValue() const { return _y; }
    void setXValue(double x) { setValue(QPointF(x, _y), false); }
    void setYValue(double y) { setValue(QPointF(_x, y), false); }
    void setStep(double f) { _step = f; }
    const QColor& knobColor() const {
        return _knobColor.isValid() ? _knobColor : palette().color(QPalette::ButtonText);
    }
    void setKnobColor(const QColor& c) {
        _knobColor = c;
        update();
    }
    QSize sizeHint() const { return QSize(150, 150); }
    QSize minimumSizeHint() const { return QSize(30, 30); }

    Q_INVOKABLE
    void setBackgroundImage(const QtCollider::SharedImage& image, const QRectF& rect, int tileMode, double opacity);
public Q_SLOTS:
    void incrementX(double factor = 1.f);
    void decrementX(double factor = 1.f);
    void incrementY(double factor = 1.f);
    void decrementY(double factor = 1.f);
Q_SIGNALS:
    void action();
    void randomize();

private:
    QRect thumbRect();
    void setValue(const QPointF val, bool doAction = true);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void paintEvent(QPaintEvent*);

    double _x;
    double _y;
    QSize _thumbSize;
    double _step;

    QColor _knobColor;
    QtCollider::ImagePainter _backgroundImage;
};
