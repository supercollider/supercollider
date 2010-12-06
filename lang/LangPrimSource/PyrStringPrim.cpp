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

Primitives for String.

*/

#include "PyrPrimitive.h"
#include "PyrKernel.h"
#include "GC.h"
#include "Hash.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "PyrLexer.h"
#include "SC_DirUtils.h"
#ifdef SC_WIN32
# include <direct.h>
# include "SC_Win32Utils.h"
#else
# include <sys/param.h>
# include <regex.h>
#endif

int prStringAsSymbol(struct VMGlobals *g, int numArgsPushed);
int prStringAsSymbol(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[1024], *strp=0;
	int len;

	a = g->sp;
	len = slotRawObject(a)->size;
	strp = len > 1023 ? (char*)malloc(len+1) : str;

	memcpy(strp, slotRawString(a)->s, len);
	strp[len] = 0;

	SetSymbol(a, getsym(strp));

	if (len > 1023) free(strp);

	return errNone;
}

int prString_AsInteger(struct VMGlobals *g, int numArgsPushed);
int prString_AsInteger(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char str[256];
	int err = slotStrVal(a, str, 255);
	if (err) return err;

	SetInt(a, atoi(str));

	return errNone;
}

int prString_AsFloat(struct VMGlobals *g, int numArgsPushed);
int prString_AsFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char str[256];
	int err = slotStrVal(a, str, 255);
	if (err) return err;

	SetFloat(a, atof(str));

	return errNone;
}

int prString_AsCompileString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrString* scstr = slotRawString(a);
	char *chars1 = scstr->s;
	int newSize = scstr->size + 2;
	for (int i=0; i<scstr->size; ++i) {
		if (chars1[i] == '"' || chars1[i] == '\\') newSize++;
	}
	PyrString *newString = newPyrStringN(g->gc, newSize, 0, true);
	char *chars2 = newString->s;
	chars2[0] = '"';
	chars2[newSize - 1] = '"';
	int k = 1;
	for (int i=0; i<scstr->size; ++i) {
		int c = chars1[i];
		if (c == '"' || c == '\\') chars2[k++] = '\\';
		chars2[k++] = c;
	}
	SetObject(a, newString);
	return errNone;
}

int prString_Format(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!isKindOfSlot(b, class_array)) return errWrongType;

	char *fmt = slotRawString(a)->s;

	int asize = slotRawObject(a)->size;
	int bsize = slotRawObject(b)->size;
	int csize = asize;

	PyrSlot *slots = slotRawObject(b)->slots;
	for (int i=0; i<bsize; ++i) {
		PyrSlot *slot = slots + i;
		if (!isKindOfSlot(slot, class_string)) return errWrongType;
		csize += slotRawString(slot)->size;
	}
	PyrString *newString = newPyrStringN(g->gc, csize, 0, true);
	char* buf = newString->s;

	int k=0;
	int index = 0;
	for (int i=0; i<asize;) {
		char ch = fmt[i++];
		if (ch == '%') {
			if (index < bsize) {
				PyrString* bstring = slotRawString(&slots[index]);
				memcpy(buf+k, bstring->s, bstring->size);
				k += bstring->size;
				index++;
			}
		} else if (ch == '\\') {
			if (i >= asize) break;
			ch = fmt[i++];
			if (ch == '%') {
				buf[k++] = '%';
			} else {
				i--;
			}
		} else {
			buf[k++] = ch;
		}
	}
	newString->size = k;
	SetObject(a, newString);
	return errNone;
};

// no regular expressions for windows yet
// include pcer at one point...
#ifndef SC_WIN32
int matchRegexp(char *string, char *pattern)
{
	int    status;
	regex_t    re;
	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
		return(2);      /* Report error. */
	}
	status = regexec(&re, string, (size_t) 0, NULL, 0);
	regfree(&re);
		if (status) {
			return(1);      /* Report error. */
		}
	return(0);
}

int prString_Regexp(struct VMGlobals *g, int numArgsPushed)
{
	int err, start, end, ret, len;

	PyrSlot *a = g->sp - 3;
	PyrSlot *b = g->sp - 2;
	PyrSlot *c = g->sp - 1;
	PyrSlot *d = g->sp;

	if (!isKindOfSlot(b, class_string)) return errWrongType;
	if (NotInt(c) || NotInt(d) && NotNil(d)) return errWrongType;
	start = slotRawInt(c);

	len = slotRawObject(b)->size; // last char index instead of size

	if(IsNil(d)) {
		end = len;
	} else {
		end = slotRawInt(d);
	}

	if(end > len)
		end = len;

	if(end - start <= 0) {
		SetFalse(a);
		return errNone;
	}

	int stringlen = end - start;
	char *string = (char*)malloc(stringlen + 1);
	memcpy(string, (char*)(slotRawString(b)->s) + start, stringlen);
	string[stringlen] = 0;

	char *pattern = (char*)malloc(slotRawObject(a)->size + 1);
	err = slotStrVal(a, pattern, slotRawObject(a)->size + 1);
	if (err) {
		free(string);
		free(pattern);
		return err;
	}

	int res = matchRegexp(string, pattern);
	free(string);
	free(pattern);

	switch (res) {
		 case 0 : SetTrue(a); break;
		 case 1 : SetFalse(a); break;
		 default : return errFailed;
	}

	return errNone;
}
#endif

int memcmpi(char *a, char *b, int len)
{
	for (int i=0; i<len; ++i) {
		char aa = toupper(a[i]);
		char bb = toupper(b[i]);
		if (aa < bb) return -1;
		if (aa > bb) return 1;
	}
	return 0;
}

int prStringCompare(struct VMGlobals *g, int numArgsPushed);
int prStringCompare(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	int cmp, length;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	if (NotObj(b) || !isKindOf(slotRawObject(b), class_string)) {
		SetNil(a);
		return errNone;
	}
	length = sc_min(slotRawObject(a)->size, slotRawObject(b)->size);
	if (IsTrue(c)) cmp = memcmpi(slotRawString(a)->s, slotRawString(b)->s, length);
	else cmp = memcmp(slotRawString(a)->s, slotRawString(b)->s, length);
	if (cmp == 0) {
		if (slotRawObject(a)->size < slotRawObject(b)->size) cmp = -1;
		else if (slotRawObject(a)->size > slotRawObject(b)->size) cmp = 1;
	}
	SetInt(a, cmp);
	return errNone;
}

int prStringHash(struct VMGlobals *g, int numArgsPushed);
int prStringHash(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	int hash = Hash(slotRawString(a)->s, slotRawString(a)->size);
	SetInt(a, hash);
	return errNone;
}

#ifndef SC_WIN32
#include <glob.h>

int prStringPathMatch(struct VMGlobals *g, int numArgsPushed);
int prStringPathMatch(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char pattern[1024];
	int err = slotStrVal(a, pattern, 1023);
	if (err) return err;

	glob_t pglob;

	int gflags = GLOB_MARK | GLOB_TILDE;
#ifdef SC_DARWIN
	gflags |= GLOB_QUOTE;
#endif

	int gerr = glob(pattern, gflags, NULL, &pglob);
	if (gerr) {
		pglob.gl_pathc = 0;
	}
	PyrObject* array = newPyrArray(g->gc, pglob.gl_pathc, 0, true);
	SetObject(a, array);
	if (gerr) return errNone;

	for (unsigned int i=0; i<pglob.gl_pathc; ++i) {
		PyrObject *string = (PyrObject*)newPyrString(g->gc, pglob.gl_pathv[i], 0, true);
		SetObject(array->slots+i, string);
		g->gc->GCWrite(array, string);
		array->size++;
	}

	globfree(&pglob);

	return errNone;
}
#else //#ifndef SC_WIN32
int prStringPathMatch(struct VMGlobals *g, int numArgsPushed);

int prStringPathMatch(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char pattern[1024];
	int err = slotStrVal(a, pattern, 1023);
	if (err) return err;

  win32_ReplaceCharInString(pattern,1024,'/','\\');
  // Remove trailing slash if found, to allow folders to be matched
  if(pattern[strlen(pattern)-1]=='\\'){
	  pattern[strlen(pattern)-1] = 0;
  }
  // extract the containing folder, including backslash
  char folder[1024];
  win32_ExtractContainingFolder(folder,pattern,1024);

  ///////// PASS 1

  WIN32_FIND_DATA findData;
  HANDLE hFind;
  int nbPaths = 0;

  hFind = ::FindFirstFile(pattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    nbPaths = 0;
  }

  if (hFind == INVALID_HANDLE_VALUE) {
	// This is what happens when no matches. So we create an empty array to return.
    PyrObject* array = newPyrArray(g->gc, 0, 0, true);
	SetObject(a, array);
    return errNone;
  }

  do {
	  if(strcmp(findData.cFileName, "..")!=0 && strcmp(findData.cFileName, "..")!=0){
	    nbPaths++;
	  }
  } while( ::FindNextFile(hFind, &findData));
  ::FindClose(hFind);

  // PASS 2

  hFind = ::FindFirstFile(pattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    nbPaths = 0;
  }

  PyrObject* array = newPyrArray(g->gc, nbPaths , 0, true);
  SetObject(a, array);
  if (hFind == INVALID_HANDLE_VALUE) {
    return errNone;
  }

  int i = 0;
  do {
	if(strcmp(findData.cFileName, "..")!=0 && strcmp(findData.cFileName, ".")!=0){
      std::string strPath(folder);
      strPath += std::string(findData.cFileName);
  	  if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		strPath += std::string("\\"); // Append trailing slash, to match behaviour on unix (used by sclang to detect folderness)
	  }
      const char* fullPath = strPath.c_str();
	  PyrObject *string = (PyrObject*)newPyrString(g->gc, fullPath, 0, true);
	  SetObject(array->slots+i, string);
	  g->gc->GCWrite(array, string);
	  array->size++;
      i++;
	}
  } while( ::FindNextFile(hFind, &findData));
  ::FindClose(hFind);
	return errNone;
}
#endif //#ifndef SC_WIN32

int prString_Getenv(struct VMGlobals* g, int numArgsPushed);
int prString_Getenv(struct VMGlobals* g, int /* numArgsPushed */)
{
	PyrSlot* arg = g->sp;
	char key[256];
	char* value;
	int err;

	err = slotStrVal(arg, key, 256);
	if (err) return err;

	value = getenv(key);

	if (value) {
		PyrString* pyrString = newPyrString(g->gc, value, 0, true);
		if (!pyrString) return errFailed;
		SetObject(arg, pyrString);
	} else {
		SetNil(arg);
	}

	return errNone;
}

int prString_Setenv(struct VMGlobals* g, int numArgsPushed);
int prString_Setenv(struct VMGlobals* g, int /* numArgsPushed */)
{
	PyrSlot* args = g->sp - 1;
	char key[256];
	int err;

	err = slotStrVal(args+0, key, 256);
	if (err) return err;

	if (IsNil(args+1)) {
#ifdef SC_WIN32
		SetEnvironmentVariable(key,NULL);
#else
		unsetenv(key);
#endif
	} else {
		char value[1024];
		err = slotStrVal(args+1, value, 1024);
		if (err) return err;
#ifdef SC_WIN32
		SetEnvironmentVariable(key, value);
#else
		setenv(key, value, 1);
#endif
	}

	return errNone;
}

int prStripRtf(struct VMGlobals *g, int numArgsPushed);
int prStripRtf(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	int len = slotRawObject(a)->size;
	char * chars = (char*)malloc(len + 1);
	memcpy(chars, slotRawString(a)->s, len);
	chars[len] = 0;
	rtf2txt(chars);

	PyrString* string = newPyrString(g->gc, chars, 0, false);
	SetObject(a, string);
	free(chars);

	return errNone;
}

int prStripHtml(struct VMGlobals *g, int numArgsPushed);
int prStripHtml(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	int len = slotRawObject(a)->size;
	char * chars = (char*)malloc(len + 1);
	memcpy(chars, slotRawString(a)->s, len);
	chars[len] = 0;
	html2txt(chars);

	PyrString* string = newPyrString(g->gc, chars, 0, false);
	SetObject(a, string);
	free(chars);

	return errNone;
}


int prString_GetResourceDirPath(struct VMGlobals *g, int numArgsPushed);
int prString_GetResourceDirPath(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char * chars = (char*)malloc(MAXPATHLEN - 32);
	sc_GetResourceDirectory(chars, MAXPATHLEN - 32);

	PyrString* string = newPyrString(g->gc, chars, 0, false);
	SetObject(a, string);
	free(chars);

	return errNone;
}


int prString_Find(struct VMGlobals *g, int numArgsPushed);
int prString_Find(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 3; // source string
	PyrSlot *b = g->sp - 2; // search string
	PyrSlot *c = g->sp - 1; // ignoreCase
	PyrSlot *d = g->sp;		// offset

	int offset;
	int err = slotIntVal(d, &offset);
	if (err) return err;

	if (!isKindOfSlot(b, class_string)) {
		SetNil(a);
		return errNone;
	}

	int alength = slotRawObject(a)->size - offset;
	int blength = slotRawObject(b)->size;

	if ((alength <= 0)
		|| (blength == 0)
			// should also return nil if search string is longer than source
		|| (blength > alength))
	{
		SetNil(a);
		return errNone;
	}

	int cmp = 1;	// assume contains will be false
	char *achar = slotRawString(a)->s + offset;
	char *bchar = slotRawString(b)->s;
	char bchar0 = bchar[0];
	int scanlength = alength - blength;
	if (IsTrue(c)) {
		bchar0 = toupper(bchar0);
		for (int i=0; i <= scanlength; ++i, ++achar) {
			if (toupper(*achar) == bchar0) {
				cmp = memcmpi(achar+1, bchar+1, blength-1);
				if (cmp == 0) break;
			}
		}
	} else {
		for (int i=0; i <= scanlength; ++i, ++achar) {
			if (*achar == bchar0) {
				cmp = memcmp(achar+1, bchar+1, blength-1);
				if (cmp == 0) break;
			}
		}
	}
	if (cmp == 0) {
		SetInt(a, achar - slotRawString(a)->s);
	} else {
		SetNil(a);
	}
	return errNone;
}

int prString_FindBackwards(struct VMGlobals *g, int numArgsPushed);
int prString_FindBackwards(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 3; // source string
	PyrSlot *b = g->sp - 2; // search string
	PyrSlot *c = g->sp - 1; // ignoreCase
	PyrSlot *d = g->sp;		// offset

	int offset;
	int err = slotIntVal(d, &offset);
	if (err) return err;

	if (!isKindOfSlot(b, class_string)) {
		SetNil(a);
		return errNone;
	}

	int alength = sc_min(offset + 1, slotRawObject(a)->size);
	int blength = slotRawObject(b)->size;

	if ((alength <= 0)
		|| (blength == 0)
			// should also return nil if search string is longer than source
		|| (blength > alength))
	{
		SetNil(a);
		return errNone;
	}

	int cmp = 1;	// assume contains will be false
	char *achar = slotRawString(a)->s + (alength - blength);
	char *bchar = slotRawString(b)->s;
	char bchar0 = bchar[0];
	int scanlength = alength - blength;
	if (IsTrue(c)) {
		bchar0 = toupper(bchar0);
		for (int i=scanlength; i >= 0; --i, --achar) {
			if (toupper(*achar) == bchar0) {
				cmp = memcmpi(achar+1, bchar+1, blength-1);
				if (cmp == 0) break;
			}
		}
	} else {
		for (int i=scanlength; i >= 0; --i, --achar) {
			if (*achar == bchar0) {
				cmp = memcmp(achar+1, bchar+1, blength-1);
				if (cmp == 0) break;
			}
		}
	}
	if (cmp == 0) {
		SetInt(a, achar - slotRawString(a)->s);
	} else {
		SetNil(a);
	}
	return errNone;
}

#if SC_DARWIN
# include <CoreFoundation/CoreFoundation.h>
#endif // SC_DARWIN

int prString_StandardizePath(struct VMGlobals* g, int numArgsPushed);
int prString_StandardizePath(struct VMGlobals* g, int /* numArgsPushed */)
{
	PyrSlot* arg = g->sp;
	char ipath[PATH_MAX];
	char opathbuf[PATH_MAX];
	char* opath = opathbuf;
	int err;

	err = slotStrVal(arg, ipath, PATH_MAX);
	if (err) return err;

	if (!sc_StandardizePath(ipath, opath)) {
		opath = ipath;
	}

#if SC_DARWIN
	CFStringRef cfstring =
		CFStringCreateWithCString(NULL,
								  opath,
								  kCFStringEncodingUTF8);
	err = !CFStringGetFileSystemRepresentation(cfstring, opath, PATH_MAX);
	CFRelease(cfstring);
	if (err) return errFailed;
#endif // SC_DARWIN

	PyrString* pyrString = newPyrString(g->gc, opath, 0, true);
	SetObject(arg, pyrString);

	return errNone;
}

void initStringPrimitives();
void initStringPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_StringCompare", prStringCompare, 3, 0);
	definePrimitive(base, index++, "_StringHash", prStringHash, 1, 0);
	definePrimitive(base, index++, "_StringPathMatch", prStringPathMatch, 1, 0);
	definePrimitive(base, index++, "_StringAsSymbol", prStringAsSymbol, 1, 0);
	definePrimitive(base, index++, "_String_AsInteger", prString_AsInteger, 1, 0);
	definePrimitive(base, index++, "_String_AsFloat", prString_AsFloat, 1, 0);
	definePrimitive(base, index++, "_String_AsCompileString", prString_AsCompileString, 1, 0);
	definePrimitive(base, index++, "_String_Getenv", prString_Getenv, 1, 0);
    definePrimitive(base, index++, "_String_Setenv", prString_Setenv, 2, 0);
    definePrimitive(base, index++, "_String_Find", prString_Find, 4, 0);
	definePrimitive(base, index++, "_String_FindBackwards", prString_FindBackwards, 4, 0);
    definePrimitive(base, index++, "_String_Format", prString_Format, 2, 0);
#ifndef SC_WIN32
	definePrimitive(base, index++, "_String_Regexp", prString_Regexp, 4, 0);
#endif
	definePrimitive(base, index++, "_StripRtf", prStripRtf, 1, 0);
	definePrimitive(base, index++, "_StripHtml", prStripHtml, 1, 0);
	definePrimitive(base, index++, "_String_GetResourceDirPath", prString_GetResourceDirPath, 1, 0);
	definePrimitive(base, index++, "_String_StandardizePath", prString_StandardizePath, 1, 0);
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
	initStringPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn()
{
	return new APlugIn();
}

#endif
