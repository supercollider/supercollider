//
//  ICUMatcher.m
//  CocoaICU
//
//  Created by Aaron Evans on 11/29/06.
//  Copyright 2006 Aaron Evans. All rights reserved.
//

#import "ICUMatcher.h"
#import "NSStringICUAdditions.h"
#import "ICUPattern.h"

size_t InitialGroupSize = 128;

struct URegularExpression;
/**
* Structure represeting a compiled regular rexpression, plus the results
 *    of a match operation.
 * @draft ICU 3.0
 */
typedef struct URegularExpression URegularExpression;

#define U_HIDE_DRAFT_API 1
#define U_DISABLE_RENAMING 1

#import "uregex.h"
#import "ustring.h"
#import "udraft.h"

#define CheckStatus(status) 	if(U_FAILURE(status)) { [NSException raise:@"Find Exception" format:@"%s", u_errorName(status)]; }


@interface ICUMatcher (Private)
-(NSString *)performReplacementWithString:(NSString *)aReplacementString replaceAll:(BOOL)replacingAll;
@end

@implementation ICUMatcher

+(ICUMatcher *)matcherWithPattern:(ICUPattern *)p overString:(NSString *)stringToSearchOver {
	return [[[[ICUMatcher class] alloc] initWithPattern:p overString:stringToSearchOver] autorelease];
}

-(ICUMatcher *)initWithPattern:(ICUPattern *)p overString:(NSString *)aStringToSearch; {
	if(![super init])
		return nil;

	[self setPattern:p];
	[[self pattern] setStringToSearch:aStringToSearch];

	return self;
}

-(void)dealloc {
	[[self pattern] release];

	[super dealloc];
}

-(void)setPattern:(ICUPattern *)p {
	if([self pattern] != nil)
		[[self pattern] release];

	pattern = [p retain];
}

-(BOOL)matches {
	URegularExpression *re = [[self pattern] re];
	
	UErrorCode status = 0;
	BOOL matches = uregex_matches(re, 0, &status);

	return matches;
}


/*
 Find the first matching substring of the input string that matches the pattern.
 
 The search for a match begins at the specified index. If a match is found, uregex_start(), uregex_end(), and uregex_group() will provide more information regarding the match.
 */
-(BOOL)findNext {
	URegularExpression *re = [[self pattern] re];
	UErrorCode status = 0;
	UBool r = uregex_findNext(re, &status);

	return r;
}

-(BOOL)findFromIndex:(unsigned)index {
	URegularExpression *re = [[self pattern] re];
	[self reset];

	UErrorCode status = 0;
	UBool r = uregex_find(re, index, &status);
	CheckStatus(status);

	return r;
}

-(NSString *)group {
	NSString *stringToMatch = [[self pattern] stringToSearch];
	return [stringToMatch substringWithRange:[self rangeOfMatch]];
}

-(NSString *)groupAtIndex:(unsigned)groupIndex {
	size_t groupSize = InitialGroupSize;
	URegularExpression *re = [[self pattern] re];

	while(YES) { 
		UErrorCode status = 0;
		UChar *dest = calloc(groupSize, sizeof(UChar));
		int32_t buffSize = uregex_group(re, groupIndex, dest, groupSize, &status);

		if(U_BUFFER_OVERFLOW_ERROR == status) {
			groupSize *= 2;
			free(dest);
			continue;
		}

		CheckStatus(status);

		groupSize = InitialGroupSize; // reset to default
		NSString *result = [[[NSString alloc] initWithBytes:dest length:buffSize*sizeof(UChar) encoding:[NSString nativeUTF16Encoding]] autorelease];
		free(dest);
		return result;
	}
}

-(unsigned)numberOfGroups {
	URegularExpression *re = [[self pattern] re];
	UErrorCode status = 0;
	int numberOfGroups = uregex_groupCount(re, &status);
	CheckStatus(status);
	
	return numberOfGroups;
}

-(BOOL)lookingAt:(unsigned)index {
	UErrorCode status = 0;
	URegularExpression *re = [[self pattern] re];
	BOOL matches = uregex_lookingAt(re, 0, &status);
	CheckStatus(status);
	return matches;
}

-(ICUPattern *)pattern {
	return pattern;
}

-(NSString *)performReplacementWithString:(NSString *)aReplacementString replaceAll:(BOOL)replacingAll {
	UErrorCode status = 0;
	UChar *replacementText = [aReplacementString UTF16String];
	URegularExpression *re = [[self pattern] re];
	int searchTextLength = [[[self pattern] stringToSearch] length];
	
	BOOL replacementCompleted = NO;
	int resultLength = 0;
	size_t destStringBufferSize = searchTextLength * 2;
	UChar *destString = NULL;
	while(!replacementCompleted) {
		
		destString = calloc(destStringBufferSize, sizeof(UChar));
		if(replacingAll)
			resultLength = uregex_replaceAll(re, replacementText, -1, destString, destStringBufferSize, &status);
		else
			resultLength = uregex_replaceFirst(re, replacementText, -1, destString, destStringBufferSize, &status);

		if(status == U_BUFFER_OVERFLOW_ERROR) {
			status = 0;
			free(destString);
			destStringBufferSize *= 2;
		}
		else if(U_FAILURE(status)) {
			free(destString);
			[NSException raise:@"Find Exception"
						format:@"Could not perform find and replace: %s", u_errorName(status)];
		} else {
			replacementCompleted = YES;
		}
	}
	
	NSString *result = [[[NSString alloc] initWithBytes:destString
												 length:resultLength * sizeof(UChar)
											   encoding:[NSString nativeUTF16Encoding]] autorelease];
	free(destString);
	return result;	
}


-(NSString *)replaceAllWithString:(NSString *)aReplacementString {
	return [self performReplacementWithString:aReplacementString replaceAll:YES];
}

-(NSString *)replaceFirstWithString:(NSString *)aReplacementString {
	return [self performReplacementWithString:aReplacementString replaceAll:NO];
}

-(void)reset {
	[[self pattern] reset];
}

-(NSRange)rangeOfMatch {
	return [self rangeOfMatchGroup:0];
}

-(NSRange)rangeOfMatchGroup:(unsigned)groupNumber {
	UErrorCode status = 0;
	URegularExpression *re = [[self pattern] re];
	int start = uregex_start(re, groupNumber, &status);
	
	int end = uregex_end(re, groupNumber, &status);
	return NSMakeRange(start, end-start);
}

@end
