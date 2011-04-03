/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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
#include "../QcWidgetFactory.h"
#include <QWebPage>
#include <QWebFrame>
#include <QAction>
#include <QShortcut>

static QcWidgetFactory<QtCollider::WebView> factory;

namespace QtCollider {

WebView::WebView( QWidget *parent ) : QWebView( parent )
{
  /// FIXME: for now we reset the base colors in order to render the help
  ///        files correctly on a dark background
  QPalette pal = palette();
  pal.setColor(QPalette::Base, QColor(Qt::white));
  pal.setColor(QPalette::Text, QColor(Qt::black));
  setPalette(pal);

  QShortcut *scutCopy = new QShortcut( QKeySequence::Copy, this );
  scutCopy->setContext( Qt::WidgetWithChildrenShortcut );
  connect( scutCopy, SIGNAL(activated()),
           pageAction(QWebPage::Copy), SLOT(trigger()) );

  connect( this, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
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

QWebPage::LinkDelegationPolicy WebView::linkDelegationPolicy () const
{
  return page()->linkDelegationPolicy();
}

void WebView::setLinkDelegationPolicy ( QWebPage::LinkDelegationPolicy p )
{
  page()->setLinkDelegationPolicy( p );
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

} // namespace QtCollider
