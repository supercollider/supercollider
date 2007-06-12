/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "SC_World.h"
#include "SC_Graph.h"
#include "SC_Unit.h"
#include "SC_Wire.h"
#include "SC_InterfaceTable.h"
#include "Unroll.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "SC_BoundsMacros.h"
#include "SC_RGen.h"
#include "SC_DemandUnit.h"
#include "clz.h"
#include "sc_msg_iter.h"
#include "SC_Altivec.h"
#include <stdlib.h>

#ifdef SC_WIN32

// temporarily override __attribute__ for (unused), later we'll remove it
#ifndef __GCC__
#define __attribute__(x)
#endif

// workaround for IN/OUT conflict with Win32 headers. see SC_Unit.h for details
#define IN SC_IN
#define OUT SC_OUT

#ifdef _MSC_VER
#include <xmath.h>
#endif //_MSC_VER

#endif
