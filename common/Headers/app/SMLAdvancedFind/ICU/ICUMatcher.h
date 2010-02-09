//
//  ICUMatcher.h
//  CocoaICU
//
//  Created by Aaron Evans on 11/29/06.
//  Copyright 2006 Aaron Evans. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class ICUPattern;

/*!
    @class		 ICUMatcher
    @abstract    ICUMatcher provides matching functionality for regular expression matching.
    @discussion  This class is based off of the <a href="http://icu.sourceforge.net/apiref/icu4c/classRegexMatcher.html">C++ ICU RegexMatcher class</a>.  For examples of how to use the matcher, see the NSString category included in this project.
*/
@interface ICUMatcher : NSObject {
	ICUPattern *pattern;
}

/*!
    @method     matcherWithPattern:overString:
    @abstract   Returns a matcher that can match the given pattern over the given string.
    @discussion
*/
+(ICUMatcher *)matcherWithPattern:(ICUPattern *)p overString:(NSString *)stringToSearchOver;

/*!
    @method     initWithPattern:overString:
    @abstract   Initializes a matcher that can match the given patter over the given string.
    @discussion
*/
-(ICUMatcher *)initWithPattern:(ICUPattern *)p overString:(NSString *)stringToSearchOver;

/*!
    @method     findNext
    @abstract   Finds the next occurrence of the pattern in the input string.
    @discussion Use <code>group</code> and <code>rangeOfMatch</code> to extract the match.
*/
-(BOOL)findNext;

/*!
    @method     findFromIndex:
    @abstract   Resets the pattern and performs a match from the specified index.
    @discussion Use <code>group</code> and <code>rangeOfMatch</code> to extract the match.
*/
-(BOOL)findFromIndex:(unsigned)index;

/*!
    @method     group
    @abstract   Returns the current match.
    @discussion Each match has one or more subexpressions associated with the match.  This returns the entire match.
*/
-(NSString *)group;

/*!
    @method     groupAtIndex:
    @abstract   Returns the given subexpression for the current match.
    @discussion <code>group</code> is equivalent to <code>groupAtIndex:0</code>.  The subexpressions for a match are indexed from 1.
*/
-(NSString *)groupAtIndex:(unsigned)groupIndex;

/*!
    @method     numberOfGroups
    @abstract   Returns the number of groups for the current match.
    @discussion Group 0 is the entire match and groups 1..n represent the groups for the subexpressions.
*/
-(unsigned)numberOfGroups;

/*!
    @method     lookingAt:
    @abstract   Returns true if the pattern matches some prefix of the input string starting at the specified index.
    @discussion This method returns YES when some prefix of the substring matches the input string.
*/
-(BOOL)lookingAt:(unsigned)index;

/*!
    @method     pattern
    @abstract   Returns the pattern for this matcher.
    @discussion
*/
-(ICUPattern *)pattern;
-(void)setPattern:(ICUPattern *)p;

/*!
    @method     matches
    @abstract   Returns YES if the patterns matches the <b>entire</b> input string.
    @discussion
*/
-(BOOL)matches;

/*!
    @method     replaceAllWithString:
    @abstract   Replaces all occurrences of the pattern with the replacement string and returns the resulting string.
    @discussion The replacement string can contain references to capture groups taking the form or $1, $2, etc.
*/
-(NSString *)replaceAllWithString:(NSString *)aReplacementString;

/*!
    @method     replaceFirstWithString:
    @abstract   Replaces the first occurrence of the pattern with the given replacement string and returns the resulting string.
    @discussion The replacement string can contain references to capture groups taking the form or $1, $2, etc.
*/
-(NSString *)replaceFirstWithString:(NSString *)aReplacementString;

/*!
    @method     reset
    @abstract   Resets any state associated with the matcher and its pattern.
    @discussion
*/
-(void)reset;

/*!
    @method     rangeOfMatch
    @abstract   Returns the range of the input string that corresponds to the current match.
    @discussion
*/
-(NSRange)rangeOfMatch;

/*!
    @method     rangeOfMatchGroup:
    @abstract   Returns the range of the input string that corresponds to the specified capture group of the current match.
    @discussion
*/
-(NSRange)rangeOfMatchGroup:(unsigned)groupNumber;

@end
