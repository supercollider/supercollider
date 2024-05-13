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

#include "doc_manager.hpp"
#include "session_manager.hpp"
#include "settings/manager.hpp"
#include "util/standard_dirs.hpp"

#include "../widgets/main_window.hpp"

#include <QFile>
#include <QFileInfo>

namespace ScIDE {

SessionManager::SessionManager(DocumentManager* docMng, QObject* parent):
    QObject(parent),
    mDocMng(docMng),
    mCurrentSession(0) {}

QDir SessionManager::sessionsDir() {
    QDir dir(standardDirectory(ScConfigUserDir));

    if (dir.mkpath("sessions"))
        dir.cd("sessions");
    else {
        qWarning("The path to sessions does not exist and could not be created!");
        return QDir();
    }

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.yaml");

    return dir;
}

QStringList SessionManager::availableSessions() {
    QStringList sessions = sessionsDir().entryList();
    QStringList::iterator it;
    for (it = sessions.begin(); it != sessions.end(); ++it)
        *it = QFileInfo(*it).baseName();
    return sessions;
}

QString SessionManager::lastSession() {
    QDir dir = sessionsDir();
    if (dir.path().isEmpty())
        return QString();

    QString path = QFile::symLinkTarget(dir.filePath(".last-session.lnk"));

    return QFileInfo(path).baseName();
}

void SessionManager::newSession() {
    if (!closeSession())
        return;

    QDir dir = sessionsDir();
    if (!dir.path().isEmpty())
        saveLastSession(dir, QString());

    emit switchSessionRequest(0);
}

Session* SessionManager::openSession(const QString& name) {
    // NOTE: This will create a session if it doesn't exists

    if (!closeSession())
        return 0;

    QDir dir = sessionsDir();
    if (dir.path().isEmpty())
        return 0;

    QString sessionFile = dir.filePath(name + ".yaml");
    mCurrentSession = new Session(sessionFile, name, Settings::serializationFormat());

    saveLastSession(dir, sessionFile);

    emit switchSessionRequest(mCurrentSession);

    return mCurrentSession;
}

void SessionManager::saveSession() {
    if (mCurrentSession) {
        emit saveSessionRequest(mCurrentSession);
        mCurrentSession->sync();
    }
}

Session* SessionManager::saveSessionAs(const QString& name) {
    // TODO:
    // Maybe use a different data structure for Session instead of QSettings?
    // A new class that would allow closing without saving would be nice.

    if (mCurrentSession) {
        delete mCurrentSession;
        mCurrentSession = 0;
    }

    QDir dir = sessionsDir();
    if (dir.path().isEmpty()) {
        emit switchSessionRequest(0);
        return 0;
    }

    QString sessionFile = dir.filePath(name + ".yaml");
    mCurrentSession = new Session(sessionFile, name, Settings::serializationFormat());

    emit saveSessionRequest(mCurrentSession);

    mCurrentSession->sync();

    saveLastSession(dir, sessionFile);

    emit currentSessionNameChanged();

    return mCurrentSession;
}

bool SessionManager::closeSession() {
    if (!MainWindow::instance()->promptSaveDocs())
        return false;

    if (mCurrentSession)
        emit saveSessionRequest(mCurrentSession);

    delete mCurrentSession;
    mCurrentSession = 0;
    return true;
}

void SessionManager::removeSession(const QString& name) {
    QDir dir = sessionsDir();
    if (dir.path().isEmpty())
        return;

    if (mCurrentSession && mCurrentSession->name() == name) {
        if (!closeSession())
            return;
        saveLastSession(dir, QString());
        emit switchSessionRequest(0);
    }

    if (!QFile::remove(dir.filePath(name + ".yaml")))
        qWarning("Could not remove a session file!");
}

void SessionManager::renameSession(const QString& oldName, const QString& newName) {
    if (mCurrentSession && mCurrentSession->name() == oldName) {
        saveSessionAs(newName);
        removeSession(oldName);
    } else {
        QDir dir = sessionsDir();
        if (dir.path().isEmpty())
            return;

        if (!dir.rename(oldName + ".yaml", newName + ".yaml"))
            qWarning("Could not rename session file!");
    }
}

bool SessionManager::saveLastSession(const QDir& dir, const QString& sessionFile) {
    QString linkFile = dir.filePath(".last-session.lnk");

    if (QFile::exists(linkFile))
        if (!QFile::remove(linkFile)) {
            qWarning("Could not remove old link to last session!");
            return false;
        }

    if (sessionFile.isEmpty() || QFile::link(sessionFile, linkFile))
        return true;
    else
        qWarning("Could not create link to last session!");

    return false;
}

} // namespace ScIDE
