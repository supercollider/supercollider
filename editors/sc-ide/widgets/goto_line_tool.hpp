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

#include "code_editor/editor.hpp"
#include "../core/doc_manager.hpp"

#include <QWidget>
#include <QSpinBox>
#include <QToolButton>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPointer>
#include <QLabel>
#include <QTextBlock>

namespace ScIDE {

class GoToLineTool : public QWidget {
    Q_OBJECT

public:
    GoToLineTool(QWidget* parent = 0): QWidget(parent), mEditor(0) {
        mSpinBox = new QSpinBox;
        mSpinBox->setMinimum(0);
        mSpinBox->setMaximum(0);

        QToolButton* goBtn = new QToolButton;
        goBtn->setText(tr("Go"));
        goBtn->setAutoRaise(true);

        QLabel* label = new QLabel(tr("Line:"));

        QHBoxLayout* layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(label);
        layout->addWidget(mSpinBox);
        layout->addWidget(goBtn);
        layout->addStretch();

        setLayout(layout);

        setFocusProxy(mSpinBox);

        connect(goBtn, SIGNAL(clicked()), this, SLOT(go()));
        connect(mSpinBox, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    }

    void setEditor(GenericCodeEditor* editor) {
        if (mEditor)
            mEditor->disconnect(this, SLOT(setMaximum(int)));

        mEditor = editor;

        if (mEditor) {
            connect(mEditor, SIGNAL(blockCountChanged(int)), this, SLOT(setMaximum(int)));
            setMaximum(mEditor->blockCount());
        } else
            setMaximum(0);
    }

    void setValue(int value) { mSpinBox->setValue(value); }

public slots:

    void setMaximum(int max) {
        if (max > 0)
            mSpinBox->setRange(1, max);
        else
            mSpinBox->setRange(0, 0);
    }

    void setFocus() {
        mSpinBox->setFocus(Qt::OtherFocusReason);
        mSpinBox->selectAll();
    }

signals:

    void activated(int lineNumber);

private slots:

    void go() {
        int lineNumber = mSpinBox->value();

        if (mEditor) {
            QTextDocument* doc = mEditor->textDocument();
            QTextBlock block(doc->findBlockByNumber(lineNumber - 1));
            if (!block.isValid())
                return;

            QTextCursor cursor(doc);
            cursor.setPosition(block.position());
            mEditor->setTextCursor(cursor);
        }

        emit activated(lineNumber);
    }

    void onEditingFinished() {
        if (mSpinBox->hasFocus())
            go();
    }

private:
    QSpinBox* mSpinBox;
    QPointer<GenericCodeEditor> mEditor;
};

} // namespace ScIDE
