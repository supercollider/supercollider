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
#include <QFile>
#include <QDebug>

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
    QTextDocument *doc = newDoc();
    Q_EMIT( opened(doc) );
}

void DocumentManager::open( const QString & filename )
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DocumentManager: the file" << filename << "could not be opened for reading.";
        return;
    }

    QByteArray bytes( file.readAll() );

    file.close();

    QTextDocument *doc = newDoc();
    doc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );

    Q_EMIT( opened(doc) );
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

void DocumentManager::save( QTextDocument *doc, const QString & filename )
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << filename << "could not be opened for writing.";
        return;
    }

    QString str = doc->toPlainText();
    file.write(str.toUtf8());

    file.close();
}

QTextDocument * DocumentManager::newDoc()
{
    QTextDocument *doc = new QTextDocument(this);
    mDocs.append(doc);
    return doc;
}
