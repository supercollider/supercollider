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

#include "ByteCodeArray.h"
#include "PyrLexer.h"
#include "PyrSlot.h"
#include "PyrKernel.h"
#include "SC_Version.hpp"
#include "ByteCodeArray.h"
#include "Opcodes.h"
#include "AdvancingAllocPool.h"
#include "SpecialSelectorsOperatorsAndClasses.h"
#include <cassert>
#include <optional>
#include <type_traits>

// TODO: remove these macros.
#define COMPILENODE(node, result, onTailBranch) (compileNode((node), (result), (onTailBranch)))
#define DUMPNODE(node, level)                                                                                          \
    do {                                                                                                               \
        if (node)                                                                                                      \
            (node)->dump(level);                                                                                       \
    } while (false);

// This value count the un-inlined functions, these are not desirable in the class library because they are slow.
// There is a primitive that returns this value so it can be checked in sclang's unit tests.
extern int gNumUninlinedFunctions;

// TODO: remove this in favour of the strongly typed version.
enum { rwPrivate = 0, rwReadOnly = 1, rwWriteOnly = 2, rwReadWrite = 3 };

enum struct ReadWriteAccessor {
    Private = rwPrivate,
    Read = rwReadOnly,
    Write = rwWriteOnly,
    ReadWrite = rwReadWrite,
};

enum { varInst, varClass, varTemp, varConst, varPseudo, varLocal };

enum ParseNodeEnum : unsigned char {
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

    pn_NewBlock,

    pn_NumTypes
};

// Base class of all parse nodes.
// Because the parser does not do any memory management, we require all parse nodes to be arena/pool allocated.
struct PyrParseNode {
protected:
    struct TAG {};
    PyrParseNode(TAG, LocationType location, ParseNodeEnum classno): mClassno(classno), mLocation(location) {}

public:
    // Pinned type. Does not move. Pointer stability must be guaranteed.
    PyrParseNode() = delete;
    PyrParseNode(PyrParseNode&&) = delete;
    PyrParseNode(const PyrParseNode&) = delete;
    PyrParseNode& operator=(PyrParseNode&&) = delete;
    PyrParseNode& operator=(const PyrParseNode&) = delete;

    // Currently this is never called due to how the parse node pool allocated works.
    // I've left all the virtual destructors commented out until this is fixed.
    // There is also a static_assert in the allocNode function to ensure all derived classes are trivial to destruct.
    // virtual ~PyrParseNode() = default;

    ~PyrParseNode() = default;

    virtual void compile(PyrSlot* result) = 0;
    virtual void dump(int level) = 0;

    [[nodiscard]] LocationInSourceCode location() const { return { mLocation.begin.absolute, mLocation.end.absolute }; }

    PyrParseNode* mNext { nullptr };
    PyrParseNode* mTail { this }; // not setting this will cause all sorts of weird bugs.
    LocationType mLocation;
    ParseNodeEnum mClassno;
    bool mParens { false }; // TODO: what does this mean? Parentheses? Parents? It is parenthesis, but why?

    // This is how you make a node.
    template <typename T, typename... ARGS> friend T* allocNode(ARGS&&... args);

    template <typename T> T* assertCast();
    template <typename T> std::optional<T*> tryCast();
};

extern AdvancingAllocPool gParseNodePool;

template <typename T, typename... ARGS> T* allocNode(LocationType loc, ARGS&&... args) {
    static_assert(std::is_base_of<PyrParseNode, T>::value, "Can only allocate PyrParseNodes through this function.");
    static_assert(std::is_trivially_destructible<T>::value,
                  "Right now the allocator does NOT call the destructors, therefore all the PyrParseNodes MUST be "
                  "trival to destruct.");
    return new (gParseNodePool.Alloc(sizeof(T))) T({}, loc, std::forward<ARGS>(args)...);
}


// The PyrSlotNode supports four 'sub' types, indicated with the ParseNodeEnum: pn_SlotNode, pn_LiteralNode,
// pn_PushLitNode, and pn_PushNameNode. In future these could be their own types, however, the parser often transmutates
// one to the other without reallocating the node. To avoid doing some crazy casting of 'this' (`new (this)
// PushNameName(...)`), the enum is used as a switch.
struct PyrSlotNode : public PyrParseNode {
    PyrSlotNode(PyrParseNode::TAG, LocationType location, PyrSlot slot = {}, ParseNodeEnum classno = pn_SlotNode):
        PyrParseNode({}, location, classno),
        mSlot(slot) {
        assert(classno == pn_SlotNode || classno == pn_LiteralNode || classno == pn_PushLitNode
               || classno == pn_PushNameNode);
    }

    // virtual ~PyrSlotNode() {}

    PyrSlotNode* changeLiteralType(ParseNodeEnum e) {
        assert(mClassno == pn_SlotNode);
        assert(e == pn_LiteralNode || e == pn_PushLitNode || e == pn_PushNameNode);
        mClassno = e;
        return this;
    }

    virtual void compile(PyrSlot* result);
    virtual void compileLiteral(PyrSlot* result);
    virtual void compilePushLit(PyrSlot* result);
    virtual void dump(int level);
    virtual void dumpLiteral(int level);
    virtual void dumpPushLit(int level);

    PyrSlot mSlot;
};


template <typename T> T* PyrParseNode::assertCast() {
    if constexpr (std::is_same_v<T, PyrSlotNode>) {
        assert(mClassno == pn_SlotNode || mClassno == pn_PushLitNode || mClassno == pn_PushNameNode
               || mClassno == pn_LiteralNode);
    } else {
        assert(mClassno == T::nodeEnum);
    }
    return reinterpret_cast<T*>(this);
}

template <typename T> std::optional<T*> PyrParseNode::tryCast() {
    if constexpr (std::is_same_v<T, PyrSlotNode>) {
        if (mClassno == pn_SlotNode || mClassno == pn_PushLitNode || mClassno == pn_PushNameNode
            || mClassno == pn_LiteralNode) {
            return { reinterpret_cast<T*>(this) };
        } else
            return std::nullopt;
    } else {
        if (mClassno == T::nodeEnum) {
            return { reinterpret_cast<T*>(this) };
        } else
            return std::nullopt;
    }
}

struct PyrCurryArgNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_CurryArgNode };

    PyrCurryArgNode(PyrParseNode::TAG, LocationType loc, int argNum = -1):
        PyrParseNode({}, loc, pn_CurryArgNode),
        mArgNum(argNum) {}

    // virtual ~PyrCurryArgNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    int mArgNum;
};

struct PyrClassExtNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_ClassExtNode };
    PyrClassExtNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* classname, struct PyrMethodNode* methods):
        PyrParseNode({}, loc, pn_ClassExtNode),
        mClassName(classname),
        mMethods(methods) {}

    // virtual ~PyrClassExtNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mClassName;
    struct PyrMethodNode* mMethods;
};

struct PyrClassNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_ClassNode };
    PyrClassNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* className, PyrSlotNode* superClassName,
                 PyrSlotNode* indexType, struct PyrVarListNode* varlists, struct PyrMethodNode* methods);

    // virtual ~PyrClassNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mClassName;
    PyrSlotNode* mSuperClassName;
    PyrSlotNode* mIndexType;
    struct PyrVarListNode* mVarlists;
    struct PyrMethodNode* mMethods;

    // This uses the enums varInst, varClass, varTemp, and varConst to count the number of instance variables.
    int mVarTally[4] { 0, 0, 0, 0 };
    int mNumSuperInstVars { 0 };
};

struct PyrMethodNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_MethodNode };
    PyrMethodNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* methodName, PyrSlotNode* primitiveName,
                  struct PyrArgListNode* arglist, struct PyrVarListNode* varlist, PyrParseNode* body,
                  bool isClassMethod);

    // virtual ~PyrMethodNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mMethodName;
    PyrSlotNode* mPrimitiveName;
    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    struct PyrParseNode* mBody;

    bool mIsClassMethod; // is class method?
    bool mExtension { false };
};

struct PyrVarListNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_VarListNode };
    PyrVarListNode(PyrParseNode::TAG, LocationType loc, struct PyrVarDefNode* vardef, int flags):
        PyrParseNode({}, loc, pn_VarListNode),
        mVarDefs(vardef),
        mFlags(flags) {}

    // virtual ~PyrVarListNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrVarDefNode* mVarDefs;
    int mFlags;
};

struct PyrVarDefNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_VarDefNode };

    PyrVarDefNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* varname, PyrParseNode* defVal,
                  ReadWriteAccessor rwAccessor):
        PyrParseNode({}, loc, nodeEnum),
        mVarName(varname),
        mDefVal(defVal),
        mFlags(static_cast<int>(rwAccessor)),
        mDrop(true) {
        assert(mVarName);
        assert(mVarName->mSlot.isSymbol());
    };

    // virtual ~PyrVarDefNode() {}

    virtual void compile(PyrSlot* result);
    virtual void compileArg(PyrSlot* result);
    virtual void dump(int level);

    bool hasExpr(PyrSlot* result);

    PyrSlotNode* mVarName;
    PyrParseNode* mDefVal;
    int mFlags;
    bool mDrop;
};

struct PyrCallNodeBase : public PyrParseNode {
    PyrCallNodeBase(PyrParseNode::TAG, LocationType loc, ParseNodeEnum classno): PyrParseNode({}, loc, classno) {}

    // virtual ~PyrCallNodeBase() {}

    virtual void compile(PyrSlot* result);
    virtual void compilePartialApplication(int numCurryArgs, PyrSlot* result);
    virtual void compileCall(PyrSlot* result) = 0;

    virtual int isPartialApplication() = 0;
};

struct PyrCallNodeBase2 : public PyrCallNodeBase {
    PyrCallNodeBase2(PyrParseNode::TAG, LocationType loc, ParseNodeEnum classno, PyrSlotNode* selector,
                     PyrParseNode* arglist, PyrParseNode* keyarglist):
        PyrCallNodeBase({}, loc, classno),
        mSelector(selector),
        mArglist(arglist),
        mKeyarglist(keyarglist) {
        assert(selector);
        assert(selector->mSlot.isSymbol());
    }

    // virtual ~PyrCallNodeBase2() {}

    PyrSlotNode* mSelector;
    PyrParseNode* mArglist;
    PyrParseNode* mKeyarglist;
    bool mTailCall { false };
};

struct PyrCallNode : public PyrCallNodeBase2 {
    static constexpr ParseNodeEnum nodeEnum { pn_CallNode };
    PyrCallNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* selector, PyrParseNode* arglist,
                PyrParseNode* keyarglist):
        PyrCallNodeBase2({}, loc, pn_CallNode, selector, arglist, keyarglist) {}

    // virtual ~PyrCallNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrBinopCallNode : public PyrCallNodeBase2 {
    static constexpr ParseNodeEnum nodeEnum { pn_BinopCallNode };
    PyrBinopCallNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* selector, PyrParseNode* arglist):
        PyrCallNodeBase2({}, loc, pn_BinopCallNode, selector, arglist, nullptr) {}

    // virtual ~PyrBinopCallNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrSetterNode : public PyrCallNodeBase {
    static constexpr ParseNodeEnum nodeEnum { pn_SetterNode };
    PyrSetterNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* selector, PyrParseNode* expr1, PyrParseNode* expr2):
        PyrCallNodeBase({}, loc, pn_SetterNode),
        mSelector(selector),
        mExpr1(expr1),
        mExpr2(expr2) {
        assert(mSelector);
        assert(mSelector->mSlot.isSymbol());
    }

    // virtual ~PyrSetterNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrSlotNode* mSelector;
    PyrParseNode* mExpr1;
    PyrParseNode* mExpr2;
    int mFlags { 0 }; // is a var def ?
};

struct PyrDynListNode : public PyrCallNodeBase {
    static constexpr ParseNodeEnum nodeEnum { pn_DynListNode };
    PyrDynListNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* className, PyrParseNode* elems):
        PyrCallNodeBase({}, loc, pn_DynListNode),
        mClassname(className),
        mElems(elems) {
        if (className)
            assert(className->mSlot.isSymbol());
    }

    // virtual ~PyrDynListNode() {}

    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrSlotNode* mClassname;
    PyrParseNode* mElems;
};

struct PyrDynDictNode : public PyrCallNodeBase {
    static constexpr ParseNodeEnum nodeEnum { pn_DynDictNode };
    PyrDynDictNode(PyrParseNode::TAG, LocationType loc, PyrParseNode* elems):
        PyrCallNodeBase({}, loc, pn_DynDictNode),
        mElems(elems) {}
    // virtual ~PyrDynDictNode() {}
    virtual void compileCall(PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrParseNode* mElems;
};

struct PyrDropNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_DropNode };
    PyrDropNode(PyrParseNode::TAG, LocationType loc, PyrParseNode* expr1, PyrParseNode* expr2):
        PyrParseNode({}, loc, pn_DropNode),
        mExpr1(expr1),
        mExpr2(expr2) {
        assert(mExpr1);
        assert(mExpr2);
    }

    // virtual ~PyrDropNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrParseNode* mExpr1;
    struct PyrParseNode* mExpr2;
};

struct PyrPushKeyArgNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_PushKeyArgNode };
    PyrPushKeyArgNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* selector, PyrParseNode* expr):
        PyrParseNode({}, loc, pn_PushKeyArgNode),
        mSelector(selector),
        mExpr(expr) {
        assert(selector);
        assert(selector->mSlot.isSymbol());
    }

    // virtual ~PyrPushKeyArgNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mSelector;
    PyrParseNode* mExpr;
};


struct PyrReturnNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_ReturnNode };
    PyrReturnNode(PyrParseNode::TAG, LocationType loc, PyrParseNode* expr):
        PyrParseNode({}, loc, pn_ReturnNode),
        mExpr(expr) {}
    // virtual ~PyrReturnNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mExpr; // if null, return self
};


struct PyrBlockReturnNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_BlockReturnNode };
    PyrBlockReturnNode(PyrParseNode::TAG, LocationType loc, PyrParseNode* expr):
        PyrParseNode({}, loc, pn_BlockReturnNode),
        mExpr(expr) {}
    // virtual ~PyrBlockReturnNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mExpr; // if null, return self
};

struct PyrAssignNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_AssignNode };
    PyrAssignNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* varName, PyrParseNode* expr):
        PyrParseNode({}, loc, pn_AssignNode),
        mVarName(varName),
        mExpr(expr) {
        assert(varName);
        assert(varName->mSlot.isSymbol());
        assert(expr);
    }

    // virtual ~PyrAssignNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mVarName;
    PyrParseNode* mExpr;
    bool mDrop { false }; // allow drop
};

struct PyrMultiAssignNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_MultiAssignNode };
    PyrMultiAssignNode(PyrParseNode::TAG, LocationType loc, struct PyrMultiAssignVarListNode* varlist,
                       PyrParseNode* expr):
        PyrParseNode({}, loc, pn_MultiAssignNode),
        mVarList(varlist),
        mExpr(expr) {
        assert(mVarList);
        assert(mExpr);
    }
    // virtual ~PyrMultiAssignNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrMultiAssignVarListNode* mVarList;
    PyrParseNode* mExpr;
    bool mDrop { false }; // allow drop
};

struct PyrMultiAssignVarListNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_MultiAssignVarListNode };
    PyrMultiAssignVarListNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* varNames, PyrSlotNode* rest):
        PyrParseNode({}, loc, pn_MultiAssignVarListNode),
        mVarNames(varNames),
        mRest(rest) {
        assert(mVarNames);
    }
    // virtual ~PyrMultiAssignVarListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mVarNames;
    PyrSlotNode* mRest;
};

struct PyrBlockNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_BlockNode };
    PyrBlockNode(PyrParseNode::TAG, LocationType loc, struct PyrArgListNode* argList, struct PyrVarListNode* varList,
                 struct PyrParseNode* body, bool topLevel);

    // virtual ~PyrBlockNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    struct PyrParseNode* mBody;
    bool mIsTopLevel;
};


struct PyrNewBlockNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_NewBlock };
    PyrNewBlockNode(PyrParseNode::TAG, LocationType loc, struct PyrArgListNode* arguments, PyrParseNode* body,
                    bool is_top_level):
        PyrParseNode({}, loc, pn_NewBlock),
        arguments(arguments),
        body(body),
        is_top_level(is_top_level) {}


    void compile(PyrSlot* result) override;
    void dump(int level) override {}

private:
    struct PyrArgListNode* arguments;
    PyrParseNode* body; // (PyrVarListNode* | PyrParseNode*) either varlist or whatever expr has produced
    bool is_top_level;

    uint32_t count_vars() const;
    std::vector<PyrVarDefNode*> gather_vars() const;

    template <typename Action> void for_each_normal_arg(Action&& action) const;
    template <typename Action> void for_each_var_arg_name(Action&& action) const;
    template <typename Action> void for_each_var(Action&& action) const;
    template <typename ExprAction, typename VarDefAction>
    void for_each_body_expr_and_var_def(ExprAction&& expr_action, VarDefAction&& var_action) const;
};

struct PyrArgListNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_ArgListNode };
    PyrArgListNode(PyrParseNode::TAG, LocationType loc, PyrVarDefNode* vardefs, PyrSlotNode* varArgName,
                   PyrSlotNode* varKwArgName):
        PyrParseNode({}, loc, pn_ArgListNode),
        mVarDefs(vardefs),
        mVariableArgumentName(varArgName),
        mVariableKeywordArgumentName(varKwArgName) {
        if (mVariableArgumentName)
            assert(mVariableArgumentName->mSlot.isSymbol());
        if (mVariableKeywordArgumentName)
            assert(mVariableKeywordArgumentName->mSlot.isSymbol());
    }

    // virtual ~PyrArgListNode() {}

    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrVarDefNode* mVarDefs;
    PyrSlotNode* mVariableArgumentName;
    PyrSlotNode* mVariableKeywordArgumentName;
};

struct PyrLitListNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_LitListNode };
    PyrLitListNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* classname, PyrParseNode* elems):
        PyrParseNode({}, loc, pn_LitListNode),
        mClassname(classname),
        mElems(elems) {
        if (mClassname)
            assert(mClassname->mSlot.isSymbol());
    }
    // virtual ~PyrLitListNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mClassname;
    PyrParseNode* mElems;
};

struct PyrLitDictNode : public PyrParseNode {
    static constexpr ParseNodeEnum nodeEnum { pn_LitDictNode };
    PyrLitDictNode(PyrParseNode::TAG, LocationType loc, PyrParseNode* elems):
        PyrParseNode({}, loc, pn_LitDictNode),
        mElems(elems) {}
    // virtual ~PyrLitDictNode() {}
    virtual void compile(PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mElems;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern PyrParseNode* gRootParseNode;
extern intptr_t gParserResult;
extern bool gIsTailCodeBranch;
extern bool gTailIsMethodReturn;

extern bool compilingCmdLine;


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

template <typename T> T* linkNextNode(T* a, T* b) {
    static_assert(std::is_base_of_v<PyrParseNode, T>);
    return static_cast<T*>(linkNextNode(static_cast<PyrParseNode*>(a), static_cast<PyrParseNode*>(b)));
}

template <typename... NODES> PyrParseNode* linkAllNodes(NODES... nodes) {
    PyrParseNode* rolling { nullptr };
    ((rolling = linkNextNode(rolling, nodes)), ...);
    return rolling;
}

extern int compileErrors;

/// Creates a compiler error if current version is greater than or equal to 'version'.
/// Otherwise posts a warning informing the user to fix their code before updating.
void emitCompilerErrorFromVersion(SemanticVersion version);

extern int numOverwrites;
extern std::string overwriteMsg;

extern PyrSymbol* ps_newlist;
extern PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
extern PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
extern PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
extern PyrSymbol* gSpecialClasses[op_NumSpecialClasses];

extern PyrClass* gCurrentClass;
extern PyrClass* gCurrentMetaClass;
extern PyrClass* gCompilingClass;
extern PyrMethod* gCompilingMethod;
extern PyrFunctionDef* gCompilingBlock;

// #define YYSTYPE intptr_t
