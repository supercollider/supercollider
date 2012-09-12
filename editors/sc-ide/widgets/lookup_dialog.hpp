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

#ifndef SCIDE_WIDGETS_LOOKUP_DIALOG_HPP_INCLUDED
#define SCIDE_WIDGETS_LOOKUP_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTreeWidget>

#include "../core/sc_process.hpp"

namespace ScIDE {

class GenericLookupDialog: public QDialog
{
    Q_OBJECT

public:
    static const int PathRole       = Qt::UserRole + 1;
    static const int CharPosRole    = Qt::UserRole + 2;

    explicit GenericLookupDialog(QWidget *parent = 0);
    void query( const QString & query ) { mQueryEdit->setText(query); this->performQuery(); }
    void clearQuery() { mQueryEdit->clear(); }

private slots:
    void onAccepted(QModelIndex);
    virtual void performQuery() = 0;

protected:
    bool eventFilter( QObject *, QEvent * );
    void paintEvent( QPaintEvent * );
    void focusResults();

    static QList<QStandardItem *> makeDialogItem(QString const & name, QString const & displayPath, QString const & path, int position);

    QTreeView *mResult;
    QLineEdit *mQueryEdit;
};

class LookupDialog : public GenericLookupDialog
{
    Q_OBJECT

public:
    explicit LookupDialog(QWidget *parent = 0);

private slots:
    void performQuery();

private:
    QStandardItemModel * modelForClass(const QString & className);
    QStandardItemModel * modelForMethod(const QString & methodName);
    QStandardItemModel * modelForCaseInsensitiveQuery(const QString & queryString);
    bool performClassQuery(const QString & className);
    bool performMethodQuery(const QString & methodName);
    bool performPartialQuery(const QString & queryString);
};

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

} // namespace ScIDE

#endif // SCIDE_WIDGETS_LOOKUP_DIALOG_HPP_INCLUDED
