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

#include "session_manager.hpp"
#include "settings/manager.hpp"
#include "doc_manager.hpp"

#include "SC_DirUtils.h"

#include <QFile>
#include <QFileInfo>

namespace ScIDE {

static QString sessionFilePath( const QString & name )
{
    char config_dir[PATH_MAX];
    sc_GetUserConfigDirectory(config_dir, PATH_MAX);

    QDir dir(config_dir);

    if (!dir.mkpath("sessions")) {
        qWarning("The path to sessions does not exist and could not be created!");
        return QString();
    }

    dir.cd("sessions");

    return dir.filePath(name + ".yaml");
}

SessionManager::SessionManager( DocumentManager *docMng, QObject * parent ) :
    QObject(parent),
    mDocMng(docMng),
    mSession(0)
{

}

QDir SessionManager::sessionsDir()
{
    char config_dir[PATH_MAX];
    sc_GetUserConfigDirectory(config_dir, PATH_MAX);

    QDir dir(config_dir);

    if (dir.mkpath("sessions"))
        dir.cd("sessions");
    else {
        qWarning("The path to sessions does not exist and could not be created!");
        return QDir();
    }

    dir.setFilter( QDir::Files );
    dir.setNameFilters( QStringList() << "*.yaml" );

    return dir;
}

QStringList SessionManager::availableSessions()
{
    QStringList sessions = sessionsDir().entryList();
    QStringList::iterator it;
    for ( it = sessions.begin(); it != sessions.end(); ++it )
        *it = QFileInfo(*it).baseName();
    return sessions;
}

QString SessionManager::lastSession()
{
    QDir dir = sessionsDir();
    if (dir.path().isEmpty())
        return QString();

    QString path = QFile::symLinkTarget( dir.filePath( ".last-session.lnk" ) );

    return QFileInfo(path).baseName();
}

void SessionManager::newSession()
{
    closeSession();

    mDocMng->create();

    QDir dir = sessionsDir();
    if (!dir.path().isEmpty())
        saveLastSession( dir, QString() );

    emit currentSessionChanged(0);
}

Session *SessionManager::openSession( const QString & name )
{
    // NOTE: This will create a session if it doesn't exists

    closeSession();

    QDir dir = sessionsDir();
    if (dir.path().isEmpty())
        return 0;

    QString sessionFile = dir.filePath(name + ".yaml");
    mSession = new Session( sessionFile, name, Settings::serializationFormat() );

    saveLastSession( dir, sessionFile );

    emit loadSessionRequest(mSession);

    emit currentSessionChanged(mSession);

    return mSession;
}

void SessionManager::saveSession()
{
    if (mSession) {
        emit saveSessionRequest(mSession);
        mSession->sync();
    }
}

Session * SessionManager::saveSessionAs( const QString & name )
{
    // TODO:
    // Maybe use a different data structure for Session instead of QSettings?
    // A new class that would allow closing without saving would be nice.

    if (mSession) {
        delete mSession;
        mSession = 0;
    }

    QDir dir = sessionsDir();
    if (dir.path().isEmpty()) {
        emit currentSessionChanged(0);
        return 0;
    }

    QString sessionFile = dir.filePath(name + ".yaml");
    mSession = new Session( sessionFile, name, Settings::serializationFormat() );

    emit saveSessionRequest(mSession);

    mSession->sync();

    saveLastSession( dir, sessionFile );

    emit currentSessionChanged(mSession);

    return mSession;
}

void SessionManager::closeSession()
{
    if (mSession)
        emit saveSessionRequest(mSession);

    QList<Document*> docs = mDocMng->documents();
    foreach (Document *doc, docs)
        mDocMng->close(doc);

    delete mSession;
    mSession = 0;
}

bool SessionManager::saveLastSession( const QDir & dir, const QString & sessionFile )
{
    QString linkFile = dir.filePath(".last-session.lnk");

    if ( QFile::exists(linkFile) )
        if (!QFile::remove(linkFile)) {
            qWarning("Could not remove old link to last session!");
            return false;
        }

    if ( sessionFile.isEmpty() || QFile::link( sessionFile, linkFile ) )
        return true;
    else
        qWarning("Could not create link to last session!");

    return false;
}

} // namespace ScIDE
