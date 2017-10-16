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

#ifndef QC_WEB_PAGE_HPP_INCLUDED
#define QC_WEB_PAGE_HPP_INCLUDED

#include <QWebPage>

namespace QtCollider {

class WebPage : public QWebPage
{
  Q_OBJECT

public:

  WebPage( QObject *parent ) :
    QWebPage( parent ),
    _delegateReload(false),
    _usingDesktopBrowser(false)
  {}

  virtual void triggerAction ( WebAction action, bool checked = false );
  virtual void javaScriptConsoleMessage ( const QString &, int, const QString & );
  bool delegateReload() const { return _delegateReload; }
  void setDelegateReload( bool flag ) { _delegateReload = flag; }

  bool usingDesktopBrowser() const { return _usingDesktopBrowser; }
  void setUsingDesktopBrowser( bool flag ) { _usingDesktopBrowser = flag; }

protected:
  // from QWebPage::acceptNavigationRequest
  virtual bool acceptNavigationRequest( QWebFrame*, const QNetworkRequest&, NavigationType );

Q_SIGNALS:
  void jsConsoleMsg( const QString &, int, const QString & );

private:

  bool _delegateReload;
  bool _usingDesktopBrowser; ///< If true, external links will open in the default desktop browser.
};

} // namespace QtCollider

#endif // QC_WEB_PAGE_HPP_INCLUDED
