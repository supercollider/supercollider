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

#include <PyrPrimitive.h>
#include <VMGlobals.h>
#include <PyrSlot.h>

#include <QList>

#include <cstring>

namespace QtCollider {

struct LangPrimitiveData {
  PrimitiveHandler mediator;
  char *name;
  int argc;
};

typedef QList<LangPrimitiveData> LangPrimitiveList;

LangPrimitiveList& langPrimitives();

template <int FN( PyrSlot*, PyrSlot*, VMGlobals* )>
class LangPrimitive
{
public:
  LangPrimitive ( const char *name, int argc ) {
    LangPrimitiveData d;
    d.mediator = &mediate;
    d.name = strdup(name);
    d.argc = argc;
    langPrimitives().append( d );
  }
private:
  static int mediate( VMGlobals *g, int i ) {
    PyrSlot *stack = g->sp - i + 1;
    int ret = (*FN)( stack, i > 1 ? stack+1 : 0, g );
    return ret;
  }
};

} // namespace

#define QC_LANG_PRIMITIVE( name, argc, receiver, args, global ) \
  int name ( receiver, args, global ); \
  static QtCollider::LangPrimitive<&name> p_##name( "_" #name, argc ); \
  int name ( receiver, args, global )
