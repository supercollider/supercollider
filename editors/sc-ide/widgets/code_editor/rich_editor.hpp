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

#pragma once

#include <qgesture.h>
#include <QTextEdit>
#include <QGraphicsScene>

namespace ScIDE {

namespace Settings {
class Manager;
}

class Document;
class RichTextToolbar;

class RichTextEditor : public QTextEdit {
    Q_OBJECT

public:
    RichTextEditor(Document* doc, QWidget* parent = nullptr);
    ~RichTextEditor();

    // Rule of 5 - delete copy/move operations
    RichTextEditor(const RichTextEditor& other) = delete;
    RichTextEditor& operator=(const RichTextEditor& other) = delete;
    RichTextEditor(RichTextEditor&& other) = delete;
    RichTextEditor& operator=(RichTextEditor&& other) = delete;

    Document* document() { return mDoc; }
    QTextDocument* textDocument() { return QTextEdit::document(); }

    void showPosition(int charPosition, int selectionLength = 0);
    QString symbolUnderCursor();

    // Lookup methods (invoked by MainWindow via meta-object)
    Q_INVOKABLE bool openDocumentation();
    Q_INVOKABLE void openDefinition();
    Q_INVOKABLE void openCommandLine();
    Q_INVOKABLE void findReferences();

    RichTextToolbar* toolbar() { return mToolbar; }

    void blinkCode(const QTextCursor& c);

public slots:
    void applySettings(Settings::Manager*);
    void setActiveAppearance(bool active);

    // Code execution
    void evaluateLine();
    void evaluateRegion();
    void evaluateDocument();

    // Formatting
    void applyCodeFormat();
    void setSelectionFont(const QFont& font);
    void setSelectionFontSize(int size);
    void setSelectionColor(const QColor& color);
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void toggleStrikeOut();

    void closeDocument();
    void updateDocLastSelection();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupToolbar();

    Document* const mDoc;
    Settings::Manager* mSettings;
    RichTextToolbar* mToolbar;
    QGraphicsScene* mOverlay;
    QWidget* mOverlayWidget;

    int mBlinkDuration;
    bool mEditorBoxIsActive;
    int mInactiveFadeAlpha;
};

} // namespace ScIDE
