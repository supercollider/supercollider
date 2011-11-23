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
#include "../layouts/classic_layouts.hpp"
#include "QcCanvas.h"

#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

class QcDefaultWidget : public QWidget
{
  Q_OBJECT
  public:
    QcDefaultWidget() { setLayout( new QcDefaultLayout() ); }
    Q_INVOKABLE void addChild( QWidget* w ) { layout()->addWidget(w); }
};

class QcHLayoutWidget : public QWidget
{
  Q_OBJECT
  public:
    QcHLayoutWidget() { setLayout( new QcHLayout() ); }
    Q_INVOKABLE void addChild( QWidget* w ) { layout()->addWidget(w); }
};

class QcVLayoutWidget : public QWidget
{
  Q_OBJECT
  public:
    QcVLayoutWidget() { setLayout( new QcVLayout() ); }
    Q_INVOKABLE void addChild( QWidget* w ) { layout()->addWidget(w); }
};


class QcListWidget : public QListWidget, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList items READ dummyVariantList WRITE setItems );
  Q_PROPERTY( VariantList colors READ dummyVariantList WRITE setColors );
  Q_PROPERTY( int currentRow READ currentRow WRITE setCurrentRowWithoutAction )

  public:
    QcListWidget();
    void setItems( const VariantList & );
    void setColors( const VariantList & ) const;
    void setCurrentRowWithoutAction( int );
  Q_SIGNALS:
    void action();
    void returnPressed();
  private Q_SLOTS:
    void onCurrentItemChanged();
  private:
    void keyPressEvent( QKeyEvent * );

    bool _emitAction;
};

class QcPopUpMenu : public QComboBox, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList items READ dummyVariantList WRITE setItems );
  Q_PROPERTY( bool signalReactivation READ signalReactivation WRITE setSignalReactivation )

  public:
    QcPopUpMenu();
    bool signalReactivation() const { return _reactivation; }
    void setSignalReactivation( bool b ) { _reactivation = b; }
  Q_SIGNALS:
    void action();
  private Q_SLOTS:
    void doAction(int);
  private:
    void setItems( const VariantList & );
    int lastChoice;
    bool _reactivation;
};

class QcTextField : public QLineEdit
{
  Q_OBJECT

  public:
    QcTextField() {
      connect( this, SIGNAL(returnPressed()), this, SIGNAL(action()) );
    };
  Q_SIGNALS:
    void action();
};

class QcButton : public QPushButton, public QcHelper
{
  Q_OBJECT
  Q_PROPERTY( VariantList states READ dummyVariantList WRITE setStates );
  Q_PROPERTY( int value READ getValue WRITE setValue );
  public:
    QcButton();
  Q_SIGNALS:
    void action(int);
  protected:
#ifdef Q_WS_MAC
    bool hitButton( const QPoint & ) const;
#endif
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
    QPalette defaultPalette;
};

class QcCustomPainted : public QcCanvas, QcHelper
{
  Q_OBJECT
  public:
    QcCustomPainted() {
      setLayout( new QcDefaultLayout() );
    }
    Q_INVOKABLE void addChild( QWidget* w ) { layout()->addWidget(w); }
  private:
    // reimplement event handlers just so events don't propagate
    void mousePressEvent( QMouseEvent * ) {}
    void mouseReleaseEvent( QMouseEvent * ) {}
    void mouseMoveEvent( QMouseEvent * ) {}
};

class QcCheckBox : public QCheckBox
{
  Q_OBJECT
  Q_PROPERTY( bool value READ value WRITE setValue );

  public:
    QcCheckBox() {
      connect( this, SIGNAL(clicked()), this, SIGNAL(action()) );
    }
  Q_SIGNALS:
    void action();
  private:
    bool value() { return isChecked(); }
    void setValue( bool val ) { setChecked(val); }
};

#endif // _WIDGETS_H
