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
#include <QDir>

using namespace ScIDE;

void DocumentManager::create()
{
    Document *doc = new Document();
    mDocHash.insert( doc->id(), doc );
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

    Document *doc = new Document();
    doc->mDoc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );
    doc->mFileName = filename;

    mDocHash.insert( doc->id(), doc );

    Q_EMIT( opened(doc) );
}

void DocumentManager::close( Document *doc )
{
    if(!doc) return;

    if( mDocHash.remove(doc->id()) == 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        return;
    }

    Q_EMIT( closed(doc) );

    delete doc;
}

void DocumentManager::save( Document *doc )
{
    if(!doc || doc->mFileName.isEmpty()) return;
    saveAs(doc, doc->mFileName);
}

void DocumentManager::saveAs( Document *doc, const QString & filename )
{
    if(!doc) return;

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << filename << "could not be opened for writing.";
        return;
    }

    QString str = doc->textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.close();

    doc->mFileName = filename;
    doc->mTitle = QDir(filename).dirName();

    Q_EMIT(saved(doc));
}
