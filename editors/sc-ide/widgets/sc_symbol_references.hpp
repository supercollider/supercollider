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

#ifndef SCIDE_SC_SYMBOL_REFERENCES_HPP_INCLUDED
#define SCIDE_SC_SYMBOL_REFERENCES_HPP_INCLUDED

#include <QDialog>
#include <QListView>
#include <QObject>
#include <QStandardItemModel>

#include "../core/sc_process.hpp"

namespace ScIDE {

// LATER: cache symbol references to avoid duplicate lookup
class SymbolReferenceRequest:
    public ScRequest
{
    Q_OBJECT

public:
    static const int PathRole       = Qt::UserRole + 1;
    static const int CharPosRole    = Qt::UserRole + 2;

    SymbolReferenceRequest(QString const & symbol, SCProcess * process, QObject * parent = NULL):
        ScRequest(process, parent)
    {
        connect(this, SIGNAL(response(QString,QString)), this, SLOT(onSymbolReferencesReply(QString, QString)));
        send("findReferencesToSymbol", symbol);
    }

private Q_SLOTS:
    void onSymbolReferencesReply(const QString &command, const QString &responseData);

private:
    QStandardItemModel * parse(QString const & responseData, QString & symbol);
};

class ReferencesDialog: public QDialog
{
    Q_OBJECT

public:
    ReferencesDialog(QString const & symbol, QStandardItemModel * model, QObject * parent = NULL);

private slots:
    void onAccepted(QModelIndex);

private:
    QListView * mList;
};

}


#endif // SCIDE_SC_SYMBOL_REFERENCES_HPP_INCLUDED
