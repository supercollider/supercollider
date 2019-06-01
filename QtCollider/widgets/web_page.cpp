/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "web_page.hpp"

#include <QApplication>
#include <QClipboard>

namespace QtCollider {

void WebPage::triggerAction(WebAction action, bool checked) {
    switch (action) {
    case QWebEnginePage::Reload:
        if (_delegateReload)
            return;
        break;
    case QWebEnginePage::Copy:
        // ignore text formatting, copy only plain text:
        QApplication::clipboard()->setText(selectedText());
        return;
    default:
        break;
    }

    QWebEnginePage::triggerAction(action, checked);
}


void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber,
                                       const QString& sourceID) {
    Q_EMIT(jsConsoleMsg(message, lineNumber, sourceID));
}

bool WebPage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    if (type == QWebEnginePage::NavigationTypeLinkClicked) {
        Q_EMIT(navigationRequested(url, type, isMainFrame));
        if (_delegateNavigation) {
            return false;
        }
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

} // namespace QtCollider
