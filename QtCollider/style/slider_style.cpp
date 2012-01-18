#include "style.hpp"
#include "routines.hpp"
#include "../widgets/QcSlider.h"

#include <QPainter>

namespace QtCollider {
namespace Style {

int Slider::pm_spaceAvailable( const QStyleOptionSlider *opt )
{
  QRect r( sunkenContentsRect(opt->rect) );
  return opt->orientation == Qt::Horizontal ? r.width() : r.height();
}

int Slider::pm_thickness( const QStyleOptionSlider* opt )
{
  QRect r( sunkenContentsRect(opt->rect) );
  return opt->orientation == Qt::Horizontal ? r.height() : r.width();
}

int Slider::pm_length( const QStyleOptionSlider* opt )
{
  QRect r( sunkenContentsRect(opt->rect) );
  return qMin( 20, opt->orientation == Qt::Horizontal ? r.width() : r.height() );
}

QRect Slider::sc_groove ( const QStyleOption *opt )
{
    return sunkenContentsRect(opt->rect);
}

QRect Slider::sc_handle ( const QStyleOptionSlider *opt )
{
  QRect contRect( sunkenContentsRect(opt->rect) );
  int hndLen = pm_length(opt);

  float valRange = opt->maximum - opt->minimum;
  int pixVal = valRange > 0 ? (opt->sliderPosition - opt->minimum) / valRange : 0;

  QRect r;
  if( opt->orientation == Qt::Horizontal ) {
    int pixVal = (opt->sliderPosition - opt->minimum) * (float)(contRect.width()-hndLen) / valRange;
    r.setX( pixVal + contRect.left() );
    r.setY( contRect.top() );
    r.setSize( QSize( hndLen, contRect.height() ) );
  }
  else {
    int pixVal = (opt->sliderPosition - opt->minimum) * (float)(contRect.height()-hndLen) / valRange;
    r.setX( contRect.left() );
    r.setY( contRect.bottom() - pixVal - hndLen + 1 );
    r.setSize( QSize( contRect.width(), hndLen ) );
  }
  return r;
}

void Slider::cc_draw ( const QStyleOptionSlider *opt, QPainter *p, const QWidget *w )
{
  const QPalette &plt = opt->palette;
  static const int hndWidth = 15;
  static const int radius = 3;

  p->save();
  p->setRenderHint( QPainter::Antialiasing, true );

  QRect rGroove = opt->rect;

  // draw groove
  RoundRect shGroove( rGroove, 3 );

  const QcSlider *slider = qobject_cast<const QcSlider*>(w);

  QColor baseColor( slider->grooveColor() );
  QColor focusColor;
  if( opt->state & QStyle::State_HasFocus && slider )
    focusColor = slider->focusColor();

  drawSunken( p, plt, shGroove, baseColor, focusColor );

  // geometry
  QRect rHandle( sc_handle( opt ) );

  // draw handle
  RoundRect shHandle( rHandle, 3 );
  drawRaised( p, plt, shHandle, plt.color(QPalette::Button).lighter(115) );

  p->restore();

    // draw marker
  QPen pen(plt.color(QPalette::ButtonText));
  pen.setWidth(2);
  pen.setCapStyle(Qt::FlatCap);
  p->setPen(pen);
  if(opt->orientation == Qt::Horizontal) {
    qreal center = rHandle.center().x();
    QLine line( center, rHandle.top()+2, center, rHandle.bottom() - 1 );
    p->drawLine(line);
    pen.setColor(plt.color(QPalette::Light));
    pen.setWidth(1);
    p->setPen(pen);
    line.translate(1,0);
    p->drawLine(line);
  } else {
    qreal center = rHandle.center().y();
    QLine line( rHandle.left()+2, center, rHandle.right() - 1, center );
    p->drawLine(line);
    pen.setColor(plt.color(QPalette::Light));
    pen.setWidth(1);
    p->setPen(pen);
    line.translate(0,1);
    p->drawLine(line);

  }
}

} // namespace Style
} // namespace QtCollider
