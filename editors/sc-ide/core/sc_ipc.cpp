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

#include "main.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

namespace ScIDE {

void SCIpcServer::tryEvaluateReceivedData ( void )
{
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

    emit command(selector, message);
}

}

