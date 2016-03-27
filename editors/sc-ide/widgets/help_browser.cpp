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
#include "../core/util/overriding_action.hpp"
#include "QtCollider/widgets/web_page.hpp"
#include "QtCollider/hacks/hacks_qt.hpp"

#include <QVBoxLayout>
#include <QToolBar>
#include <QWebEngineSettings>
#include <QAction>
#include <QMenu>
#include <QStyle>
#include <QShortcut>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QKeyEvent>

namespace ScIDE {

HelpBrowser::HelpBrowser( QWidget * parent ):
    QWidget(parent)
{
    QRect availableScreenRect = qApp->desktop()->availableGeometry(this);
    mSizeHint = QSize( availableScreenRect.width() * 0.4, availableScreenRect.height() * 0.7 );

    QtCollider::WebPage *webPage = new QtCollider::WebPage(this);
    webPage->setDelegateReload(true);
    connect( webPage, &QtCollider::WebPage::requestUrl, this, [this] (QUrl url) {
        this->onLinkClicked(url);
    });


    mWebEngineView = new QWebEngineView;
    mWebEngineView->setPage( webPage );
    mWebEngineView->settings()->setAttribute( QWebEngineSettings::LocalStorageEnabled, true );
    mWebEngineView->setContextMenuPolicy( Qt::CustomContextMenu );

    // Set the style's standard palette to avoid system's palette incoherencies
    // get in the way of rendering web pages
    mWebEngineView->setPalette( style()->standardPalette() );

    mWebEngineView->installEventFilter(this);

    mLoadProgressIndicator = new LoadProgressIndicator;
    mLoadProgressIndicator->setIndent(10);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mWebEngineView);
    setLayout(layout);

    connect( mWebEngineView, SIGNAL(loadStarted()), mLoadProgressIndicator, SLOT(start()) );
    connect( mWebEngineView, SIGNAL(loadFinished(bool)), mLoadProgressIndicator, SLOT(stop()) );
    connect( mWebEngineView, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(onContextMenuRequest(QPoint)) );

    connect( webPage->action(QWebEnginePage::Reload), SIGNAL(triggered(bool)), this, SLOT(onReload()) );
    connect( webPage, SIGNAL(jsConsoleMsg(QString,int,QString)),
             this, SLOT(onJsConsoleMsg(QString,int,QString)) );

    ScProcess * scProcess = Main::scProcess();
    connect( scProcess, SIGNAL(response(QString,QString)),
             this, SLOT(onScResponse(QString,QString)) );
    connect( scProcess, SIGNAL(finished(int)), mLoadProgressIndicator, SLOT(stop()) );
    // FIXME: should actually respond to class library shutdown, but we don't have that signal
    connect( scProcess, SIGNAL(classLibraryRecompiled()), mLoadProgressIndicator, SLOT(stop()) );

    createActions();

    applySettings( Main::settings() );

    setFocusProxy(mWebEngineView);
}

void HelpBrowser::createActions()
{
    QAction * action;
    OverridingAction *ovrAction;

    mActions[GoHome] = action = new QAction(tr("Home"), this);
    connect( action, SIGNAL(triggered()), this, SLOT(goHome()) );

    mActions[DocClose] = ovrAction = new OverridingAction(tr("Close"), this);
    connect( ovrAction, SIGNAL(triggered()), this, SLOT(closeDocument()) );
    ovrAction->addToWidget(mWebEngineView);

    mActions[ZoomIn] = ovrAction = new OverridingAction(tr("Zoom In"), this);
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    ovrAction->addToWidget(mWebEngineView);

    mActions[ZoomOut] = ovrAction = new OverridingAction(tr("Zoom Out"), this);
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
    ovrAction->addToWidget(mWebEngineView);

    mActions[ResetZoom] = ovrAction = new OverridingAction(tr("Reset Zoom"), this);
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(resetZoom()));
    ovrAction->addToWidget(mWebEngineView);

    mActions[Evaluate] = ovrAction = new OverridingAction(tr("Evaluate as Code"), this);
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(evaluateSelection()));
    ovrAction->addToWidget(mWebEngineView);

    // For the sake of display:
    mWebEngineView->pageAction(QWebEnginePage::Copy)->setShortcut( QKeySequence::Copy );
    mWebEngineView->pageAction(QWebEnginePage::Paste)->setShortcut( QKeySequence::Paste );
}

void HelpBrowser::applySettings( Settings::Manager *settings )
{
    settings->beginGroup("IDE/shortcuts");

    mActions[DocClose]->setShortcut( settings->shortcut("ide-document-close") );

    mActions[ZoomIn]->setShortcut( settings->shortcut("editor-enlarge-font") );

    mActions[ZoomOut]->setShortcut( settings->shortcut("editor-shrink-font") );

    mActions[ResetZoom]->setShortcut( settings->shortcut("editor-reset-font-size") );

    QList<QKeySequence> evalShortcuts;
    evalShortcuts.append( settings->shortcut("editor-eval-smart") );
    evalShortcuts.append( settings->shortcut("editor-eval-line") );
    evalShortcuts.append( QKeySequence(Qt::Key_Enter) );
    mActions[Evaluate]->setShortcuts( evalShortcuts );

    settings->endGroup();

    QString codeFontFamily = settings->codeFont().family();
    mWebEngineView->settings()->setFontFamily( QWebEngineSettings::FixedFont, codeFontFamily );
}

void HelpBrowser::goHome()
{
    static QString code( "HelpBrowser.goHome" );
    sendRequest(code);
}

void HelpBrowser::closeDocument()
{
    MainWindow::instance()->helpBrowserDocklet()->close();
}

void HelpBrowser::gotoHelpFor( const QString & symbol )
{
    QString code = QStringLiteral("HelpBrowser.openHelpFor(\"%1\")").arg(symbol);
    sendRequest(code);
}

void HelpBrowser::gotoHelpForMethod( const QString & className, const QString & methodName )
{
    QString code = QStringLiteral("HelpBrowser.openHelpForMethod( %1.findMethod(\\%2) )").arg(className, methodName);
    sendRequest(code);
}

void HelpBrowser::onLinkClicked( const QUrl & url )
{
    qDebug() << "link clicked:" << url;

    static const QStringList nonHelpFileExtensions = QStringList() << ".sc" << ".scd" << ".schelp" << ".txt" << ".rtf";

    QString urlString = url.toString();

    foreach ( const QString & extension, nonHelpFileExtensions ) {
        if (urlString.endsWith( extension )) {
            if ( QURL_IS_LOCAL_FILE(url) ) {
                Main::documentManager()->open(url.toLocalFile(), -1, 0, false );
                return;
            }
            break;
        }
    }

    sendRequest( QStringLiteral("SCDoc.prepareHelpForURL( URI(\"%1\") )").arg( urlString ) );
}

void HelpBrowser::onReload()
{
    onLinkClicked( mWebEngineView->url() );
}

void HelpBrowser::zoomIn()
{
    qreal zoomFactor = mWebEngineView->zoomFactor();
    zoomFactor = qMin( zoomFactor + 0.1, 2.0 );
    mWebEngineView->setZoomFactor(zoomFactor);
}

void HelpBrowser::zoomOut()
{
    qreal zoomFactor = mWebEngineView->zoomFactor();
    zoomFactor = qMax( zoomFactor - 0.1, 0.1 );
    mWebEngineView->setZoomFactor(zoomFactor);
}

void HelpBrowser::resetZoom()
{
    mWebEngineView->setZoomFactor(1.0);
}

void HelpBrowser::findText( const QString & text, bool backwards )
{
    QWebEnginePage::FindFlags flags;
    if (backwards)
        flags |= QWebEnginePage::FindFlags( QWebEnginePage::FindBackward );

    mWebEngineView->findText( text, flags );
}

bool HelpBrowser::eventFilter(QObject *object, QEvent *event)
{
    if (object == mWebEngineView) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);
            switch (mouseEvent->button()) {
            case Qt::XButton1:
                mWebEngineView->pageAction(QWebEnginePage::Back)->trigger();
                return true;

            case Qt::XButton2:
                mWebEngineView->pageAction(QWebEnginePage::Forward)->trigger();
                return true;

            default:
                break;
            }
            break;
        }
        case QEvent::ShortcutOverride: {
            QKeyEvent * kevent = static_cast<QKeyEvent*>(event);
            if ( kevent == QKeySequence::Copy ||
                 kevent == QKeySequence::Paste )
            {
                kevent->accept();
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return false;
}

void HelpBrowser::sendRequest( const QString &code )
{
    ScProcess *scProcess = Main::scProcess();
    if (scProcess->state() == QProcess::NotRunning) {
        qDebug() << "HelpBrowser: aborting request - sclang not running.";
        MainWindow::instance()->showStatusMessage(
                    tr("Can not use help - interpreter not running!"));
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

    mWebEngineView->load( urlString );
  
    HelpBrowserDocklet *helpDock = MainWindow::instance()->helpBrowserDocklet();
    if (helpDock) helpDock->focus();

    emit urlChanged();
}

void HelpBrowser::evaluateSelection()
{
    static const QString javaScript("selectLine()");
    mWebEngineView->page()->runJavaScript( javaScript );

    QString code( mWebEngineView->selectedText() );
    if (!code.isEmpty())
        Main::scProcess()->evaluateCode(code);
}

void HelpBrowser::onJsConsoleMsg(const QString &arg1, int arg2, const QString & arg3 )
{
    qWarning() << "*** ERROR in JavaScript:" << arg1;
    qWarning() << "* line:" << arg2;
    qWarning() << "* source ID:" << arg3;
}

void HelpBrowser::onContextMenuRequest( const QPoint & pos )
{
    QMenu menu;

//    if (!hitTest.linkElement().isNull()) {
//        menu.addAction( mWebEngineView->pageAction(QWebEnginePage::CopyLinkToClipboard) );
//    }

    menu.addSeparator();

    if( mWebEngineView->page()->hasSelection() ) {
        menu.addAction( mWebEngineView->pageAction(QWebEnginePage::Copy) );
        menu.addAction( mWebEngineView->pageAction(QWebEnginePage::Paste) );
        menu.addAction( mActions[Evaluate] );
    }

    menu.addSeparator();

    menu.addAction( mWebEngineView->pageAction(QWebEnginePage::Back) );
    menu.addAction( mWebEngineView->pageAction(QWebEnginePage::Forward) );
    menu.addAction( mWebEngineView->pageAction(QWebEnginePage::Reload) );

    menu.addSeparator();

    menu.addAction( mActions[ZoomIn] );
    menu.addAction( mActions[ZoomOut] );
    menu.addAction( mActions[ResetZoom] );

    menu.addAction( mActions[DocClose] );

    menu.exec( mWebEngineView->mapToGlobal(pos) );
}

QString HelpBrowser::symbolUnderCursor()
{
    return mWebEngineView->selectedText();
    // FIXME: should parse out word under cursor if no selection
}

bool HelpBrowser::openDocumentation()
{
    return Main::openDocumentation(symbolUnderCursor());
}

void HelpBrowser::openDefinition()
{
    return Main::openDefinition(symbolUnderCursor(), window());
}

void HelpBrowser::openCommandLine()
{
    return Main::openCommandLine(symbolUnderCursor());
}

void HelpBrowser::findReferences()
{
    return Main::findReferences(symbolUnderCursor(), window());
}

HelpBrowserFindBox::HelpBrowserFindBox( QWidget * parent ):
    QLineEdit(parent)
{
    setPlaceholderText(tr("Find in page..."));
    connect( this, SIGNAL(textChanged(QString)), this, SIGNAL(query(QString)) );
}

bool HelpBrowserFindBox::event( QEvent * event )
{
    switch(event->type()) {
    case QEvent::ShortcutOverride:
    {
        QKeyEvent *kevent = static_cast<QKeyEvent*>(event);
        if (kevent->key() == Qt::Key_Escape) {
            event->accept();
            return true;
        }
        if (kevent->key() == Qt::Key_W && (kevent->modifiers() & Qt::CTRL)) {
            MainWindow::instance()->helpBrowserDocklet()->close();
            event->accept();
            return true;
        }
    }
    case QEvent::KeyPress:
    {
        QKeyEvent *kevent = static_cast<QKeyEvent*>(event);
        switch (kevent->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            bool backwards = kevent->modifiers() & Qt::ShiftModifier;
            emit query( text(), backwards );
            return true;
        }
        case Qt::Key_Escape:
            clear();
        default:
            break;
        }
    }
    default:
        break;
    }

    return QLineEdit::event(event);
}

HelpBrowserDocklet::HelpBrowserDocklet( QWidget *parent ):
    Docklet(tr("Help browser"), parent)
{
    mHelpBrowser = new HelpBrowser;

    setAllowedAreas(Qt::AllDockWidgetAreas);
    setWidget(mHelpBrowser);

    mFindBox = new HelpBrowserFindBox();

    toolBar()->addWidget( mHelpBrowser->loadProgressIndicator(), 1 );
    toolBar()->addAction( mHelpBrowser->mActions[HelpBrowser::GoHome] );
    toolBar()->addAction( mHelpBrowser->mWebEngineView->pageAction(QWebEnginePage::Back) );
    toolBar()->addAction( mHelpBrowser->mWebEngineView->pageAction(QWebEnginePage::Forward) );
    toolBar()->addAction( mHelpBrowser->mWebEngineView->pageAction(QWebEnginePage::Reload) );
    toolBar()->addWidget( mFindBox );

    connect( mFindBox, SIGNAL(query(QString, bool)),
             mHelpBrowser, SLOT(findText(QString, bool)) );

    connect(Main::scProcess(), SIGNAL(started()), this, SLOT(onInterpreterStart()));

    OverridingAction * action;
    action = new OverridingAction(this);
    action->setShortcut(QKeySequence::Find);
    action->addToWidget(mHelpBrowser);
    action->addToWidget(toolBar());
    connect( action, SIGNAL(triggered(bool)), mFindBox, SLOT(setFocus()) );
    connect( action, SIGNAL(triggered(bool)), mFindBox, SLOT(selectAll()) );
}

} // namespace ScIDE
