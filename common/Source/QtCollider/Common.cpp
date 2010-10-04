#include "Common.h"

#include <QApplication>
#include <QThread>

void QtCollider::lockLang()
{
  qscDebugMsg("locking lang!\n");
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
