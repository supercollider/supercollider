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

#ifndef SCIDE_WIDGETS_FILE_TREE_HPP_INCLUDED
#define SCIDE_WIDGETS_FILE_TREE_HPP_INCLUDED

#include "util/docklet.hpp"

#include <QTreeView>
#include <QSignalMapper>
#include <QFileSystemModel>

namespace ScIDE {

class FileTreeWidget : public QTreeView
{
    Q_OBJECT

public:
    FileTreeWidget(QWidget * parent = 0);
    void setRoot(const QString& path);

public Q_SLOTS:
    void onItemDoubleClicked(const QModelIndex& index);

Q_SIGNALS:
    void clicked( const QString & path );

protected:
    virtual QSize sizeHint() const { return QSize(200,200); }

private:
    QFileSystemModel mModel;
};

class FileTreeDocklet : public Docklet
{
    Q_OBJECT
public:
    FileTreeDocklet(QWidget* parent = 0);
    FileTreeWidget *tree() { return mFileTree; }
    void setRoot(const QString &dir, const QString &title);

private:
    FileTreeWidget *mFileTree;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_FILE_TREE_HPP_INCLUDED
