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
#include <QTreeWidget>
#include <QLineEdit>

namespace ScIDE {

class LookupDialog : public QDialog
{
    Q_OBJECT

public:
    LookupDialog(QWidget *parent = 0);
    void query( const QString & query ) { mQueryEdit->setText(query); performQuery(); }
    void clearQuery() { mQueryEdit->clear(); }

private slots:
    void onAccepted();
    void performQuery();

private:
    bool performClassQuery(const QString & className);
    bool performMethodQuery(const QString & methodName);
    bool eventFilter( QObject *, QEvent * );
    void paintEvent( QPaintEvent * );

    QTreeWidget *mResultList;
    QLineEdit *mQueryEdit;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_LOOKUP_DIALOG_HPP_INCLUDED
