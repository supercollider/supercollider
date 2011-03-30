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
  pthread_mutex_lock (&gLangMutex);
  qcDebugMsg(2,"locked");
  return;
}
