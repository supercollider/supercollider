#ifndef QC_PROXY_STYLE_H
#define QC_PROXY_STYLE_H

#include <QProxyStyle>

namespace QtCollider {

class ProxyStyle : public QProxyStyle
{
public:
  ProxyStyle ( QStyle *style = 0 ) : QProxyStyle(style) { }

  void drawComplexControl ( ComplexControl, const QStyleOptionComplex *,
                            QPainter *, const QWidget * w = 0 ) const;
};

} // namespace QtCollider

#endif // QC_PROXY_STYLE_H
