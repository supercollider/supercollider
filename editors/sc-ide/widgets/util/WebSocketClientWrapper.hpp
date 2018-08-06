/*
    SuperCollider Qt IDE
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

#pragma once

#include <QWebSocketServer>
#include <QObject>

#include "WebSocketTransport.hpp"

namespace ScIDE {

class WebSocketClientWrapper : public QObject
{
    Q_OBJECT
public:
    WebSocketClientWrapper(QWebSocketServer *server, QObject *parent = nullptr) :
        QObject(parent),
        m_server(server)
    {
        connect(server, &QWebSocketServer::newConnection,
            this, &WebSocketClientWrapper::handleNewConnection);
    }

signals:
    void clientConnected(WebSocketTransport *client);
private slots:
    void handleNewConnection() {
        emit clientConnected(new WebSocketTransport(m_server->nextPendingConnection()));
    }
private:
    QWebSocketServer *m_server;
};

} // namespace ScIDE
