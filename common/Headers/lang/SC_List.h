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

/*

A doubly linked list template.

*/

#ifndef _SC_List_
#define _SC_List_

#include <stdexcept>
#include <assert.h>


// A Link can be a node in a list or a list itself.

template <class T>
class Link
{
public:
	Link() : mNext(this), mPrev(this) {}

	T* Prev() { return static_cast<T*>(mPrev); }
	T* Next() { return static_cast<T*>(mNext); }

	void RemoveLeaveDangling()
	{
		mPrev->mNext = mNext;
		mNext->mPrev = mPrev;
	}

	void Remove()
	{
		RemoveLeaveDangling();
		mNext = mPrev = this;
	}

	void InsertAfter(T *inLink)
	{
		mPrev = inLink;
		mNext = inLink->mNext;
		mNext->mPrev = this;
		mPrev->mNext = this;
	}

	void InsertBefore(T *inLink)
	{
		mNext = inLink;
		mPrev = inLink->mPrev;
		mNext->mPrev = this;
		mPrev->mNext = this;
	}

	T* Head() { return static_cast<T*>(mNext); }
	T* Tail() { return static_cast<T*>(mPrev); }

	T* PopHead();
	T* PopTail();
	void PushHead(T* inBuf);
	void PushTail(T* inBuf);

	bool ContainsBuf(T* inBuf);
	bool IsEmpty() { return mNext == this; }
	void BeEmpty() { mNext = mPrev = this; }

	void Cat(T* inLink);

	bool SanityCheck();
	void DebugDump();

//private:
// Codewarrior refuses to inline Next() in some places..
	Link<T> *mNext, *mPrev;
};

template <class T, class Alloc>
void MakeListEmpty(Link<T> *inLink, Alloc* inAlloc)
{
	Link<T>* link = inLink->mNext;
	while (link != inLink) {
		Link<T>* nextlink = link->mNext;
		// SC uses placement new extensively, so here we do a 'placement delete'.
		// Using DestructSelf allows me to have either virtual
		// or non virtual destructors in subclasses at the discretion of the subclass.
		((T*)(link))->DestructSelf();
		inAlloc->Free(static_cast<T*>(link));
		link = nextlink;
	}
	inLink->mNext = inLink->mPrev = inLink;
}

template <class T>
void Link<T>::PushHead(T* inLink)
{
	assert(SanityCheck());

	Link<T>* link = static_cast<Link<T>*>(inLink);
	link->InsertAfter(static_cast<T*>(this));

	assert(SanityCheck());
}

template <class T>
T* Link<T>::PopHead()
{
	assert(SanityCheck());
	if (IsEmpty()) return 0;

	Link<T>* link = mNext;

	link->Remove();

	assert(SanityCheck());
	return static_cast<T*>(link);
}

template <class T>
void Link<T>::PushTail(T* inLink)
{
	assert(SanityCheck());

	Link<T>* link = static_cast<Link<T>*>(inLink);
	link->InsertBefore(static_cast<T*>(this));

	assert(SanityCheck());
}

template <class T>
T* Link<T>::PopTail()
{
	assert(SanityCheck());
	if (IsEmpty()) return 0;

	Link<T>* link = mPrev;
	link->Remove();

	assert(SanityCheck());
	return static_cast<T*>(link);
}

template <class T>
void Link<T>::Cat(T* inLink)
{
	assert(SanityCheck());

	Link<T>* link = static_cast<Link<T>*>(inLink);

	if (link->IsEmpty()) return;
	if (IsEmpty()) {
		mNext = link->mNext;
		mPrev = link->mPrev;
		link->mNext->mPrev = this;
		link->mPrev->mNext = this;
	} else {
		link->mNext->mPrev = mPrev;
		link->mPrev->mNext = this;
		mPrev->mNext = link->mNext;
		mPrev = link->mPrev;
	}
	link->mPrev = link;
	link->mNext = link;

	assert(SanityCheck());
}

template <class T>
bool Link<T>::ContainsBuf(T* inLink)
{
	Link<T>* link = static_cast<Link<T>*>(inLink);
	Link<T>* curLink = mNext;
	while (curLink != this) {
		if (curLink == link) return true;
		curLink = curLink->mNext;
	}
	return false;
}

template <class T>
void Link<T>::DebugDump()
{
	Link<T>* link = mNext;
	while (link != this) {
		//post("Link-> %08X   next %08X   prev %08X\n",
		//	link, link->mNext, link->mPrev);
		link = link->mNext;
	}
}

template <class T>
bool Link<T>::SanityCheck()
{
	Link<T>* link = mNext;
	while (link != this) {
		if (link->mPrev->mNext != link) {
			throw std::runtime_error("Link: bad link <-,->");
		}
		if (link->mNext->mPrev != link) {
			throw std::runtime_error("Link: bad link ->,<-");
		}
		link = link->mNext;
	}
	return true;
}



#endif
