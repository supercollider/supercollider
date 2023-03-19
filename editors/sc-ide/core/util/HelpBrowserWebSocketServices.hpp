#pragma once

#ifdef SC_USE_QTWEBENGINE
#    include <QWebChannel>
#    include "../../widgets/util/WebSocketClientWrapper.hpp"
#    include "../../widgets/util/WebSocketTransport.hpp"
#    include "../../widgets/util/IDEWebChannelWrapper.hpp"

namespace ScIDE {

/** RAII class to encapsulate websocket services needed by the help browser.
 */
class HelpBrowserWebSocketServices {
public:
    HelpBrowserWebSocketServices(HelpBrowser* browser) {
        m_server = new QWebSocketServer("SCIDE HelpBrowser Server", QWebSocketServer::NonSecureMode);
        if (m_server->listen(QHostAddress::LocalHost)) {
            browser->setServerPort(m_server->serverPort());

            // setup comm channel
            m_clientWrapper = new WebSocketClientWrapper(m_server);
            m_channel = new QWebChannel();
            QObject::connect(m_clientWrapper, &WebSocketClientWrapper::clientConnected, m_channel,
                             &QWebChannel::connectTo);

            // publish IDE interface
            m_ideWrapper = new IDEWebChannelWrapper(browser);
            m_channel->registerObject("IDE", m_ideWrapper);
        } else {
            qWarning("Failed to set up help browser web socket.");
            // [don't take up heap space with a useless object]
            delete m_server;
            m_server = nullptr;
        }
    }

    ~HelpBrowserWebSocketServices() {
        delete m_server;
        delete m_clientWrapper;
        delete m_channel;
        delete m_ideWrapper;
    }

private:
    QWebSocketServer* m_server; // non-null only if server listens successfully
    WebSocketClientWrapper* m_clientWrapper;
    QWebChannel* m_channel;
    IDEWebChannelWrapper* m_ideWrapper;
};

} // namespace ScIDE

#endif // SC_USE_QTWEBENGINE
