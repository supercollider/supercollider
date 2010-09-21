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

#ifndef _WIDGETS_H
#define _WIDGETS_H

#include "../QcHelper.h"
#include "../QcPen.h"
#include "QcAbstractLayout.h"

#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

///////////////////////// Layouts /////////////////////////////////////////////

class QcDefaultLayout : public QcAbstractLayout {
  public:
    QcDefaultLayout() : initialized( false ) {}
  private:
    void setGeometry ( const QRect & r );
    QRect geom;
    bool initialized;
};

class QcHLayout : public QcAbstractLayout
{
  private:
    void setGeometry ( const QRect & r );
};

class QcVLayout : public QcAbstractLayout
{
  private:
    void setGeometry ( const QRect & r );
};

///////////////////////// Basic Widgets ///////////////////////////////////////

class QcDefaultWidget : public QWidget
{
  public:
    QcDefaultWidget() { setLayout( new QcDefaultLayout() ); }
};

class QcHLayoutWidget : public QWidget
{
  public:
    QcHLayoutWidget() { setLayout( new QcHLayout() ); }
};

class QcVLayoutWidget : public QWidget
{
  public:
    QcVLayoutWidget() { setLayout( new QcVLayout() ); }
};


class QcListWidget : public QListWidget, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList items READ dummyVariantList WRITE setItems );

  public:
    QcListWidget();
  private Q_SLOTS:
    void doAction();
  private:
    void setItems( const VariantList & );
    void keyPressEvent( QKeyEvent * );
};

class QcPopUpMenu : public QComboBox, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList items READ dummyVariantList WRITE setItems );

  public:
    QcPopUpMenu();
  private Q_SLOTS:
    void doAction(int);
  private:
    void setItems( const VariantList & );
    int lastChoice;
};

class QcTextField : public QLineEdit
{
  Q_OBJECT

  public:
    QcTextField() {
      connect( this, SIGNAL(returnPressed()), this, SLOT(doAction()) );
    };

  private Q_SLOTS:
    void doAction();
};

class QcButton : public QPushButton, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList states READ dummyVariantList WRITE setStates );
  Q_PROPERTY( int value READ getValue WRITE setValue );
  public:
    QcButton();
  private Q_SLOTS:
    void doAction();
  private:
    struct State {
      QString text;
      QColor textColor;
      QColor buttonColor;
    };
    void setStates( const VariantList & );
    void setValue( int val ) { setState( val ); }
    int getValue() const { return currentState; }
    void setState( int );
    void cycleStates();
    QList<State> states;
    int currentState;
};

class QcCustomPainted : public QcDefaultWidget, QcHelper
{
  Q_OBJECT
  Q_PROPERTY( QColor background READ dummyColor WRITE setBackground );
  Q_PROPERTY( bool paint READ dummyBool WRITE setPaint );
  public:
    QcCustomPainted() : paint(false), painting(false) {}
    Q_INVOKABLE void rebuildPen();
  private:
    // reimplement event handlers just so events don't propagate
    void mousePressEvent( QMouseEvent * ) {}
    void mouseReleaseEvent( QMouseEvent * ) {}
    void mouseMoveEvent( QMouseEvent * ) {}
    void setBackground ( const QColor &color );
    void setPaint( bool b ) { paint = b; }
    void paintEvent( QPaintEvent * );
    bool paint;
    bool painting;
    QColor bkg;
    QtCollider::Pen pen;
};

#endif // _WIDGETS_H
