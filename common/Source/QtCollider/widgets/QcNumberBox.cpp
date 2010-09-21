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

#include "QcNumberBox.h"
#include "../Common.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>

QcNumberBox::QcNumberBox()
: scroll( true ),
  lastPos( 0 ),
  editedTextColor( QColor( "red" ) ),
  normalTextColor( QColor( "black" ) ),
  _validator( new QDoubleValidator( this ) ),
  step( 0.1f ),
  scrollStep( 0.1f ),
  _value( 0. )
{
  _validator->setDecimals( 2 );
  setValidator( _validator );
  setLocked( true );
  setValue( 0 );
  connect( this, SIGNAL( editingFinished() ),
           this, SLOT( onEditingFinished() ) );
}

void QcNumberBox::setLocked( bool locked )
{
  if( locked ) {
    setReadOnly( true );
    setSelection( 0, 0 );
    if( normalTextColor.isValid() ) {
      QPalette p( palette() );
      p.setColor( QPalette::Text, normalTextColor );
      setPalette( p );
    }
  }
  else {
    setReadOnly( false );
    if( editedTextColor.isValid() ) {
      QPalette p( palette() );
      p.setColor( QPalette::Text, editedTextColor );
      setPalette( p );
    }
  }
}

void QcNumberBox::setValue( double val )
{
  if( val > _validator->top() ) val = _validator->top();
  else if ( val < _validator->bottom() ) val = _validator->bottom();

  blockSignals(true);

  setText( QString::number( val, 'f', _validator->decimals() ) );

  blockSignals(false);

  _value = val;
}

double QcNumberBox::value () const
{
  return _value;
}

void QcNumberBox::setDecimals( int d )
{
  _validator->setDecimals( d );
  setValue( _value );
}

void QcNumberBox::onEditingFinished()
{
  setValue( text().toDouble() ); setLocked( true );
  doAction();
}

void QcNumberBox::stepBy( int steps, float stepSize )
{
  modifyStep( &stepSize );
  setValue( value() + (steps * stepSize) );
}

void QcNumberBox::keyPressEvent ( QKeyEvent * event )
{
  int key = event->key();
  if( isReadOnly() &&
      ( (key >= Qt::Key_0 && key <= Qt::Key_9)
        || key == Qt::Key_Comma || key == Qt::Key_Period
      )
    )
  {
    blockSignals(true);
    clear();
    blockSignals( false );
    setLocked( false );
  }
  else if( key == Qt::Key_Up ){
    _value = text().toDouble();
    stepBy( 1, step );
    doAction();
  }
  else if( key == Qt::Key_Down ) {
    _value = text().toDouble();
    stepBy( -1, step );
    doAction();
  }

  QLineEdit::keyPressEvent( event );
}

void QcNumberBox::mouseDoubleClickEvent ( QMouseEvent * event )
{
  Q_UNUSED( event );
  setLocked( false );
}

void QcNumberBox::mousePressEvent ( QMouseEvent * event )
{
  lastPos = event->globalY();
  QLineEdit::mousePressEvent( event );
}

void QcNumberBox::mouseMoveEvent ( QMouseEvent * event )
{
  if( scroll
      && ( event->buttons() & Qt::LeftButton )
      && isReadOnly() )
  {
    int dif = event->globalY() - lastPos;
    if( dif != 0 ) {
      lastPos = event->globalY();
      //Q_EMIT( scrolled( dif * -1 ) );
      stepBy( dif * -1, scrollStep );
      doAction();
    }
  }
  else
    QLineEdit::mouseMoveEvent( event );
}

inline void QcNumberBox::doAction()
{
  QApplication::postEvent( this, new ScMethodCallEvent( "doAction" ) );
}

#if 0
NumberBoxWidget::NumberBoxWidget()
: modifier( 0 ), scrollStep( 1 )
{
  ScrollLineEdit *scrollLineEdit = new ScrollLineEdit();
  scrollLineEdit->setScroll( true );
  setLineEdit( scrollLineEdit );

  connect( scrollLineEdit, SIGNAL( scrolled(int) ),
           this, SLOT( scrollBy(int) ) );
  connect( this, SIGNAL( editingFinished() ),
           this, SLOT( onEditingFinished() ) );
}

void NumberBoxWidget::setScroll( bool b )
{
  ScrollLineEdit *edit = qobject_cast<ScrollLineEdit*>( lineEdit() );
  edit->setScroll( b );
}

void NumberBoxWidget::stepBy( int steps )
{
  stepBy( steps, singleStep() );
}

void NumberBoxWidget::scrollBy( int steps )
{
  stepBy( steps, scrollStep );
}

void NumberBoxWidget::onEditingFinished()
{
  ScrollLineEdit *edit = qobject_cast<ScrollLineEdit*>( lineEdit() );
  edit->setLocked( true );
}

void NumberBoxWidget::stepBy( int steps, float stepSize )
{
  modifier->modifyStep( &stepSize );

  double val = qMin( maximum(), value() + (steps * stepSize) );
  val = qMax( minimum(), val );
  setValue( val );
}

void NumberBoxWidget::keyPressEvent ( QKeyEvent * event )
{
  if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return )
  {
    interpretText();
    Q_EMIT( editingFinished() );
  }
  else
    QDoubleSpinBox::keyPressEvent( event );
}
#endif
