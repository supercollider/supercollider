/*
    SuperCollider Qt IDE
    Copyright(c) 2012 Jakob Leben & Tim Blechmann
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

#include <QTextCharFormat>
#include <QString>
#include <QMap>
#include <QList>
#include <QApplication>
#include <QPalette>
#include <QDebug>

#include "../main.hpp"
#include "theme.hpp"
#include "manager.hpp"

namespace ScIDE { namespace Settings {

int legacyTheme(Manager * settings)
{
    QString group = QStringLiteral("IDE/editor/colors/");
    QString newGroup = QStringLiteral("IDE/editor/themes/My old theme/");

    if (!settings->contains(group + "evaluatedCode"))
        return 0;

    /* import default values */
    Theme theme("My old theme", "default", settings);
    theme.save();

    QList<QString> keys;
    keys << "evaluatedCode" << "lineNumbers" << "matchingBrackets"
         << "searchResult" << "selection" << "text" << "currentLine"
         << "matchingBrackets" << "postwindowtext";

    foreach(QString key, keys) {
        if (settings->contains(group + key)) {
            QTextCharFormat fm = settings->value(group + key).value<QTextCharFormat>();
            settings->setValue(newGroup + key, QVariant::fromValue<QTextCharFormat>(fm));
        }
    }
    settings->remove(group);

    group = QStringLiteral("IDE/editor/highlighting/");
    keys.clear();

    keys << "keyword" << "built-in" << "env-var" << "class" << "number"
         << "symbol" << "string" << "char" << "comment" << "primitive"
         << "postwindowemphasis" << "postwindowerror" << "postwindowsuccess"
         << "postwindowwarning" << "whitespace";

    foreach(QString key, keys) {
        if (settings->contains(group + key)) {
            QTextCharFormat fm = theme.format(key);
            fm.merge(settings->value(group + key).value<QTextCharFormat>());
            settings->setValue(newGroup + key, QVariant::fromValue<QTextCharFormat>(fm));
        }
    }
    settings->remove(group);

    settings->setValue("IDE/editor/theme", "My old theme");

    return 1;
}

static void addToTheme(QMap<QString, QTextCharFormat *> & map, const char *key,
                  const QColor & fg, const QColor & bg = QColor(Qt::transparent),
                  bool bold = false, bool italic = false)
{
    QTextCharFormat *format = new QTextCharFormat();

    if (bg != QColor(Qt::transparent))
        format->setBackground(bg);

    if (fg != QColor(Qt::transparent))
        format->setForeground(fg);

    if (bold)
        format->setFontWeight(QFont::Bold);
    format->setFontItalic(italic);

    map.insert(key, format);
}

void Theme::fillDefault()
{
    addToTheme(mFormats, "text", QColor(Qt::black), QColor(Qt::white));

    QPalette appPlt(QApplication::palette());
    QColor bkg = appPlt.color(QPalette::Base);
    int value = bkg.value();
    if (value > 40)
        bkg.setHsv(bkg.hue(), bkg.saturation(), value - 11);
    else
        bkg.setHsv(bkg.hue(), bkg.saturation(), value + 20);
    addToTheme(mFormats, "currentLine", QColor(Qt::black), bkg.toRgb());
    addToTheme(mFormats, "searchResult",
               appPlt.color(QPalette::HighlightedText).darker(200),
               appPlt.color(QPalette::Highlight).darker(200));
    addToTheme(mFormats, "matchingBrackets", QColor("#2bc93d"), Qt::yellow, true);
    addToTheme(mFormats, "mismatchedBrackets", Qt::white, QColor(150,0,0));
    addToTheme(mFormats, "evaluatedCode", Qt::black, QColor("#F8A200"));

    QPalette plt(QApplication::palette());
    QColor base = plt.color(QPalette::Base);
    QColor text = plt.color(QPalette::Text);
    int shade = (base.red() + base.green() + base.blue() < 380) ? 160 : 100;

    QColor whitespace_color((base.red() + text.red()) / 2,
                            (base.green() + text.green()) / 2,
                            (base.blue() + text.blue()) / 2);

    addToTheme(mFormats, "whitespace", whitespace_color);
    addToTheme(mFormats, "keyword", QColor(0,0,230).lighter(shade),
                                    QColor(Qt::transparent), true);
    addToTheme(mFormats, "built-in", QColor(51,51,191).lighter(shade));
    addToTheme(mFormats, "env-var", QColor(140,70,20).lighter(shade));
    addToTheme(mFormats, "class", QColor(0,0,210).lighter(shade));
    addToTheme(mFormats, "number", QColor(152,0,153).lighter(shade));
    addToTheme(mFormats, "symbol", QColor(0,115,0).lighter(shade));
    addToTheme(mFormats, "string", QColor(95,95,95).lighter(shade));
    addToTheme(mFormats, "char", QColor(0,115,0).lighter(shade));
    addToTheme(mFormats, "comment", QColor(191,0,0).lighter(shade));
    addToTheme(mFormats, "primitive", QColor(51,51,191).lighter(shade));
    addToTheme(mFormats, "lineNumbers", plt.color(QPalette::ButtonText),
                                        plt.color(QPalette::Mid));
    addToTheme(mFormats, "selection", plt.color(QPalette::HighlightedText),
                                      plt.color(QPalette::Highlight));
    addToTheme(mFormats, "postwindowtext", plt.color(QPalette::ButtonText));
    addToTheme(mFormats, "postwindowerror", QColor(209, 28, 36));
    addToTheme(mFormats, "postwindowwarning", QColor(165, 119, 6));
    addToTheme(mFormats, "postwindowsuccess", QColor(115, 138, 5));
    addToTheme(mFormats, "postwindowemphasis", Qt::black, Qt::transparent, true);
}

void Theme::fillDark()
{
    addToTheme(mFormats, "text",               QColor("#ffa4e2"), Qt::black);
    addToTheme(mFormats, "currentLine",        QColor("#e4e4e4"), QColor("#393939"));
    addToTheme(mFormats, "searchResult",       QColor("#e4e4e4"), QColor("#194c7f"));
    addToTheme(mFormats, "matchingBrackets",   QColor("#ff5500"), QColor("#001d49"), true);
    addToTheme(mFormats, "mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    addToTheme(mFormats, "evaluatedCode",      QColor("#e4e4e4"), QColor("#636397"));
    addToTheme(mFormats, "whitespace",         QColor("#e4e4e4"));
    addToTheme(mFormats, "keyword",            QColor("#aaaaff"), Qt::transparent, true);
    addToTheme(mFormats, "built-in",           QColor("#ffa4e2"));
    addToTheme(mFormats, "env-var",            QColor("#73e7ad"));
    addToTheme(mFormats, "class",              QColor("#00abff"), Qt::transparent, true);
    addToTheme(mFormats, "number",             QColor("#4aff00"));
    addToTheme(mFormats, "symbol",             QColor("#ddde00"));
    addToTheme(mFormats, "string",             QColor("#d7d7d7"));
    addToTheme(mFormats, "char",               QColor("#ff55ff"));
    addToTheme(mFormats, "comment",            QColor("#d4b982"));
    addToTheme(mFormats, "primitive",          QColor("#aaff7f"));
    addToTheme(mFormats, "lineNumbers",        QColor("#cfcfcf"));
    addToTheme(mFormats, "selection",          QColor("#ff5500"));
    addToTheme(mFormats, "postwindowtext",     QColor("#e4e4e4"));
    addToTheme(mFormats, "postwindowerror",    QColor("#ff1f2a"));
    addToTheme(mFormats, "postwindowwarning",  QColor("#de7100"));
    addToTheme(mFormats, "postwindowsuccess",  QColor("#b0d206"));
    addToTheme(mFormats, "postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
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
    addToTheme(mFormats, "text",               QColor("#f7fdff"), QColor("#282a36"));
    addToTheme(mFormats, "currentLine",        QColor("#909194"), QColor("#282a36"));
    addToTheme(mFormats, "searchResult",       QColor("#e4e4e4"), QColor("#194c7f"));
    addToTheme(mFormats, "matchingBrackets",   QColor("#ff5500"), QColor("#001d49"), true);
    addToTheme(mFormats, "mismatchedBrackets", QColor("#ffaa00"), QColor("#980000"));
    addToTheme(mFormats, "evaluatedCode",      QColor("#e4e4e4"), QColor("#636397"));
    addToTheme(mFormats, "whitespace",         QColor("#e4e4e4"));
    addToTheme(mFormats, "keyword",            QColor("#ff76c7"), Qt::transparent, true);
    addToTheme(mFormats, "built-in",           QColor("#e2e37a"));
    addToTheme(mFormats, "env-var",            QColor("#ffb965"));
    addToTheme(mFormats, "class",              QColor("#65e5ff"), Qt::transparent, true);
    addToTheme(mFormats, "number",             QColor("#be90fc"));
    addToTheme(mFormats, "symbol",             QColor("#45fc75"));
    addToTheme(mFormats, "string",             QColor("#5df884"));
    addToTheme(mFormats, "char",               QColor("#ff55ff"));
    addToTheme(mFormats, "comment",            QColor("#6071a6"));
    addToTheme(mFormats, "primitive",          QColor("#aaff7f"));
    addToTheme(mFormats, "lineNumbers",        QColor("#909194"));
    addToTheme(mFormats, "selection",          QColor("#e0eeff"));
    addToTheme(mFormats, "postwindowtext",     QColor("#dfe0fc"));
    addToTheme(mFormats, "postwindowerror",    QColor("#ff1f2a"));
    addToTheme(mFormats, "postwindowwarning",  QColor("#de7100"));
    addToTheme(mFormats, "postwindowsuccess",  QColor("#b0d206"));
    addToTheme(mFormats, "postwindowemphasis", QColor("#e4e4e4"), Qt::transparent, true);
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
    addToTheme(mFormats, "text",               QColor("#657b83"), QColor("#fdf6e3"));
    addToTheme(mFormats, "currentLine",        Qt::transparent, QColor("#eee8d5"));
    addToTheme(mFormats, "searchResult",       QColor("#93a1a1"), QColor("#073642"));
    addToTheme(mFormats, "matchingBrackets",   QColor("#002b36"), QColor("#eee8d5"), true);
    addToTheme(mFormats, "mismatchedBrackets", QColor("#eee8d5"), QColor("#586e75"));
    addToTheme(mFormats, "evaluatedCode",      QColor("#586e75"), QColor("#eee8d5"));
    addToTheme(mFormats, "whitespace",         Qt::transparent);
    addToTheme(mFormats, "keyword",            QColor("#dc322f"), Qt::transparent, true);
    addToTheme(mFormats, "built-in",           QColor("#b58900"));
    addToTheme(mFormats, "env-var",            QColor("#d33682"));
    addToTheme(mFormats, "class",              QColor("#268bd2"));
    addToTheme(mFormats, "number",             QColor("#6c71c4"));
    addToTheme(mFormats, "symbol",             QColor("#b58900"));
    addToTheme(mFormats, "string",             QColor("#93a1a1"));
    addToTheme(mFormats, "char",               QColor("#cb4b16"));
    addToTheme(mFormats, "comment",            QColor("#586e75"), Qt::transparent, false, true);
    addToTheme(mFormats, "primitive",          QColor("#2aa198"));
    addToTheme(mFormats, "lineNumbers",        QColor("#839496"), QColor("#eee8d5"));
    addToTheme(mFormats, "selection",          QColor("#fdf6e3"), QColor("#657b83"));
    addToTheme(mFormats, "postwindowtext",     QColor("#657b83"));
    addToTheme(mFormats, "postwindowerror",    QColor("#dc322f"));
    addToTheme(mFormats, "postwindowwarning",  QColor("#cb4b16"));
    addToTheme(mFormats, "postwindowsuccess",  QColor("#859900"));
    addToTheme(mFormats, "postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

void Theme::fillSolarizedDark()
{
    addToTheme(mFormats, "text",               QColor("#839496"), QColor("#002b36"));
    addToTheme(mFormats, "currentLine",        Qt::transparent, QColor("#073642"));
    addToTheme(mFormats, "searchResult",       QColor("#586e75"), QColor("#eee8d5"));
    addToTheme(mFormats, "matchingBrackets",   QColor("#fdf6e3"), QColor("#073642"), true);
    addToTheme(mFormats, "mismatchedBrackets", QColor("#073642"), QColor("#93a1a1"));
    addToTheme(mFormats, "evaluatedCode",      QColor("#93a1a1"), QColor("#073642"));
    addToTheme(mFormats, "whitespace",         Qt::transparent);
    addToTheme(mFormats, "keyword",            QColor("#dc322f"), Qt::transparent, true);
    addToTheme(mFormats, "built-in",           QColor("#b58900"));
    addToTheme(mFormats, "env-var",            QColor("#d33682"));
    addToTheme(mFormats, "class",              QColor("#268bd2"));
    addToTheme(mFormats, "number",             QColor("#6c71c4"));
    addToTheme(mFormats, "symbol",             QColor("#b58900"));
    addToTheme(mFormats, "string",             QColor("#586e75"));
    addToTheme(mFormats, "char",               QColor("#cb4b16"));
    addToTheme(mFormats, "comment",            QColor("#93a1a1"), Qt::transparent, false, true);
    addToTheme(mFormats, "primitive",          QColor("#2aa198"));
    addToTheme(mFormats, "lineNumbers",        QColor("#657b83"), QColor("#073642"));
    addToTheme(mFormats, "selection",          QColor("#002b36"), QColor("#839496"));
    addToTheme(mFormats, "postwindowtext",     QColor("#839496"));
    addToTheme(mFormats, "postwindowerror",    QColor("#dc322f"));
    addToTheme(mFormats, "postwindowwarning",  QColor("#cb4b16"));
    addToTheme(mFormats, "postwindowsuccess",  QColor("#859900"));
    addToTheme(mFormats, "postwindowemphasis", QColor("#b58900"), Qt::transparent, true);
}

/* END MIT LICENSED COSE */

void Theme::fillUser(const QString & name, const Manager *settings)
{
    QString group = QStringLiteral("IDE/editor/themes/%1/").arg(name);

    if (!settings->contains(group + "whitespace")) {
        qDebug() << "Failed to find theme" << name << settings->group();
        return;
    }

    QList<QString> keys;
    keys << "text" << "currentLine" << "searchResult" << "matchingBrackets"
         << "mismatchedBrackets" << "evaluatedCode" << "whitespace" << "keyword"
         << "built-in" << "env-var" << "class" << "number" << "symbol" << "string"
         << "char" << "comment" << "primitive" << "lineNumbers" << "selection"
         << "postwindowtext" << "postwindowerror" << "postwindowwarning"
         << "postwindowsuccess" << "postwindowemphasis";

    foreach(QString key, keys) {
            QTextCharFormat *format =
                new QTextCharFormat(settings->value(group + key).value<QTextCharFormat>());
            mFormats.insert(key, format);
    }
}

Theme::Theme(const QString & _name, Manager * settings)
{
    if (!settings)
        settings = Main::settings();
    mSettings = settings;
    mName = _name;

    if (mName == "default") {
        fillDefault();
        mLocked = true;
    } else if (mName == "dark") {
        fillDark();
        mLocked = true;
    } else if (mName == "dracula") {
        fillDracula();
        mLocked = true;
    } else if (mName == "solarizedLight") {
        fillSolarizedLight();
        mLocked = true;
    } else if (mName == "solarizedDark") {
        fillSolarizedDark();
        mLocked = true;
    } else {
        fillUser(mName, settings);
        mLocked = false;
    }
}

Theme::Theme(const QString & _name, const QString & _source, Manager * settings):
    mName(_name)
{
    if (!settings)
        settings = Main::settings();
    mSettings = settings;

    if (_source == "default") {
        fillDefault();
    } else if (_source == "dark") {
        fillDark();
    } else if (_source == "dracula") {
        fillDracula();
    } else if (_source == "solarizedLight") {
        fillSolarizedLight();
    } else if (_source == "solarizedDark") {
        fillSolarizedDark();
    } else {
        fillUser(_source, settings);
    }

    mLocked = false;
}

Theme::~Theme()
{
    qDeleteAll(mFormats);
}

void Theme::setFormat(const QString & key, const QTextCharFormat & newFormat)
{
    QMap<QString, QTextCharFormat *>::iterator i = mFormats.find(key);
    bool fontWeight = (newFormat.fontWeight() == QFont::Bold) ? true : false;
    QColor bg = (newFormat.background() == Qt::NoBrush) ?
                    QColor(Qt::transparent): newFormat.background().color();
    QColor fg = (newFormat.foreground() == Qt::NoBrush) ?
                    QColor(Qt::transparent): newFormat.foreground().color();

    if (i == mFormats.end()) {
        qDebug() <<  __FUNCTION__ << "Theme::setFormat" << "Failed to find key " << key;
        return;
    }

    mFormats.remove(key);
    addToTheme(mFormats, key.toStdString().c_str(), fg, bg, fontWeight,
                                                    newFormat.fontItalic());
}

const QTextCharFormat & Theme::format(const QString & key)
{
    QMap<QString, QTextCharFormat *>::iterator i = mFormats.find(key);

    if (i == mFormats.end())
        qDebug() << "Failed to find key " << key;

    return (*i.value());
}

bool Theme::locked()
{
    return mLocked;
}

QList<QString> Theme::availableThemes()
{
    QList<QString> themes;

    themes.append("default");
    themes.append("dark");
    themes.append("dracula");
    themes.append("solarizedLight");
    themes.append("solarizedDark");

    mSettings->beginGroup("IDE/editor/themes");
    themes.append(mSettings->childGroups());
    mSettings->endGroup();

    return themes;
}

void Theme::save()
{
    if (mLocked)
        return;

    QMap<QString, QTextCharFormat *>::const_iterator i = mFormats.begin();
    QString group = QStringLiteral("IDE/editor/themes/").append(mName);

    mSettings->beginGroup(group);
    while (i != mFormats.end()) {
        QTextCharFormat fm = *i.value();
        mSettings->setValue(i.key(), QVariant::fromValue<QTextCharFormat>(fm));
        ++i;
    }
    mSettings->endGroup();
}

void Theme::remove()
{
    if (mLocked)
        return;

    QString key = QStringLiteral("IDE/editor/themes/").append(mName);

    mSettings->remove(key);
}

QString & Theme::name()
{
    return mName;
}

}} // namespace ScIDE::Settings
