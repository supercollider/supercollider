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
#include <QFileDialog>
#include <QMessageBox>

using namespace ScIDE;

void DocumentManager::create()
{
    Document *doc = new Document();
    mDocHash.insert( doc->id(), doc );
    Q_EMIT( opened(doc) );
}

void DocumentManager::open()
{
    QString filename = QFileDialog::getOpenFileName( NULL, "Open File" );
    if(filename.isEmpty()) return;

    open(filename);
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
    doc->mDoc->setModified(false);
    doc->mFileName = filename;
    doc->mTitle = QDir(filename).dirName();

    mDocHash.insert( doc->id(), doc );

    Q_EMIT( opened(doc) );
}

void DocumentManager::close( Document *doc, bool * p_ok )
{
    Q_ASSERT(doc);

    bool ok = true;

    if(doc->textDocument()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(
            NULL,
            tr("SuperCollider IDE"),
            tr("There are unsaved changes to document '%1'.\n"
                "Do you want to save the document?").arg(doc->title()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        if( ret == QMessageBox::Save ) {
            save(doc, &ok);
        }
        else if( ret == QMessageBox::Cancel )
            ok = false;
    }

    if( ok && mDocHash.remove(doc->id()) == 0 ) {
        qWarning("DocumentManager: trying to close an unmanaged document.");
        ok = false;
    }

    if(ok) {
        Q_EMIT( closed(doc) );
        delete doc;
    }

    if(p_ok) *p_ok = ok;
}

void DocumentManager::closeAll( bool * p_ok )
{
    bool ok = true;
    QHash<QByteArray, Document*>::iterator it;
    while((it = mDocHash.begin()) != mDocHash.end())
    {
        Document *doc = it.value();
        close(doc, &ok);
        if(!ok) break;
    }
    if(p_ok) *p_ok = ok;
}

void DocumentManager::save( Document *doc, bool * p_ok )
{
    Q_ASSERT(doc);

    bool ok = true;

    if(doc->mFileName.isEmpty())
        saveAs(doc, &ok);
    else if(doc->textDocument()->isModified())
        ok = saveAs(doc, doc->mFileName);

    if(p_ok) *p_ok = ok;
}

void DocumentManager::saveAs( Document *doc, bool * p_ok )
{
    Q_ASSERT(doc);
    bool ok = false;
    QString filename = QFileDialog::getSaveFileName( NULL, "Save Document" );
    if(!filename.isEmpty())
        ok = saveAs(doc, filename);
    if(p_ok) *p_ok = ok;
}

bool DocumentManager::saveAs( Document *doc, const QString & filename )
{
    Q_ASSERT(doc);

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << "DocumentManager: the file" << filename << "could not be opened for writing.";
        return false;
    }

    QString str = doc->textDocument()->toPlainText();
    file.write(str.toUtf8());
    file.close();

    doc->mFileName = filename;
    doc->mTitle = QDir(filename).dirName();
    doc->mDoc->setModified(false);

    Q_EMIT(saved(doc));

    return true;
}
