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
#include "QtCollider.h"

#include <SCBase.h>

using namespace QtCollider;

LangPrimitiveList& QtCollider::langPrimitives() {
  static LangPrimitiveList * primitives = new LangPrimitiveList();
  return *primitives;
}

namespace QtCollider {

PyrSymbol *s_doFunction;
PyrSymbol *s_doDrawFunc;
PyrSymbol *s_Rect;
PyrSymbol *s_Point;
PyrSymbol *s_Color;
PyrSymbol *s_Size;
PyrSymbol *s_Array;
PyrSymbol *s_FloatArray;
PyrSymbol *s_SymbolArray;
PyrSymbol *s_String;
PyrSymbol *s_QPalette;
PyrSymbol *s_QFont;
PyrSymbol *s_QObject;
PyrSymbol *s_QLayout;

QC_PUBLIC
void initPrimitives () {
  QtCollider::init();

  qcDebugMsg(1,"initializing QtGUI primitives");

  int base = nextPrimitiveIndex();
  int index = 0;
  LangPrimitiveList& primitives = langPrimitives();

  Q_FOREACH( LangPrimitiveData p, primitives ) {
    qcDebugMsg(2, QString("defining primitive '%1'").arg(p.name) );
    definePrimitive( base, index++, p.name, p.mediator, p.argc + 1, 0 );
  }

  s_doFunction = getsym("doFunction");
  s_doDrawFunc = getsym("doDrawFunc");

  s_Rect = getsym("Rect");
  s_Point = getsym("Point");
  s_Size = getsym("Size");
  s_Color = getsym("Color");
  s_Array = getsym("Array");
  s_FloatArray = getsym("FloatArray");
  s_SymbolArray = getsym("SymbolArray");
  s_String = getsym("String");
  s_QObject = getsym("QObject");
  s_QLayout = getsym("QLayout");
  s_QFont = getsym("QFont");
  s_QPalette = getsym("QPalette");
}

} // namespace QtCollider
