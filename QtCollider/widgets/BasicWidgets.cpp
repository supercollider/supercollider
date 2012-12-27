/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#include "BasicWidgets.h"
#include "../QcWidgetFactory.h"

#include <QPaintEvent>
#include <QPainter>

QC_DECLARE_QWIDGET_FACTORY( QcDefaultWidget );
QC_DECLARE_QWIDGET_FACTORY( QcHLayoutWidget );
QC_DECLARE_QWIDGET_FACTORY( QcVLayoutWidget );

void QcSimpleWidget::setBackground( const QColor &c )
{
  if(_bkg == c) return;

  _bkg = c;
  setAttribute(Qt::WA_OpaquePaintEvent, c.isValid() && c.alpha() == 255);
  update();
}

void QcSimpleWidget::paintEvent( QPaintEvent *e )
{
  if (!_bkg.isValid()) return;

  QPainter p(this);
  p.fillRect(e->rect(), _bkg);
}

class QcCustomPaintedFactory : public QcWidgetFactory<QcCustomPainted>
{
protected:
  virtual void initialize( QWidgetProxy *p, QcCustomPainted *w )
  {
    QObject::connect( w, SIGNAL(painting(QPainter*)),
                      p, SLOT(customPaint(QPainter*)) );
  }
};

QC_DECLARE_FACTORY( QcCustomPainted, QcCustomPaintedFactory );
