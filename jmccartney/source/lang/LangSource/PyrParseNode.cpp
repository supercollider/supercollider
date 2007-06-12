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
#include "PyrSymbolTable.h"
#include "Opcodes.h"
#include "PyrKernelProto.h"
#include "PyrObjectProto.h"
#include "GC.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "InitAlloc.h"
#include "PredefinedSymbols.h"

AdvancingAllocPool gParseNodePool;

PyrSymbol *gSpecialUnarySelectors[opNumUnarySelectors];
PyrSymbol *gSpecialBinarySelectors[opNumBinarySelectors];
PyrSymbol *gSpecialSelectors[opmNumSpecialSelectors];
PyrSymbol* gSpecialClasses[op_NumSpecialClasses];
double gSpecialValues[svNumSpecialValues];
PyrParseNodeClass* parseNodeClasses[pn_NumTypes];

PyrParseNode* gRootParseNode;
int gParserResult;

/*
PyrParseNodeClass *pyrSlotNodeClass;
PyrParseNodeClass *pyrPushNameNodeClass;
PyrParseNodeClass *pyrClassNodeClass;
PyrParseNodeClass *pyrMethodNodeClass;
PyrParseNodeClass *pyrVarListNodeClass;
PyrParseNodeClass *pyrVarDefNodeClass;
PyrParseNodeClass *pyrCallNodeClass;
PyrParseNodeClass *pyrBinopCallNodeClass;
PyrParseNodeClass *pyrDropNodeClass;
PyrParseNodeClass *pyrPushLitNodeClass;
PyrParseNodeClass *pyrLiteralNodeClass;
PyrParseNodeClass *pyrReturnNodeClass;
PyrParseNodeClass *pyrBlockReturnNodeClass;
PyrParseNodeClass *pyrAssignNodeClass;
PyrParseNodeClass *pyrSetterNodeClass;
PyrParseNodeClass *pyrMultiAssignNodeClass;
PyrParseNodeClass *pyrMultiAssignVarListNodeClass;
PyrParseNodeClass *pyrBlockNodeClass;
PyrParseNodeClass *pyrArgListNodeClass;
PyrParseNodeClass *pyrDynListNodeClass;
PyrParseNodeClass *pyrLitListNodeClass;
*/

PyrClass *gCurrentClass = NULL;
PyrClass *gCurrentMetaClass = NULL;
PyrClass *gCompilingClass = NULL;
PyrMethod *gCompilingMethod = NULL;
PyrBlock *gCompilingBlock = NULL;
long gInliningLevel;

int compileErrors = 0;

extern bool compilingCmdLine;


char *nodename[pn_NumTypes] = {
};

GC* compileGC();
GC* compileGC()
{
	return gCompilingVMGlobals ? gCompilingVMGlobals->gc : 0;
}

void initParser() 
{
	compileErrors = 0;
}

void finiParser()
{
}

void initParseNodes()
{
	newParseNodeClass(pn_SlotNode, 
		(PyrCompileNodeFunc)compilePyrSlotNode, (PyrDumpNodeFunc)dumpPyrSlotNode);
	newParseNodeClass(pn_PushNameNode, 
		(PyrCompileNodeFunc)compilePyrPushNameNode, (PyrDumpNodeFunc)dumpPyrPushNameNode);
	newParseNodeClass(pn_PushKeyArgNode, 
		(PyrCompileNodeFunc)compilePyrPushKeyArgNode, (PyrDumpNodeFunc)dumpPyrPushKeyArgNode);
	newParseNodeClass(pn_ClassExtNode, 
		(PyrCompileNodeFunc)compilePyrClassExtNode, (PyrDumpNodeFunc)dumpPyrClassExtNode);
	newParseNodeClass(pn_ClassNode, 
		(PyrCompileNodeFunc)compilePyrClassNode, (PyrDumpNodeFunc)dumpPyrClassNode);
	newParseNodeClass(pn_MethodNode, 
		(PyrCompileNodeFunc)compilePyrMethodNode, (PyrDumpNodeFunc)dumpPyrMethodNode);
	newParseNodeClass(pn_VarListNode, 
		(PyrCompileNodeFunc)compilePyrVarListNode, (PyrDumpNodeFunc)dumpPyrVarListNode);
	newParseNodeClass(pn_VarDefNode, 
		(PyrCompileNodeFunc)compilePyrVarDefNode, (PyrDumpNodeFunc)dumpPyrVarDefNode);
	newParseNodeClass(pn_CallNode, 
		(PyrCompileNodeFunc)compilePyrCallNode, (PyrDumpNodeFunc)dumpPyrCallNode);
	newParseNodeClass(pn_BinopCallNode, 
		(PyrCompileNodeFunc)compilePyrBinopCallNode, (PyrDumpNodeFunc)dumpPyrBinopCallNode);
	newParseNodeClass(pn_DropNode, 
		(PyrCompileNodeFunc)compilePyrDropNode, (PyrDumpNodeFunc)dumpPyrDropNode);
	newParseNodeClass(pn_PushLitNode, 
		(PyrCompileNodeFunc)compilePyrPushLitNode, (PyrDumpNodeFunc)dumpPyrPushLitNode);
	newParseNodeClass(pn_LiteralNode, 
		(PyrCompileNodeFunc)compilePyrLiteralNode, (PyrDumpNodeFunc)dumpPyrLiteralNode);
	newParseNodeClass(pn_ReturnNode, 
		(PyrCompileNodeFunc)compilePyrReturnNode, (PyrDumpNodeFunc)dumpPyrReturnNode);
	newParseNodeClass(pn_BlockReturnNode, 
		(PyrCompileNodeFunc)compilePyrBlockReturnNode, (PyrDumpNodeFunc)dumpPyrBlockReturnNode);
	newParseNodeClass(pn_AssignNode, 
		(PyrCompileNodeFunc)compilePyrAssignNode, (PyrDumpNodeFunc)dumpPyrAssignNode);
	newParseNodeClass(pn_SetterNode, 
		(PyrCompileNodeFunc)compilePyrSetterNode, (PyrDumpNodeFunc)dumpPyrSetterNode);
	newParseNodeClass(pn_MultiAssignNode, 
		(PyrCompileNodeFunc)compilePyrMultiAssignNode, (PyrDumpNodeFunc)dumpPyrMultiAssignNode);
	newParseNodeClass(pn_MultiAssignVarListNode, 
		(PyrCompileNodeFunc)compilePyrMultiAssignVarListNode, (PyrDumpNodeFunc)dumpPyrMultiAssignVarListNode);
	newParseNodeClass(pn_BlockNode, 
		(PyrCompileNodeFunc)compilePyrBlockNode, (PyrDumpNodeFunc)dumpPyrBlockNode);
	newParseNodeClass(pn_ArgListNode, 
		(PyrCompileNodeFunc)compilePyrArgListNode, (PyrDumpNodeFunc)dumpPyrArgListNode);
	newParseNodeClass(pn_DynListNode, 
		(PyrCompileNodeFunc)compilePyrDynListNode, (PyrDumpNodeFunc)dumpPyrDynListNode);
	newParseNodeClass(pn_LitListNode, 
		(PyrCompileNodeFunc)compilePyrLitListNode, (PyrDumpNodeFunc)dumpPyrLitListNode);
}

void initParserPool()
{
	//postfl("initPool gParseNodePool pyr_pool_compile\n");
	gParseNodePool.Init(pyr_pool_compile, 32000, 32000, 2000);
}

void freeParserPool()
{
	//postfl("freePool gParseNodePool pyr_pool_compile\n");
	gParseNodePool.FreeAll();
}

PyrParseNodeClass* newParseNodeClass(int type, 
	PyrCompileNodeFunc compileFunc, PyrDumpNodeFunc dumpFunc)
{
	PyrParseNodeClass* classptr;
	classptr = (PyrParseNodeClass*)pyr_pool_runtime->Alloc(sizeof(PyrParseNodeClass));
	MEMFAIL(classptr);
	classptr->type = type;
	classptr->compileFunc = compileFunc;
	classptr->dumpFunc = dumpFunc;
	parseNodeClasses[type] = classptr;
	return classptr;
}

void compileNodeList(PyrParseNode *node)
{
	int dummy;
	//postfl("->compileNodeList\n");
	for (; node; node = node->next) {
            //postfl("-->compileNodeList %08X\n", node);
		COMPILENODE(node, &dummy);
            //postfl("<--compileNodeList %08X\n", node);
	}
	//postfl("<-compileNodeList\n");
}

void nodePostErrorLine(PyrParseNode* node)
{
	postErrorLine(node->lineno, linestarts[node->lineno], node->charno);
}

PyrPushNameNode* newPyrPushNameNode(PyrSlotNode *slotNode)
{
	PyrPushNameNode* node;
	node = (PyrPushNameNode*)slotNode;
	node->classno = pn_PushNameNode;
	return node;
}

void compilePushVar(PyrParseNode *node, PyrSymbol *varName)
{
	int level, index, vindex, varType;
	PyrBlock *tempfunc;
	PyrClass *classobj;
	
	//postfl("compilePushVar\n");
	classobj = gCompilingClass;
	if (varName->name[0] >= 'A' && varName->name[0] <= 'Z') {
		if (compilingCmdLine && varName->u.classobj == NULL) {
			error("Class not defined.\n");
			nodePostErrorLine(node);
			compileErrors++;
		} else {
			if (findSpecialClassName(varName, &index)) {
				compileOpcode(opExtended, opPushSpecialValue); // special op for pushing a class
				compileByte(index);
			} else {
				PyrSlot slot;
				SetSymbol(&slot, varName);
				index = conjureLiteralSlotIndex(node, gCompilingBlock, &slot);
				compileOpcode(opExtended, opExtended); // special op for pushing a class
				compileByte(index);
			}
		}
	} else if (varName == s_this || varName == s_super) {
		compileOpcode(opPushSpecialValue, opsvSelf);
	} else if (varName == s_true) {
		compileOpcode(opPushSpecialValue, opsvTrue);
	} else if (varName == s_false) {
		compileOpcode(opPushSpecialValue, opsvFalse);
	} else if (varName == s_nil) {
		compileOpcode(opPushSpecialValue, opsvNil);
	} else if (findVarName(gCompilingBlock, &classobj, varName, 
		&varType, &level, &index, &tempfunc)) {

		switch (varType) {
			case varInst :
				compileOpcode(opPushInstVar, index);
				break;
			case varClass : {
				PyrSlot slot;
				slot.ucopy = classobj->name.ucopy;
				vindex = conjureLiteralSlotIndex(node, gCompilingBlock, &slot);
				compileOpcode(opPushClassVar, vindex);
				compileByte(index);
			} break;
			case varTemp :
				vindex = METHRAW(tempfunc)->numtemps - index - 1;
				if (level == 0) {
					compileOpcode(opPushTempZeroVar, vindex);
				} else {
					compileOpcode(opPushTempVar, level);
					compileByte(vindex);
				}
				break;
			case varPseudo :
				compileOpcode(opExtended, opSpecialOpcode);
				compileByte(index);
				break;
		}
	} else {
		error("Variable '%s' not defined.\n", varName->name);
		nodePostErrorLine(node);
		compileErrors++;
		//Debugger();
	}
}

void compilePyrPushNameNode(PyrPushNameNode* node, void *result)
{
	//postfl("compilePyrPushNameNode\n");
	compilePushVar((PyrParseNode*)node, node->varName.us);
}

PyrSlotNode* newPyrSlotNode(PyrSlot *slot)
{
	PyrSlotNode* node;
	node = ALLOCNODE(PyrSlotNode);
	node->classno = pn_SlotNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->slot = *slot;
	return node;
}

void compilePyrSlotNode(PyrSlotNode* node, void *result)
{
	error("compilePyrSlotNode: shouldn't get here.\n"); 
	dumpObjectSlot(&node->slot);
	nodePostErrorLine((PyrParseNode*)node);
	compileErrors++;
	//Debugger();
}


PyrClassExtNode* newPyrClassExtNode(PyrSlotNode* className, PyrMethodNode* methods)
{
	PyrClassExtNode* node;
	node = ALLOCNODE(PyrClassExtNode);
	node->classno = pn_ClassExtNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->className = className;
	
	node->methods = methods;
	return node;
}

void compilePyrClassExtNode(PyrClassExtNode* node, void *result)
{
	PyrClass *classobj = node->className->slot.us->u.classobj;
	if (!classobj) {
		error("Class extension for nonexisting class '%s'\n", node->className->slot.us->name);
		return;
	}
	gCurrentClass = classobj;
	gCurrentMetaClass = classobj->classptr;
	compileExtNodeMethods(node);
}

void compileExtNodeMethods(PyrClassExtNode* node)
{
	PyrMethodNode *method;
	method = node->methods;
	for (; method; method = (PyrMethodNode*)method->next) {
		int dummy;
		//post("compile ext %s-%s\n", 
		method->extension = true;
		compilePyrMethodNode(method, &dummy);
	}
	gCompilingMethod = NULL;
	gCompilingBlock = NULL;
	gInliningLevel = 0;
}

PyrClassNode* newPyrClassNode(PyrSlotNode* className, PyrSlotNode* superClassName,
	PyrVarListNode* varlists, PyrMethodNode* methods, PyrSlotNode* indexType)
{
	PyrClassNode* node;
	node = ALLOCNODE(PyrClassNode);
	node->classno = pn_ClassNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->className = className;
	node->indexType = indexType;
	
	node->superClassName = superClassName;
	node->varlists = varlists;
	node->methods = methods;
	node->varTally[varInst] = 0;
	node->varTally[varClass] = 0;
	node->varTally[varTemp] = 0;
	//node->varTally[varPool] = 0;
	return node;
}

bool compareVarDefs(PyrClassNode* node, PyrClass* classobj)
{
	int numinstvars, numclassvars;
	int i, xinst, xclass;
	PyrVarListNode* varlist;
	PyrVarDefNode *vardef;
	PyrParseNode *errnode;
	PyrSymbol **varNames;
	bool isIntrinsic;
	
	isIntrinsic = classobj->classFlags.ui & classIsIntrinsic;

	numinstvars = numInstVars(classobj);
	numclassvars = numClassVars(classobj);
	if (numinstvars == node->varTally[varInst] + node->numsuperinstvars
		&& numclassvars <= node->varTally[varClass]) {
		
		xclass = 0;
		xinst = node->numsuperinstvars;
		varlist = node->varlists;
		for (; varlist; varlist = (PyrVarListNode*)varlist->next) {
			int type = varlist->flags;
			if (type == varInst) {
				vardef = varlist->varDefs;
				varNames = classobj->instVarNames.uosym->symbols;
				for (i=0; vardef; vardef = (PyrVarDefNode*)vardef->next, xinst++, ++i) {
					if (vardef->varName->slot.us != varNames[xinst]) {
						errnode = (PyrParseNode*)vardef;
						//post("A %s %d %d %d\n", vardef->varName->slot.us->name, 
						//	vardef->varName->slot.us, varNames[xinst].us, xinst);
						//post("A %s %s %d\n", vardef->varName->slot.us->name, 
						//	varNames[xinst].us->name, xinst);
						goto differExit;
					}
				}
			} else if (type == varClass) {
				vardef = varlist->varDefs;
				varNames = classobj->classVarNames.uosym->symbols;
				for (i=0; vardef && xclass < numclassvars; vardef = (PyrVarDefNode*)vardef->next, xclass++, ++i) {
					if (vardef->varName->slot.us != varNames[xclass]) {
						errnode = (PyrParseNode*)vardef;
						//post("B %d %d %d\n", vardef->varName->slot.us, varNames[xclass].us, xclass);
						goto differExit; 
					}
				}
			}
		}
	} else {
		//post("C %d %d %d   %d %d\n", numinstvars, node->varTally[varInst], node->numsuperinstvars,
		//	numclassvars, node->varTally[varClass]);
		errnode = (node->varlists ? (PyrParseNode*)node->varlists : (PyrParseNode*)node->className);
		goto differExit;
	}
	return false;
	
	differExit:
	if (isIntrinsic) {
		error("You may not change variable definitions of intrinsic classes.\n");
		nodePostErrorLine(errnode);
		compileErrors++;
	}
	return true;
}

void countClassVarDefs(PyrClassNode* node, int *numClassMethods, int *numInstMethods)
{
	PyrVarListNode* varlist;
	PyrVarDefNode *vardef;

	//*numClassMethods = 0;
	//*numInstMethods = 0;

	node->varTally[0] = 0;
	node->varTally[1] = 0;
	node->varTally[2] = 0;
	
	// count number of variables of each type
	varlist = node->varlists;
	for (; varlist; varlist = (PyrVarListNode*)varlist->next) {
		int type = varlist->flags;
		vardef = varlist->varDefs;
		for (; vardef; vardef = (PyrVarDefNode*)vardef->next) {
			node->varTally[type]++;
			if (type == varClass) {
				if (vardef->flags & rwReadOnly) {
					*numClassMethods = *numClassMethods + 1;
				}
				if (vardef->flags & rwWriteOnly) {
					*numClassMethods = *numClassMethods + 1;
				}
			} else if (type == varInst) {
				if (vardef->flags & rwReadOnly) {
					*numInstMethods = *numInstMethods + 1;
				}
				if (vardef->flags & rwWriteOnly) {
					*numInstMethods = *numInstMethods + 1;
				}
			}
		}
	}
}

void countNodeMethods(PyrClassNode* node, int *numClassMethods, int *numInstMethods)
{
	// count methods
	PyrMethodNode *method;
	//*numClassMethods = 0;
	//*numInstMethods = 0;
	method = node->methods;
	for (; method; method = (PyrMethodNode*)method->next) {
		if (method->isClassMethod) *numClassMethods = *numClassMethods + 1;
		else *numInstMethods = *numInstMethods + 1;
	}
}

void compileNodeMethods(PyrClassNode* node)
{
	PyrMethodNode *method;
	method = node->methods;
	for (; method; method = (PyrMethodNode*)method->next) {
		int dummy;
		method->extension = false;
		compilePyrMethodNode(method, &dummy);
	}
	gCompilingMethod = NULL;
	gCompilingBlock = NULL;
	gInliningLevel = 0;
}

PyrClass* getNodeSuperclass(PyrClassNode *node)
{
	PyrClass *superclassobj = NULL;
//	postfl("getNodeSuperclass node %d\n", node);
//	postfl("getNodeSuperclass node->superClassName %d\n", node->superClassName);
//	postfl("getNodeSuperclass node->superClassName->slot.utag %d\n", 
//		node->superClassName->slot.utag);
	if (node->superClassName && node->superClassName->slot.utag == tagSym) {
		superclassobj = node->superClassName->slot.us->u.classobj;
		if (superclassobj == NULL) {
			error("Cannot find superclass '%s' for class '%s'\n", 
				slotSymString(&node->superClassName->slot), 
				slotSymString(&node->className->slot));
			nodePostErrorLine((PyrParseNode*)node->superClassName);
			superclassobj = (PyrClass*)-1;
			compileErrors++;
		}
	} else {
		if (node->className->slot.us != s_object) {
			superclassobj = class_object;
		} // else this is object and there is no superclass
	}
	return superclassobj;
}

void fillClassPrototypes(PyrClassNode *node, PyrClass *classobj, PyrClass *superclassobj)
{
	PyrVarListNode* varlist;
	PyrVarDefNode *vardef;
	PyrSlot *islot, *cslot;
	PyrSymbol **inameslot, **cnameslot;
	PyrClass *metaclassobj;
	PyrMethod *method;
	PyrMethodRaw *methraw;
	int instVarIndex, classVarIndex;
	
	// copy superclass's prototype to here
	if (superclassobj && NotNil(&superclassobj->iprototype) 
		&& superclassobj->iprototype.uo->size) {
		
		memcpy(classobj->iprototype.uo->slots, superclassobj->iprototype.uo->slots,
			sizeof(PyrSlot) * superclassobj->iprototype.uo->size);
		//classobj->iprototype.uo->size = superclassobj->iprototype.uo->size;
		classobj->iprototype.uo->size = node->numsuperinstvars;
		
		memcpy(classobj->instVarNames.uosym->symbols, superclassobj->instVarNames.uosym->symbols,
			sizeof(PyrSymbol*) * superclassobj->instVarNames.uo->size);
		//classobj->instVarNames.uo->size = superclassobj->iprototype.uo->size;
		classobj->instVarNames.uo->size = node->numsuperinstvars;
	}
	// fill the class' own part of prototypes
	metaclassobj = classobj->classptr;
	varlist = node->varlists;
	if (NotNil(&classobj->iprototype)) {
		islot = classobj->iprototype.uo->slots + node->numsuperinstvars;
	}
	if (NotNil(&classobj->cprototype)) {
		cslot = classobj->cprototype.uo->slots;
	}
	if (NotNil(&classobj->instVarNames)) {
		inameslot = classobj->instVarNames.uosym->symbols + node->numsuperinstvars;
	}
	if (NotNil(&classobj->classVarNames)) {
		cnameslot = classobj->classVarNames.uosym->symbols;
	}
	instVarIndex = node->numsuperinstvars;
	classVarIndex = 0;
	for (; varlist; varlist = (PyrVarListNode*)varlist->next) {
		int type = varlist->flags;
		switch (type) {
			case varInst :
				vardef = varlist->varDefs;
				for (; vardef; vardef = (PyrVarDefNode*)vardef->next) {
					PyrSlot litslot;
					compilePyrLiteralNode(vardef->defVal, &litslot);
					*islot++ = litslot;
					classobj->iprototype.uo->size++;
					*inameslot++ = vardef->varName->slot.us;
					classobj->instVarNames.uosym->size++;
					if (vardef->flags & rwReadOnly) {
						// create getter method
						method = newPyrMethod();
						methraw = METHRAW(method);
                                                methraw->unused1 = 0;
                                                methraw->unused2 = 0;
						methraw->numargs = 1;
						methraw->numvars = 0;
						methraw->posargs = 1;
						methraw->varargs = 0;
						methraw->numtemps = 1;
						methraw->popSize = 0;
						SetNil(&method->context);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, classobj);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->varName->lineno]);
						method->name.ucopy = vardef->varName->slot.ucopy;
						methraw->methType = methReturnInstVar;
						methraw->specialIndex = instVarIndex;
						addMethod(classobj, method);
					}
					if (vardef->flags & rwWriteOnly) {
						char setterName[256];
						PyrSymbol *setterSym;
						sprintf(setterName, "%s_", vardef->varName->slot.us->name);
						//underscore = strcpy(setterName, vardef->varName->slot.us->name);
						//underscore[0] = '_';
						//underscore[1] = 0;
						setterSym = getsym(setterName);
						// create setter method
						method = newPyrMethod();
						methraw = METHRAW(method);
                                                methraw->unused1 = 0;
                                                methraw->unused2 = 0;
						methraw->numargs = 2;
						methraw->numvars = 0;
						methraw->posargs = 2;
						methraw->varargs = 0;
						methraw->numtemps = 2;
						methraw->popSize = 1;
						SetNil(&method->context);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, classobj);
						SetSymbol(&method->name, setterSym);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->varName->lineno]);

						methraw->methType = methAssignInstVar;
						methraw->specialIndex = instVarIndex;
						addMethod(classobj, method);
					}
					instVarIndex++;
				}
				break;
			case varClass :
				vardef = varlist->varDefs;
				for (; vardef; vardef = (PyrVarDefNode*)vardef->next) {
					PyrSlot litslot;
					compilePyrLiteralNode(vardef->defVal, &litslot);
					*cslot++ = litslot;
					classobj->cprototype.uo->size++;
					*cnameslot++ = vardef->varName->slot.us;
					classobj->classVarNames.uosym->size++;
					if (vardef->flags & rwReadOnly) {
						// create getter method
						method = newPyrMethod();
						methraw = METHRAW(method);
                                                methraw->unused1 = 0;
                                                methraw->unused2 = 0;
						methraw->numargs = 1;
						methraw->numvars = 0;
						methraw->posargs = 1;
						methraw->varargs = 0;
						methraw->numtemps = 1;
						methraw->popSize = 0;
						SetNil(&method->context);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, metaclassobj);
						method->name.ucopy = vardef->varName->slot.ucopy;
						SetSymbol(&method->literals, classobj->name.us);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->varName->lineno]);

						methraw->methType = methReturnClassVar;
						methraw->specialIndex = classVarIndex;
						addMethod(metaclassobj, method);
					}
					if (vardef->flags & rwWriteOnly) {
						char setterName[256];
						PyrSymbol *setterSym;
						sprintf(setterName, "%s_", vardef->varName->slot.us->name);
						//underscore = strcpy(setterName, vardef->varName->slot.us->name);
						//underscore[0] = '_';
						//underscore[1] = 0;
						setterSym = getsym(setterName);
						// create setter method
						method = newPyrMethod();
						methraw = METHRAW(method);
						methraw->numargs = 2;
						methraw->numvars = 0;
						methraw->posargs = 2;
						methraw->varargs = 0;
						methraw->numtemps = 2;
						methraw->popSize = 1;
						SetNil(&method->context);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, metaclassobj);
						SetSymbol(&method->name, setterSym);
						SetSymbol(&method->literals, classobj->name.us);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->varName->lineno]);

						methraw->methType = methAssignClassVar;
						methraw->specialIndex = classVarIndex;
						addMethod(metaclassobj, method);
					}
					classVarIndex++;
				}
				break;
		}
	}
}

int getIndexType(PyrClassNode *classnode)
{
	PyrSlotNode *node;
	int res;
	
	node = classnode->indexType;
	if (node == NULL) res = obj_notindexed;
	else {
		char *name;
		name = node->slot.us->name;
		if (strcmp(name, "slot") == 0) res = obj_slot;
		else if (strcmp(name, "double") == 0) res = obj_double;
		else if (strcmp(name, "float") == 0) res = obj_float;
		else if (strcmp(name, "int32") == 0) res = obj_int32;
		else if (strcmp(name, "int16") == 0) res = obj_int16;
		else if (strcmp(name, "int8") == 0) res = obj_int8;
		else if (strcmp(name, "char") == 0) res = obj_char;
		else if (strcmp(name, "symbol") == 0) res = obj_symbol;
		else {
			error("Illegal indexed type. Must be one of:\n"
				"   slot, double, float, int8, int16, int32, char\n");
			res = obj_slot;
			compileErrors++;
		}
	
	}
	return res;
}

void compilePyrClassNode(PyrClassNode* node, void *result)
{
	PyrClass *classobj, *superclassobj, *metaclassobj;
	int numClassMethods, numInstMethods;
	bool isIntrinsic;
	bool varsDiffer, superclassesDiffer, indexTypesDiffer;
	bool shouldRecompileSubclasses = false;
	int indexType;
	
	// find num instvars in superclass
	//postfl("class '%s'\n", node->className->slot.us->name);
	superclassobj = getNodeSuperclass(node);
	indexType = getIndexType(node);
	//postfl("%s %d\n", node->className->slot.us->name, indexType);
	
	if ((long)superclassobj == -1) {
		// redundant error message removed:
		//error("Can't find superclass of '%s'\n", node->className->slot.us->name);
		//nodePostErrorLine(node);
		return; // can't find superclass
	}
	node->numsuperinstvars = numSuperInstVars(superclassobj);
	
	numClassMethods = 0;
	numInstMethods = 0;
	countClassVarDefs(node, &numClassMethods, &numInstMethods);
	//postfl("accessor methods %d %d\n", numClassMethods, numInstMethods);
	countNodeMethods(node, &numClassMethods, &numInstMethods);
	//postfl("total methods %d %d\n", numClassMethods, numInstMethods);
	
	// get or make a class object
	// see if it already exists
	classobj = node->className->slot.us->u.classobj;
	if (classobj) {
		// deal with intrinsic classes or other classes being recompiled here.
		// recompile of subclasses not necessary if inst and class vars are
		// unchanged.
		metaclassobj = (PyrClass*)classobj->classptr;
		isIntrinsic = classobj->classFlags.ui & classIsIntrinsic;
		
		varsDiffer = compareVarDefs(node, classobj);
		if (varsDiffer) {
			if (isIntrinsic) {
				//error("Class '%s' declaration doesn't match intrinsic definition.\n",
				//	node->className->slot.us->name);
				return;
			} else {
				shouldRecompileSubclasses = true;
			}
		}
		superclassesDiffer = superclassobj != classobj->superclass.us->u.classobj;
		indexTypesDiffer = indexType != classobj->instanceFormat.ui;
		//postfl("%d %d %d\n", indexType, classobj->instanceFormat.ui);
		//if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
		if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
			if (isIntrinsic) {
				if (superclassesDiffer) {
					error("Superclass of '%s' does not match intrinisic definition.\n",
						node->className->slot.us->name);
					nodePostErrorLine((PyrParseNode*)(node->superClassName ? 
						node->superClassName : node->className));
					compileErrors++;
				}
				if (indexTypesDiffer) {
					error("Index type of '%s' does not match intrinisic definition.\n",
						node->className->slot.us->name);
					nodePostErrorLine((PyrParseNode*)(node->indexType ? 
						node->indexType : node->className));
					compileErrors++;
				}
				error("Class '%s' declaration doesn't match intrinsic definition.\n",
						node->className->slot.us->name);
				return;
			} else {
				shouldRecompileSubclasses = true;
			}
		}
		// reallocate fields in the class object
		reallocClassObj(metaclassobj,
			classClassNumInstVars, 0, 
			numClassMethods, indexType, 0);
			
		//postfl("^3 %d %d\n", metaclassobj, class_class);
		//postfl("^4 %d %d\n", metaclassobj->iprototype.uo, class_class->iprototype.uo);
		memcpy(metaclassobj->iprototype.uo->slots, class_class->iprototype.uo->slots,
			sizeof(PyrSlot) * classClassNumInstVars);
		memcpy(metaclassobj->instVarNames.uosym->symbols, class_class->instVarNames.uosym->symbols,
			sizeof(PyrSymbol*) * classClassNumInstVars);
		metaclassobj->iprototype.uo->size = classClassNumInstVars;
		metaclassobj->instVarNames.uosym->size = classClassNumInstVars;
		
		reallocClassObj(classobj,
			node->varTally[varInst] + node->numsuperinstvars, 
			node->varTally[varClass],
			numInstMethods, indexType, 0);
		
	} else {
		PyrSymbol *superClassName, *metaClassName, *metaSuperClassName;
		
		superClassName = superclassobj ? superclassobj->name.us : NULL;
		metaClassName = getmetasym(node->className->slot.us->name);
		metaClassName->flags |= sym_MetaClass;
		metaSuperClassName = superClassName ? getmetasym(superClassName->name) : NULL;
		
		metaclassobj = newClassObj(class_class, 
			metaClassName, metaSuperClassName,
			classClassNumInstVars, 0, numClassMethods, indexType, 0);
// test			
		//postfl("^1 %d %d\n", metaclassobj, class_class);
		//postfl("^2 %d %d\n", metaclassobj->iprototype.uo, class_class->iprototype.uo);
			
		memcpy(metaclassobj->iprototype.uo->slots, class_class->iprototype.uo->slots,
			sizeof(PyrSlot) * classClassNumInstVars);
		memcpy(metaclassobj->instVarNames.uosym->symbols, class_class->instVarNames.uosym->symbols,
			sizeof(PyrSymbol*) * classClassNumInstVars);
		metaclassobj->iprototype.uo->size = classClassNumInstVars;
		metaclassobj->instVarNames.uo->size = classClassNumInstVars;
// end test			
		classobj = newClassObj(metaclassobj, 
			node->className->slot.us, superClassName,
			node->varTally[varInst] + node->numsuperinstvars, 
			node->varTally[varClass], numInstMethods, indexType, 0);
	}
	gCurrentClass = classobj;
	gCurrentMetaClass = metaclassobj;
	if (gCompilingFileSym) {
		SetSymbol(&classobj->filenameSym, gCompilingFileSym);
		SetInt(&classobj->charPos, linestarts[node->className->lineno]);
		SetSymbol(&metaclassobj->filenameSym, gCompilingFileSym);
		SetInt(&metaclassobj->charPos, linestarts[node->className->lineno]);
	} else {
		SetNil(&classobj->filenameSym);
		SetNil(&metaclassobj->filenameSym);
	}
	
	// fill inst and class prototypes
	fillClassPrototypes(node, classobj, superclassobj);
		
	// compile methods
	compileNodeMethods(node);
	
	// recompileSubclasses
	if (shouldRecompileSubclasses) {
		recompileSubclasses(classobj);
	}
}

void recompileSubclasses(PyrClass* classobj)
{
}

#if 0
void catVarLists(PyrVarListNode *varlist);
void catVarLists(PyrVarListNode *varlist)
{
	PyrVarListNode *prevvarlist;
	PyrVarDefNode *vardef, *lastvardef;
	
	if (varlist) {
		// find end of this list
		vardef = varlist->varDefs;
		for (; vardef; vardef = (PyrVarDefNode*)vardef->next) {
			lastvardef = vardef;
		}
		prevvarlist = varlist;
		varlist = (PyrVarListNode*)varlist->next;
		
		for (; varlist; varlist = (PyrVarListNode*)varlist->next) {
			vardef = varlist->varDefs;
			if (lastvardef) {
				lastvardef->next = (PyrParseNode*)vardef;
			} else {
				prevvarlist->varDefs = vardef;
			}
			// find end of this list
			for (; vardef; vardef = (PyrVarDefNode*)vardef->next) {
				lastvardef = vardef;
			}
		}
	}
}

#else

void catVarLists(PyrVarListNode *varlist);
void catVarLists(PyrVarListNode *varlist)
{
	PyrVarListNode *prevvarlist;
	PyrVarDefNode *vardef, *lastvardef;
	
	if (varlist) {
		// find end of this list
		vardef = varlist->varDefs;
		lastvardef = (PyrVarDefNode*)vardef->tail;
		prevvarlist = varlist;
		varlist = (PyrVarListNode*)varlist->next;
		
		for (; varlist; varlist = (PyrVarListNode*)varlist->next) {
			vardef = varlist->varDefs;
			lastvardef->next = (PyrParseNode*)vardef;

			// find end of this list
			lastvardef = (PyrVarDefNode*)vardef->tail;
		}
	}
}
#endif

PyrMethodNode* newPyrMethodNode(PyrSlotNode* methodName, PyrSlotNode* primitiveName,
	PyrArgListNode* arglist, PyrVarListNode *varlist, PyrParseNode* body, int isClassMethod)
{
	PyrMethodNode* node;
	node = ALLOCNODE(PyrMethodNode);
	node->classno = pn_MethodNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->methodName = methodName;
	node->primitiveName = primitiveName;
	node->arglist = arglist;
	catVarLists(varlist);
	node->varlist = varlist;
	node->body = body;
	node->isClassMethod = isClassMethod;
	return node;
}

enum { push_Normal, push_AllArgs, push_AllButFirstArg, push_AllButFirstArg2  };

int checkPushAllArgs(PyrParseNode *actualArg, int numArgs);
int checkPushAllArgs(PyrParseNode *actualArg, int numArgs)
{
	PyrBlock *block;
	PyrPushNameNode *nameNode;
	block = gCompilingBlock;
	int i;
		
	//if (strcmp("ar", gCompilingMethod->name.us->name)==0) Debugger();
	if (actualArg->classno != pn_PushNameNode) {
		if (numArgs < 3) {
			return push_Normal;
		}
		actualArg = actualArg->next;
		for (i=1; i<numArgs; ++i) {
			if (actualArg->classno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			if (nameNode->varName.us != block->argNames.uosym->symbols[i]) {
				return push_Normal;
			}
			
			actualArg = actualArg->next;
		}
		return push_AllButFirstArg;
				
	} else {
		for (i=0; i<numArgs; ++i) {
			if (actualArg->classno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			if (nameNode->varName.us != block->argNames.uosym->symbols[i]) {
				return push_Normal;
			}
			actualArg = actualArg->next;
		}
		return push_AllArgs;
	}
}


int checkPushAllButFirstTwoArgs(PyrParseNode *actualArg, int numArgs);
int checkPushAllButFirstTwoArgs(PyrParseNode *actualArg, int numArgs)
{
	PyrBlock *block;
	PyrPushNameNode *nameNode;
	block = gCompilingBlock;
	int i;
		
	if (numArgs >= 2) {
		actualArg = actualArg->next;
		actualArg = actualArg->next;
		for (i=1; i<numArgs; ++i) {
			if (actualArg->classno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			/*if (gCompilingClass->name.us == s_ugen) {
				post("check meth %s  %d  '%s' '%s'\n", gCompilingMethod->name.us->name, i, 
					nameNode->varName.us->name, 
					block->argNames.uosym->symbols[i]->name);
			}*/
			if (nameNode->varName.us != block->argNames.uosym->symbols[i]) {
				return push_Normal;
			}
			
			actualArg = actualArg->next;
		}
		return push_AllButFirstArg2;
	}
	return push_Normal;
}

int compareCallArgs(PyrMethodNode* node, PyrCallNode *cnode, int *varIndex)
{
	int i, numFormalArgs, numActualArgs;
	int special, varType, varLevel;
	PyrParseNode *actualArg;
	PyrVarDefNode *formalArg;
	PyrPushNameNode *nameNode;
	
	// fail if has a rest arg .. too much trouble?
	if (node->arglist && node->arglist->rest) {
		return methNormal;
	}
	
	// check first actual arg is 'this'
	actualArg = cnode->arglist;
	if (actualArg->classno != pn_PushNameNode) {
		return methNormal;
	}
	nameNode = (PyrPushNameNode*)actualArg;
	if (nameNode->varName.us == s_this) {
		special = methRedirect;
	} else {
		bool varFound;
		PyrClass *classobj;
		
		classobj = gCompilingClass;
		varFound = findVarName(gCompilingBlock, &classobj, nameNode->varName.us, 
			&varType, &varLevel, varIndex, NULL);
		if (!varFound ) return methNormal;

		if (varType == varInst) special = methForward;
		//else if (varType == varTemp) special = methTempDelegate;
		else return methNormal;
	}
	
	actualArg = actualArg->next;
	numActualArgs = nodeListLength((PyrParseNode*)cnode->arglist);
	
	if (!node->arglist) {
		numFormalArgs = 1;
		if (numActualArgs > numFormalArgs) {
			return methNormal;
		}
	} else {
		numFormalArgs = 1 + nodeListLength((PyrParseNode*)node->arglist->varDefs);
		if (numActualArgs > numFormalArgs) {
			return methNormal;
		}
		
		formalArg = node->arglist->varDefs;
		for (i=0; i<numActualArgs-1; ++i) {
			if (actualArg->classno != pn_PushNameNode) {
				return methNormal;
			}

			nameNode = (PyrPushNameNode*)actualArg;
			if (nameNode->varName.us != formalArg->varName->slot.us) {
				return methNormal;
			}
			
			formalArg = (PyrVarDefNode*)formalArg->next;
			actualArg = actualArg->next;
		}
	}
//	if (special == methForward) {
//		postfl("methForward %s-%s\n", gCompilingClass->name.us->name, 
//			gCompilingMethod->name.us->name);
//	}
//	if (special == methTempDelegate) {
//		postfl("methTempDelegate %s-%s\n", gCompilingClass->name.us->name, 
//			gCompilingMethod->name.us->name);
//	}
	return special;
}

void installByteCodes(PyrBlock *block)
{
	PyrInt8Array *byteArray;
	long length, flags;
	ByteCodes byteCodes;
 	byteCodes = getByteCodes();
 	if (byteCodes) {
	 	length = byteCodeLength(byteCodes);
	 	if (length) {
			flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
			byteArray = newPyrInt8Array(compileGC(), length, flags, false);
			copyByteCodes(byteArray->b, byteCodes);
			byteArray->size = length;
			freeByteCodes(byteCodes);
			SetObject(&block->code, byteArray);
		} else {
			error("installByteCodes: zero length byte codes\n");
		}
	} else {
		error("installByteCodes: NULL byte codes\n");
	}
}

void compilePyrMethodNode(PyrMethodNode* node, void *result)
{
	PyrMethod *method, *oldmethod;
	PyrMethodRaw *methraw;
	int i, j, numArgs, numVars, methType, funcVarArgs, firstKeyIndex;
	int index, numSlots, numArgNames;
	bool hasPrimitive = false;
	PyrVarDefNode *vardef;
	PyrObject *proto;
	PyrSymbolArray *argNames, *varNames;
		
	//postfl("->method '%s'\n", node->methodName->slot.us->name);
	if (node->isClassMethod) {
		gCompilingClass = gCurrentMetaClass;
		oldmethod = classFindDirectClassMethod(gCompilingClass, node->methodName->slot.us);
	} else {
		gCompilingClass = gCurrentClass;
		oldmethod = classFindDirectInstMethod(gCompilingClass, node->methodName->slot.us);
	}
	if (oldmethod && !node->extension) {
		error("Method %s-%s already defined.\n", 
			oldmethod->ownerclass.uoc->name.us->name, oldmethod->name.us->name);
		nodePostErrorLine((PyrParseNode*)node->methodName);
		compileErrors++;
		return;
	}
	if (oldmethod) {
		method = oldmethod;
		freePyrSlot(&method->code);
		freePyrSlot(&method->literals);
		freePyrSlot(&method->prototypeFrame);
		freePyrSlot(&method->argNames);
		freePyrSlot(&method->varNames);
	} else {
		method = newPyrMethod();
		SetObject(&method->ownerclass, gCompilingClass);
	}
	//method->flags = node->isClassMethod;
	
	methraw = METHRAW(method);
	methraw->unused1 = 0;
	methraw->unused2 = 0;
	
	//postfl("method %08X raw %08X\n", method, methraw);
	method->context = o_nil;
	method->name = node->methodName->slot;
	if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
	SetInt(&method->charPos, linestarts[node->methodName->lineno]);
	if (node->primitiveName) {
		hasPrimitive = true;
		method->primitiveName = node->primitiveName->slot;
		methraw->specialIndex = node->primitiveName->slot.us->u.index;
	}
	gCompilingBlock = (PyrBlock*)method;
	gCompilingMethod = (PyrMethod*)method;
	gInliningLevel = 0;
		
#if USESTACKFRAMES
	methraw->needsHeapContext = 0;
#else
	methraw->needsHeapContext = 1;
#endif

	methraw->varargs = funcVarArgs = (node->arglist && node->arglist->rest) ? 1 : 0;
	numArgs = node->arglist ? nodeListLength((PyrParseNode*)node->arglist->varDefs) + 1 : 1;
	numVars = node->varlist ? nodeListLength((PyrParseNode*)node->varlist->varDefs) : 0;

	numSlots = numArgs + funcVarArgs + numVars;
	methraw->frameSize = (numSlots + FRAMESIZE) * sizeof(PyrSlot);

	methraw->numargs = numArgs;
	methraw->numvars = numVars;
	methraw->posargs = numArgs + funcVarArgs;
	methraw->numtemps = numSlots;
	methraw->popSize = numSlots - 1;
	firstKeyIndex = numArgs + funcVarArgs;

	numArgNames = methraw->posargs;
	
	if (numSlots == 1) {
		method->argNames.ucopy = o_argnamethis.ucopy;
		method->prototypeFrame.ucopy = o_onenilarray.ucopy;
	} else {
		argNames = newPyrSymbolArray(NULL, numArgNames, obj_permanent | obj_immutable, false);
		argNames->size = numArgNames;
		SetObject(&method->argNames, argNames);

		proto = newPyrArray(NULL, numSlots, obj_permanent | obj_immutable, false);
		proto->size = numSlots;
		SetObject(&method->prototypeFrame, proto);

		// declare args
		method->argNames.uosym->symbols[0] = s_this;
		if (node->arglist) {
			PyrSymbol **methargs;
			methargs = method->argNames.uosym->symbols;
			vardef = node->arglist->varDefs;
			for (i=1; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->next) {
				PyrSlot *varslot;
				varslot = &vardef->varName->slot;
				// already declared as arg?
				for (j=0; j<i; ++j) {
					if (methargs[j] == varslot->us) {
						error("Argument '%s' already declared in %s-%s\n",
							varslot->us->name, 
							gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
						nodePostErrorLine((PyrParseNode*)vardef);
						compileErrors++;
					}
				}
				// put it in arglist
				methargs[i] = varslot->us;
				//postfl("defarg %d '%s'\n", i, slot->us->name);
				/*if (varslot->us->name[0] == 'a' 
					&& varslot->us->name[1] == 'r' 
					&& varslot->us->name[2] == 'g')
				{
					post("%d  %s-%s   '%s'\n", i,
						gCompilingClass->name.us->name, 
						gCompilingMethod->name.us->name, 
						varslot->us->name);
				}*/
			}
			if (funcVarArgs) {
				PyrSlot *varslot;
				varslot = &node->arglist->rest->slot;
				// already declared as arg?
				for (j=0; j<numArgs; ++j) {
					if (methargs[j] == varslot->us) {
						error("Argument '%s' already declared in %s-%s\n",
							varslot->us->name, 
							gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
						nodePostErrorLine((PyrParseNode*)vardef);
						compileErrors++;
					}
				}
				// put it in arglist
				methargs[i] = varslot->us;
				//postfl("defrest '%s'\n", slot->us->name);
			}
		}
		// fill prototype args
		if (NotNil(&method->prototypeFrame)) {
			SetNil(&method->prototypeFrame.uo->slots[0]);
		}
		if (node->arglist) {
			vardef = node->arglist->varDefs;
			for (i=1; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->next) {
				PyrSlot *slot, litval;
				slot = method->prototypeFrame.uo->slots + i;
				compilePyrLiteralNode(vardef->defVal, &litval);
				*slot = litval;
			}
			if (funcVarArgs) {
				//SetNil(&method->prototypeFrame.uo->slots[numArgs]);
				method->prototypeFrame.uo->slots[numArgs].ucopy = o_emptyarray.ucopy;
			}
		}
	}
	
	if (numVars) {
		varNames = newPyrSymbolArray(NULL, numVars, obj_permanent | obj_immutable, false);
		varNames->size = numVars;
		SetObject(&method->varNames, varNames);
	} else {
		SetNil(&method->varNames);
	}
	
	// declare vars
	if (node->varlist) {
		PyrSymbol **methargs, **methvars;
		methargs = method->argNames.uosym->symbols;
		methvars = method->varNames.uosym->symbols;
		vardef = node->varlist->varDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *varslot;
			varslot = &vardef->varName->slot;
			// already declared as arg?
			for (j=0; j<numArgNames; ++j) {
				if (methargs[j] == varslot->us) {
					error("Variable '%s' already declared in %s-%s\n",
						varslot->us->name, 
						gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// already declared as var?
			for (j=0; j<i; ++j) {
				if (methvars[j] == varslot->us) {
					error("Variable '%s' already declared in %s-%s\n",
						varslot->us->name, 
						gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in varlist
			methvars[i] = varslot->us;
			//postfl("defvar %d '%s'\n", i, slot->us->name);
		}
	}
	
	if (node->varlist) {
		vardef = node->varlist->varDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *slot, litval;
			slot = method->prototypeFrame.uo->slots + i + numArgs + funcVarArgs;
			compilePyrLiteralNode(vardef->defVal, &litval);
			*slot = litval;
		}
	}
	
	methType = methNormal;
	if (hasPrimitive) {
		methType = methPrimitive;
	} else {
		int bodyType = node->body->classno;
		if (bodyType == pn_ReturnNode) {
			PyrReturnNode *rnode;
			PyrParseNode *xnode;
			int rtype;
			PyrSlot rslot;
			
			rnode = (PyrReturnNode*)node->body;
			xnode = (PyrParseNode*)rnode->expr;
			if (xnode) {
				rtype = xnode->classno;
				if (rtype == pn_PushLitNode) { // return literal ?
					compilePyrLiteralNode((PyrLiteralNode*)xnode, &rslot);
					if (rslot.utag == tagObj && rslot.uo->classptr == class_fundef) {
						methType = methNormal;
					} else {
						methType = methReturnLiteral;
						method->literals = rslot;
					}
				} else if (rtype == pn_PushNameNode) {
					PyrSlot *rslot;
					rslot = &((PyrPushNameNode*)xnode)->varName;
					if (rslot->us == s_this) {	// return this
						methType = methReturnSelf;
					} else {
						if (funcFindArg((PyrBlock*)method, rslot->us, &index)) { // return arg ?
							// eliminate the case where its an ellipsis or keyword argument
							if (index < methraw->numargs) { 
								methType = methReturnArg;
								methraw->specialIndex = index; // when you change sp to sp - 1
								//methraw->specialIndex = index - 1;
							}
						} else if (classFindInstVar(gCompilingClass, rslot->us, &index)) {
							// return inst var
							methType = methReturnInstVar;
							methraw->specialIndex = index;
						}
					}
				} else if (rtype == pn_CallNode) {
					// need to do this for binary opcodes too..
					int specialIndex;
					PyrCallNode *cnode;
					cnode = (PyrCallNode*)xnode;
					methType = compareCallArgs(node, cnode, &specialIndex);
					if (methType != methNormal) {
						methraw->specialIndex = specialIndex;
						method->literals = cnode->selector->slot;
					}
				}
			} else {
				methType = methReturnSelf;
			}
		} else if (bodyType == pn_AssignNode && numArgs == 2) { // assign inst var ?
			PyrAssignNode *anode;
					//post("methAssignInstVar 1  %s-%s\n", 
					//	gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
			anode = (PyrAssignNode*)node->body;
			if (anode->next && anode->next->classno == pn_ReturnNode
				&& ((PyrReturnNode*)anode->next)->expr == NULL) {
					//post("methAssignInstVar 2  %s-%s\n", 
					//	gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
				if (classFindInstVar(gCompilingClass, anode->varName->slot.us, &index)) {
					methType = methAssignInstVar;
					methraw->specialIndex = index;
					//post("methAssignInstVar 3  %s-%s\n", 
					//	gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
				}
			}
		}
	}

	methraw->methType = methType;
	// set primitive
	// optimize common cases
	
	if (methType == methNormal || methType == methPrimitive) {
		long dummy;
		PyrSymbol *name;
		
		// compile body
		initByteCodes();
		
		if (gCompilingClass == class_int) {
			// handle some special cases
			name = method->name.us;
			if (name == gSpecialSelectors[opmDo]) {
				compileByte(143);
				compileByte(0);
				compileByte(143);
				compileByte(1);
			} else if (name == gSpecialSelectors[opmReverseDo]) {
				compileByte(143);
				compileByte(2);
				compileByte(143);
				compileByte(3);
				compileByte(143);
				compileByte(4);
			} else if (name == gSpecialSelectors[opmFor]) {
				compileByte(143);
				compileByte(5);
				compileByte(143);
				compileByte(6);
				compileByte(143);
				compileByte(16);
			} else if (name == gSpecialSelectors[opmForBy]) {
				compileByte(143);
				compileByte(7);
				compileByte(143);
				compileByte(8);
				compileByte(143);
				compileByte(9);
			} else goto compile_body;
		} else if (gCompilingClass == class_arrayed_collection) {
			name = method->name.us;
			if (name == gSpecialSelectors[opmDo]) {
				compileByte(143);
				compileByte(10);
				compileByte(143);
				compileByte(1);
			} else if (name == gSpecialSelectors[opmReverseDo]) {
				compileByte(143);
				compileByte(11);
				compileByte(143);
				compileByte(12);
				compileByte(143);
				compileByte(4);
			} else goto compile_body;
		} else if (gCompilingClass->name.us == s_dictionary) {
			name = method->name.us;
			if (name == getsym("keysValuesArrayDo")) {
				compileByte(143);
				compileByte(13);
				compileByte(143);
				compileByte(14);
			} else goto compile_body;
		} else if (gCompilingClass == class_inf) {
			name = method->name.us;
			if (name == gSpecialSelectors[opmDo]) {
				compileByte(143);
				compileByte(15);
				compileByte(143);
				compileByte(1);
			} else goto compile_body;
		} else if (gCompilingClass == class_float) {
			// handle some special cases
			name = method->name.us;
			if (name == gSpecialSelectors[opmDo]) {
				compileByte(143);
				compileByte(17);
				compileByte(143);
				compileByte(18);
			} else if (name == gSpecialSelectors[opmReverseDo]) {
				compileByte(143);
				compileByte(19);
				compileByte(143);
				compileByte(20);
				compileByte(143);
				compileByte(21);
			} else goto compile_body;
		} else {
			compile_body:
			COMPILENODE(node->body, &dummy);
		}
		installByteCodes((PyrBlock*)method);
	}

	if (!node->extension || !oldmethod) {
		addMethod(gCompilingClass, method);
	}

	gCompilingMethod = NULL;
	gCompilingBlock = NULL;

	//postfl("<-method '%s'\n", node->methodName->slot.us->name);
}

PyrArgListNode* newPyrArgListNode(PyrVarDefNode* varDefs, PyrSlotNode* rest)
{
	PyrArgListNode* node;
	node = ALLOCNODE(PyrArgListNode);
	node->classno = pn_ArgListNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varDefs = varDefs;
	node->rest = rest;
	return node;
}

void compilePyrArgListNode(PyrArgListNode* node, void *result)
{
	error("compilePyrArgListNode: shouldn't get here.\n"); 
	compileErrors++;
}


PyrVarListNode* newPyrVarListNode(PyrVarDefNode* vardefs, int flags)
{
	PyrVarListNode* node;
	node = ALLOCNODE(PyrVarListNode);
	node->classno = pn_VarListNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varDefs = vardefs;
	node->flags = flags;
	return node;
}

void compilePyrVarListNode(PyrVarListNode* node, void *result)
{
	error("compilePyrVarListNode: shouldn't get here.\n"); 
	compileErrors++;
}

PyrVarDefNode* newPyrVarDefNode(PyrSlotNode* varName, PyrLiteralNode* defVal,
	int flags)
{
	PyrVarDefNode* node;
	node = ALLOCNODE(PyrVarDefNode);
	node->classno = pn_VarDefNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varName = varName;
	node->defVal = defVal;
	node->flags = flags;
	return node;
}

void compilePyrVarDefNode(PyrVarDefNode* node, void *result)
{
	error("compilePyrVarDefNode: shouldn't get here.\n"); 
	compileErrors++;
}

PyrCallNode* newPyrCallNode(PyrSlotNode* selector, PyrParseNode* arglist, 
	PyrParseNode* keyarglist)
{
	PyrCallNode* node;
	node = ALLOCNODE(PyrCallNode);
	node->classno = pn_CallNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->selector = selector;
	node->arglist = arglist;
	node->keyarglist = keyarglist;
	return node;
}

void compilePyrCallNode(PyrCallNode* node, void *result)
{
	int index, selType, isSuper, numArgs, numKeyArgs, numBlockArgs;
	long dummy;
	bool varFound;
	PyrParseNode *argnode, *argnode2;
	PyrParseNode *keynode;
	
	//postfl("compilePyrCallNode\n");	
	argnode = node->arglist;
	keynode = node->keyarglist;
	numArgs = nodeListLength(argnode);
	numKeyArgs = nodeListLength(keynode);
	isSuper = isSuperObjNode(argnode);
	numBlockArgs = METHRAW(gCompilingBlock)->numargs;
	
	node->selector->slot.us->flags |= sym_Called;
	index = conjureSelectorIndex((PyrParseNode*)node->selector, gCompilingBlock, 
		isSuper, node->selector->slot.us, &selType);

	if (numKeyArgs > 0 || numArgs > 15) {
		for (; argnode; argnode = argnode->next) {
			COMPILENODE(argnode, &dummy);
		}
		for (; keynode; keynode = keynode->next) {
			COMPILENODE(keynode, &dummy);
		}
		if (isSuper) {
			compileByte(opSendSuper);
			compileByte(numArgs + 2*numKeyArgs);
			compileByte(numKeyArgs);
			compileByte(index);
		} else {
			switch (selType) {
				case selNormal :
					compileByte(opSendMsg);
					compileByte(numArgs + 2*numKeyArgs);
					compileByte(numKeyArgs);
					compileByte(index);
					break;
				case selSpecial :
					compileByte(opSendSpecialMsg);
					compileByte(numArgs + 2*numKeyArgs);
					compileByte(numKeyArgs);
					compileByte(index);
					break;
				default:
					compileByte(opSendMsg);
					compileByte(numArgs + 2*numKeyArgs);
					compileByte(numKeyArgs);
					compileByte(index);
					break;
			}
		}
	} else if (isSuper) {
		if (numArgs == 1) {
			// pushes this as well, don't compile arg
			compileOpcode(opSendSuper, 0);
			compileByte(index);
		} else {
			for (; argnode; argnode = argnode->next) {
				COMPILENODE(argnode, &dummy);
			}
			compileOpcode(opSendSuper, numArgs);
			compileByte(index);
		}
	} else {
		PyrSymbol *varname;
		if (argnode->classno == pn_PushNameNode) {
			varname = ((PyrPushNameNode*)argnode)->varName.us;
		} else {
			varname = NULL;
		}
		switch (selType) {
			case selNormal :
				if (numArgs == 1 && varname == s_this) {
					compileOpcode(opSendMsg, 0);
					compileByte(index);
				//} else if (numArgs>1 && numArgs == numBlockArgs) {
				} else if (numArgs>1 && numArgs == numBlockArgs) {
					// try for multiple push optimization
					int code;
					code = checkPushAllArgs(argnode, numArgs);
					if (code == push_Normal) goto normal;
					else if (code == push_AllArgs) {
						compileByte(137); // push all args, send msg
						compileByte(index);
						//post("137 pushAllArgs     %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else if (code == push_AllButFirstArg) {
						COMPILENODE(argnode, &dummy);
						compileByte(138); // push all but first arg, send msg
						compileByte(index);
						//post("138 pushAllButFirstArg     %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else goto normal;
				} else if (numArgs>2 && numArgs == numBlockArgs+1) {
					int code;
					code = checkPushAllButFirstTwoArgs(argnode, numBlockArgs);
					if (code == push_Normal) goto normal;
					else if (code == push_AllButFirstArg2) {
						COMPILENODE(argnode, &dummy);
						COMPILENODE(argnode->next, &dummy);
						compileByte(141); // one arg pushed, push all but first arg, send msg
						compileByte(index);
						//post("141 pushAllButFirstArg2    %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else goto normal;
					
				} else {
					normal:
					for (; argnode; argnode = argnode->next) {
						COMPILENODE(argnode, &dummy);
					}
					compileOpcode(opSendMsg, numArgs);
					compileByte(index);
				}
				break;
			case selSpecial :
				if (numArgs == 1) {
					if (varname == s_this) {
						compileOpcode(opSendSpecialMsg, 0);
						compileByte(index);
					} else if (varname) {
						PyrClass *classobj;
						PyrBlock *tempFunc;
						int varType, varLevel, varIndex;
						classobj = gCompilingClass;
						varFound = findVarName(gCompilingBlock, &classobj, varname,
							&varType, &varLevel, &varIndex, &tempFunc);
						if (varFound && varType == varInst) {
							//post("136 pushInstVar(sp) %s-%s '%s' %d %d\n", gCompilingClass->name.us->name,
							//	gCompilingMethod->name.us->name, varname->name, varIndex, index);
							compileByte(136);
							compileByte(varIndex);
							compileByte(index);
						} else goto special;
					} else goto special;
				} else if (numArgs>1 && numArgs == numBlockArgs) {
				//} else if (numArgs>1 && numArgs == numBlockArgs) {
					// try for multiple push optimization
					int code;
					code = checkPushAllArgs(argnode, numArgs);
					if (code == push_Normal) goto special;
					else if (code == push_AllArgs) {
						compileByte(139); // push all args, send special msg
						compileByte(index);
						//post("139 pushAllArgs(sp) %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else if (code == push_AllButFirstArg) {
						COMPILENODE(argnode, &dummy);
						compileByte(140); // push all but first arg, send special msg
						compileByte(index);
						//post("140 pushAllButFirstArg(sp) %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else goto special;
				} else if (numArgs>2 && numArgs == numBlockArgs+1) {
					int code;
					code = checkPushAllButFirstTwoArgs(argnode, numBlockArgs);
					if (code == push_Normal) goto special;
					else if (code == push_AllButFirstArg2) {
						COMPILENODE(argnode, &dummy);
						COMPILENODE(argnode->next, &dummy);
						compileByte(142); // one arg pushed, push all but first arg, send msg
						compileByte(index);
						//post("142 pushAllButFirstArg2(sp)    %s-%s\n", gCompilingClass->name.us->name,
						//	gCompilingMethod->name.us->name);
					} else goto special;
				} else {
					int i;
					special:
					for (i=0; argnode; argnode = argnode->next,i++) {
						COMPILENODE(argnode, &dummy);
					}
					compileOpcode(opSendSpecialMsg, numArgs);
					compileByte(index);
				}
				break;
			case selUnary :
				if (numArgs != 1) {
					index = conjureLiteralSlotIndex((PyrParseNode*)node->selector, gCompilingBlock, 
								&node->selector->slot);
					goto defaultCase;
				}
				for (; argnode; argnode = argnode->next) {
					COMPILENODE(argnode, &dummy);
				}
				compileOpcode(opSendSpecialUnaryArithMsg, index);
				break;
			case selBinary :
				if (numArgs != 2) {
					index = conjureLiteralSlotIndex((PyrParseNode*)node->selector, gCompilingBlock, 
								&node->selector->slot);
					goto defaultCase;
				}
				//for (; argnode; argnode = argnode->next) {
				//	COMPILENODE(argnode, &dummy);
				//}
				argnode2 = argnode->next;
				if (index == opAdd && argnode2->classno == pn_PushLitNode
					&& ((PyrPushLitNode*)argnode2)->literalSlot.utag == tagInt
					&& ((PyrPushLitNode*)argnode2)->literalSlot.ui == 1) {
					COMPILENODE(argnode, &dummy);
					compileOpcode(opPushSpecialValue, opsvPlusOne);
				} else if (index == opSub && argnode2->classno == pn_PushLitNode
					&& ((PyrPushLitNode*)argnode2)->literalSlot.utag == tagInt
					&& ((PyrPushLitNode*)argnode2)->literalSlot.ui == 1) {
					COMPILENODE(argnode, &dummy);
					compileOpcode(opPushSpecialValue, opsvMinusOne);
				} else {
					COMPILENODE(argnode, &dummy);
					COMPILENODE(argnode->next, &dummy);
					compileOpcode(opSendSpecialBinaryArithMsg, index);
				}
				break;
			case selIf :
				compileIfMsg(node);
				break;
			case selWhile :
				compileWhileMsg(node);
				break;
			case selLoop :
				compileLoopMsg(node);
				break;
			case selAnd :
				if (numArgs == 2) compileAndMsg(argnode, argnode->next);
				else goto defaultCase;
				break;
			case selOr :
				if (numArgs == 2) compileOrMsg(argnode, argnode->next);
				else goto defaultCase;
				break;
			default :
				defaultCase:
				if (numArgs == 1 && varname == s_this) {
					compileOpcode(opSendMsg, 0);
					compileByte(index);
				} else {
					for (; argnode; argnode = argnode->next) {
						COMPILENODE(argnode, &dummy);
					}
					compileOpcode(opSendMsg, numArgs);
					compileByte(index);
				}
				break;
		}
	}
}

ByteCodes compileSubExpression(PyrPushLitNode* litnode)
{
	return compileSubExpressionWithGoto(litnode, 0);
}

ByteCodes compileSubExpressionWithGoto(PyrPushLitNode* litnode, int branchLen)
{
	ByteCodes	currentByteCodes, subExprByteCodes;
	PyrBlockNode *bnode;
	long dummy;
	
	bnode = (PyrBlockNode*)litnode->literalSlot.uo;
	currentByteCodes = saveByteCodeArray();

	//compileStatements(expr->body, false);
	COMPILENODE(bnode->body, &dummy);
	if (branchLen) {
		compileJump(opcJumpFwd, branchLen);
	}

	subExprByteCodes = getByteCodes();
	restoreByteCodeArray(currentByteCodes);

	return subExprByteCodes;
}

#if 0
ByteCodes compileDefaultValue(int litIndex, int realExprLen)
{
  ByteCodes	currentByteCodes, defaultByteCodes;

  currentByteCodes = saveByteCodeArray();

  compileOpcode(opPushSpecialValue, litIndex);
  compileJump(realExprLen, unconditionalJump);

  defaultByteCodes = getByteCodes();
  restoreByteCodeArray(currentByteCodes);

  return (defaultByteCodes);
}
#endif

bool isAnInlineableBlock(PyrParseNode *node)
{
	bool res = false;
	if (node->classno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		PyrBlockNode *bnode;
		anode = (PyrPushLitNode*)node;
		if (anode->literalSlot.utag == tagPtr 
				&& (bnode = (PyrBlockNode*)(anode->literalSlot.uo))->classno == pn_BlockNode) {
			if (bnode->arglist || bnode->varlist) {
				post("WARNING: FunctionDef contains variable declarations and so"
				" will not be inlined.\n");
				if (bnode->arglist) nodePostErrorLine((PyrParseNode*)bnode->arglist);
				else nodePostErrorLine((PyrParseNode*)bnode->varlist);
				
			} else {
				res = true;
			}
		}
	}
	return res;
}

bool isWhileTrue(PyrParseNode *node)
{
	bool res = false;
	if (node->classno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		PyrBlockNode *bnode;
		anode = (PyrPushLitNode*)node;
		if (anode->literalSlot.utag == tagPtr 
				&& (bnode = (PyrBlockNode*)(anode->literalSlot.uo))->classno == pn_BlockNode) {
			if (bnode->arglist || bnode->varlist) {
				/*
				post("WARNING: FunctionDef contains variable declarations and so"
					" will not be inlined.\n");
				if (bnode->arglist) nodePostErrorLine((PyrParseNode*)bnode->arglist);
				else nodePostErrorLine((PyrParseNode*)bnode->varlist);
				*/
			} else {
				if (bnode->body->classno == pn_PushLitNode 
						&& ((PyrPushLitNode*)bnode->body)->literalSlot.utag == tagTrue) {
					res = true;
				}
			}
		} else if (anode->literalSlot.utag == tagTrue) {
			res = true;
		} 
	}
	return res;
}

void compileAndMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	long dummy;
	ByteCodes trueByteCodes;
	
	COMPILENODE(arg1, &dummy);
	if (isAnInlineableBlock(arg2)) {
		trueByteCodes = compileSubExpression((PyrPushLitNode*)arg2);
		
		compileJump(opcJumpIfFalsePushFalse, byteCodeLength(trueByteCodes));
		compileAndFreeByteCodes(trueByteCodes);
	} else {
		COMPILENODE(arg2, &dummy);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAnd);
	}
}

void compileOrMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	long dummy;
	ByteCodes falseByteCodes;
	
	COMPILENODE(arg1, &dummy);
	if (isAnInlineableBlock(arg2)) {
		falseByteCodes = compileSubExpression((PyrPushLitNode*)arg2);
		
		compileJump(opcJumpIfTruePushTrue, byteCodeLength(falseByteCodes));
		compileAndFreeByteCodes(falseByteCodes);
	} else {
		COMPILENODE(arg2, &dummy);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmOr);
	}
}

void compileIfMsg(PyrCallNode* node)
{
	int numArgs;
	long dummy;
	PyrParseNode *arg1, *arg2, *arg3;
	ByteCodes trueByteCodes, falseByteCodes;
		
	numArgs = nodeListLength(node->arglist);
	arg1 = node->arglist;
	
	if (numArgs < 2) {
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmIf);
	} else if (numArgs == 2) {
		arg2 = arg1->next;

		if (isAnInlineableBlock(arg2)) {
			COMPILENODE(arg1, &dummy);
			
			trueByteCodes = compileSubExpression((PyrPushLitNode*)arg2);
			if (byteCodeLength(trueByteCodes)) {
				compileJump(opcJumpIfFalsePushNil, byteCodeLength(trueByteCodes));
				compileAndFreeByteCodes(trueByteCodes);
			} else {
				compileOpcode(opSpecialOpcode, opcDrop); // drop the boolean
				compileOpcode(opPushSpecialValue, opsvNil); // push nil
			}
		} else {
			COMPILENODE(arg1, &dummy);
			COMPILENODE(arg2, &dummy);
			compileOpcode(opSendSpecialMsg, numArgs);
			compileByte(opmIf);
		}
	} else if (numArgs == 3) {
		arg2 = arg1->next;
		arg3 = arg2->next;
		if (isAnInlineableBlock(arg2) && isAnInlineableBlock(arg3)) {
			COMPILENODE(arg1, &dummy);
			falseByteCodes = compileSubExpression((PyrPushLitNode*)arg3);
			trueByteCodes = compileSubExpressionWithGoto((PyrPushLitNode*)arg2, byteCodeLength(falseByteCodes));
			if (byteCodeLength(falseByteCodes)) {
				compileJump(opcJumpIfFalse, byteCodeLength(trueByteCodes));
				compileAndFreeByteCodes(trueByteCodes);
				compileAndFreeByteCodes(falseByteCodes);
			} else if (byteCodeLength(trueByteCodes)) {
				compileJump(opcJumpIfFalsePushNil, byteCodeLength(trueByteCodes));
				compileAndFreeByteCodes(trueByteCodes);
			} else {
				compileOpcode(opSpecialOpcode, opcDrop); // drop the boolean
				compileOpcode(opPushSpecialValue, opsvNil); // push nil
			}
		} else {
			COMPILENODE(arg1, &dummy);
			COMPILENODE(arg2, &dummy);
			COMPILENODE(arg3, &dummy);
			compileOpcode(opSendSpecialMsg, numArgs);
			compileByte(opmIf);
		}
	} else {
		//arg1 = node->arglist;
		for (; arg1; arg1 = arg1->next) {
			COMPILENODE(arg1, &dummy);
		}
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmIf);
	}
}

void compileWhileMsg(PyrCallNode* node)
{
	int numArgs;
	PyrParseNode *argnode;
	long dummy;
	ByteCodes whileByteCodes, exprByteCodes;
	int whileByteCodeLen, exprByteCodeLen;
	
	numArgs = nodeListLength(node->arglist);
	if (numArgs == 1 && isAnInlineableBlock(node->arglist)) {
		
		whileByteCodes = compileSubExpression((PyrPushLitNode*)node->arglist);
		
		whileByteCodeLen = byteCodeLength(whileByteCodes);
		compileAndFreeByteCodes(whileByteCodes);
		
		exprByteCodeLen = 1;
		compileJump(opcJumpIfFalsePushNil, exprByteCodeLen + 3);
		
		// opcJumpBak does a drop..
		compileOpcode(opPushSpecialValue, opsvNil);
			
		compileJump(opcJumpBak, exprByteCodeLen + whileByteCodeLen + 4);

	} else if (numArgs == 2 && isWhileTrue(node->arglist) 
		&& isAnInlineableBlock(node->arglist->next)) {
		
		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->arglist->next);
		
		exprByteCodeLen = byteCodeLength(exprByteCodes);
		compileAndFreeByteCodes(exprByteCodes);
		
		compileJump(opcJumpBak, exprByteCodeLen + 1);
				
	} else if (numArgs == 2 && isAnInlineableBlock(node->arglist) 
		&& isAnInlineableBlock(node->arglist->next)) {
		
		whileByteCodes = compileSubExpression((PyrPushLitNode*)node->arglist);
		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->arglist->next);
		
		whileByteCodeLen = byteCodeLength(whileByteCodes);
		compileAndFreeByteCodes(whileByteCodes);
		
		exprByteCodeLen = byteCodeLength(exprByteCodes);
		compileJump(opcJumpIfFalsePushNil, exprByteCodeLen + 3);
		
		compileAndFreeByteCodes(exprByteCodes);
		
		compileJump(opcJumpBak, exprByteCodeLen + whileByteCodeLen + 4);
		
	} else {
		argnode = node->arglist;
		for (; argnode; argnode = argnode->next) {
			COMPILENODE(argnode, &dummy);
		}
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmWhile);
	}
}

void compileLoopMsg(PyrCallNode* node)
{
	int numArgs;
	PyrParseNode *argnode;
	long dummy;
	ByteCodes exprByteCodes;
	int exprByteCodeLen;
	
	numArgs = nodeListLength(node->arglist);
	if (numArgs == 1 && isAnInlineableBlock(node->arglist)) {
		
		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->arglist);
		
		exprByteCodeLen = byteCodeLength(exprByteCodes);
		compileAndFreeByteCodes(exprByteCodes);
		
		compileJump(opcJumpBak, exprByteCodeLen + 1);

	} else {
		argnode = node->arglist;
		for (; argnode; argnode = argnode->next) {
			COMPILENODE(argnode, &dummy);
		}
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmLoop);
	}
}

PyrBinopCallNode* newPyrBinopCallNode(PyrSlotNode* selector,
	PyrParseNode* arg1, PyrParseNode* arg2)
{
	PyrBinopCallNode* node;
	node = ALLOCNODE(PyrBinopCallNode);
	node->classno = pn_BinopCallNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->selector = selector;
	node->arg1 = arg1;
	node->arg2 = arg2;
	return node;
}

void compilePyrBinopCallNode(PyrBinopCallNode* node, void *result)
{
	int index, selType, isSuper;
	long dummy;
	
	//postfl("compilePyrBinopCallNode\n");
	isSuper = isSuperObjNode(node->arg1);
	node->selector->slot.us->flags |= sym_Called;
	index = conjureSelectorIndex((PyrParseNode*)node->selector, gCompilingBlock, 
		isSuper, node->selector->slot.us, &selType);
	if (isSuper) {
		COMPILENODE(node->arg1, &dummy);
		COMPILENODE(node->arg2, &dummy);
		compileOpcode(opSendSuper, 2);
		compileByte(index);
	} else {
		switch (selType) {
			case selNormal :
				COMPILENODE(node->arg1, &dummy);
				COMPILENODE(node->arg2, &dummy);
				compileOpcode(opSendMsg, 2);
				compileByte(index);
				break;
			case selSpecial :
				COMPILENODE(node->arg1, &dummy);
				COMPILENODE(node->arg2, &dummy);
				compileOpcode(opSendSpecialMsg, 2);
				compileByte(index);
				break;
			case selBinary :
				if (index == opAdd && node->arg2->classno == pn_PushLitNode
					&& ((PyrPushLitNode*)node->arg2)->literalSlot.utag == tagInt
					&& ((PyrPushLitNode*)node->arg2)->literalSlot.ui == 1) {
					COMPILENODE(node->arg1, &dummy);
					compileOpcode(opPushSpecialValue, opsvPlusOne);
				} else if (index == opSub && node->arg2->classno == pn_PushLitNode
					&& ((PyrPushLitNode*)node->arg2)->literalSlot.utag == tagInt
					&& ((PyrPushLitNode*)node->arg2)->literalSlot.ui == 1) {
					COMPILENODE(node->arg1, &dummy);
					compileOpcode(opPushSpecialValue, opsvMinusOne);
				} else {
					COMPILENODE(node->arg1, &dummy);
					COMPILENODE(node->arg2, &dummy);
					compileOpcode(opSendSpecialBinaryArithMsg, index);
				}
				break;
			case selAnd :
				compileAndMsg(node->arg1, node->arg2);
				break;
			case selOr :
				compileOrMsg(node->arg1, node->arg2);
				break;
			default :
				COMPILENODE(node->arg1, &dummy);
				COMPILENODE(node->arg2, &dummy);
				compileOpcode(opSendMsg, 2);
				compileByte(index);
				break;
		}
	}
}

PyrPushKeyArgNode* newPyrPushKeyArgNode(PyrSlotNode* selector, PyrParseNode* expr)
{
	PyrPushKeyArgNode* node;
	node = ALLOCNODE(PyrPushKeyArgNode);
	node->classno = pn_PushKeyArgNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->selector = selector;
	node->expr = expr;
	return node;
}

void compilePyrPushKeyArgNode(PyrPushKeyArgNode* node, void *result)
{
	long dummy, index;
	//postfl("->compilePyrPushKeyArgNode\n");
	
	index = conjureLiteralSlotIndex((PyrParseNode*)node, gCompilingBlock, &node->selector->slot);
	
	compileOpcode(opPushLiteral, index);
	
	COMPILENODE(node->expr, &dummy);
}

PyrDropNode* newPyrDropNode(PyrParseNode* expr1, PyrParseNode* expr2)
{
	PyrDropNode* node;
	node = ALLOCNODE(PyrDropNode);
	node->classno = pn_DropNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->expr1 = expr1;
	node->expr2 = expr2;
	return node;
}

void compilePyrDropNode(PyrDropNode* node, void *result)
{
	//postfl("->compilePyrDropNode\n");
	long dummy;
	// eliminate as many drops as possible
	if (node->expr2 && node->expr2->classno == pn_BlockReturnNode) {
		// no drop before a block return
		COMPILENODE(node->expr1, &dummy);
		COMPILENODE(node->expr2, &dummy);
	} else if (node->expr1 && node->expr1->classno == pn_AssignNode) {
		// let the store do the drop
		((PyrAssignNode*)node->expr1)->drop = 1;
		COMPILENODE(node->expr1, &dummy);
		COMPILENODE(node->expr2, &dummy);
	} else if (node->expr1 && node->expr1->classno == pn_DropNode) {
		PyrDropNode *znode;
		// let the store do the drop, a bit more complex.
		// find the ultimate expression in the left subtree before the drop.
		znode = (PyrDropNode*)node->expr1;
		while (znode->expr2 && znode->expr2->classno == pn_DropNode) {
			znode = (PyrDropNode*)znode->expr2;
		}
		if (znode->expr2->classno == pn_AssignNode) {
			((PyrAssignNode*)znode->expr2)->drop = 1;
			COMPILENODE(node->expr1, &dummy);
			COMPILENODE(node->expr2, &dummy);
		} else {
			COMPILENODE(node->expr1, &dummy);
			compileOpcode(opSpecialOpcode, opcDrop);
			COMPILENODE(node->expr2, &dummy);
		}
	} else {
		COMPILENODE(node->expr1, &dummy);
		compileOpcode(opSpecialOpcode, opcDrop);
		COMPILENODE(node->expr2, &dummy);
	}
	//postfl("<-compilePyrDropNode\n");
}

PyrPushLitNode* newPyrPushLitNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj)
{
	PyrPushLitNode* node;
	if (literalSlot) {
		node = (PyrPushLitNode*)literalSlot;
		node->classno = pn_PushLitNode;
	} else {
		node = ALLOCNODE(PyrPushLitNode);
		node->classno = pn_PushLitNode;
		node->next = 0;
		node->tail = (PyrParseNode*)node;
		node->charno = charno;
		node->lineno = lineno;
		node->literalSlot.utag = tagPtr;
		node->literalSlot.uo = (PyrObject*)literalObj;
	}
	return node;
}

void compilePushInt(int value)
{
	//postfl("compilePushInt\n");
	if (value >= -1 && value <= 2) {
		compileOpcode(opPushSpecialValue, opsvZero + value);
	} else if (value >= 0) {
		compileOpcode(opSpecialOpcode, opcPushPosInt);
		compileNumber( value);
	} else {
		compileOpcode(opSpecialOpcode, opcPushNegInt);
		compileNumber(-value);
	}
}

void compilePyrPushLitNode(PyrPushLitNode* node, void *result)
{
	int index;
	PyrSlot slot;
	ByteCodes savedBytes;
	
	//postfl("compilePyrPushLitNode\n");
	if (node->literalSlot.utag == tagPtr) {
		PyrParseNode *literalObj;
		literalObj = (PyrParseNode*)node->literalSlot.uo;
		//index = conjureLiteralObjIndex(gCompilingBlock, node->literalObj);
		if (literalObj->classno == pn_BlockNode) {
			METHRAW(gCompilingBlock)->needsHeapContext = 1;
			savedBytes = saveByteCodeArray();
			COMPILENODE(literalObj, &slot);
			restoreByteCodeArray(savedBytes);
			index = conjureLiteralSlotIndex(literalObj, gCompilingBlock, &slot);
			compileOpcode(opExtended, opPushLiteral);
			compileByte(index);
		} else {
			COMPILENODE(literalObj, &slot);
			index = conjureLiteralSlotIndex(literalObj, gCompilingBlock, &slot);
			compileOpcode(opPushLiteral, index);
		}
	} else {
		slot = node->literalSlot;
		if (slot.utag == tagInt) {
			compilePushInt(slot.ui);
		} else if (slot.utag == tagSym || slot.utag == tagChar 
			|| slot.utag == tagObj) {
			index = conjureLiteralSlotIndex((PyrParseNode*)node, gCompilingBlock, &slot);
			compileOpcode(opPushLiteral, index);
		} else if (SlotEq(&slot, &o_nil)) {
			compileOpcode(opPushSpecialValue, opsvNil);
		} else if (SlotEq(&slot, &o_true)) {
			compileOpcode(opPushSpecialValue, opsvTrue);
		} else if (SlotEq(&slot, &o_false)) {
			compileOpcode(opPushSpecialValue, opsvFalse);
		} else if (SlotEq(&slot, &o_fhalf)) {
			//post("opsvFHalf %s-%s\n", gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
			compileOpcode(opPushSpecialValue, opsvFHalf);
		} else if (SlotEq(&slot, &o_fnegone)) {
			//post("opsvFNegOne %s-%s\n", gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
			compileOpcode(opPushSpecialValue, opsvFNegOne);
		} else if (SlotEq(&slot, &o_fzero)) {
			compileOpcode(opPushSpecialValue, opsvFZero);
		} else if (SlotEq(&slot, &o_fone)) {
			compileOpcode(opPushSpecialValue, opsvFOne);
		} else if (SlotEq(&slot, &o_ftwo)) {
			//post("opsvFTwo %s-%s\n", gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
			compileOpcode(opPushSpecialValue, opsvFTwo);
		//} else if (SlotEq(&slot, &o_twopi)) {
		//	compileOpcode(opPushSpecialValue, opsv2PI);
		} else {
			index = conjureLiteralSlotIndex((PyrParseNode*)node, gCompilingBlock, &slot);
			compileOpcode(opPushLiteral, index);
		}
	}
}

PyrLiteralNode* newPyrLiteralNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj)
{
	PyrLiteralNode* node;
	if (literalSlot) {
		node = (PyrLiteralNode*)literalSlot;
		node->classno = pn_LiteralNode;
	} else {
		node = ALLOCNODE(PyrLiteralNode);
		node->classno = pn_LiteralNode;
		node->next = 0;
		node->tail = (PyrParseNode*)node;
		node->charno = charno;
		node->lineno = lineno;
		node->literalSlot.utag = tagPtr;
		node->literalSlot.uo = (PyrObject*)literalObj;
	}
	return node;
}


void compilePyrLiteralNode(PyrLiteralNode* node, void *result)
{
	ByteCodes savedBytes;
	
	if (!node) {	
		*(PyrSlot*)result = o_nil;
	} else if (node->literalSlot.utag == tagPtr) {
		PyrParseNode* literalObj;
		literalObj = (PyrParseNode*)node->literalSlot.uo;
		if (literalObj->classno == pn_BlockNode) {
			METHRAW(gCompilingBlock)->needsHeapContext = 1;
			savedBytes = saveByteCodeArray();
			COMPILENODE(literalObj, result);
			restoreByteCodeArray(savedBytes);
		} else {
			COMPILENODE(literalObj, result);
		}
	} else {
		*(PyrSlot*)result = node->literalSlot;
	}
}

PyrReturnNode* newPyrReturnNode(PyrParseNode* expr)
{
	PyrReturnNode* node;
	node = ALLOCNODE(PyrReturnNode);
	node->classno = pn_ReturnNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->expr = expr;
	return node;
}


	
void compilePyrReturnNode(PyrReturnNode* node, void *result)
{
	PyrPushLitNode *lit;
	long dummy;
	
	//postfl("->compilePyrReturnNode\n");
	if (!node->expr) {
		compileOpcode(opSpecialOpcode, opcReturnSelf);
	} else if (node->expr->classno == pn_PushLitNode) {
		lit = (PyrPushLitNode*)node->expr;
		if (lit->literalSlot.us == s_this && lit->literalSlot.utag == tagSym) {
			compileOpcode(opSpecialOpcode, opcReturnSelf);
		} else if (lit->literalSlot.utag == tagNil) {
			compileOpcode(opSpecialOpcode, opcReturnNil);
		} else if (lit->literalSlot.utag == tagTrue) {
			compileOpcode(opSpecialOpcode, opcReturnTrue);
		} else if (lit->literalSlot.utag == tagFalse) {
			compileOpcode(opSpecialOpcode, opcReturnFalse);
		} else {
			COMPILENODE(lit, &dummy);
			compileOpcode(opSpecialOpcode, opcReturn);
		}
	} else {
		COMPILENODE(node->expr, &dummy);
		compileOpcode(opSpecialOpcode, opcReturn);
	}
	//postfl("<-compilePyrReturnNode\n");
}

PyrBlockReturnNode* newPyrBlockReturnNode()
{
	PyrBlockReturnNode* node;
	node = ALLOCNODE(PyrBlockReturnNode);
	node->classno = pn_BlockReturnNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	return node;
}


void compilePyrBlockReturnNode(PyrBlockReturnNode* node, void *result)
{
	//postfl("compilePyrBlockReturnNode\n");
	//compileOpcode(opSpecialOpcode, opcFunctionReturn);
}

PyrAssignNode* newPyrAssignNode(PyrSlotNode* varName, PyrParseNode* expr, int flags)
{
	PyrAssignNode* node;
	node = ALLOCNODE(PyrAssignNode);
	node->classno = pn_AssignNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varName = varName;
	node->expr = expr;
	node->drop = 0;
	return node;
}

PyrSetterNode* newPyrSetterNode(PyrSlotNode* selector, PyrParseNode* expr1, PyrParseNode* expr2)
{
	PyrSetterNode* node;
	node = ALLOCNODE(PyrSetterNode);
	node->classno = pn_SetterNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->selector = selector;
	node->expr1 = expr1;
	node->expr2 = expr2;
	return node;
}

PyrMultiAssignNode* newPyrMultiAssignNode(PyrMultiAssignVarListNode* varList, 
	PyrParseNode* expr, int flags)
{
	PyrMultiAssignNode* node;
	node = ALLOCNODE(PyrMultiAssignNode);
	node->classno = pn_MultiAssignNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varList = varList;
	node->expr = expr;
	node->drop = 0;
	return node;
}

PyrMultiAssignVarListNode* newPyrMultiAssignVarListNode(PyrSlotNode* varNames, 
	PyrSlotNode* rest)
{
	PyrMultiAssignVarListNode* node;
	node = ALLOCNODE(PyrMultiAssignVarListNode);
	node->classno = pn_MultiAssignVarListNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->varNames = varNames;
	node->rest = rest;
	return node;
}

void compileAssignVar(PyrParseNode* node, PyrSymbol* varName, bool drop)
{
	int level, index, vindex, varType;
	PyrBlock *tempfunc;
	PyrClass *classobj;
	
	//postfl("compileAssignVar\n");
	classobj = gCompilingClass;
	if (varName->name[0] >= 'A' && varName->name[0] <= 'Z') {
		// actually this shouldn't even parse, so you won't get here.
		error("You may not assign to a class name.");
		nodePostErrorLine(node);
		compileErrors++;
	} else if (findVarName(gCompilingBlock, &classobj, varName, 
		&varType, &level, &index, &tempfunc)) {
		switch (varType) {
			case varInst :
				if (drop) {
					if (index <= 15) {
						compileByte((opStoreInstVar<<4) | index);
					} else {
						compileByte(opStoreInstVar);
						compileByte(index);
						compileByte((opSpecialOpcode<<4) | opcDrop);
					}
				} else {
					compileByte(opStoreInstVar);
					compileByte(index);
				}
				break;
			case varClass : {
				PyrSlot slot;
				slot.ucopy = classobj->name.ucopy;
				vindex = conjureLiteralSlotIndex(node, gCompilingBlock, &slot);
				//compileOpcode(opStoreClassVar, vindex);
				//compileByte(index);
				if (drop) {
					if (vindex <= 15) {
						compileByte((opStoreClassVar<<4) | vindex);
						compileByte(index);
					} else {
						compileByte(opStoreClassVar);
						compileByte(vindex);
						compileByte(index);
						compileByte((opSpecialOpcode<<4) | opcDrop);
					}
				} else {
					compileByte(opStoreClassVar);
					compileByte(vindex);
					compileByte(index);
				}
			} break;
			case varTemp :
				index = METHRAW(tempfunc)->numtemps - index - 1;
				//compileOpcode(opStoreTempVar, level);
				//compileByte(index);
				if (drop) {
					if (index <= 15 && level < 8) {
						compileByte((opStoreTempVar<<4) | level);
						compileByte(index);
					} else {
						compileByte(opStoreTempVar);
						compileByte(level);
						compileByte(index);
						compileByte((opSpecialOpcode<<4) | opcDrop);
					}
				} else {
					compileByte(opStoreTempVar);
					compileByte(level);
					compileByte(index);
				}
				break;
		}
	} else {
		error("Variable '%s' not defined.\n", varName->name);
		nodePostErrorLine(node);
		compileErrors++;
		//Debugger();
	}
}

void compilePyrAssignNode(PyrAssignNode* node, void* result)
{
	long dummy;
	
	//postfl("compilePyrAssignNode\n");
	COMPILENODE(node->expr, &dummy);
	compileAssignVar((PyrParseNode*)node, node->varName->slot.us, node->drop);	
}

void compilePyrSetterNode(PyrSetterNode* node, void* result)
{
	int index, selType, isSuper;
	long dummy;
	char setterName[128];
	PyrSymbol *setterSym;
	
	//postfl("compilePyrSetterNode\n");
	if (nodeListLength(node->expr1) > 1) {
		error("Setter method called with too many arguments.\n");
		nodePostErrorLine(node->expr1);
		compileErrors++;
	} else {
		COMPILENODE(node->expr1, &dummy);
		COMPILENODE(node->expr2, &dummy);

		
		//postfl("compilePyrCallNode\n");
		isSuper = isSuperObjNode(node->expr1);
		
		sprintf(setterName, "%s_", node->selector->slot.us->name);
		setterSym = getsym(setterName);
		
		node->selector->slot.us->flags |= sym_Called;
		index = conjureSelectorIndex((PyrParseNode*)node->selector, gCompilingBlock, 
			isSuper, setterSym, &selType);
		if (isSuper) {
			compileOpcode(opSendSuper, 2);
			compileByte(index);
		} else {
			compileOpcode(opSendMsg, 2);
			compileByte(index);
		}
	}
}

void compilePyrMultiAssignNode(PyrMultiAssignNode* node, void* result)
{
	long dummy;
	
	//postfl("compilePyrMultiAssignNode\n");
	COMPILENODE(node->expr, &dummy);
	COMPILENODE(node->varList, &dummy);
}

void compilePyrMultiAssignVarListNode(PyrMultiAssignVarListNode* node, void* result)
{
	int i, numAssigns;
	PyrSlotNode *varname;
	
	//postfl("compilePyrMultiAssignVarListNode\n");
	numAssigns = nodeListLength((PyrParseNode*)node->varNames);
	varname = node->varNames;
	for (i=0; i<numAssigns; ++i, varname = (PyrSlotNode*)varname->next) {
		compileOpcode(opSpecialOpcode, opcDup);
		compilePushInt(i);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAt);
		compileAssignVar((PyrParseNode*)varname, varname->slot.us, 1);
		//compileOpcode(opSpecialOpcode, opcDrop);
	}
	if (node->rest) {
		compileOpcode(opSpecialOpcode, opcDup);
		compilePushInt(i);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmCopyToEnd);
		compileAssignVar((PyrParseNode*)node->rest, node->rest->slot.us, 1);
		//compileOpcode(opSpecialOpcode, opcDrop);
	}
}


PyrDynListNode* newPyrDynListNode(PyrParseNode *classname, PyrParseNode *elems)
{
	PyrDynListNode* node;
	
	//if (compilingCmdLine) post("newPyrDynListNode\n");
	node = ALLOCNODE(PyrDynListNode);
	node->classno = pn_DynListNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->classname = classname;
	node->elems = elems;
	return node;
}

void compilePyrDynListNode(PyrDynListNode* node, void* result)
{
	int i, numItems;
	PyrParseNode *inode;
	long dummy;
	
	//postfl("compilePyrDynListNode\n");
	numItems = nodeListLength(node->elems);

	if (node->classname) {
		compilePushVar((PyrParseNode*)node, ((PyrSlotNode*)node->classname)->slot.us);
	} else {
		compilePushVar((PyrParseNode*)node, s_array);
	}

	//compileOpcode(opExtended, opPushSpecialValue);
	//compileByte(op_class_list);

	compileOpcode(opSpecialOpcode, opcPushPosInt);
	compileNumber(numItems);
	
	compileOpcode(opSendSpecialMsg, 2);
	compileByte(opmNew);
	
	inode = node->elems;
	for (i=0; i<numItems; ++i, inode = (PyrParseNode*)inode->next) {
		//if (compilingCmdLine) post("+ %d %d\n", i, gCompilingByteCodes->size);
		COMPILENODE(inode, &dummy);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAdd);
	}
}

PyrLitListNode* newPyrLitListNode(PyrParseNode *classname, PyrParseNode *elems)
{
	PyrLitListNode* node;
	node = ALLOCNODE(PyrLitListNode);
	node->classno = pn_LitListNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->classname = classname;
	node->elems = elems;
	return node;
}

void compilePyrLitListNode(PyrLitListNode* node, void* result)
{
	PyrSlot *resultSlot;
	PyrSlot itemSlot;
	PyrObject *array;
	PyrParseNode *inode;
	int i, numItems, flags;
	
	//postfl("->compilePyrLitListNode\n");
	if (node->classname && ((PyrSlotNode*)node->classname)->slot.us != s_array) {
		error("Only Array is supported as literal type.\n");
		post("Compiling as an Array.\n");
	}
	resultSlot = (PyrSlot*)result;
	numItems = node->elems ? nodeListLength(node->elems) : 0;
	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	array = newPyrArray(compileGC(), numItems, flags, false);
	inode = node->elems;
	for (i=0; i<numItems; ++i, inode = (PyrParseNode*)inode->next) {
		COMPILENODE(inode, &itemSlot);
		array->slots[i] = itemSlot;
	}
	array->size = numItems;
	SetObject(resultSlot, array);
	//postfl("<-compilePyrLitListNode\n");
}


PyrBlockNode* newPyrBlockNode(PyrArgListNode *arglist, PyrVarListNode *varlist, PyrParseNode *body, bool isTopLevel)
{
	PyrBlockNode* node;
	node = ALLOCNODE(PyrBlockNode);
	node->classno = pn_BlockNode;
	node->next = 0;
	node->tail = (PyrParseNode*)node;
	node->charno = charno;
	node->lineno = lineno;
	node->arglist = arglist;
	catVarLists(varlist);
	node->varlist = varlist;
	node->body = body;
	node->isTopLevel = isTopLevel;
	return node;
}

void compilePyrBlockNode(PyrBlockNode* node, void* result)
{
	PyrBlock *block, *prevBlock;
	PyrMethodRaw *methraw;
	int i, j, numArgs, numVars, funcVarArgs;
	int numSlots, numArgNames, flags;
	PyrVarDefNode *vardef;
	PyrObject *proto;
	PyrSymbolArray *argNames, *varNames;
	PyrSlot *slotResult;
	long dummy;
	
	//postfl("->block\n");
	slotResult = (PyrSlot*)result;
	
	// create a new block object

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	block = newPyrBlock(flags);
	SetObject(slotResult, block);
	
	prevBlock = gCompilingBlock;
	gCompilingBlock = block;

	methraw = METHRAW(block);
	methraw->unused1 = 0;
	methraw->unused2 = 0;

#if USESTACKFRAMES
	methraw->needsHeapContext = 0;
#else
	methraw->needsHeapContext = 1;
#endif
	
	if (node->isTopLevel) {
		SetNil(&block->context);
	} else {
		SetObject(&block->context, prevBlock);
	}

	methraw->varargs = funcVarArgs = (node->arglist && node->arglist->rest) ? 1 : 0;
	numArgs = node->arglist ? nodeListLength((PyrParseNode*)node->arglist->varDefs) : 0;
	numVars = node->varlist ? nodeListLength((PyrParseNode*)node->varlist->varDefs) : 0;

	numSlots = numArgs + funcVarArgs + numVars;
	methraw->frameSize = (numSlots + FRAMESIZE) * sizeof(PyrSlot);
	if (numSlots) {
		proto = newPyrArray(compileGC(), numSlots, flags, false);
		proto->size = numSlots;
		SetObject(&block->prototypeFrame, proto);
	} else {
		SetNil(&block->prototypeFrame);
	}

	numArgNames = numArgs + funcVarArgs;
	
	if (numArgNames) {
		argNames = newPyrSymbolArray(compileGC(), numArgNames, flags, false);
		argNames->size = numArgNames;
		SetObject(&block->argNames, argNames);
	} else {
		SetNil(&block->argNames);
	}
	
	if (numVars) {
		varNames = newPyrSymbolArray(compileGC(), numVars, flags, false);
		varNames->size = numVars;
		SetObject(&block->varNames, varNames);
	} else {
		SetNil(&block->varNames);
	}
	
	methraw->numargs = numArgs;
	methraw->numvars = numVars;
	methraw->posargs = numArgs + funcVarArgs;
	methraw->numtemps = numSlots;
	methraw->popSize = numSlots;

	// declare args
	if (numArgs) {
		PyrSymbol **blockargs;
		blockargs = block->argNames.uosym->symbols;
		vardef = node->arglist->varDefs;
		for (i=0; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *varslot;
			varslot = &vardef->varName->slot;
			// already declared as arg?
			for (j=0; j<i; ++j) {
				if (blockargs[j] == varslot->us) {
					error("Function argument '%s' already declared in %s-%s\n",
						varslot->us->name, 
						gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in arglist
			blockargs[i] = varslot->us;
			//postfl("defarg %d '%s'\n", i, slot->us->name);
		}
	}
	
	if (funcVarArgs) {
		PyrSlot *varslot;
		PyrSymbol **blockargs;
		blockargs = block->argNames.uosym->symbols;
		varslot = &node->arglist->rest->slot;
		// already declared as arg?
		for (j=0; j<numArgs; ++j) {
			if (blockargs[j] == varslot->us) {
				error("Function argument '%s' already declared in %s-%s\n",
					varslot->us->name, 
					gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
				nodePostErrorLine((PyrParseNode*)vardef);
				compileErrors++;
			}
		}
		// put it in arglist
		blockargs[numArgs] = varslot->us;
		//postfl("defrest '%s'\n", slot->us->name);
	}
	
	// declare vars
	if (numVars) {
		PyrSymbol **blockargs, **blockvars;
		blockargs = block->argNames.uosym->symbols;
		blockvars = block->varNames.uosym->symbols;
		vardef = node->varlist->varDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *varslot;
			varslot = &vardef->varName->slot;
			// already declared as arg?
			for (j=0; j<numArgNames; ++j) {
				if (blockargs[j] == varslot->us) {
					error("Function variable '%s' already declared in %s-%s\n",
						varslot->us->name, 
						gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// already declared as var?
			for (j=0; j<i; ++j) {
				if (blockvars[j] == varslot->us) {
					error("Function variable '%s' already declared in %s-%s\n",
						varslot->us->name, 
						gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in varlist
			blockvars[i] = varslot->us;
			//postfl("defvar %d '%s'\n", i, slot->us->name);
		}
	}
	
	if (numArgs) {
		vardef = node->arglist->varDefs;
		for (i=0; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *slot, litval;
			slot = block->prototypeFrame.uo->slots + i;
			compilePyrLiteralNode(vardef->defVal, &litval);
			*slot = litval;
		}
	}
	
	if (funcVarArgs) {
		//SetNil(&block->prototypeFrame.uo->slots[numArgs]);
		block->prototypeFrame.uo->slots[numArgs].ucopy = o_emptyarray.ucopy;
	}
	
	if (numVars) {
		vardef = node->varlist->varDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->next) {
			PyrSlot *slot, litval;
			slot = block->prototypeFrame.uo->slots + i + numArgs + funcVarArgs;
			compilePyrLiteralNode(vardef->defVal, &litval);
			*slot = litval;
		}
	}
	methraw->methType = methBlock;
	
	// compile body

	initByteCodes();
	if (node->body->classno == pn_BlockReturnNode) {
		compileOpcode(opPushSpecialValue, opsvNil);
	} else {
		COMPILENODE(node->body, &dummy);
	}
	compileOpcode(opSpecialOpcode, opcFunctionReturn);
	installByteCodes(block);

	//postfl("literals %d\n", 
	//	block->literals.uo ? block->literals.uo->size : 0);

	gCompilingBlock = prevBlock;
	//postfl("<-block\n");

}



PyrParseNode* linkNextNode(PyrParseNode* a, PyrParseNode* b)
{		
	if (a == NULL) return b;
	if (b) {
		a->tail->next = b;
		a->tail = b->tail;
	}
	return a;
}

bool isSuperObjNode(PyrParseNode *node)
{
	return node->classno == pn_PushNameNode 
		&& ((PyrPushNameNode*)node)->varName.us == s_super;
}

bool isThisObjNode(PyrParseNode *node)
{
	return node->classno == pn_PushNameNode 
		&& ((PyrPushNameNode*)node)->varName.us == s_this;
}

int nodeListLength(PyrParseNode *node)
{
	int length = 0;
	for (; node; node=node->next) length++;
	return length;
}


int conjureSelectorIndex(PyrParseNode *node, PyrBlock* func, 
	bool isSuper, PyrSymbol *selector, int *selType)
{
	int i;
	PyrObject *literals;
	PyrSlot *slot;
	int newsize, flags;

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	if (!isSuper) {
		if (selector == gSpecialSelectors[opmIf]) {
			*selType = selIf;
			return opmIf;
		} else if (selector == gSpecialSelectors[opmWhile]) {
			*selType = selWhile;
			return opmWhile;
		} else if (selector == gSpecialSelectors[opmAnd]) {
			*selType = selAnd;
			return opmAnd;
		} else if (selector == gSpecialSelectors[opmOr]) {
			*selType = selOr;
			return opmOr;
		} else if (selector == gSpecialSelectors[opmLoop]) {
			*selType = selLoop;
			return opmLoop;
		}
		
		for (i=0; i<opmNumSpecialSelectors; ++i) {
			if (selector == gSpecialSelectors[i]) {
				*selType = selSpecial;
				return i;
			}
		}
		
		for (i=0; i<opNumUnarySelectors; ++i) {
			if (selector == gSpecialUnarySelectors[i]) {
				*selType = selUnary;
				return i;
			}
		}
		
		for (i=0; i<opNumBinarySelectors; ++i) {
			if (selector == gSpecialBinarySelectors[i]) {
				*selType = selBinary;
				return i;
			}
		}
	}
	
	literals = func->literals.uo;
	if (literals) {
		for (i=0; i<literals->size; ++i) {
			if (literals->slots[i].utag == tagSym && literals->slots[i].us == selector) {
				*selType = selNormal;
				return i;
			}
		}
	} else {
		func->literals.utag = tagObj;
		literals = func->literals.uo = 
			(PyrObject*)newPyrArray(compileGC(), 2, flags, false);
	}
	// otherwise add it to the literals table
	
	if (literals->size+1 >= 256) { 
		error("Literal table too big. Simplify the function.\n");
		post("Next selector was: %s\n", selector->name);
		nodePostErrorLine(node);
		compileErrors++;
		return 0;
	}

	if (literals->size+1 > ARRAYMAXINDEXSIZE(literals)) { 
		// resize literal table
		newsize = ARRAYMAXINDEXSIZE(literals) * 2;
		func->literals.uo = 
			(PyrObject*)newPyrArray(compileGC(), newsize, flags, false);
		memcpy(func->literals.uo->slots, literals->slots, literals->size * sizeof(PyrSlot));
		func->literals.uo->size = literals->size;
		freePyrObject(literals);
		literals = func->literals.uo;
	}
	slot = literals->slots + literals->size++;
	SetSymbol(slot, selector);
		
	*selType = selNormal;
	return literals->size-1;
}

int conjureLiteralSlotIndex(PyrParseNode *node, PyrBlock* func, PyrSlot *slot)
{
	int i;
	PyrObject *literals;
	PyrSlot *slot2;
	int newsize, flags;

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	// lookup slot in literals table
	literals = func->literals.uo;
	/*if (literals && literals->classptr != class_array) {
		post("compiling %s-%s\n", gCompilingClass->name.us->name, gCompilingMethod->name.us->name);
		post("literals is a '%s'\n", literals->classptr->name.us->name);
		dumpObjectSlot(slot);
		Debugger();
	}*/
	if (literals) {
		for (i=0; i<literals->size; ++i) {
			if (literals->slots[i].utag == slot->utag 
				&& literals->slots[i].ui == slot->ui) {
				return i;
			}
		}
	} else {
		func->literals.utag = tagObj;
		literals = func->literals.uo = 
			(PyrObject*)newPyrArray(compileGC(), 4, flags, false);
	}
	// otherwise add it to the literals table
	
	if (literals->size+1 >= 256) { 
		error("Literal table too big. Simplify the function.\n");
		post("Next literal was:\n");
		dumpPyrSlot(slot); 
		nodePostErrorLine(node);
		compileErrors++;
		return 0;
	}
	if (literals->size+1 > ARRAYMAXINDEXSIZE(literals)) { 
		// resize literal table
		newsize = ARRAYMAXINDEXSIZE(literals) * 2;
		// resize literal table
		func->literals.uo = 
			(PyrObject*)newPyrArray(compileGC(), newsize, flags, false);
		memcpy(func->literals.uo->slots, literals->slots, literals->size * sizeof(PyrSlot));
		func->literals.uo->size = literals->size;
		freePyrObject(literals);  
		literals = func->literals.uo;
	}
	slot2 = literals->slots + literals->size++;
	*(double*)slot2 = *(double*)slot;
		
	return literals->size-1;
}

bool findVarName(PyrBlock* func, PyrClass **classobj, PyrSymbol *name, 
	int *varType, int *level, int *index, PyrBlock** tempfunc)
{
	int i, j, k;
	int numargs;
	PyrSymbol *argname, *varname;
	PyrMethodRaw *methraw;
	
	//postfl("->findVarName %s\n", name->name);
	// find var in enclosing blocks, instance, class
	if (name == s_super) name = s_this;
	if (name->name[0] >= 'A' && name->name[0] <= 'Z') return false;
	for (j=0; func; func = func->context.uoblk, ++j) {
		methraw = METHRAW(func);
		numargs = methraw->posargs;
		for (i=0; i<numargs; ++i) {
			argname = func->argNames.uosym->symbols[i];
			//postfl("    %d %d arg '%s' '%s'\n", j, i, argname->name, name->name);
			if (argname == name) {
				*level = j;
				*index = i;
				*varType = varTemp;
				if (tempfunc) *tempfunc = func;
				return true;
			}
		}
		for (i=0, k=numargs; i<methraw->numvars; ++i,++k) {
			varname = func->varNames.uosym->symbols[i];
			//postfl("    %d %d %d var '%s' '%s'\n", j, i, k, varname->name, name->name);
			if (varname == name) {
				*level = j;
				*index = k;
				*varType = varTemp;
				if (tempfunc) *tempfunc = func;
				return true;
			}
		}
	}

	if (classFindInstVar(*classobj, name, index)) {
		*level = 0;
		*varType = varInst;
		return true;
	}
	if (classFindClassVar(classobj, name, index)) {
		*varType = varClass;
		return true;
	}
	if (name == s_curProcess) {
		*varType = varPseudo;
		*index = opgProcess;
		return true;
	}
	if (name == s_curThread) {
		*varType = varPseudo;
		*index = opgThread;
		return true;
	}
	if (name == s_curMethod) {
		*varType = varPseudo;
		*index = opgMethod;
		return true;
	}
	if (name == s_curBlock) {
		*varType = varPseudo;
		*index = opgFunctionDef;
		return true;
	}
	if (name == s_curClosure) {
		*varType = varPseudo;
		*index = opgFunction;
		return true;
	}
	/*
	if (name == s_sampleRate) {
		*varType = varPseudo;
		*index = opgSampleRate;
		return true;
	}
	if (name == s_logicalClock) {
		*varType = varPseudo;
		*index = opgLogicalClock;
		return true;
	}
	if (name == s_audioClock) {
		*varType = varPseudo;
		*index = opgAudioClock;
		return true;
	}*/
	return false;
}

extern PyrSymbol *s_env;

void initSpecialClasses()
{
	gSpecialClasses[op_class_object] = s_object;
		gSpecialClasses[op_class_symbol] = s_symbol;
		gSpecialClasses[op_class_nil] = s_nil;
		gSpecialClasses[op_class_boolean] = s_boolean;
			gSpecialClasses[op_class_true] = s_true;
			gSpecialClasses[op_class_false] = s_false;
		gSpecialClasses[op_class_magnitude] = s_magnitude;
			gSpecialClasses[op_class_char] = s_char;
			gSpecialClasses[op_class_number] = s_number;
				gSpecialClasses[op_class_complex] = s_complex;
				gSpecialClasses[op_class_simple_number] = s_simple_number;
					gSpecialClasses[op_class_int] = s_int;
					gSpecialClasses[op_class_float] = s_float;
		gSpecialClasses[op_class_method] = s_method;
		gSpecialClasses[op_class_fundef] = s_fundef;
		gSpecialClasses[op_class_stream] = s_stream;
		gSpecialClasses[op_class_func] = s_func;
		gSpecialClasses[op_class_frame] = s_frame;
		gSpecialClasses[op_class_process] = s_process;
		gSpecialClasses[op_class_main] = s_main;
		gSpecialClasses[op_class_class] = s_class;
		gSpecialClasses[op_class_string] = s_string;
		
		gSpecialClasses[op_class_collection] = s_collection;
		gSpecialClasses[op_class_sequenceable_collection] = s_sequenceable_collection;
			gSpecialClasses[op_class_arrayed_collection] = s_arrayed_collection;
				gSpecialClasses[op_class_array] = s_array;
				gSpecialClasses[op_class_int8array] = s_int8array;
				gSpecialClasses[op_class_int16array] = s_int16array;
				gSpecialClasses[op_class_int32array] = s_int32array;
				gSpecialClasses[op_class_floatarray] = s_floatarray;
					gSpecialClasses[op_class_signal] = s_signal;
				gSpecialClasses[op_class_doublearray] = s_doublearray;
				gSpecialClasses[op_class_symbolarray] = s_symbolarray;
			gSpecialClasses[op_class_list] = s_list;
				gSpecialClasses[op_class_linkedlist] = s_linkedlist;
			gSpecialClasses[op_class_bag] = s_bag;
			gSpecialClasses[op_class_set] = s_set;
			gSpecialClasses[op_class_identityset] = s_identityset;
			gSpecialClasses[op_class_dictionary] = s_dictionary;
			gSpecialClasses[op_class_identitydictionary] = s_identitydictionary;
			gSpecialClasses[op_class_sortedlist] = s_sortedlist;

			gSpecialClasses[op_class_synth] = s_synth;
			gSpecialClasses[op_class_ref] = s_ref;
			gSpecialClasses[op_class_environment] = s_environment;
			gSpecialClasses[op_class_wavetable] = s_wavetable;
			gSpecialClasses[op_class_env] = s_env;
			
			//Infinitum, Point, Rect, ??
}

void initSpecialSelectors()
{
	PyrSymbol **sel;
	long i;
	
	sel = gSpecialUnarySelectors;
	sel[opNeg] = getsym("neg");
	sel[opRecip] = getsym("reciprocal");
	sel[opNot] = getsym("not");
	sel[opIsNil] = getsym("isNil");
	sel[opNotNil] = getsym("notNil");
	sel[opBitNot] = getsym("bitNot");
	sel[opAbs] = getsym("abs");
	sel[opAsFloat] = getsym("asFloat");
	sel[opAsInt] = getsym("asInt");
	sel[opCeil] = getsym("ceil");			//5
	sel[opFloor] = getsym("floor");
	sel[opFrac] = getsym("frac");
	sel[opSign] = getsym("sign");
	sel[opSquared] = getsym("squared");
	sel[opCubed] = getsym("cubed");		//10
	sel[opSqrt] = getsym("sqrt");		
	sel[opExp] = getsym("exp");
	sel[opMIDICPS] = getsym("midicps");
	sel[opCPSMIDI] = getsym("cpsmidi");
	sel[opMIDIRatio] = getsym("midiratio");
	sel[opRatioMIDI] = getsym("ratiomidi");
	sel[opAmpDb] = getsym("ampdb");		//15
	sel[opDbAmp] = getsym("dbamp");
	sel[opOctCPS] = getsym("octcps");
	sel[opCPSOct] = getsym("cpsoct");
	sel[opLog] = getsym("log");
	sel[opLog2] = getsym("log2");			//20
	sel[opLog10] = getsym("log10");
	sel[opSin] = getsym("sin");
	sel[opCos] = getsym("cos");
	sel[opTan] = getsym("tan");
	sel[opArcSin] = getsym("asin");		//25
	sel[opArcCos] = getsym("acos");
	sel[opArcTan] = getsym("atan");
	sel[opSinH] = getsym("sinh");
	sel[opCosH] = getsym("cosh");
	sel[opTanH] = getsym("tanh");			//30
	sel[opRand] = getsym("rand");
	sel[opRand2] = getsym("rand2");
	sel[opLinRand] = getsym("linrand");
	sel[opBiLinRand] = getsym("bilinrand");
	sel[opSum3Rand] = getsym("sum3rand");
/*
	sel[opExpRand] = getsym("exprand");
	sel[opBiExpRand] = getsym("biexprand");
	sel[opGammaRand] = getsym("gammarand");
	sel[opGaussRand] = getsym("gaussrand");
	sel[opPoiRand] = getsym("poirand");
*/
	sel[opDistort] = getsym("distort");
	sel[opSoftClip] = getsym("softclip");
	sel[opCoin] = getsym("coin");

	sel[opRectWindow] = getsym("rectWindow");
	sel[opHanWindow] = getsym("hanWindow");
	sel[opWelchWindow] = getsym("welWindow");
	sel[opTriWindow] = getsym("triWindow");

	sel[opSCurve] = getsym("scurve");
	sel[opRamp] = getsym("ramp");
	
	sel[opDigitValue] = getsym("digitValue");	
	sel[opSilence] = getsym("silence");	
	sel[opThru] = getsym("thru");	


	sel = gSpecialBinarySelectors;

	sel[opAdd] = getsym("+");		
	sel[opSub] = getsym("-"); 
	sel[opMul] = getsym("*"); 

	sel[opFDiv] = getsym("/"); 
	sel[opIDiv] = getsym("div"); 
	sel[opMod] = getsym("mod");
	sel[opEQ] = getsym("==");		
	sel[opNE] = getsym("!="); 
	sel[opLT] = getsym("<"); 
	sel[opGT] = getsym(">"); 
	sel[opLE] = getsym("<=");
	sel[opGE] = getsym(">=");		
	//sel[opIdentical] = getsym("===");		
	//sel[opNotIdentical] = getsym("!==");		
	sel[opMin] = getsym("min"); 
	sel[opMax] = getsym("max");
	sel[opBitAnd] = getsym("&");
	sel[opBitOr] = getsym("|");
	sel[opBitXor] = getsym("bitxor");	
	sel[opLCM] = getsym("lcm");
	sel[opGCD] = getsym("gcd");
	sel[opRound] = getsym("round");
	sel[opTrunc] = getsym("trunc");
	sel[opAtan2] = getsym("atan2");	
	sel[opHypot] = getsym("hypot");
	sel[opHypotx] = getsym("hypotApx");
	sel[opPow] = getsym("pow");
	sel[opShiftLeft] = getsym("leftShift");
	sel[opShiftRight] = getsym("rightShift");
	sel[opUnsignedShift] = getsym("unsignedRightShift");
	sel[opFill] = getsym("fill");
	sel[opRing1] = getsym("ring1");	// a * (b + 1) == a * b + a
	sel[opRing2] = getsym("ring2");	// a * b + a + b
	sel[opRing3] = getsym("ring3");	// a*a*b
	sel[opRing4] = getsym("ring4");	// a*a*b - a*b*b
	sel[opDifSqr] = getsym("difsqr");	// a*a - b*b
	sel[opSumSqr] = getsym("sumsqr");	// a*a + b*b
	sel[opSqrSum] = getsym("sqrsum");	// (a + b)^2
	sel[opSqrDif] = getsym("sqrdif");	// (a - b)^2
	sel[opAbsDif] = getsym("absdif");	// 
	sel[opThresh] = getsym("thresh");	// 
	sel[opAMClip] = getsym("amclip");	// 
	sel[opScaleNeg] = getsym("scaleneg");	// 
	sel[opClip2] = getsym("clip2");	
	sel[opFold2] = getsym("fold2");	
	sel[opWrap2] = getsym("wrap2");	
	sel[opExcess] = getsym("excess");	
	sel[opFirstArg] = getsym("<!");	
	sel[opRandRange] = getsym("rrand");	
	sel[opExpRandRange] = getsym("exprand");	
        
	
	sel = gSpecialSelectors;
	
	sel[opmNew] = getsym("new");
	sel[opmNewClear] = getsym("newClear");
	sel[opmInit] = getsym("init");			
	sel[opmAt] = getsym("at");
	sel[opmPut] = getsym("put");
	sel[opmNext] = getsym("next");
	sel[opmReset] = getsym("reset");
	sel[opmValue] = getsym("value");		
	sel[opmCopyToEnd] = getsym("copyToEnd");		// used by multiple assignment
	//sel[opmIsNil] = getsym("isNil");
	//sel[opmNotNil] = getsym("notNil");
	sel[opmSize] = getsym("size");
	sel[opmClass] = getsym("class");		
	sel[opmIf] = getsym("if");
	sel[opmWhile] = getsym("while");		
	sel[opmFor] = getsym("for");
	sel[opmAnd] = getsym("and");
	sel[opmOr] = getsym("or");				
	sel[opmIdentical] = getsym("===");		
	sel[opmNotIdentical] = getsym("!==");	
		
	sel[opmPrint] = getsym("print");
	sel[opmAdd] = getsym("add");			
	sel[opmRemove] = getsym("remove");
	sel[opmIndexOf] = getsym("indexOf");
	sel[opmWrapAt] = getsym("wrapAt");		
	sel[opmClipAt] = getsym("clipAt");
	sel[opmFoldAt] = getsym("foldAt");
	sel[opmWrapPut] = getsym("wrapPut");
	sel[opmClipPut] = getsym("clipPut");
	sel[opmFoldPut] = getsym("foldPut");	
	sel[opmDo] = getsym("do");
	sel[opmCollect] = getsym("collect");
	sel[opmSelect] = getsym("select"); 
	sel[opmReject] = getsym("reject");
	sel[opmAny] = getsym("any");
	sel[opmEvery] = getsym("every");
	sel[opmFind] = getsym("find");
	
	sel[opmChoose] = getsym("choose");
	
	sel[opmValueList] = getsym("valueList");
	sel[opmAddFirst] = getsym("addFirst");
	sel[opmIn] = getsym("in");
	sel[opmOut] = getsym("out");
	
	sel[opmPrimitiveFailed] = getsym("primitiveFailed");	
	sel[opmSubclassResponsibility] = getsym("subclassResponsibility");
	sel[opmShouldNotImplement] = getsym("shouldNotImplement");
	sel[opmIndexOutOfRange] = getsym("indexOutOfRange");
	sel[opmDoesNotUnderstand] = getsym("doesNotUnderstand");	// not really needed
	sel[opmNotYetImplemented] = getsym("notYetImplemented");	
	
	sel[opmAtSign] = getsym("@");
	sel[opmWrapAtSign] = getsym("@@");
	sel[opmClipAtSign] = getsym("|@|");
	sel[opmFoldAtSign] = getsym("@|@");

	sel[opmMultiChanPerform] = getsym("multiChannelPerform"); // UGens
	sel[opmAR] = getsym("ar"); // UGens
	sel[opmAR1] = getsym("ar1"); // UGens
	sel[opmKR] = getsym("kr"); // UGens
	sel[opmKR1] = getsym("kr1"); // UGens
	
	sel[opmEnvirGet] = getsym("envirGet");
	sel[opmEnvirPut] = getsym("envirPut");
	
	sel[opmHalt] = getsym("halt");
	sel[opmForBy] = getsym("forBy");
	sel[opmReverseDo] = getsym("reverseDo");
	sel[opmLoop] = getsym("loop");
	sel[opmNonBooleanError] = getsym("mustBeBoolean");

	for (i=0; i<opNumUnarySelectors; ++i) {
		gSpecialUnarySelectors[i]->specialIndex = i;
	}
	for (i=0; i<opNumBinarySelectors; ++i) {
		gSpecialBinarySelectors[i]->specialIndex = i;
	}
}

bool findSpecialClassName(PyrSymbol *className, int *index)
{
	int i;
	for (i=0; i<op_NumSpecialClasses; ++i) {
		if (gSpecialClasses[i] == className) {
			*index = i;
			return true;
		}
	}
	return false;
}