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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef LANG_SIMPLESTACK_H
#define LANG_SIMPLESTACK_H
#include <PyrSlot.h>
typedef struct {
	ptr_sized_int *stak;
	short num, maxsize;
} LongStack;

void initLongStack(LongStack *self) ;
void freeLongStack(LongStack *self);
void growLongStack(LongStack *self);
void pushls(LongStack *self, ptr_sized_int value);
ptr_sized_int popls(LongStack *self);
int emptyls(LongStack *self);

#endif
