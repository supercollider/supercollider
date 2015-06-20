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

#include "sc_ipc_channel.hpp"

// ----------------------------------------------------------------------------------------------
// ScIpcChannel - the interesting code for this is in the header, because they are templates.
// ------------
ScIpcChannel::ScIpcChannel(QTcpSocket *socket, const QString &tag, IIpcHandler *handler) : 
	mSocket(socket), 
	mTag(tag), 
	mIpcHandler(handler),
	mReadLock(),
	mWriteLock()
{};

ScIpcChannel::~ScIpcChannel() {
	bail();
}

void ScIpcChannel::bail() {
	if (mSocket) {
		mSocket->deleteLater();
		mSocket = nullptr;
	}
	mIpcHandler = nullptr;
}

void ScIpcChannel::log(const QString &message) {
	if (mIpcHandler != nullptr)
		mIpcHandler->onIpcLog(QString("%1 - IPC - %2\n").arg(mTag, message));
}


void ScIpcChannel::read() {

	if (!mSocket) {
		log("read - No socket.");
		return;
	}

	mReadLock.lock();

	while (mSocket->bytesAvailable() > 0) {

		QDataStream inStream(mSocket);
		//inStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

		quint32 bytesExpected;

		while (mSocket->bytesAvailable() < sizeof(bytesExpected)) {
			mSocket->waitForReadyRead();
		}

		inStream >> bytesExpected;
		if (inStream.status() != QDataStream::Ok) {
			log(QString("read - Error reading bytesExpected, status: %1").arg(inStream.status()));
			bail();
			return;
		}
		bytesExpected += sizeof(quint32);

		// spin until we have all the data
		while (mSocket->bytesAvailable() < bytesExpected)
			mSocket->waitForReadyRead();

		bytesExpected += sizeof(quint32);

		QString selector;
		inStream >> selector;
		if (inStream.status() != QDataStream::Ok) {
			log(QString("read - %1 - Error reading selector, status: %2").arg(selector, inStream.status()));
			bail();
			return;
		}

		QVariantList data;
		inStream >> data;

		if (inStream.status() != QDataStream::Ok) {
			log(QString("read - %1 - Error reading data, status: %2").arg(selector, inStream.status()));
			bail();
			return;
		}

		// at the end we expect to see check ^ CheckMask = the length specified by the other end at the start.
		// so we know we read the right number of bytes, and we can trust that the next thing we read will be another length.
		quint32 check;
		inStream >> check;
		if (inStream.status() != QDataStream::Ok) {
			log(QString("read - %1 - Error reading check, status: %2").arg(selector, inStream.status()));
			bail();
			return;
		}

		check ^= CheckMask;

		if (bytesExpected != check) {
			log(QString("read - %1 - Check failed: %2 != %3").arg(selector, bytesExpected, check));
			bail();
			return;
		}

		if (selector == LargeMessageSelector) {
			QByteArray lotsOfBytes;
			{
				// when we get LargeMessageSelector, the data is always a QString of the shared memory key.
				ScIpcLargeMessage* largeMessage = new ScIpcLargeMessage(data[0].toString(), mIpcHandler);
				largeMessage->receive(lotsOfBytes);
				delete largeMessage;
			}

			QBuffer largeBuf(&lotsOfBytes);
			largeBuf.open(QIODevice::ReadOnly);
			QDataStream largeStream(&largeBuf);
			//largeStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

			largeStream >> selector;
			if (largeStream.status() != QDataStream::Ok) {
				log(QString("read - largeStream - Error reading selector, status: %1").arg(largeStream.status()));
				bail();
				return;
			}

			//log(QString("Reading data from large message"));
			largeStream >> data;
			if (largeStream.status() != QDataStream::Ok) {
				log(QString("read - largeStream - %1 - Error reading check, status: %2").arg(selector, largeStream.status()));
				bail();
				return;
			}

			largeBuf.close();
		}

		mIpcHandler->onIpcMessage(selector, data);

	}

	mReadLock.unlock();
}

void ScIpcChannel::write(const QString &selector, const QVariantList &data) {

	//log(QString("write - %1 - started.").arg(selector));

	if (!mSocket) {
		log(QString("write - %1 - No socket.").arg(selector));
		return;
	}

	QByteArray bytes;
	QBuffer buf(&bytes);
	buf.open(QIODevice::WriteOnly);
	QDataStream bufStream(&buf);
	//bufStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

	bufStream << selector;
	if (bufStream.status() != QDataStream::Ok) {
		log(QString("write - %1 - bufStream - Error writing selector, status: %2").arg(selector, bufStream.status()));
		bail();
		return;
	}
	bufStream << data;
	if (bufStream.status() != QDataStream::Ok) {
		log(QString("write - %1 - bufStream - Error writing data, status: %2").arg(selector, bufStream.status()));
		bail();
		return;
	}

	buf.close();

	// when we serialize a QByteArray, the length (quint32) of the array is automatically sent first.
	// On the other side, we must adjust this, as we send a quint32 check after.
	quint32 left = bytes.size();
	quint32 totalLength = left + 2 * sizeof(quint32);

	if (totalLength < ScIpcChannel::MaxInbandSize) {
		mWriteLock.lock();
		QDataStream socketStream(mSocket);
		//socketStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

		socketStream << bytes;
		if (socketStream.status() != QDataStream::Ok) {
			log(QString("write - %1 - socketStream - Error writing selector, status: %2").arg(selector, socketStream.status()));
			bail();
			mWriteLock.unlock();
			return;
		}

		totalLength ^= CheckMask;
		socketStream << totalLength;
		if (socketStream.status() != QDataStream::Ok) {
			log(QString("write - %1 - socketStream - Error writing check, status: %2").arg(selector, socketStream.status()));
			bail();
			mWriteLock.unlock();
			return;
		}

		if (!mSocket->waitForBytesWritten()) {
			log(QString("write - %1 - mSocket->waitForBytesWritten() timeout - %2 bytes attempted.").arg(selector, totalLength));
		}
		mWriteLock.unlock();
	}
	else {
		ScIpcLargeMessage* largeMessage = new ScIpcLargeMessage(bytes, mTag, mIpcHandler);
		largeMessage->send(this); // returns after the other side has read it, or after we've given up.
		delete largeMessage;
	}

	//log(QString("write - %1 - complete.").arg(selector));
}
// ----------------------------------------------------------------------------------
// ScIpcLargeMessageMemory - The short-lived shared memory segment for large messages.
// -----------------------

char * ScIpcLargeMessageMemory::data(void *memory) {
	return static_cast<char *>(memory) + sizeof(ScIpcLargeMessageMemory);
}

void ScIpcLargeMessageMemory::coolStoryBro() {
	mFresh = 0;
	mStatus = DONE;
}

qint32 ScIpcLargeMessageMemory::ripen() {
	return --mFresh;
}

// ------------------------------------------------------
// ScIpcLargeMessage - implements a shared memory message
// -----------------

// Here we are a writer.
ScIpcLargeMessage::ScIpcLargeMessage(QByteArray &bytes, const QString &tag, IIpcHandler *logger) {
	static int i = 0;
	QString key = QString("ScIpcLargeMessage<%1>:%2").arg(tag, ++i);

	mIpcHandler = logger;

	quint32 bytesLength = bytes.size();
	quint32 totalLength = sizeof(ScIpcLargeMessageMemory) + bytesLength + 1;

	//log(QString("Creating shared memory segment - %1 - %2 bytes").arg(key, totalLength));

	shm = new QSharedMemory();
	shm->setKey(key);
	if (!shm->create(totalLength)) {
		log(QString("%1 - Couldn't create %2 byte shared memory; %3")
			.arg(key, QString(totalLength), shm->errorString()));
		return;
	}

	shm->lock();
	
	void *p = shm->data();
	
	memory = static_cast<ScIpcLargeMessageMemory*>(p);
	memory->mFresh = 10;
	memory->mStatus = ScIpcLargeMessageMemory::UNREAD;
	memory->mLength = bytesLength;

	data = memory->data(p);

	memcpy(data, bytes.constData(), (size_t)bytesLength);
	data[bytesLength] = '\0'; // just to be polite.
	shm->unlock();

}

// Here we are a reader.
ScIpcLargeMessage::ScIpcLargeMessage(const QString &key, IIpcHandler *logger) {

	mIpcHandler = logger;

	shm = new QSharedMemory();
	shm->setKey(key);
	if (!shm->attach()) {
		log(QString("%1 - Couldn't attach to shared memory; %2").arg(key, shm->errorString()));
		memory = nullptr;
		data = nullptr;
		return;
	}

	void *p = shm->data();
	memory = static_cast<ScIpcLargeMessageMemory*>(p);
	data = memory->data(p);

}

ScIpcLargeMessage::~ScIpcLargeMessage() {
	if (shm) {
		if (shm->isAttached())
			shm->detach();
		shm->deleteLater();
		shm = nullptr;
	}
}

void ScIpcLargeMessage::send(ScIpcChannel *channel) {
	const QString &key = shm->key();

	//log(QString("send - Signalling other side that there's data."));
	channel->write(channel->LargeMessageSelector, { QVariant(key) });

	// wait until other side has read this, give up when it's not fresh anymore.
	shm->lock();
	while (memory->ripen() > 0) {
		//log(QString("send - memory->mFresh == %1").arg(memory->mFresh));
		shm->unlock();
		QThread::msleep(200L);
		shm->lock();
	}

	if (memory->mStatus != ScIpcLargeMessageMemory::DONE) {
		log(QString("send - Other side didn't read shared memory, just deleting. status: %1").arg(memory->mStatus));
		shm->unlock();
		return;
	}

	shm->unlock();
}

void ScIpcLargeMessage::receive(QByteArray &dst) {
	shm->lock();
	if (memory->mFresh > 0) {
		dst.append(data, memory->mLength);
		memory->coolStoryBro();
		//log(QString("receive - Cool story bro."));
	}
	else {
		log(QString("receive - WARNING - Ignoring stale message."));
	}
	shm->unlock();
}

void ScIpcLargeMessage::log(const QString &message) {
	mIpcHandler->onIpcLog(QString("IPC Large Message: %1\n").arg(message));
}
