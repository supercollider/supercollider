/************************************************************************
*
* Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
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
#include <QScrollArea>

static QcWidgetFactory<QcHelpBrowser> factory;

QcHelpBrowser::QcHelpBrowser()
{

  // content display
  helpFileView = new HelpFileView;

  // top controls
  QToolBar *toolbar = new QToolBar;
  QAction *actHome = toolbar->addAction( style()->standardIcon( QStyle::SP_DirHomeIcon ),
                                         "Go To Start Page" );
  toolbar->addAction( helpFileView->pageAction( QWebPage::Back ) );
  toolbar->addAction( helpFileView->pageAction( QWebPage::Forward ) );

  findTool = new SimpleFindTool;
  findTool->setMaximumWidth( 200 );

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget( toolbar );
  topLayout->addStretch();
  topLayout->addWidget( new QLabel( "Find:" ) );
  topLayout->addWidget( findTool );
  topLayout->setContentsMargins(0,0,0,0);

  // bottom controls
  // QToolBar *bottomToolbar = new QToolBar;

  // main layout
  QVBoxLayout *l = new QVBoxLayout;
  // l->setContentsMargins(10,10,10,0);
  l->addLayout( topLayout );
  l->addWidget( helpFileView );
  // l->addWidget( bottomToolbar );

  setLayout( l );
  setWindowTitle( "SuperCollider Help" );

  QShortcut *copyScut = new QShortcut( QKeySequence::Copy, helpFileView );
  copyScut->setContext( Qt::WidgetWithChildrenShortcut );

  connect( helpFileView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)) );
  connect( copyScut, SIGNAL(activated()),
           helpFileView->pageAction( QWebPage::Copy ), SLOT(trigger()) );
  connect( findTool, SIGNAL(activated(const QString&,bool)),
           helpFileView, SLOT(findText(const QString&,bool)) );
  connect( actHome, SIGNAL(activated()), this, SLOT(goHome()) );
}

void QcHelpBrowser::load( const QString &urlString ) {
  helpFileView->load( QUrl( urlString ) );
}

void QcHelpBrowser::goHome()
{
  load( _home );
}

void QcHelpBrowser::onLoadFinished( bool ok )
{
  if( !ok ) {
    setWindowTitle( "SuperCollider Help: Failed to load page" );
    return;
  }

  QString title = helpFileView->title();
  if( title.isEmpty() ) {
    title = QFileInfo( helpFileView->url().toLocalFile() ).fileName();
    title.truncate( title.lastIndexOf('.') );
  }

  setWindowTitle( "SuperCollider Help: " + title );
}
