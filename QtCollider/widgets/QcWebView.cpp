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
#include "web_page.hpp"
#include "../QcWidgetFactory.h"
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>

QC_DECLARE_QWIDGET_FACTORY(WebView);

namespace QtCollider {

WebView::WebView( QWidget *parent ) :
  QWebView( parent ),
  _interpretSelection(false),
  _editable(false)
{
  QtCollider::WebPage *page = new WebPage(this);
  page->setDelegateReload(true);
  setPage( page );

  // Set the style's standard palette to avoid system's palette incoherencies
  // get in the way of rendering web pages
  setPalette( style()->standardPalette() );

  setAttribute(Qt::WA_AcceptTouchEvents);

  page->action( QWebPage::Copy )->setShortcut( QKeySequence::Copy );
  page->action( QWebPage::Paste )->setShortcut( QKeySequence::Paste );

  connect( this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
  connect( page->action(QWebPage::Reload), SIGNAL(triggered(bool)),
           this, SLOT(onPageReload()) );

  connect( this, SIGNAL(interpret(QString)),
           qApp, SLOT(interpret(QString)),
           Qt::QueuedConnection );

  connect( page, SIGNAL(jsConsoleMsg(const QString&, int, const QString&)),
           this, SIGNAL(jsConsoleMsg(const QString&, int, const QString&)) );
    
  connect( this, SIGNAL(loadFinished(bool)), this, SLOT(updateEditable(bool)) );
}

QString WebView::url() const
{
  return QWebView::url().toString();
}

void WebView::setUrl( const QString & str )
{
  load( urlFromString(str) );
}

QString WebView::html () const
{
  return page()->mainFrame()->toHtml();
}

void WebView::setHtml ( const QString &html, const QString &baseUrl )
{
  QUrl url( baseUrl.isEmpty() ? QUrl() : urlFromString(baseUrl) );
  QWebView::setHtml( html, url );
}

QString WebView::plainText () const
{
  return page()->mainFrame()->toPlainText();
}

int WebView::linkDelegationPolicy () const
{
  return (int)page()->linkDelegationPolicy();
}

void WebView::setLinkDelegationPolicy ( int p )
{
  page()->setLinkDelegationPolicy( (QWebPage::LinkDelegationPolicy)p );
}

bool WebView::delegateReload() const
{
  WebPage *p = qobject_cast<QtCollider::WebPage*>(page());
  Q_ASSERT(p);
  return p->delegateReload();
}

void WebView::setDelegateReload( bool flag )
{
  WebPage *p = qobject_cast<QtCollider::WebPage*>(page());
  Q_ASSERT(p);
  p->setDelegateReload( flag );
}

void WebView::setFontFamily( int generic, const QString & specific )
{
  settings()->setFontFamily( (QWebSettings::FontFamily) generic, specific );
}

void WebView::evaluateJavaScript ( const QString &script )
{
  if( script.isEmpty() ) return;
  QWebFrame *frame = page()->currentFrame();
  if( frame ) frame->evaluateJavaScript( script );
}

void WebView::findText( const QString &searchText, bool reversed )
{
  QWebPage::FindFlags flags( QWebPage::FindWrapsAroundDocument );
  if( reversed ) flags |= QWebPage::FindBackward;
  QWebView::findText( searchText, flags );
}

void WebView::onLinkClicked( const QUrl &url )
{
  Q_EMIT( linkActivated( url.toString() ) );
}

void WebView::onPageReload()
{
  Q_EMIT( reloadTriggered( url() ) );
}

void WebView::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu;

    QPoint pos = event->pos();

    QWebHitTestResult hitTest = page()->mainFrame()->hitTestContent( pos );

    if (!hitTest.linkElement().isNull()) {
        menu.addAction( pageAction(QWebPage::CopyLinkToClipboard) );
        menu.addSeparator();
    }

    if (hitTest.isContentEditable() || hitTest.isContentSelected()) {
        menu.addAction( pageAction(QWebPage::Copy) );
        if (hitTest.isContentEditable())
            menu.addAction( pageAction(QWebPage::Paste) );
        menu.addSeparator();
    }

    menu.addAction( pageAction(QWebPage::Back) );
    menu.addAction( pageAction(QWebPage::Forward) );
    menu.addAction( pageAction(QWebPage::Reload) );

    menu.exec( event->globalPos() );
}

void WebView::keyPressEvent( QKeyEvent *e )
{
    int key = e->key();
    int mods = e->modifiers();

    if( _interpretSelection &&
            ( key == Qt::Key_Enter ||
              ( key == Qt::Key_Return && mods & (Qt::ControlModifier|Qt::ShiftModifier) ) ) )
    {
        QString selection = selectedText();
        if( !selection.isEmpty() ) {
            Q_EMIT( interpret( selection ) );
            return;
        }
    }

    QWebView::keyPressEvent( e );
}

} // namespace QtCollider
