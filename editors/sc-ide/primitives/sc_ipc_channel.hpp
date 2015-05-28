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


class ScIpcChannel {

	QTcpSocket *mSocket;

	void bail();
	void bail(const char *msg);

	// used in creating the shared memory key, should uniquely identify the sender.
	// (ie "sclang" or "scide")
	QString mTag;

public:
	const quint32 CheckMask = 0xAAAA9999;
	static const int Port = 5228;

	ScIpcChannel(QTcpSocket *socket, QString &tag);
	~ScIpcChannel();

	template <typename T, typename HandlerContext>
	void read(HandlerContext *that, void (HandlerContext::*each)(const QString &selector, const T &data));

	template <typename T> 
	void write(const QString &selector, const T &data);

};

// If these templates go in the cpp file we get linking errors.
template <typename T, typename HandlerContext> 
void ScIpcChannel::read(HandlerContext *that, void (HandlerContext::*each)(const QString &selector, const T &data)) {

	if (!mSocket)
		return;

	//QByteArray inputBytes;
	//QBuffer buf(&inputBytes);
	//buf.open(QIODevice::ReadOnly);
	QDataStream inStream(mSocket);

	quint32 available = mSocket->bytesAvailable();
	quint32 bytesExpected;
		
	while (available < sizeof(bytesExpected))
		available = mSocket->bytesAvailable();

	//available = loadInputBytes();
	inStream >> bytesExpected;
	bytesExpected += sizeof(quint32); 
		
	// spin until we have all the data
	while (mSocket->bytesAvailable() < bytesExpected);
		//available = loadInputBytes();
	bytesExpected += sizeof(quint32);

	QString selector;
	inStream >> selector;
	if (inStream.status() != QDataStream::Ok) {
		int status = inStream.status();
		bail(QString("IPC FAIL. reading selector, status: %1").arg(inStream.status()).toStdString().c_str());
		return;
	}

	T data;
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

	if (selector == ScIpcLargeMessage::Selector) {
		QByteArray lotsOfBytes;
		{
			ScIpcLargeMessage* blob = new ScIpcLargeMessage(data);
			blob->receive(lotsOfBytes);
			delete blob;
		}
			
		QBuffer largeBuf(&lotsOfBytes);
		largeBuf.open(QIODevice::ReadOnly);
		QDataStream largeStream(&largeBuf);
			
		largeStream >> selector;
		largeStream >> data;
		largeBuf.close();
	}

	// call the handler
	(that->*each)(selector, data);


}

template <typename T> void ScIpcChannel::write(const QString &selector, const T &data) {

	if (!mSocket)
		return;

	QByteArray bytes;
	QBuffer buf(&bytes);
	buf.open(QIODevice::WriteOnly);
	QDataStream bufStream(&buf);

	bufStream << selector;
	bufStream << data;
	buf.close();
	
	quint32 left = bytes.size();
	quint32 total_length = left + 2 * sizeof(quint32);

	
	if (total_length < 4000) {
		QDataStream socketStream(mSocket);
		socketStream << bytes;
		total_length ^= CheckMask;
		socketStream << total_length;
	}
	else {
		ScIpcLargeMessage* blob = new ScIpcLargeMessage(bytes, mTag);
		blob->send(this); // returns after the other side has read it.
		delete blob;
	}
	
}

struct ScIpcLargeMessageMemory {
	quint32 mLength;
	qint32 mFresh;

	char *data(void *memory) {
		return static_cast<char *>(memory)+sizeof(ScIpcLargeMessageMemory);
	}

	void coolStoryBro() {
		mFresh = 0;
	}

	quint32 ripen() {
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
		data = memory->data(p);
		
		memcpy(data, bytes.constData(), (size_t)n);
		data[n] = '\0';
		shm->unlock();

	}

	// Here we are a reader.
	ScIpcLargeMessage(QString key) {
		
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
	ScIpcLargeMessage(QVariantList data) {}
	ScIpcLargeMessage(QStringList data) {}

	~ScIpcLargeMessage() {
		if (shm) {
			if (shm->isAttached())
				shm->detach();
			shm->deleteLater();
			shm = nullptr;
		}
	}

	void send(ScIpcChannel *channel) {
		QString key = shm->key();

		channel->write<QString>(Selector, key);
		
		// wait until other side has read this, give up when it's not fresh anymore.
		shm->lock();
		while (memory->ripen() > 0) {
			shm->unlock();
			QThread::msleep(250L);
			shm->lock();
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