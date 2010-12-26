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

#import "SCGeom.h"

class DrawBackground
{
public:
    DrawBackground();
	virtual ~DrawBackground() {}
    virtual void draw(CGContextRef cgc, CGRect rect);
    virtual void drawSelf(CGContextRef cgc, CGRect rect);
};


class SolidColorBackground : public DrawBackground
{
public:
    SolidColorBackground(SCColor inColor);
	virtual ~SolidColorBackground() {}
    virtual void drawSelf(CGContextRef cgc, CGRect rect);
private:
    SCColor mColor;
};

enum {
    grad_Horizontal,
    grad_Vertical,
    grad_Narrow,
    grad_Wide
};

class GradientBackground : public DrawBackground
{
public:
    GradientBackground(SCColor inStartColor, SCColor inEndColor, int inDirection, int inSteps);
	virtual ~GradientBackground() {}
    virtual void drawSelf(CGContextRef cgc, CGRect rect);

protected:
    SCColor mStartColor, mEndColor;
    int mDirection, mSteps;
};

class HiliteGradientBackground : public GradientBackground
{
public:
    HiliteGradientBackground(SCColor inStartColor, SCColor inEndColor, int inDirection, int inSteps, float inFrac = .33);
	virtual ~HiliteGradientBackground() {}

    virtual void drawSelf(CGContextRef cgc, CGRect rect);

protected:
    float mFrac, mFrac1;
};

// SCImageBackground
#ifndef SC_IPHONE
@class SCImage;

class SCImageBackground : public DrawBackground
{
public:
    SCImageBackground(SCImage* inSCImage, NSRect inFromRect, int inTileMode, float inFraction);
    ~SCImageBackground();
    virtual void drawSelf(CGContextRef cgc, CGRect rect);
	#if 0
    void rebuildCache(); // see for later if needed
	#endif

protected:
    SCImage* mSCImage;
	NSRect mFromRect;
	int mTileMode;
	float mFraction;
	void* mCache; // CIImage cache for CIImages / CGLayerRef cache for others
	float mSwappedYOrigin;
	float mOriginalYOrigin;
};
#endif
/*
class TiledBackground : public DrawBackground
{
public:
    TiledBackground(NSImage* inTile);

    virtual void drawSelf(CGContextRef cgc, CGRect rect);

protected:
    NSImage* mTile;
    CGRect mTiledBounds;
};

*/

