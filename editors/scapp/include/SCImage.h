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
 *  SCImage.h
 *
 *	SCImage interface
 *  charles picasso && blackrain 14/08/08.
 */

#import <Cocoa/Cocoa.h>
#import	<QuartzCore/QuartzCore.h>
#import <sys/time.h>

#define SCIMAGE_DEBUG				0
#define SCIMAGE_BACKGROUND_DEBUG	0
#define SCIMAGE_FILTER_DEBUG		0
#define SCCIIMAGE_MAX_WIDTH			4096
#define SCCIIMAGE_MAX_HEIGHT		4096

/*
	May be we should force the 10.4 version ??
	with
	#define SCIMAGE_MAC_OS_10_5			0
	because it does run with no problem on 10.5
	anyway currently in both platform the 10.4 version should be compiled instead of the 10.5
	in case of a problem force the 10.4

	Why 2 different versions ?
	--------------------------
	- CIImageAccumulator does not behave the same with 10.4 SDK on 10.5 : seems like use Generator type Filters
	make it refuse to take the CIImage. This crash the app because of an EXEC_BAD_ACCESS later if drawing.
	- On 10.4 the CIImageAccumulator is replaced by a plain CIImage : this does not change global behaviour
	it just disable the possibility to use Dirty Rect wich should be considered as a marginal option for now.

*/
#define SCIMAGE_MAC_OS_10_4			(MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
//#if defined(MAC_OS_X_VERSION_10_5)
//#define SCIMAGE_MAC_OS_10_5			(MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)
//#else
#define SCIMAGE_MAC_OS_10_5			0
//#endif

#if (SCIMAGE_DEBUG || SCIMAGE_BACKGROUND_DEBUG)
inline double GetTimeOfDay();
double GetTimeOfDay()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + 1.0e-6 * (double)tv.tv_usec;
}
#endif

enum {
	SCImageUseGraphicsAccelerationWhenPossible	=	0,	// SCImage will always use CIImage for drawing - worst performance if user will repeatedly:
														// 1_ draw inside the image
														// 2_ use direct pixel data manipulation (get/set)
														// but definitely better for fast drawing and accelerated image processing techniques (CoreImageFilters)

	SCImageDoNotUseGraphicsAcceleration			=	1	// SCImage will never use CIImage - better performance if user always update the bitmap representation
														// using standard Quartz techniques like :
														// 1_ drawing inside the Image (with lockFocus - unlockFocus)
														// 2_ or using a lot direct pixel data manipulation (get/set)
};
typedef int SCImageHint;

@interface SCImage : NSObject
{
@public
	#if SC_IMAGE_MAC_OS_10_5
	CIImageAccumulator		*_ciimageStore; // accumulator
	#else
	CIImage					*_ciimage; // in 10.4
	#endif
//@private
	CIFormat				_ciFormat; // hint for prefered format when processing CoreImages
	NSBitmapImageRep		*_bitmap;  // ref to data used for pixel manipulation
	NSLock					*_wLock;	// write on lock for pixel manip
	NSGraphicsContext		*_prevCtx; // used for lockFocus - unlockFocus store previous context
	NSImage					*_nsimage; // front model when not accelerated
	NSImageInterpolation	_interpolation;
	SCImageHint				_hints; // store for all hints - only drawing hint for now - not really used...
	BOOL					_accelerated; // switch between different reps
	BOOL					_scales;
	BOOL					_inited; //
	BOOL					_bitmapSynced; // is bitmap rep in sync
	BOOL					_ciimageSynced; // is ciimage rep in sync
	BOOL					_savedCtxState; // flag used for lockFocus - unlockFocus

	void*					_cache; // used to cache reps : might be a CGLayerRef / NSImage / CGPattern (CGLayerRef are cached to GPU when possible)
}
- (id)initWithContentsOfURL:(NSURL*)url isAccelerated:(BOOL)yorn format:(CIFormat)fmt;
- (id)initWithSize:(NSSize)size isAccelerated:(BOOL)yorn format:(CIFormat)fmt;
- (id)initWithCIImage:(CIImage*)image extent:(CGRect)extent format:(CIFormat)imageFormat;
- (id)initWithNSImage:(NSImage*)image;
- (void)commonInit;
- (CIImage*)ciimage;
//- (void)rebuildAccumulatorFromCIImage:(CIImage*)newCIImage; // rebuild everything for inplace operation if extent if different
- (NSImage*)nsimage;
- (NSBitmapImageRep*)bitmapRepresentation;
- (unsigned char*)bitmapData;
- (void)rebuildBitmapRepFromCIImage;
- (void)rebuildNSImageFromBitmapRep;
- (void)rebuildCIImageFromBitmapRep;
- (void)setDrawingHint:(SCImageHint)hint;
- (void)setPixel:(NSUInteger *)pixel atX:(int)x y:(int)y;
- (void)setColor:(NSColor*)color atX:(unsigned int)x y:(unsigned int)y;
- (void)getPixel:(NSUInteger *)pixel atX:(int)x y:(int)y;
- (NSColor*)colorAtX:(unsigned int)x y:(unsigned int)y;
- (void)setScalesWhenResized:(BOOL)yorn;
- (BOOL)scalesWhenResized;
- (BOOL)isAccelerated;
- (void)setAccelerated:(BOOL)yorn;
- (NSImageInterpolation)imageInterpolation;
- (void)setImageInterpolation:(NSImageInterpolation)interp;
- (NSSize)size;
- (CGRect)extent;
- (void)setSize:(NSSize)s;
- (void)lock;
- (void)unlock;
- (void)lockFocus;
- (void)unlockFocus;
- (void)recache;
- (BOOL)isSynced;
- (BOOL)hasChanged;
- (void)setShouldSyncBitmap:(BOOL)yorn;
- (void)setShouldSyncCIImage:(BOOL)yorn;
- (void)syncBitmap;
- (void)syncCIImage;
- (int)width;
- (int)height;
- (CIFormat)format;
#if SCIMAGE_MAC_OS_10_5
- (CIImageAccumulator*)accumulator;
#endif
- (void)syncRepresentations; // synchronize representations
- (void)drawAtPoint:(NSPoint)point fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)drawInRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)drawInSCRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)tileInSCRect:(NSRect)dstRect fromRect:(NSRect)srcRect operation:(NSCompositingOperation)op fraction:(float)delta;
- (void)applyFilter:(CIFilter*)filter;
-(SCImage*)imageFilteredWith:(CIFilter*)filter;
@end
