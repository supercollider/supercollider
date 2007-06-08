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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


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
    [mTile draw];n
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
