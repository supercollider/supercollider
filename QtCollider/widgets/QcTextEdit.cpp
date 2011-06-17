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
#include <QKeyEvent>
#include <QApplication>

class QcTextEditFactory : public QcWidgetFactory<QcTextEdit>
{
  void initialize( QWidgetProxy *p, QcTextEdit *w, QList<QVariant> & ) {
    p->setMouseEventWidget( w->viewport() );
  }
};

static QcTextEditFactory factory;

QcTextEdit::QcTextEdit() : _interpretSelection(false)
{
  connect( this, SIGNAL(interpret(QString)),
           qApp, SLOT(interpret(QString)),
           Qt::QueuedConnection );
}


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

int QcTextEdit::selectionStart() const
{
  return textCursor().selectionStart();
}

int QcTextEdit::selectionSize() const
{
  QTextCursor cursor = textCursor();
  return cursor.selectionEnd() - cursor.selectionStart();
}

void QcTextEdit::select( int start, int size )
{
  if( start < 0 ) start = 0;

  QTextCursor cursor( document() );

  cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, start );
  cursor.movePosition( size > 0 ? QTextCursor::Right : QTextCursor::Left,
                       QTextCursor::KeepAnchor, qAbs(size) );

  setTextCursor( cursor );
}

QString QcTextEdit::selectedString() const
{
  // NOTE QTextCuror.selectedText() contains unicode paragraph separators U+2029
  // instead of newline \n characters
  return textCursor().selectedText().replace( QChar( 0x2029 ), QChar( '\n' ) );
}

void QcTextEdit::replaceSelectedText( const QString &string )
{
  QTextCursor cursor( textCursor() );
  if( cursor.hasSelection() ) {
    cursor.removeSelectedText();
    cursor.insertText( string );
  }
}

void QcTextEdit::setTextFont( const QFont &f )
{
  QTextCharFormat format;
  format.setFont( f );

  QTextCursor cursor( document() );
  cursor.select( QTextCursor::Document );
  cursor.setCharFormat( format );

  QTextEdit::setFont(f);
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

void QcTextEdit::keyPressEvent( QKeyEvent *e )
{
  if( _interpretSelection && e->modifiers() & (Qt::ControlModifier|Qt::ShiftModifier)
      && ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) )
  {
    QString selection = selectedString();
    if( !selection.isEmpty() ) {
      Q_EMIT( interpret( selection ) );
      return;
    }
  }

  QTextEdit::keyPressEvent( e );
}
