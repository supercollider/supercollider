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

#include <QTextCharFormat>
#include <QPalette>
#include <QApplication>

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

    beginGroup("interpreter");
    setDefault("autoStart", true);
    setDefault("command", "sclang");
    endGroup();

    beginGroup("editor");

    setDefault("spaceIndent", true);
    setDefault("indentWidth", 4);
    setDefault("stepForwardEvaluation", false);

    QFont fnt(QApplication::font());
    fnt.setFamily("monospace");
    fnt.setStyleHint(QFont::TypeWriter);
    setDefault("font", fnt.toString());

    beginGroup("colors");
    setDefault("background", appPlt.color(QPalette::Base));
    setDefault("text", appPlt.color(QPalette::Text));
    setDefault("matchingBrackets", Qt::gray);
    setDefault("evaluatedCodeBackground", QColor("#F8A200"));
    setDefault("evaluatedCodeText", Qt::black);
    setDefault("lineNumbersBackground", appPlt.color(QPalette::Button));
    setDefault("lineNumbers", appPlt.color(QPalette::ButtonText));
    endGroup(); // colors

    beginGroup("highlighting");
    initHighlightingDefaults();
    endGroup(); // highlighting

    endGroup(); // editor

    beginGroup("shortcuts");
    setDefault("cmdLineFocus", tr("Ctrl+Tab", "Toggle command line focus"));
    setDefault("quit", tr("Ctrl+Q", "Quit application"));
    setDefault("evaluateSelection", tr("Ctrl+Return", "Evaluate selection"));
    setDefault("evaluateRegion", tr("Alt+Return", "Evaluate region"));
    setDefault("stopMain", tr("Ctrl+.", "Stop Main (a.k.a. cmd-period)"));
    setDefault("helpForSelection", tr("Ctrl+H", "Help for selection"));
    setDefault("newDocument", tr("Ctrl+N", "New document"));
    setDefault("openDocument", tr("Ctrl+O", "Open document"));
    setDefault("saveDocument", tr("Ctrl+S", "Save document"));
    setDefault("reloadDocument", tr("F5", "Reload document"));
    setDefault("closeDocument", tr("Ctrl+W", "Close document"));
    setDefault("undo", tr("Ctrl+Z", "Undo"));
    setDefault("redo", tr("Ctrl+Shift+Z", "Redo"));
    setDefault("cut", tr("Ctrl+X", "Cut"));
    setDefault("copy", tr("Ctrl+C", "Copy"));
    setDefault("paste", tr("Ctrl+V", "Paste"));
    setDefault("find", tr("Ctrl+F", "Find"));
    setDefault("replace", tr("Ctrl+R", "Replace"));
    setDefault("hideToolPanel", tr("Esc", "Hide tool panel"));
    setDefault("enlargeFont", tr("Ctrl++", "Enlarge font"));
    setDefault("shrinkFont", tr("Ctrl+-", "Shrink font"));
    setDefault("openDefinition", tr("Ctrl+D", "Open definition"));
    endGroup(); // shortcuts

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
    setDefault( "builtin", makeHlFormat( QColor(51,51,191).lighter(shade) ) );
    setDefault( "envvar", makeHlFormat( QColor(255,102,0).lighter(shade) ) );
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

void Manager::addAction ( QAction *action )
{
    mActions.append(action);
    QString key = keyForAction(action);

    beginGroup("IDE/shortcuts");

    setDefault( key, QVariant::fromValue<QKeySequence>(action->shortcut()) );
    action->setShortcut( value(key).value<QKeySequence>() );

    endGroup();
}

QString Manager::keyForAction ( QAction *action )
{
    return action->text().toLower().remove('&').replace(' ', '_');
}

}} // namespace ScIDE::Settings
