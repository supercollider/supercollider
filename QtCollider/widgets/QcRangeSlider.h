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

#include <QWidget>

class QcRangeSlider : public QWidget, QcHelper, QcAbstractStepValue, QtCollider::Style::Client {
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation);
    Q_PROPERTY(double loValue READ loValue WRITE setLoValue)
    Q_PROPERTY(double hiValue READ hiValue WRITE setHiValue)
    Q_PROPERTY(double shiftScale READ dummyFloat WRITE setShiftScale);
    Q_PROPERTY(double ctrlScale READ dummyFloat WRITE setCtrlScale);
    Q_PROPERTY(double altScale READ dummyFloat WRITE setAltScale);
    Q_PROPERTY(double step READ dummyFloat WRITE setStep)
    Q_PROPERTY(QColor grooveColor READ grooveColor WRITE setGrooveColor);
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);
    Q_PROPERTY(QColor knobColor READ knobColor WRITE setKnobColor);

public:
    enum MouseMode { None = 0, Move, MoveHi, MoveLo, SetLo, SetHi };

    QcRangeSlider();
    Qt::Orientation orientation() const { return _ort; }
    void setOrientation(Qt::Orientation o);
    double loValue() const { return _lo; }
    void setLoValue(double);
    double hiValue() const { return _hi; }
    void setHiValue(double);
    void setRange(double val, double range);
    void setStep(double f) { _step = f; }
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    const QColor& knobColor() const {
        return _knobColor.isValid() ? _knobColor : palette().color(QPalette::ButtonText);
    }
    void setKnobColor(const QColor& c) {
        _knobColor = c;
        update();
    }

public Q_SLOTS:
    void increment(double factor);
    void decrement(double factor);

Q_SIGNALS:
    void action();

private:
    QRect thumbRect();
    QRect valueRect();
    double valueFromPos(const QPoint& pos);
    void moveBy(double);
    void increment();
    void decrement();
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void paintEvent(QPaintEvent*);

    Qt::Orientation _ort;
    double _lo;
    double _hi;
    double _step;
    QPoint dragOrigin;
    double dragVal, dragRange;
    MouseMode mouseMode;

    QColor _knobColor;
};
