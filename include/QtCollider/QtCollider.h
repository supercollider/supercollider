#ifndef QT_COLLIDER_H
#define QT_COLLIDER_H

#include "QC_Export.h"

namespace QtCollider {
  QC_PUBLIC void initPrimitives ();
  QC_PUBLIC void init();
  QC_PUBLIC int exec(int argc, char** argv);
  QC_PUBLIC int run(int argc, char** argv);
}

#endif // QT_COLLIDER_H
