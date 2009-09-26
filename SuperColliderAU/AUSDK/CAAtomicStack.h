/*	Copyright © 2007 Apple Inc. All Rights Reserved.

	Disclaimer: IMPORTANT:  This Apple software is supplied to you by
			Apple Inc. ("Apple") in consideration of your agreement to the
			following terms, and your use, installation, modification or
			redistribution of this Apple software constitutes acceptance of these
			terms.  If you do not agree with these terms, please do not use,
			install, modify or redistribute this Apple software.

			In consideration of your agreement to abide by the following terms, and
			subject to these terms, Apple grants you a personal, non-exclusive
			license, under Apple's copyrights in this original Apple software (the
			"Apple Software"), to use, reproduce, modify and redistribute the Apple
			Software, with or without modifications, in source and/or binary forms;
			provided that if you redistribute the Apple Software in its entirety and
			without modifications, you must retain this notice and the following
			text and disclaimers in all such redistributions of the Apple Software.
			Neither the name, trademarks, service marks or logos of Apple Inc.
			may be used to endorse or promote products derived from the Apple
			Software without specific prior written permission from Apple.  Except
			as expressly stated in this notice, no other rights or licenses, express
			or implied, are granted by Apple herein, including but not limited to
			any patent rights that may be infringed by your derivative works or by
			other works in which the Apple Software may be incorporated.

			The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
			MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
			THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
			FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
			OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

			IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
			OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
			INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
			MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
			AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
			STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
			POSSIBILITY OF SUCH DAMAGE.
*/
/*=============================================================================
	TStack.h

=============================================================================*/

#ifndef __TStack_h__
#define __TStack_h__

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <libkern/OSAtomic.h>
#else
	#include <DriverSynchronization.h>
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
	#include <CoreServices/CoreServices.h>
#endif

//  linked list LIFO or FIFO (pop_all_reversed) stack, elements are pushed and popped atomically
//  class T must implement set_next() and get_next()
template <class T>
class TAtomicStack {
public:
	TAtomicStack() : mHead(NULL) { }

	// non-atomic routines, for use when initializing/deinitializing, operate NON-atomically
	void	push_NA(T *item)
	{
		item->set_next(mHead);
		mHead = item;
	}

	T *		pop_NA()
	{
		T *result = mHead;
		if (result)
			mHead = result->get_next();
		return result;
	}

	bool	empty() { return mHead == NULL; }

	// atomic routines
	void	push_atomic(T *item)
	{
		T *head;
		do {
			head = mHead;
			item->set_next(head);
		} while (!compare_and_swap(head, item, &mHead));
	}

	void	push_multiple_atomic(T *item)
		// pushes entire linked list headed by item
	{
		T *head, *p = item, *tail;
		// find the last one -- when done, it will be linked to head
		do {
			tail = p;
			p = p->get_next();
		} while (p);
		do {
			head = mHead;
			tail->set_next(head);
		} while (!compare_and_swap(head, item, &mHead));
	}

	T *		pop_atomic_single_reader()
		// this may only be used when only one thread may potentially pop from the stack.
		// if multiple threads may pop, this suffers from the ABA problem.
		// <rdar://problem/4606346> TAtomicStack suffers from the ABA problem
	{
		T *result;
		do {
			if ((result = mHead) == NULL)
				break;
		} while (!compare_and_swap(result, result->get_next(), &mHead));
		return result;
	}

	T *		pop_atomic()
		// This is inefficient for large linked lists.
		// prefer pop_all() to a series of calls to pop_atomic.
		// push_multiple_atomic has to traverse the entire list.
	{
		T *result = pop_all();
		if (result) {
			T *next = result->get_next();
			if (next)
				// push all the remaining items back onto the stack
				push_multiple_atomic(next);
		}
		return result;
	}

	T *		pop_all()
	{
		T *result;
		do {
			if ((result = mHead) == NULL)
				break;
		} while (!compare_and_swap(result, NULL, &mHead));
		return result;
	}

	T*		pop_all_reversed()
	{
		TAtomicStack<T> reversed;
		T *p = pop_all(), *next;
		while (p != NULL) {
			next = p->get_next();
			reversed.push_NA(p);
			p = next;
		}
		return reversed.mHead;
	}

	bool	compare_and_swap(T *oldvalue, T *newvalue, T **pvalue)
	{
#if TARGET_OS_MAC
	#if __LP64__
			return ::OSAtomicCompareAndSwap64Barrier(int64_t(oldvalue), int64_t(newvalue), (int64_t *)pvalue);
	#elif MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
			return ::OSAtomicCompareAndSwap32Barrier(int32_t(oldvalue), int32_t(newvalue), (int32_t *)pvalue);
	#else
			return ::CompareAndSwap(UInt32(oldvalue), UInt32(newvalue), (UInt32 *)pvalue);
	#endif
#else
			return ::CompareAndSwap(UInt32(oldvalue), UInt32(newvalue), (UInt32 *)pvalue);
#endif
	}

protected:
	T *		mHead;
};

#endif // __TStack_h__
