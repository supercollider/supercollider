/*
 *    SuperCollider Qt IDE
 *    Copyright (c) 2012 Jakob Leben & Tim Blechmann
 *    http://www.audiosynth.com
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <cassert>

#include "main.hpp"

#include <QMenu>

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

namespace ScIDE {

void SCIpcServer::tryEvaluateReceivedData ( void )
{
    static QString classes("classes");
    static QString symbolTable("symbolTable");
    static QString classDefinitions("classDefinitions");
    static QString methodDefinitions("methodDefinitions");

    QBuffer receivedData ( &mReceivedData );
    receivedData.open ( QIODevice::ReadOnly );

    QDataStream in ( &receivedData );
    in.setVersion ( QDataStream::Qt_4_6 );
    QString selector, message;
    in >> selector;
    if ( in.status() != QDataStream::Ok )
        return;

    in >> message;
    if ( in.status() != QDataStream::Ok )
        return;

    mReceivedData.remove ( 0, receivedData.pos() );

    if ( selector == classDefinitions )
        openClassDefinitionHandler(message);
    else if ( selector == classes )
        classesHandler(message);
    else if ( selector == symbolTable )
        symbolTableHandler(message);
    else if ( selector == methodDefinitions )
        methodDefinitionHandler(message);
    else
        qDebug ( "selector not recognized: %s", selector.toStdString().c_str() );
}

void SCIpcServer::classesHandler ( const QString& YAMLString )
{

}

void SCIpcServer::openClassDefinitionHandler ( const QString& YAMLString )
{
    std::stringstream stream;
    stream << YAMLString.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        assert(doc.Type() == YAML::NodeType::Sequence);

        switch (doc.size()) {
        case 0:
        {
            emit message("Class not defined!");
            return;
        }

        case 1:
        {
            YAML::Iterator it = doc.begin();
            YAML::Node const & entry = *it;
            assert(entry.Type() == YAML::NodeType::Sequence);

            // name, path, character position
            std::string path = entry[1].to<std::string>();
            int charPosition = entry[2].to<int>() - 1; // position is one off
            Main::instance()->documentManager()->open(QString(path.c_str()), charPosition);
            return;
        }

        default:
        {
            QMenu * menu = new QMenu();
            QList<QAction*> actions;
            for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it) {
                YAML::Node const & entry = *it;
                assert(entry.Type() == YAML::NodeType::Sequence);

                std::string path = entry[1].to<std::string>();
                int charPosition = entry[2].to<int>() - 1; // position is one off

                QAction * action = new QAction(QString(path.c_str()), menu);
                action->setData(QVariant(charPosition));
                actions.append(action);
            }

            menu->addActions(actions);
            QAction * selectedAction = menu->exec(QCursor::pos());
            if (selectedAction)
                Main::instance()->documentManager()->open(selectedAction->text(), selectedAction->data().toInt());
            delete menu;
        }
        }
    }
}

void SCIpcServer::symbolTableHandler ( const QString& YAMLString )
{

}

static QString constructMethodSignature(const YAML::Node & node)
{
    assert(node.Type() == YAML::NodeType::Sequence);

    QString sig( node[0].to<std::string>().c_str() );
    sig.append(": ");

    QString method( node[1].to<std::string>().c_str() );
    sig.append(method);

    int count = node.size();
    if (count > 2) {
        sig.append(" (");
        for (int i = 2; i < count; ++i) {
            if (i > 2) sig.append(", ");
            sig.append( node[i].to<std::string>().c_str() );
        }
        sig.append(")");
    }
    else if (method.endsWith('_')) {
        sig.append(" (value)");
    }

    return sig;
}

void SCIpcServer::methodDefinitionHandler ( const QString & data )
{
    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        assert(doc.Type() == YAML::NodeType::Sequence);

        switch (doc.size()) {
        case 0:
        {
            emit message("Method not defined!");
            return;
        }
        case 1:
        {
            YAML::Node const & entry = doc[0];
            QString sig = constructMethodSignature( entry[0] );
            YAML::Node const & location = entry[1];
            assert(location.Type() == YAML::NodeType::Sequence);
            QString fileName( location[0].to<std::string>().c_str() );
            int pos = location[1].to<int>();
            Main::instance()->documentManager()->open(fileName, pos);
            return;
        }
        default:
        {
            QMenu * menu = new QMenu();
            int idx = 0;
            for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it, ++idx) {
                YAML::Node const & entry = *it;
                assert(entry.Type() == YAML::NodeType::Sequence);
                QString sig = constructMethodSignature( entry[0] );
                QAction * action = menu->addAction(sig);
                action->setData(idx);
            }

            QAction * selectedAction = menu->exec(QCursor::pos());
            if (selectedAction) {
                int selIdx = selectedAction->data().toInt();
                YAML::Node const & location = doc[selIdx][1];
                QString fileName( location[0].to<std::string>().c_str() );
                int pos = location[1].to<int>();
                Main::instance()->documentManager()->open(fileName, pos);
            }
            delete menu;
        }
        }
    }
}

QString constructMethodSignature(const struct YAML::Node &);

}

