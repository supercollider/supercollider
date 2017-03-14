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
#include "../multi_editor.hpp"

namespace ScIDE {

class MultiEditor;

class MultiSplitter : public QSplitter
{
public:
    explicit MultiSplitter(MultiEditor *editor, QWidget *parent = 0):
        QSplitter(parent), mEditor(editor)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    MultiEditor *editor() { return mEditor; }

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
            // store parent's current distribution:
            QList<int> parentsSizes = parentSplitter->sizes();
            QSplitter * splitter = new QSplitter(direction);
            // move the neighbour to the new splitter, and add the new widget:
            splitter->addWidget(neighbour);
            splitter->addWidget(widget);
            // insert the new splitter at the neighbour's old position:
            parentSplitter->insertWidget(posInParent, splitter);
            // restore parent's old distribution, affected by operations above:
            parentSplitter->setSizes(parentsSizes);
            // change the parent to the new splitter, for operations below:
            parentSplitter = splitter;
        }

        setEqualSizes(parentSplitter);
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

        if (parent->count() == 1) {
            QWidget *neighbour = parent->widget(0);
            if (parent != this) {
                // replace parent splitter with it's only child
                QSplitter *grandParent = parentSplitterOf(parent);
                QList<int> grandParentsSizes = grandParent->sizes();
                int posInParent = grandParent->indexOf(parent);
                Q_ASSERT(posInParent != -1);
                grandParent->insertWidget(posInParent, neighbour);
                delete parent;
                grandParent->setSizes(grandParentsSizes);
            }
            else {
                // replace child splitter with it's own children
                QSplitter *splitter = qobject_cast<QSplitter*>(neighbour);
                if (splitter) {
                    QList<int> childSizes = splitter->sizes();
                    Qt::Orientation childOrientation = splitter->orientation();
                    int childCount = splitter->count();
                    while(childCount--)
                        parent->addWidget(splitter->widget(0)); // 0 is always another widget
                    delete splitter;
                    parent->setOrientation(childOrientation);
                    parent->setSizes(childSizes);
                }
            }
        }
    }

    template<typename T> T* findChild()
    {
        return MultiSplitter::findChild<T>(this);
    }

private:
    template<typename T> static T* findChild( QSplitter * splitter ) {
        int childCount = splitter->count();
        for (int idx = 0; idx < childCount; ++idx) {
            QWidget *child = splitter->widget(idx);
            T* typedChild = qobject_cast<T*>(child);
            if (typedChild)
                return typedChild;
            QSplitter *childSplitter = qobject_cast<QSplitter*>(child);
            if (childSplitter) {
                typedChild = findChild<T>( childSplitter );
                if (typedChild)
                    return typedChild;
            }
        }
        return 0;
    }

    QSplitter *parentSplitterOf( QWidget *widget )
    {
        return qobject_cast<QSplitter*>( widget->parent() );
    }

    void setEqualSizes(QSplitter *splitter)
    {
        int widgetCount = splitter->count();
        int splitterSize = splitter->orientation() == Qt::Horizontal ?
                    splitter->size().width() : splitter->size().height();
        QList<int> newWidgetSizes;
        int singleWidgetSize = splitterSize / widgetCount;
        for( int idx = 0; idx < widgetCount; ++idx )
            newWidgetSizes << singleWidgetSize;
        splitter->setSizes( newWidgetSizes );
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

    MultiEditor *mEditor;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED
