/*
    SuperCollider Qt IDE
    Copyright (c) 2010-2012 Jakob Leben
    Copyright (c) 2012 Tim Blechmann
    http://www.audiosynth.com

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

#pragma once

#include <QProxyStyle>

namespace ScIDE {

class Style : public QProxyStyle {
public:
    Style(QStyle* style = 0): QProxyStyle(style) {}

    void polish(QWidget* widget) override;
    void unpolish(QWidget* widget) override;

    void drawPrimitive(PrimitiveElement, const QStyleOption*, QPainter*, const QWidget* = 0) const override;

    void drawControl(ControlElement, const QStyleOption*, QPainter*, const QWidget* = 0) const override;

    void drawComplexControl(ComplexControl, const QStyleOptionComplex*, QPainter*, const QWidget* = 0) const override;

    QRect subElementRect(SubElement, const QStyleOption*, const QWidget* = 0) const override;

    QSize sizeFromContents(ContentsType, const QStyleOption*, const QSize&, const QWidget* = 0) const override;

    int pixelMetric(PixelMetric, const QStyleOption* = 0, const QWidget* = 0) const override;

    int styleHint(StyleHint, const QStyleOption* = 0, const QWidget* = 0, QStyleHintReturn* = 0) const override;

private:
    bool shouldNotHandle(const QWidget* widget) const;

    static constexpr float kDeselectedTabBlend = 0.3f;
};

} // namespace ScIDE
