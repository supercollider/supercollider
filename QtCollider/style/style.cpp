#include "style.hpp"
#include "routines.hpp"

using namespace QtCollider::Style;


void StyleImpl::polish ( QPalette & p )
{
  _colors[Focus] = p.color(QPalette::Highlight);
  _colors[Groove] = p.color(QPalette::Window).darker(120);
}

void StyleImpl::drawControl ( ControlElement elem, const QStyleOption * option,
                          QPainter * p, const QWidget * w ) const
{
  if( elem == CE_PushButton ) {

    p->save();
    p->setRenderHint( QPainter::Antialiasing, true );

    const QStyleOptionButton *opt = static_cast<const QStyleOptionButton*>(option);
    const QPalette &plt = opt->palette;
    bool sunken = opt->state & State_Sunken || opt->state & State_On;
    int radius = 2;

    QColor color( sunken ? plt.color(QPalette::Button).darker(120) : plt.color(QPalette::Button) );
    QColor focusColor( opt->state & State_HasFocus ? _colors[Focus] : QColor() );

    RoundRect frame( opt->rect, radius );
    if( sunken )
      drawSunken( p, plt, frame, color, focusColor);
    else
      drawRaised( p, plt, frame, color, focusColor );

    drawControl( CE_PushButtonLabel, option, p, w );

    p->restore();
  }
  else {
    ProxyStyle::drawControl( elem, option, p, w );
  }
}
