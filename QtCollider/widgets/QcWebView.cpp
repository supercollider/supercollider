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

#ifdef SC_USE_QTWEBENGINE

#    include "QcWebView.h"
#    include "../widgets/web_page.hpp"
#    include <QWebEnginePage>
#    include <QWebEngineSettings>
#    include <QAction>
#    include <QMenu>
#    include <QShortcut>
#    include <QKeyEvent>
#    include <QApplication>
#    include <QStyle>

#    if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
#        include <QWebEngineCallback>
#        include <QWebEngineContextMenuData>
#    else
#        include <QWebEngineContextMenuRequest>
#        include <QWebEngineView>
#        include <QWebEngineFindTextResult>
#    endif

namespace QtCollider {

WebView::WebView(QWidget* parent): QWebEngineView(parent), _editable(false) {
    QtCollider::WebPage* page = new WebPage(this);
    setPage(page);
    connectPage(page);

    // Set the style's standard palette to avoid system's palette incoherencies
    // get in the way of rendering web pages
    setPalette(style()->standardPalette());

    setAttribute(Qt::WA_AcceptTouchEvents);

    page->action(QWebEnginePage::Copy)->setShortcut(QKeySequence::Copy);
    page->action(QWebEnginePage::Paste)->setShortcut(QKeySequence::Paste);
    page->action(QWebEnginePage::Reload)->setShortcut(QKeySequence::Refresh);

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded(bool)));
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

    connect(page, &WebPage::renderProcessTerminated, this, &WebView::onRenderProcessTerminated);
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

#    if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    const QWebEngineContextMenuData& contextData = page()->contextMenuData();
    if (!contextData.linkUrl().isEmpty()) {
#    else
    auto contextData = this->lastContextMenuRequest();
    if (!contextData->linkUrl().isEmpty()) {
#    endif
        menu.addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));
        menu.addSeparator();
    }

#    if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    if (contextData.isContentEditable() || !contextData.selectedText().isEmpty()) {
#    else
    if (contextData->isContentEditable() || !contextData->selectedText().isEmpty()) {
#    endif
        menu.addAction(pageAction(QWebEnginePage::Copy));
#    if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        if (contextData.isContentEditable())
#    else
        if (contextData->isContentEditable())
#    endif
            menu.addAction(pageAction(QWebEnginePage::Paste));

        menu.addSeparator();
    }

    menu.addAction(pageAction(QWebEnginePage::Back));
    menu.addAction(pageAction(QWebEnginePage::Forward));
    menu.addAction(pageAction(QWebEnginePage::Reload));

    menu.exec(event->globalPos());
}

// webView's renderer keypresses don't arrive to webView
// duplicate them
bool WebView::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        // takes ownership of newEvent
#    if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        auto newEvent = new QKeyEvent(*static_cast<QKeyEvent*>(event));
#    else
        auto newEvent = static_cast<QKeyEvent*>(event->clone());
#    endif
        QApplication::postEvent(this, newEvent);
    }

    event->ignore();
    return false;
}

// stop keypresses here to avoid duplicates in parents
bool WebView::event(QEvent* ev) {
    if (ev->type() == QEvent::KeyPress)
        return true;

    return QWebEngineView::event(ev);
}

void WebView::pageLoaded(bool ok) { this->focusProxy()->installEventFilter(this); }

void WebView::setEditable(bool b) {
    _editable = b;
    if (_editable) {
        page()->runJavaScript("document.documentElement.contentEditable = true");
    } else {
        page()->runJavaScript("document.documentElement.contentEditable = false");
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

#endif // SC_USE_QTWEBENGINE
