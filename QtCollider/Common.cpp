#include "Common.h"

#include <QApplication>
#include <QThread>
#include <QAtomicInt>

static QAtomicInt& debugLevelInt() {
  static QAtomicInt *i = new QAtomicInt(1);
  return *i;
}

int QtCollider::debugLevel() {
  int l = debugLevelInt();
  return l;
}

void QtCollider::setDebugLevel( int i ) {
  debugLevelInt() = i;
}

void QtCollider::lockLang()
{
  qcDebugMsg(2,"locking lang!");
  //printf("trying to lock\n");
  QString msg;
  while( pthread_mutex_trylock (&gLangMutex) ) {
    /* FIXME Dangerous! This sends to all QObjects. no matter what thread they
      live in */
    msg += QChar('.');
    QApplication::sendPostedEvents( 0, QtCollider::Event_Sync );
    QThread::yieldCurrentThread();
  }
  //msg += "locked\n";
  //printf("%s", msg.toStdString().c_str() );
}
