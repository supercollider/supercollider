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
#include "main_window.hpp"
#include "../core/sc_process.hpp"
#include "../core/main.hpp"
#include "QtCollider/widgets/web_page.hpp"

#include <QVBoxLayout>
#include <QToolBar>
#include <QWebSettings>
#include <QWebFrame>
#include <QStyle>
#include <QShortcut>
#include <QApplication>
#include <QDesktopWidget>

namespace ScIDE {

HelpBrowser::HelpBrowser( QWidget * parent ):
    QWidget(parent)
{
    QRect availableScreenRect = qApp->desktop()->availableGeometry(this);
    mSizeHint = QSize( availableScreenRect.width() * 0.4, availableScreenRect.height() * 0.7 );

    QtCollider::WebPage *webPage = new QtCollider::WebPage(this);
    webPage->setDelegateReload(true);
    webPage->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

    mWebView = new QWebView;
    mWebView->setPage( webPage );
    mWebView->settings()->setAttribute( QWebSettings::LocalStorageEnabled, true );

    // NOTE: we assume all web page shortcuts have Qt::WidgetShortcut context
    mWebView->setContextMenuPolicy( Qt::ActionsContextMenu );
    mWebView->addAction( webPage->action( QWebPage::Copy ) );
    mWebView->addAction( webPage->action( QWebPage::Paste ) );

    // Set the style's standard palette to avoid system's palette incoherencies
    // get in the way of rendering web pages
    mWebView->setPalette( style()->standardPalette() );

    mWebView->installEventFilter(this);

    mLoadProgressIndicator = new LoadProgressIndicator;
    mLoadProgressIndicator->setIndent(10);

    QToolBar *toolBar = new QToolBar;
    toolBar->setIconSize( QSize(16,16) );
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
    connect( mWebView, SIGNAL(loadStarted()), mLoadProgressIndicator, SLOT(start()) );
    connect( mWebView, SIGNAL(loadFinished(bool)), mLoadProgressIndicator, SLOT(stop()) );

    connect( webPage->action(QWebPage::Reload), SIGNAL(triggered(bool)), this, SLOT(onReload()) );
    connect( webPage, SIGNAL(jsConsoleMsg(QString,int,QString)),
             this, SLOT(onJsConsoleMsg(QString,int,QString)) );

    ScProcess * scProcess = Main::scProcess();
    connect( scProcess, SIGNAL(response(QString,QString)),
             this, SLOT(onScResponse(QString,QString)) );
    connect( scProcess, SIGNAL(finished(int)), mLoadProgressIndicator, SLOT(stop()) );
    // FIXME: should actually respond to class library shutdown, but we don't have that signal
    connect( scProcess, SIGNAL(classLibraryRecompiled()), mLoadProgressIndicator, SLOT(stop()) );

    mEvaluateShortcut = new QShortcut(this);
    mEvaluateShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( mEvaluateShortcut, SIGNAL(activated()), this, SLOT(evaluateSelection()) );

    applySettings( Main::settings() );
}

void HelpBrowser::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/shortcuts");

    mWebView->pageAction(QWebPage::Copy)
            ->setShortcut( settings->shortcut("editor-copy") );
    mWebView->pageAction(QWebPage::Paste)
            ->setShortcut( settings->shortcut("editor-paste") );
    mEvaluateShortcut->setKey( settings->shortcut("editor-eval-smart") );

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

    static const QStringList nonHelpFileExtensions = QStringList() << ".sc" << ".scd" << ".schelp" << ".txt" << ".rtf";
    static const QString fileScheme("file");

    QString urlString = url.toString();

    foreach ( const QString & extension, nonHelpFileExtensions ) {
        if (urlString.endsWith( extension )) {
            if (url.scheme() == fileScheme) {
                Main::documentManager()->open(url.toLocalFile());
                return;
            }
            break;
        }
    }

    sendRequest( QString("HelpBrowser.goTo(\"%1\")").arg( urlString ) );
}

void HelpBrowser::onReload()
{
    QWebSettings::clearMemoryCaches();
    onLinkClicked( mWebView->url() );
}

bool HelpBrowser::eventFilter(QObject *object, QEvent *event)
{
    if (object == mWebView) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            switch (mouseEvent->button()) {
            case Qt::XButton1:
                mWebView->pageAction(QWebPage::Back)->trigger();
                return true;

            case Qt::XButton2:
                mWebView->pageAction(QWebPage::Forward)->trigger();
                return true;

            default:
                return false;
            }
        }
    }
    return false;
}

void HelpBrowser::sendRequest( const QString &code )
{
    ScProcess *scProcess = Main::scProcess();
    if (scProcess->state() == QProcess::NotRunning) {
        qDebug() << "HelpBrowser: aborting request - sclang not running.";
        MainWindow::instance()->showStatusMessage("Can not use help - interpreter not running!");
        return;
    }

    qDebug() << "sending request...";
    mLoadProgressIndicator->start(tr("Sending request"));
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
