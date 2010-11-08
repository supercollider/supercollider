/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_HELP_BROWSER_H
#define QC_HELP_BROWSER_H

#include "../Common.h"

#include <QWidget>
#include <QAbstractScrollArea>
#include <QTreeWidget>

class QWebView;
class QLineEdit;
class QLabel;
class QStackedWidget;
class QcWidgetScroll;

class QcHelpBrowser : public QWidget
{
  Q_OBJECT
  Q_PROPERTY( int minimumListWidth READ minimumListWidth WRITE setMinimumListWidth );

public:
  QcHelpBrowser();

  Q_INVOKABLE
  void load( const QString &urlString );

  Q_INVOKABLE
  void addChild( QWidget * child );

  Q_INVOKABLE
  void setCurrentIndex( int i );

  Q_INVOKABLE
  void setSearchResults( const VariantList& l );

  int minimumListWidth() const { return 0; }
  void setMinimumListWidth( int i );
  QSize sizeHint() const {
    return QWidget::sizeHint() - QSize(200, 0 );
  }

Q_SIGNALS:
  void searchInvoked( const QString & );
  void searchResultActivated( int );

private Q_SLOTS:
  void onSearchCurrent();
  void onSearch();
  void onSearchResultActivated( QTreeWidgetItem * item );
  void onLoadFinished( bool ok );

private:
  QWebView *webView;
  QcWidgetScroll *categoryScroll;
  QLineEdit *findField;
  QLineEdit *searchField;
  QLabel *searchResultTitle;
  QTreeWidget *searchResultList;
  QWidget *searchResultWidget;
  QStackedWidget *browseStack;
};

class QcWidgetScroll : public QAbstractScrollArea
{
  Q_OBJECT
public:
  QcWidgetScroll( QWidget *parent = 0 );
  Q_INVOKABLE void addChild( QWidget * );
  void setMinimumChildWidth( int w ) { _minChildWidth = w; updateLayout(); }
  void setCurrentIndex( int i );
private Q_SLOTS:
  void onChildDeleted( QObject * );
private:
  void updateLayout();
  void setFirstVisible( int );
  void scrollContentsBy ( int dx, int dy );
  void resizeEvent( QResizeEvent * );
  QSize sizeHint() const { return QSize( 200, 200 ); }


  QList<QWidget*> _children;
  int _minChildWidth;
  //int _currentIndex;
  int _visibleChildrenCount;
};

#endif
