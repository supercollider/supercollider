/*
Smultron version 3.1, 2007-05-19
Written by Peter Borg, pgw3@mac.com
Find the latest version at http://smultron.sourceforge.net

Copyright 2004-2007 Peter Borg

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
*/

#import <Cocoa/Cocoa.h>
//#import "SMLStandardHeader.h"
#import <Carbon/Carbon.h>

#import <SyncServices/SyncServices.h>

#import <SystemConfiguration/SCNetwork.h>

#import <ApplicationServices/ApplicationServices.h>
#import "SCTextView.h"

typedef enum {
	SMLCurrentDocumentScope = 0,
	SMLCurrentProjectScope,
	SMLAllDocumentsScope
} SMLAdvancedFindScope;

#define OKBUTTON NSLocalizedString(@"OK", @"OK-button")
#define DELETEBUTTON NSLocalizedString(@"Delete", @"Delete-button")

@interface SMLAdvancedFindController : NSObject
{
    IBOutlet NSWindow *advancedFindWindow;
    IBOutlet NSSearchField *findSearchField;
    IBOutlet NSSearchField *replaceSearchField;
    IBOutlet NSTextField *findResultTextField;
    IBOutlet NSOutlineView *findResultsOutlineView;
//	IBOutlet NSView *resultDocumentContentView;
	IBOutlet NSTreeController *findResultsTreeController;

	IBOutlet NSSplitView *advancedFindSplitView;

	IBOutlet NSButton *currentDocumentScope;
	IBOutlet NSButton *currentProjectScope;
	IBOutlet NSButton *allDocumentsScope;

	IBOutlet NSTextView *mTextView;
	IBOutlet NSButton *setUseRegularExpressionsButton;
	IBOutlet NSButton *setSearchInSelectionButton;
	IBOutlet NSButton *setIgnoreCaseButton;

	IBOutlet NSScrollView * mScrollView;

	id currentlyDisplayedDocumentInAdvancedFind;

	NSMutableArray *mContents; //search results

	IBOutlet NSPanel *regularExpressionsHelpPanel;

	SMLAdvancedFindScope mSearchScope;
	NSDocument *mCurrentDocument;

	NSWindowController * myWindowController;
}

+ (SMLAdvancedFindController *)sharedInstance;

- (IBAction)findAction:(id)sender;
- (IBAction)replaceAction:(id)sender;

- (void)performNumberOfReplaces:(NSInteger)numberOfReplaces;

- (void)showAdvancedFindWindow;

- (NSEnumerator *)scopeEnumerator;

- (id)currentlyDisplayedDocumentInAdvancedFind;
- (void)removeCurrentlyDisplayedDocumentInAdvancedFind;

//- (NSView *)resultDocumentContentView;

//- (NSManagedObjectContext *)managedObjectContext;

- (NSMutableDictionary *)preparedResultDictionaryFromString:(NSString *)completeString searchStringLength:(NSInteger)searchStringLength range:(NSRange)foundRange lineNumber:(NSInteger)lineNumber document:(id)document;

- (void)alertThatThisIsNotAValidRegularExpression:(NSString *)string;

- (IBAction)searchScopeChanged:(id)sender;

- (NSWindow *)advancedFindWindow;
- (NSOutlineView *)findResultsOutlineView;

- (IBAction)showRegularExpressionsHelpPanelAction:(id)sender;

- (NSMutableArray*) contents;

- (void) setContents: (NSArray *)newContents;

- (void)insertDocumentIntoFourthContentView:(id)document selectRange: (NSRange) selectRange;
- (void*) getSCObject;

- (void)showRegularExpressionsHelpPanel;


@end
