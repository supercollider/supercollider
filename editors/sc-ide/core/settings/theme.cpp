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

int legacyTheme(Manager* settings) {
    QString group = QStringLiteral("IDE/editor/colors/");
    QString newGroup = QStringLiteral("IDE/editor/themes/My old theme/");

    if (!settings->contains(group + "evaluatedCode"))
        return 0;

    /* import default values */
    Theme theme("My old theme", "default", settings);
    theme.save();

    QList<QString> keys;
    keys << "evaluatedCode"
         << "lineNumbers"
         << "matchingBrackets"
         << "searchResult"
         << "selection"
         << "text"
         << "currentLine"
         << "matchingBrackets"
         << "postwindowtext";

    foreach (QString key, keys) {
        if (settings->contains(group + key)) {
            QTextCharFormat fm = settings->value(group + key).value<QTextCharFormat>();
            settings->setValue(newGroup + key, QVariant::fromValue<QTextCharFormat>(fm));
        }
    }
    settings->remove(group);

    group = QStringLiteral("IDE/editor/highlighting/");
    keys.clear();

    keys << "keyword"
         << "built-in"
         << "env-var"
         << "class"
         << "number"
         << "symbol"
         << "string"
         << "char"
         << "comment"
         << "primitive"
         << "postwindowemphasis"
         << "postwindowerror"
         << "postwindowsuccess"
         << "postwindowwarning"
         << "whitespace";

    foreach (QString key, keys) {
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

void Theme::add(const char* key, const QColor& fg,
                const QColor& bg, // = QColor(Qt::transparent)
                bool bold, // = false
                bool italic // = false
) {
    QTextCharFormat* format = new QTextCharFormat();

    if (bg != QColor(Qt::transparent))
        format->setBackground(bg);

    if (fg != QColor(Qt::transparent))
        format->setForeground(fg);

    if (bold)
        format->setFontWeight(QFont::Bold);
    format->setFontItalic(italic);

    mFormats.insert(key, format);
}

void Theme::fillUser(const QString& name, const Manager* settings) {
    QString group = QStringLiteral("IDE/editor/themes/%1/").arg(name);

    if (!settings->contains(group + "whitespace")) {
        qDebug() << "Failed to find theme" << name << settings->group();
        return;
    }

    QList<QString> keys;
    keys << "text"
         << "currentLine"
         << "searchResult"
         << "matchingBrackets"
         << "mismatchedBrackets"
         << "evaluatedCode"
         << "whitespace"
         << "keyword"
         << "built-in"
         << "env-var"
         << "class"
         << "number"
         << "symbol"
         << "string"
         << "char"
         << "comment"
         << "primitive"
         << "lineNumbers"
         << "selection"
         << "postwindowtext"
         << "postwindowerror"
         << "postwindowwarning"
         << "postwindowsuccess"
         << "postwindowemphasis";

    foreach (QString key, keys) {
        QTextCharFormat* format = new QTextCharFormat(settings->value(group + key).value<QTextCharFormat>());
        mFormats.insert(key, format);
    }
}

Theme::Theme(const QString& _name, Manager* settings) {
    if (!settings)
        settings = Main::settings();
    mSettings = settings;
    mName = _name;

    if (mName == "default") {
        fillDefault();
        mLocked = true;
    } else if (mName == "classic") {
        fillClassic();
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

Theme::Theme(const QString& _name, const QString& _source, Manager* settings): mName(_name) {
    if (!settings)
        settings = Main::settings();
    mSettings = settings;

    if (_source == "default") {
        fillDefault();
    } else if (_source == "classic") {
        fillClassic();
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

Theme::~Theme() { qDeleteAll(mFormats); }

void Theme::setFormat(const QString& key, const QTextCharFormat& newFormat) {
    QMap<QString, QTextCharFormat*>::iterator i = mFormats.find(key);
    bool fontWeight = (newFormat.fontWeight() == QFont::Bold) ? true : false;
    QColor bg = (newFormat.background() == Qt::NoBrush) ? QColor(Qt::transparent) : newFormat.background().color();
    QColor fg = (newFormat.foreground() == Qt::NoBrush) ? QColor(Qt::transparent) : newFormat.foreground().color();

    if (i == mFormats.end()) {
        qDebug() << __FUNCTION__ << "Theme::setFormat"
                 << "Failed to find key " << key;
        return;
    }

    mFormats.remove(key);
    add(key.toStdString().c_str(), fg, bg, fontWeight, newFormat.fontItalic());
}

const QTextCharFormat& Theme::format(const QString& key) {
    QMap<QString, QTextCharFormat*>::iterator i = mFormats.find(key);

    if (i == mFormats.end())
        qDebug() << "Failed to find key " << key;

    return (*i.value());
}

bool Theme::locked() { return mLocked; }

QList<QString> Theme::availableThemes() {
    QList<QString> themes;

    themes.append("default");
    themes.append("classic");
    themes.append("dark");
    themes.append("dracula");
    themes.append("solarizedLight");
    themes.append("solarizedDark");

    mSettings->beginGroup("IDE/editor/themes");
    themes.append(mSettings->childGroups());
    mSettings->endGroup();

    return themes;
}

void Theme::save() {
    if (mLocked)
        return;

    QMap<QString, QTextCharFormat*>::const_iterator i = mFormats.begin();
    QString group = QStringLiteral("IDE/editor/themes/").append(mName);

    mSettings->beginGroup(group);
    while (i != mFormats.end()) {
        QTextCharFormat fm = *i.value();
        mSettings->setValue(i.key(), QVariant::fromValue<QTextCharFormat>(fm));
        ++i;
    }
    mSettings->endGroup();
}

void Theme::remove() {
    if (mLocked)
        return;

    QString key = QStringLiteral("IDE/editor/themes/").append(mName);

    mSettings->remove(key);
}

QString& Theme::name() { return mName; }

}} // namespace ScIDE::Settings
