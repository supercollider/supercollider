/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_WEB_VIEW_H
#define QC_WEB_VIEW_H

#include <QWebView>
#include <QWebPage>
#include <QUrl>

namespace QtCollider {

class WebPage;

class WebView : public QWebView
{
  Q_OBJECT
  Q_PROPERTY( QString url READ url WRITE setUrl );
  Q_PROPERTY( QString html READ html )
  Q_PROPERTY( QString plainText READ plainText )
  Q_PROPERTY( int linkDelegationPolicy
              READ linkDelegationPolicy WRITE setLinkDelegationPolicy )
  Q_PROPERTY( bool delegateReload READ delegateReload WRITE setDelegateReload );
  Q_PROPERTY( bool enterInterpretsSelection
              READ interpretSelection WRITE setInterpretSelection );
  Q_PROPERTY( bool editable
               READ editable WRITE setEditable );

public:
  Q_INVOKABLE void setHtml ( const QString &html, const QString &baseUrl = QString() );
  Q_INVOKABLE void evaluateJavaScript ( const QString &script );
  Q_INVOKABLE void setFontFamily( int genericFontFamily, const QString & fontFamily );

public Q_SLOTS:
  void findText( const QString &searchText, bool reversed = false );

Q_SIGNALS:
  void linkActivated( const QString & );
  void reloadTriggered( const QString & );
  void interpret( const QString & code );
  void jsConsoleMsg( const QString &, int, const QString & );

public:

  WebView( QWidget *parent = 0 );

  QString url() const;
  void setUrl( const QString & );
  QString html () const;
  QString plainText () const;

  int linkDelegationPolicy () const;
  void setLinkDelegationPolicy ( int );
  bool delegateReload() const;
  void setDelegateReload( bool );
  bool interpretSelection() const { return _interpretSelection; }
  void setInterpretSelection( bool b ) { _interpretSelection = b; }
    bool editable() const { return _editable; }
  void setEditable( bool b ) { _editable = b; page()->setContentEditable(b); }

  inline static QUrl urlFromString( const QString & str ) {
      return QUrl::fromUserInput(str);
  }

protected:
  virtual void keyPressEvent( QKeyEvent * );
  virtual void contextMenuEvent ( QContextMenuEvent * );

private Q_SLOTS:
  void onLinkClicked( const QUrl & );
  void onPageReload();
  void updateEditable(bool ok) { if(ok) page()->setContentEditable(_editable); }

private:
  bool _interpretSelection;
  bool _editable;
};

} // namespace QtCollider

#endif // QC_WEB_VIEW_H
