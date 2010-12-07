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
#include <QScrollArea>

static QcWidgetFactory<QcHelpBrowser> factory;

QcHelpBrowser::QcHelpBrowser() :
  curSearchType(-1),
  reqSearchType(-1)
{

  // content display
  helpFileView = new HelpFileView;
  classRefView = new ClassView;
  contentStack = new QStackedWidget;
  contentStack->addWidget( helpFileView );
  contentStack->addWidget( classRefView );

  // browsing
  categoryScroll = new QcWidgetScroll;

  // searching
  searchResultTitle = new QLabel;

  searchResultList = new QTreeWidget;
  QHeaderView *hHeader = searchResultList->header();
  hHeader->setResizeMode( QHeaderView::ResizeToContents );
  hHeader->hide();
  searchResultList->setSelectionBehavior( QAbstractItemView::SelectRows );
  searchResultList->setRootIsDecorated( false );
  searchResultList->setAlternatingRowColors( true );

  searchField = new QLineEdit;

  searchTypeChoice = new QComboBox();
  searchTypeChoice->setEditable(false);
  searchTypeChoice->addItem( "All Help" );
  searchTypeChoice->addItem( "Classes" );
  searchTypeChoice->addItem( "Methods" );

  searchCaseChoice = new QCheckBox("Case Insensitive");

  QHBoxLayout *searchReqL = new QHBoxLayout;
  searchReqL->setContentsMargins(0,0,0,0);
  searchReqL->setSpacing( 10 );
  searchReqL->addWidget( new QLabel("Search For:") );
  searchReqL->addWidget( searchField );
  searchReqL->addWidget( new QLabel("In:") );
  searchReqL->addWidget( searchTypeChoice );
  searchReqL->addWidget( searchCaseChoice );

  QVBoxLayout *searchL = new QVBoxLayout;
  searchL->addWidget( searchResultTitle );
  searchL->addWidget( searchResultList );
  searchL->addLayout( searchReqL );
  searchResultWidget = new QWidget;
  searchResultWidget->setLayout( searchL );

  // browsing + searching layout
  browseStack = new QStackedWidget;
  browseStack->addWidget( searchResultWidget );
  browseStack->addWidget( categoryScroll );
  browseStack->setCurrentWidget( categoryScroll );

  // content - browsing layout
  QSplitter *contentSplit = new QSplitter( Qt::Vertical );
  contentSplit->addWidget( contentStack );
  contentSplit->addWidget( browseStack );
  contentSplit->setStretchFactor( 0, 1 );
  contentSplit->setStretchFactor( 1, 0 );
  browseStack->hide();

  // top controls
  QToolBar *toolbar = new QToolBar;
  toolbar->addAction( helpFileView->pageAction( QWebPage::Back ) );
  toolbar->addAction( helpFileView->pageAction( QWebPage::Forward ) );
  toolbar->addSeparator();
  QActionGroup *viewActionGroup = new QActionGroup( this );
  aShowHelp = viewActionGroup->addAction( "Help Page" );
  aShowRef = viewActionGroup->addAction( "Class Reference" );
  Q_FOREACH( QAction *a, viewActionGroup->actions() ) {
    a->setCheckable(true);
    toolbar->addAction( a );
  }

  findTool = new SimpleFindTool;
  findTool->setMaximumWidth( 200 );

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget( toolbar );
  topLayout->addStretch();
  topLayout->addWidget( new QLabel( "Find:" ) );
  topLayout->addWidget( findTool );
  topLayout->setContentsMargins(0,0,0,0);

  // bottom controls
  QToolBar *bottomToolbar = new QToolBar;
  QAction *searchAction = bottomToolbar->addAction( "Search" );
  QAction *browseAction = bottomToolbar->addAction( "Browse" );
  QIcon closeIcon( style()->standardPixmap(QStyle::SP_DialogCloseButton) );
  QAction *hideAction = bottomToolbar->addAction( closeIcon, "Hide" );

  // main layout
  QVBoxLayout *l = new QVBoxLayout;
  //l->setContentsMargins(10,10,10,0);
  l->addLayout( topLayout );
  l->addWidget( contentSplit );
  l->addWidget( bottomToolbar );

  setLayout( l );
  setWindowTitle( "SuperCollider Help" );

  QShortcut *copyScut = new QShortcut( QKeySequence::Copy, helpFileView );
  copyScut->setContext( Qt::WidgetWithChildrenShortcut );

  QSignalMapper *browseMapper = new QSignalMapper( this );
  browseMapper->setMapping( browseAction, categoryScroll );
  browseMapper->setMapping( searchAction, searchResultWidget );

  QSignalMapper *contentMapper = new QSignalMapper( this );
  contentMapper->setMapping( aShowHelp, helpFileView );
  contentMapper->setMapping( aShowRef, classRefView );

  connect( helpFileView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)) );
  connect( copyScut, SIGNAL(activated()),
           helpFileView->pageAction( QWebPage::Copy ), SLOT(trigger()) );

  connect( aShowHelp, SIGNAL(triggered()), contentMapper, SLOT(map()) );
  connect( aShowRef, SIGNAL(triggered()), contentMapper, SLOT(map()) );
  connect( contentMapper, SIGNAL(mapped(QWidget*)),
           this, SLOT(showContent(QWidget*)) );

  connect( browseAction, SIGNAL(triggered()), browseMapper, SLOT(map()) );
  connect( searchAction, SIGNAL(triggered()), browseMapper, SLOT(map()) );
  connect( browseMapper, SIGNAL(mapped(QWidget*)), browseStack, SLOT(setCurrentWidget(QWidget*)) );
  connect( browseMapper, SIGNAL(mapped(QWidget*)), browseStack, SLOT(show()) );
  connect( hideAction, SIGNAL(triggered()), browseStack, SLOT(hide()) );

  connect( searchField, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
  connect( searchResultList, SIGNAL(itemActivated(QTreeWidgetItem*,int) ),
           this, SLOT(onSearchResultActivated(QTreeWidgetItem*)) );

  aShowHelp->trigger();
}

void QcHelpBrowser::load( const QString &urlString ) {
  helpFileView->load( QUrl( urlString ) );
}

void QcHelpBrowser::addChild( QWidget * child ) {
  categoryScroll->addChild( child );
}

void QcHelpBrowser::setCurrentIndex( int i )
{
  categoryScroll->setCurrentIndex(i);
}

void QcHelpBrowser::setSearchResults( const QString &text, int type,
                                      const VariantList &l )
{

  if( text != reqSearchString || type != reqSearchType ) return;

  curSearchType = type;

  searchResultList->setSortingEnabled(false);
  searchResultList->clear();

  searchResultTitle->setText( QString("Search Results For '%1' In %2").arg(text)
                              .arg( searchTypeChoice->itemText(type) ) );

  switch( type ) {
    case Help:
      searchResultList->setColumnCount(2);
      searchResultList->setHeaderLabels( QStringList() << "Document" << "Context" );

      Q_FOREACH( QVariant var, l.data ) {
        VariantList dataList = var.value<VariantList>();
        if( dataList.data.size() < 3 ) continue;
        QString docName = dataList.data[0].toString();
        QString context = dataList.data[1].toString();
        QString url = dataList.data[2].toString();
        QTreeWidgetItem *item = new QTreeWidgetItem( searchResultList,
                                                    QStringList() << docName << context );
        item->setData( 0, Qt::UserRole, QVariant(url) );
      }

      searchResultList->setSortingEnabled(true);
      searchResultList->sortByColumn( 0, Qt::AscendingOrder );

      break;

    case Class:
      searchResultList->setColumnCount(1);
      searchResultList->setHeaderLabels( QStringList() << "Class" );

      Q_FOREACH( QVariant var, l.data ) {
        QString className = var.toString();
        QTreeWidgetItem *item = new QTreeWidgetItem( searchResultList,
                                                      QStringList() << className );
      }

      searchResultList->setSortingEnabled(true);
      searchResultList->sortByColumn( 0, Qt::AscendingOrder );

      break;

    case Method:
      searchResultList->setColumnCount(2);
      searchResultList->setHeaderLabels( QStringList() << "Class" << "Method" );

      Q_FOREACH( QVariant var, l.data ) {
        VariantList dataList = var.value<VariantList>();
        if( dataList.data.size() < 2 ) continue;
        QString className = dataList.data[0].toString();
        QString method = dataList.data[1].toString();
        QTreeWidgetItem *item = new QTreeWidgetItem( searchResultList,
                                                    QStringList() << className << method );
      }

      searchResultList->setSortingEnabled(true);
      searchResultList->sortByColumn( 1, Qt::AscendingOrder );

      break;
  }

  searchResultList->header()->show();
  browseStack->setCurrentWidget( searchResultWidget );
  browseStack->show();
}

void QcHelpBrowser::setClassReference( const QString &className, const VariantList &data ) {
  printf("QcHelpBrowser::setClassReference\n");
  if( className == reqClassName )
    classRefView->setClassData( className, data );
}

void QcHelpBrowser::setMinimumListWidth( int i ){
  categoryScroll->setMinimumChildWidth( i );
}

void QcHelpBrowser::onSearch()
{
  QString searchString = searchField->text();
  if( searchString.isEmpty() ) return;

  reqSearchString = searchString;
  reqSearchType = searchTypeChoice->currentIndex();

  Q_EMIT( searchRequest( searchField->text(), searchTypeChoice->currentIndex() ) );
}

void QcHelpBrowser::onSearchResultActivated( QTreeWidgetItem * item ) {
  QString data;
  switch( curSearchType ) {
    case Help:
      data = item->data( 0, Qt::UserRole ).toString();
      if( !data.isEmpty() )
        load( data );
      aShowHelp->trigger();
      break;
    case Class:
    case Method:
      data = item->text( 0 );
      if( !data.isEmpty() ) {
        reqClassName = data;
        Q_EMIT( classReferenceRequest( data ) );
      }
      aShowRef->trigger();
      break;
  }

  /*int index = searchResultList->indexOfTopLevelItem( item );
  Q_EMIT( searchResultActivated( index ) );*/
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

void QcHelpBrowser::showContent( QWidget *newContent )
{
  // FIXME use that check, but take care to initialize correctly at QcHelpBrowser construction
  // if( content == newContent ) return;

  QWidget *content = contentStack->currentWidget();
  Q_ASSERT(content);

  findTool->disconnect( content, SLOT(findText(const QString&,bool)) );

  contentStack->setCurrentWidget( newContent );

  connect( findTool, SIGNAL(activated(const QString&,bool)),
           newContent, SLOT(findText(const QString&,bool)) );
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

ClassView::ClassView( QWidget *parent ) : QWidget( parent )
{
  QFont titlesF;
  titlesF.setBold( true );

  nameLabel = new QLabel;
  QFont nameF;
  nameF.setPointSize(15);
  nameLabel->setFont( nameF );

  cvLabel = new QLabel;
  ivLabel = new QLabel;
  cmLayout = new QVBoxLayout();
  imLayout = new QVBoxLayout();

  QLabel *cvTitle = new QLabel("Class variables:");
  cvTitle->setFont( titlesF );
  QLabel *cmTitle = new QLabel("Class methods:");
  cmTitle->setFont( titlesF );
  QLabel *ivTitle = new QLabel("Instance variables:");
  ivTitle->setFont( titlesF );
  QLabel *imTitle = new QLabel("Instance methods:");
  imTitle->setFont( titlesF );

  QVBoxLayout *contentL = new QVBoxLayout();
  contentL->addWidget( cvTitle );
  contentL->addWidget( cvLabel );

  contentL->addWidget( ivTitle );
  contentL->addWidget( ivLabel );

  contentL->addWidget( cmTitle );
  contentL->addLayout( cmLayout );

  contentL->addWidget( imTitle );
  contentL->addLayout( imLayout );

  contentL->addStretch(1);

  QWidget *contentW = new QWidget();
  contentW->setLayout( contentL );

  QScrollArea *scroll = new QScrollArea();
  scroll->setWidget( contentW );
  scroll->setWidgetResizable( true );
  scroll->setBackgroundRole( QPalette::Base );

  QVBoxLayout *mainL = new QVBoxLayout();
  mainL->setContentsMargins( 0,0,0,0 );
  mainL->addWidget( nameLabel );
  mainL->addWidget( scroll );

  setLayout( mainL );
}

ClassView::~ClassView()
{
}

void ClassView::fillLabel( QLabel *label, const QStringList &list )
{
  QString text;
  Q_FOREACH( QString str, list ) {
    text.append(str).append("\n");
  }
  text.chop(1);
  label->setText( text );
}

static void parseMethod( const QVariant &var, QString &method, QStringList &args )
{
  VariantList varMethod = var.value<VariantList>();
  if( !varMethod.data.size() ) return;

  method = QString( varMethod.data.takeFirst().toString() );

  if( !varMethod.data.size() ) return;

  Q_FOREACH( QVariant varArg, varMethod.data ) {
    QString arg = varArg.toString();
    if( arg == "this" ) continue;
    args.append(arg);
  }
}

void ClassView::setClassData( const QString &name, const VariantList &list )
{
  QString superName = list.data[1].toString();

  QStringList subNames;
  VariantList varSubNames = list.data[2].value<VariantList>();
  Q_FOREACH( QVariant var, varSubNames.data )
    subNames << var.toString();

  QStringList cvars;
  VariantList varCVars = list.data[3].value<VariantList>();
  Q_FOREACH( QVariant var, varCVars.data )
    cvars << var.toString();

  QStringList ivars;
  VariantList varIVars = list.data[5].value<VariantList>();
  Q_FOREACH( QVariant var, varIVars.data )
    ivars << var.toString();

  qDeleteAll( cmWidgets );
  cmWidgets.clear();
  qDeleteAll( imWidgets );
  imWidgets.clear();

  VariantList varCMethods = list.data[4].value<VariantList>();
  Q_FOREACH( QVariant varMethod, varCMethods.data ) {
    QString method;
    QStringList args;
    parseMethod( varMethod, method, args );
    method.prepend('*');
    MethodWidget *mw = new MethodWidget( method, args );
    cmWidgets << mw;
    cmLayout->addWidget( mw );
  }

  VariantList varIMethods = list.data[6].value<VariantList>();
  QStringList imethods;
  Q_FOREACH( QVariant varMethod, varIMethods.data ) {
    QString method;
    QStringList args;
    parseMethod( varMethod, method, args );
    MethodWidget *mw = new MethodWidget( method, args );
    imWidgets << mw;
    imLayout->addWidget( mw );
  }

  nameLabel->setText(name + " Class Reference");
  setClassVariables( cvars );
  setInstanceVariables( ivars );
}

void ClassView::setClassVariables( const QStringList &vars ) {
  fillLabel( cvLabel, vars );
}

void ClassView::setInstanceVariables( const QStringList &vars ) {
  fillLabel( ivLabel, vars );
}

MethodWidget::MethodWidget( const QString &n, const QStringList a, const QString &html )
: _name(n), _args(a), _html(html)
{
  QString methodString = _name;

  QString argString;
  Q_FOREACH( QString arg, _args ) {
    if( arg == "this" ) continue;
    argString.append(arg);
    argString.append(", ");
  }
  if( !_args.isEmpty() ) {
    argString.chop(2);
    methodString.append(" ( ");
    methodString.append(argString);
    methodString.append(" )");
  }

  QLabel *label = new QLabel(methodString);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  setLayout( layout );
}
