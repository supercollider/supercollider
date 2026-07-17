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

static QPointF getScrollSteps(const QWheelEvent* e) {
    // use pixelDelta on systems that support it, except x11 (Qt docs say it's unreliable)
    const auto platform = QGuiApplication::platformName();
    const bool isX11 = platform == "xcb";
    // angleDelta: map 15deg/step (conventional)
    // pixelDelta: map 10px/step (found empirically to match angleDelta on wayland)
    QPointF steps =
        (e->pixelDelta().isNull() || isX11) ? QPointF(e->angleDelta()) / 120. : QPointF(e->pixelDelta()) / 10.;

    // If Alt is pressed, Qt swaps scroll axis: undo it because we use alt to change scale
    // only on linux (x11 and wayland) and Windows, not on macos
    if (e->modifiers().testFlag(Qt::AltModifier) && (isX11 || platform == "wayland" || platform == "windows"))
        steps = steps.transposed();

    // Note: on some platforms (x11, wayland) "natural scrolling" is not detectable: inverted() returns always false
    if (e->inverted()) {
        steps.setY(steps.y() * -1);
    } else {
        // Normally horiz scroll produces positive delta values if the wheel is moved to the left (Qt docs)
        steps.setX(steps.x() * -1);
    };

    return steps;
}
