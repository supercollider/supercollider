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

#pragma once

#include <QWebEnginePage>

namespace QtCollider {

class WebPage : public QWebEnginePage {
    Q_OBJECT

public:
    WebPage(QObject* parent): QWebEnginePage(parent), _delegateReload(false), _delegateNavigation(false) {}

    virtual void triggerAction(WebAction action, bool checked = false) override;

    virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber,
                                          const QString& sourceID) override;

    bool delegateReload() const { return _delegateReload; }
    void setDelegateReload(bool flag) { _delegateReload = flag; }

    bool delegateNavigation() const { return _delegateNavigation; }
    void setDelegateNavigation(bool flag) { _delegateNavigation = flag; }

    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;

Q_SIGNALS:
    void jsConsoleMsg(const QString&, int, const QString&);
    void navigationRequested(const QUrl& url, QWebEnginePage::NavigationType, bool);

private:
    bool _delegateReload;
    bool _delegateNavigation;
};

} // namespace QtCollider
