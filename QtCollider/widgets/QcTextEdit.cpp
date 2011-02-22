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

#include "QcTextEdit.h"
#include "../QcWidgetFactory.h"

#include <QFile>

class QcTextEditFactory : public QcWidgetFactory<QcTextEdit>
{
  void initialize( QWidgetProxy *p, QcTextEdit *w, QList<QVariant> & ) {
    p->setMouseEventWidget( w->viewport() );
  }
};

static QcTextEditFactory factory;

QString QcTextEdit::documentFilename() const
{
  return _document;
}

void QcTextEdit::setDocument( const QString &filename )
{
  QFile file( filename );
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  QByteArray data = file.readAll();
  setText( data );
  _document = filename;
}

int QcTextEdit::selectionStart()
{
  return textCursor().selectionStart();
}

int QcTextEdit::selectionSize()
{
  QTextCursor cursor = textCursor();
  return cursor.selectionEnd() - cursor.selectionStart();
}

QString QcTextEdit::selectedString()
{
  return textCursor().selectedText();
}

void QcTextEdit::setTextFont( const QFont &f )
{
  QTextCharFormat format;
  format.setFont( f );

  QTextCursor cursor( document() );
  cursor.select( QTextCursor::Document );
  cursor.setCharFormat( format );
}

void QcTextEdit::setTextColor( const QColor &color )
{
  QTextCharFormat format;
  format.setForeground( color );

  QTextCursor cursor( document() );
  cursor.select( QTextCursor::Document );
  cursor.setCharFormat( format );
}

void QcTextEdit::setRangeColor( const VariantList &list )
{
  if( list.data.count() < 3 ) return;
  QColor c = list.data[0].value<QColor>();
  int start = list.data[1].toInt();
  int size = list.data[2].toInt();

  QTextCharFormat format;
  format.setForeground( c );

  QTextCursor cursor( document() );
  cursor.setPosition( start );
  cursor.setPosition( start + size, QTextCursor::KeepAnchor );
  cursor.setCharFormat( format );
}

void QcTextEdit::setRangeFont( const VariantList & list )
{
  if( list.data.count() < 3 ) return;
  QFont f = list.data[0].value<QFont>();
  int start = list.data[1].toInt();
  int size = list.data[2].toInt();

  QTextCharFormat format;
  format.setFont( f );

  QTextCursor cursor( document() );
  cursor.setPosition( start );
  cursor.setPosition( start + size, QTextCursor::KeepAnchor );
  cursor.setCharFormat( format );
}

void QcTextEdit::setRangeText( const VariantList & list )
{
  if( list.data.count() < 3 ) return;
  QString text = list.data[0].value<QString>();
  int start = list.data[1].toInt();
  int size = list.data[2].toInt();

  QTextCursor cursor( document() );
  cursor.setPosition( start );
  cursor.setPosition( start + size, QTextCursor::KeepAnchor );
  cursor.insertText( text );
}
