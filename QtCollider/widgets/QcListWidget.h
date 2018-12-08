/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "../QcHelper.h"
#include "../Common.h"
#include <QListWidget>

class QcListWidget : public QListWidget, QcHelper
{
  Q_OBJECT
  Q_PROPERTY( QVariantList items READ dummyVariantList WRITE setItems );
  Q_PROPERTY( QVariantList colors READ dummyVariantList WRITE setColors );
  Q_PROPERTY( int currentRow READ currentRow WRITE setCurrentRowWithoutAction )
  Q_PROPERTY( QVariantList selection READ selection WRITE setSelection );

  public:
    QcListWidget();
    void setItems( const QVariantList & );
    void setColors( const QVariantList & ) const;
    void setCurrentRowWithoutAction( int );
    QVariantList selection() const;
    void setSelection( const QVariantList & );

  Q_SIGNALS:
    void action();
    void returnPressed();
  private Q_SLOTS:
    void onCurrentItemChanged();
  protected:
    virtual void keyPressEvent( QKeyEvent * );
  private:
    bool _emitAction;
};
