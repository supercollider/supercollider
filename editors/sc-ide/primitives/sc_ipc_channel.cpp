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

	//log(QString("write - %1 - complete.").arg(selector));
}
