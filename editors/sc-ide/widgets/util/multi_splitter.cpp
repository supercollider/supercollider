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

#include "multi_splitter.hpp"

namespace ScIDE {
    
    MultiSplitter::MultiSplitter(EditorWindow *window, QWidget *parent):
        QSplitter(parent)
        {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            //if the splitter has opened on a new editor, it stores the window's address in order to close it when switch-session is called.
            mWindow = window;
            
            mCmdLine = new CmdLine(tr("Command Line:"));
            connect(mCmdLine, SIGNAL(invoked(QString, bool)),
                    Main::scProcess(), SLOT(evaluateCode(QString, bool)));
            
            mReplaceTool = new TextFindReplacePanel;
            
            mGoToLineTool = new GoToLineTool();
            connect(mGoToLineTool, SIGNAL(activated(int)), this, SLOT(hideToolBox()));
            
            mToolBox = new ToolBox;
            mToolBox->addWidget(mCmdLine);
            mToolBox->addWidget(mReplaceTool);
            mToolBox->addWidget(mGoToLineTool);
            mToolBox->hide();
        }
        
        ToolBox * MultiSplitter::toolBox() { return mToolBox; }
        CmdLine * MultiSplitter::cmdLine() { return mCmdLine; }
        TextFindReplacePanel * MultiSplitter::findReplaceTool() { return mReplaceTool; }
        GoToLineTool * MultiSplitter::goToLineTool() { return mGoToLineTool; }
        
        void MultiSplitter::addSplit(CodeEditorBox *split) {
            splitList << split;
        }
        
        void MultiSplitter::removeSplit(CodeEditorBox *split) {
            splitList.removeOne(split);
        }
        
        QList<CodeEditorBox*> MultiSplitter::splits() { return splitList; }
        
        void MultiSplitter::insertWidget( QWidget *widget, QWidget *neighbour, Qt::Orientation direction )
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
        
        void MultiSplitter::removeWidget( QWidget *widget )
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

        void MultiSplitter::hideToolBox() { MainWindow::instance()->editor()->hideToolBox(); }
    
        QSplitter * MultiSplitter::parentSplitterOf( QWidget *widget )
        {
            return qobject_cast<QSplitter*>( widget->parent() );
        }
        
        void MultiSplitter::setEqualSizes(QSplitter *splitter)
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
        
        bool MultiSplitter::widgetIsChild( QWidget *widget )
        {
            QObject *object = widget->parent();
            while (object) {
                if (object == this)
                    return true;
                object = object->parent();
            }
            return false;
        }

    
} // namespace ScIDE
