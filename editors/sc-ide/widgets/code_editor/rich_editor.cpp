#include "rich_editor.hpp"
#include "rich_toolbar.hpp"
#include "tokens.hpp"
#include "../../core/main.hpp"
#include "../../core/doc_manager.hpp"
#include "../../core/sc_lexer.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/settings/theme.hpp"

#include <QApplication>
#include <QDebug>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>
#include <QVBoxLayout>

#include "help_browser.hpp"
#include "main_window.hpp"
#include "sc_editor.hpp"

namespace ScIDE {

/*! @class RichTextEditor
 *  @brief This is based on ScCodeEditor and GenericCodeEditor
 *
 *  @discussion There are some differences to the GenericCodeEditor:
 *  * Bracket/Region based evaluation is not implemented yet. The necessary code can be found at
 *    `ScCodeEditor::evaluateRegion`, but it seems better to properly move this into a dedicated
 *    independent implementation so it can be re-used by both editors.
 *  * Since the pinch zooming on the GenericCodeEditor simply re-scales the font size of its text,
 *  we would need a different approach here, therefore this feature is not implemented.
 *  * The line indicator (mLineIndicator) is not implemented here.
 *  * Find/replace/go-to widgets are not implemented
 */
RichTextEditor::RichTextEditor(Document* doc, QWidget* parent):
    QTextEdit(parent),
    mDoc(doc),
    mSettings(Main::settings()),
    mToolbar(nullptr),
    mOverlay(nullptr),
    mOverlayWidget(nullptr),
    mBlinkDuration(600),
    mEditorBoxIsActive(false),
    mInactiveFadeAlpha(0) {
    Q_ASSERT(mDoc != nullptr);

    setFrameShape(QFrame::NoFrame);

    // Set up the overlay for blink animation
    mOverlay = new QGraphicsScene(this);

    QPalette overlayPalette;
    overlayPalette.setBrush(QPalette::Base, Qt::NoBrush);

    QGraphicsView* overlayView = new QGraphicsView(mOverlay, this);
    overlayView->setFrameShape(QFrame::NoFrame);
    overlayView->setPalette(overlayPalette);
    overlayView->setFocusPolicy(Qt::NoFocus);
    overlayView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    overlayView->setSceneRect(QRectF(0, 0, 1, 1));
    overlayView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    mOverlayWidget = overlayView;

    // Create and set up toolbar
    setupToolbar();

    // Connect to document signals
    connect(mDoc, &Document::defaultFontChanged, this, [this]() { setFont(mDoc->defaultFont()); });
    connect(this, &QTextEdit::selectionChanged, this, &RichTextEditor::updateDocLastSelection);
    connect(Main::instance(), &Main::applySettingsRequest, this, &RichTextEditor::applySettings);

    QTextEdit::setDocument(doc->textDocument());

    applySettings(mSettings);
}

RichTextEditor::~RichTextEditor() {}

void RichTextEditor::setupToolbar() {
    mToolbar = new RichTextToolbar(this, this, mSettings->codeFont());
    // Toolbar height will be used to set viewport margins
    int toolbarHeight = mToolbar->sizeHint().height();
    setViewportMargins(0, toolbarHeight, 0, 0);
    mToolbar->move(0, 0);
}

/*! This only resizes the toolbar currently, not the document */
void RichTextEditor::resizeEvent(QResizeEvent* event) {
    QTextEdit::resizeEvent(event);

    // Resize the toolbar to match editor width
    if (mToolbar) {
        mToolbar->resize(width(), mToolbar->sizeHint().height());
    }

    // Resize the overlay widget to match the viewport
    if (mOverlayWidget) {
        mOverlayWidget->resize(viewport()->size());
        mOverlayWidget->move(viewport()->pos());
    }
}

void RichTextEditor::applySettings(Settings::Manager* settings) {
    settings->beginGroup("IDE/editor");

    bool lineWrap = settings->value("lineWrap").toBool();
    mInactiveFadeAlpha = settings->value("inactiveEditorFadeAlpha").toInt();
    mBlinkDuration = settings->value("blinkDuration").toInt();

    QPalette palette;

    const QTextCharFormat* format = &settings->getThemeVal("text");
    QBrush bg = format->background();
    QBrush fg = format->foreground();
    if (bg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::Base, bg);
    if (fg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::Text, fg);

    format = &settings->getThemeVal("selection");
    bg = format->background();
    fg = format->foreground();
    if (bg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::Highlight, bg);
    if (fg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::HighlightedText, fg);

    setPalette(palette);

    setLineWrapMode(lineWrap ? WidgetWidth : NoWrap);

    settings->endGroup();
}

void RichTextEditor::setActiveAppearance(bool active) { mEditorBoxIsActive = active; }

void RichTextEditor::showPosition(int charPosition, int selectionLength) {
    QTextCursor cursor(textDocument());
    cursor.setPosition(charPosition);
    if (selectionLength > 0)
        cursor.setPosition(charPosition + selectionLength, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
    ensureCursorVisible();
}

QString RichTextEditor::symbolUnderCursor() {
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

bool RichTextEditor::openDocumentation() { return Main::openDocumentation(symbolUnderCursor()); }

void RichTextEditor::openDefinition() { Main::openDefinition(symbolUnderCursor(), this); }

void RichTextEditor::openCommandLine() { Main::openCommandLine(symbolUnderCursor()); }

void RichTextEditor::findReferences() { Main::findReferences(symbolUnderCursor(), this); }

void RichTextEditor::evaluateLine() {
#ifdef SC_USE_QTWEBENGINE
    HelpBrowserDocklet* help = MainWindow::instance()->helpBrowserDocklet();
    if (help && help->browser()->helpBrowserHasFocus()) {
        help->browser()->evaluateSelection(true);
        return; // early return
    }
#endif // SC_USE_QTWEBENGINE

    QString text;

    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        text = cursor.selectedText();
    } else {
        text = cursor.block().text();

        // Adjust cursor for code blinking
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    }

    if (text.isEmpty())
        return;

    // Replace paragraph separators with newlines
    text.replace(QChar(0x2029), QChar('\n'));

    Main::evaluateCode(text);
    blinkCode(cursor);
}

void RichTextEditor::evaluateRegion() {
#ifdef SC_USE_QTWEBENGINE
    HelpBrowserDocklet* help = MainWindow::instance()->helpBrowserDocklet();
    if (help && help->browser()->helpBrowserHasFocus()) {
        help->browser()->evaluateSelection(true);
        return; // early return
    }
#endif // SC_USE_QTWEBENGINE

    // In rich text, we don't have bracket-based regions
    // Just evaluate selection or current line
    evaluateLine();
}

void RichTextEditor::evaluateDocument() {
    QString documentText = textDocument()->toPlainText();
    Main::evaluateCode(documentText);
}

void RichTextEditor::blinkCode(const QTextCursor& c) {
    if (!c.document() || !c.hasSelection())
        return;

    // For QTextEdit, we use extra selections to highlight the evaluated code
    Settings::Manager* settings = Main::settings();
    QTextCharFormat evalCodeTextFormat = settings->getThemeVal("evaluatedCode");

    QTextEdit::ExtraSelection selection;
    selection.cursor = c;
    selection.format = evalCodeTextFormat;

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);

    // Use a timer to clear the highlight after the blink duration
    QTimer::singleShot(mBlinkDuration, this, [this]() { setExtraSelections(QList<QTextEdit::ExtraSelection>()); });
}

void RichTextEditor::applyCodeFormat() {
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return;

    QFont codeFont = mSettings->codeFont();

    // Get theme formats
    const QTextCharFormat& textFormat = mSettings->getThemeVal("text");
    const QTextCharFormat& keywordFormat = mSettings->getThemeVal("keyword");
    const QTextCharFormat& builtinFormat = mSettings->getThemeVal("built-in");
    const QTextCharFormat& primitiveFormat = mSettings->getThemeVal("primitive");
    const QTextCharFormat& classFormat = mSettings->getThemeVal("class");
    const QTextCharFormat& numberFormat = mSettings->getThemeVal("number");
    const QTextCharFormat& symbolFormat = mSettings->getThemeVal("symbol");
    const QTextCharFormat& envVarFormat = mSettings->getThemeVal("env-var");
    const QTextCharFormat& stringFormat = mSettings->getThemeVal("string");
    const QTextCharFormat& charFormat = mSettings->getThemeVal("char");
    const QTextCharFormat& commentFormat = mSettings->getThemeVal("comment");

    int selectionStart = cursor.selectionStart();
    QString selectedText = cursor.selectedText();

    // Replace paragraph separators with newlines for lexer
    selectedText.replace(QChar(0x2029), QChar('\n'));

    // First, apply base code font to entire selection
    QTextCharFormat baseFormat;
    baseFormat.setFont(codeFont);
    if (textFormat.foreground().style() != Qt::NoBrush)
        baseFormat.setForeground(textFormat.foreground());
    cursor.mergeCharFormat(baseFormat);

    // Now tokenize and apply syntax highlighting
    ScLexer lexer(selectedText);

    while (lexer.offset() < selectedText.length()) {
        int tokenLength = 0;
        int tokenStart = lexer.offset();
        Token::Type tokenType = lexer.nextToken(tokenLength);

        if (tokenLength == 0)
            break;

        const QTextCharFormat* formatToApply = nullptr;

        switch (tokenType) {
        case Token::Keyword:
            formatToApply = &keywordFormat;
            break;
        case Token::Builtin:
            formatToApply = &builtinFormat;
            break;
        case Token::Primitive:
            formatToApply = &primitiveFormat;
            break;
        case Token::Class:
            formatToApply = &classFormat;
            break;
        case Token::Float:
        case Token::HexInt:
        case Token::ScaleDegreeFloat:
        case Token::RadixFloat:
            formatToApply = &numberFormat;
            break;
        case Token::Symbol:
        case Token::SymbolArg:
        case Token::SymbolMark:
            formatToApply = &symbolFormat;
            break;
        case Token::EnvVar:
            formatToApply = &envVarFormat;
            break;
        case Token::StringMark:
            formatToApply = &stringFormat;
            break;
        case Token::Char:
            formatToApply = &charFormat;
            break;
        case Token::SingleLineComment:
        case Token::MultiLineCommentStart:
        case Token::MultiLineCommentEnd:
            formatToApply = &commentFormat;
            break;
        default:
            break;
        }

        if (formatToApply && formatToApply->foreground().style() != Qt::NoBrush) {
            QTextCursor tokenCursor(textDocument());
            tokenCursor.setPosition(selectionStart + tokenStart);
            tokenCursor.setPosition(selectionStart + tokenStart + tokenLength, QTextCursor::KeepAnchor);

            QTextCharFormat tokenFormat;
            tokenFormat.setForeground(formatToApply->foreground());
            if (formatToApply->fontWeight() != QFont::Normal)
                tokenFormat.setFontWeight(formatToApply->fontWeight());
            if (formatToApply->fontItalic())
                tokenFormat.setFontItalic(true);

            tokenCursor.mergeCharFormat(tokenFormat);
        }
    }

    setTextCursor(cursor);
}

void RichTextEditor::setSelectionFont(const QFont& font) {
    QTextCursor cursor = textCursor();
    // if (!cursor.hasSelection())
    //     return;

    QTextCharFormat format;
    format.setFontFamilies(QStringList() << font.family());
    cursor.mergeCharFormat(format);
    setTextCursor(cursor);
}

void RichTextEditor::setSelectionFontSize(int size) {
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return;

    QTextCharFormat format;
    format.setFontPointSize(size);
    cursor.mergeCharFormat(format);
    setTextCursor(cursor);
}

void RichTextEditor::setSelectionColor(const QColor& color) {
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    cursor.mergeCharFormat(format);
    setTextCursor(cursor);
}

void RichTextEditor::toggleBold() {
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    bool isBold = format.fontWeight() >= QFont::Bold;

    QTextCharFormat newFormat;
    newFormat.setFontWeight(isBold ? QFont::Normal : QFont::Bold);
    cursor.mergeCharFormat(newFormat);
    setTextCursor(cursor);
}

void RichTextEditor::toggleItalic() {
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();

    QTextCharFormat newFormat;
    newFormat.setFontItalic(!format.fontItalic());
    cursor.mergeCharFormat(newFormat);
    setTextCursor(cursor);
}

void RichTextEditor::toggleUnderline() {
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();

    QTextCharFormat newFormat;
    newFormat.setFontUnderline(!format.fontUnderline());
    cursor.mergeCharFormat(newFormat);
    setTextCursor(cursor);
}

void RichTextEditor::toggleStrikeOut() {
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();

    QTextCharFormat newFormat;
    newFormat.setFontStrikeOut(!format.fontStrikeOut());
    cursor.mergeCharFormat(newFormat);
    setTextCursor(cursor);
}


void RichTextEditor::closeDocument() { Main::documentManager()->close(mDoc); }

void RichTextEditor::updateDocLastSelection() {
    QTextCursor cursor = textCursor();
    mDoc->setInitialSelection(cursor.selectionStart(), cursor.selectionEnd() - cursor.selectionStart());
}

} // namespace ScIDE
