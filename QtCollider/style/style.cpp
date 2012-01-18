#include "style.hpp"

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
    case CC_Slider:
      Slider::cc_draw( static_cast<const QStyleOptionSlider*>(option), p, w );
      return;
    default:
      ProxyStyle::drawComplexControl( cc, option, p, w );
  }
}

int StyleImpl::pixelMetric ( QStyle::PixelMetric pm, const QStyleOption *option,
                         const QWidget *w ) const
{
  int iPM = pm;
  switch( (int) pm ) {
    case PM_SliderThickness:
    case PM_SliderControlThickness:
      return Slider::pm_thickness(static_cast<const QStyleOptionSlider*>(option));
    case PM_SliderLength:
      return Slider::pm_length(static_cast<const QStyleOptionSlider*>(option));
    case PM_SliderSpaceAvailable:
      return Slider::pm_spaceAvailable( static_cast<const QStyleOptionSlider*>(option) );
    default:
      return QProxyStyle::pixelMetric( pm, option, w );
  }
}

QRect StyleImpl::subControlRect ( QStyle::ComplexControl cc, const QStyleOptionComplex *option,
                              QStyle::SubControl sc, const QWidget *w ) const
{
  int iCC = cc;
  switch(iCC) {
    case CC_Slider:
    {
      const QStyleOptionSlider *opt = static_cast<const QStyleOptionSlider*>(option);
      switch( sc ) {
        case SC_SliderHandle: return Slider::sc_handle( opt );
        case SC_SliderGroove: return Slider::sc_groove( opt );
        default: ;
      }
      break;
    }
    default:
      break;
  }

  return QProxyStyle::subControlRect ( cc, option, sc, w );
}
