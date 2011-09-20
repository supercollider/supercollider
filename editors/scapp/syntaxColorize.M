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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include <Cocoa/Cocoa.h>
#include <SCGeom.h>


static int linepos, lineno, charno, yylen;
static int textpos, textlen, totallen;
static unichar *text;
static unichar yytext[256];

//initializing to default SC colors, these will get overriden 
//by CocoaDocument.sc upon lang compilation, defaults are given
//here so when lang compilation fails, we still have some colors 
//over the text to debug.
SCColor synColText = SCMakeColor(0, 0, 0, 1), 
		synColClass = SCMakeColor(0, 0, 0.75, 1), 
		synColString = SCMakeColor(0.375, 0.375, 0.375, 1), 
		synColSymbol = SCMakeColor(0, 0.45, 0, 1), 
		synColComment = SCMakeColor(0.75, 0, 0, 1), 
		synColNumber = SCMakeColor(0.0, 0.0, 0.0, 1),
		synColSpecialVals = SCMakeColor(0.2, 0.2, 0.75, 1), // true false nil inf 
		synColSpecialVars = SCMakeColor(0.4, 0.4, 0.75, 1), // super, thisProcess
		synColDecl = SCMakeColor(0, 0, 1, 1), // var, const, args
		synColPunc = SCMakeColor(0.0, 0.0, 0.0, 1), // : ; ' [ ] { } ( )
		synColEnv = SCMakeColor(1.0, 0.4, 0, 1);

static bool streq(char *cstr, unichar *ustr)
{
    while (true) {
        char c = *cstr++;
        unichar u = *ustr++;
        if (u != c) return false;
        else if (u == 0) return true;
    }
    return false;
}

static unichar input() 
{
	unichar c;
	if (textpos > textlen) {
		c = 0;
	} else {
		c = text[textpos++];
		charno++;
	}
	if (c == '\n' || c == '\r') {
		lineno++;
		linepos = textpos;
		charno = 0;
	}
	if (c != 0) yytext[yylen++] = c;
	//if (gDebugLexer) post("input '%c' %d\n",c,c);
	return c;
}

static unichar input0() 
{
	unichar c;
	if (textpos > textlen) {
		c = 0;
		textpos++; // so unput will work properly
	} else {
		c = text[textpos++];
		charno++;
	}
	if (c == '\n' || c == '\r') {
		lineno++;
		linepos = textpos;
		charno = 0;
	}
	//if (gDebugLexer) post("input0 '%c' %d\n",c,c);
	return c;
}

static void unput(unichar c) 
{
	if (textpos>0) textpos--;
	if (c) {
		if (yylen) --yylen;
		if (charno) --charno;
		if (c == '\n' || c == '\r') {
			--lineno;
		}
	}
}

unichar* startColorizer(NSTextView* textView);
unichar* startColorizer(NSTextView* textView) 
{
    totallen = [[textView textStorage] length];

    NSRange range = [textView selectedRange];
    if (range.length == 0) range = NSMakeRange(0, totallen);

    if (![textView shouldChangeTextInRange: range replacementString: nil]) return 0;
    
    textpos = range.location;
    textlen = textpos + range.length;
    
    text = (unichar*)malloc((totallen+1) * sizeof(unichar));
    [[[textView textStorage] string] getCharacters: text];
    text[totallen] = 0;
    
    linepos = 0;
    lineno = 1;
    charno = 0;

    yylen = 0;

    return text;
}

void SyntaxColorize(NSTextView* textView)
{
	unichar c;
	long startrun = 0, endrun;
	NSRange range;
	int startline, clevel, prevc;
	NSColor* textColor = [NSColor colorWithCalibratedRed: synColText.red green: synColText.green blue: synColText.blue alpha: synColText.alpha];
	NSColor* commentColor = [NSColor colorWithCalibratedRed: synColComment.red green: synColComment.green blue: synColComment.blue alpha: synColComment.alpha];
	NSColor* classColor = [NSColor colorWithCalibratedRed: synColClass.red green: synColClass.green blue: synColClass.blue alpha: synColClass.alpha];
	NSColor* symbolColor = [NSColor colorWithCalibratedRed: synColSymbol.red green: synColSymbol.green blue: synColSymbol.blue alpha: synColSymbol.alpha];
	NSColor* stringColor = [NSColor colorWithCalibratedRed: synColString.red green: synColString.green blue: synColString.blue alpha: synColString.alpha];
	NSColor* numberColor = [NSColor colorWithCalibratedRed: synColNumber.red green: synColNumber.green blue: synColNumber.blue alpha: synColNumber.alpha];
	NSColor* specialValsColor = [NSColor colorWithCalibratedRed: synColSpecialVals.red green: synColSpecialVals.green blue: synColSpecialVals.blue alpha: synColSpecialVals.alpha];
	NSColor* specialVarsColor = [NSColor colorWithCalibratedRed: synColSpecialVars.red green: synColSpecialVars.green blue: synColSpecialVars.blue alpha: synColSpecialVars.alpha];
	NSColor* declColor = [NSColor colorWithCalibratedRed: synColDecl.red green: synColDecl.green blue: synColDecl.blue alpha: synColDecl.alpha];
	NSColor* puncColor = [NSColor colorWithCalibratedRed: synColPunc.red green: synColPunc.green blue: synColPunc.blue alpha: synColPunc.alpha];
	NSColor* environColor = [NSColor colorWithCalibratedRed: synColEnv.red green: synColEnv.green blue: synColEnv.blue alpha: synColEnv.alpha]; 
	NSTextStorage *textStorage = [textView textStorage];
	
	if (!startColorizer(textView)) return;
	
	[textStorage beginEditing];
	
	// reset to textColor	        
	range = NSMakeRange(textpos, textlen - textpos);
	[textStorage addAttribute:NSForegroundColorAttributeName value: textColor range: range];

	bool bracket = false;
	
	for (;;) {
		yylen = 0;
		c = input();
		if (c == 0 || textpos > textlen) goto leave;
		else if (c==' ' || c=='\t' || c=='\v' || c=='\f' || c=='\n' || c=='\r') {
			continue;
		}
		
		if (c == '|' && bracket) goto arglist;
		if (c == '{') { bracket = true; }
			else bracket = false;
		
		if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_') goto ident;
		else if (c == '\\') goto symbol1;
		else if (c == '\'') goto symbol3;
		else if (c == '"') goto string1;
		else if (c == '/') {
			c = input0();
			if (c == '/') goto comment1;
			else if (c == '*') goto comment2;
			else {
				unput(c);
				continue; 
			}
		}
		else if (c == '$') { 	
			c = input0();
			if (c == '\\') {
				c = input0();
			}
			goto characterLiteral; 
		}
		else if(c == '-')	goto negnumber; //might be a negative number
		else if(c >= '0' && c <= '9')	goto number;
		else if(c==':' || c==';' || c=='.' || c == ',' || c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}')	goto punct;
		else if(c=='~') goto environment;
		else {
			continue; 
		}
	ident:
		c = input();
		
		if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' 
			|| c == '_' || c >= '0' && c <= '9') goto ident;
		else if (c == ':') {
			yytext[yylen] = 0;
			//r = processkeywordbinop(yytext) ; 
			continue;
		} else {
			unput(c);
			yytext[yylen] = 0;
			endrun = textpos;
			startrun = textpos - yylen;
			range = NSMakeRange(startrun, endrun - startrun);
			if (yytext[0] == '_') {
			  [textStorage addAttribute:NSForegroundColorAttributeName value: classColor range: range];
			}
			else if (yytext[0] >= 'A' && yytext[0] <= 'Z') {
			  [textStorage addAttribute:NSForegroundColorAttributeName value: classColor range: range];
			}
			else if (  streq("var",yytext)
					|| streq("arg",yytext)
					|| streq("classvar",yytext) 
					|| streq("const",yytext)
			) {
				range = NSMakeRange(startrun, endrun - startrun);
				[textStorage addAttribute:NSForegroundColorAttributeName value: declColor range: range];
			}
			else if (  streq("this",yytext)
					|| streq("super",yytext)
					|| streq("thisProcess",yytext)
					|| streq("thisThread",yytext) 
					|| streq("thisMethod",yytext)
					|| streq("thisFunctionDef",yytext)
					|| streq("thisFunction",yytext)
			) {
				[textStorage addAttribute:NSForegroundColorAttributeName value: specialVarsColor range: range];
			}
			else if (  streq("nil",yytext)
					|| streq("false",yytext)
					|| streq("true",yytext) 
					|| streq("inf",yytext)
			) {
				[textStorage addAttribute:NSForegroundColorAttributeName value: specialValsColor range: range];
			}
			continue;
		}
	symbol1:
		c = input();
		
		if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' 
			|| c == '_') {
				startrun = textpos - 2;
				goto symbol2;
		} else {
			unput(c);
			yytext[yylen] = 0;
			continue;
		}
	symbol2:
		c = input();
		
		if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' 
			|| c == '_' || c >= '0' && c <= '9') goto symbol2;
		else {
			unput(c);
			endrun = textpos;
			range = NSMakeRange(startrun, endrun - startrun);
			[textStorage addAttribute:NSForegroundColorAttributeName value: symbolColor range: range];
			continue;
		}
	symbol3 : {
			int startline;
			unichar endchar;
			startrun = textpos - 1;
			startline = lineno;
			endchar = '\'';
		
			c = 0;
			do {
				prevc = c;
				c = input0();
			} while (!(c == endchar && prevc != '\\') && c != 0);
			if (c == 0 || textpos > textlen) {
				//postbuf("Open ended symbol ... started on line %d in file '%s'\n", 
				//	startline, curfilename);
				goto error2;
			} else {
				endrun = textpos;
				range = NSMakeRange(startrun, endrun - startrun);
				[textStorage addAttribute:NSForegroundColorAttributeName value: symbolColor range: range];
				continue;
			}
		}

	string1 : {
			int startline;
			unichar endchar;
                        
			startrun = textpos - 1;
			startline = lineno;
			endchar = '\"';
		
			c = 0;
			do  {
				prevc = c;
				c = input0();
				if(prevc == '\\' && c == '\\') {
					prevc = c;
					c = input0();
					if(c == endchar) break;
				}
			} while (!(c == endchar && prevc != '\\') && c != 0);
			if (c == 0 || textpos > textlen) {
				//postbuf("Open ended string ... started on line %d in file '%s'\n", 
				//	startline, curfilename);
				goto error2;
			} else {
				endrun = textpos;
				range = NSMakeRange(startrun, endrun - startrun);
				[textStorage addAttribute:NSForegroundColorAttributeName value: stringColor range: range];
				continue;
			}
		}
	arglist:
		startrun = textpos-1;
		do {
			c = input0(); 
		} while (c != '|' && c != 0);
		endrun = textpos;
		if(c != 0){ 
			range = NSMakeRange(startrun, endrun - startrun);
			[textStorage addAttribute:NSForegroundColorAttributeName value: declColor range: range];
		}
		continue;
	comment1:	/* comment -- to end of line */
		startrun = textpos-2;
		do {
			c = input0(); 
		} while (c != '\n' && c != '\r' && c != 0);
		endrun = textpos;
		range = NSMakeRange(startrun, endrun - startrun - 1);
		[textStorage addAttribute:NSForegroundColorAttributeName value: commentColor range: range];
		continue;

	comment2:
		startrun = textpos-2;
		startline = lineno;
		prevc = 0;
		clevel = 1;
		do {
			c = input0();
			if (c == '/' && prevc == '*') {
				if (--clevel <= 0) break;
			} else if (c == '*' && prevc == '/') clevel++;
			prevc = c;
		} while (c != 0);
		if (textpos > textlen) {
			goto error2;
		} else {
			endrun = textpos;
			range = NSMakeRange(startrun, endrun - startrun);
			[textStorage addAttribute:NSForegroundColorAttributeName value: commentColor range: range];
			continue;
		}
	number:
 		startrun = textpos -1;
 		do {
 			c = input0();
 			if(c == '.') { c = input0(); if(c < '0' || c > '9') { unput(c); break; }; }; 
 		}	while ((c >= '0' && c <= '9') || c == '.' || c == ' ');
 		endrun = textpos - 1;
 		range = NSMakeRange(startrun, endrun - startrun);
 		[textStorage addAttribute:NSForegroundColorAttributeName value: numberColor range: range];
		unput(c);
 		continue;
	environment:
 		startrun = textpos -1;
 		do {
 			c = input0();
 		}	while (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_' || c >= '0' && c <= '9');
 		endrun = textpos - 1;
 		range = NSMakeRange(startrun, endrun - startrun);
 		[textStorage addAttribute:NSForegroundColorAttributeName value: environColor range: range];
		unput(c);
 		continue;
	characterLiteral:
		if(c == '\\') {
			startrun = textpos -3;
		} else {
			startrun = textpos -2;
		}
 		endrun = textpos;
 		range = NSMakeRange(startrun, endrun - startrun);
 		[textStorage addAttribute:NSForegroundColorAttributeName value: numberColor range: range];
 		continue;
	punct:
 		startrun = textpos -1;
 		endrun = textpos;
 		range = NSMakeRange(startrun, endrun - startrun);
 		[textStorage addAttribute:NSForegroundColorAttributeName value: puncColor range: range];
 		continue;
	negnumber:
		bool numFound = false; //no digit found yet, after minus sign
		bool reachedStart = false;
		int tempTextpos = textpos, tempCharno = charno, tempYylen = yylen, tempLineno = lineno;
		do { //go backwards until you find stg. other than whitespace and numbers
			unput(c); unput(c); 
			c = input0();
			if(textpos <= 1) { //beginning of doc. escape!
				reachedStart = true;
				break;
			}
		} while (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' ||  c == '\r' || c == 0 || (c >= '0' && c <= '9'));
		textpos = tempTextpos; charno = tempCharno; yylen = tempYylen; lineno = tempLineno; //restore original position
		if(reachedStart) continue;
		if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_' || c == ')' || c == ']') continue; //it must be a subtraction operator, skip.
		startrun = textpos - 1;
		do {
			c = input0();			
		} while(c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' ||  c == '\r');
		if(c >= '0' && c <= '9') { 
			numFound = true;	
			unput(c);
			endrun = textpos;
		};		
		if(numFound)
		{
			range = NSMakeRange(startrun, endrun - startrun);
			[textStorage addAttribute:NSForegroundColorAttributeName value: numberColor range: range];
		} else goto punct;
		continue;
	}
	
error2:
	
leave:
	free(text);
	[textStorage endEditing];
}
