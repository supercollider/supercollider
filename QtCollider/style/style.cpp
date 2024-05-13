#include "style.hpp"
#include "routines.hpp"

using namespace QtCollider::Style;


void StyleImpl::polish(QPalette& p) {
    _colors[Focus] = p.color(QPalette::Highlight);
    _colors[Groove] = p.color(QPalette::Window).darker(120);
}
