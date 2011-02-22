/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#include "primitives/primitives.h"
#include "Common.h"
#include "QC_Export.h"

namespace QtCollider {
  PyrSymbol *symSize;
}

using namespace QtCollider;

LangPrimitiveList& QtCollider::langPrimitives() {
  static LangPrimitiveList * primitives = new LangPrimitiveList();
  return *primitives;
}

namespace QtCollider {

QC_PUBLIC
void initPrimitives () {
  qcDebugMsg(1,"initializing QtGUI primitives");

  int base = nextPrimitiveIndex();
  int index = 0;
  LangPrimitiveList& primitives = langPrimitives();

  Q_FOREACH( LangPrimitiveData p, primitives ) {
    qcDebugMsg(2, QString("defining primitive '%1'").arg(p.name) );
    definePrimitive( base, index++, p.name, p.mediator, p.argc + 1, 0 );
  }

  symSize = getsym("Size");
}

} // namespace QtCollider
