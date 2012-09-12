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
#include "main_window.hpp"
#include "../core/sc_introspection.hpp"
#include "../core/main.hpp"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>

namespace ScIDE {

GenericLookupDialog::GenericLookupDialog( QWidget * parent ):
    QDialog(parent, Qt::Popup | Qt::FramelessWindowHint)
{
    mQueryEdit = new QLineEdit(this);

    mResult = new QTreeView(this);
    mResult->setRootIsDecorated(false);
    mResult->setAllColumnsShowFocus(true);
    mResult->setHeaderHidden(true);
    mResult->header()->setStretchLastSection(false);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    layout->setSpacing(1);
    layout->addWidget(mQueryEdit);
    layout->addWidget(mResult);
    setLayout(layout);

    connect(mQueryEdit, SIGNAL(returnPressed()), this, SLOT(performQuery()));
    connect(mResult, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAccepted(QModelIndex)));
    connect(mResult, SIGNAL(activated(QModelIndex)),     this, SLOT(onAccepted(QModelIndex)));

    mResult->installEventFilter(this);

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

void GenericLookupDialog::onAccepted(QModelIndex currentIndex)
{
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(mResult->model());
    currentIndex = currentIndex.sibling(currentIndex.row(), 0);
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    if (!currentItem) {
        reject();
        return;
    }

    QString path = currentItem->data( PathRole ).toString();
    int pos      = currentItem->data( CharPosRole ).toInt();

    Main::documentManager()->open(path, pos);
    accept();
}

bool GenericLookupDialog::eventFilter( QObject *object, QEvent *event )
{
    if (object == mResult && event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch(ke->key()){
        case Qt::Key_Enter:
        case Qt::Key_Return:
            onAccepted(mResult->currentIndex());
            return true;
        default:;
        }
    }

    return QDialog::eventFilter(object,event);
}

void GenericLookupDialog::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(palette().color(QPalette::Dark));
    painter.drawRect(rect().adjusted(0,0,-1,-1));
}

void GenericLookupDialog::focusResults()
{
    mResult->header()->resizeSections(QHeaderView::ResizeToContents);
    mResult->setFocus(Qt::OtherFocusReason);

    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(mResult->model());
    QStandardItem * firstItem = model->item(0, 0);
    if (firstItem) {
        QModelIndex firstIndex = model->indexFromItem(firstItem);
        mResult->setCurrentIndex(firstIndex);
    }
}

using namespace ScLanguage;
using std::pair;
using std::vector;

LookupDialog::LookupDialog( QWidget * parent ):
    GenericLookupDialog(parent)
{
    setWindowTitle(tr("Look Up Class or Method Definition"));

    mQueryEdit->setText(tr("Enter symbol to look up"));
    mQueryEdit->selectAll();
}

void LookupDialog::performQuery()
{
    QString queryString = mQueryEdit->text();

    if (queryString.isEmpty()) {
        mResult->setModel(NULL);
        return;
    }

    const Introspection & introspection = Main::scProcess()->introspection();
    if (!introspection.introspectionAvailable()) {
        MainWindow::instance()->showStatusMessage("Introspection Data not available");
        return;
    }

    if (queryString[0].isUpper()) {
        bool success = performClassQuery(queryString);
        if (success) {
            focusResults();
            return;
        }
    } else {
        bool success = performMethodQuery(queryString);
        if (success) {
            focusResults();
            return;
        }
    }

    bool success = performPartialQuery(queryString);
    if (success)
        focusResults();
}

QList<QStandardItem*> GenericLookupDialog::makeDialogItem( QString const & name, QString const & displayPath,
                                                           QString const & path, int position )
{
    QStandardItem * item = new QStandardItem( name );
    item->setData( path, PathRole );
    item->setData( position, CharPosRole );
    QStandardItem * pathItem = new QStandardItem(displayPath);

    QList<QStandardItem*> ret;
    ret << item << pathItem;

    return ret;
}

QStandardItemModel * LookupDialog::modelForClass(const QString &className)
{
    const Introspection & introspection = Main::scProcess()->introspection();
    const Class *klass = introspection.findClass(className);

    if (!klass)
        return NULL;

    QStandardItemModel * model = new QStandardItemModel(this);
    QStandardItem *parentItem = model->invisibleRootItem();

    while (klass) {
        Class *metaClass = klass->metaClass;

        QString displayPath = introspection.compactLibraryPath(klass->definition.path);

        parentItem->appendRow(makeDialogItem(klass->name.get(), displayPath,
                                             klass->definition.path.get(),
                                             klass->definition.position));

        foreach (const Method * method, metaClass->methods) {
            QString signature = method->signature( Method::SignatureWithoutArguments );
            QString displayPath = introspection.compactLibraryPath(method->definition.path);

            parentItem->appendRow(makeDialogItem( signature, displayPath,
                                                  method->definition.path.get(),
                                                  method->definition.position ));
        }

        foreach (const Method * method, klass->methods) {
            QString signature = method->signature( Method::SignatureWithoutArguments );
            QString displayPath = introspection.compactLibraryPath(method->definition.path);

            parentItem->appendRow(makeDialogItem( signature, displayPath,
                                                  method->definition.path.get(),
                                                  method->definition.position ));
        }

        klass = klass->superClass;
    }

    return model;
}

QStandardItemModel * LookupDialog::modelForMethod(const QString & methodName)
{
    const Introspection & introspection = Main::scProcess()->introspection();

    const MethodMap & methods = introspection.methodMap();
    pair<MethodMap::const_iterator, MethodMap::const_iterator> matchingMethods = methods.equal_range(methodName);

    if (matchingMethods.first == matchingMethods.second)
        return NULL;

    QStandardItemModel * model = new QStandardItemModel(this);
    QStandardItem *parentItem = model->invisibleRootItem();

    for (MethodMap::const_iterator it = matchingMethods.first; it != matchingMethods.second; ++it) {
        Method *method = it->second.data();
        QString signature = method->signature( Method::SignatureWithoutArguments );

        const QString & path = method->definition.path;
        QString displayPath = introspection.compactLibraryPath(path);

        parentItem->appendRow(makeDialogItem( signature, displayPath,
                                              method->definition.path.get(),
                                              method->definition.position ));
    }

    model->sort(0);
    return model;
}

QStandardItemModel * LookupDialog::modelForCaseInsensitiveQuery(const QString & queryString)
{
    const Introspection & introspection = Main::scProcess()->introspection();
    vector<const Class *>  classes = introspection.findClassPartial(queryString);
    vector<const Method *> methods = introspection.findMethodPartial(queryString);
    typedef vector<const Method *>::const_iterator MethodIterator;
    typedef vector<const Class *>::const_iterator  ClassIterator;

    if (classes.empty() && methods.empty()) {
        MainWindow::instance()->showStatusMessage("No result for query");
        return NULL;
    }

    QStandardItemModel * model = new QStandardItemModel(this);
    QStandardItem *parentItem = model->invisibleRootItem();

    for (MethodIterator it = methods.begin(); it != methods.end(); ++it) {
        const Method *method = *it;
        QString signature = method->signature( Method::SignatureWithoutArguments );

        const QString & path = method->definition.path;
        QString displayPath = introspection.compactLibraryPath(path);

        parentItem->appendRow(makeDialogItem( signature, displayPath,
                                              method->definition.path.get(),
                                              method->definition.position ));
    }

    for (ClassIterator it = classes.begin(); it != classes.end(); ++it) {
        const Class * klass = *it;
        QString displayPath = introspection.compactLibraryPath(klass->definition.path);

        parentItem->appendRow(makeDialogItem(klass->name.get(), displayPath,
                                             klass->definition.path.get(),
                                             klass->definition.position));
    }

    model->sort(0);
    return model;
}

bool LookupDialog::performClassQuery(const QString & className)
{
    QStandardItemModel * model = modelForClass(className);
    mResult->setModel(model);
    return model;
}

bool LookupDialog::performMethodQuery(const QString & methodName)
{
    QStandardItemModel * model = modelForMethod(methodName);
    mResult->setModel(model);
    return model;
}

bool LookupDialog::performPartialQuery(const QString & queryString)
{
    QStandardItemModel * model = modelForCaseInsensitiveQuery(queryString);
    mResult->setModel(model);
    return model;
}


} // namespace ScIDE
