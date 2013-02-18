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

#ifndef SCIDE_SC_SERVER_HPP_INCLUDED
#define SCIDE_SC_SERVER_HPP_INCLUDED

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QAction>
#include <QProcess>

namespace ScIDE {

class ScProcess;
class VolumeWidget;
namespace Settings { class Manager; }

class ScServer : public QObject
{
    Q_OBJECT

public:
    enum ActionRole {
        ToggleRunning,
        Boot,
        Quit,
        Reboot,
        ShowMeters,
        DumpNodeTree,
        DumpNodeTreeWithControls,
        Mute,
        Volume,
        VolumeUp,
        VolumeDown,
        VolumeRestore,


        ActionCount
    };

    ScServer(ScProcess *scLang, Settings::Manager * settings, QObject * parent);

    bool isRunning() { return mPort != 0; }

    QAction *action(ActionRole role) { return mActions[role]; }

    float volume() const;
    void setVolume( float volume );

    bool isMuted() const;
    void setMuted( bool muted );


public slots:
    void boot();
    void reboot();
    void quit();
    void toggleRunning();
    void showMeters();
    void dumpNodeTree();
    void dumpNodeTreeWithControls();
    void queryAllNodes(bool dumpControls);
    void increaseVolume();
    void decreaseVolume();
    void restoreVolume();
    void mute() { setMuted(true); }
    void unmute() { setMuted(false); }

signals:
    void runningStateChange( bool running, QString const & hostName, int port );
    void updateServerStatus (int ugenCount, int synthCount, int groupCount, int defCount, float avgCPU, float peakCPU);

private slots:
    void onScLangStateChanged( QProcess::ProcessState );
    void onScLangReponse( const QString & selector, const QString & data );
    void updateToggleRunningAction();
    void sendMuted( bool muted );
    void sendVolume( float volume );

protected:
    virtual void timerEvent(QTimerEvent * event);

private:
    void createActions( Settings::Manager * );

    void handleRuningStateChangedMsg( const QString & data );
    void onRunningStateChanged( bool running, QString const & hostName, int port );

    QUdpSocket * mUdpSocket;
    QHostAddress mServerAddress;
    int mPort;

    QAction * mActions[ActionCount];

    VolumeWidget *mVolumeWidget;
};

}

#endif // SCIDE_SC_SERVER_HPP_INCLUDED
