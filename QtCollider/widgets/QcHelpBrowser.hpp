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
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QWebView>

class QStackedWidget;
class QcWidgetScroll;

class SimpleFindTool;
class ClassView;
class HelpFileView;

class QcHelpBrowser : public QWidget
{
  enum SearchType {
    Help,
    Class,
    Method
  };

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
  void setSearchResults( const QString &text, int type, const VariantList &data );

  Q_INVOKABLE
  void setClassReference( const QString &className, const VariantList &data );

  int minimumListWidth() const { return 0; }
  void setMinimumListWidth( int i );
  QSize sizeHint() const {
    return QWidget::sizeHint() - QSize(200, 0 );
  }

Q_SIGNALS:
  void searchRequest( const QString &, int );
  void searchResultActivated( int );
  void classReferenceRequest( const QString &className );

private Q_SLOTS:
  void onSearch();
  void onSearchResultActivated( QTreeWidgetItem * item );
  void onLoadFinished( bool ok );
  void showContent( QWidget *contentView );

private:
  // actions
  QAction *aShowHelp;
  QAction *aShowRef;

  // general dialog operation
  SimpleFindTool *findTool;
  QLineEdit *searchField;

  // content display
  HelpFileView *helpFileView;
  ClassView *classRefView;
  QStackedWidget *contentStack;

  // database browsing and searching
  QLabel *searchResultTitle;
  QTreeWidget *searchResultList;
  QComboBox *searchTypeChoice;
  QCheckBox *searchCaseChoice;
  QWidget *searchResultWidget;

  QcWidgetScroll *categoryScroll;

  QStackedWidget *browseStack;

  // data
  int curSearchType;

  int reqSearchType;
  QString reqSearchString;

  QString reqClassName;
};

class SimpleFindTool : public QLineEdit
{
  Q_OBJECT
public:
  SimpleFindTool( QWidget *parent = 0 ) : QLineEdit( parent ) {
    connect( this, SIGNAL(textEdited(const QString&)), this, SLOT(activate()) );
    connect( this, SIGNAL(returnPressed()), this, SLOT(activate()) );
  }
Q_SIGNALS:
  void activated( const QString & searchText, bool reversed = false );
private Q_SLOTS:
  void activate() {
    Q_EMIT( activated( text() ) );
  }
};

class HelpFileView : public QWebView
{
  Q_OBJECT
public:
  HelpFileView( QWidget *parent = 0 ) : QWebView( parent ) {
    /// FIXME: for now we reset the base colors in order to render the help
    ///        files correctly on a dark background
    QPalette pal = palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    setPalette(pal);
  }
public Q_SLOTS:
  void findText( const QString &searchText, bool reversed = false ) {
    QWebPage::FindFlags flags( QWebPage::FindWrapsAroundDocument );
    if( reversed ) flags |= QWebPage::FindBackward;
    QWebView::findText( searchText, flags );
  }
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

class MethodWidget : public QWidget
{
public:
  MethodWidget ( const QString &name, const QStringList args, const QString &html = QString() );
private:
  QString _name;
  QStringList _args;
  QString _html;
};

class ClassView : public QWidget
{
  Q_OBJECT
public:
  ClassView( QWidget *parent = 0 );
  ~ClassView();
  void setClassData( const QString &className, const VariantList & );
  void setClassVariables( const QStringList & );
  void setInstanceVariables( const QStringList & );
public Q_SLOTS:
  void findText( const QString &searchText, bool reversed = false ) {}
private:
  void fillLabel( QLabel *, const QStringList & );
  QLabel *nameLabel;
  QLabel *cvLabel;
  QLabel *ivLabel;
  QVBoxLayout *imLayout;
  QVBoxLayout *cmLayout;
  QList<MethodWidget*> cmWidgets;
  QList<MethodWidget*> imWidgets;
  QString currentClassName;
};

#endif
