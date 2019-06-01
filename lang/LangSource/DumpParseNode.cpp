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
#include "PyrLexer.h"
#include "PyrKernel.h"
#include "Opcodes.h"
#include "PyrPrimitive.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef _WIN32
#    define snprintf _snprintf
#    define PATH_MAX _MAX_PATH
#endif


extern int textpos;

void dumpNodeList(PyrParseNode* node) {
    for (; node; node = node->mNext) {
        DUMPNODE(node, 0);
    }
}

void PyrCurryArgNode::dump(int level) { postfl("%2d CurryArg %d\n", level, mArgNum); }

void PyrSlotNode::dump(int level) {
    if (mClassno == pn_PushLitNode)
        dumpPushLit(level);
    else if (mClassno == pn_PushNameNode)
        postfl("%2d PushName '%s'\n", level, slotRawSymbol(&mSlot)->name);
    else if (mClassno == pn_LiteralNode)
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

void dumpPyrSlot(PyrSlot* slot) {
    char str[1024];
    slotString(slot, str);
    post("   %s\n", str);
}

void slotString(PyrSlot* slot, char* str) {
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "Integer %d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "Character %d '%c'", static_cast<int>(slotRawChar(slot)), static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        if (strlen(slotRawSymbol(slot)->name) > 240) {
            char str2[256];
            memcpy(str2, slotRawSymbol(slot)->name, 240);
            str2[240] = 0;
            snprintf(str, 256, "Symbol '%s...'", str2);
        } else {
            snprintf(str, 256, "Symbol '%s'", slotRawSymbol(slot)->name);
        }
        break;
    case tagObj:
        if (slotRawObject(slot)) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class) {
                sprintf(str, "class %s (%p)", slotRawSymbol(&((PyrClass*)slotRawObject(slot))->name)->name,
                        slotRawObject(slot));
            } else if (classptr == class_string) {
                char str2[48];
                int len;
                if (slotRawObject(slot)->size > 47) {
                    memcpy(str2, (char*)slotRawObject(slot)->slots, 44);
                    str2[44] = '.';
                    str2[45] = '.';
                    str2[46] = '.';
                    str2[47] = 0;
                } else {
                    len = sc_min(47, slotRawObject(slot)->size);
                    memcpy(str2, (char*)slotRawObject(slot)->slots, len);
                    str2[len] = 0;
                }
                sprintf(str, "\"%s\"", str2);
            } else if (classptr == class_method) {
                sprintf(str, "instance of Method %s:%s (%p)",
                        slotRawSymbol(&slotRawClass(&slotRawMethod(slot)->ownerclass)->name)->name,
                        slotRawSymbol(&slotRawMethod(slot)->name)->name, slotRawMethod(slot));
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
                        sprintf(str, "instance of FunctionDef in Method %s:%s",
                                slotRawSymbol(&slotRawClass(&slotRawMethod(context)->ownerclass)->name)->name,
                                slotRawSymbol(&slotRawMethod(context)->name)->name);
                    } else {
                        sprintf(str, "instance of FunctionDef in closed FunctionDef");
                    }
                } else {
                    sprintf(str, "instance of FunctionDef - closed");
                }
            } else if (classptr == class_frame) {
                if (!slotRawFrame(slot)) {
                    sprintf(str, "Frame (%0X)", slotRawInt(slot));
                } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
                    sprintf(str, "Frame (%p) of %s:%s", slotRawObject(slot),
                            slotRawSymbol(&slotRawClass(&slotRawMethod(&slotRawFrame(slot)->method)->ownerclass)->name)
                                ->name,
                            slotRawSymbol(&slotRawMethod(&slotRawFrame(slot)->method)->name)->name);
                } else {
                    sprintf(str, "Frame (%p) of Function", slotRawFrame(slot));
                }
            } else {
                sprintf(str, "instance of %s (%p, size=%d, set=%d)", slotRawSymbol(&classptr->name)->name,
                        slotRawObject(slot), slotRawObject(slot)->size, slotRawObject(slot)->obj_sizeclass);
            }
        } else {
            sprintf(str, "NULL Object Pointer");
        }
        break;
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
        sprintf(str, "RawPointer %p", slotRawPtr(slot));
        break;
    default: {
        union {
            int32 i[2];
            double f;
        } u;
        u.f = slotRawFloat(slot);
        sprintf(str, "Float %f   %08X %08X", u.f, u.i[0], u.i[1]);
        break;
    }
    }
}

static void printObject(PyrSlot* slot, PyrObject* obj, char* str) {
    assert(obj);
    PyrClass* classptr = obj->classptr;
    if (classptr == class_class) {
        sprintf(str, "class %s", slotRawSymbol(&((PyrClass*)obj)->name)->name);
    } else if (classptr == class_string) {
        char str2[32];
        int len;
        if (obj->size > 31) {
            memcpy(str2, (char*)obj->slots, 28);
            str2[28] = '.';
            str2[29] = '.';
            str2[30] = '.';
            str2[31] = 0;
        } else {
            len = sc_min(31, obj->size);
            memcpy(str2, (char*)obj->slots, len);
            str2[len] = 0;
        }
        sprintf(str, "\"%s\"", str2);
    } else if (classptr == class_method) {
        sprintf(str, "%s:%s", slotRawSymbol(&slotRawClass(&slotRawMethod(slot)->ownerclass)->name)->name,
                slotRawSymbol(&slotRawMethod(slot)->name)->name);
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
                sprintf(str, "< FunctionDef in Method %s:%s >",
                        slotRawSymbol(&slotRawClass(&slotRawMethod(context)->ownerclass)->name)->name,
                        slotRawSymbol(&slotRawMethod(context)->name)->name);
            } else {
                sprintf(str, "< FunctionDef in closed FunctionDef >");
            }
        } else {
            sprintf(str, "< closed FunctionDef >");
        }
    } else if (classptr == class_frame) {
        if (!slotRawFrame(slot)) {
            sprintf(str, "Frame (null)");
        } else if (!slotRawBlock(&slotRawFrame(slot)->method)) {
            sprintf(str, "Frame (null method)");
        } else if (slotRawBlock(&slotRawFrame(slot)->method)->classptr == class_method) {
            sprintf(str, "Frame (%p) of %s:%s", obj,
                    slotRawSymbol(&slotRawClass(&slotRawMethod(&slotRawFrame(slot)->method)->ownerclass)->name)->name,
                    slotRawSymbol(&slotRawMethod(&slotRawFrame(slot)->method)->name)->name);
        } else {
            sprintf(str, "Frame (%p) of Function", obj);
        }
    } else if (classptr == class_array) {
        sprintf(str, "[*%d]", obj->size);
    } else {
        sprintf(str, "<instance of %s>", slotRawSymbol(&classptr->name)->name);
    }
}

// Assumed: str has enough space to hold the representation of d
static void prettyFormatFloat(char* str, double d) {
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

void slotOneWord(PyrSlot* slot, char* str) {
    str[0] = 0;
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "$%c", static_cast<int>(slotRawChar(slot)));
        break;
    case tagSym:
        if (strlen(slotRawSymbol(slot)->name) > 240) {
            char str2[256];
            memcpy(str2, slotRawSymbol(slot)->name, 240);
            str2[240] = 0;
            snprintf(str, 256, "'%s...'", str2);
        } else {
            snprintf(str, 256, "'%s'", slotRawSymbol(slot)->name);
        }
        break;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj)
            printObject(slot, slotObj, str);
        else
            sprintf(str, "NULL Object Pointer");
        break;
    }
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
        sprintf(str, "ptr%p", slotRawPtr(slot));
        break;
    default:
        prettyFormatFloat(str, slotRawFloat(slot));
        break;
    }
}

bool postString(PyrSlot* slot, char* str) {
    bool res = true;
    switch (GetTag(slot)) {
    case tagInt:
        sprintf(str, "%d", slotRawInt(slot));
        break;
    case tagChar:
        sprintf(str, "%c", slotRawChar(slot));
        break;
    case tagSym:
        str[0] = 0;
        res = false;
        break;
    case tagObj: {
        PyrObject* slotObj = slotRawObject(slot);
        if (slotObj) {
            PyrClass* classptr = slotRawObject(slot)->classptr;
            if (classptr == class_class || classptr == class_method || classptr == class_fundef
                || classptr == class_frame)
                printObject(slot, slotObj, str);
            else {
                str[0] = 0;
                res = false;
            }
        } else
            sprintf(str, "NULL Object Pointer");
        break;
    }
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
        sprintf(str, "%p", slotRawPtr(slot));
        break;
    default:
        prettyFormatFloat(str, slotRawFloat(slot));
        break;
    }
    return res;
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

void pstrncpy(unsigned char* s1, unsigned char* s2, int n);

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
