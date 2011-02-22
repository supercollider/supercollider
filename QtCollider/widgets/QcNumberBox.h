/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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

#ifndef QC_NUMBER_BOX
#define QC_NUMBER_BOX

#include <QLineEdit>

#include "QcAbstractStepValue.h"
#include "../QcHelper.h"

#include <QDoubleValidator>

class QcNumberBox : public QLineEdit, QcHelper, QcAbstractStepValue
{
  Q_OBJECT
  Q_PROPERTY( float shiftScale READ dummyFloat WRITE setShiftScale );
  Q_PROPERTY( float ctrlScale READ dummyFloat WRITE setCtrlScale );
  Q_PROPERTY( float altScale READ dummyFloat WRITE setAltScale );

  Q_PROPERTY( int decimals READ decimals WRITE setDecimals );
  Q_PROPERTY( double value READ value WRITE setValue );

  Q_PROPERTY( float step READ dummyFloat WRITE setStep )
  Q_PROPERTY( float scrollStep READ dummyFloat WRITE setScrollStep )
  Q_PROPERTY( bool scroll READ dummyBool WRITE setScroll );

  Q_PROPERTY( QColor normalColor READ dummyColor WRITE setTextColor );
  Q_PROPERTY( QColor editingColor READ dummyColor WRITE setEditedTextColor );

  public:
    QcNumberBox();
    void setStep( float step_ ) { step = step_;}
    void setScrollStep( float step_ ) { scrollStep = step_; }
    void setScroll( bool b ) { scroll = b; }
    void setLocked( bool );
    void setEditedTextColor( const QColor& c ) { editedTextColor = c; }
    void setTextColor( const QColor& c ) { normalTextColor = c; }
    void setValue( double );
    double value() const;
    void setDecimals( int );
    int decimals() const { return _validator->decimals(); }
  Q_SIGNALS:
    void scrolled( int steps );
    void valueChanged();
    void action();
  private Q_SLOTS:
    void onEditingFinished();
    void onValueChanged();
  protected:
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
  private:
    void stepBy( int steps, float stepSize );
    inline void doAction();

    bool scroll;
    int lastPos;
    QColor editedTextColor;
    QColor normalTextColor;

    QDoubleValidator *_validator;
    float step;
    float scrollStep;
    double _value;
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

#endif