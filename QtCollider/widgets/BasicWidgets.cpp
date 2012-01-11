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

#include "BasicWidgets.h"
#include "../QcWidgetFactory.h"
#include "../Common.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#ifdef Q_WS_MAC
# include <QMacStyle>
#endif

QcWidgetFactory<QcDefaultWidget> defaultWidgetFactory;
QcWidgetFactory<QcHLayoutWidget> hLayoutWidgetFactory;
QcWidgetFactory<QcVLayoutWidget> vLayoutWidgetFactory;
QcWidgetFactory<QLabel> labelFactory;
QcWidgetFactory<QcTextField> textFieldFactory;
QcWidgetFactory<QcCheckBox> checkBoxFactory;

//////////////////////////// QcListWidget //////////////////////////////////////

class QcListWidgetFactory : public QcWidgetFactory<QcListWidget>
{
  void initialize( QWidgetProxy *p, QcListWidget *l ) {
    p->setMouseEventWidget( l->viewport() );
  }
};

QcListWidgetFactory listWidgetFactory;

QcListWidget::QcListWidget() : _emitAction(true)
{
  connect( this, SIGNAL( currentItemChanged( QListWidgetItem*, QListWidgetItem* ) ),
           this, SLOT( onCurrentItemChanged() ) );
}

void QcListWidget::setItems( const VariantList & items )
{
  _emitAction = false;
  clear();
  Q_FOREACH( QVariant item, items.data )
    addItem( item.toString() );
  setCurrentRow( 0 );
  _emitAction = true;
}

void QcListWidget::setColors( const VariantList & colors ) const
{
  int cc = colors.data.count();
  int ic = count();
  for( int i=0; i<cc && i < ic; ++i ) {
    QListWidgetItem *it = item(i);
    QColor color( colors.data[i].value<QColor>() );
    if( color.isValid() ) it->setBackground( color );
  }
}

void QcListWidget::setCurrentRowWithoutAction( int row )
{
  bool b = _emitAction;
  _emitAction = false;
  setCurrentRow( row );
  _emitAction = b;
}

void QcListWidget::onCurrentItemChanged()
{
  if( _emitAction ) Q_EMIT( action() );
}

void QcListWidget::keyPressEvent( QKeyEvent *e )
{
  QListWidget::keyPressEvent( e );
  if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
    Q_EMIT( returnPressed() );
}

////////////////////////// QcPopUpMenu /////////////////////////////////////////

QcWidgetFactory<QcPopUpMenu> popUpMenuFactory;

QcPopUpMenu::QcPopUpMenu()
: lastChoice( -1 ), _reactivation(false)
{
  connect( this, SIGNAL(activated(int)), this, SLOT(doAction(int)) );
}

void QcPopUpMenu::setItems( const VariantList & items )
{
  clear();
  Q_FOREACH( QVariant item, items.data )
      addItem( item.toString() );
}

void QcPopUpMenu::doAction( int choice )
{
  if( _reactivation || (choice != lastChoice) ) {
    lastChoice = choice;
    Q_EMIT( action() );
  }
}

/////////////////////////////// QcButton ///////////////////////////////////////

QcWidgetFactory<QcButton> buttonFactory;

QcButton::QcButton()
: currentState(0), defaultPalette( palette() )
{
  connect( this, SIGNAL(clicked()), this, SLOT(doAction()) );
}

#ifdef Q_WS_MAC
bool QcButton::hitButton( const QPoint & pos ) const
{
  // FIXME: This is a workaround for Qt Bug 15936:
  // incorrect QPushButton hit area.

  QMacStyle *macStyle = qobject_cast<QMacStyle *>(style());

  if( !macStyle || isFlat() )
    return QAbstractButton::hitButton(pos);
  else
    return QPushButton::hitButton(pos);
}
#endif

void QcButton::setStates( const VariantList & statesArg )
{
  if( !statesArg.data.count() ) return;

  states.clear();

  Q_FOREACH( QVariant var, statesArg.data ) {
    VariantList stateArg = var.value<VariantList>();
    int count = stateArg.data.size();
    State state;
    if( count >= 1 )
      state.text = stateArg.data[0].toString();
    if( count >= 2 )
      state.textColor = stateArg.data[1].value<QColor>();
    if( count >= 3 )
      state.buttonColor = stateArg.data[2].value<QColor>();
    states.append( state );
  }
  setState( 0 );
}

void QcButton::setState( int i )
{
  int c = states.count();
  if( !c ) return;

  i = qBound( 0, i, c-1 );

  currentState = i;

  State state = states[i];

  setText( state.text );

  QPalette p ( defaultPalette );

  if( state.textColor.isValid() )
    p.setColor( QPalette::ButtonText, state.textColor );
  if( state.buttonColor.isValid() )
    p.setColor( QPalette::Button, state.buttonColor );

  setPalette( p );
}

void QcButton::cycleStates()
{
  if( states.size() < 2 ) return;
  int i = currentState + 1;
  if( i >= states.size() ) i = 0;
  setState( i );
}

void QcButton::doAction()
{
  cycleStates();
  Q_EMIT( action((int)QApplication::keyboardModifiers()) );
}

class QcCustomPaintedFactory : public QcWidgetFactory<QcCustomPainted>
{
protected:
  virtual void initialize( QWidgetProxy *p, QcCustomPainted *w )
  {
    QObject::connect( w, SIGNAL(painting(QPainter*)),
                      p, SLOT(customPaint(QPainter*)) );
  }
};

static QcCustomPaintedFactory customPaintedFactory;
