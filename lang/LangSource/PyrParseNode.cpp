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

#include "ClassLibraryInfo.hpp"
#include "CompilerContext.hpp"
#include "PyrObjectHdr.h"
#include "SC_Version.hpp"
#include "ByteCodeArray.h"
#include "OpcodeOperands.h"
#include "PyrObject.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
#include "SCBase.h"
#include "PyrParseNode.h"
#include "PyrKernel.h"
#include "PyrListPrim.h"
#include "PyrSymbolTable.h"
#include "Opcodes.h"
#include "PyrKernelProto.h"
#include "PyrObjectProto.h"
#include "GC.h"
#include "PredefinedSymbols.h"
#include "SC_LanguageConfig.hpp"
#include "SpecialSelectorsOperatorsAndClasses.h"
#include "text_location.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <optional>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cctype>
#include <unordered_map>
#include <variant>
#include "PredefinedSymbols.h"
#include "SC_LanguageConfig.hpp"
#include "SpecialSelectorsOperatorsAndClasses.h"
#include "text_location.hpp"
#include "PyrPrimitive.h"


class SetTailBranch {
    CompilerContext& cxt;
    bool mSave;

public:
    SetTailBranch(CompilerContext& cxt, bool inValue): cxt(cxt) {
        mSave = cxt.isTailCodeBranch;
        cxt.isTailCodeBranch = inValue;
    }
    ~SetTailBranch() { cxt.isTailCodeBranch = mSave; }
};

void compileNode(CompilerContext& cxt, PyrParseNode* node, PyrSlot* result, bool onTailBranch) {
    SetTailBranch branch(cxt, cxt.isTailCodeBranch && onTailBranch);
    node->compile(cxt, result);
}


using namespace Opcode;

int allocationFlags(const std::shared_ptr<const TextInfo>& textInfo) {
    return textInfo->isClassFile ? obj_permanent | obj_immutable : obj_immutable;
}
int allocationFlags(CompilerContext& cxt) { return allocationFlags(cxt.textInfo); }

struct PyrGC* compileGC(CompilerContext& cxt) {
    return cxt.vm_globals ? cxt.vm_globals->gc : nullptr;
}


template <typename T> T* reParent(PyrParseNode& parent, T* child) {
    PyrParseNode* c { child };

    while (c) {
        c->mParent = &parent;
        c = c->mNext;
    }

    return child;
}

// Takes code like...
// ```
// var a, b, c;
// var d, e, f;
// ```
// and turns into into one var list of `var a, b, c, d, e, f`.
PyrVarListNode* joinVarLists(PyrVarListNode* varlist) {
    if (varlist) {
        PyrVarListNode* prevvarlist;
        // find end of this list
        auto vardef = varlist->mVarDefs;
        auto lastvardef = (PyrVarDefNode*)vardef->mTail;
        prevvarlist = varlist;
        varlist = (PyrVarListNode*)varlist->mNext;

        for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
            vardef = varlist->mVarDefs;
            lastvardef->mNext = (PyrParseNode*)vardef;

            // find end of this list
            lastvardef = (PyrVarDefNode*)vardef->mTail;
        }
    }
    return varlist;
}

PyrParseNode::PyrParseNode(Tag, PyrParseNodeType classno, sc::lex::SourceCodeRange loc):
    mNext(nullptr),
    mTail(this),
    location(loc),
    mClassno(classno) {}

PyrRootNode::PyrRootNode(Tag t, sc::lex::SourceCodeRange loc, PyrParseNode* children):
    PyrParseNode(t, PyrParseNodeType::RootNode, loc),
    children(reParent(*this, children)) {}

void PyrRootNode::compile(CompilerContext& cxt, PyrSlot* result) { compileNodeList(cxt, children, false); }

PyrSlotNode::PyrSlotNode(Tag t, sc::lex::SourceCodeRange loc, PyrSlot s, PyrParseNodeType subtype):
    PyrParseNode(t, subtype, loc),
    mSlot(s) {
    if (mSlot.isPtr()) {
        auto child = reinterpret_cast<PyrParseNode*>(mSlot.getPtr());
        child->mParent = this;
    }
    assert(subtype == types[0] || subtype == types[1] || subtype == types[2] || subtype == types[3]);
}

PyrCurryArgNode::PyrCurryArgNode(Tag t, sc::lex::SourceCodeRange loc):
    PyrParseNode(t, PyrParseNodeType::CurryArgNode, loc) {}

PyrClassExtNode::PyrClassExtNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* classname,
                                 struct PyrMethodNode* methods):
    PyrParseNode(t, PyrParseNodeType::ClassExtNode, l),
    mClassName(reParent(*this, classname)),
    mMethods(reParent(*this, methods)) {}

PyrClassNode::PyrClassNode(Tag t, sc::lex::SourceCodeRange l, struct PyrSlotNode* mClassName,
                           struct PyrSlotNode* mSuperClassName, struct PyrSlotNode* mIndexType,
                           struct PyrVarListNode* mVarlists, struct PyrMethodNode* mMethods):
    PyrParseNode(t, PyrParseNodeType::ClassNode, l),
    mClassName(reParent(*this, mClassName)),
    mSuperClassName(reParent(*this, mSuperClassName)),
    mIndexType(reParent(*this, mIndexType)),
    mVarlists(reParent(*this, mVarlists)),
    mMethods(reParent(*this, mMethods)) {}

PyrMethodNode::PyrMethodNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mMethodName, PyrSlotNode* mPrimitiveName,
                             struct PyrArgListNode* mArglist, struct PyrVarListNode* mVarlist_in, PyrParseNode* mBody,
                             bool mIsClassMethod, bool mExtension):
    PyrParseNode(t, PyrParseNodeType::MethodNode, l),
    mMethodName(reParent(*this, mMethodName)),
    mPrimitiveName(reParent(*this, mPrimitiveName)),
    mArglist(reParent(*this, mArglist)),
    mVarlist(mVarlist_in),
    mBody(reParent(*this, mBody)),
    mIsClassMethod(mIsClassMethod),
    mExtension(mExtension) {
    joinVarLists(mVarlist);
    mVarlist = reParent(*this, mVarlist);
}

PyrVarListNode::PyrVarListNode(Tag t, sc::lex::SourceCodeRange l, struct PyrVarDefNode* mVarDefs, int mFlags):
    PyrParseNode(t, PyrParseNodeType::VarListNode, l),
    mVarDefs(reParent(*this, mVarDefs)),
    mFlags(mFlags) {}

PyrVarDefNode::PyrVarDefNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarName, PyrParseNode* mDefVal,
                             ReadWriteAccessor accessor):
    PyrParseNode(t, PyrParseNodeType::VarDefNode, l),
    mVarName(reParent(*this, mVarName)),
    mDefVal(reParent(*this, mDefVal)),
    accessor(accessor) {}

PyrCallNodeBase::PyrCallNodeBase(Tag t, sc::lex::SourceCodeRange l, PyrParseNodeType classno):
    PyrParseNode(t, classno, l) {}

PyrCallNodeBase2::PyrCallNodeBase2(Tag tg, sc::lex::SourceCodeRange l, PyrParseNodeType t, PyrSlotNode* mSelector,
                                   PyrParseNode* mArglist, PyrParseNode* mKeyarglist):
    PyrCallNodeBase(tg, l, t),
    mSelector(reParent(*this, mSelector)),
    mArglist(reParent(*this, mArglist)),
    mKeyarglist(reParent(*this, mKeyarglist)) {}

PyrCallNode::PyrCallNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mArglist,
                         PyrParseNode* mKeyarglist):
    PyrCallNodeBase2(t, l, PyrParseNodeType::CallNode, mSelector, mArglist, mKeyarglist) {}

PyrBinopCallNode::PyrBinopCallNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* arglist):
    PyrCallNodeBase2(t, l, PyrParseNodeType::BinopCallNode, mSelector, arglist, nullptr) {}

PyrSetterNode::PyrSetterNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mExpr1,
                             PyrParseNode* mExpr2):
    PyrCallNodeBase(t, l, PyrParseNodeType::SetterNode),
    mSelector(reParent(*this, mSelector)),
    mExpr1(reParent(*this, mExpr1)),
    mExpr2(reParent(*this, mExpr2)) {}

PyrDynListNode::PyrDynListNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mClassname, PyrParseNode* mElems):
    PyrCallNodeBase(t, l, PyrParseNodeType::DynListNode),
    mClassname(reParent(*this, mClassname)),
    mElems(reParent(*this, mElems)) {}

PyrDynDictNode::PyrDynDictNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mElems):
    PyrCallNodeBase(t, l, PyrParseNodeType::DynDictNode),
    mElems(reParent(*this, mElems)) {}

PyrDropNode::PyrDropNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* e1, PyrParseNode* e2):
    PyrParseNode(t, PyrParseNodeType::DropNode, l),
    mExpr1(reParent(*this, e1)),
    mExpr2(reParent(*this, e2)) {}

PyrPushKeyArgNode::PyrPushKeyArgNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mSelector, PyrParseNode* mExpr):
    PyrParseNode(t, PyrParseNodeType::PushKeyArgNode, l),
    mSelector(reParent(*this, mSelector)),
    mExpr(reParent(*this, mExpr)) {}

PyrReturnNode::PyrReturnNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mExpr):
    PyrParseNode(t, PyrParseNodeType::ReturnNode, l),
    mExpr(reParent(*this, mExpr)) {}

PyrBlockReturnNode::PyrBlockReturnNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mExpr):
    PyrParseNode(t, PyrParseNodeType::BlockReturnNode, l),
    mExpr(reParent(*this, mExpr)) {}

PyrAssignNode::PyrAssignNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarName, PyrParseNode* mExpr):
    PyrParseNode(t, PyrParseNodeType::AssignNode, l),
    mVarName(reParent(*this, mVarName)),
    mExpr(reParent(*this, mExpr)) {}

PyrMultiAssignNode::PyrMultiAssignNode(Tag t, sc::lex::SourceCodeRange l, struct PyrMultiAssignVarListNode* mVarList,
                                       PyrParseNode* mExpr):
    PyrParseNode(t, PyrParseNodeType::MultiAssignNode, l),
    mVarList(reParent(*this, mVarList)),
    mExpr(reParent(*this, mExpr)) {}

PyrMultiAssignVarListNode::PyrMultiAssignVarListNode(Tag t, sc::lex::SourceCodeRange l, PyrSlotNode* mVarNames,
                                                     PyrSlotNode* mRest):
    PyrParseNode(t, PyrParseNodeType::MultiAssignVarListNode, l),
    mVarNames(reParent(*this, mVarNames)),
    mRest(reParent(*this, mRest)) {}

PyrBlockNode::PyrBlockNode(Tag t, sc::lex::SourceCodeRange l, struct PyrArgListNode* mArglist,
                           struct PyrVarListNode* mVarlist_in, PyrParseNode* mBody, bool mIsTopLevel):
    PyrParseNode(t, PyrParseNodeType::BlockNode, l),
    mArglist(reParent(*this, mArglist)),
    mVarlist(mVarlist_in),
    mBody(reParent(*this, mBody)),
    mIsTopLevel(mIsTopLevel) {
    joinVarLists(mVarlist);
    mVarlist = reParent(*this, mVarlist);
}

PyrArgListNode::PyrArgListNode(Tag t, sc::lex::SourceCodeRange l, struct PyrVarDefNode* mVarDefs, PyrSlotNode* mRest,
                               PyrSlotNode* mKeywordArgs):
    PyrParseNode(t, PyrParseNodeType::ArgListNode, l),
    mVarDefs(reParent(*this, mVarDefs)),
    mRest(reParent(*this, mRest)),
    mKeywordArgs(reParent(*this, mKeywordArgs)) {}

PyrLitListNode::PyrLitListNode(Tag t, sc::lex::SourceCodeRange l, PyrParseNode* mClassname, PyrParseNode* mElems):
    PyrParseNode(t, PyrParseNodeType::LitListNode, l),
    mClassname(reParent(*this, mClassname)),
    mElems(reParent(*this, mElems)) {}


PyrSlot deduceNameOfFunctionFromParent(PyrParseNode* node) {
    if (auto slot = nodeCast<PyrSlotNode>(node)) {
        return deduceNameOfFunctionFromParent(slot->mParent);
    } else if (auto vardef = nodeCast<PyrVarDefNode>(node)) {
        // Get the name from the variable or argument, if there was one.
        return vardef->mVarName->mSlot;
    } else if (auto call = nodeCast<PyrCallNode>(node)) {
        const auto msg = call->mSelector->mSlot.getSymbol();
        if (msg == s_envirPut) {
            // Get the name from the environment variable, if there was one.
            if (auto slot_node = nodeCast<PyrSlotNode>(call->mArglist)) {
                if (slot_node->mClassno == PyrParseNodeType::PushLitNode) {
                    return slot_node->mSlot;
                }
            }
        } else if (const auto sz = strlen(msg->name); msg->name[sz - 1] == '_') {
            // Setter: a.bang_({});
            // Name the function 'bang'.
            const auto name = getsymlen(msg->name, sz - 1);
            return PyrSlot::make(name);
        }
        // TODO: there are many many things that *could* be added here, what *ought* we do!
        // For example `Class({})`, in this case we could name the function after the argument as the call might (!) be
        // known at compile time.
    } else if (auto setter = nodeCast<PyrSetterNode>(node)) {
        // `foo.bar = {...}` In this case, we name the function 'bar'
        return setter->mSelector->mSlot;
    } else if (auto assign = nodeCast<PyrAssignNode>(node)) {
        return assign->mVarName->mSlot;
    }
    return PyrSlot {};
}


PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
PyrSymbol* gSpecialClasses[op_NumSpecialClasses];
SpecialValuesStruct gSpecialValues;
SpecialNumberStruct gSpecialNumbers;

int conjureConstantIndex(CompilerContext&, PyrParseNode* node, PyrBlock* func, PyrSlot* slot);
void compilePushConstant(CompilerContext& cxt, PyrParseNode* node, PyrSlot* slot);

// Forward declare helpers.
// This means they aren't a part of the public interface of the header.
void compileAnyIfMsg(CompilerContext& cxt, PyrCallNodeBase2* node);
void compileIfMsg(CompilerContext& cxt, PyrCallNodeBase2* node);
void compileIfNilMsg(CompilerContext& cxt, PyrCallNodeBase2* node, bool flag);
void compileCaseMsg(CompilerContext& cxt, PyrCallNodeBase2* node);
void compileWhileMsg(CompilerContext& cxt, PyrCallNodeBase2* node);
void compileLoopMsg(CompilerContext& cxt, PyrCallNodeBase2* node);
void compileAndMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2);
void compileOrMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2);
void compileQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2);
void compileQQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2);
void compileXQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2);
void compileSwitchMsg(CompilerContext& cxt, PyrCallNode* node);
void compileAssignVar(CompilerContext& cxt, PyrParseNode* node, PyrSymbol* varName, bool drop);
void compilePushVar(CompilerContext& cxt, PyrParseNode* node, PyrSymbol* varName);
bool isAnInlineableBlock(CompilerContext& cxt, PyrParseNode* node);
bool isAnInlineableAtomicLiteralBlock(PyrParseNode* node);
bool isAtomicLiteral(PyrParseNode* node);
bool isWhileTrue(PyrParseNode* node);

void compilePyrMethodNode(CompilerContext& cxt, PyrMethodNode* node, PyrSlot* result);
void compilePyrLiteralNode(CompilerContext& cxt, PyrSlotNode* node, PyrSlot* result);

PyrClass* getNodeSuperclass(CompilerContext& cxt, PyrClassNode* node);
void countNodeMethods(PyrClassNode* node, int* numClassMethods, int* numInstMethods);
void compileExtNodeMethods(CompilerContext& cxt, PyrClassExtNode* node);
void countVarDefs(PyrClassNode* node);
bool compareVarDefs(CompilerContext& cxt, PyrClassNode* node, PyrClass* classobj);
void recompileSubclasses(CompilerContext& cxt, PyrClass* classobj);
void compileNodeMethods(CompilerContext& cxt, PyrClassNode* node);
void fillClassPrototypes(CompilerContext& cxt, PyrClassNode* node, PyrClass* classobj, PyrClass* superclassobj);

bool isThisObjNode(PyrParseNode* node);
int conjureSelectorIndex(CompilerContext& cxt, PyrParseNode* node, PyrBlock* func, bool isSuper, PyrSymbol* selector,
                         int* selType);
Byte conjureLiteralSlotIndex(CompilerContext& cxt, PyrParseNode* node, PyrBlock* func, PyrSlot* slot);
bool findNamedIdentifier(CompilerContext& cxt, PyrBlock* func, PyrClass** classobj, PyrSymbol* name, int* varType,
                         int* level, int* index, PyrBlock** tempfunc);
void countClassVarDefs(PyrClassNode* node, int* numClassMethods, int* numInstMethods);
void dumpNodeList(PyrParseNode* node);
int compareCallArgs(CompilerContext& cxt, PyrMethodNode* node, PyrCallNode* cnode, int* varIndex,
                    PyrClass* specialClass);

bool findSpecialClassName(PyrSymbol* className, int* index);

CompilingBytecodes compileSubExpression(CompilerContext& cxt, PyrSlotNode* litnode, bool onTailBranch);
CompilingBytecodes compileSubExpressionWithGoto(CompilerContext& cxt, PyrSlotNode* litnode, unsigned int branchLen,
                                                bool onTailBranch);
CompilingBytecodes compileBodyWithGoto(CompilerContext& cxt, PyrParseNode* body, unsigned int branchLen,
                                       bool onTailBranch);


class SetTailIsMethodReturn {
    bool mSave;
    CompilerContext& cxt;

public:
    SetTailIsMethodReturn(CompilerContext& cxt, bool inValue): cxt(cxt) {
        mSave = cxt.tailIsMethodReturn;
        cxt.tailIsMethodReturn = inValue;
    }
    ~SetTailIsMethodReturn() { cxt.tailIsMethodReturn = mSave; }
};

void emitTailCall(CompilerContext& cxt, sc::lex::SourceCodeRange loc) {
    if (cxt.generateTailCallByteCodes && cxt.isTailCodeBranch) {
        if (cxt.tailIsMethodReturn)
            TailCallReturnFromMethod.emit(cxt.bytecodes, loc);
        else
            TailCallReturnFromFunction.emit(cxt.bytecodes, loc);
    }
}


void compileNodeList(CompilerContext& cxt, PyrParseNode* node, bool onTailBranch) {
    PyrSlot dummy;
    for (; node; node = node->mNext) {
        compileNode(cxt, node, &dummy, onTailBranch);
    }
}


std::optional<OpSpecialClassEnum> findSpecialClassFromName(PyrSymbol* className) {
    for (int i = 0; i < static_cast<int>(OpSpecialClassEnum::COUNT); ++i)
        if (gSpecialClasses[i] == className) {
            return static_cast<OpSpecialClassEnum>(i);
        }
    return std::nullopt;
}

struct FindVarNameResult {
    int level, index, varType;
    PyrBlock* tempfunc;
    PyrClass* classobj;
};

std::optional<FindVarNameResult> findVarName(CompilerContext& cxt, PyrBlock* func, PyrClass* classobjC,
                                             PyrSymbol* varName) {
    int level, index, varType;
    PyrClass* classobj = classobjC;
    PyrBlock* tempfunc;

    if (findNamedIdentifier(cxt, func, &classobj, varName, &varType, &level, &index, &tempfunc))
        return FindVarNameResult { level, index, varType, tempfunc, classobj };
    else
        return std::nullopt;
}

void compilePushVar(CompilerContext& cxt, PyrParseNode* node, PyrSymbol* varName) {
    if (std::isupper(varName->name[0])) {
        if (!cxt.textInfo->isClassFile && varName->u.classobj == nullptr) {
            cxt.postErrorInCurrentFile(node->location, "Undefined class", "This class has not been defined.");
        } else if (const auto specialClass = findSpecialClassFromName(varName)) {
            PushSpecialClass.emit(cxt.bytecodes, node->location, Operands::SpecialClass { *specialClass });
        } else {
            PyrSlot slot;
            SetSymbol(&slot, varName);
            PushClassX.emit(cxt.bytecodes, node->location,
                            Operands::Class { conjureLiteralSlotIndex(cxt, node, cxt.compilingBlock, &slot) });
        }

    } else if (varName == s_this || varName == s_super) {
        cxt.functionCantBeClosed = true;
        PushSpecialValueThis.emit(cxt.bytecodes, node->location);

    } else if (varName == s_true) {
        PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::True });
    } else if (varName == s_false) {
        PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::False });
    } else if (varName == s_nil) {
        PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
    } else if (const auto result = findVarName(cxt, cxt.compilingBlock, cxt.compilingClass, varName)) {
        const FindVarNameResult findResult = *result;
        switch (findResult.varType) {
        case varInst:
            PushInstVarX.emit(cxt.bytecodes, node->location, Operands::Index::fromRaw(findResult.index));
            break;

        case varClass: {
            const unsigned int indexOffset = findResult.index + slotRawInt(&findResult.classobj->classVarIndex);
            if (PushClassVar.validNibble(indexOffset))
                PushClassVar.emit(cxt.bytecodes, node->location, indexOffset);
            else
                PushClassVarX.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(indexOffset),
                                   Operands::UnsignedInt<16, 0>::fromFull(indexOffset));
        } break;

        case varConst: {
            PyrSlot* slot = slotRawObject(&findResult.classobj->constValues)->slots + findResult.index;
            compilePushConstant(cxt, node, slot);
        } break;

        case varTemp: {
            const auto vindex = findResult.index;
            if (findResult.level == 0) {
                if (PushTempZeroVar.validNibble(vindex)) {
                    PushTempZeroVar.emit(cxt.bytecodes, node->location, vindex);
                } else {
                    PushTempZeroVarX.emit(cxt.bytecodes, node->location, Operands::Index::fromRaw(vindex));
                }
            } else if (PushTempVar.validNibble(findResult.level))
                PushTempVar.emit(cxt.bytecodes, node->location, findResult.level, Operands::Index::fromRaw(vindex));
            else
                PushTempVarX.emit(cxt.bytecodes, node->location, Operands::FrameOffset::fromRaw(findResult.level),
                                  Operands::Index::fromRaw(vindex));
        } break;

        case varPseudo:
            SpecialOpcode.emit(cxt.bytecodes, node->location, Operands::PseudoVar::fromRaw(findResult.index));
            break;
        }
    } else {
        cxt.postErrorInCurrentFile(node->location, "Undefined variable", "This variable has not been defined.");
    }
}

void PyrCurryArgNode::compile(CompilerContext& cxt, PyrSlot* result) {
    if (cxt.compilingPartiallyAppliedFunction) {
        PushTempZeroVar.emit(cxt.bytecodes, location, mArgNum);
    } else {
        cxt.postErrorInCurrentFile(location, "In correct curry arg", "'_' is not allowed outside of a call.");
    }
}


void PyrSlotNode::compile(CompilerContext& cxt, PyrSlot* result) {
    if (mClassno == PyrParseNodeType::LiteralNode)
        compileLiteral(cxt, result);
    else if (mClassno == PyrParseNodeType::PushLitNode)
        compilePushLit(cxt, result);
    else if (mClassno == PyrParseNodeType::PushNameNode)
        compilePushVar(cxt, (PyrParseNode*)this, slotRawSymbol(&mSlot));
    else {
        cxt.postErrorInCurrentFile(location, "Compiler Error, please report.", "");
    }
}


void PyrClassExtNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrClass* classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
    if (!classobj) {
        cxt.postErrorInCurrentFile(location, "Undefined class.", "This class does not exist.");
        return;
    }
    cxt.currentClass = classobj;
    cxt.currentMetaClass = classobj->classptr;
    compileExtNodeMethods(cxt, this);
}

void compileExtNodeMethods(CompilerContext& cxt, PyrClassExtNode* node) {
    PyrMethodNode* method;
    method = node->mMethods;
    for (; method; method = (PyrMethodNode*)method->mNext) {
        PyrSlot dummy;
        method->mExtension = true;
        compilePyrMethodNode(cxt, method, &dummy);
    }
    cxt.compilingMethod = nullptr;
    cxt.compilingBlock = nullptr;
    cxt.compilingPartiallyAppliedFunction = nullptr;
}

bool compareVarDefs(CompilerContext& cxt, PyrClassNode* node, PyrClass* classobj) {
    PyrParseNode* errnode;

    const bool isIntrinsic = slotRawInt(&classobj->classFlags) & classIsIntrinsic;
    const auto numinstvars = numInstVars(classobj);
    const auto numclassvars = numClassVars(classobj);

    if (numinstvars == node->mVarTally[varInst] + node->mNumSuperInstVars
        && numclassvars == node->mVarTally[varClass]) {
        auto xclass = 0;
        auto xinst = node->mNumSuperInstVars;
        for (auto varlist = node->mVarlists; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
            int type = varlist->mFlags;
            if (type == varInst) {
                auto vardef = varlist->mVarDefs;
                auto varNames = slotRawSymbolArray(&classobj->instVarNames)->symbols;
                for (int i = 0; vardef; vardef = (PyrVarDefNode*)vardef->mNext, xinst++, ++i) {
                    if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xinst]) {
                        errnode = (PyrParseNode*)vardef;

                        goto differExit;
                    }
                }
            } else if (type == varClass) {
                auto vardef = varlist->mVarDefs;
                auto varNames = slotRawSymbolArray(&classobj->classVarNames)->symbols;
                for (int i = 0; vardef && xclass < numclassvars;
                     vardef = (PyrVarDefNode*)vardef->mNext, xclass++, ++i) {
                    if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xclass]) {
                        errnode = (PyrParseNode*)vardef;
                        goto differExit;
                    }
                }
            }
        }
    } else {
        errnode = (node->mVarlists ? (PyrParseNode*)node->mVarlists : (PyrParseNode*)node->mClassName);
        goto differExit;
    }
    return false;

differExit:
    if (isIntrinsic) {
        cxt.postErrorInCurrentFile(errnode->location, "Attempting to redefine an intrinsic class",
                                   "This class is defined by the compiler, you cannot change it.");
    }
    return true;
}

void countClassVarDefs(PyrClassNode* node, int* numClassMethods, int* numInstMethods) {
    PyrVarListNode* varlist;
    PyrVarDefNode* vardef;

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
                if (has_read(vardef->accessor)) {
                    *numClassMethods = *numClassMethods + 1;
                }
                if (has_write(vardef->accessor)) {
                    *numClassMethods = *numClassMethods + 1;
                }
            } else if (type == varInst) {
                if (has_read(vardef->accessor)) {
                    *numInstMethods = *numInstMethods + 1;
                }
                if (has_write(vardef->accessor)) {
                    *numInstMethods = *numInstMethods + 1;
                }
            }
        }
    }
}

void countNodeMethods(PyrClassNode* node, int* numClassMethods, int* numInstMethods) {
    // count methods
    PyrMethodNode* method;
    //*numClassMethods = 0;
    //*numInstMethods = 0;
    method = node->mMethods;
    for (; method; method = (PyrMethodNode*)method->mNext) {
        if (method->mIsClassMethod)
            *numClassMethods = *numClassMethods + 1;
        else
            *numInstMethods = *numInstMethods + 1;
    }
}

void compileNodeMethods(CompilerContext& cxt, PyrClassNode* node) {
    PyrMethodNode* method;
    method = node->mMethods;
    for (; method; method = (PyrMethodNode*)method->mNext) {
        PyrSlot dummy;
        method->mExtension = false;
        compilePyrMethodNode(cxt, method, &dummy);
    }
    cxt.compilingMethod = nullptr;
    cxt.compilingBlock = nullptr;
    cxt.compilingPartiallyAppliedFunction = nullptr;
}


PyrClass* getNodeSuperclass(PyrClassNode* compiling_class) {
    if (compiling_class->mSuperClassName && IsSym(&compiling_class->mSuperClassName->mSlot)) {
        if (auto superclassobj = slotRawSymbol(&compiling_class->mSuperClassName->mSlot)->u.classobj)
            return superclassobj;

        return nullptr;
    } else {
        return slotRawSymbol(&compiling_class->mClassName->mSlot) == s_abstract_object ? nullptr : class_object;
    }
}

void fillClassPrototypes(CompilerContext& cxt, PyrClassNode* node, PyrClass* classobj, PyrClass* superclassobj) {
    // copy superclass's prototype to here
    if (superclassobj && NotNil(&superclassobj->iprototype) && slotRawObject(&superclassobj->iprototype)->size) {
        memcpy(slotRawObject(&classobj->iprototype)->slots, slotRawObject(&superclassobj->iprototype)->slots,
               sizeof(PyrSlot) * slotRawObject(&superclassobj->iprototype)->size);
        // slotRawObject(&classobj->iprototype)->size = slotRawObject(&superclassobj->iprototype)->size;
        slotRawObject(&classobj->iprototype)->size = node->mNumSuperInstVars;

        memcpy(slotRawSymbolArray(&classobj->instVarNames)->symbols,
               slotRawSymbolArray(&superclassobj->instVarNames)->symbols,
               sizeof(PyrSymbol*) * slotRawObject(&superclassobj->instVarNames)->size);
        // slotRawObject(&classobj->instVarNames)->size = slotRawObject(&superclassobj->iprototype)->size;
        slotRawObject(&classobj->instVarNames)->size = node->mNumSuperInstVars;
    }

    // fill the class' own part of prototypes
    auto metaclassobj = classobj->classptr;
    auto varlist = node->mVarlists;

    PyrSlot *iprototype_it = nullptr, *cprototype_it = nullptr, *const_values_it = nullptr;
    PyrSymbol **inst_var_names_it = nullptr, **class_var_names_it = nullptr, **const_names_it = nullptr;
    if (NotNil(&classobj->iprototype)) {
        iprototype_it = slotRawObject(&classobj->iprototype)->slots + node->mNumSuperInstVars;
    }
    if (NotNil(&classobj->cprototype)) {
        cprototype_it = slotRawObject(&classobj->cprototype)->slots;
    }
    if (NotNil(&classobj->constValues)) {
        const_values_it = slotRawObject(&classobj->constValues)->slots;
    }
    if (NotNil(&classobj->instVarNames)) {
        inst_var_names_it = slotRawSymbolArray(&classobj->instVarNames)->symbols + node->mNumSuperInstVars;
    }
    if (NotNil(&classobj->classVarNames)) {
        class_var_names_it = slotRawSymbolArray(&classobj->classVarNames)->symbols;
    }
    if (NotNil(&classobj->constNames)) {
        const_names_it = slotRawSymbolArray(&classobj->constNames)->symbols;
    }
    auto instVarIndex = node->mNumSuperInstVars;
    int classVarIndex = 0;


    // prepends underscore
    const auto create_setter_name = [](PyrSymbol* name) {
        char setterName[256];
        sprintf(setterName, "%s_", name->name);
        return getsym(setterName);
    };
    // sets up method correctly
    const auto create_getter_or_setter_method =
        [&](unsigned char num_args, MethodType method_type, unsigned short speical_index, PyrClass* owner_class,
            PyrSlot method_name, sc::lex::SourceCodeRange loc, std::optional<PyrSlot> selector = std::nullopt) {
            assert(num_args > 0);
            auto* method = newPyrMethod();
            *METHRAW(method) = { 0,
                                 speical_index,
                                 static_cast<unsigned short>(method_type),
                                 0,
                                 0,
                                 num_args,
                                 0,
                                 0,
                                 num_args,
                                 0,
                                 static_cast<unsigned char>(num_args - 1),
                                 num_args };
            method->contextDef = PyrSlot {};
            method->varNames = PyrSlot {};
            method->ownerclass = PyrSlot::make(owner_class);
            method->name = method_name;
            method->filePath = PyrSlot::make(cxt.textInfo->filePathDescription);
            method->isClosed = PyrSlot::make(false);
            method->sourceCodeFileOrSnippet = PyrSlot::make(cxt.textInfo->normalisedSourceAsPyr);
            method->sourceCodeStartIndex = PyrSlot::make(static_cast<int>(loc.begin.absolute));
            method->sourceCodeEndIndex = PyrSlot::make(static_cast<int>(loc.end.absolute));
            if (selector)
                method->selectors = *selector;
            return method;
        };

    for (; varlist; varlist = (PyrVarListNode*)varlist->mNext) {
        const int type = varlist->mFlags;
        auto vardef = varlist->mVarDefs;
        switch (type) {
        case varInst: {
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode(cxt, (PyrSlotNode*)vardef->mDefVal, &litslot);
                *iprototype_it++ = litslot;
                slotRawObject(&classobj->iprototype)->size++;
                *inst_var_names_it++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->instVarNames)->size++;

                // create auto getters and setters
                if (has_read(vardef->accessor)) {
                    auto* meth = create_getter_or_setter_method(1, methReturnInstVar, instVarIndex, classobj,
                                                                vardef->mVarName->mSlot, vardef->mVarName->location);
                    addMethod(classobj, meth);
                }
                if (has_write(vardef->accessor)) {
                    auto* meth = create_getter_or_setter_method(
                        2, methAssignInstVar, instVarIndex, classobj,
                        PyrSlot::make(create_setter_name(vardef->mVarName->mSlot.getSymbol())),
                        vardef->mVarName->location);
                    addMethod(classobj, meth);
                }
                instVarIndex++;
            }
        }; break;


        case varClass: {
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode(cxt, (PyrSlotNode*)vardef->mDefVal, &litslot);
                *cprototype_it++ = litslot;
                slotRawObject(&classobj->cprototype)->size++;
                *class_var_names_it++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->classVarNames)->size++;
                if (has_read(vardef->accessor)) {
                    auto* meth = create_getter_or_setter_method(
                        1, methReturnClassVar, classVarIndex + classobj->classVarIndex.getInt(), metaclassobj,
                        vardef->mVarName->mSlot, vardef->mVarName->location,

                        classobj->name);
                    addMethod(metaclassobj, meth);
                }

                if (has_write(vardef->accessor)) {
                    auto* meth = create_getter_or_setter_method(
                        2, methAssignClassVar, classVarIndex + classobj->classVarIndex.getInt(), metaclassobj,
                        PyrSlot::make(create_setter_name(vardef->mVarName->mSlot.getSymbol())),
                        vardef->mVarName->location, classobj->name);
                    addMethod(metaclassobj, meth);
                }
                classVarIndex++;
            }
        }; break;

        case varConst: {
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode(cxt, (PyrSlotNode*)vardef->mDefVal, &litslot);
                *const_values_it++ = litslot;
                slotRawObject(&classobj->constValues)->size++;
                *const_names_it++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->constNames)->size++;
                if (has_read(vardef->accessor)) {
                    auto* meth =
                        create_getter_or_setter_method(1, methReturnLiteral, 0, metaclassobj, vardef->mVarName->mSlot,
                                                       vardef->mVarName->location, litslot);
                    addMethod(metaclassobj, meth);
                }
            }
        }; break;
        }
    }


    // The next section of code deals with printing errors for duplicate variables inside of classes.

    auto findDuplicateName =
        [names = std::vector<PyrSymbol*>()](const PyrSymbolArray* array) mutable -> std::optional<PyrSymbol*> {
        names.clear();
        if (array == nullptr || array->size == 0)
            return std::nullopt; // can be null, meaning, empty.

        names.insert(names.end(), array->symbols, array->symbols + array->size);
        std::sort(names.begin(), names.end());
        const auto maybe_duplicate = std::adjacent_find(names.begin(), names.end());

        return (maybe_duplicate != names.end()) ? std::optional<PyrSymbol*> { *maybe_duplicate } : std::nullopt;
    };

    const auto get_duplicate_locations =
        [&](const PyrSymbol* duplicate,
            int varFlagType) -> std::variant<std::monostate, PyrParseNode*, std::tuple<PyrParseNode*, PyrParseNode*>> {
        PyrParseNode* first { nullptr };
        for (auto varlist = node->mVarlists; varlist; varlist = static_cast<PyrVarListNode*>(varlist->mNext)) {
            if (varlist->mFlags == varFlagType) {
                for (auto def = varlist->mVarDefs; def; def = static_cast<PyrVarDefNode*>(def->mNext)) {
                    const auto varName = def->mVarName->mSlot;
                    assert(varName.isSymbol());
                    if (varName.getSymbol() == duplicate) {
                        if (first) {
                            return { std::tuple { first, def } };
                        } else {
                            first = def;
                        }
                    }
                }
            }
        }

        if (first)
            return { first };
        return std::monostate {};
    };

    const auto print_duplicate_error = [&](const PyrSymbol* duplicate, int varFlagType) {
        const auto loc = get_duplicate_locations(duplicate, varInst);
        if (const auto r = std::get_if<std::monostate>(&loc)) {
            // Both duplicates where delcared in a super class, no need to print anything here.
            return;
        }


        if (const auto r = std::get_if<PyrParseNode*>(&loc)) {
            cxt.postErrorInCurrentFile(
                node->mClassName->location, "Duplicate identifier within a class heirachy.",
                "This identifier was also declared in a superclass, please rename it, or delete it.", { { 3, 16, 0 } });
            // TODO: how do we find the other duplicate?
        } else if (const auto r = std::get_if<std::tuple<PyrParseNode*, PyrParseNode*>>(&loc)) {
            // Both duplicates where in this class.
            const auto [a, b] = *r;

            const PyrParseNode *first, *last;
            if (a->location.begin.absolute < b->location.begin.absolute) {
                first = a;
                last = b;
            } else {
                first = b;
                last = a;
            }

            const auto f_vardef = nodeCast<PyrVarDefNode>(first);
            const auto l_vardef = nodeCast<PyrVarDefNode>(last);

            const auto className = classobj->name.getSymbol()->name;
            const auto msg = std::string { "Duplicate identifier within the class heirachy of '" } + className + "'.";

            const DiagnosticHighlight hg[2] {
                cxt.textInfo->createDiagnosticHighlight(f_vardef->mVarName->location, "Duplicate here..."),
                cxt.textInfo->createDiagnosticHighlight(l_vardef->mVarName->location, "... and here.")
            };
            const auto str = diagnosticToCompilerError(ErrorType::Error, msg.c_str(), hg, 2);
            cxt.postError(str);
        }
    };

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->instVarNames))) {
        print_duplicate_error(*duplicate, varInst);
    }

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->classVarNames))) {
        print_duplicate_error(*duplicate, varClass);
    }

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->constNames))) {
        print_duplicate_error(*duplicate, varConst);
    }
}


void PyrClassNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrClass *classobj, *metaclassobj;
    int numClassMethods, numInstMethods;
    bool isIntrinsic;
    bool varsDiffer, superclassesDiffer, indexTypesDiffer;
    bool shouldRecompileSubclasses = false;

    PyrClass* superclassobj;
    { // assign to superclassobj, can produce errors and early return from the function.
        if (this->mClassName->mSlot.getSymbol() == s_abstract_object) {
            if (mSuperClassName) {
                cxt.postErrorInCurrentFile(mSuperClassName->location, "Cannot change intrinsic class.",
                                           "'AbstractObject' cannot have a superclass");
                return;
            }
            // This is the acceptable case where superclassobject can be nullptr, Object truly does not have one.
            superclassobj = nullptr;
        } else if (mSuperClassName) {
            if (!mSuperClassName->mSlot.isSymbol()) {
                assert(false); // this is a part of the grammar and should not happen.
                cxt.postErrorInCurrentFile(mSuperClassName->location, "Compilation error",
                                           "Expected a class name token here");
                return;
            }
            const auto super_name = mSuperClassName->mSlot.getSymbol();
            if (auto maybe_super_class = super_name->u.classobj) {
                superclassobj = maybe_super_class;
            } else {
                cxt.postErrorInCurrentFile(mSuperClassName->location, "Non existance class",
                                           "This class does not exist.");
                return;
            }
        } else {
            // No explicit super class, assume it is object.
            superclassobj = class_object;
        }
    }

    const auto indexType = [&]() {
        if (!mIndexType)
            return obj_notindexed;
        const char* name = mIndexType->mSlot.getSymbol()->name;
        if (strcmp(name, "slot") == 0)
            return obj_slot;
        else if (strcmp(name, "double") == 0)
            return obj_double;
        else if (strcmp(name, "float") == 0)
            return obj_float;
        else if (strcmp(name, "int32") == 0)
            return obj_int32;
        else if (strcmp(name, "int16") == 0)
            return obj_int16;
        else if (strcmp(name, "int8") == 0)
            return obj_int8;
        else if (strcmp(name, "char") == 0)
            return obj_char;
        else if (strcmp(name, "symbol") == 0)
            return obj_symbol;
        else {
            cxt.postErrorInCurrentFile(mClassName->location, "Invalid index type.",
                                       "Must be one of: slot, double, float, int8, int16, int32, or char.");
            return obj_slot;
        }
    }();


    mNumSuperInstVars = numSuperInstVars(superclassobj);

    numClassMethods = 0;
    numInstMethods = 0;
    countClassVarDefs(this, &numClassMethods, &numInstMethods);
    // postfl("accessor methods %d %d\n", numClassMethods, numInstMethods);
    countNodeMethods(this, &numClassMethods, &numInstMethods);
    // postfl("total methods %d %d\n", numClassMethods, numInstMethods);

    // get or make a class object
    // see if it already exists
    classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
    if (classobj) {
        // deal with intrinsic classes or other classes being recompiled here.
        // recompile of subclasses not necessary if inst and class vars are
        // unchanged.
        metaclassobj = (PyrClass*)classobj->classptr;
        isIntrinsic = slotRawInt(&classobj->classFlags) & classIsIntrinsic;

        varsDiffer = compareVarDefs(cxt, this, classobj);
        if (varsDiffer) {
            if (isIntrinsic) {
                cxt.postErrorInCurrentFile(mClassName->location, "Attempt to change intrinsic class.",
                                           "You cannot change the variables of an instrinsic class, the compiler is "
                                           "expecting a certain layout.");
                return;
            } else {
                shouldRecompileSubclasses = true;
            }
        }
        superclassesDiffer = superclassobj != slotRawSymbol(&classobj->superclass)->u.classobj;
        indexTypesDiffer = indexType != slotRawInt(&classobj->instanceFormat);
        // postfl("%d %d %d\n", indexType, slotRawInt(&classobj->instanceFormat));
        // if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
        if (varsDiffer || superclassesDiffer || indexTypesDiffer) {
            if (isIntrinsic) {
                if (superclassesDiffer) {
                    const auto t = mSuperClassName ? mSuperClassName : mClassName;
                    std::string msg { "This is an intrinsic class, you cannot change the superclass, expected '" };
                    msg += slotRawSymbol(&classobj->superclass)->name;
                    msg += "'.";

                    cxt.postErrorInCurrentFile(mSuperClassName ? mSuperClassName->location : mClassName->location,
                                               "Attempt to change intrinsic class.", msg.c_str());
                }
                if (indexTypesDiffer) {
                    const auto t = indexType ? (PyrParseNode*)mIndexType : (PyrParseNode*)mClassName;

                    cxt.postErrorInCurrentFile(
                        t->location, "Attempt to change intrinsic class.",
                        "This is an intrinsic class, you cannot change the index type (in square brackets).");
                }
                cxt.postErrorInCurrentFile(mClassName->location, "Attempt to change intrinsic class.",
                                           "This class cannot be altered, it is required by the compiler.");
                return;
            } else {
                shouldRecompileSubclasses = true;
            }
        }
        // reallocate fields in the class object
        reallocClassObj(metaclassobj, classClassNumInstVars, 0, 0, numClassMethods, indexType, 0);

        // postfl("^3 %d %d\n", metaclassobj, class_class);
        // postfl("^4 %d %d\n", slotRawObject(&metaclassobj->iprototype), slotRawObject(&class_class->iprototype));
        memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
               sizeof(PyrSlot) * classClassNumInstVars);
        memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
               slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);
        slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
        slotRawSymbolArray(&metaclassobj->instVarNames)->size = classClassNumInstVars;

        reallocClassObj(classobj, mVarTally[varInst] + mNumSuperInstVars, mVarTally[varClass], mVarTally[varConst],
                        numInstMethods, indexType, 0);

    } else {
        PyrSymbol *superClassName, *metaClassName, *metaSuperClassName;

        superClassName = superclassobj ? slotRawSymbol(&superclassobj->name) : nullptr;
        metaClassName = getmetasym(slotRawSymbol(&mClassName->mSlot)->name);
        metaClassName->flags |= sym_MetaClass;
        metaSuperClassName = superClassName ? getmetasym(superClassName->name) : nullptr;

        metaclassobj = newClassObj(class_class, metaClassName, metaSuperClassName, classClassNumInstVars, 0, 0,
                                   numClassMethods, indexType, 0);
        memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
               sizeof(PyrSlot) * classClassNumInstVars);
        memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
               slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);
        slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
        slotRawObject(&metaclassobj->instVarNames)->size = classClassNumInstVars;
        classobj = newClassObj(metaclassobj, slotRawSymbol(&mClassName->mSlot), superClassName,
                               mVarTally[varInst] + mNumSuperInstVars, mVarTally[varClass], mVarTally[varConst],
                               numInstMethods, indexType, 0);
        mClassName->mSlot.getSymbol()->u.classobj = classobj;
        metaClassName->u.classobj = metaclassobj;
    }
    cxt.currentClass = classobj;
    cxt.currentMetaClass = metaclassobj;
    classobj->filenameSym = PyrSlot::make(cxt.textInfo->filePathDescription);
    classobj->charPos = PyrSlot::make(static_cast<int>(mClassName->location.begin.absolute));
    metaclassobj->filenameSym = PyrSlot::make(cxt.textInfo->filePathDescription);
    metaclassobj->charPos = PyrSlot::make(static_cast<int>(mClassName->location.begin.absolute));

    // fill inst and class prototypes
    fillClassPrototypes(cxt, this, classobj, superclassobj);

    // compile methods
    compileNodeMethods(cxt, this);

    // recompileSubclasses
    if (shouldRecompileSubclasses) {
        recompileSubclasses(cxt, classobj);
    }
}

void recompileSubclasses(CompilerContext& cxt, PyrClass* classobj) {
    // TODO: why does this do nothing?
}

enum { push_Normal, push_AllArgs, push_AllButFirstArg, push_AllButFirstArg2 };

int checkPushAllArgs(CompilerContext& cxt, PyrParseNode* actualArg, int numArgs) {
    auto block = cxt.compilingBlock;

    // if (strcmp("ar", slotRawSymbol(&cxt.compiling_method->name)->name)==0) Debugger();
    if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
        if (numArgs < 3) {
            return push_Normal;
        }
        actualArg = actualArg->mNext;
        for (int i = 1; i < numArgs; ++i) {
            if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
                return push_Normal;
            }
            auto nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }

            actualArg = actualArg->mNext;
        }
        return push_AllButFirstArg;

    } else {
        for (int i = 0; i < numArgs; ++i) {
            if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
                return push_Normal;
            }
            auto nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }
            actualArg = actualArg->mNext;
        }
        return push_AllArgs;
    }
}


int checkPushAllButFirstTwoArgs(CompilerContext& cxt, PyrParseNode* actualArg, int numArgs) {
    auto block = cxt.compilingBlock;

    if (numArgs >= 2) {
        actualArg = actualArg->mNext;
        actualArg = actualArg->mNext;
        for (int i = 1; i < numArgs; ++i) {
            if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
                return push_Normal;
            }
            auto nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }

            actualArg = actualArg->mNext;
        }
        return push_AllButFirstArg2;
    }
    return push_Normal;
}

// cannot have variadic args and kwargs
// returns methNormal if no optimisation could be performed.
int tryToOptimiseCall(CompilerContext& cxt, PyrMethodNode* node, PyrCallNode* cnode, int* varIndex,
                      PyrClass** specialClass) {
    int i, numFormalArgs, numActualArgs;
    int special, varType, varLevel;
    PyrParseNode* actualArg;
    PyrVarDefNode* formalArg;
    PyrSlotNode* nameNode;

    // fail if has a rest arg .. too much trouble?
    if (node->mArglist && node->mArglist->mRest) {
        return methNormal;
    }

    // check first actual arg is 'this'
    actualArg = cnode->mArglist;
    if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
        return methNormal;
    }
    nameNode = (PyrSlotNode*)actualArg;
    if (slotRawSymbol(&nameNode->mSlot) == s_this) {
        special = methRedirect;
    } else if (slotRawSymbol(&nameNode->mSlot) == s_super) {
        special = methRedirectSuper;
    } else {
        bool varFound;
        PyrClass* classobj;

        classobj = cxt.compilingClass;
        varFound = findNamedIdentifier(cxt, cxt.compilingBlock, &classobj, slotRawSymbol(&nameNode->mSlot), &varType,
                                       &varLevel, varIndex, nullptr);
        if (!varFound)
            return methNormal;

        if (varType == varInst)
            special = methForwardInstVar;
        else if (varType == varClass) {
            special = methForwardClassVar;
            *varIndex += slotRawInt(&classobj->classVarIndex);
            *specialClass = classobj;
        } else
            return methNormal;
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
        for (i = 0; i < numActualArgs - 1; ++i) {
            if (actualArg->mClassno != PyrParseNodeType::PushNameNode) {
                return methNormal;
            }

            nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbol(&formalArg->mVarName->mSlot)) {
                return methNormal;
            }

            formalArg = (PyrVarDefNode*)formalArg->mNext;
            actualArg = actualArg->mNext;
        }
    }

    return special;
}


struct CompiledBytecodeInfoForBlock {
    PyrInt8Array* codes;
    PyrInt32Array* codeLocations;
    PyrInt8Array* codeSizes;
};

[[nodiscard]] std::optional<CompiledBytecodeInfoForBlock> finaliseBytecodes(CompilerContext& cxt) {
    auto data = std::move(cxt.bytecodes).finish();

    const auto length = data.codes.size();
    if (length == 0) {
        return std::nullopt;
    }

    const auto flags = allocationFlags(cxt);

    // bytecodes
    auto byteArray = newPyrInt8Array(compileGC(cxt), length, flags, false);
    memcpy(byteArray->b, data.codes.data(), length * sizeof(Byte));
    byteArray->size = static_cast<int>(length);

    // bytecode locations
    const auto locLength = data.startAndEndLocations.size();
    auto locArray = newPyrInt32Array(compileGC(cxt), locLength, flags, false);
    memcpy(locArray->i, data.startAndEndLocations.data(), locLength * sizeof(uint32_t));
    locArray->size = static_cast<int>(locLength);

    // bytecode sizes --- almost no codes take up a single byte.
    const auto codeSizesLength = data.sizeOfCodes.size();
    auto szArray = newPyrInt8Array(compileGC(cxt), codeSizesLength, flags, false);
    memcpy(szArray->b, data.sizeOfCodes.data(), codeSizesLength * sizeof(uint8_t));
    szArray->size = codeSizesLength;
    return { { byteArray, locArray, szArray } };
}


PyrMethod* initPyrMethod(PyrMethod* method);

void compilePyrMethodNode(CompilerContext& cxt, PyrMethodNode* node, PyrSlot* result) { node->compile(cxt, result); }

template <typename T> struct defer {
    defer(T&& t): t(t) {}
    defer(defer&&) = delete;
    defer(const defer&) = delete;
    defer& operator=(defer&&) = delete;
    defer& operator=(const defer&) = delete;
    ~defer() { t(); }

private:
    T t;
};

void postDuplicateIdentiferError(CompilerContext& cxt, sc::lex::SourceCodeRange first,
                                 sc::lex::SourceCodeRange second) {
    const DiagnosticHighlight hg[2] {
        cxt.textInfo->createDiagnosticHighlight(first, "This identifier is duplicated..."),
        cxt.textInfo->createDiagnosticHighlight(second, "...here. Names must be unique, rename one of these."),
    };
    const auto str = diagnosticToCompilerError(ErrorType::Error, "Duplicate named indentifier", hg, 2);
    cxt.postError(str);
}

void checkRedefiningPseudo(CompilerContext& cxt, sc::lex::SourceCodeRange loc, PyrSymbol* name) {
    const std::array reserved { s_this, s_curProcess, s_curMethod, s_curBlock, s_curClosure, s_curThread, s_super };
    if (auto fnd = std::find(reserved.begin(), reserved.end(), name); fnd != reserved.end()) {
        cxt.postErrorInCurrentFile(loc, "Redefining a special identifier.",
                                   "please use a different name for this identifier as this is a reserved name.",
                                   { { 3, 16, 0 } });
    }
}

PyrSlot createArgNames(CompilerContext& cxt, std::size_t size, bool hasThis, PyrArgListNode* arglist,
                       std::unordered_map<PyrSymbol*, sc::lex::SourceCodeRange>& identifiers) {
    if (hasThis && size == 1) {
        return o_argnamethis;
    }
    if (size == 0 || !arglist) {
        assert(!hasThis);
        return PyrSlot {};
    }
    const auto flags = allocationFlags(cxt);
    auto names = newPyrSymbolArray(compileGC(cxt), size, flags, false);
    names->size = size;

    std::size_t counter { 0 };

    if (hasThis) {
        names->symbols[counter] = s_this;
        counter += 1;
    }

    const auto insertAndCheck = [&](PyrSymbol* name, sc::lex::SourceCodeRange loc) {
        checkRedefiningPseudo(cxt, loc, name);
        if (auto fnd = identifiers.find(name); fnd != identifiers.end())
            postDuplicateIdentiferError(cxt, fnd->second, loc);
        else
            identifiers.insert({ name, loc });
        names->symbols[counter] = name;
        counter += 1;
    };

    for (auto arg { arglist->mVarDefs }; arg; arg = nodeCast<PyrVarDefNode>(arg->mNext))
        insertAndCheck(arg->mVarName->mSlot.getSymbol(), arg->location);


    if (auto slotNode = arglist->mRest)
        insertAndCheck(slotNode->mSlot.getSymbol(), slotNode->location);


    if (auto slotNode = arglist->mKeywordArgs)
        insertAndCheck(slotNode->mSlot.getSymbol(), slotNode->location);

    assert(size == counter);

    return PyrSlot::make(names);
}

PyrSlot createVarNames(CompilerContext& cxt, std::size_t size, PyrVarListNode* varlist,
                       std::unordered_map<PyrSymbol*, sc::lex::SourceCodeRange>& identifiers) {
    if (size == 0 || !varlist)
        return PyrSlot {};

    const auto flags = allocationFlags(cxt);
    auto names = newPyrSymbolArray(compileGC(cxt), size, flags, false);
    names->size = size;

    std::size_t counter { 0 };

    const auto insertAndCheck = [&](PyrSymbol* name, sc::lex::SourceCodeRange loc) {
        checkRedefiningPseudo(cxt, loc, name);
        if (auto fnd = identifiers.find(name); fnd != identifiers.end())
            postDuplicateIdentiferError(cxt, fnd->second, loc);
        else
            identifiers.insert({ name, loc });
        names->symbols[counter] = name;
        counter += 1;
    };

    for (auto var { varlist->mVarDefs }; var; var = nodeCast<PyrVarDefNode>(var->mNext))
        insertAndCheck(var->mVarName->mSlot.getSymbol(), var->location);

    assert(size == counter);
    return PyrSlot::make(names);
}

// returns slot with result, and a bool indicating whether default argument/variable values require compilatio (i.e.,
// are not constant expressions).
std::tuple<PyrSlot, bool, bool> createPrototypeFrame(CompilerContext& cxt, std::size_t size, bool hasThis,
                                                     PyrArgListNode* argList, PyrVarListNode* vars) {
    if (hasThis && size == 1) {
        return { o_onenilarray, false, false };
    }
    if (size == 0) {
        assert(!hasThis);
        return { PyrSlot {}, false, false };
    }

    const auto flags = allocationFlags(cxt);
    auto p = newPyrArray(compileGC(cxt), size, flags, false);
    p->size = size;
    std::size_t counter { 0 };

    if (hasThis) {
        p->slots[counter] = PyrSlot {};
        counter += 1;
    }

    bool argsRequiresCompilation { false };
    bool varsRequiresCompilation { false };

    if (argList) {
        for (auto arg { argList->mVarDefs }; arg; arg = nodeCast<PyrVarDefNode>(arg->mNext)) {
            PyrSlot lit {};
            argsRequiresCompilation |= arg->hasExpr(cxt, &lit);
            p->slots[counter] = lit;
            counter += 1;
        }
        if (auto args = argList->mRest) {
            p->slots[counter] = o_emptyarray;
            counter += 1;
        }
        if (auto kwargs = argList->mKeywordArgs) {
            p->slots[counter] = o_emptyarray;
            counter += 1;
        }
    }
    if (vars) {
        for (auto var { vars->mVarDefs }; var; var = nodeCast<PyrVarDefNode>(var->mNext)) {
            PyrSlot lit {};
            varsRequiresCompilation |= var->hasExpr(cxt, &lit);
            p->slots[counter] = lit;
            counter += 1;
        }
    }
    assert(counter == size);

    return { PyrSlot::make(p), argsRequiresCompilation, varsRequiresCompilation };
}


void PyrMethodNode::compile(CompilerContext& cxt, PyrSlot* result) {
    const auto flags = allocationFlags(cxt);

    // we add 1 because of `this`.
    const std::size_t numPositionalArgs = 1 + (mArglist ? nodeListLength(mArglist->mVarDefs) : 0);
    const std::size_t numVariadicArgs = (mArglist && mArglist->mRest) ? (mArglist->mKeywordArgs ? 2 : 1) : 0;
    const std::size_t numTotalArguments = numPositionalArgs + numVariadicArgs;
    const std::size_t numVariables = mVarlist ? nodeListLength(mVarlist->mVarDefs) : 0;
    const std::size_t numSlots = numTotalArguments + numVariables;
    const bool hasPrimitive = mPrimitiveName != nullptr;

    if (numTotalArguments > 255)
        cxt.postErrorInCurrentFile(mArglist->location, "Too many arguments.",
                                   "Too many arguments, max of 255, use an IdentityDictionary instead.");
    if (numVariables > 255)
        cxt.postErrorInCurrentFile(mVarlist->location, "Too many variable.",
                                   "Too many variables, max of 255, use an IdentityDictionary instead.");

    // If this isn't nullptr then we have a duplicate, that is okay if this is an extention.
    // TODO: (FUTURE) It is probably better to check this in a previous compiler pass.
    auto oldmethod =
        classFindDirectMethod(mIsClassMethod ? cxt.currentMetaClass : cxt.currentClass, mMethodName->mSlot.getSymbol());

    // Not an extension and oldmethod exist, we are redefining an already defined method. This is an error, bail.
    if (oldmethod && !mExtension) {
        const PyrMethodNode* other;
        // Because this is not an extention, we always have a class above us.
        // If it was, it would be a PyrClassExtNode
        auto classNode = nodeCast<PyrClassNode>(this->mParent);
        for (auto it = classNode->mMethods; it; it = static_cast<PyrMethodNode*>(it->mNext)) {
            if (it != this && it->mMethodName->mSlot == mMethodName->mSlot) {
                other = it;
                break;
            }
        }
        const PyrParseNode *first, *last;
        if (other->location.begin.absolute < location.begin.absolute) {
            first = other;
            last = mMethodName;
        } else {
            last = other;
            first = mMethodName;
        }
        const DiagnosticHighlight hg[2] {
            cxt.textInfo->createDiagnosticHighlight(first->location, "Method declared here..."),
            cxt.textInfo->createDiagnosticHighlight(last->location,
                                                    "... was redclared here. Each method must have a unique name."),
        };

        const auto str = diagnosticToCompilerError(ErrorType::Error, "Duplicate method.", hg, 2);
        cxt.postError(str);
        return;
    }


    // MUTABLE VARIABLES {

    // This function builds methods
    auto method = oldmethod == nullptr ? newPyrMethod() : [&]() {
        gClassLibraryInfo.incrementMethodOverrideCount();
        // accumulate overwrite message onto the string buffer
        std::string msg;
        msg.append(slotRawSymbol(&slotRawClass(&oldmethod->ownerclass)->name)->name)
            .append(":")
            .append(slotRawSymbol(&oldmethod->name)->name)
            .append("\t")
            .append(cxt.textInfo->filePathDescription.name)
            .append("\t")
            .append(slotRawSymbol(&oldmethod->filePath)->name)
            .append("\n");
        gClassLibraryInfo.appendToOverwriteMsg(std::move(msg));

        // recycle old method
        freePyrSlot(&oldmethod->code);
        freePyrSlot(&oldmethod->selectors);
        freePyrSlot(&oldmethod->prototypeFrame);
        freePyrSlot(&oldmethod->argNames);
        freePyrSlot(&oldmethod->varNames);
        initPyrMethod(oldmethod);
        return oldmethod;
    }();

    auto methraw = METHRAW(method);

    // All the argument and variable names, they must be unique
    std::unordered_map<PyrSymbol*, sc::lex::SourceCodeRange> namedIdentifiers {};

    // } MUTABLE VARIABLES

    // out arg
    *result = PyrSlot::make(method);


    // Mutate global state in prep for compiling.

    // These cannot contain nested methods.
    assert(cxt.compilingMethod == nullptr);
    assert(cxt.compilingBlock == nullptr);
    assert(cxt.compilingPartiallyAppliedFunction == nullptr);

    cxt.compilingClass = mIsClassMethod ? cxt.currentMetaClass : cxt.currentClass;
    cxt.compilingMethod = method;
    cxt.compilingBlock = method;

    // Put it back at the end of the scope.
    defer d { [&]() {
        if (!oldmethod) {
            addMethod(cxt.compilingClass, method);
        }
        cxt.compilingMethod = nullptr;
        cxt.compilingBlock = nullptr;
        cxt.compilingPartiallyAppliedFunction = nullptr;
    } };

    method->ownerclass = PyrSlot::make(cxt.compilingClass);
    *methraw = PyrMethodRaw { 0,
                              0,
                              hasPrimitive ? methPrimitive : methNormal, // might be mutated later.
                              static_cast<unsigned short>((numSlots + FRAMESIZE) * sizeof(PyrSlot)),
                              0,
                              static_cast<unsigned char>(numPositionalArgs),
                              static_cast<unsigned char>(numVariadicArgs),
                              static_cast<unsigned char>(numVariables),
                              static_cast<unsigned char>(numSlots),
                              0, // might be mutated later.
                              static_cast<unsigned char>(numSlots - 1), // don't pop `this`.
                              static_cast<unsigned char>(numTotalArguments) };

    method->contextDef = PyrSlot {}; // not a closure, the syntax isn't designed for this.
    method->name = mMethodName->mSlot;
    method->filePath = PyrSlot::make(cxt.textInfo->filePathDescription);
    method->isClosed = PyrSlot::make(false);
    method->sourceCodeStartIndex = PyrSlot::make(static_cast<int>(location.begin.absolute));
    method->sourceCodeEndIndex = PyrSlot::make(static_cast<int>(location.end.absolute));
    method->sourceCodeFileOrSnippet = PyrSlot::make(cxt.textInfo->normalisedSourceAsPyr);

    method->argNames = createArgNames(cxt, numTotalArguments, true, mArglist, namedIdentifiers);
    method->varNames = createVarNames(cxt, numVariables, mVarlist, namedIdentifiers);
    const auto [proto, a, v] = createPrototypeFrame(cxt, numSlots, true, mArglist, mVarlist);
    // C++ 20 is required to capture a and v in lambdas.
    const auto argsRequiresCompilation = a;
    const auto varsRequiresCompilation = v;
    method->prototypeFrame = proto;

    method->primitiveName = hasPrimitive ? mPrimitiveName->mSlot : PyrSlot {};
    // NOTE: this is not the special index of the symbol.
    methraw->specialIndex = hasPrimitive ? mPrimitiveName->mSlot.getSymbol()->u.index : 0;
    // Many conditions prevent methods from being optimised.
    const auto mightBeOptimised = [&]() {
        if (argsRequiresCompilation || varsRequiresCompilation)
            return false;

        if (hasPrimitive)
            return false;

        // All the variadic arguments are on the stack, to optimise this, we need to turn them into an array.
        // This could be done in the future.
        if (mArglist && mArglist->mRest)
            return false;

        if (mMethodName->mSlot.getSymbol() == s_doesNotUnderstand)
            return false;

        if (auto c = nodeCast<PyrClassNode>(mParent)) {
            if (c && c->mClassName->mSlot.getSymbol() == getsym("Interpreter")
                && mMethodName->mSlot.getSymbol() == getsym("functionCompileContext")) {
                return false;
            }
        }
        // class ext node, shouldn't occur really
        if (auto c = nodeCast<PyrClassExtNode>(mParent)) {
            if (c && c->mClassName->mSlot.getSymbol() == getsym("Interpreter")
                && mMethodName->mSlot.getSymbol() == getsym("functionCompileContext")) {
                return false;
            }
        }

        return true;
    }();


    // compile body
    cxt.bytecodes.assertEmpty();

    // If it is optimised, it doesn't get bytecode.
    const auto wasOptimised = [&]() -> bool {
        if (!mightBeOptimised)
            return false;

        if (auto anode = nodeCast<PyrAssignNode>(mBody)) {
            if (anode->mNext && anode->mNext->mClassno == PyrParseNodeType::ReturnNode
                && ((PyrReturnNode*)anode->mNext)->mExpr == nullptr) {
                int index;
                if (classFindInstVar(cxt.compilingClass, slotRawSymbol(&anode->mVarName->mSlot), &index)) {
                    methraw->methType = methAssignInstVar;
                    methraw->specialIndex = index;
                    return true;
                }
            }
        } else if (auto rnode = nodeCast<PyrReturnNode>(mBody)) {
            auto xnode = rnode->mExpr;
            if (!xnode) {
                // empty, this shouldn't be possible in the grammar, but does make sense here.
                methraw->methType = methReturnSelf;
                return true;
            }

            switch (xnode->mClassno) {
            default:
                return false;

            case PyrParseNodeType::PushLitNode: {
                PyrSlot rslot;
                compilePyrLiteralNode(cxt, (PyrSlotNode*)xnode, &rslot);

                if (!(IsObj(&rslot) && slotRawObject(&rslot)->classptr == class_fundef)) {
                    methraw->methType = methReturnLiteral;
                    method->selectors = rslot;
                    return true;
                }
                return false;
            }

            case PyrParseNodeType::PushNameNode: {
                PyrSlot* rslot;
                rslot = &((PyrSlotNode*)xnode)->mSlot;
                if (slotRawSymbol(rslot) == s_this) {
                    methraw->methType = methReturnSelf;
                    return true;
                }
                int index { 0 };
                if (funcFindArg((PyrBlock*)method, slotRawSymbol(rslot), &index)) { // return arg ?
                    assert(index < methraw->totalNumberArguments);
                    methraw->methType = methReturnArg;
                    methraw->specialIndex = index;
                    return true;
                } else if (classFindInstVar(cxt.compilingClass, slotRawSymbol(rslot), &index)) {
                    // return inst var
                    methraw->methType = methReturnInstVar;
                    methraw->specialIndex = index;
                    return true;
                }

                return false;
            }

            case PyrParseNodeType::CallNode: {
                // need to do this for binary opcodes too..
                int specialIndex;
                auto cnode = nodeCast<PyrCallNode>(xnode);
                assert(cnode);
                PyrClass* specialClass = nullptr;
                // The optimization breaks when there are keyword arguments in the call.
                if (cnode->mKeyarglist)
                    return false;

                const auto type = tryToOptimiseCall(cxt, this, cnode, &specialIndex, &specialClass);

                if (type == methNormal)
                    return false;

                methraw->methType = type;
                methraw->specialIndex = specialIndex;
                method->selectors = cnode->mSelector->mSlot;
                if (specialClass)
                    method->constants = specialClass->name;
                return true;
            }
            }
        }

        return false;
    }();

    if (hasPrimitive) {
        auto prim = gPrimitiveTable.table[methraw->specialIndex];
        if (prim.func != undefinedPrimitive) {
            if (prim.numNormalArguments != numPositionalArgs) {
                const DiagnosticHighlight hg[2] {
                    cxt.textInfo->createDiagnosticHighlight(mArglist->location, "The argument count here... "),
                    cxt.textInfo->createDiagnosticHighlight(mPrimitiveName->location,
                                                            "...should match the argugment count of this primitive."),
                };
                const auto str = diagnosticToCompilerError(ErrorType::Error, "Primitive argument mismatch", hg, 2);
                cxt.postError(str);
            }

            if (prim.hasVariablePositionalArguments && methraw->numVariableArguments < 1) {
                const DiagnosticHighlight hg[2] {
                    cxt.textInfo->createDiagnosticHighlight(mArglist->mVarDefs->mTail->location,
                                                            "Insert variadic postional arguments: `...args`... "),
                    cxt.textInfo->createDiagnosticHighlight(mPrimitiveName->location,
                                                            "... to match the defintion of this primitive."),
                };
                const auto str = diagnosticToCompilerError(ErrorType::Error, "Primitive argument mismatch", hg, 2);
                cxt.postError(str);
            }

            if (prim.hasVariableKeywordArguments && methraw->numVariableArguments < 2) {
                const DiagnosticHighlight hg[2] {
                    cxt.textInfo->createDiagnosticHighlight(mArglist->mVarDefs->mTail->location,
                                                            "Insert variadic keyword arguments: `...args, kwargs`... "),
                    cxt.textInfo->createDiagnosticHighlight(mPrimitiveName->location,
                                                            "... to match the defintion of this primitive."),
                };
                const auto str = diagnosticToCompilerError(ErrorType::Error, "Primitive argument mismatch", hg, 2);
                cxt.postError(str);
            }
        }
    }

    if (!wasOptimised)
        [&]() {
            assert(methraw->methType == methNormal || methraw->methType == methPrimitive);
            SetTailBranch branch(cxt, false); // set to true in the return method node
            SetTailIsMethodReturn mr { cxt, false };

            // These optimisations are special bytecodes that get emitted instead of looking at the source.
            // TODO: this means the source should be deleted and we should construct them here, or better yet, right at
            // the top of this function so all this nasty logic can be abstracted.

            const auto checkArgsAndVarsAreSimple = [&]() {
                PyrVarDefNode* badArg { nullptr };
                if (argsRequiresCompilation)
                    for (auto arg { mArglist->mVarDefs }; arg; arg = nodeCast<PyrVarDefNode>(arg->mNext)) {
                        PyrSlot dummy {};
                        if (arg->hasExpr(cxt, &dummy)) {
                            badArg = arg;
                            break;
                        }
                    }
                if (badArg) {
                    cxt.postErrorInCurrentFile(
                        badArg->mDefVal->location, "Non simple arg in optimised method.",
                        "This method is optimised by the compiler, you cannot have non-literal arguments.");
                }
                badArg = nullptr;
                if (varsRequiresCompilation)
                    for (auto var { mVarlist->mVarDefs }; var; var = nodeCast<PyrVarDefNode>(var->mNext)) {
                        PyrSlot dummy {};
                        if (var->hasExpr(cxt, &dummy)) {
                            badArg = var;
                            break;
                        }
                    }
                if (badArg) {
                    cxt.postErrorInCurrentFile(
                        badArg->mDefVal->location, "Non simple var in body.",
                        "This method is optimised by the compiler, you cannot have non-literal variable defaults.");
                }
            };

            const auto checkArgsVars = [&](std::size_t numArgs, std::size_t numVars) {
                if (slotRawSymbolArray(&method->argNames)->size != numArgs) {
                    std::stringstream ss;
                    ss << "There should be " << numArgs - 1 << " arguments here.";
                    cxt.postErrorInCurrentFile(mArglist ? mArglist->location : mMethodName->location,
                                               "Incorrect number of arguments in optimised method.", ss.str());
                }
                if (slotRawSymbolArray(&method->varNames)->size != numVars) {
                    std::stringstream ss;
                    ss << "There should be " << numVars << " variables here.";
                    cxt.postErrorInCurrentFile(mVarlist ? mVarlist->location : mBody->location,
                                               "Incorrect number of variables in optimised method.", ss.str());
                }
            };

            if (cxt.compilingClass == class_int) {
                const PyrSymbol* name = slotRawSymbol(&method->name);
                if (name == gSpecialSelectors[opmDo]) {
                    Extended::IntegerDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 1);
                    return;
                } else if (name == gSpecialSelectors[opmReverseDo]) {
                    Extended::IntegerReverseDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 2);
                    return;
                } else if (name == gSpecialSelectors[opmFor]) {
                    Extended::IntegerFor.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(3, 3);
                    return;
                } else if (name == gSpecialSelectors[opmForBy]) {
                    Extended::IntegerForBy.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(4, 2);
                    return;
                }
            } else if (cxt.compilingClass == class_arrayed_collection) {
                const PyrSymbol* name = slotRawSymbol(&method->name);
                if (name == gSpecialSelectors[opmDo]) {
                    Extended::ArrayedCollectionDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 1);
                    return;
                } else if (name == gSpecialSelectors[opmReverseDo]) {
                    Extended::ArrayedCollectionReversedDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 2);
                    return;
                }
            } else if (slotRawSymbol(&cxt.compilingClass->name) == s_dictionary) {
                const PyrSymbol* name = slotRawSymbol(&method->name);
                if (name == getsym("keysValuesArrayDo")) {
                    Extended::DictionaryKeyValuesArrayDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(3, 5);
                    return;
                }
            } else if (cxt.compilingClass == class_number) {
                const PyrSymbol* name = slotRawSymbol(&method->name);
                if (name == gSpecialSelectors[opmForSeries]) {
                    Extended::NumberForSeries.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(4, 2);
                    return;
                }
            } else if (cxt.compilingClass == class_float) {
                const PyrSymbol* name = slotRawSymbol(&method->name);
                if (name == gSpecialSelectors[opmDo]) {
                    Extended::FloatDo.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 1);
                    return;
                } else if (name == gSpecialSelectors[opmReverseDo]) {
                    Extended::FloatDoReverse.emit(cxt.bytecodes, location);
                    checkArgsAndVarsAreSimple();
                    checkArgsVars(2, 2);
                    return;
                }
            }

            // Normal method compilation.

            if (argsRequiresCompilation)
                for (auto arg { mArglist->mVarDefs }; arg; arg = nodeCast<PyrVarDefNode>(arg->mNext)) {
                    PyrSlot dummy {};
                    arg->compileArg(cxt, &dummy);
                }


            if (varsRequiresCompilation)
                for (auto var { mVarlist->mVarDefs }; var; var = nodeCast<PyrVarDefNode>(var->mNext)) {
                    PyrSlot dummy {};
                    var->compile(cxt, &dummy);
                }

            PyrSlot dummy {};
            compileNode(cxt, mBody, &dummy, true);
        }();

    if (auto maybe_codes = finaliseBytecodes(cxt)) {
        const auto [codes, codeLocations, codeSizes] = *maybe_codes;
        method->code = PyrSlot::make(codes);
        method->codeLocations = PyrSlot::make(codeLocations);
        method->codeSizes = PyrSlot::make(codeSizes);
    } else {
        method->code = PyrSlot {};
        method->codeLocations = PyrSlot {};
        method->codeSizes = PyrSlot {};
    }
}


void PyrArgListNode::compile(CompilerContext& cxt, PyrSlot* result) { assert(false); }


void PyrVarListNode::compile(CompilerContext& cxt, PyrSlot* result) { assert(false); }


bool PyrVarDefNode::hasExpr(CompilerContext& cxt, PyrSlot* result) {
    if (result)
        SetNil(result);
    if (!mDefVal)
        return false;
    if (mDefVal->mClassno != PyrParseNodeType::PushLitNode && mDefVal->mClassno != PyrParseNodeType::LiteralNode) {
        // post("hasExpr A %s:%s %s %d\n", slotRawSymbol(&cxt.compiling_class->name)->name,
        // slotRawSymbol(&cxt.compiling_method->name)->name, mVarName->slotRawSymbol(&mSlot)->name,
        // mDefVal->mClassno);
        return true;
    }
    PyrSlotNode* node = (PyrSlotNode*)mDefVal;

    if (IsPtr(&node->mSlot)) {
        PyrParseNode* litnode = (PyrParseNode*)slotRawPtr(&node->mSlot);
        if (litnode) {
            if (litnode->mClassno == PyrParseNodeType::BlockNode) {
                // post("hasExpr B %s:%s %s %d\n", slotRawSymbol(&cxt.compiling_class->name)->name,
                // slotRawSymbol(&cxt.compiling_method->name)->name, mVarName->slotRawSymbol(&mSlot)->name,
                // node->mClassno);
                return true;
            } else {
                if (result)
                    node->compileLiteral(cxt, result);
            }
        }
    } else if (result)
        *result = node->mSlot;
    if (node->mParens)
        return true;
    return false;
}

void PyrVarDefNode::compile(CompilerContext& cxt, PyrSlot* result) {
    if (hasExpr(cxt, nullptr)) {
        compileNode(cxt, mDefVal, result, false);
        compileAssignVar(cxt, (PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
    }
}

void PyrVarDefNode::compileArg(CompilerContext& cxt, PyrSlot* result) {
    if (hasExpr(cxt, nullptr)) {
        compilePushVar(cxt, (PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot));

        mDrop = false;
        auto trueByteCodes = compileBodyWithGoto(cxt, this, 0, true);
        const unsigned int jumplen = static_cast<unsigned int>(trueByteCodes.length());

        Extended::IfNotNilJumpPushNilElsePop.emit(
            cxt.bytecodes, location,
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });

        cxt.bytecodes.consume(std::move(trueByteCodes));
        Drop.emit(cxt.bytecodes, location);
    }
}

int PyrCallNode::isPartialApplication() {
    int sum = 0;
    PyrParseNode* argnode = mArglist;
    for (; argnode; argnode = argnode->mNext) {
        if (argnode->mClassno == PyrParseNodeType::CurryArgNode) {
            ((PyrCurryArgNode*)argnode)->mArgNum = sum;
            sum++;
        }
    }

    PyrParseNode* keynode = mKeyarglist;
    for (; keynode; keynode = keynode->mNext) {
        if (keynode->mClassno == PyrParseNodeType::CurryArgNode) {
            ((PyrCurryArgNode*)keynode)->mArgNum = sum;
            sum++;
        }
    }
    return sum;
}

void PyrCallNodeBase::compilePartialApplication(CompilerContext& cxt, unsigned char numCurryArgs, PyrSlot* result) {
    const auto flags = allocationFlags(cxt);


    // This function builds this block
    auto block = newPyrBlock(!cxt.textInfo->isClassFile, flags);

    { // This scope is where we enter the block and emit bytecodes to it.

        // Old bytecodes from block above us.
        // At the end of this function we will emit the block into them.
        auto savedBytecodes { std::move(cxt.bytecodes) };
        const auto prevFunctionHighestExternalRef = cxt.functionHighestExternalRef;
        const auto prevFunctionCantBeClosed = cxt.functionCantBeClosed;
        cxt.functionHighestExternalRef = 0;
        cxt.functionCantBeClosed = false;
        const auto prevBlock = cxt.compilingBlock;
        cxt.compilingBlock = block;
        auto* prevPartiallyAppliedFunction = cxt.compilingPartiallyAppliedFunction;
        cxt.compilingPartiallyAppliedFunction = block;

        // put everything back
        defer d { [&]() {
            // NOTE: we don't put the bytecodes back here as we need to emit the partial app function into the parent
            // bytecodes.
            cxt.compilingBlock = prevBlock;
            cxt.compilingPartiallyAppliedFunction = prevPartiallyAppliedFunction;
            cxt.functionCantBeClosed = cxt.functionCantBeClosed || prevFunctionCantBeClosed;
            cxt.functionHighestExternalRef =
                std::max(cxt.functionHighestExternalRef - 1, prevFunctionHighestExternalRef);

            cxt.bytecodes = std::move(savedBytecodes);
        } };

        *METHRAW(block) = { 0,
                            0,
                            methBlock,
                            static_cast<unsigned short>((numCurryArgs + FRAMESIZE) * sizeof(PyrSlot)),
                            0,
                            numCurryArgs,
                            0,
                            0,
                            numCurryArgs,
                            0,
                            numCurryArgs,
                            numCurryArgs };

        block->sourceCodeFileOrSnippet = PyrSlot::make(cxt.textInfo->normalisedSourceAsPyr);
        block->name = deduceNameOfFunctionFromParent(this->mParent);
        block->filePath = PyrSlot::make(cxt.textInfo->filePathDescription);
        block->sourceCodeStartIndex = PyrSlot::make(static_cast<int>(location.begin.absolute));
        block->sourceCodeEndIndex = PyrSlot::make(static_cast<int>(location.end.absolute));
        block->fileLocation = cxt.textInfo->isClassFile ? PyrSlot {} : [&]() {
            auto ar = newPyrArray(compileGC(cxt), 2, 0, false);
            ar->size = 2;
            ar->slots[0] = PyrSlot::make(static_cast<int>(cxt.textInfo->offsetInFile.line_number));
            ar->slots[1] = PyrSlot::make(static_cast<int>(cxt.textInfo->offsetInFile.column));
            return PyrSlot::make(ar);
        }();

        PyrObject* proto = newPyrArray(compileGC(cxt), numCurryArgs, flags, false);
        proto->size = numCurryArgs;
        block->prototypeFrame = PyrSlot::make(proto);

        PyrSymbolArray* argNames = newPyrSymbolArray(compileGC(cxt), numCurryArgs, flags, false);
        argNames->size = numCurryArgs;
        block->argNames = PyrSlot::make(argNames);
        block->varNames = PyrSlot {};
        block->contextDef = PyrSlot::make(prevBlock);

        {
            PyrSymbol* s_empty = getsym("_");
            PyrSymbol** blockargs = block->argNames.getPyrObjType<PyrSymbolArray>()->symbols;
            for (int i = 0; i < numCurryArgs; ++i) {
                // put it in mArglist
                blockargs[i] = s_empty;
                proto->slots[i] = PyrSlot {};
            }
        }

        cxt.bytecodes.assertEmpty();
        {
            SetTailBranch branch { cxt, true };
            SetTailIsMethodReturn mr { cxt, false };
            PyrSlot body;
            compileCall(cxt, &body);
        }

        BlockReturn.emit(cxt.bytecodes, location);
        if (auto c = finaliseBytecodes(cxt)) {
            const auto [codes, codeLocations, codeSizes] = *c;
            block->code = PyrSlot::make(codes);
            block->codeLocations = PyrSlot::make(codeLocations);
            block->codeSizes = PyrSlot::make(codeSizes);
        } else {
            block->code = PyrSlot {};
            block->codeLocations = PyrSlot {};
            block->codeSizes = PyrSlot {};
        }

        const auto isClosed = (!cxt.functionCantBeClosed && cxt.functionHighestExternalRef == 0);

        block->contextDef = isClosed ? PyrSlot {} : PyrSlot::make(prevBlock);
        if (isClosed) {
            METHRAW(prevBlock)->needsHeapContext = 1;
        }
    } // exit the block, now back to the current compiling block/method

    auto blockSlot = PyrSlot::make(block);
    PushLiteralX.emit(cxt.bytecodes, location,
                      Operands::Index::fromRaw(conjureLiteralSlotIndex(cxt, this, cxt.compilingBlock, &blockSlot)));
}

void PyrCallNodeBase::compile(CompilerContext& cxt, PyrSlot* result) {
    int numCurryArgs = isPartialApplication();
    if (numCurryArgs) {
        compilePartialApplication(cxt, numCurryArgs, result);
    } else {
        compileCall(cxt, result);
    }
}

bool isSeries(PyrParseNode* node, PyrParseNode** args) {
    if (node->mClassno != PyrParseNodeType::CallNode)
        return false;
    PyrCallNode* callnode = (PyrCallNode*)node;
    if (slotRawSymbol(&callnode->mSelector->mSlot) != s_series)
        return false;
    if (callnode->mKeyarglist)
        return false;
    *args = callnode->mArglist;
    return true;
}

void PyrCallNode::compileCall(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;
    PyrParseNode* argnode2;

    PyrParseNode* argnode = mArglist;
    PyrParseNode* keynode = mKeyarglist;

    int numArgs = nodeListLength(argnode);
    const int numKeyArgs = nodeListLength(keynode);
    const int isSuper = isSuperObjNode(argnode);
    const int numBlockArgs = METHRAW(cxt.compilingBlock)->totalNumberArguments;

    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;

    int selType;
    auto selectorSlotOrSpecialIndex = conjureSelectorIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, isSuper,
                                                           slotRawSymbol(&mSelector->mSlot), &selType);

    if (numKeyArgs > 0 || (numArgs > 15 && !(selType == selSwitch || selType == selCase))) {
        for (; argnode; argnode = argnode->mNext)
            compileNode(cxt, argnode, &dummy, false);
        for (; keynode; keynode = keynode->mNext)
            compileNode(cxt, keynode, &dummy, false);

        if (isSuper) {
            emitTailCall(cxt, location);
            assert(selType == selNormal);
            SendSuperMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                               Operands::KwArgumentCount::fromRaw(numKeyArgs),
                               Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
        } else {
            switch (selType) {
            case selNormal:
                // When the selector type is normal, conjureSelectorIndex has added the symbol to the functiondef's
                // selector array and we just send a normal message.
                emitTailCall(cxt, location);
                SendMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                              Operands::KwArgumentCount::fromRaw(numKeyArgs),
                              Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                break;

            case selUnary:
                [[fallthrough]];
            case selBinary: {
                // When the selector is of the type unary or binary, no selector has been emited to the function
                // def. This is because it is indented to be called with special bytes codes for the unary and
                // binary message format respectively, however, these do not take kwargs. Therefore, we put the
                // selector into the function def and use its index for a normal message send.
                const auto selectorSlotIndex =
                    conjureLiteralSlotIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, &mSelector->mSlot);
                emitTailCall(cxt, location);
                SendMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                              Operands::KwArgumentCount::fromRaw(numKeyArgs),
                              Operands::SelectorIndex::fromRaw(selectorSlotIndex));
                break;
            }

            default:
                // In this case, the selector is a special one, and we can use the send special message.
                emitTailCall(cxt, location);
                SendSpecialMsgX.emit(cxt.bytecodes, location,
                                     Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                                     Operands::KwArgumentCount::fromRaw(numKeyArgs),
                                     Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                break;
            }
        }
    } else if (isSuper) {
        if (numArgs == 1) {
            // No need to compile the 'this' arg.
            cxt.functionCantBeClosed = true;
            emitTailCall(cxt, location);
            SendSuperMsgThisOpt.emit(cxt.bytecodes, location,
                                     Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
        } else {
            for (; argnode; argnode = argnode->mNext)
                compileNode(cxt, argnode, &dummy, false);
            emitTailCall(cxt, location);
            if (SendSuperMsg.validNibble(numArgs)) {
                SendSuperMsg.emit(cxt.bytecodes, location, numArgs,
                                  Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else {
                SendSuperMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs),
                                   Operands::KwArgumentCount::fromRaw(0),
                                   Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
        }

    } else {
        PyrSymbol* varname = (argnode->mClassno == PyrParseNodeType::PushNameNode)
            ? slotRawSymbol(&((PyrSlotNode*)argnode)->mSlot)
            : nullptr;

        if (varname == s_this)
            cxt.functionCantBeClosed = true;

        switch (selType) {
        case selNormal: {
            if (numArgs == 1 && varname == s_this) {
                emitTailCall(cxt, location);
                SendMsgThisOpt.emit(cxt.bytecodes, location,
                                    Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else if (numArgs > 1 && numArgs == numBlockArgs) {
                switch (checkPushAllArgs(cxt, argnode, numArgs)) {
                case push_Normal:
                    goto normal;

                case push_AllArgs: {
                    emitTailCall(cxt, location);
                    PushAllArgsAndSendMsg.emit(cxt.bytecodes, location,
                                               Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                case push_AllButFirstArg: {
                    compileNode(cxt, argnode, &dummy, false);
                    emitTailCall(cxt, location);
                    PushAllButFirstArgAndSendMsg.emit(cxt.bytecodes, location,
                                                      Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto normal;
                }

            } else if (numArgs > 2 && numArgs == numBlockArgs + 1) {
                switch (checkPushAllButFirstTwoArgs(cxt, argnode, numBlockArgs)) {
                case push_Normal:
                    goto normal;

                case push_AllButFirstArg2: {
                    compileNode(cxt, argnode, &dummy, false);
                    compileNode(cxt, argnode->mNext, &dummy, false);
                    emitTailCall(cxt, location);
                    PushAllButFirstTwoArgsAndSendMsg.emit(cxt.bytecodes, location,
                                                          Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto normal;
                }

            } else {
            normal:
                for (; argnode; argnode = argnode->mNext)
                    compileNode(cxt, argnode, &dummy, false);
                emitTailCall(cxt, location);

                if (SendMsg.validNibble(numArgs))
                    SendMsg.emit(cxt.bytecodes, location, numArgs,
                                 Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs),
                                  Operands::KwArgumentCount::fromRaw(0),
                                  Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
        } break;

        case selSpecial:
            if (numArgs == 1) {
                if (varname == s_this) {
                    emitTailCall(cxt, location);
                    SendSpecialMsgThisOpt.emit(cxt.bytecodes, location,
                                               Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } else if (varname) {
                    if (const auto result = findVarName(cxt, cxt.compilingBlock, cxt.compilingClass, varname);
                        result && result->varType == varInst) {
                        emitTailCall(cxt, location);
                        PushInstVarAndSendSpecialMsg.emit(cxt.bytecodes, location,
                                                          Operands::Index::fromRaw(result->index),
                                                          Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                    } else
                        goto special;

                } else
                    goto special;

            } else if (selectorSlotOrSpecialIndex == opmDo && isSeries(argnode, &argnode)) {
                selectorSlotOrSpecialIndex = opmForSeries;
                mArglist = linkNodes(argnode, mArglist->mNext);
                numArgs = nodeListLength(mArglist);
                goto special;

            } else if (numArgs > 1 && numArgs == numBlockArgs) {
                switch (checkPushAllArgs(cxt, argnode, numArgs)) {
                case push_Normal:
                    goto special;

                case push_AllArgs: {
                    emitTailCall(cxt, location);
                    PushAllArgsAndSendSpecialMsg.emit(cxt.bytecodes, location,
                                                      Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                case push_AllButFirstArg: {
                    compileNode(cxt, argnode, &dummy, false);
                    emitTailCall(cxt, location);
                    PushAllButFirstArgAndSendSpecialMsg.emit(cxt.bytecodes, location,
                                                             Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto special;
                }

            } else if (numArgs > 2 && numArgs == numBlockArgs + 1) {
                switch (checkPushAllArgs(cxt, argnode, numBlockArgs)) {
                case push_Normal:
                    goto special;

                case push_AllButFirstArg2: {
                    compileNode(cxt, argnode, &dummy, false);
                    compileNode(cxt, argnode->mNext, &dummy, false);
                    emitTailCall(cxt, location);
                    PushAllButFirstTwoArgsAndSendSpecialMsg.emit(cxt.bytecodes, location,
                                                                 Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto special;
                }

            } else {
            special:
                for (; argnode; argnode = argnode->mNext)
                    compileNode(cxt, argnode, &dummy, false);
                emitTailCall(cxt, location);
                if (SendSpecialMsg.validNibble(numArgs))
                    SendSpecialMsg.emit(cxt.bytecodes, location, numArgs,
                                        Operands::SpecialSelectors::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendSpecialMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs),
                                         Operands::KwArgumentCount::fromRaw(0),
                                         Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;

        case selUnary: {
            if (numArgs != 1) {
                selectorSlotOrSpecialIndex =
                    conjureLiteralSlotIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, &mSelector->mSlot);
                goto defaultCase;
            }
            for (; argnode; argnode = argnode->mNext)
                compileNode(cxt, argnode, &dummy, false);

            emitTailCall(cxt, location);
            SendSpecialUnaryArithMsgX.emit(cxt.bytecodes, location,
                                           Operands::UnaryMath::fromRaw(selectorSlotOrSpecialIndex));
        } break;

        case selBinary:
            if (numArgs != 2) {
                selectorSlotOrSpecialIndex =
                    conjureLiteralSlotIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, &mSelector->mSlot);
                goto defaultCase;
            }
            argnode2 = argnode->mNext;
            if (selectorSlotOrSpecialIndex == static_cast<int>(OpBinaryMath::Add)
                && argnode2->mClassno == PyrParseNodeType::PushLitNode && IsInt(&((PyrSlotNode*)argnode2)->mSlot)
                && slotRawInt(&((PyrSlotNode*)argnode2)->mSlot) == 1) {
                compileNode(cxt, argnode, &dummy, false);
                PushOneAndAddOne.emit(cxt.bytecodes, location);
            } else if (selectorSlotOrSpecialIndex == opSub && argnode2->mClassno == PyrParseNodeType::PushLitNode
                       && IsInt(&((PyrSlotNode*)argnode2)->mSlot)
                       && slotRawInt(&((PyrSlotNode*)argnode2)->mSlot) == 1) {
                compileNode(cxt, argnode, &dummy, false);
                PushOneAndSubtract.emit(cxt.bytecodes, location);
            } else {
                compileNode(cxt, argnode, &dummy, false);
                compileNode(cxt, argnode->mNext, &dummy, false);
                emitTailCall(cxt, location);
                if (selectorSlotOrSpecialIndex < 16)
                    SendSpecialBinaryArithMsg.emit(cxt.bytecodes, location,
                                                   Operands::BinaryMathNibble::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendSpecialBinaryArithMsgX.emit(cxt.bytecodes, location,
                                                    Operands::BinaryMath::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;

        case selIf:
            compileAnyIfMsg(cxt, this);
            break;

        case selCase:
            compileCaseMsg(cxt, this);
            break;

        case selSwitch:
            compileSwitchMsg(cxt, this);
            break;

        case selWhile:
            compileWhileMsg(cxt, this);
            break;

        case selLoop:
            compileLoopMsg(cxt, this);
            break;

        case selAnd:
            if (numArgs == 2)
                compileAndMsg(cxt, argnode, argnode->mNext);
            else
                goto special;
            break;

        case selOr:
            if (numArgs == 2)
                compileOrMsg(cxt, argnode, argnode->mNext);
            else
                goto special;
            break;

        case selQuestionMark:
            if (numArgs == 2)
                compileQMsg(cxt, argnode, argnode->mNext);
            break;

        case selDoubleQuestionMark:
            if (numArgs == 2)
                compileQQMsg(cxt, argnode, argnode->mNext);
            break;

        case selExclamationQuestionMark:
            if (numArgs == 2)
                compileXQMsg(cxt, argnode, argnode->mNext);
            break;

        default:
        defaultCase:
            if (numArgs == 1 && varname == s_this) {
                emitTailCall(cxt, location);
                SendMsgThisOpt.emit(cxt.bytecodes, location,
                                    Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else {
                for (; argnode; argnode = argnode->mNext)
                    compileNode(cxt, argnode, &dummy, false);

                emitTailCall(cxt, location);
                if (SendMsg.validNibble(numArgs))
                    SendMsg.emit(cxt.bytecodes, location, numArgs,
                                 Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendMsgX.emit(cxt.bytecodes, location, Operands::ArgumentCount::fromRaw(numArgs),
                                  Operands::KwArgumentCount::fromRaw(0),
                                  Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;
        }
    }
}

CompilingBytecodes compileSubExpression(CompilerContext& cxt, PyrSlotNode* litnode, bool onTailBranch) {
    return compileSubExpressionWithGoto(cxt, litnode, 0, onTailBranch);
}

CompilingBytecodes compileSubExpressionWithGoto(CompilerContext& cxt, PyrSlotNode* litnode, unsigned int branchLen,
                                                bool onTailBranch) {
    auto* bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
    return compileBodyWithGoto(cxt, bnode->mBody, branchLen, onTailBranch);
}

CompilingBytecodes compileBodyWithGoto(CompilerContext& cxt, PyrParseNode* body, unsigned int branchLen,
                                       bool onTailBranch) {
    PyrSlot dummy;

    auto* prevPartiallyAppliedFunction = cxt.compilingPartiallyAppliedFunction;
    cxt.compilingPartiallyAppliedFunction = nullptr;

    auto oldByteCodes = std::move(cxt.bytecodes);
    CompilingBytecodes bytes;

    compileNode(cxt, body, &dummy, onTailBranch);
    if (branchLen) {
        if (cxt.bytecodes.length() == 0) {
            PushSpecialValue.emit(cxt.bytecodes, body->location, { OpSpecialValue::Nil_ });
        }
        JumpFwd.emit(cxt.bytecodes, body->location, Operands::UnsignedInt<16, 1>::fromFull(branchLen),
                     Operands::UnsignedInt<16, 0>::fromFull(branchLen));
    }

    auto thisBytecodes = std::move(cxt.bytecodes);
    cxt.bytecodes = std::move(oldByteCodes);

    cxt.compilingPartiallyAppliedFunction = prevPartiallyAppliedFunction;

    return thisBytecodes;
}

void print_inline_warning(CompilerContext& cxt, PyrParseNode* args, PyrParseNode* vars) {
    if (args) {
        cxt.postErrorInCurrentFile(args->location, "Function contains arguments, it cannot be inlined.",
                                   "remove this to make the function inlinable.");
    }
    if (vars) {
        cxt.postErrorInCurrentFile(vars->location, "Function contains variables, it cannot be inlined.",
                                   "remove this to make the function inlineable.");
    }
}

bool isAnInlineableBlock(CompilerContext& cxt, PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == PyrParseNodeType::PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot)
            && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == PyrParseNodeType::BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                gClassLibraryInfo.incrementNumUninlinedFunctions();
                if (SC_LanguageConfig::getPostInlineWarnings()) {
                    print_inline_warning(cxt, bnode->mArglist, bnode->mVarlist);
                }
            } else
                res = true;
        }
    }
    return res;
}

bool isAnInlineableAtomicLiteralBlock(CompilerContext& cxt, PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == PyrParseNodeType::PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot)
            && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == PyrParseNodeType::BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                gClassLibraryInfo.incrementNumUninlinedFunctions();
                if (SC_LanguageConfig::getPostInlineWarnings()) {
                    print_inline_warning(cxt, bnode->mArglist, bnode->mVarlist);
                }
            } else {
                if (bnode->mBody->mClassno == PyrParseNodeType::DropNode
                    && ((PyrDropNode*)bnode->mBody)->mExpr2->mClassno == PyrParseNodeType::BlockReturnNode)
                    res = isAtomicLiteral(((PyrDropNode*)bnode->mBody)->mExpr1);
                else
                    res = false;
            }
        }
    }
    return res;
}

bool isAtomicLiteral(PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == PyrParseNodeType::PushLitNode) {
        PyrSlotNode* anode;
        anode = (PyrSlotNode*)node;
        if (NotObj(&anode->mSlot) && !IsPtr(&anode->mSlot))
            res = true;
    }
    return res;
}

enum struct UninlinableWarningOption { PostWarning, DontPostWarning };

/// Return the value of a literal, allows literal to be wrap in a single pair of curly braces.
/// Will post a warning by default if it can't produce a value and is a block.
template <UninlinableWarningOption Warning = UninlinableWarningOption::PostWarning>
std::optional<PyrSlot> getAtomicValueFromLiteralOrBlockMaybePostWarning(CompilerContext& cxt,
                                                                        const PyrParseNode& node) {
    if (node.mClassno != PyrParseNodeType::PushLitNode)
        return std::nullopt;

    const auto& lit = static_cast<const PyrSlotNode&>(node);
    const auto& slot = lit.mSlot;

    // There are no literal objects, arrays don't currently count as literals.
    if (slot.isObjectHdr())
        return std::nullopt;

    // A literal object stored in the slot.
    if (!slot.isPtr())
        return { slot };

    // The only thing we store in a pointer at this point in the parsing are other parse nodes.
    // This is a little bit risky, but is wide spread.
    const auto& maybeBlock = *reinterpret_cast<PyrParseNode*>(slot.getPtr());

    // We are now expecting a block node, then a drop node containing a literal (as expression 1) and a block node
    // return (as expression 2).

    if (maybeBlock.mClassno != PyrParseNodeType::BlockNode)
        return std::nullopt;

    const auto& block = static_cast<const PyrBlockNode&>(maybeBlock);

    // Having arguments and variables mean we can't inline it, therefore, it isn't a literal.
    // Printing warnings first if requested to.
    if constexpr (Warning == UninlinableWarningOption::PostWarning) {
        if (block.mArglist || block.mVarlist) {
            gClassLibraryInfo.incrementNumUninlinedFunctions();
            if (SC_LanguageConfig::getPostInlineWarnings()) {
                print_inline_warning(cxt, block.mArglist, block.mVarlist);
            }
            return std::nullopt;
        }
    } else {
        if (block.mArglist || block.mVarlist)
            return std::nullopt;
    }

    if (block.mBody->mClassno != PyrParseNodeType::DropNode)
        return std::nullopt;

    const auto& dropNode = *static_cast<PyrDropNode*>(block.mBody);

    // Not a single return statement, e.g., { 1 },
    if (dropNode.mExpr2->mClassno != PyrParseNodeType::BlockReturnNode)
        return std::nullopt;

    if (dropNode.mExpr1->mClassno != PyrParseNodeType::PushLitNode)
        return std::nullopt;

    const auto& blockedLit = static_cast<PyrSlotNode&>(*dropNode.mExpr1);
    const auto& blockedSlot = blockedLit.mSlot;
    if (blockedSlot.isObjectHdr())
        return std::nullopt;
    // We don't allow functions to be literals, e.g., here the value returned would be a function`{ {1} }` but that
    // is not a literal. Otherwise we could do recursion with tail call for this function.
    if (blockedSlot.isPtr())
        return std::nullopt;

    return blockedSlot;
}

bool isWhileTrue(PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == PyrParseNodeType::PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot)
            && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == PyrParseNodeType::BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                /*
                post("WARNING: FunctionDef contains variable declarations and so"
                    " will not be inlined.\n");
                if (bnode->mArglist) nodePostErrorLine((PyrParseNode*)bnode->mArglist);
                else nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
                */
            } else {
                if (bnode->mBody->mClassno == PyrParseNodeType::PushLitNode
                    && IsTrue(&((PyrSlotNode*)bnode->mBody)->mSlot)) {
                    res = true;
                }
            }
        } else if (IsTrue(&anode->mSlot)) {
            res = true;
        }
    }
    return res;
}

void compileAndMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2) {
    PyrSlot dummy;
    compileNode(cxt, arg1, &dummy, false);
    if (isAnInlineableBlock(cxt, arg2)) {
        auto trueByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);

        const unsigned jumpLen = trueByteCodes.length();
        JumpIfFalsePushFalse.emit(cxt.bytecodes, arg2->location, Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                  Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
        cxt.bytecodes.consume(std::move(trueByteCodes));
    } else {
        compileNode(cxt, arg2, &dummy, false);
        emitTailCall(cxt, arg1->location);
        SendSpecialMsg.emit(cxt.bytecodes, arg1->location, 2, { OpSpecialSelectors::And });
    }
}

void compileOrMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2) {
    PyrSlot dummy;
    compileNode(cxt, arg1, &dummy, false);
    if (isAnInlineableBlock(cxt, arg2)) {
        auto falseByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);

        const unsigned int jumpLen = falseByteCodes.length();
        JumpIfTruePushTrue.emit(cxt.bytecodes, arg2->location, Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
        cxt.bytecodes.consume(std::move(falseByteCodes));
    } else {
        compileNode(cxt, arg2, &dummy, false);

        emitTailCall(cxt, arg2->location);
        SendSpecialMsg.emit(cxt.bytecodes, arg2->location, 2, { OpSpecialSelectors::Or });
    }
}

void compileQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2) {
    // question mark.
    PyrSlot dummy;

    compileNode(cxt, arg1, &dummy, false);
    compileNode(cxt, arg2, &dummy, false);
    Extended::QuestionMark.emit(cxt.bytecodes, arg1->location);
}

void compileQQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2) {
    // double question mark. ?? {|obj| ^if (this.notNil, this, func) }
    PyrSlot dummy;

    compileNode(cxt, arg1, &dummy, false);
    if (isAnInlineableBlock(cxt, arg2)) {
        auto nilByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);
        const unsigned int jumplen = nilByteCodes.length();
        Extended::DoubleQuestionMark.emit(
            cxt.bytecodes, arg2->location,
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
        cxt.bytecodes.consume(std::move(nilByteCodes));
    } else {
        compileNode(cxt, arg2, &dummy, false);
        emitTailCall(cxt, arg1->location);
        SendSpecialMsg.emit(cxt.bytecodes, arg2->location, 2, { OpSpecialSelectors::DoubleQuestionMark });
    }
}

void compileXQMsg(CompilerContext& cxt, PyrParseNode* arg1, PyrParseNode* arg2) {
    // double question mark. !? {|obj| ^if (this.isNil, this, func) }
    PyrSlot dummy;

    compileNode(cxt, arg1, &dummy, false);
    if (isAnInlineableBlock(cxt, arg2)) {
        auto nilByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);

        const unsigned int jumplen = nilByteCodes.length();
        Extended::IfNilThenJumpElsePopNil.emit(
            cxt.bytecodes, arg1->location,
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
        cxt.bytecodes.consume(std::move(nilByteCodes));
    } else {
        compileNode(cxt, arg2, &dummy, false);
        emitTailCall(cxt, arg2->location);
        SendSpecialMsg.emit(cxt.bytecodes, arg2->location, 2, { OpSpecialSelectors::ExclamationQuestionMark });
    }
}

void compileAnyIfMsg(CompilerContext& cxt, PyrCallNodeBase2* node) {
    PyrParseNode* arg1 = node->mArglist;

    if (arg1->mClassno == PyrParseNodeType::CallNode) {
        PyrCallNode* callNode = (PyrCallNode*)arg1;
        const int numCallArgs = nodeListLength(callNode->mArglist);
        const int numCallKeyArgs = nodeListLength(callNode->mKeyarglist);
        if (numCallArgs == 1 && numCallKeyArgs == 0) { // Is a binary op with no keywords
            if (slotRawSymbol(&callNode->mSelector->mSlot) == gSpecialUnarySelectors[opIsNil]) {
                compileIfNilMsg(cxt, node, true);
                return;
            } else if (slotRawSymbol(&callNode->mSelector->mSlot) == gSpecialUnarySelectors[opNotNil]) {
                compileIfNilMsg(cxt, node, false);
                return;
            }
        }
    }
    compileIfMsg(cxt, node);
}

void compileIfMsg(CompilerContext& cxt, PyrCallNodeBase2* node) {
    const int numArgs = nodeListLength(node->mArglist);
    PyrParseNode* arg1 = node->mArglist;

    PyrSlot dummy;
    if (numArgs == 2) {
        PyrParseNode* arg2 = arg1->mNext;

        if (isAnInlineableBlock(cxt, arg2)) {
            compileNode(cxt, arg1, &dummy, false);

            auto trueByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);
            const unsigned int jumpLen = trueByteCodes.length();
            if (jumpLen > 0) {
                JumpIfFalsePushNil.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                        Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
                cxt.bytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(cxt.bytecodes, node->location);
                PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
            }
        } else {
            for (; arg1; arg1 = arg1->mNext)
                compileNode(cxt, arg1, &dummy, false);

            emitTailCall(cxt, node->location);
            SendSpecialMsg.emit(cxt.bytecodes, node->location, 2, { OpSpecialSelectors::If });
        }
    } else if (numArgs == 3) {
        PyrParseNode* arg2 = arg1->mNext;
        PyrParseNode* arg3 = arg2->mNext;
        if (isAnInlineableBlock(cxt, arg2) && isAnInlineableBlock(cxt, arg3)) {
            compileNode(cxt, arg1, &dummy, false);
            auto falseByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg3, true);
            const unsigned int false_len = falseByteCodes.length();
            auto trueByteCodes = compileSubExpressionWithGoto(cxt, (PyrSlotNode*)arg2, false_len, true);
            const unsigned int true_len = trueByteCodes.length();
            if (false_len > 0) {
                JumpIfFalse.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(true_len),
                                 Operands::UnsignedInt<16, 0>::fromFull(true_len));
                cxt.bytecodes.consume(std::move(trueByteCodes));
                cxt.bytecodes.consume(std::move(falseByteCodes));
            } else if (true_len > 0) {
                JumpIfFalsePushNil.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(true_len),
                                        Operands::UnsignedInt<16, 0>::fromFull(true_len));
                cxt.bytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(cxt.bytecodes, node->location);
                PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
            }
        } else {
            for (; arg1; arg1 = arg1->mNext)
                compileNode(cxt, arg1, &dummy, false);

            emitTailCall(cxt, node->location);
            SendSpecialMsg.emit(cxt.bytecodes, node->location, 3, { OpSpecialSelectors::If });
        }
    } else {
        for (; arg1; arg1 = arg1->mNext)
            compileNode(cxt, arg1, &dummy, false);

        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::If });
        else
            SendSpecialMsgX.emit(cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::If)));
    }
}


// TODO: what is flag? Give it a better name.
void compileIfNilMsg(CompilerContext& cxt, PyrCallNodeBase2* node, bool flag) {
    PyrSlot dummy;

    const int numArgs = nodeListLength(node->mArglist);
    PyrParseNode* arg1 = node->mArglist;

    if (numArgs < 2) {
        compileNode(cxt, arg1, &dummy, false);
        emitTailCall(cxt, node->location);
        SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::If });
    } else if (numArgs == 2) {
        PyrParseNode* arg2 = arg1->mNext;
        if (isAnInlineableBlock(cxt, arg2)) {
            PyrCallNode* callNode = (PyrCallNode*)arg1;
            compileNode(cxt, callNode->mArglist, &dummy, false);

            auto trueByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg2, true);
            const unsigned int jumplen = trueByteCodes.length();
            if (jumplen) {
                if (flag)
                    Extended::IfNotNilJumpPushNilElsePop.emit(cxt.bytecodes, node->location,
                                                              { Operands::UnsignedInt<16, 1>::fromFull(jumplen),
                                                                Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
                else
                    Extended::IfNilThenJumpElsePopNil.emit(cxt.bytecodes, node->location,
                                                           { Operands::UnsignedInt<16, 1>::fromFull(jumplen),
                                                             Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
                cxt.bytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(cxt.bytecodes, node->location); // Drop the boolean
                PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
            }
        } else {
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            emitTailCall(cxt, node->location);
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::If });
        }
    } else if (numArgs == 3) {
        PyrParseNode* arg2 = arg1->mNext;
        PyrParseNode* arg3 = arg2->mNext;
        if (isAnInlineableBlock(cxt, arg2) && isAnInlineableBlock(cxt, arg3)) {
            PyrCallNode* callNode = (PyrCallNode*)arg1;
            compileNode(cxt, callNode->mArglist, &dummy, false);

            auto falseByteCodes = compileSubExpression(cxt, (PyrSlotNode*)arg3, true);
            const unsigned int falseLen = falseByteCodes.length();
            auto trueByteCodes = compileSubExpressionWithGoto(cxt, (PyrSlotNode*)arg2, falseLen, true);
            const unsigned int trueLen = trueByteCodes.length();
            if (falseLen) {
                if (flag)
                    Extended::IfNotNilJump.emit(cxt.bytecodes, node->location,
                                                { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                  Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                else
                    Extended::IfNilJump.emit(cxt.bytecodes, node->location,
                                             { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                               Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                cxt.bytecodes.consume(std::move(trueByteCodes));
                cxt.bytecodes.consume(std::move(falseByteCodes));
            } else if (trueLen) {
                if (flag)
                    Extended::IfNotNilJumpPushNilElsePop.emit(cxt.bytecodes, node->location,
                                                              { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                                Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                else
                    Extended::IfNilThenJumpElsePopNil.emit(cxt.bytecodes, node->location,
                                                           { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                             Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                cxt.bytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(cxt.bytecodes, node->location); // Drop the boolean
                PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
            }
        } else {
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            compileNode(cxt, arg3, &dummy, false);
            emitTailCall(cxt, arg3->location);
            SendSpecialMsg.emit(cxt.bytecodes, arg3->location, numArgs, { OpSpecialSelectors::If });
        }
    } else {
        for (; arg1; arg1 = arg1->mNext) {
            compileNode(cxt, arg1, &dummy, false);
        }
        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::If });
        else
            SendSpecialMsgX.emit(
                cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                Operands::KwArgumentCount::fromRaw(0), // it is not possible to have keyword arguments with if calls
                Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::If)));
    }
}

PyrParseNode* reverseNodeList(PyrParseNode** list) {
    PyrParseNode* temp1 = *list;
    PyrParseNode* temp2 = nullptr;
    PyrParseNode* temp3 = nullptr;
    while (temp1) {
        *list = temp1;
        temp2 = temp1->mNext;
        temp1->mNext = temp3;
        temp3 = temp1;
        temp1 = temp2;
    }
    return *list;
}


PyrCallNode* buildCase(CompilerContext& cxt, PyrParseNode* arg1) {
    // transform case statement into nested if statements.

    PyrParseNode* arg2 = arg1->mNext;

    PyrSlotNode* litnode = (PyrSlotNode*)arg1;
    PyrBlockNode* bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
    PyrParseNode* bbody = bnode->mBody;
    if (bbody->mClassno == PyrParseNodeType::DropNode) {
        PyrDropNode* dropNode = (PyrDropNode*)bbody;
        if (dropNode->mExpr2->mClassno == PyrParseNodeType::BlockReturnNode) {
            arg1 = dropNode->mExpr1;
        } else {
            arg1 = dropNode;
        }
    } else {
        arg1 = bbody;
    }
    arg1->mNext = arg2;

    PyrParseNode* arg3 = nullptr;
    if (arg2) {
        arg3 = arg2->mNext;
        if (arg3) {
            PyrParseNode* arg4 = arg3->mNext;
            if (arg4) {
                arg3 = buildCase(cxt, arg3);
                PyrBlockNode* bnode = cxt.allocParseNode<PyrBlockNode>(arg1->location, nullptr, nullptr, arg3, false);
                arg3 = cxt.allocParseNode<PyrSlotNode>(arg1->location, PyrSlot::make(static_cast<void*>(bnode)),
                                                       PyrParseNodeType::PushLitNode);
                arg2->mNext = arg3;
                arg3->mNext = nullptr;
                arg1->mTail = arg3;
            }
        } else {
            arg1->mTail = arg2;
        }
    } else {
        arg1->mTail = arg1;
    }

    auto selector_node = cxt.allocParseNode<PyrSlotNode>(arg1->location, PyrSlot::make(gSpecialSelectors[opmIf]));
    return cxt.allocParseNode<PyrCallNode>(arg1->location, selector_node, arg1);
}

void compileCaseMsg(CompilerContext& cxt, PyrCallNodeBase2* node) {
    PyrParseNode* argnode = node->mArglist;
    bool canInline = true;
    for (; argnode; argnode = argnode->mNext) {
        if (!isAnInlineableBlock(cxt, argnode)) {
            canInline = false;
            break;
        }
    }
    PyrSlot dummy;
    if (canInline) {
        PyrCallNode* callNode = buildCase(cxt, node->mArglist);
        callNode->compile(cxt, &dummy);
    } else {
        int numArgs = 0;
        argnode = node->mArglist;
        for (; argnode; argnode = argnode->mNext, ++numArgs) {
            compileNode(cxt, argnode, &dummy, false);
        }
        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::Case });
        else
            SendSpecialMsgX.emit(cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Case)));
    }
}

void compileSwitchMsg(CompilerContext& cxt, PyrCallNode* node) {
    PyrSlot dummy;
    bool canInline = true;
    int numArgs;
    {
        PyrParseNode* argnode = node->mArglist;
        numArgs = nodeListLength(argnode);

        if (numArgs <= 2) {
            if (numArgs == 0) {
                // This is actually impossible as per the grammar
                assert(false);
            } else if (numArgs == 1) {
                cxt.postErrorInCurrentFile(argnode->location, "Invalid switch statement.",
                                           "please provide cases for this switch statement.");
            } else if (numArgs == 2) {
                cxt.postErrorInCurrentFile(argnode->mNext->location, "Invalid switch statement.",
                                           "a switch statement must have more than 1 case.");
            }
        };

        argnode = argnode->mNext; // skip first arg.

        PyrParseNode* nextargnode = nullptr;
        for (; argnode; argnode = nextargnode) {
            // This loop is confusing, argnode can refer to either the case or the default depending on whether the
            // nextargnode is nullptr or not.
            nextargnode = argnode->mNext;
            if (nextargnode == nullptr) {
                // argnode is the default, this is how this loop terminates.
                if (!isAnInlineableBlock(cxt, argnode))
                    canInline = false;
                break; // nothing left, leave.
            }

            const auto& case_node = argnode;
            const auto& function_node = nextargnode;

            const auto case_literal = getAtomicValueFromLiteralOrBlockMaybePostWarning(cxt, *case_node);
            if (!case_literal.has_value()) {
                canInline = false;
                break;
            }

            // If the case is 'nil', do not inline as the empty element in the identity dictionary is nil.
            if (case_literal->isNil()) {
                canInline = false;
                break;
            }

            // Check the function after the case.
            if (!isAnInlineableBlock(cxt, function_node)) {
                canInline = false;
                break;
            }

            nextargnode = function_node->mNext;
        }
    }

    // TODO: this will need a rebase once my switch fix is implemented.
    if (canInline) {
        PyrParseNode* argnode = node->mArglist;
        const int flags = allocationFlags(cxt);
        int arraySize = NEXTPOWEROFTWO(numArgs * 2);
        PyrObject* array = newPyrArray(compileGC(cxt), arraySize, flags, false);
        array->size = arraySize;
        nilSlots(array->slots, arraySize);

        PyrSlot slot;
        SetObject(&slot, array);

        compileNode(cxt, argnode, &dummy, false);
        compilePushConstant(cxt, node, &slot);

        Extended::Switch.emit(cxt.bytecodes, node->location);

        argnode = argnode->mNext; // skip first arg.

        PyrParseNode* nextargnode = nullptr;
        const auto absoluteOffset = cxt.bytecodes.length();
        int offset = 0;
        int lastOffset = 0;
        for (; argnode; argnode = nextargnode) {
            nextargnode = argnode->mNext;
            if (nextargnode != nullptr) {
                // Where does 0x6666 come from?
                auto byteCodes = compileSubExpressionWithGoto(cxt, (PyrSlotNode*)nextargnode, 0x6666, true);

                PyrSlot* key;
                auto value = PyrSlot::make(offset);
                PyrSlotNode* keyargnode = nodeCast<PyrSlotNode>(argnode);
                if (isAtomicLiteral(argnode)) {
                    key = &keyargnode->mSlot;
                } else {
                    PyrBlockNode* bnode = (PyrBlockNode*)slotRawPtr(&keyargnode->mSlot);
                    PyrDropNode* dropnode = (PyrDropNode*)bnode->mBody;
                    PyrSlotNode* litnode = (PyrSlotNode*)dropnode->mExpr1;
                    key = &litnode->mSlot;
                }

                const int index = arrayAtIdentityHashInPairs(array, key);
                PyrSlot* keyvalue_ptr = array->slots + index;
                keyvalue_ptr[0] = *key;
                keyvalue_ptr[1] = PyrSlot::make(offset);

                const unsigned int len = byteCodes.length();
                if (len != 0) {
                    offset += len;
                    cxt.bytecodes.consume(std::move(byteCodes));
                } else {
                    PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
                    offset += 1;
                }

                nextargnode = nextargnode->mNext;
                if (nextargnode == nullptr) {
                    PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
                    lastOffset = offset;
                    offset += 1;
                }
            } else {
                auto byteCodes = compileSubExpressionWithGoto(cxt, (PyrSlotNode*)argnode, 0, true);

                lastOffset = offset;
                const unsigned int len = byteCodes.length();
                if (len > 0) {
                    offset += len;
                    cxt.bytecodes.consume(std::move(byteCodes));
                } else {
                    PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
                    lastOffset = offset;
                    offset += 1;
                }
            }
        }

        {
            const auto jumplen = offset - lastOffset;
            cxt.bytecodes.backSetByte(absoluteOffset + lastOffset - 2, (jumplen >> 8) & 255);
            cxt.bytecodes.backSetByte(absoluteOffset + lastOffset - 1, jumplen & 255);
        }

        PyrSlot* slots = array->slots;
        for (size_t i = 0; i < arraySize; i += 2) {
            PyrSlot* key = slots + i;
            PyrSlot* value = key + 1;

            if (IsNil(value)) {
                *value = PyrSlot::make(lastOffset);
            } else {
                if (const auto offsetToHere = value->getInt()) {
                    const auto jumplen = offset - offsetToHere;
                    cxt.bytecodes.backSetByte(absoluteOffset + offsetToHere - 2, (jumplen >> 8) & 255);
                    cxt.bytecodes.backSetByte(absoluteOffset + offsetToHere - 1, jumplen & 255);
                }
            }
        }


    } else {
        PyrParseNode* argnode = node->mArglist;
        for (; argnode; argnode = argnode->mNext) {
            compileNode(cxt, argnode, &dummy, false);
        }
        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::Switch });
        else
            SendSpecialMsgX.emit(cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Switch)));
    }
}

void compileWhileMsg(CompilerContext& cxt, PyrCallNodeBase2* node) {
    const int numArgs = nodeListLength(node->mArglist);
    if (numArgs == 1 && isAnInlineableBlock(cxt, node->mArglist)) {
        auto whileByteCodes = compileSubExpression(cxt, (PyrSlotNode*)node->mArglist, false);
        const unsigned int whileByteCodeLen = whileByteCodes.length();
        cxt.bytecodes.consume(std::move(whileByteCodes));

        // Where do these numbers come from?
        const unsigned int jumpIfFalseLength = 1 + 3;
        JumpIfFalsePushNil.emit(cxt.bytecodes, node->location,
                                Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalseLength),
                                Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalseLength));

        // opcJumpBak does a drop...
        PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });

        // UNKNOWN: where does this '4' come from?
        const auto jumpBackLength = 1 + whileByteCodeLen + 4;
        JumpBack.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpBackLength),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackLength));


    } else if (numArgs == 2 && isWhileTrue(node->mArglist) && isAnInlineableBlock(cxt, node->mArglist->mNext)) {
        auto exprByteCodes = compileSubExpression(cxt, (PyrSlotNode*)node->mArglist->mNext, false);
        const unsigned int exprByteCodeLen = exprByteCodes.length();
        cxt.bytecodes.consume(std::move(exprByteCodes));
        // UNKNOWN: where does this '1' come from?
        const auto jumpBackLength = exprByteCodeLen + 1;
        JumpBack.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpBackLength),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackLength));


    } else if (numArgs == 2 && isAnInlineableBlock(cxt, node->mArglist)
               && isAnInlineableBlock(cxt, node->mArglist->mNext)) {
        auto whileByteCodes = compileSubExpression(cxt, (PyrSlotNode*)node->mArglist, false);
        auto exprByteCodes = compileSubExpression(cxt, (PyrSlotNode*)node->mArglist->mNext, false);

        const unsigned int whileByteCodeLen = whileByteCodes.length();
        cxt.bytecodes.consume(std::move(whileByteCodes));

        const unsigned int exprByteCodeLen = exprByteCodes.length();
        if (exprByteCodeLen > 0) {
            // UNKNOWN: where does this '3' come from?
            const unsigned int jumpIfFalsePushNilLength = exprByteCodeLen + 3;
            JumpIfFalsePushNil.emit(cxt.bytecodes, node->location,
                                    Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalsePushNilLength),
                                    Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalsePushNilLength));

            cxt.bytecodes.consume(std::move(exprByteCodes));

            const unsigned int jumpBackSize = exprByteCodeLen + whileByteCodeLen + 4;
            JumpBack.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                          Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));

        } else {
            const unsigned int exprByteCodeLen = 1;
            // UNKNOWN: where does this '3' come from?
            const unsigned int jumpIfFalsePushNilLength = exprByteCodeLen + 3;
            JumpIfFalsePushNil.emit(cxt.bytecodes, node->location,
                                    Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalsePushNilLength),
                                    Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalsePushNilLength));
            // JumpBak does a drop..
            PushSpecialValue.emit(cxt.bytecodes, node->location, { OpSpecialValue::Nil_ });
            const unsigned int jumpBackSize = exprByteCodeLen + whileByteCodeLen + 4;
            JumpBack.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                          Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));
        }


    } else {
        PyrParseNode* argnode = node->mArglist;
        PyrSlot dummy;
        for (; argnode; argnode = argnode->mNext)
            compileNode(cxt, argnode, &dummy, false);

        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::While });
        else
            SendSpecialMsgX.emit(cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::While)));
    }
}

void compileLoopMsg(CompilerContext& cxt, PyrCallNodeBase2* node) {
    const int numArgs = nodeListLength(node->mArglist);
    if (numArgs == 1 && isAnInlineableBlock(cxt, node->mArglist)) {
        auto exprByteCodes = compileSubExpression(cxt, (PyrSlotNode*)node->mArglist, false);

        const unsigned int exprByteCodeLen = exprByteCodes.length();
        cxt.bytecodes.consume(std::move(exprByteCodes));

        // UNKNOWN: where does this '1' come from?
        const unsigned int jumpBackSize = exprByteCodeLen + 1;
        JumpBack.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));

    } else {
        PyrParseNode* argnode = node->mArglist;
        PyrSlot dummy;
        for (; argnode; argnode = argnode->mNext)
            compileNode(cxt, argnode, &dummy, false);

        emitTailCall(cxt, node->location);
        if (numArgs < 16)
            SendSpecialMsg.emit(cxt.bytecodes, node->location, numArgs, { OpSpecialSelectors::Loop });
        else
            SendSpecialMsgX.emit(cxt.bytecodes, node->location, Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Loop)));
    }
}


int PyrBinopCallNode::isPartialApplication() {
    int sum = 0;
    PyrParseNode* argnode = mArglist;
    for (; argnode; argnode = argnode->mNext) {
        if (argnode->mClassno == PyrParseNodeType::CurryArgNode) {
            ((PyrCurryArgNode*)argnode)->mArgNum = sum;
            sum++;
        }
    }
    return sum;
}

void PyrBinopCallNode::compileCall(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;

    PyrParseNode* arg1 = mArglist;
    PyrParseNode* arg2 = arg1->mNext;
    PyrParseNode* arg3 = arg2->mNext;

    const int isSuper = isSuperObjNode(arg1);
    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
    int selType;
    const int index = conjureSelectorIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, isSuper,
                                           slotRawSymbol(&mSelector->mSlot), &selType);

    const int numArgs = arg3 ? 3 : 2;
    if (isSuper) {
        compileNode(cxt, arg1, &dummy, false);
        compileNode(cxt, arg2, &dummy, false);
        if (arg3)
            compileNode(cxt, arg3, &dummy, false);

        emitTailCall(cxt, location);
        SendSuperMsg.emit(cxt.bytecodes, location, numArgs, Operands::SelectorIndex::fromRaw(index));

    } else {
        switch (selType) {
        case selNormal:
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            if (arg3)
                compileNode(cxt, arg3, &dummy, false);

            emitTailCall(cxt, location);
            SendMsg.emit(cxt.bytecodes, location, numArgs, Operands::SelectorIndex::fromRaw(index));
            break;

        case selSpecial:
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            if (arg3)
                compileNode(cxt, arg3, &dummy, false);

            emitTailCall(cxt, location);
            SendSpecialMsg.emit(cxt.bytecodes, location, numArgs, Operands::SpecialSelectors::fromRaw(index));
            break;

        case selUnary:
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            if (arg3)
                compileNode(cxt, arg3, &dummy, false);

            emitTailCall(cxt, location);

            // Drop extra arguments
            if (arg3)
                Drop.emit(cxt.bytecodes, location);
            Drop.emit(cxt.bytecodes, location);

            SendSpecialUnaryArithMsg.emit(cxt.bytecodes, location, { static_cast<OpUnaryMathNibble>((Byte)index) });
            break;

        case selBinary:
            if (arg3) {
                compileNode(cxt, arg1, &dummy, false);
                compileNode(cxt, arg2, &dummy, false);
                compileNode(cxt, arg3, &dummy, false);

                emitTailCall(cxt, location);
                SpecialBinaryOpWithAdverb.emit(cxt.bytecodes, location, Operands::TrinaryMath::fromRaw(index));

            } else if (index == opAdd && arg2->mClassno == PyrParseNodeType::PushLitNode
                       && IsInt(&((PyrSlotNode*)arg2)->mSlot) && slotRawInt(&((PyrSlotNode*)arg2)->mSlot) == 1) {
                compileNode(cxt, arg1, &dummy, false);

                PushOneAndAddOne.emit(cxt.bytecodes, location);

            } else if (index == opSub && arg2->mClassno == PyrParseNodeType::PushLitNode
                       && IsInt(&((PyrSlotNode*)arg2)->mSlot) && slotRawInt(&((PyrSlotNode*)arg2)->mSlot) == 1) {
                compileNode(cxt, arg1, &dummy, false);

                emitTailCall(cxt, location);
                PushOneAndSubtract.emit(cxt.bytecodes, location);

            } else {
                compileNode(cxt, arg1, &dummy, false);
                compileNode(cxt, arg2, &dummy, false);

                emitTailCall(cxt, location);
                if (index < 16)
                    SendSpecialBinaryArithMsg.emit(cxt.bytecodes, location, Operands::BinaryMathNibble::fromRaw(index));
                else
                    SendSpecialBinaryArithMsgX.emit(cxt.bytecodes, location, Operands::BinaryMath::fromRaw(index));
            }
            break;
        case selIf:
            compileAnyIfMsg(cxt, this);
            break;
        case selCase:
            compileCaseMsg(cxt, this);
            break;
        case selWhile:
            compileWhileMsg(cxt, this);
            break;
        case selLoop:
            compileLoopMsg(cxt, this);
            break;
        case selAnd:
            compileAndMsg(cxt, arg1, arg2);
            break;
        case selOr:
            compileOrMsg(cxt, arg1, arg2);
            break;
        case selQuestionMark:
            compileQMsg(cxt, arg1, arg2);
            break;
        case selDoubleQuestionMark:
            compileQQMsg(cxt, arg1, arg2);
            break;
        case selExclamationQuestionMark:
            compileXQMsg(cxt, arg1, arg2);
            break;
        default:
            compileNode(cxt, arg1, &dummy, false);
            compileNode(cxt, arg2, &dummy, false);
            if (arg3)
                compileNode(cxt, arg3, &dummy, false);

            emitTailCall(cxt, location);
            SendMsg.emit(cxt.bytecodes, location, numArgs, Operands::SelectorIndex::fromRaw(index));
            break;
        }
    }
}

void PyrPushKeyArgNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;
    // postfl("->compilePyrPushKeyArgNode\n");

    compilePushConstant(cxt, (PyrParseNode*)this, &mSelector->mSlot);

    compileNode(cxt, mExpr, &dummy, false);
}


void PyrDropNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;
    // eliminate as many drops as possible
    if (!mExpr2) {
        compileNode(cxt, mExpr1, &dummy, true);
    } else if (mExpr2->mClassno == PyrParseNodeType::BlockReturnNode) {
        // no drop before a block return
        compileNode(cxt, mExpr1, &dummy, true);
    } else if (mExpr1 && mExpr1->mClassno == PyrParseNodeType::AssignNode) {
        // let the store do the drop
        ((PyrAssignNode*)mExpr1)->mDrop = 1;
        compileNode(cxt, mExpr1, &dummy, false);
        compileNode(cxt, mExpr2, &dummy, true);
    } else if (mExpr1 && mExpr1->mClassno == PyrParseNodeType::DropNode) {
        // let the store do the drop, a bit more complex.
        // find the ultimate expression in the left subtree before the drop.
        PyrDropNode* znode = (PyrDropNode*)mExpr1;
        while (znode->mExpr2 && znode->mExpr2->mClassno == PyrParseNodeType::DropNode) {
            znode = (PyrDropNode*)znode->mExpr2;
        }
        if (znode->mExpr2->mClassno == PyrParseNodeType::AssignNode) {
            ((PyrAssignNode*)znode->mExpr2)->mDrop = 1;
            compileNode(cxt, mExpr1, &dummy, false);
            compileNode(cxt, mExpr2, &dummy, true);
        } else {
            compileNode(cxt, mExpr1, &dummy, false);
            Drop.emit(cxt.bytecodes, location);
            compileNode(cxt, mExpr2, &dummy, true);
        }
    } else {
        compileNode(cxt, mExpr1, &dummy, false);
        Drop.emit(cxt.bytecodes, location);
        compileNode(cxt, mExpr2, &dummy, true);
    }
}

void compilePushConstant(CompilerContext& cxt, PyrParseNode* node, PyrSlot* slot) {
    const unsigned int index = conjureConstantIndex(cxt, node, cxt.compilingBlock, slot);

    if (index < (1 << 4))
        PushLiteral.emit(cxt.bytecodes, node->location, index);

    else if (index < (1 << 8))
        PushConstant8.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<8, 0>::fromRaw(index));

    else if (index < (1 << 16))
        PushConstant16.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(index),
                            Operands::UnsignedInt<16, 0>::fromFull(index));

    else if (index < (1 << 24))
        PushConstant24.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<24, 2>::fromFull(index),
                            Operands::UnsignedInt<24, 1>::fromFull(index),
                            Operands::UnsignedInt<24, 0>::fromFull(index));

    else
        PushConstant32.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<32, 3>::fromFull(index),
                            Operands::UnsignedInt<32, 2>::fromFull(index),
                            Operands::UnsignedInt<32, 1>::fromFull(index),
                            Operands::UnsignedInt<32, 0>::fromFull(index));
}

void emitPushInt(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, int value) {
    if (value == -1)
        PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::MinusOne });

    else if (value == 0)
        PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::Zero });

    else if (value == 1)
        PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::One });

    else if (value == 2)
        PushSpecialNumber.emit(bc, loc, { OpSpecialNumbers::Two });

    else if (value >= -(1 << 7) && value <= ((1 << 7) - 1))
        PushInteger8.emit(bc, loc, Operands::Int<8, 0>::fromFull(value));

    else if (value >= -(1 << 15) && value <= ((1 << 15) - 1))
        PushInteger16.emit(bc, loc, Operands::Int<16, 1>::fromFull(value), Operands::Int<16, 0>::fromFull(value));

    else if (value >= -(1 << 23) && value <= ((1 << 23) - 1))
        PushInteger24.emit(bc, loc, Operands::Int<24, 2>::fromFull(value), Operands::Int<24, 1>::fromFull(value),
                           Operands::Int<24, 0>::fromFull(value));

    else
        PushInteger32.emit(bc, loc, Operands::Int<32, 3>::fromFull(value), Operands::Int<32, 2>::fromFull(value),
                           Operands::Int<32, 1>::fromFull(value), Operands::Int<32, 0>::fromFull(value));
}

void PyrSlotNode::compilePushLit(CompilerContext& cxt, PyrSlot* result) {
    if (IsPtr(&mSlot)) {
        PyrParseNode* literalObj = (PyrParseNode*)slotRawPtr(&mSlot);

        if (literalObj->mClassno == PyrParseNodeType::BlockNode) {
            CompilingBytecodes storedCodes { std::move(cxt.bytecodes) };
            PyrSlot slot;
            compileNode(cxt, literalObj, &slot, false);
            cxt.bytecodes = std::move(storedCodes);

            const Byte index = conjureLiteralSlotIndex(cxt, literalObj, cxt.compilingBlock, &slot);
            PushLiteralX.emit(cxt.bytecodes, location, Operands::Index::fromRaw(index));

            PyrBlock* block = slotRawBlock(&slot);
            if (NotNil(&block->contextDef))
                METHRAW(cxt.compilingBlock)->needsHeapContext = 1;

        } else {
            PyrSlot slot;
            compileNode(cxt, literalObj, &slot, false);
            compilePushConstant(cxt, (PyrParseNode*)literalObj, &slot);
        }
    } else {
        PyrSlot slot = mSlot;
        if (IsInt(&slot)) {
            emitPushInt(cxt.bytecodes, location, slotRawInt(&slot));
        } else if (SlotEq(&slot, &o_nil)) {
            PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::Nil_ });
        } else if (SlotEq(&slot, &o_true)) {
            PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::True });
        } else if (SlotEq(&slot, &o_false)) {
            PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::False });
        } else if (SlotEq(&slot, &o_fhalf)) {
            PushSpecialNumber.emit(cxt.bytecodes, location, { OpSpecialNumbers::Half });
        } else if (SlotEq(&slot, &o_fnegone)) {
            PushSpecialNumber.emit(cxt.bytecodes, location, { OpSpecialNumbers::MinusOneFloat });
        } else if (SlotEq(&slot, &o_fzero)) {
            PushSpecialNumber.emit(cxt.bytecodes, location, { OpSpecialNumbers::ZeroFloat });
        } else if (SlotEq(&slot, &o_fone)) {
            PushSpecialNumber.emit(cxt.bytecodes, location, { OpSpecialNumbers::OneFloat });
        } else if (SlotEq(&slot, &o_ftwo)) {
            PushSpecialNumber.emit(cxt.bytecodes, location, { OpSpecialNumbers::TwoFloat });
        } else if (SlotEq(&slot, &o_inf)) {
            PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::Inf });
        } else if (IsFloat(&slot)) {
            compilePushConstant(cxt, (PyrParseNode*)this, &slot);
        } else if (IsSym(&slot)) {
            compilePushConstant(cxt, (PyrParseNode*)this, &slot);
        } else {
            compilePushConstant(cxt, (PyrParseNode*)this, &slot);
        }
    }
}

void compilePyrLiteralNode(CompilerContext& cxt, PyrSlotNode* node, PyrSlot* result) {
    if (!node) {
        SetNil(result);
    } else {
        node->compileLiteral(cxt, result);
    }
}

void PyrSlotNode::compileLiteral(CompilerContext& cxt, PyrSlot* result) {
    if (IsPtr(&mSlot)) {
        PyrParseNode* literalObj = (PyrParseNode*)slotRawPtr(&mSlot);
        if (literalObj->mClassno == PyrParseNodeType::BlockNode) {
            CompilingBytecodes storedCodes { std::move(cxt.bytecodes) };
            compileNode(cxt, literalObj, result, false);
            cxt.bytecodes = std::move(storedCodes);

            PyrBlock* block = slotRawBlock(result);
            if (NotNil(&block->contextDef)) {
                METHRAW(cxt.compilingBlock)->needsHeapContext = 1;
            }
        } else {
            compileNode(cxt, literalObj, result, false);
        }
    } else {
        *(PyrSlot*)result = mSlot;
    }
}


void PyrReturnNode::compile(CompilerContext& cxt, PyrSlot* result) {
    cxt.functionCantBeClosed = true;
    if (!mExpr) {
        ReturnSelf.emit(cxt.bytecodes, location);
    } else if (mExpr->mClassno == PyrParseNodeType::PushLitNode) {
        PyrSlotNode* lit = (PyrSlotNode*)mExpr;
        if (IsSym(&(lit->mSlot)) && slotRawSymbol(&lit->mSlot) == s_this) {
            ReturnSelf.emit(cxt.bytecodes, location);
        } else if (IsNil(&lit->mSlot)) {
            ReturnNil.emit(cxt.bytecodes, location);
        } else if (IsTrue(&lit->mSlot)) {
            ReturnTrue.emit(cxt.bytecodes, location);
        } else if (IsFalse(&lit->mSlot)) {
            ReturnFalse.emit(cxt.bytecodes, location);
        } else {
            PyrSlot dummy;
            compileNode(cxt, lit, &dummy, false);
            Return.emit(cxt.bytecodes, location);
        }
    } else {
        SetTailBranch branch { cxt, true };
        SetTailIsMethodReturn mr { cxt, true };
        PyrSlot dummy;
        compileNode(cxt, mExpr, &dummy, true);
        Return.emit(cxt.bytecodes, location);
    }
}


void PyrBlockReturnNode::compile(CompilerContext& cxt, PyrSlot* result) {}

bool isUnassignableSymbol(PyrSymbol* varName) {
    return varName == s_this || varName == s_super || varName == s_curProcess || varName == s_curThread
        || varName == s_curMethod || varName == s_curBlock || varName == s_curClosure;
}

void compileAssignVar(CompilerContext& cxt, PyrParseNode* node, PyrSymbol* varName, bool drop) {
    if (isUnassignableSymbol(varName)) {
        cxt.postErrorInCurrentFile(node->location, "Attempting to assign to a reserved name.",
                                   "cannot assign as this is a special variable.");
        return;
    }
    if (std::isupper(varName->name[0])) {
        std::string msg { "You cannot assign to a class name. Did you mean '" };
        msg += std::tolower(varName->name[0]);
        msg += (varName->name + 1);
        msg += "'?";
        cxt.postErrorInCurrentFile(node->location, "Attempting to assign to a class name.", std::move(msg));
        return;
    }

    const auto result = findVarName(cxt, cxt.compilingBlock, cxt.compilingClass, varName);
    if (!result) {
        // TODO: once/if variables can be decalred in the middle of the block we can make this message a little
        // clearer.
        std::string msg { "This variable is not defined. Did you mean to declare it with 'var " };
        msg += varName->name;
        msg += "'?";
        cxt.postErrorInCurrentFile(node->location, "Undefined variable.", std::move(msg));
        return;
    }

    const FindVarNameResult findResult = *result;

    switch (findResult.varType) {
    case varInst: {
        if (drop) {
            if (findResult.index <= 15) {
                StoreInstVar.emit(cxt.bytecodes, node->location, findResult.index);
            } else {
                StoreInstVarX.emit(cxt.bytecodes, node->location, Operands::Index::fromRaw(findResult.index));
                Drop.emit(cxt.bytecodes, node->location);
            }
        } else {
            // TODO: why can't we use the shorter StoreInstVar here? It breaks for some reason.
            StoreInstVarX.emit(cxt.bytecodes, node->location, Operands::Index::fromRaw(findResult.index));
        }
    } break;

    case varClass: {
        const unsigned int index = findResult.index + slotRawInt(&findResult.classobj->classVarIndex);
        if (drop) {
            if (index < 4096) {
                StoreClassVar.emit(cxt.bytecodes, node->location, index);
            } else {
                StoreClassVarX.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(index),
                                    Operands::UnsignedInt<16, 0>::fromFull(index));
                Drop.emit(cxt.bytecodes, node->location);
            }
        } else {
            StoreClassVarX.emit(cxt.bytecodes, node->location, Operands::UnsignedInt<16, 1>::fromFull(index),
                                Operands::UnsignedInt<16, 0>::fromFull(index));
        }
    } break;

    case varConst: {
        cxt.postErrorInCurrentFile(node->location, "Assigning to a constant.",
                                   "this variable was declared as a constant, you cannot assign to it.");
    } break;

    case varTemp: {
        if (drop) {
            if (findResult.index <= 15 && findResult.level < 8) {
                StoreTempVar.emit(cxt.bytecodes, node->location, findResult.level,
                                  Operands::Index::fromRaw(findResult.index));
            } else {
                StoreTempVarX.emit(cxt.bytecodes, node->location, Operands::FrameOffset::fromRaw(findResult.level),
                                   Operands::Index::fromRaw(findResult.index));
                Drop.emit(cxt.bytecodes, node->location);
            }
        } else {
            // TODO: why can't we use the shorter StoreTempVarX here? It breaks for some reason.
            StoreTempVarX.emit(cxt.bytecodes, node->location, Operands::FrameOffset::fromRaw(findResult.level),
                               Operands::Index::fromRaw(findResult.index));
        }
    } break;

    default: {
        assert(false);
        cxt.postErrorInCurrentFile(node->location, "Internal error.", "please report this error");
    } break;
    }
}

void PyrAssignNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;

    // postfl("compilePyrAssignNode\n");
    compileNode(cxt, mExpr, &dummy, false);
    compileAssignVar(cxt, (PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
}


int PyrSetterNode::isPartialApplication() {
    int sum = 0;
    if (mExpr1->mClassno == PyrParseNodeType::CurryArgNode) {
        ((PyrCurryArgNode*)mExpr1)->mArgNum = sum;
        sum++;
    }
    if (mExpr2->mClassno == PyrParseNodeType::CurryArgNode) {
        ((PyrCurryArgNode*)mExpr2)->mArgNum = sum;
        sum++;
    }
    return sum;
}

void PyrSetterNode::compileCall(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;
    char setterName[128];

    if (nodeListLength(mExpr1) > 1) {
        cxt.postErrorInCurrentFile(mExpr1->location, "Invalid setter call.",
                                   "setters should only be called with one argument.");
        return;
    }

    compileNode(cxt, mExpr1, &dummy, false);
    compileNode(cxt, mExpr2, &dummy, false);

    const int isSuper = isSuperObjNode(mExpr1);

    sprintf(setterName, "%s_", slotRawSymbol(&mSelector->mSlot)->name);
    PyrSymbol* setterSym = getsym(setterName);

    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
    int selType; // TODO: this is not used.
    const int index =
        conjureSelectorIndex(cxt, (PyrParseNode*)mSelector, cxt.compilingBlock, isSuper, setterSym, &selType);
    if (isSuper) {
        emitTailCall(cxt, location);
        SendSuperMsg.emit(cxt.bytecodes, location, 2, Operands::SelectorIndex::fromRaw(index));
    } else {
        emitTailCall(cxt, location);
        SendMsg.emit(cxt.bytecodes, location, 2, Operands::SelectorIndex::fromRaw(index));
    }
}

void PyrMultiAssignNode::compile(CompilerContext& cxt, PyrSlot* result) {
    PyrSlot dummy;

    compileNode(cxt, mExpr, &dummy, false);
    compileNode(cxt, mVarList, &dummy, false);
}

void PyrMultiAssignVarListNode::compile(CompilerContext& cxt, PyrSlot* result) {
    int i, numAssigns;
    PyrSlotNode* varname;

    // postfl("compilePyrMultiAssignVarListNode\n");
    numAssigns = nodeListLength((PyrParseNode*)mVarNames);
    varname = mVarNames;
    for (i = 0; i < numAssigns; ++i, varname = (PyrSlotNode*)varname->mNext) {
        Dup.emit(cxt.bytecodes, location);
        emitPushInt(cxt.bytecodes, location, i);
        SendSpecialMsg.emit(cxt.bytecodes, location, 2, { OpSpecialSelectors::At });
        compileAssignVar(cxt, (PyrParseNode*)varname, slotRawSymbol(&varname->mSlot), 1);
    }

    if (mRest) {
        Dup.emit(cxt.bytecodes, location);
        emitPushInt(cxt.bytecodes, location, i);
        SendSpecialMsg.emit(cxt.bytecodes, location, 2, { OpSpecialSelectors::CopyToEnd });
        compileAssignVar(cxt, (PyrParseNode*)mRest, slotRawSymbol(&mRest->mSlot), 1);
    }
}


int PyrDynDictNode::isPartialApplication() {
    int sum = 0;
    int numItems = nodeListLength(mElems);
    PyrParseNode* inode = mElems;
    for (int i = 0; i < numItems; ++i) {
        if (inode->mClassno == PyrParseNodeType::CurryArgNode) {
            ((PyrCurryArgNode*)inode)->mArgNum = sum;
            sum++;
        }
        inode = (PyrParseNode*)inode->mNext;
    }
    return sum;
}

void PyrDynDictNode::compileCall(CompilerContext& cxt, PyrSlot* result) {
    const int numItems = nodeListLength(mElems) >> 1;

    compilePushVar(cxt, (PyrParseNode*)this, s_event);

    emitPushInt(cxt.bytecodes, location, numItems);
    PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::Nil_ }); // push nil for proto
    PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::Nil_ }); // push nil for parent
    PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::True }); // push true for know
    SendSpecialMsg.emit(cxt.bytecodes, location, 5, { OpSpecialSelectors::New });

    PyrParseNode* inode = mElems;
    PyrSlot dummy;
    for (int i = 0; i < numItems; ++i) {
        compileNode(cxt, inode, &dummy, false);
        inode = (PyrParseNode*)inode->mNext;
        compileNode(cxt, inode, &dummy, false);
        inode = (PyrParseNode*)inode->mNext;
        SendSpecialMsg.emit(cxt.bytecodes, location, 3, { OpSpecialSelectors::Put });
    }
}


int PyrDynListNode::isPartialApplication() {
    int sum = 0;
    int numItems = nodeListLength(mElems);
    PyrParseNode* inode = mElems;
    for (int i = 0; i < numItems; ++i) {
        if (inode->mClassno == PyrParseNodeType::CurryArgNode) {
            ((PyrCurryArgNode*)inode)->mArgNum = sum;
            sum++;
        }
        inode = (PyrParseNode*)inode->mNext;
    }
    return sum;
}

void PyrDynListNode::compileCall(CompilerContext& cxt, PyrSlot* result) {
    const int numItems = nodeListLength(mElems);

    compilePushVar(cxt, (PyrParseNode*)this, mClassname ? slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) : s_array);
    emitPushInt(cxt.bytecodes, location, numItems);
    SendSpecialMsg.emit(cxt.bytecodes, location, 2, { OpSpecialSelectors::New });

    PyrParseNode* inode = mElems;
    PyrSlot dummy;
    for (int i = 0; i < numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
        compileNode(cxt, inode, &dummy, false);
        SendSpecialMsg.emit(cxt.bytecodes, location, 2, { OpSpecialSelectors::Add });
    }
}


void PyrLitListNode::compile(CompilerContext& cxt, PyrSlot* result) {
    // postfl("->compilePyrLitListNode\n");
    if (mClassname && slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) != s_array) {
        // TODO: this should error instead.
        cxt.postWarningInCurrentFile(mClassname->location, "Invalid literal array.",
                                     "only 'Array' is supported as a literal, continueing as-if this was an array.");
    }
    const auto numItems = mElems ? nodeListLength(mElems) : 0;
    const auto flags = allocationFlags(cxt);
    auto array = newPyrArray(compileGC(cxt), numItems, flags, false);
    auto inode = mElems;
    for (int i = 0; i < numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
        PyrSlot itemSlot {};
        compileNode(cxt, inode, &itemSlot, false);
        array->slots[i] = itemSlot;
    }
    array->size = numItems;
    SetObject(result, array);
    // postfl("<-compilePyrLitListNode\n");
}


// Does not check this is valid! Will segfault if not.
PyrParseNode* getNthChild(PyrParseNode* start, std::size_t n) {
    for (size_t i { 0 }; i < n; ++i) {
        start = start->mNext;
    }
    return start;
}

void PyrBlockNode::compile(CompilerContext& cxt, PyrSlot* slotResult) {
    const auto flags = allocationFlags(cxt);

    const std::size_t numPositionalArgs = mArglist ? nodeListLength(mArglist->mVarDefs) : 0;
    const std::size_t numVariadicArgs = (mArglist && mArglist->mRest) ? (mArglist->mKeywordArgs ? 2 : 1) : 0;
    const std::size_t numTotalArguments = numPositionalArgs + numVariadicArgs;
    const std::size_t numVariables = mVarlist ? nodeListLength(mVarlist->mVarDefs) : 0;
    const std::size_t numSlots = numTotalArguments + numVariables;

    if (numTotalArguments > 255)
        cxt.postErrorInCurrentFile(mArglist->location, "Too many arguments.",
                                   "Too many arguments, max of 255, use an IdentityDictionary instead.");
    if (numVariables > 255)
        cxt.postErrorInCurrentFile(mVarlist->location, "Too many variable.",
                                   "Too many variables, max of 255, use an IdentityDictionary instead.");


    // MUTABLE VARIABLES {

    // This function builds this block
    auto block = newPyrBlock(!cxt.textInfo->isClassFile, flags);

    // All the argument and variable names, they must be unique
    std::unordered_map<PyrSymbol*, sc::lex::SourceCodeRange> namedIdentifiers {};

    // Addtionally, the cxt is mutated, e.g. the bytecodes.

    // } MUTABLE VARIABLES

    // out arg
    *slotResult = PyrSlot::make(block);


    // Mutate compiler context
    const auto prevFunctionHighestExternalRef = cxt.functionHighestExternalRef;
    const auto prevFunctionCantBeClosed = cxt.functionCantBeClosed;
    cxt.functionHighestExternalRef = 0;
    cxt.functionCantBeClosed = false;

    const auto prevBlock = cxt.compilingBlock;
    cxt.compilingBlock = block;

    // This bit is a little subtle.
    // When compiling cmd code, we are always inside the method Interpreter:functionCompileContext.
    // The first block (main expr) of cmd code will set this to class_interpreter over in PyrPrimitive.cpp
    // mIsTopLevel also gets set when you have a function like `#{...}`, essentially, jumping back to the top of the
    // stack of classes and blocks. Confusingly this also requires us to set the block->contextDef (where we do the
    // lookup for the closure), but that can't be done until compiling is finished, as we want to set it to nil and
    // make the function closed automatically if possible.
    auto* const prevClass = cxt.compilingClass;
    cxt.compilingClass = mIsTopLevel ? class_interpreter : cxt.compilingClass;

    auto* const prevPartiallyAppliedFunction = cxt.compilingPartiallyAppliedFunction;
    cxt.compilingPartiallyAppliedFunction = nullptr;

    // put everything back
    defer d { [&]() {
        cxt.compilingBlock = prevBlock;
        cxt.compilingClass = prevClass;
        cxt.compilingPartiallyAppliedFunction = prevPartiallyAppliedFunction;
        cxt.functionCantBeClosed = cxt.functionCantBeClosed || prevFunctionCantBeClosed;
        cxt.functionHighestExternalRef = std::max(cxt.functionHighestExternalRef - 1, prevFunctionHighestExternalRef);
    } };


    *METHRAW(block) = PyrMethodRaw { 0,
                                     0,
                                     methBlock,
                                     static_cast<unsigned short>((numSlots + FRAMESIZE) * sizeof(PyrSlot)),
                                     0,
                                     static_cast<unsigned char>(numPositionalArgs),
                                     static_cast<unsigned char>(numVariadicArgs),
                                     static_cast<unsigned char>(numVariables),
                                     static_cast<unsigned char>(numSlots),
                                     0,
                                     static_cast<unsigned char>(numSlots),
                                     static_cast<unsigned char>(numTotalArguments) };

    // TODO: this needs updating to remove any \r
    block->sourceCodeFileOrSnippet = PyrSlot::make(cxt.textInfo->normalisedSourceAsPyr);
    block->name = deduceNameOfFunctionFromParent(this->mParent);
    block->filePath = PyrSlot::make(cxt.textInfo->filePathDescription);
    block->sourceCodeStartIndex = PyrSlot::make(static_cast<int>(location.begin.absolute));
    block->sourceCodeEndIndex = PyrSlot::make(static_cast<int>(location.end.absolute));

    block->fileLocation = cxt.textInfo->isClassFile ? PyrSlot {} : [&]() {
        // TODO: test this!
        auto ar = newPyrArray(compileGC(cxt), 2, 0, false);
        ar->size = 2;
        ar->slots[0] = PyrSlot::make(static_cast<int>(cxt.textInfo->offsetInFile.line_number));
        ar->slots[1] = PyrSlot::make(static_cast<int>(cxt.textInfo->offsetInFile.column));
        return PyrSlot::make(ar);
    }();


    // create a symbol array with all the argument names in, checking for duplicates
    block->argNames = createArgNames(cxt, numTotalArguments, false, mArglist, namedIdentifiers);
    block->varNames = createVarNames(cxt, numVariables, mVarlist, namedIdentifiers);


    // Here we set the context of the closure.
    // Might remove this later if we don't use it (aka, the function is closed.)
    block->contextDef = mIsTopLevel ? PyrSlot {} : PyrSlot::make(prevBlock);

    // Contains all the default values (if constant expr else nil) of the arguments and variables.
    // Variadic args are set to the empty array.
    const auto [proto, a, v] = createPrototypeFrame(cxt, numSlots, false, mArglist, mVarlist);
    const auto argsRequiresCompilation = a;
    const auto varsRequiresCompilation = v;
    block->prototypeFrame = proto;

    // compile bytecodes
    cxt.bytecodes.assertEmpty();

    // This block mutates the context, adding stuff to the bytecodes
    {
        SetTailBranch branch(cxt, true);
        SetTailIsMethodReturn mr(cxt, false);
        if (argsRequiresCompilation)
            for (auto arg { mArglist->mVarDefs }; arg; arg = nodeCast<PyrVarDefNode>(arg->mNext)) {
                PyrSlot dummy {};
                arg->compileArg(cxt, &dummy);
            }
        // no need to compile variadic args as they can't have defaults
        if (varsRequiresCompilation)
            for (auto var { mVarlist->mVarDefs }; var; var = nodeCast<PyrVarDefNode>(var->mNext)) {
                PyrSlot dummy {};
                var->compile(cxt, &dummy);
            }

        // If there are no expressions inside the body return nil.
        if (nodeCast<PyrBlockReturnNode>(mBody) || mBody == nullptr) {
            PushSpecialValue.emit(cxt.bytecodes, location, { OpSpecialValue::Nil_ });
        } else {
            PyrSlot dummy {};
            compileNode(cxt, mBody, &dummy, true);
        }
    }

    // This must happen AFTER the tail call stuff
    BlockReturn.emit(cxt.bytecodes, location);

    if (auto c = finaliseBytecodes(cxt)) {
        const auto [codes, codeLocations, codeSizes] = *c;
        block->code = PyrSlot::make(codes);
        block->codeLocations = PyrSlot::make(codeLocations);
        block->codeSizes = PyrSlot::make(codeSizes);
    } else {
        block->code = PyrSlot {};
        block->codeLocations = PyrSlot {};
        block->codeSizes = PyrSlot {};
    }

    const auto isClosed = (!cxt.functionCantBeClosed && cxt.functionHighestExternalRef == 0) || mIsTopLevel;
    // NOTE: this is odd, but we need to the context when doing identifier
    // If we haven't looked up any, then it is closed, and we can remove it.
    if (isClosed)
        block->contextDef = PyrSlot {};

    // Note: we don't have to declare the function like `#{...}` to make it closed, we automatically promote the
    // function if possible.
    block->isClosed = PyrSlot::make(isClosed);
}


bool isSuperObjNode(PyrParseNode* node) {
    return node->mClassno == PyrParseNodeType::PushNameNode && slotRawSymbol(&((PyrSlotNode*)node)->mSlot) == s_super;
}

bool isThisObjNode(PyrParseNode* node) {
    return node->mClassno == PyrParseNodeType::PushNameNode && slotRawSymbol(&((PyrSlotNode*)node)->mSlot) == s_this;
}

int nodeListLength(PyrParseNode* node) {
    int length = 0;
    for (; node; node = node->mNext)
        length++;
    return length;
}


// TODO: refactor this somehow so it is clear **exactly** what all the return arguments do.
int conjureSelectorIndex(CompilerContext& cxt, PyrParseNode* node, PyrBlock* func, bool isSuper, PyrSymbol* selector,
                         int* selType) {
    int i;
    PyrObject* selectors;
    PyrSlot* slot;
    int newsize;

    const auto flags = allocationFlags(cxt);
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
            return opmQuestionMark;
        } else if (selector == gSpecialSelectors[opmDoubleQuestionMark]) {
            *selType = selDoubleQuestionMark;
            return opmDoubleQuestionMark;
        } else if (selector == gSpecialSelectors[opmExclamationQuestionMark]) {
            *selType = selExclamationQuestionMark;
            return opmExclamationQuestionMark;
        }

        for (i = 0; i < opmNumSpecialSelectors; ++i) {
            if (selector == gSpecialSelectors[i]) {
                *selType = selSpecial;
                return i;
            }
        }

        for (i = 0; i < opNumUnarySelectors; ++i) {
            if (selector == gSpecialUnarySelectors[i]) {
                *selType = selUnary;
                return i;
            }
        }

        for (i = 0; i < opNumBinarySelectors; ++i) {
            if (selector == gSpecialBinarySelectors[i]) {
                *selType = selBinary;
                return i;
            }
        }
    }

    if (NotNil(&func->selectors)) {
        selectors = slotRawObject(&func->selectors);
        for (i = 0; i < selectors->size; ++i) {
            if (IsSym(&selectors->slots[i]) && slotRawSymbol(&selectors->slots[i]) == selector) {
                *selType = selNormal;
                return i;
            }
        }
    } else {
        selectors = (PyrObject*)newPyrArray(compileGC(cxt), 2, flags, false);
        SetObject(&func->selectors, selectors);
    }
    // otherwise add it to the selectors table

    if (selectors->size + 1 >= 256) {
        cxt.postErrorInCurrentFile(
            node->location, "Selector table too big",
            "this function has too many selectors, delete some variables, arguments or referenced class names");
        return 0;
    }

    if (selectors->size + 1 > ARRAYMAXINDEXSIZE(selectors)) {
        // resize literal table
        newsize = ARRAYMAXINDEXSIZE(selectors) * 2;
        SetRaw(&func->selectors, (PyrObject*)newPyrArray(compileGC(cxt), newsize, flags, false));
        memcpy(slotRawObject(&func->selectors)->slots, selectors->slots, selectors->size * sizeof(PyrSlot));
        slotRawObject(&func->selectors)->size = selectors->size;
        freePyrObject(selectors);
        selectors = slotRawObject(&func->selectors);
    }
    slot = selectors->slots + selectors->size++;
    SetSymbol(slot, selector);

    *selType = selNormal;
    return selectors->size - 1;
}

Byte conjureLiteralSlotIndex(CompilerContext& cxt, PyrParseNode* node, PyrBlock* func, PyrSlot* slot) {
    int i;
    PyrObject* selectors;
    PyrSlot* slot2;
    int newsize;

    const auto flags = allocationFlags(cxt);
    // lookup slot in selectors table

    if (IsObj(&func->selectors)) {
        selectors = slotRawObject(&func->selectors);
        /*if (selectors->classptr != class_array) {
            post("compiling %s:%s\n", slotRawSymbol(&cxt.compiling_class->name)->name,
        slotRawSymbol(&cxt.compiling_method->name)->name); post("selectors is a '%s'\n",
        selectors->classptr->name.us->name); dumpObjectSlot(slot); Debugger();
        }*/
        for (i = 0; i < selectors->size; ++i)
            if (SlotEq(&selectors->slots[i], slot))
                return i;
    } else {
        selectors = (PyrObject*)newPyrArray(compileGC(cxt), 4, flags, false);
        SetObject(&func->selectors, selectors);
    }
    // otherwise add it to the selectors table

    if (selectors->size + 1 >= 256) {
        cxt.postErrorInCurrentFile(
            node->location, "Selector table too big",
            "this function has too many selectors, delete some variables, arguments or referenced class names");
        return 0;
    }
    if (selectors->size + 1 > ARRAYMAXINDEXSIZE(selectors)) {
        // resize literal table
        newsize = ARRAYMAXINDEXSIZE(selectors) * 2;
        // resize literal table
        SetRaw(&func->selectors, (PyrObject*)newPyrArray(compileGC(cxt), newsize, flags, false));
        memcpy(slotRawObject(&func->selectors)->slots, selectors->slots, selectors->size * sizeof(PyrSlot));
        slotRawObject(&func->selectors)->size = selectors->size;
        freePyrObject(selectors);
        selectors = slotRawObject(&func->selectors);
    }
    slot2 = selectors->slots + selectors->size++;
    slotCopy(slot2, slot);

    return static_cast<Byte>(selectors->size - 1);
}


int conjureConstantIndex(CompilerContext& cxt, PyrParseNode* node, PyrBlock* func, PyrSlot* slot) {
    const int flags = allocationFlags(cxt);

    // lookup slot in constants table
    PyrObject* constants;
    if (IsObj(&func->constants)) {
        constants = slotRawObject(&func->constants);
        for (int i = 0; i < constants->size; ++i)
            if (SlotEq(&constants->slots[i], slot))
                return i;
    } else {
        constants = (PyrObject*)newPyrArray(compileGC(cxt), 4, flags, false);
        SetObject(&func->constants, constants);
    }

    // otherwise add it to the constants table
    if (constants->size + 1 > ARRAYMAXINDEXSIZE(constants)) {
        // resize literal table
        int newsize = ARRAYMAXINDEXSIZE(constants) * 2;
        // resize literal table
        SetRaw(&func->constants, (PyrObject*)newPyrArray(compileGC(cxt), newsize, flags, false));
        memcpy(slotRawObject(&func->constants)->slots, constants->slots, constants->size * sizeof(PyrSlot));
        slotRawObject(&func->constants)->size = constants->size;
        freePyrObject((PyrObject*)constants);
        constants = slotRawObject(&func->constants);
    }
    slotCopy(&constants->slots[constants->size++], slot);

    return constants->size - 1;
}

bool findNamedIdentifier(CompilerContext& cxt, PyrBlock* block, PyrClass** classobj, PyrSymbol* name, int* varType,
                         int* level, int* index, PyrBlock** tempfunc) {
    int i, j, k;
    int numargs;
    PyrSymbol *argname, *varname;
    PyrMethodRaw* methraw;

    // postfl("->findVarName %s\n", name->name);
    // find var in enclosing blocks, instance, class
    if (name == s_super) {
        cxt.functionCantBeClosed = true;
        name = s_this;
    }

    if (name->name[0] >= 'A' && name->name[0] <= 'Z')
        return false;

    j = 0;
    while (block != nullptr) {
        methraw = METHRAW(block);
        numargs = methraw->totalNumberArguments;
        for (i = 0; i < numargs; ++i) {
            argname = slotRawSymbolArray(&block->argNames)->symbols[i];
            // postfl("    %d %d arg '%s' '%s'\n", j, i, argname->name, name->name);
            if (argname == name) {
                *level = j;
                *index = i;
                *varType = varTemp;
                if (tempfunc)
                    *tempfunc = block;
                if (j > cxt.functionHighestExternalRef)
                    cxt.functionHighestExternalRef = j;
                return true;
            }
        }
        for (i = 0, k = numargs; i < methraw->numVariables; ++i, ++k) {
            varname = slotRawSymbolArray(&block->varNames)->symbols[i];
            // postfl("    %d %d %d var '%s' '%s'\n", j, i, k, varname->name, name->name);
            if (varname == name) {
                *level = j;
                *index = k;
                *varType = varTemp;
                if (tempfunc)
                    *tempfunc = block;
                if (j > cxt.functionHighestExternalRef)
                    cxt.functionHighestExternalRef = j;
                return true;
            }
        }

        block = slotRawBlock(&block->contextDef);
        ++j;
    }

    if (classFindInstVar(*classobj, name, index)) {
        *level = 0;
        *varType = varInst;
        if (cxt.compilingClass != class_interpreter)
            cxt.functionCantBeClosed = true;
        return true;
    }
    if (classFindClassVar(classobj, name, index)) {
        *varType = varClass;
        if (cxt.compilingClass != class_interpreter)
            cxt.functionCantBeClosed = true;
        return true;
    }
    if (classFindConst(classobj, name, index)) {
        *varType = varConst;
        // if (cxt.compiling_class != class_interpreter) gFunctionCantBeClosed = true;
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

extern PyrSymbol* s_env;

void initSpecialClasses() {
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

    // Infinitum, Point, Rect, ??
}

void initSpecialSelectors() {
    PyrSymbol** sel;
    std::int64_t i;

    sel = gSpecialUnarySelectors;
    sel[opNeg] = getsym("neg");
    sel[opRecip] = getsym("reciprocal");
    sel[opNot] = getsym("not");
    sel[opIsNil] = getsym("isNil");
    sel[opNotNil] = getsym("notNil");
    sel[opBitNot] = getsym("bitNot");
    sel[opAbs] = getsym("abs");
    sel[opAsFloat] = getsym("asFloat");
    sel[opAsInteger] = getsym("asInteger");
    sel[opCeil] = getsym("ceil"); // 5
    sel[opFloor] = getsym("floor");
    sel[opFrac] = getsym("frac");
    sel[opSign] = getsym("sign");
    sel[opSquared] = getsym("squared");
    sel[opCubed] = getsym("cubed"); // 10
    sel[opSqrt] = getsym("sqrt");
    sel[opExp] = getsym("exp");
    sel[opMIDICPS] = getsym("midicps");
    sel[opCPSMIDI] = getsym("cpsmidi");
    sel[opMIDIRatio] = getsym("midiratio");
    sel[opRatioMIDI] = getsym("ratiomidi");
    sel[opAmpDb] = getsym("ampdb"); // 15
    sel[opDbAmp] = getsym("dbamp");
    sel[opOctCPS] = getsym("octcps");
    sel[opCPSOct] = getsym("cpsoct");
    sel[opLog] = getsym("log");
    sel[opLog2] = getsym("log2"); // 20
    sel[opLog10] = getsym("log10");
    sel[opSin] = getsym("sin");
    sel[opCos] = getsym("cos");
    sel[opTan] = getsym("tan");
    sel[opArcSin] = getsym("asin"); // 25
    sel[opArcCos] = getsym("acos");
    sel[opArcTan] = getsym("atan");
    sel[opSinH] = getsym("sinh");
    sel[opCosH] = getsym("cosh");
    sel[opTanH] = getsym("tanh"); // 30
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
    // sel[opIdentical] = getsym("===");
    // sel[opNotIdentical] = getsym("!==");
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
    sel[opRing1] = getsym("ring1"); // a * (b + 1) == a * b + a
    sel[opRing2] = getsym("ring2"); // a * b + a + b
    sel[opRing3] = getsym("ring3"); // a*a*b
    sel[opRing4] = getsym("ring4"); // a*a*b - a*b*b
    sel[opDifSqr] = getsym("difsqr"); // a*a - b*b
    sel[opSumSqr] = getsym("sumsqr"); // a*a + b*b
    sel[opSqrSum] = getsym("sqrsum"); // (a + b)^2
    sel[opSqrDif] = getsym("sqrdif"); // (a - b)^2
    sel[opAbsDif] = getsym("absdif"); //
    sel[opThresh] = getsym("thresh"); //
    sel[opAMClip] = getsym("amclip"); //
    sel[opScaleNeg] = getsym("scaleneg"); //
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
    sel[opmCopyToEnd] = getsym("copyToEnd"); // used by multiple assignment
    // sel[opmIsNil] = getsym("isNil");
    // sel[opmNotNil] = getsym("notNil");
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
    sel[opmDoesNotUnderstand] = getsym("doesNotUnderstand"); // not really needed
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

    for (i = 0; i < opNumUnarySelectors; ++i) {
        gSpecialUnarySelectors[i]->specialIndex = i;
    }
    for (i = 0; i < opNumBinarySelectors; ++i) {
        gSpecialBinarySelectors[i]->specialIndex = i;
    }
}
