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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>

using namespace ScIDE;

DocumentManager::DocumentManager( QObject *parent ):
    QObject(parent)
{
    connect(&mFsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
}

void DocumentManager::create()
{
    Document *doc = new Document();
    mDocHash.insert( doc->id(), doc );
    Q_EMIT( opened(doc, 0) );
}

void DocumentManager::open( const QString & filename, int initialCursorPosition )
{
    // Check if file already opened

    DocIterator it;
    for( it = mDocHash.begin(); it != mDocHash.end(); ++it )
    {
        Document *doc = it.value();
        if(doc->fileName() == filename) {
            Q_EMIT( showRequest(doc, initialCursorPosition) );
            return;
        }
    }

    // Open the file

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DocumentManager: the file" << filename << "could not be opened for reading.";
        return;
    }
    QByteArray bytes( file.readAll() );
    file.close();

    Document *doc = new Document();
    doc->mDoc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );
    doc->mDoc->setModified(false);
    doc->mFileName = filename;
    doc->mTitle = QDir(filename).dirName();

    mDocHash.insert( doc->id(), doc );

    mFsWatcher.addPath(filename);

    Q_EMIT( opened(doc, initialCursorPosition) );
}

bool DocumentManager::reload( Document *doc )
{
    Q_ASSERT(doc);

    if (doc->mFileName.isEmpty())
        return false;

    QFile file(doc->mFileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DocumentManager: the file" << doc->mFileName << "could not be opened for reading.";
        return false;
    }
    QByteArray bytes( file.readAll() );
    file.close();

    doc->mDoc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );
    doc->mDoc->setModified(false);

    if (!mFsWatcher.files().contains(doc->mFileName))
        mFsWatcher.addPath(doc->mFileName);

    return true;
}

void DocumentManager::close( Document *doc )
{
    Q_ASSERT(doc);

    if( mDocHash.remove(doc->id()) == 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        return;
    }

    if (!doc->mFileName.isEmpty())
        mFsWatcher.removePath(doc->mFileName);

    Q_EMIT( closed(doc) );
    delete doc;
}

bool DocumentManager::save( Document *doc )
{
    Q_ASSERT(doc);

    if (doc->mFileName.isEmpty())
        return false;

    return doSaveAs( doc, doc->mFileName );
}

bool DocumentManager::saveAs( Document *doc, const QString & filename )
{
    Q_ASSERT(doc);
    return doSaveAs( doc, filename );
}

bool DocumentManager::doSaveAs( Document *doc, const QString & fileName )
{
    Q_ASSERT(doc);

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << fileName << "could not be opened for writing.";
        return false;
    }

    QString str = doc->textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.close();

    doc->mFileName = fileName;
    doc->mTitle = QDir(fileName).dirName();
    doc->mDoc->setModified(false);
    QFileInfo finfo(file);
    doc->mSaveTime = finfo.lastModified();

    // Always try to start watching, because the file could have been removed:
    if (!mFsWatcher.files().contains(fileName))
        mFsWatcher.addPath(fileName);

    Q_EMIT(saved(doc));

    return true;
}

void DocumentManager::onFileChanged( const QString & path )
{
    DocIterator it;
    for( it = mDocHash.begin(); it != mDocHash.end(); ++it )
    {
        Document *doc = it.value();
        if (doc->mFileName == path) {
            QFileInfo info(doc->mFileName);
            if (doc->mSaveTime < info.lastModified()) {
                doc->mDoc->setModified(true);
                emit changedExternally(doc);
            }
        }
    }
}
