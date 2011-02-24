#include "Common.h"

#include <QApplication>
#include <QThread>
#include <QAtomicInt>

static QAtomicInt& debugLevelInt() {
  static QAtomicInt *i = new QAtomicInt(0);
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

#ifdef QC_DEBUG
  QString msg;
#endif

  while( pthread_mutex_trylock (&gLangMutex) ) {
#ifdef QC_DEBUG
    msg += QChar('.');
#endif
    /* FIXME Dangerous! This sends to all QObjects. no matter what thread they
      live in */
    QApplication::sendPostedEvents( 0, QtCollider::Event_Sync );
    QThread::yieldCurrentThread();
  }

#ifdef QC_DEBUG
  msg += "locked";
#endif
  qcDebugMsg(2,msg);
}
