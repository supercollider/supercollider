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
#include "theme.hpp"

#include <QApplication>
#include <QPalette>
#include <QTextCharFormat>
#include <QDebug>

namespace ScIDE { namespace Settings {

// manages preferences
Manager::Manager( const QString & filename, QObject * parent ):
    QObject(parent),
    mSettings( new QSettings(filename, serializationFormat(), this) ),
    mDefaultCursorFlashTime(QApplication::cursorFlashTime())
{
    QString th;

    initDefaults();

    if (legacyTheme(this))
        th = "My old theme";
    else
        th = value("IDE/editor/theme").toString();

    mTheme = new Theme(th, this);
}

void Manager::initDefaults()
{
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
    setDefault("disableBlinkingCursor", false);
    setDefault("highlightBracketContents", true);
    setDefault("inactiveEditorFadeAlpha", 64);
    setDefault("insertMatchingTokens", false);
    setDefault("showLinenumber", true);

    setDefault("blinkDuration", 600);

    setDefault("font/family", "monospace");
    setDefault("font/antialias", true);

    setDefault("theme", "default");

    endGroup(); // editor

    endGroup(); // IDE
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
    // This may be called inside a beginGroup/endGroup
    // Save current group state, and then restore later, so we can use abs keys
    QStringList oldGroup;
    while (!group().isEmpty()) {
        oldGroup.push_front(group());
        endGroup();
    }
    
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
    
    // restore group stack
    foreach(const QString& group, oldGroup) {
        beginGroup(group);
    }

    return font;
}

void Manager::setThemeVal(QString key, const QTextCharFormat &val)
{
    mTheme->setFormat(key, val);
}

const QTextCharFormat & Manager::getThemeVal(QString key)
{
    return mTheme->format(key);
}

void Manager::updateTheme()
{
    QString theme = value("IDE/editor/theme").toString();

    delete(mTheme);
    mTheme = new Theme(theme);
}

}} // namespace ScIDE::Settings
