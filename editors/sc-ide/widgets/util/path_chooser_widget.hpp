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

#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPointer>

namespace ScIDE {

class PathChooserWidget : public QWidget {
    Q_OBJECT

public:
    PathChooserWidget(QWidget* parent = 0, QFileDialog::FileMode mode = QFileDialog::ExistingFile):
        QWidget(parent),
        mMode(mode),
        dialog(0) {
        mTextField = new QLineEdit;

        mButton = new QPushButton;
        mButton->setIcon(QIcon::fromTheme("folder"));
        mButton->setFixedWidth(30);
        mButton->setFlat(true);
        // setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QHBoxLayout* box = new QHBoxLayout(this);
        box->addWidget(mTextField);
        box->addWidget(mButton);
        box->setContentsMargins(0, 0, 0, 0);
        box->setSpacing(1);

        connect(mButton, SIGNAL(clicked()), this, SLOT(execDialog()), Qt::QueuedConnection);
    }

    QString text() const { return mTextField->text(); }

    void setText(const QString& text) { mTextField->setText(text); }

    void setFileMode(QFileDialog::FileMode mode) { mMode = mode; }

public Q_SLOTS:
    void execDialog() {
        if (!dialog) {
            QWidget* parent = parentWidget();
            dialog = new QFileDialog(parent ? parent : this);
        }
        dialog->setFileMode(mMode);
        dialog->selectFile(mTextField->text());

        QPointer<QLineEdit> textField = mTextField;
        int result = dialog->exec();
        if (result == QDialog::Accepted && textField) {
            QStringList paths = dialog->selectedFiles();
            textField->setText(paths.count() ? paths[0] : "");
        }
    }

Q_SIGNALS:
    void textChanged(const QString&);

private:
    QLineEdit* mTextField;
    QPushButton* mButton;
    QFileDialog::FileMode mMode;
    QFileDialog* dialog;
};

} // namespace ScIDE
