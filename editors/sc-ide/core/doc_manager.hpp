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

#include "sc_syntax_highlighter.hpp"

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
    {
        SyntaxHighlighter *highligher = new SyntaxHighlighter(mDoc);
    }

    QTextDocument *textDocument() { return mDoc; }
    const QByteArray & id() { return mId; }
    QString fileName() { return mFileName; }
    QString title() { return mTitle; }

private:
    QByteArray mId;
    QTextDocument *mDoc;
    QString mFileName;
    QString mTitle;
};

class DocumentManager : public QObject
{
    Q_OBJECT

public:

    DocumentManager( QObject *parent = 0 ) : QObject(parent) {}
    QList<Document*> documents() {
        return mDocHash.values();
    }

public Q_SLOTS:

    void create();
    void open();
    void open( const QString & filename );
    void close( Document *, bool * ok = 0 );
    void closeAll( bool * ok = 0 );
    void save( Document *, bool * ok = 0 );
    void saveAs( Document *, bool * ok = 0 );

Q_SIGNALS:

    void opened( Document * );
    void closed( Document * );
    void saved( Document * );

private:
    bool saveAs( Document *, const QString & filename );

    QHash<QByteArray, Document*> mDocHash;
};

} // namespace ScIDE


#endif // SCIDE_DOC_MANAGER_HPP_INCLUDED
