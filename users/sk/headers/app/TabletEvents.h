/*----------------------------------------------------------------------------

FILE NAME

TabletEvents.h - Header file for TabletEvent Category.
                 This is an extension to the NSEvent class.

COPYRIGHT

Copyright WACOM Technology, Inc. 2001.

All rights reserved.

----------------------------------------------------------------------------*/

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

@interface NSEvent ( TabletEvents )

- (void *)eventRef;
- (BOOL) isTabletPointerEvent;
- (BOOL) isTabletProximityEvent;
- (void) setLocation:(NSPoint)loc;
- (TabletPointerRec) tabletRec;
- (SInt32) absoluteX;
- (SInt32) absoluteY;
- (SInt32) absoluteZ;
- (void) getAbsoluteX:(SInt32*)absX Y:(SInt32*)absY Z:(SInt32*)absZ;
- (NSPoint) tilt;
- (UInt16) rawTabletPressure;
- (float) scaledTabletPressure;
- (float) rotationInDegrees; /* 0¡ <-> +359.9999¡ */
- (float) rotationInRadians; /* 0 <-> 2¹ */
- (UInt16) deviceID;

@end

///////////////////////////////////////////////////////////////////////////
/* This is the name of the Notification sent when a proximity event is
   captured by the application */
extern NSString *kProximityNotification;   

/* vendor-defined ID - typically will be USB vendor ID */
extern NSString *kVendorID; 

/* vendor-defined tablet ID  */
extern NSString *kTabletID;

/* vendor-defined ID of the specific pointing device */
extern NSString *kPointerID; 

/*  unique device ID - matches to deviceID field in tablet event */
extern NSString *kDeviceID;

/*  unique tablet ID */
extern NSString *kSystemTabletID;

/* vendor-defined pointer type */
extern NSString *kVendorPointerType;      

 /* vendor-defined serial number of the specific pointing device */
extern NSString *kPointerSerialNumber;

 /* vendor-defined unique ID for this pointer */
extern NSString *kUniqueID;

/* mask representing the capabilities of the device */
extern NSString *kCapabilityMask;

 /* type of pointing device - enum to be defined */
extern NSString *kPointerType;

 /* non-zero = entering; zero = leaving */
extern NSString *kEnterProximity;  
