//
//  sc_ipc_client.hpp
//  SuperCollider
//
//  Created by Scott Wilson on 08/08/2013.
//
//

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
    
    void setTextMirrorForDocument(QByteArray & id, const QString & text, int pos = 0, int range = -1);

private Q_SLOTS:
    
    void readIDEData();
    
private:
    
    void onResponse( const QString & selector, const QVariantList & argList );
    
    void updateDocText( const QVariantList & argList );
    
    QByteArray mIpcData;
    QHash<QByteArray, QString> mDocumentTextMirrors;
    QMutex mTextMirrorHashMutex;
};



#endif
