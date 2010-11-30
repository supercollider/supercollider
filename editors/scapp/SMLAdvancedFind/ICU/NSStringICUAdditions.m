//
//  NSStringICUAdditions.m
//  CocoaICU
//
//  Created by Aaron Evans on 11/19/06.
//  Copyright 2006 Aaron Evans. All rights reserved.
//

#import "NSStringICUAdditions.h"
#import "ICUPattern.h"
#import "ICUMatcher.h"

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

@interface NSString (NSStringICUPrivateAdditions)

@end

@implementation NSString (NSStringICUAdditions)//

-(NSString *)replaceOccurrencesOfPattern:(NSString *)aPattern withString:(NSString *)replacementText {
	ICUPattern *p = [ICUPattern patternWithString:aPattern];
	ICUMatcher *m = [ICUMatcher matcherWithPattern:p overString:self];
	return [m replaceAllWithString:replacementText];
}

-(BOOL)matchesPattern:(NSString *)aRegex {
	ICUPattern *p = [ICUPattern patternWithString:aRegex];
	ICUMatcher *m = [ICUMatcher matcherWithPattern:p overString:self];
	return [m matches];
}

-(NSArray *)findPattern:(NSString *)aRegex {
	ICUPattern *p = [ICUPattern patternWithString:aRegex];
	ICUMatcher *matcher = [ICUMatcher matcherWithPattern:p overString:self];
	NSMutableArray *foundGroups = [NSMutableArray array];
	
	[matcher findFromIndex:0];	
	int i;
	for(i=0;i<=[matcher numberOfGroups];i++)
		[foundGroups addObject:[matcher groupAtIndex:i]];

	return [NSArray arrayWithArray:foundGroups];
}

-(NSArray *)componentsSeparatedByPattern:(NSString *)aRegex {
	ICUPattern *p = [ICUPattern patternWithString:aRegex];
	return [p componentsSplitFromString:self];
}

+(NSString *)stringWithICUString:(void *)utf16EncodedString {
	return [[[NSString alloc] initWithBytes:utf16EncodedString 
									 length:u_strlen(utf16EncodedString)*sizeof(UChar) 
								   encoding:[self nativeUTF16Encoding]] autorelease];	
}

+(NSStringEncoding)nativeUTF16Encoding {
	CFStringEncoding stringEncoding;
#if __BIG_ENDIAN__
	stringEncoding = kCFStringEncodingUTF16BE;
#elif __LITTLE_ENDIAN__
	stringEncoding = kCFStringEncodingUTF16LE;
#endif

	return CFStringConvertEncodingToNSStringEncoding(stringEncoding);
}

-(void *)UTF16String {
	UChar *ret = (UChar *)[self cStringUsingEncoding:[NSString nativeUTF16Encoding]];

	// for some reason, the null-terminator doesn't always show up at the right place and this
	// causes extra characters to be created in the unicode string.  We remove them here by force.
	int len = [self length];
	ret[len] = '\0';
	return ret;	
}

@end
