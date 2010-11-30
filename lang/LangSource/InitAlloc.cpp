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


#include "InitAlloc.h"

AllocPool *pyr_pool_compile = 0;
AllocPool *pyr_pool_runtime = 0;

#define HOST_ALLOC(size)	malloc(size)
#define HOST_FREE(ptr)		free((void*)(ptr))

#define AREASIZE 65536L

void* pyr_new_area(size_t size);
void* pyr_new_area(size_t size)
{
#ifdef SC_WIN32
  size += kAlign;
  char* ptr = (char*)malloc(size);
  return (ptr + kAlign);
#else
	return (char*)HOST_ALLOC(size);
#endif
}

void pyr_free_area(void *ptr);
void pyr_free_area(void *ptr)
{
#ifdef SC_WIN32
  free((void*)((char*)ptr - kAlign));
#else
	HOST_FREE(ptr);
#endif
}

void* pyr_new_area_from_runtime(size_t size);
void* pyr_new_area_from_runtime(size_t size)
{
	void *ptr = pyr_pool_runtime->Alloc(size);
	MEMFAIL(ptr);
	return ptr;
}

void pyr_free_area_from_runtime(void *ptr);
void pyr_free_area_from_runtime(void *ptr)
{
	pyr_pool_runtime->Free(ptr);
}

bool pyr_init_mem_pools(int runtime_space, int runtime_grow)
{
	pyr_pool_runtime = new AllocPool(pyr_new_area, pyr_free_area, runtime_space, runtime_grow);
	if (!pyr_pool_runtime) return false;

	pyr_pool_compile = new AllocPool(pyr_new_area_from_runtime, pyr_free_area_from_runtime, 0L, AREASIZE);
	if (!pyr_pool_compile) return false;

	//pyr_pool_runtime->DoCheckPool();

	return true;
}
