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

#include <QLineEdit>

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"

#include <QDoubleValidator>

class QcNumberBox : public QLineEdit, QcHelper, QcAbstractStepValue {
    Q_OBJECT
    Q_PROPERTY(double shiftScale READ dummyFloat WRITE setShiftScale);
    Q_PROPERTY(double ctrlScale READ dummyFloat WRITE setCtrlScale);
    Q_PROPERTY(double altScale READ dummyFloat WRITE setAltScale);

    Q_PROPERTY(double minimum READ minimum WRITE setMinimum);
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum);
    Q_PROPERTY(int decimals READ decimals WRITE setDecimals);
    Q_PROPERTY(int maxDecimals READ maxDecimals WRITE setMaxDecimals);
    Q_PROPERTY(int minDecimals READ minDecimals WRITE setMinDecimals);
    Q_PROPERTY(double value READ value WRITE setValue);
    Q_PROPERTY(int valueType READ valueType);
    Q_PROPERTY(QString text READ text WRITE setTextValue);

    Q_PROPERTY(double step READ dummyFloat WRITE setStep)
    Q_PROPERTY(double scrollStep READ dummyFloat WRITE setScrollStep)
    Q_PROPERTY(bool scroll READ dummyBool WRITE setScroll);

    Q_PROPERTY(QColor normalColor READ dummyColor WRITE setTextColor);
    Q_PROPERTY(QColor editingColor READ dummyColor WRITE setEditedTextColor);

public:
    enum ValueType { Number, Infinite, InfiniteNegative, NaN, Text };

    QcNumberBox();
    void setStep(double step_) { step = step_; }
    void setScrollStep(double step_) { scrollStep = step_; }
    void setScroll(bool b) { scroll = b; }
    void setLocked(bool);
    void setTextColor(const QColor& c);
    void setEditedTextColor(const QColor& c);
    void setValue(double);
    Q_INVOKABLE void setInfinite(bool positive = true);
    Q_INVOKABLE void setNaN();
    void setTextValue(const QString&);
    double value() const;
    int valueType() const { return (int)_valueType; }
    double minimum() const { return _validator->bottom(); }
    double maximum() const { return _validator->top(); }
    void setMinimum(double);
    void setMaximum(double);
    int decimals() const { return maxDecimals(); }
    int minDecimals() const { return _minDec; }
    int maxDecimals() const { return _maxDec; }
    void setDecimals(int);
    void setMinDecimals(int);
    void setMaxDecimals(int);

public Q_SLOTS:
    void increment(double factor);
    void decrement(double factor);

Q_SIGNALS:
    void scrolled(int steps);
    void valueChanged();
    void action();

private Q_SLOTS:
    void onEditingFinished();
    void updateText();

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    void stepBy(int steps, double stepSize);
    double roundedVal(double val);
    QString stringForVal(double val);
    void updateTextColor();
    inline void doAction();

    bool scroll;
    int lastPos;
    QColor editedTextColor;
    QColor normalTextColor;

    QDoubleValidator* _validator;
    double step;
    double scrollStep;
    float dragDist;
    double _value;
    ValueType _valueType;
    int _minDec;
    int _maxDec;
};

#if 0
class NumberBoxWidget : public QDoubleSpinBox
{
  friend class SCNumberBox;

  Q_OBJECT

  public:
    NumberBoxWidget( );
    void setScroll( bool b );
  private Q_SLOTS:
    void stepBy( int steps );
    void scrollBy( int steps );
    void onEditingFinished();
  protected:
    virtual void keyPressEvent ( QKeyEvent * event );
  private:
    void stepBy( int steps, float stepSize );
    SCAbstractStepValue *modifier;
    float scrollStep;
};
#endif
