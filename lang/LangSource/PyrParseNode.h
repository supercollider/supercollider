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

#include "CompilerContext.hpp"
#include "InitAlloc.h"
#include "PyrSlot.h"
#include "PyrKernel.h"
#include "AdvancingAllocPool.h"
#include "SpecialSelectorsOperatorsAndClasses.h"
#include "BisonHeaderInclude.hpp"
#include "text_location.hpp"

#include <array>


enum { varInst, varClass, varTemp, varConst, varPseudo, varLocal };

enum struct PyrParseNodeType : char {
    RootNode,
    ClassNode,
    ClassExtNode,
    MethodNode,
    BlockNode,
    SlotNode,

    VarListNode,
    VarDefNode,
    DynDictNode,
    DynListNode,
    LitListNode,

    ArgListNode,

    LiteralNode,

    PushLitNode,
    PushNameNode,
    PushKeyArgNode,
    CallNode,
    BinopCallNode,
    DropNode,
    AssignNode,
    MultiAssignNode,
    MultiAssignVarListNode,
    SetterNode,
    CurryArgNode,

    StringLine,
    String,

    ReturnNode,
    BlockReturnNode,
};


// This is how you create parse nodes, this is the only way to do so.
// It allocates them into gParseNodePool.
template <typename T, typename... ARGS>
T* allocParseNode(AdvancingAllocPool& pool, sc::lex::SourceCodeRange loc, ARGS&&... args);

struct PyrParseNode {
protected:
    struct Tag {
    private:
        Tag() {};

    public:
        template <typename T, typename... ARGS>
        friend T* allocParseNode(AdvancingAllocPool& pool, sc::lex::SourceCodeRange loc, ARGS&&... args);
    };

    PyrParseNode(Tag, PyrParseNodeType classno, sc::lex::SourceCodeRange loc);

public:
    PyrParseNode() = delete;
    // Note the absence of a virtual destructor, that is on purpose.
    // The current allocate requires all these parse nodes be trivially desctructible.
    // That means you cannot store, say, a std::vector inside one.
    PyrParseNode(PyrParseNode&&) = delete;
    PyrParseNode(const PyrParseNode&) = delete;
    PyrParseNode& operator=(PyrParseNode&&) = delete;
    PyrParseNode& operator=(const PyrParseNode&) = delete;

    virtual void compile(CompilerContext& cxt, PyrSlot* result) = 0;
    virtual void dump(int level) = 0;


    PyrParseNode* mNext;
    PyrParseNode* mTail;
    PyrParseNode* mParent { nullptr };
    sc::lex::SourceCodeRange location; // location in the source (not the file)
    PyrParseNodeType mClassno;

    // TODO: create a unique argument node for a = 1 and a (1), then this can be removed.
    unsigned char mParens { 0 };

    template <typename T, typename... ARGS>
    friend T* allocParseNode(AdvancingAllocPool& pool, sc::lex::SourceCodeRange loc, ARGS&&... args);
};

template <typename T> T* nodeCast(PyrParseNode* n) {
    static_assert(std::is_base_of_v<PyrParseNode, T>);
    static_assert(std::is_final_v<T>);
    if (!n)
        return nullptr;
    for (const auto t : T::types)
        if (n->mClassno == t)
            return static_cast<T*>(n);
    return nullptr;
}

template <typename T> const T* nodeCast(const PyrParseNode* n) {
    static_assert(std::is_base_of_v<PyrParseNode, T>);
    static_assert(std::is_final_v<T>);
    if (!n)
        return nullptr;
    for (const auto t : T::types)
        if (n->mClassno == t)
            return static_cast<const T*>(n);
    return nullptr;
}

template <typename T, typename... ARGS>
T* allocParseNode(AdvancingAllocPool& pool, sc::lex::SourceCodeRange loc, ARGS&&... args) {
    static_assert(std::is_trivially_destructible_v<T>);
    static_assert(std::is_base_of_v<PyrParseNode, T>);

    auto ptr = pool.Alloc(sizeof(T));
    MEMFAIL(ptr);
    T* r = new (ptr) T { PyrParseNode::Tag {}, loc, std::forward<ARGS>(args)... };
    return r;
}

template <typename T, typename... ARGS>
[[nodiscard]] T* CompilerContext::allocParseNode(sc::lex::SourceCodeRange location, ARGS&&... args) {
    return ::allocParseNode<T>(parseNodePool, location, args...);
}


struct PyrRootNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::RootNode };

    // line number offset refers to the
    PyrRootNode(Tag t, sc::lex::SourceCodeRange loc, PyrParseNode* children);

    void compile(CompilerContext& cxt, PyrSlot* result) override;
    void dump(int level) override {};

    PyrParseNode* children;

    bool commandLineMode;
};

struct PyrSlotNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 4> types { PyrParseNodeType::SlotNode, PyrParseNodeType::PushLitNode,
                                                             PyrParseNodeType::PushNameNode,
                                                             PyrParseNodeType::LiteralNode };
    PyrSlotNode(Tag t, sc::lex::SourceCodeRange loc, PyrSlot slot,
                PyrParseNodeType subtype = PyrParseNodeType::SlotNode);

    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void compileLiteral(CompilerContext& cxt, PyrSlot* result);
    virtual void compilePushLit(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);
    virtual void dumpLiteral(int level);
    virtual void dumpPushLit(int level);

    // Changes the 'type' of the node. Only changes the type flag.
    template <PyrParseNodeType Target> PyrSlotNode* changeLiteralType() {
        static_assert(Target == types[1] || Target == types[2] || Target == types[3]);
        mClassno = Target;
        return this;
    }

    PyrSlot mSlot;
};

struct PyrCurryArgNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::CurryArgNode };
    PyrCurryArgNode(Tag t, sc::lex::SourceCodeRange loc);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    int mArgNum { -1 };
};


struct PyrClassExtNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::ClassExtNode };
    PyrClassExtNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* classname, struct PyrMethodNode* methods);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mClassName;
    struct PyrMethodNode* mMethods;
};

struct PyrClassNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::ClassNode };
    PyrClassNode(Tag t, sc::lex::SourceCodeRange l, struct PyrSlotNode* mClassName, struct PyrSlotNode* mSuperClassName,
                 struct PyrSlotNode* mIndexType, struct PyrVarListNode* mVarlists, struct PyrMethodNode* mMethods);

    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mClassName;
    struct PyrSlotNode* mSuperClassName;
    struct PyrSlotNode* mIndexType;
    struct PyrVarListNode* mVarlists;
    struct PyrMethodNode* mMethods;
    int mVarTally[4] = {
        0,
        0,
        0,
        0,
    };
    int mNumSuperInstVars { 0 };
};

struct PyrMethodNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::MethodNode };
    PyrMethodNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mMethodName, PyrSlotNode* mPrimitiveName,
                  struct PyrArgListNode* mArglist, struct PyrVarListNode* mVarlist, PyrParseNode* mBody,
                  bool mIsClassMethod, bool mExtension = false);

    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrSlotNode* mMethodName;
    struct PyrSlotNode* mPrimitiveName;
    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    struct PyrParseNode* mBody;
    int mIsClassMethod; // is class method?
    bool mExtension;
};

struct PyrVarListNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::VarListNode };
    PyrVarListNode(Tag t, sc::lex::SourceCodeRange l, struct PyrVarDefNode* mVarDefs, int mFlags);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrVarDefNode* mVarDefs;
    int mFlags;
};

struct PyrVarDefNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::VarDefNode };
    PyrVarDefNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarName, PyrParseNode* mDefVal,
                  ReadWriteAccessor mFlags);

    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void compileArg(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);
    bool hasExpr(CompilerContext& cxt, PyrSlot* result);

    struct PyrSlotNode* mVarName;
    PyrParseNode* mDefVal;
    ReadWriteAccessor accessor;
    bool mDrop { true };
};

struct PyrCallNodeBase : public PyrParseNode {
    PyrCallNodeBase(Tag t, sc::lex::SourceCodeRange l, PyrParseNodeType classno);

    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void compilePartialApplication(CompilerContext& cxt, unsigned char numCurryArgs, PyrSlot* result);
    virtual void compileCall(CompilerContext& cxt, PyrSlot* result) = 0;

    virtual int isPartialApplication() = 0;
};

struct PyrCallNodeBase2 : public PyrCallNodeBase {
    PyrCallNodeBase2(Tag tg, sc::lex::SourceCodeRange l, PyrParseNodeType t, PyrSlotNode* mSelector,
                     PyrParseNode* mArglist, PyrParseNode* mKeyarglist);

    PyrSlotNode* mSelector;
    PyrParseNode* mArglist;
    PyrParseNode* mKeyarglist;
    bool mTailCall { false };
};

struct PyrCallNode final : public PyrCallNodeBase2 {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::CallNode };
    PyrCallNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mArglist,
                PyrParseNode* mKeyarglist = nullptr);

    virtual void compileCall(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrBinopCallNode final : public PyrCallNodeBase2 {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::BinopCallNode };
    PyrBinopCallNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* arglist);

    virtual void compileCall(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();
};

struct PyrSetterNode final : public PyrCallNodeBase {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::SetterNode };
    PyrSetterNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mExpr1,
                  PyrParseNode* mExpr2);

    virtual void compileCall(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrSlotNode* mSelector;
    PyrParseNode* mExpr1;
    PyrParseNode* mExpr2;
    int mFlags; // is a var def ?
};

struct PyrDynListNode final : public PyrCallNodeBase {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::DynListNode };
    PyrDynListNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mClassname, PyrParseNode* mElems);
    virtual void compileCall(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrParseNode* mClassname;
    PyrParseNode* mElems;
};

struct PyrDynDictNode final : public PyrCallNodeBase {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::DynDictNode };
    PyrDynDictNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mElems);
    virtual void compileCall(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    virtual int isPartialApplication();

    PyrParseNode* mElems;
};


struct PyrDropNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::DropNode };
    PyrDropNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* e1, PyrParseNode* e2);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mExpr1;
    PyrParseNode* mExpr2;
};

struct PyrPushKeyArgNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::PushKeyArgNode };
    PyrPushKeyArgNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mExpr);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mSelector;
    PyrParseNode* mExpr;
};

struct PyrReturnNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::ReturnNode };
    PyrReturnNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mExpr);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mExpr; // if null, return self
};

struct PyrBlockReturnNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::BlockReturnNode };
    PyrBlockReturnNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mExpr);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mExpr; // if null, return self
};

struct PyrAssignNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::AssignNode };
    PyrAssignNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarName, PyrParseNode* mExpr);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mVarName;
    PyrParseNode* mExpr;
    bool mDrop { false }; // allow drop
};

struct PyrMultiAssignNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::MultiAssignNode };
    PyrMultiAssignNode(Tag t, sc::lex::SourceCodeRange l, struct PyrMultiAssignVarListNode* mVarList,
                       PyrParseNode* mExpr);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrMultiAssignVarListNode* mVarList;
    PyrParseNode* mExpr;
    bool mDrop { false }; // allow drop
};

struct PyrMultiAssignVarListNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::MultiAssignVarListNode };
    PyrMultiAssignVarListNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarNames, PyrSlotNode* mRest);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrSlotNode* mVarNames;
    PyrSlotNode* mRest;
};

struct PyrBlockNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::BlockNode };
    PyrBlockNode(Tag t, sc::lex::SourceCodeRange l, struct PyrArgListNode* mArglist, struct PyrVarListNode* mVarlist,
                 PyrParseNode* mBody, bool mIsTopLevel);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrArgListNode* mArglist;
    struct PyrVarListNode* mVarlist;
    PyrParseNode* mBody;
    bool mIsTopLevel;
};

struct PyrArgListNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::ArgListNode };
    PyrArgListNode(Tag t, sc::lex::SourceCodeRange l, struct PyrVarDefNode* mVarDefs, PyrSlotNode* mRest,
                   PyrSlotNode* mKeywordArgs);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    struct PyrVarDefNode* mVarDefs;
    PyrSlotNode* mRest;
    PyrSlotNode* mKeywordArgs;
};

struct PyrLitListNode final : public PyrParseNode {
    static constexpr std::array<PyrParseNodeType, 1> types { PyrParseNodeType::LitListNode };
    PyrLitListNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mClassname, PyrParseNode* mElems);
    virtual void compile(CompilerContext& cxt, PyrSlot* result);
    virtual void dump(int level);

    PyrParseNode* mClassname;
    PyrParseNode* mElems;
};

void compileNode(CompilerContext& cxt, PyrParseNode* node, PyrSlot* result, bool onTailBranch);

int nodeListLength(PyrParseNode* node);
bool isSuperObjNode(PyrParseNode* node);

void compileNodeList(CompilerContext& cxt, PyrParseNode* node, bool onTailBranch);

// void initParser();
// void initParserPool();
// void freeParserPool();

void initSpecialSelectors();
void initSpecialClasses();

void printErrorLine(PyrParseNode* node, const char* description = nullptr);

inline PyrParseNode* linkAfterHead(PyrParseNode* a, PyrParseNode* b) {
    b->mNext = a->mNext;
    if (!a->mNext)
        a->mTail = b;
    a->mNext = b;
    return a;
}

template <typename T, typename... NODES> T* linkNodes(T* first, NODES*... nodes) {
    static_assert(std::is_base_of_v<PyrParseNode, T>);
    static_assert((std::is_base_of_v<PyrParseNode, NODES> && ...));
    const auto link = [](PyrParseNode* a, PyrParseNode* b) {
        if (a == nullptr)
            return b;
        if (b) {
            a->mTail->mNext = b;
            a->mTail = b->mTail;
        }
        return a;
    };
    PyrParseNode* rolling { first };
    ((rolling = link(rolling, nodes)), ...);
    return static_cast<T*>(rolling);
}


extern PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
extern PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
extern PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
extern PyrSymbol* gSpecialClasses[op_NumSpecialClasses];

extern PyrClass* gCurrentMetaClass;
extern PyrClass* gCompilingClass;
extern PyrMethod* gCompilingMethod;
extern PyrBlock* gCompilingBlock;
;
