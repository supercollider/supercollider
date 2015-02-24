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
    QString group = QString("IDE/editor/colors/");
    QString newGroup = QString("IDE/editor/themes/My old theme/");

    if (!settings->contains(group + "evaluatedCode"))
        return 0;

    /* import default values */
    Theme theme("My old theme", "default", settings);
    theme.save();

    QList<QString> keys;
    keys << "evaluatedCode" << "lineNumbers" << "matchingBrackets"
         << "searchResult" << "selection" << "text";

    foreach(QString key, keys) {
        if (settings->contains(group + key)) {
            QTextCharFormat fm = settings->value(group + key).value<QTextCharFormat>();
            settings->setValue(newGroup + key, QVariant::fromValue<QTextCharFormat>(fm));
        }
    }
    settings->remove(group);

    group = QString("IDE/editor/highlighting/");
    keys.clear();

    keys << "keyword" << "built-in" << "env-var" << "class" << "number"
         << "symbol" << "string" << "char" << "comment" << "primitive";

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
    addToTheme(mFormats, "matchingBrackets", QColor("#ffff7f"), Qt::red, true);
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

Theme::Theme(const QString & _name, Manager * settings)
{
    if (!settings)
        settings = Main::settings();
    mSettings = settings;
    mName = _name;

    if (mName == "default") {
        fillDefault();
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
    bool fontWeight = (newFormat.fontWeight() == QFont::Bold) ? true : false;
    QColor bg = (newFormat.background() == Qt::NoBrush) ?
                    QColor(Qt::transparent): newFormat.background().color();

    if (i == mFormats.end()) {
        qDebug() << __func__ << "Failed to find key " << key;
        return;
    }

    mFormats.remove(key);
    addToTheme(mFormats, key.toStdString().c_str(), newFormat.foreground().color(),
               bg, fontWeight, newFormat.fontItalic());
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
    QString group = QString("IDE/editor/themes/").append(mName);

    mSettings->beginGroup(group);
    while (i != mFormats.end()) {
        QTextCharFormat fm = *i.value();
        mSettings->setValue(i.key(), QVariant::fromValue<QTextCharFormat>(fm));
        i++;
    }
    mSettings->endGroup();
}

void Theme::remove()
{
    if (mLocked)
        return;

    QMap<QString, QTextCharFormat *>::const_iterator i = mFormats.begin();
    QString key = QString("IDE/editor/themes/").append(mName);

    mSettings->remove(key);
}

QString & Theme::name()
{
    return mName;
}

}} // namespace ScIDE::Settings
