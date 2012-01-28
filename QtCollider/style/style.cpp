#include "style.hpp"

#include "../widgets/QcSlider.h"

using namespace QtCollider::Style;


void StyleImpl::polish ( QPalette & p )
{
  ProxyStyle::polish( p );

  _colors[Focus] = p.color(QPalette::Highlight);
  _colors[Groove] = p.color(QPalette::Window).darker(125);
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
