/*
 NewPyrObjectPtr.h
 SuperCollider

 Created by Scott Wilson on 02/01/2018.
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

#ifndef NewPyrObjectPtr_h
#define NewPyrObjectPtr_h

class PyrGC;

struct PyrObjectHdr;
struct PyrObject;

class NewPyrObjectPtr
{
private:
	PyrObjectHdr* mPyrObj;
	bool mMadeReachable = false;
	class PyrGC* mGC;
	
public:
	void incrementGC();
	void decrementGC();
	
	NewPyrObjectPtr(class PyrGC* inGC = 0, PyrObjectHdr* inPtr = 0) : mPyrObj(inPtr), mGC(inGC)
	{
		// eventually, this should happen in the GC itself to avoid circumvention
		// for now it happens here for testing purposes and backwards compatibility.
		incrementGC();
	}
	
	NewPyrObjectPtr(NewPyrObjectPtr&other) = delete; // no copy constructor
	
	NewPyrObjectPtr& operator=(NewPyrObjectPtr &other) = delete; // no assignment operator
	
	// move constructor
	NewPyrObjectPtr(NewPyrObjectPtr&& other) : mPyrObj(other.mPyrObj), mMadeReachable(other.mMadeReachable), mGC(other.mGC)
	{
		other.mPyrObj = nullptr;
	}
	
	NewPyrObjectPtr& operator=(NewPyrObjectPtr &&other) // move assignment constructor
	{
		if (this != &other) {
			mPyrObj = other.mPyrObj;
			mMadeReachable = other.mMadeReachable;
			mGC = other.mGC;
			other.mPyrObj = nullptr;
		}
		return *this;
	}
	
	operator bool() const // test for nullptr/if object has been set or moved
	{
		return bool(mPyrObj);
	}
	
	~NewPyrObjectPtr();
	
	PyrObjectHdr* release() //consume the object
	{
		mMadeReachable = true;
		PyrObjectHdr* released = mPyrObj;
		mPyrObj = nullptr;
		decrementGC();
		return released;
	}
	
	PyrObjectHdr* releaseAndWriteNew(PyrObject* parent);
	
	PyrObjectHdr* get() // access without consuming
	{
		mMadeReachable = true;
		return mPyrObj;
	}
	
	// might be needed
	PyrObjectHdr& operator* () const
	{
		return *mPyrObj;
	}
	
	PyrObjectHdr* operator-> () const
	{
		return mPyrObj;
	}
};

#endif /* NewPyrObjectPtr_h */
