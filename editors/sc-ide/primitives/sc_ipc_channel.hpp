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

class IIpcHandler {};

class ScIpcChannel {

	QTcpSocket *mSocket;
	void bail();
	void bail(const char *msg);

	// used in creating the shared memory key, should uniquely identify the sender.
	// (ie "sclang" or "scide")
	QString mTag;
	IIpcLogger *mLogger;

public:
	const quint32 CheckMask = 0xAAAA9999;
	const QString LargeMessageSelector = "LargeMessage";
	static const int Port = 5228;

	// on windows, difficulties arrise when we serialize large QT objects.
	static const int MaxInbandSize = 4000;

	ScIpcChannel(QTcpSocket *socket, QString &tag, IIpcLogger *logger) :
		mTag(tag),
		mLogger(logger)
	{
		mSocket = socket;
	};

	~ScIpcChannel();

	template <typename TData, typename TIpcHandler>
	void read(TIpcHandler *that, void (TIpcHandler::*onResponse)(const QString &selector, const TData &data));

	template <typename T> 
	void write(const QString &selector, const T &data);

	void log(const QString &message) {
		mLogger->onIpcLog(QString("IPC: %1\n").arg(message));
	}

};

// If these templates go in the cpp file we get linking errors.
template <typename TData, typename TIpcHandler>
void ScIpcChannel::read(TIpcHandler *that, void (TIpcHandler::*onResponse)(const QString &selector, const TData &data)) {

	log(QString("READ:<?> - A - ScIpcChannel::read started."));

	if (!mSocket) {
		log(QString("READ:<?> ScIpcChannel::read - No socket."));
		return;
	}

	while (mSocket->bytesAvailable() > 0) {
		//QByteArray inputBytes;
		//QBuffer buf(&inputBytes);
		//buf.open(QIODevice::ReadOnly);
		QDataStream inStream(mSocket);
		inStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

		quint32 available = mSocket->bytesAvailable();
		quint32 bytesExpected;

		while (available < sizeof(bytesExpected)) {
			mSocket->waitForReadyRead();
			available = mSocket->bytesAvailable();
		}

		//available = loadInputBytes();
		inStream >> bytesExpected;
		bytesExpected += sizeof(quint32);

		// spin until we have all the data
		while (mSocket->bytesAvailable() < bytesExpected)
			mSocket->waitForReadyRead();		//available = loadInputBytes();
		bytesExpected += sizeof(quint32);

		QString selector;
		inStream >> selector;
		if (inStream.status() != QDataStream::Ok) {
			int status = inStream.status();
			log(QString("IPC FAIL. reading selector, status: %1").arg(inStream.status()));
			bail();
			return;
		}

		log(QString("READ:B: Selector: %1").arg(selector));

		TData data;
		inStream >> data;

		if (inStream.status() != QDataStream::Ok) {
			bail(QString("IPC FAIL. reading data, status: %1").arg(inStream.status()).toStdString().c_str());
			return;
		}

		// at the end we expect to see check ^ CheckMask = the length specified by the other end at the start.
		// so we know we read the right number of bytes, and we can trust that the next thing we read will be another length.
		quint32 check;
		inStream >> check;
		check ^= CheckMask;

		if (inStream.status() != QDataStream::Ok || bytesExpected != check) {
			bail("Check failed.");
			return;
		}

		log(QString("READ:<%1> - C").arg(selector));

		if (selector == LargeMessageSelector) {
			log(QString("READ:<%1> - WOOT").arg(selector));
			QByteArray lotsOfBytes;
			{
				ScIpcLargeMessage* blob = new ScIpcLargeMessage(data);
				blob->receive(lotsOfBytes);
				delete blob;
			}

			QBuffer largeBuf(&lotsOfBytes);
			largeBuf.open(QIODevice::ReadOnly);
			QDataStream largeStream(&largeBuf);
			largeStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

			largeStream >> selector;
			largeStream >> data;
			largeBuf.close();
		}

		log(QString("READ<%1>:D").arg(selector));
		// call the handler
		(that->*onResponse)(selector, data);
		log(QString("READ<%1>:EEEE").arg(selector));

	}
	
}

template <typename T> void ScIpcChannel::write(const QString &selector, const T &data) {


	log(QString("ScIpcChannel::write started. <%1>").arg(selector));

	if (!mSocket) {
		log(QString("ScIpcChannel::write - No socket."));
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
	
	quint32 left = bytes.size();
	quint32 total_length = left + 2 * sizeof(quint32);
			 
	if (total_length < ScIpcChannel::MaxInbandSize) {
		QDataStream socketStream(mSocket);
		socketStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
		socketStream << bytes;
		total_length ^= CheckMask;
		socketStream << total_length;
		
		auto waitReturn = mSocket->waitForBytesWritten();
		if (!waitReturn) {
			log(QString("waitForBytesWritten timedout. <%1>:%2 bytes.").arg(selector, total_length));
		}
	}
	else {
		ScIpcLargeMessage* blob = new ScIpcLargeMessage(bytes, mTag);
		blob->send(this); // returns after the other side has read it.
		delete blob;
	}
	
	log(QString("ScIpcChannel::write complete. <%1>").arg(selector));
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

	char *data(void *memory) {
		return static_cast<char *>(memory)+sizeof(ScIpcLargeMessageMemory);
	}

	void coolStoryBro() {
		mFresh = 0;
		mStatus = DONE;
	}

	qint32 ripen() {
		return --mFresh;
	}

};

class ScIpcLargeMessage {
public:

	static const QString Selector;

	QSharedMemory *shm;
	ScIpcLargeMessageMemory *memory;
	char *data;

	// Here we are a writer.
	ScIpcLargeMessage(QByteArray &bytes, QString &tag) {
		static int i = 0;
		QString key = QString("ScIpcLargeMessage<%1>:%2").arg(tag, ++i);

		quint32 n = bytes.size();
		shm = new QSharedMemory();
		shm->setKey(key);
		if (!shm->create(sizeof(quint32) + n + 1)) {
			const char *wtf = shm->errorString().toStdString().c_str();
			return;
		}

		shm->lock();
		void *p = shm->data();
		memory = static_cast<ScIpcLargeMessageMemory*>(p);
		memory->mLength = n;
		memory->mFresh = 10;
		memory->mStatus = ScIpcLargeMessageMemory::UNREAD;
		data = memory->data(p);
		
		memcpy(data, bytes.constData(), (size_t)n);
		data[n] = '\0';
		shm->unlock();

	}

	// Here we are a reader.
	ScIpcLargeMessage(QString &key) {
		
		shm = new QSharedMemory();
		shm->setKey(key);
		if (!shm->attach()) {
			const char *wtf = shm->errorString().toStdString().c_str();
			return;
		}

		void *p = shm->data();
		memory = static_cast<ScIpcLargeMessageMemory*>(p);
		data = memory->data(p);
		
	}

	// these are here so the template ScIpcChannel::read can compile. they are not used.
	ScIpcLargeMessage(QVariantList &data) {}
	ScIpcLargeMessage(QStringList &data) {}

	~ScIpcLargeMessage() {
		if (shm) {
			if (shm->isAttached())
				shm->detach();
			shm->deleteLater();
			shm = nullptr;
		}
	}

	void send(ScIpcChannel *channel) {
		QString &key = shm->key();

		channel->log(QString("ScIpcLargeMessage::send - Signaling other side that there's data."));
		channel->write<QString>(channel->LargeMessageSelector, key);
		
		// wait until other side has read this, give up when it's not fresh anymore.
		shm->lock();
		while (memory->ripen() > 0) {																	
			channel->log(QString("ScIpcLargeMessage::send - memory->mFresh == %1").arg(memory->mFresh));
			shm->unlock();
			QThread::msleep(500L);
			shm->lock();
		}

 		if (memory->mStatus != ScIpcLargeMessageMemory::DONE) {
			auto status = memory->mStatus;
			// log error
			channel->log(QString("ScIpcLargeMessage::send - Other side didn't read shared memory, deleting. status:%1").arg(status));
			shm->unlock();
			return;
		}

		shm->unlock();
	}

	void receive(QByteArray &dst) {
		shm->lock();
		if (memory->mFresh > 0) {
			dst.append(data, memory->mLength);
			memory->coolStoryBro();
		}
		shm->unlock();
	}

};



#endif //SuperCollider_ScIpcChannel_hpp