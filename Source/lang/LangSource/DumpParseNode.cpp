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

#ifdef SC_WIN32
# define snprintf _snprintf
# define PATH_MAX _MAX_PATH
#endif
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
extern int textpos;

extern "C" {
	extern char *g_fmt(char *, double);
}


void dumpNodeList(PyrParseNode *node)
{
	for (; node; node = node->mNext) {
		DUMPNODE(node, 0);
	}
}

void PyrCurryArgNode::dump(int level)
{
	postfl("%2d CurryArg %d\n", level, mArgNum);
}

void PyrSlotNode::dump(int level)
{
	if (mClassno == pn_PushLitNode)
		dumpPushLit(level);
	else if (mClassno == pn_PushNameNode)
		postfl("%2d PushName '%s'\n", level, mSlot.us->name);
	else if (mClassno == pn_LiteralNode)
		dumpLiteral(level);
	else {
		postfl("%2d SlotNode\n", level);
		dumpPyrSlot(&mSlot); 
	}
	DUMPNODE(mNext, level);
}

void PyrPushKeyArgNode::dump(int level)
{
	postfl("%2d PushKeyArgNode\n", level);
	DUMPNODE(mSelector, level+1);
	DUMPNODE(mExpr, level+1);
	DUMPNODE(mNext, level);
}

void PyrClassExtNode::dump(int level)
{
	postfl("%2d ClassExt '%s'\n", level, mClassName->mSlot.us->name);
	DUMPNODE(mMethods, level+1);
	DUMPNODE(mNext, level);
}

void PyrClassNode::dump(int level)
{
	postfl("%2d Class '%s'\n", level, mClassName->mSlot.us->name);
	DUMPNODE(mSuperClassName, level+1);
	DUMPNODE(mVarlists, level+1);
	DUMPNODE(mMethods, level+1);
	DUMPNODE(mNext, level);
}

void PyrMethodNode::dump(int level)
{
	postfl("%2d MethodNode '%s'  %s\n", level, mMethodName->mSlot.us->name,
		mPrimitiveName ? mPrimitiveName->mSlot.us->name:"");
	DUMPNODE(mArglist, level+1);
	DUMPNODE(mBody, level+1);
	DUMPNODE(mNext, level);
}

void PyrArgListNode::dump(int level)
{
	postfl("%2d ArgList\n", level);
	DUMPNODE(mVarDefs, level+1);
	DUMPNODE(mRest, level+1);
	DUMPNODE(mNext, level);
}

void PyrVarListNode::dump(int level)
{
	postfl("%2d VarList\n", level);
	DUMPNODE(mVarDefs, level+1);
	DUMPNODE(mNext, level);
}

void PyrVarDefNode::dump(int level)
{
	postfl("%2d VarDef '%s'\n", level, mVarName->mSlot.us->name);
	DUMPNODE(mDefVal, level);
	DUMPNODE(mNext, level);
}

void PyrCallNode::dump(int level)
{
	postfl("%2d Call '%s'\n", level, mSelector->mSlot.us->name);
	DUMPNODE(mArglist, level+1);
	DUMPNODE(mKeyarglist, level+1);
	DUMPNODE(mNext, level);
}

void PyrBinopCallNode::dump(int level)
{
	postfl("%2d BinopCall '%s'\n", level, mSelector->mSlot.us->name);
	DUMPNODE(mArglist, level+1);
	DUMPNODE(mNext, level);
}

void PyrDropNode::dump(int level)
{
	postfl("%2d Drop (\n", level);
	DUMPNODE(mExpr1, level+1);
	postfl(" -> %2d Drop\n", level);
	DUMPNODE(mExpr2, level+1);
	postfl(") %2d Drop\n", level);
	DUMPNODE(mNext, level);
}

void PyrSlotNode::dumpPushLit(int level)
{
	postfl("%2d PushLit\n", level);
	if (mSlot.utag != tagPtr) dumpPyrSlot(&mSlot);
	else {
		DUMPNODE((PyrParseNode*)mSlot.uo, level);
	}
}

void PyrSlotNode::dumpLiteral(int level)
{
	postfl("%2d Literal\n", level);
	if (mSlot.utag != tagPtr) dumpPyrSlot(&mSlot);
	else {
		DUMPNODE((PyrParseNode*)mSlot.uo, level);
	}
}

void PyrReturnNode::dump(int level)
{
	postfl("%2d Return (\n", level);
	DUMPNODE(mExpr, level+1);
	postfl(") %2d Return \n", level);
	DUMPNODE(mNext, level);
}

void PyrBlockReturnNode::dump(int level)
{
	postfl("%2d FuncReturn\n", level);
	DUMPNODE(mNext, level);
}

void PyrAssignNode::dump(int level)
{
	postfl("%2d Assign '%s'\n", level, mVarName->mSlot.us->name);
	DUMPNODE(mVarName, level+1);
	DUMPNODE(mExpr, level+1);
	DUMPNODE(mNext, level);
}

void PyrSetterNode::dump(int level)
{
	//postfl("%2d Assign '%s'\n", level, mVarName->mSlot.us->name);
	DUMPNODE(mSelector, level+1);
	DUMPNODE(mExpr1, level+1);
	DUMPNODE(mExpr2, level+1);
}

void PyrMultiAssignNode::dump(int level)
{
	postfl("%2d MultiAssign\n", level);
	DUMPNODE(mVarList, level+1);
	DUMPNODE(mExpr, level+1);
	DUMPNODE(mNext, level);
}

void PyrMultiAssignVarListNode::dump(int level)
{
	postfl("%2d MultiAssignVarList\n", level);
	DUMPNODE(mVarNames, level+1);
	DUMPNODE(mRest, level+1);
	DUMPNODE(mNext, level);
}

void PyrDynDictNode::dump(int level)
{
	postfl("%2d DynDict\n", level);
	DUMPNODE(mElems, level+1);
	DUMPNODE(mNext, level);
}

void PyrDynListNode::dump(int level)
{
	postfl("%2d DynList\n", level);
	DUMPNODE(mElems, level+1);
	DUMPNODE(mNext, level);
}

void PyrLitListNode::dump(int level)
{
	postfl("%2d LitList\n", level);
	postfl(" %2d mElems\n", level);
	DUMPNODE(mElems, level+1);
	postfl(" %2d mNext\n", level);
	DUMPNODE(mNext, level);
}

void PyrBlockNode::dump(int level)
{
	postfl("%2d Func\n", level);
	DUMPNODE(mArglist, level+1);
	DUMPNODE(mBody, level+1);
	DUMPNODE(mNext, level);
}

void dumpPyrSlot(PyrSlot* slot)
{
	char str[128];
	slotString(slot, str);
	post("   %s\n", str);
}

void slotString(PyrSlot *slot, char *str)
{
	switch (slot->utag) {
		case tagInt :
			sprintf(str, "Integer %d", slot->ui);
			break;
		case tagChar :
			sprintf(str, "Character %d '%c'", slot->ui, slot->ui);
			break;
		case tagSym :
			if (strlen(slot->us->name) > 240) {
				char str2[256];
				memcpy(str2, slot->us->name, 240);
				str2[240] = 0;
				snprintf(str, 256, "Symbol '%s...'", str2);
			} else {
				snprintf(str, 256, "Symbol '%s'", slot->us->name);
			}
			break;
		case tagObj :
			if (slot->uo) {
				if (isKindOf(slot->uo, class_class)) {
					sprintf(str, "class %s (%p)",
						((PyrClass*)slot->uo)->name.us->name, slot->uo);
				} else if (slot->uo->classptr == class_string) {
					char str2[48];
					int len;
					if (slot->uo->size > 47) {
						memcpy(str2, (char*)slot->uo->slots, 44);
						str2[44] = '.';
						str2[45] = '.';
						str2[46] = '.';
						str2[47] = 0;
					} else {
						len = sc_min(47, slot->uo->size);
						memcpy(str2, (char*)slot->uo->slots, len);
						str2[len] = 0;
					}
					sprintf(str, "\"%s\"", str2);
				} else if (slot->uo->classptr == class_method) {
					sprintf(str, "instance of Method %s:%s (%p)",
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name, slot->uom);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->contextDef;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->contextDef;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "instance of FunctionDef in Method %s:%s", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "instance of FunctionDef in closed FunctionDef");
						}
					} else {
						sprintf(str, "instance of FunctionDef - closed");
					}
				} else if (slot->uo->classptr == class_frame) {
					if (!slot->uof) {
						sprintf(str, "Frame (%0X)", slot->ui);
					} else if (slot->uof->method.uoblk->classptr == class_method) {
						sprintf(str, "Frame (%0X) of %s:%s", slot->ui,
							slot->uof->method.uom->ownerclass.uoc->name.us->name,
							slot->uof->method.uom->name.us->name);
					} else {
						sprintf(str, "Frame (%0X) of Function", slot->ui);
					}
				} else {
					sprintf(str, "instance of %s (%08lX, size=%d, set=%d)", 
						slot->uo->classptr->name.us->name, 
						(unsigned long)slot->uo, slot->uo->size,
						slot->uo->obj_sizeclass);
				}
			} else {
				sprintf(str, "NULL Object Pointer");
			}
			break;
		case tagNil :
			sprintf(str, "nil");
			break;
		case tagFalse :
			sprintf(str, "false");
			break;
		case tagTrue :
			sprintf(str, "true");
			break;
		case tagPtr :
			sprintf(str, "RawPointer %X", slot->ui);
			break;
		default :
		{
			char fstr[32];
			g_fmt(fstr, slot->uf);
			sprintf(str, "Float %s   %08X %08X", fstr, slot->utag, slot->ui);
			break;
		}
	}
}

void slotOneWord(PyrSlot *slot, char *str)
{
	str[0] = 0;
	switch (slot->utag) {
		case tagInt :
			sprintf(str, "%d", slot->ui);
			break;
		case tagChar :
			sprintf(str, "$%c", slot->ui);
			break;
		case tagSym :
			if (strlen(slot->us->name) > 240) {
				char str2[256];
				memcpy(str2, slot->us->name, 240);
				str2[240] = 0;
				snprintf(str, 256, "'%s...'", str2);
			} else {
				snprintf(str, 256, "'%s'", slot->us->name);
			}
			break;
		case tagObj :
			if (slot->uo) {
				if (isKindOf(slot->uo, class_class)) {
					sprintf(str, "class %s", ((PyrClass*)slot->uo)->name.us->name);
				} else if (slot->uo->classptr == class_string) {
					char str2[32];
					int len;
					if (slot->uo->size > 31) {
						memcpy(str2, (char*)slot->uo->slots, 28);
						str2[28] = '.';
						str2[29] = '.';
						str2[30] = '.';
						str2[31] = 0;
					} else {
						len = sc_min(31, slot->uo->size);
						memcpy(str2, (char*)slot->uo->slots, len);
						str2[len] = 0;
					}
					sprintf(str, "\"%s\"", str2);
				} else if (slot->uo->classptr == class_method) {
					sprintf(str, "%s:%s", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->contextDef;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->contextDef;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "< FunctionDef in Method %s:%s >", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "< FunctionDef in closed FunctionDef >");
						}
					} else {
						sprintf(str, "< closed FunctionDef >");
					}
				} else if (slot->uo->classptr == class_frame) {
					if (!slot->uof) {
						sprintf(str, "Frame (null)");
					} else if (!slot->uof->method.uoblk) {
						sprintf(str, "Frame (null method)");
					} else if (slot->uof->method.uoblk->classptr == class_method) {
						sprintf(str, "Frame (%0X) of %s:%s", slot->ui,
							slot->uof->method.uom->ownerclass.uoc->name.us->name,
							slot->uof->method.uom->name.us->name);
					} else {
						sprintf(str, "Frame (%0X) of Function", slot->ui);
					}
				} else if (slot->uo->classptr == class_array) {
					sprintf(str, "[*%d]", slot->uo->size);
				} else {
					sprintf(str, "<instance of %s>", slot->uo->classptr->name.us->name);
				}				
			} else {
				sprintf(str, "NULL Object Pointer");
			}
			break;
		case tagNil :
			sprintf(str, "nil");
			break;
		case tagFalse :
			sprintf(str, "false");
			break;
		case tagTrue :
			sprintf(str, "true");
			break;
		case tagPtr :
			sprintf(str, "ptr%08X", slot->ui);
			break;
		default :
			g_fmt(str, slot->uf);
			break;
	}
}

bool postString(PyrSlot *slot, char *str)
{
	bool res = true;
	switch (slot->utag) {
		case tagInt :
			sprintf(str, "%d", slot->ui);
			break;
		case tagChar :
			sprintf(str, "%c", slot->ui);
			break;
		case tagSym :
			str[0] = 0;
			res = false;
			break;
		case tagObj :
			/*if (slot->uo) {
				if (slot->uo->classptr == class_method) {
					sprintf(str, "instance of Method %s:%s", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name);
				} else {
					sprintf(str, "instance of %s (%08X, size=%d, set=%02X)", 
						slot->uo->classptr->name.us->name, 
						slot->uo, slot->uo->size,
						slot->uo->obj_sizeclass);
				}
			} else {
				sprintf(str, "NULL Object Pointer");
			}*/

			if (slot->uo) {
				if (isKindOf(slot->uo, class_class)) {
					sprintf(str, "class %s", ((PyrClass*)slot->uo)->name.us->name);
/*				} else if (slot->uo->classptr == class_string) {
					char str2[48];
					int len;
					if (slot->uo->size > 47) {
						memcpy(str2, (char*)slot->uo->slots, 44);
						str2[44] = '.';
						str2[45] = '.';
						str2[46] = '.';
						str2[47] = 0;
					} else {
						len = sc_min(47, slot->uo->size);
						memcpy(str2, (char*)slot->uo->slots, len);
						str2[len] = 0;
					}
					sprintf(str, "\"%s\"", str2);
*/
				} else if (slot->uo->classptr == class_method) {
					sprintf(str, "Method %s:%s", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->contextDef;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->contextDef;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "a FunctionDef in Method %s:%s", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "a FunctionDef in closed FunctionDef");
						}
					} else {
						sprintf(str, "a FunctionDef - closed");
					}
				} else if (slot->uo->classptr == class_frame) {
					if (!slot->uof) {
						sprintf(str, "Frame (null)");
					} else if (!slot->uof->method.uoblk) {
						sprintf(str, "Frame (null method)");
					} else if (slot->uof->method.uoblk->classptr == class_method) {
						sprintf(str, "Frame (%0X) of %s:%s", slot->ui,
							slot->uof->method.uom->ownerclass.uoc->name.us->name,
							slot->uof->method.uom->name.us->name);
					} else {
						sprintf(str, "Frame (%0X) of Function", slot->ui);
					}
				} else {
					str[0] = 0;
					res = false;
//					sprintf(str, "instance of %s (%08X, size=%d, gcset=%02X)", 
//						slot->uo->classptr->name.us->name, 
//						slot->uo, slot->uo->size,
//						slot->uo->obj_sizeclass);
				}
			} else {
				sprintf(str, "NULL Object Pointer");
			}
			break;
		case tagNil :
			sprintf(str, "nil");
			break;
		case tagFalse :
			sprintf(str, "false");
			break;
		case tagTrue :
			sprintf(str, "true");
			break;
		case tagPtr :
			sprintf(str, "%X", slot->ui);
			break;
		default :
			sprintf(str, "%.14g", slot->uf);
			//g_fmt(str, slot->uf);
			break;
	}
	return res;
}


int asCompileString(PyrSlot *slot, char *str)
{
	switch (slot->utag) {
		case tagInt :
			sprintf(str, "%d", slot->ui);
			break;
		case tagChar :
		{
			int c = slot->ui;
			if (isprint(c)) {
				sprintf(str, "$%c", c);
			} else {
				switch (c) {
					case '\n' : strcpy(str, "$\\n"); break;
					case '\r' : strcpy(str, "$\\r"); break;
					case '\t' : strcpy(str, "$\\t"); break;
					case '\f' : strcpy(str, "$\\f"); break;
					case '\v' : strcpy(str, "$\\v"); break;
					default: sprintf(str, "%d.asAscii", c);
				}
			}
			break;
		}
		case tagSym :
			return errFailed;
		case tagObj :
			return errFailed;
		case tagNil :
			sprintf(str, "nil");
			break;
		case tagFalse :
			sprintf(str, "false");
			break;
		case tagTrue :
			sprintf(str, "true");
			break;
		case tagPtr :
			strcpy(str, "/*Ptr*/ nil");
			break;
		default :
			g_fmt(str, slot->uf);
			break;
	}
	return errNone;
}


void stringFromPyrString(PyrString *obj, char *str, int maxlength);
void stringFromPyrString(PyrString *obj, char *str, int maxlength)
{
	if (obj->classptr == class_string) {
		int len;
		if (obj->size > maxlength-4) {
			memcpy(str, obj->s, maxlength-4);
			str[maxlength-4] = '.';
			str[maxlength-3] = '.';
			str[maxlength-2] = '.';
			str[maxlength-1] = 0;
		} else {
			len = sc_min(maxlength-1, obj->size);
			memcpy(str, obj->s, len);
			str[len] = 0;
		}
	} else {
		sprintf(str, "not a string");
	}
}

void pstrncpy(unsigned char *s1, unsigned char *s2, int n);

void pstringFromPyrString(PyrString *obj, unsigned char *str, int maxlength);
void pstringFromPyrString(PyrString *obj, unsigned char *str, int maxlength)
{
	if (obj && obj->classptr == class_string) {
		int len;
		len = sc_min(maxlength-1, obj->size);
		memcpy(str+1, obj->s, len);
		str[0] = len;
	} else {
#ifdef SC_DARWIN
		pstrncpy(str, (unsigned char*)"\pnot a string", maxlength-1);
#else
		strncpy((char*)str, "not a string", maxlength-1);
#endif
	}
}
