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

#include <QVBoxLayout>

#include "sc_symbol_references.hpp"
#include "../core/main.hpp"
#include "../core/sc_introspection.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

using namespace ScIDE;

void SymbolReferenceRequest::onSymbolReferencesReply(const QString &command, const QString &responseData)
{
    QString symbol;
    QStandardItemModel * model = parse(responseData, symbol);
    if (!model)
        return; // show error?
    ReferencesDialog * dialog = new ReferencesDialog(symbol, model, parent());
    dialog->exec();

    delete dialog;
}

QStandardItemModel * SymbolReferenceRequest::parse(const QString &responseData, QString & symbol)
{
    std::stringstream stream;
    stream << responseData.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if(!parser.GetNextDocument(doc)) {
        qWarning("no YAML document");
        return NULL;
    }

    assert (doc.Type() == YAML::NodeType::Sequence);

    symbol = doc[0].to<std::string>().c_str();

    QStandardItemModel * model = new QStandardItemModel(this);

    YAML::Node const & references = doc[1];

    for (YAML::Iterator refIt = references.begin(); refIt != references.end(); ++refIt ) {
        YAML::Node const & reference = *refIt;
        QString className  = reference[0].to<std::string>().c_str();
        QString methodName = reference[1].to<std::string>().c_str();
        QString fileName   = reference[2].to<std::string>().c_str();
        int charPos        = reference[3].to<int>();

        QString fullName = ScLanguage::makeFullMethodName(className, methodName);

        QStandardItem * item = new QStandardItem(fullName);
        item->setData(fileName, PathRole);
        item->setData(charPos, CharPosRole);

        model->appendRow(item);
    }

    return model;
}

ReferencesDialog::ReferencesDialog(QString const & symbol, QStandardItemModel * model, QObject * parentL)
{
    setWindowTitle(tr("References to: ") + symbol);

    mList = new QListView(this);
    mList->setModel(model);
    mList->setModelColumn(0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->addWidget(mList);
    setLayout(layout);

    connect(mList, SIGNAL(activated(QModelIndex)), this, SLOT(onAccepted(QModelIndex)));
}


void ReferencesDialog::onAccepted(QModelIndex currentIndex)
{
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(mList->model());
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    if (!currentItem)
        return;

    QString path = currentItem->data( SymbolReferenceRequest::PathRole ).toString();
    int pos      = currentItem->data( SymbolReferenceRequest::CharPosRole ).toInt();

    Main::documentManager()->open(path, pos);
}
