/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.

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


#ifndef _ControlSpecs_
#define _ControlSpecs_
#include <Carbon/Carbon.h>
#include <AudioUnit/AudioUnitProperties.h>
#include "SC_World.h"


const  CFStringRef kSynthDefSpecKey = CFSTR("Synthdef");
const  CFStringRef kParamsSpecKey = CFSTR("Params");
const  CFStringRef kParamNameSpecKey = CFSTR("ParamName");
const  CFStringRef kMinValueSpecKey = CFSTR("MinValue");
const  CFStringRef kMaxValueSpecKey = CFSTR("MaxValue");
const  CFStringRef kDefaultValueSpecKey = CFSTR("DefaultValue");
const  CFStringRef kUnitSpecKey = CFSTR("Unit");
const  CFStringRef kDisplaySpecKey = CFSTR("Display");


class ControlSpecs {

public:
    ControlSpecs(CFDictionaryRef dict);
	CFStringRef getName(UInt32 specNum, CFStringRef nameKey);
    float getValue(UInt32 specNum, CFStringRef valueKey);
	void  setValue(UInt32 specNum, CFStringRef valueKey, float value);
	long  getDisplayFlag(UInt32 specNum);
    CFArrayRef params;
    CFIndex numberOfParameters;
    CFStringRef synthName;


};
#endif