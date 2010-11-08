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

#include "QcHelpBrowser.hpp"
#include "../QcWidgetFactory.h"

#include <QWebView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollBar>
#include <QLineEdit>
#include <QToolBar>
#include <QStackedWidget>
#include <QLabel>
#include <QHeaderView>
#include <QSignalMapper>
#include <QAction>
#include <QFileInfo>
#include <QShortcut>

static QcWidgetFactory<QcHelpBrowser> factory;

QcHelpBrowser::QcHelpBrowser() {
  webView = new QWebView;
  categoryScroll = new QcWidgetScroll;
  findField = new QLineEdit;
  findField->setMaximumWidth( 200 );
  searchField = new QLineEdit;

  searchResultTitle = new QLabel;
  searchResultList = new QTreeWidget;
  searchResultList->setColumnCount(2);
  QHeaderView *hHeader = searchResultList->header();
  hHeader->setResizeMode( QHeaderView::ResizeToContents );
  searchResultList->setHeaderLabels( QStringList() << "Document" << "Context" );
  searchResultList->setSelectionBehavior( QAbstractItemView::SelectRows );
  searchResultList->setRootIsDecorated( false );
  searchResultList->setAlternatingRowColors( true );

  QVBoxLayout *searchL = new QVBoxLayout;
  searchL->addWidget( searchResultTitle );
  searchL->addWidget( searchResultList );
  searchResultWidget = new QWidget;
  searchResultWidget->setLayout( searchL );

  browseStack = new QStackedWidget;
  browseStack->addWidget( searchResultWidget );
  browseStack->addWidget( categoryScroll );
  browseStack->setCurrentWidget( categoryScroll );

  QSplitter *contentSplit = new QSplitter( Qt::Vertical );
  contentSplit->addWidget( webView );
  contentSplit->addWidget( browseStack );
  contentSplit->setStretchFactor( 0, 1 );
  contentSplit->setStretchFactor( 1, 0 );
  browseStack->hide();

  QToolBar *toolbar = new QToolBar;
  toolbar->addAction( webView->pageAction( QWebPage::Back ) );
  toolbar->addAction( webView->pageAction( QWebPage::Forward ) );

  QToolBar *bottomToolbar = new QToolBar;
  bottomToolbar->addWidget( new QLabel("Search:") );
  bottomToolbar->addWidget( searchField );
  QAction *searchAction = bottomToolbar->addAction( "Search" );
  QAction *browseAction = bottomToolbar->addAction( "Browse" );
  QIcon closeIcon( style()->standardPixmap(QStyle::SP_DialogCloseButton) );
  QAction *hideAction = bottomToolbar->addAction( closeIcon, "Hide" );

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget( toolbar );
  topLayout->addStretch();
  topLayout->addWidget( new QLabel("Find:") );
  topLayout->addWidget( findField );
  topLayout->setContentsMargins(0,0,0,0);

  QVBoxLayout *l = new QVBoxLayout;
  l->addLayout( topLayout );
  l->addWidget( contentSplit );
  l->addWidget( bottomToolbar );

  setLayout( l );
  setWindowTitle( "SuperCollider Help" );

  QShortcut *copyScut = new QShortcut( QKeySequence::Copy, webView );
  copyScut->setContext( Qt::WidgetWithChildrenShortcut );

  QSignalMapper *mapper = new QSignalMapper( this );
  mapper->setMapping( browseAction, categoryScroll );
  mapper->setMapping( searchAction, searchResultWidget );

  connect( webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)) );
  connect( copyScut, SIGNAL(activated()),
           webView->pageAction( QWebPage::Copy ), SLOT(trigger()) );
  connect( findField, SIGNAL(textEdited(const QString&)), this, SLOT(onSearchCurrent()) );
  connect( findField, SIGNAL(returnPressed()), this, SLOT(onSearchCurrent()) );
  connect( browseAction, SIGNAL(triggered()), mapper, SLOT(map()) );
  connect( searchAction, SIGNAL(triggered()), mapper, SLOT(map()) );
  connect( mapper, SIGNAL(mapped(QWidget*)), browseStack, SLOT(setCurrentWidget(QWidget*)) );
  connect( mapper, SIGNAL(mapped(QWidget*)), browseStack, SLOT(show()) );
  connect( hideAction, SIGNAL(triggered()), browseStack, SLOT(hide()) );
  connect( searchField, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
  connect( searchResultList, SIGNAL(itemActivated(QTreeWidgetItem*,int) ),
           this, SLOT(onSearchResultActivated(QTreeWidgetItem*)) );
}

void QcHelpBrowser::load( const QString &urlString ) {
  webView->load( QUrl( urlString ) );
}

void QcHelpBrowser::addChild( QWidget * child ) {
  categoryScroll->addChild( child );
}

void QcHelpBrowser::setCurrentIndex( int i )
{
  categoryScroll->setCurrentIndex(i);
}

void QcHelpBrowser::setSearchResults( const VariantList& l ) {

  searchResultList->clear();

  Q_FOREACH( QVariant var, l.data ) {
    VariantList dataList = var.value<VariantList>();
    QString docName, context;
    if( dataList.data.size() >= 2 ) {
      docName = dataList.data[0].toString();
      context = dataList.data[1].toString();
    }
    if( docName.isEmpty() ) docName = "(no document name)";
    QIcon icon;
    if( docName.left(2) == "[[" && docName.right(2) == "]]" ) {
      docName.remove(2);
      docName.chop(2);
      icon = style()->standardIcon( QStyle::SP_DirIcon );
    }
    QTreeWidgetItem *item = new QTreeWidgetItem( searchResultList,
                                                 QStringList() << docName << context );
    item->setIcon( 0, icon );
  }

  browseStack->setCurrentWidget( searchResultWidget );
  browseStack->show();
}

void QcHelpBrowser::setMinimumListWidth( int i ){
  categoryScroll->setMinimumChildWidth( i );
}

void QcHelpBrowser::onSearchCurrent()
{
  webView->findText( findField->text(), QWebPage::FindWrapsAroundDocument );
}

void QcHelpBrowser::onSearch()
{
  QString searchText = searchField->text();
  if( searchText.isEmpty() ) return;

  searchResultTitle->setText( QString("Search results for '%1':").arg(searchText) );
  Q_EMIT( searchInvoked( searchField->text() ) );
}

void QcHelpBrowser::onSearchResultActivated( QTreeWidgetItem * item ) {
  int index = searchResultList->indexOfTopLevelItem( item );
  Q_EMIT( searchResultActivated( index ) );
}

void QcHelpBrowser::onLoadFinished( bool ok )
{
  if( !ok ) {
    setWindowTitle( "SuperCollider Help: Failed to load page" );
    return;
  }

  QString title = webView->title();
  if( title.isEmpty() ) {
    title = QFileInfo( webView->url().toLocalFile() ).fileName();
    title.truncate( title.lastIndexOf('.') );
  }

  setWindowTitle( "SuperCollider Help: " + title );
}

QcWidgetScroll::QcWidgetScroll( QWidget *parent ) : QAbstractScrollArea( parent ),
  _minChildWidth(150), _visibleChildrenCount(1)
{
  QFrame::setFrameShape( QFrame::NoFrame );

  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  horizontalScrollBar()->setPageStep(1);

  QWidget *viewport = new QWidget;
  QHBoxLayout *vl = new QHBoxLayout;
  vl->setContentsMargins(0,0,0,0);
  vl->setSpacing(0);
  viewport->setLayout(vl);

  setViewport( viewport );
}

void QcWidgetScroll::addChild( QWidget *w )
{
  viewport()->layout()->addWidget( w );
  _children.append( w );

  connect( w, SIGNAL(destroyed(QObject*)), this, SLOT(onChildDeleted(QObject*)) );

  updateLayout();
}

void QcWidgetScroll::setCurrentIndex( int i ) {
  horizontalScrollBar()->setValue( i );
}

void QcWidgetScroll::updateLayout()
{
  int c = _children.count();

  horizontalScrollBar()->setRange( 0, c - _visibleChildrenCount );
  setFirstVisible( horizontalScrollBar()->value() );
}

void QcWidgetScroll::setFirstVisible( int firstVisible )
{
  int c = _children.count();
  if( !c ) return;

  int visible = _visibleChildrenCount;
  int childCount = 0;
  Q_FOREACH( QWidget *w, _children ) {
    if( childCount < firstVisible ) {
      w->hide();
    }
    else {
      if( visible ) {
        w->show();
      --visible;
      }
      else {
        w->hide();
      }
    }
    ++childCount;
  }
}

void QcWidgetScroll::scrollContentsBy ( int dx, int dy )
{
  setFirstVisible( horizontalScrollBar()->value() );
}

void QcWidgetScroll::resizeEvent( QResizeEvent * )
{
  _visibleChildrenCount = qMax( viewport()->width() / _minChildWidth, 1 );
  updateLayout();
}

void QcWidgetScroll::onChildDeleted( QObject *o )
{
  QWidget *w = qobject_cast<QWidget*>(o);
  if( !w ) return;
  _children.removeAll( w );
  updateLayout();
}
