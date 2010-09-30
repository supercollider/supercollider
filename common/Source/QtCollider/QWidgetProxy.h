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

#ifndef QC_WIDGET_PROXY_H
#define QC_WIDGET_PROXY_H

#include "QObjectProxy.h"

#include <QWidget>

class QWidgetProxy : public QObjectProxy
{
  Q_OBJECT
  public:
    QWidgetProxy( QWidget *w, PyrObject *po ) : QObjectProxy( w, po )
    {
      widget = w;
    }
    Q_INVOKABLE void setFocus( bool b ) {
      if( b )
        widget->setFocus( Qt::OtherFocusReason );
      else
        widget->clearFocus();
    }
    Q_INVOKABLE void bringFront() {
      widget->setWindowState( widget->windowState() & ~Qt::WindowMinimized
                              | Qt::WindowActive );
      widget->show();
      widget->raise();
    }
  private:
    QWidget *widget;
};

#endif //QC_WIDGET_PROXY_H
