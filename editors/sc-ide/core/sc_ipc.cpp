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

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

namespace ScIDE {

void SCIpcServer::tryEvaluateReceivedData ( void )
{
    static QString classes("classes");
    static QString symbolTable("symbolTable");
    static QString classDefinitions("classDefinitions");

    QBuffer receivedData ( &mReceivedData );
    receivedData.open ( QIODevice::ReadOnly );

    QDataStream in ( &receivedData );
    in.setVersion ( QDataStream::Qt_4_6 );
    QString selector, message;
    in >> selector;
    if ( in.status() != QDataStream::Ok )
        return;

    qDebug("%s", selector.toStdString().c_str());

    in >> message;
    if ( in.status() != QDataStream::Ok )
        return;

    mReceivedData.remove ( 0, receivedData.pos() );

    qDebug("%s", message.toStdString().c_str());

    if ( selector == classDefinitions )
        openClassDefinitionHandler(message);
    else if ( selector == classes )
        classesHandler(message);
    else if ( selector == symbolTable )
        symbolTableHandler(message);
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

        for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it) {
            assert(doc.Type() == YAML::NodeType::Sequence);
            std::string name, path;
            int charPosition;

            YAML::Node const & entry = *it;

            name = entry[0].to<std::string>();
            path = entry[1].to<std::string>();
            charPosition = entry[2].to<int>() - 1; // position is one off

            // LATER: we want to show a popup list so that the user can select which definition to open
            Main::instance()->documentManager()->open(QString(path.c_str()), charPosition);
            return;
        }
    }

}

void SCIpcServer::symbolTableHandler ( const QString& YAMLString )
{

}


}

