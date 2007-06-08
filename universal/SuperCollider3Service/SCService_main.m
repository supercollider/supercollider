//
//  SCService_main.h
//  SC3lang
//
//  Created by C. Ramakrishnan on Mon Feb 24 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//
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

// This application is completely vacuous -- it exists purely to let 
// the Finder know that SuperCollider can handle the 
// "Execute in SuperCollider" service

#import <Foundation/Foundation.h>

int main (int argc, const char *argv[]) {
    exit(0);       // insure the process exit status is 0
    return 0;      // ...and make main fit the ANSI spec.
}
