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
#include <QTimer>
#include <boost/chrono/system_clocks.hpp>
#include <osc/OscReceivedElements.h>

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
        KillAll,
        Reboot,
        ShowMeters,
        ShowScope,
        ShowFreqScope,
        DumpNodeTree,
        DumpNodeTreeWithControls,
        PlotTree,
        DumpOSC,
        Mute,
        Volume,
        VolumeUp,
        VolumeDown,
        VolumeRestore,
        Record,
        PauseRecord,

        ActionCount
    };

    ScServer(ScProcess *scLang, Settings::Manager * settings, QObject * parent);

    bool isRunning() { return mPort != 0; }

    QAction *action(ActionRole role) { return mActions[role]; }

    Q_PROPERTY( float volume READ volume WRITE setVolume NOTIFY volumeChanged )

    float volume() const;
    void setVolume( float volume );
    void setVolumeRange( float min, float max );

    bool isMuted() const;
    void setMuted( bool muted );

    bool isDumpingOSC() const;
    void setDumpingOSC( bool dumping );

    bool isRecording() const;
    bool isPaused() const;

    int recordingTime() const;

public slots:
    void boot();
    void reboot();
    void quit();
    void killAll();
    void toggleRunning();
    void showMeters();
    void showScope();
    void showFreqScope();
    void dumpNodeTree();
    void dumpNodeTreeWithControls();
    void plotTree();
    void queryAllNodes(bool dumpControls);
    void increaseVolume();
    void decreaseVolume();
    void changeVolume(float);
    void restoreVolume();
    void mute() { setMuted(true); }
    void unmute() { setMuted(false); }
    void sendRecording( bool active );
    void setRecording( bool active );
    void pauseRecording( bool flag );

signals:
	void runningStateChanged( bool running, QString const & hostName, int port, bool unresponsive );
	void updateServerStatus (int ugenCount, int synthCount,
                             int groupCount, int defCount,
                             float avgCPU, float peakCPU);
    void volumeChanged( float volume );
    void volumeRangeChanged( float min, float max);
    void mutedChanged( bool muted );
    void recordingChanged( bool recording );
    void pauseChanged( bool paused );


private slots:
    void onScLangStateChanged( QProcess::ProcessState );
    void onScLangReponse( const QString & selector, const QString & data );
    void onServerDataArrived();
    void updateToggleRunningAction();
    void updateRecordingAction();
    void updateEnabledActions();
    void sendMuted( bool muted );
    void sendVolume( float volume );
    void sendDumpingOSC( bool dumping );

protected:
    virtual void timerEvent(QTimerEvent * event);

private:
    void createActions( Settings::Manager * );
    void handleRuningStateChangedMsg( const QString & data );
    void onRunningStateChanged( bool running, QString const & hostName, int port );

    void processServerStatusMessage( const osc::ReceivedMessage & );

    void processOscMessage( const osc::ReceivedMessage & );

    void processOscPacket( const osc::ReceivedPacket & packet )
    {
        if (packet.IsMessage())
            processOscMessage( osc::ReceivedMessage(packet) );
        else
            processOscBundle( osc::ReceivedBundle(packet) );
    }

    void processOscBundle( const osc::ReceivedBundle & bundle )
    {
        for (auto iter = bundle.ElementsBegin(); iter != bundle.ElementsEnd(); ++iter)
        {
            const osc::ReceivedBundleElement & element = *iter;
            if (element.IsMessage())
                processOscMessage( osc::ReceivedMessage(element) );
            else
                processOscBundle( osc::ReceivedBundle(element) );
        }
    }

    ScProcess *mLang;

    QUdpSocket * mUdpSocket;
    QHostAddress mServerAddress;
    int mPort;

    QAction * mActions[ActionCount];

    float mVolume = 0, mVolumeMin = -90, mVolumeMax = 6;
    VolumeWidget *mVolumeWidget;
    int mRecordingSeconds;
    bool mIsRecording;
    bool mIsRecordingPaused;
};

}

#endif // SCIDE_SC_SERVER_HPP_INCLUDED
