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

#include "SCBase.h"
#include "PyrParseNode.h"
#include "PyrKernel.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <optional>
#include <string>

#ifdef _WIN32
#    define PATH_MAX _MAX_PATH
#endif

#define DUMPNODE(node, level)                                                                                          \
    do {                                                                                                               \
        if (node)                                                                                                      \
            (node)->dump(level);                                                                                       \
    } while (false);

void dumpNodeList(PyrParseNode* node) {
    for (; node; node = node->mNext) {
        DUMPNODE(node, 0);
    }
}

void PyrCurryArgNode::dump(int level) { postfl("%2d CurryArg %d\n", level, mArgNum); }

void PyrSlotNode::dump(int level) {
    if (mClassno == PyrParseNodeType::PushLitNode)
        dumpPushLit(level);
    else if (mClassno == PyrParseNodeType::PushNameNode)
        postfl("%2d PushName '%s'\n", level, slotRawSymbol(&mSlot)->name);
    else if (mClassno == PyrParseNodeType::LiteralNode)
        dumpLiteral(level);
    else {
        postfl("%2d SlotNode\n", level);
        dumpPyrSlot(&mSlot);
    }
    DUMPNODE(mNext, level);
}

void PyrPushKeyArgNode::dump(int level) {
    postfl("%2d PushKeyArgNode\n", level);
    DUMPNODE(mSelector, level + 1);
    DUMPNODE(mExpr, level + 1);
    DUMPNODE(mNext, level);
}

void PyrClassExtNode::dump(int level) {
    postfl("%2d ClassExt '%s'\n", level, slotRawSymbol(&mClassName->mSlot)->name);
    DUMPNODE(mMethods, level + 1);
    DUMPNODE(mNext, level);
}

void PyrClassNode::dump(int level) {
    postfl("%2d Class '%s'\n", level, slotRawSymbol(&mClassName->mSlot)->name);
    DUMPNODE(mSuperClassName, level + 1);
    DUMPNODE(mVarlists, level + 1);
    DUMPNODE(mMethods, level + 1);
    DUMPNODE(mNext, level);
}

void PyrMethodNode::dump(int level) {
    postfl("%2d MethodNode '%s'  %s\n", level, slotRawSymbol(&mMethodName->mSlot)->name,
           mPrimitiveName ? slotRawSymbol(&mPrimitiveName->mSlot)->name : "");
    DUMPNODE(mArglist, level + 1);
    DUMPNODE(mBody, level + 1);
    DUMPNODE(mNext, level);
}

void PyrArgListNode::dump(int level) {
    postfl("%2d ArgList\n", level);
    DUMPNODE(mVarDefs, level + 1);
    DUMPNODE(mRest, level + 1);
    DUMPNODE(mKeywordArgs, level + 1);
    DUMPNODE(mNext, level);
}

void PyrVarListNode::dump(int level) {
    postfl("%2d VarList\n", level);
    DUMPNODE(mVarDefs, level + 1);
    DUMPNODE(mNext, level);
}

void PyrVarDefNode::dump(int level) {
    postfl("%2d VarDef '%s'\n", level, slotRawSymbol(&mVarName->mSlot)->name);
    DUMPNODE(mDefVal, level);
    DUMPNODE(mNext, level);
}

void PyrCallNode::dump(int level) {
    postfl("%2d Call '%s'\n", level, slotRawSymbol(&mSelector->mSlot)->name);
    DUMPNODE(mArglist, level + 1);
    DUMPNODE(mKeyarglist, level + 1);
    DUMPNODE(mNext, level);
}

void PyrBinopCallNode::dump(int level) {
    postfl("%2d BinopCall '%s'\n", level, slotRawSymbol(&mSelector->mSlot)->name);
    DUMPNODE(mArglist, level + 1);
    DUMPNODE(mNext, level);
}

void PyrDropNode::dump(int level) {
    postfl("%2d Drop (\n", level);
    DUMPNODE(mExpr1, level + 1);
    postfl(" -> %2d Drop\n", level);
    DUMPNODE(mExpr2, level + 1);
    postfl(") %2d Drop\n", level);
    DUMPNODE(mNext, level);
}

void PyrSlotNode::dumpPushLit(int level) {
    postfl("%2d PushLit\n", level);
    if (!IsPtr(&mSlot))
        dumpPyrSlot(&mSlot);
    else {
        DUMPNODE((PyrParseNode*)slotRawObject(&mSlot), level);
    }
}

void PyrSlotNode::dumpLiteral(int level) {
    postfl("%2d Literal\n", level);
    if (!IsPtr(&mSlot))
        dumpPyrSlot(&mSlot);
    else {
        DUMPNODE((PyrParseNode*)slotRawObject(&mSlot), level);
    }
}

void PyrReturnNode::dump(int level) {
    postfl("%2d Return (\n", level);
    DUMPNODE(mExpr, level + 1);
    postfl(") %2d Return \n", level);
    DUMPNODE(mNext, level);
}

void PyrBlockReturnNode::dump(int level) {
    postfl("%2d FuncReturn\n", level);
    DUMPNODE(mNext, level);
}

void PyrAssignNode::dump(int level) {
    postfl("%2d Assign '%s'\n", level, slotRawSymbol(&mVarName->mSlot)->name);
    DUMPNODE(mVarName, level + 1);
    DUMPNODE(mExpr, level + 1);
    DUMPNODE(mNext, level);
}

void PyrSetterNode::dump(int level) {
    // postfl("%2d Assign '%s'\n", level, slotRawSymbol(&mVarName->mSlot)->name);
    DUMPNODE(mSelector, level + 1);
    DUMPNODE(mExpr1, level + 1);
    DUMPNODE(mExpr2, level + 1);
}

void PyrMultiAssignNode::dump(int level) {
    postfl("%2d MultiAssign\n", level);
    DUMPNODE(mVarList, level + 1);
    DUMPNODE(mExpr, level + 1);
    DUMPNODE(mNext, level);
}

void PyrMultiAssignVarListNode::dump(int level) {
    postfl("%2d MultiAssignVarList\n", level);
    DUMPNODE(mVarNames, level + 1);
    DUMPNODE(mRest, level + 1);
    DUMPNODE(mNext, level);
}

void PyrDynDictNode::dump(int level) {
    postfl("%2d DynDict\n", level);
    DUMPNODE(mElems, level + 1);
    DUMPNODE(mNext, level);
}

void PyrDynListNode::dump(int level) {
    postfl("%2d DynList\n", level);
    DUMPNODE(mElems, level + 1);
    DUMPNODE(mNext, level);
}

void PyrLitListNode::dump(int level) {
    postfl("%2d LitList\n", level);
    postfl(" %2d mElems\n", level);
    DUMPNODE(mElems, level + 1);
    postfl(" %2d mNext\n", level);
    DUMPNODE(mNext, level);
}

void PyrBlockNode::dump(int level) {
    postfl("%2d Func\n", level);
    DUMPNODE(mArglist, level + 1);
    DUMPNODE(mBody, level + 1);
    DUMPNODE(mNext, level);
}

static void appendf(std::string& out, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list argsCopy;
    va_copy(argsCopy, args);
    const int size = vsnprintf(nullptr, 0, fmt, argsCopy);
    va_end(argsCopy);
    if (size > 0) {
        const auto oldSize = out.size();
        out.resize(oldSize + size + 1);
        vsnprintf(out.data() + oldSize, size + 1, fmt, args);
        out.resize(oldSize + size);
    }
    va_end(args);
}

static void appendClassName(std::string& out, PyrClass* classobj) { out += slotRawSymbol(&classobj->name)->view(); }

static void appendMethodName(std::string& out, PyrMethod* method) {
    appendClassName(out, slotRawClass(&method->ownerclass));
    out += ':';
    out += slotRawSymbol(&method->name)->view();
}

static void appendSymbol(std::string& out, const PyrSymbol* sym) {
    if (sym->length > 240) {
        out += sym->view().substr(0, 240);
        out += "...";
    } else {
        out += sym->view();
    }
}

void dumpPyrSlot(PyrSlot* slot) { post("   %s\n", slotString(slot).c_str()); }

std::string slotString(PyrSlot* slot) {
    std::string str;
    switch (GetTag(slot)) {
    case tagInt:
        appendf(str, "Integer %d", slotRawInt(slot));
        break;
    case tagChar:
        appendf(str, "Character %d '%c'", static_cast<int>(slotRawChar(slot)), static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        str += "Symbol '";
        appendSymbol(str, slotRawSymbol(slot));
        str += "'";
        break;
    case tagObj:
        if (slotRawObject(slot)) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class) {
                str += "class ";
                appendClassName(str, (PyrClass*)slotRawObject(slot));
                appendf(str, " (%p)", slotRawObject(slot));
            } else if (classptr == class_string) {
                const PyrObject* obj = slotRawObject(slot);
                const char* chars = (const char*)obj->slots;
                str += '"';
                if (obj->size > 47) {
                    str.append(chars, 44);
                    str += "...";
                } else {
                    str.append(chars, obj->size);
                }
                str += '"';
            } else if (classptr == class_method) {
                str += "instance of Method ";
                appendMethodName(str, slotRawMethod(slot));
                appendf(str, " (%p)", slotRawMethod(slot));
            } else if (classptr == class_fundef) {
                PyrSlot *context, *nextcontext;
                // find function's method
                nextcontext = &slotRawBlock(slot)->contextDef;
                if (NotNil(nextcontext)) {
                    do {
                        context = nextcontext;
                        nextcontext = &slotRawBlock(context)->contextDef;
                    } while (NotNil(nextcontext));
                    if (isKindOf(slotRawObject(context), class_method)) {
                        str += "instance of FunctionDef in Method ";
                        appendMethodName(str, slotRawMethod(context));
                    } else {
                        str += "instance of FunctionDef in closed FunctionDef";
                    }
                } else {
                    str += "instance of FunctionDef - closed";
                }
            } else if (classptr == class_frame) {
                if (!slotRawFrame(slot)) {
                    appendf(str, "Frame (%0X)", slotRawInt(slot));
                } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
                    appendf(str, "Frame (%p) of ", slotRawObject(slot));
                    appendMethodName(str, slotRawMethod(&slotRawFrame(slot)->method));
                } else {
                    appendf(str, "Frame (%p) of Function", slotRawFrame(slot));
                }
            } else {
                str += "instance of ";
                appendClassName(str, classptr);
                appendf(str, " (%p, size=%d, set=%d)", slotRawObject(slot), slotRawObject(slot)->size,
                        slotRawObject(slot)->obj_sizeclass);
            }
        } else {
            str += "NULL Object Pointer";
        }
        break;
    case tagNil:
        str += "nil";
        break;
    case tagFalse:
        str += "false";
        break;
    case tagTrue:
        str += "true";
        break;
    case tagPtr:
        appendf(str, "RawPointer %p", slotRawPtr(slot));
        break;
    default: {
        union {
            int32 i[2];
            double f;
        } u;
        u.f = slotRawFloat(slot);
        appendf(str, "Float %f   %08X %08X", u.f, u.i[0], u.i[1]);
        break;
    }
    }
    return str;
}

static std::string printObject(PyrSlot* slot, PyrObject* obj) {
    assert(obj);
    std::string str;
    PyrClass* classptr = obj->classptr;
    if (classptr == class_class) {
        str += "class ";
        appendClassName(str, (PyrClass*)obj);
    } else if (classptr == class_string) {
        const char* chars = (const char*)obj->slots;
        str += '"';
        if (obj->size > 31) {
            str.append(chars, 28);
            str += "...";
        } else {
            str.append(chars, obj->size);
        }
        str += '"';
    } else if (classptr == class_method) {
        appendMethodName(str, slotRawMethod(slot));
    } else if (classptr == class_fundef) {
        PyrSlot *context, *nextcontext;
        // find function's method
        nextcontext = &slotRawBlock(slot)->contextDef;
        if (NotNil(nextcontext)) {
            do {
                context = nextcontext;
                nextcontext = &slotRawBlock(context)->contextDef;
            } while (NotNil(nextcontext));
            if (isKindOf(slotRawObject(context), class_method)) {
                str += "< FunctionDef in Method ";
                appendMethodName(str, slotRawMethod(context));
                str += " >";
            } else {
                str += "< FunctionDef in closed FunctionDef >";
            }
        } else {
            str += "< closed FunctionDef >";
        }
    } else if (classptr == class_frame) {
        if (!slotRawFrame(slot)) {
            str += "Frame (null)";
        } else if (!slotRawBlock(&slotRawFrame(slot)->method)) {
            str += "Frame (null method)";
        } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
            appendf(str, "Frame (%p) of ", obj);
            appendMethodName(str, slotRawMethod(&slotRawFrame(slot)->method));
        } else {
            appendf(str, "Frame (%p) of Function", obj);
        }
    } else if (classptr == class_array) {
        appendf(str, "[*%d]", obj->size);
    } else {
        str += "<instance of ";
        appendClassName(str, classptr);
        str += ">";
    }
    return str;
}

// Assumed: str has enough space to hold the representation of d
static void prettyFormatFloat(char* str, double d) {
    if (std::isnan(d)) {
        // Do this manually as different implementations disagree on how to format nan.
        str[0] = 'n';
        str[1] = 'a';
        str[2] = 'n';
        str[3] = '\0';
        return;
    } else if (std::isinf(d)) {
        if (d > 0.0) {
            str[0] = 'i';
            str[1] = 'n';
            str[2] = 'f';
            str[3] = '\0';
        } else {
            str[0] = '-';
            str[1] = 'i';
            str[2] = 'n';
            str[3] = 'f';
            str[4] = '\0';
        }
        return;
    }
    sprintf(str, "%.14g", d);

    // append a trailing '.0' if the number would look like an integer.
    for (; *str; ++str) {
        if ((*str < '0' || *str > '9') && *str != '-')
            return;
    }

    str[0] = '.';
    str[1] = '0';
    str[2] = '\0';
}

static void appendPrettyFloat(std::string& out, double d) {
    char str[64];
    prettyFormatFloat(str, d);
    out += str;
}

std::string slotOneWord(PyrSlot* slot) {
    std::string str;
    switch (GetTag(slot)) {
    case tagInt:
        appendf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        appendf(str, "$%c", static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        str += "'";
        appendSymbol(str, slotRawSymbol(slot));
        str += "'";
        break;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj)
            str = printObject(slot, slotObj);
        else
            str += "NULL Object Pointer";
        break;
    }
    case tagNil:
        str += "nil";
        break;
    case tagFalse:
        str += "false";
        break;
    case tagTrue:
        str += "true";
        break;
    case tagPtr:
        appendf(str, "ptr%p", slotRawPtr(slot));
        break;
    default:
        appendPrettyFloat(str, slotRawFloat(slot));
        break;
    }
    return str;
}

std::optional<std::string> postString(PyrSlot* slot) {
    std::string str;
    switch (GetTag(slot)) {
    case tagInt:
        appendf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        appendf(str, "%c", slotRawChar(slot));
        break;
    case tagSym:
        return std::nullopt;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class || classptr == class_method || classptr == class_fundef
                || classptr == class_frame)
                str = printObject(slot, slotObj);
            else
                return std::nullopt;
        } else
            str += "NULL Object Pointer";
        break;
    }
    case tagNil:
        str += "nil";
        break;
    case tagFalse:
        str += "false";
        break;
    case tagTrue:
        str += "true";
        break;
    case tagPtr:
        appendf(str, "%p", slotRawPtr(slot));
        break;
    default:
        appendPrettyFloat(str, slotRawFloat(slot));
        break;
    }
    return str;
}


int asCompileString(PyrSlot* slot, char* str) {
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar: {
        int c = slotRawChar(slot);
        if (isprint(c)) {
            sprintf(str, "$%c", c);
        } else {
            switch (c) {
            case '\n':
                strcpy(str, "$\\n");
                break;
            case '\r':
                strcpy(str, "$\\r");
                break;
            case '\t':
                strcpy(str, "$\\t");
                break;
            case '\f':
                strcpy(str, "$\\f");
                break;
            case '\v':
                strcpy(str, "$\\v");
                break;
            default:
                sprintf(str, "%d.asAscii", c);
            }
        }
        break;
    }
    case tagSym:
        return errFailed;
    case tagObj:
        return errFailed;
    case tagNil:
        sprintf(str, "nil");
        break;
    case tagFalse:
        sprintf(str, "false");
        break;
    case tagTrue:
        sprintf(str, "true");
        break;
    case tagPtr:
        strcpy(str, "/*Ptr*/ nil");
        break;
    default:
        sprintf(str, "%f", slotRawFloat(slot));
        break;
    }
    return errNone;
}


void stringFromPyrString(PyrString* obj, char* str, int maxlength);
void stringFromPyrString(PyrString* obj, char* str, int maxlength) {
    if (obj->classptr == class_string) {
        int len;
        if (obj->size > maxlength - 4) {
            memcpy(str, obj->s, maxlength - 4);
            str[maxlength - 4] = '.';
            str[maxlength - 3] = '.';
            str[maxlength - 2] = '.';
            str[maxlength - 1] = 0;
        } else {
            len = sc_min(maxlength - 1, obj->size);
            memcpy(str, obj->s, len);
            str[len] = 0;
        }
    } else {
        sprintf(str, "not a string");
    }
}

void pstrncpy(unsigned char* s1, unsigned char* s2, int n) {
    int i, m;
    m = *s2++;
    n = (n < m) ? n : m;
    *s1 = n;
    s1++;
    for (i = 0; i < n; ++i) {
        *s1 = *s2;
        s1++;
        s2++;
    }
}
void pstringFromPyrString(PyrString* obj, unsigned char* str, int maxlength) {
    static const char not_a_string[] = "not a string";
    const char* src;
    int len;
    if (obj && obj->classptr == class_string) {
        len = sc_min(maxlength - 1, obj->size);
        src = obj->s;
    } else {
        len = sizeof(not_a_string);
        src = not_a_string;
    }
    memcpy(str + 1, src, len);
    str[0] = len;
}
