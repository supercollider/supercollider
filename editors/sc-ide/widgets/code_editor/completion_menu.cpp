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

    mTextEdit = new QTextEdit();
    mTextEdit->setFrameShape(QFrame::NoFrame);
    mTextEdit->setReadOnly(true);
    mTextEdit->setFixedSize(0, 200);

    mLayout = new QHBoxLayout(this);
    mLayout->addWidget(mListView);
    mLayout->addWidget(mTextEdit);
    mLayout->setContentsMargins(1,1,1,1);

    connect(mListView, SIGNAL(clicked(QModelIndex)), this, SLOT(accept()));

    mListView->setFocus(Qt::OtherFocusReason);

    resize(200, 200);

    parent->installEventFilter(this);
}

void CompletionMenu::addItem(QStandardItem * item)
{
    mModel->appendRow(item);
    mListView->setFixedWidth(mListView->sizeHintForColumn(0));
}

void CompletionMenu::addInfo(QString info)
{
    mTextEdit->setText(info);
    mTextEdit->setFixedSize(500, 400);
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
