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


#import <Cocoa/Cocoa.h>

@interface GetStringFromUser : NSObject {
    NSString *string;
    id textField;
    id promptField;
    id okButton;
    id cancelButton;
}

+ (id)sharedInstance;

/* Loads UI lazily */
- (NSPanel *)getStringPanel;

- (NSString *)string;
- (void)setString:(NSString *)string;

- (void)setPrompt:(NSString *)string;

/* Action methods, sent from the find panel UI; can also be connected to menu items */
- (void)ok:(id)sender;
- (void)cancel:(id)sender;

@end
