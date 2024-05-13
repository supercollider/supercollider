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
#include "../style/style.hpp"
#include "../Common.h"

#include <QWidget>

class QcMultiSlider : public QWidget, QcHelper, QtCollider::Style::Client {
    // TODO setting selection with mouse
    Q_OBJECT
    Q_PROPERTY(int sliderCount READ sliderCount WRITE setSliderCount);
    Q_PROPERTY(QVector<double> values READ values WRITE setValues);
    Q_PROPERTY(QVector<double> reference READ reference WRITE setReference);
    Q_PROPERTY(double value READ value WRITE setValue);
    Q_PROPERTY(double step READ step WRITE setStep);
    Q_PROPERTY(int index READ index WRITE setIndex);
    Q_PROPERTY(int selectionSize READ selectionSize WRITE setSelectionSize);
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation);
    Q_PROPERTY(bool elastic READ dummyBool WRITE setElastic);
    Q_PROPERTY(int indexThumbSize READ dummyFloat WRITE setIndexThumbSize);
    Q_PROPERTY(float valueThumbSize READ dummyFloat WRITE setValueThumbSize);
    Q_PROPERTY(int gap READ dummyInt WRITE setGap);
    Q_PROPERTY(bool drawLines READ dummyBool WRITE setDrawLines);
    Q_PROPERTY(bool drawRects READ dummyBool WRITE setDrawRects);
    Q_PROPERTY(bool isFilled READ dummyBool WRITE setIsFilled);
    Q_PROPERTY(bool highlight READ dummyBool WRITE setHighlight);
    Q_PROPERTY(bool editable READ dummyBool WRITE setEditable);
    Q_PROPERTY(int startIndex READ dummyInt WRITE setStartIndex);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor);
    Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor);
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setFocusColor);

Q_SIGNALS:
    void modified();
    void interacted();
    void action();
    void metaAction();

public Q_SLOTS:
    void doAction();

public:
    QcMultiSlider();

    QVector<double> values() const;
    void setValues(const QVector<double>&);
    double value() const;
    void setValue(double);

    QVector<double> reference() const;
    void setReference(const QVector<double>&);

    double step() const { return roundStep; }
    void setStep(double);

    QSize sizeHint() const { return QSize(500, 300); }
    QSize minimumSizeHint() const { return QSize(50, 50); }

    const QColor& background() const { return _bkgColor.isValid() ? _bkgColor : palette().color(QPalette::Base); }
    void setBackground(const QColor& c) {
        _bkgColor = c;
        update();
    }

    const QColor& fillColor() const { return _fillColor.isValid() ? _fillColor : palette().color(QPalette::Text); }
    void setFillColor(const QColor& c) {
        _fillColor = c;
        update();
    }

    const QColor& strokeColor() const {
        return _strokeColor.isValid() ? _strokeColor : palette().color(QPalette::Text);
    }
    void setStrokeColor(const QColor& c) {
        _strokeColor = c;
        update();
    }

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    Qt::Orientation orientation() const { return ort; }
    int index() const { return _currentIndex; }
    int selectionSize() const { return _selectionSize; }
    int sliderCount() const { return _values.size(); }
    void setSliderCount(int size);
    void setIndex(int i);
    void setSelectionSize(int i);
    void setOrientation(Qt::Orientation o) {
        ort = o;
        update();
    }
    void setElastic(bool b) {
        elastic = b;
        update();
    }
    void setIndexThumbSize(float f) {
        thumbSize.setWidth(f);
        update();
    }
    void setValueThumbSize(float f) {
        thumbSize.setHeight(f);
        update();
    }
    void setGap(int i) {
        gap = i;
        update();
    }
    void setDrawLines(bool b) {
        drawLines = b;
        update();
    }
    void setDrawRects(bool b) {
        drawRects = b;
        update();
    }
    void setIsFilled(bool b) {
        isFilled = b;
        update();
    }
    void setHighlight(bool b) {
        highlight = b;
        update();
    }
    void setEditable(bool b) { editable = b; }
    void setStartIndex(int i) {
        startIndex = qBound(0, i, _values.count() - 1);
        update();
    }

    QRect contentsRect();
    QRect valueRect(int count, qreal& spacing);
    inline float valueFromPos(float pos, float range);
    inline void setValue(int index, double value);
    double rounded(double value);

    // values
    QList<double> _values;
    QList<double> _ref;
    int _currentIndex;
    int _selectionSize;

    // functional properties
    double roundStep;
    bool editable;

    // visual properties
    Qt::Orientation ort;
    bool elastic;
    QSizeF thumbSize;
    int gap;
    bool drawRects;
    bool drawLines;
    bool isFilled;
    bool highlight;
    int startIndex;

    QColor _bkgColor;
    QColor _fillColor;
    QColor _strokeColor;
    QColor _focusColor;

    // temporary
    QPoint moveOrigin;
};
