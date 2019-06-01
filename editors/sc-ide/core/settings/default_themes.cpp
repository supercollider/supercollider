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
void Theme::fillDefault() {
    QColor background("#f7f7f7");
    QColor current_line("#eaeaea");
    QColor selection("#d6d6d6");
    QColor foreground("#1a1a1a");
    QColor line_number("#c7c4c2");
    QColor comment("#848484");
    QColor red("#cc3626");
    QColor orange("#c07f00");
    // QColor yellow("#ada526"); // Not used currently.
    QColor green("#3f831e");
    QColor cyan("#3478bc");
    QColor blue("#3f39c9");
    QColor purple("#af33a6");

    add("text", foreground, background);
    add("currentLine", foreground, current_line);
    add("searchResult", background, blue);
    add("matchingBrackets", foreground, current_line, true);
    add("mismatchedBrackets", background, red);
    add("evaluatedCode", background, orange);
    add("whitespace", background);
    add("keyword", red);
    add("built-in", purple);
    add("env-var", orange);
    add("class", cyan);
    add("number", purple);
    add("symbol", green);
    add("string", blue);
    add("char", purple);
    add("comment", comment);
    add("primitive", orange);
    add("lineNumbers", line_number);
    add("selection", foreground, selection);
    add("postwindowtext", foreground);
    add("postwindowerror", red);
    add("postwindowwarning", orange);
    add("postwindowsuccess", green);
    add("postwindowemphasis", foreground, Qt::transparent, true);
}
/* END Kary Pro Colors */

void Theme::fillClassic() {
    QColor orange("#F8A200");
    QColor pale_blue("#3333bf");
    QColor green("#007300");

    add("text", Qt::black, Qt::white);
    add("currentLine", Qt::black, QColor("#f4f4f4"));
    add("searchResult", Qt::black, orange);
    add("matchingBrackets", QColor("#2bc93d"), QColor("#ffff00"), true);
    add("mismatchedBrackets", Qt::white, QColor("#960000"));
    add("evaluatedCode", Qt::black, orange);
    add("whitespace", QColor("#888888"));
    add("keyword", QColor("#0000e6"), QColor(Qt::transparent), true);
    add("built-in", pale_blue);
    add("env-var", QColor("#8c4614"));
    add("class", QColor("#0000d2"));
    add("number", QColor("#980099"));
    add("symbol", green);
    add("string", QColor("#5f5f5f"));
    add("char", green);
    add("comment", QColor("#bf0000"));
    add("primitive", pale_blue);
    add("lineNumbers", Qt::black, QColor("#b8b8b8"));
    add("selection", Qt::white, QColor("#308cc6"));
    add("postwindowtext", Qt::black);
    add("postwindowerror", QColor("#d11c24"));
    add("postwindowwarning", QColor("#a57706"));
    add("postwindowsuccess", QColor("#738a05"));
    add("postwindowemphasis", Qt::black, Qt::transparent, true);
}

void Theme::fillDark() {
    add("text", QColor("#ffa4e2"), Qt::black);
    add("currentLine", QColor("#e4e4e4"), QColor("#393939"));
    add("searchResult", QColor("#e4e4e4"), QColor("#194c7f"));
    add("matchingBrackets", QColor("#ff5500"), QColor("#001d49"), true);
    add("mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    add("evaluatedCode", QColor("#e4e4e4"), QColor("#636397"));
    add("whitespace", QColor("#e4e4e4"));
    add("keyword", QColor("#aaaaff"), Qt::transparent, true);
    add("built-in", QColor("#ffa4e2"));
    add("env-var", QColor("#73e7ad"));
    add("class", QColor("#00abff"), Qt::transparent, true);
    add("number", QColor("#4aff00"));
    add("symbol", QColor("#ddde00"));
    add("string", QColor("#d7d7d7"));
    add("char", QColor("#ff55ff"));
    add("comment", QColor("#d4b982"));
    add("primitive", QColor("#aaff7f"));
    add("lineNumbers", QColor("#cfcfcf"));
    add("selection", QColor("#ff5500"));
    add("postwindowtext", QColor("#e4e4e4"));
    add("postwindowerror", QColor("#ff1f2a"));
    add("postwindowwarning", QColor("#de7100"));
    add("postwindowsuccess", QColor("#b0d206"));
    add("postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
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

void Theme::fillDracula() {
    add("text", QColor("#f7fdff"), QColor("#282a36"));
    add("currentLine", QColor("#909194"), QColor("#282a36"));
    add("searchResult", QColor("#e4e4e4"), QColor("#194c7f"));
    add("matchingBrackets", QColor("#ff5500"), QColor("#001d49"), true);
    add("mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    add("evaluatedCode", QColor("#e4e4e4"), QColor("#636397"));
    add("whitespace", QColor("#e4e4e4"));
    add("keyword", QColor("#ff76c7"), Qt::transparent, true);
    add("built-in", QColor("#e2e37a"));
    add("env-var", QColor("#ffb965"));
    add("class", QColor("#65e5ff"), Qt::transparent, true);
    add("number", QColor("#be90fc"));
    add("symbol", QColor("#45fc75"));
    add("string", QColor("#5df884"));
    add("char", QColor("#ff55ff"));
    add("comment", QColor("#6071a6"));
    add("primitive", QColor("#aaff7f"));
    add("lineNumbers", QColor("#909194"));
    add("selection", QColor("#e0eeff"));
    add("postwindowtext", QColor("#dfe0fc"));
    add("postwindowerror", QColor("#ff1f2a"));
    add("postwindowwarning", QColor("#de7100"));
    add("postwindowsuccess", QColor("#b0d206"));
    add("postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
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

void Theme::fillSolarizedLight() {
    add("text", QColor("#657b83"), QColor("#fdf6e3"));
    add("currentLine", Qt::transparent, QColor("#eee8d5"));
    add("searchResult", QColor("#93a1a1"), QColor("#073642"));
    add("matchingBrackets", QColor("#002b36"), QColor("#eee8d5"), true);
    add("mismatchedBrackets", QColor("#eee8d5"), QColor("#586e75"));
    add("evaluatedCode", QColor("#586e75"), QColor("#eee8d5"));
    add("whitespace", Qt::transparent);
    add("keyword", QColor("#dc322f"), Qt::transparent, true);
    add("built-in", QColor("#b58900"));
    add("env-var", QColor("#d33682"));
    add("class", QColor("#268bd2"));
    add("number", QColor("#6c71c4"));
    add("symbol", QColor("#b58900"));
    add("string", QColor("#93a1a1"));
    add("char", QColor("#cb4b16"));
    add("comment", QColor("#586e75"), Qt::transparent, false, true);
    add("primitive", QColor("#2aa198"));
    add("lineNumbers", QColor("#839496"), QColor("#eee8d5"));
    add("selection", QColor("#fdf6e3"), QColor("#657b83"));
    add("postwindowtext", QColor("#657b83"));
    add("postwindowerror", QColor("#dc322f"));
    add("postwindowwarning", QColor("#cb4b16"));
    add("postwindowsuccess", QColor("#859900"));
    add("postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

void Theme::fillSolarizedDark() {
    add("text", QColor("#839496"), QColor("#002b36"));
    add("currentLine", Qt::transparent, QColor("#073642"));
    add("searchResult", QColor("#586e75"), QColor("#eee8d5"));
    add("matchingBrackets", QColor("#fdf6e3"), QColor("#073642"), true);
    add("mismatchedBrackets", QColor("#073642"), QColor("#93a1a1"));
    add("evaluatedCode", QColor("#93a1a1"), QColor("#073642"));
    add("whitespace", Qt::transparent);
    add("keyword", QColor("#dc322f"), Qt::transparent, true);
    add("built-in", QColor("#b58900"));
    add("env-var", QColor("#d33682"));
    add("class", QColor("#268bd2"));
    add("number", QColor("#6c71c4"));
    add("symbol", QColor("#b58900"));
    add("string", QColor("#586e75"));
    add("char", QColor("#cb4b16"));
    add("comment", QColor("#93a1a1"), Qt::transparent, false, true);
    add("primitive", QColor("#2aa198"));
    add("lineNumbers", QColor("#657b83"), QColor("#073642"));
    add("selection", QColor("#002b36"), QColor("#839496"));
    add("postwindowtext", QColor("#839496"));
    add("postwindowerror", QColor("#dc322f"));
    add("postwindowwarning", QColor("#cb4b16"));
    add("postwindowsuccess", QColor("#859900"));
    add("postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

/* END MIT LICENSED CODE */

}} // namespace ScIDE
