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

namespace ScIDE {

class ScRequest;

namespace Settings { class Manager; }

class HelpBrowser : public QWidget
{
    Q_OBJECT

public:
    HelpBrowser( QWidget * parent = 0 );

    QSize sizeHint() const
    {
        return QSize(500,300);
    }

public slots:
    void goHome();
    void applySettings( Settings::Manager * );

signals:
    void urlChanged();

private slots:
    void onLinkClicked( const QUrl & );
    void onReload();
    void onScResponse( const QString & command, const QString & data );

private:
    QWebView *mWebView;

    ScRequest *mRequest;
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
