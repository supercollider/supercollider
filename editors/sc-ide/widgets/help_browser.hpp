/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
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

#include "util/docklet.hpp"
#include "QtCollider/widgets/web_page.hpp"

#include <QWebEngineView>
#include <QLabel>
#include <QLineEdit>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QList>
#include <QKeySequence>

namespace ScIDE {

namespace Settings {
class Manager;
}

class HelpBrowserDocklet;
class HelpBrowserFindBox;
class HelpBrowser;

class LoadProgressIndicator : public QLabel {
    Q_OBJECT
public slots:
    void start(const QString& msg = tr("Loading")) {
        mMsg = msg;
        mDotCount = 0;
        mUpdateTimer.start(200, this);
    }
    void stop() {
        mUpdateTimer.stop();
        clear();
    }

protected:
    virtual void timerEvent(QTimerEvent* event) {
        if (event->timerId() != mUpdateTimer.timerId())
            return;

        ++mDotCount;
        if (mDotCount > 6)
            mDotCount = 1;

        QString string(mDotCount, '.');
        string.prepend(mMsg);

        setText(string);
    }

private:
    QBasicTimer mUpdateTimer;
    QString mMsg;
    int mDotCount;
};

class HelpWebPage : public QtCollider::WebPage {
    Q_OBJECT

public:
    HelpWebPage(HelpBrowser* browser);

private:
    HelpBrowser* mBrowser;
};

class HelpBrowser : public QWidget {
    Q_OBJECT

public:
    enum ActionRole {
        GoHome,
        DocClose,
        ZoomIn,
        ZoomOut,
        ResetZoom,
        Evaluate,

        ActionCount
    };

    HelpBrowser(QWidget* parent = 0);

    QSize sizeHint() const { return mSizeHint; }
    QSize minimumSizeHint() const { return QSize(50, 50); }

    void gotoHelpFor(const QString&);
    void gotoHelpForMethod(const QString& className, const QString& methodName);
    QWidget* loadProgressIndicator() { return mLoadProgressIndicator; }

    QUrl url() const { return mWebView->url(); }

    bool helpBrowserHasFocus() const;

public slots:
    void applySettings(Settings::Manager*);
    void goHome();
    void closeDocument();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void evaluateSelection(bool region = false);
    void findText(const QString& text, bool backwards = false);
    bool openDocumentation();
    void openDefinition();
    void openCommandLine();
    void findReferences();
    void onLinkClicked(const QUrl&, QWebEnginePage::NavigationType type, bool isMainFrame);

signals:
    void urlChanged();

private slots:
    void onContextMenuRequest(const QPoint& pos);
    void onReload();
    void onScResponse(const QString& command, const QString& data);
    void onJsConsoleMsg(const QString&, int, const QString&);

private:
    friend class HelpBrowserDocklet;

    void createActions();
    bool eventFilter(QObject* object, QEvent* event);
    void sendRequest(const QString& code);
    QString symbolUnderCursor();

    QWebEngineView* mWebView;
    LoadProgressIndicator* mLoadProgressIndicator;

    QSize mSizeHint;

    QAction* mActions[ActionCount];
};

class HelpBrowserFindBox : public QLineEdit {
    Q_OBJECT

public:
    HelpBrowserFindBox(QWidget* parent = 0);

signals:
    void query(const QString& text, bool backwards = false);

protected:
    virtual bool event(QEvent* event);
};

class HelpBrowserDocklet : public Docklet {
    Q_OBJECT

public:
    explicit HelpBrowserDocklet(QWidget* parent = 0);
    HelpBrowser* browser() { return mHelpBrowser; }

private slots:
    void onInterpreterStart() {
        if (isVisible() && mHelpBrowser->url().isEmpty())
            mHelpBrowser->goHome();
    }

private:
    HelpBrowser* mHelpBrowser;
    HelpBrowserFindBox* mFindBox;
};

} // namespace ScIDE
