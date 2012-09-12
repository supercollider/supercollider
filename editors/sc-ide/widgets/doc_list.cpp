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

#include "doc_list.hpp"
#include "../core/doc_manager.hpp"

#include <QApplication>
#include <QStyle>

namespace ScIDE {

DocumentListWidget::DocumentListWidget(DocumentManager *manager, QWidget * parent):
    QListWidget(parent),
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) )
{
    connect(manager, SIGNAL(opened(Document*, int, int)), this, SLOT(onOpen(Document*, int, int)));
    connect(manager, SIGNAL(closed(Document*)), this, SLOT(onClose(Document*)));
    connect(manager, SIGNAL(saved(Document*)), this, SLOT(onSaved(Document*)));
    connect(&mModificationMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(onModificationChanged(QObject*)));
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onItemClicked(QListWidgetItem*)));
}

void DocumentListWidget::setCurrent( Document *doc )
{
    if(!doc)
        setCurrentRow(-1);
    else {
        Item *itm = itemFor(doc);
        if(itm)
            setCurrentItem(itm);
    }
}

void DocumentListWidget::onOpen( Document *doc, int, int )
{
    addItemFor(doc);
}

void DocumentListWidget::onClose( Document *doc )
{
    delete itemFor(doc);
}

void DocumentListWidget::onSaved( Document *doc )
{
    Item *item = itemFor(doc);
    if(item)
        item->setText(doc->title());
}

void DocumentListWidget::onModificationChanged( QObject * obj )
{
    Document *doc = qobject_cast<Document*>(obj);
    Item *item = itemFor(doc);
    if(item)
        item->setIcon(
            doc->textDocument()->isModified() ?
            mDocModifiedIcon : QIcon()
        );
}

void DocumentListWidget::onItemClicked(QListWidgetItem* litem)
{
    Item *item = itemFor(litem);
    if(item)
        Q_EMIT( clicked(item->mDoc) );
}

DocumentListWidget::Item * DocumentListWidget::addItemFor( Document *doc )
{
    Item *item = new Item(doc, this);

    QTextDocument *tdoc = doc->textDocument();

    if(tdoc->isModified())
        item->setIcon( mDocModifiedIcon );

    mModificationMapper.setMapping(tdoc, doc);
    connect(tdoc, SIGNAL(modificationChanged(bool)),
            &mModificationMapper, SLOT(map()));

    return item;
}

DocumentListWidget::Item *DocumentListWidget::itemFor( Document *doc )
{
    int c = count();
    for(int i = 0; i < c; ++i)
    {
        Item *itm = itemFor( item(i) );
        if(itm && itm->mDoc == doc) return itm;
    }
    return 0;
}

DocumentListWidget::Item *DocumentListWidget::itemFor( QListWidgetItem *litem )
{
    if(litem->type() == QListWidgetItem::UserType)
        return static_cast<Item*>(litem);
    else
        return 0;
}

} // namespace ScIDE
