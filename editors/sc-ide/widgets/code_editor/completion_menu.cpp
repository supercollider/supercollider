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

#include "completion_menu.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QApplication>

namespace ScIDE {

CompletionMenu::CompletionMenu(QWidget * parent):
    PopUpWidget(parent),
    mCompletionRole(Qt::DisplayRole)
{
    mModel = new QStandardItemModel(this);
    mFilterModel = new QSortFilterProxyModel(this);
    mFilterModel->setSourceModel(mModel);

    mListView = new QListView();
    mListView->setModel(mFilterModel);
    mListView->setFrameShape(QFrame::NoFrame);

    mTextBrowser = new QTextBrowser();
    mTextBrowser->setFrameShape(QFrame::NoFrame);
    mTextBrowser->setReadOnly(true);
    mTextBrowser->setFixedSize(0, 200);
    mTextBrowser->setOpenLinks(false);

    mLayout = new QHBoxLayout(this);
    mLayout->addWidget(mListView);
    mLayout->addWidget(mTextBrowser);
    mLayout->setContentsMargins(1,1,1,1);

    connect(mListView, SIGNAL(clicked(QModelIndex)), this, SLOT(accept()));

    mListView->setFocus(Qt::OtherFocusReason);

    parent->installEventFilter(this);
}

void CompletionMenu::addItem(QStandardItem * item)
{
    mModel->appendRow(item);
}

void CompletionMenu::adapt()
{
    mListView->setFixedWidth(mListView->sizeHintForColumn(0));
    resize(0, 0);
}

void CompletionMenu::addInfo(QString info)
{
    mTextBrowser->setText(info);
    mTextBrowser->setFixedSize(500, 400);
}

void CompletionMenu::setCompletionRole(int role)
{
    mFilterModel->setFilterRole(role);
    mFilterModel->setSortRole(role);
    mCompletionRole = role;
}

QString CompletionMenu::currentText()
{
    QStandardItem *item =
        mModel->itemFromIndex (
            mFilterModel->mapToSource (
                mListView->currentIndex()));
    if (item)
        return item->data(mCompletionRole).toString();

    return QString();
}

const ScLanguage::Method * CompletionMenu::currentMethod()
{
    QStandardItem *item =
        mModel->itemFromIndex (
            mFilterModel->mapToSource (
                mListView->currentIndex()));

    return item ? item->data(MethodRole).value<const ScLanguage::Method*>() : 0;
}

QString CompletionMenu::exec(const QRect & rect)
{
    QString result;
    QPointer<CompletionMenu> self = this;
    if (PopUpWidget::exec(rect)) {
        if (!self.isNull())
            result = currentText();
    }
    return result;
}

QSortFilterProxyModel *CompletionMenu::model()
{
    return mFilterModel;
}

QListView *CompletionMenu::view()
{
    return mListView;
}

bool CompletionMenu::eventFilter(QObject * obj, QEvent * ev)
{
    if (isVisible() && obj == parentWidget() && ev->type() == QEvent::KeyPress)
    {
        bool filtered = false;

        QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
        switch(kev->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            QApplication::sendEvent(mListView, ev);

            static int oldIndex = 0;
            if (oldIndex != mListView->currentIndex().row()) {
                emit itemChanged(mListView->currentIndex().row());
                oldIndex = mListView->currentIndex().row();
            }

            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            accept();
            return true;
        }
    }

    return PopUpWidget::eventFilter(obj, ev);
}

} // namespace ScIDE
