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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
extern int textpos;


void dumpNodeList(PyrParseNode *node)
{
	for (; node; node = node->next) {
		DUMPNODE(node, 0);
	}
}

void dumpPyrPushNameNode(PyrPushNameNode* node, int level)
{
	postfl("%2d PushName '%s'\n", level, node->varName.us->name);
	DUMPNODE(node->next, level);
}

void dumpPyrSlotNode(PyrSlotNode* node, int level)
{
	postfl("%2d SlotNode\n", level);
	dumpPyrSlot(&node->slot); 
	DUMPNODE(node->next, level);
}

void dumpPyrPushKeyArgNode(PyrPushKeyArgNode* node, int level)
{
	postfl("%2d PushKeyArgNode\n", level);
	DUMPNODE(node->selector, level+1);
	DUMPNODE(node->expr, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrClassExtNode(PyrClassExtNode* node, int level)
{
	postfl("%2d ClassExt '%s'\n", level, node->className->slot.us->name);
	DUMPNODE(node->methods, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrClassNode(PyrClassNode* node, int level)
{
	postfl("%2d Class '%s'\n", level, node->className->slot.us->name);
	DUMPNODE(node->superClassName, level+1);
	DUMPNODE(node->varlists, level+1);
	DUMPNODE(node->methods, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrMethodNode(PyrMethodNode* node, int level)
{
	postfl("%2d MethodNode '%s'  %s\n", level, node->methodName->slot.us->name,
		node->primitiveName?node->primitiveName->slot.us->name:"");
	DUMPNODE(node->arglist, level+1);
	DUMPNODE(node->body, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrArgListNode(PyrArgListNode* node, int level)
{
	postfl("%2d ArgList\n", level);
	DUMPNODE(node->varDefs, level+1);
	DUMPNODE(node->rest, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrVarListNode(PyrVarListNode* node, int level)
{
	postfl("%2d VarList\n", level);
	DUMPNODE(node->varDefs, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrVarDefNode(PyrVarDefNode* node, int level)
{
	postfl("%2d VarDef '%s'\n", level, node->varName->slot.us->name);
	DUMPNODE(node->defVal, level);
	DUMPNODE(node->next, level);
}

void dumpPyrCallNode(PyrCallNode* node, int level)
{
	postfl("%2d Call '%s'\n", level, node->selector->slot.us->name);
	DUMPNODE(node->arglist, level+1);
	DUMPNODE(node->keyarglist, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrBinopCallNode(PyrBinopCallNode* node, int level)
{
	postfl("%2d BinopCall '%s'\n", level, node->selector->slot.us->name);
	DUMPNODE(node->arg1, level+1);
	DUMPNODE(node->arg2, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrDropNode(PyrDropNode* node, int level)
{
	postfl("%2d Drop (\n", level);
	DUMPNODE(node->expr1, level+1);
	postfl(" -> %2d Drop\n", level);
	DUMPNODE(node->expr2, level+1);
	postfl(") %2d Drop\n", level);
	DUMPNODE(node->next, level);
}

void dumpPyrPushLitNode(PyrPushLitNode* node, int level)
{
	postfl("%2d PushLit\n", level);
	if (node->literalSlot.utag != tagPtr) dumpPyrSlot(&node->literalSlot);
	else {
		DUMPNODE((PyrParseNode*)node->literalSlot.uo, level);
	}
	DUMPNODE(node->next, level);
}

void dumpPyrLiteralNode(PyrLiteralNode* node, int level)
{
	postfl("%2d Literal\n", level);
	if (node->literalSlot.utag != tagPtr) dumpPyrSlot(&node->literalSlot);
	else {
		DUMPNODE((PyrParseNode*)node->literalSlot.uo, level);
	}
	DUMPNODE(node->next, level);
}

void dumpPyrReturnNode(PyrReturnNode* node, int level)
{
	postfl("%2d Return (\n", level);
	DUMPNODE(node->expr, level+1);
	postfl(") %2d Return \n", level);
	DUMPNODE(node->next, level);
}

void dumpPyrBlockReturnNode(PyrBlockReturnNode* node, int level)
{
	postfl("%2d FuncReturn\n", level);
	DUMPNODE(node->next, level);
}

void dumpPyrAssignNode(PyrAssignNode* node, int level)
{
	postfl("%2d Assign '%s'\n", level, node->varName->slot.us->name);
	DUMPNODE(node->varName, level+1);
	DUMPNODE(node->expr, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrSetterNode(PyrSetterNode* node, int level)
{
	//postfl("%2d Assign '%s'\n", level, node->varName->slot.us->name);
	DUMPNODE(node->selector, level+1);
	DUMPNODE(node->expr1, level+1);
	DUMPNODE(node->expr2, level+1);
}

void dumpPyrMultiAssignNode(PyrMultiAssignNode* node, int level)
{
	postfl("%2d MultiAssign\n", level);
	DUMPNODE(node->varList, level+1);
	DUMPNODE(node->expr, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrMultiAssignVarListNode(PyrMultiAssignVarListNode* node, int level)
{
	postfl("%2d MultiAssignVarList\n", level);
	DUMPNODE(node->varNames, level+1);
	DUMPNODE(node->rest, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrDynListNode(PyrDynListNode* node, int level)
{
	postfl("%2d DynList\n", level);
	DUMPNODE(node->elems, level+1);
	DUMPNODE(node->next, level);
}

void dumpPyrLitListNode(PyrLitListNode* node, int level)
{
	postfl("%2d LitList\n", level);
	postfl(" %2d elems\n", level);
	DUMPNODE(node->elems, level+1);
	postfl(" %2d next\n", level);
	DUMPNODE(node->next, level);
}

void dumpPyrBlockNode(PyrBlockNode* node, int level)
{
	postfl("%2d Func\n", level);
	DUMPNODE(node->arglist, level+1);
	DUMPNODE(node->body, level+1);
	DUMPNODE(node->next, level);
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
			sprintf(str, "Integer %ld", slot->ui);
			break;
		case tagChar :
			sprintf(str, "Character %ld '%c'", slot->ui, slot->ui);
			break;
		case tagSym :
			sprintf(str, "Symbol '%s'", slot->us->name);
			break;
		case tagObj :
			if (slot->uo) {
				if (isKindOf(slot->uo, class_class)) {
					sprintf(str, "class %s (%08X)", 
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
					sprintf(str, "instance of Method %s-%s (%08X)", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name, slot->uom);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->context;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->context;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "instance of FunctionDef in Method %s-%s", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "instance of FunctionDef in closed FunctionDef");
						}
					} else {
						sprintf(str, "instance of FunctionDef - closed");
					}
				} else {
					sprintf(str, "instance of %s (%08X, size=%ld, set=%02X)", 
						slot->uo->classptr->name.us->name, 
						slot->uo, slot->uo->size,
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
		case tagInf :
			sprintf(str, "inf");
			break;
		case tagHFrame :
			if (slot->uof->method.uoblk->classptr == class_method) {
				sprintf(str, "Frame (%0X)%s of %s-%s", slot->ui,
					slot->utag == tagHFrame ? "H":"S",
					slot->uof->method.uom->ownerclass.uoc->name.us->name,
					slot->uof->method.uom->name.us->name);
			} else {
				sprintf(str, "Frame (%0X)%s of Function", slot->ui,
					slot->utag == tagHFrame ? "H":"S");
			}
			break;
		case tagSFrame :
			sprintf(str, "Frame (%0X)S", slot->ui);
			break;
		case tagPtr :
			sprintf(str, "RawPointer %X", slot->ui);
			break;
		default :
			sprintf(str, "Float %.14g   %08X %08X", slot->uf, slot->utag, slot->ui);
			break;
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
			sprintf(str, "'%s'", slot->us->name);
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
					sprintf(str, "%s-%s", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->context;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->context;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "< FunctionDef in Method %s-%s >", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "< FunctionDef in closed FunctionDef >");
						}
					} else {
						sprintf(str, "< closed FunctionDef >");
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
		case tagInf :
			sprintf(str, "inf");
			break;
		case tagHFrame :
                        if (!slot->uof) {
				sprintf(str, "Frame (null)");
			} else if (!slot->uof->method.uoblk) {
				sprintf(str, "Frame (null method)");
			} else if (slot->uof->method.uoblk->classptr == class_method) {
				sprintf(str, "Frame (%0X)%s of %s-%s", slot->ui,
					slot->utag == tagHFrame ? "H":"S",
					slot->uof->method.uom->ownerclass.uoc->name.us->name,
					slot->uof->method.uom->name.us->name);
			} else {
				sprintf(str, "Frame (%0X)%s of Function", slot->ui,
					slot->utag == tagHFrame ? "H":"S");
			}
			break;
		case tagSFrame :
			sprintf(str, "Frame (%0X)S", slot->ui);
			break;
		case tagPtr :
			sprintf(str, "ptr%08X", slot->ui);
			break;
		default :
			sprintf(str, "%f", slot->uf);
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
			sprintf(str, "%s", slot->us->name);
			break;
		case tagObj :
			/*if (slot->uo) {
				if (slot->uo->classptr == class_method) {
					sprintf(str, "instance of Method %s-%s", 
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
					sprintf(str, "Method %s-%s", 
						slot->uom->ownerclass.uoc->name.us->name,
						slot->uom->name.us->name);
				} else if (slot->uo->classptr == class_fundef) {
					PyrSlot *context, *nextcontext;
					// find function's method
					nextcontext = &slot->uoblk->context;
					if (NotNil(nextcontext)) {
						do {
							context = nextcontext;
							nextcontext = &context->uoblk->context;
						} while (nextcontext->utag != tagNil);
						if (isKindOf(context->uo, class_method)) {
							sprintf(str, "a FunctionDef in Method %s-%s", 
								context->uom->ownerclass.uoc->name.us->name,
								context->uom->name.us->name);
						} else {
							sprintf(str, "a FunctionDef in closed FunctionDef");
						}
					} else {
						sprintf(str, "a FunctionDef - closed");
					}
				} else {
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
		case tagInf :
			sprintf(str, "inf");
			break;
		case tagHFrame :
		case tagSFrame :
			if (!slot->uof) {
				sprintf(str, "Frame (null)");
			} else if (!slot->uof->method.uoblk) {
				sprintf(str, "Frame (null method)");
			} else if (slot->uof->method.uoblk->classptr == class_method) {
				sprintf(str, "Frame (%0X)%s of %s-%s", slot->ui,
					slot->utag == tagHFrame ? "H":"S",
					slot->uof->method.uom->ownerclass.uoc->name.us->name,
					slot->uof->method.uom->name.us->name);
			} else {
				sprintf(str, "Frame (%0X)%s of Function", slot->ui,
					slot->utag == tagHFrame ? "H":"S");
			}
			break;
		case tagPtr :
			sprintf(str, "%X", slot->ui);
			break;
		default :
			sprintf(str, "%.14g", slot->uf);
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
			sprintf(str, "$%c", slot->ui);
			break;
		case tagSym :
			sprintf(str, "'%s'", slot->us->name);
			break;
		case tagObj :
			//sprintf(str, "%s.new", slot->uo->classptr->name.us->name);
			return errFailed;
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
		case tagInf :
			sprintf(str, "inf");
			break;
		case tagHFrame :
		case tagSFrame :
			strcpy(str, "/*Frame*/ nil");
			break;
		case tagPtr :
			strcpy(str, "/*Ptr*/ nil");
			break;
		default :
			sprintf(str, "%.14g", slot->uf);
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
		pstrncpy(str, (unsigned char*)"\pnot a string", maxlength-1);
	}
}