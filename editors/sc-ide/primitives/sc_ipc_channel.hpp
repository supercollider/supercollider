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

	// on windows, difficulties arise when we serialize large QT objects.
	static const int MaxInbandSize = 4000;

	ScIpcChannel(QTcpSocket *socket, const QString &tag, IIpcHandler *logger);
	~ScIpcChannel();

	void log(const QString &message);

	void read();
	void write(const QString &selector, const QVariantList &data);
	
};



struct ScIpcLargeMessageMemory {
	
	enum Status:quint32 {
		UNREAD,
		DONE,
		ERROR
	};

	qint32 mFresh;
	Status mStatus;
	quint32 mLength;

	char *data(void *memory);
	void coolStoryBro();
	qint32 ripen();

};

class ScIpcLargeMessage {
public:
	QSharedMemory *shm;
	ScIpcLargeMessageMemory *memory;
	char *data;
	IIpcHandler *mIpcHandler;

	// Here we are a writer.
	ScIpcLargeMessage(QByteArray &bytes, const QString &tag, IIpcHandler *handler);

	// Here we are a reader.
	ScIpcLargeMessage(const QString &key, IIpcHandler *handler);

	~ScIpcLargeMessage();

	void send(ScIpcChannel *channel);
	void receive(QByteArray &dst);
	void log(const QString &message);
};

#endif //SuperCollider_ScIpcChannel_hpp
