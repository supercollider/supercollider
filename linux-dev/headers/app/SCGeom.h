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

#import <Carbon/Carbon.h>
#include "SC_BoundsMacros.h"

struct SCColor {
    float red, green, blue, alpha;
};
typedef struct SCColor SCColor;

inline SCColor SCMakeColor(float red, float green, float blue, float alpha)
{
	SCColor sccolor;
	sccolor.red = red;
	sccolor.green = green;
	sccolor.blue = blue;
	sccolor.alpha = alpha;
	return sccolor;
}

struct SCPoint {
	float x, y;
};

inline SCPoint SCMakePoint(float x, float y)
{
	SCPoint p;
	p.x = x;
	p.y = y;
	return p;
}

struct SCRect {
	float x, y, width, height;
};

inline SCRect SCRectUnion(SCRect a, SCRect b)
{
        if (a.height <= 0. && a.width <= 0.) return b;
        if (b.height <= 0. && b.width <= 0.) return a;
        
	SCRect u;
	u.x = sc_min(a.x, b.x);
	u.y = sc_min(a.y, b.y);
	u.width = sc_max(a.x + a.width, b.x + b.width) - u.x;
	u.height = sc_max(a.y + a.height, b.y + b.height) - u.y;
	return u;
}

inline bool SCRectsDoIntersect(SCRect a, SCRect b)
{
	if (a.x + a.width < b.x) return false;
	if (a.y + a.height < b.y) return false;
	if (a.x > b.x + b.width) return false;
	if (a.y > b.y + b.height) return false;
	return true;
}

inline SCRect SCMakeRect(float x, float y, float width, float height)
{
	SCRect r;
	r.x = x; r.y = y; r.width = width; r.height = height;
	return r;
}

inline bool SCPointInRect(SCPoint p, SCRect r)
{
	return
	    p.x >= r.x && p.x <= r.x + r.width
	 && p.y >= r.y && p.y <= r.y + r.height;

}
