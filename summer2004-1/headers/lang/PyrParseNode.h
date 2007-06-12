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

#ifndef _PYRPARSENODE_H_
#define _PYRPARSENODE_H_

#include "PyrSlot.h"
#include "PyrKernel.h"
#include "ByteCodeArray.h"
#include "Opcodes.h"
#include "AdvancingAllocPool.h"

extern AdvancingAllocPool gParseNodePool;

#define ALLOCNODE(type)  (type*)gParseNodePool.Alloc(sizeof(type))
//#define FREENODE(node)	if (node) (*parseNodeClasses[(node)->classno]->deleteFunc)(node);
#define DUMPNODE(node, level)	if (node) (*parseNodeClasses[(node)->classno]->dumpFunc)((node),(level));
#define COMPILENODE(node, result)	(*parseNodeClasses[(node)->classno]->compileFunc)((node),(result));

typedef void (*PyrCompileNodeFunc)(void*, void*);
typedef void (*PyrDumpNodeFunc)(void*,int);

typedef struct pyrparsenodeclass {
	int type;
	PyrCompileNodeFunc compileFunc;
	PyrDumpNodeFunc dumpFunc;
} PyrParseNodeClass;


struct PyrParseNode {
	struct PyrParseNode *next;
	struct PyrParseNode *tail;
	short lineno;
	unsigned char charno, classno;
};


struct PyrSlotNode : public PyrParseNode {
	PyrSlot slot;
};

extern PyrParseNodeClass *pyrSlotNodeClass;

struct PyrPushNameNode : public PyrParseNode {
	PyrSlot varName;
} ;

extern PyrParseNodeClass *pyrPushNameNodeClass;

struct PyrClassExtNode : public PyrParseNode {
	struct PyrSlotNode* className;
	struct PyrMethodNode *methods;
} ;

extern PyrParseNodeClass *pyrClassExtNodeClass;

struct PyrClassNode : public PyrParseNode {
	struct PyrSlotNode* className;
	struct PyrSlotNode* superClassName;
	struct PyrSlotNode* indexType;
	struct PyrVarListNode *varlists;
	struct PyrMethodNode *methods;
	int varTally[3];
	int numsuperinstvars;
} ;

extern PyrParseNodeClass *pyrClassNodeClass;

struct PyrMethodNode : public PyrParseNode {
	struct PyrSlotNode* methodName;
	struct PyrSlotNode* primitiveName;
	struct PyrArgListNode *arglist;
	struct PyrVarListNode *varlist;
	struct PyrParseNode *body;
	int isClassMethod; // is class method?
	bool extension;
} ;

extern PyrParseNodeClass *pyrMethodNodeClass;

struct PyrVarListNode : public PyrParseNode {
	struct PyrVarDefNode *varDefs;
	int flags; 
} ;

extern PyrParseNodeClass *pyrVarListNodeClass;

struct PyrVarDefNode : public PyrParseNode {
	struct PyrSlotNode* varName;
	struct PyrLiteralNode* defVal;
	int flags;
} ;

extern PyrParseNodeClass *pyrVarDefNodeClass;

struct PyrCallNode : public PyrParseNode {
	struct PyrSlotNode* selector;
	struct PyrParseNode *arglist;
	struct PyrParseNode *keyarglist;
} ;

extern PyrParseNodeClass *pyrCallNodeClass;

struct PyrBinopCallNode : public PyrParseNode {
	struct PyrSlotNode* selector;
	struct PyrParseNode *arg1;
	struct PyrParseNode *arg2;
	struct PyrParseNode *arg3;
} ;

extern PyrParseNodeClass *pyrBinopCallNodeClass;

struct PyrDropNode : public PyrParseNode {
	struct PyrParseNode *expr1;
	struct PyrParseNode *expr2;
} ;

extern PyrParseNodeClass *pyrDropNodeClass;

struct PyrPushLitNode : public PyrParseNode {
	PyrSlot literalSlot;
} ;

extern PyrParseNodeClass *pyrPushLitNodeClass;

struct PyrPushKeyArgNode : public PyrParseNode {
	struct PyrSlotNode* selector;
	struct PyrParseNode *expr;
} ;

extern PyrParseNodeClass *pyrPushKeyArgNodeClass;

struct PyrLiteralNode : public PyrParseNode {
	PyrSlot literalSlot;
} ;

extern PyrParseNodeClass *pyrLiteralNodeClass;


struct PyrReturnNode : public PyrParseNode {
	struct PyrParseNode *expr; // if null, return self
} ;

extern PyrParseNodeClass *pyrReturnNodeClass;

struct PyrBlockReturnNode : public PyrParseNode {
	struct PyrParseNode *expr; // if null, return self
} ;

extern PyrParseNodeClass *pyrBlockReturnNodeClass;


struct PyrAssignNode : public PyrParseNode {
	struct PyrSlotNode* varName;
	struct PyrParseNode *expr;
	bool drop; // allow drop
} ;

extern PyrParseNodeClass *pyrAssignNodeClass;

struct PyrSetterNode : public PyrParseNode {
	struct PyrSlotNode* selector;
	struct PyrParseNode *expr1;
	struct PyrParseNode *expr2;
	int flags; // is a var def ?
} ;

extern PyrParseNodeClass *pyrSetterNodeClass;
	
struct PyrMultiAssignNode : public PyrParseNode {
	struct PyrMultiAssignVarListNode *varList;
	struct PyrParseNode *expr;
	bool drop; // allow drop
} ;

extern PyrParseNodeClass *pyrMultiAssignNodeClass;

struct PyrMultiAssignVarListNode : public PyrParseNode {
	struct PyrSlotNode *varNames;
	struct PyrSlotNode *rest;
} ;

extern PyrParseNodeClass *pyrMultiAssignVarListNodeClass;
	
struct PyrBlockNode : public PyrParseNode {
	struct PyrArgListNode *arglist;
	struct PyrVarListNode *varlist;
	struct PyrParseNode *body;
	bool isTopLevel;
	int beginCharNo;
} ;


extern PyrParseNodeClass *pyrBlockNodeClass;
	
struct PyrArgListNode : public PyrParseNode {
	struct PyrVarDefNode *varDefs;
	struct PyrSlotNode *rest;
} ;

extern PyrParseNodeClass *pyrArgListNodeClass;
	
struct PyrDynListNode : public PyrParseNode {
	struct PyrParseNode *classname;
	struct PyrParseNode *elems;
} ;

extern PyrParseNodeClass *pyrDynListNodeClass;
		
struct PyrDynDictNode : public PyrParseNode {
	struct PyrParseNode *elems;
} ;

extern PyrParseNodeClass *pyrDynDictNodeClass;
	
struct PyrLitListNode : public PyrParseNode {
	struct PyrParseNode *classname;
	struct PyrParseNode *elems;
} ;

extern PyrParseNodeClass *pyrLitListNodeClass;
	
extern PyrParseNode* gRootParseNode;
extern int gParserResult;

enum { rwPrivate=0, rwReadOnly=1, rwWriteOnly=2, rwReadWrite=3 };

enum { varInst, varClass, varTemp, varPseudo };

enum {
	/* structural units */
	pn_ClassNode,
	pn_ClassExtNode,
	pn_MethodNode,
	pn_BlockNode,
	pn_SlotNode,

	/* variable declarations */
	pn_VarListNode,
	pn_VarDefNode,
	pn_DynDictNode,
	pn_DynListNode,
	pn_LitListNode,
	
	pn_StaticVarListNode,
	pn_InstVarListNode,
	pn_PoolVarListNode,
	pn_ArgListNode,
	pn_SlotDefNode,
	
	/* selectors */
	pn_LiteralNode,
	
	/* code */
	pn_PushLitNode,
	pn_PushNameNode,
	pn_PushKeyArgNode,
	pn_CallNode,
	pn_BinopCallNode,
	pn_DropNode,
	pn_AssignNode,
	pn_MultiAssignNode,
	pn_MultiAssignVarListNode,
	pn_SetterNode,
	
	pn_ReturnNode,
	pn_BlockReturnNode,
	
	pn_NumTypes
};

extern char *parseNodeFormat[pn_NumTypes];
extern PyrParseNodeClass* parseNodeClasses[pn_NumTypes];


void initParseNodes();

PyrParseNodeClass* newParseNodeClass(int type, PyrCompileNodeFunc compileFunc, 
	PyrDumpNodeFunc dumpFunc);

PyrSlotNode* newPyrSlotNode(PyrSlot *slot);
PyrClassNode* newPyrClassNode(PyrSlotNode* className, PyrSlotNode* superClassName,
	PyrVarListNode* varlists, PyrMethodNode* methods, PyrSlotNode* indexType);
PyrClassExtNode* newPyrClassExtNode(PyrSlotNode* className, PyrMethodNode* methods);
PyrMethodNode* newPyrMethodNode(PyrSlotNode* methodName, PyrSlotNode* primitiveName,
	PyrArgListNode* arglist, PyrVarListNode *varlist, PyrParseNode* body, int isClassMethod);
PyrArgListNode* newPyrArgListNode(PyrVarDefNode* varDefs, PyrSlotNode* rest);
PyrVarListNode* newPyrVarListNode(PyrVarDefNode* vardefs, int flags);
PyrVarDefNode* newPyrVarDefNode(PyrSlotNode* varName, PyrLiteralNode* defVal, int flags);
PyrCallNode* newPyrCallNode(PyrSlotNode* selector, PyrParseNode* arglist,
	PyrParseNode* keyarglist, PyrParseNode* blocklist);
PyrBinopCallNode* newPyrBinopCallNode(PyrSlotNode* selector,
	PyrParseNode* arg1, PyrParseNode* arg2, PyrParseNode* arg3);
PyrDropNode* newPyrDropNode(PyrParseNode* expr1, PyrParseNode* expr2);
PyrPushKeyArgNode* newPyrPushKeyArgNode(PyrSlotNode* selector, PyrParseNode* expr);
PyrPushLitNode* newPyrPushLitNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj);
PyrLiteralNode* newPyrLiteralNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj);
PyrReturnNode* newPyrReturnNode(PyrParseNode* expr);
PyrBlockReturnNode* newPyrBlockReturnNode();
PyrAssignNode* newPyrAssignNode(PyrSlotNode* varName, PyrParseNode* expr, int flags);
PyrSetterNode* newPyrSetterNode(PyrSlotNode* varName, 
	PyrParseNode* expr1, PyrParseNode* expr2);
PyrMultiAssignNode* newPyrMultiAssignNode(PyrMultiAssignVarListNode* varList, 
	PyrParseNode* expr, int flags);
PyrPushNameNode* newPyrPushNameNode(PyrSlotNode *slotNode);
PyrDynDictNode* newPyrDynDictNode(PyrParseNode *elems);
PyrDynListNode* newPyrDynListNode(PyrParseNode *classname, PyrParseNode *elems);
PyrLitListNode* newPyrLitListNode(PyrParseNode *classname, PyrParseNode *elems);
PyrMultiAssignVarListNode* newPyrMultiAssignVarListNode(PyrSlotNode* varNames, 
	PyrSlotNode* rest);
PyrBlockNode* newPyrBlockNode(PyrArgListNode *arglist, PyrVarListNode *varlist, PyrParseNode *body, bool isTopLevel);

void compilePyrSlotNode(PyrSlotNode* node, void *result);
void compilePyrClassNode(PyrClassNode* node, void *result);
void compilePyrClassExtNode(PyrClassExtNode* node, void *result);
void compilePyrMethodNode(PyrMethodNode* node, void *result);
void compilePyrArgListNode(PyrArgListNode* node, void *result);
void compilePyrVarListNode(PyrVarListNode* node, void *result);
void compilePyrVarDefNode(PyrVarDefNode* node, void *result);
void compilePyrCallNode(PyrCallNode* node, void *result);
void compilePyrBinopCallNode(PyrBinopCallNode* node, void *result);
void compilePyrPushLitNode(PyrPushLitNode* node, void *result);
void compilePyrLiteralNode(PyrLiteralNode* node, void *result);
void compilePyrReturnNode(PyrReturnNode* node, void *result);
void compilePyrBlockReturnNode(PyrBlockReturnNode* node, void *result);
void compilePyrAssignNode(PyrAssignNode* node, void *result);
void compilePyrSetterNode(PyrSetterNode* node, void* result);
void compilePyrMultiAssignNode(PyrMultiAssignNode* node, void *result);
void compilePyrMultiAssignVarListNode(PyrMultiAssignVarListNode* node, void *result);
void compilePyrDynDictNode(PyrDynDictNode* node, void *result);
void compilePyrDynListNode(PyrDynListNode* node, void *result);
void compilePyrLitListNode(PyrLitListNode* node, void *result);
void compilePyrBlockNode(PyrBlockNode* node, void *result);
void compilePyrPushNameNode(PyrPushNameNode* node, void *result);
void compilePyrDropNode(PyrDropNode* node, void *result);
void compilePyrPushKeyArgNode(PyrPushKeyArgNode* node, void *result);

void dumpPyrSlotNode(PyrSlotNode* node, int level);
void dumpPyrClassNode(PyrClassNode* node, int level);
void dumpPyrClassExtNode(PyrClassExtNode* node, int level);
void dumpPyrMethodNode(PyrMethodNode* node, int level);
void dumpPyrArgListNode(PyrArgListNode* node, int level);
void dumpPyrVarListNode(PyrVarListNode* node, int level);
void dumpPyrVarDefNode(PyrVarDefNode* node, int level);
void dumpPyrCallNode(PyrCallNode* node, int level);
void dumpPyrBinopCallNode(PyrBinopCallNode* node, int level);
void dumpPyrPushLitNode(PyrPushLitNode* node, int level);
void dumpPyrLiteralNode(PyrLiteralNode* node, int level);
void dumpPyrReturnNode(PyrReturnNode* node, int level);
void dumpPyrBlockReturnNode(PyrBlockReturnNode* node, int level);
void dumpPyrAssignNode(PyrAssignNode* node, int level);
void dumpPyrSetterNode(PyrSetterNode* node, int level);
void dumpPyrMultiAssignNode(PyrMultiAssignNode* node, int level);
void dumpPyrMultiAssignVarListNode(PyrMultiAssignVarListNode* node, int level);
void dumpPyrDynDictNode(PyrDynDictNode* node, int level);
void dumpPyrDynListNode(PyrDynListNode* node, int level);
void dumpPyrLitListNode(PyrLitListNode* node, int level);
void dumpPyrBlockNode(PyrBlockNode* node, int level);
void dumpPyrPushNameNode(PyrPushNameNode* node, int level);
void dumpPyrPushKeyArgNode(PyrPushKeyArgNode* node, int level);
void dumpPyrDropNode(PyrDropNode* node, int level);

PyrClass* getNodeSuperclass(PyrClassNode *node);
void countNodeMethods(PyrClassNode* node, int *numClassMethods, int *numInstMethods);
void compileExtNodeMethods(PyrClassExtNode* node);
void countVarDefs(PyrClassNode* node);
bool compareVarDefs(PyrClassNode* node, PyrClass* classobj);
void recompileSubclasses(PyrClass* classobj);
void compileNodeMethods(PyrClassNode* node);
void fillClassPrototypes(PyrClassNode *node, PyrClass *classobj, PyrClass *superclassobj);

int nodeListLength(PyrParseNode *node);
bool isSuperObjNode(PyrParseNode *node);
bool isThisObjNode(PyrParseNode *node);
int conjureSelectorIndex(PyrParseNode *node, PyrBlock* func, 
		bool isSuper, PyrSymbol *selector, int *selType);
int conjureLiteralSlotIndex(PyrParseNode *node, PyrBlock* func, PyrSlot *slot);
bool findVarName(PyrBlock* func, PyrClass **classobj, PyrSymbol *name, 
	int *varType, int *level, int *index, PyrBlock** tempfunc);
void countClassVarDefs(PyrClassNode* node, int *numClassMethods, int *numInstMethods);
void compileNodeList(PyrParseNode *node);
void dumpNodeList(PyrParseNode *node);
int compareCallArgs(PyrMethodNode* node, PyrCallNode *cnode, int *varIndex);

bool findSpecialClassName(PyrSymbol *className, int *index);
int getIndexType(PyrClassNode *classnode);

void compileIfMsg(PyrCallNode* node);
void compileWhileMsg(PyrCallNode* node);
void compileLoopMsg(PyrCallNode* node);
void compileAndMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileOrMsg(PyrParseNode* arg1, PyrParseNode* arg2);

void compilePushInt(int value);
void compileAssignVar(PyrParseNode *node, PyrSymbol* varName, bool drop);
void compilePushVar(PyrParseNode *node, PyrSymbol *varName);
bool isAnInlineableBlock(PyrParseNode *node);
bool isWhileTrue(PyrParseNode *node);
void installByteCodes(PyrBlock *block);

ByteCodes compileSubExpression(PyrPushLitNode* litnode);
ByteCodes compileSubExpressionWithGoto(PyrPushLitNode* litnode, int branchLen);
//ByteCodes compileDefaultValue(int litIndex, int realExprLen);

void initParser();
void finiParser();
void initParserPool();
void freeParserPool();

void initSpecialSelectors();
void initSpecialClasses();

void nodePostErrorLine(PyrParseNode* node);

PyrParseNode* linkNextNode(PyrParseNode* a, PyrParseNode* b);
PyrParseNode* linkAfterHead(PyrParseNode* a, PyrParseNode* b);

extern int compileErrors;

extern long zzval;
extern PyrSymbol *ps_newlist;
extern PyrSymbol *gSpecialUnarySelectors[opNumUnarySelectors];
extern PyrSymbol *gSpecialBinarySelectors[opNumBinarySelectors];
extern PyrSymbol *gSpecialSelectors[opmNumSpecialSelectors];
extern PyrSymbol* gSpecialClasses[op_NumSpecialClasses];

extern PyrClass *gCurrentClass;
extern PyrClass *gCurrentMetaClass;
extern PyrClass *gCompilingClass;
extern PyrMethod *gCompilingMethod;
extern PyrBlock *gCompilingBlock;

/* 
	compiling
	"inlining" of special arithmetic opcodes.
	inlining of IF, WHILE, AND, OR
*/

#endif
