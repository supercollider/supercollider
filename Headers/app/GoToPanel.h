/*
 *  GoToPanel.M
 *  SC3lang
 *
 *  Created by j. trutzschler on 02 sept 2003.
	derived from TextFinder.m by Ali Ozer

	a panel that searches and selects a line

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

#import <Cocoa/Cocoa.h>

@interface GoToPanel : NSObject {
    id findTextField;
    id okButton;
}

/* Common way to get a text finder. One instance of TextFinder per app is good enough. */
+ (id)sharedInstance;

/* Main method for external users; does a find in the first responder. Selects found range or beeps. */
- (void)selectLine:(id)sender;
- (void) getAndDisplayCurrentLine;
- (void)prselectLine:(int)linenum;

/* Loads UI lazily */
- (NSPanel *)gotoLinePanel;

/* Gets the first responder and returns it if it's an NSTextView */
- (NSTextView *)textObjectToSelectIn;

- (void)orderFrontGotoLinePanel:(id)sender;

/* Misc internal methods */
- (void)appDidActivate:(NSNotification *)notification;


@end
