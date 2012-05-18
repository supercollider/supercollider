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

#ifndef SCIDE_DOC_MANAGER_HPP_INCLUDED
#define SCIDE_DOC_MANAGER_HPP_INCLUDED

#include <QObject>
#include <QTextDocument>
#include <QUuid>
#include <QHash>
#include <QFileSystemWatcher>
#include <QDateTime>

namespace ScIDE
{

class DocumentManager;

class Document : public QObject
{
    Q_OBJECT

    friend class DocumentManager;

public:
    Document() :
        mId( QUuid::createUuid().toString().toAscii() ),
        mDoc( new QTextDocument(this) ),
        mTitle( "Untitled" )
    {}

    QTextDocument *textDocument() { return mDoc; }
    const QByteArray & id() { return mId; }
    const QString & fileName() { return mFileName; }
    const QString & title() { return mTitle; }

private:
    QByteArray mId;
    QTextDocument *mDoc;
    QString mFileName;
    QString mTitle;
    QDateTime mSaveTime;
};

class DocumentManager : public QObject
{
    Q_OBJECT

public:

    DocumentManager( QObject *parent = 0 );
    QList<Document*> documents() {
        return mDocHash.values();
    }

    void create();
    // initialCursorPosition -1 means "don't change position if already open"
    void open( const QString & filename, int initialCursorPosition = -1 );
    void close( Document * );
    bool save( Document * );
    bool saveAs( Document *, const QString & filename );
    bool reload( Document * );

Q_SIGNALS:

    void opened( Document *, int );
    void closed( Document * );
    void saved( Document * );
    void showRequest( Document *, int pos = -1 );
    void changedExternally( Document * );

private slots:
    void onFileChanged( const QString & path );

private:
    bool doSaveAs( Document *, const QString & filename );

    typedef QHash<QByteArray, Document*>::iterator DocIterator;

    QHash<QByteArray, Document*> mDocHash;
    QFileSystemWatcher mFsWatcher;
};

} // namespace ScIDE


#endif // SCIDE_DOC_MANAGER_HPP_INCLUDED
