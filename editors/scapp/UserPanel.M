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

#import "UserPanel.h"
#import <objc/objc-class.h>
//#import <objc/runtime.h>

#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include <pthread.h>

NSMutableArray *gUserPanels = nil;
extern PyrSymbol *s_doaction;
extern PyrSymbol *s_closed;
extern pthread_mutex_t gLangMutex;

@implementation UserPanel

+ (void)closeAll
{
    if (gUserPanels && [gUserPanels count]) {
        NSMutableArray *copy = [gUserPanels copy];
        [gUserPanels removeAllObjects];
        [copy makeObjectsPerformSelector: @selector(close)];
        [copy release];
    }
}

- (id)init
{
    [super init];
    if (!gUserPanels) {
        gUserPanels = [NSMutableArray arrayWithCapacity: 8];
        [gUserPanels retain];
    }
    [gUserPanels addObject: self];
    return self;
}

- (NSWindow*)window { return window; }

- (void)close 
{
    [window close];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
    [gUserPanels removeObject: self];

    pthread_mutex_lock (&gLangMutex);
    if (scobject) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, scobject); // push window obj
        runInterpreter(g, s_closed, 1);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}

- (void)setSCObject: (struct PyrObject*)inObject { scobject = inObject; }

- (struct PyrObject*) getSCObject { return scobject; }

void post(const char *fmt, ...);

- (IBAction) panelAction: (id) sender
{
    id hitThing = sender;
    post("sender '%s' %d\n", class_getName(sender->isa), [sender tag]);
    if ([sender respondsToSelector: @selector(selectedCell)]) {
        id cell = [sender selectedCell];
        if (cell) {
            hitThing = cell;
        } else {
            post("  cell nil\n");
        }
    }
    int tag = [hitThing tag];
    post("hitThing '%s' %d\n", class_getName(hitThing->isa), [sender tag]);
    // what kind of value is appropriate?
    double controlValue = 0.;
    if ([hitThing respondsToSelector: @selector(doubleValue)]) {
        controlValue = [hitThing doubleValue];
    }
    char *cstring = 0;
    if ([hitThing respondsToSelector: @selector(stringValue)]) {
        cstring = (char*)[[hitThing stringValue] cString];
    }
    
    pthread_mutex_lock (&gLangMutex);
    if (scobject) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, scobject); // push window obj
        ++g->sp;  SetInt(g->sp, tag); // push tag
        ++g->sp;  SetFloat(g->sp, controlValue); 
        if (cstring) {
            PyrString *scstring = newPyrString(g->gc, cstring, 0, false);
            ++g->sp;  SetObject(g->sp, scstring); // push tag
        } else {
            ++g->sp;  SetNil(g->sp); // push tag
        }
        runInterpreter(g, s_doaction, 4);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}


@end
