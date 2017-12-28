/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann & Miguel Negrão
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

#include "file_tree.hpp"
#include "../core/doc_manager.hpp"

#include <QApplication>
#include <QFileSystemModel>
#include <QDebug>

namespace ScIDE {

FileTreeWidget::FileTreeWidget(QWidget * parent):
    QTreeView(parent)
{
    setFrameShape( QFrame::NoFrame );
    mModel.setRootPath(QDir::currentPath());
    setModel(&mModel);
    hideColumn(1);
    hideColumn(2);
    hideColumn(3);
    setProperty("headerHidden", true);

    connect(this, SIGNAL(doubleClicked(QModelIndex)),
        this, SLOT(onItemDoubleClicked(QModelIndex)));
}

void FileTreeWidget::onItemDoubleClicked(const QModelIndex& index)
{
  QString path = mModel.filePath(index);
  qDebug() << path;
  QFileInfo info(path);
  QString ext = info.suffix();
  if (ext == "sc" || ext == "scd" || ext == "schelp" || ext == "txt" || ext == "yaml")
    Q_EMIT( clicked(path) );
}

void FileTreeWidget::setRoot(const QString& path)
{
    mModel.setRootPath(path);
    setRootIndex(mModel.index(path));
}

FileTreeDocklet::FileTreeDocklet(QWidget* parent):
    Docklet(tr("File Tree"), parent),
    mFileTree(new FileTreeWidget())
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWidget(mFileTree);
}

void FileTreeDocklet::setRoot(const QString &dir, const QString &title)
{
    tree()->setRoot(dir);
    toolBar()->setTitle(title);
}

} // namespace ScIDE
