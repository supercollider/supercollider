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
#include "../../core/settings/manager.hpp"
#include "../../core/sc_syntax_highlighter.hpp"
#include "../../core/main.hpp"

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

void EditorPage::load( Manager *s )
{
    s->beginGroup("IDE/editor");

    ui->spaceIndent->setChecked( s->value("spaceIndent").toBool() );
    ui->indentWidth->setValue( s->value("indentWidth").toInt() );
    ui->stepForwardEvaluation->setChecked( s->value("stepForwardEvaluation").toBool() );

    s->beginGroup("colors");

    ui->bgColor->setColor( s->value("background").value<QColor>() );
    ui->textColor->setColor( s->value("text").value<QColor>() );
    ui->bracketColor->setColor( s->value("matchingBrackets").value<QColor>() );

    s->endGroup(); // colors

    s->beginGroup("highlighting");

    QStringList formatNames;
    formatNames << "Keyword" << "BuiltIn" << "EnvVar" << "Class" << "Number"
        << "Symbol" << "String" << "Char" << "Comment" << "Primitive";
    foreach(const QString &name, formatNames)
        ui->textFormatList->addFormat(name, s->value(name.toLower()).value<QTextCharFormat>());

    s->endGroup(); // highlighting

    s->endGroup(); // IDE/editor
}

void EditorPage::store( Manager *s )
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

    QString formatName = ui->textFormatList->name(i);
    QMenu *menu = new QMenu(formatName, this);
    QAction *actReset = menu->addAction("Reset to Default");
    QAction *actClearBg = menu->addAction("Clear Background");

    QAction *result = menu->exec(ui->textFormatList->viewport()->mapToGlobal(pos));

    if (result == actReset)
    {
        QList<SyntaxFormat> formats;
        formats /*<< PlainFormat*/ << KeywordFormat << BuiltinFormat << EnvVarFormat
            << ClassFormat << NumberFormat << SymbolFormat
            << StringFormat << CharFormat << CommentFormat << PrimitiveFormat;

        Manager *mng = Main::instance()->settings();
        mng->beginGroup("IDE/editor/highlighting");
        QTextCharFormat fm = mng->defaultValue(formatName.toLower()).value<QTextCharFormat>();
        mng->endGroup();

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

