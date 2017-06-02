#ifndef SCIDE_LOCALSOCKET_UTILS_HPP
#define SCIDE_LOCALSOCKET_UTILS_HPP

#include <QByteArray>
#include <QLocalSocket>
#include <QDataStream>

// Aux functions for LocalSocket data handling
// Make template so that it supports QVariantLists and QStringList, which are te data types that we send in sc_process and main
QByteArray IntToArray(qint32 source);
qint32 ArrayToInt(QByteArray source);

template <typename T>
void sendSelectorAndData(QLocalSocket *socket, const QString& selector, const T& data)
{
    QByteArray baToStream;
    QDataStream stream(&baToStream, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_6);
    stream << selector;
    stream << data;
    // Write the length so that we know how long it is on the other end
    int len = baToStream.length();
    QByteArray baLen = IntToArray(len);
    socket->write(baLen);
    socket->write(baToStream);
    socket->flush();
}


#endif

