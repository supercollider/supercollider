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
#include <QList>
#include <QTextDocument>

namespace ScIDE
{

class DocumentManager : public QObject
{
    Q_OBJECT

public:

    DocumentManager( QObject *parent = 0 ) : QObject(parent) {}

    QTextDocument * document( int index );
    int index( QTextDocument * );


public Q_SLOTS:

    void create();
    void open( const QString & filename );
    void close( QTextDocument * );
    void save( QTextDocument *, const QString & filename );

Q_SIGNALS:

    void opened( QTextDocument * );
    void closed( QTextDocument * );

private:
    QTextDocument *newDoc();

    QList<QTextDocument*> mDocs;
};

} // namespace ScIDE


#endif // SCIDE_DOC_MANAGER_HPP_INCLUDED
