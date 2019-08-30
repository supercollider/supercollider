/*
    SuperCollider Qt IDE
    Copyright (c) 2018 SuperCollider Team
    https://supercollider.github.io/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "color.hpp"

namespace ScIDE { namespace color {

QColor lighten(const QColor& color, int amount) {
    int value = color.value() + amount;

    if (value < 0) {
        value = 0;
    } else if (value > 255) {
        value = 255;
    }

    return QColor::fromHsv(color.hue(), color.saturation(), value);
}

QColor darken(const QColor& color, int amount) { return lighten(color, -amount); }

QColor interpolate(const QColor& color_1, const QColor& color_2, float amount) {
    return QColor(color_1.red() * (1 - amount) + color_2.red() * amount,
                  color_1.green() * (1 - amount) + color_2.green() * amount,
                  color_1.blue() * (1 - amount) + color_2.blue() * amount);
}

QColor setValue(const QColor& color, int value) { return QColor::fromHsv(color.hue(), color.saturation(), value); }

} // namespace color
} // namespace ScIDE
