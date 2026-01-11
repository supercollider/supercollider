#pragma once

#include <QToolBar>

class QFontComboBox;
class QSpinBox;
class QToolButton;

namespace ScIDE {

class RichTextEditor;

class RichTextToolbar : public QToolBar {
    Q_OBJECT

public:
    RichTextToolbar(RichTextEditor* editor, QWidget* parent = nullptr, QFont defaultFont = {});
    void setStartFont(const QFont& font);

    QSize sizeHint() const override;

private slots:
    void onFontFamilyChanged(const QFont& font);
    void onFontSizeChanged(int size);
    void onBoldClicked();
    void onItalicClicked();
    void onUnderlineClicked();
    void onStrikeOutClicked();
    void onColorButtonClicked();
    void onCodeFormatClicked();
    void updateFromSelection();

private:
    void updateColorButtonIcon(const QColor& color);

    RichTextEditor* mEditor;
    QFontComboBox* mFontCombo;
    QSpinBox* mSizeSpinBox;
    QToolButton* mBoldButton;
    QToolButton* mItalicButton;
    QToolButton* mUnderlineButton;
    QToolButton* mStrikeOutButton;
    QToolButton* mColorButton;
    QToolButton* mCodeButton;
    QColor mCurrentColor;
};

} // namespace ScIDE
