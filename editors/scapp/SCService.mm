//
//  SCService.m
//  SC3lang
//
//  Created by C. Ramakrishnan on Mon Oct 20 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

#import "SCService.h"

#import "SCVirtualMachine.h"
#include "SCBase.h"
#include "PyrSymbol.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"

#import <AppKit/NSPasteboard.h>
#include <pthread.h>
#include "../../lang/LangSource/PyrSched.h"


extern bool compiledOK;

@implementation SCService

- (void)doExecuteService:(NSPasteboard *)pboard userData:(NSString *)data error:(NSString **)error
{
    NSString *pboardString;
	NSArray *types;

	types = [pboard types];

	// if there's a problem with the data passed to this method
	if(![types containsObject:NSStringPboardType] ||
	!(pboardString = [pboard stringForType:NSStringPboardType])) {
		*error = NSLocalizedString(@"Error: Pasteboard doesn't contain a string.",
		@"Pasteboard couldn't give a string.");
		return;
	}   
	 
	if (!compiledOK) {
		*error = NSLocalizedString(@"Error: SuperCollider library is not compiled.",
			@"Could not execute selection.");
		return;
    }

    const char *text = [pboardString UTF8String];

    [[SCVirtualMachine sharedInstance] setCmdLine: text length: [pboardString length]];
    
    gLangMutex.lock();
    runLibrary(getsym("interpretPrintCmdLine"));
    gLangMutex.unlock();

	return;
}

@end
