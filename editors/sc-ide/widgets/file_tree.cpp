#include "file_tree.hpp"

#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include <QFileSystemModel>
#include <QMenu>

#include "doc_manager.hpp"
#include "sc_editor.hpp"

namespace ScIDE {

FileTreeWidget::FileTreeWidget(DocumentManager* manager, QWidget* parent): QTreeView(parent) {
    QString rootPath = QDir::homePath();
    mFileSystemModel = new QFileSystemModel(this);
    mFileSystemModel->setRootPath(rootPath);
    mFileSystemModel->setFilter(QDir::Files | QDir::Dirs | QDir::NoDot);

    QTreeView::setModel(mFileSystemModel);
    QTreeView::setRootIndex(mFileSystemModel->index(rootPath));

    for (int i = 1; i < mFileSystemModel->columnCount(); i++) {
        QTreeView::hideColumn(i);
    }
    QTreeView::expand(QTreeView::rootIndex());

    connect(this, &QTreeView::doubleClicked, this, &FileTreeWidget::doubleClicked);
    connect(manager, SIGNAL(opened(Document*, int, int)), this, SLOT(onOpen(Document*, int, int)));
    mDocumentManager = manager;
}

void FileTreeWidget::doubleClicked(const QModelIndex& index) {
    QString path = mFileSystemModel->filePath(index);
    auto fileInfo = QFileInfo(path);
    if (fileInfo.isDir()) {
        QTreeView::setRootIndex(mFileSystemModel->index(fileInfo.absoluteFilePath()));
    } else {
        bool isScFile = fileInfo.suffix().toLower() == "sc" | fileInfo.suffix().toLower() == "scd";
        if (isScFile) {
            mDocumentManager->open(path);
        } else {
            // insert absolute path to document
            if (auto document = mDocumentManager->open(path)) {
                // @todo
            }
        }
    }
}

void FileTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
    QModelIndex index = indexAt(event->pos());

    if (!index.isValid()) {
        return; // Ignore if no item is clicked
    }
    auto fileInfo = QFileInfo(mFileSystemModel->filePath(index));
    bool isScFile = fileInfo.suffix().toLower() == "sc" | fileInfo.suffix().toLower() == "scd";
    bool hasSavedDocument = mDocumentManager->activeDocument()->filePath().length() > 0;

    QMenu contextMenu(this);

    QAction* openAction = contextMenu.addAction("Open");
    QAction* copyAbsolutePath = contextMenu.addAction("Copy absolute path");
    QAction* copyRelativeDocumentPath = contextMenu.addAction("Copy relative path to current document");
    QAction* copyRelativeViewPath = contextMenu.addAction("Copy relative path from view");

    openAction->setEnabled(isScFile);
    copyRelativeDocumentPath->setEnabled(hasSavedDocument);

    QAction* selectedAction = contextMenu.exec(event->globalPos());

    if (selectedAction == openAction) {
        emit doubleClicked(index);
    } else if (selectedAction == copyRelativeViewPath) {
        QDir viewPath = mFileSystemModel->rootPath();
        QGuiApplication::clipboard()->setText(viewPath.relativeFilePath(fileInfo.absolutePath()));
    } else if (selectedAction == copyRelativeDocumentPath) {
        if (hasSavedDocument) {
            QDir documentPath = mDocumentManager->activeDocument()->filePath();
            QGuiApplication::clipboard()->setText(documentPath.relativeFilePath(fileInfo.path()));
        }
    } else if (selectedAction == copyAbsolutePath) {
        QDir absolutePath = fileInfo.absoluteFilePath();
        QGuiApplication::clipboard()->setText(absolutePath.absolutePath());
    }
}

void FileTreeWidget::onOpen(Document* document, int, int) {
    if (document->filePath().length() > 0) {
        auto dir = QFileInfo(document->filePath());
        QTreeView::setRootIndex(mFileSystemModel->index(dir.absolutePath()));
    }
}

FileTreeDocklet::FileTreeDocklet(DocumentManager* manager, QWidget* parent): Docklet(tr("File tree"), parent) {
    mFileTreeWidget = new FileTreeWidget(manager, parent);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWidget(mFileTreeWidget);
}
}