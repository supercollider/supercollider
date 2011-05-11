#include "Common.h"

void QtCollider::lockLang()
{
  qcDebugMsg(2,"locking lang!");
  pthread_mutex_lock (&gLangMutex);
  qcDebugMsg(2,"locked");
  return;
}

int QtCollider::wrongThreadError ()
{
  qcErrorMsg( "You can not use this Qt functionality in the current thread. "
              "Try scheduling on AppClock instead." );
  return errFailed;
}
