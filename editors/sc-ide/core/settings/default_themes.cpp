/*
    SuperCollider IDE
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

#include "theme.hpp"
#include <QPalette>
#include <QApplication>

namespace ScIDE { namespace Settings {

/*
    Kary Pro Colors
    Copyright (c) 2018 Pouya Kary
    modified and adaptated for SuperCollider by Nathan Ho

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
void Theme::fillDefault()
{
    QColor background("#f7f7f7");
    QColor current_line("#efefef");
    QColor selection("#d6d6d6");
    QColor foreground("#1a1a1a");
    QColor line_number("#c7c4c2");
    QColor comment("#848484");
    QColor red("#cc3626");
    QColor orange("#c07f00");
    // QColor yellow("#ada526"); // Not used currently.
    QColor green("#3f831e");
    QColor cyan("#3582bc");
    QColor blue("#3951c9");
    QColor purple("#af33a6");

    addToTheme("text",               foreground, background);
    addToTheme("currentLine",        foreground, current_line);
    addToTheme("searchResult",       background, blue);
    addToTheme("matchingBrackets",   foreground, background, true);
    addToTheme("mismatchedBrackets", background, red);
    addToTheme("evaluatedCode",      background, orange);
    addToTheme("whitespace",         background);
    addToTheme("keyword",            red);
    addToTheme("built-in",           purple);
    addToTheme("env-var",            orange);
    addToTheme("class",              cyan);
    addToTheme("number",             purple);
    addToTheme("symbol",             green);
    addToTheme("string",             blue);
    addToTheme("char",               purple);
    addToTheme("comment",            comment);
    addToTheme("primitive",          orange);
    addToTheme("lineNumbers",        line_number);
    addToTheme("selection",          foreground, selection);
    addToTheme("postwindowtext",     foreground);
    addToTheme("postwindowerror",    red);
    addToTheme("postwindowwarning",  orange);
    addToTheme("postwindowsuccess",  green);
    addToTheme("postwindowemphasis", foreground, Qt::transparent, true);
}
/* END Kary Pro Colors */

void Theme::fillClassic()
{
    addToTheme("text", QColor(Qt::black), QColor(Qt::white));

    QPalette appPlt(QApplication::palette());
    QColor bkg = appPlt.color(QPalette::Base);
    int value = bkg.value();
    if (value > 40)
        bkg.setHsv(bkg.hue(), bkg.saturation(), value - 11);
    else
        bkg.setHsv(bkg.hue(), bkg.saturation(), value + 20);
    addToTheme("currentLine", QColor(Qt::black), bkg.toRgb());
    addToTheme("searchResult",
               appPlt.color(QPalette::HighlightedText).darker(200),
               appPlt.color(QPalette::Highlight).darker(200));
    addToTheme("matchingBrackets", QColor("#2bc93d"), Qt::yellow, true);
    addToTheme("mismatchedBrackets", Qt::white, QColor(150,0,0));
    addToTheme("evaluatedCode", Qt::black, QColor("#F8A200"));

    QPalette plt(QApplication::palette());
    QColor base = plt.color(QPalette::Base);
    QColor text = plt.color(QPalette::Text);
    int shade = (base.red() + base.green() + base.blue() < 380) ? 160 : 100;

    QColor whitespace_color((base.red() + text.red()) / 2,
                            (base.green() + text.green()) / 2,
                            (base.blue() + text.blue()) / 2);

    addToTheme("whitespace", whitespace_color);
    addToTheme("keyword", QColor(0,0,230).lighter(shade),
                                    QColor(Qt::transparent), true);
    addToTheme("built-in", QColor(51,51,191).lighter(shade));
    addToTheme("env-var", QColor(140,70,20).lighter(shade));
    addToTheme("class", QColor(0,0,210).lighter(shade));
    addToTheme("number", QColor(152,0,153).lighter(shade));
    addToTheme("symbol", QColor(0,115,0).lighter(shade));
    addToTheme("string", QColor(95,95,95).lighter(shade));
    addToTheme("char", QColor(0,115,0).lighter(shade));
    addToTheme("comment", QColor(191,0,0).lighter(shade));
    addToTheme("primitive", QColor(51,51,191).lighter(shade));
    addToTheme("lineNumbers", plt.color(QPalette::ButtonText),
                                        plt.color(QPalette::Mid));
    addToTheme("selection", plt.color(QPalette::HighlightedText),
                                      plt.color(QPalette::Highlight));
    addToTheme("postwindowtext", plt.color(QPalette::ButtonText));
    addToTheme("postwindowerror", QColor(209, 28, 36));
    addToTheme("postwindowwarning", QColor(165, 119, 6));
    addToTheme("postwindowsuccess", QColor(115, 138, 5));
    addToTheme("postwindowemphasis", Qt::black, Qt::transparent, true);
}

void Theme::fillDark()
{
    addToTheme("text",               QColor("#ffa4e2"), Qt::black);
    addToTheme("currentLine",        QColor("#e4e4e4"), QColor("#393939"));
    addToTheme("searchResult",       QColor("#e4e4e4"), QColor("#194c7f"));
    addToTheme("matchingBrackets",   QColor("#ff5500"), QColor("#001d49"), true);
    addToTheme("mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    addToTheme("evaluatedCode",      QColor("#e4e4e4"), QColor("#636397"));
    addToTheme("whitespace",         QColor("#e4e4e4"));
    addToTheme("keyword",            QColor("#aaaaff"), Qt::transparent, true);
    addToTheme("built-in",           QColor("#ffa4e2"));
    addToTheme("env-var",            QColor("#73e7ad"));
    addToTheme("class",              QColor("#00abff"), Qt::transparent, true);
    addToTheme("number",             QColor("#4aff00"));
    addToTheme("symbol",             QColor("#ddde00"));
    addToTheme("string",             QColor("#d7d7d7"));
    addToTheme("char",               QColor("#ff55ff"));
    addToTheme("comment",            QColor("#d4b982"));
    addToTheme("primitive",          QColor("#aaff7f"));
    addToTheme("lineNumbers",        QColor("#cfcfcf"));
    addToTheme("selection",          QColor("#ff5500"));
    addToTheme("postwindowtext",     QColor("#e4e4e4"));
    addToTheme("postwindowerror",    QColor("#ff1f2a"));
    addToTheme("postwindowwarning",  QColor("#de7100"));
    addToTheme("postwindowsuccess",  QColor("#b0d206"));
    addToTheme("postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
}

/*
The MIT License (MIT)

Copyright (c) 2016 Dracula Theme

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

https://github.com/dracula/dracula-theme/
*/

void Theme::fillDracula()
{
    addToTheme("text",               QColor("#f7fdff"), QColor("#282a36"));
    addToTheme("currentLine",        QColor("#909194"), QColor("#282a36"));
    addToTheme("searchResult",       QColor("#e4e4e4"), QColor("#194c7f"));
    addToTheme("matchingBrackets",   QColor("#ff5500"), QColor("#001d49"), true);
    addToTheme("mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    addToTheme("evaluatedCode",      QColor("#e4e4e4"), QColor("#636397"));
    addToTheme("whitespace",         QColor("#e4e4e4"));
    addToTheme("keyword",            QColor("#ff76c7"), Qt::transparent, true);
    addToTheme("built-in",           QColor("#e2e37a"));
    addToTheme("env-var",            QColor("#ffb965"));
    addToTheme("class",              QColor("#65e5ff"), Qt::transparent, true);
    addToTheme("number",             QColor("#be90fc"));
    addToTheme("symbol",             QColor("#45fc75"));
    addToTheme("string",             QColor("#5df884"));
    addToTheme("char",               QColor("#ff55ff"));
    addToTheme("comment",            QColor("#6071a6"));
    addToTheme("primitive",          QColor("#aaff7f"));
    addToTheme("lineNumbers",        QColor("#909194"));
    addToTheme("selection",          QColor("#e0eeff"));
    addToTheme("postwindowtext",     QColor("#dfe0fc"));
    addToTheme("postwindowerror",    QColor("#ff1f2a"));
    addToTheme("postwindowwarning",  QColor("#de7100"));
    addToTheme("postwindowsuccess",  QColor("#b0d206"));
    addToTheme("postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
}
/* END MIT LICENSED CODE */

/*
The MIT license (MIT)

Copyright (c) 2011 Ethan Schoonover

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

https://github.com/altercation/solarized
*/

void Theme::fillSolarizedLight()
{
    addToTheme("text",               QColor("#657b83"), QColor("#fdf6e3"));
    addToTheme("currentLine",        Qt::transparent, QColor("#eee8d5"));
    addToTheme("searchResult",       QColor("#93a1a1"), QColor("#073642"));
    addToTheme("matchingBrackets",   QColor("#002b36"), QColor("#eee8d5"), true);
    addToTheme("mismatchedBrackets", QColor("#eee8d5"), QColor("#586e75"));
    addToTheme("evaluatedCode",      QColor("#586e75"), QColor("#eee8d5"));
    addToTheme("whitespace",         Qt::transparent);
    addToTheme("keyword",            QColor("#dc322f"), Qt::transparent, true);
    addToTheme("built-in",           QColor("#b58900"));
    addToTheme("env-var",            QColor("#d33682"));
    addToTheme("class",              QColor("#268bd2"));
    addToTheme("number",             QColor("#6c71c4"));
    addToTheme("symbol",             QColor("#b58900"));
    addToTheme("string",             QColor("#93a1a1"));
    addToTheme("char",               QColor("#cb4b16"));
    addToTheme("comment",            QColor("#586e75"), Qt::transparent, false, true);
    addToTheme("primitive",          QColor("#2aa198"));
    addToTheme("lineNumbers",        QColor("#839496"), QColor("#eee8d5"));
    addToTheme("selection",          QColor("#fdf6e3"), QColor("#657b83"));
    addToTheme("postwindowtext",     QColor("#657b83"));
    addToTheme("postwindowerror",    QColor("#dc322f"));
    addToTheme("postwindowwarning",  QColor("#cb4b16"));
    addToTheme("postwindowsuccess",  QColor("#859900"));
    addToTheme("postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

void Theme::fillSolarizedDark()
{
    addToTheme("text",               QColor("#839496"), QColor("#002b36"));
    addToTheme("currentLine",        Qt::transparent, QColor("#073642"));
    addToTheme("searchResult",       QColor("#586e75"), QColor("#eee8d5"));
    addToTheme("matchingBrackets",   QColor("#fdf6e3"), QColor("#073642"), true);
    addToTheme("mismatchedBrackets", QColor("#073642"), QColor("#93a1a1"));
    addToTheme("evaluatedCode",      QColor("#93a1a1"), QColor("#073642"));
    addToTheme("whitespace",         Qt::transparent);
    addToTheme("keyword",            QColor("#dc322f"), Qt::transparent, true);
    addToTheme("built-in",           QColor("#b58900"));
    addToTheme("env-var",            QColor("#d33682"));
    addToTheme("class",              QColor("#268bd2"));
    addToTheme("number",             QColor("#6c71c4"));
    addToTheme("symbol",             QColor("#b58900"));
    addToTheme("string",             QColor("#586e75"));
    addToTheme("char",               QColor("#cb4b16"));
    addToTheme("comment",            QColor("#93a1a1"), Qt::transparent, false, true);
    addToTheme("primitive",          QColor("#2aa198"));
    addToTheme("lineNumbers",        QColor("#657b83"), QColor("#073642"));
    addToTheme("selection",          QColor("#002b36"), QColor("#839496"));
    addToTheme("postwindowtext",     QColor("#839496"));
    addToTheme("postwindowerror",    QColor("#dc322f"));
    addToTheme("postwindowwarning",  QColor("#cb4b16"));
    addToTheme("postwindowsuccess",  QColor("#859900"));
    addToTheme("postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

/* END MIT LICENSED CODE */

} } // namespace ScIDE
