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
#include "../code_editor/highlighter.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/main.hpp"

#include <QMenu>
#include <QDebug>
#include <QListWidgetItem>
#include <QFontDatabase>

namespace ScIDE { namespace Settings {

EditorPage::EditorPage(QWidget *parent) :
    QWidget(parent),
    ui( new Ui::EditorConfigPage ),
    fontDatabase(new QFontDatabase)
{
    ui->setupUi(this);

    new SyntaxHighlighter(static_cast<QPlainTextEdit*>(ui->fontPreview)->document());

    connect( ui->onlyMonoFonts, SIGNAL(toggled(bool)), this, SLOT(onMonospaceToggle(bool)) );
    connect( ui->fontList, SIGNAL(currentRowChanged(int)), this, SLOT(onFontFamilyChanged(int)) );
    connect( ui->fontStyle, SIGNAL(currentRowChanged(int)), this, SLOT(onFontStyleChanged(int)) );
    connect( ui->fontSize, SIGNAL(valueChanged(double)), this, SLOT(updateFontPreview()) );
    connect( ui->textFormatList, SIGNAL(customContextMenuRequested(const QPoint&)),
             this, SLOT(execSyntaxFormatContextMenu(const QPoint&)) );
}

EditorPage::~EditorPage()
{
    delete ui;
    delete fontDatabase;
}

void EditorPage::load( Manager *s )
{
    s->beginGroup("IDE/editor");

    ui->spaceIndent->setChecked( s->value("spaceIndent").toBool() );
    ui->indentWidth->setValue( s->value("indentWidth").toInt() );
    ui->stepForwardEvaluation->setChecked( s->value("stepForwardEvaluation").toBool() );

    QFont f;
    f.fromString(s->value("font").toString());
    fontFamily = f.family();
    fontStyle = fontDatabase->styleString(f);

    ui->fontList->clear();
    int idx = 0;
    QStringList fontFamilies = fontDatabase->families();
    foreach(QString family, fontFamilies)
    {
        ui->fontList->addItem(family);
        if(family.compare(fontFamily, Qt::CaseInsensitive) == 0)
            idx = ui->fontList->count() - 1;
    }
    ui->fontList->setCurrentRow(idx);
    ui->fontList->scrollToItem(ui->fontList->item(idx));

    int c = ui->fontStyle->count();
    for(int i = 0; i < c; ++i) {
        QString style = ui->fontStyle->item(i)->text();
        if(style.compare(fontStyle, Qt::CaseInsensitive)==0) {
            ui->fontStyle->setCurrentRow(i);
            break;
        }
    }

    ui->fontSize->setValue( f.pointSizeF() );

    s->beginGroup("colors");

    ui->bgColor->setColor( s->value("background").value<QColor>() );
    ui->textColor->setColor( s->value("text").value<QColor>() );
    ui->bracketColor->setColor( s->value("matchingBrackets").value<QColor>() );
    ui->evalCodeFg->setColor( s->value("evaluatedCodeText").value<QColor>() );
    ui->evalCodeBg->setColor( s->value("evaluatedCodeBackground").value<QColor>() );
    ui->lineNumbersFg->setColor( s->value("lineNumbers").value<QColor>() );
    ui->lineNumbersBg->setColor( s->value("lineNumbersBackground").value<QColor>() );

    s->endGroup(); // colors

    s->beginGroup("highlighting");

    QStringList formatNames;
    formatNames << "Keyword" << "BuiltIn" << "EnvVar" << "Class" << "Number"
        << "Symbol" << "String" << "Char" << "Comment" << "Primitive";
    foreach(const QString &name, formatNames)
        ui->textFormatList->addFormat(name, s->value(name.toLower()).value<QTextCharFormat>());

    s->endGroup(); // highlighting

    s->endGroup(); // IDE/editor

    ui->fontPreview->applySettings(s);
}

void EditorPage::store( Manager *s )
{
    s->beginGroup("IDE/editor");

    s->setValue("spaceIndent", ui->spaceIndent->isChecked());
    s->setValue("indentWidth", ui->indentWidth->value());
    s->setValue("stepForwardEvaluation", ui->stepForwardEvaluation->isChecked());

    QFont f = constructFont();
    s->setValue("font", f.toString());

    s->beginGroup("colors");

    s->setValue("background", ui->bgColor->color());
    s->setValue("text", ui->textColor->color());
    s->setValue("matchingBrackets", ui->bracketColor->color());
    s->setValue("evaluatedCodeText", ui->evalCodeFg->color());
    s->setValue("evaluatedCodeBackground", ui->evalCodeBg->color());
    s->setValue("lineNumbers", ui->lineNumbersFg->color());
    s->setValue("lineNumbersBackground", ui->lineNumbersBg->color());

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

void EditorPage::onFontFamilyChanged(int idx)
{
    if(idx < 0) return;

    Q_ASSERT(fontDatabase);

    fontFamily = ui->fontList->item(idx)->text();

    QStringList styles = fontDatabase->styles(fontFamily);
    ui->fontStyle->clear();
    int styleIdx = 0;
    foreach(QString style, styles) {
        ui->fontStyle->addItem(style);
        if( style.compare("Regular", Qt::CaseInsensitive) == 0
            || style.compare("Normal", Qt::CaseInsensitive) == 0
        )
            styleIdx = ui->fontStyle->count()-1;
    }

    ui->fontStyle->setCurrentRow(styleIdx);
    updateFontPreview();
}

void EditorPage::onFontStyleChanged(int idx)
{
    updateFontPreview();
}

void EditorPage::onMonospaceToggle(bool on)
{
    int c = ui->fontList->count();
    for(int i = 0; i < c; ++i)
    {
        QListWidgetItem *item = ui->fontStyle->currentItem();
        QString style;
        if(item) style = item->text();

        item = ui->fontList->item(i);
        QString family = item->text();

        item->setHidden(on && !fontDatabase->isFixedPitch(family, style));
    }
}

void EditorPage::updateFontPreview()
{
    ui->fontPreview->setFont(constructFont());
}

QFont EditorPage::constructFont()
{
    QString family, style;
    double size;

    QListWidgetItem *item = ui->fontList->currentItem();
    if(item)
        family = item->text();

    item = ui->fontStyle->currentItem();
    if(item)
        style = item->text();

    size = ui->fontSize->value();

    QFont f = fontDatabase->font(family, style, size);
    f.setPointSizeF(size);

    return f;
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

