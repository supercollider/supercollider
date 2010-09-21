#include "Common.h"
#include "Slot.h"

#include <PyrKernel.h>
#include <VMGlobals.h>

#include <QApplication>
#include <QThread>

void QtCollider::execute(PyrObject *obj, PyrSymbol *method,
                        const QList<QVariant> & args, PyrSlot *result,
                        bool locked)
{
  if (!obj) {
    qscErrorMsg("QtCollider::execute: no SC object!\n");
    return;
  }

  qscDebugMsg("+++ QtCollider::execute\n");

  if( !locked ) {
    lockLang();
  }

  VMGlobals *g = gMainVMGlobals;
  g->canCallOS = true;
  ++g->sp;  SetObject(g->sp, obj);
  Q_FOREACH( QVariant var, args ) {
    ++g->sp;
    if( Slot::setVariant( g->sp, var ) )
      SetNil( g->sp );
  }
  runInterpreter(g, method, args.size() + 1);
  g->canCallOS = false;
  if (result) slotCopy(result, &g->result);

  if( !locked ) unlockLang();

  qscDebugMsg("--- QtCollider::execute\n");
}

void QtCollider::lockLang()
{
  qscDebugMsg("locking lang!\n");
  //printf("trying to lock\n");
  QString msg;
  while( pthread_mutex_trylock (&gLangMutex) ) {
    /* FIXME Dangerous! This sends to all QObjects. no matter what thread they
      live in */
    msg += QChar('.');
    QApplication::sendPostedEvents( 0, CreationType );
    QApplication::sendPostedEvents( 0, CustomType );
    QThread::yieldCurrentThread();
  }
  //msg += "locked\n";
  //printf("%s", msg.toStdString().c_str() );
}
