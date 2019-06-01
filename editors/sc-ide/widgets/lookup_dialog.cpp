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
#include <QFileInfo>

#include <yaml-cpp/yaml.h>

namespace ScIDE {

GenericLookupDialog::GenericLookupDialog(QWidget* parent): QDialog(parent) {
    addAction(MainWindow::instance()->action(MainWindow::LookupDocumentationForCursor));

    mQueryEdit = new QLineEdit(this);

    mResult = new QTreeView(this);
    mResult->setRootIsDecorated(false);
    mResult->setAllColumnsShowFocus(true);
    mResult->setHeaderHidden(true);
    mResult->header()->setStretchLastSection(false);

    mPreviewDocument = new Document(false);
    mPreviewEditor = new ScCodeEditor(mPreviewDocument);
    mPreviewEditor->setReadOnly(true);
    mPreviewEditor->setVisible(false);
    mPreviewEditor->setTabChangesFocus(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    layout->addWidget(mQueryEdit, 0);
    layout->addWidget(mResult, 2);
    layout->addWidget(mPreviewEditor, 1);

    setLayout(layout);

    connect(mQueryEdit, SIGNAL(returnPressed()), this, SLOT(performQuery()));
    connect(mResult, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onAccepted(QModelIndex)));
    connect(mResult, SIGNAL(activated(QModelIndex)), this, SLOT(onAccepted(QModelIndex)));

    mResult->installEventFilter(this);

    QRect bounds(0, 0, 600, 300);
    if (parent) {
        QRect parentRect = parent->rect();
        bounds.moveCenter(parent->mapToGlobal(parentRect.center()));
    } else {
        QRect availableBounds = QApplication::desktop()->availableGeometry(this);
        bounds.moveCenter(availableBounds.center());
    }

    setGeometry(bounds);

    mQueryEdit->setFocus(Qt::OtherFocusReason);
}

void GenericLookupDialog::setModel(QStandardItemModel* model) {
    if (mResult->selectionModel()) {
        disconnect(mResult->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this,
                   SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
    }

    mResult->setModel(model);

    if (mResult->selectionModel()) {
        mPreviewEditor->setActiveAppearance(true);
        connect(mResult->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this,
                SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
    } else {
        mPreviewEditor->setActiveAppearance(false);
    }
}

void GenericLookupDialog::currentChanged(const QModelIndex& item, const QModelIndex& oldItem) {
    QString path = item.data(PathRole).toString();
    int pos = item.data(CharPosRole).toInt();

    QFileInfo info(path);
    QString cpath = info.canonicalFilePath();
    info.setFile(cpath);

    if (cpath.isEmpty()) {
        MainWindow::instance()->showStatusMessage(tr("Cannot open file: %1 (file does not exist)").arg(path));
        return;
    }

    // Open the file
    QFile file(cpath);
    if (!file.open(QIODevice::ReadOnly)) {
        MainWindow::instance()->showStatusMessage(tr("Cannot open file for reading: %1").arg(cpath));
        return;
    }

    QByteArray bytes(file.readAll());
    file.close();

    QTextStream stream(bytes);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);

    mPreviewDocument->setTextInRange(stream.readAll(), 0, -1);
    mPreviewEditor->showPosition(pos, 0);
    mPreviewEditor->selectCurrentRegion();
}

bool GenericLookupDialog::openDocumentation() {
    QModelIndex currentIndex = mResult->currentIndex();
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(mResult->model());
    if (!model)
        return true;

    currentIndex = currentIndex.sibling(currentIndex.row(), 0);
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);
    if (!currentItem)
        return true;

    bool isClass = currentItem->data(IsClassRole).toBool();
    if (isClass) {
        Main::openDocumentation(currentItem->text());
    } else {
        QString className = currentItem->data(ClassNameRole).toString();
        QString methodName = currentItem->data(MethodNameRole).toString();
        Main::openDocumentationForMethod(className, methodName);
    }
    accept();

    return true;
}

void GenericLookupDialog::onAccepted(QModelIndex currentIndex) {
    QStandardItem* currentItem = firstItemInLine(currentIndex);
    if (!currentItem) {
        reject();
        return;
    }

    QString path = currentItem->data(PathRole).toString();
    int pos = currentItem->data(CharPosRole).toInt();

    Main::documentManager()->open(path, pos, 0, false);
    accept();
}

QStandardItem* GenericLookupDialog::firstItemInLine(QModelIndex index) {
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(mResult->model());
    if (!model)
        return NULL;
    index = index.sibling(index.row(), 0);
    return model->itemFromIndex(index);
}

bool GenericLookupDialog::event(QEvent* event) {
    switch (event->type()) {
    case QEvent::ShortcutOverride: {
        QKeyEvent* kevent = static_cast<QKeyEvent*>(event);
        if (kevent->key() == Qt::Key_Escape) {
            accept();
            return true;
        }
    }
    default:;
    }

    return QDialog::event(event);
}

bool GenericLookupDialog::eventFilter(QObject* object, QEvent* event) {
    if (object == mResult && event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        switch (ke->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            onAccepted(mResult->currentIndex());
            return true;
        default:;
        }
    }

    return QDialog::eventFilter(object, event);
}

void GenericLookupDialog::focusResults() {
    mResult->header()->resizeSections(QHeaderView::ResizeToContents);
    mResult->setFocus(Qt::OtherFocusReason);

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(mResult->model());
    QStandardItem* firstItem = model->item(0, 0);
    if (firstItem) {
        QModelIndex firstIndex = model->indexFromItem(firstItem);
        mResult->setCurrentIndex(firstIndex);
    }
}

using namespace ScLanguage;
using std::pair;
using std::vector;

LookupDialog::LookupDialog(QWidget* parent): GenericLookupDialog(parent), mIsPartialQuery(false) {
    setWindowTitle(tr("Look Up Implementations"));

    mQueryEdit->setText(tr("Enter class or method name..."));
    mQueryEdit->selectAll();
}

void LookupDialog::performQuery() {
    QString queryString = mQueryEdit->text();

    if (queryString.isEmpty()) {
        setModel(NULL);
        return;
    }

    const Introspection& introspection = Main::scProcess()->introspection();
    if (!introspection.introspectionAvailable()) {
        MainWindow::instance()->showStatusMessage(tr("Introspection data not yet available"));
        return;
    }

    mIsPartialQuery = false;
    if (queryString[0].isUpper()) {
        bool success = performClassQuery(queryString);
        mPreviewEditor->setVisible(success);

        if (success) {
            focusResults();
            return;
        }
    } else {
        bool success = performMethodQuery(queryString);
        mPreviewEditor->setVisible(success);

        if (success) {
            focusResults();
            return;
        }
    }

    bool success = performPartialQuery(queryString);
    mPreviewEditor->setVisible(success);
    if (success)
        focusResults();
}

void LookupDialog::onAccepted(QModelIndex currentIndex) {
    if (!mIsPartialQuery) {
        GenericLookupDialog::onAccepted(currentIndex);
        return;
    }

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(mResult->model());
    currentIndex = currentIndex.sibling(currentIndex.row(), 0);
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    if (!currentItem) {
        reject();
        return;
    }

    bool isClass = currentItem->data(IsClassRole).toBool();
    if (!isClass) {
        GenericLookupDialog::onAccepted(currentIndex);
        return;
    }

    QString className = currentItem->text();
    mQueryEdit->setText(className);
    performQuery();
}

QList<QStandardItem*> GenericLookupDialog::makeDialogItem(QString const& displayString, QString const& displayPath,
                                                          QString const& path, int position, QString const& className,
                                                          QString const& methodName, bool isClassItem) {
    // LATER: shall we just store a shared pointer to the class or method?
    QStandardItem* item = new QStandardItem(displayString);
    item->setData(path, PathRole);
    item->setData(position, CharPosRole);
    item->setData(isClassItem, IsClassRole);
    item->setData(className, ClassNameRole);
    item->setData(methodName, MethodNameRole);

    QStandardItem* pathItem = new QStandardItem(displayPath);

    QList<QStandardItem*> ret;
    ret << item << pathItem;

    return ret;
}

QStandardItemModel* LookupDialog::modelForClass(const QString& className, const QString& methodName) {
    const Introspection& introspection = Main::scProcess()->introspection();
    const Class* klass = introspection.findClass(className);

    if (!klass)
        return NULL;

    QStandardItemModel* model = new QStandardItemModel(this);
    QStandardItem* parentItem = model->invisibleRootItem();

    while (klass) {
        Class* metaClass = klass->metaClass;

        QString displayPath = introspection.compactLibraryPath(klass->definition.path);

        if (methodName.isEmpty()) {
            parentItem->appendRow(makeDialogItem(klass->name.get(), displayPath, klass->definition.path.get(),
                                                 klass->definition.position, klass->name.get(), QStringLiteral(""),
                                                 true));
        }

        foreach (const Method* method, metaClass->methods) {
            QString signature = method->signature(Method::SignatureWithoutArguments);
            QString displayPath = introspection.compactLibraryPath(method->definition.path);

            if (method->matches(methodName)) {
                parentItem->appendRow(makeDialogItem(signature, displayPath, method->definition.path.get(),
                                                     method->definition.position, metaClass->name.get(),
                                                     method->name.get(), false));
            }
        }

        foreach (const Method* method, klass->methods) {
            QString signature = method->signature(Method::SignatureWithoutArguments);
            QString displayPath = introspection.compactLibraryPath(method->definition.path);


            if (method->matches(methodName)) {
                parentItem->appendRow(makeDialogItem(signature, displayPath, method->definition.path.get(),
                                                     method->definition.position, klass->name.get(), method->name.get(),
                                                     false));
            }
        }

        klass = klass->superClass;
    }

    return model;
}

QStandardItemModel* LookupDialog::modelForMethod(const QString& methodName) {
    const Introspection& introspection = Main::scProcess()->introspection();

    const MethodMap& methods = introspection.methodMap();
    pair<MethodMap::const_iterator, MethodMap::const_iterator> matchingMethods = methods.equal_range(methodName);

    if (matchingMethods.first == matchingMethods.second)
        return NULL;

    QStandardItemModel* model = new QStandardItemModel(this);
    QStandardItem* parentItem = model->invisibleRootItem();

    for (MethodMap::const_iterator it = matchingMethods.first; it != matchingMethods.second; ++it) {
        Method* method = it->second.data();
        QString signature = method->signature(Method::SignatureWithoutArguments);

        const QString& path = method->definition.path;
        QString displayPath = introspection.compactLibraryPath(path);

        parentItem->appendRow(makeDialogItem(signature, displayPath, method->definition.path.get(),
                                             method->definition.position, method->ownerClass->name.get(),
                                             method->name.get(), false));
    }

    model->sort(0);
    return model;
}

QStandardItemModel* LookupDialog::modelForPartialQuery(const QString& queryString) {
    const Introspection& introspection = Main::scProcess()->introspection();
    vector<const Class*> classes = introspection.findClassPartial(queryString);
    vector<const Method*> methods = introspection.findMethodPartial(queryString);
    typedef vector<const Method*>::const_iterator MethodIterator;
    typedef vector<const Class*>::const_iterator ClassIterator;

    if (classes.empty() && methods.empty()) {
        MainWindow::instance()->showStatusMessage(tr("No result for query"));
        return NULL;
    }

    mIsPartialQuery = true;
    QStandardItemModel* model = new QStandardItemModel(this);
    QStandardItem* parentItem = model->invisibleRootItem();

    for (MethodIterator it = methods.begin(); it != methods.end(); ++it) {
        const Method* method = *it;
        QString signature = method->signature(Method::SignatureWithoutArguments);

        const QString& path = method->definition.path;
        QString displayPath = introspection.compactLibraryPath(path);

        parentItem->appendRow(makeDialogItem(signature, displayPath, method->definition.path.get(),
                                             method->definition.position, method->ownerClass->name.get(),
                                             method->name.get(), false));
    }

    for (ClassIterator it = classes.begin(); it != classes.end(); ++it) {
        const Class* klass = *it;
        QString displayPath = introspection.compactLibraryPath(klass->definition.path);

        parentItem->appendRow(makeDialogItem(klass->name.get(), displayPath, klass->definition.path.get(),
                                             klass->definition.position, klass->name.get(), "", true));
    }

    model->sort(0);
    return model;
}

bool LookupDialog::performClassQuery(const QString& className) {
    QStandardItemModel* model;
    if (className.contains(QChar(':'))) {
        QStringList split = className.split(":");
        model = modelForClass(split[0].trimmed(), split[1].trimmed());
    } else {
        model = modelForClass(className);
    }
    setModel(model);
    return model;
}

bool LookupDialog::performMethodQuery(const QString& methodName) {
    QStandardItemModel* model = modelForMethod(methodName);
    setModel(model);
    return model;
}

bool LookupDialog::performPartialQuery(const QString& queryString) {
    QStandardItemModel* model = modelForPartialQuery(queryString);
    setModel(model);
    return model;
}


ReferencesDialog::ReferencesDialog(QWidget* parent): LookupDialog(parent) {
    mRequest = new SymbolReferenceRequest(Main::scProcess(), this);
    connect(mRequest, SIGNAL(response(QString, QString)), this, SLOT(onResposeFromLanguage(QString, QString)));
    connect(mRequest, SIGNAL(cancelled()), this, SLOT(requestCancelled()));

    setWindowTitle(tr("Look Up References"));

    mQueryEdit->setText(tr("Enter class or method name..."));
    mQueryEdit->selectAll();
}

void ReferencesDialog::performQuery() {
    QString queryString = mQueryEdit->text();

    if (queryString.isEmpty()) {
        setModel(NULL);
    } else {
        queryString.replace('\"', "\\\"");
        mRequest->sendRequest(queryString);
    }
}

void ReferencesDialog::requestCancelled() { setModel(NULL); }

void ReferencesDialog::onResposeFromLanguage(const QString&, const QString& responseData) {
    QStandardItemModel* model = parse(responseData);
    setModel(model);

    if (model)
        focusResults();
}

QStandardItemModel* ReferencesDialog::parse(const QString& responseData) {
    using namespace ScLanguage;
    const Introspection& introspection = Main::scProcess()->introspection();

    if (!introspection.introspectionAvailable()) { // just required for short path name
        MainWindow::instance()->showStatusMessage(tr("Introspection data not yet available"));
        return NULL;
    }

    YAML::Node doc = YAML::Load(responseData.toStdString());
    if (!doc) {
        qWarning("no YAML document");
        return NULL;
    }

    assert(doc.Type() == YAML::NodeType::Sequence);

    QString symbol = doc[0].as<std::string>().c_str();

    QStandardItemModel* model = new QStandardItemModel(this);
    QStandardItem* parentItem = model->invisibleRootItem();

    YAML::Node const& references = doc[1];

    for (YAML::Node const& reference : references) {
        QString className = reference[0].as<std::string>().c_str();
        QString methodName = reference[1].as<std::string>().c_str();
        QString path = reference[2].as<std::string>().c_str();
        int charPos = reference[3].as<int>();

        QString displayPath = introspection.compactLibraryPath(path);
        QString fullName = ScLanguage::makeFullMethodName(className, methodName);

        parentItem->appendRow(makeDialogItem(fullName, displayPath, path, charPos, className, methodName, false));
    }

    return model;
}

} // namespace ScIDE
