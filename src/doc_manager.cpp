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

#include "doc_manager.hpp"

#include <QPlainTextDocumentLayout>

using namespace ScIDE;

QTextDocument * DocumentManager::document( int index )
{
    if (index < 0 || index > mDocs.count())
        return 0;
    return mDocs[index];
}

int DocumentManager::index( QTextDocument *doc )
{
   return mDocs.indexOf(doc);
}

void DocumentManager::create()
{
    QTextDocument *doc = new QTextDocument(this);
    doc->setDocumentLayout( new QPlainTextDocumentLayout(doc) );
    mDocs.append(doc);

    Q_EMIT( opened(doc) );
}

void DocumentManager::open( const QString & filename )
{
    // TODO
}

void DocumentManager::close( QTextDocument *doc )
{
    int i = index(doc);
    if( i < 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        return;
    }

    mDocs.removeAll(doc);

    Q_EMIT( closed(doc) );

    delete doc;
}
