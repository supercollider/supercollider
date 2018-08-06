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

#include "WebSocketTransport.hpp"

#include <QJsonDocument>
#include <QWebSocket>

namespace ScIDE {

WebSocketTransport::WebSocketTransport(QWebSocket *socket)
        : QWebChannelAbstractTransport(socket), m_socket(socket) {
    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketTransport::textMessageReceived);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketTransport::deleteLater);
}

WebSocketTransport::~WebSocketTransport() {
    m_socket->deleteLater();
}

void WebSocketTransport::sendMessage(const QJsonObject &message) {
    QJsonDocument doc(message);
    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void WebSocketTransport::textMessageReceived(const QString &messageData) {
    QJsonParseError error;
    QJsonDocument message = QJsonDocument::fromJson(messageData.toUtf8(), &error);
    if (error.error) {
        qWarning() << "Failed to parse text message as JSON object:" << messageData
                   << "Error is:" << error.errorString();
        return;
    } else if (!message.isObject()) {
        qWarning() << "Received JSON message that is not an object: " << messageData;
        return;
    }
    emit messageReceived(message.object(), this);
}

} // namespace ScIDE
