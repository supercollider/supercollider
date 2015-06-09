/*
 SuperCollider Qt IDE
 Copyright (c) 2012, 2013 Jakob Leben, Tim Blechmann, and Scott Wilson
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
#include <QByteArray>
#include <QIODevice>
#include <QBuffer>
#include <QDataStream>
#include <QTcpSocket>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QThread>

struct IIpcLogger {
	virtual void onIpcLog(const QString &message) = 0;
};

class ScIpcChannel {

	QTcpSocket *mSocket;
	void bail();

	// used in creating the shared memory key, should uniquely identify the sender.
	// (ie "sclang" or "scide")
	QString mTag;
	IIpcLogger *mLogger;

public:
	const quint32 CheckMask = 0xAAAA9999;
	const QString LargeMessageSelector = "LargeMessage";
	static const int Port = 5228;

	// on windows, difficulties arise when we serialize large QT objects.
	static const int MaxInbandSize = 4000;

	ScIpcChannel(QTcpSocket *socket, QString &tag, IIpcLogger *logger);
	~ScIpcChannel();

	void log(const QString &message);

	template <typename TData, typename TIpcHandler>
	void read(TIpcHandler *that, void (TIpcHandler::*onResponse)(const QString &selector, const TData &data));

	template <typename T> 
	void write(const QString &selector, const T &data);

};

// If these templates go in the cpp file we get linking errors.
template <typename TData, typename TIpcHandler>
void ScIpcChannel::read(TIpcHandler *that, void (TIpcHandler::*onResponse)(const QString &selector, const TData &data)) {

	log("read - started.");

	if (!mSocket) {
		log("read - No socket.");
		return;
	}

	while (mSocket->bytesAvailable() > 0) {
		
		QDataStream inStream(mSocket);
		inStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

		quint32 bytesExpected;

		while (mSocket->bytesAvailable() < sizeof(bytesExpected)) {
			mSocket->waitForReadyRead();
		}

		inStream >> bytesExpected;
		bytesExpected += sizeof(quint32);

		// spin until we have all the data
		while (mSocket->bytesAvailable() < bytesExpected)
			mSocket->waitForReadyRead();

		bytesExpected += sizeof(quint32);

		QString selector;
		inStream >> selector;
		if (inStream.status() != QDataStream::Ok) {
			int status = inStream.status();
			log(QString("read - %1 - Error reading selector, status: %2").arg(selector, inStream.status() ));
			bail();
			return;
		}

		TData data;
		inStream >> data;

		if (inStream.status() != QDataStream::Ok) {
			log(QString("read - %1 - Error reading data, status: %2").arg(selector, inStream.status() ));
			bail();
			return;
		}

		// at the end we expect to see check ^ CheckMask = the length specified by the other end at the start.
		// so we know we read the right number of bytes, and we can trust that the next thing we read will be another length.
		quint32 check;
		inStream >> check;
		check ^= CheckMask;

		if (inStream.status() != QDataStream::Ok || bytesExpected != check) {
			log("read - Check failed.");
			bail();
			return;
		}

		if (selector == LargeMessageSelector) {
			QByteArray lotsOfBytes;
			{
				ScIpcLargeMessage* largeMessage = new ScIpcLargeMessage(data, mLogger);
				largeMessage->receive(lotsOfBytes);
				delete largeMessage;
			}

			QBuffer largeBuf(&lotsOfBytes);
			largeBuf.open(QIODevice::ReadOnly);
			QDataStream largeStream(&largeBuf);
			largeStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

			largeStream >> selector;
			largeStream >> data;
			largeBuf.close();
		}

		// call the handler
		(that->*onResponse)(selector, data);

	}
	
}

template <typename T> void ScIpcChannel::write(const QString &selector, const T &data) {

	//log(QString("write - %1 - started.").arg(selector));

	if (!mSocket) {
		log(QString("write - %1 - No socket.").arg(selector));
		return;
	}

	QByteArray bytes;
	QBuffer buf(&bytes);
	buf.open(QIODevice::WriteOnly);
	QDataStream bufStream(&buf);
	bufStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

	bufStream << selector;
	bufStream << data;
	buf.close();
	
	// when we serialize a QByteArray, the length (quint32) of the array is automatically sent first.
	// On the other side, we must adjust this, as we send a quint32 check after.
	quint32 left = bytes.size();
	quint32 totalLength = left + 2 * sizeof(quint32);
			 
	if (totalLength < ScIpcChannel::MaxInbandSize) {
		QDataStream socketStream(mSocket);
		socketStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

		socketStream << bytes;
		totalLength ^= CheckMask;
		socketStream << totalLength;
		
		if (!mSocket->waitForBytesWritten()) {
			log(QString("write - %1 - waitForBytesWritten timeout - %2 bytes attempted.").arg(selector, totalLength));
		}
	}
	else {
		ScIpcLargeMessage* largeMessage = new ScIpcLargeMessage(bytes, mTag, mLogger);
		largeMessage->send(this); // returns after the other side has read it, or after we've given up.
		delete largeMessage;
	}
	
	//log(QString("write - %1 - complete.").arg(selector));
}

struct ScIpcLargeMessageMemory {
	
	enum Status:quint32 {
		UNREAD,
		DONE,
		ERROR
	};

	quint32 mLength;
	qint32 mFresh;
	Status mStatus;

	char *data(void *memory);
	void coolStoryBro();
	qint32 ripen();

};

class ScIpcLargeMessage {
public:

	static const QString Selector;

	QSharedMemory *shm;
	ScIpcLargeMessageMemory *memory;
	char *data;
	IIpcLogger *mLogger;

	// Here we are a writer.
	ScIpcLargeMessage(QByteArray &bytes, QString &tag, IIpcLogger *logger);

	// Here we are a reader.
	ScIpcLargeMessage(QString &key, IIpcLogger *logger);

	// these are here so the template ScIpcChannel::read can compile. they are not used.
	ScIpcLargeMessage(QVariantList &data, IIpcLogger *logger);
	ScIpcLargeMessage(QStringList &data, IIpcLogger *logger);

	~ScIpcLargeMessage();

	void send(ScIpcChannel *channel);
	void receive(QByteArray &dst);
	void log(const QString &message);
};

#endif //SuperCollider_ScIpcChannel_hpp