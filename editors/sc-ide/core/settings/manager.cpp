/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
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

#include "manager.hpp"
#include "serialization.hpp"

#include <QApplication>
#include <QPalette>
#include <QTextCharFormat>

namespace ScIDE { namespace Settings {

Manager::Manager( const QString & filename, QObject * parent ):
    QObject(parent),
    mSettings( new QSettings(filename, serializationFormat(), this) )
{
    initDefaults();
}

void Manager::initDefaults()
{
    QPalette appPlt( QApplication::palette() );

    beginGroup("IDE");

    setDefault("startWithSession", "last");

    beginGroup("interpreter");
    setDefault("autoStart", true);
    endGroup();

    setDefault("postWindow/scrollback", 1000);

    beginGroup("editor");

    setDefault("spaceIndent", false);
    setDefault("indentWidth", 4);
    setDefault("stepForwardEvaluation", false);
    setDefault("lineWrap", true);

    setDefault("blinkDuration", 600);

    setDefault("font/family", "monospace");
    setDefault("font/antialias", true);

    beginGroup("colors");

    QTextCharFormat matchingBracketsFormat;
    matchingBracketsFormat.setForeground(Qt::red);
    matchingBracketsFormat.setFontWeight(QFont::Bold);
    setDefault("matchingBrackets", QVariant::fromValue(matchingBracketsFormat));

    QTextCharFormat bracketMismatchFormat;
    bracketMismatchFormat.setBackground(QColor(150,0,0));
    bracketMismatchFormat.setForeground(Qt::white);
    setDefault("mismatchedBrackets", QVariant::fromValue(bracketMismatchFormat));

    QTextCharFormat evaluatedCodeFormat;
    evaluatedCodeFormat.setBackground(QColor("#F8A200"));
    evaluatedCodeFormat.setForeground(Qt::black);
    setDefault("evaluatedCode", QVariant::fromValue(evaluatedCodeFormat));

    QTextCharFormat searchResultFormat;
    searchResultFormat.setBackground(appPlt.color(QPalette::Highlight).darker(200));
    searchResultFormat.setForeground(appPlt.color(QPalette::HighlightedText).darker(200));
    setDefault("searchResult", QVariant::fromValue(searchResultFormat));

    endGroup(); // colors

    beginGroup("highlighting");
    initHighlightingDefaults();
    endGroup(); // highlighting

    endGroup(); // editor

    endGroup(); // IDE
}

inline static QVariant makeHlFormat
    ( const QBrush &fg, QFont::Weight w = QFont::Normal )
{
    QTextCharFormat f;
    f.setForeground(fg);
    if(w != QFont::Normal)
        f.setFontWeight(w);
    return QVariant::fromValue<QTextCharFormat>(f);
}

void Manager::initHighlightingDefaults()
{
    QPalette plt( QApplication::palette() );
    QColor base = plt.color(QPalette::Base);
    int shade = (base.red() + base.green() + base.blue() < 380) ? 160 : 100;

    setDefault( "keyword", makeHlFormat( QColor(0,0,230).lighter(shade), QFont::Bold ) );
    setDefault( "built-in", makeHlFormat( QColor(51,51,191).lighter(shade) ) );
    setDefault( "env-var", makeHlFormat( QColor(140,70,20).lighter(shade) ) );
    setDefault( "class", makeHlFormat( QColor(0,0,210).lighter(shade) ) );
    setDefault( "number", makeHlFormat( QColor(152,0,153).lighter(shade) ) );
    setDefault( "symbol", makeHlFormat( QColor(0,115,0).lighter(shade) ) );
    setDefault( "string", makeHlFormat( QColor(95,95,95).lighter(shade) ) );
    setDefault( "char", makeHlFormat( QColor(0,115,0).lighter(shade) ) );
    setDefault( "comment", makeHlFormat( QColor(191,0,0).lighter(shade) ) );
    setDefault( "primitive", makeHlFormat( QColor(51,51,191).lighter(shade) ) );
}

bool Manager::contains ( const QString & key ) const
{
    if ( mSettings->contains(key) )
        return true;
    else
        return mDefaults.contains( resolvedKey(key) );
}

QVariant Manager::value ( const QString & key ) const
{
    if ( mSettings->contains(key) )
        return mSettings->value(key);
    else
        return mDefaults.value(resolvedKey(key));
}

void Manager::setValue ( const QString & key, const QVariant & value )
{
    mSettings->setValue(key, value);
}

QKeySequence Manager::shortcut( const QString & key )
{
    return QKeySequence( value(key).toString() );
}

void Manager::addAction ( QAction *action, const QString &key, const QString &category )
{
    ActionData actionData;
    actionData.category = category;
    actionData.key = key;

    if (action->data().isValid()) {
        qWarning( "Settings::Manager: action '%s' of class '%s' has data."
                  " It will be overridden for settings purposes!",
                  qPrintable(action->text()),
                  action->parent()->metaObject()->className() );
    }

    action->setData( QVariant::fromValue(actionData) );

    mActions.append(action);

    beginGroup("IDE/shortcuts");

    setDefault( actionData.key, QVariant::fromValue(action->shortcut()) );
    action->setShortcut( value(actionData.key).value<QKeySequence>() );

    endGroup();
}

QString Manager::keyForAction ( QAction *action )
{
    ActionData actionData = action->data().value<ActionData>();
    return actionData.key;
}

QFont Manager::codeFont()
{
    QString fontFamily = value("IDE/editor/font/family").toString();
    int fontSize = value("IDE/editor/font/size").toInt();
    bool fontAntialas = value("IDE/editor/font/antialias").toBool();

    QFont font = QApplication::font("QPlainTextEdit");
    font.setStyleHint(QFont::TypeWriter);
    font.setFamily(fontFamily);
    if (fontSize > 0)
        font.setPointSize(fontSize);

    if (!fontAntialas)
        font.setStyleStrategy(QFont::StyleStrategy(font.styleStrategy() | QFont::NoAntialias));

    return font;
}

}} // namespace ScIDE::Settings
