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
/*

Primitives for Symbol.

*/

#include <string.h>
#include <stdlib.h>
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "VMGlobals.h"
#include "PyrKernel.h"
#include "SCBase.h"

/*
int prSymbolString(struct VMGlobals *g, int numArgsPushed);
int prSymbolString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrString *string;

	a = g->sp;
	if (NotSym(a)) return errWrongType;
	string = newPyrString(g->gc, slotRawSymbol(a)->name, 0, true);
	SetObject(a, string);
	return errNone;
}
*/

int prSymbolIsPrefix(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsPrefix(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int length;

	a = g->sp - 1;
	b = g->sp;
	if (!IsSym(a) || !IsSym(b)) return errWrongType;
	int32 alen = slotRawSymbol(a)->length;
	int32 blen = slotRawSymbol(b)->length;
	length = sc_min(alen, blen);
	if (memcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name, length) == 0) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolClass(struct VMGlobals *g, int numArgsPushed);
int prSymbolClass(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrClass *classobj;
	//char firstChar;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Class) {
	//firstChar = slotRawSymbol(a)->name[0];
	//if (firstChar >= 'A' && firstChar <= 'Z') {
		classobj = slotRawSymbol(a)->u.classobj;
		if (classobj) {
			SetObject(a, classobj);
		} else {
			SetNil(a);
		}
	} else {
		SetNil(a);
	}
	return errNone;
}

int prSymbolIsSetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsSetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Setter) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolAsSetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolAsSetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[256];
	int len;

	a = g->sp;
	if (!(slotRawSymbol(a)->flags & sym_Setter)) {
		if ((slotRawSymbol(a)->flags & sym_Class) || (slotRawSymbol(a)->flags & sym_Primitive)) {
			error("Cannot convert class names or primitive names to setters.\n");
			return errFailed;
		}
		if (strlen(slotRawSymbol(a)->name)>255) {
			error("symbol name too long.\n");
			return errFailed;
		}
		strcpy(str, slotRawSymbol(a)->name);
		len = strlen(str);
		str[len] = '_';
		str[len+1] = 0;

		//postfl("prSymbolAsSetter %s\n", str);
		SetRaw(a, getsym(str));
	}
	return errNone;
}

int prSymbolAsGetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolAsGetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[256];

	a = g->sp;
	if ((slotRawSymbol(a)->flags & sym_Setter)) {
		if ((slotRawSymbol(a)->flags & sym_Class) || (slotRawSymbol(a)->flags & sym_Primitive)) {
			error("Cannot convert class names or primitive names to getters.\n");
			return errFailed;
		}
		strcpy(str, slotRawSymbol(a)->name);
		str[strlen(str)-1] = 0;
		//postfl("prSymbolAsGetter %s\n", str);
		SetRaw(a, getsym(str));
	}
	return errNone;
}

int prSymbolIsClassName(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Class) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolIsMetaClassName(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsMetaClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_MetaClass) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}


int prSymbol_IsBinaryOp(struct VMGlobals *g, int numArgsPushed);
int prSymbol_IsBinaryOp(struct VMGlobals *g, int numArgsPushed)
{
	static const char *binary_op_characters = "!@%&*-+=|<>?/";
	PyrSlot *a = g->sp;
	const char* str = slotRawSymbol(a)->name;

	if (str[0] == '\0') {
		// An empty symbol is not a valid binary operator.
		SetFalse(a);
	} else if (str[1] == '\0') {
		// A symbol of length 1 should be any binary operator character except '='.
		if (str[0] == '=') {
			SetFalse(a);
		} else {
			bool character_is_binary_operator = strchr(binary_op_characters, str[0]);
			SetBool(a, character_is_binary_operator);
		}
	} else {
		// A symbol of length 2+ must contain only binary operator characters, and must not start
		// with '//' or '/*'.
		if (str[0] == '/' && (str[1] == '/' || str[1] == '*')) {
			SetFalse(a);
		} else {
			const int length = strlen(str);
			SetTrue(a);
			for (int i = 0; i < length; i++) {
				bool character_is_binary_operator = strchr(binary_op_characters, str[i]);
				if (!character_is_binary_operator) {
					SetFalse(a);
					break;
				}
			}
		}
	}

	return errNone;
}

int prSymbol_IsIdentifier(struct VMGlobals *g, int numArgsPushed);
int prSymbol_IsIdentifier(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	const char* str = slotRawSymbol(a)->name;

	// An empty symbol is not a valid identifier.
	if (str[0] == '\0') {
		SetFalse(a);

	// The first character must be a lowercase letter.
	} else if ('a' <= str[0] && str[0] <= 'z') {
		int length = strlen(str);
		SetTrue(a);
		// All other characters must be alphanumeric or '_'.
		for (int i = 1; i < length; i++) {
			char c = str[i];
			if (
				!(
					(c == '_')
					|| ('a' <= c && c <= 'z')
					|| ('A' <= c && c <= 'Z')
					|| ('0' <= c && c <= '9')
				)
			) {
				SetFalse(a);
				break;
			}
		}
	} else {
		SetFalse(a);
	}

	return errNone;
}

int prSymbol_AsInteger(struct VMGlobals *g, int numArgsPushed);
int prSymbol_AsInteger(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	SetInt(a, atoi(str));

	return errNone;
}

int prSymbol_PrimitiveIndex(struct VMGlobals *g, int numArgsPushed);
int prSymbol_PrimitiveIndex(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, slotRawSymbol(a)->u.index);

	return errNone;
}

int prSymbol_SpecialIndex(struct VMGlobals *g, int numArgsPushed);
int prSymbol_SpecialIndex(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, slotRawSymbol(a)->specialIndex);

	return errNone;
}


int prSymbol_AsFloat(struct VMGlobals *g, int numArgsPushed);
int prSymbol_AsFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	SetFloat(a, atof(str));

	return errNone;
}

// following function lifted from liblo: http://sourceforge.net/projects/liblo/

/* Open SoundControl kit in C++                                              */
/* Copyright (C) 2002-2004 libOSC++ contributors. See AUTHORS                */
/*                                                                           */
/* This library is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU Lesser General Public                */
/* License as published by the Free Software Foundation; either              */
/* version 2.1 of the License, or (at your option) any later version.        */
/*                                                                           */
/* This library is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         */
/* Lesser General Public License for more details.                           */
/*                                                                           */
/* You should have received a copy of the GNU Lesser General Public          */
/* License along with this library; if not, write to the Free Software       */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                           */
/* For questions regarding this program contact                              */
/* Daniel Holth <dholth@fastmail.fm> or visit                                */
/* http://wiretap.stetson.edu/                                               */

/* In the sprit of the public domain, my modifications to this file are also */
/* dedicated to the public domain. Daniel Holth, Oct. 2004                   */

/* ChangeLog:
 *
 * 2004-10-29 Import, convert to C++, begin OSC syntax changes. -dwh
 *              OSC syntax changes are now working, needs more testing.
 *
 */

// Original header and syntax:

/*
 * robust glob pattern matcher
 * ozan s. yigit/dec 1994
 * public domain
 *
 * glob patterns:
 *  *   matches zero or more characters
 *  ?   matches any single character
 *  [set]   matches any character in the set
 *  [^set]  matches any character NOT in the set
 *      where a set is a group of characters or ranges. a range
 *      is written as two characters seperated with a hyphen: a-z denotes
 *      all characters between a to z inclusive.
 *  [-set]  set matches a literal hypen and any character in the set
 *  []set]  matches a literal close bracket and any character in the set
 *
 *  char    matches itself except where char is '*' or '?' or '['
 *  \char   matches char, including any pattern character
 *
 * examples:
 *  a*c     ac abc abbc ...
 *  a?c     acc abc aXc ...
 *  a[a-z]c     aac abc acc ...
 *  a[-a-z]c    a-c aac abc ...
 *
 * $Log$
 * Revision 1.1  2004/11/19 23:00:57  theno23
 * Added lo_send_timestamped
 *
 * Revision 1.3  1995/09/14  23:24:23  oz
 * removed boring test/main code.
 *
 * Revision 1.2  94/12/11  10:38:15  oz
 * cset code fixed. it is now robust and interprets all
 * variations of cset [i think] correctly, including [z-a] etc.
 *
 * Revision 1.1  94/12/08  12:45:23  oz
 * Initial revision
 */


#ifndef lo_NEGATE
#define lo_NEGATE  '!'
#endif

#ifndef lo_true
#define lo_true 1
#endif
#ifndef lo_false
#define lo_false 0
#endif

inline int lo_pattern_match(const char *str, const char *p)
{
    int negate;
    int match;
    char c;

    while (*p) {
        if (!*str && *p != '*')
            return lo_false;

        switch (c = *p++) {

			case '*':
				while (*p == '*' && *p != '/')
					p++;

				if (!*p)
					return lo_true;

				//                if (*p != '?' && *p != '[' && *p != '\\')
				if (*p != '?' && *p != '[' && *p != '{')
					while (*str && *p != *str)
						str++;

				while (*str) {
					if (lo_pattern_match(str, p))
						return lo_true;
					str++;
				}
				return lo_false;

			case '?':
				if (*str)
					break;
				return lo_false;
				/*
				 * set specification is inclusive, that is [a-z] is a, z and
				 * everything in between. this means [z-a] may be interpreted
				 * as a set that contains z, a and nothing in between.
				 */
			case '[':
				if (*p != lo_NEGATE)
					negate = lo_false;
				else {
					negate = lo_true;
					p++;
				}

				match = lo_false;

				while (!match && (c = *p++)) {
					if (!*p)
						return lo_false;
					if (*p == '-') {        /* c-c */
						if (!*++p)
							return lo_false;
						if (*p != ']') {
							if (*str == c || *str == *p ||
								(*str > c && *str < *p))
								match = lo_true;
						} else {    /* c-] */
							if (*str >= c)
								match = lo_true;
							break;
						}
					} else {        /* cc or c] */
						if (c == *str)
							match = lo_true;
						if (*p != ']') {
							if (*p == *str)
								match = lo_true;
						} else
							break;
					}
				}

				if (negate == match)
					return lo_false;
				/*
				 * if there is a match, skip past the cset and continue on
				 */
				while (*p && *p != ']')
					p++;
				if (!*p++)          /* oops! */
					return lo_false;
				break;

				/*
				 * {astring,bstring,cstring}
				 */
			case '{':
            {
                // *p is now first character in the {brace list}
                const char *place = str;        // to backtrack
                const char *remainder = p;      // to forwardtrack

                // find the end of the brace list
                while (*remainder && *remainder != '}')
                    remainder++;
                if (!*remainder++)      /* oops! */
                    return lo_false;

                c = *p++;

                while (c) {
                    if (c == ',') {
                        if (lo_pattern_match(str, remainder)) {
                            return lo_true;
                        } else {
                            // backtrack on test string
                            str = place;
                            // continue testing,
                            // skip comma
                            if (!*p++)  // oops
                                return lo_false;
                        }
                    } else if (c == '}') {
                        // continue normal pattern matching
                        if (!*p && !*str)
                            return lo_true;
                        str--;  // str is incremented again below
                        break;
                    } else if (c == *str) {
                        str++;
                        if (!*str && *remainder)
                            return lo_false;
                    } else {    // skip to next comma
                        str = place;
                        while (*p != ',' && *p != '}' && *p)
                            p++;
                        if (*p == ',')
                            p++;
                        else if (*p == '}') {
                            return lo_false;
                        }
                    }
                    c = *p++;
                }
            }

				break;

				/* Not part of OSC pattern matching
				 case '\\':
				 if (*p)
				 c = *p++;
				 */

			default:
				if (c != *str)
					return lo_false;
				break;

        }
        str++;
    }

    return !*str;
}

// end: following function lifted from liblo



int prSymbol_matchOSCPattern(struct VMGlobals *g, int numArgsPushed);
int prSymbol_matchOSCPattern(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;

	a = g->sp - 1;
	b = g->sp;
	if (!IsSym(a) || !IsSym(b)) return errWrongType;
//	int32 alen = slotRawSymbol(a)->length;
//	int32 blen = slotRawSymbol(b)->length;
//	length = sc_min(alen, blen);
	if (lo_pattern_match(slotRawSymbol(a)->name, slotRawSymbol(b)->name)) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbol_isMap(struct VMGlobals *g, int numArgsPushed);
int prSymbol_isMap(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	if(strlen(str)>1 && (str[0]=='a' || str[0]=='c') && str[1]>='0' && str[1]<='9')
		SetTrue(a);
	else
		SetFalse(a);

	return errNone;
}



void initSymbolPrimitives();
void initSymbolPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_SymbolIsPrefix", prSymbolIsPrefix, 2, 0);
	//definePrimitive(base, index++, "_SymbolString", prSymbolString, 1, 0);
	definePrimitive(base, index++, "_SymbolClass", prSymbolClass, 1, 0);
	definePrimitive(base, index++, "_SymbolIsClassName", prSymbolIsClassName, 1, 0);
	definePrimitive(base, index++, "_SymbolIsMetaClassName", prSymbolIsMetaClassName, 1, 0);
	definePrimitive(base, index++, "_Symbol_IsIdentifier", prSymbol_IsIdentifier, 1, 0);
	definePrimitive(base, index++, "_Symbol_IsBinaryOp", prSymbol_IsBinaryOp, 1, 0);
	definePrimitive(base, index++, "_SymbolIsSetter", prSymbolIsSetter, 1, 0);
	definePrimitive(base, index++, "_SymbolAsSetter", prSymbolAsSetter, 1, 0);
	definePrimitive(base, index++, "_SymbolAsGetter", prSymbolAsGetter, 1, 0);
	definePrimitive(base, index++, "_Symbol_AsInteger", prSymbol_AsInteger, 1, 0);
	definePrimitive(base, index++, "_Symbol_PrimitiveIndex", prSymbol_PrimitiveIndex, 1, 0);
	definePrimitive(base, index++, "_Symbol_SpecialIndex", prSymbol_SpecialIndex, 1, 0);
	definePrimitive(base, index++, "_Symbol_AsFloat", prSymbol_AsFloat, 1, 0);
	definePrimitive(base, index++, "_Symbol_matchOSCPattern", prSymbol_matchOSCPattern, 2, 0);
	definePrimitive(base, index++, "_Symbol_IsMap", prSymbol_isMap, 1, 0);
}


#if _SC_PLUGINS_

#include "SCPlugin.h"

// export the function that SC will call to load the plug in.
#pragma export on
extern "C" { SCPlugIn* loadPlugIn(void); }
#pragma export off


// define plug in object
class APlugIn : public SCPlugIn
{
public:
	APlugIn();
	virtual ~APlugIn();

	virtual void AboutToCompile();
};

APlugIn::APlugIn()
{
	// constructor for plug in
}

APlugIn::~APlugIn()
{
	// destructor for plug in
}

void APlugIn::AboutToCompile()
{
	// this is called each time the class library is compiled.
	initSymbolPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn()
{
	return new APlugIn();
}

#endif

