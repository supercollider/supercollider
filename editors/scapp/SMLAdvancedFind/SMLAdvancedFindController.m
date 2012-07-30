/*
Smultron version 3.1, 2007-05-19
Written by Peter Borg, pgw3@mac.com
Find the latest version at http://smultron.sourceforge.net

Copyright 2004-2007 Peter Borg
Adapted for SuperCollider by Jan Trutzschler
 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
*/
#define U_HIDE_DRAFT_API 1
#import "SMLAdvancedFindController.h"
#define __LP64__ false
//#define SMLCurrentDocument [[NSApp mainWindow] firstResponder]
#import "ICUPattern.h"
#import "ICUMatcher.h"
#import "SCTextView.h"
#import "SMLStatusBarTextFieldCell.h"


@implementation SMLAdvancedFindController

static id sharedInstance = nil;

+ (SMLAdvancedFindController *)sharedInstance
{ 
	if (sharedInstance == nil) { 
		sharedInstance = [[self alloc] init];
	}
	
	return sharedInstance;
} 


- (id)init 
{
    if (sharedInstance != nil) {
        [self dealloc];
    } else {
        sharedInstance = [super init];
		mContents = [[NSMutableArray alloc] init]; 
    }
    return sharedInstance;
}

- (NSMutableArray*) contents
{
	return mContents;
}

- (void) setContents: (NSArray *)newContents 
{
if (mContents != newContents) 
    { 
        [mContents autorelease]; 
        mContents = [[NSMutableArray alloc] initWithArray:newContents]; 
    } 
} 

- (IBAction)findAction:(id)sender
{
	NSString *searchString = [findSearchField stringValue];
	BOOL useRegularExpressions = (BOOL) [setUseRegularExpressionsButton state];
	BOOL ignoreCaseAdvancedFind = (BOOL) [setIgnoreCaseButton state]; 
	BOOL onlyInSelectionAdvancedFind = (BOOL) [setSearchInSelectionButton state]; 
	
	[findResultsOutlineView setDelegate:nil];
	
	[findResultsTreeController setContent:nil];
	[findResultsTreeController setContent:[NSArray array]];
	[mContents autorelease];
	mContents = [[NSMutableArray alloc] init]; 
	
	
	NSMutableArray *recentSearches = [[[NSMutableArray alloc] initWithArray:[findSearchField recentSearches]] autorelease];
	if ([recentSearches indexOfObject:searchString] != NSNotFound) {
		[recentSearches removeObject:searchString];
	}
	[recentSearches insertObject:searchString atIndex:0];
	if ([recentSearches count] > 15) {
		[recentSearches removeLastObject];
	}
	[findSearchField setRecentSearches:recentSearches];
	
	NSInteger searchStringLength = [searchString length];
	if (!searchStringLength > 0 ) {
//		NSBeep();
		return;
	}

	NSString *completeString;
	NSInteger completeStringLength; 
	NSInteger startLocation;
	NSInteger resultsInThisDocument = 0;
	NSInteger lineNumber;
	NSInteger index;
	NSInteger numberOfResults = 0;
	NSRange foundRange;
	NSRange searchRange;
	NSIndexPath *folderIndexPath;
	NSMutableDictionary *node;
	
	NSEnumerator *enumerator = [self scopeEnumerator];
	id document;
	NSInteger documentIndex = 0;

	while (document = [enumerator nextObject]) {
		node = [NSMutableDictionary dictionary];
//		if ([[SMLDefaults valueForKey:@"ShowFullPathInWindowTitle"] boolValue] == YES) {
//			[node setValue:[document valueForKey:@"nameWithPath"] forKey:@"displayString"];
//		} else {
			[node setValue:[document displayName] forKey:@"displayString"];
//		}

		[node setValue:[NSNumber numberWithBool:NO] forKey:@"isLeaf"];
		
		folderIndexPath = [[[NSIndexPath alloc] initWithIndex:documentIndex] autorelease];
		[findResultsTreeController insertObject:node atArrangedObjectIndexPath:folderIndexPath];
		documentIndex++;

		completeString = [[document textView] string];
		searchRange = [[document textView] selectedRange];
		completeStringLength = [completeString length];
		
		if (onlyInSelectionAdvancedFind == NO || searchRange.length == 0) {
			searchRange = NSMakeRange(0, completeStringLength);
		}
		startLocation = searchRange.location;
		resultsInThisDocument = 0;

		if (useRegularExpressions) {
			ICUPattern *pattern;
			@try {
				if (ignoreCaseAdvancedFind) {
					pattern = [[ICUPattern alloc] initWithString:searchString flags:(CaseInsensitiveMatching | Multiline)];
				} else {
					pattern = [[ICUPattern alloc] initWithString:searchString flags:Multiline];
				}
			}
			@catch (NSException *exception) {
				[self alertThatThisIsNotAValidRegularExpression:searchString];
				return;
			}
			@finally {
			}
			
			if ([completeString length] > 0) { // Otherwise ICU throws an exception
				ICUMatcher *matcher;
				if (onlyInSelectionAdvancedFind == NO || searchRange.length == 0) {
					matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:completeString] autorelease];
				} else {
					matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:[completeString substringWithRange:searchRange]] autorelease];
				}
				
				NSInteger indexTemp;
				while ([matcher findNext]) {
					NSInteger foundLocation = [matcher rangeOfMatch].location + startLocation;
					for (index = 0, lineNumber = 0; index <= foundLocation; lineNumber++) {
						indexTemp = index;
						index = NSMaxRange([completeString lineRangeForRange:NSMakeRange(index, 0)]);
						if (indexTemp == index) {
							index++; // Make sure it moves forward if it is stuck when searching e.g. for [ \t\n]*
						}
					}
					
					NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
					NSRange rangeMatch = NSMakeRange([matcher rangeOfMatch].location + searchRange.location, [matcher rangeOfMatch].length);
					[findResultsTreeController insertObject:[self preparedResultDictionaryFromString:completeString searchStringLength:searchStringLength range:rangeMatch lineNumber:lineNumber document:document] atArrangedObjectIndexPath:[folderIndexPath indexPathByAddingIndex:resultsInThisDocument]];
					[pool release];
					
					resultsInThisDocument++;
				}
			}
			
		} else {			
			while (startLocation < completeStringLength) {
				if (ignoreCaseAdvancedFind) {
					foundRange = [completeString rangeOfString:searchString options:NSCaseInsensitiveSearch range:NSMakeRange(startLocation, NSMaxRange(searchRange) - startLocation)];
				} else {
					foundRange = [completeString rangeOfString:searchString options:NSLiteralSearch range:NSMakeRange(startLocation, NSMaxRange(searchRange) - startLocation)];
				}

				if (foundRange.location == NSNotFound) {
					break;
				}
				for (index = 0, lineNumber = 0; index <= foundRange.location; lineNumber++) {
					index = NSMaxRange([completeString lineRangeForRange:NSMakeRange(index, 0)]);	
				}
			
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
//				NSLog(@"found range: %i, lineNumber: %i", foundRange.length, lineNumber); 
				[findResultsTreeController insertObject:[self preparedResultDictionaryFromString:completeString searchStringLength:searchStringLength range:foundRange lineNumber:lineNumber document:document] atArrangedObjectIndexPath:[folderIndexPath indexPathByAddingIndex:resultsInThisDocument]];


				[pool release];
				
				resultsInThisDocument++;
				startLocation = NSMaxRange(foundRange);
			}
		}
		if (resultsInThisDocument == 0) {
			[findResultsTreeController removeObjectAtArrangedObjectIndexPath:folderIndexPath];
			documentIndex--;
		} else {
			numberOfResults += resultsInThisDocument;
		}
			
	}
	
	NSString *searchResultString;
	if (numberOfResults == 0) {
		searchResultString = [NSString stringWithFormat:NSLocalizedString(@"Could not find a match for search-string %@", @"Could not find a match for search-string %@ in Advanced Find"), searchString];
	} else if (numberOfResults == 1) {
		searchResultString = [NSString stringWithFormat:NSLocalizedString(@"Found one match for search-string %@", @"Found one match for search-string %@ in Advanced Find"), searchString];
	} else {
		searchResultString = [NSString stringWithFormat:NSLocalizedString(@"Found %i matches for search-string %@", @"Found %i matches for search-string %@ in Advanced Find"), numberOfResults, searchString];
	}
//	NSLog(searchResultString);
	[findResultTextField setStringValue:searchResultString];
	
	[findResultsOutlineView setDelegate:self];
}


- (IBAction)replaceAction:(id)sender
{	
	NSString *searchString = [findSearchField stringValue];
	NSString *replaceString = [replaceSearchField stringValue];
	BOOL useRegularExpressions = (BOOL) [setUseRegularExpressionsButton state];
	BOOL ignoreCaseAdvancedFind = (BOOL) [setIgnoreCaseButton state]; 	
	BOOL onlyInSelectionAdvancedFind = (BOOL) [setSearchInSelectionButton state]; 
	
	NSMutableArray *recentSearches = [[[NSMutableArray alloc] initWithArray:[findSearchField recentSearches]] autorelease];
	if ([recentSearches indexOfObject:searchString] != NSNotFound) {
		[recentSearches removeObject:searchString];
	}
	[recentSearches insertObject:searchString atIndex:0];
	if ([recentSearches count] > 15) {
		[recentSearches removeLastObject];
	}
	[findSearchField setRecentSearches:recentSearches];
	
	NSMutableArray *recentReplaces = [[[NSMutableArray alloc] initWithArray:[replaceSearchField recentSearches]] autorelease];
	if ([recentReplaces indexOfObject:replaceString] != NSNotFound) {
		[recentReplaces removeObject:replaceString];
	}
	[recentReplaces insertObject:replaceString atIndex:0];
	if ([recentReplaces count] > 15) {
		[recentReplaces removeLastObject];
	}
	[replaceSearchField setRecentSearches:recentReplaces];
	
//	 [[NSApp mainWindow] firstResponder];
//	if (!searchStringLength > 0 || SMLCurrentDocument == nil || SMLCurrentProject == nil) {
//		NSBeep();
//		return;
//	}
	
	NSString *completeString;
	NSInteger completeStringLength; 
	NSInteger startLocation;
	NSInteger resultsInThisDocument = 0;
	NSInteger numberOfResults = 0;
	NSRange foundRange;
	NSRange searchRange;
	
	NSEnumerator *enumerator = [self scopeEnumerator];
	id document;
	
	while (document = [enumerator nextObject]) {
		completeString = [[document textView] string];
		searchRange = [[document textView] selectedRange];
		completeStringLength = [completeString length];
//		if(searchRange.length == 0) searchRange = 	NSMakeRange(0, completeStringLength);
		
		if (onlyInSelectionAdvancedFind == NO || searchRange.length == 0) {
			searchRange = NSMakeRange(0, completeStringLength);
		}
		
		startLocation = searchRange.location;
		resultsInThisDocument = 0;
		if (useRegularExpressions) {
			ICUPattern *pattern;
			@try { 
				if (ignoreCaseAdvancedFind) {
					pattern = [[[ICUPattern alloc] initWithString:searchString flags:(CaseInsensitiveMatching | Multiline)] autorelease];
				} else {
					pattern = [[[ICUPattern alloc] initWithString:searchString flags:Multiline] autorelease];
				}
			}
			@catch (NSException *exception) {
				[self alertThatThisIsNotAValidRegularExpression:searchString];
				return;
			}
			@finally {
			}
			
			ICUMatcher *matcher;
			if (onlyInSelectionAdvancedFind == NO || searchRange.length == 0) {
				matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:completeString] autorelease];
			} else {
				matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:[completeString substringWithRange:searchRange]] autorelease];
			}
			

			while ([matcher findNext]) {
				resultsInThisDocument++;
			}
	
			
		} else {
			NSInteger searchLength;

			if (onlyInSelectionAdvancedFind == NO || searchRange.length == 0) {
				searchLength = completeStringLength;
			} else {
				searchLength = NSMaxRange(searchRange);
			}
			while (startLocation < searchLength) {

				if (ignoreCaseAdvancedFind) {
					foundRange = [completeString rangeOfString:searchString options:NSCaseInsensitiveSearch range:NSMakeRange(startLocation, NSMaxRange(searchRange) - startLocation)];
				} else {
					foundRange = [completeString rangeOfString:searchString options:NSLiteralSearch range:NSMakeRange(startLocation, NSMaxRange(searchRange) - startLocation)];
				}
				
				if (foundRange.location == NSNotFound) {
					break;
				}
				resultsInThisDocument++;
				startLocation = NSMaxRange(foundRange);
			}
	
		}
		numberOfResults += resultsInThisDocument;
	}
	
	if (numberOfResults == 0) {
		[findResultTextField setObjectValue:[NSString stringWithFormat:NSLocalizedString(@"Could not find a match for search-string %@", @"Could not find a match for search-string %@ in Advanced Find"), searchString]];
		NSBeep();
		return;
	}
	#if 0

	if ([[SMLDefaults valueForKey:@"SuppressReplaceWarning"] boolValue] == YES) {
		[self performNumberOfReplaces:numberOfResults];
	} else {
	#endif
		NSString *title;
		NSString *defaultButton;
		if ([replaceString length] > 0) {
			if (numberOfResults != 1) {
				title = [NSString stringWithFormat:NSLocalizedString(@"Are you sure that you want to replace %i occurrences of %@ with %@?", @"Ask if you are sure that you want to replace %i occurrences of %@ with %@ in ask-if-sure-you-want-to-replace-in-advanced-find-sheet"), numberOfResults, searchString, replaceString];
			} else {
				title = [NSString stringWithFormat:NSLocalizedString(@"Are you sure that you want to replace one occurrence of %@ with %@?", @"Ask if you are sure that you want to replace one occurrence of %@ with %@ in ask-if-sure-you-want-to-replace-in-advanced-find-sheet"), searchString, replaceString];
			}
			defaultButton = NSLocalizedString(@"Replace", @"Replace-button in ask-if-sure-you-want-to-replace-in-advanced-find-sheet");
		} else {
			if (numberOfResults != 1) {
				title = [NSString stringWithFormat:NSLocalizedString(@"Are you sure that you want to delete %i occurrences of %@?", @"Ask if you are sure that you want to delete %i occurrences of %@ in ask-if-sure-you-want-to-replace-in-advanced-find-sheet"), numberOfResults, searchString, replaceString];
			} else {
				title = [NSString stringWithFormat:NSLocalizedString(@"Are you sure that you want to delete the one occurrence of %@?", @"Ask if you are sure that you want to delete the one occurrence of %@ in ask-if-sure-you-want-to-replace-in-advanced-find-sheet"), searchString, replaceString];
			}
			defaultButton = DELETEBUTTON;
		}

		NSBeginAlertSheet(title,
						  defaultButton,
						  nil,
						  NSLocalizedString(@"Cancel", @"Cancel-button"),
						  advancedFindWindow,
						  self,
						  @selector(replaceSheetDidEnd:returnCode:contextInfo:),
						  nil,
						  (void *)numberOfResults,
						  NSLocalizedString(@"Remember that you can always Undo any changes", @"Remember that you can always Undo any changes in ask-if-sure-you-want-to-replace-in-advanced-find-sheet"));
	#if 0

	}
	#endif
}


- (void)replaceSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[sheet close];
    if (returnCode == NSAlertDefaultReturn) { // Replace
		[self performNumberOfReplaces:(NSInteger)contextInfo];
	}
}


- (void)performNumberOfReplaces:(NSInteger)numberOfReplaces
{
	NSString *searchString = [findSearchField stringValue];
	NSString *replaceString = [replaceSearchField stringValue];
	NSRange searchRange;
	NSInteger completeStringLength; 
	BOOL useRegularExpressions = (BOOL) [setUseRegularExpressionsButton state];
	BOOL ignoreCaseAdvancedFind = (BOOL) [setIgnoreCaseButton state]; 
	BOOL onlyInSelectionAdvancedFind = (BOOL) [setSearchInSelectionButton state]; 
//	NSLog(@"performNumberOfReplaces %i", numberOfReplaces);
	
	NSEnumerator *enumerator = [self scopeEnumerator];
	id document;
	while (document = [enumerator nextObject]) {
		NSTextView *textView = [document textView];
		NSString *originalString = [NSString stringWithString:[textView string]];
		NSMutableString *completeString = [NSMutableString stringWithString:[textView string]];
		
//		completeString = [[document textView] string];
		searchRange = [[document textView] selectedRange];
		completeStringLength = [completeString length];
//		if(searchRange.length == 0) searchRange = 	NSMakeRange(0, completeStringLength);
		if (!onlyInSelectionAdvancedFind || searchRange.length == 0) {
			searchRange = NSMakeRange(0, completeStringLength);
		}
		
		if (useRegularExpressions == YES) {		
			ICUPattern *pattern;
			@try {
				if (ignoreCaseAdvancedFind) {
					pattern = [[ICUPattern alloc] initWithString:searchString flags:(CaseInsensitiveMatching | Multiline)];
				} else {
					pattern = [[ICUPattern alloc] initWithString:searchString flags:Multiline];
				}
			}
			@catch (NSException *exception) {
				[self alertThatThisIsNotAValidRegularExpression:searchString];
				return;
			}
			@finally {
			}
			ICUMatcher *matcher;
			if (onlyInSelectionAdvancedFind== NO) {
				matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:completeString] autorelease];
			} else {
				matcher = [[[ICUMatcher alloc] initWithPattern:pattern overString:[completeString substringWithRange:searchRange]] autorelease];
			}

			NSMutableString *regularExpressionReplaceString = [NSMutableString stringWithString:replaceString];
			[regularExpressionReplaceString replaceOccurrencesOfString:@"\\n" withString:[NSString stringWithFormat:@"%C", 0x000A] options:NSLiteralSearch range:NSMakeRange(0, [regularExpressionReplaceString length])]; // It doesn't seem to work without this workaround
			[regularExpressionReplaceString replaceOccurrencesOfString:@"\\r" withString:[NSString stringWithFormat:@"%C", 0x000D] options:NSLiteralSearch range:NSMakeRange(0, [regularExpressionReplaceString length])];
			[regularExpressionReplaceString replaceOccurrencesOfString:@"\\t" withString:[NSString stringWithFormat:@"%C", 0x0009] options:NSLiteralSearch range:NSMakeRange(0, [regularExpressionReplaceString length])];
			
			if (onlyInSelectionAdvancedFind == NO) {
				[completeString setString:[matcher replaceAllWithString:regularExpressionReplaceString]];
			} else {
				[completeString replaceCharactersInRange:searchRange withString:[matcher replaceAllWithString:regularExpressionReplaceString]];
			}
			
		} else {

			if (ignoreCaseAdvancedFind == YES) {
				[completeString replaceOccurrencesOfString:searchString withString:replaceString options:NSCaseInsensitiveSearch range:searchRange];
			} else {
				[completeString replaceOccurrencesOfString:searchString withString:replaceString options:NSLiteralSearch range:searchRange];

			}
		}
		
		NSRange selectedRange = [textView selectedRange];
		if (![originalString isEqualToString:completeString] && [originalString length] != 0) {
			if ([textView shouldChangeTextInRange:NSMakeRange(0, [[textView string] length]) replacementString:completeString]) { // Do it this way to mark it as an Undo
				[textView replaceCharactersInRange:NSMakeRange(0, [[textView string] length]) withString:completeString];
				[textView didChangeText];
			}
		}		
		
		if (selectedRange.location <= [[textView string] length]) {
			[textView setSelectedRange:NSMakeRange(selectedRange.location, 0)];
		}
	}
	
	if (numberOfReplaces != 1) {
		[findResultTextField setObjectValue:[NSString stringWithFormat:NSLocalizedString(@"Replaced %i occurrences of %@ with %@", @"Indicate that we replaced %i occurrences of %@ with %@ in update-search-textField-after-replace"), numberOfReplaces, searchString, replaceString]];
	} else {
		[findResultTextField setObjectValue:[NSString stringWithFormat:NSLocalizedString(@"Replaced one occurrence of %@ with %@", @"Indicate that we replaced one occurrence of %@ with %@ in update-search-textField-after-replace"), searchString, replaceString]];
	}
	[findResultsTreeController setContent:nil];
	[findResultsTreeController setContent:[NSArray array]];
	[mContents autorelease]; 
	mContents = [[NSMutableArray alloc] init]; 
	
	[self removeCurrentlyDisplayedDocumentInAdvancedFind];
	[advancedFindWindow makeKeyAndOrderFront:self];
}


- (void)showAdvancedFindWindow
{
	if (advancedFindWindow == nil) {
		[NSBundle loadNibNamed:@"SCAdvancedFind.nib" owner:self];
			[advancedFindWindow makeKeyAndOrderFront:self];
			
//		mScrollView = [mTextView superview];
//		[mTextView removeFromSuperview];			
		
		SMLStatusBarTextFieldCell *statusBarTextFieldCell = [[SMLStatusBarTextFieldCell alloc] initTextCell:@""];
		[findResultTextField setCell:statusBarTextFieldCell];
		[findResultsOutlineView setBackgroundColor:[[NSColor controlAlternatingRowBackgroundColors] objectAtIndex:1]];

		mSearchScope = SMLCurrentDocumentScope;

		[allDocumentsScope setState:NSOffState];
		[currentDocumentScope setState:NSOnState]; // If the user has clicked an already clicked button make sure it is on and not turned off

		#if 0		
	
		SMLAdvancedFindScope searchScope = [[SMLDefaults valueForKey:@"AdvancedFindScope"] intValue];

		SMLAdvancedFindScope searchScope  = SMLCurrentDocumentScope;
		NSInteger distanceFromEdge = 26;
		
		[currentDocumentScope setTitle:NSLocalizedStringFromTable(@"Current document", @"Localizable3", @"Current document")];
		[currentDocumentScope sizeToFit];
		[currentDocumentScope setFrameOrigin:NSMakePoint(distanceFromEdge, [currentDocumentScope frame].origin.y)];
		[(SMLFilterBarButton *)currentDocumentScope frameDidChange:nil];		
		
		distanceFromEdge = distanceFromEdge + [currentDocumentScope bounds].size.width + 26;
		
		[currentProjectScope setTitle:NSLocalizedStringFromTable(@"Current project", @"Localizable3", @"Current project")];
		[currentProjectScope sizeToFit];
		[currentProjectScope setFrameOrigin:NSMakePoint(distanceFromEdge, [currentProjectScope frame].origin.y)];
		[(SMLFilterBarButton *)currentProjectScope frameDidChange:nil];
		
		distanceFromEdge = distanceFromEdge + [currentProjectScope bounds].size.width + 26;
		
		[allDocumentsScope setTitle:NSLocalizedStringFromTable(@"All documents", @"Localizable3", @"All documents")];
		[allDocumentsScope sizeToFit];
		[allDocumentsScope setFrameOrigin:NSMakePoint(distanceFromEdge, [allDocumentsScope frame].origin.y)];
		[(SMLFilterBarButton *)allDocumentsScope frameDidChange:nil];
		
		if (searchScope == SMLCurrentDocumentScope) {
			[currentDocumentScope setState:NSOnState];
		} else if (searchScope == SMLCurrentProjectScope) {
			[currentProjectScope setState:NSOnState];
		} else if (searchScope == SMLAllDocumentsScope) {
			[allDocumentsScope setState:NSOnState];
		}
		#endif	
		[findResultsTreeController setContent:nil];
		[findResultsTreeController setContent:[NSArray array]];
	}
	
	[advancedFindWindow makeKeyAndOrderFront:self];
}


- (void)outlineViewSelectionDidChange:(NSNotification *)aNotification
{
	if ([[findResultsTreeController arrangedObjects] count] == 0) {
		return;
	}
	id object = [[findResultsTreeController selectedObjects] objectAtIndex:0];	
	if ([[object valueForKey:@"isLeaf"] boolValue] == NO) {
		return;
	}

	id document = [object valueForKey:@"document"];
	if (document == nil) {
		NSString *title = [NSString stringWithFormat:NSLocalizedString(@"The document %@ is no longer open", @"Indicate that the document %@ is no longer open in Document-is-no-longer-opened-after-selection-in-advanced-find-sheet"), [document valueForKey:@"name"]];
		NSBeginAlertSheet(title,
						  NSLocalizedString(@"OK", @"OK-button"),
						  nil,
						  nil,
						  advancedFindWindow,
						  self,
						  nil,
						  NULL,
						  nil,
						  @"");
		return;
	}
	
	currentlyDisplayedDocumentInAdvancedFind = document;
	
#if 0
	[resultDocumentContentView addSubview:[document valueForKey:@"fourthTextScrollView"]];
	if ([[document valueForKey:@"showLineNumberGutter"] boolValue] == YES) {
		[resultDocumentContentView addSubview:[document valueForKey:@"fourthGutterScrollView"]];
	}

	[[document valueForKey:@"lineNumbers"] updateLineNumbersForClipView:[[document valueForKey:@"fourthTextScrollView"] contentView] checkWidth:YES recolour:YES]; // If the window has changed since the view was last visible
#endif	

	[self removeCurrentlyDisplayedDocumentInAdvancedFind];
		
	NSRange selectRange = NSRangeFromString([[[findResultsTreeController selectedObjects] objectAtIndex:0] valueForKey:@"range"]);
	NSString *completeString = [[document textView] string];
	if (NSMaxRange(selectRange) > [completeString length]) {
//		NSBeep();
		return;
	}
	[self insertDocumentIntoFourthContentView: document selectRange:selectRange ];

	#if 0
	if ([[SMLDefaults valueForKey:@"FocusOnTextInAdvancedFind"] boolValue] == YES) {
		[advancedFindWindow makeFirstResponder:[document valueForKey:@"fourthTextView"]];
	}
	#endif
}

- (void)insertDocumentIntoFourthContentView:(id)document selectRange: (NSRange) selectRange
{

	if(!myWindowController) myWindowController = [[NSWindowController alloc] initWithWindow: [mScrollView window]];
	if(mCurrentDocument != (NSDocument*) document || mTextView == NULL){
		mTextView = [[SCTextView alloc] initWithFrame: 
						[mScrollView bounds]];
		[mTextView setAutoresizingMask: 63];
		[[mTextView textContainer] setWidthTracksTextView: YES];
		[mTextView setAllowsUndo: YES];
		[mTextView setRichText: YES];
		[mTextView setSmartInsertDeleteEnabled: NO];
		[mTextView setImportsGraphics: YES];
		[mTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];

		[mTextView setLangClassToCall:@"Document" 
				withKeyDownActionIndex:4 withKeyUpActionIndex:5];
		[mTextView setObjectKeyDownActionIndex:2 setObjectKeyUpActionIndex:1];
	#if 1 //in order to have an SCTextView running a few methods would need to be moved from MyDocument to SCTextView
		[mTextView setDelegate: document];
		[mTextView setAcceptsFirstResponder:YES];
//		[mTextView setEditable:YES];
	#endif
		[document addWindowController: myWindowController];
		[[mTextView layoutManager] replaceTextStorage: [[document textView] textStorage]];

		[mScrollView setDocumentView: mTextView];
		[mTextView release];
		mCurrentDocument = document;
	}
	[mTextView setSelectedRange: selectRange];
    [mTextView scrollRangeToVisible: selectRange];
//	[[mTextView window] makeKeyAndOrderFront:nil];

}

// no sc - actions supported
- (void*) getSCObject 
{
	return NULL;
}

- (NSEnumerator *)scopeEnumerator 
{

	NSEnumerator *enumerator;
	if (mSearchScope == SMLAllDocumentsScope) {
		NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
		enumerator = [[docctl documents] objectEnumerator];	
	} else {
		enumerator = [[NSArray arrayWithObject:[[NSDocumentController sharedDocumentController] currentDocument]] objectEnumerator];
	}
	// no project at the moment
	return enumerator;
}


- (id)currentlyDisplayedDocumentInAdvancedFind
{
    return currentlyDisplayedDocumentInAdvancedFind; 
}


- (void)removeCurrentlyDisplayedDocumentInAdvancedFind
{
//	[SMLInterface removeAllSubviewsFromView:resultDocumentContentView];
//	NSArray *array = [NSArray arrayWithArray:[resultDocumentContentView subviews]];
//	NSEnumerator *enumerator = [array objectEnumerator];
//	id item;
//	while (item = [enumerator nextObject]) {
//		[item removeFromSuperview];
//		item = nil;
//	}
	// NSTextView superview
//	NSTextStorage *textStorage = NSTextStorage;
//		mScrollView = [mTextView superview];
//		[mTextView removeFromSuperview];
//		[[mTextView textStorage] removeLayoutManager:[mTextView layoutManager]];	
//
//		[[mTextView layoutManager] replaceTextStorage: nil];
		if(mTextView) {
			[[mTextView textStorage] removeLayoutManager:[mTextView layoutManager]];			
			[mCurrentDocument removeWindowController: myWindowController];
			[mTextView removeFromSuperview]; 
			mTextView = NULL; 
			mCurrentDocument = NULL;
			myWindowController =NULL;
			
		}
}

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	#if 0
	if ([[SMLDefaults valueForKey:@"SizeOfDocumentsListTextPopUp"] intValue] == 0) {
		[cell setFont:[NSFont systemFontOfSize:11.0]];
	} else {
	#endif
		[cell setFont:[NSFont systemFontOfSize:11.0]];
	#if 0
	}
	#endif
}	


- (NSMutableDictionary *)preparedResultDictionaryFromString:(NSString *)completeString searchStringLength:(NSInteger)searchStringLength range:(NSRange)foundRange lineNumber:(NSInteger)lineNumber document:(id)document
{
	NSMutableString *displayString = [[[NSMutableString alloc] init] autorelease];
	NSString *lineNumberString = [NSString stringWithFormat:@"%d\t", lineNumber];
	[displayString appendString:lineNumberString];
	NSRange linesRange = [completeString lineRangeForRange:foundRange];
	#if 0
	[displayString appendString:[SMLText replaceAllNewLineCharactersWithSymbolInString:[completeString substringWithRange:linesRange]]];
	#endif
	[displayString appendString:[completeString substringWithRange:linesRange]];
	
	NSMutableDictionary *node = [NSMutableDictionary dictionary];
	[node setValue:[NSNumber numberWithBool:YES] forKey:@"isLeaf"];
//	[node setValue:1 forKey:@"isLeaf"];

	[node setValue:NSStringFromRange(foundRange) forKey:@"range"]; //orginal
//	[node setValue:lineNumberString forKey:@"displayString"]; // jt

//	[node setValue:[[document objectID] URIRepresentation] forKey:@"document"];
//	[node setValue:[SMLBasic uriFromObject:document] forKey:@"document"];
	[node setValue:document forKey:@"document"];

	NSInteger fontSize;
//	if ([[SMLDefaults valueForKey:@"SizeOfDocumentsListTextPopUp"] intValue] == 0) {
//		fontSize = 11;
//	} else {
		fontSize = 11;
//	}


	NSMutableAttributedString *attributedString = [[[NSMutableAttributedString alloc] initWithString:displayString attributes:[NSDictionary dictionaryWithObject:[NSFont systemFontOfSize:fontSize] forKey:NSFontAttributeName]] autorelease];
	NSMutableParagraphStyle *style = [[[NSMutableParagraphStyle alloc] init] autorelease];
		
	[style setLineBreakMode:NSLineBreakByTruncatingMiddle];
	[attributedString addAttribute:NSParagraphStyleAttributeName value:style range:NSMakeRange(0, [displayString length])];
	[attributedString applyFontTraits:NSBoldFontMask range:NSMakeRange(foundRange.location - linesRange.location + [lineNumberString length], foundRange.length)];
	[node setValue:attributedString forKey:@"displayString"];

	return node;
}


- (void)alertThatThisIsNotAValidRegularExpression:(NSString *)string
{
	NSString *title = [NSString stringWithFormat:NSLocalizedStringFromTable(@"%@ is not a valid regular expression", @"Localizable3", @"%@ is not a valid regular expression"), string];
	NSBeginAlertSheet(title,
					  OKBUTTON,
					  nil,
					  nil,
					  advancedFindWindow,
					  self,
					  @selector(notAValidRegularExpressionSheetDidEnd:returnCode:contextInfo:),
					  nil,
					  nil,
					  @"");
}


- (void)notAValidRegularExpressionSheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	[sheet close];
	[findResultsTreeController setContent:nil];
	[findResultsTreeController setContent:[NSArray array]];
	[advancedFindWindow makeKeyAndOrderFront:nil];
}


- (IBAction)searchScopeChanged:(id)sender
{
	 mSearchScope = [sender tag];

	if (mSearchScope == SMLCurrentDocumentScope) {
//		[currentProjectScope setState:NSOffState];
		[allDocumentsScope setState:NSOffState];
		[currentDocumentScope setState:NSOnState]; // If the user has clicked an already clicked button make sure it is on and not turned off
	} else if (mSearchScope == SMLCurrentProjectScope) {
//		[currentDocumentScope setState:NSOffState];
//		[allDocumentsScope setState:NSOffState];
//		[currentProjectScope setState:NSOnState];
	} else if (mSearchScope == SMLAllDocumentsScope) {
		[currentDocumentScope setState:NSOffState];
//		[currentProjectScope setState:NSOffState];
		[allDocumentsScope setState:NSOnState];
	}
		
	if (![[findSearchField stringValue] isEqualToString:@""]) {
		[self findAction:nil];
	}	
}

- (void)showRegularExpressionsHelpPanel
{
	if (regularExpressionsHelpPanel == nil) {
		[NSBundle loadNibNamed:@"SMLRegularExpressionHelp.nib" owner:self];
	}
	
	[regularExpressionsHelpPanel makeKeyAndOrderFront:nil];
	
}

- (NSWindow *)advancedFindWindow
{
    return advancedFindWindow; 
}


- (NSOutlineView *)findResultsOutlineView
{
    return findResultsOutlineView; 
}


- (IBAction)showRegularExpressionsHelpPanelAction:(id)sender
{
	[self showRegularExpressionsHelpPanel];
}


@end
