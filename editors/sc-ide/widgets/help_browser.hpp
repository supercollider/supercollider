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

#ifndef SCIDE_WIDGETS_HELP_BROWSER_HPP_INCLUDED
#define SCIDE_WIDGETS_HELP_BROWSER_HPP_INCLUDED

#include <QWebView>
#include <QDockWidget>
#include <QShortcut>
#include <QLabel>
#include <QBasicTimer>
#include <QTimerEvent>

namespace ScIDE {

namespace Settings { class Manager; }

class LoadProgressIndicator : public QLabel
{
    Q_OBJECT
public slots:
    void start( const QString & msg = QString("Loading") )
    {
        mMsg = msg;
        mDotCount = 0;
        mUpdateTimer.start(200, this);
    }
    void stop()
    {
        mUpdateTimer.stop(); clear();
    }

protected:
    virtual void timerEvent( QTimerEvent *event )
    {
        if (event->timerId() != mUpdateTimer.timerId())
            return;

        ++mDotCount;
        if (mDotCount > 10)
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

class HelpBrowser : public QWidget
{
    Q_OBJECT

public:
    HelpBrowser( QWidget * parent = 0 );

    QSize sizeHint() const { return mSizeHint; }
    QSize minimumSizeHint() const { return QSize(50,50); }

    void gotoHelpFor( const QString & );
    void gotoHelpForMethod( const QString & className, const QString & methodName );

public slots:
    void applySettings( Settings::Manager * );
    void goHome();
    void evaluateSelection();

signals:
    void urlChanged();

private slots:
    void onLinkClicked( const QUrl & );
    void onReload();
    void onScResponse( const QString & command, const QString & data );
    void onJsConsoleMsg(const QString &, int, const QString & );

private:
    bool eventFilter( QObject * object, QEvent * event);
    void sendRequest( const QString &code );

    QWebView *mWebView;

    QShortcut *mEvaluateShortcut;
    LoadProgressIndicator *mLoadProgressIndicator;
    QSize mSizeHint;
};

class HelpBrowserDockable : public QDockWidget
{
public:
    HelpBrowserDockable( QWidget *parent = 0 ):
        QDockWidget("Help browser", parent)
    {
        mHelpBrowser = new HelpBrowser;

        setAllowedAreas(Qt::AllDockWidgetAreas);
        setFeatures(DockWidgetFloatable | DockWidgetMovable | DockWidgetClosable);
        setWidget(mHelpBrowser);

        connect( mHelpBrowser, SIGNAL(urlChanged()), this, SLOT(show()) );
    }

    HelpBrowser *browser() { return mHelpBrowser; }

private:
    HelpBrowser *mHelpBrowser;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_HELP_BROWSER_HPP_INCLUDED
