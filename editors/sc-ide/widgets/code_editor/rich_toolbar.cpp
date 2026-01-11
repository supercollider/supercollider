#include "rich_toolbar.hpp"
#include "rich_editor.hpp"

#include <QColorDialog>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSpinBox>
#include <QTextCursor>
#include <QToolButton>

namespace ScIDE {

RichTextToolbar::RichTextToolbar(RichTextEditor* editor, QWidget* parent, QFont defaultFont):
    QToolBar(parent),
    mEditor(editor),
    mCurrentColor(Qt::black) {
    setMovable(false);
    setFloatable(false);
    setIconSize(QSize(16, 16));

    // Font family combo box
    mFontCombo = new QFontComboBox(this);
    mFontCombo->setToolTip(tr("Font Family"));
    mFontCombo->setMaximumWidth(250);
    mFontCombo->setCurrentFont(defaultFont);
    addWidget(mFontCombo);

    addSeparator();

    // Font size spin box
    QLabel* sizeLabel = new QLabel(tr("Size:"), this);
    addWidget(sizeLabel);

    mSizeSpinBox = new QSpinBox(this);
    mSizeSpinBox->setToolTip(tr("Font Size"));
    mSizeSpinBox->setRange(4, 180);
    mSizeSpinBox->setValue(12);
    mSizeSpinBox->setMaximumWidth(100);
    addWidget(mSizeSpinBox);

    addSeparator();

    // Color button
    mColorButton = new QToolButton(this);
    mColorButton->setToolTip(tr("Text Color"));
    updateColorButtonIcon(mCurrentColor);
    addWidget(mColorButton);

    addSeparator();

    // Bold button
    mBoldButton = new QToolButton(this);
    mBoldButton->setText(tr("B"));
    mBoldButton->setToolTip(tr("Bold"));
    mBoldButton->setCheckable(true);
    mBoldButton->setFont(QFont(mBoldButton->font().family(), 14, QFont::Bold));
    addWidget(mBoldButton);

    // Italic button
    mItalicButton = new QToolButton(this);
    mItalicButton->setText(tr("I"));
    mItalicButton->setToolTip(tr("Italic"));
    mItalicButton->setCheckable(true);
    QFont italicFont = mItalicButton->font();
    italicFont.setItalic(true);
    italicFont.setPointSize(14);
    mItalicButton->setFont(italicFont);
    addWidget(mItalicButton);

    // Underline button
    mUnderlineButton = new QToolButton(this);
    mUnderlineButton->setText(tr("U"));
    mUnderlineButton->setToolTip(tr("Underline"));
    mUnderlineButton->setCheckable(true);
    QFont underlineFont = mUnderlineButton->font();
    underlineFont.setUnderline(true);
    underlineFont.setPointSize(14);
    mUnderlineButton->setFont(underlineFont);
    addWidget(mUnderlineButton);

    // Strikeout button
    mStrikeOutButton = new QToolButton(this);
    mStrikeOutButton->setText(tr("S"));
    mStrikeOutButton->setToolTip(tr("Strikeout"));
    mStrikeOutButton->setCheckable(true);
    QFont strikeFont = mStrikeOutButton->font();
    strikeFont.setStrikeOut(true);
    strikeFont.setPointSize(14);
    mStrikeOutButton->setFont(strikeFont);
    addWidget(mStrikeOutButton);

    addSeparator();

    // Code format button
    mCodeButton = new QToolButton(this);
    mCodeButton->setText(tr("Code"));
    mCodeButton->setToolTip(tr("Apply Code Format"));
    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSize(14);
    mCodeButton->setFont(codeFont);
    addWidget(mCodeButton);

    // Connect signals
    connect(mFontCombo, &QFontComboBox::currentFontChanged, this, &RichTextToolbar::onFontFamilyChanged);

    connect(mSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &RichTextToolbar::onFontSizeChanged);

    connect(mBoldButton, &QToolButton::clicked, this, &RichTextToolbar::onBoldClicked);

    connect(mItalicButton, &QToolButton::clicked, this, &RichTextToolbar::onItalicClicked);

    connect(mUnderlineButton, &QToolButton::clicked, this, &RichTextToolbar::onUnderlineClicked);

    connect(mStrikeOutButton, &QToolButton::clicked, this, &RichTextToolbar::onStrikeOutClicked);

    connect(mColorButton, &QToolButton::clicked, this, &RichTextToolbar::onColorButtonClicked);

    connect(mCodeButton, &QToolButton::clicked, this, &RichTextToolbar::onCodeFormatClicked);

    // Update toolbar state when selection changes
    connect(mEditor, &QTextEdit::cursorPositionChanged, this, &RichTextToolbar::updateFromSelection);

    // Initialize toolbar state from current selection
    updateFromSelection();
}

QSize RichTextToolbar::sizeHint() const {
    QSize hint = QToolBar::sizeHint();
    hint.setHeight(qMin(hint.height(), 32));
    return hint;
}

void RichTextToolbar::onFontFamilyChanged(const QFont& font) {
    mEditor->setSelectionFont(font);
    mEditor->setFocus();
}

void RichTextToolbar::onFontSizeChanged(int size) {
    mEditor->setSelectionFontSize(size);
    mEditor->setFocus();
}

void RichTextToolbar::onBoldClicked() {
    mEditor->toggleBold();
    mEditor->setFocus();
}

void RichTextToolbar::onItalicClicked() {
    mEditor->toggleItalic();
    mEditor->setFocus();
}

void RichTextToolbar::onUnderlineClicked() {
    mEditor->toggleUnderline();
    mEditor->setFocus();
}

void RichTextToolbar::onStrikeOutClicked() {
    mEditor->toggleStrikeOut();
    mEditor->setFocus();
}

void RichTextToolbar::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(mCurrentColor, this, tr("Select Text Color"));
    if (color.isValid()) {
        mCurrentColor = color;
        updateColorButtonIcon(color);
        mEditor->setSelectionColor(color);
    }
    mEditor->setFocus();
}

void RichTextToolbar::onCodeFormatClicked() {
    mEditor->applyCodeFormat();
    mEditor->setFocus();
}

void RichTextToolbar::updateFromSelection() {
    QTextCursor cursor = mEditor->textCursor();
    QTextCharFormat format = cursor.charFormat();

    // Block signals while updating to prevent feedback loops
    mFontCombo->blockSignals(true);
    mSizeSpinBox->blockSignals(true);
    mBoldButton->blockSignals(true);
    mItalicButton->blockSignals(true);
    mUnderlineButton->blockSignals(true);
    mStrikeOutButton->blockSignals(true);

    // Update font family
    QString fontFamily = format.fontFamilies().toStringList().value(0, mFontCombo->currentFont().family());
    if (!fontFamily.isEmpty()) {
        mFontCombo->setCurrentFont(QFont(fontFamily));
    }

    // Update font size
    qreal pointSize = format.fontPointSize();
    if (pointSize > 0) {
        mSizeSpinBox->setValue(static_cast<int>(pointSize));
    }

    // Update bold/italic/underline/strikeout buttons
    mBoldButton->setChecked(format.fontWeight() >= QFont::Bold);
    mItalicButton->setChecked(format.fontItalic());
    mUnderlineButton->setChecked(format.fontUnderline());
    mStrikeOutButton->setChecked(format.fontStrikeOut());

    // Update color
    QColor textColor = format.foreground().color();
    if (textColor.isValid()) {
        mCurrentColor = textColor;
        updateColorButtonIcon(textColor);
    }

    mFontCombo->blockSignals(false);
    mSizeSpinBox->blockSignals(false);
    mBoldButton->blockSignals(false);
    mItalicButton->blockSignals(false);
    mUnderlineButton->blockSignals(false);
    mStrikeOutButton->blockSignals(false);
}

void RichTextToolbar::updateColorButtonIcon(const QColor& color) {
    QPixmap pixmap(28, 28);
    pixmap.fill(color);
    mColorButton->setIcon(QIcon(pixmap));
}

} // namespace ScIDE
