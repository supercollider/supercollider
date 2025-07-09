/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#pragma once

#include <QWheelEvent>
#include <QGuiApplication>

class QcAbstractStepValue {
protected:
    QcAbstractStepValue(): _shiftScale(100.f), _ctrlScale(10.f), _altScale(0.1f) {}
    bool modifyStep(double* step);
    void setShiftScale(double scale) { _shiftScale = scale; }
    void setCtrlScale(double scale) { _ctrlScale = scale; }
    void setAltScale(double scale) { _altScale = scale; }
    double shiftScale() { return _shiftScale; }
    double ctrlScale() { return _ctrlScale; }
    double altScale() { return _altScale; }

private:
    double _shiftScale;
    double _ctrlScale;
    double _altScale;
};

static QPointF getNormalizedScrollRatio(const QWheelEvent* e, QSizeF pixelRatio) {
    const auto platform = QGuiApplication::platformName();
    const bool isX11 = platform == "xcb";
    // angleDelta: relative to 360deg; pixelDelta: relative to widget size
    QPointF ratio;
    // Force using angleDelta on X11 (advice from Qt docs)
    // the check for pixelRatio.isEmpty is to allow forcing angleDelta by passing pixelRatio = 0
    if (e->pixelDelta().isNull() || pixelRatio.isEmpty() || isX11) {
        ratio.setX(e->angleDelta().x() / 8. / 360.);
        ratio.setY(e->angleDelta().y() / 8. / 360.);
    } else {
        ratio.setX(e->pixelDelta().x() * pixelRatio.width());
        ratio.setY(e->pixelDelta().y() * pixelRatio.height());
    }

    // If Alt is pressed, Qt swaps scroll axis: undo it because we use alt to change scale
    // only on linux (x11 and wayland) and Windows, not on macos
    if (e->modifiers().testFlag(Qt::AltModifier) && (isX11 || platform == "wayland" || platform == "windows"))
        ratio = ratio.transposed();

    // Normally horiz scroll produces positive delta values if the wheel is moved to the left (Qt docs)
    ratio.setX(ratio.x() * -1);

    // Note: on some platforms (x11, wayland) "natural scrolling" is not detectable: inverted() returns always false
    if (e->inverted())
        ratio.setY(ratio.y() * -1);

    return ratio;
}

static QPointF getNormalizedScrollRatio(const QWheelEvent* e, double pixelRatio) {
    return getNormalizedScrollRatio(e, QSizeF(pixelRatio, pixelRatio));
}
