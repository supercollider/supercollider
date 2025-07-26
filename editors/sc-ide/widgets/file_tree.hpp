#pragma once
#include <QTreeView>
#include <QFileSystemModel>

#include "doc_manager.hpp"
#include "sc_editor.hpp"
#include "util/docklet.hpp"

namespace ScIDE {
class ScCodeEditor;

class FileTreeWidget : public QTreeView {
    Q_OBJECT

public:
    FileTreeWidget(DocumentManager* manager, QWidget* parent = 0);

private Q_SLOTS:
    void doubleClicked(const QModelIndex& index);
    void onOpen(Document* document, int, int);
    void contextMenuEvent(QContextMenuEvent* event);

private:
    QFileSystemModel* mFileSystemModel;
    DocumentManager* mDocumentManager;
};

class FileTreeDocklet : public Docklet {
    Q_OBJECT
public:
    FileTreeDocklet(DocumentManager* manager, QWidget* parent = 0);

private:
    FileTreeWidget* mFileTreeWidget;
};
}