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

#define QT_NO_DEBUG_OUTPUT

#include "help_browser.hpp"
#include "../core/sc_process.hpp"
#include "../core/main.hpp"

#include <QVBoxLayout>
#include <QToolBar>

namespace ScIDE {

HelpBrowser::HelpBrowser( QWidget * parent ):
    QWidget(parent)
{
    mWebView = new QWebView;
    mWebView->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

    QToolBar *toolBar = new QToolBar;
    QAction *action = toolBar->addAction("Home");
    connect( action, SIGNAL(triggered()), this, SLOT(goHome()) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Back) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Forward) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Reload) );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(toolBar);
    layout->addWidget(mWebView);

    setLayout(layout);

    connect( mWebView, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
    connect( Main::scProcess(), SIGNAL(response(QString,QString)), this, SLOT(onScResponse(QString,QString)) );
}

void HelpBrowser::goHome()
{
    Main::scProcess()->evaluateCode("HelpBrowser.goHome", true);
}

void HelpBrowser::onLinkClicked( const QUrl & url )
{
    qDebug() << "link clicked:" << url;

    Main::scProcess()->evaluateCode(
                QString("HelpBrowser.goTo(\"%1\")").arg(url.toString()) );
}

void HelpBrowser::onScResponse( const QString & command, const QString & data )
{
    const QString openHelpUrlString = "openHelpUrl";

    if (command != openHelpUrlString)
        return;

    qDebug() << "HelpBrowser: got response:" << data;

    QString urlString = data;
    // undress YAML string:
    urlString.remove(0,1).chop(1);

    mWebView->setUrl( urlString );

    emit urlChanged();
}

} // namespace ScIDE
