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

#pragma once

#include "PyrSlot.h"
#include "PyrKernel.h"
#include "ByteCodeArray.h"
#include "Opcodes.h"
#include "AdvancingAllocPool.h"
#include "SpecialSelectorsOperatorsAndClasses.h"


enum { rwPrivate = 0, rwReadOnly = 1, rwWriteOnly = 2, rwReadWrite = 3 };

enum { varInst, varClass, varTemp, varConst, varPseudo, varLocal };

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
    pn_LitDictNode,

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
    pn_CurryArgNode,

    pn_ReturnNode,
    pn_BlockReturnNode,

    pn_NumTypes
};

extern AdvancingAllocPool gParseNodePool;

// This value count the un-inlined functions, these are not desirable in the class library because they are slow.
// There is a primitive that returns this value so it can be checked in sclang's unit tests.
extern int gNumUninlinedFunctions;

#define ALLOCNODE(type) (new (gParseNodePool.Alloc(sizeof(type))) type())
#define ALLOCSLOTNODE(type, classno) (new (gParseNodePool.Alloc(sizeof(type))) type(classno))
#define COMPILENODE(node, result, onTailBranch) (compileNode((node), (result), (onTailBranch)))
#define DUMPNODE(node, level)                                                                                          \
    do {                                                                                                               \
        if (node)                                                                                                      \
            (node)->dump(level);                                                                                       \
    } while (false);

struct PyrParseNode {
    PyrParseNode(int classno);
    virtual ~PyrParseNode() {}
    virtual void compile(PyrSlot* result) = 0;
    virtual void dump(int level) = 0;

    struct PyrParseNode* mNext;
    struct PyrParseNode* mTail;
    int mLineno;
    int mCharno;
    unsigned char mClassno;
    unsigned char mParens;
};

struct PyrSlotNode : public PyrParseNode {
    PyrSlotNode(): PyrParseNode(pn_SlotNode) {}
    PyrSlotNode(int classno): PyrParseNode(classno) {}
    virtual ~PyrSlotNode() {}

    virtual void compile(PyrSlot* result);
    virtual void compileLiteral(PyrSlot* result);
    virtual void compilePushLit(PyrSlot* result);
    virtual void dump(int level);
    virtual void dumpLiteral(int level);
    virtual void dumpPushLit(int level);

    PyrSlot mSlot;
};

typedef PyrSlotNode PyrLiteralNode;
typedef PyrSlotNode PyrPushLitNode;
typedef PyrSlotNode PyrPushNameNode;

struct PyrCurryArgNode : public PyrParseNode {
    PyrCurryArgNode(): PyrParseNode(pn_CurryArgNode), mArgNum(-1) {}
    virtual ~PyrCurryArgNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    int mArgNum;
};


struct PyrClassExtNode : public PyrParseNode {
    PyrClassExtNode(): PyrParseNode(pn_ClassExtNode) {}
    virtual ~PyrClassExtNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mClassName;
    struct PyrMethodNode* mMethods;
};

struct PyrClassNode : public PyrParseNode {
    PyrClassNode(): PyrParseNode(pn_ClassNode) {}
    virtual ~PyrClassNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mClassName;
    struct PyrSlotNode* mSuperClassName;
    struct PyrSlotNode* mIndexType;
    struct PyrVarListNode* mVarlists;
    struct PyrMethodNode* mMethods;
    int mVarTally[4];
    int mNumSuperInstVars;
};

struct PyrMethodNode : public PyrParseNode {
    PyrMethodNode(): PyrParseNode(pn_MethodNode) {}
    virtual ~PyrMethodNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mMethodName;
    struct PyrSlotNode* mPrimitiveName;
    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    struct PyrParseNode* mBody;
    int mIsClassMethod; // is class method?
    bool mExtension;
};

struct PyrVarListNode : public PyrParseNode {
    PyrVarListNode(): PyrParseNode(pn_VarListNode) {}
    virtual ~PyrVarListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrVarDefNode* mVarDefs;
    int mFlags;
};

struct PyrVarDefNode : public PyrParseNode {
    PyrVarDefNode(): PyrParseNode(pn_VarDefNode) {}
    virtual ~PyrVarDefNode() {}
    virtual void compile(PyrSlot* result);
    virtual void compileArg(PyrSlot* result);
    virtual void dump(int level);
    bool hasExpr(PyrSlot* result);

    struct PyrSlotNode* mVarName;
    PyrParseNode* mDefVal;
    int mFlags;
    bool mDrop;
};

struct PyrCallNodeBase : public PyrParseNode {
    PyrCallNodeBase(int classno): PyrParseNode(classno) {}
    virtual ~PyrCallNodeBase() {}

    virtual void compile(PyrSlot* result);
    virtual void compilePartialApplication(int numCurryArgs, PyrSlot* result);
    virtual void compileCall(PyrSlot* result) = 0;

    virtual int isPartialApplication() = 0;
};

struct PyrCallNodeBase2 : public PyrCallNodeBase {
    PyrCallNodeBase2(int classno): PyrCallNodeBase(classno) {}
    virtual ~PyrCallNodeBase2() {}

    struct PyrSlotNode* mSelector;
    struct PyrParseNode* mArglist;
    struct PyrParseNode* mKeyarglist;
    bool mTailCall;
};

struct PyrCallNode : public PyrCallNodeBase2 {
    PyrCallNode(): PyrCallNodeBase2(pn_CallNode) {}
    virtual ~PyrCallNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrBinopCallNode : public PyrCallNodeBase2 {
    PyrBinopCallNode(): PyrCallNodeBase2(pn_BinopCallNode) {}
    virtual ~PyrBinopCallNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrSetterNode : public PyrCallNodeBase {
    PyrSetterNode(): PyrCallNodeBase(pn_SetterNode) {}
    virtual ~PyrSetterNode() {}
    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    struct PyrSlotNode* mSelector;
    struct PyrParseNode* mExpr1;
    struct PyrParseNode* mExpr2;
    int mFlags; // is a var def ?
};

struct PyrDynListNode : public PyrCallNodeBase {
    PyrDynListNode(): PyrCallNodeBase(pn_DynListNode) {}
    virtual ~PyrDynListNode() {}
    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    struct PyrParseNode* mClassname;
    struct PyrParseNode* mElems;
};

struct PyrDynDictNode : public PyrCallNodeBase {
    PyrDynDictNode(): PyrCallNodeBase(pn_DynDictNode) {}
    virtual ~PyrDynDictNode() {}
    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    struct PyrParseNode* mElems;
};


struct PyrDropNode : public PyrParseNode {
    PyrDropNode(): PyrParseNode(pn_DropNode) {}
    virtual ~PyrDropNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mExpr1;
    struct PyrParseNode* mExpr2;
};

struct PyrPushKeyArgNode : public PyrParseNode {
    PyrPushKeyArgNode(): PyrParseNode(pn_PushKeyArgNode) {}
    virtual ~PyrPushKeyArgNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mSelector;
    struct PyrParseNode* mExpr;
};

struct PyrReturnNode : public PyrParseNode {
    PyrReturnNode(): PyrParseNode(pn_ReturnNode) {}
    virtual ~PyrReturnNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mExpr; // if null, return self
};

struct PyrBlockReturnNode : public PyrParseNode {
    PyrBlockReturnNode(): PyrParseNode(pn_BlockReturnNode) {}
    virtual ~PyrBlockReturnNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mExpr; // if null, return self
};

struct PyrAssignNode : public PyrParseNode {
    PyrAssignNode(): PyrParseNode(pn_AssignNode) {}
    virtual ~PyrAssignNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mVarName;
    struct PyrParseNode* mExpr;
    bool mDrop; // allow drop
};

struct PyrMultiAssignNode : public PyrParseNode {
    PyrMultiAssignNode(): PyrParseNode(pn_MultiAssignNode) {}
    virtual ~PyrMultiAssignNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrMultiAssignVarListNode* mVarList;
    struct PyrParseNode* mExpr;
    bool mDrop; // allow drop
};

struct PyrMultiAssignVarListNode : public PyrParseNode {
    PyrMultiAssignVarListNode(): PyrParseNode(pn_MultiAssignVarListNode) {}
    virtual ~PyrMultiAssignVarListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mVarNames;
    struct PyrSlotNode* mRest;
};

struct PyrBlockNode : public PyrParseNode {
    PyrBlockNode(): PyrParseNode(pn_BlockNode) {}
    virtual ~PyrBlockNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    struct PyrParseNode* mBody;
    bool mIsTopLevel;
    int mBeginCharNo;
};

struct PyrArgListNode : public PyrParseNode {
    PyrArgListNode(): PyrParseNode(pn_ArgListNode) {}
    virtual ~PyrArgListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrVarDefNode* mVarDefs;
    struct PyrSlotNode* mRest;
    struct PyrSlotNode* mKeywordArgs = nullptr;
};

struct PyrLitListNode : public PyrParseNode {
    PyrLitListNode(): PyrParseNode(pn_LitListNode) {}
    virtual ~PyrLitListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mClassname;
    struct PyrParseNode* mElems;
};

struct PyrLitDictNode : public PyrParseNode {
    PyrLitDictNode(): PyrParseNode(pn_LitDictNode) {}
    virtual ~PyrLitDictNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mElems;
};

extern PyrParseNode* gRootParseNode;
extern intptr_t gParserResult;
extern bool gIsTailCodeBranch;
extern bool gTailIsMethodReturn;

extern bool compilingCmdLine;

extern const char* nodename[];

class SetTailBranch {
    bool mSave;

public:
    SetTailBranch(bool inValue) {
        mSave = gIsTailCodeBranch;
        gIsTailCodeBranch = inValue;
    }
    ~SetTailBranch() { gIsTailCodeBranch = mSave; }
};

inline void compileNode(PyrParseNode* node, PyrSlot* result, bool onTailBranch) {
    SetTailBranch branch(gIsTailCodeBranch && onTailBranch);
    node->compile(result);
}

void initParseNodes();

PyrSlotNode* newPyrSlotNode(PyrSlot* slot);
PyrCurryArgNode* newPyrCurryArgNode();
PyrClassNode* newPyrClassNode(PyrSlotNode* className, PyrSlotNode* superClassName, PyrVarListNode* varlists,
                              PyrMethodNode* methods, PyrSlotNode* indexType);
PyrClassExtNode* newPyrClassExtNode(PyrSlotNode* className, PyrMethodNode* methods);
PyrMethodNode* newPyrMethodNode(PyrSlotNode* methodName, PyrSlotNode* primitiveName, PyrArgListNode* arglist,
                                PyrVarListNode* varlist, PyrParseNode* body, int isClassMethod);
PyrArgListNode* newPyrArgListNode(PyrVarDefNode* varDefs, PyrSlotNode* rest, PyrSlotNode* kwArgs);
PyrVarListNode* newPyrVarListNode(PyrVarDefNode* vardefs, int flags);
PyrVarDefNode* newPyrVarDefNode(PyrSlotNode* varName, PyrParseNode* defVal, int flags);
PyrCallNode* newPyrCallNode(PyrSlotNode* selector, PyrParseNode* arglist, PyrParseNode* keyarglist,
                            PyrParseNode* blocklist);
PyrBinopCallNode* newPyrBinopCallNode(PyrSlotNode* selector, PyrParseNode* arg1, PyrParseNode* arg2,
                                      PyrParseNode* arg3);
PyrDropNode* newPyrDropNode(PyrParseNode* expr1, PyrParseNode* expr2);
PyrPushKeyArgNode* newPyrPushKeyArgNode(PyrSlotNode* selector, PyrParseNode* expr);
PyrPushLitNode* newPyrPushLitNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj);
PyrLiteralNode* newPyrLiteralNode(PyrSlotNode* literalSlot, PyrParseNode* literalObj);
PyrReturnNode* newPyrReturnNode(PyrParseNode* expr);
PyrBlockReturnNode* newPyrBlockReturnNode();
PyrAssignNode* newPyrAssignNode(PyrSlotNode* varName, PyrParseNode* expr, int flags);
PyrSetterNode* newPyrSetterNode(PyrSlotNode* varName, PyrParseNode* expr1, PyrParseNode* expr2);
PyrMultiAssignNode* newPyrMultiAssignNode(PyrMultiAssignVarListNode* varList, PyrParseNode* expr, int flags);
PyrPushNameNode* newPyrPushNameNode(PyrSlotNode* slotNode);
PyrDynDictNode* newPyrDynDictNode(PyrParseNode* elems);
PyrDynListNode* newPyrDynListNode(PyrParseNode* classname, PyrParseNode* elems);
PyrLitListNode* newPyrLitListNode(PyrParseNode* classname, PyrParseNode* elems);
PyrLitDictNode* newPyrLitDictNode(PyrParseNode* elems);
PyrMultiAssignVarListNode* newPyrMultiAssignVarListNode(PyrSlotNode* varNames, PyrSlotNode* rest);
PyrBlockNode* newPyrBlockNode(PyrArgListNode* arglist, PyrVarListNode* varlist, PyrParseNode* body, bool isTopLevel);


int nodeListLength(PyrParseNode* node);
bool isSuperObjNode(PyrParseNode* node);

void compileNodeList(PyrParseNode* node, bool onTailBranch);

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
extern int numOverwrites;
extern std::string overwriteMsg;

extern intptr_t zzval;
extern PyrSymbol* ps_newlist;
extern PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
extern PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
extern PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
extern PyrSymbol* gSpecialClasses[op_NumSpecialClasses];

extern PyrClass* gCurrentClass;
extern PyrClass* gCurrentMetaClass;
extern PyrClass* gCompilingClass;
extern PyrMethod* gCompilingMethod;
extern PyrBlock* gCompilingBlock;

#define YYSTYPE intptr_t
