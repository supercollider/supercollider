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
#include "main.hpp"
#include "settings/manager.hpp"

#include <QPlainTextDocumentLayout>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextBlock>

using namespace ScIDE;

void Document::deleteTrailingSpaces()
{
    QTextCursor cursor (textDocument());
    cursor.movePosition(QTextCursor::EndOfBlock);
    QTextDocument * doc = textDocument();

    while( !cursor.atEnd() ) {
        while( (cursor.block().length() > 1) && doc->characterAt(cursor.position() - 1).isSpace())
            cursor.deletePreviousChar();

        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
    }
}

DocumentManager::DocumentManager( Main *main ):
    QObject(main)
{
    connect(&mFsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));

    connect(main, SIGNAL(storeSettingsRequest(Settings::Manager*)),
            this, SLOT(storeSettings(Settings::Manager*)));

    applySettings(main->settings());
}

void DocumentManager::create()
{
    Document *doc = new Document();
    mDocHash.insert( doc->id(), doc );
    Q_EMIT( opened(doc, 0) );
}

void DocumentManager::open( const QString & path, int initialCursorPosition, bool toRecent )
{
    QFileInfo info(path);
    QString cpath = info.canonicalFilePath();
    info.setFile(cpath);

    if (cpath.isEmpty()) {
        qWarning() << "DocumentManager: Can not open file: canonical path is empty.";
        return;
    }

    // Check if file already opened

    DocIterator it;
    for( it = mDocHash.begin(); it != mDocHash.end(); ++it )
    {
        Document *doc = it.value();
        if(doc->mFilePath == cpath) {
            Q_EMIT( showRequest(doc, initialCursorPosition) );
            if (toRecent) addToRecent(doc);
            return;
        }
    }

    // Open the file

    QFile file(cpath);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DocumentManager: the file" << cpath << "could not be opened for reading.";
        return;
    }
    QByteArray bytes( file.readAll() );
    file.close();

    Document *doc = new Document();
    doc->mDoc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );
    doc->mDoc->setModified(false);
    doc->mFilePath = cpath;
    doc->mTitle = info.fileName();

    mDocHash.insert( doc->id(), doc );

    mFsWatcher.addPath(cpath);

    Q_EMIT( opened(doc, initialCursorPosition) );

    if (toRecent) this->addToRecent(doc);
}

bool DocumentManager::reload( Document *doc )
{
    Q_ASSERT(doc);

    if (doc->mFilePath.isEmpty())
        return false;

    QFile file(doc->mFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DocumentManager: the file" << doc->mFilePath << "could not be opened for reading.";
        return false;
    }
    QByteArray bytes( file.readAll() );
    file.close();

    doc->mDoc->setPlainText( QString::fromUtf8( bytes.data(), bytes.size() ) );
    doc->mDoc->setModified(false);

    if (!mFsWatcher.files().contains(doc->mFilePath))
        mFsWatcher.addPath(doc->mFilePath);

    return true;
}

void DocumentManager::close( Document *doc )
{
    Q_ASSERT(doc);

    if( mDocHash.remove(doc->id()) == 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        return;
    }

    if (!doc->mFilePath.isEmpty())
        mFsWatcher.removePath(doc->mFilePath);

    mRecentActiveDocuments.removeOne(doc);
    Q_EMIT( closed(doc) );
    delete doc;
}

bool DocumentManager::save( Document *doc )
{
    Q_ASSERT(doc);

    return doSaveAs( doc, doc->mFilePath );
}

bool DocumentManager::saveAs( Document *doc, const QString & path )
{
    Q_ASSERT(doc);

    if (path.isEmpty()) {
        qWarning() << "DocumentManager: the saving path is empty.";
        return false;
    }

    bool ok = doSaveAs( doc, path );
    if (ok)
        addToRecent(doc);
    return ok;
}

bool DocumentManager::doSaveAs( Document *doc, const QString & path )
{
    Q_ASSERT(doc);

    doc->deleteTrailingSpaces();

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << path << "could not be opened for writing.";
        return false;
    }

    QString str = doc->textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.close();

    QFileInfo info(path);
    QString cpath = info.canonicalFilePath();

    doc->mFilePath = cpath;
    doc->mTitle = info.fileName();
    doc->mDoc->setModified(false);
    doc->mSaveTime = info.lastModified();

    // Always try to start watching, because the file could have been removed:
    if (!mFsWatcher.files().contains(cpath))
        mFsWatcher.addPath(cpath);

    Q_EMIT(saved(doc));

    return true;
}

void DocumentManager::onFileChanged( const QString & path )
{
    DocIterator it;
    for( it = mDocHash.begin(); it != mDocHash.end(); ++it )
    {
        Document *doc = it.value();
        if (doc->mFilePath == path) {
            QFileInfo info(doc->mFilePath);
            if (doc->mSaveTime < info.lastModified()) {
                doc->mDoc->setModified(true);
                emit changedExternally(doc);
            }
        }
    }
}

void DocumentManager::addToRecent( Document *doc )
{
    const QString &path = doc->mFilePath;
    int i = mRecent.indexOf(path);
    if (i != -1)
        mRecent.move( i, 0 );
    else {
        mRecent.prepend(path);
        if (mRecent.count() > mMaxRecent)
            mRecent.removeLast();
    }

    emit recentsChanged();
}

void DocumentManager::clearRecents()
{
    mRecent.clear();
    emit recentsChanged();
}

void DocumentManager::applySettings( Settings::Manager *s )
{
    QVariantList list = s->value("IDE/recentDocuments").value<QVariantList>();
    mRecent.clear();
    foreach (const QVariant & var, list)
        mRecent << var.toString();
}

void DocumentManager::storeSettings( Settings::Manager *s )
{
    QVariantList list;
    foreach (const QString & path, mRecent)
        list << QVariant(path);

    s->setValue("IDE/recentDocuments", QVariant::fromValue<QVariantList>(list));
}

void DocumentManager::activeDocumentChanged( Document * document )
{
    mRecentActiveDocuments.removeOne(document);
    mRecentActiveDocuments.push_front(document);
}
