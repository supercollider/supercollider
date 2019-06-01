/************************************************************************
 *
 * Copyright 2010-2013 Jakob Leben (jakob.leben@gmail.com)
 * Copyright 2010 Ivan Leben (ivan.leben@gmail.com) (QPen_ArcTo)
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

#include "primitives.h"
#include "../painting.h"
#include "../image.h"
#include "../type_codec.hpp"
#include "PyrKernel.h"

#include <QPainter>
#include <QVector2D>
#include <QVector3D>
#include <cmath>

static bool announced = false;
static QObject* paintingObject = NULL;
static QPainter* painter = 0;
static QPainterPath path;

namespace QtCollider {

void announcePainting() { announced = true; }
bool paintingAnnounced() { return announced; }
bool isPaintingObject(QObject* obj) { return obj == paintingObject; }

bool beginPainting(QPainter* p, QObject* obj) {
    if (painter) {
        qcErrorMsg(QStringLiteral("Painting already in progress!"));
        return false;
    }

    painter = p;
    paintingObject = obj;

    painter->setRenderHint(QPainter::Antialiasing, true);
    QColor black(0, 0, 0);
    QPen pen(black);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(black);

    path = QPainterPath();

    return true;
}

void endPainting() {
    painter = 0;
    announced = false;
    paintingObject = NULL;
}

QPainter* globalPainter() { return painter; }
}

typedef QVector2D vec2;
typedef QVector3D vec3;
static const double PI = 3.14159265358979323846264338327950288419717;

inline static qreal globalAngle(const vec2& v) {
    // assuming v is normalized
    qreal cosa = v.x();
    qreal sina = -v.y();
    return sina >= 0.0 ? acosf(cosa) : 2.0 * PI - acosf(cosa);
}

inline static qreal vectorAngle(const vec2& v1, const vec2& v2) {
    // assuming v1,v2 are normalized
    return acosf(vec2::dotProduct(v1, v2));
}

inline static qreal signedAngle(const vec2& v1, const vec2& v2) {
    // assuming v1,v2 are normalized
    qreal a = vectorAngle(v1, v2);
    vec3 c = vec3::crossProduct(vec3(v1), vec3(v2));
    return c.z() > 0.0 ? a : -a;
}

inline static qreal radToDeg(qreal rad) { return rad * 180.0 / PI; }

inline static bool isPenValid() {
    if (!painter) {
        qcErrorMsg(QStringLiteral("Usage of QPen is not allowed at this point!"));
        return false;
    }
    return true;
}

#define QC_QPEN_PRIMITIVE(NAME, ARGC, RECEIVER, ARGS, GLOBAL)                                                          \
    struct NAME {};                                                                                                    \
    template <> struct LangPrimitive<NAME> {                                                                           \
        static int implementation(RECEIVER, ARGS, GLOBAL);                                                             \
        static int mediate(VMGlobals* g, int i) {                                                                      \
            if (!isPenValid())                                                                                         \
                return errFailed;                                                                                      \
            PyrSlot* stack = g->sp - i + 1;                                                                            \
            return implementation(stack, i > 1 ? stack + 1 : 0, g);                                                    \
        }                                                                                                              \
        static void define(int base, int index) { definePrimitive(base, index, "_" #NAME, &mediate, ARGC + 1, 0); }    \
    };                                                                                                                 \
    int LangPrimitive<NAME>::implementation(RECEIVER, ARGS, GLOBAL)

namespace QtCollider {

QC_QPEN_PRIMITIVE(QPen_Save, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    painter->save();
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Restore, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    painter->restore();
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Clear, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    path = QPainterPath();
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_FillColor, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QColor color = QtCollider::get(a);
    painter->setBrush(color);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_StrokeColor, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QColor color = QtCollider::get(a);
    QPen pen = painter->pen();
    pen.setColor(color);
    painter->setPen(pen);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Width, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float width;
    if (slotFloatVal(a, &width))
        return errWrongType;
    QPen pen = painter->pen();
    pen.setWidthF(width);
    painter->setPen(pen);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetJoinStyle, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    int style = QtCollider::get(a);
    QPen pen = painter->pen();

    switch (style) {
    case 0:
        pen.setJoinStyle(Qt::MiterJoin);
        break;
    case 1:
        pen.setJoinStyle(Qt::RoundJoin);
        break;
    case 2:
        pen.setJoinStyle(Qt::BevelJoin);
        break;
    default:
        return errFailed;
    }

    painter->setPen(pen);

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetCapStyle, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    int style = QtCollider::get(a);
    QPen pen = painter->pen();

    switch (style) {
    case 0:
        pen.setCapStyle(Qt::FlatCap);
        break;
    case 1:
        pen.setCapStyle(Qt::RoundCap);
        break;
    case 2:
        pen.setCapStyle(Qt::SquareCap);
        break;
    default:
        return errFailed;
    }

    painter->setPen(pen);

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetDashPattern, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!IsObj(a))
        return errWrongType;
    PyrObject* obj = slotRawObject(a);
    if (obj->classptr != class_floatarray)
        return errWrongType;
    PyrFloatArray* farray = reinterpret_cast<PyrFloatArray*>(obj);

    int s = farray->size;
    float* f = farray->f;
    QVector<qreal> pattern;

    int i = 1;
    while (i < s) {
        pattern << *f << *(f + 1);
        f += 2;
        i += 2;
    }

    if (pattern.size()) {
        QPen pen = painter->pen();
        pen.setDashPattern(pattern);
        painter->setPen(pen);
    }

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetOpacity, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float opacity = QtCollider::get(a);
    painter->setOpacity(opacity);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Clip, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    painter->setClipPath(path);
    path = QPainterPath();
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AntiAliasing, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    bool b = IsTrue(a);
    if (!b && !IsFalse(a))
        return errWrongType;
    painter->setRenderHint(QPainter::Antialiasing, b);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetFont, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    painter->setFont(QtCollider::get(a));
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Translate, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float x, y;
    if (slotFloatVal(a, &x))
        return errWrongType;
    if (slotFloatVal(a + 1, &y))
        return errWrongType;
    painter->translate(x, y);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Scale, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float x, y;
    if (slotFloatVal(a, &x))
        return errWrongType;
    if (slotFloatVal(a + 1, &y))
        return errWrongType;
    painter->scale(x, y);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Shear, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float x, y;
    if (slotFloatVal(a, &x))
        return errWrongType;
    if (slotFloatVal(a + 1, &y))
        return errWrongType;
    painter->shear(x, y);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Rotate, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float angle, x, y;
    if (slotFloatVal(a, &angle))
        return errWrongType;
    if (slotFloatVal(a + 1, &x))
        return errWrongType;
    if (slotFloatVal(a + 2, &y))
        return errWrongType;

    painter->translate(x, y);
    painter->rotate(angle);
    painter->translate(-x, -y);

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_SetTransform, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QVariantList list = QtCollider::get(a);
    if (list.count() < 6)
        return errWrongType;
    float f[6];
    int i = 6;
    while (i) {
        --i;
        const QVariant& var = list[i];
        if (!var.canConvert(QVariant::Double))
            return errWrongType;
        f[i] = var.value<float>();
    }
    QTransform transform(f[0], f[1], f[2], f[3], f[4], f[5]);
    painter->setWorldTransform(transform, true);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Transform, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    const QTransform& transform = painter->worldTransform();

    PyrDoubleArray* doubleArray = newPyrDoubleArray(g->gc, 6, 0, true);
    SetObject(r, doubleArray);

    double* val = doubleArray->d;
    val[0] = transform.m11();
    val[1] = transform.m12();
    val[2] = transform.m21();
    val[3] = transform.m22();
    val[4] = transform.dx();
    val[5] = transform.dy();
    doubleArray->size = 6;

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_MoveTo, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF point = QtCollider::get(a);
    path.moveTo(point);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_LineTo, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF point = QtCollider::get(a);
    path.lineTo(point);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_CubicTo, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF endPoint = QtCollider::get(a);
    QPointF cPoint1 = QtCollider::get(a + 1);
    QPointF cPoint2 = QtCollider::get(a + 2);
    path.cubicTo(cPoint1, cPoint2, endPoint);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_QuadTo, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF endPoint = QtCollider::get(a);
    QPointF cPoint = QtCollider::get(a + 1);
    path.quadTo(cPoint, endPoint);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_ArcTo, 3, PyrSlot* r, PyrSlot* arg, VMGlobals* g) {
    QPointF pt1 = QtCollider::get(arg);
    QPointF pt2 = QtCollider::get(arg + 1);
    float radius;
    if (slotFloatVal(arg + 2, &radius))
        return errWrongType;
    radius = qMax(0.f, radius);

    vec2 a(path.currentPosition());
    vec2 b(pt1);
    vec2 c(pt2);

    vec2 va = (a - b).normalized();
    vec2 vc = (c - b).normalized();
    vec2 m = (va + vc).normalized();

    qreal lineAngle = vectorAngle(va, vc);
    qreal dm = radius / sinf(lineAngle * 0.5f);
    qreal dv = radius / tanf(lineAngle * 0.5f);

    vec2 center = b + dm * m;
    vec2 start = b + dv * va;
    vec2 end = b + dv * vc;

    vec2 vs = (start - center).normalized();
    vec2 ve = (end - center).normalized();

    qreal arcAngle = signedAngle(ve, vs);
    qreal arcStart = globalAngle(vs);

    path.lineTo(start.x(), start.y());

    path.arcTo(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius, radToDeg(arcStart),
               radToDeg(arcAngle));

    // The current SC API does not want to pull the line to the last point.
    // Personally, I think it would be better:

    // path.lineTo( pt2 );

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddRect, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QRectF rect = QtCollider::get(a);
    path.addRect(rect);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddRoundedRect, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    float radiusX, radiusY;
    QRectF rect;

    rect = QtCollider::get(a);
    if (slotFloatVal(a + 1, &radiusX))
        return errWrongType;
    if (slotFloatVal(a + 2, &radiusY))
        return errWrongType;

    path.addRoundedRect(rect, radiusX, radiusY);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddEllipse, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QRectF rect = QtCollider::get(a);
    path.addEllipse(rect);
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddArc, 4, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF center = QtCollider::get(a);
    float radius, startAngle, arcAngle;
    if (slotFloatVal(a + 1, &radius))
        return errWrongType;
    if (slotFloatVal(a + 2, &startAngle))
        return errWrongType;
    if (slotFloatVal(a + 3, &arcAngle))
        return errWrongType;

    // have to swap angle direction for sinf()
    QPointF start(radius * cosf(startAngle), -radius * sinf(startAngle));
    start += center;

    QRectF rect;
    rect.setSize(QSizeF(2 * radius, 2 * radius));
    rect.moveCenter(center);

    path.moveTo(start);
    path.arcTo(rect, radToDeg(startAngle), radToDeg(arcAngle));

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddWedge, 4, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF center = QtCollider::get(a);
    float radius, startAngle, sweepLength;
    if (slotFloatVal(a + 1, &radius))
        return errWrongType;
    if (slotFloatVal(a + 2, &startAngle))
        return errWrongType;
    if (slotFloatVal(a + 3, &sweepLength))
        return errWrongType;
    path.moveTo(center);
    QRectF rect;
    rect.setSize(QSizeF(2 * radius, 2 * radius));
    rect.moveCenter(center);
    path.arcTo(rect, startAngle, sweepLength);
    path.closeSubpath();
    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_AddAnnularWedge, 5, PyrSlot*, PyrSlot* a, VMGlobals* g) {
    QPointF c = QtCollider::get(a);
    float innerRadius, outerRadius, startAngle, arcAngle;
    if (slotFloatVal(a + 1, &innerRadius))
        return errWrongType;
    if (slotFloatVal(a + 2, &outerRadius))
        return errWrongType;
    if (slotFloatVal(a + 3, &startAngle))
        return errWrongType;
    if (slotFloatVal(a + 4, &arcAngle))
        return errWrongType;

    float saDeg = radToDeg(startAngle);
    float aaDeg = radToDeg(arcAngle);

    QPointF start(outerRadius * cosf(startAngle), -outerRadius * sinf(startAngle));
    start += c;
    path.moveTo(start);

    QPointF pt(innerRadius, innerRadius);
    path.arcTo(QRectF(c - pt, c + pt), saDeg, aaDeg);

    pt = QPointF(outerRadius, outerRadius);
    path.arcTo(QRectF(c - pt, c + pt), saDeg + aaDeg, -aaDeg);

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_Draw, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (path.isEmpty())
        return errNone;

    int style = QtCollider::get(a);
    QPen pen = painter->pen();
    QBrush brush = painter->brush();

    switch (style) {
    case 0:
    case 1:
        painter->setPen(Qt::NoPen);
        break;
    case 2:
        painter->setBrush(Qt::NoBrush);
        break;
    case 3:
    case 4:
    default:;
    }
    if (style == 1 || style == 4) {
        path.setFillRule(Qt::OddEvenFill);
    } else {
        path.setFillRule(Qt::WindingFill);
    }

    painter->drawPath(path);

    path = QPainterPath();

    painter->setPen(pen);
    painter->setBrush(brush);

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_FillAxialGradient, 4, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF pt1 = QtCollider::get(a + 0);
    QPointF pt2 = QtCollider::get(a + 1);
    QColor c1 = QtCollider::get(a + 2);
    QColor c2 = QtCollider::get(a + 3);

    QLinearGradient grad(pt1, pt2);
    grad.setColorAt(0, c1);
    grad.setColorAt(1, c2);

    QPen pen = painter->pen();
    QBrush brush = painter->brush();

    painter->setPen(Qt::NoPen);
    painter->setBrush(grad);

    painter->drawPath(path);

    painter->setPen(pen);
    painter->setBrush(brush);

    path = QPainterPath();

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_FillRadialGradient, 6, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPointF pt1 = QtCollider::get(a + 0);
    QPointF pt2 = QtCollider::get(a + 1);
    float r1 = QtCollider::get(a + 2);
    float r2 = QtCollider::get(a + 3);
    QColor c1 = QtCollider::get(a + 4);
    QColor c2 = QtCollider::get(a + 5);

    QRadialGradient grad(pt2, r2, pt1);
    grad.setColorAt((r2 > 0 ? r1 / r2 : 0), c1);
    grad.setColorAt(1, c2);

    QPen pen = painter->pen();
    QBrush brush = painter->brush();

    painter->setPen(Qt::NoPen);
    painter->setBrush(grad);

    painter->drawPath(path);

    painter->setPen(pen);
    painter->setBrush(brush);

    path = QPainterPath();

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_StringAtPoint, 4, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QString str = QtCollider::get(a);
    if (str.isEmpty())
        return errNone;
    QPointF pt = QtCollider::get(a + 1);

    painter->save();
    if (NotNil(a + 2))
        painter->setFont(QtCollider::get(a + 2));
    QPen pen(painter->pen());
    pen.setColor(NotNil(a + 3) ? QtCollider::get(a + 3) : painter->brush().color());
    painter->setPen(pen);

    QFont f(painter->font());
    QFontMetrics fm(f);
    QRect rect = fm.boundingRect(str);
    painter->drawText(pt - rect.topLeft(), str);

    painter->restore();

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_StringInRect, 5, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QString str = QtCollider::get(a);
    if (str.isEmpty())
        return errNone;
    QRectF rect = QtCollider::get(a + 1);

    painter->save();
    if (NotNil(a + 2))
        painter->setFont(QtCollider::get(a + 2));
    QPen pen(painter->pen());
    pen.setColor(NotNil(a + 3) ? QtCollider::get(a + 3) : painter->brush().color());
    painter->setPen(pen);

    Qt::Alignment align;
    if (NotNil(a + 4))
        align = static_cast<Qt::Alignment>(QtCollider::get<int>(a + 4));
    else
        align = Qt::AlignTop | Qt::AlignLeft;

    painter->drawText(rect, align, str);

    painter->restore();

    return errNone;
}

QC_QPEN_PRIMITIVE(QPen_DrawImage, 5, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QtCollider::SharedImage image = QtCollider::get(a + 1);
    if (!image)
        return errWrongType;

    if (image->isPainting()) {
        qcErrorMsg("QImage: can not draw while being painted.");
        return errFailed;
    }

    QPixmap& pixmap = image->pixmap();

    QRectF source;

    if (IsNil(a + 2))
        source = QRectF(pixmap.rect());
    else if (isKindOfSlot(a + 2, SC_CLASS(Rect)))
        source = QtCollider::read<QRectF>(a + 2);
    else
        return errWrongType;

    int composition = QtCollider::get(a + 3);
    float opacity = QtCollider::get(a + 4);

    painter->save();
    painter->setCompositionMode((QPainter::CompositionMode)composition);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, image->transformationMode == Qt::SmoothTransformation);
    painter->setOpacity(opacity);

    int result = errNone;

    if (isKindOfSlot(a + 0, SC_CLASS(Point))) {
        QPointF point = QtCollider::read<QPointF>(a + 0);
        painter->drawPixmap(point, pixmap, source);
    } else if (isKindOfSlot(a + 0, SC_CLASS(Rect))) {
        QRectF target = QtCollider::read<QRectF>(a + 0);
        painter->drawPixmap(target, pixmap, source);
    } else {
        result = errWrongType;
    }

    painter->restore();

    return result;
}

QC_QPEN_PRIMITIVE(QPen_TileImage, 5, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QtCollider::SharedImage image = QtCollider::get(a + 1);
    if (!image)
        return errWrongType;

    if (image->isPainting()) {
        qcErrorMsg("QImage: can not draw while being painted.");
        return errFailed;
    }

    QPixmap& pixmap = image->pixmap();

    QRectF source;

    if (IsNil(a + 2))
        source = QRectF(pixmap.rect());
    else if (isKindOfSlot(a + 2, SC_CLASS(Rect)))
        source = QtCollider::read<QRectF>(a + 2);
    else
        return errWrongType;

    QRectF target;

    if (isKindOfSlot(a + 0, SC_CLASS(Rect)))
        target = QtCollider::read<QRectF>(a + 0);
    else
        return errWrongType;

    if (target.isEmpty() || source.isEmpty())
        return errNone;

    int composition = QtCollider::get(a + 3);
    float opacity = QtCollider::get(a + 4);

    painter->save();
    painter->setCompositionMode((QPainter::CompositionMode)composition);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, image->transformationMode == Qt::SmoothTransformation);
    painter->setOpacity(opacity);
    painter->setClipRect(target);

    QRectF area = source;
    area.moveTo(target.topLeft());
    float max_x = target.right();
    float max_y = target.bottom();
    int iter_x, iter_y;
    for (iter_y = 0;; ++iter_y) {
        area.moveTop(iter_y * source.height() + target.top());
        if (area.top() > max_y)
            break;
        for (iter_x = 0;; ++iter_x) {
            area.moveLeft(iter_x * source.width() + target.left());
            if (area.left() > max_x)
                break;
            painter->drawPixmap(area, pixmap, source);
        }
    }

    painter->restore();

    return errNone;
}

void defineQPenPrimitives() {
    LangPrimitiveDefiner definer;
    definer.define<QPen_Save>();
    definer.define<QPen_Restore>();
    definer.define<QPen_Clear>();
    definer.define<QPen_FillColor>();
    definer.define<QPen_StrokeColor>();
    definer.define<QPen_Width>();
    definer.define<QPen_SetJoinStyle>();
    definer.define<QPen_SetCapStyle>();
    definer.define<QPen_SetDashPattern>();
    definer.define<QPen_SetOpacity>();
    definer.define<QPen_Clip>();
    definer.define<QPen_AntiAliasing>();
    definer.define<QPen_SetFont>();
    definer.define<QPen_Translate>();
    definer.define<QPen_Scale>();
    definer.define<QPen_Shear>();
    definer.define<QPen_Rotate>();
    definer.define<QPen_Transform>();
    definer.define<QPen_SetTransform>();
    definer.define<QPen_MoveTo>();
    definer.define<QPen_LineTo>();
    definer.define<QPen_CubicTo>();
    definer.define<QPen_QuadTo>();
    definer.define<QPen_ArcTo>();
    definer.define<QPen_AddRect>();
    definer.define<QPen_AddRoundedRect>();
    definer.define<QPen_AddEllipse>();
    definer.define<QPen_AddArc>();
    definer.define<QPen_AddWedge>();
    definer.define<QPen_AddAnnularWedge>();
    definer.define<QPen_Draw>();
    definer.define<QPen_FillAxialGradient>();
    definer.define<QPen_FillRadialGradient>();
    definer.define<QPen_StringAtPoint>();
    definer.define<QPen_StringInRect>();
    definer.define<QPen_DrawImage>();
    definer.define<QPen_TileImage>();
}

} // namespace QtCollider
