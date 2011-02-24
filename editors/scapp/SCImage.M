//
//  SCImage.M
//	SCImage primitive set for SuperCollider3
//
//  blackrain 2008
//  charles picasso 2008
//


//#if SCIMAGE_MAC_OS_10_4

#import "SCImage.h"
#import <sys/time.h>
#import <pthread.h>
#import <objc/objc.h>
#import <objc/objc-class.h>
#import <ctype.h>

#import "SCBase.h"
#import "PyrSymbol.h"
#import "PyrPrimitive.h"
#import "PyrObject.h"
#import "PyrSlot.h"
#import "PyrKernel.h"
#import "VMGlobals.h"
#import "SCGeom.h"
#import "SCVirtualMachine.h"
#import "GC.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if (MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
#define class_getSuperclass(a)	((a)->super_class)
#define class_getName(a)		((a)->name)
typedef float ColorData;
#else
typedef CGFloat ColorData;
#endif

extern int slotGetPoint(PyrSlot* a, NSPoint *p);
extern int slotGetNSRect(PyrSlot* a, NSRect *r);
extern int allocSlotStrVal(PyrSlot *slot, char **str);
extern int slotColorVal(PyrSlot *slot, SCColor *sccolor);
extern int slotStrLen(PyrSlot* slot);

extern pthread_mutex_t	gLangMutex; // in case
PyrSymbol				*s_scimage;
PyrSymbol				*s_scfilter;
PyrSymbol				*s_scimagekernel;
static BOOL				gCIFilterPlugInsLoaded = NO; //

CGRect OutsetRect(CGRect r, const float dx, const float dy);

/*
 * SCImage Finalizer callback function
 */
int FinalizeSCImageObject(struct VMGlobals* g, struct PyrObject* object)
{
	if( !IsNil(object->slots+0) ) {
		SCImage * scimage = (SCImage*)slotRawPtr(object->slots);
		#if SCIMAGE_DEBUG
		post("finalize scimage (%p), dataptr (%p)\n", object, scimage);
		#endif
		[scimage release];
		SetNil(object->slots+0);
	}
	return errNone;
}

/*
 * SCImageKernel Finalizer callback function
 */
int			FinalizeSCImageKernelObject(struct VMGlobals* g, struct PyrObject* object)
{
	if( !IsNil(object->slots+4) ) {
		CIKernel * kernel = (CIKernel*)slotRawPtr(&object->slots[4]);
		#if SCIMAGE_DEBUG
		post("finalize scimagekernel (%p), dataptr (%p)\n", object, kernel);
		#endif
		[kernel release];
		SetNil(object->slots+4);
	}
	return errNone;
}

/* This function should be the one to use to create a new PyrObject of class s_scimage
 * it will setup directly the Finalizer object
 * so the PyrObject will free directly the SCImage instance
 * Not calling this function in any other situation may result in a memory leak.
 */
PyrObject*	newPyrSCImage(VMGlobals* g)
{
	PyrObject* object = instantiateObject(g->gc, s_scimage->u.classobj, 0, false, true);
	if(object)
		InstallFinalizer(g, object, 9, FinalizeSCImageObject);
	return object;
}

inline NSString*		nsStringFromSlot(PyrSlot *slot)
{
	int len;
	NSString *nsString;

	len = slotStrLen(slot); // check if Symbol or String
	if(len == -1) // returns -1 if failure
		return nil;

	nsString = [[NSString alloc]initWithBytes:IsSym(slot) ? (void*)slotRawSymbol(slot)->name : (void*)slotRawString(slot)->s length:len encoding:NSASCIIStringEncoding];

	return [nsString autorelease];
}

//==================================================================================================
// SCImage uses now two models:  NSBitmapImageRep + CIImage underneath
//
// Concerning NSBitmapImageRep
// ---------------------------
// (+) advantages : Quartz compliant - pixel access + manipulation via simple setters
// although NSBitmapImageRep does handle only pre-multiplied alpha pixels
// NSAlphaNonpremultipliedBitmapFormat does not work in 10.5 so not portable for now...

// Concerning CIImage
// ------------------
// (-) drawbacks : no direct pixel manipulation like the nsbitmapimagerep (wich is anyway a not very good model for that too...
// a CGImage should be better
// we may need also to download _from_ gpu memory to get the nsbitmapimagerep if CoreImage uses acceleration
// (+) advantages : can be in gpu memory
// possibility to use the CoreImage possibilities : Filters...ect...
//==================================================================================================

// Getting Pixel Macro -
// handle different conversions but there is roundoff errors... so
// integer division -> better seems to be the MultiplyAlpha
// change COLOR_SCALE macro to point to another function for a _forward_ alpha premultiplication
// change COLOR_UNSCALE macro to point to another function for a _reverse_ alpha premultiplication
// better to use getPixel:AtX:y because it handles the unscale really better

#pragma mark -- Pixel Macros

//#if (0)
static const float	oneOn255			= (1.f/255.f);


// Get Rid of signed integer interpretation in sc - kill the 2-complement info
#define PIXEL_RED(pixel)				((pixel >> 24) & 0x000000FF)
#define PIXEL_GREEN(pixel)				((pixel >> 16) & 0x000000FF)
#define PIXEL_BLUE(pixel)				((pixel >> 8) & 0x000000FF)
#define PIXEL_ALPHA(pixel)				(pixel & 0x000000FF)

// Setting Pixel Macro
#define SET_PIXEL_RED(pixel, value)		pixel = (pixel & 0x00FFFFFF) | ((value) << 24)
#define SET_PIXEL_GREEN(pixel, value)	pixel = (pixel & 0xFF00FFFF) | ((value) << 16)
#define SET_PIXEL_BLUE(pixel, value)	pixel = (pixel & 0xFFFF00FF) | ((value) << 8)
#define SET_PIXEL_ALPHA(pixel, value)	pixel = (pixel & 0xFFFFFF00) | (value)

// Macro for working with premultiplied alpha data
// May introduce several roundoff errors if several convert - unconvert passes so... careful with that...
#define COLOR_SCALE						COLOR_SCALE2
#define COLOR_SCALE1(channel, alpha)	(((channel) * (alpha)) + 127) / 255 // simple
#define COLOR_SCALE2(channel, alpha)	MultiplyAlpha(alpha, channel)

// none of those are ok :(
#define COLOR_UNSCALE					COLOR_UNSCALE3
#define COLOR_UNSCALE1(channel, alpha)	((channel) * 255) / alpha // simple Formula - rounding errors if scale - unscale !!! so...
#define COLOR_UNSCALE2(channel, alpha)	((channel) * 256 - 1) / (alpha) // ...modified one... beware
#define COLOR_UNSCALE3(channel, alpha)	((((channel) * 255) + ((alpha) >> 1)) / (alpha)) // should be better - from libnr - sodipodi

// more Macros see libnr - sodipodi here:
// http://sodipodi.cvs.sourceforge.net/sodipodi/libnr/nr-pixops.h
// but currently none handles de-premultiplication better than the getPixel method


static inline int MultiplyAlpha(const int a, const int r)
{
     register int temp;
     temp = (r * a) + 0x80;
     return ((temp + (temp >> 8)) >> 8);
}

#pragma mark -- Graphics Context Utilities
// returns a new rgba bitmap context. you owns the returned context. should be released after
NSBitmapImageRep* CreateBitmapContextRGBA(const int width, const int height)
{
	return
		[[NSBitmapImageRep alloc]
			initWithBitmapDataPlanes:NULL
			pixelsWide:width
			pixelsHigh:height
			bitsPerSample:8
			samplesPerPixel:4
			hasAlpha:YES
			isPlanar:NO
			colorSpaceName:NSCalibratedRGBColorSpace //may be NSDeviceRGBColorSpace is faster since it matches hardware properties
			// bitmapFormat: NSAlphaNonpremultipliedBitmapFormat // better for pixel manipulation - does not work on 10.5
			// bitmapFormat: NSAlphaFirstBitmapFormat
			bytesPerRow:/*(width * height * 4)*/ 0 // calc word alignment - faster
			bitsPerPixel:0 // guess it for me ;-)
		];
}

// creates and return an autoreleased NSBitmapImageRep built from an CIImage or an NSImage
// if image is nil then it will return a zero filled pixel NSBitmapImageRep
NSBitmapImageRep* BitmapContextRGBAWithImage(const int width, const int height, id image)
{
	NSGraphicsContext *nsPrevCtx = NULL;
	NSBitmapImageRep *bitmap = CreateBitmapContextRGBA(width, height);
	if(!bitmap) {
		NSLog(@"Error: CreateBitmapContextRGBAWithImage Failed creating valid RGBA Bitmap context");
		return nil;
	}

	if(image) {

			[NSGraphicsContext saveGraphicsState];

			NSGraphicsContext *nsCtx =
				[NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap];

			if(!nsCtx) {
				NSLog(@"Error: CreateBitmapContextRGBAWithImage Failed creating valid NSGraphicsContext with RGBA Bitmap Attributes");
				[bitmap release];
				return nil;
			}

			[NSGraphicsContext setCurrentContext:nsCtx]; // set the RGBA ctx

			if([image isKindOfClass:[NSImage class]]) {
				if([image isValid]) {
					[image drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0]; // draw inside of it
				}
				else {
					NSLog(@"Error: invalid image supplied as input to the bitmap context");
					goto failure;
				}
			}
			else if([image isKindOfClass:[CIImage class]]) {
				CGRect extent = [image extent];
				extent.origin = (CGPoint){0.f, 0.f};
				[ [nsCtx CIContext] drawImage:image inRect:CGRectMake(0.f, 0.f, (float)width, (float)height) fromRect:extent];
			}
			else {
				NSLog(@"Error: Invalid class supplied as input to bitmap context !");
				goto failure;
			}

			[NSGraphicsContext restoreGraphicsState];

	}
	else {
		memset([bitmap bitmapData], 0, [bitmap bytesPerRow]*[bitmap pixelsHigh]); // better to clear everything
	}

	return [bitmap autorelease];

failure:
	[bitmap release];
	if(nsPrevCtx) [NSGraphicsContext setCurrentContext:nsPrevCtx]; // restore previous ctx
	return nil;
}

#pragma mark -- SCImage Class

@implementation SCImage

-(id)initWithCIImage:(CIImage*)image extent:(CGRect)extent format:(CIFormat)imageFormat
{
	if([super init]) {

		[self commonInit];
		_ciFormat = imageFormat;

#if SCIMAGE_MAC_OS_10_5
		_ciimageStore = [[CIImageAccumulator allocWithZone:[self zone]]initWithExtent:extent format:_ciFormat];
		if(!_ciimageStore) {
			NSLog(@"SCImage initWithContentsOfURL failed creating valid CIImageAccumulator !");
			goto failure;
		}
		[_ciimageStore setImage:image];
#else
		// extent and format cannot be choosen
		_ciimage = [image retain];
#endif
		_accelerated = true;
		[self setShouldSyncBitmap:YES];

		return self;
	}
#if SCIMAGE_MAC_OS_10_5
failure:
#endif
	[super dealloc];
	return nil;
}

-(id)initWithNSImage:(NSImage*)image {
	if([super init]) {

		// TODO: clean up the initFromURL method...ect... to use this method instead
		NSSize size;
		size = [image size];

		/*
		 NSImage seems to have an incorrect size depending on the dpi of the image
		 find the largest representation in the nsimage and the set the size according to it
		 */
		int pixelsHigh=0, pixelsWide=0; // should be NSInteger in 10.5 - but stick with an <int> for now
		NSImageRep *irep;
		NSEnumerator *e = [[image representations]objectEnumerator];
		while( (irep = [e nextObject]) ) {
			/*
			 do not check ratio - they should be always preserved - just take the max
			 they should always be part of the same rep
			 */
			pixelsHigh = sc_max([irep pixelsHigh], pixelsHigh);
			pixelsWide = sc_max([irep pixelsWide], pixelsWide);
		}

		if( size.width < (float)pixelsWide || size.height < (float)pixelsHigh ) {
			//post("converting : %f %f -> %i %i\n", size.width, size.height, pixelsWide, pixelsHigh);
			[image setSize:NSMakeSize((float)pixelsWide, (float)pixelsHigh)];
			size = [image size];
		}

		_bitmap = BitmapContextRGBAWithImage([image size].width, [image size].height, image);
		if(!_bitmap)
			goto failure;
		[_bitmap retain];
		[self rebuildNSImageFromBitmapRep];

		_accelerated = NO;
		[self commonInit];
		[self setShouldSyncCIImage:YES];

		return self;
	}

failure:
	[super dealloc];
	return nil;
}

-(id)initWithSize:(NSSize)size isAccelerated:(BOOL)yorn format:(CIFormat)imageFormat
{
	if([super init]) {

		_bitmap = BitmapContextRGBAWithImage((int)size.width, (int)size.height, nil);
		if(!_bitmap)
			goto failure;
		[_bitmap retain];
		[self rebuildNSImageFromBitmapRep];

		if(yorn) {
			[self rebuildCIImageFromBitmapRep];
		}

		_accelerated = yorn;
		[self commonInit];

		if(!yorn)
			[self setShouldSyncCIImage:YES];

		return self;
	}

failure:
	[super dealloc];
	return nil;
}

-(id)copy {
	return [[SCImage alloc]initWithNSImage:[self nsimage]]; // perform a full pixel copy
}

-(id)initWithContentsOfURL:(NSURL*)url isAccelerated:(BOOL)yorn format:(CIFormat)imageFormat
{
	if([super init]) {

		_ciFormat = imageFormat;
		[self commonInit];

		if(yorn) {
			CIImage *ciimage = [[CIImage alloc] initWithContentsOfURL:url options:[NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:_ciFormat], @"CIFormat"]];
#if SCIMAGE_MAC_OS_10_5
			_ciimageStore = [[CIImageAccumulator allocWithZone:[self zone]]initWithExtent:[ciimage extent] format:_ciFormat];
			if(!_ciimageStore) {
				NSLog(@"SCImage initWithContentsOfURL failed creating valid CIImageAccumulator !");
				[ciimage release];
				goto failure;
			}
			[_ciimageStore setImage:ciimage];
#else
			_ciimage = [ciimage retain];
#endif
		}
		else
		{
			NSImage *nsimage = [[NSImage alloc] initWithContentsOfURL:url];
			if(!nsimage)
				goto failure;

			NSSize size;
			size = [nsimage size];

			/*
				NSImage seems to have an incorrect size depending on the dpi of the image
				find the largest representation in the nsimage and the set the size according to it
			*/
			int pixelsHigh=0, pixelsWide=0; // should be NSInteger in 10.5 - but stick with an <int> for 10.4
			NSImageRep *irep;
			NSEnumerator *e = [[nsimage representations]objectEnumerator];
			while( (irep = [e nextObject]) ) {
				/*
				do not check ratio - they should be always preserved - just take the max
				they should always be part of the same rep
				*/
				pixelsHigh = sc_max([irep pixelsHigh], pixelsHigh);
				pixelsWide = sc_max([irep pixelsWide], pixelsWide);
			}

			if( size.width < (float)pixelsWide || size.height < (float)pixelsHigh ) {
				//post("converting : %f %f -> %i %i\n", size.width, size.height, pixelsWide, pixelsHigh);
				[nsimage setSize:NSMakeSize((float)pixelsWide, (float)pixelsHigh)];
				size = [nsimage size];
			}

			_bitmap = BitmapContextRGBAWithImage( (int)size.width, (int)size.height, nsimage );
			if(!_bitmap) {
				[nsimage release];
				goto failure;
			}
			[_bitmap retain];
			[nsimage release];
		}

		_accelerated = yorn;

		if(yorn)
			[self setShouldSyncBitmap:YES];
		else
			[self setShouldSyncCIImage:YES];

		return self;
	}

failure:
	[super dealloc];
	return nil;
}

// Common initializer for both models
-(void)commonInit
{
	_inited = NO;
	_bitmapSynced = _ciimageSynced = YES;
	_interpolation = NSImageInterpolationDefault;
	_nsimage = nil;
#if SCIMAGE_MAC_OS_10_5
	_ciimageStore = nil;// CAREFUL: may lead to error / crash if commonInit is called after previous _ciimageStore initialization
#else
	_ciimage = nil;// CAREFUL: may lead to error / crash if commonInit is called after previous _ciimage initialization
#endif
	_wLock = [[NSLock alloc]init];
	_scales = NO;
	_prevCtx = NULL;
	_hints = (SCImageHint)1; // do not optimize drawing when using non-accelerated model - hence using filter will do nothing
	_inited = YES; // initialization flag
	_savedCtxState = NO;
	_cache = NULL;
}
- (void)setDrawingHint:(SCImageHint)hint
{
	_hints = hint; //
}
-(void)setScalesWhenResized:(BOOL)yorn
{
	_scales = yorn;
	[[self nsimage]setScalesWhenResized:yorn]; // test for now
}
-(BOOL)scalesWhenResized
{
	return _scales;
}
-(void)setSize:(NSSize)s
{
	if([self size].width != s.width || [self size].height != s.height)
	{
		[self lock]; // in case

		#if SCIMAGE_DEBUG
			NSLog(@"Resizing Image %f %f", s.width, s.height);
		#endif

		BOOL scales = [self scalesWhenResized];

		NSBitmapImageRep *new_bitmap = CreateBitmapContextRGBA(s.width, s.height);
	//	NSGraphicsContext* prevctx = [NSGraphicsContext currentContext];

		[NSGraphicsContext saveGraphicsState];

		NSGraphicsContext *nctx =
		[
			NSGraphicsContext graphicsContextWithAttributes:
				[NSDictionary dictionaryWithObject:new_bitmap forKey:NSGraphicsContextDestinationAttributeName]
		]; // create new RGBA ctx with bitmap as destination

		[NSGraphicsContext setCurrentContext:nctx];
		if([self isAccelerated])
		{
			if(scales)
				[[nctx CIContext]drawImage:[self ciimage] inRect:CGRectMake(0.f, 0.f, s.width, s.height) fromRect:[self extent]];
			else
				[[nctx CIContext]drawImage:[self ciimage] atPoint:CGPointMake(0.f, 0.f) fromRect:[self extent]];
		}
		else
		{
			if(scales)
				[_bitmap drawInRect:NSMakeRect(0.f, 0.f, s.width, s.height)];
			else
				[_bitmap drawAtPoint:NSMakePoint(0.f, 0.f)];
		}
	//	[NSGraphicsContext setCurrentContext:prevctx];
		[NSGraphicsContext restoreGraphicsState];

		[_bitmap release];
		_bitmap = [new_bitmap retain];

		[self rebuildNSImageFromBitmapRep];

		if([self isAccelerated])
			[self rebuildCIImageFromBitmapRep];

		_ciimageSynced = _bitmapSynced = YES; // everybody's synced

		[self unlock];
	}
}
-(void)setAccelerated:(BOOL)yorn
{
	if(yorn != _accelerated) {
		[self syncRepresentations]; // sync representations
		_accelerated = yorn;
	}
}
-(BOOL)isAccelerated
{
	return _accelerated;
}
-(BOOL)isSynced
{
	return (_bitmapSynced && _ciimageSynced);
}
-(NSImage*)nsimage
{
	if(!_nsimage)
		[self rebuildNSImageFromBitmapRep]; // in case

	return _nsimage;
}
-(CIImage*)ciimage
{
#if SCIMAGE_MAC_OS_10_5
	if(!_ciimageStore && _bitmap)
		[self rebuildCIImageFromBitmapRep];
	return [_ciimageStore image];
#else
	if(!_ciimage && _bitmap)
		[self rebuildCIImageFromBitmapRep];
	return _ciimage;
#endif

}
-(NSBitmapImageRep*)bitmapRepresentation
{
#if SCIMAGE_MAC_OS_10_5
	if(!_bitmap && _ciimageStore)
#else
	if(!_bitmap && _ciimage)
#endif
		[self rebuildBitmapRepFromCIImage];// in case

	return _bitmap;
}
-(void)rebuildNSImageFromBitmapRep
{
	if(!_bitmap) {
		NSLog(@"Error: SCImage rebuildCIImageFromBitmapRep: No valuable Bitmap Context to rebuild the CIImage");
		return;
	}

	if(_nsimage) {
		[_nsimage release];
		_nsimage = nil;
	}
	_nsimage = [[NSImage alloc]initWithSize:NSMakeSize([_bitmap pixelsWide], [_bitmap pixelsHigh])]; // simple store
	[_nsimage addRepresentation:_bitmap];

	#if SCIMAGE_DEBUG
		NSLog(@"SCImage (%p) Recaching NSImage (%p) from BitmapRep (%p)", self, _nsimage, _bitmap);
	#endif
}
-(void)rebuildCIImageFromBitmapRep
{
	if(!_bitmap) {
		NSLog(@"Error: SCImage rebuildCIImageFromBitmapRep: No valuable Bitmap Context to rebuild the CIImage");
		return;
	}

#if SCIMAGE_MAC_OS_10_5
	if(_ciimageStore)
		[_ciimageStore release];

	CIImage * _ciimage = [[CIImage alloc]initWithBitmapImageRep:[self bitmapRepresentation]];
	_ciimageStore = [[CIImageAccumulator allocWithZone:[self zone]]initWithExtent:[_ciimage extent] format:_ciFormat];
	//#endif
	if(_ciimageStore) {
		[_ciimageStore setImage:[_ciimage autorelease]];
	}
	else
		NSLog(@"Error: SCImage (%p) Failed Creating valid CIImageAccumulator");
#else
	[_ciimage release];
	_ciimage = [[CIImage alloc]initWithBitmapImageRep:[self bitmapRepresentation]];
#endif

	#if SCIMAGE_DEBUG
		NSLog(@"SCImage (%p) Recaching CIImage (%p) from BitmapRep (%p)", self, _ciimage, _bitmap);
	#endif
}
-(void)rebuildBitmapRepFromCIImage
{
	// rebuild a BitmapContext from the CIImage
	// expensive since it may reload it from the GPU.
	// only if	- user wants to manipulate pixels after using filter
	//			- user wants to save data in file
	if(_bitmap) {
		[_bitmap release];
		_bitmap = nil;
	}
	CIImage *ciimage = [self ciimage];
	CGRect extent = [ciimage extent];
	_bitmap = BitmapContextRGBAWithImage((int)extent.size.width, (int)extent.size.height, ciimage); // create a context from CIImage
	[_bitmap retain]; // retain it since it is an autoreleased one
	#if SCIMAGE_DEBUG
		NSLog(@"SCImage (%p) Recaching NSBitmapImageRep (%p) from CIImage (%p)", self, _bitmap, ciimage);
	#endif
}

-(void)recache
{
	// currently does nothing
}

- (NSImageInterpolation)imageInterpolation
{
	return _interpolation;
}

- (void)setImageInterpolation:(NSImageInterpolation)interp
{
	#if SCIMAGE_DEBUG
		NSLog(@"SCImage (%p) setImageInterpolation: %i", self, interp);
	#endif
	_interpolation = interp;
}

// update all representations from the current used rep
-(void)syncRepresentations
{
	if([self isSynced])
		return;

	if([self isAccelerated]) {
		[self syncBitmap];
	}else{
		[self syncCIImage];
	}
}

-(void)syncBitmap {
	if(!_bitmapSynced) {
		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) syncBitmap", self);
		#endif
		[self rebuildBitmapRepFromCIImage];
		[self rebuildNSImageFromBitmapRep];
		_bitmapSynced = YES;
	}
}

-(void)syncCIImage {
	if(!_ciimageSynced) {
		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) syncCIImage", self);
		#endif
		[self rebuildCIImageFromBitmapRep];
		_ciimageSynced = YES;
	}
}

- (BOOL)hasChanged {
	return (![self isSynced] || (_cache == NULL));
}

-(void)setShouldSyncCIImage:(BOOL)yorn {
	_ciimageSynced = !yorn;
}

-(void)setShouldSyncBitmap:(BOOL)yorn {
	_bitmapSynced = !yorn;
}

-(int)width
{
	return (int)[self size].width;
}

-(int)height
{
	return (int)[self size].height;
}

-(CGRect)extent {
	return [self isAccelerated] ? [[self ciimage]extent] : CGRectMake(0.f, 0.f, (float)[_bitmap pixelsWide], (float)[_bitmap pixelsHigh]);
}

-(NSSize)size {
	if([self isAccelerated]) {
		CGSize s = [[self ciimage]extent].size;
		return NSMakeSize(s.width, s.height);
	}else{
		return NSMakeSize([_bitmap pixelsWide], [_bitmap pixelsHigh]);
	}
}

-(unsigned char*)bitmapData
{
	if([self isAccelerated])
		[self syncBitmap];
	return [[self bitmapRepresentation] bitmapData];
}

-(void)setPixel:(PixelData *)pixel atX:(int)x y:(int)y
{
	if([self isAccelerated])
		[self syncBitmap];
	[[self bitmapRepresentation] setPixel:pixel atX:(long)x y:(long)y];
	[self setShouldSyncCIImage:YES];
}

-(void)setColor:(NSColor*)color atX:(unsigned int)x y:(unsigned int)y
{
	if([self isAccelerated])
		[self syncBitmap];
	[[self bitmapRepresentation] setColor:color atX:x y:y];
	[self setShouldSyncCIImage:YES];
}

-(void)getPixel:(PixelData *)pixel atX:(int)x y:(int)y
{
	if([self isAccelerated])
		[self syncBitmap];
	[[self bitmapRepresentation] getPixel:pixel atX:x y:y];
}

-(NSColor*)colorAtX:(unsigned int)x y:(unsigned int)y
{
	if([self isAccelerated])
		[self syncBitmap];
	return [[self bitmapRepresentation]colorAtX:x y:y];
}

-(void)lock {
	[_wLock lock];
}

-(void)unlock {
	[_wLock unlock];
}

-(void)lockFocus
{
	[self lock];
	if([self isAccelerated])
		[self syncBitmap];

	//_prevCtx = [NSGraphicsContext currentContext];

	_savedCtxState = YES;
	[NSGraphicsContext saveGraphicsState];

	NSGraphicsContext *nsCtx =
	[NSGraphicsContext graphicsContextWithAttributes:
	 [NSDictionary dictionaryWithObject:[self bitmapRepresentation] forKey:NSGraphicsContextDestinationAttributeName]
	 ]; // create new RGBA ctx with bitmap as destination

	if(!nsCtx) {
		NSLog(@"Error: SCImage lockFocus: Failed setting valid NSGraphicsContext");
		_prevCtx = NULL;
		[NSGraphicsContext restoreGraphicsState];
	}
	if(_interpolation != NSImageInterpolationDefault) {
		[nsCtx setImageInterpolation:_interpolation];
	}
	[NSGraphicsContext setCurrentContext:nsCtx]; // set the RGBA ctx

}

-(void)unlockFocus
{
//	if(_prevCtx)
//		[NSGraphicsContext setCurrentContext:_prevCtx];

	if ( _savedCtxState )
		[NSGraphicsContext restoreGraphicsState];

	_savedCtxState = NO;

	_prevCtx = NULL;

	[self setShouldSyncCIImage:YES];
	[self unlock];
}

- (void)drawAtPoint:(NSPoint)point fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta
{
	NSImageInterpolation interp = NSImageInterpolationDefault;
	NSGraphicsContext	*ctx = NULL;

	ctx = [NSGraphicsContext currentContext];
	NSAssert(ctx != nil, @"SCImage DrawAtPoint Failed retrieving valid GraphicsContext");

	if(_interpolation != NSImageInterpolationDefault) {
		interp = [ctx imageInterpolation];
		[ctx setImageInterpolation:_interpolation];
	}

	#if SCIMAGE_DEBUG
		double past = GetTimeOfDay();
	#endif

	if(_accelerated || _hints == SCImageUseGraphicsAccelerationWhenPossible) // Keep CIImage up-to-date
	{
		[self syncCIImage]; // update if needed

		if(delta < 1.0f || op != NSCompositeCopy)
			[[self ciimage]drawAtPoint:point fromRect:srcRect operation:op fraction:delta];
		else // true CIContext draw
			[[ctx CIContext]drawImage:[self ciimage] atPoint:*(CGPoint*)&point fromRect:*(CGRect*)&srcRect];

		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) CIImage drawAtPoint Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif
	}
	else
	{
		[self syncBitmap]; // update if needed

		[[self nsimage]drawAtPoint:point fromRect:srcRect operation:op fraction:delta];
		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) NSImage drawAtPoint Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif
	}

	[ctx setImageInterpolation:interp];
}

- (void)drawInRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta
{
	NSGraphicsContext	*ctx = [NSGraphicsContext currentContext];
	NSAssert(ctx != nil, @"SCImage DrawAtPoint Failed retrieving valid GraphicsContext");

	#if SCIMAGE_DEBUG
		double past = GetTimeOfDay();
	#endif

	if(_accelerated)
	{
		[self syncCIImage]; // update if needed

		if(delta < 1.0f || op != NSCompositeSourceOver)
			[[self ciimage]drawInRect:dstRect fromRect:srcRect operation:op fraction:delta];
		else
			[[ctx CIContext]drawImage:[self ciimage] inRect:*(CGRect*)&dstRect fromRect:*(CGRect*)&srcRect];

		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) CIImage drawInRect Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif
	}
	else
	{
		[self syncBitmap];

		NSImageInterpolation interp = NSImageInterpolationDefault;
		if(_interpolation != NSImageInterpolationDefault) {
			interp = [ctx imageInterpolation];
			[ctx setImageInterpolation:_interpolation];
		}
		[[self nsimage]drawInRect:dstRect fromRect:srcRect operation:op fraction:delta];

		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) NSImage drawInRect Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif

		[ctx setImageInterpolation:interp];
	}
}


- (void)drawInSCRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta
{
	NSGraphicsContext	*ctx = [NSGraphicsContext currentContext];
	NSAssert(ctx != nil, @"SCImage DrawAtPoint Failed retrieving valid GraphicsContext");

	#if SCIMAGE_DEBUG
		double past = GetTimeOfDay();
	#endif

	srcRect.origin.y = [self size].height - srcRect.origin.y - srcRect.size.height;

	if(_accelerated)
	{
		[self syncCIImage]; // update if needed

		CIImage* timage;
		NSAffineTransform *transform = [NSAffineTransform transform];
		[transform translateXBy:0 yBy:[self extent].size.height];
		[transform scaleXBy:1.0 yBy:-1.0];

		CIFilter* fmat = [CIFilter filterWithName:@"CIAffineTransform"];
		if(!fmat) {
			NSLog(@"SCImageBackground Failed getting valid CIfilter !");
			return;
		}

		[fmat setDefaults];
		[fmat setValue: [self ciimage] forKey: @"inputImage"];
		[fmat setValue: transform forKey: @"inputTransform"];
		timage = [fmat valueForKey:@"outputImage"];
		if(op != NSCompositeSourceOver && op != NSCompositeCopy)
		{
			[timage drawInRect:dstRect fromRect:srcRect operation:op fraction:delta];
		}
		else
		{
			if(delta < 1.f) {
				CIFilter* fcol = [CIFilter filterWithName:@"CIColorMatrix"];
				[fcol setDefaults];
				[fcol setValue: timage forKey:@"inputImage"];
				[fcol setValue: [CIVector vectorWithX:0.0 Y:0.0 Z:0.0 W:delta] forKey:@"inputAVector"];
				timage = [fcol valueForKey:@"outputImage"];
			}

			[[ctx CIContext]drawImage:timage inRect:*(CGRect*)&dstRect fromRect:*(CGRect*)&srcRect];
		}

		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) CIImage drawInRect Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif
	}
	else
	{
		CGContextRef cgctx = (CGContextRef)[[NSGraphicsContext currentContext]graphicsPort];
		CGContextSaveGState(cgctx);

		NSImageInterpolation interp = NSImageInterpolationDefault;
		if(_interpolation != NSImageInterpolationDefault) {
			interp = [ctx imageInterpolation];
			[ctx setImageInterpolation:_interpolation];
		}

		[self syncBitmap];

		CGContextTranslateCTM(cgctx, dstRect.origin.x, dstRect.origin.y + dstRect.size.height);
		CGContextScaleCTM(cgctx, 1, -1.0f);
		dstRect.origin.y = dstRect.origin.x = 0.f;
		[[self nsimage]drawInRect:dstRect fromRect:srcRect operation:op fraction:delta];

		#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) NSImage drawInRect Called Time To Render: %f", self, GetTimeOfDay() - past);
		#endif

		[ctx setImageInterpolation:interp]; // needed before ?? to test
		CGContextRestoreGState(cgctx);
	}
}

- (void)tileInSCRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta
{

	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext]graphicsPort];
	if(!cgc)
		return;

	CGContextSaveGState(cgc);
	CGContextClipToRect(cgc, *(CGRect*)&dstRect);

	int numCols = (int)ceilf(sc_max(1, (dstRect.size.width / srcRect.size.width) + 0.5));
	int numRows = (int)ceilf(sc_max(1, (dstRect.size.height / srcRect.size.height) + 0.5));

	int y,x;
	srcRect.origin.y = [self size].height - srcRect.origin.y - srcRect.size.height;
	CGPoint where = CGPointMake(dstRect.origin.x, dstRect.origin.y);
	CGLayerRef cgLayer;
	if(_accelerated)
	{
		[self syncCIImage];
		cgLayer = CGLayerCreateWithContext(cgc, *(CGSize*)&srcRect.size, NULL);
		CGContextRef lcg = CGLayerGetContext(cgLayer);
		CGContextTranslateCTM(lcg, 0.f, srcRect.size.height);
		CGContextScaleCTM(lcg, 1, -1.0f);
		CIContext *cictx = [[CIContext contextWithCGContext:lcg options:nil]retain];
		[cictx drawImage:[self ciimage] inRect:CGRectMake(0.f, 0.f, srcRect.size.width, srcRect.size.height) fromRect:*(CGRect*)&srcRect];
		[cictx release];
	}
	else
	{
		[self syncBitmap];
		cgLayer = CGLayerCreateWithContext(cgc, *(CGSize*)&srcRect.size, NULL);
		CGContextRef lcg = CGLayerGetContext(cgLayer);
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithGraphicsPort:lcg flipped:NO]];
			CGContextTranslateCTM(lcg, 0.f, srcRect.size.height);
			CGContextScaleCTM(lcg, 1, -1.0f);
			[[self nsimage]drawInRect:NSMakeRect(0.f, 0.f, srcRect.size.width, srcRect.size.height) fromRect:*(NSRect*)&srcRect operation:NSCompositeCopy fraction:delta];
		[NSGraphicsContext restoreGraphicsState];
	}
	_cache = (void*)cgLayer;

	#if SCIMAGE_DEBUG
		double past = GetTimeOfDay();
	#endif

	if(cgLayer)
	{
		if(fraction < 1.0)
			CGContextSetAlpha(cgc, delta);

		for(y=0; y < numRows; ++y)
		{
			where.x = dstRect.origin.x;
			for(x=0; x < numCols; ++x)
			{
				CGContextDrawLayerAtPoint(cgc, where, cgLayer);
				where.x += srcRect.size.width;
			}
			where.y += srcRect.size.height;
		}
	}

	#if SCIMAGE_DEBUG
			NSLog(@"SCImage (%p) drawInRect Called Time To Render: %f", self, GetTimeOfDay() - past);
	#endif
	CGLayerRelease(cgLayer);
	CGContextRestoreGState(cgc);
}

- (void)applyFilter:(CIFilter*)filter
{
	if(![self isAccelerated])
		[self syncCIImage]; // update ciimage rep

	if([[filter inputKeys] containsObject:@"inputImage"]) {
		[filter setValue:[self ciimage] forKey:@"inputImage"];
	}

	CIImage *result = [filter valueForKey:@"outputImage"];
	if(!result) {
		NSLog(@"Failed Appying filter to image !");
		return;
	}

#if SCIMAGE_MAC_OS_10_5
	[_ciimageStore setImage:result];
#else
	[result retain];
	[_ciimage release];
	_ciimage = result;
#endif

	[self setShouldSyncBitmap:YES]; // invalidates bitmap
}

-(SCImage*)imageFilteredWith:(CIFilter*)filter
{
	if(![self isAccelerated])
		[self syncCIImage];

	[filter setValue:[self ciimage] forKey:@"inputImage"];
	CIImage *result = [filter valueForKey:@"outputImage"];

	if(!result) {
		NSLog(@"Failed Appying filter to image !");
		return nil;
	}

	#if SCIMAGE_MAC_OS_10_5
	SCImage *newSCImage = [[SCImage alloc]initWithCIImage:result extent:[_ciimageStore extent] format:_ciFormat];
	#else
	SCImage *newSCImage = [[SCImage alloc]initWithCIImage:result extent:[_ciimage extent] format:_ciFormat];
	#endif

	return [newSCImage autorelease];
}

-(CIFormat)format
{
	return _ciFormat;
}

#if SCIMAGE_MAC_OS_10_5
-(CIImageAccumulator*)accumulator {
	return _ciimageStore;
}
#endif

-(void)dealloc {

#if SCIMAGE_DEBUG
	NSLog(@"SCImage (%p) Dealloc", self);
#endif

	[_wLock release];
#if SCIMAGE_MAC_OS_10_5
	if(_ciimageStore) [_ciimageStore release];
#else
	if(_ciimage) [_ciimage release];
#endif
	if(_bitmap) [_bitmap release];
	if(_nsimage) [_nsimage release];
	[super dealloc];
}
@end

#pragma mark -- SCImage Primitive Set
//----------------------------------------------------------------------------------------------------
// SCImage primitive set

int prSCImage_New(struct VMGlobals *g, int numArgsPushed);
int prSCImage_New(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 3;	// instance of SCImage
	PyrSlot *a = g->sp - 2;		// width
	PyrSlot *b = g->sp - 1;		// height
	PyrSlot *c = g->sp;		// graphics acceleration

	float width, height;
 	int err = slotFloatVal(a, &width);
	if (err) return err;
 	err = slotFloatVal(b, &height);
	if (err) return err;


	SCImage* image = [[SCImage alloc]initWithSize: NSMakeSize(width, height) isAccelerated:IsTrue(c) format:kCIFormatARGB8];
	if(!image){
		NSLog(@"prSCImage_New Failed Creating valid SCImage !");
		return errFailed;
	}

	SetPtr(slotRawObject(receiver)->slots + 0, image);
	SetFloat(slotRawObject(receiver)->slots + 1, width);
	SetFloat(slotRawObject(receiver)->slots + 2, height);
	InstallFinalizer(g, slotRawObject(receiver), 9, FinalizeSCImageObject);

	return errNone;
}

int prSCImage_NewFromURL(struct VMGlobals *g, int numArgsPushed);
int prSCImage_NewFromURL(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 1;	// instance of SCImage
	PyrSlot *a = g->sp;				// URL

	int err;
	char *urlpath=0;
	err = allocSlotStrVal(a, &urlpath);
	if (err) return err;

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
	NSString *ns_urlpath = [[[NSString alloc] initWithCString:urlpath length:strlen(urlpath)]autorelease]; // Deprecated >= 10.4
#else
	NSString *ns_urlpath = [NSString stringWithCString:urlpath encoding:NSASCIIStringEncoding];
#endif
	if(!ns_urlpath) {
		NSLog(@"Failed Creating valid NSURL from string: %s", urlpath);
		return errFailed;
	}
	delete [] urlpath;

	SCImage* image = [[SCImage alloc]initWithContentsOfURL:[NSURL URLWithString:ns_urlpath] isAccelerated:NO format:kCIFormatARGB8]; // by default for now set to NO
	if(!image) {
		NSLog(@"prSCImage_NewFromURL Failed Creating valid SCImage !");
		return errFailed;
	}

	SetPtr(slotRawObject(receiver)->slots + 0, image);
	SetFloat(slotRawObject(receiver)->slots + 1, (float)[image width]); // using size.width / size.height can return non integral number (ex: 499.996 instead of 499)
	SetFloat(slotRawObject(receiver)->slots + 2, (float)[image height]); // better to stick with bitmap properties to have true pixel size
	InstallFinalizer(g, slotRawObject(receiver), 9, FinalizeSCImageObject);

	return errNone;
}

int prSCImage_WriteToFile(struct VMGlobals *g, int numArgsPushed);
int prSCImage_WriteToFile(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 2;	// instance of SCImage
	PyrSlot *a = g->sp - 1;			// path
	PyrSlot *b = g->sp;				// image_type

	int err;
	char *path=0;
	err = allocSlotStrVal(a, &path);
	if (err) return err;

	int image_type;
 	err = slotIntVal(b, &image_type);
	if (err) return err;

	NSString *StringPath = [[NSString alloc] initWithCString:path length:strlen(path)];
	delete [] path;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);

	if([image isAccelerated]) {
		[image syncBitmap]; // force sync of the bitmap
	}
	NSData *data = [[image bitmapRepresentation] representationUsingType:(NSBitmapImageFileType)image_type properties:nil];
	BOOL result = [data writeToFile:StringPath atomically:YES];

	[StringPath release];

	SetBool(receiver, result ? true : false); // inform the user if save is ok

	return errNone;
}

int prSCImage_interpolation(struct VMGlobals *g, int numArgsPushed);
int prSCImage_interpolation(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage
	SCImage *image = (SCImage *)slotRawPtr(&slotRawObject(receiver)->slots[0]);
	if(!image)
		return errNone;

	SetInt(receiver, [image imageInterpolation]); //
	return errNone;
}

int prSCImage_setInterpolation(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setInterpolation(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver	= g->sp - 1;		// SCImage
	PyrSlot *a			= g->sp;		// interpolation

	int interp = NSImageInterpolationDefault;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone;

	if(slotIntVal(a, &interp) != errNone)
		return errFailed;

	[image setImageInterpolation:(NSImageInterpolation)interp];

	return errNone;
}

int prSCImage_Free(struct VMGlobals *g, int numArgsPushed);
int prSCImage_Free(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone;

	#if SCIMAGE_DEBUG
		NSLog(@"Releasing SCImage (%p)", image);
	#endif
	[image release];

	SetNil(slotRawObject(receiver)->slots + 0); // ensure slot is NIL
	SetNil(slotRawObject(receiver)->slots + 1); // not only the pointer
	SetNil(slotRawObject(receiver)->slots + 2);

	return errNone;
}

int prSCImage_sync(struct VMGlobals *g, int numArgsPushed);
int prSCImage_sync(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;	// instance of SCImage
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	[image syncRepresentations]; // force sync

#if SCIMAGE_DEBUG
	NSLog(@"SCImage (%p) Force Sync", image);
#endif
	return errNone;
}

int prSCImage_setSize(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setSize(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 2;	// instance of SCImage
	PyrSlot *a = g->sp - 1;			// width
	PyrSlot *b = g->sp;				// height

	int err;
	float width;
	err = slotFloatVal(a, &width);
	if (err) return err;

	float height;
	err = slotFloatVal(b, &height);
	if (err) return err;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	[image setSize: NSMakeSize(width, height)];

	SetFloat(slotRawObject(receiver)->slots + 1, width);
	SetFloat(slotRawObject(receiver)->slots + 2, height);

	return errNone;
}

int prSCImage_setScalesWhenResized(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setScalesWhenResized(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 1;	// instance of SCImage
	PyrSlot *a = g->sp;				// bool flag

	BOOL flag = IsTrue(a);
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	[image setScalesWhenResized: flag];

	return errNone;
}

int prSCImage_scalesWhenResized(struct VMGlobals *g, int numArgsPushed);
int prSCImage_scalesWhenResized(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	SetBool(receiver, [image scalesWhenResized]);
	return errNone;
}

int prSCImage_DrawAtPoint(struct VMGlobals *g, int numArgsPushed);
int prSCImage_DrawAtPoint(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *d = g->sp;					// fraction
	PyrSlot *c = g->sp - 1;				// compositing operation
	PyrSlot *b = g->sp - 2;				// fromRect
	PyrSlot *a = g->sp - 3;				// point
	PyrSlot *receiver = g->sp - 4;		// instance of SCImage

	int err;
    NSPoint nsPoint;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone; // just do nothing

	if(![image ciimage])
	{
		NSLog(@"SCImage CIImage is nil !");
		return errFailed;
	}

    err = slotGetPoint(a, &nsPoint);
    if (err) return err;

	NSRect rect;
	if (IsNil(b)) {
		rect = NSMakeRect(0.0,0.0,[image size].width,[image size].height);
	} else {
		err = slotGetNSRect(b, &rect);
		if (err) return err;
	}

	int co;
	err = slotIntVal(c, &co);
	if (err) return err;

	float fraction;
	err = slotFloatVal(d, &fraction);
	if (err) return err;

//	[image drawInSCRect:NSMakeRect(nsPoint.x, nsPoint.y, [image size].width, [image size].height) fromRect:rect operation:(NSCompositingOperation)co fraction:fraction];
	[image drawInSCRect:NSMakeRect(nsPoint.x, nsPoint.y, rect.size.width, rect.size.height) fromRect:rect operation:(NSCompositingOperation)co fraction:fraction];

	return errNone;
}

int prSCImage_DrawInRect(struct VMGlobals *g, int numArgsPushed);
int prSCImage_DrawInRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *d = g->sp;					// fraction
	PyrSlot *c = g->sp - 1;				// compositing operation
	PyrSlot *b = g->sp - 2;				// fromRect
	PyrSlot *a = g->sp - 3;				// rect
	PyrSlot *receiver = g->sp - 4;		// instance of SCImage

	int err;
	NSRect rect, fromRect;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image) {
		return errNone;
	}

	err = slotGetNSRect(a, &rect);
	if (err) return err;

	if (IsNil(b)) {
		fromRect = NSMakeRect(0.0,0.0,[image size].width,[image size].height); // CIImage does not support NSZeroRect as argument
	} else {
		err = slotGetNSRect(b, &fromRect);
		if (err) return err;
	}

	int co;
	err = slotIntVal(c, &co);
	if (err) return err;

	float fraction;
	err = slotFloatVal(d, &fraction);
	if (err) return err;

	[image drawInSCRect:rect fromRect:fromRect operation:(NSCompositingOperation)co fraction:fraction];

	return errNone;
}

int prSCImage_TileInRect(struct VMGlobals *g, int numArgsPushed);
int prSCImage_TileInRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *d = g->sp;					// fraction
	PyrSlot *c = g->sp - 1;				// compositing operation
	PyrSlot *b = g->sp - 2;				// fromRect
	PyrSlot *a = g->sp - 3;				// rect
	PyrSlot *receiver = g->sp - 4;		// instance of SCImage

	NSRect nsFromRect, nsRect;
	int err, co;
	float fraction;
	SCImage *image;

	if(IsNil(slotRawObject(receiver)->slots+0))
		return errNone;

	image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image) {
		return errNone;
	}

	err = slotGetNSRect(a, &nsRect);
	if (err) return err;

	err = slotGetNSRect(b, &nsFromRect);
	if (err) return err;

	err = slotFloatVal(d, &fraction);
	if (err) return err;

	err = slotIntVal(c, &co);
	if (err) return err;

	[image tileInSCRect:nsRect fromRect:nsFromRect operation:(NSCompositingOperation)co fraction:fraction];

	return errNone;
}
int prSCImage_lockFocus(struct VMGlobals *g, int numArgsPushed);
int prSCImage_lockFocus(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	// here we do not use the set flipped because it will create problem
	// if we draw on top of an already image loaded with URL and write it after
	// better to S(T(ctx)) directly so everything is fine now
	// and we can write it with no problem
	[image lockFocus];
	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext]graphicsPort];
	CGContextSaveGState(ctx);
		CGContextTranslateCTM(ctx, 0, [image size].height);
		CGContextScaleCTM(ctx, 1, -1.0f);

	return errNone;
}

int prSCImage_unlockFocus(struct VMGlobals *g, int numArgsPushed);
int prSCImage_unlockFocus(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	// revert the coordinate system
	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext]graphicsPort];
	CGContextRestoreGState(ctx);

	[image unlockFocus];

	return errNone;
}

int prSCImage_recache(struct VMGlobals *g, int numArgsPushed);
int prSCImage_recache(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp;		// instance of SCImage

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	[image recache];
	return errNone;
}

// returns a premultipled alpha pixel as an uint32
int prSCImage_pixelAt(struct VMGlobals *g, int numArgsPushed);
int prSCImage_pixelAt(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 2;		// instance of SCImage
	PyrSlot *xSlot = g->sp - 1;
	PyrSlot *ySlot = g->sp;

	int x, y, err;
	SCImage *image;
	PixelData pixelAsArray[4] = {0, 0, 0, 0};

	err = errNone;

	image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image) return errNone;

	err = slotIntVal(xSlot, &x);
	if(err) return err;

	err = slotIntVal(ySlot, &y);
	if(err) return err;

	[image setAccelerated:NO];
	[image getPixel:pixelAsArray atX:x y:y];
	//SetInt(receiver, ((pixelAsArray[0] << 24) | (pixelAsArray[1] << 16) | (pixelAsArray[2] << 8) | pixelAsArray[3]));
	if( pixelAsArray[3] == 0 ) { // avoid EXC_ARITHMETIC
		SetInt(receiver,
			(pixelAsArray[0] << 24) |
			(pixelAsArray[1] << 16) |
			(pixelAsArray[2] << 8)
		);
	}else{
		SetInt(receiver,
			   (COLOR_UNSCALE( pixelAsArray[0], pixelAsArray[3] ) << 24) |
			   (COLOR_UNSCALE( pixelAsArray[1], pixelAsArray[3] ) << 16) |
			   (COLOR_UNSCALE( pixelAsArray[2], pixelAsArray[3] ) <<  8) |
			   pixelAsArray[3]
			   );
	}
	return errNone;
}

int prSCImage_setPixelAt(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setPixelAt(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 3;		// instance of SCImage
	PyrSlot *pixelSlot = g->sp - 2;
	PyrSlot *xSlot = g->sp - 1;
	PyrSlot *ySlot = g->sp;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone;

	int x, y, err;
	uint32 pixel;
	PixelData pixelData[4]={0, 0, 0, 0};

	err = slotIntVal(pixelSlot, (int*)&pixel);
	if(err) return err;

	err = slotIntVal(xSlot, &x);
	if(err) return err;

	err = slotIntVal(ySlot, &y);
	if(err) return err;

	[image setAccelerated:NO];

	pixelData[0] = COLOR_SCALE(PIXEL_RED(pixel), PIXEL_ALPHA(pixel));
	pixelData[1] = COLOR_SCALE(PIXEL_GREEN(pixel), PIXEL_ALPHA(pixel));
	pixelData[2] = COLOR_SCALE(PIXEL_BLUE(pixel), PIXEL_ALPHA(pixel));
	pixelData[3] = PIXEL_ALPHA(pixel);

	[image setPixel:pixelData atX:x y:y];

	return errNone;
}

int prSCImage_setColorAt(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setColorAt(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 3;		// instance of SCImage
	PyrSlot *colorSlot = g->sp - 2;
	PyrSlot *xSlot = g->sp - 1;
	PyrSlot *ySlot = g->sp;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone;

	[image setAccelerated:NO];

	int x, y, err;
	SCColor	color;
	NSSize size = [image size];

	err = slotColorVal(colorSlot, &color);
	if(err) return err;

	err = slotIntVal(xSlot, &x);
	if(err) return err;

	err = slotIntVal(ySlot, &y);
	if(err) return err;

	[image
		setColor:[NSColor colorWithCalibratedRed:color.red green:color.green blue:color.blue alpha:color.alpha]
		atX:x
		y:y
	];

	return errNone;
}

int prSCImage_getColorAt(struct VMGlobals *g, int numArgsPushed);
int prSCImage_getColorAt(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver = g->sp - 2;		// instance of SCImage
	PyrSlot *xSlot = g->sp - 1;
	PyrSlot *ySlot = g->sp;

	NSColor *nsColor;
	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image)
		return errNone;

	int x, y, err;

	err = slotIntVal(xSlot, &x);
	if(err) return err;

	err = slotIntVal(ySlot, &y);
	if(err) return err;

	[image setAccelerated:NO];

	nsColor = [image
		colorAtX:x
		y:y
	];

	if(!nsColor)
		SetNil(receiver);
	else {
		nsColor = [nsColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
		PyrObject *color = instantiateObject(g->gc, s_color->u.classobj, 0, false, true);
		PyrSlot *slots = color->slots;
		SetFloat(slots+0, [nsColor redComponent]);
		SetFloat(slots+1, [nsColor greenComponent]);
		SetFloat(slots+2, [nsColor blueComponent]);
		SetFloat(slots+3, [nsColor alphaComponent]);
		SetObject(receiver, color);
	}

	return errNone;
}

int prSCImage_loadPixels(struct VMGlobals *g, int numArgsPushed);
int prSCImage_loadPixels(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot  *receiver	= g->sp - 3;
	PyrSlot  *pixelSlot = g->sp - 2;
	PyrSlot	 *regionSlot= g->sp - 1;
	PyrClass *pyrclass;

	uint32				*pixelsData;
	NSBitmapImageRep	*bitmap;
	SCImage				*image;
	int					height, width, y=0, x=0, startIndex=0;
	NSSize				size;
	NSRect				region;

	image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image){
		SetNil(receiver);
		return errNone;
	}
	size = [image size];

	pyrclass = slotRawObject(pixelSlot)->classptr;
	if(pyrclass != class_int32array) {
		NSLog(@"prSCImage_loadPixels array argument is not a PyrInt32Array !");
		return errFailed;
	}

	slotIntVal(g->sp, &startIndex);
	if(startIndex < 0)
		return errIndexOutOfRange;

	PyrInt32Array* allocatedPixelArray = (PyrInt32Array*)slotRawObject(pixelSlot);

	bitmap = [image bitmapRepresentation];

	if(IsNil(regionSlot)) {
		region = NSIntegralRect(NSMakeRect(0.f, 0.f, [bitmap pixelsWide], [bitmap pixelsHigh]));
	}else{
		if(slotGetNSRect(regionSlot, &region) != errNone)
			return errWrongType;
		region = NSIntegralRect(region);
		if( region.origin.x < 0 || region.origin.y < 0 || ((region.origin.x + region.size.width) > [bitmap pixelsWide]) || ((region.origin.y + region.size.height) > [bitmap pixelsHigh]) )
			return errIndexOutOfRange;
	}

	height = region.size.height;
	width = region.size.width;
	pixelsData = (uint32*)allocatedPixelArray->i + startIndex;
	PixelData pixel[4]={0, 0, 0, 0};

	if((allocatedPixelArray->size - startIndex) < ( height * width /** 4*/ )  ) {
		post("Error: pixel array is not of good size ! should be %i bytes long\n", height * width /** 4*/);
		return errIndexOutOfRange;
	}

	[image lock];
	unsigned char alpha;
	height = height + region.origin.y; //padding h and w to meet origin
	width = width + region.origin.x;
	for(y=(int)region.origin.y; y < height; y++) {
		for(x=(int)region.origin.x; x < width; x++) {

			// most of the unscale routines may be not as precise as we need
			// especially when dealing with CIImage <-> NSImage conversion...
			// it should be better to use vImage framework directly !

			[bitmap getPixel:pixel atX:x y:y];
			if(alpha > 0) {
				SET_PIXEL_RED  ( *pixelsData, COLOR_UNSCALE(pixel[0], pixel[3]) );
				SET_PIXEL_GREEN( *pixelsData, COLOR_UNSCALE(pixel[1], pixel[3]) );
				SET_PIXEL_BLUE ( *pixelsData, COLOR_UNSCALE(pixel[2], pixel[3]) );
				SET_PIXEL_ALPHA( *pixelsData, pixel[3]);
			}else{
				SET_PIXEL_RED  ( *pixelsData, pixel[0] );
				SET_PIXEL_GREEN( *pixelsData, pixel[1] );
				SET_PIXEL_BLUE ( *pixelsData, pixel[2] );
				SET_PIXEL_ALPHA( *pixelsData, pixel[3] );
			}
			++pixelsData;
		}
		//bitmapData += bytesLeft;
	}
	[image unlock];

	return errNone;
}

int prSCImage_updatePixels(struct VMGlobals *g, int numArgsPushed);
int prSCImage_updatePixels(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver	= g->sp - 2;
	PyrSlot *pixelSlot = g->sp - 1;
	SCImage *image;

	NSBitmapImageRep *bitmap;
	int width, height, bytesPerRow, bytesLeft, startIndex=0, y=0, x=0;
	unsigned char *bitmapData;
	uint32 *pixelsData;
	PyrInt32Array *pixels;
	PyrClass *pyrclass;

	if(IsNil(pixelSlot))
		return errNone;

	slotIntVal(g->sp, &startIndex);
	if(startIndex < 0)
		return errIndexOutOfRange;

	image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image){
		SetNil(receiver);
		return errNone;
	}

	pyrclass = slotRawObject(pixelSlot)->classptr;
	if(pyrclass != class_int32array) {
		NSLog(@"prSCImage_loadPixels array argument is not a PyrInt32Array !");
		return errFailed;
	}

	pixels = (PyrInt32Array*)slotRawObject(pixelSlot);

	bitmap = [image bitmapRepresentation];
	if(!bitmap) {
		NSLog(@"prSCImage_updatePixels failed retrieving valid NSBitmapImageRep...");
		return errFailed;
	}

	width		= [bitmap pixelsWide]; // RGBA
	height		= [bitmap pixelsHigh];
	bytesPerRow = [bitmap bytesPerRow];
	bitmapData	= [bitmap bitmapData];
	pixelsData	= (uint32*)pixels->i + startIndex;
	bytesLeft	= bytesPerRow - (width*4);

	if( (pixels->size - startIndex) < (int)(width*height /* *4 */)) {
		post("Error: pixel array is not of good size ! should be %i bytes long from index: %i\n", (int)(width*height/**4*/), startIndex);
		return errIndexOutOfRange;
	}

	[image lock];
	unsigned char alpha;
	for(y=0; y < height; ++y) {
		for(x=0; x < width; ++x) {
			alpha = PIXEL_ALPHA(*pixelsData);
			*bitmapData++ = COLOR_SCALE(PIXEL_RED(*pixelsData), alpha);
			*bitmapData++ = COLOR_SCALE(PIXEL_GREEN(*pixelsData), alpha);
			*bitmapData++ = COLOR_SCALE(PIXEL_BLUE(*pixelsData), alpha);
			*bitmapData++ = alpha;
			++pixelsData;
		}
		bitmapData += bytesLeft;
	}
	[image unlock];

	[image setShouldSyncCIImage:YES];

	return errNone;
}


int prSCImage_updatePixelsInRect(struct VMGlobals *g, int numArgsPushed);
int prSCImage_updatePixelsInRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *receiver	= g->sp - 3;
	PyrSlot *pixelSlot  = g->sp - 2;
	PyrSlot *rectSlot   = g->sp - 1;
	SCImage *image;

	NSBitmapImageRep *bitmap;
	int width, height, bytesPerRow, bytesLeft, y=0, x=0, startIndex=0, err;
	unsigned char *bitmapData;
	uint32 *pixelsData;
	PyrInt32Array *pixels;
	PyrClass *pyrclass;
	NSRect rect;

	if(IsNil(pixelSlot))
		return errNone;

	slotIntVal(g->sp, &startIndex); // get the Array start index
	if(startIndex < 0)
		return errIndexOutOfRange;

	image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image){
		SetNil(receiver);
		return errNone;
	}

	err = slotGetNSRect(rectSlot, &rect);
	if (err) return err;
	rect = NSIntegralRect(rect);

	pyrclass = slotRawObject(pixelSlot)->classptr;
	if(pyrclass != class_int32array) {
		NSLog(@"prSCImage_loadPixels array argument is not a PyrInt32Array !");
		return errFailed;
	}

	pixels = (PyrInt32Array*)slotRawObject(pixelSlot);
	bitmap = [image bitmapRepresentation];
	if(!bitmap) {
		NSLog(@"prSCImage_updatePixels failed retrieving valid NSBitmapImageRep...");
		return errIndexOutOfRange;
	}

	width		= [bitmap pixelsWide]; // RGBA
	height		= [bitmap pixelsHigh];
	bytesPerRow = [bitmap bytesPerRow];
	bitmapData	= [bitmap bitmapData];
	pixelsData	= (uint32*)pixels->i + startIndex;

	// clipping
	rect.origin.x = sc_max(rect.origin.x, 0); // negative idx not allowed
	rect.origin.y = sc_max(rect.origin.y, 0); // negative idx not allowed
	rect.size.width = sc_min(rect.size.width, width); // clip to image width max
	rect.size.height = sc_min(rect.size.height, height); // clip to image height max

	// what is the best :
	//		1 - clipping + wrapping thus giving strange results  ?
	//		2 - throw an error ?
	// just throw an error for now -> so no strange results and no scratching head... :)
	// zero tolerance >:^(
	if(
		((rect.origin.x + rect.size.width) > width) ||
		((rect.origin.y + rect.size.height) > height)
	)
		return errIndexOutOfRange;

	bytesLeft	= (bytesPerRow - (rect.size.width * 4));
	if( (pixels->size - startIndex) < (int)(rect.size.width * rect.size.height)) {
		post("Error: pixel array is not of good size ! should be %i bytes long from index: %i\n", (int)(rect.size.width * rect.size.height), startIndex);
		return errNone;
	}

	[image lock];
	unsigned char alpha;
	bitmapData += ((int)rect.origin.y * bytesPerRow) + ((int)rect.origin.x * 4);
	for(y=0; y < rect.size.height; ++y) {
		for(x=0; x < rect.size.width; ++x) {
			alpha = PIXEL_ALPHA(*pixelsData);
			*bitmapData++ = COLOR_SCALE(PIXEL_RED(*pixelsData), alpha);
			*bitmapData++ = COLOR_SCALE(PIXEL_GREEN(*pixelsData), alpha);
			*bitmapData++ = COLOR_SCALE(PIXEL_BLUE(*pixelsData), alpha);
			*bitmapData++ = alpha;
			++pixelsData;
		}
		bitmapData += bytesLeft;
	}
	[image unlock];

	[image setShouldSyncCIImage:YES];

	return errNone;
}

int prSCImage_isAccelerated(struct VMGlobals *g, int numArgsPushed);
int prSCImage_isAccelerated(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot  *receiver	= g->sp;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image){
		SetNil(receiver);
		return errNone;
	}

	SetBool(receiver, [image isAccelerated]);

	return errNone;
}

int prSCImage_setAccelerated(struct VMGlobals *g, int numArgsPushed);
int prSCImage_setAccelerated(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot  *receiver	= g->sp - 1;
	PyrSlot  *yornSlot	= g->sp;

	SCImage *image = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!image){
		SetNil(receiver);
		return errNone;
	}

	[image setAccelerated:IsTrue(yornSlot)];

	return errNone;
}


@class SCGraphView;
// Currently it grabs a portion from the scgraphview
// user should here pass the ABSOLUTE view bounds or a specific rect
int prSCImage_imageFromSCWindowRect(struct VMGlobals *g, int numArgsPushed);
int prSCImage_imageFromSCWindowRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot  *receiver	= g->sp - 2;	// SCImage class
	PyrSlot  *a			= g->sp - 1;	// SCWindow instance
	PyrSlot  *b			= g->sp;		// SCRect instance - portion in the SCGraphView

	NSRect				rect;
	NSBitmapImageRep	*nsbitmap	= nil;
	NSImage				*nsimage	= nil;
	SCGraphView			*nsview		= nil;
	SCImage				*scimage	= nil;
	int					err			= errNone;

	if(!IsPtr(&slotRawObject(a)->slots[0])) {
		return errFailed;
	}

	nsview	= (SCGraphView*)slotRawPtr(&slotRawObject(a)->slots[0]); // get SCGraphView instance
	err		= slotGetNSRect(b, &rect);
	if (err)
		return err;

	[nsview lockFocus];
	nsbitmap = [[NSBitmapImageRep alloc]initWithFocusedViewRect:rect];
	[nsview unlockFocus];

	if(!nsbitmap)
	{
		post("Error: Failed getting valid image from SCView !\n");
	}
	else
	{
		PyrObject* object = NULL;

		nsimage =
		[[NSImage alloc]initWithSize:NSMakeSize((float)[nsbitmap pixelsWide], (float)[nsbitmap pixelsHigh])];

		if(!nsimage) {
			post("Error: Failed creating valid NSImage !\n");
			SetNil(receiver);
			goto Bail;
		}

		[nsimage addRepresentation:nsbitmap];

		scimage = [[SCImage alloc]initWithNSImage:nsimage];
		if(!scimage) {
			post("Error: Failed creating valid SCImage !\n");
			SetNil(receiver);
			return errNone;
		}

		// Instantiate and set the SCObject
		object = newPyrSCImage(g);
		if(!object) {
			post("Error: Failed creating valid SCImage class !");
			[scimage release];
			SetNil(receiver);
			goto Bail;
		}

		SetPtr(object->slots + 0, scimage);
		SetFloat(object->slots + 1, (float)[nsbitmap pixelsWide]);
		SetFloat(object->slots + 2, (float)[nsbitmap pixelsHigh]);
		SetObject(receiver, object);
	}

	Bail:
		if(nsimage) [nsimage release];
		if(nsbitmap) [nsbitmap release];

	return errNone;
}

#pragma mark --- SCImageFilter Primitive Set

PyrClass* slotArrayGetCommonClass(PyrObject* array)
{
	int size = array->size;
	if(size == 0)
		return NULL;

	PyrClass *commonClass = slotRawObject(&array->slots[0])->classptr;

	for(int i=1; i < size; ++i) {
		if(slotRawObject(&array->slots[0])->classptr != commonClass)
			return NULL;
	}

	return commonClass;
}

BOOL nsClassIsKindOfClass(Class currentClass, Class match);
BOOL nsClassIsKindOfClass(Class currentClass, Class match)
{
	if(currentClass == match)
		return YES;

	Class superClass;
	while( (superClass = class_getSuperclass(currentClass)) ) {
		if(superClass == match)
			return YES;
	}

	return NO;
}

id		ciVectorFromSlot(PyrObject* slot);
id		ciVectorFromSlot(PyrObject* slot)
{
	ColorData values[4] = {0.f, 0.f, 0.f, 0.f};
	int size = slot->size, err;

	for (int i=0; i < size; ++i) {
		float val;
		err = slotFloatVal(slot->slots + i, &val);
		if(err) return nil;
		values[i] = val;
	}

	return [CIVector vectorWithValues:values count:(size_t)4];
}

int		safeSlotFloatVal(PyrSlot* slot, float* value);
int		safeSlotFloatVal(PyrSlot* slot, float* value)
{
	if(IsNil(slot)) {
		*value = 0.f;
		return errNone;
	}
	return slotFloatVal(slot, value);
}

CGRect OutsetRect(CGRect r, const float dx, const float dy)
{
	CGRect ret = CGRectMake(r.origin.x - dx, r.origin.y - dy, r.size.width + dx + dx, r.size.height + dy + dy);
	return ret;
}

CIImage*	CropAndCenterCIImage(CIImage *image, CGRect originalExtent, CGRect* cropRegionRef);
CIImage*	CropAndCenterCIImage(CIImage *image, CGRect originalExtent, CGRect* cropRegionRef)
{
#if SCIMAGE_FILTER_DEBUG
	NSLog(@"Cropping and centering CIImage %p", image);
#endif
	CGRect extent = [image extent];
	CIFilter * ciFilter;

	// cropping
	if(cropRegionRef)
	{
		CGRect cropRegion = *cropRegionRef;
		// Crop to specified rectangle.
		if(extent.size.width > cropRegion.size.width || extent.size.height > cropRegion.size.height)
		{
			if(extent.origin.x > 0) {cropRegion.origin.x = extent.origin.x;}
			if(extent.origin.y > 0) {cropRegion.origin.y = extent.origin.y;}


			// quite strange but happens sometime...
			if(extent.origin.x < 0 && ((extent.origin.x + extent.size.width) < cropRegion.size.width)) {
				float diff = extent.size.width + extent.origin.x;
				cropRegion.origin.x -= (cropRegion.size.width - diff);
			}

			if(extent.origin.y < 0 && ((extent.origin.y + extent.size.height) < cropRegion.size.height)) {
				float diff = extent.size.height + extent.origin.y;
				cropRegion.origin.y -= (cropRegion.size.height - diff);
			}

#if SCIMAGE_FILTER_DEBUG
			NSLog(@"SCImageFilter: Cropping To Size: (%4.2f, %4.2f, %4.2f, %4.2f) fromExtent: (%4.2f, %4.2f, %4.2f, %4.2f)",
				cropRegion.origin.x,cropRegion.origin.y,cropRegion.size.width,cropRegion.size.height,
				extent.origin.x,extent.origin.y,extent.size.width,extent.size.height
				);
#endif

			ciFilter = [CIFilter filterWithName:@"CICrop"]; // crop the final result according
			[ciFilter setValue:image forKey:@"inputImage"];
			[ciFilter setValue:[CIVector vectorWithX:cropRegion.origin.x Y:cropRegion.origin.y Z:cropRegion.size.width W:cropRegion.size.height] forKey:@"inputRectangle"];
			image = [ciFilter valueForKey:@"outputImage"];
			extent = [image extent];
#if SCIMAGE_FILTER_DEBUG
			NSLog(@"SCImageFilter: After CROP Extent: (%4.2f, %4.2f, %4.2f, %4.2f)", extent.origin.x,extent.origin.y,extent.size.width, extent.size.height);
#endif
		}
	}

	// translating origin and center it if we need it
	// currently auto translation is now deactivated by default - let the user choose

	/*
	if(extent.origin.x != 0 || extent.origin.y != 0) {
		ciFilter = [CIFilter filterWithName:@"CIAffineTransform"];
		NSAffineTransform* trs = [NSAffineTransform transform];
		[trs translateXBy:-extent.origin.x yBy:-extent.origin.y]; // move to zero
		[ciFilter setValue:trs forKey:@"inputTransform"];
		[ciFilter setValue:image forKey:@"inputImage"];
		image = [ciFilter valueForKey:@"outputImage"];
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"SCImageFilter: After Translate: (%4.2f, %4.2f, %4.2f, %4.2f)", extent.origin.x,extent.origin.y,extent.size.width, extent.size.height);
		#endif
	}
	*/

	return image;
}

id		nsciObjectFromSlot(PyrSlot *slot, Class attributeClass, NSString *attributeType);
id		nsciObjectFromSlot(PyrSlot *slot, Class attributeClass, NSString *attributeType)
{
	int err;
	if(attributeClass == [CIImage class] || attributeType == kCIAttributeTypeGradient)
	{
		SCImage *image = (SCImage *)slotRawPtr(slotRawObject(slot)->slots);
		return [image ciimage];
	}
	else if(isKindOfSlot(slot, s_scimage->u.classobj) && (attributeClass == NULL) && (attributeType == NULL))  // here returns the CISampler by default
	{
		SCImage *image = (SCImage *)slotRawPtr(slotRawObject(slot)->slots);
		return [CISampler samplerWithImage:[image ciimage]];
	}
//	else if(isKindOfSlot(slot, s_vector->u.classobj)) {
/*
	else if(isKindOfSlot(slot, s_array->u.classobj)) {
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Creating CIVector From Vector Slot");
		#endif

		return ciVectorFromSlot(slotRawObject(slot));
	}
*/
	else if(isKindOfSlot(slot, s_color->u.classobj))
	{
		PyrSlot* color_slots = slotRawObject(slot)->slots;
		float cr,cg,cb,ca;
		err = slotFloatVal(color_slots+0, &cr);
		if (err) cr = 0.f;
		err = slotFloatVal(color_slots+1, &cg);
		if (err) cg = 0.f;
		err = slotFloatVal(color_slots+2, &cb);
		if (err) cb = 0.f;
		err = slotFloatVal(color_slots+3, &ca);
		if (err) ca = 1.f;

		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Creating CIColor/NSColor From Color Slot");
		#endif

		if(attributeClass == [NSColor class])
		{
			return [NSColor colorWithCalibratedRed:cr green:cg blue:cb alpha:ca];
		}
		return [CIColor colorWithRed:cr green:cg blue:cb alpha:ca];
	}
	else if(isKindOfSlot(slot, s_array->u.classobj))
	{
		if(attributeClass == [NSAffineTransform class]) {
			#if SCIMAGE_FILTER_DEBUG
				NSLog(@"Creating NSAffineTransform From Array Slot");
			#endif
			// array containing [Tx, Ty, Sx, Sy, Rot in radians]
			PyrObject* array = slotRawObject(slot);
			int size = sc_min(array->size, 5), err;
			float attributes[5] = {0.f, 0.f, 1.f, 1.f, 0.f};
			for(int i=0; i < size; ++i) {
				err = slotFloatVal(array->slots+i, attributes+i);
				if(err)
					attributes[i] = 0.f;
			}

			NSAffineTransform *transform =  [NSAffineTransform transform];
			[transform translateXBy:attributes[0] yBy:attributes[1]];
			[transform scaleXBy:attributes[2] yBy:attributes[3]];
			[transform rotateByRadians:attributes[4]];
			return transform;
		}

		if(attributeClass == [CIVector class] || attributeClass == NULL) {
			#if SCIMAGE_FILTER_DEBUG
				NSLog(@"Creating CIVector From Array Slot");
			#endif
			return ciVectorFromSlot(slotRawObject(slot));
		}

		#if 0
		if(nsClassIsKindOfClass(attributeClass, [NSData class])) {
			// handle specific case here...
			// mostly for CIColorCube Filter
			#if SCIMAGE_FILTER_DEBUG
				NSLog(@"Creating NSData From Array Slot");
			#endif
			PyrClass *support = slotArrayGetCommonClass(slotRawObject(slot));
			if(
				support == NULL ||
				/*
				(
					support != s_int8array->u.classobj &&
					support != s_int16array->u.classobj &&
					support != s_int32array->u.classobj &&
					support != s_doublearray->u.classobj &&
					support != class_floatarray->u.classobj
				)
				*/
			)
				return nil;

		}
		#endif
	}
	else if(IsTrue(slot) || IsFalse(slot) )
	{
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Creating NSNumber From Boolean Slot");
		#endif
		return [NSNumber numberWithBool:(IsTrue(slot))];
	}
	else if(IsFloat(slot) || IsInt(slot))
	{
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Creating NSNumber From Float/Int Slot");
		#endif

		float val;
		err = safeSlotFloatVal(slot, &val);
		if(err) return nil;

		if(attributeType == kCIAttributeTypeBoolean) {
			return [NSNumber numberWithBool:val > 0];
		}
		return [NSNumber numberWithFloat:val];
	}

	post("Non supported type of class: %s", slotRawObject(slot)->classptr->name);
	return nil; // failure
}


int prSCImageFilter_ApplyMultiple(struct VMGlobals *g, int numArgsPushed);
int prSCImageFilter_ApplyMultiple(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver			= g->sp - 4;
	PyrSlot		*filtersSlot		= g->sp - 3;
	PyrSlot		*inPlace			= g->sp - 2;
	PyrSlot		*updateRegionSlot	= g->sp - 1;
	PyrSlot		*cropRegionSlot		= g->sp;
	int			err = errNone;

	SCImage *scimage = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!scimage){
		SetNil(receiver);
		return errNone;
	}

	int			numberOfFilters;
	PyrObject*	scFilter;
	PyrObject	*arguments;
	PyrSymbol	*key;
	PyrSlot		*value;
	id			nsvalue;
	//NSArray		*inputKeys;
	NSString	*nskey;
	Class		valueClass;
	CIImage*	ciimage;
	CIFilter*	ciFilter;
	CGRect		origExtent = [scimage extent];
	CGRect		destExtent;
	//CGPoint		delta;

	numberOfFilters = slotRawObject(filtersSlot)->size;

	// Get the CIImage rep in the SCImage
	if(![scimage isAccelerated])
		[scimage syncCIImage]; // update ciimage rep
	ciimage = [scimage ciimage]; // now we can get it

	// Iterate through all the filters
	for(int i=0; i < numberOfFilters; ++i)
	{
		int size = 0;

		scFilter = slotRawObject(&(slotRawObject(filtersSlot)->slots[i]));

		if(IsFalse(scFilter->slots+3)) // filter enabled ?
			continue;

		ciFilter = [CIFilter filterWithName: nsStringFromSlot(scFilter->slots+0)]; // filter name
		if(!ciFilter) return errFailed;
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"SCImageFilter --- Setting Filter Named %@", nsStringFromSlot(scFilter->slots+0));
		#endif
		[ciFilter setDefaults];

		arguments = slotRawObject(&scFilter->slots[2]); // filter values
		size = (arguments->size >> 1);

		for(int i=0; i < size; ++i)
		{
			key				= slotRawSymbol(arguments->slots + (i<<1));
			value			= arguments->slots + ((i<<1) + 1);
			nskey			= [NSString stringWithFormat:@"input%c%s", toupper(*key->name), key->name+1];
			valueClass		= NSClassFromString( [[[ciFilter attributes] objectForKey:nskey] objectForKey:@"CIAttributeClass"]);
			nsvalue			= nsciObjectFromSlot(value, valueClass, [[ciFilter attributes] objectForKey:kCIAttributeType]);

			if(nsvalue)
			{
				#if SCIMAGE_FILTER_DEBUG
					NSLog(@"Setting Key: %@ Value: %@ Class: %@", nskey, nsvalue, valueClass);
				#endif
				[ciFilter setValue:nsvalue forKey:nskey];
			}
			else
			{
				post("Error: bad (nil) slot value for Filter: %s key %s, NSObject conversion failed for class %s !", slotRawSymbol(&scFilter->slots[0])->name, [nskey UTF8String], class_getName(valueClass));
			}
		}

		// concatenate filters
		if([[ciFilter inputKeys]containsObject:@"inputImage"])
		{
			[ciFilter setValue:ciimage forKey:@"inputImage"];
		}

		// get results
		ciimage = [ciFilter valueForKey:@"outputImage"];
		////
	}

	destExtent = [ciimage extent];

#if SCIMAGE_MAC_OS_10_5
	if(
		//CGRectIsInfinite(destExtent) || // infinite rects are mosly results from generator filters
		!IsNil(updateRegionSlot) // better to keep original extent if constrained update
	)
	{
		destExtent = origExtent;
	}
	else
	{
#endif
		// if the rect is not infinite :
		// 1 - crop the image if a cropRegion is specified
		// 2 - translate the image to align origin of extent with coordindate origin of image (0,0)
		CGRect cropRegion;
		if(IsNil(cropRegionSlot)) {
			#if SCIMAGE_FILTER_DEBUG
			NSLog(@"SCImageFilter: Nil Crop Region - creating from %i, %i maximum crop size", SCCIIMAGE_MAX_WIDTH, SCCIIMAGE_MAX_HEIGHT);
			#endif
			cropRegion = OutsetRect(origExtent, floorf((SCCIIMAGE_MAX_WIDTH - origExtent.size.width) * 0.5), floorf((SCCIIMAGE_MAX_HEIGHT - origExtent.size.height) * 0.5));
		} else {
			err = slotGetNSRect(cropRegionSlot, (NSRect*)&cropRegion);
			if(err) return err;
		}
		ciimage = CropAndCenterCIImage(ciimage, origExtent, &cropRegion);
		destExtent = [ciimage extent];

#if SCIMAGE_MAC_OS_10_5
	}
#endif


	PyrObject *receiverObj;
	if(IsTrue(inPlace))
	{
			// Here the caller should be directly updated !
			// Size will be equal if :
			// 1 - the filter does not extend the image bounds (modified / infinite extent)
			// 2 - the update region slot if not nil to constrain updates
			if(CGSizeEqualToSize(destExtent.size, origExtent.size))
			{
#if SCIMAGE_MAC_OS_10_5
				if(!IsNil(updateRegionSlot))
				{
					CGRect updateRegion;
					err = slotGetNSRect(updateRegionSlot, (NSRect*)&updateRegion);
					if(err) return err;
					// Invert SC Coordinates
					updateRegion.origin.y = [[scimage accumulator]extent].size.height - (updateRegion.origin.y + updateRegion.size.height);
					[[scimage accumulator] setImage:ciimage dirtyRect:updateRegion];
				}
				else
				{
					[[scimage accumulator] setImage:ciimage];
				}
#else
				[ciimage retain];
				[scimage->_ciimage release];
				scimage->_ciimage = ciimage;
#endif
				[scimage setShouldSyncBitmap:YES]; // keep the sync info
				return errNone;
			}
			else
			{
				receiverObj = slotRawObject(receiver);
			}
			#if SCIMAGE_FILTER_DEBUG
				NSLog(@"SCImageFilter: New Extent (%4.2f, %4.2f, %4.2f, %4.2f)", destExtent.origin.x, destExtent.origin.y, destExtent.size.width, destExtent.size.height);
			#endif
	}
	else
	{
		receiverObj = newPyrSCImage(g);
		if(!receiverObj)
		{
			post("Failed creating valid SCImage class !");
			return errFailed;
		}
	}

#if SCIMAGE_DEBUG || SCIMAGE_FILTER_DEBUG
	NSLog(@"SCImageFilter: Creating New SCImage Class");
	NSLog(@"SCImageFilter: OriginalImage Extent: (%4.2f, %4.2f, %4.2f, %4.2f) NewImage Extent: (%4.2f, %4.2f, %4.2f, %4.2f)",
		  origExtent.origin.x, origExtent.origin.y, origExtent.size.width, origExtent.size.height,
		  destExtent.origin.x,destExtent.origin.y,destExtent.size.width, destExtent.size.height
	);
#endif

	SCImage *newSCImage = [[SCImage alloc]initWithCIImage:ciimage extent:destExtent format:[scimage format]];
	if(!newSCImage)
	{
		post("Error: Failed creating valid CIImage cache !!!");
		return errFailed;
	}

	value = receiverObj->slots;
	SetPtr(value + 0, newSCImage);
	SetFloat(value + 1, destExtent.size.width);
	SetFloat(value + 2, destExtent.size.height);

	if(receiverObj == slotRawObject(receiver))
		[scimage release];
	else
		SetObject(receiver, receiverObj);
	return errNone;
}


int prSCImageFilter_Apply(struct VMGlobals *g, int numArgsPushed);
int prSCImageFilter_Apply(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver			= g->sp - 3;
	PyrSlot		*filterNameSlot		= g->sp - 2;
	PyrSlot		*argumentsSlot		= g->sp - 1;
	PyrSlot		*inPlace			= g->sp;

	SCImage *scimage = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!scimage){
		SetNil(receiver);
		return errNone;
	}

	CIFilter*	ciFilter = [CIFilter filterWithName: nsStringFromSlot(filterNameSlot)];
	if(!ciFilter)
		return errFailed;

	[ciFilter setDefaults];

	PyrObject	*arguments = slotRawObject(argumentsSlot); // array
	PyrSymbol	*key;
	PyrSlot		*value;
	NSString	*nskey;
	Class		valueClass;
	id nsvalue;

	int size = (arguments->size >> 1);

	for(int i=0; i < size; ++i) {
		key = (PyrSymbol*)slotRawSymbol(arguments->slots + (i<<1));
		value = arguments->slots + ((i<<1) + 1);
		nskey = [NSString stringWithFormat:@"input%c%s", toupper(*key->name), key->name+1];
		valueClass = NSClassFromString( [[[ciFilter attributes] objectForKey:nskey] objectForKey:@"CIAttributeClass"]);
		nsvalue = nsciObjectFromSlot(value, valueClass, [[ciFilter attributes] objectForKey:kCIAttributeType]);
		if(nsvalue) {
		#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Setting Key: %@ Value: %@ Class: %@", nskey, nsvalue, valueClass);
		#endif
			[ciFilter setValue:nsvalue forKey:nskey];
		}else{
			post("Error: bad (nil) slot value for key %s, NSObject conversion failed for class %s !", [nskey UTF8String], class_getName(valueClass));
		}
	}

	if(IsTrue(inPlace)) {
		[scimage applyFilter:ciFilter]; // in place filter
	}
	else {
		#if SCIMAGE_DEBUG
			NSLog(@"Creating New SCImage Class");
		#endif

		scimage = [scimage imageFilteredWith:ciFilter];  // filtered copy
		PyrObject* object = newPyrSCImage(g);
		PyrSlot *slots = object->slots;
		SetPtr(slots + 0, scimage);
		SetFloat(slots + 1, (float)[scimage width]);
		SetFloat(slots + 2, (float)[scimage height]);
		SetObject(receiver, object);
	}

	return errNone;
}


/*
 *	Currently only cache the CIKernel object + install a finalizer on it
 */
int cachePyrKernel(VMGlobals *g, PyrObject* kernelObject)
{
	NSString*	kernelCmd = nsStringFromSlot(kernelObject->slots+0);
	if(!kernelCmd) {
		post("Error: Kernel string command does not seem valid !\n");
		return errFailed;
	}

	id	kernel = [CIKernel kernelsWithString:kernelCmd];
	if(!kernel) {
		post("Error: Failed creating valid Kernel from shader string !!\n");
		return errFailed;
	}

	// Create the Kernel
	kernel = [(NSArray*)kernel objectAtIndex:0];
	if(!kernel) {
		post("Error: Failed retrieving valid CIKernel !");
		return errFailed;
	}
	[kernel retain];

	if( !IsNil(kernelObject->slots+4) ) {
		id current = (id)slotRawPtr(&kernelObject->slots[4]);
		if(current)
			[current release];
	}

	SetPtr(kernelObject->slots+4, kernel);

	if( IsNil(kernelObject->slots+5) ) {
		#if SCIMAGE_DEBUG
		post("install finalizer scimagekernel (%p)\n", kernelObject);
		#endif
		InstallFinalizer(g, kernelObject, 5, FinalizeSCImageKernelObject);
	}
	#if SCIMAGE_DEBUG
	post("caching scimagekernel (%p)\n", kernelObject);
	#endif
	return errNone;
}

int prSCImageKernel_Compile(struct VMGlobals *g, int numArgsPushed);
int prSCImageKernel_Compile(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* receiver = g->sp;
	return cachePyrKernel(g, slotRawObject(receiver));
}

/*
 * This function is intended to apply a specific CIKernel to the image
 * Still experimental
 */
int prSCImageFilter_ApplyKernel(struct VMGlobals *g, int numArgsPushed);
int prSCImageFilter_ApplyKernel(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver			= g->sp - 3;
	PyrSlot		*kernelSlot			= g->sp - 2;
	PyrSlot		*cropRegionSlot		= g->sp - 1;
	PyrSlot		*inPlace			= g->sp;

	PyrSlot		*argumentSlot;
	SCImage		*newSCImage=nil;
	SCImage		*scimage;
	PyrObject	*kernelObject=nil;
	CIKernel	*kernel=nil;

	int			err;
	CGRect		destExtent;
	CGRect		cropRegion;
	CGRect		maxRect;


	err = errNone;
	scimage = (SCImage *)slotRawPtr(slotRawObject(receiver)->slots);
	if(!scimage){
		SetNil(receiver);
		return errNone;
	}

	[scimage setAccelerated:YES];
	CIFilter*	ciFilter = [[CIFilter new]autorelease];

	///// Setting and caching the Kernel Object
	kernelObject = slotRawObject(kernelSlot);
	if(IsNil(kernelObject->slots+4)) {
		err = cachePyrKernel(g, kernelObject);
		if(err) return err;
	}
	argumentSlot = kernelObject->slots+1;
	kernel = (CIKernel*)(slotRawPtr(&kernelObject->slots[4]));
	if(!kernel)
		return errFailed;

	// Get the extent of the image
	CGRect extent = [[scimage ciimage]extent];

	#if SCIMAGE_FILTER_DEBUG
		NSLog(@"SCImageFilter: CIKernel created Successfully !");
	#endif
	// Setting the Argument Array + PyrObject->Cocoa conversion
	// from the docs:
	/*
	You can pass the following types to a kernel routine:
		sampler:
			Requires a CISampler object when applied. // directly added in SCApp
		__table:
			A qualifier for a sampler type.
		float, vec2, vec3, vec4:
			Requires an NSNumber or CIVector.
		__color:
			A color that will be matched to the CIContext working color space when passed into the program.
			It requires a CIColor object when applied. To the kernel program it appears to be a vec4 type in premultiplied RGBA format.
	*/

	NSMutableArray* arguments = [NSMutableArray arrayWithCapacity:16];
	if(!arguments) {
		post("Error: memory error ? allocation failed for NSMutableArray. applying Kernel aborted !");
		return errFailed;
	}

	#if SCIMAGE_FILTER_DEBUG
		NSLog(@"SCImageFilter: CIKernel setting Arguments !");
	#endif
	// set all the arguments
	if( !IsNil(argumentSlot) ) { // iterate through all the arguments
		PyrObject * pyrArray = slotRawObject(argumentSlot);
		int size = pyrArray->size;
		for(int i=0; i < size; ++i) {
			id object = nsciObjectFromSlot(pyrArray->slots+i, NULL, NULL);
			if(object) {
				[arguments addObject:object];
			}
			else
				return errFailed; // Kernels require no bad arguments
		}
	}

	// Create the Option Dictionary that contains additional info about final rendering
	// kCIApplyOptionDefinition : DOD of image
	// kCIApplyOptionExtent : size of the resulting image
	NSMutableDictionary * dictionary = [[[NSDictionary dictionary] mutableCopy] autorelease];

	[dictionary
		setValue:[[scimage ciimage] definition] // better to pass directly the filterShape
		forKey:kCIApplyOptionDefinition
	];

	// Set the Extent of the result image if one is specified
	/*
	if(!IsNil(maxBounds)) {
		CGRect maxRect;
		err = slotGetNSRect(maxBounds, (NSRect*)&maxRect);
		if(err) goto Bail;
		[dictionary setValue:
			[NSArray arrayWithObjects:
				[NSNumber numberWithFloat:maxRect.origin.x], // x
				[NSNumber numberWithFloat:maxRect.origin.y], // y
				[NSNumber numberWithFloat:maxRect.size.width], // w
				[NSNumber numberWithFloat:maxRect.size.height] // h
				,nil
			]
			forKey:kCIApplyOptionExtent
		];
	}
	*/

	#if SCIMAGE_FILTER_DEBUG
		NSLog(@"SCImageFilter: CIKernel created Options successfully !");
	#endif

	CIImage *ciimage;
	@try {
		ciimage = [ciFilter apply:kernel arguments:arguments options:dictionary];

		// if the rect is not infinite :
		// 1 - crop the image if a cropRegion is specified
		// 2 - translate the image to align origin of extent with coordindate origin of image (0,0)

		if(IsNil(cropRegionSlot)) {
			cropRegion = OutsetRect(extent, floorf((SCCIIMAGE_MAX_WIDTH - extent.size.width) * 0.5), floorf((SCCIIMAGE_MAX_HEIGHT - extent.size.height) * 0.5));
		} else {
			err = slotGetNSRect(cropRegionSlot, (NSRect*)&cropRegion);
			if(err) return err;
		}

		ciimage = CropAndCenterCIImage(ciimage, extent, &cropRegion);

		destExtent = [ciimage extent];

#if SCIMAGE_FILTER_DEBUG
		NSLog(@"SCImageFilter: applied Kernel Successfully !");
#endif

		// Setting the PyrSlot
		PyrObject* receiverObj;
		if(IsTrue(inPlace))
		{
			if( CGSizeEqualToSize(extent.size, destExtent.size) ) {
			#if SCIMAGE_MAC_OS_10_5
				[[scimage accumulator] setImage:ciimage];
			#else
				[scimage->_ciimage release];
				scimage->_ciimage = [ciimage retain];
			#endif
				[scimage setShouldSyncBitmap:YES]; // invalidates bitmap
				return errNone;
			}else{
				NSLog(@"CIKernel different size returned");
				receiverObj = slotRawObject(receiver);
			}
		}
		else
		{
			receiverObj = newPyrSCImage(g);
		}

#if SCIMAGE_FILTER_DEBUG
		NSLog(@"SCImageFilter: Creating New SCImage Class");
#endif

		newSCImage = [[SCImage alloc]initWithCIImage:ciimage extent:destExtent format:[scimage format]];

		PyrSlot *slots = receiverObj->slots;
		[(id)slotRawPtr(slots) release];

		SetPtr(slots + 0, newSCImage);
		SetFloat(slots + 1, destExtent.size.width);
		SetFloat(slots + 2, destExtent.size.height);

		if(receiverObj != slotRawObject(receiver)) {
			SetObject(receiver, receiverObj);
		}
	}
	@catch ( NSException *e )
	{
		//if([[e name]isEqualToString: @"CIKernelMissingArgument"])
		post("Error: %s\n", [[NSString stringWithFormat:@"%@ %@", @"SCImageKernel", e]UTF8String]);
		return errFailed;
	}

	return errNone;
}

int prSCImageFilter_GetAttributeMinMax(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver			= g->sp - 2;
	PyrSlot		*filterAttributeSlot= g->sp - 1;
	PyrSlot		*returnedArraySlot = g->sp;
	CIFilter	*ciFilter;

	ciFilter = [CIFilter filterWithName: nsStringFromSlot(slotRawObject(receiver)->slots+0)]; // filter name
	if(!ciFilter) return errFailed;

	NSDictionary* attrDict = [ciFilter attributes];
	NSString *key = nsStringFromSlot(filterAttributeSlot);
	id attribute;

	#if SCIMAGE_FILTER_DEBUG
			NSLog(@"Checking for CIFilter MinMax of attribute: %@", key);
	#endif

	if( !key ) {
	    SetNil(receiver);
	    return errNone;
	}
//	    goto BailErr;

	const char* cStr = [key cStringUsingEncoding:NSASCIIStringEncoding];
	attribute = [attrDict objectForKey: [NSString stringWithFormat:@"input%c%s", toupper(*cStr), cStr+1]];
	if( !attribute ) {
	    post("SCImageFilter : no attributes named %s found !\n", [key UTF8String]);
	    SetNil(receiver);
	    return errNone;
//	    goto BailErr;
	}

	PyrObject* pyrArray = slotRawObject(returnedArraySlot); // should be an array of size 3
	NSString* attrType = [attribute objectForKey:kCIAttributeType];

	id minmax[3];
	minmax[0] = [attribute objectForKey: kCIAttributeSliderMin] ? [attribute objectForKey: kCIAttributeSliderMin] : [attribute objectForKey: kCIAttributeMin];
	minmax[1] = [attribute objectForKey: kCIAttributeSliderMax] ? [attribute objectForKey: kCIAttributeSliderMax] : [attribute objectForKey: kCIAttributeMax];
	minmax[2] = [attribute objectForKey: kCIAttributeDefault];

    if(!minmax[0] && !minmax[1] && !minmax[2]){ // if no slider min or slider max or default - set to nil directly
	    //goto BailErr;
	    SetNil(receiver);
	    return errNone;
	}

	if(attrType == kCIAttributeTypeBoolean)
	{
		SetFalse(pyrArray->slots+0);
		SetTrue(pyrArray->slots+1);
		SetBool(pyrArray->slots+2, ([ minmax[2] floatValue ] > 0.f));
	}
	else if(
		attrType == kCIAttributeTypeAngle  || attrType == kCIAttributeTypeDistance ||
		attrType == kCIAttributeTypeScalar || attrType == kCIAttributeTypeTime
	)
	{
		if(minmax[0])
			SetFloat(pyrArray->slots+0, [minmax[0] doubleValue]);
		else
			SetNil(pyrArray->slots+0);

		if(minmax[1])
			SetFloat(pyrArray->slots+1, [minmax[1] doubleValue]);
		else
			SetNil(pyrArray->slots+1);

		if(minmax[2])
			SetFloat(pyrArray->slots+2, [minmax[2] doubleValue]);
		else
			SetNil(pyrArray->slots+1);
	}
	else if( attrType == kCIAttributeTypePosition ||  attrType == kCIAttributeTypeOffset )
	{
		// 2-element vector
		PyrObject* vector[3];
		for(int i=0; i < 3; ++i) {
			if( ! minmax[i] ) {
				SetNil(pyrArray->slots+i); continue;
			}
			vector[i] = newPyrArray(g->gc, sizeof(double) * 2, 0, true);
			SetFloat( vector[i]->slots+0, [ (CIVector*)minmax[i] X ]);
			SetFloat( vector[i]->slots+1, [ (CIVector*)minmax[i] Y ]);
			vector[i]->size = 2;
			SetObject(pyrArray->slots+i, vector[i]);
				g->gc->GCWrite(pyrArray, vector[i]);
		}
	}
	else if( attrType == kCIAttributeTypePosition3 )
	{
		// 3-element vector
		PyrObject* vector[3];
		for(int i=0; i < 3; ++i) {
			if( ! minmax[i] ) {
				SetNil(pyrArray->slots+i); continue;
			}
			vector[i] = newPyrArray(g->gc, sizeof(double) * 3, 0, true);
			SetFloat( vector[i]->slots+0, [ (CIVector*)minmax[i] X ]);
			SetFloat( vector[i]->slots+1, [ (CIVector*)minmax[i] Y ]);
			SetFloat( vector[i]->slots+2, [ (CIVector*)minmax[i] Z ]);
			vector[i]->size = 3;
			SetObject(pyrArray->slots+i, vector[i]);
				g->gc->GCWrite(pyrArray, vector[i]);
		}
	}
	else if( attrType == kCIAttributeTypeRectangle )
	{
		// 4-element vector
		PyrObject* vector[3];
		for(int i=0; i < 3; ++i) {
			if( ! minmax[i] ) {
				SetNil(pyrArray->slots+i); continue;
			}
			vector[i] = newPyrArray(g->gc, sizeof(double) * 4, 0, true);
			SetFloat( vector[i]->slots+0, [ (CIVector*)minmax[i] X ]);
			SetFloat( vector[i]->slots+1, [ (CIVector*)minmax[i] Y ]);
			SetFloat( vector[i]->slots+2, [ (CIVector*)minmax[i] Z ]);
			SetFloat( vector[i]->slots+3, [ (CIVector*)minmax[i] W ]);
			vector[i]->size = 4;
			SetObject(pyrArray->slots+i, vector[i]);
				g->gc->GCWrite(pyrArray, vector[i]);
		}
	}
	else if( attrType == kCIAttributeTypeOpaqueColor )
	{
		// 4-element vector
		PyrObject* vector[3];
		for(int i=0; i < 3; ++i) {
			vector[i] = ::instantiateObject(g->gc, s_color->u.classobj, 0, false, true);
			SetFloat( vector[i]->slots+0, (double)[ (CIColor*)minmax[i] red ]);
			SetFloat( vector[i]->slots+1, (double)[ (CIColor*)minmax[i] green ]);
			SetFloat( vector[i]->slots+2, (double)[ (CIColor*)minmax[i] blue ]);
			SetFloat( vector[i]->slots+3, (double)[ (CIColor*)minmax[i] alpha ]);
			SetObject(pyrArray->slots+i, vector[i]);
				g->gc->GCWrite(pyrArray, vector[i]);
		}
	}
	else {
	    SetNil(receiver);
	    return errNone;
	}
	//	goto BailErr;

	SetObject(receiver, pyrArray);
	return errNone;

//	BailErr:
//	SetNil(receiver);
//	return errNone;
}

int prSCImageFilter_Attributes(struct VMGlobals *g, int numArgsPushed);
int prSCImageFilter_Attributes(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver			= g->sp - 1;
	PyrSlot		*filterNameSlot		= g->sp;
	CIFilter	*filter;

	filter = [CIFilter filterWithName: nsStringFromSlot(filterNameSlot)];
	if(!filter)
		return errFailed;

	NSDictionary* attr = [filter attributes];
	NSArray *inputKeys = [filter inputKeys];

	NSEnumerator* e = [inputKeys objectEnumerator];
	NSString *key;

	int maxsize, i=0;
	maxsize  = [inputKeys count] * 2;
	if([inputKeys containsObject:@"inputImage"]) {
		maxsize -= 2; // remove the inputImage
	}
	PyrSymbolArray* returnObject = newPyrSymbolArray(g->gc, maxsize, 0, true);
	returnObject->size = maxsize;

	const char* cStr;
	while( (key = [e nextObject]) )
	{
		if(i >= maxsize) return errNone; // in case
		if(![key isEqualToString:@"inputImage"]) { // we do not need it
			cStr = [[key substringFromIndex:5] cStringUsingEncoding:NSASCIIStringEncoding]; // remove the 'input'
			returnObject->symbols[i++] = getsym([ [NSString stringWithFormat: @"%c%s", tolower(*cStr), (cStr + 1)] UTF8String]);
			returnObject->symbols[i++] = getsym([ [ [ attr objectForKey: key] objectForKey: @"CIAttributeClass" ] UTF8String ]);
		}
	}

	SetObject(receiver, returnObject);

	return errNone;
}

int prSCImageFilter_NamesInCategory(struct VMGlobals *g, int numArgsPushed);
int prSCImageFilter_NamesInCategory(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot		*receiver, *category, *returnArray;
	NSString	*type;
	NSArray		*array;

	returnArray = g->sp;
	category = g->sp- 1;
	receiver = g->sp - 2;

	//PyrObject *returnObject = slotRawObject(returnArray);
	//int maxsize = MAXINDEXSIZE(returnObject);

	if (!IsSym(category)) return errWrongType;

	type = nsStringFromSlot(category);
	if(!type) return errFailed;

	array = [CIFilter filterNamesInCategory:type];
	if(!array) return errFailed;

	int maxsize = [array count];
	PyrSymbolArray* returnObject = newPyrSymbolArray(g->gc, maxsize, 0, true);
	returnObject->size = maxsize;

	int i=0;
	NSEnumerator *e = [array objectEnumerator];


	while( (type = [e nextObject]) )
	{
		if(i >= maxsize) break; // in case
		returnObject->symbols[i] = getsym([type UTF8String]);
		++i;
	}

	SetObject(receiver, returnObject);

	return errNone;
}

#pragma mark - InitSCImagePrims

void initSCImagePrimitives()
{
	int base, index;

	s_scimage  = getsym("SCImage");
	s_scfilter = getsym("SCImageFilter");
	s_scimagekernel = getsym("SCImageKernel");

	#if SCIMAGE_DEBUG
		NSLog(@"---- SCIMAGE: Init Primitives ----");
		NSLog(@"---- SCIMAGE: Using Mac OS 10.%c Version ----", (SCIMAGE_MAC_OS_10_5 == 1) ? '5' : '4');
	#endif

	if(!gCIFilterPlugInsLoaded) {
		#if SCIMAGE_DEBUG
		NSLog(@"---- SCIMAGE: Loading CIFilter PlugIns ----");
		#endif
		[CIPlugIn loadAllPlugIns];
		gCIFilterPlugInsLoaded = YES;
	}

	base = nextPrimitiveIndex();
	index = 0;


	definePrimitive(base, index++, "_SCImage_New", prSCImage_New, 4, 0);
	//definePrimitive(base, index++, "_SCImage_NewFromFile", prSCImage_NewFromFile, 2, 0); // use only URL
	definePrimitive(base, index++, "_SCImage_NewFromURL", prSCImage_NewFromURL, 2, 0);
	definePrimitive(base, index++, "_SCImage_WriteToFile", prSCImage_WriteToFile, 3, 0);
	definePrimitive(base, index++, "_SCImage_Free", prSCImage_Free, 1, 0);
	definePrimitive(base, index++, "_SCImage_setSize", prSCImage_setSize, 3, 0);
	definePrimitive(base, index++, "_SCImage_setScalesWhenResized", prSCImage_setScalesWhenResized, 2, 0);
	definePrimitive(base, index++, "_SCImage_scalesWhenResized", prSCImage_scalesWhenResized, 1, 0);
	definePrimitive(base, index++, "_SCImage_DrawAtPoint", prSCImage_DrawAtPoint, 5, 0);
	definePrimitive(base, index++, "_SCImage_DrawInRect", prSCImage_DrawInRect, 5, 0);
	definePrimitive(base, index++, "_SCImage_tileInRect", prSCImage_TileInRect, 5, 0);
	definePrimitive(base, index++, "_SCImage_lockFocus", prSCImage_lockFocus, 1, 0);
	definePrimitive(base, index++, "_SCImage_unlockFocus", prSCImage_unlockFocus, 1, 0);
	definePrimitive(base, index++, "_SCImage_recache", prSCImage_recache, 1, 0);
	definePrimitive(base, index++, "_SCImage_setPixelAt", prSCImage_setPixelAt, 4, 0);
	definePrimitive(base, index++, "_SCImage_getPixelAt", prSCImage_pixelAt, 3, 0);
	definePrimitive(base, index++, "_SCImage_setColorAt", prSCImage_setColorAt, 4, 0);
	definePrimitive(base, index++, "_SCImage_getColorAt", prSCImage_getColorAt, 3, 0);
	definePrimitive(base, index++, "_SCImage_setAccelerated", prSCImage_setAccelerated, 2, 0);
	definePrimitive(base, index++, "_SCImage_isAccelerated", prSCImage_isAccelerated, 1, 0);
	definePrimitive(base, index++, "_SCImage_loadPixels", prSCImage_loadPixels, 4, 0);
	definePrimitive(base, index++, "_SCImage_updatePixels", prSCImage_updatePixels, 3, 0);
	definePrimitive(base, index++, "_SCImage_updatePixelsInRect", prSCImage_updatePixelsInRect, 4, 0);
	definePrimitive(base, index++, "_SCImage_interpolation", prSCImage_interpolation, 1, 0);
	definePrimitive(base, index++, "_SCImage_setInterpolation", prSCImage_setInterpolation, 2, 0);
	definePrimitive(base, index++, "_SCImage_sync", prSCImage_sync, 1, 0);
	definePrimitive(base, index++, "_SCImage_fromWindowRect", prSCImage_imageFromSCWindowRect, 3, 0);

	// image Filter - experimental for now
	definePrimitive(base, index++, "_SCImageFilter_NamesInCategory", prSCImageFilter_NamesInCategory, 3, 0);
	definePrimitive(base, index++, "_SCImageFilter_Attributes", prSCImageFilter_Attributes, 2, 0);
	definePrimitive(base, index++, "_SCImageFilter_Apply", prSCImageFilter_Apply, 4, 0);
	definePrimitive(base, index++, "_SCImageFilter_ApplyMultiple", prSCImageFilter_ApplyMultiple, 5, 0);
	definePrimitive(base, index++, "_SCImageFilter_GetAttributeMinMax", prSCImageFilter_GetAttributeMinMax, 3, 0);
	definePrimitive(base, index++, "_SCImageFilter_ApplyKernel", prSCImageFilter_ApplyKernel, 4, 0);
	definePrimitive(base, index++, "_SCImageKernel_Compile", prSCImageKernel_Compile, 1, 0);
}

/*
#else

void initSCImagePrimitives()
{
	// SCImage not supported for os version < 10.4...
}

#endif
*/
