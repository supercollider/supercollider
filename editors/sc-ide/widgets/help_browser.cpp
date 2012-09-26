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
#include "QtCollider/widgets/web_page.hpp"

#include <QVBoxLayout>
#include <QToolBar>
#include <QWebSettings>
#include <QWebFrame>
#include <QStyle>
#include <QShortcut>

namespace ScIDE {

HelpBrowser::HelpBrowser( QWidget * parent ):
    QWidget(parent)
{
    QtCollider::WebPage *webPage = new QtCollider::WebPage(this);
    webPage->setDelegateReload(true);
    webPage->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

    mWebView = new QWebView;
    mWebView->setPage( webPage );

    // NOTE: we assume all web page shortcuts have Qt::WidgetShortcut context
    mWebView->setContextMenuPolicy( Qt::ActionsContextMenu );
    mWebView->addAction( webPage->action( QWebPage::Copy ) );
    mWebView->addAction( webPage->action( QWebPage::Paste ) );

    // Set the style's standard palette to avoid system's palette incoherencies
    // get in the way of rendering web pages
    mWebView->setPalette( style()->standardPalette() );

    mLoadProgressIndicator = new LoadProgressIndicator;
    mLoadProgressIndicator->setIndent(10);

    QToolBar *toolBar = new QToolBar;
    QAction *action = toolBar->addAction("Home");
    connect( action, SIGNAL(triggered()), this, SLOT(goHome()) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Back) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Forward) );
    toolBar->addAction( mWebView->pageAction(QWebPage::Reload) );
    toolBar->addWidget(mLoadProgressIndicator);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(toolBar);
    layout->addWidget(mWebView);
    setLayout(layout);

    connect( mWebView, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
    connect( webPage->action(QWebPage::Reload), SIGNAL(triggered(bool)), this, SLOT(onReload()) );
    connect( webPage, SIGNAL(jsConsoleMsg(QString,int,QString)),
             this, SLOT(onJsConsoleMsg(QString,int,QString)) );
    connect( Main::scProcess(), SIGNAL(response(QString,QString)),
             this, SLOT(onScResponse(QString,QString)) );

    mEvaluateShortcut = new QShortcut(this);
    mEvaluateShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( mEvaluateShortcut, SIGNAL(activated()), this, SLOT(evaluateSelection()) );

    applySettings( Main::settings() );
}

void HelpBrowser::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/shortcuts");

    mWebView->pageAction(QWebPage::Copy)
            ->setShortcut( Main::settings()->shortcut("copy") );
    mWebView->pageAction(QWebPage::Paste)
            ->setShortcut( Main::settings()->shortcut("paste") );
    mEvaluateShortcut->setKey( Main::settings()->shortcut("evaluate_selection,_line_or_region") );

    settings->endGroup();

    QString codeFontFamily = settings->codeFont().family();
    mWebView->settings()->setFontFamily( QWebSettings::FixedFont, codeFontFamily );
}

void HelpBrowser::goHome()
{
    static QString code( "HelpBrowser.goHome" );
    sendRequest(code);
}

void HelpBrowser::gotoHelpFor( const QString & symbol )
{
    QString code = QString("HelpBrowser.openHelpFor(\"%1\")").arg(symbol);
    sendRequest(code);
}

void HelpBrowser::gotoHelpForMethod( const QString & className, const QString & methodName )
{
    QString code = QString("HelpBrowser.openHelpForMethod( %1.findMethod(\\%2) )").arg(className, methodName);
    sendRequest(code);
}

void HelpBrowser::onLinkClicked( const QUrl & url )
{
    qDebug() << "link clicked:" << url;

    sendRequest( QString("HelpBrowser.goTo(\"%1\")").arg(url.toString()) );
}

void HelpBrowser::onReload()
{
    QWebSettings::clearMemoryCaches();
    onLinkClicked( mWebView->url() );
}

void HelpBrowser::sendRequest( const QString &code )
{
    mLoadProgressIndicator->start();
    Main::scProcess()->evaluateCode( code, true );
}

void HelpBrowser::onScResponse( const QString & command, const QString & data )
{
    const QString openHelpUrlString = "openHelpUrl";

    if (command != openHelpUrlString)
        return;

    qDebug() << "HelpBrowser: got response:" << data;

    mLoadProgressIndicator->stop();

    QString urlString = data;
    // undress YAML string:
    urlString.remove(0,1).chop(1);

    mWebView->load( urlString );

    emit urlChanged();
}

void HelpBrowser::evaluateSelection()
{
    static const QString javaScript("selectLine()");
    QWebFrame *frame = mWebView->page()->currentFrame();
    if( frame ) frame->evaluateJavaScript( javaScript );

    QString code( mWebView->selectedText() );
    if (!code.isEmpty())
        Main::scProcess()->evaluateCode(code);
}

void HelpBrowser::onJsConsoleMsg(const QString &arg1, int arg2, const QString & arg3 )
{
    qWarning() << "*** ERROR in JavaScript:" << arg1;
    qWarning() << "* line:" << arg2;
    qWarning() << "* source ID:" << arg3;
}

} // namespace ScIDE
