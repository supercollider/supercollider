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

#ifndef SuperCollider_sc_ipc_client_hpp
#define SuperCollider_sc_ipc_client_hpp

#include <QObject>
#include <QLocalSocket>
#include <QMutex>
#include <QHash>

class SCIpcClient : public QObject
{
    Q_OBJECT
public:
    SCIpcClient( const char * ideName );
    
    void send(const char * data, size_t length);
    
    ~SCIpcClient();
    QLocalSocket * mSocket;
    
    QString getTextMirrorForDocument(QByteArray & id, int pos = 0, int range = -1);
    QPair<int, int> getSelectionMirrorForDocument(QByteArray & id);
    
    void setTextMirrorForDocument(QByteArray & id, const QString & text, int pos = 0, int range = -1);
    void setSelectionMirrorForDocument(QByteArray & id, int start = 0, int range = 0);

private Q_SLOTS:
    
    void readIDEData();
    
private:
    
    void onResponse( const QString & selector, const QVariantList & argList );
    
    void updateDocText( const QVariantList & argList );
    void updateDocSelection( const QVariantList & argList );
    
    int mReadSize = 0;
    QByteArray mIpcData;
    QHash<QByteArray, QString> mDocumentTextMirrors;
    QHash<QByteArray, QPair<int, int>> mDocumentSelectionMirrors;
    QMutex mTextMirrorHashMutex;
    QMutex mSelMirrorHashMutex;
};



#endif
