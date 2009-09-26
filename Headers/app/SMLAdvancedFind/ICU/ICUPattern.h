//
//  ICUPattern.h
//  CocoaICU
//
//  Created by Aaron Evans on 11/19/06.
//  Copyright 2006 Aaron Evans. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ICUMatcher;

/**
 * flags. see http://icu.sourceforge.net/apiref/icu4c/uregex_8h.html#874989dfec4cbeb6baf4d1a51cb529ae
 */
extern unsigned CaseInsensitiveMatching;
extern unsigned Comments;
extern unsigned DotMatchesAll;
extern unsigned Multiline;
extern unsigned UnicodeWordBoundaries;

/*!
    @class		 ICUPattern
    @abstract    A compiled regular expression.
    @discussion  <a href="http://icu.sourceforge.net/">ICU</a> provides a widely used Unicode
 regular expression library.  This class can be roughly mapped to the <a href="http://icu.sourceforge.net/apiref/icu4c/classRegexPattern.html">ICU C++ Pattern class</a>.  Notes about ICU regular expressions can be found at the
 <a href="http://icu.sourceforge.net/userguide/regexp.html">ICU User Guide</a>.
*/
@interface ICUPattern: NSObject <NSCopying> {
	void *re;
	void *textToSearch;
	unsigned flags;
	NSString *stringToSearch;
}

/*!
    @method     patternWithString:flags:
    @abstract   Returns an autoreleased pattern with the specified flags set.
    @discussion Flags are defined as the OR of the constants defined in the class.
*/
+(ICUPattern *)patternWithString:(NSString *)aPattern flags:(unsigned)flags;

/*!
	@method     patternWithString:
	@abstract   Returns an autoreleased pattern with the default flags.
	@discussion Flags are given by 0.
*/
+(ICUPattern *)patternWithString:(NSString *)aPattern;

/*!
	@method     initWithString:flags:
	@abstract   Returns a pattern with the specified flags set.
	@discussion Flags are defined as the OR of the constants defined in the class.
*/
-(id)initWithString:(NSString *)aPattern flags:(unsigned)flags;

/*!
    @method     initWithString:
	@abstract   Returns a pattern with the specified flags set.
	@discussion Flags are defined as 0.
*/
-(id)initWithString:(NSString *)aPattern;

/*!
    @method		componentsSplitFromString:
    @abstract   Splits the input string into fields delineated by the expression given by the pattern.
    @discussion Returns an empty array if the pattern is not found.
*/
-(NSArray *)componentsSplitFromString:(NSString *)stringToSplit;

/*!
    @method     matchesString:
    @abstract   Returns YES if the pattern matches the entire input string.
    @discussion Returns YES if the pattern matches the entire input string.
*/
-(BOOL)matchesString:(NSString *)stringToMatchAgainst;

/*!
    @method     pattern
    @abstract   Returns the string representing the regular expression of this pattern.
    @discussion Returns the string representing the regular expression of this pattern.
*/
-(NSString *)pattern;

/*!
    @method     setStringToSearch:
    @abstract   Sets the string that is being searched by this pattern.
    @discussion This method also resets any internal state of the pattern.
*/
-(void)setStringToSearch:(NSString *)aStringToSearchOver;

/*!
    @method     stringToSearch
    @abstract   Returns the string that is being searched over with the pattern.
    @discussion This method creates an NSString from the underlying UTF16 character array used by ICU.  For large
 strings, this may be memory-intensive/time-consuming.  Performance-critical applications may want to modify this
 class to suite their needs to reduce the number of NSString objects that are created.
*/
-(NSString *)stringToSearch;

/*!
    @method     reset
    @abstract   Resets the pattern.
    @discussion Patterns maintain state about the current match and this method resets that state.
*/
-(void)reset;

/*!
    @method     re
    @abstract   Returns the compiled ICU URegularExpression for this pattern.
    @discussion This method primarily exists to allow the ICUMatcher to access the pattern.
*/
-(void *)re;

@end
