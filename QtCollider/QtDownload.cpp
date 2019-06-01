/*
 * QtDownload.cpp
 *
 *
 * Copyright 2013 Scott Wilson.
 *
 * This file is part of SuperCollider.
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
 */

#include "Common.h"
#include "QtDownload.h"
#include "QcWidgetFactory.h"

#include <QCoreApplication>
#include <QUrl>
#include <QNetworkRequest>
#include <QFile>
#include <QDebug>

QC_DECLARE_QOBJECT_FACTORY(QtDownload);

QtDownload::QtDownload(): QObject(0), started(false) {}

QtDownload::~QtDownload() {}


void QtDownload::setSource(const QString& t) { m_target = t; }

void QtDownload::setDestination(const QString& l) { m_local = l; }

void QtDownload::downloadFinished() {
    if (m_reply->error() == QNetworkReply::NoError) { // only write if no error
        QFile localFile(this->m_local);
        if (!localFile.open(QIODevice::WriteOnly))
            return;
        const QByteArray sdata = m_reply->readAll();
        localFile.write(sdata);
        qDebug() << sdata;
        localFile.close();

        // call action
        Q_EMIT(doFinished());
    }

    m_reply->deleteLater();
    m_manager->deleteLater();
}

void QtDownload::download() {
    if (!started) {
        started = true;
        m_manager = new QNetworkAccessManager(this);
        QUrl url = QUrl::fromEncoded(this->m_target.toLocal8Bit());
        QNetworkRequest request;
        request.setUrl(url);
        m_reply = m_manager->get(request);
        QObject::connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this,
                         SLOT(downloadProgress(qint64, qint64)));
        QObject::connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
                         SLOT(replyError(QNetworkReply::NetworkError)));
        bool fin = QObject::connect(m_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
        if (!fin) {
            qWarning("Download could not connect");
        }
    }
}

void QtDownload::cancel() {
    if (m_reply) {
        m_reply->disconnect();
        m_reply->abort();
    }
}

void QtDownload::replyError(QNetworkReply::NetworkError errorCode) {
    qWarning() << m_reply->errorString();

    // call action
    Q_EMIT(doError());
}

void QtDownload::downloadProgress(qint64 received, qint64 total) {
    qDebug() << received << total;

    // call action
    Q_EMIT(doProgress(static_cast<int>(received), static_cast<int>(total)));
}