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

#include "QcButton.h"
#include "../QcWidgetFactory.h"

QC_DECLARE_QWIDGET_FACTORY(QcButton);

QcButton::QcButton():
  QtCollider::Style::Client(this),
  currentState(0)
{
  setAttribute(Qt::WA_AcceptTouchEvents);
  connect( this, SIGNAL(clicked()), this, SLOT(doAction()) );
}

void QcButton::setStates( const QVariantList & statesArg )
{
  if( !statesArg.count() ) return;

  states.clear();

  Q_FOREACH( const QVariant & var, statesArg ) {
    QVariantList stateArg = var.toList();
    int count = stateArg.size();
    State state;
    if( count >= 1 )
      state.text = stateArg[0].toString();
    if( count >= 2 )
      state.textColor = stateArg[1].value<QColor>();
    if( count >= 3 )
      state.buttonColor = stateArg[2].value<QColor>();
    states.append( state );
  }
  setState( 0 );
}

void QcButton::setState( int i )
{
  int c = states.count();
  if( !c ) return;

  currentState = qBound( 0, i, c-1 );
  setText( states[currentState].text );
  update();
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

void QcButton::paintEvent ( QPaintEvent * )
{
  using namespace QtCollider::Style;
  using QtCollider::Style::RoundRect;

  QPainter p(this);
  p.setRenderHint( QPainter::Antialiasing, true );

  State state;
  if(states.count()) state = states[currentState];

  const QPalette &plt = palette();
  bool sunken = isDown();
  int radius = 2;

  if(!state.buttonColor.isValid()) state.buttonColor = plt.color(QPalette::Button);
  if(sunken) state.buttonColor = state.buttonColor.darker(120);

  QColor focusClr( hasFocus() ? focusColor() : QColor() );

  QRect r(rect());

  RoundRect frame( r, radius );
  if( sunken )
    drawSunken( &p, plt, frame, state.buttonColor, focusClr);
  else
    drawRaised( &p, plt, frame, state.buttonColor, focusClr );

  p.setPen( state.textColor.isValid() ? state.textColor : plt.color(QPalette::ButtonText) );
  if(sunken) r.translate(1,1);
  p.drawText( r,  Qt::AlignCenter, text() );
}
