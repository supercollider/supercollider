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

static void addToTheme(QMap<QString, QTextCharFormat *> & map, const char *key,
                  const QColor & bg, const QColor & fg,
                  bool bold = false, bool italic = false)
{
    QTextCharFormat *format = new QTextCharFormat();

    format->setBackground(bg);
    format->setForeground(fg);

    if (bold)
        format->setFontWeight(QFont::Bold);
    format->setFontItalic(italic);

    map.insert(key, format);
}

void Theme::fillDefault()
{
    addToTheme(mFormats, "text", QColor(Qt::white), QColor(Qt::black));

    QPalette appPlt(QApplication::palette());
    QColor bkg = appPlt.color(QPalette::Base);
    int value = bkg.value();
    if (value > 40)
        bkg.setHsv(bkg.hue(), bkg.saturation(), value - 11);
    else
        bkg.setHsv(bkg.hue(), bkg.saturation(), value + 20);
    addToTheme(mFormats, "currentLine", QColor(Qt::white), bkg.toRgb());
    addToTheme(mFormats, "searchResult",
               appPlt.color(QPalette::Highlight).darker(200),
               appPlt.color(QPalette::HighlightedText).darker(200));
    addToTheme(mFormats, "matchingBrackets", Qt::red, QColor("#ffff7f"), true);
    addToTheme(mFormats, "mismatchedBrackets", QColor(150,0,0), Qt::white);
    addToTheme(mFormats, "evaluatedCode", QColor("#F8A200"), Qt::black);

    QPalette plt(QApplication::palette());
    QColor base = plt.color(QPalette::Base);
    QColor text = plt.color(QPalette::Text);
    int shade =(base.red() + base.green() + base.blue() < 380) ? 160 : 100;

    QColor whitespace_color(
               (base.red() + text.red()) / 2,
               (base.green() + text.green()) / 2,
               (base.blue() + text.blue()) / 2);

    addToTheme(mFormats, "whitespace", Qt::white, whitespace_color);
    addToTheme(mFormats, "keyword", Qt::white, QColor(0,0,230).lighter(shade), true);
    addToTheme(mFormats, "built-in", Qt::white, QColor(51,51,191).lighter(shade));
    addToTheme(mFormats, "env-var", Qt::white, QColor(140,70,20).lighter(shade));
    addToTheme(mFormats, "class", Qt::white, QColor(0,0,210).lighter(shade));
    addToTheme(mFormats, "number", Qt::white, QColor(152,0,153).lighter(shade));
    addToTheme(mFormats, "symbol", Qt::white, QColor(0,115,0).lighter(shade));
    addToTheme(mFormats, "string", Qt::white, QColor(95,95,95).lighter(shade));
    addToTheme(mFormats, "char", Qt::white, QColor(0,115,0).lighter(shade));
    addToTheme(mFormats, "comment", Qt::white, QColor(191,0,0).lighter(shade));
    addToTheme(mFormats, "primitive", Qt::white, QColor(51,51,191).lighter(shade));
    addToTheme(mFormats, "lineNumbers", plt.color(QPalette::Mid),
                                       plt.color(QPalette::ButtonText));
    addToTheme(mFormats, "selection", plt.color(QPalette::Highlight),
                                       plt.color(QPalette::HighlightedText));
    addToTheme(mFormats, "postwindowtext", Qt::white, Qt::black);
    addToTheme(mFormats, "postwindowerror", Qt::white, QColor(209, 28, 36));
    addToTheme(mFormats, "postwindowwarning", Qt::white, QColor(165, 119, 6));
    addToTheme(mFormats, "postwindowsuccess", Qt::white, QColor(115, 138, 5));
    addToTheme(mFormats, "postwindowemphasis", Qt::white, Qt::black, true);
}

void Theme::fillUser(const QString & name, const Manager *settings)
{
    QString group = QString("IDE/editor/themes/%1/").arg(name);

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

Theme::Theme(const QString & _name, const Manager * settings)
{
    if (!settings)
        settings = Main::settings();
    mName = _name;

    if (mName == "default") {
        fillDefault();
        mLocked = true;
    } else {
        fillUser(mName, settings);
        mLocked = false;
    }
}

Theme::Theme(const QString & _name, const QString & _source):
    mName(_name)
{
    Manager *settings = Main::settings();

    if (_source == "default") {
        fillDefault();
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
    QTextCharFormat *format = i.value();

    if (i == mFormats.end()) {
        qDebug() << __func__ << "Failed to find key " << key;
        return;
    }

    format->setBackground(newFormat.background());
    format->setForeground(newFormat.foreground());
    format->setFontWeight(newFormat.fontWeight());
    format->setFontItalic(newFormat.fontItalic());
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
    Manager *settings = Main::settings();
    QList<QString> themes;

    themes.append("default");

    settings->beginGroup("IDE/editor/themes");
    themes.append(settings->childGroups());
    settings->endGroup();

    return themes;
}

void Theme::save()
{
    if (mLocked)
        return;

    Manager *settings = Main::settings();
    QMap<QString, QTextCharFormat *>::const_iterator i = mFormats.begin();
    QString group = QString("IDE/editor/themes/").append(mName);

    settings->beginGroup(group);
    while (i != mFormats.end()) {
        QTextCharFormat fm = *i.value();
        settings->setValue(i.key(), QVariant::fromValue<QTextCharFormat>(fm));
        i++;
    }
    settings->endGroup();
}

void Theme::remove()
{
    if (mLocked)
        return;

    Manager *settings = Main::settings();
    QMap<QString, QTextCharFormat *>::const_iterator i = mFormats.begin();
    QString key = QString("IDE/editor/themes/").append(mName);

    settings->remove(key);
}

QString & Theme::name()
{
    return mName;
}

}} // namespace ScIDE::Settings
