/************************************************************************
*
* Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "web_page.hpp"

#include <QApplication>
#include <QClipboard>

namespace QtCollider {

void WebPage::triggerAction ( WebAction action, bool checked )
{
  switch ( action ) {
    case QWebPage::Reload:
      if( _delegateReload ) return;
      break;
    case QWebPage::Copy:
      // ignore text formatting, copy only plain text:
      QApplication::clipboard()->setText( selectedText() );
      return;
    default:
      break;
  }

  QWebPage::triggerAction( action, checked );
}

void WebPage::javaScriptConsoleMessage ( const QString & msg, int line, const QString & src )
{
  Q_EMIT( jsConsoleMsg(msg,line,src) );
}

} // namespace QtCollider
