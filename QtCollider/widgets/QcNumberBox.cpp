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
#include "../QcWidgetFactory.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>

#include <math.h>
#include <qmath.h>

static QcWidgetFactory<QcNumberBox> factory;

QcNumberBox::QcNumberBox()
: scroll( true ),
  lastPos( 0 ),
  editedTextColor( QColor( "red" ) ),
  normalTextColor( palette().color(QPalette::Text) ),
  _validator( new QDoubleValidator( this ) ),
  step( 0.1f ),
  scrollStep( 0.1f ),
  dragDist( 10.f ),
  _value( 0. ),
  _minDec(0),
  _maxDec(2)
{
  _validator->setDecimals( _maxDec );
  setValidator( _validator );
  setLocked( true );
  connect( this, SIGNAL( editingFinished() ),
           this, SLOT( onEditingFinished() ) );
  connect( this, SIGNAL( valueChanged() ),
           this, SLOT( onValueChanged() ), Qt::QueuedConnection );
  setValue( 0 );
}

void QcNumberBox::setLocked( bool locked )
{
  if( locked ) {
    setReadOnly( true );
    setSelection( 0, 0 );
  }
  else {
    setReadOnly( false );
  }
  updateTextColor();
}

void QcNumberBox::setTextColor( const QColor& c ) {
  if( c.isValid() ) {
    normalTextColor = c;
    updateTextColor();
  }
}

void QcNumberBox::setEditedTextColor( const QColor& c ) {
  if( c.isValid() ) {
    editedTextColor = c;
    updateTextColor();
  }
}

void QcNumberBox::setValue( double val )
{
  if( val > _validator->top() ) val = _validator->top();
  else if ( val < _validator->bottom() ) val = _validator->bottom();

  val = roundedVal( val );

  _value = val;

  Q_EMIT( valueChanged() );
}

double QcNumberBox::value () const
{
  return _value;
}

void QcNumberBox::setDecimals( int d )
{
  if( d < 0 ) return;
  _minDec = _maxDec = d;
  _validator->setDecimals( d );
  setValue( _value );
}

void QcNumberBox::setMinDecimals( int d )
{
  if( d < 0 ) return;
  _minDec = d;
  if( _minDec > _maxDec ) {
    _maxDec = d;
    _validator->setDecimals( d );
  }
  setValue( _value );
}

void QcNumberBox::setMaxDecimals( int d )
{
  if( d < 0 ) return;
  _maxDec = d;
  if( _maxDec < _minDec ) _minDec = d;
  _validator->setDecimals( d );
  setValue( _value );
}

void QcNumberBox::onEditingFinished()
{
  if( isReadOnly() ) return;
  setValue( locale().toDouble( text() ) );
  setLocked( true );
  Q_EMIT( action() );
}

void QcNumberBox::onValueChanged()
{
  QString str = stringForVal( _value );

  blockSignals(true);
  setText( str );
  blockSignals(false);
}

void QcNumberBox::stepBy( int steps, float stepSize )
{
  modifyStep( &stepSize );
  setValue( value() + (steps * stepSize) );
}

double QcNumberBox::roundedVal( double val )
{
  double k = pow( 10, _maxDec );
  return round( val * k ) / k;
}

QString QcNumberBox::stringForVal( double val )
{
  QLocale loc = locale();
  QString str = loc.toString( val, 'f', _maxDec );
  int i = str.indexOf( loc.decimalPoint() );
  if( i > -1 ) {
    QString dec = str.mid(i+1);
    while( dec.size() > _minDec && dec.endsWith('0') ) dec.chop(1);
    if( dec.isEmpty() )
      str = str.left(i);
    else
      str = str.left(i+1) + dec;
  }
  return str;
}

void QcNumberBox::updateTextColor()
{
  QPalette p( palette() );
  p.setColor( QPalette::Text, isReadOnly() ? normalTextColor : editedTextColor );
  setPalette( p );
}

void QcNumberBox::keyPressEvent ( QKeyEvent * event )
{
  int key = event->key();

  if( key == Qt::Key_Up ){
    onEditingFinished();
    stepBy( 1, step );
    Q_EMIT( action() );
    return;
  }
  else if( key == Qt::Key_Down ) {
    onEditingFinished();
    stepBy( -1, step );
    Q_EMIT( action() );
    return;
  }
  else if( isReadOnly() ) {
    QString t = event->text();
    int i = 0;
    if( !t.isEmpty() &&
        ( _validator->validate( t, i ) != QValidator::Invalid ) )
    {
      blockSignals(true);
      clear();
      blockSignals( false );
      setLocked( false );
    }
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
    int steps = (event->globalY() - lastPos) / dragDist;
    if( steps != 0 ) {
      lastPos = lastPos + (steps * dragDist);
      stepBy( -steps, scrollStep );
      Q_EMIT( action() );
    }
  }
  else
    QLineEdit::mouseMoveEvent( event );
}

void QcNumberBox::wheelEvent ( QWheelEvent * event )
{
  if( isReadOnly() && event->orientation() == Qt::Vertical ) {
    stepBy( event->delta() > 0 ? 1 : -1, scrollStep );
  }
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
