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

#ifndef SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED
#define SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED

#include <QSplitter>

namespace ScIDE {

class MultiSplitter : public QSplitter
{
public:
    explicit MultiSplitter(QWidget *firstWidget, QWidget *parent = 0):
        QSplitter(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        addWidget(firstWidget);
    }

    void insertWidget( QWidget *widget, QWidget *neighbour, Qt::Orientation direction )
    {
        if (!neighbour || !widgetIsChild(neighbour)) {
            qWarning("MultiSplitter: neighbour widget invalid");
            return;
        }

        QSplitter *parentSplitter = parentSplitterOf(neighbour);
        Q_ASSERT(parentSplitter);
        int posInParent = parentSplitter->indexOf(neighbour);
        Q_ASSERT(posInParent != -1);

        if (parentSplitter->orientation() == direction) {
            parentSplitter->insertWidget(posInParent + 1, widget);
        }
        else if (parentSplitter->count() < 2)
        {
            parentSplitter->setOrientation(direction);
            parentSplitter->addWidget(widget);
        }
        else {
            QSplitter * splitter = new QSplitter(direction);
            splitter->addWidget(neighbour);
            splitter->addWidget(widget);
            parentSplitter->insertWidget(posInParent, splitter);
        }
    }

    void removeWidget( QWidget *widget )
    {
        if (!widget || !widgetIsChild(widget)) {
            qWarning("MultiSplitter: widget invalid");
            return;
        }

        QSplitter *parent = parentSplitterOf(widget);
        if (parent == this && parent->count() < 2) {
            qWarning("MultiSplitter: can not remove last widget");
            return;
        }

        delete widget;

        Q_ASSERT(parent->count());

        if (parent != this && parent->count() == 1) {
            QSplitter *grandParent = parentSplitterOf(parent);
            int posInParent = grandParent->indexOf(parent);
            Q_ASSERT(posInParent != -1);
            grandParent->insertWidget(posInParent, parent->widget(0));
            delete parent;
        }
    }

private:
    QSplitter *parentSplitterOf( QWidget *widget )
    {
        return qobject_cast<QSplitter*>( widget->parent() );
    }

    bool widgetIsChild( QWidget *widget )
    {
        QObject *object = widget->parent();
        while (object) {
            if (object == this)
                return true;
            object = object->parent();
        }
        return false;
    }
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED
