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

/*
    A quick and simple HTML to Attributed string converter
*/
// Modified by Scott Wilson 2003

#import "AIHTMLDecoder.h"
#import "AITextAttributes.h"
#import "AIAttributedStringAdditions.h"
#import "AIColorAdditions.h"

int HTMLEquivalentForFontSize(int fontSize);

@interface AIHTMLDecoder (PRIVATE)
+ (NSDictionary *)parseArguments:(NSString *)arguments;
+ (void)processFontTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes;
+ (void)processBodyTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes;
+ (void)processLinkTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes;
@end

@implementation AIHTMLDecoder

//For compatability
+ (NSString *)encodeHTML:(NSAttributedString *)inMessage encodeFullString:(BOOL)encodeFullString
{
    if(encodeFullString){
        return([self encodeHTML:inMessage headers:YES fontTags:YES closeFontTags:YES styleTags:YES closeStyleTagsOnFontChange:YES encodeNonASCII:YES]);
    }else{
        return([self encodeHTML:inMessage headers:NO fontTags:NO closeFontTags:NO styleTags:YES closeStyleTagsOnFontChange:NO encodeNonASCII:NO]);
    }
}

// inMessage: AttributedString to encode
// headers: YES to include HTML and BODY tags
// fontTags: YES to inclued FONT tags
// closeFontTags: YES to close the font tags
// styleTags: YES to include B/I/U tags
// closeStyleTagsOnFontChange: YES to close and re-insert style tags when opening a new font tag
// encodeNonASCII: YES to encode non-ASCII characters as their HTML equivalents
+ (NSString *)encodeHTML:(NSAttributedString *)inMessage headers:(BOOL)includeHeaders fontTags:(BOOL)includeFontTags closeFontTags:(BOOL)closeFontTags styleTags:(BOOL)includeStyleTags closeStyleTagsOnFontChange:(BOOL)closeStyleTagsOnFontChange encodeNonASCII:(BOOL)encodeNonASCII
{
    NSFontManager	*fontManager = [NSFontManager sharedFontManager];
    NSRange		searchRange;
    NSColor		*pageColor = nil;
    BOOL		openFontTag = NO;

    //Setup the destination HTML string
    NSMutableString	*string = [NSMutableString stringWithString:(includeHeaders ? @"<HTML>" : @"")];

    //Setup the incoming message as a regular string, and it's length
    NSString		*inMessageString = [inMessage string];
    int			messageLength = [inMessage length];
    
    //Setup the default attributes
    /*
    NSString		*currentFamily = [@"Helvetica" retain];
    NSString		*currentColor = [@"#000000" retain];
    int			currentSize = 12;
    */
    // This is a quick hack to ensure that initial fonts are written
    NSString		*currentFamily = [@"foo" retain];
    NSString		*currentColor = [@"bar" retain];
    int			currentSize = 4000;
    
    BOOL		currentItalic = NO;
    BOOL		currentBold = NO;
    BOOL		currentUnderline = NO;
    
    //Append the body tag (If there is a background color)
    if(includeHeaders && messageLength > 0 && (pageColor = [inMessage attribute:AIBodyColorAttributeName atIndex:0 effectiveRange:nil])){
        [string appendString:@"<BODY BGCOLOR=\"#"];
        [string appendString:[pageColor hexString]];
        [string appendString:@"\">"];
    }

    //Loop through the entire string
    searchRange = NSMakeRange(0,0);
    while(searchRange.location < messageLength){
        NSDictionary	*attributes = [inMessage 
            attributesAtIndex:searchRange.location effectiveRange:&searchRange];
        NSFont		*font = [attributes objectForKey:NSFontAttributeName];
        NSString	*color = [[attributes objectForKey:NSForegroundColorAttributeName] hexString];
        NSString	*familyName = [font familyName];
        float		pointSize = [font pointSize];
        
        NSFontTraitMask	traits = [fontManager traitsOfFont:font];
        BOOL		underline = [[attributes objectForKey:NSUnderlineStyleAttributeName] intValue];
        BOOL		bold = (traits & NSBoldFontMask);
        BOOL		italic = (traits & NSItalicFontMask);
        
        NSString	*link = [attributes objectForKey:NSLinkAttributeName];

        NSMutableString	*chunk = [[inMessageString substringWithRange:searchRange] mutableCopy];

        //
        if(!color) color = @"000000";
        
        //Close style tags
        if(includeStyleTags){
            if(!italic && currentItalic) [string appendString:@"</I>"];
            if(!bold && currentBold) [string appendString:@"</B>"];
            if(!underline && currentUnderline) [string appendString:@"</U>"];
        }

        //Font (If the color, font, or size has changed)
        if(includeFontTags && ([color compare:currentColor] || pointSize != currentSize || [familyName compare:currentFamily])){
            //Close any existing font tags, and open a new one
            if(closeFontTags && openFontTag){
                [string appendString:@"</FONT>"];
            }
            openFontTag = YES;
            [string appendString:@"<FONT"];

            // Conditionals removed to ensure that font always written. SW.
            
            //Family
            //if([familyName caseInsensitiveCompare:currentFamily] != 0){
            int langNum = 0; 
        
                //(traits | NSNonStandardCharacterSetFontMask) seems to be the proper test... but it is true for all fonts!
                //NSMacOSRomanStringEncoding seems to be the encoding of all standard Roman fonts... and langNum="11" seems to make the others send properly.
                //It serves us well here.  Once non-AIM HTML is coming through, this will probably need to be an option in the function call.
            if ([font mostCompatibleStringEncoding] != NSMacOSRomanStringEncoding) {
                langNum = 11;
            }   
            
            [string appendString:[NSString stringWithFormat:@" FACE=\"%@\" LANG=\"%i\"",familyName,langNum]];

            [currentFamily release]; currentFamily = [familyName retain];
            //}

            //Size
            //if(pointSize != currentSize){
            [string appendString:[NSString stringWithFormat:@" ABSZ=\"%i\" SIZE=\"%i\"", (int)pointSize, HTMLEquivalentForFontSize((int)pointSize)]];
            currentSize = pointSize;
            //}

            //Color
            //if([color compare:currentColor] != 0){
            [string appendString:[NSString stringWithFormat:@" COLOR=\"#%@\"",color]];
            [currentColor release]; currentColor = [color retain];
            //}

            //Close the font tag
            [string appendString:@">"];
        }

        //Style (Bold, italic, underline)
        if(includeStyleTags){
            //Open style tags
            if(!currentUnderline && underline) [string appendString:@"<U>"];
            if(!currentBold && bold) [string appendString:@"<B>"];
            if(!currentItalic && italic) [string appendString:@"<I>"];

            //Set our current styles
            currentUnderline = underline;
            currentBold = bold;
            currentItalic = italic;
        }
        
        //Link
        if(link && [link length] != 0){
            [string appendString:@"<a href=\""];
            [string appendString:link];
            [string appendString:@"\">"];
        }

        //Escape special HTML characters.
        [chunk replaceOccurrencesOfString:@"&" withString:@"&amp;"
            options:NSLiteralSearch range:NSMakeRange(0, [chunk length])];
        [chunk replaceOccurrencesOfString:@"<" withString:@"&lt;"
            options:NSLiteralSearch range:NSMakeRange(0, [chunk length])];
        [chunk replaceOccurrencesOfString:@">" withString:@"&gt;"
            options:NSLiteralSearch range:NSMakeRange(0, [chunk length])];
        
        
        //Append string character by character, replacing any non-ascii characters with the designated unicode
        //escape sequence.
        int i;
        for(i = 0; i < [chunk length]; i++){
            unichar currentChar = [chunk characterAtIndex:i];
            if(encodeNonASCII && (currentChar > 127) ){
                [string appendFormat:@"&#%d;", currentChar];
            }else if(currentChar == '\r' || currentChar == '\n'){
                [string appendString:@"<BR>"];
                [string appendFormat:@"%C", currentChar]; // add returns for ease of reading
            }else{
                //unichar characters may have a length of up to 3; be careful to get the whole character
                NSRange composedCharRange = [chunk rangeOfComposedCharacterSequenceAtIndex:i];
                [string appendString:[chunk substringWithRange:composedCharRange]];
                i += composedCharRange.length - 1;
            }
        }

        //Release the chunk
        [chunk release];

        //Close Link
        if(link && [link length] != 0){
            [string appendString:@"</a>"];
        }

        //Close all open style tags
        if(includeStyleTags && closeStyleTagsOnFontChange){
            if(currentItalic) [string appendString:@"</I>"];
            if(currentBold) [string appendString:@"</B>"];
            if(currentUnderline) [string appendString:@"</U>"];
            currentItalic = NO;
            currentBold = NO;
            currentUnderline = NO;
        }

        searchRange.location += searchRange.length;
    }

    [currentFamily release];
    [currentColor release];

    //Finish off the HTML
    if(includeStyleTags && currentItalic) [string appendString:@"</I>"];
    if(includeStyleTags && currentBold) [string appendString:@"</B>"];
    if(includeStyleTags && currentUnderline) [string appendString:@"</U>"];
    if(includeFontTags && closeFontTags && openFontTag) [string appendString:@"</FONT>"];	//Close any open font tag
    if(includeHeaders && pageColor) [string appendString:@"</BODY>"];				//Close the body tag
    if(includeHeaders) [string appendString:@"</HTML>"];				//Close the HTML, etc.

    return(string);
}

int HTMLEquivalentForFontSize(int fontSize)
{
    if(fontSize <= 9){
        return(1);
    }else if(fontSize <= 10){
        return(2);
    }else if(fontSize <= 12){
        return(3);
    }else if(fontSize <= 14){
        return(4);
    }else if(fontSize <= 18){
        return(5);
    }else if(fontSize <= 24){
        return(6);
    }else{
        return(7);
    }
}

//
+ (NSAttributedString *)decodeHTML:(NSString *)inMessage
{
    NSScanner			*scanner;
    NSCharacterSet		*tagCharStart, *tagEnd, *charEnd, *absoluteTagEnd;
    NSString			*chunkString, *tagOpen;
    NSMutableAttributedString	*attrString;
    AITextAttributes		*textAttributes;
    BOOL			send = NO, receive = NO, inDiv = NO;
    
    //set up
    textAttributes = [AITextAttributes textAttributesWithFontFamily:@"Helvetica" traits:0 size:12];
    attrString = [[NSMutableAttributedString alloc] init];

    tagCharStart = [NSCharacterSet characterSetWithCharactersInString:@"<&"];
    tagEnd = [NSCharacterSet characterSetWithCharactersInString:@" >"];
    charEnd = [NSCharacterSet characterSetWithCharactersInString:@";"];
    absoluteTagEnd = [NSCharacterSet characterSetWithCharactersInString:@">"];

    scanner = [NSScanner scannerWithString:inMessage];
    [scanner setCharactersToBeSkipped:[NSCharacterSet characterSetWithCharactersInString:@""]];

    //Parse the HTML
    while(![scanner isAtEnd]){
        //Find an HTML tag or escaped character
        if([scanner scanUpToCharactersFromSet:tagCharStart intoString:&chunkString]){
            [attrString appendString:chunkString withAttributes:[textAttributes dictionary]];
        }

        //Process the tag
        if([scanner scanCharactersFromSet:tagCharStart intoString:&tagOpen]){ //If a tag wasn't found, we don't process.
            unsigned scanLocation = [scanner scanLocation]; //Remember our location (if this is an invalid tag we'll need to move back)

            if([tagOpen compare:@"<"] == 0){ // HTML <tag>
                BOOL validTag = [scanner scanUpToCharactersFromSet:tagEnd intoString:&chunkString]; //Get the tag
                if(validTag){ 
                    //HTML
                    if([chunkString caseInsensitiveCompare:@"HTML"] == 0){
                        //ignore
                    }else if([chunkString caseInsensitiveCompare:@"/HTML"] == 0){
                        //ignore

                    //PRE -- ignore attributes for logViewer
                    }else if([chunkString caseInsensitiveCompare:@"PRE"] == 0 || [chunkString caseInsensitiveCompare:@"/PRE"] == 0){

                        [scanner scanUpToCharactersFromSet:absoluteTagEnd intoString:&chunkString];
                        
                        [textAttributes setTextColor:[NSColor blackColor]];
                    //DIV
                    }else if ([chunkString caseInsensitiveCompare:@"DIV"] == 0){
                        [scanner scanUpToCharactersFromSet:absoluteTagEnd 				    intoString:&chunkString];
                        inDiv = YES;
                        if ([chunkString caseInsensitiveCompare:@" class=\"send\""] == 0) {
                            send = YES;
                            receive = NO;
                        } else if ([chunkString caseInsensitiveCompare:@" class=\"receive\""] == 0) {
                            receive = YES;
                            send = NO;
                        } else if ([chunkString caseInsensitiveCompare:@" class=\"status\""] == 0) {
                            [textAttributes setTextColor:[NSColor grayColor]];
                        }
                    }else if ([chunkString caseInsensitiveCompare:@"/DIV"] == 0) {
                        inDiv = NO;
                    //LINK
                    }else if([chunkString caseInsensitiveCompare:@"A"] == 0){
                        [textAttributes setUnderline:YES];
                        [textAttributes setTextColor:[NSColor blueColor]];
                        if([scanner scanUpToCharactersFromSet:absoluteTagEnd intoString:&chunkString]){
                            [self processLinkTagArgs:[self parseArguments:chunkString] attributes:textAttributes]; //Process the linktag's contents
                        }

                    }else if([chunkString caseInsensitiveCompare:@"/A"] == 0){
                        [textAttributes setLinkURL:nil];
                        [textAttributes setUnderline:NO];
                        [textAttributes setTextColor:[NSColor blackColor]];

                    //Body
                    }else if([chunkString caseInsensitiveCompare:@"BODY"] == 0){
                        if([scanner scanUpToCharactersFromSet:absoluteTagEnd intoString:&chunkString]){
                            [self processBodyTagArgs:[self parseArguments:chunkString] attributes:textAttributes]; //Process the font tag's contents
                        }

                    }else if([chunkString caseInsensitiveCompare:@"/BODY"] == 0){
                        //ignore

                    //Font
                    }else if([chunkString caseInsensitiveCompare:@"FONT"] == 0){
                        if([scanner scanUpToCharactersFromSet:absoluteTagEnd intoString:&chunkString]){

                            //Process the font tag if it's in a log
                            if([chunkString caseInsensitiveCompare:@" class=\"sender\""] == 0) {
                                if(inDiv && send) {
                                    [textAttributes setTextColor:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.0 alpha:1.0]];
                                } else if(inDiv && receive) {
                                    [textAttributes setTextColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.5 alpha:1.0]];
                                }
                            } else {
                                [textAttributes setTextColor:[NSColor blackColor]];
                            }
                            
                            //Process the font tag's contents
                            [self processFontTagArgs:[self parseArguments:chunkString] attributes:textAttributes];
                        }
                        
                    }else if([chunkString caseInsensitiveCompare:@"/FONT"] == 0){
                        [textAttributes setTextColor:[NSColor blackColor]];
                        [textAttributes setFontFamily:@"Helvetica"];
                        [textAttributes setFontSize:12];

                    //span
                    }else if([chunkString caseInsensitiveCompare:@"SPAN"] == 0){
                        if([scanner scanUpToCharactersFromSet:absoluteTagEnd intoString:&chunkString]){

                            //Process the span tag if it's in a log
                            if([chunkString caseInsensitiveCompare:@" class=\"sender\""] == 0) {
                                if(inDiv && send) {
                                    [textAttributes setTextColor:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.0 alpha:1.0]];
                                } else if(inDiv && receive) {
                                    [textAttributes setTextColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.5 alpha:1.0]];
                                }
                            } else {
                                [textAttributes setTextColor:[NSColor grayColor]];
                            }
                        }
                    } else if ([chunkString caseInsensitiveCompare:@"/SPAN"] == 0 ) {
                        [textAttributes setTextColor:[NSColor blackColor]];
                        [textAttributes setFontFamily:@"Helvetica"];
                        [textAttributes setFontSize:12];
                    //Line Break
                    }else if([chunkString caseInsensitiveCompare:@"BR"] == 0 || [chunkString caseInsensitiveCompare:@"/BR"] == 0){
                        [attrString appendString:@"\r" withAttributes:nil];

                    //Bold
                    }else if([chunkString caseInsensitiveCompare:@"B"] == 0){
                        [textAttributes enableTrait:NSBoldFontMask];
                    }else if([chunkString caseInsensitiveCompare:@"/B"] == 0){
                        [textAttributes disableTrait:NSBoldFontMask];

                    //Italic
                    }else if([chunkString caseInsensitiveCompare:@"I"] == 0){
                        [textAttributes enableTrait:NSItalicFontMask];
                    }else if([chunkString caseInsensitiveCompare:@"/I"] == 0){
                        [textAttributes disableTrait:NSItalicFontMask];

                    //Underline
                    }else if([chunkString caseInsensitiveCompare:@"U"] == 0){
                        [textAttributes setUnderline:YES];
                    }else if([chunkString caseInsensitiveCompare:@"/U"] == 0){
                        [textAttributes setUnderline:NO];

                    //Invalid
                    }else{
                        validTag = NO;
                    }
                }

                if(validTag){ //Skip over the end tag character '>'
                    [scanner setScanLocation:[scanner scanLocation]+1];
                }else{
                    //When an invalid tag is encountered, we add the <, and then move our scanner back to continue processing
                    [attrString appendString:@"<" withAttributes:[textAttributes dictionary]];
                    [scanner setScanLocation:scanLocation];
                }

            }else if([tagOpen compare:@"&"] == 0){ // escape character, eg &gt;
                BOOL validTag = [scanner scanUpToCharactersFromSet:charEnd intoString:&chunkString];
                
                if(validTag){
                    // We could upgrade this to use an NSDictionary with lots of chars
                    // but for now, if-blocks will do
                    if ([chunkString caseInsensitiveCompare:@"GT"] == 0){
                        [attrString appendString:@">" withAttributes:[textAttributes dictionary]];
                        
                    }else if ([chunkString caseInsensitiveCompare:@"LT"] == 0){
                        [attrString appendString:@"<" withAttributes:[textAttributes dictionary]];

                    }else if ([chunkString caseInsensitiveCompare:@"AMP"] == 0){
                        [attrString appendString:@"&" withAttributes:[textAttributes dictionary]];

                    }else if ([chunkString caseInsensitiveCompare:@"QUOT"] == 0){
                        [attrString appendString:@"\"" withAttributes:[textAttributes dictionary]];

                    }else if ([chunkString caseInsensitiveCompare:@"NBSP"] == 0){
                        [attrString appendString:@" " withAttributes:[textAttributes dictionary]];

                    }else if ([chunkString hasPrefix:@"#x"]) {
                        [attrString appendString:[NSString stringWithFormat:@"%C",
                            [chunkString substringFromIndex:1]]
                            withAttributes:[textAttributes dictionary]];
                    }else if ([chunkString hasPrefix:@"#"]) {
                        [attrString appendString:[NSString stringWithFormat:@"%C", 
                            [[chunkString substringFromIndex:1] intValue]] 
                            withAttributes:[textAttributes dictionary]];
                    }
                    else{ //Invalid
                        validTag = NO;
                    }           
                }

                
                if(validTag){ //Skip over the end tag character ';'
                    [scanner scanCharactersFromSet:charEnd intoString:nil];
                }else{
                    //When an invalid tag is encountered, we add the &, and then move our scanner back to continue processing
                    [attrString appendString:@"&" withAttributes:[textAttributes dictionary]];
                    [scanner setScanLocation:scanLocation];
                }
                
            }else{ //Invalid tag character (most likely a stray < or &)
                if([tagOpen length] > 1){
                    //If more than one character was scanned, add the first one, and move the scanner back to re-process the additional characters
                    [attrString appendString:[tagOpen substringToIndex:1] withAttributes:[textAttributes dictionary]];
                    [scanner setScanLocation:[scanner scanLocation] - ([tagOpen length]-1)]; 
                    
                }else{
                    [attrString appendString:tagOpen withAttributes:[textAttributes dictionary]];

                }
            }
        }
    }

    return([attrString autorelease]);
}

//Process the contents of a font tag
+ (void)processFontTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes
{
    NSEnumerator 	*enumerator;
    NSString		*arg;

    enumerator = [[inArgs allKeys] objectEnumerator];
    while((arg = [enumerator nextObject])){
        if([arg caseInsensitiveCompare:@"FACE"] == 0){
            [textAttributes setFontFamily:[inArgs objectForKey:arg]];

        }else if([arg caseInsensitiveCompare:@"SIZE"] == 0){
            int	size;

            //Always prefer an ABSZ to a size
            if(![inArgs objectForKey:@"ABSZ"] && ![inArgs objectForKey:@"absz"]){
                switch([[inArgs objectForKey:arg] intValue]){
                    case 1: size = 9; break;
                    case 2: size = 10; break;
                    case 3: size = 12; break;
                    case 4: size = 14; break;
                    case 5: size = 18; break;
                    case 6: size = 24; break;
                    case 7: size = 48; break;
                    case 8: size = 72; break;
                    default: size = 12; break;
                }
                [textAttributes setFontSize:size];
            }

        }else if([arg caseInsensitiveCompare:@"ABSZ"] == 0){
            [textAttributes setFontSize:[[inArgs objectForKey:arg] intValue]];

        }else if([arg caseInsensitiveCompare:@"COLOR"] == 0){
            [textAttributes setTextColor:[[inArgs objectForKey:arg] hexColor]];

        }else if([arg caseInsensitiveCompare:@"BACK"] == 0){
            [textAttributes setTextBackgroundColor:[[inArgs objectForKey:arg] hexColor]];
            
        }
    }
}

+ (void)processBodyTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes
{
    NSEnumerator 	*enumerator;
    NSString		*arg;

    enumerator = [[inArgs allKeys] objectEnumerator];
    while((arg = [enumerator nextObject])){
        if([arg caseInsensitiveCompare:@"BGCOLOR"] == 0){
            [textAttributes setBackgroundColor:[[inArgs objectForKey:arg] hexColor]];
        }
    }
}

+ (void)processLinkTagArgs:(NSDictionary *)inArgs attributes:(AITextAttributes *)textAttributes
{
    NSEnumerator 	*enumerator;
    NSString		*arg;

    enumerator = [[inArgs allKeys] objectEnumerator];
    while((arg = [enumerator nextObject])){
        if([arg caseInsensitiveCompare:@"HREF"] == 0){
            [textAttributes setLinkURL:[inArgs objectForKey:arg]];
        }
    }
}


+ (NSDictionary *)parseArguments:(NSString *)arguments
{
    NSMutableDictionary	*argDict;
    NSScanner		*scanner;
    NSCharacterSet	*equalsSet, *quoteSet, *spaceSet;
    NSString		*key = nil, *value = nil;

    //Setup
    equalsSet = [NSCharacterSet characterSetWithCharactersInString:@"="];
    quoteSet = [NSCharacterSet characterSetWithCharactersInString:@"\""];
    spaceSet = [NSCharacterSet characterSetWithCharactersInString:@" "];
    scanner = [NSScanner scannerWithString:arguments];
    argDict = [[NSMutableDictionary alloc] init];
    
    while(![scanner isAtEnd]){
        BOOL	validKey, validValue;
        
        //Find a tag
        validKey = [scanner scanUpToCharactersFromSet:equalsSet intoString:&key];
        [scanner scanCharactersFromSet:equalsSet intoString:nil];
        
        //check for quotes
        if([scanner scanCharactersFromSet:quoteSet intoString:nil]){
            validValue = [scanner scanUpToCharactersFromSet:quoteSet intoString:&value];
            [scanner scanCharactersFromSet:quoteSet intoString:nil];           
        }else{
            validValue = [scanner scanUpToCharactersFromSet:spaceSet intoString:&value];
        }

        //Store in dict
        if(validValue && value != nil && [value length] != 0 && validKey && key != nil && [key length] != 0){ //Watch out for invalid & empty tags
            [argDict setObject:value forKey:key];
        }
    }
    
    return([argDict autorelease]);
}

@end
