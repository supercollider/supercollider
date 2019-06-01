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
#include "util/color.hpp"
#include <QBrush>
#include <cmath>

using namespace ScIDE;

void Main::setAppPaletteFromSettings() {
    const QTextCharFormat* format = &mSettings->getThemeVal("text");

    // QPalette::Window = general background color.
    QColor window = format->background().color();

    // QPalette::WindowText = general foreground color.
    QColor window_text = format->foreground().color();

    // QPalette::Button = background color of buttons.
    QColor button = window;

    // QPalette::Light = shadow for disabled text.
    QColor disabled_shadow;

    // QPalette::Mid = background color for the help and log dock bars as well
    // as inactive tabs.
    // QPalette::Dark = background color around the "Auto Scroll" button.
    // We use the same color for Dark and Mid currently.
    QColor mid;

    // QPalette::Highlight = highlight background color, used on menus.
    QColor highlight;

    // QPalette::Disabled / QPalette::Text = the foreground color of disabled
    // text and the selection background color.
    QColor disabled_text;

    // QPalette::Shadow = color of dividers between docklets.
    QColor shadow;

    int value_difference = window.value() - window_text.value();
    if (abs(value_difference) < 32) {
        // If we are on the darker end of the spectrum we lighten the background.
        if (window.value() < 127) {
            window = QColor::fromHsv(window.hue(), window.saturation(), window.value() + 32 - value_difference);
        } else {
            // Otherwise we can darken the foreground color.
            window_text = QColor::fromHsv(window_text.hue(), window_text.saturation(),
                                          window_text.value() - (32 - value_difference));
        }
    }

    int window_value = window.value();
    bool dark_on_light = window_text.value() < window_value;
    if (dark_on_light) {
        mid = color::darken(window, 23);
        highlight = color::darken(window, 50);

        // Disabled text is rendered twice, once in disabled text foreground
        // color and once with a "shadow" color. We base the disabled text
        // colors here off the background color, to make them pop less than
        // regular text.
        disabled_text = color::darken(window, 20);
        disabled_shadow = color::darken(window, 20);
    } else {
        // mid should pretty much always be darker than window -- subjectively,
        // it just looks nicer. However, some people set the background really
        // dark, and mid and window aren't distinguishable. Making mid lighter
        // than window tends to distractingly highlight mid-colored elements
        // (such as tabs), so it's not the best idea.

        // The best results, we decided, were to allow mid and window to be
        // indistinguishable for those people, and use other ways to
        // distinguish the selected tab.
        mid = color::darken(window, 20);
        highlight = color::lighten(window, 30);
        disabled_shadow = color::lighten(window, 40);
        disabled_text = color::lighten(window, 40);
    }

    // Shadow (borders between docklets -- see above) is just a hair darker
    // than mid.
    shadow = color::darken(mid, 15);
    // But sometimes window and mid are already both really dark, so we have to
    // make shadow lighter than window.
    if (abs(shadow.value() - window.value()) < 20) {
        shadow = color::lighten(window, 20);
    }

    QPalette palette = QPalette(QBrush(window_text), // windowText
                                QBrush(button), // button
                                QBrush(disabled_shadow), // light
                                QBrush(mid), // dark
                                QBrush(mid), // mid
                                QBrush(window_text), // text
                                QBrush(window_text), // bright_text
                                QBrush(window), // base
                                QBrush(window) // window
    );

    palette.setBrush(QPalette::Disabled, QPalette::Text, QBrush(disabled_text));
    palette.setBrush(QPalette::Shadow, QBrush(shadow));
    palette.setBrush(QPalette::Highlight, QBrush(highlight));
    palette.setBrush(QPalette::HighlightedText, QBrush(window_text));

    qApp->setPalette(palette);
}
