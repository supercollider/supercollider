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
#include "../../core/settings/theme.hpp"
#include "../../core/main.hpp"

#include <QMenu>
#include <QDebug>
#include <QListWidgetItem>
#include <QFontDatabase>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>

namespace ScIDE { namespace Settings {

EditorPage::EditorPage(QWidget *parent) :
    QWidget(parent),
    fontDatabase(new QFontDatabase),
    ui( new Ui::EditorConfigPage )
{
    ui->setupUi(this);

    connect( ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)) );

    connect( ui->onlyMonoFonts, SIGNAL(toggled(bool)), this, SLOT(onMonospaceToggle(bool)) );
    connect( ui->fontCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateFontPreview()) );
    connect( ui->fontSize, SIGNAL(valueChanged(int)), this, SLOT(updateFontPreview()) );
    connect( ui->fontAntialias, SIGNAL(stateChanged(int)), this, SLOT(updateFontPreview()) );

    connect(ui->themeCombo, SIGNAL(currentIndexChanged(QString)),
             this, SLOT(updateTheme(QString)) );
    connect(ui->themeCopyBtn, SIGNAL(clicked()), this, SLOT(dialogCopyTheme()));
    connect(ui->themeDeleteBtn, SIGNAL(clicked()), this, SLOT(deleteTheme()));
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
    qDeleteAll(mThemes);
}

void EditorPage::load( Manager *s )
{
    s->beginGroup("IDE/editor");

    ui->spaceIndent->setChecked( s->value("spaceIndent").toBool() );
    ui->indentWidth->setValue( s->value("indentWidth").toInt() );
    ui->stepForwardEvaluation->setChecked( s->value("stepForwardEvaluation").toBool() );
    ui->editorLineWrap->setChecked( s->value("lineWrap").toBool() );
    ui->disableBlinkingCursor->setChecked( s->value("disableBlinkingCursor").toBool() );
    ui->insertMatchingTokens->setChecked( s->value("insertMatchingTokens").toBool() );
    ui->blinkDuration->setValue( s->value("blinkDuration").toInt() );
    ui->highlightCurrentLine->setChecked( s->value("highlightCurrentLine").toBool() );
    ui->highlightBracketContents->setChecked( s->value("highlightBracketContents").toBool() );
    ui->inactiveEditorFadeAlpha->setValue( s->value("inactiveEditorFadeAlpha").toInt() );
    ui->useComboBox->setChecked( s->value("useComboBox").toBool() );
    ui->useComboBoxWhenSplitting->setChecked( s->value("useComboBoxWhenSplitting").toBool() );

    s->beginGroup("font");
    QString fontFamily = s->value("family").toString();
    int fontSize = s->value("size").toInt();
    ui->fontAntialias->setChecked( s->value("antialias").toBool() );

    s->endGroup();

    // Display info about the font that would actually be used:
    QFont defaultFont = QApplication::font("QPlainTextEdit");
    defaultFont.setFamily(fontFamily);
    if (fontSize > 0)
        defaultFont.setPointSize(fontSize);
    defaultFont.setStyleHint( QFont::TypeWriter );

    QFontInfo fontInfo( defaultFont );
    fontFamily = fontInfo.family();
    fontSize = fontInfo.pointSize();

    populateFontList( ui->onlyMonoFonts->isChecked() );
    int fontFamilyIndex = ui->fontCombo->findText( fontFamily, Qt::MatchFixedString );
    ui->fontCombo->setCurrentIndex( fontFamilyIndex );

    ui->fontSize->setValue( fontSize );

    ui->textFormats->clear();

    s->endGroup(); // IDE/editor

    QString themeName = s->value("IDE/editor/theme").toString();
    updateTheme(themeName);
    populateThemeList(themeName);

    s->beginGroup("IDE/postWindow");
    ui->postWindowScrollback->setValue( s->value("scrollback").toInt() );
    ui->postWindowLineWrap->setChecked( s->value("lineWrap").toBool() );
    s->endGroup();

    updateFontPreview();
    updateTextFormatDisplayCommons();

    ui->textFormats->expandAll();
}

void EditorPage::updateTheme(QString  name)
{
    Theme *theme;
    QMap<QString, Theme *>::const_iterator i = mThemes.find(name);

    if (i == mThemes.end()) {
        theme = new Theme(name);
        mThemes.insert(name, theme);
    } else {
        theme = i.value();
    }

    loadThemeFormats(*theme);

    ui->bgPicker->setDisabled(theme->locked());
    ui->fgPicker->setDisabled(theme->locked());
    ui->bgClearBtn->setDisabled(theme->locked());
    ui->fgClearBtn->setDisabled(theme->locked());
    ui->themeDeleteBtn->setDisabled(theme->locked());
    ui->italicOption->setDisabled(theme->locked());
    ui->boldOption->setDisabled(theme->locked());
}

void EditorPage::loadThemeFormats(Theme & theme)
{
    ui->textFormats->clear();

    // common text format item is special - don't set foreground and background on the item!
    mCommonTextFormatItem = new QTreeWidgetItem(ui->textFormats);
    mCommonTextFormatItem->setText( 0, tr("Text") );
    mCommonTextFormatItem->setData( 0, TextFormatConfigKeyRole, "text" );
    mCommonTextFormatItem->setData( 0, TextFormatRole, QVariant::fromValue(theme.format("text")) );
    updateTextFormatDisplayCommons();

    static char const * const keys[] = {
        "currentLine", "searchResult", "matchingBrackets", "mismatchedBrackets",
        "evaluatedCode", "lineNumbers", "selection", "postwindowtext",
        "whitespace", "keyword", "built-in", "env-var", "class", "number",
        "symbol", "string", "char", "comment", "primitive",
        "postwindowerror", "postwindowwarning", "postwindowsuccess", "postwindowemphasis"
    };

    static QStringList strings = QStringList()
            << tr("Current Line") << tr("Search Result") << tr("Matching Brackets")
            << tr("Mismatched Brackets") << tr("Evaluated Code")
            << tr("Line Numbers") << tr("Selected Text") << tr("Post Window Text")
            << tr("Whitespace")
            << tr("Keyword") << tr("Built-in Value") << tr("Environment Variable")
            << tr("Class") << tr("Number") << tr("Symbol") << tr("String") << tr("Char")
            << tr("Comment") << tr("Primitive")
            << tr("Post Window Error") << tr("Post Window Warning") << tr("Post Window Success")
            << tr("Post Window Emphasis");

    static int count = strings.count();

    for (int idx = 0; idx < count; ++idx) {
        QTextCharFormat format = theme.format(keys[idx]);
        addTextFormat(strings[idx], keys[idx], format);
    }
}

void EditorPage::populateFontList( bool onlyMonospaced )
{
    ui->fontCombo->clear();
    QStringList fontFamilies = fontDatabase->families();
    foreach(QString family, fontFamilies)
    {
        if (onlyMonospaced && !fontDatabase->isFixedPitch(family))
            continue;

        ui->fontCombo->addItem(family);
    }
}

void EditorPage::populateThemeList(const QString & sel)
{
    /* managing the combo box send parasite signals */
    disconnect( ui->themeCombo, SIGNAL(currentIndexChanged(QString)),
             this, SLOT(updateTheme(QString)) );

    QMap<QString, Theme *>::const_iterator itr = mThemes.begin();
    QList<QString> list =  itr.value()->availableThemes();
    int i = 0;

    foreach(QString th, mThemes.keys()) {
        if (list.indexOf(th) < 0)
                list.append(th);
    }

    ui->themeCombo->clear();
    foreach(QString themeName, list) {
        ui->themeCombo->addItem(themeName);
        if (sel == themeName)
                ui->themeCombo->setCurrentIndex(i);
        else
            i++;
    }

    connect( ui->themeCombo, SIGNAL(currentIndexChanged(QString)),
             this, SLOT(updateTheme(QString)) );
}

void EditorPage::store( Manager *s )
{
    s->beginGroup("IDE/editor");

    s->setValue("spaceIndent", ui->spaceIndent->isChecked());
    s->setValue("indentWidth", ui->indentWidth->value());
    s->setValue("stepForwardEvaluation", ui->stepForwardEvaluation->isChecked());
    s->setValue("lineWrap", ui->editorLineWrap->isChecked());
    s->setValue("disableBlinkingCursor", ui->disableBlinkingCursor->isChecked());
    s->setValue("insertMatchingTokens", ui->insertMatchingTokens->isChecked());
    s->setValue("highlightCurrentLine", ui->highlightCurrentLine->isChecked());
    s->setValue("highlightBracketContents", ui->highlightBracketContents->isChecked());
    s->setValue("inactiveEditorFadeAlpha", ui->inactiveEditorFadeAlpha->value());
    s->setValue("useComboBox", ui->useComboBox->isChecked());
    s->setValue("useComboBoxWhenSplitting", ui->useComboBoxWhenSplitting->isChecked());

    s->setValue("blinkDuration", ui->blinkDuration->value());

    s->beginGroup("font");
    QString fontFamily = ui->fontCombo->currentText();
    if (!fontFamily.isEmpty())
        s->setValue("family", fontFamily);
    s->setValue("size", ui->fontSize->value());
    s->setValue("antialias", ui->fontAntialias->checkState());
    s->endGroup();

    s->setValue("theme", ui->themeCombo->currentText());

    s->endGroup();

    s->beginGroup("IDE/postWindow");
    s->setValue("scrollback", ui->postWindowScrollback->value());
    s->setValue("lineWrap", ui->postWindowLineWrap->isChecked());
    s->endGroup();

    foreach(Theme *theme, mThemes) {
        if (ui->themeCombo->findText(theme->name()) > 0)
            theme->save();
        else
            theme->remove();
    }
    s->updateTheme();
}

void EditorPage::onCurrentTabChanged(int)
{
    if (ui->tabs->currentWidget() == ui->colorsTab)
        updateFontPreview();
}

void EditorPage::onMonospaceToggle(bool onlyMonospaced)
{
    QString fontFamily = ui->fontCombo->currentText();

    bool signals_blocked = ui->fontCombo->blockSignals(true);

    populateFontList( onlyMonospaced );

    if (!fontFamily.isEmpty()) {
        int fontFamilyIndex = ui->fontCombo->findText( fontFamily, Qt::MatchFixedString );
        if (fontFamilyIndex == -1)
            fontFamilyIndex = 0;
        ui->fontCombo->setCurrentIndex( fontFamilyIndex );
    }

    ui->fontCombo->blockSignals(signals_blocked);

    updateFontPreview();
}

void EditorPage::updateFontPreview()
{
    if (ui->tabs->currentWidget() == ui->colorsTab)
        ui->textFormats->setFont( constructFont() );
}

QFont EditorPage::constructFont()
{
    QString family = ui->fontCombo->currentText();
    int size = ui->fontSize->value();
    bool antialias = (ui->fontAntialias->checkState() == Qt::Checked);

    QFont font(family, size);
    font.setStyleHint( QFont::TypeWriter );

    if (!antialias)
        font.setStyleStrategy(QFont::StyleStrategy(font.styleStrategy() | QFont::NoAntialias));

    return font;
}

QTreeWidgetItem * EditorPage::addTextFormat
( const QString & name, const QString &key,
  const QTextCharFormat & format, const QTextCharFormat &defaultFormat )
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->textFormats);
    item->setText( 0, name );
    item->setData( 0, TextFormatConfigKeyRole, key );
    item->setData( 0, TextFormatRole, QVariant::fromValue(format) );
    item->setData( 0, DefaultTextFormatRole, QVariant::fromValue(defaultFormat) );
    updateTextFormatDisplay( item );

    return item;
}

void EditorPage::updateTextFormatDisplay( QTreeWidgetItem *item )
{
    QTextCharFormat format = item->data( 0, DefaultTextFormatRole ).value<QTextCharFormat>();
    format.merge( item->data( 0, TextFormatRole ).value<QTextCharFormat>() );

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
}

QTextCharFormat EditorPage::constructTextFormat()
{
    QTextCharFormat format;

    QBrush fg = ui->fgPicker->brush();
    if (ui->fgPicker->isEnabled() && fg.style() != Qt::NoBrush)
        format.setForeground(fg);

    QBrush bg = ui->bgPicker->brush();
    if (ui->bgPicker->isEnabled() && bg.style() != Qt::NoBrush)
        format.setBackground(bg);

    if (ui->italicOption->isEnabled() && ui->italicOption->isChecked())
        format.setFontItalic(true);

    if (ui->boldOption->isEnabled() && ui->boldOption->isChecked())
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
        int enable = true;

        ui->fgPicker->setBrush( format.foreground() );
        ui->bgPicker->setBrush( format.background() );
        ui->italicOption->setChecked( format.fontItalic() );
        ui->boldOption->setChecked( format.fontWeight() == QFont::Bold );

        if (item->data(0, TextFormatConfigKeyRole).toString() == "currentLine")
            enable = false;

        ui->fgPicker->setEnabled(enable);
        ui->italicOption->setEnabled(enable);
        ui->boldOption->setEnabled(enable);
        ui->fgClearBtn->setEnabled(enable);
    }
}

void EditorPage::updateTextFormatDisplay()
{
    QTreeWidgetItem *item = ui->textFormats->currentItem();
    bool canEdit = item && item->data(0, TextFormatConfigKeyRole).isValid();
    if (!canEdit)
        return;

    QTextCharFormat format = constructTextFormat();
    item->setData( 0, TextFormatRole, QVariant::fromValue(format) );

    if (item != mCommonTextFormatItem)
        updateTextFormatDisplay( item );
    else
        updateTextFormatDisplayCommons();

    Theme *theme = mThemes.value(ui->themeCombo->currentText());
    theme->setFormat(item->data(0, TextFormatConfigKeyRole).toString(), format);
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

    ui->textFormats->setPalette(palette);
}

void EditorPage::dialogCopyTheme()
{
    bool ok;
    QString themeName = ui->themeCombo->currentText();
    QString newThemeName = QInputDialog::getText
        (this, tr("Copy theme"),
         tr("copy the selected theme:"), QLineEdit::Normal,
         themeName, &ok);

   if ( ok && !newThemeName.isEmpty() ) {
        QMap<QString, Theme *>::const_iterator i = mThemes.begin();
        QList<QString> themes = i.value()->availableThemes();

        if (themes.indexOf(newThemeName)  >= 0) {
            QMessageBox::information (this,
                 tr("Theme Already Existing"),
                 tr("This theme already exists, pick another one\n"));
        } else {
            Theme * theme = new Theme(newThemeName, themeName);
            mThemes.insert(newThemeName, theme);
            ui->themeCombo->addItem(newThemeName);
            ui->themeCombo->setCurrentIndex(ui->themeCombo->findText(newThemeName));
            updateTheme(newThemeName);
        }
    }
}

void EditorPage::deleteTheme()
{
    ui->themeCombo->removeItem(ui->themeCombo->currentIndex());
}

}} // namespace ScIDE::Settings
