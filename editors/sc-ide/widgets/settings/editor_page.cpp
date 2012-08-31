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
    fontDatabase(new QFontDatabase),
    ui( new Ui::EditorConfigPage )
{
    ui->setupUi(this);

    connect( ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)) );

    connect( ui->onlyMonoFonts, SIGNAL(toggled(bool)), this, SLOT(onMonospaceToggle(bool)) );
    connect( ui->fontList, SIGNAL(currentRowChanged(int)), this, SLOT(onFontFamilyChanged(int)) );
    connect( ui->fontStyle, SIGNAL(currentRowChanged(int)), this, SLOT(onFontStyleChanged(int)) );
    connect( ui->fontSize, SIGNAL(valueChanged(double)), this, SLOT(updateFontPreview()) );

    connect( ui->textFormats, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem*)),
             this, SLOT(updateTextFormatEdit()) );
    connect( ui->fgPicker, SIGNAL(colorPicked(QColor)),
             this, SLOT(updateTextFormatDisplay()) );
    connect( ui->bgPicker, SIGNAL(colorPicked(QColor)),
             this, SLOT(updateTextFormatDisplay()) );
    connect( ui->italicOption, SIGNAL(clicked()),
             this, SLOT(updateTextFormatDisplay()) );
    connect( ui->boldOption, SIGNAL(clicked()),
             this, SLOT(updateTextFormatDisplay()) );
    connect( ui->fgClearBtn, SIGNAL(clicked()), ui->fgPicker, SLOT(clear()) );
    connect( ui->fgClearBtn, SIGNAL(clicked()),
             this, SLOT(updateTextFormatDisplay()) );
    connect( ui->bgClearBtn, SIGNAL(clicked()), ui->bgPicker, SLOT(clear()) );
    connect( ui->bgClearBtn, SIGNAL(clicked()),
             this, SLOT(updateTextFormatDisplay()) );

    updateTextFormatEdit();
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

    ui->blinkDuration->setValue( s->value("blinkDuration").toInt() );
    ui->postWindowScrollback->setValue( s->value("postWindowScrollback").toInt() );

    QFont f;
    f.fromString(s->value("font").toString());

    ui->fontList->clear();
    int idx = 0;
    QString fontFamily = f.family();
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
    QString fontStyle = fontDatabase->styleString(f);
    for(int i = 0; i < c; ++i) {
        QString style = ui->fontStyle->item(i)->text();
        if(style.compare(fontStyle, Qt::CaseInsensitive)==0) {
            ui->fontStyle->setCurrentRow(i);
            break;
        }
    }

    ui->fontSize->setValue( f.pointSizeF() );

    ui->textFormats->clear();

    s->beginGroup("colors");
    loadGeneralTextFormats( s );
    s->endGroup(); // colors

    s->beginGroup("highlighting");
    loadSyntaxTextFormats( s );
    s->endGroup(); // highlighting

    s->endGroup(); // IDE/editor

    updateFontPreview();
    updateTextFormatDisplayCommons();

    ui->textFormats->expandAll();
}

void EditorPage::loadGeneralTextFormats( Manager *settings )
{
    mGeneralFormatsItem = new QTreeWidgetItem( ui->textFormats );
    mGeneralFormatsItem->setText(0, "General" );

    // commont text format item is special - don't set foreground and background on the item!
    mCommonTextFormatItem = new QTreeWidgetItem();
    mCommonTextFormatItem->setText( 0, "Text" );
    mCommonTextFormatItem->setData( 0, TextFormatConfigKeyRole, "text" );
    mCommonTextFormatItem->setData( 0, TextFormatRole, settings->value( "text" ) );
    mGeneralFormatsItem->addChild( mCommonTextFormatItem );

    static char const * const keys[] = {
        "matchingBrackets", "evaluatedCode", "lineNumbers"
    };

    static char const * const strings[] = {
        "Matching Brackets", "Evaluated Code", "Line Numbers"
    };

    static int count = sizeof(keys) / sizeof(keys[0]);

    for (int idx = 0; idx < count; ++idx) {
        QTextCharFormat format = settings->value( keys[idx] ).value<QTextCharFormat>();
        addTextFormat( mGeneralFormatsItem, strings[idx], keys[idx], format );
    }
}

void EditorPage::loadSyntaxTextFormats( Manager *settings )
{
    mSyntaxFormatsItem = new QTreeWidgetItem( ui->textFormats );
    mSyntaxFormatsItem->setText(0, "Syntax Highlighting" );

    static char const * const keys[] = {
        "keyword", "built-in", "env-var", "class", "number",
        "symbol", "string", "char", "comment", "primitive"
    };

    static char const * const strings[] = {
        "Keyword", "Built-in Value", "Environment Variable", "Class",
        "Number", "Symbol", "String", "Char", "Comment", "Primitive"
    };

    static int count = sizeof(keys) / sizeof(keys[0]);

    for (int idx = 0; idx < count; ++idx) {
        QTextCharFormat format = settings->value( keys[idx] ).value<QTextCharFormat>();
        addTextFormat( mSyntaxFormatsItem, strings[idx], keys[idx], format );
    }
}

void EditorPage::store( Manager *s )
{
    s->beginGroup("IDE/editor");

    s->setValue("spaceIndent", ui->spaceIndent->isChecked());
    s->setValue("indentWidth", ui->indentWidth->value());
    s->setValue("stepForwardEvaluation", ui->stepForwardEvaluation->isChecked());

    s->setValue("blinkDuration", ui->blinkDuration->value());
    s->setValue("postWindowScrollback", ui->postWindowScrollback->value());

    QFont f = constructFont();
    s->setValue("font", f.toString());

    s->beginGroup("colors");

    int generalFormatCount = mGeneralFormatsItem->childCount();
    for(int i = 0; i < generalFormatCount; ++i)
    {
        QTreeWidgetItem *item = mGeneralFormatsItem->child(i);
        QTextCharFormat fm = item->data( 0, TextFormatRole).value<QTextCharFormat>();
        QString key = item->data( 0, TextFormatConfigKeyRole ).toString();
        s->setValue( key, QVariant::fromValue<QTextCharFormat>(fm) );
    }

    s->endGroup(); // colors

    s->beginGroup("highlighting");

    int syntaxFormatCount = mSyntaxFormatsItem->childCount();
    for(int i = 0; i < syntaxFormatCount; ++i)
    {
        QTreeWidgetItem *item =mSyntaxFormatsItem->child(i);
        QTextCharFormat fm = item->data( 0, TextFormatRole).value<QTextCharFormat>();
        QString key = item->data( 0, TextFormatConfigKeyRole ).toString();
        s->setValue( key, QVariant::fromValue<QTextCharFormat>(fm) );
    }

    s->endGroup(); // highlighting

    s->endGroup();
}

void EditorPage::onCurrentTabChanged(int)
{
    if (ui->tabs->currentWidget() == ui->colorsTab)
        updateFontPreview();
}

void EditorPage::onFontFamilyChanged(int idx)
{
    if(idx < 0) return;

    Q_ASSERT(fontDatabase);

    ui->fontStyle->clear();

    QString fontFamily = ui->fontList->item(idx)->text();
    QStringList styles = fontDatabase->styles(fontFamily);
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
    if (ui->tabs->currentWidget() == ui->colorsTab)
        ui->textFormats->setFont( constructFont() );
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

void EditorPage::addTextFormat
( QTreeWidgetItem * parent, const QString & name, const QString &key, const QTextCharFormat & format )
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText( 0, name );
    item->setData( 0, TextFormatConfigKeyRole, key );
    setTextFormat( item, format );

    if (!parent)
        parent = ui->textFormats->invisibleRootItem();

    parent->addChild(item);
}

void EditorPage::setTextFormat( QTreeWidgetItem *item, const QTextCharFormat & format )
{
    QBrush fg = format.foreground();
    if ( fg != Qt::NoBrush)
        item->setForeground( 0, fg );
    else
        item->setData( 0, Qt::ForegroundRole, QVariant() );

    QBrush bg = format.background();
    if ( bg != Qt::NoBrush)
        item->setBackground( 0, bg );
    else
        item->setData( 0, Qt::BackgroundRole, QVariant() );

    QFont f;
    if (format.hasProperty(QTextFormat::FontItalic))
        f.setItalic( format.fontItalic() );
    if (format.hasProperty(QTextFormat::FontWeight))
        f.setWeight( format.fontWeight() );

    item->setFont( 0, f );

    item->setData( 0, TextFormatRole, QVariant::fromValue(format) );
}

QTextCharFormat EditorPage::constructTextFormat()
{
    QTextCharFormat format;

    QBrush fg = ui->fgPicker->brush();
    if ( fg.style() != Qt::NoBrush)
        format.setForeground(fg);

    QBrush bg = ui->bgPicker->brush();
    if (bg.style() != Qt::NoBrush)
        format.setBackground(bg);

    if (ui->italicOption->isChecked())
        format.setFontItalic(true);

    if (ui->boldOption->isChecked())
        format.setFontWeight(QFont::Bold);

    return format;
}

void EditorPage::updateTextFormatEdit()
{
    QTreeWidgetItem *item = ui->textFormats->currentItem();
    bool canEdit = item && item->data(0, TextFormatConfigKeyRole).isValid();
    ui->textFormatEdit->setEnabled(canEdit);

    if (!canEdit) {
        ui->fgPicker->setBrush( QBrush() );
        ui->bgPicker->setBrush( QBrush() );
        ui->italicOption->setChecked(false);
        ui->italicOption->setChecked(false);
    }
    else {
        QTextCharFormat format = item->data( 0, TextFormatRole).value<QTextCharFormat>();
        ui->fgPicker->setBrush( format.foreground() );
        ui->bgPicker->setBrush( format.background() );
        ui->italicOption->setChecked( format.fontItalic() );
        ui->boldOption->setChecked( format.fontWeight() == QFont::Bold );
    }
}

void EditorPage::updateTextFormatDisplay()
{
    QTreeWidgetItem *item = ui->textFormats->currentItem();
    bool canEdit = item && item->data(0, TextFormatConfigKeyRole).isValid();
    if (!canEdit)
        return;

    QTextCharFormat format = constructTextFormat();

    if (item != mCommonTextFormatItem)
        setTextFormat( item, format );
    else {
        // treat common text format specially
        item->setData( 0, TextFormatRole, QVariant::fromValue(format) );
        updateTextFormatDisplayCommons();
    }
}

void EditorPage::updateTextFormatDisplayCommons()
{
    QTextCharFormat commonFormat =
            mCommonTextFormatItem->data(0, TextFormatRole).value<QTextCharFormat>();

    QPalette palette;

    QBrush fg = commonFormat.foreground();
    if ( fg != Qt::NoBrush)
        palette.setBrush( QPalette::Text, fg );

    QBrush bg = commonFormat.background();
    if (bg != Qt::NoBrush)
        palette.setBrush( QPalette::Base, bg );

    setPalette(palette);
}

}} // namespace ScIDE::Settings
