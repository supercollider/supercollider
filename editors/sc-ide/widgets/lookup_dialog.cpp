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

#include "lookup_dialog.hpp"
#include "../core/sc_introspection.hpp"
#include "../core/main.hpp"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>

namespace ScIDE {

LookupDialog::LookupDialog( QWidget * parent ):
    QDialog(parent, Qt::Popup | Qt::FramelessWindowHint)
{
    setWindowTitle(tr("Look Up Class or Method Definition"));

    mQueryEdit = new QLineEdit(this);
    mQueryEdit->setText(tr("Enter symbol to look up"));
    mQueryEdit->selectAll();

    mResultList = new QTreeWidget(this);
    mResultList->setRootIsDecorated(false);
    mResultList->setAllColumnsShowFocus(true);
    mResultList->setHeaderHidden(true);
    mResultList->header()->setStretchLastSection(false);
    mResultList->setColumnCount(2);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    layout->setSpacing(1);
    layout->addWidget(mQueryEdit);
    layout->addWidget(mResultList);
    setLayout(layout);

    connect(mQueryEdit, SIGNAL(returnPressed()), this, SLOT(performQuery()));
    connect(mResultList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(accept()));
    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));

    mResultList->installEventFilter(this);

    QRect bounds(0,0,600,300);
    if (parent) {
        QRect parentRect = parent->rect();
        bounds.moveCenter( parent->mapToGlobal( parentRect.center() ) );
    } else {
        QRect availableBounds = QApplication::desktop()->availableGeometry(this);
        bounds.moveCenter( availableBounds.center() );
    }

    setGeometry(bounds);

    mQueryEdit->setFocus( Qt::OtherFocusReason );
}


void LookupDialog::performQuery()
{
    QString queryString = mQueryEdit->text();

    if (queryString.isEmpty()) {
        mResultList->clear();
        return;
    }

    bool result;
    if (queryString[0].isUpper())
        result = performClassQuery(queryString);
    else
        result = performMethodQuery(queryString);

    if (result) {
        mResultList->header()->resizeSections(QHeaderView::ResizeToContents);
        mResultList->setFocus(Qt::OtherFocusReason);
        mResultList->setCurrentItem( mResultList->topLevelItem(0) );
        mResultList->scrollToItem( mResultList->topLevelItem(0) );
    }
}

bool LookupDialog::performClassQuery(const QString & className)
{
    using namespace ScLanguage;

    mResultList->clear();

    const Introspection & introspection = Main::instance()->scProcess()->introspection();

    const Class *klass = introspection.findClass(className);
    if (!klass)
        return false;

    QColor pathColor = palette().color(QPalette::Light);

    while (klass) {
        Class *metaClass = klass->metaClass;

        QString displayPath = introspection.compactLibraryPath(klass->definition.path);

        QTreeWidgetItem *classItem = new QTreeWidgetItem( QStringList() << klass->name.get() << displayPath );
        classItem->setData( 0, Qt::UserRole, klass->definition.path.get() );
        classItem->setData( 0, Qt::UserRole+1, klass->definition.position );
        classItem->setData( 1, Qt::ForegroundRole, pathColor );
        mResultList->addTopLevelItem(classItem);

        foreach (const Method * method, metaClass->methods) {
            QString signature = method->signature( Method::SignatureWithoutArguments );
            QString displayPath = introspection.compactLibraryPath(method->definition.path);

            QTreeWidgetItem *methodItem = new QTreeWidgetItem( QStringList() << signature << displayPath );
            methodItem->setData( 0, Qt::UserRole, method->definition.path.get() );
            methodItem->setData( 0, Qt::UserRole+1, method->definition.position );
            methodItem->setData( 1, Qt::ForegroundRole, pathColor );
            mResultList->addTopLevelItem(methodItem);
        }

        foreach (const Method * method, klass->methods) {
            QString signature = method->signature( Method::SignatureWithoutArguments );
            QString displayPath = introspection.compactLibraryPath(method->definition.path);

            QTreeWidgetItem *methodItem = new QTreeWidgetItem( QStringList() << signature << displayPath  );
            methodItem->setData( 0, Qt::UserRole, method->definition.path.get() );
            methodItem->setData( 0, Qt::UserRole+1, method->definition.position );
            methodItem->setData( 1, Qt::ForegroundRole, pathColor );
            mResultList->addTopLevelItem(methodItem);
        }

        klass = klass->superClass;
    }

    return true;
}

bool LookupDialog::performMethodQuery(const QString & methodName)
{
    using namespace ScLanguage;
    using std::pair;

    mResultList->clear();

    const Introspection & introspection = Main::instance()->scProcess()->introspection();
    const MethodMap & methods = introspection.methodMap();

    pair<MethodMap::const_iterator, MethodMap::const_iterator> matchingMethods =
        methods.equal_range(methodName);

    if (matchingMethods.first == matchingMethods.second) {
        qWarning("Method not defined!");
        return false;
    }

    QColor pathColor = palette().color(QPalette::Light);

    for (MethodMap::const_iterator it = matchingMethods.first;
         it != matchingMethods.second; ++it) {
        Method *method = it->second.data();

        const QString & path = method->definition.path;
        QString displayPath = introspection.compactLibraryPath(path);

        QTreeWidgetItem *item = new QTreeWidgetItem (
            mResultList,
            QStringList()
            << method->signature(Method::SignatureWithoutArguments)
            << displayPath
        );

        item->setData( 0, Qt::UserRole, path );
        item->setData( 0, Qt::UserRole + 1, method->definition.position );
        item->setData( 1, Qt::ForegroundRole, pathColor );
    }

    mResultList->model()->sort(0);

    return true;
}

void LookupDialog::onAccepted()
{
    QTreeWidgetItem *currentItem = mResultList->currentItem();
    if (!currentItem)
        return;

    QString path = currentItem->data( 0, Qt::UserRole ).toString();
    int pos = currentItem->data( 0, Qt::UserRole + 1 ).toInt();

    Main::instance()->documentManager()->open(path, pos);
}

bool LookupDialog::eventFilter( QObject *object, QEvent *event )
{
    if (object == mResultList && event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch(ke->key()){
        case Qt::Key_Enter:
        case Qt::Key_Return:
            accept();
            return true;
        default:;
        }
    }

    return QDialog::eventFilter(object,event);
}

void LookupDialog::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(palette().color(QPalette::Dark));
    painter.drawRect(rect().adjusted(0,0,-1,-1));
}

} // namespace ScIDE
