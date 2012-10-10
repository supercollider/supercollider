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

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QHash>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QStringList>
#include <QTextDocument>
#include <QPlainTextDocumentLayout>
#include <QUuid>

namespace ScIDE {

namespace Settings { class Manager; }
class SyntaxHighlighter;

class Main;
class DocumentManager;

class Document : public QObject
{
    Q_OBJECT

    friend class DocumentManager;

public:
    Document( bool isPlainText );

    QTextDocument *textDocument() { return mDoc; }
    const QByteArray & id() { return mId; }
    const QString & filePath() { return mFilePath; }
    const QString & title() { return mTitle; }

    QFont defaultFont() const { return mDoc->defaultFont(); }
    void setDefaultFont( const QFont & font );

    int indentWidth() const { return mIndentWidth; }
    void setIndentWidth( int numSpaces );

    void deleteTrailingSpaces();

    bool isPlainText() const { return mHighlighter == NULL; }
    bool isModified() const  { return mDoc->isModified(); }

public slots:
    void applySettings( Settings::Manager * );
    void resetDefaultFont();

signals:
    void defaultFontChanged();

private:
    QByteArray mId;
    QTextDocument *mDoc;
    QString mFilePath;
    QString mTitle;
    QDateTime mSaveTime;
    int mIndentWidth;
    SyntaxHighlighter * mHighlighter;
};

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    typedef QList< Document * > DocumentList;

    DocumentManager( Main *, Settings::Manager * );
    QList<Document*> documents() {
        return mDocHash.values();
    }

    void create();
    void close( Document * );
    bool save( Document * );
    bool saveAs( Document *, const QString & path );
    bool reload( Document * );
    const QStringList & recents() const { return mRecent; }

public slots:
    // initialCursorPosition -1 means "don't change position if already open"
    Document * open( const QString & path, int initialCursorPosition = -1, int selectionLength = 0, bool addToRecent = true );
    void clearRecents();
    void storeSettings( Settings::Manager * );

Q_SIGNALS:
    void opened( Document *, int cursorPosition, int selectionLength );
    void closed( Document * );
    void saved( Document * );
    void showRequest( Document *, int pos = -1, int selectionLength = 0 );
    void changedExternally( Document * );
    void recentsChanged();

private slots:
    void onFileChanged( const QString & path );

private:
    Document * createDocument( bool isPlainText );
    bool doSaveAs( Document *, const QString & path );
    void addToRecent( Document * );
    void loadRecentDocuments( Settings::Manager * );
    void closeSingleUntitledIfUnmodified();


    typedef QHash<QByteArray, Document*>::iterator DocIterator;

    QHash<QByteArray, Document*> mDocHash;
    QFileSystemWatcher mFsWatcher;

    QStringList mRecent;
    static const int mMaxRecent = 10;
};

} // namespace ScIDE

Q_DECLARE_METATYPE( ScIDE::Document* )

#endif // SCIDE_DOC_MANAGER_HPP_INCLUDED
