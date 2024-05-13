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

#pragma once

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"
#include "../style/style.hpp"

#include <QWidget>

class QcSlider : public QWidget, QcHelper, QcAbstractStepValue, QtCollider::Style::Client {
    Q_OBJECT
    Q_PROPERTY(double shiftScale READ dummyFloat WRITE setShiftScale);
    Q_PROPERTY(double ctrlScale READ dummyFloat WRITE setCtrlScale);
    Q_PROPERTY(double altScale READ dummyFloat WRITE setAltScale);
    Q_PROPERTY(double step READ step WRITE setStep)
    Q_PROPERTY(double pixelStep READ pixelStep)
    Q_PROPERTY(double value READ value WRITE setValue);
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation);
    Q_PROPERTY(int handleLength READ handleLength WRITE setHandleLength);
    Q_PROPERTY(QColor grooveColor READ grooveColor WRITE setGrooveColor);
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);
    Q_PROPERTY(QColor knobColor READ knobColor WRITE setKnobColor);

public Q_SLOTS:
    void increment(double factor);
    void decrement(double factor);

Q_SIGNALS:
    void action();
    void preAction(double);

public:
    QcSlider();
    double value() { return _value; }
    void setValue(double val);

    double step() { return _step; }
    void setStep(double d) { _step = d; }
    double pixelStep();

    Qt::Orientation orientation() const { return _ort; }
    void setOrientation(int);

    int handleLength() const { return _hndLen; }
    void setHandleLength(int i) {
        _hndLen = i;
        updateGeometry();
        update();
    }

    const QColor& knobColor() const {
        return _knobColor.isValid() ? _knobColor : palette().color(QPalette::ButtonText);
    }
    void setKnobColor(const QColor& c) {
        _knobColor = c;
        update();
    }

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void wheelEvent(QWheelEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    QRect thumbRect();
    double valueAt(const QPoint& pos);

    Qt::Orientation _ort;
    double _value;
    double _step;
    int _hndLen;

    QColor _knobColor;
};
