/*
 * QtDownload.h
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

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class QtDownload : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource);
    Q_PROPERTY(QString destination READ destination WRITE setDestination);

public:
    explicit QtDownload();
    ~QtDownload();

    void setSource(const QString& t);
    void setDestination(const QString& l);
    QString source() { return m_target; }
    QString destination() { return m_local; }
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void download();

Q_SIGNALS:
    void doFinished();
    void doError();
    void doProgress(int, int);

private:
    QNetworkAccessManager* m_manager;
    QString m_target;
    QString m_local;
    QNetworkReply* m_reply;
    bool started;

public Q_SLOTS:
    void downloadFinished();
    void downloadProgress(qint64 recieved, qint64 total);
    void replyError(QNetworkReply::NetworkError errorCode);
};