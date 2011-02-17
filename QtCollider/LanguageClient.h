/************************************************************************
*
* Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_LANGUAGE_CLIENT_H
#define QC_LANGUAGE_CLIENT_H

#include "SC_TerminalClient.h"
#include "QC_Export.h"

#include <QObject>
#include <QTimer>

namespace QtCollider {

class LangClient : public QObject, public SC_TerminalClient
{
  Q_OBJECT
public:
  LangClient( const char* name );
  virtual ~LangClient() {};
private Q_SLOTS:
  void cmdLineTick();
  void daemonTick();
protected:
  virtual void commandLoop();
  virtual void daemonLoop();
private:
  QTimer *langTimer;
};

} // namespace QtCollider

#endif // QC_LANGUAGE_CLIENT_H
