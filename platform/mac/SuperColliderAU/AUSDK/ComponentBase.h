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
	ComponentBase.h

=============================================================================*/

#ifndef __ComponentBase_h__
#define __ComponentBase_h__

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreServices/CoreServices.h>
#else
	#include <ConditionalMacros.h>
	#include <CoreServices.h>
#endif
#include <new>
#include "CADebugMacros.h"

#if !TARGET_OS_WIN32
#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
	typedef Float32 AudioUnitParameterValue;
	typedef Float32	AudioSampleType;
#endif
#endif

#ifndef COMPONENT_THROW
	#define COMPONENT_THROW(err) \
		do { DebugMessage(#err); throw static_cast<OSStatus>(err); } while (0)
#endif

#define COMPONENT_CATCH \
	catch (std::bad_alloc &) { result = memFullErr; } \
	catch (OSStatus err) { result = err; } \
	catch (OSErr err) { result = err; } \
	catch (...) { result = -1; }

#if SUPPORT_AU_VERSION_1

#if TARGET_OS_WIN32
	#include "CAGuard.h"
#endif

class ComponentInitLocker
{
#if TARGET_OS_MAC
public:
	ComponentInitLocker() { pthread_mutex_lock(&sComponentOpenMutex); }
	~ComponentInitLocker() { pthread_mutex_unlock(&sComponentOpenMutex); }
private:
	static pthread_mutex_t sComponentOpenMutex;
#elif TARGET_OS_WIN32
public:
	bool sNeedsUnlocking;
	ComponentInitLocker() { sNeedsUnlocking = sComponentOpenGuard.Lock(); }
	~ComponentInitLocker() { if(sNeedsUnlocking) { sComponentOpenGuard.Unlock(); } }
private:
	static CAGuard	sComponentOpenGuard;
#endif
};

#endif


	/*! @class ComponentEntryPoint */
template <class Class>
class ComponentEntryPoint {
public:
	/*! @method Dispatch */
	static ComponentResult Dispatch(ComponentParameters *params, Class *obj)
	{
		ComponentResult result = noErr;

		try {
			if (params->what == kComponentOpenSelect) {
#if SUPPORT_AU_VERSION_1
				// solve a host of initialization thread safety issues.
				ComponentInitLocker lock;
#endif
				ComponentInstance ci = (ComponentInstance)(params->params[0]);
				Class *This = new Class(ci);
				This->PostConstructor();	// allows base class to do additional initialization
											// once the derived class is fully constructed

				SetComponentInstanceStorage(ci, (Handle)This);
			} else
				result = Class::ComponentEntryDispatch(params, obj);
		}
		COMPONENT_CATCH

		return result;
	}

	/*! @method Register */
	static Component Register(OSType compType, OSType subType, OSType manufacturer)
	{
		ComponentDescription	description = {compType, subType, manufacturer, 0, 0};
		Component	component = RegisterComponent(&description, (ComponentRoutineUPP) Dispatch, registerComponentGlobal, NULL, NULL, NULL);
		if (component != NULL) {
			SetDefaultComponent(component, defaultComponentAnyFlagsAnyManufacturerAnySubType);
		}
		return component;
	}
};

#if TARGET_OS_MAC && TARGET_CPU_PPC && !TARGET_RT_MAC_MACHO
	// for OS 9, a PPC native component's entry point must be a routine descriptor
	#define COMPONENT_ENTRY(Class) \
		extern "C" ComponentResult Class##Entry(ComponentParameters *params, Class *obj); \
		extern "C" ComponentResult Class##Entry(ComponentParameters *params, Class *obj) { \
			return ComponentEntryPoint<Class>::Dispatch(params, obj); \
		} \
	\
	struct RoutineDescriptor Class##EntryRD = \
		BUILD_ROUTINE_DESCRIPTOR((kPascalStackBased | RESULT_SIZE (kFourByteCode) | \
								STACK_ROUTINE_PARAMETER (1, kFourByteCode) | \
								STACK_ROUTINE_PARAMETER (2, kFourByteCode)), Class##Entry);
#else
	#define COMPONENT_ENTRY(Class) \
		extern "C" ComponentResult Class##Entry(ComponentParameters *params, Class *obj); \
		extern "C" ComponentResult Class##Entry(ComponentParameters *params, Class *obj) { \
			return ComponentEntryPoint<Class>::Dispatch(params, obj); \
		}
#endif

	/*! @class ComponentRegistrar */
template <class Class, OSType Type, OSType Subtype, OSType Manufacturer>
class ComponentRegistrar {
public:
	/*! @ctor ComponentRegistrar */
	ComponentRegistrar() { ComponentEntryPoint<Class>::Register(Type, Subtype, Manufacturer); }
};

#define	COMPONENT_REGISTER(Class,Type,Subtype,Manufacturer) \
	static ComponentRegistrar<Class, Type, Subtype, Manufacturer>	gRegistrar##Class


	/*! @class ComponentBase */
class ComponentBase {
public:
	/*! @ctor ComponentBase */
				ComponentBase(ComponentInstance inInstance) : mComponentInstance(inInstance) { }
	/*! @dtor ~ComponentBase */
	virtual 	~ComponentBase();

	/*! @method Version */
	virtual ComponentResult	Version();

	/*! @method PostConstructor */
	virtual void			PostConstructor();

	/*! @method PreDestructor */
	virtual void			PreDestructor();

	/*! @method ComponentEntryDispatch */
	static ComponentResult	ComponentEntryDispatch(ComponentParameters *p, ComponentBase *This);

	/*! @method GetComponentInstance */
	ComponentInstance		GetComponentInstance() const { return mComponentInstance; }

protected:
	/*! @var mComponentInstance */
	ComponentInstance		mComponentInstance;
};

#endif // __ComponentBase_h__
