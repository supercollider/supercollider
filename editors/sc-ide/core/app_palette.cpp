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

#include "main.hpp"
#include <QBrush>

using namespace ScIDE;

void Main::setAppPaletteFromSettings() {
    const QTextCharFormat *format = &mSettings->getThemeVal("text");
    QBrush text_bg = format->background();
    QBrush text_fg = format->foreground();

    int value_difference = text_bg.color().value() - text_fg.color().value();
    if (std::abs(value_difference) < 32) {
        // If we are on the darker end of the spectrum we lighten the background.
        if (text_bg.color().value() < 127) {
            text_bg = QColor::fromHsv(text_bg.color().hue(),
                                      text_bg.color().saturation(),
                                      text_bg.color().value() + 32 - value_difference);
        } else {
            // Otherwise we can darken the foreground color.
            text_fg = QColor::fromHsv(text_fg.color().hue(),
                                      text_fg.color().saturation(),
                                      text_fg.color().value() - (32 - value_difference));
        }
    }

    QBrush active_bg, inactive_bg, highlight, disabled_fg, disabled_shadow;
    // If we are using a dark text on light background we use the light background
    // color for active tabs, and darken it somewhat for inactive tabs.
    if (text_fg.color().value() < text_bg.color().value()) {
        active_bg = text_bg;
        inactive_bg = text_bg.color().darker(125);
        highlight = text_bg.color().darker(150);
        // Disabled text is rendered twice, once in disabled text foreground
        // color and once with a "shadow" color. We base the disabled text
        // colors here off the background color, to make them pop less than
        // regular text.
        disabled_fg = text_bg.color().darker(200);
        disabled_shadow = text_bg.color().darker(250);
    } else {
        // When using light text on a dark background the active tab pops more
        // as a lighter version of the background color.
        inactive_bg = text_bg;

        // QtColor::lighter() multiplies the value of the color by the provided
        // percentage factor, so if the value is zero it has no effect. In that
        // case we darken the foreground color, since hue information is lost
        // in maximum black.
        if (text_bg.color().value() > 0) {
            active_bg = text_bg.color().lighter(150);
            highlight = text_bg.color().lighter(175);
            disabled_shadow = inactive_bg.color().lighter(200);
            disabled_fg = inactive_bg.color().lighter(225);
        } else {
            active_bg = text_fg.color().darker(300);
            highlight = text_fg.color().darker(350);
            disabled_fg = text_fg.color().darker(400);
            disabled_shadow = text_fg.color().darker(450);
        }
    }

    QPalette palette(
        text_fg,          // windowText - text color for active and inactive tab
                          //    bars, and most non-bold text in controls, including
                          //    the selector buttons at the top of the editor
                          //    settings tab.
        active_bg,        // button - background color of *active* tab and buttons.
        disabled_shadow,  // light - shadow for disabled text.
        inactive_bg,      // dark - color for dividers around tabs and the background
                          //     color around the Auto Scroll button.
        inactive_bg,      // mid - background color for the help and log dock bars
                          //     as well as *inactive* tabs.
        text_fg,          // text - text color for home and autoscroll dock bars,
                          //     tab selector names in settings, and most buttons.
        text_fg,          // bright_text - no observed use in current UI.
        text_bg,          // base - no observed use in current UI.
        text_bg           // window - background color of menu bars, dialogs.
    );

    // Set a few other colors, namely the foreground color of disabled text
    // and the selection background color.
    palette.setBrush(QPalette::Disabled, QPalette::Text, disabled_fg);
    palette.setBrush(QPalette::Normal, QPalette::Highlight, highlight);
    palette.setBrush(QPalette::Normal, QPalette::HighlightedText, text_fg);

    qApp->setPalette(palette);
}
