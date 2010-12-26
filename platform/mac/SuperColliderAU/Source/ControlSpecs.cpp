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

#include "ControlSpecs.h"


ControlSpecs::ControlSpecs(CFDictionaryRef dict){
	if (CFDictionaryContainsKey(dict, kParamsSpecKey)){
        params =  (CFArrayRef) CFDictionaryGetValue(dict, kParamsSpecKey);
        numberOfParameters = CFArrayGetCount(params);
        synthName =  (CFStringRef)CFDictionaryGetValue(dict, kSynthDefSpecKey);
	}
}


float  ControlSpecs::getValue(UInt32 specNum, CFStringRef valueKey)
{
    float value = 0;
    CFDictionaryRef thisParam  = (CFDictionaryRef) CFArrayGetValueAtIndex(params, specNum);
    CFNumberRef cfValue = (CFNumberRef)CFDictionaryGetValue(thisParam, valueKey);
	if(cfValue) CFNumberGetValue(cfValue,kCFNumberFloat32Type,(void *)&value);
    return value;
}



void  ControlSpecs::setValue(UInt32 specNum, CFStringRef valueKey, float value)
{
    CFDictionaryRef thisParam  = (CFDictionaryRef) CFArrayGetValueAtIndex(params, specNum);
    CFDictionarySetValue((CFMutableDictionaryRef)thisParam, (void*)valueKey,(void *)&value);
}


CFStringRef  ControlSpecs::getName(UInt32 specNum, CFStringRef nameKey)
{
    CFDictionaryRef thisParam  = (CFDictionaryRef) CFArrayGetValueAtIndex(params, specNum);
    return (CFStringRef)CFDictionaryGetValue(thisParam, nameKey);
}


long  ControlSpecs::getDisplayFlag(UInt32 specNum)
{
    int value = 0;
    CFDictionaryRef thisParam  = (CFDictionaryRef) CFArrayGetValueAtIndex(params, specNum);
    CFNumberRef cfValue = (CFNumberRef)CFDictionaryGetValue(thisParam, kDisplaySpecKey);
	if (cfValue)CFNumberGetValue(cfValue,kCFNumberIntType,(void *)&value);
	switch(value){
		case 0: return 0;
		case 1: return kAudioUnitParameterFlag_DisplaySquareRoot;
		case 2: return kAudioUnitParameterFlag_DisplaySquared;
		case 3: return kAudioUnitParameterFlag_DisplayCubed;
		case 4: return kAudioUnitParameterFlag_DisplayCubeRoot;
		case 5: return kAudioUnitParameterFlag_DisplayExponential;
		case 6: return kAudioUnitParameterFlag_DisplayLogarithmic;
	}
    return value;
}
