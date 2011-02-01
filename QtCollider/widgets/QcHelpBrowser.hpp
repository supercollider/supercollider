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
  Q_OBJECT

public:
  QcHelpBrowser();

  Q_INVOKABLE
  void load( const QString &urlString );

  Q_INVOKABLE
  void setHome( const QString &homeUrl ) { _home = homeUrl; goHome(); }

  QSize sizeHint() const {
    return QWidget::sizeHint() - QSize(200, 0 );
  }

public Q_SLOTS:
   void goHome();

private Q_SLOTS:
  void onLoadFinished( bool ok );

private:
  // general dialog operation
  SimpleFindTool *findTool;

  // content display
  HelpFileView *helpFileView;

  QString _home;
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

#endif
