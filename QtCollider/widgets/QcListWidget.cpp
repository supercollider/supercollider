/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcListWidget.h"
#include "../QcWidgetFactory.h"

#include <QKeyEvent>

class QcListWidgetFactory : public QcWidgetFactory<QcListWidget>
{
  void initialize( QWidgetProxy *p, QcListWidget *l ) {
    p->setMouseEventWidget( l->viewport() );
  }
};

QC_DECLARE_FACTORY( QcListWidget, QcListWidgetFactory );

QcListWidget::QcListWidget() : _emitAction(true)
{
  connect( this, SIGNAL( currentItemChanged( QListWidgetItem*, QListWidgetItem* ) ),
           this, SLOT( onCurrentItemChanged() ) );
}

void QcListWidget::setItems( const VariantList & items )
{
  _emitAction = false;
  clear();
  Q_FOREACH( QVariant item, items.data )
    addItem( item.toString() );
  setCurrentRow( 0 );
  _emitAction = true;
}

void QcListWidget::setColors( const VariantList & colors ) const
{
  int cc = colors.data.count();
  int ic = count();
  for( int i=0; i<cc && i < ic; ++i ) {
    QListWidgetItem *it = item(i);
    QColor color( colors.data[i].value<QColor>() );
    if( color.isValid() ) it->setBackground( color );
  }
}

void QcListWidget::setCurrentRowWithoutAction( int row )
{
  bool b = _emitAction;
  _emitAction = false;
  setCurrentRow( row );
  _emitAction = b;
}

void QcListWidget::onCurrentItemChanged()
{
  if( _emitAction ) Q_EMIT( action() );
}

void QcListWidget::keyPressEvent( QKeyEvent *e )
{
  QListWidget::keyPressEvent( e );

  switch (e->key())
  {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    Q_EMIT( returnPressed() );
    e->accept();
    break;
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_PageUp:
  case Qt::Key_PageDown:
  case Qt::Key_Home:
  case Qt::Key_End:
    // Prevent propagating to parent when scroller reaches minimum or maximum:
    e->accept();
  default: break;
  }
}
