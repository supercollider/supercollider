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

#include "../core/sc_process.hpp"
#include "lookup_dialog.hpp"

namespace ScIDE {

// LATER: cache symbol references to avoid duplicate lookup
class SymbolReferenceRequest:
    public ScRequest
{
    Q_OBJECT

public:
    SymbolReferenceRequest(SCProcess * process, QObject * parent = NULL):
        ScRequest(process, parent)
    {}

    void sendRequest(QString const & symbol)
    {
        send("findReferencesToSymbol", symbol);
    }
};

class ReferencesDialog:
    public LookupDialog
{
    Q_OBJECT

public:
    explicit ReferencesDialog(QWidget * parent = NULL);

private slots:
    void requestCanceled();
    void performQuery();
    void onResposeFromLanguage(const QString &command, const QString &responseData);
    QStandardItemModel * parse(QString const & responseData);
};

}

#endif // SCIDE_SC_SYMBOL_REFERENCES_HPP_INCLUDED
