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

#ifndef SCIDE_WIDGETS_PATH_CHOOSER_WIDGET_HPP_INCLUDED
#define SCIDE_WIDGETS_PATH_CHOOSER_WIDGET_HPP_INCLUDED

#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPointer>

namespace ScIDE {

class PathChooserWidget : public QWidget
{
    Q_OBJECT

public:
    PathChooserWidget(QWidget *parent = 0, QFileDialog::FileMode mode = QFileDialog::ExistingFile ):
        QWidget(parent),
        mMode(mode),
        dialog(0)
    {
        mTextField = new QComboBox;
        mTextField->setEditable(true);
        mTextField->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));

        mBrowseButton = new QPushButton;
        mBrowseButton->setIconSize(QSize(24, 24));
        mBrowseButton->setMaximumSize(QSize(24, 24));
        mBrowseButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
        mBrowseButton->setFlat(true);

        QHBoxLayout *box = new QHBoxLayout();
        box->addWidget(mTextField);
        box->addWidget(mBrowseButton);
        box->setContentsMargins(0,0,0,0);
        box->setSpacing(5);
        
        setLayout(box);

        connect(mBrowseButton, SIGNAL(clicked()), this, SLOT(execDialog()), Qt::QueuedConnection);
    }

    QString text() const { return mTextField->currentText(); }

    void setText( const QString & text ) { mTextField->setCurrentText(text); }
    
    void setRecentPaths(const QStringList& recents) { mTextField->addItems(recents); }
    
    QStringList recentPaths() {
        QStringList recentPaths;
        for (int i = 0; i < mTextField->count(); ++i) {
            recentPaths.push_back(mTextField->itemText(i));
        }
        return recentPaths;
    }

    void setFileMode( QFileDialog::FileMode mode ) { mMode = mode; }

public Q_SLOTS:
    void execDialog()
    {
        if(!dialog) {
            QWidget *parent = parentWidget();
            dialog = new QFileDialog(parent ? parent : this);
        }
        dialog->setFileMode(mMode);
        dialog->selectFile(mTextField->currentText());

        QPointer<QComboBox> textField = mTextField;
        int result = dialog->exec();
        if (result == QDialog::Accepted && textField) {
            QStringList paths = dialog->selectedFiles();
            if (paths.count()) {
                QString path = paths[0];
                textField->setCurrentText(path);
                textField->insertItem(0, path);
            }
        }
    }

Q_SIGNALS:
    void textChanged( const QString & );

private:
    QFileDialog::FileMode mMode;
    QComboBox       *mTextField;
    QPushButton     *mBrowseButton;
    QFileDialog     *dialog;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_PATH_CHOOSER_WIDGET_HPP_INCLUDED
