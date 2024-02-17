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

#include <cstring>
#include <cstdlib>
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "VMGlobals.h"
#include "PyrKernel.h"
#include "SCBase.h"


// small symbol size optimization.
static constexpr size_t small_symbol_size = 16;
static constexpr size_t medium_symbol_size = 256; // the value used before
static_assert(small_symbol_size < medium_symbol_size);

template <size_t FreeCharacters, class WithCharArray, class WithStdString>
auto do_with_copy_sym_string_with_free_characters(const PyrSymbol& sym, WithCharArray&& with_char_array,
                                                  WithStdString&& with_std_string) {
    static_assert(std::is_invocable<WithCharArray, char*, decltype(sym.length)>::value,
                  "WithCharArray must be invocable with a char* and length of the symbol.\n");

    static_assert(std::is_invocable<WithStdString, std::string&>::value,
                  "WithStdString must be invocable with a std::string.\n");

    static_assert(std::is_same_v<std::invoke_result_t<WithCharArray, char*, decltype(sym.length)>,
                                 std::invoke_result_t<WithStdString, std::string&>>,
                  "WithCharArray must return the same as WithStdString.\n");

    if (sym.length <= small_symbol_size - FreeCharacters) {
        char setter[small_symbol_size];
        memcpy(setter, sym.name, sym.length);
        return std::forward<WithCharArray>(with_char_array)(setter, sym.length);
    } else if (sym.length <= medium_symbol_size - FreeCharacters) {
        char setter[medium_symbol_size];
        memcpy(setter, sym.name, sym.length);
        return std::forward<WithCharArray>(with_char_array)(setter, sym.length);
    } else {
        std::string setter(sym.name);
        return std::forward<WithStdString>(with_std_string)(setter);
    }
}
template <class WithCharArray, class WithStdString>
void do_with_copy_sym_string(const PyrSymbol& sym, WithCharArray&& with_char_array, WithStdString&& with_std_string) {
    do_with_copy_sym_string_with_free_characters<0>(sym, std::forward<WithCharArray>(with_char_array),
                                                    std::forward<WithStdString>(with_std_string));
}

int prSymbolIsPrefix(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    if (!IsSym(a) || !IsSym(b))
        return errWrongType;
    const auto alen = slotRawSymbol(a)->length;
    const auto blen = slotRawSymbol(b)->length;
    const auto length = sc_min(alen, blen);
    if (memcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name, length) == 0) {
        SetTrue(a);
    } else {
        SetFalse(a);
    }
    return errNone;
}

int prSymbolClass(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    if (slotRawSymbol(a)->flags & sym_Class) {
        PyrClass* classobj = slotRawSymbol(a)->u.classobj;
        if (classobj)
            SetObject(a, classobj);
        else
            SetNil(a);
    } else {
        SetNil(a);
    }
    return errNone;
}

int prSymbolIsSetter(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    if (slotRawSymbol(a)->flags & sym_Setter)
        SetTrue(a);
    else
        SetFalse(a);
    return errNone;
}

int prSymbolAsSetter(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    // note the pointer aliasing here
    PyrSymbol& sym = *slotRawSymbol(a);
    if (!(sym.flags & sym_Setter)) {
        if ((sym.flags & sym_Class) || (sym.flags & sym_Primitive)) {
            error("Cannot convert class names or primitive names to setters.\n");
            return errFailed;
        }

        do_with_copy_sym_string_with_free_characters<1>(
            sym,
            [&](char* setter, size_t len) {
                setter[len] = '_';
                setter[len + 1] = '\0';
                SetRaw(a, getsym(setter));
            },
            [&](std::string setter) {
                setter += '_';
                SetRaw(a, getsym(setter.c_str()));
            });
    }
    return errNone;
}

int prSymbolAsGetter(struct VMGlobals* g, int numArgsPushed) {
    // chops off the underscore if present
    PyrSlot* a = g->sp;
    // note the pointer aliasing here
    const PyrSymbol& sym = *slotRawSymbol(a);

    if ((sym.flags & sym_Setter)) {
        if ((sym.flags & sym_Class) || (sym.flags & sym_Primitive)) {
            error("Cannot convert class names or primitive names to getters.\n");
            return errFailed;
        }
        do_with_copy_sym_string(
            sym,
            [&](char* c, size_t len) {
                c[len - 1] = '\0';
                SetRaw(a, getsym(c));
            },
            [&](std::string c) {
                c.erase(c.size());
                SetRaw(a, getsym(c.c_str()));
            });
    }
    return errNone;
}

int prSymbolIsClassName(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    if (slotRawSymbol(a)->flags & sym_Class)
        SetTrue(a);
    else
        SetFalse(a);

    return errNone;
}

int prSymbolIsMetaClassName(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    if (slotRawSymbol(a)->flags & sym_MetaClass)
        SetTrue(a);
    else
        SetFalse(a);
    return errNone;
}


int prSymbol_IsBinaryOp(struct VMGlobals* g, int numArgsPushed) {
    static const char* binary_op_characters = "!@%&*-+=|<>?/";
    PyrSlot* a = g->sp;
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
            const auto len = slotRawSymbol(a)->length;
            SetTrue(a);
            for (std::remove_const<decltype(len)>::type i = 0; i < len; i++) {
                const bool character_is_binary_operator = strchr(binary_op_characters, str[i]);
                if (!character_is_binary_operator) {
                    SetFalse(a);
                    break;
                }
            }
        }
    }

    return errNone;
}

int prSymbol_IsIdentifier(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    const char* str = slotRawSymbol(a)->name;

    // all symbols are valid c string, so they have at least a size of 1
    if (!std::islower(static_cast<unsigned char>(str[0]))) {
        SetFalse(a);
        return errNone;
    }
    const auto len = slotRawSymbol(a)->length;
    const bool all_chars_are_valid = [&]() -> bool {
        for (std::remove_const<decltype(len)>::type i = 1; i < len; i++) {
            const char c = str[i];
            if (!(std::isalnum(c) or c == '_')) {
                return false;
            }
        }
        return true;
    }();
    if (all_chars_are_valid)
        SetTrue(a);
    else
        SetFalse(a);

    return errNone;
}

int prSymbol_AsInteger(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    const char* str = slotRawSymbol(a)->name;
    SetInt(a, atoi(str));
    return errNone;
}

int prSymbol_PrimitiveIndex(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetInt(a, slotRawSymbol(a)->u.index);
    return errNone;
}

int prSymbol_SpecialIndex(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetInt(a, slotRawSymbol(a)->specialIndex);
    return errNone;
}


int prSymbol_AsFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    const char* str = slotRawSymbol(a)->name;
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
 *      is written as two characters separated with a hyphen: a-z denotes
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

inline bool lo_pattern_match(const char* str, const char* p) {
    int negate;
    int match;
    char c;

    while (*p) {
        if (!*str && *p != '*')
            return false;

        switch (c = *p++) {
        case '*':
            while (*p == '*')
                p++;

            if (!*p)
                return true;

            //                if (*p != '?' && *p != '[' && *p != '\\')
            if (*p != '?' && *p != '[' && *p != '{')
                while (*str && *p != *str)
                    str++;

            while (*str) {
                if (lo_pattern_match(str, p))
                    return true;
                str++;
            }
            return false;

        case '?':
            if (*str)
                break;
            return false;
            /*
             * set specification is inclusive, that is [a-z] is a, z and
             * everything in between. this means [z-a] may be interpreted
             * as a set that contains z, a and nothing in between.
             */
        case '[':
            if (*p != '!')
                negate = false;
            else {
                negate = true;
                p++;
            }

            match = false;

            while (!match && (c = *p++)) {
                if (!*p)
                    return false;
                if (*p == '-') { /* c-c */
                    if (!*++p)
                        return false;
                    if (*p != ']') {
                        if (*str == c || *str == *p || (*str > c && *str < *p))
                            match = true;
                    } else { /* c-] */
                        if (*str >= c)
                            match = true;
                        break;
                    }
                } else { /* cc or c] */
                    if (c == *str)
                        match = true;
                    if (*p != ']') {
                        if (*p == *str)
                            match = true;
                    } else
                        break;
                }
            }

            if (negate == match)
                return false;
            /*
             * if there is a match, skip past the cset and continue on
             */
            while (*p && *p != ']')
                p++;
            if (!*p++) /* oops! */
                return false;
            break;

            /*
             * {astring,bstring,cstring}
             */
        case '{': {
            // *p is now first character in the {brace list}
            const char* place = str; // to backtrack
            const char* remainder = p; // to forwardtrack

            // find the end of the brace list
            while (*remainder && *remainder != '}')
                remainder++;
            if (!*remainder++) /* oops! */
                return false;

            c = *p++;

            while (c) {
                if (c == ',') {
                    if (lo_pattern_match(str, remainder)) {
                        return true;
                    } else {
                        // backtrack on test string
                        str = place;
                        // continue testing,
                        // skip comma
                        if (!*p++) // oops
                            return false;
                    }
                } else if (c == '}') {
                    // continue normal pattern matching
                    if (!*p && !*str)
                        return true;
                    str--; // str is incremented again below
                    break;
                } else if (c == *str) {
                    str++;
                    if (!*str && *remainder)
                        return false;
                } else { // skip to next comma
                    str = place;
                    while (*p != ',' && *p != '}' && *p)
                        p++;
                    if (*p == ',')
                        p++;
                    else if (*p == '}') {
                        return false;
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
                return false;
            break;
        }
        str++;
    }

    return !*str;
}

// end: following function lifted from liblo


int prSymbol_matchOSCPattern(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    if (!IsSym(a) || !IsSym(b))
        return errWrongType;
    if (lo_pattern_match(slotRawSymbol(a)->name, slotRawSymbol(b)->name)) {
        SetTrue(a);
    } else {
        SetFalse(a);
    }
    return errNone;
}

int prSymbol_isMap(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    const char* str = slotRawSymbol(a)->name;
    if ((str[0] == 'a' || str[0] == 'c') && std::isdigit(static_cast<unsigned char>(str[1])))
        SetTrue(a);
    else
        SetFalse(a);

    return errNone;
}

void initSymbolPrimitives() {
    int base = nextPrimitiveIndex();
    int index = 0;

    definePrimitive(base, index++, "_SymbolIsPrefix", prSymbolIsPrefix, 2, 0);
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
