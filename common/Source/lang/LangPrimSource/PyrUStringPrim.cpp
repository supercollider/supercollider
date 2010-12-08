/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

	Primitives for regular expression search by Florian Schmidt, adapted to ICU by Jan Trutzschler

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
#if !defined(SC_WIN32) && !defined(SC_IPHONE)

struct URegularExpression;
/**
* Structure represeting a compiled regular rexpression, plus the results
 *    of a match operation.
 * @draft ICU 3.0
 */
typedef struct URegularExpression URegularExpression;

#define U_HIDE_DRAFT_API 1
#ifdef SC_DARWIN
	#define U_DISABLE_RENAMING 1
#endif
#include <unicode/uregex.h>
#include <unicode/ustring.h>
#include <unicode/udraft.h>
#include "PyrPrimitive.h"
#include "PyrKernel.h"
#include "GC.h"
#include <cstring>

#define MAXREGEXFIND 256;


struct SCRegExRegion {
    int start, end, group, matched;
};
typedef struct SCRegExRegion SCRegExRegion;

static int prString_FindRegexp(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2; // source string
	PyrSlot *b = g->sp - 1; // pattern
	PyrSlot *c = g->sp;     // offset

	if (!isKindOfSlot(b, class_string) || (NotInt(c))) return errWrongType;
//	post("prString_FindRegexp\n");
	int maxfind = MAXREGEXFIND;
	int offset = slotRawInt(c);
	int stringsize = slotRawObject(a)->size + 1;
	int patternsize =  slotRawObject(b)->size + 1;
	char *string = (char*)malloc(slotRawObject(a)->size + 1);
	int err = slotStrVal(a, string, slotRawObject(a)->size + 1);
	if (err){
		free(string);
		return err;
	}
	char *pattern = (char*)malloc(slotRawObject(b)->size + 1);
	err = slotStrVal(b, pattern, slotRawObject(b)->size + 1);
	if (err) return err;
	UParseError uerr;
	UErrorCode status = (UErrorCode)0;
	UChar *regexStr;
	UChar *ustring;

	regexStr =  (UChar*)malloc((patternsize)*sizeof(UChar));
	u_charsToUChars (pattern, regexStr, patternsize);

	ustring =  (UChar*)malloc((stringsize)*sizeof(UChar));
	u_charsToUChars (string+offset, ustring, stringsize-offset);


	unsigned flags = UREGEX_MULTILINE;
	int groupNumber = 0;
	SCRegExRegion * what;
	int indx = 0;
	int size = 0;

	URegularExpression *expression = uregex_open(regexStr, -1, flags, &uerr, &status);
	if(U_FAILURE(status)) goto nilout;

	 if(!U_FAILURE(status)) {
		uregex_setText(expression, ustring, -1, &status);
		what =  (SCRegExRegion*)malloc((maxfind)*sizeof(SCRegExRegion));
		for(int i=0; i< maxfind; i++)
		{
			SCRegExRegion range;
			range.matched = false;
			what[i] = range;
		}

		int32_t groups = uregex_groupCount(expression, &status) + 1;
		if(U_FAILURE(status)) goto nilout;
//		post("groups: %i\n", groups);
		while (uregex_findNext(expression, &status) && size<maxfind)
		{
			if(U_FAILURE(status)) return errNone;

			for(int i=0; i< groups; ++i){
				what[size].group = i;
				what[size].start = sc_clip(uregex_start(expression, i, &status), 0, stringsize) ;
				if(U_FAILURE(status)) goto nilout;
				what[size].end = sc_clip(uregex_end(expression, i, &status), 0, stringsize);
				what[size].matched = true;
//				post("index:%i, size:%i, start %i, end %i\n", i, size, what[i].start, what[i].end);
				size = indx++ + 1;
				if(U_FAILURE(status)) goto nilout;
			}
		}

		PyrObject *result_array = newPyrArray(g->gc, size, 0, true);
		result_array->size = 0;

		if (size>0) //(matched)
		{
			for (int i = 0; i < size; i++)
			{
				if (what[0].matched == false)
				{
					result_array->size++;
					SetNil(result_array->slots+i);
				} else {
					result_array->size++;

					int match_start =  what[i].start;
					int match_length = what[i].end -  what[i].start;
//					post("for i:%i, start %i, end %i\n",  i, what[i].start,  what[i].end);
					char match[match_length+1];

					strncpy(match, string + offset + match_start, match_length);
					match[match_length] = 0;
					PyrObject *array = newPyrArray(g->gc, 2, 0, true);
					array->size = 2;
					SetInt(array->slots, match_start + offset);

					PyrObject *matched_string = (PyrObject*)newPyrString(g->gc, match, 0, true);
					SetObject(array->slots+1, matched_string);
					g->gc->GCWrite(array, matched_string);

					SetObject(result_array->slots + i, array);
					g->gc->GCWrite(result_array, array);
				}
			}
		}
		else
			SetNil(a);

		free(what);
		free(pattern);
		free(regexStr);
		free(ustring);
		free(string);
		SetObject(a, result_array);
		//uregex_close(expression);
		return errNone;
	}

nilout:
	free(string);
	free(what);
	free(pattern);
	free(regexStr);
	free(ustring);
	SetNil(a);
	return errNone;
}

void initUStringPrimitives();
void initUStringPrimitives()
{
	int base, index = 0;
	base = nextPrimitiveIndex();
	definePrimitive(base, index++, "_String_FindRegexp", prString_FindRegexp, 3, 0);
}

#else // !SC_DARWIN
void initUStringPrimitives();
void initUStringPrimitives()
{
	//other platforms? - icu should be running on linux too
}
#endif // SC_DARWIN
