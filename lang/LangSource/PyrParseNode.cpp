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
#include "PyrListPrim.h"
#include "PyrSymbolTable.h"
#include "Opcodes.h"
#include "PyrKernelProto.h"
#include "PyrObjectProto.h"
#include "GC.h"
#include <new>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "InitAlloc.h"
#include "PredefinedSymbols.h"
#include "SimpleStack.h"
#include "PyrPrimitive.h"
#include "SC_Win32Utils.h"
#include "SC_LanguageConfig.hpp"
#include "SC_Codecvt.hpp"

namespace bfs = boost::filesystem;

AdvancingAllocPool gParseNodePool;

PyrSymbol *gSpecialUnarySelectors[opNumUnarySelectors];
PyrSymbol *gSpecialBinarySelectors[opNumBinarySelectors];
PyrSymbol *gSpecialSelectors[opmNumSpecialSelectors];
PyrSymbol* gSpecialClasses[op_NumSpecialClasses];
PyrSlot gSpecialValues[svNumSpecialValues];

PyrParseNode* gRootParseNode;
intptr_t gParserResult;

int conjureConstantIndex(PyrParseNode *node, PyrBlock* func, PyrSlot *slot);
void compilePushConstant(PyrParseNode* node, PyrSlot *slot);

PyrClass *gCurrentClass = NULL;
PyrClass *gCurrentMetaClass = NULL;
PyrClass *gCompilingClass = NULL;
PyrMethod *gCompilingMethod = NULL;
PyrBlock *gCompilingBlock = NULL;
PyrBlock *gPartiallyAppliedFunction = NULL;

bool gIsTailCodeBranch = false;
bool gTailIsMethodReturn = false;
int gFunctionHighestExternalRef = 1;
bool gFunctionCantBeClosed = true;
#if TAILCALLOPTIMIZE
bool gGenerateTailCallByteCodes = true;
#else
bool gGenerateTailCallByteCodes = false;
#endif

long gInliningLevel;

int compileErrors = 0;
int numOverwrites = 0;
std::string overwriteMsg;

extern bool compilingCmdLine;
extern int errLineOffset, errCharPosOffset;

const char* nodename[] = {
	"ClassNode",
	"ClassExtNode",
	"MethodNode",
	"BlockNode",
	"SlotNode",

	/* variable declarations */
	"VarListNode",
	"VarDefNode",
	"DynDictNode",
	"DynListNode",
	"LitListNode",
	"LitDictNode",

	"StaticVarListNode",
	"InstVarListNode",
	"PoolVarListNode",
	"ArgListNode",
	"SlotDefNode",

	/* selectors */
	"LiteralNode",

	/* code */
	"PushLitNode",
	"PushNameNode",
	"PushKeyArgNode",
	"CallNode",
	"BinopCallNode",
	"DropNode",
	"AssignNode",
	"MultiAssignNode",
	"MultiAssignVarListNode",
	"SetterNode",
	"CurryArgNode",

	"ReturnNode",
	"BlockReturnNode"
};

void compileTail()
{
	if (gGenerateTailCallByteCodes && gIsTailCodeBranch)
	{
		//if (gCompilingClass && gCompilingMethod) post("tail call %s:%s  ismethod %d\n",
		//	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name, gTailIsMethodReturn);
		if (gTailIsMethodReturn)
			compileByte(255);
		else
			compileByte(176);
	}
}



PyrGC* compileGC();
PyrGC* compileGC()
{
	return gCompilingVMGlobals ? gCompilingVMGlobals->gc : 0;
}

void initParser()
{
	compileErrors = 0;
	numOverwrites = 0;
	overwriteMsg.clear();
}

void finiParser()
{
}

void initParseNodes()
{
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

PyrParseNode::PyrParseNode(int inClassNo)
{
	mClassno = inClassNo;
	mNext = 0;
	mTail = this;
	mCharno = ::charno;
	mLineno = ::lineno;
	mParens = 0;
}

void compileNodeList(PyrParseNode *node, bool onTailBranch)
{
	PyrSlot dummy;
	//postfl("->compileNodeList\n");
	for (; node; node = node->mNext) {
            //postfl("-->compileNodeList %p\n", node);
		COMPILENODE(node, &dummy, onTailBranch);
            //postfl("<--compileNodeList %p\n", node);
	}
	//postfl("<-compileNodeList\n");
}

void nodePostErrorLine(PyrParseNode* node)
{
	postErrorLine(node->mLineno, linestarts[node->mLineno], node->mCharno);
}

PyrPushNameNode* newPyrPushNameNode(PyrSlotNode *slotNode)
{
	slotNode->mClassno = pn_PushNameNode;
	return (PyrPushNameNode*)slotNode;
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
		gFunctionCantBeClosed = true;
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
				index += slotRawInt(&classobj->classVarIndex);
				if (index < 4096) {
					compileByte((opPushClassVar<<4) | ((index >> 8) & 15));
					compileByte(index & 255);
				} else {
					compileByte(opPushClassVar);
					compileByte((index >> 8) & 255);
					compileByte(index & 255);
				}
			} break;
			case varConst : {
				PyrSlot *slot = slotRawObject(&classobj->constValues)->slots + index;
				compilePushConstant(node, slot);
			} break;
			case varTemp :
				vindex = index;
				if (level == 0) {
					compileOpcode(opPushTempZeroVar, vindex);
				} else if (level < 8) {
					compileOpcode(opPushTempVar, level);
					compileByte(vindex);
				} else {
					compileByte(opPushTempVar);
					compileByte(level);
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

PyrCurryArgNode* newPyrCurryArgNode()
{
	PyrCurryArgNode* node = ALLOCNODE(PyrCurryArgNode);
	return node;
}

void PyrCurryArgNode::compile(PyrSlot *result)
{
	if (gPartiallyAppliedFunction) {
		compileOpcode(opPushTempZeroVar, mArgNum);
	} else {
		error("found _ argument outside of a call.\n");
		nodePostErrorLine((PyrParseNode*)this);
		compileErrors++;
	}
}

PyrSlotNode* newPyrSlotNode(PyrSlot *slot)
{
	PyrSlotNode* node = ALLOCNODE(PyrSlotNode);
	node->mSlot = *slot;
	return node;
}

void PyrSlotNode::compile(PyrSlot *result)
{
	if (mClassno == pn_LiteralNode)
		compileLiteral(result);
	else if (mClassno == pn_PushLitNode)
		compilePushLit(result);
	else if (mClassno == pn_PushNameNode)
		compilePushVar((PyrParseNode*)this, slotRawSymbol(&mSlot));
	else {
		error("compilePyrSlotNode: shouldn't get here.\n");
		dumpObjectSlot(&mSlot);
		nodePostErrorLine((PyrParseNode*)this);
		compileErrors++;
		//Debugger();
	}
}

PyrClassExtNode* newPyrClassExtNode(PyrSlotNode* className, PyrMethodNode* methods)
{
	PyrClassExtNode* node = ALLOCNODE(PyrClassExtNode);
	node->mClassName = className;

	node->mMethods = methods;
	return node;
}

void PyrClassExtNode::compile(PyrSlot *result)
{
	PyrClass *classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
	if (!classobj) {
		const bfs::path relpath = relativeToCompileDir(bfs::path(gCompilingFileSym->name));
		error("Class extension for nonexistent class '%s'\n     In file:'%s'\n",
			slotRawSymbol(&mClassName->mSlot)->name,
			SC_Codecvt::path_to_utf8_str(relpath).c_str()
		);
		return;
	}
	gCurrentClass = classobj;
	gCurrentMetaClass = classobj->classptr;
	compileExtNodeMethods(this);
}

void compileExtNodeMethods(PyrClassExtNode* node)
{
	PyrMethodNode *method;
	method = node->mMethods;
	for (; method; method = (PyrMethodNode*)method->mNext) {
		PyrSlot dummy;
		//post("compile ext %s:%s\n",
		method->mExtension = true;
		compilePyrMethodNode(method, &dummy);
	}
	gCompilingMethod = NULL;
	gCompilingBlock = NULL;
	gPartiallyAppliedFunction = NULL;
	gInliningLevel = 0;
}

PyrClassNode* newPyrClassNode(PyrSlotNode* className, PyrSlotNode* superClassName,
	PyrVarListNode* varlists, PyrMethodNode* methods, PyrSlotNode* indexType)
{
	PyrClassNode* node = ALLOCNODE(PyrClassNode);
	node->mClassName = className;
	node->mIndexType = indexType;

	node->mSuperClassName = superClassName;
	node->mVarlists = varlists;
	node->mMethods = methods;
	node->mVarTally[varInst] = 0;
	node->mVarTally[varClass] = 0;
	node->mVarTally[varTemp] = 0;
	node->mVarTally[varConst] = 0;
	//node->mVarTally[varPool] = 0;
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

	isIntrinsic = slotRawInt(&classobj->classFlags) & classIsIntrinsic;

	numinstvars = numInstVars(classobj);
	numclassvars = numClassVars(classobj);
	if (numinstvars == node->mVarTally[varInst] + node->mNumSuperInstVars
		&& numclassvars == node->mVarTally[varClass]) {

		xclass = 0;
		xinst = node->mNumSuperInstVars;
		varlist = node->mVarlists;
		for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
			int type = varlist->mFlags;
			if (type == varInst) {
				vardef = varlist->mVarDefs;
				varNames = slotRawSymbolArray(&classobj->instVarNames)->symbols;
				for (i=0; vardef; vardef = (PyrVarDefNode*)vardef->mNext, xinst++, ++i) {
					if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xinst]) {
						errnode = (PyrParseNode*)vardef;
						//post("A %s %d %d %d\n", vardef->mVarName->slotRawSymbol(&mSlot)->name,
						//	vardef->mVarName->slotRawSymbol(&mSlot), varNames[xinst].us, xinst);
						//post("A %s %s %d\n", vardef->mVarName->slotRawSymbol(&mSlot)->name,
						//	varNames[xinst].us->name, xinst);
						goto differExit;
					}
				}
			} else if (type == varClass) {
				vardef = varlist->mVarDefs;
				varNames = slotRawSymbolArray(&classobj->classVarNames)->symbols;
				for (i=0; vardef && xclass < numclassvars; vardef = (PyrVarDefNode*)vardef->mNext, xclass++, ++i) {
					if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xclass]) {
						errnode = (PyrParseNode*)vardef;
						//post("B %d %d %d\n", vardef->mVarName->slotRawSymbol(&mSlot), varNames[xclass].us, xclass);
						goto differExit;
					}
				}
			}
		}
	} else {
		//post("C %d %d %d   %d %d\n", numinstvars, node->mVarTally[varInst], node->mNumSuperInstVars,
		//	numclassvars, node->mVarTally[varClass]);
		errnode = (node->mVarlists ? (PyrParseNode*)node->mVarlists : (PyrParseNode*)node->mClassName);
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

	node->mVarTally[varInst] = 0;
	node->mVarTally[varClass] = 0;
	node->mVarTally[varTemp] = 0;
	node->mVarTally[varConst] = 0;

	// count number of variables of each type
	varlist = node->mVarlists;
	for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
		int type = varlist->mFlags;
		vardef = varlist->mVarDefs;
		for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
			node->mVarTally[type]++;
			if (type == varClass) {
				if (vardef->mFlags & rwReadOnly) {
					*numClassMethods = *numClassMethods + 1;
				}
				if (vardef->mFlags & rwWriteOnly) {
					*numClassMethods = *numClassMethods + 1;
				}
			} else if (type == varInst) {
				if (vardef->mFlags & rwReadOnly) {
					*numInstMethods = *numInstMethods + 1;
				}
				if (vardef->mFlags & rwWriteOnly) {
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
	method = node->mMethods;
	for (; method; method = (PyrMethodNode*)method->mNext) {
		if (method->mIsClassMethod) *numClassMethods = *numClassMethods + 1;
		else *numInstMethods = *numInstMethods + 1;
	}
}

void compileNodeMethods(PyrClassNode* node)
{
	PyrMethodNode *method;
	method = node->mMethods;
	for (; method; method = (PyrMethodNode*)method->mNext) {
		PyrSlot dummy;
		method->mExtension = false;
		compilePyrMethodNode(method, &dummy);
	}
	gCompilingMethod = NULL;
	gCompilingBlock = NULL;
	gPartiallyAppliedFunction = NULL;
	gInliningLevel = 0;
}

PyrClass* getNodeSuperclass(PyrClassNode *node)
{
	PyrClass *superclassobj = NULL;
//	postfl("getNodeSuperclass node %d\n", node);
//	postfl("getNodeSuperclass node->mSuperClassName %d\n", node->mSuperClassName);
//	postfl("getNodeSuperclass node->mSuperClassName->mSlot.utag %d\n",
//		node->mSuperClassName->mSlot.utag);
	if (node->mSuperClassName && IsSym(&node->mSuperClassName->mSlot)) {
		superclassobj = slotRawSymbol(&node->mSuperClassName->mSlot)->u.classobj;
		if (superclassobj == NULL) {
			error("Cannot find superclass '%s' for class '%s'\n",
				slotSymString(&node->mSuperClassName->mSlot),
				slotSymString(&node->mClassName->mSlot));
			nodePostErrorLine((PyrParseNode*)node->mSuperClassName);
			superclassobj = (PyrClass*)-1;
			compileErrors++;
		}
	} else {
		if (slotRawSymbol(&node->mClassName->mSlot) != s_object) {
			superclassobj = class_object;
		} // else this is object and there is no superclass
	}
	return superclassobj;
}

void fillClassPrototypes(PyrClassNode *node, PyrClass *classobj, PyrClass *superclassobj)
{
	PyrVarListNode* varlist;
	PyrVarDefNode *vardef;
	PyrSlot *islot, *cslot, *kslot;
	PyrSymbol **inameslot, **cnameslot, **knameslot;
	PyrClass *metaclassobj;
	PyrMethod *method;
	PyrMethodRaw *methraw;
	int instVarIndex, classVarIndex;

	// copy superclass's prototype to here
	if (superclassobj && NotNil(&superclassobj->iprototype)
		&& slotRawObject(&superclassobj->iprototype)->size) {

		memcpy(slotRawObject(&classobj->iprototype)->slots, slotRawObject(&superclassobj->iprototype)->slots,
			sizeof(PyrSlot)* slotRawObject(&superclassobj->iprototype)->size);
		//slotRawObject(&classobj->iprototype)->size = slotRawObject(&superclassobj->iprototype)->size;
		slotRawObject(&classobj->iprototype)->size = node->mNumSuperInstVars;

		memcpy(slotRawSymbolArray(&classobj->instVarNames)->symbols,
			slotRawSymbolArray(&superclassobj->instVarNames)->symbols,
			sizeof(PyrSymbol*)* slotRawObject(&superclassobj->instVarNames)->size);
		//slotRawObject(&classobj->instVarNames)->size = slotRawObject(&superclassobj->iprototype)->size;
		slotRawObject(&classobj->instVarNames)->size = node->mNumSuperInstVars;
	}
	// fill the class' own part of prototypes
	metaclassobj = classobj->classptr;
	varlist = node->mVarlists;
	if (NotNil(&classobj->iprototype)) {
		islot = slotRawObject(&classobj->iprototype)->slots + node->mNumSuperInstVars;
	}
	if (NotNil(&classobj->cprototype)) {
		cslot = slotRawObject(&classobj->cprototype)->slots;
	}
	if (NotNil(&classobj->constValues)) {
		kslot = slotRawObject(&classobj->constValues)->slots;
	}
	if (NotNil(&classobj->instVarNames)) {
		inameslot = slotRawSymbolArray(&classobj->instVarNames)->symbols + node->mNumSuperInstVars;
	}
	if (NotNil(&classobj->classVarNames)) {
		cnameslot = slotRawSymbolArray(&classobj->classVarNames)->symbols;
	}
	if (NotNil(&classobj->constNames)) {
		knameslot = slotRawSymbolArray(&classobj->constNames)->symbols;
	}
	instVarIndex = node->mNumSuperInstVars;
	classVarIndex = 0;
	for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
		int type = varlist->mFlags;
		switch (type) {
			case varInst :
				vardef = varlist->mVarDefs;
				for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
					PyrSlot litslot;
					compilePyrLiteralNode((PyrLiteralNode*)vardef->mDefVal, &litslot);
					*islot++ = litslot;
					slotRawObject(&classobj->iprototype)->size++;
					*inameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
					slotRawSymbolArray(&classobj->instVarNames)->size++;
					if (vardef->mFlags & rwReadOnly) {
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
						SetNil(&method->contextDef);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, classobj);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->mVarName->mLineno] + errCharPosOffset);
						slotCopy(&method->name, &vardef->mVarName->mSlot);
						methraw->methType = methReturnInstVar;
						methraw->specialIndex = instVarIndex;
						addMethod(classobj, method);
					}
					if (vardef->mFlags & rwWriteOnly) {
						char setterName[256];
						PyrSymbol *setterSym;
						sprintf(setterName, "%s_", slotRawSymbol(&vardef->mVarName->mSlot)->name);
						//underscore = strcpy(setterName, slotRawSymbol(&vardef->mVarName->mSlot)->name);
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
						SetNil(&method->contextDef);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, classobj);
						SetSymbol(&method->name, setterSym);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->mVarName->mLineno] + errCharPosOffset);

						methraw->methType = methAssignInstVar;
						methraw->specialIndex = instVarIndex;
						addMethod(classobj, method);
					}
					instVarIndex++;
				}
				break;
			case varClass :
				vardef = varlist->mVarDefs;
				for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
					PyrSlot litslot;
					compilePyrLiteralNode((PyrLiteralNode*)vardef->mDefVal, &litslot);
					*cslot++ = litslot;
					slotRawObject(&classobj->cprototype)->size++;
					*cnameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
					slotRawSymbolArray(&classobj->classVarNames)->size++;
					if (vardef->mFlags & rwReadOnly) {
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
						SetNil(&method->contextDef);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, metaclassobj);
						slotCopy(&method->name, &vardef->mVarName->mSlot);
						SetSymbol(&method->selectors, slotRawSymbol(&classobj->name));
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->mVarName->mLineno] + errCharPosOffset);

						methraw->methType = methReturnClassVar;
						methraw->specialIndex = classVarIndex + slotRawInt(&classobj->classVarIndex);
						addMethod(metaclassobj, method);
					}
					if (vardef->mFlags & rwWriteOnly) {
						char setterName[256];
						PyrSymbol *setterSym;
						sprintf(setterName, "%s_", slotRawSymbol(&vardef->mVarName->mSlot)->name);
						//underscore = strcpy(setterName, slotRawSymbol(&vardef->mVarName->mSlot)->name);
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
						SetNil(&method->contextDef);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, metaclassobj);
						SetSymbol(&method->name, setterSym);
						SetSymbol(&method->selectors, slotRawSymbol(&classobj->name));
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->mVarName->mLineno] + errCharPosOffset);

						methraw->methType = methAssignClassVar;
						methraw->specialIndex = classVarIndex + slotRawInt(&classobj->classVarIndex);
						addMethod(metaclassobj, method);
					}
					classVarIndex++;
				}
				break;
			case varConst :
				vardef = varlist->mVarDefs;
				for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
					PyrSlot litslot;
					compilePyrLiteralNode((PyrLiteralNode*)vardef->mDefVal, &litslot);
					*kslot++ = litslot;
					slotRawObject(&classobj->constValues)->size++;
					*knameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
					slotRawSymbolArray(&classobj->constNames)->size++;
					if (vardef->mFlags & rwReadOnly) {
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
						SetNil(&method->contextDef);
						SetNil(&method->varNames);
						SetObject(&method->ownerclass, metaclassobj);
						slotCopy(&method->name, &vardef->mVarName->mSlot);
						if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
						SetInt(&method->charPos, linestarts[vardef->mVarName->mLineno] + errCharPosOffset);

						methraw->methType = methReturnLiteral;
						slotCopy(&method->selectors, &litslot);
						addMethod(metaclassobj, method);
					}
				}
				break;
		}
	}
}

int getIndexType(PyrClassNode *classnode)
{
	PyrSlotNode *node;
	int res;

	node = classnode->mIndexType;
	if (node == NULL) res = obj_notindexed;
	else {
		char *name;
		name = slotRawSymbol(&node->mSlot)->name;
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

void PyrClassNode::compile(PyrSlot *result)
{
	PyrClass *classobj, *superclassobj, *metaclassobj;
	int numClassMethods, numInstMethods;
	bool isIntrinsic;
	bool varsDiffer, superclassesDiffer, indexTypesDiffer;
	bool shouldRecompileSubclasses = false;
	int indexType;

	// find num instvars in superclass
	//postfl("class '%s'\n", slotRawSymbol(&mClassName->mSlot)->name);
	superclassobj = getNodeSuperclass(this);
	indexType = getIndexType(this);
	//postfl("%s %d\n", slotRawSymbol(&mClassName->mSlot)->name, indexType);

	if ((size_t)superclassobj == -1) {
		// redundant error message removed:
		//error("Can't find superclass of '%s'\n", slotRawSymbol(&mClassName->mSlot)->name);
		//nodePostErrorLine(node);
		return; // can't find superclass
	}
	mNumSuperInstVars = numSuperInstVars(superclassobj);

	numClassMethods = 0;
	numInstMethods = 0;
	countClassVarDefs(this, &numClassMethods, &numInstMethods);
	//postfl("accessor methods %d %d\n", numClassMethods, numInstMethods);
	countNodeMethods(this, &numClassMethods, &numInstMethods);
	//postfl("total methods %d %d\n", numClassMethods, numInstMethods);

	// get or make a class object
	// see if it already exists
	classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
	if (classobj) {
		// deal with intrinsic classes or other classes being recompiled here.
		// recompile of subclasses not necessary if inst and class vars are
		// unchanged.
		metaclassobj = (PyrClass*)classobj->classptr;
		isIntrinsic = slotRawInt(&classobj->classFlags) & classIsIntrinsic;

		varsDiffer = compareVarDefs(this, classobj);
		if (varsDiffer) {
			if (isIntrinsic) {
				//error("Class '%s' declaration doesn't match intrinsic definition.\n",
				//	slotRawSymbol(&mClassName->mSlot)->name);
				return;
			} else {
				shouldRecompileSubclasses = true;
			}
		}
		superclassesDiffer = superclassobj != slotRawSymbol(&classobj->superclass)->u.classobj;
		indexTypesDiffer = indexType != slotRawInt(&classobj->instanceFormat);
		//postfl("%d %d %d\n", indexType, slotRawInt(&classobj->instanceFormat));
		//if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
		if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
			if (isIntrinsic) {
				if (superclassesDiffer) {
					error("Superclass of '%s' does not match intrinsic definition.\n",
						slotRawSymbol(&mClassName->mSlot)->name);
					nodePostErrorLine((PyrParseNode*)(mSuperClassName ?
						mSuperClassName : mClassName));
					compileErrors++;
				}
				if (indexTypesDiffer) {
					error("Index type of '%s' does not match intrinsic definition.\n",
						slotRawSymbol(&mClassName->mSlot)->name);
					nodePostErrorLine((indexType ? (PyrParseNode*)mIndexType : (PyrParseNode*)mClassName));
					compileErrors++;
				}
				error("Class '%s' declaration doesn't match intrinsic definition.\n",
						slotRawSymbol(&mClassName->mSlot)->name);
				return;
			} else {
				shouldRecompileSubclasses = true;
			}
		}
		// reallocate fields in the class object
		reallocClassObj(metaclassobj,
			classClassNumInstVars, 0, 0,
			numClassMethods, indexType, 0);

		//postfl("^3 %d %d\n", metaclassobj, class_class);
		//postfl("^4 %d %d\n", slotRawObject(&metaclassobj->iprototype), slotRawObject(&class_class->iprototype));
		memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
			sizeof(PyrSlot) * classClassNumInstVars);
		memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
			slotRawSymbolArray(&class_class->instVarNames)->symbols,
			sizeof(PyrSymbol*) * classClassNumInstVars);
		slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
		slotRawSymbolArray(&metaclassobj->instVarNames)->size = classClassNumInstVars;

		reallocClassObj(classobj,
			mVarTally[varInst] + mNumSuperInstVars,
			mVarTally[varClass],
			mVarTally[varConst],
			numInstMethods, indexType, 0);

	} else {
		PyrSymbol *superClassName, *metaClassName, *metaSuperClassName;

		superClassName = superclassobj ? slotRawSymbol(&superclassobj->name) : NULL;
		metaClassName = getmetasym(slotRawSymbol(&mClassName->mSlot)->name);
		metaClassName->flags |= sym_MetaClass;
		metaSuperClassName = superClassName ? getmetasym(superClassName->name) : NULL;

		metaclassobj = newClassObj(class_class,
			metaClassName, metaSuperClassName,
			classClassNumInstVars, 0, 0, numClassMethods, indexType, 0);
// test
		//postfl("^1 %d %d\n", metaclassobj, class_class);
		//postfl("^2 %d %d\n", slotRawObject(&metaclassobj->iprototype), slotRawObject(&class_class->iprototype));

		memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
			sizeof(PyrSlot) * classClassNumInstVars);
		memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
			slotRawSymbolArray(&class_class->instVarNames)->symbols,
			sizeof(PyrSymbol*) * classClassNumInstVars);
		slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
		slotRawObject(&metaclassobj->instVarNames)->size = classClassNumInstVars;
// end test
		classobj = newClassObj(metaclassobj,
			slotRawSymbol(&mClassName->mSlot), superClassName,
			mVarTally[varInst] + mNumSuperInstVars,
			mVarTally[varClass], mVarTally[varConst], numInstMethods, indexType, 0);
	}
	gCurrentClass = classobj;
	gCurrentMetaClass = metaclassobj;
	if (gCompilingFileSym) {
		SetSymbol(&classobj->filenameSym, gCompilingFileSym);
		SetInt(&classobj->charPos, linestarts[mClassName->mLineno] + errCharPosOffset);
		SetSymbol(&metaclassobj->filenameSym, gCompilingFileSym);
		SetInt(&metaclassobj->charPos, linestarts[mClassName->mLineno] + errCharPosOffset);
	} else {
		SetNil(&classobj->filenameSym);
		SetNil(&metaclassobj->filenameSym);
	}

	// fill inst and class prototypes
	fillClassPrototypes(this, classobj, superclassobj);

	// compile methods
	compileNodeMethods(this);

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
		vardef = varlist->mVarDefs;
		for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
			lastvardef = vardef;
		}
		prevvarlist = varlist;
		varlist = (PyrVarListNode*)varlist->mNext;

		for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
			vardef = varlist->mVarDefs;
			if (lastvardef) {
				lastvardef->mNext = (PyrParseNode*)vardef;
			} else {
				prevvarlist->mVarDefs = vardef;
			}
			// find end of this list
			for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
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
		vardef = varlist->mVarDefs;
		lastvardef = (PyrVarDefNode*)vardef->mTail;
		prevvarlist = varlist;
		varlist = (PyrVarListNode*)varlist->mNext;

		for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
			vardef = varlist->mVarDefs;
			lastvardef->mNext = (PyrParseNode*)vardef;

			// find end of this list
			lastvardef = (PyrVarDefNode*)vardef->mTail;
		}
	}
}
#endif

PyrMethodNode* newPyrMethodNode(PyrSlotNode* methodName, PyrSlotNode* primitiveName,
	PyrArgListNode* arglist, PyrVarListNode *varlist, PyrParseNode* body, int isClassMethod)
{
	PyrMethodNode* node = ALLOCNODE(PyrMethodNode);
	node->mMethodName = methodName;
	node->mPrimitiveName = primitiveName;
	node->mArglist = arglist;
	catVarLists(varlist);
	node->mVarlist = varlist;
	node->mBody = body;
	node->mIsClassMethod = isClassMethod;
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

	//if (strcmp("ar", slotRawSymbol(&gCompilingMethod->name)->name)==0) Debugger();
	if (actualArg->mClassno != pn_PushNameNode) {
		if (numArgs < 3) {
			return push_Normal;
		}
		actualArg = actualArg->mNext;
		for (i=1; i<numArgs; ++i) {
			if (actualArg->mClassno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
				return push_Normal;
			}

			actualArg = actualArg->mNext;
		}
		return push_AllButFirstArg;

	} else {
		for (i=0; i<numArgs; ++i) {
			if (actualArg->mClassno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
				return push_Normal;
			}
			actualArg = actualArg->mNext;
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
		actualArg = actualArg->mNext;
		actualArg = actualArg->mNext;
		for (i=1; i<numArgs; ++i) {
			if (actualArg->mClassno != pn_PushNameNode) {
				return push_Normal;
			}
			nameNode = (PyrPushNameNode*)actualArg;
			if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
				return push_Normal;
			}

			actualArg = actualArg->mNext;
		}
		return push_AllButFirstArg2;
	}
	return push_Normal;
}

int compareCallArgs(PyrMethodNode* node, PyrCallNode *cnode, int *varIndex, PyrClass **specialClass)
{
	int i, numFormalArgs, numActualArgs;
	int special, varType, varLevel;
	PyrParseNode *actualArg;
	PyrVarDefNode *formalArg;
	PyrPushNameNode *nameNode;

	// fail if has a rest arg .. too much trouble?
	if (node->mArglist && node->mArglist->mRest) {
		return methNormal;
	}

	// check first actual arg is 'this'
	actualArg = cnode->mArglist;
	if (actualArg->mClassno != pn_PushNameNode) {
		return methNormal;
	}
	nameNode = (PyrPushNameNode*)actualArg;
	if (slotRawSymbol(&nameNode->mSlot) == s_this) {
		special = methRedirect;
	} else if (slotRawSymbol(&nameNode->mSlot) == s_super) {
		special = methRedirectSuper;
	} else {
		bool varFound;
		PyrClass *classobj;

		classobj = gCompilingClass;
		varFound = findVarName(gCompilingBlock, &classobj, slotRawSymbol(&nameNode->mSlot),
			&varType, &varLevel, varIndex, NULL);
		if (!varFound ) return methNormal;

		if (varType == varInst) special = methForwardInstVar;
		else if (varType == varClass) {
			special = methForwardClassVar;
			*varIndex += slotRawInt(&classobj->classVarIndex);
			*specialClass = classobj;
		}
		else return methNormal;
	}

	actualArg = actualArg->mNext;
	numActualArgs = nodeListLength((PyrParseNode*)cnode->mArglist);

	if (!node->mArglist) {
		numFormalArgs = 1;
		if (numActualArgs != numFormalArgs) {
			return methNormal;
		}
	} else {
		numFormalArgs = 1 + nodeListLength((PyrParseNode*)node->mArglist->mVarDefs);
		if (numActualArgs != numFormalArgs) {
			return methNormal;
		}

		formalArg = node->mArglist->mVarDefs;
		for (i=0; i<numActualArgs-1; ++i) {
			if (actualArg->mClassno != pn_PushNameNode) {
				return methNormal;
			}

			nameNode = (PyrPushNameNode*)actualArg;
			if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbol(&formalArg->mVarName->mSlot)) {
				return methNormal;
			}

			formalArg = (PyrVarDefNode*)formalArg->mNext;
			actualArg = actualArg->mNext;
		}
	}
	/*
	if (special == methForwardInstVar) {
		postfl("methForwardInstVar %s:%s  formal %d  actual %d\n", slotRawSymbol(&gCompilingClass->name)->name,
			slotRawSymbol(&gCompilingMethod->name)->name, numFormalArgs, numActualArgs);
	}
	if (special == methForwardClassVar) {
		postfl("methForwardClassVar %s:%s  formal %d  actual %d\n", slotRawSymbol(&gCompilingClass->name)->name,
			slotRawSymbol(&gCompilingMethod->name)->name, numFormalArgs, numActualArgs);
	}
	if (special == methRedirectSuper) {
		postfl("methRedirectSuper %s:%s  formal %d  actual %d\n", slotRawSymbol(&gCompilingClass->name)->name,
			slotRawSymbol(&gCompilingMethod->name)->name, numFormalArgs, numActualArgs);
	}
	*/

//	if (special == methTempDelegate) {
//		postfl("methTempDelegate %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
//			slotRawSymbol(&gCompilingMethod->name)->name);
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

PyrMethod* initPyrMethod(PyrMethod* method);

void compilePyrMethodNode(PyrMethodNode *node, PyrSlot *result)
{
	node->compile(result);
}

void PyrMethodNode::compile(PyrSlot *result)
{
	PyrMethod *method, *oldmethod;
	PyrMethodRaw *methraw;
	int i, j, numArgs, numVars, methType, funcVarArgs, firstKeyIndex;
	int index, numSlots, numArgNames;
	bool hasPrimitive = false;
	bool hasVarExprs = false;
	PyrVarDefNode *vardef;
	PyrObject *proto;
	PyrSymbolArray *argNames, *varNames;

	SetTailBranch branch(false);

	//postfl("->method '%s'\n", slotRawSymbol(&mMethodName->mSlot)->name);
	gCompilingClass = mIsClassMethod ? gCurrentMetaClass : gCurrentClass;
	oldmethod = classFindDirectMethod(gCompilingClass, slotRawSymbol(&mMethodName->mSlot));

	if (oldmethod && !mExtension) {
		error("Method %s:%s already defined.\n",
			slotRawSymbol(&slotRawClass(&oldmethod->ownerclass)->name)->name, slotRawSymbol(&oldmethod->name)->name);
		nodePostErrorLine((PyrParseNode*)mMethodName);
		compileErrors++;
		return;
	}

	if (oldmethod) {
		++numOverwrites;

		// accumulate overwrite message onto the string buffer
		overwriteMsg
			.append(slotRawSymbol(&slotRawClass(&oldmethod->ownerclass)->name)->name)
			.append(":")
			.append(slotRawSymbol(&oldmethod->name)->name)
			.append("\t")
			.append(gCompilingFileSym->name)
			.append("\t")
			.append(slotRawSymbol(&oldmethod->filenameSym)->name)
			.append("\n");

		method = oldmethod;
		freePyrSlot(&method->code);
		freePyrSlot(&method->selectors);
		freePyrSlot(&method->prototypeFrame);
		freePyrSlot(&method->argNames);
		freePyrSlot(&method->varNames);
		initPyrMethod(method);
	} else {
		method = newPyrMethod();
	}
	SetObject(&method->ownerclass, gCompilingClass);

	methraw = METHRAW(method);
	methraw->unused1 = 0;
	methraw->unused2 = 0;

	//postfl("method %p raw %p\n", method, methraw);
	method->contextDef = o_nil;
	method->name = mMethodName->mSlot;
	if (gCompilingFileSym) SetSymbol(&method->filenameSym, gCompilingFileSym);
	SetInt(&method->charPos, linestarts[mMethodName->mLineno] + errCharPosOffset);
	if (mPrimitiveName) {
		hasPrimitive = true;
		method->primitiveName = mPrimitiveName->mSlot;
		methraw->specialIndex = slotRawSymbol(&mPrimitiveName->mSlot)->u.index;
	}
	gCompilingBlock = (PyrBlock*)method;
	gCompilingMethod = (PyrMethod*)method;
	gPartiallyAppliedFunction = NULL;
	gInliningLevel = 0;

	methraw->needsHeapContext = 0;

	methraw->varargs = funcVarArgs = (mArglist && mArglist->mRest) ? 1 : 0;
	numArgs = mArglist ? nodeListLength((PyrParseNode*)mArglist->mVarDefs) + 1 : 1;
	numVars = mVarlist ? nodeListLength((PyrParseNode*)mVarlist->mVarDefs) : 0;

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
		slotCopy(&method->argNames, &o_argnamethis);
		slotCopy(&method->prototypeFrame, &o_onenilarray);
	} else {
		argNames = newPyrSymbolArray(NULL, numArgNames, obj_permanent | obj_immutable, false);
		argNames->size = numArgNames;
		SetObject(&method->argNames, argNames);

		proto = newPyrArray(NULL, numSlots, obj_permanent | obj_immutable, false);
		proto->size = numSlots;
		SetObject(&method->prototypeFrame, proto);

		// declare args
		slotRawSymbolArray(&method->argNames)->symbols[0] = s_this;
		if (mArglist) {
			PyrSymbol **methargs;
			methargs = slotRawSymbolArray(&method->argNames)->symbols;
			vardef = mArglist->mVarDefs;
			for (i=1; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
				PyrSlot *varslot;
				varslot = &vardef->mVarName->mSlot;
				// already declared as arg?
				for (j=0; j<i; ++j) {
					if (methargs[j] == slotRawSymbol(varslot)) {
						error("Argument '%s' already declared in %s:%s\n",
							slotRawSymbol(varslot)->name,
							slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
						nodePostErrorLine((PyrParseNode*)vardef);
						compileErrors++;
					}
				}
				// put it in arglist
				methargs[i] = slotRawSymbol(varslot);
				//postfl("defarg %d '%s'\n", i, slotRawSymbol(slot)->name);
				/*if (slotRawSymbol(varslot)->name[0] == 'a'
					&& slotRawSymbol(varslot)->name[1] == 'r'
					&& slotRawSymbol(varslot)->name[2] == 'g')
				{
					post("%d  %s:%s   '%s'\n", i,
						slotRawSymbol(&gCompilingClass->name)->name,
						slotRawSymbol(&gCompilingMethod->name)->name,
						slotRawSymbol(varslot)->name);
				}*/
			}
			if (funcVarArgs) {
				PyrSlot *varslot;
				varslot = &mArglist->mRest->mSlot;
				// already declared as arg?
				for (j=0; j<numArgs; ++j) {
					if (methargs[j] == slotRawSymbol(varslot)) {
						error("Argument '%s' already declared in %s:%s\n",
							slotRawSymbol(varslot)->name,
							slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
						nodePostErrorLine((PyrParseNode*)vardef);
						compileErrors++;
					}
				}
				// put it in arglist
				methargs[i] = slotRawSymbol(varslot);
				//postfl("defrest '%s'\n", slotRawSymbol(slot)->name);
			}
		}
		// fill prototype args
		if (NotNil(&method->prototypeFrame)) {
			SetNil(&slotRawObject(&method->prototypeFrame)->slots[0]);
		}
		if (mArglist) {
			vardef = mArglist->mVarDefs;
			for (i=1; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
				PyrSlot *slot, litval;
				slot = slotRawObject(&method->prototypeFrame)->slots + i;
				//compilePyrLiteralNode((PyrLiteralNode*)vardef->mDefVal, &litval);
				if (vardef->hasExpr(&litval)) hasVarExprs = true;
				*slot = litval;
			}
			if (funcVarArgs) {
				slotCopy(&slotRawObject(&method->prototypeFrame)->slots[numArgs], &o_emptyarray);
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
	if (mVarlist) {
		PyrSymbol **methargs, **methvars;
		methargs = slotRawSymbolArray(&method->argNames)->symbols;
		methvars = slotRawSymbolArray(&method->varNames)->symbols;
		vardef = mVarlist->mVarDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *varslot;
			varslot = &vardef->mVarName->mSlot;
			// already declared as arg?
			for (j=0; j<numArgNames; ++j) {
				if (methargs[j] == slotRawSymbol(varslot)) {
					error("Variable '%s' already declared in %s:%s\n",
						slotRawSymbol(varslot)->name,
						slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// already declared as var?
			for (j=0; j<i; ++j) {
				if (methvars[j] == slotRawSymbol(varslot)) {
					error("Variable '%s' already declared in %s:%s\n",
						slotRawSymbol(varslot)->name,
						slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in mVarlist
			methvars[i] = slotRawSymbol(varslot);
			//postfl("defvar %d '%s'\n", i, slotRawSymbol(slot)->name);
		}
	}

	if (mVarlist) {
		vardef = mVarlist->mVarDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *slot, litval;
			slot = slotRawObject(&method->prototypeFrame)->slots + i + numArgs + funcVarArgs;
			if (vardef->hasExpr(&litval)) hasVarExprs = true;
			//compilePyrLiteralNode(vardef->mDefVal, &litval);
			*slot = litval;
		}
	}

	methType = methNormal;
	if (hasVarExprs) {
		methType = methNormal;
	} else if (hasPrimitive) {
		methType = methPrimitive;
		/*
		if (getPrimitiveNumArgs(methraw->specialIndex) != numArgs) {
			post("warning: number of arguments for method %s:%s does not match primitive %s. %d vs %d\n",
				slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name,
				getPrimitiveName(methraw->specialIndex)->name,
				numArgs, getPrimitiveNumArgs(methraw->specialIndex));
		}
		*/
	} else if (slotRawSymbol(&gCompilingMethod->name) == s_doesNotUnderstand) {
		methType = methNormal;
	} else {
		int bodyType = mBody->mClassno;
		if (bodyType == pn_ReturnNode) {
			PyrReturnNode *rnode;
			PyrParseNode *xnode;
			int rtype;
			PyrSlot rslot;

			rnode = (PyrReturnNode*)mBody;
			xnode = (PyrParseNode*)rnode->mExpr;
			if (xnode) {
				rtype = xnode->mClassno;
				if (rtype == pn_PushLitNode) { // return literal ?
					compilePyrLiteralNode((PyrLiteralNode*)xnode, &rslot);
					if (IsObj(&rslot) && slotRawObject(&rslot)->classptr == class_fundef) {
						methType = methNormal;
					} else {
						methType = methReturnLiteral;
						method->selectors = rslot;
					}
				} else if (rtype == pn_PushNameNode) {
					PyrSlot *rslot;
					rslot = &((PyrPushNameNode*)xnode)->mSlot;
					if (slotRawSymbol(rslot) == s_this) {	// return this
						methType = methReturnSelf;
					} else {
						if (funcFindArg((PyrBlock*)method, slotRawSymbol(rslot), &index)) { // return arg ?
							// eliminate the case where its an ellipsis or keyword argument
							if (index < methraw->numargs) {
								methType = methReturnArg;
								methraw->specialIndex = index; // when you change sp to sp - 1
								//methraw->specialIndex = index - 1;
							}
						} else if (classFindInstVar(gCompilingClass, slotRawSymbol(rslot), &index)) {
							// return inst var
							methType = methReturnInstVar;
							methraw->specialIndex = index;
						}
					}
				} else if (rtype == pn_CallNode) {
					// need to do this for binary opcodes too..
					int specialIndex;
					PyrCallNode *cnode;
					PyrClass *specialClass = 0;
					cnode = (PyrCallNode*)xnode;
					methType = compareCallArgs(this, cnode, &specialIndex, &specialClass);
					if (methType != methNormal) {
						methraw->specialIndex = specialIndex;
						method->selectors = cnode->mSelector->mSlot;
						if (specialClass) method->constants = specialClass->name;
					}
				}
			} else {
				methType = methReturnSelf;
			}
		} else if (bodyType == pn_AssignNode && numArgs == 2) { // assign inst var ?
			PyrAssignNode *anode;
					//post("methAssignInstVar 1  %s:%s\n",
					//	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
			anode = (PyrAssignNode*)mBody;
			if (anode->mNext && anode->mNext->mClassno == pn_ReturnNode
				&& ((PyrReturnNode*)anode->mNext)->mExpr == NULL) {
					//post("methAssignInstVar 2  %s:%s\n",
					//	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
				if (classFindInstVar(gCompilingClass, slotRawSymbol(&anode->mVarName->mSlot), &index)) {
					methType = methAssignInstVar;
					methraw->specialIndex = index;
					//post("methAssignInstVar 3  %s:%s\n",
					//	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
				}
			}
		}
	}

	methraw->methType = methType;
	// set primitive
	// optimize common cases

	if (methType == methNormal || methType == methPrimitive) {
		PyrSlot dummy;
		PyrSymbol *name;

		// compile body
		initByteCodes();

		if (gCompilingClass == class_int) {
			// handle some special cases
			name = slotRawSymbol(&method->name);
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
			name = slotRawSymbol(&method->name);
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
		} else if (slotRawSymbol(&gCompilingClass->name) == s_dictionary) {
			name = slotRawSymbol(&method->name);
			if (name == getsym("keysValuesArrayDo")) {
				compileByte(143);
				compileByte(13);
				compileByte(143);
				compileByte(14);
			} else goto compile_body;
		} else if (gCompilingClass == class_number) {
			name = slotRawSymbol(&method->name);
			if (name == gSpecialSelectors[opmForSeries]) {
				compileByte(143);
				compileByte(29);
				compileByte(143);
				compileByte(30);
				compileByte(143);
				compileByte(31);
			} else goto compile_body;
		} else if (gCompilingClass == class_float) {
			// handle some special cases
			name = slotRawSymbol(&method->name);
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
			SetTailIsMethodReturn mr(false);
			if (mArglist) {
				vardef = mArglist->mVarDefs;
				for (i=1; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
					vardef->compileArg(&dummy);
				}
			}
			if (mVarlist) {
				vardef = mVarlist->mVarDefs;
				for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
					vardef->compile(&dummy);
				}
			}
			COMPILENODE(mBody, &dummy, true);
		}
		installByteCodes((PyrBlock*)method);
	}

	if (!oldmethod) {
		addMethod(gCompilingClass, method);
	}

	gCompilingMethod = NULL;
	gCompilingBlock = NULL;
	gPartiallyAppliedFunction = NULL;

	//postfl("<-method '%s'\n", slotRawSymbol(&mMethodName->mSlot)->name);
}

PyrArgListNode* newPyrArgListNode(PyrVarDefNode* varDefs, PyrSlotNode* rest)
{
	PyrArgListNode* node = ALLOCNODE(PyrArgListNode);
	node->mVarDefs = varDefs;
	node->mRest = rest;
	return node;
}

void PyrArgListNode::compile(PyrSlot *result)
{
	error("compilePyrArgListNode: shouldn't get here.\n");
	compileErrors++;
}


PyrVarListNode* newPyrVarListNode(PyrVarDefNode* vardefs, int flags)
{
	PyrVarListNode* node = ALLOCNODE(PyrVarListNode);
	node->mVarDefs = vardefs;
	node->mFlags = flags;
	return node;
}

void PyrVarListNode::compile(PyrSlot *result)
{
	error("compilePyrVarListNode: shouldn't get here.\n");
	compileErrors++;
}

PyrVarDefNode* newPyrVarDefNode(PyrSlotNode* varName, PyrParseNode* defVal,
	int flags)
{
	PyrVarDefNode* node = ALLOCNODE(PyrVarDefNode);
	node->mVarName = varName;
	node->mDefVal = defVal;
	node->mFlags = flags;
	node->mDrop = true;
	return node;
}

bool PyrVarDefNode::hasExpr(PyrSlot *result)
{
	if (result) SetNil(result);
	if (!mDefVal) return false;
	if (mDefVal->mClassno != pn_PushLitNode && mDefVal->mClassno != pn_LiteralNode) {
			//post("hasExpr A %s:%s %s %d\n", slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name, mVarName->slotRawSymbol(&mSlot)->name, mDefVal->mClassno);
			return true;
	}
	PyrPushLitNode *node = (PyrPushLitNode*)mDefVal;

	if (IsPtr(&node->mSlot)) {
		PyrParseNode* litnode = (PyrParseNode*)slotRawPtr(&node->mSlot);
		if (litnode) {
			if (litnode->mClassno == pn_BlockNode) {
				//post("hasExpr B %s:%s %s %d\n", slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name, mVarName->slotRawSymbol(&mSlot)->name, node->mClassno);
				return true;
			} else {
				if (result) node->compileLiteral(result);
			}
		}
	} else if (result) *result = node->mSlot;
	if (node->mParens) return true;
	return false;
}

void PyrVarDefNode::compile(PyrSlot *result)
{
	if (hasExpr(NULL)) {
		COMPILENODE(mDefVal, result, false);
		compileAssignVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
	}

	//error("compilePyrVarDefNode: shouldn't get here.\n");
	//compileErrors++;
}

void PyrVarDefNode::compileArg(PyrSlot *result)
{
	if (hasExpr(NULL)) {
		ByteCodes trueByteCodes;

		compilePushVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot));

		mDrop = false;
		trueByteCodes = compileBodyWithGoto(this, 0, true);
		int jumplen = byteCodeLength(trueByteCodes);

		compileByte(143); // special opcodes
		compileByte(26);
		compileByte((jumplen >> 8) & 0xFF);
		compileByte(jumplen & 0xFF);
		compileAndFreeByteCodes(trueByteCodes);
		compileOpcode(opSpecialOpcode, opcDrop); // drop the boolean
	}

	//error("compilePyrVarDefNode: shouldn't get here.\n");
	//compileErrors++;
}


PyrCallNode* newPyrCallNode(PyrSlotNode* selector, PyrParseNode* arglist,
	PyrParseNode* keyarglist, PyrParseNode* blocklist)
{
	PyrCallNode* node = ALLOCNODE(PyrCallNode);
	node->mSelector = selector;

	arglist = linkNextNode(arglist, blocklist);

	node->mArglist = arglist;
	node->mKeyarglist = keyarglist;
	return node;
}

int PyrCallNode::isPartialApplication()
{
	int sum = 0;
	PyrParseNode* argnode = mArglist;
	for (; argnode; argnode = argnode->mNext) {
		if (argnode->mClassno == pn_CurryArgNode) {
			((PyrCurryArgNode*)argnode)->mArgNum = sum;
			sum ++;
		}
	}

	PyrParseNode* keynode = mKeyarglist;
	for (; keynode; keynode = keynode->mNext) {
		if (keynode->mClassno == pn_CurryArgNode) {
			((PyrCurryArgNode*)keynode)->mArgNum = sum;
			sum ++;
		}
	}
	return sum;
}

void PyrCallNodeBase::compilePartialApplication(int numCurryArgs, PyrSlot *result)
{
	// create a function
	// compile the call

	ByteCodes savedBytes = saveByteCodeArray();

	int flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	PyrBlock* block = newPyrBlock(flags);

	PyrSlot blockSlot;
	SetObject(&blockSlot, block);

	int prevFunctionHighestExternalRef = gFunctionHighestExternalRef;
	bool prevFunctionCantBeClosed = gFunctionCantBeClosed;
	gFunctionHighestExternalRef = 0;
	gFunctionCantBeClosed = false;

	PyrClass* prevClass = gCompilingClass;
	PyrBlock* prevBlock = gCompilingBlock;
	gCompilingBlock = block;

	PyrBlock* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
	gPartiallyAppliedFunction = block;

	PyrMethodRaw* methraw = METHRAW(block);
	methraw->unused1 = 0;
	methraw->unused2 = 0;

	methraw->needsHeapContext = 0;

	SetObject(&block->contextDef, prevBlock);
////
	methraw->varargs = 0;

	methraw->frameSize = (numCurryArgs + FRAMESIZE) * sizeof(PyrSlot);
	PyrObject* proto = newPyrArray(compileGC(), numCurryArgs, flags, false);
	proto->size = numCurryArgs;
	SetObject(&block->prototypeFrame, proto);

	PyrSymbolArray* argNames = newPyrSymbolArray(compileGC(), numCurryArgs, flags, false);
	argNames->size = numCurryArgs;
	SetObject(&block->argNames, argNames);

	SetNil(&block->varNames);

	methraw->numargs = numCurryArgs;
	methraw->numvars = 0;
	methraw->posargs = numCurryArgs;
	methraw->numtemps = numCurryArgs;
	methraw->popSize = numCurryArgs;
	methraw->methType = methBlock;

	{
		PyrSymbol* s_empty = getsym("_");
		PyrSymbol **blockargs = slotRawSymbolArray(&block->argNames)->symbols;
		for (int i=0; i<numCurryArgs; ++i) {
			// put it in mArglist
			blockargs[i] = s_empty;
			SetNil(proto->slots + i);
		}
	}

	initByteCodes();
	{
		SetTailBranch branch(true);
		SetTailIsMethodReturn mr(false);
		PyrSlot body;
		compileCall(&body);
	}
	compileOpcode(opSpecialOpcode, opcFunctionReturn);
	installByteCodes(block);

	gCompilingBlock = prevBlock;
	gPartiallyAppliedFunction = prevPartiallyAppliedFunction;

	restoreByteCodeArray(savedBytes);
	int index = conjureLiteralSlotIndex(this, gCompilingBlock, &blockSlot);
	compileOpcode(opExtended, opPushLiteral);
	compileByte(index);

	if (!gFunctionCantBeClosed && gFunctionHighestExternalRef == 0) {
		SetNil(&block->contextDef);
	} else {
		METHRAW(prevBlock)->needsHeapContext = 1;
	}

	gCompilingBlock = prevBlock;
	gCompilingClass = prevClass;
	gPartiallyAppliedFunction = prevPartiallyAppliedFunction;
	gFunctionCantBeClosed = gFunctionCantBeClosed || prevFunctionCantBeClosed;
	gFunctionHighestExternalRef = sc_max(gFunctionHighestExternalRef - 1, prevFunctionHighestExternalRef);
}

void PyrCallNodeBase::compile(PyrSlot *result)
{

	int numCurryArgs = isPartialApplication();
	if (numCurryArgs) {
		compilePartialApplication(numCurryArgs, result);
	} else {
		compileCall(result);
	}
}

bool isSeries(PyrParseNode* node, PyrParseNode** args)
{
	if (node->mClassno != pn_CallNode) return false;
	PyrCallNode *callnode = (PyrCallNode*)node;
	if (slotRawSymbol(&callnode->mSelector->mSlot) != s_series) return false;
	if (callnode->mKeyarglist) return false;
	*args = callnode->mArglist;
	return true;
}

void PyrCallNode::compileCall(PyrSlot *result)
{
	int index, selType;
	PyrSlot dummy;
	bool varFound;
	PyrParseNode *argnode2;

	//postfl("compilePyrCallNode\n");
	PyrParseNode* argnode = mArglist;
	PyrParseNode* keynode = mKeyarglist;
	int numArgs = nodeListLength(argnode);
	int numKeyArgs = nodeListLength(keynode);
	int isSuper = isSuperObjNode(argnode);
	int numBlockArgs = METHRAW(gCompilingBlock)->numargs;

	slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
	index = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock,
		isSuper, slotRawSymbol(&mSelector->mSlot), &selType);

	if (numKeyArgs > 0 || (numArgs > 15 && !(selType == selSwitch || selType == selCase))) {
		for (; argnode; argnode = argnode->mNext) {
			COMPILENODE(argnode, &dummy, false);
		}
		for (; keynode; keynode = keynode->mNext) {
			COMPILENODE(keynode, &dummy, false);
		}
		if (isSuper) {
			compileTail();
			compileByte(opSendSuper);
			compileByte(numArgs + 2*numKeyArgs);
			compileByte(numKeyArgs);
			compileByte(index);
		} else {
			switch (selType) {
				case selNormal :
					compileTail();
					compileByte(opSendMsg);
					compileByte(numArgs + 2*numKeyArgs);
					compileByte(numKeyArgs);
					compileByte(index);
					break;
				case selSpecial :
					compileTail();
					compileByte(opSendSpecialMsg);
					compileByte(numArgs + 2*numKeyArgs);
					compileByte(numKeyArgs);
					compileByte(index);
					break;
				case selUnary :
				case selBinary :
					index = conjureLiteralSlotIndex((PyrParseNode*)mSelector,
gCompilingBlock, &mSelector->mSlot);
					// fall through
				default:
					compileTail();
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
			gFunctionCantBeClosed = true;
			compileTail();
			compileOpcode(opSendSuper, numArgs);
			compileByte(index);
		} else {
			for (; argnode; argnode = argnode->mNext) {
				COMPILENODE(argnode, &dummy, false);
			}
			compileTail();
			compileOpcode(opSendSuper, numArgs);
			compileByte(index);
		}
	} else {
		PyrSymbol *varname;
		if (argnode->mClassno == pn_PushNameNode) {
			varname = slotRawSymbol(&((PyrPushNameNode*)argnode)->mSlot);
		} else {
			varname = NULL;
		}
		if (varname == s_this) {
			gFunctionCantBeClosed = true;
		}
		switch (selType) {
			case selNormal :
				if (numArgs == 1 && varname == s_this) {
					compileTail();
					compileOpcode(opSendMsg, 0);
					compileByte(index);
				//} else if (numArgs>1 && numArgs == numBlockArgs) {
				} else if (numArgs>1 && numArgs == numBlockArgs) {
					// try for multiple push optimization
					int code;
					code = checkPushAllArgs(argnode, numArgs);
					if (code == push_Normal) goto normal;
					else if (code == push_AllArgs) {
						compileTail();
						compileByte(137); // push all args, send msg
						compileByte(index);
						//post("137 pushAllArgs     %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else if (code == push_AllButFirstArg) {
						COMPILENODE(argnode, &dummy, false);
						compileTail();
						compileByte(138); // push all but first arg, send msg
						compileByte(index);
						//post("138 pushAllButFirstArg     %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else goto normal;
				} else if (numArgs>2 && numArgs == numBlockArgs+1) {
					int code;
					code = checkPushAllButFirstTwoArgs(argnode, numBlockArgs);
					if (code == push_Normal) goto normal;
					else if (code == push_AllButFirstArg2) {
						COMPILENODE(argnode, &dummy, false);
						COMPILENODE(argnode->mNext, &dummy, false);
						compileTail();
						compileByte(141); // one arg pushed, push all but first arg, send msg
						compileByte(index);
						//post("141 pushAllButFirstArg2    %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else goto normal;

				} else {
					normal:
					for (; argnode; argnode = argnode->mNext) {
						COMPILENODE(argnode, &dummy, false);
					}
					compileTail();
					compileOpcode(opSendMsg, numArgs);
					compileByte(index);
				}
				break;
			case selSpecial :
				if (numArgs == 1) {
					if (varname == s_this) {
						compileTail();
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
							//post("136 pushInstVar(sp) %s:%s '%s' %d %d\n", slotRawSymbol(&gCompilingClass->name)->name,
							//	slotRawSymbol(&gCompilingMethod->name)->name, varname->name, varIndex, index);
							compileTail();
							compileByte(136);
							compileByte(varIndex);
							compileByte(index);
						} else goto special;
					} else goto special;
				} else if (index == opmDo && isSeries(argnode, &argnode)) {
					index = opmForSeries;
					mArglist = linkNextNode(argnode, mArglist->mNext);
					numArgs = nodeListLength(mArglist);
					goto special;
				} else if (numArgs>1 && numArgs == numBlockArgs) {
				//} else if (numArgs>1 && numArgs == numBlockArgs) {
					// try for multiple push optimization
					int code;
					code = checkPushAllArgs(argnode, numArgs);
					if (code == push_Normal) goto special;
					else if (code == push_AllArgs) {
						compileTail();
						compileByte(139); // push all args, send special msg
						compileByte(index);
						//post("139 pushAllArgs(sp) %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else if (code == push_AllButFirstArg) {
						COMPILENODE(argnode, &dummy, false);
						compileTail();
						compileByte(140); // push all but first arg, send special msg
						compileByte(index);
						//post("140 pushAllButFirstArg(sp) %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else goto special;
				} else if (numArgs>2 && numArgs == numBlockArgs+1) {
					int code;
					code = checkPushAllButFirstTwoArgs(argnode, numBlockArgs);
					if (code == push_Normal) goto special;
					else if (code == push_AllButFirstArg2) {
						COMPILENODE(argnode, &dummy, false);
						COMPILENODE(argnode->mNext, &dummy, false);
						compileTail();
						compileByte(142); // one arg pushed, push all but first arg, send msg
						compileByte(index);
						//post("142 pushAllButFirstArg2(sp)    %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
						//	slotRawSymbol(&gCompilingMethod->name)->name);
					} else goto special;
				} else {
					int i;
					special:
					for (i=0; argnode; argnode = argnode->mNext,i++) {
						COMPILENODE(argnode, &dummy, false);
					}
					compileTail();
					compileOpcode(opSendSpecialMsg, numArgs);
					compileByte(index);
				}
				break;
			case selUnary :
				if (numArgs != 1) {
					index = conjureLiteralSlotIndex((PyrParseNode*)mSelector, gCompilingBlock,
								&mSelector->mSlot);
					goto defaultCase;
				}
				for (; argnode; argnode = argnode->mNext) {
					COMPILENODE(argnode, &dummy, false);
				}
				compileTail();
				compileOpcode(opSendSpecialUnaryArithMsg, index);
				break;
			case selBinary :
				if (numArgs != 2) {
					index = conjureLiteralSlotIndex((PyrParseNode*)mSelector, gCompilingBlock,
								&mSelector->mSlot);
					goto defaultCase;
				}
				//for (; argnode; argnode = argnode->mNext) {
				//	COMPILENODE(argnode, &dummy, false);
				//}
				argnode2 = argnode->mNext;
				if (index == opAdd && argnode2->mClassno == pn_PushLitNode
					&& IsInt(&((PyrPushLitNode*)argnode2)->mSlot)
					&& slotRawInt(&((PyrPushLitNode*)argnode2)->mSlot) == 1) {
					COMPILENODE(argnode, &dummy, false);
					compileOpcode(opPushSpecialValue, opsvPlusOne);
				} else if (index == opSub && argnode2->mClassno == pn_PushLitNode
					&& IsInt(&((PyrPushLitNode*)argnode2)->mSlot)
					&& slotRawInt(&((PyrPushLitNode*)argnode2)->mSlot) == 1) {
					COMPILENODE(argnode, &dummy, false);
					compileOpcode(opPushSpecialValue, opsvMinusOne);
				} else {
					COMPILENODE(argnode, &dummy, false);
					COMPILENODE(argnode->mNext, &dummy, false);
					compileTail();
					compileOpcode(opSendSpecialBinaryArithMsg, index);
				}
				break;
			case selIf :
				compileAnyIfMsg(this);
				break;
			case selCase :
				compileCaseMsg(this);
				break;
			case selSwitch :
				compileSwitchMsg(this);
				break;
			case selWhile :
				compileWhileMsg(this);
				break;
			case selLoop :
				compileLoopMsg(this);
				break;
			case selAnd :
				if (numArgs == 2) compileAndMsg(argnode, argnode->mNext);
				else goto special;
				break;
			case selOr :
				if (numArgs == 2) compileOrMsg(argnode, argnode->mNext);
				else goto special;
				break;
			case selQuestionMark :
				if (numArgs == 2) compileQMsg(argnode, argnode->mNext);
				break;
			case selDoubleQuestionMark :
				if (numArgs == 2) compileQQMsg(argnode, argnode->mNext);
				break;
			case selExclamationQuestionMark :
				if (numArgs == 2) compileXQMsg(argnode, argnode->mNext);
				break;
			default :
				defaultCase:
				if (numArgs == 1 && varname == s_this) {
					compileTail();
					compileOpcode(opSendMsg, 0);
					compileByte(index);
				} else {
					for (; argnode; argnode = argnode->mNext) {
						COMPILENODE(argnode, &dummy, false);
					}
					compileTail();
					compileOpcode(opSendMsg, numArgs);
					compileByte(index);
				}
				break;
		}
	}
}

ByteCodes compileSubExpression(PyrPushLitNode* litnode, bool onTailBranch)
{
	return compileSubExpressionWithGoto(litnode, 0, onTailBranch);
}

ByteCodes compileSubExpressionWithGoto(PyrPushLitNode* litnode, int branchLen, bool onTailBranch)
{
	PyrBlockNode *bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
	return compileBodyWithGoto(bnode->mBody, branchLen, onTailBranch);
}

ByteCodes compileBodyWithGoto(PyrParseNode* body, int branchLen, bool onTailBranch)
{
	ByteCodes	currentByteCodes, subExprByteCodes;
	PyrSlot dummy;

	PyrBlock* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
	gPartiallyAppliedFunction = NULL;

	currentByteCodes = saveByteCodeArray();

	COMPILENODE(body, &dummy, onTailBranch);
	if (branchLen) {
		if (!byteCodeLength(gCompilingByteCodes)) {
			compileOpcode(opPushSpecialValue, opsvNil); // push nil
		}
		compileJump(opcJumpFwd, branchLen);
	}

	subExprByteCodes = getByteCodes();
	restoreByteCodeArray(currentByteCodes);

	gPartiallyAppliedFunction = prevPartiallyAppliedFunction;

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
	if (node->mClassno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		PyrBlockNode *bnode;
		anode = (PyrPushLitNode*)node;
		if (IsPtr(&anode->mSlot)
				&& (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
			if (bnode->mArglist || bnode->mVarlist) {
				if (SC_LanguageConfig::getPostInlineWarnings()) {
					post("WARNING: FunctionDef contains variable declarations and so"
					" will not be inlined.\n");
					if (bnode->mArglist)
						nodePostErrorLine((PyrParseNode*)bnode->mArglist);
					else
						nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
				}
			} else
				res = true;
		}
	}
	return res;
}

bool isAnInlineableAtomicLiteralBlock(PyrParseNode *node)
{
	bool res = false;
	if (node->mClassno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		PyrBlockNode *bnode;
		anode = (PyrPushLitNode*)node;
		if (IsPtr(&anode->mSlot)
				&& (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
			if (bnode->mArglist || bnode->mVarlist) {
				if (SC_LanguageConfig::getPostInlineWarnings()) {
					post("WARNING: FunctionDef contains variable declarations and so"
					" will not be inlined.\n");
					if (bnode->mArglist)
						nodePostErrorLine((PyrParseNode*)bnode->mArglist);
					else
						nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
				}
			} else {
				if (bnode->mBody->mClassno == pn_DropNode && ((PyrDropNode*)bnode->mBody)->mExpr2->mClassno == pn_BlockReturnNode)
					res = isAtomicLiteral(((PyrDropNode*)bnode->mBody)->mExpr1);
				else
					res = false;
			}
		}
	}
	return res;
}

bool isAtomicLiteral(PyrParseNode *node)
{
	bool res = false;
	if (node->mClassno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		anode = (PyrPushLitNode*)node;
		if (NotObj(&anode->mSlot) && !IsPtr(&anode->mSlot)) res = true;
	}
	return res;
}

bool isWhileTrue(PyrParseNode *node)
{
	bool res = false;
	if (node->mClassno == pn_PushLitNode) {
		PyrPushLitNode *anode;
		PyrBlockNode *bnode;
		anode = (PyrPushLitNode*)node;
		if (IsPtr(&anode->mSlot)
				&& (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
			if (bnode->mArglist || bnode->mVarlist) {
				/*
				post("WARNING: FunctionDef contains variable declarations and so"
					" will not be inlined.\n");
				if (bnode->mArglist) nodePostErrorLine((PyrParseNode*)bnode->mArglist);
				else nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
				*/
			} else {
				if (bnode->mBody->mClassno == pn_PushLitNode
						&& IsTrue(&((PyrPushLitNode*)bnode->mBody)->mSlot)) {
					res = true;
				}
			}
		} else if (IsTrue(&anode->mSlot)) {
			res = true;
		}
	}
	return res;
}

void compileAndMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	PyrSlot dummy;
	ByteCodes trueByteCodes;

	COMPILENODE(arg1, &dummy, false);
	if (isAnInlineableBlock(arg2)) {
		trueByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);

		compileJump(opcJumpIfFalsePushFalse, byteCodeLength(trueByteCodes));
		compileAndFreeByteCodes(trueByteCodes);
	} else {
		COMPILENODE(arg2, &dummy, false);
		compileTail();
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAnd);
	}
}

void compileOrMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	PyrSlot dummy;
	ByteCodes falseByteCodes;

	COMPILENODE(arg1, &dummy, false);
	if (isAnInlineableBlock(arg2)) {
		falseByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);

		compileJump(opcJumpIfTruePushTrue, byteCodeLength(falseByteCodes));
		compileAndFreeByteCodes(falseByteCodes);
	} else {
		COMPILENODE(arg2, &dummy, false);
		compileTail();
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmOr);
	}
}

void compileQMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	// question mark.
	PyrSlot dummy;

	COMPILENODE(arg1, &dummy, false);
	COMPILENODE(arg2, &dummy, false);
	compileByte(143); // special opcodes
	compileByte(22); // ??
}

void compileQQMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	// double question mark. ?? {|obj| ^if (this.notNil, this, func) }
	PyrSlot dummy;

	COMPILENODE(arg1, &dummy, false);
	if (isAnInlineableBlock(arg2)) {
		ByteCodes nilByteCodes;
		nilByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);

		int jumplen = byteCodeLength(nilByteCodes);
		compileByte(143); // special opcodes
		compileByte(23); // ??
		compileByte((jumplen >> 8) & 0xFF);
		compileByte(jumplen & 0xFF);
		compileAndFreeByteCodes(nilByteCodes);
	} else {
		COMPILENODE(arg2, &dummy, false);
		compileTail();
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmDoubleQuestionMark);
	}
}

void compileXQMsg(PyrParseNode* arg1, PyrParseNode* arg2)
{
	// double question mark. !? {|obj| ^if (this.isNil, this, func) }
	PyrSlot dummy;

	COMPILENODE(arg1, &dummy, false);
	if (isAnInlineableBlock(arg2)) {
		ByteCodes nilByteCodes;
		nilByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);

		int jumplen = byteCodeLength(nilByteCodes);
		compileByte(143); // special opcodes
		compileByte(27); // !?
		compileByte((jumplen >> 8) & 0xFF);
		compileByte(jumplen & 0xFF);
		compileAndFreeByteCodes(nilByteCodes);
	} else {
		COMPILENODE(arg2, &dummy, false);
		compileTail();
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmExclamationQuestionMark);
	}
}

void compileAnyIfMsg(PyrCallNodeBase2* node)
{
	PyrParseNode* arg1 = node->mArglist;

	if (arg1->mClassno == pn_CallNode) {
		PyrCallNode* callNode = (PyrCallNode*)arg1;
		int numCallArgs = nodeListLength(callNode->mArglist);
		int numCallKeyArgs = nodeListLength(callNode->mKeyarglist);
		if (numCallArgs == 1 && numCallKeyArgs == 0) {
			if (slotRawSymbol(&callNode->mSelector->mSlot) == gSpecialUnarySelectors[opIsNil]) {
				compileIfNilMsg(node, true);
				return;
			} else if (slotRawSymbol(&callNode->mSelector->mSlot) == gSpecialUnarySelectors[opNotNil]) {
				compileIfNilMsg(node, false);
				return;
			}
		}
	}
	compileIfMsg(node);
}

void compileIfMsg(PyrCallNodeBase2* node)
{
	PyrSlot dummy;
	ByteCodes trueByteCodes, falseByteCodes;

	int numArgs = nodeListLength(node->mArglist);
	PyrParseNode* arg1 = node->mArglist;
	PyrParseNode *arg2, *arg3;

	if (numArgs == 2) {
		arg2 = arg1->mNext;

		if (isAnInlineableBlock(arg2)) {
			COMPILENODE(arg1, &dummy, false);

			trueByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);
			if (byteCodeLength(trueByteCodes)) {
				compileJump(opcJumpIfFalsePushNil, byteCodeLength(trueByteCodes));
				compileAndFreeByteCodes(trueByteCodes);
			} else {
				compileOpcode(opSpecialOpcode, opcDrop); // drop the boolean
				compileOpcode(opPushSpecialValue, opsvNil); // push nil
			}
		} else goto unoptimized;
	} else if (numArgs == 3) {
		arg2 = arg1->mNext;
		arg3 = arg2->mNext;
		if (isAnInlineableBlock(arg2) && isAnInlineableBlock(arg3)) {
			COMPILENODE(arg1, &dummy, false);
			falseByteCodes = compileSubExpression((PyrPushLitNode*)arg3, true);
			trueByteCodes = compileSubExpressionWithGoto((PyrPushLitNode*)arg2, byteCodeLength(falseByteCodes), true);
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
		} else goto unoptimized;
	} else {
		unoptimized:
		for (; arg1; arg1 = arg1->mNext) {
			COMPILENODE(arg1, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmIf);
	}
}

void compileIfNilMsg(PyrCallNodeBase2* node, bool flag)
{
	PyrSlot dummy;
	ByteCodes trueByteCodes, falseByteCodes;
	PyrParseNode *arg2, *arg3;

	int numArgs = nodeListLength(node->mArglist);
	PyrParseNode* arg1 = node->mArglist;

	if (numArgs < 2) {
		COMPILENODE(arg1, &dummy, false);
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmIf);
	} else if (numArgs == 2) {
		arg2 = arg1->mNext;
		if (isAnInlineableBlock(arg2)) {
			PyrCallNode* callNode = (PyrCallNode*)arg1;
			COMPILENODE(callNode->mArglist, &dummy, false);

			trueByteCodes = compileSubExpression((PyrPushLitNode*)arg2, true);
			int jumplen = byteCodeLength(trueByteCodes);
			if (jumplen) {
				compileByte(143); // special opcodes
				compileByte(flag ? 26 : 27);
				compileByte((jumplen >> 8) & 0xFF);
				compileByte(jumplen & 0xFF);
				compileAndFreeByteCodes(trueByteCodes);
			} else {
				compileOpcode(opSpecialOpcode, opcDrop); // drop the value
				compileOpcode(opPushSpecialValue, opsvNil); // push nil
			}
		} else {
			COMPILENODE(arg1, &dummy, false);
			COMPILENODE(arg2, &dummy, false);
			compileTail();
			compileOpcode(opSendSpecialMsg, numArgs);
			compileByte(opmIf);
		}
	} else if (numArgs == 3) {
		arg2 = arg1->mNext;
		arg3 = arg2->mNext;
		if (isAnInlineableBlock(arg2) && isAnInlineableBlock(arg3)) {
			PyrCallNode* callNode = (PyrCallNode*)arg1;
			COMPILENODE(callNode->mArglist, &dummy, false);

			falseByteCodes = compileSubExpression((PyrPushLitNode*)arg3, true);
			int falseLen = byteCodeLength(falseByteCodes);
			trueByteCodes = compileSubExpressionWithGoto((PyrPushLitNode*)arg2, falseLen, true);
			int trueLen = byteCodeLength(trueByteCodes);
			if (falseLen) {
				compileByte(143); // special opcodes
				compileByte(flag ? 24 : 25);
				compileByte((trueLen >> 8) & 0xFF);
				compileByte(trueLen & 0xFF);
				compileAndFreeByteCodes(trueByteCodes);
				compileAndFreeByteCodes(falseByteCodes);
			} else if (trueLen) {
				compileByte(143); // special opcodes
				compileByte(flag ? 26 : 27);
				compileByte((trueLen >> 8) & 0xFF);
				compileByte(trueLen & 0xFF);
				compileAndFreeByteCodes(trueByteCodes);
			} else {
				compileOpcode(opSpecialOpcode, opcDrop); // drop the boolean
				compileOpcode(opPushSpecialValue, opsvNil); // push nil
			}
		} else {
			COMPILENODE(arg1, &dummy, false);
			COMPILENODE(arg2, &dummy, false);
			COMPILENODE(arg3, &dummy, false);
			compileTail();
			compileOpcode(opSendSpecialMsg, numArgs);
			compileByte(opmIf);
		}
	} else {
		for (; arg1; arg1 = arg1->mNext) {
			COMPILENODE(arg1, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmIf);
	}
}

PyrParseNode* reverseNodeList(PyrParseNode** list)
{
	PyrParseNode* temp1 = *list;
	PyrParseNode* temp2 = NULL;
	PyrParseNode* temp3 = NULL;
	while (temp1) {
		*list = temp1;
		temp2 = temp1->mNext;
		temp1->mNext = temp3;
		temp3 = temp1;
		temp1 = temp2;
	}
	return *list;
}


PyrCallNode* buildCase(PyrParseNode *arg1)
{
	// transform case statement into nested if statements.
	//int numArgs = nodeListLength(arg1);

	//post("->buildCase %d\n", numArgs);

	PyrParseNode *arg2 = arg1->mNext;

	PyrPushLitNode *litnode = (PyrPushLitNode*)arg1;
	PyrBlockNode *bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
	PyrParseNode *bbody = bnode->mBody;
	if (bbody->mClassno == pn_DropNode) {
		PyrDropNode* dropNode = (PyrDropNode*)bbody;
		if (dropNode->mExpr2->mClassno == pn_BlockReturnNode) {
			arg1 = dropNode->mExpr1;
		} else {
			arg1 = dropNode;
		}
	} else {
		arg1 = bbody;
	}
	arg1->mNext = arg2;

	PyrParseNode *arg3 = 0;
	if (arg2) {
		arg3 = arg2->mNext;
		if (arg3) {
			PyrParseNode *arg4 = arg3->mNext;
			if (arg4) {
				arg3 = buildCase(arg3);
				PyrBlockNode* bnode = newPyrBlockNode(NULL, NULL, arg3, false);
				arg3 = newPyrPushLitNode(NULL, bnode);
				arg2->mNext = arg3;
				arg3->mNext = NULL;
				arg1->mTail = arg3;

			}
		} else {
			arg1->mTail = arg2;
		}
	} else {
		arg1->mTail = arg1;
	}

	/*
	post("arg1->mNext %p arg2 %p\n", arg1->mNext, arg2);
	if (arg2) {
		post("arg2->mNext %p arg3 %p\n", arg2->mNext, arg3);
		post("isAnInlineableBlock arg2 %d\n", isAnInlineableBlock(arg2));
	}
	if (arg3) {
		post("isAnInlineableBlock arg3 %d\n", isAnInlineableBlock(arg3));
		post("arg3->mNext %p\n", arg3->mNext);
	}
	DUMPNODE(arg1, 0);
	*/

	PyrSlot selector;
	SetSymbol(&selector, gSpecialSelectors[opmIf]);
	PyrSlotNode* selectorNode = newPyrSlotNode(&selector);
	PyrCallNode *callNode = newPyrCallNode(selectorNode, arg1, NULL, NULL);

	//post("<-buildCase %d\n", numArgs);

	return callNode;
}

void compileCaseMsg(PyrCallNodeBase2* node)
{
	PyrParseNode *argnode = node->mArglist;
	bool canInline = true;
	for (; argnode; argnode = argnode->mNext) {
		if (!isAnInlineableBlock(argnode)) {
			canInline = false;
			break;
		}
	}
	PyrSlot dummy;
	if (canInline) {
		PyrCallNode* callNode = buildCase(node->mArglist);
		callNode->compile(&dummy);
	} else {
		int numArgs = 0;
		argnode = node->mArglist;
		for (; argnode; argnode = argnode->mNext, ++numArgs) {
			COMPILENODE(argnode, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmCase);
	}
}

void compileSwitchMsg(PyrCallNode* node)
{
	PyrSlot dummy;
	bool canInline = true;
	int numArgs;
	{
		PyrParseNode *argnode = node->mArglist;
		numArgs = nodeListLength(argnode);

		if (numArgs <= 2) {
			error("Missing argument in switch statement");
			nodePostErrorLine(node);
			compileErrors++;
		};

		argnode = argnode->mNext; // skip first arg.

		PyrParseNode* nextargnode = 0;
		for (; argnode; argnode = nextargnode) {
			nextargnode = argnode->mNext;
			if (nextargnode != NULL) {
				if (!isAtomicLiteral(argnode) && !isAnInlineableAtomicLiteralBlock(argnode)) {
					canInline = false;
					break;
				}
				if (!isAnInlineableBlock(nextargnode)) {
					canInline = false;
					break;
				}
				nextargnode = nextargnode->mNext;
			} else {
				if (!isAnInlineableBlock(argnode)) {
					canInline = false;
				}
				break;
			}
		}
	}

	if (canInline) {
		PyrParseNode *argnode = node->mArglist;

		int flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
		int arraySize = NEXTPOWEROFTWO(numArgs * 2);
		PyrObject* array = newPyrArray(compileGC(), arraySize, flags, false);
		array->size = arraySize;
		nilSlots(array->slots, arraySize);

		PyrSlot slot;
		SetObject(&slot, array);

		COMPILENODE(argnode, &dummy, false);
		compilePushConstant(node, &slot);

		compileByte(143); // lookup slot in dictionary and jump to offset.
		compileByte(28);

		argnode = argnode->mNext; // skip first arg.

		PyrParseNode* nextargnode = 0;
		int absoluteOffset = byteCodeLength(gCompilingByteCodes);
		int offset = 0;
		int lastOffset = 0;
		for (; argnode; argnode = nextargnode) {
			nextargnode = argnode->mNext;
			if (nextargnode != NULL) {
				ByteCodes byteCodes = compileSubExpressionWithGoto((PyrPushLitNode*)nextargnode, 0x6666, true);

				PyrSlot *key;
				PyrSlot value;
				SetInt(&value, offset);
				PyrPushLitNode* keyargnode = (PyrPushLitNode*)argnode;
				if (isAtomicLiteral(argnode)) {
					key = &keyargnode->mSlot;
				} else {
					PyrBlockNode *bnode = (PyrBlockNode*)slotRawPtr(&keyargnode->mSlot);
					PyrDropNode *dropnode = (PyrDropNode*)bnode->mBody;
					PyrPushLitNode* litnode = (PyrPushLitNode*)dropnode->mExpr1;
					key = &litnode->mSlot;
				}

				int index = arrayAtIdentityHashInPairs(array, key);
				PyrSlot *slot = array->slots + index;
				slotCopy(slot, key);
				SetInt(slot+1, offset);

				if (byteCodes) {
					offset += byteCodeLength(byteCodes);
					compileAndFreeByteCodes(byteCodes);
				} else {
					compileOpcode(opPushSpecialValue, opsvNil);
					offset += 1;
				}

				nextargnode = nextargnode->mNext;
				if (nextargnode == NULL) {
					compileOpcode(opPushSpecialValue, opsvNil);
					lastOffset = offset;
					offset += 1;
				}
			} else {
				ByteCodes byteCodes = compileSubExpressionWithGoto((PyrPushLitNode*)argnode, 0, true);

				lastOffset = offset;
				if (byteCodes) {
					offset += byteCodeLength(byteCodes);
					compileAndFreeByteCodes(byteCodes);
				} else {
					compileOpcode(opPushSpecialValue, opsvNil);
					lastOffset = offset;
					offset += 1;
				}
			}
		}

		Byte *bytes = gCompilingByteCodes->bytes + absoluteOffset;
		PyrSlot *slots = array->slots;
		{
			int jumplen = offset - lastOffset;
			bytes[lastOffset-2] = (jumplen >> 8) & 255;
			bytes[lastOffset-1] = jumplen & 255;
		}
		for (int i=0; i<arraySize; i+=2) {
			PyrSlot *key = slots + i;
			PyrSlot *value = key + 1;

			if (IsNil(value)) {
				SetInt(value, lastOffset);
			} else {
				int offsetToHere = slotRawInt(value);
				if (offsetToHere) {
					int jumplen = offset - offsetToHere;
					bytes[offsetToHere-2] = (jumplen >> 8) & 255;
					bytes[offsetToHere-1] = jumplen & 255;
				}
			}
		}

	} else {
		PyrParseNode *argnode = node->mArglist;
		for (; argnode; argnode = argnode->mNext) {
			COMPILENODE(argnode, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmSwitch);
	}
}

void compileWhileMsg(PyrCallNodeBase2* node)
{
	int numArgs;
	PyrParseNode *argnode;
	PyrSlot dummy;
	ByteCodes whileByteCodes, exprByteCodes;
	int whileByteCodeLen, exprByteCodeLen;

	numArgs = nodeListLength(node->mArglist);
	if (numArgs == 1 && isAnInlineableBlock(node->mArglist)) {

		whileByteCodes = compileSubExpression((PyrPushLitNode*)node->mArglist, false);

		whileByteCodeLen = byteCodeLength(whileByteCodes);
		compileAndFreeByteCodes(whileByteCodes);

		exprByteCodeLen = 1;
		compileJump(opcJumpIfFalsePushNil, exprByteCodeLen + 3);

		// opcJumpBak does a drop..
		compileOpcode(opPushSpecialValue, opsvNil);

		compileJump(opcJumpBak, exprByteCodeLen + whileByteCodeLen + 4);

	} else if (numArgs == 2 && isWhileTrue(node->mArglist)
		&& isAnInlineableBlock(node->mArglist->mNext)) {

		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->mArglist->mNext, false);

		exprByteCodeLen = byteCodeLength(exprByteCodes);
		compileAndFreeByteCodes(exprByteCodes);

		compileJump(opcJumpBak, exprByteCodeLen + 1);

	} else if (numArgs == 2 && isAnInlineableBlock(node->mArglist)
		&& isAnInlineableBlock(node->mArglist->mNext)) {

		whileByteCodes = compileSubExpression((PyrPushLitNode*)node->mArglist, false);
		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->mArglist->mNext, false);

		whileByteCodeLen = byteCodeLength(whileByteCodes);
		compileAndFreeByteCodes(whileByteCodes);

		if (exprByteCodes) {
			exprByteCodeLen = byteCodeLength(exprByteCodes);
			compileJump(opcJumpIfFalsePushNil, exprByteCodeLen + 3);
			compileAndFreeByteCodes(exprByteCodes);
		} else {
			exprByteCodeLen = 1;
			compileJump(opcJumpIfFalsePushNil, exprByteCodeLen + 3);
			// opcJumpBak does a drop..
			compileOpcode(opPushSpecialValue, opsvNil);
		}

		compileJump(opcJumpBak, exprByteCodeLen + whileByteCodeLen + 4);

	} else {
		argnode = node->mArglist;
		for (; argnode; argnode = argnode->mNext) {
			COMPILENODE(argnode, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmWhile);
	}
}

void compileLoopMsg(PyrCallNodeBase2* node)
{
	int numArgs;
	PyrParseNode *argnode;
	PyrSlot dummy;
	ByteCodes exprByteCodes;
	int exprByteCodeLen;

	numArgs = nodeListLength(node->mArglist);
	if (numArgs == 1 && isAnInlineableBlock(node->mArglist)) {

		exprByteCodes = compileSubExpression((PyrPushLitNode*)node->mArglist, false);

		exprByteCodeLen = byteCodeLength(exprByteCodes);
		compileAndFreeByteCodes(exprByteCodes);

		compileJump(opcJumpBak, exprByteCodeLen + 1);

	} else {
		argnode = node->mArglist;
		for (; argnode; argnode = argnode->mNext) {
			COMPILENODE(argnode, &dummy, false);
		}
		compileTail();
		compileOpcode(opSendSpecialMsg, numArgs);
		compileByte(opmLoop);
	}
}

PyrBinopCallNode* newPyrBinopCallNode(PyrSlotNode* selector,
	PyrParseNode* arg1, PyrParseNode* arg2, PyrParseNode* arg3)
{
	PyrBinopCallNode* node = ALLOCNODE(PyrBinopCallNode);
	node->mSelector = selector;
	node->mArglist = arg1;
	arg1->mNext = arg2;
	arg2->mNext = arg3;
	return node;
}

int PyrBinopCallNode::isPartialApplication()
{
	int sum = 0;
	PyrParseNode* argnode = mArglist;
	for (; argnode; argnode = argnode->mNext) {
		if (argnode->mClassno == pn_CurryArgNode) {
			((PyrCurryArgNode*)argnode)->mArgNum = sum;
			sum ++;
		}
	}
	return sum;
}

void PyrBinopCallNode::compileCall(PyrSlot *result)
{
	int index, selType, isSuper, numArgs;
	PyrSlot dummy;

	PyrParseNode *arg1 = mArglist;
	PyrParseNode *arg2 = arg1->mNext;
	PyrParseNode *arg3 = arg2->mNext;

	//postfl("compilePyrBinopCallNode\n");
	isSuper = isSuperObjNode(arg1);
	slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
	index = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock,
		isSuper, slotRawSymbol(&mSelector->mSlot), &selType);
	numArgs = arg3 ? 3 : 2;
	if (isSuper) {
		COMPILENODE(arg1, &dummy, false);
		COMPILENODE(arg2, &dummy, false);
		if (arg3) COMPILENODE(arg3, &dummy, false);
		compileTail();
		compileOpcode(opSendSuper, numArgs);
		compileByte(index);
	} else {
		switch (selType) {
			case selNormal :
				COMPILENODE(arg1, &dummy, false);
				COMPILENODE(arg2, &dummy, false);
				if (arg3) COMPILENODE(arg3, &dummy, false);
				compileTail();
				compileOpcode(opSendMsg, numArgs);
				compileByte(index);
				break;
			case selSpecial :
				COMPILENODE(arg1, &dummy, false);
				COMPILENODE(arg2, &dummy, false);
				if (arg3) COMPILENODE(arg3, &dummy, false);
				compileTail();
				compileOpcode(opSendSpecialMsg, numArgs);
				compileByte(index);
				break;
			case selUnary :
					COMPILENODE(arg1, &dummy, false);
					COMPILENODE(arg2, &dummy, false);
					if (arg3)
						COMPILENODE(arg3, &dummy, false);
					compileTail();
					if (arg3)
						compileOpcode(opSpecialOpcode, opcDrop); // drop third argument
					compileOpcode(opSpecialOpcode, opcDrop);     // drop second argument
					compileOpcode(opSendSpecialUnaryArithMsg, index);
				break;
			case selBinary :
				if (arg3) {
					COMPILENODE(arg1, &dummy, false);
					COMPILENODE(arg2, &dummy, false);
					COMPILENODE(arg3, &dummy, false);
					compileTail();
					compileOpcode(opSpecialOpcode, opcSpecialBinaryOpWithAdverb);
					compileByte(index);
				} else if (index == opAdd && arg2->mClassno == pn_PushLitNode
					&& IsInt(&((PyrPushLitNode*)arg2)->mSlot)
					&& slotRawInt(&((PyrPushLitNode*)arg2)->mSlot) == 1) {
					COMPILENODE(arg1, &dummy, false);
					compileOpcode(opPushSpecialValue, opsvPlusOne);
				} else if (index == opSub && arg2->mClassno == pn_PushLitNode
					&& IsInt(&((PyrPushLitNode*)arg2)->mSlot)
					&& slotRawInt(&((PyrPushLitNode*)arg2)->mSlot) == 1) {
					COMPILENODE(arg1, &dummy, false);
					compileTail();
					compileOpcode(opPushSpecialValue, opsvMinusOne);
				} else {
					COMPILENODE(arg1, &dummy, false);
					COMPILENODE(arg2, &dummy, false);
					compileTail();
					compileOpcode(opSendSpecialBinaryArithMsg, index);
				}
				break;
			case selIf :
				compileAnyIfMsg(this);
				break;
			case selCase :
				compileCaseMsg(this);
				break;
			case selWhile :
				compileWhileMsg(this);
				break;
			case selLoop :
				compileLoopMsg(this);
				break;
			case selAnd :
				compileAndMsg(arg1, arg2);
				break;
			case selOr :
				compileOrMsg(arg1, arg2);
				break;
			case selQuestionMark :
				compileQMsg(arg1, arg2);
				break;
			case selDoubleQuestionMark :
				compileQQMsg(arg1, arg2);
				break;
			case selExclamationQuestionMark :
				compileXQMsg(arg1, arg2);
				break;
			default :
				COMPILENODE(arg1, &dummy, false);
				COMPILENODE(arg2, &dummy, false);
				if (arg3) COMPILENODE(arg3, &dummy, false);
				compileTail();
				compileOpcode(opSendMsg, numArgs);
				compileByte(index);
				break;
		}
	}
}

PyrPushKeyArgNode* newPyrPushKeyArgNode(PyrSlotNode* selector, PyrParseNode* expr)
{
	PyrPushKeyArgNode* node = ALLOCNODE(PyrPushKeyArgNode);
	node->mSelector = selector;
	node->mExpr = expr;
	return node;
}

void PyrPushKeyArgNode::compile(PyrSlot *result)
{
	PyrSlot dummy;
	//postfl("->compilePyrPushKeyArgNode\n");

	compilePushConstant((PyrParseNode*)this, &mSelector->mSlot);

	COMPILENODE(mExpr, &dummy, false);
}

PyrDropNode* newPyrDropNode(PyrParseNode* expr1, PyrParseNode* expr2)
{
	PyrDropNode* node = ALLOCNODE(PyrDropNode);
	node->mExpr1 = expr1;
	node->mExpr2 = expr2;
	return node;
}

void PyrDropNode::compile(PyrSlot *result)
{
	//postfl("->compilePyrDropNode\n");
	PyrSlot dummy;
	// eliminate as many drops as possible
	if (!mExpr2) {
		post("DROP EXPR2 NULL\n");
		COMPILENODE(mExpr1, &dummy, true);
	} else if (mExpr2->mClassno == pn_BlockReturnNode) {
		// no drop before a block return
		COMPILENODE(mExpr1, &dummy, true);
	} else if (mExpr1 && mExpr1->mClassno == pn_AssignNode) {
		// let the store do the drop
		((PyrAssignNode*)mExpr1)->mDrop = 1;
		COMPILENODE(mExpr1, &dummy, false);
		COMPILENODE(mExpr2, &dummy, true);
	} else if (mExpr1 && mExpr1->mClassno == pn_DropNode) {
		PyrDropNode *znode;
		// let the store do the drop, a bit more complex.
		// find the ultimate expression in the left subtree before the drop.
		znode = (PyrDropNode*)mExpr1;
		while (znode->mExpr2 && znode->mExpr2->mClassno == pn_DropNode) {
			znode = (PyrDropNode*)znode->mExpr2;
		}
		if (znode->mExpr2->mClassno == pn_AssignNode) {
			((PyrAssignNode*)znode->mExpr2)->mDrop = 1;
			COMPILENODE(mExpr1, &dummy, false);
			COMPILENODE(mExpr2, &dummy, true);
		} else {
			COMPILENODE(mExpr1, &dummy, false);
			compileOpcode(opSpecialOpcode, opcDrop);
			COMPILENODE(mExpr2, &dummy, true);
		}
	} else {
		COMPILENODE(mExpr1, &dummy, false);
		compileOpcode(opSpecialOpcode, opcDrop);
		COMPILENODE(mExpr2, &dummy, true);
	}
	//postfl("<-compilePyrDropNode\n");
}

PyrPushLitNode* newPyrPushLitNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj)
{
	PyrPushLitNode* node;
	if (literalSlot) {
		node = literalSlot;
		node->mClassno = pn_PushLitNode;
	} else {
		node = ALLOCSLOTNODE(PyrSlotNode, pn_PushLitNode);
		SetPtr(&node->mSlot, (PyrObject*)literalObj);
	}
	return node;
}


void compilePushConstant(PyrParseNode* node, PyrSlot *slot)
{
	int index = conjureConstantIndex(node, gCompilingBlock, slot);
	if (index < (1<<4)) {
		compileByte((opPushLiteral << 4) | index);
	} else if (index < (1<<8)) {
		compileByte(40);
		compileByte(index & 0xFF);
	} else if (index < (1<<16)) {
		compileByte(41);
		compileByte((index >> 8) & 0xFF);
		compileByte(index & 0xFF);
	} else if (index < (1<<24)) {
		compileByte(42);
		compileByte((index >> 16) & 0xFF);
		compileByte((index >> 8) & 0xFF);
		compileByte(index & 0xFF);
	} else {
		compileByte(43);
		compileByte((index >> 24) & 0xFF);
		compileByte((index >> 16) & 0xFF);
		compileByte((index >> 8) & 0xFF);
		compileByte(index & 0xFF);
	}
}

void compilePushInt(int value)
{
	//postfl("compilePushInt\n");
	if (value >= -1 && value <= 2) {
		compileOpcode(opPushSpecialValue, opsvZero + value);
	} else{
		//printf("int %d\n", value);
		if (value >= -(1<<7) && value <= ((1<<7)-1)) {
			compileByte(44);
			compileByte(value & 0xFF);
		} else if (value >= -(1<<15) && value <= ((1<<15)-1)) {
			compileByte(45);
			compileByte((value >> 8) & 0xFF);
			compileByte(value & 0xFF);
		} else if (value >= -(1<<23) && value <= ((1<<23)-1)) {
			compileByte(46);
			compileByte((value >> 16) & 0xFF);
			compileByte((value >> 8) & 0xFF);
			compileByte(value & 0xFF);
		} else {
			compileByte(47);
			compileByte((value >> 24) & 0xFF);
			compileByte((value >> 16) & 0xFF);
			compileByte((value >> 8) & 0xFF);
			compileByte(value & 0xFF);
		}
	}
}

void PyrSlotNode::compilePushLit(PyrSlot *result)
{
	int index;
	PyrSlot slot;
	ByteCodes savedBytes;

	//postfl("compilePyrPushLitNode\n");
	if (IsPtr(&mSlot)) {
		PyrParseNode *literalObj = (PyrParseNode*)slotRawPtr(&mSlot);
		//index = conjureLiteralObjIndex(gCompilingBlock, literalObj);
		if (literalObj->mClassno == pn_BlockNode) {
			savedBytes = saveByteCodeArray();
			COMPILENODE(literalObj, &slot, false);
			restoreByteCodeArray(savedBytes);
			index = conjureLiteralSlotIndex(literalObj, gCompilingBlock, &slot);
			compileOpcode(opExtended, opPushLiteral);
			compileByte(index);

			PyrBlock *block = slotRawBlock(&slot);
			if (NotNil(&block->contextDef)) {
				METHRAW(gCompilingBlock)->needsHeapContext = 1;
			}
		} else {
			COMPILENODE(literalObj, &slot, false);
			compilePushConstant((PyrParseNode*)literalObj, &slot);
		}
	} else {
		slot = mSlot;
		if (IsInt(&slot)) {
			compilePushInt(slotRawInt(&slot));
		} else if (SlotEq(&slot, &o_nil)) {
			compileOpcode(opPushSpecialValue, opsvNil);
		} else if (SlotEq(&slot, &o_true)) {
			compileOpcode(opPushSpecialValue, opsvTrue);
		} else if (SlotEq(&slot, &o_false)) {
			compileOpcode(opPushSpecialValue, opsvFalse);
		} else if (SlotEq(&slot, &o_fhalf)) {
			compileOpcode(opPushSpecialValue, opsvFHalf);
		} else if (SlotEq(&slot, &o_fnegone)) {
			compileOpcode(opPushSpecialValue, opsvFNegOne);
		} else if (SlotEq(&slot, &o_fzero)) {
			compileOpcode(opPushSpecialValue, opsvFZero);
		} else if (SlotEq(&slot, &o_fone)) {
			compileOpcode(opPushSpecialValue, opsvFOne);
		} else if (SlotEq(&slot, &o_ftwo)) {
			compileOpcode(opPushSpecialValue, opsvFTwo);
		} else if (SlotEq(&slot, &o_inf)) {
			compileOpcode(opPushSpecialValue, opsvInf);
		} else if (IsFloat(&slot)) {
			compilePushConstant((PyrParseNode*)this, &slot);
		} else if (IsSym(&slot)) {
			compilePushConstant((PyrParseNode*)this, &slot);
		} else {
			compilePushConstant((PyrParseNode*)this, &slot);
		}
	}
}

PyrLiteralNode* newPyrLiteralNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj)
{
	PyrLiteralNode* node;
	if (literalSlot) {
		node = literalSlot;
		node->mClassno = pn_LiteralNode;
	} else {
		node = ALLOCSLOTNODE(PyrSlotNode, pn_LiteralNode);
		SetPtr(&node->mSlot, (PyrObject*)literalObj);
	}
	return node;
}

void compilePyrLiteralNode(PyrLiteralNode *node, PyrSlot *result)
{
	if (!node) {
		SetNil(result);
	} else {
		node->compileLiteral(result);
	}
}

void PyrSlotNode::compileLiteral(PyrSlot *result)
{
	ByteCodes savedBytes;

	if (IsPtr(&mSlot)) {
		PyrParseNode* literalObj = (PyrParseNode*)slotRawPtr(&mSlot);
		if (literalObj->mClassno == pn_BlockNode) {
			savedBytes = saveByteCodeArray();
			COMPILENODE(literalObj, result, false);
			restoreByteCodeArray(savedBytes);

			PyrBlock *block = slotRawBlock(result);
			if (NotNil(&block->contextDef)) {
				METHRAW(gCompilingBlock)->needsHeapContext = 1;
			}
		} else {
			COMPILENODE(literalObj, result, false);
		}
	} else {
		*(PyrSlot*)result = mSlot;
	}
}

PyrReturnNode* newPyrReturnNode(PyrParseNode* expr)
{
	PyrReturnNode* node = ALLOCNODE(PyrReturnNode);
	node->mExpr = expr;
	return node;
}



void PyrReturnNode::compile(PyrSlot *result)
{
	PyrPushLitNode *lit;
	PyrSlot dummy;

	//post("->compilePyrReturnNode\n");
	gFunctionCantBeClosed = true;
	if (!mExpr) {
		compileOpcode(opSpecialOpcode, opcReturnSelf);
	} else if (mExpr->mClassno == pn_PushLitNode) {
		lit = (PyrPushLitNode*)mExpr;
		if (IsSym(&(lit->mSlot)) && slotRawSymbol(&lit->mSlot) == s_this) {
			compileOpcode(opSpecialOpcode, opcReturnSelf);
		} else if (IsNil(&lit->mSlot)) {
			compileOpcode(opSpecialOpcode, opcReturnNil);
		} else if (IsTrue(&lit->mSlot)) {
			compileOpcode(opSpecialOpcode, opcReturnTrue);
		} else if (IsFalse(&lit->mSlot)) {
			compileOpcode(opSpecialOpcode, opcReturnFalse);
		} else {
			COMPILENODE(lit, &dummy, false);
			compileOpcode(opSpecialOpcode, opcReturn);
		}
	} else {
		SetTailBranch branch(true);
		SetTailIsMethodReturn mr(true);
		COMPILENODE(mExpr, &dummy, true);
		compileOpcode(opSpecialOpcode, opcReturn);
	}
	//post("<-compilePyrReturnNode\n");
}

PyrBlockReturnNode* newPyrBlockReturnNode()
{
	PyrBlockReturnNode* node = ALLOCNODE(PyrBlockReturnNode);
	return node;
}


void PyrBlockReturnNode::compile(PyrSlot *result)
{
	//postfl("compilePyrBlockReturnNode\n");
	//compileOpcode(opSpecialOpcode, opcFunctionReturn);
}

PyrAssignNode* newPyrAssignNode(PyrSlotNode* varName, PyrParseNode* expr, int flags)
{
	PyrAssignNode* node = ALLOCNODE(PyrAssignNode);
	node->mVarName = varName;
	node->mExpr = expr;
	node->mDrop = 0;
	return node;
}

PyrSetterNode* newPyrSetterNode(PyrSlotNode* selector, PyrParseNode* expr1, PyrParseNode* expr2)
{
	PyrSetterNode* node = ALLOCNODE(PyrSetterNode);
	node->mSelector = selector;
	node->mExpr1 = expr1;
	node->mExpr2 = expr2;
	return node;
}

PyrMultiAssignNode* newPyrMultiAssignNode(PyrMultiAssignVarListNode* varList,
	PyrParseNode* expr, int flags)
{
	PyrMultiAssignNode* node = ALLOCNODE(PyrMultiAssignNode);
	node->mVarList = varList;
	node->mExpr = expr;
	node->mDrop = 0;
	return node;
}

PyrMultiAssignVarListNode* newPyrMultiAssignVarListNode(PyrSlotNode* varNames,
	PyrSlotNode* rest)
{
	PyrMultiAssignVarListNode* node = ALLOCNODE(PyrMultiAssignVarListNode);
	node->mVarNames = varNames;
	node->mRest = rest;
	return node;
}

void compileAssignVar(PyrParseNode* node, PyrSymbol* varName, bool drop)
{
	int level, index, vindex, varType;
	PyrBlock *tempfunc;
	PyrClass *classobj;

	//postfl("compileAssignVar\n");
	classobj = gCompilingClass;
	if (varName == s_this || varName == s_super || varName == s_curProcess || varName == s_curThread || varName == s_curMethod ||
		varName == s_curBlock || varName == s_curClosure) {
		error("You may not assign to '%s'.", varName->name);
		nodePostErrorLine(node);
		compileErrors++;
	} else if (varName->name[0] >= 'A' && varName->name[0] <= 'Z') {
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
				index += slotRawInt(&classobj->classVarIndex);
				if (drop) {
					if (index < 4096) {
						compileByte((opStoreClassVar<<4) | ((index>>8) & 15));
						compileByte(index & 255);
					} else {
						compileByte(opStoreClassVar);
						assert(false);
						vindex = 0;
						compileByte(vindex); // FIXME: vindex is not initalized!!!!
						compileByte(index);
						compileByte((opSpecialOpcode<<4) | opcDrop);
					}
				} else {
					compileByte(opStoreClassVar);
					compileByte((index >> 8) & 255);
					compileByte(index & 255);
				}
			} break;
			case varConst : {
				error("You may not assign to a constant.");
				nodePostErrorLine(node);
				compileErrors++;
			} break;
			case varTemp :
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

void PyrAssignNode::compile(PyrSlot* result)
{
	PyrSlot dummy;

	//postfl("compilePyrAssignNode\n");
	COMPILENODE(mExpr, &dummy, false);
	compileAssignVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
}


int PyrSetterNode::isPartialApplication()
{
	int sum = 0;
	if (mExpr1->mClassno == pn_CurryArgNode) {
		((PyrCurryArgNode*)mExpr1)->mArgNum = sum;
		sum ++;
	}
	if (mExpr2->mClassno == pn_CurryArgNode) {
		((PyrCurryArgNode*)mExpr2)->mArgNum = sum;
		sum ++;
	}
	return sum;
}

void PyrSetterNode::compileCall(PyrSlot* result)
{
	int index, selType, isSuper;
	PyrSlot dummy;
	char setterName[128];
	PyrSymbol *setterSym;

	//postfl("compilePyrSetterNode\n");
	if (nodeListLength(mExpr1) > 1) {
		error("Setter method called with too many arguments.\n");
		nodePostErrorLine(mExpr1);
		compileErrors++;
	} else {
		COMPILENODE(mExpr1, &dummy, false);
		COMPILENODE(mExpr2, &dummy, false);


		//postfl("compilePyrCallNode\n");
		isSuper = isSuperObjNode(mExpr1);

		sprintf(setterName, "%s_", slotRawSymbol(&mSelector->mSlot)->name);
		setterSym = getsym(setterName);

		slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
		index = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock,
			isSuper, setterSym, &selType);
		if (isSuper) {
			compileTail();
			compileOpcode(opSendSuper, 2);
			compileByte(index);
		} else {
			compileTail();
			compileOpcode(opSendMsg, 2);
			compileByte(index);
		}
	}
}

void PyrMultiAssignNode::compile(PyrSlot* result)
{
	PyrSlot dummy;

	//postfl("compilePyrMultiAssignNode\n");
	COMPILENODE(mExpr, &dummy, false);
	COMPILENODE(mVarList, &dummy, false);
}

void PyrMultiAssignVarListNode::compile(PyrSlot* result)
{
	int i, numAssigns;
	PyrSlotNode *varname;

	//postfl("compilePyrMultiAssignVarListNode\n");
	numAssigns = nodeListLength((PyrParseNode*)mVarNames);
	varname = mVarNames;
	for (i=0; i<numAssigns; ++i, varname = (PyrSlotNode*)varname->mNext) {
		compileOpcode(opSpecialOpcode, opcDup);
		compilePushInt(i);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAt);
		compileAssignVar((PyrParseNode*)varname, slotRawSymbol(&varname->mSlot), 1);
		//compileOpcode(opSpecialOpcode, opcDrop);
	}
	if (mRest) {
		compileOpcode(opSpecialOpcode, opcDup);
		compilePushInt(i);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmCopyToEnd);
		compileAssignVar((PyrParseNode*)mRest, slotRawSymbol(&mRest->mSlot), 1);
		//compileOpcode(opSpecialOpcode, opcDrop);
	}
}


PyrDynDictNode* newPyrDynDictNode(PyrParseNode *elems)
{
	PyrDynDictNode* node;

	//if (compilingCmdLine) post("newPyrDynDictNode\n");
	node = ALLOCNODE(PyrDynDictNode);
	node->mElems = elems;
	return node;
}

int PyrDynDictNode::isPartialApplication()
{
	int sum = 0;
	int numItems = nodeListLength(mElems);
	PyrParseNode* inode = mElems;
	for (int i=0; i<numItems; ++i) {
		if (inode->mClassno == pn_CurryArgNode) {
			((PyrCurryArgNode*)inode)->mArgNum = sum;
			sum ++;
		}
		inode = (PyrParseNode*)inode->mNext;
	}
	return sum;
}

void PyrDynDictNode::compileCall(PyrSlot* result)
{
	int i, numItems;
	PyrParseNode *inode;
	PyrSlot dummy;

	//postfl("compilePyrDynDictNode\n");
	numItems = nodeListLength(mElems) >> 1;

	compilePushVar((PyrParseNode*)this, s_event);

	compilePushInt(numItems);
	compileByte(110); // push nil for proto
	compileByte(110); // push nil for parent
	compileByte(108); // push true for know
	compileOpcode(opSendSpecialMsg, 5);

	compileByte(opmNew);

	inode = mElems;
	for (i=0; i<numItems; ++i) {
            //if (compilingCmdLine) post("+ %d %d\n", i, gCompilingByteCodes->size);
            COMPILENODE(inode, &dummy, false);
            inode = (PyrParseNode*)inode->mNext;
            COMPILENODE(inode, &dummy, false);
            inode = (PyrParseNode*)inode->mNext;
            compileOpcode(opSendSpecialMsg, 3);
            compileByte(opmPut);
	}
}

PyrDynListNode* newPyrDynListNode(PyrParseNode *classname, PyrParseNode *elems)
{
	PyrDynListNode* node;

	//if (compilingCmdLine) post("newPyrDynListNode\n");
	node = ALLOCNODE(PyrDynListNode);
	node->mClassname = classname;
	node->mElems = elems;
	return node;
}

int PyrDynListNode::isPartialApplication()
{
	int sum = 0;
	int numItems = nodeListLength(mElems);
	PyrParseNode* inode = mElems;
	for (int i=0; i<numItems; ++i) {
		if (inode->mClassno == pn_CurryArgNode) {
			((PyrCurryArgNode*)inode)->mArgNum = sum;
			sum ++;
		}
		inode = (PyrParseNode*)inode->mNext;
	}
	return sum;
}

void PyrDynListNode::compileCall(PyrSlot* result)
{
	int i, numItems;
	PyrParseNode *inode;
	PyrSlot dummy;

	//postfl("compilePyrDynListNode\n");
	numItems = nodeListLength(mElems);

	if (mClassname) {
		compilePushVar((PyrParseNode*)this, slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot));
	} else {
		compilePushVar((PyrParseNode*)this, s_array);
	}

	//compileOpcode(opExtended, opPushSpecialValue);
	//compileByte(op_class_list);

	compilePushInt(numItems);

	compileOpcode(opSendSpecialMsg, 2);
	compileByte(opmNew);

	inode = mElems;
	for (i=0; i<numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
		//if (compilingCmdLine) post("+ %d %d\n", i, gCompilingByteCodes->size);
		COMPILENODE(inode, &dummy, false);
		compileOpcode(opSendSpecialMsg, 2);
		compileByte(opmAdd);
	}
}

PyrLitListNode* newPyrLitListNode(PyrParseNode *classname, PyrParseNode *elems)
{
	PyrLitListNode* node = ALLOCNODE(PyrLitListNode);
	node->mClassname = classname;
	node->mElems = elems;
	return node;
}

void PyrLitListNode::compile(PyrSlot* result)
{
	PyrSlot *resultSlot;
	PyrSlot itemSlot;
	PyrObject *array;
	PyrParseNode *inode;
	int i, numItems, flags;

	//postfl("->compilePyrLitListNode\n");
	if (mClassname && slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) != s_array) {
		error("Only Array is supported as literal type.\n");
		post("Compiling as an Array.\n");
	}
	resultSlot = (PyrSlot*)result;
	numItems = mElems ? nodeListLength(mElems) : 0;
	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	array = newPyrArray(compileGC(), numItems, flags, false);
	inode = mElems;
	for (i=0; i<numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
		COMPILENODE(inode, &itemSlot, false);
		array->slots[i] = itemSlot;
	}
	array->size = numItems;
	SetObject(resultSlot, array);
	//postfl("<-compilePyrLitListNode\n");
}


PyrLitDictNode* newPyrLitDictNode(PyrParseNode *elems)
{
	PyrLitDictNode* node = ALLOCNODE(PyrLitDictNode);
	node->mElems = elems;

	return node;
}

int litDictPut(PyrObject *dict, PyrSlot *key, PyrSlot *value);
int litDictPut(PyrObject *dict, PyrSlot *key, PyrSlot *value)
{
#if 0
	PyrSlot *slot, *newslot;
	int i, index, size;
	PyrObject *array;

	bool knows = IsTrue(dict->slots + ivxIdentDict_know);
	if (knows && IsSym(key)) {
		if (slotRawSymbol(key) == s_parent) {
			slotCopy(&dict->slots[ivxIdentDict_parent], value);
			return errNone;
		}
		if (slotRawSymbol(key) == s_proto) {
			slotCopy(&dict->slots[ivxIdentDict_proto], value);
			return errNone;
		}
	}
	array = slotRawObject(&dict->slots[ivxIdentDict_array]);
	if (!isKindOf((PyrObject*)array, class_array)) return errFailed;

	index = arrayAtIdentityHashInPairs(array, key);
	slot = array->slots + index;
	slotCopy(&slot[1], value);
	if (IsNil(slot)) {
		slotCopy(slot, key);
	}
#endif
	return errNone;
}


void PyrLitDictNode::dump(int level)
{
}

void PyrLitDictNode::compile(PyrSlot* result)
{
#if 0
	PyrSlot *resultSlot;
	PyrSlot itemSlot;
	PyrObject *array;
	PyrParseNode *inode;
	int i, numItems, flags;

	//postfl("->compilePyrLitDictNode\n");
	if (mClassname && slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) != s_array) {
		error("Only Array is supported as literal type.\n");
		post("Compiling as an Array.\n");
	}
	resultSlot = (PyrSlot*)result;
	numItems = mElems ? nodeListLength(mElems) : 0;
	int numSlots = NEXTPOWEROFTWO(numItems*2);

    PyrObject *obj = instantiateObject(g->gc, class_event->u.classobj, 0, true, false);
    PyrSlot *slots = obj->slots;

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	array = newPyrArray(compileGC(), numSlots, flags, false);
	nilSlots(array->slots, numSlots);
	inode = mElems;
	for (i=0; i<numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
		COMPILENODE(inode, &itemSlot, false);
		array->slots[i] = itemSlot;
	}
	array->size = numItems;
	SetObject(resultSlot, array);
	//postfl("<-compilePyrLitListNode\n");
#endif
}


extern LongStack closedFuncCharNo;
extern int lastClosedFuncCharNo;

PyrBlockNode* newPyrBlockNode(PyrArgListNode *arglist, PyrVarListNode *varlist, PyrParseNode *body, bool isTopLevel)
{
	PyrBlockNode* node = ALLOCNODE(PyrBlockNode);
	node->mArglist = arglist;
	catVarLists(varlist);
	node->mVarlist = varlist;
	node->mBody = body;
	node->mIsTopLevel = isTopLevel;

	node->mBeginCharNo = lastClosedFuncCharNo;

	return node;
}

void PyrBlockNode::compile(PyrSlot* slotResult)
{
	PyrBlock *block, *prevBlock;
	PyrMethodRaw *methraw;
	int i, j, numArgs, numVars, funcVarArgs;
	int numSlots, numArgNames, flags;
	PyrVarDefNode *vardef;
	PyrObject *proto;
	PyrSymbolArray *argNames, *varNames;
	PyrSlot dummy;
	bool hasVarExprs = false;

	//postfl("->block\n");

	// create a new block object

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	block = newPyrBlock(flags);
	SetObject(slotResult, block);

	int prevFunctionHighestExternalRef = gFunctionHighestExternalRef;
	bool prevFunctionCantBeClosed = gFunctionCantBeClosed;
	gFunctionHighestExternalRef = 0;
	gFunctionCantBeClosed = false;

	prevBlock = gCompilingBlock;
	PyrClass* prevClass = gCompilingClass;

	gCompilingBlock = block;
	PyrBlock* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
	gPartiallyAppliedFunction = NULL;

	methraw = METHRAW(block);
	methraw->unused1 = 0;
	methraw->unused2 = 0;

	int endCharNo = linestarts[mLineno] + mCharno;
	int stringLength = endCharNo - mBeginCharNo;
	int lastChar = text[mBeginCharNo + stringLength - 1];
	if (lastChar == 0) stringLength--;

	methraw->needsHeapContext = 0;
	if (mIsTopLevel) {
		gCompilingClass = class_interpreter;
		SetNil(&block->contextDef);
	} else {
		SetObject(&block->contextDef, prevBlock);
	}

	methraw->varargs = funcVarArgs = (mArglist && mArglist->mRest) ? 1 : 0;
	numArgs = mArglist ? nodeListLength((PyrParseNode*)mArglist->mVarDefs) : 0;
	numVars = mVarlist ? nodeListLength((PyrParseNode*)mVarlist->mVarDefs) : 0;

	if(numArgs > 255) {
		error("Too many arguments in function definition (> 255)\n");
		nodePostErrorLine((PyrParseNode*)mArglist->mVarDefs);
		compileErrors++;
	}

	if(numArgs > 255) {
		error("Too many arguments in function definition (> 255).\n");
		nodePostErrorLine((PyrParseNode*)mArglist->mVarDefs);
		compileErrors++;
	}

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
		blockargs = slotRawSymbolArray(&block->argNames)->symbols;
		vardef = mArglist->mVarDefs;
		for (i=0; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *varslot;
			varslot = &vardef->mVarName->mSlot;
			// already declared as arg?
			for (j=0; j<i; ++j) {
				if (blockargs[j] == slotRawSymbol(varslot)) {
					error("Function argument '%s' already declared in %s:%s\n",
						slotRawSymbol(varslot)->name,
						slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in mArglist
			blockargs[i] = slotRawSymbol(varslot);
			//postfl("defarg %d '%s'\n", i, slotRawSymbol(slot)->name);
		}
	}

	if (funcVarArgs) {
		PyrSlot *varslot;
		PyrSymbol **blockargs;
		blockargs = slotRawSymbolArray(&block->argNames)->symbols;
		varslot = &mArglist->mRest->mSlot;
		// already declared as arg?
		for (j=0; j<numArgs; ++j) {
			if (blockargs[j] == slotRawSymbol(varslot)) {
				error("Function argument '%s' already declared in %s:%s\n",
					slotRawSymbol(varslot)->name,
					slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
				nodePostErrorLine((PyrParseNode*)vardef);
				compileErrors++;
			}
		}
		// put it in mArglist
		blockargs[numArgs] = slotRawSymbol(varslot);
		//postfl("defrest '%s'\n", slotRawSymbol(slot)->name);
	}

	// declare vars
	if (numVars) {
		PyrSymbol **blockargs, **blockvars;
		blockargs = slotRawSymbolArray(&block->argNames)->symbols;
		blockvars = slotRawSymbolArray(&block->varNames)->symbols;
		vardef = mVarlist->mVarDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *varslot;
			varslot = &vardef->mVarName->mSlot;
			// already declared as arg?
			for (j=0; j<numArgNames; ++j) {
				if (blockargs[j] == slotRawSymbol(varslot)) {
					error("Function variable '%s' already declared in %s:%s\n",
						slotRawSymbol(varslot)->name,
						slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// already declared as var?
			for (j=0; j<i; ++j) {
				if (blockvars[j] == slotRawSymbol(varslot)) {
					error("Function variable '%s' already declared in %s:%s\n",
						slotRawSymbol(varslot)->name,
						slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
					nodePostErrorLine((PyrParseNode*)vardef);
					compileErrors++;
				}
			}
			// put it in varlist
			blockvars[i] = slotRawSymbol(varslot);
			//postfl("defvar %d '%s'\n", i, slotRawSymbol(slot)->name);
		}
	}

	if (numArgs) {
		vardef = mArglist->mVarDefs;
		for (i=0; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *slot, litval;
			slot = slotRawObject(&block->prototypeFrame)->slots + i;
			if (vardef->hasExpr(&litval)) hasVarExprs = true;
			//compilePyrLiteralNode((PyrLiteralNode*)vardef->mDefVal, &litval);
			*slot = litval;
		}
	}

	if (funcVarArgs) {
		//SetNil(&slotRawObject(&block->prototypeFrame)->slots[numArgs]);
		slotCopy(&slotRawObject(&block->prototypeFrame)->slots[numArgs], &o_emptyarray);
	}

	if (numVars) {
		vardef = mVarlist->mVarDefs;
		for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
			PyrSlot *slot, litval;
			slot = slotRawObject(&block->prototypeFrame)->slots + i + numArgs + funcVarArgs;
			if (vardef->hasExpr(&litval)) hasVarExprs = true;
			//compilePyrLiteralNode(vardef->mDefVal, &litval);
			*slot = litval;
		}
	}
	methraw->methType = methBlock;

	// compile body
	initByteCodes();
	{
		SetTailBranch branch(true);
		/*if (compilingCmdLine) {
			post("block %d\n", gIsTailCodeBranch);
			DUMPNODE(mBody, 0);
		}*/
		SetTailIsMethodReturn mr(false);
		if (hasVarExprs) {
			if (mArglist) {
				vardef = mArglist->mVarDefs;
				for (i=0; i<numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
					vardef->compileArg(&dummy);
				}
			}
			if (mVarlist) {
				vardef = mVarlist->mVarDefs;
				for (i=0; i<numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
					vardef->compile(&dummy);
				}
			}
		}
		if (mBody->mClassno == pn_BlockReturnNode) {
			compileOpcode(opPushSpecialValue, opsvNil);
		} else {
			COMPILENODE(mBody, &dummy, true);
		}
	}
	compileOpcode(opSpecialOpcode, opcFunctionReturn);
	installByteCodes(block);

	if ((!gFunctionCantBeClosed && gFunctionHighestExternalRef == 0) || mIsTopLevel) {
			SetNil(&block->contextDef);

			PyrString* string = newPyrStringN(compileGC(), stringLength, flags, false);
			memcpy(string->s, text+mBeginCharNo, stringLength);
			SetObject(&block->sourceCode, string);
			//static int totalLength = 0, totalStrings = 0;
			//totalLength += stringLength;
			//totalStrings++;
			//post("cf %4d %4d %6d %s:%s \n", totalStrings, stringLength, totalLength, slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
	}

	gCompilingBlock = prevBlock;
	gCompilingClass = prevClass;
	gPartiallyAppliedFunction = prevPartiallyAppliedFunction;
	gFunctionCantBeClosed = gFunctionCantBeClosed || prevFunctionCantBeClosed;
	gFunctionHighestExternalRef = sc_max(gFunctionHighestExternalRef - 1, prevFunctionHighestExternalRef);
}



PyrParseNode* linkNextNode(PyrParseNode* a, PyrParseNode* b)
{
	if (a == NULL) return b;
	if (b) {
		a->mTail->mNext = b;
		a->mTail = b->mTail;
	}
	return a;
}

PyrParseNode* linkAfterHead(PyrParseNode* a, PyrParseNode* b)
{
	b->mNext = a->mNext;
	if (!a->mNext) a->mTail = b;
	a->mNext = b;
	return a;
}

bool isSuperObjNode(PyrParseNode *node)
{
	return node->mClassno == pn_PushNameNode
		&& slotRawSymbol(&((PyrPushNameNode*)node)->mSlot) == s_super;
}

bool isThisObjNode(PyrParseNode *node)
{
	return node->mClassno == pn_PushNameNode
		&& slotRawSymbol(&((PyrPushNameNode*)node)->mSlot) == s_this;
}

int nodeListLength(PyrParseNode *node)
{
	int length = 0;
	for (; node; node=node->mNext) length++;
	return length;
}


int conjureSelectorIndex(PyrParseNode *node, PyrBlock* func,
	bool isSuper, PyrSymbol *selector, int *selType)
{
	int i;
	PyrObject *selectors;
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
		} else if (selector == gSpecialSelectors[opmCase]) {
			*selType = selCase;
			return opmCase;
		} else if (selector == gSpecialSelectors[opmSwitch]) {
			*selType = selSwitch;
			return opmSwitch;
		} else if (selector == gSpecialSelectors[opmLoop]) {
			*selType = selLoop;
			return opmLoop;
		} else if (selector == gSpecialSelectors[opmQuestionMark]) {
			*selType = selQuestionMark;
			return opmAnd;
		} else if (selector == gSpecialSelectors[opmDoubleQuestionMark]) {
			*selType = selDoubleQuestionMark;
			return opmAnd;
		} else if (selector == gSpecialSelectors[opmExclamationQuestionMark]) {
			*selType = selExclamationQuestionMark;
			return opmAnd;
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

	if (NotNil(&func->selectors)) {
		selectors = slotRawObject(&func->selectors);
		for (i=0; i<selectors->size; ++i) {
			if (IsSym(&selectors->slots[i]) && slotRawSymbol(&selectors->slots[i]) == selector) {
				*selType = selNormal;
				return i;
			}
		}
	} else {
		selectors = (PyrObject*)newPyrArray(compileGC(), 2, flags, false);
		SetObject(&func->selectors, selectors);
	}
	// otherwise add it to the selectors table

	if (selectors->size+1 >= 256) {
		error("Selector table too big: too many classes, method selectors or function definitions in this function. Simplify the function.\n");
		post("Next selector was: %s\n", selector->name);
		nodePostErrorLine(node);
		compileErrors++;
		return 0;
	}

	if (selectors->size+1 > ARRAYMAXINDEXSIZE(selectors)) {
		// resize literal table
		newsize = ARRAYMAXINDEXSIZE(selectors) * 2;
		SetRaw(&func->selectors, (PyrObject*)newPyrArray(compileGC(), newsize, flags, false));
		memcpy(slotRawObject(&func->selectors)->slots, selectors->slots, selectors->size * sizeof(PyrSlot));
		slotRawObject(&func->selectors)->size = selectors->size;
		freePyrObject(selectors);
		selectors = slotRawObject(&func->selectors);
	}
	slot = selectors->slots + selectors->size++;
	SetSymbol(slot, selector);

	*selType = selNormal;
	return selectors->size-1;
}

int conjureLiteralSlotIndex(PyrParseNode *node, PyrBlock* func, PyrSlot *slot)
{
	int i;
	PyrObject *selectors;
	PyrSlot *slot2;
	int newsize, flags;

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
	// lookup slot in selectors table

	if (IsObj(&func->selectors)) {
		selectors = slotRawObject(&func->selectors);
		/*if (selectors->classptr != class_array) {
			post("compiling %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
			post("selectors is a '%s'\n", selectors->classptr->name.us->name);
			dumpObjectSlot(slot);
			Debugger();
		}*/
		for (i=0; i<selectors->size; ++i)
			if (SlotEq(&selectors->slots[i], slot))
				return i;
	} else {
		selectors = (PyrObject*)newPyrArray(compileGC(), 4, flags, false);
		SetObject(&func->selectors, selectors);
	}
	// otherwise add it to the selectors table

	if (selectors->size+1 >= 256) {
		error("Selector table too big: too many classes, method selectors or function definitions in this function. Simplify the function.\n");
		post("Next literal was:\n");
		dumpPyrSlot(slot);
		nodePostErrorLine(node);
		compileErrors++;
		return 0;
	}
	if (selectors->size+1 > ARRAYMAXINDEXSIZE(selectors)) {
		// resize literal table
		newsize = ARRAYMAXINDEXSIZE(selectors) * 2;
		// resize literal table
		SetRaw(&func->selectors, (PyrObject*)newPyrArray(compileGC(), newsize, flags, false));
		memcpy(slotRawObject(&func->selectors)->slots, selectors->slots, selectors->size * sizeof(PyrSlot));
		slotRawObject(&func->selectors)->size = selectors->size;
		freePyrObject(selectors);
		selectors = slotRawObject(&func->selectors);
	}
	slot2 = selectors->slots + selectors->size++;
    slotCopy(slot2, slot);

	return selectors->size-1;
}


int conjureConstantIndex(PyrParseNode *node, PyrBlock* func, PyrSlot *slot)
{
	int i;
	PyrObject *constants;
	int newsize, flags;

	flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;

	// lookup slot in constants table
	if (IsObj(&func->constants)) {
		constants = slotRawObject(&func->constants);
		for (i=0; i<constants->size; ++i)
			if (SlotEq(&constants->slots[i], slot))
				return i;
	} else {
		constants = (PyrObject*)newPyrArray(compileGC(), 4, flags, false);
		SetObject(&func->constants, constants);
	}

	// otherwise add it to the constants table
	if (constants->size+1 > ARRAYMAXINDEXSIZE(constants)) {
		// resize literal table
		newsize = ARRAYMAXINDEXSIZE(constants) * 2;
		// resize literal table
		SetRaw(&func->constants, (PyrObject*)newPyrArray(compileGC(), newsize, flags, false));
		memcpy(slotRawObject(&func->constants)->slots, constants->slots, constants->size * sizeof(PyrSlot));
		slotRawObject(&func->constants)->size = constants->size;
		freePyrObject((PyrObject*)constants);
		constants = slotRawObject(&func->constants);
	}
	slotCopy(&constants->slots[constants->size++], slot);

	return constants->size-1;
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
	if (name == s_super) {
		gFunctionCantBeClosed = true;
		name = s_this;
	}
	if (name->name[0] >= 'A' && name->name[0] <= 'Z') return false;
	for (j=0; func; func = slotRawBlock(&func->contextDef), ++j) {
		methraw = METHRAW(func);
		numargs = methraw->posargs;
		for (i=0; i<numargs; ++i) {
			argname = slotRawSymbolArray(&func->argNames)->symbols[i];
			//postfl("    %d %d arg '%s' '%s'\n", j, i, argname->name, name->name);
			if (argname == name) {
				*level = j;
				*index = i;
				*varType = varTemp;
				if (tempfunc) *tempfunc = func;
				if (j > gFunctionHighestExternalRef) gFunctionHighestExternalRef = j;
				return true;
			}
		}
		for (i=0, k=numargs; i<methraw->numvars; ++i,++k) {
			varname = slotRawSymbolArray(&func->varNames)->symbols[i];
			//postfl("    %d %d %d var '%s' '%s'\n", j, i, k, varname->name, name->name);
			if (varname == name) {
				*level = j;
				*index = k;
				*varType = varTemp;
				if (tempfunc) *tempfunc = func;
				if (j > gFunctionHighestExternalRef) gFunctionHighestExternalRef = j;
				return true;
			}
		}
	}

	if (classFindInstVar(*classobj, name, index)) {
		*level = 0;
		*varType = varInst;
		if (gCompilingClass != class_interpreter) gFunctionCantBeClosed = true;
		return true;
	}
	if (classFindClassVar(classobj, name, index)) {
		*varType = varClass;
		if (gCompilingClass != class_interpreter) gFunctionCantBeClosed = true;
		return true;
	}
	if (classFindConst(classobj, name, index)) {
		*varType = varConst;
		//if (gCompilingClass != class_interpreter) gFunctionCantBeClosed = true;
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
			gSpecialClasses[op_class_event] = s_event;
			gSpecialClasses[op_class_wavetable] = s_wavetable;
			gSpecialClasses[op_class_env] = s_env;
			gSpecialClasses[op_class_routine] = s_routine;
			gSpecialClasses[op_class_color] = s_color;
			gSpecialClasses[op_class_rect] = s_rect;

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
	sel[opBitAnd] = getsym("bitAnd");
	sel[opBitOr] = getsym("bitOr");
	sel[opBitXor] = getsym("bitXor");
	sel[opLCM] = getsym("lcm");
	sel[opGCD] = getsym("gcd");
	sel[opRound] = getsym("round");
	sel[opRoundUp] = getsym("roundUp");
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
	sel[opFirstArg] = getsym("firstArg");
	sel[opRandRange] = getsym("rrand");
	sel[opExpRandRange] = getsym("exprand");


	sel = gSpecialSelectors;

	sel[opmNew] = getsym("new");
	sel[opmNewClear] = getsym("newClear");
	sel[opmNewCopyArgs] = getsym("newCopyArgs");
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
	sel[opmCase] = getsym("case");
	sel[opmSwitch] = getsym("switch");
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

	sel[opmPrimitiveFailed] = getsym("primitiveFailed");
	sel[opmSubclassResponsibility] = getsym("subclassResponsibility");
	sel[opmShouldNotImplement] = getsym("shouldNotImplement");
	sel[opmDoesNotUnderstand] = getsym("doesNotUnderstand");	// not really needed
	sel[opmNotYetImplemented] = getsym("notYetImplemented");

	sel[opmAtSign] = getsym("@");
	sel[opmWrapAtSign] = getsym("@@");
	sel[opmClipAtSign] = getsym("|@|");
	sel[opmFoldAtSign] = getsym("@|@");

	sel[opmMultiNew] = getsym("multiNew"); // UGens
	sel[opmMultiNewList] = getsym("multiNewList"); // UGens
	sel[opmAR] = getsym("ar"); // UGens
	sel[opmKR] = getsym("kr"); // UGens
	sel[opmIR] = getsym("ir"); // UGens

	sel[opmEnvirGet] = getsym("envirGet");
	sel[opmEnvirPut] = getsym("envirPut");

	sel[opmHalt] = getsym("halt");
	sel[opmForBy] = getsym("forBy");
	sel[opmForSeries] = getsym("forSeries");
	sel[opmReverseDo] = getsym("reverseDo");
	sel[opmLoop] = getsym("loop");
	sel[opmNonBooleanError] = getsym("mustBeBoolean");

	sel[opmCopy] = getsym("copy");
	sel[opmPerformList] = getsym("performList");
	sel[opmIsKindOf] = getsym("isKindOf");
	sel[opmPostln] = getsym("postln");
	sel[opmAsString] = getsym("asString");

	sel[opmPlusPlus] = getsym("++");
	sel[opmLTLT] = getsym("<<");
	sel[opmQuestionMark] = getsym("?");
	sel[opmDoubleQuestionMark] = getsym("??");
	sel[opmExclamationQuestionMark] = getsym("!?");

	sel[opmYield] = getsym("yield");
	sel[opmName] = getsym("name");
	sel[opmMulAdd] = getsym("madd");

	sel[opmSeries] = getsym("series");

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
