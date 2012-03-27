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

#include "editor_page.hpp"
#include "ui_settings_editor.h"
#include "../../core/settings.hpp"
#include "../../core/sc_syntax_highlighter.hpp"

#include <QMenu>
#include <QDebug>

namespace ScIDE { namespace Settings {

EditorPage::EditorPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::EditorConfigPage )
{
    ui->setupUi(this);

    connect( ui->textFormatList, SIGNAL(customContextMenuRequested(const QPoint&)),
             this, SLOT(execSyntaxFormatContextMenu(const QPoint&)) );
}

EditorPage::~EditorPage()
{
    delete ui;
}

void EditorPage::load( QSettings *s )
{
    s->beginGroup("IDE/editor");

    ui->spaceIndent->setChecked( s->value("spaceIndent", true).toBool() );
    ui->indentWidth->setValue( s->value("indentWidth", 4).toInt() );
    ui->stepForwardEvaluation->setChecked( s->value("stepForwardEvaluation", false).toBool() );

    s->beginGroup("colors");

    QPalette appPlt( QApplication::palette() );

    ui->bgColor->setColor( s->value("background", appPlt.color(QPalette::Base) ).value<QColor>() );
    ui->textColor->setColor( s->value("text", appPlt.color(QPalette::Text) ).value<QColor>() );
    ui->bracketColor->setColor( s->value("matchingBrackets", Qt::gray ).value<QColor>() );

    s->endGroup(); // colors

    const SyntaxHighlighterGlobals *scSyntax = SyntaxHighlighterGlobals::instance();
    //ui->textFormatList->addFormat( "Normal", scSyntax->format(PlainFormat) );
    ui->textFormatList->addFormat( "Keyword", scSyntax->format(KeywordFormat) );
    ui->textFormatList->addFormat( "BuiltIn", scSyntax->format(BuiltinFormat) );
    ui->textFormatList->addFormat( "EnvVar", scSyntax->format(EnvVarFormat) );
    ui->textFormatList->addFormat( "Class", scSyntax->format(ClassFormat) );
    ui->textFormatList->addFormat( "Number", scSyntax->format(NumberFormat) );
    ui->textFormatList->addFormat( "Symbol", scSyntax->format(SymbolFormat) );
    ui->textFormatList->addFormat( "String", scSyntax->format(StringFormat) );
    ui->textFormatList->addFormat( "Char", scSyntax->format(CharFormat) );
    ui->textFormatList->addFormat( "Comment", scSyntax->format(CommentFormat) );
    ui->textFormatList->addFormat( "Primitive", scSyntax->format(PrimitiveFormat) );

    s->endGroup();
}

void EditorPage::store( QSettings *s )
{
    s->beginGroup("IDE/editor");

    s->setValue("spaceIndent", ui->spaceIndent->isChecked());
    s->setValue("indentWidth", ui->indentWidth->value());
    s->setValue("stepForwardEvaluation", ui->stepForwardEvaluation->isChecked());

    s->beginGroup("colors");

    s->setValue("background", ui->bgColor->color());
    s->setValue("text", ui->textColor->color());
    s->setValue("matchingBrackets", ui->bracketColor->color());

    s->endGroup(); // colors

    s->beginGroup("highlighting");

    int formatCount = ui->textFormatList->count();
    for(int i = 0; i < formatCount; ++i)
    {
        QString name = ui->textFormatList->name(i);
        QTextCharFormat fm = ui->textFormatList->format(i);
        s->setValue(name.toLower(), QVariant::fromValue<QTextCharFormat>(fm));
    }

    s->endGroup(); // highlighting

    s->endGroup();
}

void EditorPage::execSyntaxFormatContextMenu(const QPoint &pos)
{
    int i = ui->textFormatList->currentIndex();
    if (i < 0) return;

    QMenu *menu = new QMenu(ui->textFormatList->name(i), this);
    QAction *actReset = menu->addAction("Reset to Default");
    QAction *actClearBg = menu->addAction("Clear Background");

    QAction *result = menu->exec(ui->textFormatList->viewport()->mapToGlobal(pos));

    if (result == actReset)
    {
        QList<SyntaxFormat> formats;
        formats /*<< PlainFormat*/ << KeywordFormat << BuiltinFormat << EnvVarFormat
            << ClassFormat << NumberFormat << SymbolFormat
            << StringFormat << CharFormat << CommentFormat << PrimitiveFormat;

        QTextCharFormat fm =
            SyntaxHighlighterGlobals::instance()->defaultFormat(formats[i]);

        ui->textFormatList->setFormat(i, fm);
    }
    else if (result == actClearBg)
    {
        QTextCharFormat fm( ui->textFormatList->format(i) );
        fm.clearBackground();
        ui->textFormatList->setFormat(i, fm);
    }
}

}} // namespace ScIDE::Settings

