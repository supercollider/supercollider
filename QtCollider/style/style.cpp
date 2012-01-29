#include "style.hpp"
#include "routines.hpp"

#include "../widgets/QcSlider.h"

using namespace QtCollider::Style;


void StyleImpl::polish ( QPalette & p )
{
  ProxyStyle::polish( p );

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

    if( !opt->text.isEmpty() )
      drawItemText( p, opt->rect, Qt::AlignCenter, plt, opt->state & State_Enabled, opt->text, QPalette::ButtonText );

    p->restore();
  }
  else {
    ProxyStyle::drawControl( elem, option, p, w );
  }
}

void StyleImpl::drawComplexControl ( QStyle::ComplexControl cc, const QStyleOptionComplex * option,
                                 QPainter *p, const QWidget *w ) const
{
  switch( (int) cc ) {
    case CC_Slider: {
      const QcSlider *slider = qobject_cast<const QcSlider*>(w);
      if(!slider) break;
      Slider::cc_draw( static_cast<const QStyleOptionSlider*>(option), p, slider );
      return;
    }
    default:
      break;
  }

  ProxyStyle::drawComplexControl( cc, option, p, w );
}

QRect StyleImpl::subControlRect ( QStyle::ComplexControl cc, const QStyleOptionComplex *option,
                              QStyle::SubControl sc, const QWidget *w ) const
{
  int iCC = cc;
  switch(iCC) {
    case CC_Slider:
    {
      const QcSlider *slider = qobject_cast<const QcSlider*>(w);
      if(!slider) break;
      const QStyleOptionSlider *opt = static_cast<const QStyleOptionSlider*>(option);
      switch( sc ) {
        case SC_SliderHandle: return Slider::sc_handle( opt, slider );
        case SC_SliderGroove: return Slider::sc_groove( opt, slider );
        default: ;
      }
      break;
    }
    default:
      break;
  }

  return QProxyStyle::subControlRect ( cc, option, sc, w );
}
