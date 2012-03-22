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

namespace ScIDE { namespace Settings {

EditorPage::EditorPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::EditorConfigPage )
{
    ui->setupUi(this);
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
    s->endGroup();
}

void EditorPage::store( QSettings *s )
{
    s->beginGroup("IDE/editor");
    s->setValue("spaceIndent", ui->spaceIndent->isChecked());
    s->setValue("indentWidth", ui->indentWidth->value());
    s->setValue("stepForwardEvaluation", ui->stepForwardEvaluation->isChecked());
    s->endGroup();
}

}} // namespace ScIDE::Settings

