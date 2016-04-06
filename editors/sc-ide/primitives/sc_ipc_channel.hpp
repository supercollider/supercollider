/*
 SuperCollider Qt IDE
 Copyright (c) 2012 Jakob Leben & Tim Blechmann
 http://www.audiosynth.com

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef SuperCollider_ScIpcChannel_hpp
#define SuperCollider_ScIpcChannel_hpp

#include <QObject>
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QBuffer>
#include <QTcpSocket>
#include <QSharedMemory>
#include <QThread>
#include <QMutex>

struct IIpcHandler {
  virtual void onIpcLog(const QString &message) = 0;
  virtual void onIpcMessage(const QString & selector, const QVariantList & argList) = 0;
};

class ScIpcChannel {

  QTcpSocket *mSocket;
  void bail();

  // used in creating the shared memory key, should uniquely identify the sender.
  // (ie "sclang" or "scide")
  QString mTag;
  IIpcHandler *mIpcHandler;

  QMutex mReadLock;
  QMutex mWriteLock;

public:
  const quint32 CheckMask = 0xAAAA9999;
  const QString LargeMessageSelector = "LargeMessage";
  static const int Port = 5228;

  ScIpcChannel(QTcpSocket *socket, const QString &tag, IIpcHandler *logger);
  ~ScIpcChannel();

  void log(const QString &message);

  void read();
  void write(const QString &selector, const QVariantList &data);

};

#endif //SuperCollider_ScIpcChannel_hpp
