#include "ProxyStyle.hpp"

#include "../QcApplication.h"

#include <QtGlobal>
#include <QWebView>
#include <QStyleOptionSlider>
#include <QPainter>

#ifdef Q_OS_MAC
  #include "../hacks/hacks_mac.hpp"
#endif

using namespace QtCollider;

static bool AlwaysShowScrollbars() {
#if defined(Q_OS_MAC)
  return QtCollider::Mac::AlwaysShowScrollbars();

#elif defined(Q_OS_X11)
  return !QcApplication::SystemHasMouseWheel();

#elif defined(Q_OS_WIN)
  return !QcApplication::SystemHasMouseWheel();

#else
  return !QcApplication::SystemHasMouseWheel();
#endif
};

void ProxyStyle::drawComplexControl ( ComplexControl ctrl, const QStyleOptionComplex *opt,
                                      QPainter *p, const QWidget * w) const
{
  // FIXME: this is a workaround for the WebKit bug #104116 (or a variation on it).
  if( ctrl == QStyle::CC_ScrollBar
      && qobject_cast<const QWebView*>(w) != 0
      && opt->type == QStyleOption::SO_Slider )
  {
    // WebKit tries to hide scrollbars, but mistakenly hides QWebView - NULL-ify styleObject to prevent.
    const QStyleOptionSlider *optSlider = static_cast<const QStyleOptionSlider*>(opt);
    QStyleOptionSlider opt2( *optSlider );
    opt2.styleObject = NULL;

    QProxyStyle::drawComplexControl( ctrl, &opt2, p, w );
    return;
  }

  if (ctrl == QStyle::CC_ScrollBar && AlwaysShowScrollbars()) {
    const QStyleOptionSlider *optSlider = static_cast<const QStyleOptionSlider*>(opt);
    QStyleOptionSlider opt2( *optSlider );
    opt2.state = State_On;
    QProxyStyle::drawComplexControl( ctrl, static_cast<const QStyleOptionComplex*>(&opt2), p, w );
    return;
  }

  QProxyStyle::drawComplexControl( ctrl, opt, p, w );
}

int ProxyStyle::styleHint ( StyleHint hint, const QStyleOption * option,
                            const QWidget * widget, QStyleHintReturn * returnData ) const
{
  switch( hint ) {
    case QStyle::SH_Slider_AbsoluteSetButtons:
      return Qt::LeftButton;
    case QStyle::SH_Slider_PageSetButtons:
      return Qt::NoButton;
    case QStyle::SH_ScrollBar_Transient:
      return 1;
    default:
      return QProxyStyle::styleHint( hint, option, widget, returnData );
  }
}
