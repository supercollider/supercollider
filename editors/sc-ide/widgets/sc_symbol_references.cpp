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

#include <QHeaderView>

#include "sc_symbol_references.hpp"
#include "../core/main.hpp"
#include "../core/sc_introspection.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

using namespace ScIDE;

ReferencesDialog::ReferencesDialog(QWidget * parent):
    LookupDialog(parent)
{
    setWindowTitle(tr("Look Up References"));

    mQueryEdit->setText(tr("Enter symbol to find references"));
    mQueryEdit->selectAll();
}

void ReferencesDialog::performQuery()
{
    QString queryString = mQueryEdit->text();

    if (queryString.isEmpty()) {
        mResult->setModel(NULL);
        return;
    }

    SymbolReferenceRequest * request = new SymbolReferenceRequest(Main::scProcess(), this);
    connect(request, SIGNAL(response(QString,QString)), this, SLOT(onResposeFromLanguage(QString,QString)));
    request->sendRequest(queryString);

    connect(request, SIGNAL(symbolReferencesReceived(QString)), this, SLOT(onResposeFromLanguage(QString)));
}

void ReferencesDialog::onResposeFromLanguage(const QString &, const QString &responseData)
{
    QStandardItemModel * model = parse(responseData);
    mResult->setModel(model);

    if (model)
        focusResults();
}

QStandardItemModel * ReferencesDialog::parse(const QString &responseData)
{
    using namespace ScLanguage;
    const Introspection & introspection = Main::scProcess()->introspection();

    if (!introspection.introspectionAvailable()) {
        qWarning() << "Introspection not available"; // just required for short path name
        return NULL;
    }

    std::stringstream stream;
    stream << responseData.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if(!parser.GetNextDocument(doc)) {
        qWarning("no YAML document");
        return NULL;
    }

    assert (doc.Type() == YAML::NodeType::Sequence);

    QString symbol = doc[0].to<std::string>().c_str();

    QStandardItemModel * model = new QStandardItemModel(this);
    QStandardItem *parentItem = model->invisibleRootItem();

    YAML::Node const & references = doc[1];

    for (YAML::Iterator refIt = references.begin(); refIt != references.end(); ++refIt ) {
        YAML::Node const & reference = *refIt;
        QString className  = reference[0].to<std::string>().c_str();
        QString methodName = reference[1].to<std::string>().c_str();
        QString path       = reference[2].to<std::string>().c_str();
        int charPos        = reference[3].to<int>();

        QString displayPath = introspection.compactLibraryPath(path);
        QString fullName = ScLanguage::makeFullMethodName(className, methodName);

        parentItem->appendRow(makeDialogItem(fullName, displayPath, path, charPos));
    }

    return model;
}
