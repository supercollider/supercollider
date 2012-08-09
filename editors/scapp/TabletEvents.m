/*----------------------------------------------------------------------------

FILE NAME

TabletEvents.m - Implamentation file for TabletEvent Category.
                 This is an extension to the NSEvent class.

COPYRIGHT

Copyright WACOM Technology, Inc. 2001.

All rights reserved.

----------------------------------------------------------------------------*/

#import "TabletEvents.h"

NSString *kProximityNotification = @"Proximity Event Notification"; 

NSString *kVendorID = @"vendorID";
NSString *kTabletID = @"tabletID";
NSString *kPointerID = @"pointerID";
NSString *kDeviceID = @"deviceID";
NSString *kSystemTabletID = @"systemTabletID";
NSString *kVendorPointerType = @"vendorPointerType";
NSString *kPointerSerialNumber = @"pointerSerialNumber";
NSString *kUniqueID = @"uniqueID";
NSString *kCapabilityMask = @"capabilityMask";
NSString *kPointerType = @"pointerType";
NSString *kEnterProximity = @"enterProximity"; 

@implementation NSEvent (TabletEvents) 
//////////////////////////////////////////////////////////////////////////////
- (void *)eventRef
{
   return _eventRef;
}



//////////////////////////////////////////////////////////////////////////////
- (BOOL) isTabletPointerEvent
{
	OSStatus	result;
   UInt32	eventType;
	
	switch (GetEventClass((EventRef)_eventRef))
	{
		case kEventClassMouse:
			result = GetEventParameter((EventRef)_eventRef,
										kEventParamTabletEventType,
										typeUInt32, NULL,
										sizeof( eventType ), NULL,
										&eventType	);

         if ( result == noErr )
         {
            if ( eventType == kEventTabletPoint )
            {
					return YES;
            }
         }
		break;
			
		case kEventClassTablet:
			if(GetEventKind((EventRef)_eventRef) == kEventTabletPoint)
			{
				return YES;
			}
		break;
			
		default:
		break;
	}
	
	return NO;
}



//////////////////////////////////////////////////////////////////////////////
- (BOOL) isTabletProximityEvent
{
	OSStatus	result;
   UInt32	eventType;

	switch (GetEventClass((EventRef)_eventRef))
	{
		case kEventClassMouse:
			result = GetEventParameter((EventRef)_eventRef,
										kEventParamTabletEventType,
										typeUInt32, NULL,
										sizeof( eventType ), NULL,
										&eventType	);

         if ( result == noErr )
         {
            if ( eventType == kEventTabletProximity )
            {
					return YES;
            }
         }
		break;

		case kEventClassTablet:
			if(GetEventKind((EventRef)_eventRef) == kEventTabletProximity)
			{
				return YES;
			}
		break;

		default:
		break;
	}

	return NO;
}



//////////////////////////////////////////////////////////////////////////////
- (TabletPointerRec) tabletRec
{
   OSStatus		result;
   TabletPointerRec tabletData;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   return tabletData;
}



//////////////////////////////////////////////////////////////////////////////
- (void) setLocation:(NSPoint)loc
{
   _location = loc;
}



//////////////////////////////////////////////////////////////////////////////
- (UInt16) deviceID
{
   OSStatus		result;
   TabletPointerRec tabletData;
   UInt16 theDeviceID = 0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      theDeviceID = tabletData.deviceID;
   }
   
   return theDeviceID;
}



//////////////////////////////////////////////////////////////////////////////
- (int) absoluteX
{
   OSStatus		result;
   TabletPointerRec tabletData;
   SInt32 absX = 0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      absX = tabletData.absX;
   }
   
   return absX;
}



//////////////////////////////////////////////////////////////////////////////
- (int) absoluteY
{
   OSStatus		result;
   TabletPointerRec tabletData;
   SInt32 absY = 0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      absY = tabletData.absX;
   }
   
   return absY;
}



//////////////////////////////////////////////////////////////////////////////
- (int) absoluteZ
{
   OSStatus		result;
   TabletPointerRec tabletData;
   SInt32 absZ = 0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      absZ = tabletData.absZ;
   }
   
   return absZ;
}



//////////////////////////////////////////////////////////////////////////////
- (void) getAbsoluteX:(SInt32*)absX Y:(SInt32*)absY Z:(SInt32*)absZ
{
   OSStatus		result;
   TabletPointerRec tabletData;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      if (absX != NULL)
      {
         *absX = tabletData.absX;
      }
      
      if (absY != NULL)
      {
         *absY = tabletData.absY;
      }
      
      if (absZ != NULL)
      {
         *absZ = tabletData.absZ;
      }
   }
}



//////////////////////////////////////////////////////////////////////////////
- (NSPoint) tilt
{
   OSStatus		result;
   TabletPointerRec tabletData;
   NSPoint theTilt = {0.0, 0.0};
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
   	theTilt.x = tabletData.tiltX/32767.0;
      theTilt.y = tabletData.tiltY/32767.0;
   }
   
   return theTilt;
}




//////////////////////////////////////////////////////////////////////////////
- (UInt16) rawTabletPressure
{
   OSStatus		result;
   TabletPointerRec tabletData;
   UInt16 pressure = 0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      pressure = tabletData.pressure;
   }
   
   return pressure;
}



//////////////////////////////////////////////////////////////////////////////
- (float) scaledTabletPressure
{
   OSStatus		result;
   TabletPointerRec tabletData;
   float pressure = 0.0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      pressure = (float)tabletData.pressure/65535.0;
   }
   
   return pressure;
}



//////////////////////////////////////////////////////////////////////////////
- (float) rotationInDegrees; /* 0.0¡ <-> +359.99999¡ */
{
   OSStatus		result;
   TabletPointerRec tabletData;
   UInt16 rotation = 0;
   float rotDeg = 0.0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      rotation = tabletData.rotation;
      rotDeg = (float)rotation/64.0;
   }
   
   return rotDeg;
}



//////////////////////////////////////////////////////////////////////////////
- (float) rotationInRadians; /* 0 <-> 2¹ */
{
   OSStatus		result;
   TabletPointerRec tabletData;
   UInt16 rotation = 0;
   float rotRad = 0.0;
   
   result = GetEventParameter((EventRef)_eventRef, kEventParamTabletPointerRec, 
										typeTabletPointerRec, NULL, 
										sizeof( TabletPointerRec ), NULL, &tabletData );
   
   if (result == noErr)
   {
      rotation = tabletData.rotation;
      rotRad = (float)rotation/64.0;
      rotRad = (rotRad * pi) / 180.0;
   }
   
   return rotRad;
}

@end
