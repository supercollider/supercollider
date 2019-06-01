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

#include "QcWebView.h"
#include "../widgets/web_page.hpp"
#include "../QcWidgetFactory.h"
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineContextMenuData>
#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>
#include <QWebEngineCallback>

QC_DECLARE_QWIDGET_FACTORY(WebView);
QC_DECLARE_QOBJECT_FACTORY(QcCallback);

namespace QtCollider {

WebView::WebView(QWidget* parent): QWebEngineView(parent), _interpretSelection(false), _editable(false) {
    QtCollider::WebPage* page = new WebPage(this);
    page->setDelegateReload(true);
    setPage(page);
    connectPage(page);

    // Set the style's standard palette to avoid system's palette incoherencies
    // get in the way of rendering web pages
    setPalette(style()->standardPalette());

    setAttribute(Qt::WA_AcceptTouchEvents);

    page->action(QWebEnginePage::Copy)->setShortcut(QKeySequence::Copy);
    page->action(QWebEnginePage::Paste)->setShortcut(QKeySequence::Paste);
    page->action(QWebEnginePage::Reload)->setShortcut(QKeySequence::Refresh);

    connect(this, SIGNAL(interpret(QString)), qApp, SLOT(interpret(QString)), Qt::QueuedConnection);

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(updateEditable(bool)));
}

void WebView::connectPage(QtCollider::WebPage* page) {
    connect(page, SIGNAL(jsConsoleMsg(const QString&, int, const QString&)), this,
            SIGNAL(jsConsoleMsg(const QString&, int, const QString&)));

    connect(page, SIGNAL(linkHovered(const QString&)), this, SIGNAL(linkHovered(const QString&)));

    connect(page, SIGNAL(geometryChangeRequested(const QRect&)), this, SIGNAL(geometryChangeRequested(const QRect&)));

    connect(page, SIGNAL(windowCloseRequested()), this, SIGNAL(windowCloseRequested()));

    connect(page, SIGNAL(scrollPositionChanged(const QPointF&)), this, SIGNAL(scrollPositionChanged(const QPointF&)));

    connect(page, SIGNAL(contentsSizeChanged(const QSizeF&)), this, SIGNAL(contentsSizeChanged(const QSizeF&)));

    connect(page, SIGNAL(audioMutedChanged(bool)), this, SIGNAL(audioMutedChanged(bool)));

    connect(page, SIGNAL(recentlyAudibleChanged(bool)), this, SIGNAL(recentlyAudibleChanged(bool)));

    connect(page, SIGNAL(navigationRequested(QUrl, QWebEnginePage::NavigationType, bool)), this,
            SLOT(onLinkClicked(QUrl, QWebEnginePage::NavigationType, bool)));

    connect(page->action(QWebEnginePage::Reload), SIGNAL(triggered(bool)), this, SLOT(onPageReload()));

    connect(page, SIGNAL(renderProcessTerminated(RenderProcessTerminationStatus, int)), this,
            SLOT(onRenderProcessTerminated(RenderProcessTerminationStatus, int)));
}

void WebView::onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus status, int code) {
    Q_EMIT(renderProcessTerminated((int)status, code));
}

void WebView::triggerPageAction(int action, bool checked) {
    QWebEngineView::triggerPageAction((QWebEnginePage::WebAction)action, checked);
}

QString WebView::url() const { return QWebEngineView::url().toString(); }

void WebView::setUrl(const QString& str) { load(urlFromString(str)); }

bool WebView::delegateReload() const {
    WebPage* p = qobject_cast<QtCollider::WebPage*>(page());
    Q_ASSERT(p);
    return p->delegateReload();
}

void WebView::setDelegateReload(bool flag) {
    WebPage* p = qobject_cast<QtCollider::WebPage*>(page());
    Q_ASSERT(p);
    p->setDelegateReload(flag);
}

void WebView::setFontFamily(int generic, const QString& specific) {
    settings()->setFontFamily((QWebEngineSettings::FontFamily)generic, specific);
}

QAction* WebView::pageAction(QWebEnginePage::WebAction action) const { return QWebEngineView::pageAction(action); }

void WebView::setHtml(const QString& html, const QString& baseUrl) {
    if (page()) {
        page()->setHtml(html, baseUrl);
    }
}

void WebView::setContent(const QVector<int>& data, const QString& mimeType, const QString& baseUrl) {
    if (page()) {
        QByteArray byteData;
        for (int val : data) {
            byteData.push_back((char)val);
        }
        page()->setContent(byteData, mimeType, baseUrl);
    }
}

void WebView::toHtml(QcCallback* cb) const {
    if (page()) {
        if (cb) {
            page()->toHtml(cb->asFunctor());
        } else {
            page()->toHtml([](const QString&) {});
        }
    } else {
        cb->asFunctor()(QString());
    }
}

void WebView::toPlainText(QcCallback* cb) const {
    if (page()) {
        if (cb) {
            page()->toPlainText(cb->asFunctor());
        } else {
            page()->toPlainText([](const QString&) {});
        }
    } else {
        cb->asFunctor()(QString());
    }
}

void WebView::runJavaScript(const QString& script, QcCallback* cb) {
    if (page()) {
        if (cb) {
            page()->runJavaScript(script, cb->asFunctor());
        } else {
            page()->runJavaScript(script, [](const QVariant&) {});
        }
    } else {
        cb->asFunctor()(QString());
    }
}

void WebView::setWebAttribute(int attr, bool on) {
    if (page()) {
        page()->settings()->setAttribute((QWebEngineSettings::WebAttribute)attr, on);
    }
}

bool WebView::testWebAttribute(int attr) {
    return page() ? page()->settings()->testAttribute((QWebEngineSettings::WebAttribute)attr) : false;
}

void WebView::resetWebAttribute(int attr) {
    if (page()) {
        page()->settings()->resetAttribute((QWebEngineSettings::WebAttribute)attr);
    }
}

void WebView::navigate(const QString& urlString) {
    QUrl url(urlString);
    this->load(url);
}

void WebView::findText(const QString& searchText, bool reversed, QcCallback* cb) {
    QWebEnginePage::FindFlags flags;
    if (reversed)
        flags |= QWebEnginePage::FindBackward;

    if (!cb) {
        QWebEngineView::findText(searchText, flags);
    } else {
        QWebEngineView::findText(searchText, flags, cb->asFunctor());
    }
}

void WebView::onPageReload() { Q_EMIT(reloadTriggered(url())); }

void WebView::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu;

    const QWebEngineContextMenuData& contextData = page()->contextMenuData();

    if (!contextData.linkUrl().isEmpty()) {
        menu.addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));
        menu.addSeparator();
    }

    if (contextData.isContentEditable() || !contextData.selectedText().isEmpty()) {
        menu.addAction(pageAction(QWebEnginePage::Copy));
        if (contextData.isContentEditable()) {
            menu.addAction(pageAction(QWebEnginePage::Paste));
        }
        menu.addSeparator();
    }

    menu.addAction(pageAction(QWebEnginePage::Back));
    menu.addAction(pageAction(QWebEnginePage::Forward));
    menu.addAction(pageAction(QWebEnginePage::Reload));

    menu.exec(event->globalPos());
}

void WebView::keyPressEvent(QKeyEvent* e) {
    int key = e->key();
    int mods = e->modifiers();

    if (_interpretSelection
        && (key == Qt::Key_Enter || (key == Qt::Key_Return && mods & (Qt::ControlModifier | Qt::ShiftModifier)))) {
        QString selection = selectedText();
        if (!selection.isEmpty()) {
            Q_EMIT(interpret(selection));
            return;
        }
    }

    QWebEngineView::keyPressEvent(e);
}

void WebView::updateEditable(bool ok) {
    if (ok) {
        if (_editable) {
            page()->runJavaScript("document.documentElement.contentEditable = true");
        } else {
            page()->runJavaScript("document.documentElement.contentEditable = false");
        }
    }
}

bool WebView::overrideNavigation() const {
    WebPage* p = qobject_cast<WebPage*>(page());
    return p ? p->delegateNavigation() : false;
}

void WebView::setOverrideNavigation(bool b) {
    WebPage* p = qobject_cast<WebPage*>(page());
    if (p) {
        p->setDelegateNavigation(b);
    }
}

void WebView::onLinkClicked(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame) {
    Q_EMIT(navigationRequested(url, (int)type, isMainFrame));
}

} // namespace QtCollider
