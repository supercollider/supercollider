#include "Common.h"
#include "Slot.h"

#include <PyrKernel.h>
#include <VMGlobals.h>
#include <PyrLexer.h>

// WARNING: QtCollider::lockLang() must be called before
void QtCollider::runLang (
  PyrObjectHdr *receiver,
  PyrSymbol *method,
  const QList<QVariant> & args,
  PyrSlot *result )
{
  VMGlobals *g = gMainVMGlobals;
  g->canCallOS = true;
  ++g->sp;  SetObject(g->sp, receiver);
  Q_FOREACH( QVariant var, args ) {
    ++g->sp;
    if( !Slot::setVariant( g->sp, var ) )
      SetNil( g->sp );
  }
  runInterpreter(g, method, args.size() + 1);
  g->canCallOS = false;
  if (result) slotCopy(result, &g->result);
}

int QtCollider::wrongThreadError ()
{
  qcErrorMsg( "You can not use this Qt functionality in the current thread. "
              "Try scheduling on AppClock instead." );
  return errFailed;
}
