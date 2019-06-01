/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include <QWidget>
#include "QcAbstractStepValue.h"
#include "../style/style.hpp"

class QcKnob : public QWidget, QcAbstractStepValue, QtCollider::Style::Client {
    Q_OBJECT
    Q_PROPERTY(double shiftScale READ shiftScale WRITE setShiftScale);
    Q_PROPERTY(double ctrlScale READ ctrlScale WRITE setCtrlScale);
    Q_PROPERTY(double altScale READ altScale WRITE setAltScale);

    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(int mode READ mode WRITE setMode)
    Q_PROPERTY(double step READ step WRITE setStep)
    Q_PROPERTY(bool centered READ centered WRITE setCentered)
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);

Q_SIGNALS:
    void action();

public:
    QcKnob();
    void setValue(double);
    double value() const { return _value; }
    void setMode(int i) { _mode = i; }
    int mode() const { return _mode; }
    void setStep(float step) { _step = step; }
    float step() const { return _step; }
    void setCentered(bool b) {
        _centered = b;
        update();
    }
    bool centered() const { return _centered; }

    virtual QSize sizeHint() const { return QSize(40, 40); }
    virtual QSize minimumSizeHint() const { return QSize(15, 15); }

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    double value(const QPoint&);

    double _value;
    double _step;
    int _mode;
    QPoint _prevPos;
    bool _centered;
};
