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

#include "sc_ipc_channel.hpp"

// ----------------------------------------------------------------------------------------------
// ScIpcChannel - the interesting code for this is in the header, because they are templates.
// ------------
ScIpcChannel::ScIpcChannel(QTcpSocket *socket, QString &tag, IIpcLogger *logger) :
mTag(tag),
mLogger(logger)
{
	mSocket = socket;
};

ScIpcChannel::~ScIpcChannel() {
	bail();
}

void ScIpcChannel::bail() {
	if (mSocket) {
		mSocket->deleteLater();
		mSocket = nullptr;
	}
}

void ScIpcChannel::log(const QString &message) {
	mLogger->onIpcLog(QString("IPC: %1\n").arg(message));
}

// ----------------------------------------------------------------------------------
// ScIpcLargeMessageMemory - The short-lived shared memory segment for large messages.
// -----------------------

char * ScIpcLargeMessageMemory::data(void *memory) {
	return static_cast<char *>(memory)+sizeof(ScIpcLargeMessageMemory);
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
ScIpcLargeMessage::ScIpcLargeMessage(QByteArray &bytes, QString &tag, IIpcLogger *logger) {
	static int i = 0;
	QString key = QString("ScIpcLargeMessage<%1>:%2").arg(tag, ++i);

	mLogger = logger;

	quint32 n = bytes.size();
	quint32 totalLength = sizeof(ScIpcLargeMessageMemory) + n + 1;

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
	memory->mLength = n;
	memory->mFresh = 10;
	memory->mStatus = ScIpcLargeMessageMemory::UNREAD;
	data = memory->data(p);

	memcpy(data, bytes.constData(), (size_t)n);
	data[n] = '\0';
	shm->unlock();

}

// Here we are a reader.
ScIpcLargeMessage::ScIpcLargeMessage(QString &key, IIpcLogger *logger) {

	mLogger = logger;

	shm = new QSharedMemory();
	shm->setKey(key);
	if (!shm->attach()) {
		log(QString("%1 - Couldn't attach to shared memory; %2").arg(key, shm->errorString()));
		return;
	}

	void *p = shm->data();
	memory = static_cast<ScIpcLargeMessageMemory*>(p);
	data = memory->data(p);

}

// these are here so the template ScIpcChannel::read can compile. they are not used.
ScIpcLargeMessage::ScIpcLargeMessage(QVariantList &data, IIpcLogger *logger) {}
ScIpcLargeMessage::ScIpcLargeMessage(QStringList &data, IIpcLogger *logger) {

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
	QString &key = shm->key();

	//log(QString("Send - Signaling other side that there's data."));
	channel->write<QString>(channel->LargeMessageSelector, key);

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
	mLogger->onIpcLog(QString("IPC Large Message: %1\n").arg(message));
}