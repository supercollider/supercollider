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


#import "GetStringFromUser.h"


@implementation GetStringFromUser

static id sharedGetStringObject = nil;

+ (id)sharedInstance {
    if (!sharedGetStringObject) {
        [[self allocWithZone:[[NSApplication sharedApplication] zone]] init];
    }
    return sharedGetStringObject;
}

- (id)init {
    if (sharedGetStringObject) {
        [super dealloc];
        return sharedGetStringObject;
    }

    if (!(self = [super init])) return nil;

    [self setString:@""];

    sharedGetStringObject = self;
    return self;
}

- (void)loadUI {
    if (!textField) {
        if (![NSBundle loadNibNamed:@"GetStringDlg" owner:self])  {
            NSLog(@"Failed to load GetStringDlg.nib");
            NSBeep();
        }
	if (self == sharedGetStringObject) [[textField window] setFrameAutosaveName:@"GetString"];
    }
    [textField setStringValue: [self string]];
}

/* Loads UI lazily */
- (NSPanel *)getStringPanel
{
    if (!textField) [self loadUI];
    return (NSPanel *)[textField window];
}

- (NSString *)string
{
    return string;
}

- (void)setString:(NSString *)newString
{
    if ([newString isEqualToString:string]) return;
    [string autorelease];
    string = [newString copyWithZone:[self zone]];
    if (textField) {
        [textField setStringValue: string];
        [textField selectText: nil];
    }
}


- (void)setPrompt:(NSString *)newString
{
    if (promptField) {
        [promptField setStringValue: newString];
        [promptField selectText: nil];
    }
}


/* Action methods, sent from the find panel UI; can also be connected to menu items */
- (void)ok:(id)sender
{
    if (textField) [self setString: [textField stringValue]];
    // lock the language VM and call some method..
}

- (void)cancel:(id)sender
{
}

@end
