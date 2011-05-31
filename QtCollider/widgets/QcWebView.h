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

#ifndef QC_WEB_VIEW_H
#define QC_WEB_VIEW_H

#include <QWebView>
#include <QWebPage>

namespace QtCollider {

class WebPage;

class WebView : public QWebView
{
  Q_OBJECT
  Q_PROPERTY( QString url READ url WRITE setUrl );
  Q_PROPERTY( QString html READ html )
  Q_PROPERTY( QString plainText READ plainText )
  Q_PROPERTY( QWebPage::LinkDelegationPolicy linkDelegationPolicy
              READ linkDelegationPolicy WRITE setLinkDelegationPolicy )
  Q_PROPERTY( bool delegateReload READ delegateReload WRITE setDelegateReload );
  Q_PROPERTY( bool enterInterpretsSelection
              READ interpretSelection WRITE setInterpretSelection );

public:

  WebView( QWidget *parent = 0 );
  QString url() const;
  void setUrl( const QString & );
  QString html () const;
  Q_INVOKABLE void setHtml ( const QString &html, const QString &baseUrl = QString() );
  QString plainText () const;
  QWebPage::LinkDelegationPolicy linkDelegationPolicy () const;
  void setLinkDelegationPolicy ( QWebPage::LinkDelegationPolicy );
  bool delegateReload() const;
  void setDelegateReload( bool );
  bool interpretSelection() const { return _interpretSelection; }
  void setInterpretSelection( bool b ) { _interpretSelection = b; }
  Q_INVOKABLE void evaluateJavaScript ( const QString &script );

  inline static QUrl urlFromString( const QString & str ) {
    QUrl url( str );
    if( url.scheme().isEmpty() ) url.setScheme( "file" );
    return url;
  }

Q_SIGNALS:
  void linkActivated( const QString & );
  void reloadTriggered( const QString & );
  void interpret( const QString & code );

public Q_SLOTS:
  void findText( const QString &searchText, bool reversed = false );

protected:
  virtual void keyPressEvent( QKeyEvent * );

private Q_SLOTS:
  void onLinkClicked( const QUrl & );
  void onPageReload();

private:
  bool _interpretSelection;
};

class WebPage : public QWebPage
{
  Q_OBJECT

public:

  WebPage( QObject *parent ) : QWebPage( parent ), _delegateReload(false) {}
  virtual void triggerAction ( WebAction action, bool checked = false );
  bool delegateReload() const { return _delegateReload; }
  void setDelegateReload( bool flag ) { _delegateReload = flag; }

private:

  bool _delegateReload;
};

} // namespace QtCollider

#endif // QC_WEB_VIEW_H
