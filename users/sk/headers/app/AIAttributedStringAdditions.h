/*-------------------------------------------------------------------------------------------------------*\
| Adium, Copyright (C) 2001-2003, Adam Iser  (adamiser@mac.com | http://www.adiumx.com)                   |
\---------------------------------------------------------------------------------------------------------/
 | This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 | General Public License as published by the Free Software Foundation; either version 2 of the License,
 | or (at your option) any later version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 | the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 | Public License for more details.
 |
 | You should have received a copy of the GNU General Public License along with this program; if not,
 | write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 \------------------------------------------------------------------------------------------------------ */

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

@interface NSMutableAttributedString (AIAttributedStringAdditions)

- (void)appendString:(NSString *)aString withAttributes:(NSDictionary *)attrs;
- (NSData *)dataRepresentation;
- (NSAttributedString *)safeString;
- (unsigned int)replaceOccurrencesOfString:(NSString *)target withString:(NSString*)replacement options:(unsigned)opts range:(NSRange)searchRange;
- (unsigned int)replaceOccurrencesOfString:(NSString *)target withString:(NSString*)replacement attributes:(NSDictionary*)attributes options:(unsigned)opts range:(NSRange)searchRange;
- (void)adjustColorsToShowOnBackground:(NSColor *)backgroundColor;
- (void)adjustColorsToShowOnBackgroundRelativeToOriginalBackground:(NSColor *)backgroundColor;
@end

@interface NSAttributedString (AIAttributedStringAdditions)

- (float)heightWithWidth:(float)width;
- (NSData *)dataRepresentation;
+ (NSAttributedString *)stringWithData:(NSData *)inData;
- (NSAttributedString *)safeString;

@end


