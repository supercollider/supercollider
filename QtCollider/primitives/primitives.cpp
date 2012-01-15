/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "primitives.h"
#include "../Common.h"
#include "QC_Export.h"
#include "QtCollider.h"

#include <SCBase.h>

namespace QtCollider {

PyrSymbol *s_interpretCmdLine;
PyrSymbol *s_interpretPrintCmdLine;
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
PyrSymbol *s_QTreeViewItem;

void defineQObjectPrimitives();
void defineQPenPrimitives();
void defineMiscPrimitives();
void defineQWidgetPrimitives();

QC_PUBLIC
void initPrimitives () {
  QtCollider::init();

  qcDebugMsg(1,"initializing QtGUI primitives");

  int base = nextPrimitiveIndex();
  int index = 0;

  defineQObjectPrimitives();
  defineQWidgetPrimitives();
  defineQPenPrimitives();
  defineMiscPrimitives();

  s_interpretCmdLine = getsym("interpretCmdLine");
  s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");

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
  s_QTreeViewItem = getsym("QTreeViewItem");
}

} // namespace QtCollider
