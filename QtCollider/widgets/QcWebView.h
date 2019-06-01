/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineCallback>
#include <QPointer>
#include <QUrl>
#include <QException>

const static int kWebEngineTimeout = 10000;

Q_DECLARE_METATYPE(QUrl)

namespace QtCollider {

class WebPage;
class QcCallback;

class QcCallbackWeakFunctor {
public:
    QcCallbackWeakFunctor(QPointer<QcCallback> cb): _cb(cb) {}

    template <typename RESULT> void operator()(RESULT r) const;

private:
    QPointer<QcCallback> _cb;
};

class QcCallback : public QObject {
    Q_OBJECT

public:
    QcCallback() {}

    template <typename CallbackT> void call(const CallbackT& result) { Q_EMIT(onCalled(result)); }

    QcCallbackWeakFunctor asFunctor() { return QcCallbackWeakFunctor(QPointer<QcCallback>(this)); }

Q_SIGNALS:
    void onCalled(bool);
    void onCalled(const QString&);
    void onCalled(const QVariant&);
    void onCalled(const QUrl&);
};

template <typename RESULT> void QcCallbackWeakFunctor::operator()(RESULT r) const {
    if (_cb) {
        _cb->call(r);
    }
}


class WebView : public QWebEngineView {
    Q_OBJECT

public:
    Q_INVOKABLE void setFontFamily(int genericFontFamily, const QString& fontFamily);
    Q_INVOKABLE void triggerPageAction(int action, bool checked);
    Q_INVOKABLE QAction* pageAction(QWebEnginePage::WebAction) const;

    // QWebEnginePage forwards
    Q_INVOKABLE void setHtml(const QString& html, const QString& baseUrl);
    Q_INVOKABLE void setContent(const QVector<int>& data, const QString& mimeType, const QString& baseUrl);
    Q_INVOKABLE void toHtml(QcCallback* cb) const;
    Q_INVOKABLE void toPlainText(QcCallback* cb) const;
    Q_INVOKABLE void runJavaScript(const QString& script, QcCallback* cb);
    Q_INVOKABLE void setWebAttribute(int attr, bool on);
    Q_INVOKABLE bool testWebAttribute(int attr);
    Q_INVOKABLE void resetWebAttribute(int attr);
    Q_INVOKABLE void navigate(const QString& url);

public Q_SLOTS:
    void findText(const QString& searchText, bool reversed, QcCallback* cb);

Q_SIGNALS:
    void linkActivated(const QString&, int, bool);
    void jsConsoleMsg(const QString&, int, const QString&);
    void reloadTriggered(const QString&);
    void interpret(const QString& code);

    // QWebEnginePage forwards
    void linkHovered(const QString& url);
    void geometryChangeRequested(const QRect& geom);
    void windowCloseRequested();
    void navigationRequested(const QUrl&, int, bool);

    void renderProcessTerminated(int terminationStatus, int exitCode);

    void scrollPositionChanged(const QPointF& position);
    void contentsSizeChanged(const QSizeF& size);
    void audioMutedChanged(bool muted);
    void recentlyAudibleChanged(bool recentlyAudible);

public:
    WebView(QWidget* parent = 0);

    Q_PROPERTY(qreal zoom READ zoomFactor WRITE setZoomFactor);
    Q_PROPERTY(bool hasSelection READ hasSelection);
    Q_PROPERTY(QString selectedText READ selectedText);
    Q_PROPERTY(QString title READ title);

    Q_PROPERTY(bool overrideNavigation READ overrideNavigation WRITE setOverrideNavigation);
    bool overrideNavigation() const;
    void setOverrideNavigation(bool b);

    Q_PROPERTY(QString url READ url WRITE setUrl);
    QString url() const;
    void setUrl(const QString&);

    Q_PROPERTY(bool delegateReload READ delegateReload WRITE setDelegateReload);
    bool delegateReload() const;
    void setDelegateReload(bool);

    Q_PROPERTY(bool enterInterpretsSelection READ interpretSelection WRITE setInterpretSelection);
    bool interpretSelection() const { return _interpretSelection; }
    void setInterpretSelection(bool b) { _interpretSelection = b; }

    Q_PROPERTY(bool editable READ editable WRITE setEditable);
    bool editable() const { return _editable; }
    void setEditable(bool b) {
        _editable = b;
        updateEditable(true);
    }

    // QWebEnginePage properties
    Q_PROPERTY(QString requestedUrl READ requestedUrl)
    QString requestedUrl() const { return page() ? page()->requestedUrl().toString() : QString(); }

    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    QColor backgroundColor() const { return page() ? page()->backgroundColor() : QColor(); }
    void setBackgroundColor(QColor c) {
        if (page())
            page()->setBackgroundColor(c);
    }

    Q_PROPERTY(QSizeF contentsSize READ contentsSize)
    QSizeF contentsSize() const { return page() ? page()->contentsSize() : QSizeF(0, 0); }

    Q_PROPERTY(QPointF scrollPosition READ scrollPosition)
    QPointF scrollPosition() const { return page() ? page()->scrollPosition() : QPointF(0, 0); }

    Q_PROPERTY(bool audioMuted READ isAudioMuted WRITE setAudioMuted)
    bool isAudioMuted() const { return page() ? page()->isAudioMuted() : false; }
    void setAudioMuted(bool m) {
        if (page())
            page()->setAudioMuted(m);
    }

    Q_PROPERTY(bool recentlyAudible READ recentlyAudible)
    bool recentlyAudible() const { return page() ? page()->recentlyAudible() : false; }

    inline static QUrl urlFromString(const QString& str) { return QUrl::fromUserInput(str); }

protected:
    virtual void keyPressEvent(QKeyEvent*);
    virtual void contextMenuEvent(QContextMenuEvent*);

public Q_SLOTS:
    void onPageReload();
    void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus, int);
    void onLinkClicked(const QUrl&, QWebEnginePage::NavigationType, bool);
    void updateEditable(bool);

private:
    void connectPage(QtCollider::WebPage* page);

    bool _interpretSelection;
    bool _editable;
};

} // namespace QtCollider

using namespace QtCollider;
Q_DECLARE_METATYPE(QcCallback*);
