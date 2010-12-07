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

#ifndef SC_IPHONE
#import <Cocoa/Cocoa.h>
#import "SCImage.h"
#endif

#import "DrawBackground.h"

DrawBackground::DrawBackground()
{
}

void DrawBackground::draw(CGContextRef cgc, CGRect inRect)
{
    CGContextSaveGState(cgc);
    drawSelf(cgc, inRect);
    CGContextRestoreGState(cgc);
}

void DrawBackground::drawSelf(CGContextRef cgc, CGRect inRect)
{
}

SolidColorBackground::SolidColorBackground(SCColor inColor)
    : mColor(inColor)
{
}

void SolidColorBackground::drawSelf(CGContextRef cgc, CGRect inRect)
{
    CGContextSetRGBFillColor(cgc, mColor.red, mColor.green, mColor.blue, mColor.alpha);
    CGContextFillRect(cgc, inRect);
}

GradientBackground::GradientBackground(SCColor inStartColor, SCColor inEndColor, int inDirection, int inSteps)
    : mStartColor(inStartColor), mEndColor(inEndColor), mDirection(inDirection), mSteps(inSteps)
{
}

void hPaintGradient(CGContextRef cgc, CGRect bounds, SCColor startColor, SCColor endColor, int numSteps);
void vPaintGradient(CGContextRef cgc, CGRect bounds, SCColor startColor, SCColor endColor, int numSteps);

void GradientBackground::drawSelf(CGContextRef cgc, CGRect inRect)
{
    bool isHorizontal;
    
    if (mDirection == grad_Narrow) {
        isHorizontal = inRect.size.width < inRect.size.height;
    } else if (mDirection == grad_Wide) {
        isHorizontal = inRect.size.width > inRect.size.height;
    } else {
        isHorizontal = mDirection == grad_Horizontal;
    }

    if (isHorizontal) {
        hPaintGradient(cgc, inRect, mStartColor, mEndColor, mSteps);
    } else {
        vPaintGradient(cgc, inRect, mStartColor, mEndColor, mSteps);
    }
}


HiliteGradientBackground::HiliteGradientBackground(SCColor inStartColor, SCColor inEndColor, int inDirection, int inSteps, float inFrac)
    : GradientBackground(inStartColor, inEndColor, inDirection, inSteps), mFrac(inFrac), mFrac1(1. - inFrac)
{
}


void HiliteGradientBackground::drawSelf(CGContextRef cgc, CGRect inRect)
{
    bool isHorizontal;
    
    if (mDirection == grad_Narrow) {
        isHorizontal = inRect.size.width < inRect.size.height;
    } else if (mDirection == grad_Wide) {
        isHorizontal = inRect.size.width > inRect.size.height;
    } else {
        isHorizontal = mDirection == grad_Horizontal;
    }
    
    CGRect rectA = inRect;
    CGRect rectB = inRect;

    if (isHorizontal) {
        rectA.size.width *= mFrac;
        rectB.origin.x += rectA.size.width;
        rectB.size.width *= mFrac1;
        hPaintGradient(cgc, rectA, mStartColor, mEndColor, (int)(mSteps * mFrac));
        hPaintGradient(cgc, rectB, mEndColor, mStartColor, (int)(mSteps * mFrac1));
    } else {
        rectA.size.height *= mFrac;
        rectB.origin.y += rectA.size.height;
        rectB.size.height *= mFrac1;
        vPaintGradient(cgc, rectA, mStartColor, mEndColor, (int)(mSteps * mFrac));
        vPaintGradient(cgc, rectB, mEndColor, mStartColor, (int)(mSteps * mFrac1));
    }
}

// SCImageBackground
#ifndef SC_IPHONE

void ScaleRectInRect( NSRect *bounds, NSSize *origSize, NSRect *destRect );
inline void ScaleRectInRect( NSRect *bounds, NSSize *origSize, NSRect *destRect )
{
	float imgRatio = (*origSize).width / (*origSize).height;
	//float maxDim = ( (*origSize).width > (*origSize).height) ? (*origSize).width : (*origSize).height;
	
	if( imgRatio < 1.0f ) 
	{
		(*destRect).size.height = (*bounds).size.height;
		(*destRect).size.width = (*destRect).size.height * imgRatio;
		if( (*destRect).size.width > (*bounds).size.width ) { // clip and resize
			(*destRect).size.width = (*bounds).size.width;
			(*destRect).size.height = (*destRect).size.width / imgRatio;
		}
		
	} else {
		// width >= height
		(*destRect).size.width = (*bounds).size.width;
		(*destRect).size.height = (*destRect).size.width / imgRatio;
		if( (*destRect).size.height > (*bounds).size.height ) { // clip and resize
			(*destRect).size.height = (*bounds).size.height;
			(*destRect).size.width = (*destRect).size.height * imgRatio;
		}
	}
	
	destRect->origin.x = bounds->origin.x + ((bounds->size.width - destRect->size.width) * 0.5f);
	destRect->origin.y = bounds->origin.y + ((bounds->size.height - destRect->size.height) * 0.5f);
}

SCImageBackground::SCImageBackground(SCImage* inSCImage, NSRect inFromRect, int inTileMode, float inFraction)
    : mSCImage(inSCImage), mFromRect(inFromRect), mTileMode(inTileMode), mFraction(inFraction)
{
	[mSCImage retain]; // in case
	
	// convert sc rect to image coordinates + clipping
	float w, h;
	
	w = [mSCImage size].width;
	h = [mSCImage size].height;
	
	mSwappedYOrigin = sc_clip(h - (mFromRect.origin.y + mFromRect.size.height), 0, h);
	mOriginalYOrigin = sc_clip(mFromRect.origin.y, 0, h); 
	
	mFromRect.origin.x = sc_clip(mFromRect.origin.x, 0, w);
	
	if((mFromRect.origin.x + mFromRect.size.width) > w)
		mFromRect.size.width = w - mFromRect.origin.x;
		
	if((mFromRect.origin.y + mFromRect.size.height) > h)
		mFromRect.size.height = h - mFromRect.origin.y;
}

#if 0
void SCImageBackground::rebuildCache(CGContextRef cgc)
{
	if([mSCImage isSynced])
		return;
	
	[mSCImage syncRepresentations];
		
	if([mSCImage isAccelerated]) { // build CIImage
		// no cache for now -> may be see with CGLayer conversion
	}else{
		CGContextRef ctx;
		CGLayerRef layer = CGLayerCreateWithContext(cgc, CGSizeMake([mSCImage size].width, [mSCImage size].height), NULL);
		if(!layer) {
			NSLog(@"SCImageBackground::rebuildCache failed Creating valid CGLayerRef");
		}
		ctx = CGLayerGetContext(layer);
			
		[NSGraphicsContext saveGraphicsState];
		NSGraphicsContext *layerCtx = [NSGraphicsContext contextWithGraphicsPort:ctx flipped:NO];
		[NSGraphicsContext setCurrentContext:layerCtx];
			CGContextSaveGState(ctx);
			CGContextTranslateCTM(ctx, 0, mFromRect.size.height);
			CGContextScaleCTM(ctx, 1, -1.0f);
			[mSCImage drawInRect:NSMakeRect(0.f, 0.f, mFromRect.size.width, mFromRect.size.height) fromRect:mFromRect operation:NSCompositeSourceOver fraction:mFraction];
			CGContextSaveRestoreGState(ctx);
		[NSGraphicsContext restoreGraphicsState];
	}
}
#endif

void SCImageBackground::drawSelf(CGContextRef cgc, CGRect inRect)
{	
/*
1 - fixed to left, fixed to top ---> topLeft
2 - horizontally tile, fixed to top ---> top + tilex
3 - fixed to right, fixed to top ---> topRight
4 - fixed to left, vertically tile ---> left + tiley
5 - horizontally tile, vertically tile --->  tilexy
6 - fixed to right, vertically tile --> right + tiley
7 - fixed to left, fixed to bottom --> bottomLeft
8 - horizontally tile, fixed to bottom --> bottom + tiley
9 - fixed to right, fixed to bottom ---> bottomRight
10 - fit ---> scale to fit in the view rect (meaning a full drawRect instead
of a drawAtPoint with tile options)
11 - center x y
12 - center , fixed to top
13 - center , fixed to bottom
14 - fixed to left, center
15 - fixed to right, center
*/

//	printf("SCImageBackground::drawSelf x: %f\t y: %f\t width: %f\t height: %f\n", 
//		inRect.origin.x, inRect.origin.y, inRect.size.width, inRect.size.height);

	CGContextSaveGState(cgc);
	CGContextClipToRect(cgc, inRect);
	
	#if SCIMAGE_BACKGROUND_DEBUG
		extern double GetTimeOfDay();
		double past = GetTimeOfDay();
	#endif
	
	NSRect destinationRect = NSMakeRect(inRect.origin.x, inRect.origin.y, mFromRect.size.width, mFromRect.size.height);
	
	switch(mTileMode) 
	{
		case 2:
		case 8:
		{
			destinationRect.size.width = inRect.size.width;
			destinationRect.origin.y += (mTileMode == 2 ? 0 : inRect.size.height - mFromRect.size.height);
			[mSCImage tileInSCRect:destinationRect fromRect:mFromRect operation:NSCompositeSourceOver fraction:mFraction];
		}
			goto end1;
			
		case 5:
		{
			destinationRect.size = NSMakeSize(inRect.size.width, inRect.size.height);
			[mSCImage tileInSCRect:destinationRect fromRect:mFromRect operation:NSCompositeSourceOver fraction:mFraction];
		}
			goto end1;
			
		case 4:
		case 6:
		{
			destinationRect.size.height = inRect.size.height;
			destinationRect.origin.x += (mTileMode == 4 ? 0 : (inRect.size.width - mFromRect.size.width));
			[mSCImage tileInSCRect:destinationRect fromRect:mFromRect operation:NSCompositeSourceOver fraction:mFraction];
		}
			goto end1;
			
		case 3:
			destinationRect.origin.x += inRect.size.width - mFromRect.size.width;
			break;
			
		case 9:
			destinationRect.origin.x += inRect.size.width - mFromRect.size.width;
			destinationRect.origin.y += inRect.size.height - mFromRect.size.height;
			break;
			
		case 7:
			destinationRect.origin.y += inRect.size.height - mFromRect.size.height;
			break;
			
			// center
		case 10:
			destinationRect.size = *(NSSize*)&inRect.size;
			break;
			
		case 11: // center x - center y + scale
			ScaleRectInRect((NSRect*)&inRect, &mFromRect.size, (NSRect*)&destinationRect);
			break;
			
		case 12: // center - fixed to top
			destinationRect.origin.x += ((inRect.size.width - destinationRect.size.width) * 0.5);
			break;
			
		case 13: // center - fixed to bottom
			destinationRect.origin.x += ((inRect.size.width - destinationRect.size.width) * 0.5);
			destinationRect.origin.y += inRect.size.height - mFromRect.size.height;
			break;
			
		case 14: // fixed to left - center
			destinationRect.origin.y += ((inRect.size.height - destinationRect.size.height) * 0.5);
			break;
			
		case 15: // fixed to right - center
			destinationRect.origin.y += ((inRect.size.height - destinationRect.size.height) * 0.5);
			destinationRect.origin.x += (inRect.size.width - destinationRect.size.width);
			break;
			
		case 16:
			destinationRect.origin.x += ((inRect.size.width - destinationRect.size.width) * 0.5);
			destinationRect.origin.y += ((inRect.size.height - destinationRect.size.height) * 0.5);
			break;
			
		case 1:
		default:
			break;
	}
	[mSCImage drawInSCRect:destinationRect fromRect:mFromRect operation:NSCompositeSourceOver fraction:mFraction];
	
end1:
#if SCIMAGE_BACKGROUND_DEBUG
	NSLog(@"SCImageBackground (%p) SCImage Time To Render: %f", this, GetTimeOfDay() - past);
#endif
	
	CGContextRestoreGState(cgc);
}

SCImageBackground::~SCImageBackground()
{
	#if SCIMAGE_BACKGROUND_DEBUG
		NSLog(@"SCImageBackground (%p) release SCImage (%p)", this, mSCImage);
	#endif
	[mSCImage release];
}
#endif

#if 0

CGPatternCreate(void *info, CGRect bounds, CGAffineTransform matrix, float xStep, float yStep, CGPatternTiling tiling, int isColored, const CGPatternCallbacks *callbacks)
struct CGPatternCallbacks {
    unsigned int version;
    void (*drawPattern)(void *info, CGContextRef c);
    void (*releaseInfo)(void *info);
};
typedef struct CGPatternCallbacks CGPatternCallbacks;

drawTile()
{
    [mTile draw];
}

static void drawPattern(void *info, CGContextRef c)
{
    TiledBackground *tb = (TiledBackground*)info;
    tb->drawTile();
};

CGPatternCallbacks callbacks;
callbacks.version = 0;
callbacks.drawPattern = drawPattern;
callbacks.releaseInfo = releaseInfo;

CGPatternCreate(this, tileBounds, CGAffineTransformIdentity, 
    tileBounds.size.width, tileBounds.size.height, kCGPatternTilingConstantSpacing, YES, callbacks);

#endif
