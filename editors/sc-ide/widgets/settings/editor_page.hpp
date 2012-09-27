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

#ifndef SCIDE_WIDGETS_SETTINGS_EDITOR_PAGE_HPP_INCLUDED
#define SCIDE_WIDGETS_SETTINGS_EDITOR_PAGE_HPP_INCLUDED

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextCharFormat>

class QFontDatabase;

namespace Ui {
    class EditorConfigPage;
}

namespace ScIDE { namespace Settings {

class Manager;

class EditorPage : public QWidget
{
    Q_OBJECT

public:
    EditorPage(QWidget *parent = 0);
    ~EditorPage();

public Q_SLOTS:
    void load( Manager * );
    void store( Manager * );

private Q_SLOTS:
    void onCurrentTabChanged(int);

    void onMonospaceToggle(bool);
    void updateFontPreview();

    void updateTextFormatEdit();
    void updateTextFormatDisplay();
    void updateTextFormatDisplayCommons();

private:
    enum TextFormatListRole {
        TextFormatConfigKeyRole = Qt::UserRole,
        TextFormatRole,
        DefaultTextFormatRole
    };

    void loadGeneralTextFormats( Manager * );
    void loadSyntaxTextFormats( Manager * );

    void populateFontList( bool onlyMonospaced = false );

    QFont constructFont();
    QTextCharFormat constructTextFormat();

    QTreeWidgetItem * addTextFormat
    ( QTreeWidgetItem * parent, const QString & name, const QString &key,
      const QTextCharFormat & format, const QTextCharFormat & defaultFormat = QTextCharFormat() );

    void updateTextFormatDisplay( QTreeWidgetItem * );

    QFontDatabase *fontDatabase;

    Ui::EditorConfigPage *ui;

    QStringList mFormatKeys;
    QTreeWidgetItem *mGeneralFormatsItem;
    QTreeWidgetItem *mSyntaxFormatsItem;
    QTreeWidgetItem *mCommonTextFormatItem;
};

}} // namespace ScIDE::Settings

#endif // SCIDE_WIDGETS_SETTINGS_EDITOR_PAGE_HPP_INCLUDED
