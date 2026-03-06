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

#include "ByteCodeArray.h"
#include "OpcodeDetails.h"
#include "OpcodeIterator.h"
#include "OpcodeOperands.h"
#include "PyrObject.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"
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
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cctype>
#include "PredefinedSymbols.h"
#include "SimpleStack.h"
#include "SC_LanguageConfig.hpp"
#include "SC_Codecvt.hpp"
#include "SpecialSelectorsOperatorsAndClasses.h"


namespace fs = std::filesystem;

using namespace Opcode;

AdvancingAllocPool gParseNodePool {};


int gNumUninlinedFunctions = 0;

PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
PyrSymbol* gSpecialClasses[op_NumSpecialClasses];
SpecialValuesStruct gSpecialValues;
SpecialNumberStruct gSpecialNumbers;

PyrParseNode* gRootParseNode;
intptr_t gParserResult;

int conjureConstantIndex(PyrParseNode* node, PyrFunctionDef* func, PyrSlot* slot);
void compilePushConstant(PyrParseNode* node, PyrSlot* slot);

PyrClass* gCurrentClass = nullptr;
PyrClass* gCurrentMetaClass = nullptr;
PyrClass* gCompilingClass = nullptr;
PyrMethod* gCompilingMethod = nullptr;
PyrFunctionDef* gCompilingBlock = nullptr;
PyrFunctionDef* gPartiallyAppliedFunction = nullptr;

bool gIsTailCodeBranch = false;
bool gTailIsMethodReturn = false;
int gFunctionHighestExternalRef = 1;
bool gFunctionCantBeClosed = true;

#if TAILCALLOPTIMIZE
bool gGenerateTailCallByteCodes = true;
#else
bool gGenerateTailCallByteCodes = false;
#endif

std::int64_t gInliningLevel;

int compileErrors = 0;
int numOverwrites = 0;
std::string overwriteMsg;

extern bool compilingCmdLine;
extern int errLineOffset, errCharPosOffset;

void emitCompilerErrorFromVersion(SemanticVersion version) {
    if (SC_Version >= version) {
        compileErrors++;
    } else {
        const auto str = version.asString();
        post("WARNING: From version %s onwards the preceding error will be a compilation failure, please fix the code "
             "before updating.\n\n",
             str.c_str());
    }
}

// Forward declare helpers.
// This means they aren't a part of the public interface of the header.
void emitPushInt(int value);

void installByteCodes(PyrFunctionDef* block, CompilingBytecodes::Data codes);

void compileAnyIfMsg(PyrCallNodeBase2* node);
void compileIfMsg(PyrCallNodeBase2* node);
void compileIfNilMsg(PyrCallNodeBase2* node, bool flag);
void compileCaseMsg(PyrCallNodeBase2* node);
void compileWhileMsg(PyrCallNodeBase2* node);
void compileLoopMsg(PyrCallNodeBase2* node);
void compileAndMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileOrMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileQMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileQQMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileXQMsg(PyrParseNode* arg1, PyrParseNode* arg2);
void compileSwitchMsg(PyrCallNode* node);
void compileAssignVar(PyrParseNode* node, PyrSymbol* varName, bool drop);
void compilePushVar(PyrParseNode* node, PyrSymbol* varName);
bool isAnInlineableBlock(PyrParseNode* node);
bool isAnInlineableAtomicLiteralBlock(PyrParseNode* node);
bool isAtomicLiteral(PyrParseNode* node);
bool isWhileTrue(PyrParseNode* node);

void compilePyrMethodNode(PyrMethodNode* node, PyrSlot* result);
void compilePyrLiteralNode(PyrSlotNode* node, PyrSlot* result);

PyrClass* getNodeSuperclass(PyrClassNode* node);
void countNodeMethods(PyrClassNode* node, int* numClassMethods, int* numInstMethods);
void compileExtNodeMethods(PyrClassExtNode* node);
void countVarDefs(PyrClassNode* node);
bool compareVarDefs(PyrClassNode* node, PyrClass* classobj);
void recompileSubclasses(PyrClass* classobj);
void compileNodeMethods(PyrClassNode* node);
void fillClassPrototypes(PyrClassNode* node, PyrClass* classobj, PyrClass* superclassobj);

bool isThisObjNode(PyrParseNode* node);
int conjureSelectorIndex(PyrParseNode* node, PyrFunctionDef* func, bool isSuper, PyrSymbol* selector, int* selType);
Byte conjureLiteralSlotIndex(PyrParseNode* node, PyrFunctionDef* func, PyrSlot* slot);
bool findVarName(PyrFunctionDef* func, PyrClass** classobj, PyrSymbol* name, int* varType, int* level, int* index,
                 PyrFunctionDef** tempfunc);
void countClassVarDefs(PyrClassNode* node, int* numClassMethods, int* numInstMethods);
void dumpNodeList(PyrParseNode* node);
int compareCallArgs(PyrMethodNode* node, PyrCallNode* cnode, int* varIndex, PyrClass* specialClass);

bool findSpecialClassName(PyrSymbol* className, int* index);
int getIndexType(PyrClassNode* classnode);

CompilingBytecodes compileSubExpression(PyrSlotNode* litnode, bool onTailBranch);
CompilingBytecodes compileSubExpressionWithGoto(PyrSlotNode* litnode, int branchLen, bool onTailBranch);
CompilingBytecodes compileBodyWithGoto(PyrParseNode* body, int branchLen, bool onTailBranch);


PyrClassNode::PyrClassNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* className, PyrSlotNode* superClassName,
                           PyrSlotNode* indexType, PyrVarListNode* varlists, PyrMethodNode* methods):
    PyrParseNode({}, loc, pn_ClassNode),
    mClassName(className),
    mSuperClassName(superClassName),
    mIndexType(indexType),
    mVarlists(varlists),
    mMethods(methods) {
    assert(className);
    assert(className->mSlot.isSymbol());
    if (mSuperClassName)
        assert(superClassName->mSlot.isSymbol());
    if (indexType)
        assert(indexType->mSlot.isSymbol());
}


class SetTailIsMethodReturn {
    bool mSave;

public:
    SetTailIsMethodReturn(bool inValue) {
        mSave = gTailIsMethodReturn;
        gTailIsMethodReturn = inValue;
    }
    ~SetTailIsMethodReturn() { gTailIsMethodReturn = mSave; }
};

void emitTailCall(LocationInSourceCode loc) {
    if (gGenerateTailCallByteCodes && gIsTailCodeBranch) {
        if (gTailIsMethodReturn)
            TailCallReturnFromMethod.emit(loc);
        else
            TailCallReturnFromFunction.emit(loc);
    }
}

PyrGC* compileGC() { return gCompilingVMGlobals ? gCompilingVMGlobals->gc : nullptr; }

void initParser() {
    compileErrors = 0;
    numOverwrites = 0;
    overwriteMsg.clear();
}

void finiParser() {}

void initParseNodes() {}

void initParserPool() {
    // postfl("initPool gParseNodePool pyr_pool_compile\n");
    gParseNodePool.Init(pyr_pool_compile, 32000, 32000, 2000);
}

void freeParserPool() {
    // postfl("freePool gParseNodePool pyr_pool_compile\n");
    gParseNodePool.FreeAll();
}


void compileNodeList(PyrParseNode* node, bool onTailBranch) {
    PyrSlot dummy;
    // postfl("->compileNodeList\n");
    for (; node; node = node->mNext) {
        // postfl("-->compileNodeList %p\n", node);
        COMPILENODE(node, &dummy, onTailBranch);
        // postfl("<--compileNodeList %p\n", node);
    }
    // postfl("<-compileNodeList\n");
}

void nodePostErrorLine(PyrParseNode* node) {
    postErrorLine(node->mLocation.begin.lineNumber, linestarts[node->mLocation.begin.lineNumber],
                  node->mLocation.begin.offsetInLine);
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
    PyrFunctionDef* tempfunc;
    PyrClass* classobj;
};

std::optional<FindVarNameResult> findVarName(PyrFunctionDef* func, PyrClass* classobjC, PyrSymbol* varName) {
    int level, index, varType;
    PyrClass* classobj = classobjC;
    PyrFunctionDef* tempfunc;

    if (findVarName(func, &classobj, varName, &varType, &level, &index, &tempfunc))
        return FindVarNameResult { level, index, varType, tempfunc, classobj };
    else
        return std::nullopt;
}

void compilePushVar(PyrParseNode* node, PyrSymbol* varName) {
    const auto loc { node->location() };
    if (std::isupper(varName->name[0])) {
        if (compilingCmdLine && varName->u.classobj == nullptr) {
            error("Class not defined.\n");
            nodePostErrorLine(node);
            compileErrors++;
        } else if (const auto specialClass = findSpecialClassFromName(varName)) {
            PushSpecialClass.emit(loc, Operands::SpecialClass { *specialClass });
        } else {
            PyrSlot slot;
            SetSymbol(&slot, varName);
            PushClassX.emit(loc, Operands::Class { conjureLiteralSlotIndex(node, gCompilingBlock, &slot) });
        }

    } else if (varName == s_this || varName == s_super) {
        gFunctionCantBeClosed = true;
        PushSpecialValueThis.emit(loc);

    } else if (varName == s_true) {
        PushSpecialValue.emit(loc, { OpSpecialValue::True });
    } else if (varName == s_false) {
        PushSpecialValue.emit(loc, { OpSpecialValue::False });
    } else if (varName == s_nil) {
        PushSpecialValue.emit(loc, { OpSpecialValue::Nil_ });
    } else if (const auto result = findVarName(gCompilingBlock, gCompilingClass, varName)) {
        const FindVarNameResult findResult = *result;
        switch (findResult.varType) {
        case varInst:
            PushInstVarX.emit(loc, Operands::Index::fromRaw(findResult.index));
            break;

        case varClass: {
            const auto indexOffset = findResult.index + slotRawInt(&findResult.classobj->classVarIndex);
            if (PushClassVar.validNibble(indexOffset))
                PushClassVar.emit(loc, indexOffset);
            else
                PushClassVarX.emit(loc, Operands::UnsignedInt<16, 1>::fromFull(indexOffset),
                                   Operands::UnsignedInt<16, 0>::fromFull(indexOffset));
        } break;

        case varConst: {
            PyrSlot* slot = slotRawObject(&findResult.classobj->constValues)->slots + findResult.index;
            compilePushConstant(node, slot);
        } break;

        case varTemp: {
            const auto vindex = findResult.index;
            if (findResult.level == 0) {
                if (PushTempZeroVar.validNibble(vindex)) {
                    PushTempZeroVar.emit(loc, vindex);
                } else {
                    PushTempZeroVarX.emit(loc, Operands::Index::fromRaw(vindex));
                }
            } else if (PushTempVar.validNibble(findResult.level))
                PushTempVar.emit(loc, findResult.level, Operands::Index::fromRaw(vindex));
            else
                PushTempVarX.emit(loc, Operands::FrameOffset::fromRaw(findResult.level),
                                  Operands::Index::fromRaw(vindex));
        } break;

        case varPseudo:
            SpecialOpcode.emit(loc, Operands::PseudoVar::fromRaw(findResult.index));
            break;
        }
    } else {
        error("Variable '%s' not defined.\n", varName->name);
        nodePostErrorLine(node);
        compileErrors++;
    }
}


void PyrCurryArgNode::compile(PyrSlot* result) {
    if (gPartiallyAppliedFunction) {
        PushTempZeroVar.emit(location(), mArgNum);
    } else {
        error("found _ argument outside of a call.\n");
        nodePostErrorLine((PyrParseNode*)this);
        compileErrors++;
    }
}


void PyrSlotNode::compile(PyrSlot* result) {
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
    }
}


void PyrClassExtNode::compile(PyrSlot* result) {
    PyrClass* classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
    if (!classobj) {
        const fs::path relpath = relativeToCompileDir(fs::path(gCompilingFilenameSym->name));
        error("Class extension for nonexistent class '%s'\n     In file:'%s'\n",
              slotRawSymbol(&mClassName->mSlot)->name, SC_Codecvt::path_to_utf8_str(relpath).c_str());
        return;
    }
    gCurrentClass = classobj;
    gCurrentMetaClass = classobj->classptr;
    compileExtNodeMethods(this);
}

void compileExtNodeMethods(PyrClassExtNode* node) {
    PyrMethodNode* method;
    method = node->mMethods;
    for (; method; method = (PyrMethodNode*)method->mNext) {
        PyrSlot dummy;
        // post("compile ext %s:%s\n",
        method->mExtension = true;
        compilePyrMethodNode(method, &dummy);
    }
    gCompilingMethod = nullptr;
    gCompilingBlock = nullptr;
    gPartiallyAppliedFunction = nullptr;
    gInliningLevel = 0;
}

bool compareVarDefs(PyrClassNode* node, PyrClass* classobj) {
    assert(node);
    assert(classobj);
    assert(node->mClassName);
    int numinstvars, numclassvars;
    int i, xinst, xclass;
    PyrVarListNode* varlist;
    PyrVarDefNode* vardef;
    PyrParseNode* errnode;
    PyrSymbol** varNames;
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
                for (i = 0; vardef; vardef = (PyrVarDefNode*)vardef->mNext, xinst++, ++i) {
                    if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xinst]) {
                        errnode = (PyrParseNode*)vardef;
                        // post("A %s %d %d %d\n", vardef->mVarName->slotRawSymbol(&mSlot)->name,
                        //	vardef->mVarName->slotRawSymbol(&mSlot), varNames[xinst].us, xinst);
                        // post("A %s %s %d\n", vardef->mVarName->slotRawSymbol(&mSlot)->name,
                        //	varNames[xinst].us->name, xinst);
                        goto differExit;
                    }
                }
            } else if (type == varClass) {
                vardef = varlist->mVarDefs;
                varNames = slotRawSymbolArray(&classobj->classVarNames)->symbols;
                for (i = 0; vardef && xclass < numclassvars; vardef = (PyrVarDefNode*)vardef->mNext, xclass++, ++i) {
                    if (slotRawSymbol(&vardef->mVarName->mSlot) != varNames[xclass]) {
                        errnode = (PyrParseNode*)vardef;
                        // post("B %d %d %d\n", vardef->mVarName->slotRawSymbol(&mSlot), varNames[xclass].us, xclass);
                        goto differExit;
                    }
                }
            }
        }
    } else {
        // post("C %d %d %d   %d %d\n", numinstvars, node->mVarTally[varInst], node->mNumSuperInstVars,
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

void compileNodeMethods(PyrClassNode* node) {
    PyrMethodNode* method;
    method = node->mMethods;
    for (; method; method = (PyrMethodNode*)method->mNext) {
        PyrSlot dummy;
        method->mExtension = false;
        compilePyrMethodNode(method, &dummy);
    }
    gCompilingMethod = nullptr;
    gCompilingBlock = nullptr;
    gPartiallyAppliedFunction = nullptr;
    gInliningLevel = 0;
}

PyrClass* getNodeSuperclass(PyrClassNode* node) {
    PyrClass* superclassobj = nullptr;
    //	postfl("getNodeSuperclass node %d\n", node);
    //	postfl("getNodeSuperclass node->mSuperClassName %d\n", node->mSuperClassName);
    //	postfl("getNodeSuperclass node->mSuperClassName->mSlot.utag %d\n",
    //		node->mSuperClassName->mSlot.utag);
    if (node->mSuperClassName && IsSym(&node->mSuperClassName->mSlot)) {
        superclassobj = slotRawSymbol(&node->mSuperClassName->mSlot)->u.classobj;
        if (superclassobj == nullptr) {
            error("Cannot find superclass '%s' for class '%s'\n", slotSymString(&node->mSuperClassName->mSlot),
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

void fillClassPrototypes(PyrClassNode* node, PyrClass* classobj, PyrClass* superclassobj) {
    PyrVarListNode* varlist = nullptr;
    PyrVarDefNode* vardef = nullptr;
    PyrSlot *islot = nullptr, *cslot = nullptr, *kslot = nullptr;
    PyrSymbol **inameslot = nullptr, **cnameslot = nullptr, **knameslot = nullptr;
    PyrClass* metaclassobj = nullptr;
    PyrMethod* method = nullptr;
    PyrMethodRaw* methraw = nullptr;
    int instVarIndex, classVarIndex;

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
        case varInst:
            vardef = varlist->mVarDefs;
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode((PyrSlotNode*)vardef->mDefVal, &litslot);
                *islot++ = litslot;
                slotRawObject(&classobj->iprototype)->size++;
                *inameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->instVarNames)->size++;
                if (vardef->mFlags & rwReadOnly) {
                    // create getter method
                    method = newPyrMethod(node->location());
                    methraw = METHRAW(method);
                    methraw->unused1 = 0;
                    methraw->unused2 = 0;
                    methraw->numNormalArguments = 1;
                    methraw->numVariables = 0;
                    methraw->totalNumArguments = 1;
                    methraw->numVariableArguments = 0;
                    methraw->numtemps = 1;
                    methraw->popSize = 0;
                    SetNil(&method->contextDef);
                    SetNil(&method->varNames);
                    SetObject(&method->ownerclass, classobj);
                    if (gCompilingFilenameSym)
                        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
                    SetInt(&method->charPos,
                           linestarts[vardef->mVarName->mLocation.begin.lineNumber] + errCharPosOffset);
                    slotCopy(&method->name, &vardef->mVarName->mSlot);
                    methraw->methType = methReturnInstVar;
                    methraw->specialIndex = instVarIndex;
                    addMethod(classobj, method);
                }
                if (vardef->mFlags & rwWriteOnly) {
                    char setterName[256];
                    PyrSymbol* setterSym;
                    sprintf(setterName, "%s_", slotRawSymbol(&vardef->mVarName->mSlot)->name);
                    // underscore = strcpy(setterName, slotRawSymbol(&vardef->mVarName->mSlot)->name);
                    // underscore[0] = '_';
                    // underscore[1] = 0;
                    setterSym = getsym(setterName);
                    // create setter method
                    method = newPyrMethod(node->location());
                    methraw = METHRAW(method);
                    methraw->unused1 = 0;
                    methraw->unused2 = 0;
                    methraw->numNormalArguments = 2;
                    methraw->numVariables = 0;
                    methraw->totalNumArguments = 2;
                    methraw->numVariableArguments = 0;
                    methraw->numtemps = 2;
                    methraw->popSize = 1;
                    SetNil(&method->contextDef);
                    SetNil(&method->varNames);
                    SetObject(&method->ownerclass, classobj);
                    SetSymbol(&method->name, setterSym);
                    if (gCompilingFilenameSym)
                        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
                    SetInt(&method->charPos,
                           linestarts[vardef->mVarName->mLocation.begin.lineNumber] + errCharPosOffset);

                    methraw->methType = methAssignInstVar;
                    methraw->specialIndex = instVarIndex;
                    addMethod(classobj, method);
                }
                instVarIndex++;
            }
            break;
        case varClass:
            vardef = varlist->mVarDefs;
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode((PyrSlotNode*)vardef->mDefVal, &litslot);
                *cslot++ = litslot;
                slotRawObject(&classobj->cprototype)->size++;
                *cnameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->classVarNames)->size++;
                if (vardef->mFlags & rwReadOnly) {
                    // create getter method
                    method = newPyrMethod(node->location());
                    methraw = METHRAW(method);
                    methraw->unused1 = 0;
                    methraw->unused2 = 0;
                    methraw->numNormalArguments = 1;
                    methraw->numVariables = 0;
                    methraw->totalNumArguments = 1;
                    methraw->numVariableArguments = 0;
                    methraw->numtemps = 1;
                    methraw->popSize = 0;
                    SetNil(&method->contextDef);
                    SetNil(&method->varNames);
                    SetObject(&method->ownerclass, metaclassobj);
                    slotCopy(&method->name, &vardef->mVarName->mSlot);
                    SetSymbol(&method->selectors, slotRawSymbol(&classobj->name));
                    if (gCompilingFilenameSym)
                        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
                    SetInt(&method->charPos,
                           linestarts[vardef->mVarName->mLocation.begin.lineNumber] + errCharPosOffset);

                    methraw->methType = methReturnClassVar;
                    methraw->specialIndex = classVarIndex + slotRawInt(&classobj->classVarIndex);
                    addMethod(metaclassobj, method);
                }
                if (vardef->mFlags & rwWriteOnly) {
                    char setterName[256];
                    PyrSymbol* setterSym;
                    sprintf(setterName, "%s_", slotRawSymbol(&vardef->mVarName->mSlot)->name);
                    // underscore = strcpy(setterName, slotRawSymbol(&vardef->mVarName->mSlot)->name);
                    // underscore[0] = '_';
                    // underscore[1] = 0;
                    setterSym = getsym(setterName);
                    // create setter method
                    method = newPyrMethod(node->location());
                    methraw = METHRAW(method);
                    methraw->numNormalArguments = 2;
                    methraw->numVariables = 0;
                    methraw->totalNumArguments = 2;
                    methraw->numVariableArguments = 0;
                    methraw->numtemps = 2;
                    methraw->popSize = 1;
                    SetNil(&method->contextDef);
                    SetNil(&method->varNames);
                    SetObject(&method->ownerclass, metaclassobj);
                    SetSymbol(&method->name, setterSym);
                    SetSymbol(&method->selectors, slotRawSymbol(&classobj->name));
                    if (gCompilingFilenameSym)
                        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
                    SetInt(&method->charPos,
                           linestarts[vardef->mVarName->mLocation.begin.lineNumber] + errCharPosOffset);

                    methraw->methType = methAssignClassVar;
                    methraw->specialIndex = classVarIndex + slotRawInt(&classobj->classVarIndex);
                    addMethod(metaclassobj, method);
                }
                classVarIndex++;
            }
            break;
        case varConst:
            vardef = varlist->mVarDefs;
            for (; vardef; vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot litslot;
                compilePyrLiteralNode((PyrSlotNode*)vardef->mDefVal, &litslot);
                *kslot++ = litslot;
                slotRawObject(&classobj->constValues)->size++;
                *knameslot++ = slotRawSymbol(&vardef->mVarName->mSlot);
                slotRawSymbolArray(&classobj->constNames)->size++;
                if (vardef->mFlags & rwReadOnly) {
                    // create getter method
                    method = newPyrMethod(node->location());
                    methraw = METHRAW(method);
                    methraw->unused1 = 0;
                    methraw->unused2 = 0;
                    methraw->numNormalArguments = 1;
                    methraw->numVariables = 0;
                    methraw->totalNumArguments = 1;
                    methraw->numVariableArguments = 0;
                    methraw->numtemps = 1;
                    methraw->popSize = 0;
                    SetNil(&method->contextDef);
                    SetNil(&method->varNames);
                    SetObject(&method->ownerclass, metaclassobj);
                    slotCopy(&method->name, &vardef->mVarName->mSlot);
                    if (gCompilingFilenameSym)
                        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
                    SetInt(&method->charPos,
                           linestarts[vardef->mVarName->mLocation.begin.lineNumber] + errCharPosOffset);

                    methraw->methType = methReturnLiteral;
                    slotCopy(&method->selectors, &litslot);
                    addMethod(metaclassobj, method);
                }
            }
            break;
        }
    }

    // Vector seems faster than set here, this could change if we have a lot (100s) of members, but that seems unlikely.
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

    const auto attemptToPrintDuplicateLocation = [&](const PyrSymbol* duplicate, int varFlagType) {
        for (auto varlist = node->mVarlists; varlist; varlist = static_cast<PyrVarListNode*>(varlist->mNext)) {
            if (varlist->mFlags == varFlagType) {
                for (auto def = varlist->mVarDefs; def; def = static_cast<PyrVarDefNode*>(def->mNext)) {
                    const auto varName = def->mVarName->mSlot;
                    assert(varName.isSymbol());
                    if (varName.getSymbol() == duplicate) {
                        nodePostErrorLine(def->mVarName);
                        return;
                    }
                }
            }
        }

        // In this case, the duplicate was in the parent class.
        // Since that has to be fixed anyway, and the location has already been printed, let's not print it twice.
        post("Duplicate found in superclass %s.\n", node->mSuperClassName->mSlot.getSymbol()->name);
        nodePostErrorLine(node->mClassName);
    };

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->instVarNames))) {
        error("Found duplicate instance variable name '%s'\n", (*duplicate)->name);
        attemptToPrintDuplicateLocation(*duplicate, varInst);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
    }

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->classVarNames))) {
        error("Found duplicate class variable name '%s'\n", (*duplicate)->name);
        attemptToPrintDuplicateLocation(*duplicate, varClass);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
    }

    if (const auto duplicate = findDuplicateName(slotRawSymbolArray(&classobj->constNames))) {
        error("Found duplicate const variable name '%s'\n", (*duplicate)->name);
        attemptToPrintDuplicateLocation(*duplicate, varConst);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
    }
}

int getIndexType(PyrClassNode* classnode) {
    PyrSlotNode* node;
    int res;

    node = classnode->mIndexType;
    if (node == nullptr)
        res = obj_notindexed;
    else {
        char* name;
        name = slotRawSymbol(&node->mSlot)->name;
        if (strcmp(name, "slot") == 0)
            res = obj_slot;
        else if (strcmp(name, "double") == 0)
            res = obj_double;
        else if (strcmp(name, "float") == 0)
            res = obj_float;
        else if (strcmp(name, "int32") == 0)
            res = obj_int32;
        else if (strcmp(name, "int16") == 0)
            res = obj_int16;
        else if (strcmp(name, "int8") == 0)
            res = obj_int8;
        else if (strcmp(name, "char") == 0)
            res = obj_char;
        else if (strcmp(name, "symbol") == 0)
            res = obj_symbol;
        else {
            error("Illegal indexed type. Must be one of:\n"
                  "   slot, double, float, int8, int16, int32, char\n");
            res = obj_slot;
            compileErrors++;
        }
    }
    return res;
}

void PyrClassNode::compile(PyrSlot* result) {
    PyrClass *classobj, *superclassobj, *metaclassobj;
    int numClassMethods, numInstMethods;
    bool isIntrinsic;
    bool varsDiffer, superclassesDiffer, indexTypesDiffer;
    bool shouldRecompileSubclasses = false;
    int indexType;

    // find num instvars in superclass
    // postfl("class '%s'\n", slotRawSymbol(&mClassName->mSlot)->name);
    superclassobj = getNodeSuperclass(this);
    indexType = getIndexType(this);
    // postfl("%s %d\n", slotRawSymbol(&mClassName->mSlot)->name, indexType);

    if ((size_t)superclassobj == -1) {
        // redundant error message removed:
        // error("Can't find superclass of '%s'\n", slotRawSymbol(&mClassName->mSlot)->name);
        // nodePostErrorLine(node);
        return; // can't find superclass
    }
    mNumSuperInstVars = numSuperInstVars(superclassobj);

    numClassMethods = 0;
    numInstMethods = 0;
    countClassVarDefs(this, &numClassMethods, &numInstMethods);
    // postfl("accessor methods %d %d\n", numClassMethods, numInstMethods);
    countNodeMethods(this, &numClassMethods, &numInstMethods);
    // postfl("total methods %d %d\n", numClassMethods, numInstMethods);

    // get or make a class object
    // see if it already exists
    assert(mClassName);
    classobj = slotRawSymbol(&mClassName->mSlot)->u.classobj;
    if (classobj) {
        assert(mClassName->mSlot.isSymbol());
        // deal with intrinsic classes or other classes being recompiled here.
        // recompile of subclasses not necessary if inst and class vars are
        // unchanged.
        metaclassobj = (PyrClass*)classobj->classptr;
        isIntrinsic = slotRawInt(&classobj->classFlags) & classIsIntrinsic;

        varsDiffer = compareVarDefs(this, classobj);
        if (varsDiffer) {
            if (isIntrinsic) {
                // error("Class '%s' declaration doesn't match intrinsic definition.\n",
                //	slotRawSymbol(&mClassName->mSlot)->name);
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
                    error("Superclass of '%s' does not match intrinsic definition.\n",
                          slotRawSymbol(&mClassName->mSlot)->name);
                    nodePostErrorLine((PyrParseNode*)(mSuperClassName ? mSuperClassName : mClassName));
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
        // test
        // postfl("^1 %d %d\n", metaclassobj, class_class);
        // postfl("^2 %d %d\n", slotRawObject(&metaclassobj->iprototype), slotRawObject(&class_class->iprototype));

        memcpy(slotRawObject(&metaclassobj->iprototype)->slots, slotRawObject(&class_class->iprototype)->slots,
               sizeof(PyrSlot) * classClassNumInstVars);
        memcpy(slotRawSymbolArray(&metaclassobj->instVarNames)->symbols,
               slotRawSymbolArray(&class_class->instVarNames)->symbols, sizeof(PyrSymbol*) * classClassNumInstVars);
        slotRawObject(&metaclassobj->iprototype)->size = classClassNumInstVars;
        slotRawObject(&metaclassobj->instVarNames)->size = classClassNumInstVars;
        // end test
        classobj = newClassObj(metaclassobj, slotRawSymbol(&mClassName->mSlot), superClassName,
                               mVarTally[varInst] + mNumSuperInstVars, mVarTally[varClass], mVarTally[varConst],
                               numInstMethods, indexType, 0);
    }
    gCurrentClass = classobj;
    gCurrentMetaClass = metaclassobj;
    if (gCompilingFilenameSym) {
        SetSymbol(&classobj->filenameSym, gCompilingFilenameSym);
        SetInt(&classobj->charPos, linestarts[mClassName->mLocation.begin.lineNumber] + errCharPosOffset);
        SetSymbol(&metaclassobj->filenameSym, gCompilingFilenameSym);
        SetInt(&metaclassobj->charPos, linestarts[mClassName->mLocation.begin.lineNumber] + errCharPosOffset);
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

void recompileSubclasses(PyrClass* classobj) {}


// TODO: What does this do?
void catVarLists(PyrVarListNode* varlist) {
    PyrVarListNode* prevvarlist;
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


PyrMethodNode::PyrMethodNode(PyrParseNode::TAG, LocationType loc, PyrSlotNode* methodName, PyrSlotNode* primitiveName,
                             PyrArgListNode* arglist, PyrVarListNode* varlist, PyrParseNode* body, bool isClassMethod):
    PyrParseNode({}, loc, pn_MethodNode),
    mMethodName(methodName),
    mPrimitiveName(primitiveName),
    mArglist(arglist),
    mVarlist(varlist),
    mBody(body),
    mIsClassMethod(isClassMethod) {
    assert(methodName);
    assert(methodName->mSlot.isSymbol());
    if (primitiveName)
        assert(primitiveName->mSlot.isSymbol());
    catVarLists(mVarlist);
}

PyrBlockNode::PyrBlockNode(PyrParseNode::TAG, LocationType loc, struct PyrArgListNode* argList,
                           struct PyrVarListNode* varList, struct PyrParseNode* body, bool topLevel):
    PyrParseNode({}, loc, pn_BlockNode),
    mArglist(argList),
    mVarlist(varList),
    mBody(body),
    mIsTopLevel(topLevel) {
    assert(mBody);
    catVarLists(mVarlist);
}


enum { push_Normal, push_AllArgs, push_AllButFirstArg, push_AllButFirstArg2 };

int checkPushAllArgs(PyrParseNode* actualArg, int numArgs);
int checkPushAllArgs(PyrParseNode* actualArg, int numArgs) {
    PyrFunctionDef* block;
    PyrSlotNode* nameNode;
    block = gCompilingBlock;
    int i;

    // if (strcmp("ar", slotRawSymbol(&gCompilingMethod->name)->name)==0) Debugger();
    if (actualArg->mClassno != pn_PushNameNode) {
        if (numArgs < 3) {
            return push_Normal;
        }
        actualArg = actualArg->mNext;
        for (i = 1; i < numArgs; ++i) {
            if (actualArg->mClassno != pn_PushNameNode) {
                return push_Normal;
            }
            nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }

            actualArg = actualArg->mNext;
        }
        return push_AllButFirstArg;

    } else {
        for (i = 0; i < numArgs; ++i) {
            if (actualArg->mClassno != pn_PushNameNode) {
                return push_Normal;
            }
            nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }
            actualArg = actualArg->mNext;
        }
        return push_AllArgs;
    }
}


int checkPushAllButFirstTwoArgs(PyrParseNode* actualArg, int numArgs);
int checkPushAllButFirstTwoArgs(PyrParseNode* actualArg, int numArgs) {
    PyrFunctionDef* block;
    PyrSlotNode* nameNode;
    block = gCompilingBlock;
    int i;

    if (numArgs >= 2) {
        actualArg = actualArg->mNext;
        actualArg = actualArg->mNext;
        for (i = 1; i < numArgs; ++i) {
            if (actualArg->mClassno != pn_PushNameNode) {
                return push_Normal;
            }
            nameNode = (PyrSlotNode*)actualArg;
            if (slotRawSymbol(&nameNode->mSlot) != slotRawSymbolArray(&block->argNames)->symbols[i]) {
                return push_Normal;
            }

            actualArg = actualArg->mNext;
        }
        return push_AllButFirstArg2;
    }
    return push_Normal;
}

int compareCallArgs(PyrMethodNode* node, PyrCallNode* cnode, int* varIndex, PyrClass** specialClass) {
    int i, numFormalArgs, numActualArgs;
    int special, varType, varLevel;
    PyrParseNode* actualArg;
    PyrVarDefNode* formalArg;
    PyrSlotNode* nameNode;

    // fail if has a rest arg .. too much trouble?
    if (node->mArglist && node->mArglist->mVariableArgumentName) {
        return methNormal;
    }

    // check first actual arg is 'this'
    actualArg = cnode->mArglist;
    if (actualArg->mClassno != pn_PushNameNode) {
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

        classobj = gCompilingClass;
        varFound = findVarName(gCompilingBlock, &classobj, slotRawSymbol(&nameNode->mSlot), &varType, &varLevel,
                               varIndex, nullptr);
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
            if (actualArg->mClassno != pn_PushNameNode) {
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

void installByteCodes(PyrFunctionDef* fdef, CompilingBytecodes::Data data) {
    // Print all bytecodes.
    // {
    // 	auto start = data.codes.begin().base();
    // 	const auto end = data.codes.end().base();
    // 	Opcode::printAllBytecodes(std::cout, start, end);
    // 	std::cout << std::endl;
    // }

    const auto length = data.codes.size();
    if (length == 0) {
        error("installByteCodes: zero length byte codes\n");
        assert(false);
        return;
    }

    const auto flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;

    // bytecodes
    auto byteArray = newPyrInt8Array(compileGC(), length, flags, false);
    memcpy(byteArray->b, data.codes.data(), length * sizeof(Byte));
    byteArray->size = static_cast<int>(length);
    fdef->code = PyrSlot::make(byteArray);

    // bytecode locations
    const auto locLength = data.startAndEndLocations.size();
    auto locArray = newPyrInt32Array(compileGC(), locLength, flags, false);
    memcpy(locArray->i, data.startAndEndLocations.data(), locLength * sizeof(uint32_t));
    locArray->size = static_cast<int>(locLength);
    fdef->codeCharacterLocations = PyrSlot::make(locArray);

    // bytecode sizes --- almost no codes take up a single byte.
    const auto codeSizesLength = data.sizeOfCodes.size();
    auto szArray = newPyrInt8Array(compileGC(), codeSizesLength, flags, false);
    memcpy(szArray->b, data.sizeOfCodes.data(), codeSizesLength * sizeof(uint8_t));
    szArray->size = codeSizesLength;
    fdef->codeSizes = PyrSlot::make(szArray);

    // source
    assert(gCurrentCompilingTextAsSCString);
    fdef->sourceCode = PyrSlot::make(gCurrentCompilingTextAsSCString);

    assert(fdef->sourceCodeStartIndex.isInt());
    assert(fdef->sourceCodeEndIndex.isInt());
}

PyrMethod* initPyrMethod(PyrMethod* method, LocationInSourceCode loc);

void compilePyrMethodNode(PyrMethodNode* node, PyrSlot* result) { node->compile(result); }

void PyrMethodNode::compile(PyrSlot* result) {
    PyrMethod *method, *oldmethod;
    PyrMethodRaw* methraw;
    int i, j, numArgs, numVars, methType, firstKeyIndex;
    int numVariableArgs = 0;
    int numVariableKwArgs = 0;
    int index, numSlots, numArgNames;
    bool hasPrimitive = false;
    bool hasVarExprs = false;
    PyrVarDefNode* vardef;
    PyrObject* proto;
    PyrSymbolArray *argNames, *varNames;

    SetTailBranch branch(false);

    // postfl("->method '%s'\n", slotRawSymbol(&mMethodName->mSlot)->name);
    gCompilingClass = mIsClassMethod ? gCurrentMetaClass : gCurrentClass;
    oldmethod = classFindDirectMethod(gCompilingClass, slotRawSymbol(&mMethodName->mSlot));

    if (oldmethod && !mExtension) {
        error("Method %s:%s already defined.\n", slotRawSymbol(&slotRawClass(&oldmethod->ownerclass)->name)->name,
              slotRawSymbol(&oldmethod->name)->name);
        nodePostErrorLine((PyrParseNode*)mMethodName);
        compileErrors++;
        return;
    }

    if (oldmethod) {
        ++numOverwrites;

        // accumulate overwrite message onto the string buffer
        overwriteMsg.append(slotRawSymbol(&slotRawClass(&oldmethod->ownerclass)->name)->name)
            .append(":")
            .append(slotRawSymbol(&oldmethod->name)->name)
            .append("\t")
            .append(gCompilingFilenameSym->name)
            .append("\t")
            .append(slotRawSymbol(&oldmethod->filenameSym)->name)
            .append("\n");

        method = oldmethod;
        freePyrSlot(&method->code);
        freePyrSlot(&method->selectors);
        freePyrSlot(&method->prototypeFrame);
        freePyrSlot(&method->argNames);
        freePyrSlot(&method->varNames);
        initPyrMethod(method, location());
    } else {
        method = newPyrMethod(location());
    }
    SetObject(&method->ownerclass, gCompilingClass);

    methraw = METHRAW(method);
    methraw->unused1 = 0;
    methraw->unused2 = 0;

    // postfl("method %p raw %p\n", method, methraw);
    method->contextDef = o_nil;
    method->name = mMethodName->mSlot;
    if (gCompilingFilenameSym)
        SetSymbol(&method->filenameSym, gCompilingFilenameSym);
    SetInt(&method->charPos, linestarts[mMethodName->mLocation.begin.lineNumber] + errCharPosOffset);
    if (mPrimitiveName) {
        hasPrimitive = true;
        method->primitiveName = mPrimitiveName->mSlot;
        methraw->specialIndex = slotRawSymbol(&mPrimitiveName->mSlot)->u.index;
    }
    gCompilingBlock = (PyrFunctionDef*)method;
    gCompilingMethod = (PyrMethod*)method;
    gPartiallyAppliedFunction = nullptr;
    gInliningLevel = 0;

    methraw->needsHeapContext = 0;

    methraw->numVariableArguments = 0;
    if (mArglist) {
        if (mArglist->mVariableArgumentName) {
            methraw->numVariableArguments += 1;
            numVariableArgs = 1;
            if (mArglist->mVariableKeywordArgumentName) {
                methraw->numVariableArguments += 1;
                numVariableKwArgs = 1;
            }
        }
    }
    numArgs = mArglist ? nodeListLength((PyrParseNode*)mArglist->mVarDefs) + 1 : 1;
    numVars = mVarlist ? nodeListLength((PyrParseNode*)mVarlist->mVarDefs) : 0;

    numSlots = numArgs + numVariableArgs + numVariableKwArgs + numVars;
    methraw->frameSize = (numSlots + FRAMESIZE) * sizeof(PyrSlot);

    methraw->numNormalArguments = numArgs;
    methraw->numVariables = numVars;
    methraw->totalNumArguments = numArgs + numVariableArgs + numVariableKwArgs;
    methraw->numtemps = numSlots;
    methraw->popSize = numSlots - 1;
    firstKeyIndex = numArgs + numVariableArgs + numVariableKwArgs;

    numArgNames = methraw->totalNumArguments;

    if (numSlots == 1) {
        slotCopy(&method->argNames, &o_argnamethis);
        slotCopy(&method->prototypeFrame, &o_onenilarray);
    } else {
        argNames = newPyrSymbolArray(nullptr, numArgNames, obj_permanent | obj_immutable, false);
        argNames->size = numArgNames;
        SetObject(&method->argNames, argNames);

        proto = newPyrArray(nullptr, numSlots, obj_permanent | obj_immutable, false);
        proto->size = numSlots;
        SetObject(&method->prototypeFrame, proto);

        // declare args
        slotRawSymbolArray(&method->argNames)->symbols[0] = s_this;
        if (mArglist) {
            PyrSymbol** methargs;
            methargs = slotRawSymbolArray(&method->argNames)->symbols;
            vardef = mArglist->mVarDefs;
            for (i = 1; i < numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot* varslot;
                varslot = &vardef->mVarName->mSlot;
                // already declared as arg?
                for (j = 0; j < i; ++j) {
                    if (methargs[j] == slotRawSymbol(varslot)) {
                        error("Argument '%s' already declared in %s:%s\n", slotRawSymbol(varslot)->name,
                              slotRawSymbol(&gCompilingClass->name)->name,
                              slotRawSymbol(&gCompilingMethod->name)->name);
                        nodePostErrorLine((PyrParseNode*)vardef);
                        compileErrors++;
                    }
                }
                // put it in arglist
                methargs[i] = slotRawSymbol(varslot);
                // postfl("defarg %d '%s'\n", i, slotRawSymbol(slot)->name);
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
            if (numVariableArgs > 0) {
                PyrSlot* varslot;
                varslot = &mArglist->mVariableArgumentName->mSlot;
                // already declared as arg?
                for (j = 0; j < numArgs; ++j) {
                    if (methargs[j] == slotRawSymbol(varslot)) {
                        error("Argument '%s' already declared in %s:%s\n", slotRawSymbol(varslot)->name,
                              slotRawSymbol(&gCompilingClass->name)->name,
                              slotRawSymbol(&gCompilingMethod->name)->name);
                        nodePostErrorLine((PyrParseNode*)vardef);
                        compileErrors++;
                    }
                }
                // put it in arglist
                methargs[i] = slotRawSymbol(varslot);
                // postfl("defrest '%s'\n", slotRawSymbol(slot)->name);


                if (numVariableKwArgs > 0) {
                    // 'i' is the variable used in the for loop. Be careful of this!
                    i += 1;
                    PyrSlot* kwvarslot;
                    kwvarslot = &mArglist->mVariableKeywordArgumentName->mSlot;
                    // already declared as arg?
                    // Add one here to numArgs to include the name of the variableArgument slot
                    for (j = 0; j < numArgs + 1; ++j) {
                        if (methargs[j] == slotRawSymbol(kwvarslot)) {
                            error("Argument '%s' already declared in %s:%s\n", slotRawSymbol(kwvarslot)->name,
                                  slotRawSymbol(&gCompilingClass->name)->name,
                                  slotRawSymbol(&gCompilingMethod->name)->name);
                            nodePostErrorLine((PyrParseNode*)kwvarslot);
                            compileErrors++;
                        }
                    }
                    methargs[i] = slotRawSymbol(kwvarslot);
                }
            }
        }
        // fill prototype args
        if (NotNil(&method->prototypeFrame)) {
            SetNil(&slotRawObject(&method->prototypeFrame)->slots[0]);
        }
        if (mArglist) {
            vardef = mArglist->mVarDefs;
            for (i = 1; i < numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
                PyrSlot *slot, litval;
                slot = slotRawObject(&method->prototypeFrame)->slots + i;
                if (vardef->hasExpr(&litval))
                    hasVarExprs = true;
                *slot = litval;
            }
        }
        if (numVariableArgs > 0) {
            slotRawObject(&method->prototypeFrame)->slots[numArgs] = o_emptyarray;
            if (numVariableKwArgs > 0) {
                slotRawObject(&method->prototypeFrame)->slots[numArgs + 1] = o_emptyarray;
            }
        }
    }

    if (numVars) {
        varNames = newPyrSymbolArray(nullptr, numVars, obj_permanent | obj_immutable, false);
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
        for (i = 0; i < numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
            PyrSlot* varslot;
            varslot = &vardef->mVarName->mSlot;
            // already declared as arg?
            for (j = 0; j < numArgNames; ++j) {
                if (methargs[j] == slotRawSymbol(varslot)) {
                    error("Variable '%s' already declared in %s:%s\n", slotRawSymbol(varslot)->name,
                          slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
                    nodePostErrorLine((PyrParseNode*)vardef);
                    compileErrors++;
                }
            }
            // already declared as var?
            for (j = 0; j < i; ++j) {
                if (methvars[j] == slotRawSymbol(varslot)) {
                    error("Variable '%s' already declared in %s:%s\n", slotRawSymbol(varslot)->name,
                          slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
                    nodePostErrorLine((PyrParseNode*)vardef);
                    compileErrors++;
                }
            }
            // put it in mVarlist
            methvars[i] = slotRawSymbol(varslot);
            // postfl("defvar %d '%s'\n", i, slotRawSymbol(varslot)->name);
        }
    }

    if (mVarlist) {
        vardef = mVarlist->mVarDefs;
        for (i = 0; i < numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
            PyrSlot *slot, litval;
            slot = slotRawObject(&method->prototypeFrame)->slots + i + numArgs + numVariableArgs + numVariableKwArgs;
            if (vardef->hasExpr(&litval))
                hasVarExprs = true;
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
            PyrReturnNode* rnode;
            PyrParseNode* xnode;
            int rtype;
            PyrSlot rslot;

            rnode = (PyrReturnNode*)mBody;
            xnode = (PyrParseNode*)rnode->mExpr;
            if (xnode) {
                rtype = xnode->mClassno;
                if (rtype == pn_PushLitNode) { // return literal ?
                    compilePyrLiteralNode((PyrSlotNode*)xnode, &rslot);
                    if (IsObj(&rslot) && slotRawObject(&rslot)->classptr == class_fundef) {
                        methType = methNormal;
                    } else {
                        methType = methReturnLiteral;
                        method->selectors = rslot;
                    }
                } else if (rtype == pn_PushNameNode) {
                    PyrSlot* rslot;
                    rslot = &((PyrSlotNode*)xnode)->mSlot;
                    if (slotRawSymbol(rslot) == s_this) { // return this
                        methType = methReturnSelf;
                    } else {
                        if (funcFindArg((PyrFunctionDef*)method, slotRawSymbol(rslot), &index)) { // return arg ?
                            // eliminate the case where its an ellipsis or keyword argument
                            if (index < methraw->numNormalArguments) {
                                methType = methReturnArg;
                                methraw->specialIndex = index; // when you change sp to sp - 1
                                // methraw->specialIndex = index - 1;
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
                    PyrCallNode* cnode;
                    PyrClass* specialClass = nullptr;
                    cnode = (PyrCallNode*)xnode;
                    // The optimization breaks when there are keyword arguments in the call.
                    if (cnode->mKeyarglist) {
                        methType = methNormal;
                    } else {
                        methType = compareCallArgs(this, cnode, &specialIndex, &specialClass);
                    }
                    if (methType != methNormal) {
                        methraw->specialIndex = specialIndex;
                        method->selectors = cnode->mSelector->mSlot;
                        if (specialClass)
                            method->constants = specialClass->name;
                    }
                }
            } else {
                methType = methReturnSelf;
            }
        } else if (bodyType == pn_AssignNode && numArgs == 2) { // assign inst var ?
            PyrAssignNode* anode;
            // post("methAssignInstVar 1  %s:%s\n",
            //	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
            anode = (PyrAssignNode*)mBody;
            if (anode->mNext && anode->mNext->mClassno == pn_ReturnNode
                && ((PyrReturnNode*)anode->mNext)->mExpr == nullptr) {
                // post("methAssignInstVar 2  %s:%s\n",
                //	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
                if (classFindInstVar(gCompilingClass, slotRawSymbol(&anode->mVarName->mSlot), &index)) {
                    methType = methAssignInstVar;
                    methraw->specialIndex = index;
                    // post("methAssignInstVar 3  %s:%s\n",
                    //	slotRawSymbol(&gCompilingClass->name)->name, slotRawSymbol(&gCompilingMethod->name)->name);
                }
            }
        }
    }

    methraw->methType = methType;
    // set primitive
    // optimize common cases

    if (methType == methNormal || methType == methPrimitive) {
        gCompilingBytecodes.assertEmpty();
        // initByteCodes();

        if (gCompilingClass == class_int) {
            const PyrSymbol* name = slotRawSymbol(&method->name);
            if (name == gSpecialSelectors[opmDo]) {
                const auto loc = location();
                Extended::IntegerDo.emit(location());
            } else if (name == gSpecialSelectors[opmReverseDo]) {
                Extended::IntegerReverseDo.emit(location());
            } else if (name == gSpecialSelectors[opmFor]) {
                Extended::IntegerFor.emit(location());
            } else if (name == gSpecialSelectors[opmForBy]) {
                Extended::IntegerForBy.emit(location());
            } else
                goto compile_body;
        } else if (gCompilingClass == class_arrayed_collection) {
            const PyrSymbol* name = slotRawSymbol(&method->name);
            if (name == gSpecialSelectors[opmDo]) {
                Extended::ArrayedCollectionDo.emit(location());
            } else if (name == gSpecialSelectors[opmReverseDo]) {
                Extended::ArrayedCollectionReversedDo.emit(location());
            } else
                goto compile_body;
        } else if (slotRawSymbol(&gCompilingClass->name) == s_dictionary) {
            const PyrSymbol* name = slotRawSymbol(&method->name);
            if (name == getsym("keysValuesArrayDo")) {
                Extended::DictionaryKeyValuesArrayDo.emit(location());
            } else
                goto compile_body;
        } else if (gCompilingClass == class_number) {
            const PyrSymbol* name = slotRawSymbol(&method->name);
            if (name == gSpecialSelectors[opmForSeries]) {
                Extended::NumberForSeries.emit(location());
            } else
                goto compile_body;
        } else if (gCompilingClass == class_float) {
            const PyrSymbol* name = slotRawSymbol(&method->name);
            if (name == gSpecialSelectors[opmDo]) {
                Extended::FloatDo.emit(location());
            } else if (name == gSpecialSelectors[opmReverseDo]) {
                Extended::FloatDoReverse.emit(location());
            } else
                goto compile_body;
        } else {
        compile_body:
            SetTailIsMethodReturn mr(false);
            PyrSlot dummy;
            if (mArglist) {
                vardef = mArglist->mVarDefs;
                for (i = 1; i < numArgs; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
                    vardef->compileArg(&dummy);
                }
            }
            if (mVarlist) {
                vardef = mVarlist->mVarDefs;
                for (i = 0; i < numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext) {
                    vardef->compile(&dummy);
                }
            }
            COMPILENODE(mBody, &dummy, true);
        }
        installByteCodes(method, std::move(gCompilingBytecodes).finishGetData());
    }

    if (!oldmethod) {
        addMethod(gCompilingClass, method);
    }

    gCompilingMethod = nullptr;
    gCompilingBlock = nullptr;
    gPartiallyAppliedFunction = nullptr;

    // postfl("<-method '%s'\n", slotRawSymbol(&mMethodName->mSlot)->name);
}


void PyrArgListNode::compile(PyrSlot* result) {
    error("compilePyrArgListNode: shculdn't get here.\n");
    compileErrors++;
}


void PyrVarListNode::compile(PyrSlot* result) {
    error("compilePyrVarListNode: shouldn't get here.\n");
    compileErrors++;
}


bool PyrVarDefNode::hasExpr(PyrSlot* result) {
    if (result)
        SetNil(result);
    if (!mDefVal)
        return false;
    if (mDefVal->mClassno != pn_PushLitNode && mDefVal->mClassno != pn_LiteralNode) {
        // post("hasExpr A %s:%s %s %d\n", slotRawSymbol(&gCompilingClass->name)->name,
        // slotRawSymbol(&gCompilingMethod->name)->name, mVarName->slotRawSymbol(&mSlot)->name, mDefVal->mClassno);
        return true;
    }
    PyrSlotNode* node = (PyrSlotNode*)mDefVal;

    if (IsPtr(&node->mSlot)) {
        PyrParseNode* litnode = (PyrParseNode*)slotRawPtr(&node->mSlot);
        if (litnode) {
            if (litnode->mClassno == pn_BlockNode || litnode->mClassno == pn_NewBlock) {
                // post("hasExpr B %s:%s %s %d\n", slotRawSymbol(&gCompilingClass->name)->name,
                // slotRawSymbol(&gCompilingMethod->name)->name, mVarName->slotRawSymbol(&mSlot)->name, node->mClassno);
                return true;
            } else {
                if (result)
                    node->compileLiteral(result);
            }
        }
    } else if (result)
        *result = node->mSlot;
    if (node->mParens)
        return true;
    return false;
}

void PyrVarDefNode::compile(PyrSlot* result) {
    if (hasExpr(nullptr)) {
        COMPILENODE(mDefVal, result, false);
        compileAssignVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
    }
}

void PyrVarDefNode::compileArg(PyrSlot* result) {
    // If not a literal.
    // hasExpr returns as an out param the literal value is present, which should be placed in the prototypeframe at
    // compile time.
    if (hasExpr(nullptr)) {
        compilePushVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot));

        mDrop = false;
        auto trueByteCodes = compileBodyWithGoto(this, 0, true);
        const auto jumplen = trueByteCodes.length();

        Extended::IfNotNilJumpPushNilElsePop.emit(
            location(),
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });

        gCompilingBytecodes.consume(std::move(trueByteCodes));
        Drop.emit(location());
    }

    // error("compilePyrVarDefNode: shouldn't get here.\n");
    // compileErrors++;
}


int PyrCallNode::isPartialApplication() {
    int sum = 0;
    PyrParseNode* argnode = mArglist;
    for (; argnode; argnode = argnode->mNext) {
        if (argnode->mClassno == pn_CurryArgNode) {
            ((PyrCurryArgNode*)argnode)->mArgNum = sum;
            sum++;
        }
    }

    PyrParseNode* keynode = mKeyarglist;
    for (; keynode; keynode = keynode->mNext) {
        if (keynode->mClassno == pn_CurryArgNode) {
            ((PyrCurryArgNode*)keynode)->mArgNum = sum;
            sum++;
        }
    }
    return sum;
}

void PyrCallNodeBase::compilePartialApplication(int numCurryArgs, PyrSlot* result) {
    // create a function
    // compile the call


    CompilingBytecodes savedBytecodes { std::move(gCompilingBytecodes) };

    int flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
    PyrFunctionDef* block = newPyrFunctionDef(flags, location());

    PyrSlot blockSlot;
    SetObject(&blockSlot, block);

    int prevFunctionHighestExternalRef = gFunctionHighestExternalRef;
    bool prevFunctionCantBeClosed = gFunctionCantBeClosed;
    gFunctionHighestExternalRef = 0;
    gFunctionCantBeClosed = false;

    PyrClass* prevClass = gCompilingClass;
    PyrFunctionDef* prevBlock = gCompilingBlock;
    gCompilingBlock = block;

    PyrFunctionDef* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
    gPartiallyAppliedFunction = block;

    PyrMethodRaw* methraw = METHRAW(block);
    methraw->unused1 = 0;
    methraw->unused2 = 0;

    methraw->needsHeapContext = 0;

    SetObject(&block->contextDef, prevBlock);
    ////
    methraw->numVariableArguments = 0;

    methraw->frameSize = (numCurryArgs + FRAMESIZE) * sizeof(PyrSlot);
    PyrObject* proto = newPyrArray(compileGC(), numCurryArgs, flags, false);
    proto->size = numCurryArgs;
    SetObject(&block->prototypeFrame, proto);

    PyrSymbolArray* argNames = newPyrSymbolArray(compileGC(), numCurryArgs, flags, false);
    argNames->size = numCurryArgs;
    SetObject(&block->argNames, argNames);

    SetNil(&block->varNames);

    methraw->numNormalArguments = numCurryArgs;
    methraw->numVariables = 0;
    methraw->totalNumArguments = numCurryArgs;
    methraw->numtemps = numCurryArgs;
    methraw->popSize = numCurryArgs;
    methraw->methType = methBlock;

    {
        PyrSymbol* s_empty = getsym("_");
        PyrSymbol** blockargs = slotRawSymbolArray(&block->argNames)->symbols;
        for (int i = 0; i < numCurryArgs; ++i) {
            // put it in mArglist
            blockargs[i] = s_empty;
            SetNil(proto->slots + i);
        }
    }

    // initByteCodes();
    gCompilingBytecodes.assertEmpty();
    {
        SetTailBranch branch(true);
        SetTailIsMethodReturn mr(false);
        PyrSlot body;
        compileCall(&body);
    }

    BlockReturn.emit(location());
    installByteCodes(block, std::move(gCompilingBytecodes).finishGetData());

    gCompilingBlock = prevBlock;
    gPartiallyAppliedFunction = prevPartiallyAppliedFunction;

    gCompilingBytecodes = std::move(savedBytecodes);
    PushLiteralX.emit(location(), Operands::Index::fromRaw(conjureLiteralSlotIndex(this, gCompilingBlock, &blockSlot)));

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

void PyrCallNodeBase::compile(PyrSlot* result) {
    int numCurryArgs = isPartialApplication();
    if (numCurryArgs) {
        compilePartialApplication(numCurryArgs, result);
    } else {
        compileCall(result);
    }
}

bool isSeries(PyrParseNode* node, PyrParseNode** args) {
    if (node->mClassno != pn_CallNode)
        return false;
    PyrCallNode* callnode = (PyrCallNode*)node;
    if (slotRawSymbol(&callnode->mSelector->mSlot) != s_series)
        return false;
    if (callnode->mKeyarglist)
        return false;
    *args = callnode->mArglist;
    return true;
}

void PyrCallNode::compileCall(PyrSlot* result) {
    PyrSlot dummy;
    PyrParseNode* argnode2;

    PyrParseNode* argnode = mArglist;
    PyrParseNode* keynode = mKeyarglist;
    int numArgs = nodeListLength(argnode);
    int numKeyArgs = nodeListLength(keynode);
    int isSuper = isSuperObjNode(argnode);
    int numBlockArgs = METHRAW(gCompilingBlock)->numNormalArguments;

    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
    int selType;
    auto selectorSlotOrSpecialIndex = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock, isSuper,
                                                           slotRawSymbol(&mSelector->mSlot), &selType);

    if (numKeyArgs > 0 || (numArgs > 15 && !(selType == selSwitch || selType == selCase))) {
        for (; argnode; argnode = argnode->mNext)
            COMPILENODE(argnode, &dummy, false);
        for (; keynode; keynode = keynode->mNext)
            COMPILENODE(keynode, &dummy, false);

        if (isSuper) {
            emitTailCall(location());
            assert(selType == selNormal);
            SendSuperMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                               Operands::KwArgumentCount::fromRaw(numKeyArgs),
                               Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
        } else {
            switch (selType) {
            case selNormal:
                // When the selector type is normal, conjureSelectorIndex has added the symbol to the functiondef's
                // selector array and we just send a normal message.
                emitTailCall(location());
                SendMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                              Operands::KwArgumentCount::fromRaw(numKeyArgs),
                              Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                break;

            case selUnary:
                [[fallthrough]];
            case selBinary: {
                // When the selector is of the type unary or binary, no selector has been emited to the function def.
                // This is because it is indented to be called with special bytes codes for the unary and binary message
                // format respectively, however, these do not take kwargs. Therefore, we put the selector into the
                // function def and use its index for a normal message send.
                const auto selectorSlotIndex =
                    conjureLiteralSlotIndex((PyrParseNode*)mSelector, gCompilingBlock, &mSelector->mSlot);
                emitTailCall(location());
                SendMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                              Operands::KwArgumentCount::fromRaw(numKeyArgs),
                              Operands::SelectorIndex::fromRaw(selectorSlotIndex));
                break;
            }

            default:
                // In this case, the selector is a special one, and we can use the send special message.
                emitTailCall(location());
                SendSpecialMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs + 2 * numKeyArgs),
                                     Operands::KwArgumentCount::fromRaw(numKeyArgs),
                                     Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                break;
            }
        }
    } else if (isSuper) {
        if (numArgs == 1) {
            // No need to compile the 'this' arg.
            gFunctionCantBeClosed = true;
            emitTailCall(location());
            SendSuperMsgThisOpt.emit(location(), Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
        } else {
            for (; argnode; argnode = argnode->mNext)
                COMPILENODE(argnode, &dummy, false);
            emitTailCall(location());
            if (SendSuperMsg.validNibble(numArgs)) {
                SendSuperMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else {
                SendSuperMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs),
                                   Operands::KwArgumentCount::fromRaw(0),
                                   Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
        }

    } else {
        PyrSymbol* varname =
            (argnode->mClassno == pn_PushNameNode) ? slotRawSymbol(&((PyrSlotNode*)argnode)->mSlot) : nullptr;

        if (varname == s_this)
            gFunctionCantBeClosed = true;

        switch (selType) {
        case selNormal: {
            if (numArgs == 1 && varname == s_this) {
                emitTailCall(location());
                SendMsgThisOpt.emit(location(), Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else if (numArgs > 1 && numArgs == numBlockArgs) {
                switch (checkPushAllArgs(argnode, numArgs)) {
                case push_Normal:
                    goto normal;

                case push_AllArgs: {
                    emitTailCall(location());
                    PushAllArgsAndSendMsg.emit(location(),
                                               Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                case push_AllButFirstArg: {
                    COMPILENODE(argnode, &dummy, false);
                    emitTailCall(location());
                    PushAllButFirstArgAndSendMsg.emit(location(),
                                                      Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto normal;
                }

            } else if (numArgs > 2 && numArgs == numBlockArgs + 1) {
                switch (checkPushAllButFirstTwoArgs(argnode, numBlockArgs)) {
                case push_Normal:
                    goto normal;

                case push_AllButFirstArg2: {
                    COMPILENODE(argnode, &dummy, false);
                    COMPILENODE(argnode->mNext, &dummy, false);
                    emitTailCall(location());
                    PushAllButFirstTwoArgsAndSendMsg.emit(location(),
                                                          Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto normal;
                }

            } else {
            normal:
                for (; argnode; argnode = argnode->mNext)
                    COMPILENODE(argnode, &dummy, false);
                emitTailCall(location());

                if (SendMsg.validNibble(numArgs))
                    SendMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs),
                                  Operands::KwArgumentCount::fromRaw(0),
                                  Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
        } break;

        case selSpecial:
            if (numArgs == 1) {
                if (varname == s_this) {
                    emitTailCall(location());
                    SendSpecialMsgThisOpt.emit(location(), Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } else if (varname) {
                    if (const auto result = findVarName(gCompilingBlock, gCompilingClass, varname);
                        result && result->varType == varInst) {
                        emitTailCall(location());
                        PushInstVarAndSendSpecialMsg.emit(location(), Operands::Index::fromRaw(result->index),
                                                          Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                    } else
                        goto special;

                } else
                    goto special;

            } else if (selectorSlotOrSpecialIndex == opmDo && isSeries(argnode, &argnode)) {
                selectorSlotOrSpecialIndex = opmForSeries;
                mArglist = linkNextNode(argnode, mArglist->mNext);
                numArgs = nodeListLength(mArglist);
                goto special;

            } else if (numArgs > 1 && numArgs == numBlockArgs) {
                switch (checkPushAllArgs(argnode, numArgs)) {
                case push_Normal:
                    goto special;

                case push_AllArgs: {
                    emitTailCall(location());
                    PushAllArgsAndSendSpecialMsg.emit(location(), Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                case push_AllButFirstArg: {
                    COMPILENODE(argnode, &dummy, false);
                    emitTailCall(location());
                    PushAllButFirstArgAndSendSpecialMsg.emit(location(),
                                                             Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto special;
                }

            } else if (numArgs > 2 && numArgs == numBlockArgs + 1) {
                switch (checkPushAllArgs(argnode, numBlockArgs)) {
                case push_Normal:
                    goto special;

                case push_AllButFirstArg2: {
                    COMPILENODE(argnode, &dummy, false);
                    COMPILENODE(argnode->mNext, &dummy, false);
                    emitTailCall(location());
                    PushAllButFirstTwoArgsAndSendSpecialMsg.emit(location(),
                                                                 Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
                } break;

                default:
                    goto special;
                }

            } else {
            special:
                for (; argnode; argnode = argnode->mNext)
                    COMPILENODE(argnode, &dummy, false);
                emitTailCall(location());
                if (SendSpecialMsg.validNibble(numArgs))
                    SendSpecialMsg.emit(location(), numArgs,
                                        Operands::SpecialSelectors::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendSpecialMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs),
                                         Operands::KwArgumentCount::fromRaw(0),
                                         Operands::Index::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;

        case selUnary: {
            if (numArgs != 1) {
                selectorSlotOrSpecialIndex =
                    conjureLiteralSlotIndex((PyrParseNode*)mSelector, gCompilingBlock, &mSelector->mSlot);
                goto defaultCase;
            }
            for (; argnode; argnode = argnode->mNext)
                COMPILENODE(argnode, &dummy, false);

            emitTailCall(location());
            SendSpecialUnaryArithMsgX.emit(location(), Operands::UnaryMath::fromRaw(selectorSlotOrSpecialIndex));
        } break;

        case selBinary:
            if (numArgs != 2) {
                selectorSlotOrSpecialIndex =
                    conjureLiteralSlotIndex((PyrParseNode*)mSelector, gCompilingBlock, &mSelector->mSlot);
                goto defaultCase;
            }
            argnode2 = argnode->mNext;
            if (selectorSlotOrSpecialIndex == static_cast<int>(OpBinaryMath::Add)
                && argnode2->mClassno == pn_PushLitNode && IsInt(&((PyrSlotNode*)argnode2)->mSlot)
                && slotRawInt(&((PyrSlotNode*)argnode2)->mSlot) == 1) {
                COMPILENODE(argnode, &dummy, false);
                PushOneAndAddOne.emit(location());
            } else if (selectorSlotOrSpecialIndex == opSub && argnode2->mClassno == pn_PushLitNode
                       && IsInt(&((PyrSlotNode*)argnode2)->mSlot)
                       && slotRawInt(&((PyrSlotNode*)argnode2)->mSlot) == 1) {
                COMPILENODE(argnode, &dummy, false);
                PushOneAndSubtract.emit(location());
            } else {
                COMPILENODE(argnode, &dummy, false);
                COMPILENODE(argnode->mNext, &dummy, false);
                emitTailCall(location());
                if (selectorSlotOrSpecialIndex < 16)
                    SendSpecialBinaryArithMsg.emit(location(),
                                                   Operands::BinaryMathNibble::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendSpecialBinaryArithMsgX.emit(location(),
                                                    Operands::BinaryMath::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;

        case selIf:
            compileAnyIfMsg(this);
            break;

        case selCase:
            compileCaseMsg(this);
            break;

        case selSwitch:
            compileSwitchMsg(this);
            break;

        case selWhile:
            compileWhileMsg(this);
            break;

        case selLoop:
            compileLoopMsg(this);
            break;

        case selAnd:
            if (numArgs == 2)
                compileAndMsg(argnode, argnode->mNext);
            else
                goto special;
            break;

        case selOr:
            if (numArgs == 2)
                compileOrMsg(argnode, argnode->mNext);
            else
                goto special;
            break;

        case selQuestionMark:
            if (numArgs == 2)
                compileQMsg(argnode, argnode->mNext);
            break;

        case selDoubleQuestionMark:
            if (numArgs == 2)
                compileQQMsg(argnode, argnode->mNext);
            break;

        case selExclamationQuestionMark:
            if (numArgs == 2)
                compileXQMsg(argnode, argnode->mNext);
            break;

        default:
        defaultCase:
            if (numArgs == 1 && varname == s_this) {
                emitTailCall(location());
                SendMsgThisOpt.emit(location(), Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            } else {
                for (; argnode; argnode = argnode->mNext)
                    COMPILENODE(argnode, &dummy, false);

                emitTailCall(location());
                if (SendMsg.validNibble(numArgs))
                    SendMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
                else
                    SendMsgX.emit(location(), Operands::ArgumentCount::fromRaw(numArgs),
                                  Operands::KwArgumentCount::fromRaw(0),
                                  Operands::SelectorIndex::fromRaw(selectorSlotOrSpecialIndex));
            }
            break;
        }
    }
}

CompilingBytecodes compileSubExpression(PyrSlotNode* litnode, bool onTailBranch) {
    return compileSubExpressionWithGoto(litnode, 0, onTailBranch);
}

CompilingBytecodes compileSubExpressionWithGoto(PyrSlotNode* litnode, int branchLen, bool onTailBranch) {
    PyrBlockNode* bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
    return compileBodyWithGoto(bnode->mBody, branchLen, onTailBranch);
}

CompilingBytecodes compileBodyWithGoto(PyrParseNode* body, int branchLen, bool onTailBranch) {
    PyrSlot dummy;

    PyrFunctionDef* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
    gPartiallyAppliedFunction = nullptr;

    auto oldByteCodes = std::move(gCompilingBytecodes);


    COMPILENODE(body, &dummy, onTailBranch);
    if (branchLen) {
        if (gCompilingBytecodes.length() == 0) {
            PushSpecialValue.emit(body->location(), { OpSpecialValue::Nil_ });
        }
        JumpFwd.emit(body->location(), Operands::UnsignedInt<16, 1>::fromFull(branchLen),
                     Operands::UnsignedInt<16, 0>::fromFull(branchLen));
    }

    auto thisBytecodes = std::move(gCompilingBytecodes);
    gCompilingBytecodes = std::move(oldByteCodes);

    gPartiallyAppliedFunction = prevPartiallyAppliedFunction;

    return thisBytecodes;
}


bool isAnInlineableBlock(PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == pn_PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot) && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                gNumUninlinedFunctions += 1;
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

bool isAnInlineableAtomicLiteralBlock(PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == pn_PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot) && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                gNumUninlinedFunctions += 1;
                if (SC_LanguageConfig::getPostInlineWarnings()) {
                    post("WARNING: FunctionDef contains variable declarations and so"
                         " will not be inlined.\n");
                    if (bnode->mArglist)
                        nodePostErrorLine((PyrParseNode*)bnode->mArglist);
                    else
                        nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
                }
            } else {
                if (bnode->mBody->mClassno == pn_DropNode
                    && ((PyrDropNode*)bnode->mBody)->mExpr2->mClassno == pn_BlockReturnNode)
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
    if (node->mClassno == pn_PushLitNode) {
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
std::optional<PyrSlot> getAtomicValueFromLiteralOrBlockMaybePostWarning(const PyrParseNode& node) {
    if (node.mClassno != pn_PushLitNode)
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

    if (maybeBlock.mClassno != pn_BlockNode)
        return std::nullopt;

    const auto& block = static_cast<const PyrBlockNode&>(maybeBlock);

    // Having arguments and variables mean we can't inline it, therefore, it isn't a literal.
    // Printing warnings first if requested to.
    if constexpr (Warning == UninlinableWarningOption::PostWarning) {
        const auto postWarning = SC_LanguageConfig::getPostInlineWarnings();
        if (block.mArglist && postWarning) {
            post("WARNING: FunctionDef contains argument declarations and so will not be inlined.\n");
            nodePostErrorLine((PyrParseNode*)block.mArglist);
        }
        if (block.mVarlist && postWarning) {
            post("WARNING: FunctionDef contains variable declarations and so will not be inlined.\n");
            nodePostErrorLine((PyrParseNode*)block.mVarlist);
        }
        if (block.mArglist || block.mVarlist) {
            gNumUninlinedFunctions += 1;
            return std::nullopt;
        }
    } else {
        if (block.mArglist || block.mVarlist)
            return std::nullopt;
    }

    if (block.mBody->mClassno != pn_DropNode)
        return std::nullopt;

    const auto& dropNode = *static_cast<PyrDropNode*>(block.mBody);

    // Not a single return statement, e.g., { 1 },
    if (dropNode.mExpr2->mClassno != pn_BlockReturnNode)
        return std::nullopt;

    if (dropNode.mExpr1->mClassno != pn_PushLitNode)
        return std::nullopt;

    const auto& blockedLit = static_cast<PyrSlotNode&>(*dropNode.mExpr1);
    const auto& blockedSlot = blockedLit.mSlot;
    if (blockedSlot.isObjectHdr())
        return std::nullopt;
    // We don't allow functions to be literals, e.g., here the value returned would be a function`{ {1} }` but that is
    // not a literal. Otherwise we could do recursion with tail call for this function.
    if (blockedSlot.isPtr())
        return std::nullopt;

    return blockedSlot;
}

bool isWhileTrue(PyrParseNode* node) {
    bool res = false;
    if (node->mClassno == pn_PushLitNode) {
        PyrSlotNode* anode;
        PyrBlockNode* bnode;
        anode = (PyrSlotNode*)node;
        if (IsPtr(&anode->mSlot) && (bnode = (PyrBlockNode*)(slotRawPtr(&anode->mSlot)))->mClassno == pn_BlockNode) {
            if (bnode->mArglist || bnode->mVarlist) {
                /*
                post("WARNING: FunctionDef contains variable declarations and so"
                    " will not be inlined.\n");
                if (bnode->mArglist) nodePostErrorLine((PyrParseNode*)bnode->mArglist);
                else nodePostErrorLine((PyrParseNode*)bnode->mVarlist);
                */
            } else {
                if (bnode->mBody->mClassno == pn_PushLitNode && IsTrue(&((PyrSlotNode*)bnode->mBody)->mSlot)) {
                    res = true;
                }
            }
        } else if (IsTrue(&anode->mSlot)) {
            res = true;
        }
    }
    return res;
}

void compileAndMsg(PyrParseNode* arg1, PyrParseNode* arg2) {
    PyrSlot dummy;
    COMPILENODE(arg1, &dummy, false);
    if (isAnInlineableBlock(arg2)) {
        auto trueByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
        const auto jumpLen = trueByteCodes.length();
        JumpIfFalsePushFalse.emit(arg1->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                  Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
        gCompilingBytecodes.consume(std::move(trueByteCodes));
    } else {
        COMPILENODE(arg2, &dummy, false);
        emitTailCall(arg1->location());
        SendSpecialMsg.emit(arg1->location(), 2, { OpSpecialSelectors::And });
    }
}

void compileOrMsg(PyrParseNode* arg1, PyrParseNode* arg2) {
    PyrSlot dummy;
    COMPILENODE(arg1, &dummy, false);
    if (isAnInlineableBlock(arg2)) {
        auto falseByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
        const auto jumpLen = falseByteCodes.length();
        JumpIfTruePushTrue.emit(arg1->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
        gCompilingBytecodes.consume(std::move(falseByteCodes));
    } else {
        COMPILENODE(arg2, &dummy, false);
        emitTailCall(arg1->location());
        SendSpecialMsg.emit(arg1->location(), 2, { OpSpecialSelectors::Or });
    }
}

void compileQMsg(PyrParseNode* arg1, PyrParseNode* arg2) {
    // question mark.
    PyrSlot dummy;

    COMPILENODE(arg1, &dummy, false);
    COMPILENODE(arg2, &dummy, false);
    Extended::QuestionMark.emit(arg1->location());
}

void compileQQMsg(PyrParseNode* arg1, PyrParseNode* arg2) {
    // double question mark. ?? {|obj| ^if (this.notNil, this, func) }
    PyrSlot dummy;

    COMPILENODE(arg1, &dummy, false);
    if (isAnInlineableBlock(arg2)) {
        auto nilByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
        const auto jumplen = nilByteCodes.length();
        Extended::DoubleQuestionMark.emit(
            arg1->location(),
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
        gCompilingBytecodes.consume(std::move(nilByteCodes));
    } else {
        COMPILENODE(arg2, &dummy, false);
        emitTailCall(arg1->location());
        SendSpecialMsg.emit(arg1->location(), 2, { OpSpecialSelectors::DoubleQuestionMark });
    }
}

void compileXQMsg(PyrParseNode* arg1, PyrParseNode* arg2) {
    // double question mark. !? {|obj| ^if (this.isNil, this, func) }
    PyrSlot dummy;

    COMPILENODE(arg1, &dummy, false);
    if (isAnInlineableBlock(arg2)) {
        auto nilByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
        const auto jumplen = nilByteCodes.length();
        Extended::IfNilThenJumpElsePopNil.emit(
            arg1->location(),
            { Operands::UnsignedInt<16, 1>::fromFull(jumplen), Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
        gCompilingBytecodes.consume(std::move(nilByteCodes));
    } else {
        COMPILENODE(arg2, &dummy, false);
        emitTailCall(arg1->location());
        SendSpecialMsg.emit(arg1->location(), 2, { OpSpecialSelectors::ExclamationQuestionMark });
    }
}

void compileAnyIfMsg(PyrCallNodeBase2* node) {
    PyrParseNode* arg1 = node->mArglist;

    if (arg1->mClassno == pn_CallNode) {
        PyrCallNode* callNode = (PyrCallNode*)arg1;
        const int numCallArgs = nodeListLength(callNode->mArglist);
        const int numCallKeyArgs = nodeListLength(callNode->mKeyarglist);
        if (numCallArgs == 1 && numCallKeyArgs == 0) { // Is a binary op with no keywords
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

void compileIfMsg(PyrCallNodeBase2* node) {
    const int numArgs = nodeListLength(node->mArglist);
    PyrParseNode* arg1 = node->mArglist;

    PyrSlot dummy;
    if (numArgs == 2) {
        PyrParseNode* arg2 = arg1->mNext;

        if (isAnInlineableBlock(arg2)) {
            COMPILENODE(arg1, &dummy, false);

            auto trueByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
            const auto jumpLen = trueByteCodes.length();
            if (jumpLen != 0) {
                JumpIfFalsePushNil.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                        Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
                gCompilingBytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(node->location());
                PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
            }
        } else {
            for (; arg1; arg1 = arg1->mNext)
                COMPILENODE(arg1, &dummy, false);
            emitTailCall(node->location());
            SendSpecialMsg.emit(node->location(), 2, { OpSpecialSelectors::If });
        }
    } else if (numArgs == 3) {
        PyrParseNode* arg2 = arg1->mNext;
        PyrParseNode* arg3 = arg2->mNext;
        if (isAnInlineableBlock(arg2) && isAnInlineableBlock(arg3)) {
            COMPILENODE(arg1, &dummy, false);
            auto falseByteCodes = compileSubExpression((PyrSlotNode*)arg3, true);
            auto trueByteCodes = compileSubExpressionWithGoto((PyrSlotNode*)arg2, falseByteCodes.length(), true);

            if (falseByteCodes.length() != 0) {
                const auto jumpLen = trueByteCodes.length();
                JumpIfFalse.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                 Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
                gCompilingBytecodes.consume(std::move(trueByteCodes));
                gCompilingBytecodes.consume(std::move(falseByteCodes));
            } else if (trueByteCodes.length() != 0) {
                const auto jumpLen = trueByteCodes.length();
                JumpIfFalsePushNil.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpLen),
                                        Operands::UnsignedInt<16, 0>::fromFull(jumpLen));
                gCompilingBytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(node->location());
                PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
            }
        } else {
            for (; arg1; arg1 = arg1->mNext)
                COMPILENODE(arg1, &dummy, false);
            emitTailCall(node->location());
            SendSpecialMsg.emit(node->location(), 3, { OpSpecialSelectors::If });
        }
    } else {
        for (; arg1; arg1 = arg1->mNext)
            COMPILENODE(arg1, &dummy, false);
        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::If });
        else
            SendSpecialMsgX.emit(node->location(), Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::If)));
    }
}


// TODO: what is flag? Give it a better name.
void compileIfNilMsg(PyrCallNodeBase2* node, bool flag) {
    PyrSlot dummy;

    const int numArgs = nodeListLength(node->mArglist);
    PyrParseNode* arg1 = node->mArglist;

    if (numArgs < 2) {
        COMPILENODE(arg1, &dummy, false);
        emitTailCall(node->location());
        SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::If });
    } else if (numArgs == 2) {
        PyrParseNode* arg2 = arg1->mNext;
        if (isAnInlineableBlock(arg2)) {
            PyrCallNode* callNode = (PyrCallNode*)arg1;
            COMPILENODE(callNode->mArglist, &dummy, false);

            auto trueByteCodes = compileSubExpression((PyrSlotNode*)arg2, true);
            const auto jumplen = trueByteCodes.length();
            if (jumplen) {
                if (flag)
                    Extended::IfNotNilJumpPushNilElsePop.emit(node->location(),
                                                              { Operands::UnsignedInt<16, 1>::fromFull(jumplen),
                                                                Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
                else
                    Extended::IfNilThenJumpElsePopNil.emit(node->location(),
                                                           { Operands::UnsignedInt<16, 1>::fromFull(jumplen),
                                                             Operands::UnsignedInt<16, 0>::fromFull(jumplen) });
                gCompilingBytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(node->location()); // Drop the boolean
                PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
            }
        } else {
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            emitTailCall(node->location());
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::If });
        }
    } else if (numArgs == 3) {
        PyrParseNode* arg2 = arg1->mNext;
        PyrParseNode* arg3 = arg2->mNext;
        if (isAnInlineableBlock(arg2) && isAnInlineableBlock(arg3)) {
            PyrCallNode* callNode = (PyrCallNode*)arg1;
            COMPILENODE(callNode->mArglist, &dummy, false);

            auto falseByteCodes = compileSubExpression((PyrSlotNode*)arg3, true);
            const auto falseLen = falseByteCodes.length();
            auto trueByteCodes = compileSubExpressionWithGoto((PyrSlotNode*)arg2, falseLen, true);
            const auto trueLen = trueByteCodes.length();
            if (falseLen) {
                if (flag)
                    Extended::IfNotNilJump.emit(node->location(),
                                                { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                  Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                else
                    Extended::IfNilJump.emit(node->location(),
                                             { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                               Operands::UnsignedInt<16, 0>::fromFull(trueLen) });

                gCompilingBytecodes.consume(std::move(trueByteCodes));
                gCompilingBytecodes.consume(std::move(falseByteCodes));
            } else if (trueLen) {
                if (flag)
                    Extended::IfNotNilJumpPushNilElsePop.emit(node->location(),
                                                              { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                                Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                else
                    Extended::IfNilThenJumpElsePopNil.emit(node->location(),
                                                           { Operands::UnsignedInt<16, 1>::fromFull(trueLen),
                                                             Operands::UnsignedInt<16, 0>::fromFull(trueLen) });
                gCompilingBytecodes.consume(std::move(trueByteCodes));
            } else {
                Drop.emit(node->location()); // Drop the boolean
                PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
            }
        } else {
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            COMPILENODE(arg3, &dummy, false);
            emitTailCall(node->location());
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::If });
        }
    } else {
        for (; arg1; arg1 = arg1->mNext) {
            COMPILENODE(arg1, &dummy, false);
        }
        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::If });
        else
            SendSpecialMsgX.emit(
                node->location(), Operands::ArgumentCount::fromRaw(numArgs),
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


PyrCallNode* buildCase(PyrParseNode* arg1) {
    // transform case statement into nested if statements.
    // int numArgs = nodeListLength(arg1);

    // post("->buildCase %d\n", numArgs);

    PyrParseNode* arg2 = arg1->mNext;

    PyrSlotNode* litnode = (PyrSlotNode*)arg1;
    PyrBlockNode* bnode = (PyrBlockNode*)slotRawPtr(&litnode->mSlot);
    PyrParseNode* bbody = bnode->mBody;
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

    PyrParseNode* arg3 = nullptr;
    if (arg2) {
        arg3 = arg2->mNext;
        if (arg3) {
            PyrParseNode* arg4 = arg3->mNext;
            if (arg4) {
                arg3 = buildCase(arg3);
                PyrBlockNode* bnode = allocNode<PyrBlockNode>(arg3->mLocation, nullptr, nullptr, arg3, false);
                arg3 = allocNode<PyrSlotNode>(bnode->mLocation, PyrSlot::make<void*>(bnode), pn_PushLitNode);
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

    PyrSlot selector;
    SetSymbol(&selector, gSpecialSelectors[opmIf]);
    auto* selectorNode = allocNode<PyrSlotNode>(arg1->mLocation, PyrSlot::make(gSpecialSelectors[opmIf]));
    return allocNode<PyrCallNode>(arg1->mLocation, selectorNode, arg1, nullptr);
}

void compileCaseMsg(PyrCallNodeBase2* node) {
    PyrParseNode* argnode = node->mArglist;
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
        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::Case });
        else
            SendSpecialMsgX.emit(node->location(), Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Case)));
    }
}

void compileSwitchMsg(PyrCallNode* node) {
    PyrSlot dummy;
    bool canInline = true;
    int numArgs;
    {
        PyrParseNode* argnode = node->mArglist;
        numArgs = nodeListLength(argnode);

        if (numArgs <= 2) {
            error("Missing argument in switch statement");
            nodePostErrorLine(node);
            compileErrors++;
        };

        argnode = argnode->mNext; // skip first arg.

        PyrParseNode* nextargnode = nullptr;
        for (; argnode; argnode = nextargnode) {
            // This loop is confusing, argnode can refer to either the case or the default depending on whether the
            // nextargnode is nullptr or not.
            nextargnode = argnode->mNext;
            if (nextargnode == nullptr) {
                // argnode is the default, this is how this loop terminates.
                if (!isAnInlineableBlock(argnode))
                    canInline = false;
                break; // nothing left, leave.
            }

            const auto& case_node = argnode;
            const auto& function_node = nextargnode;

            const auto case_literal = getAtomicValueFromLiteralOrBlockMaybePostWarning(*case_node);
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
            if (!isAnInlineableBlock(function_node)) {
                canInline = false;
                break;
            }

            nextargnode = function_node->mNext;
        }
    }

    if (canInline) {
        PyrParseNode* argnode = node->mArglist;

        const auto flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
        const auto arraySize = NEXTPOWEROFTWO(numArgs * 2);
        PyrObject* array = newPyrArray(compileGC(), arraySize, flags, false);
        array->size = arraySize;
        nilSlots(array->slots, arraySize);

        PyrSlot slot;
        SetObject(&slot, array);

        COMPILENODE(argnode, &dummy, false);
        compilePushConstant(node, &slot);

        Extended::Switch.emit(node->location());

        argnode = argnode->mNext; // skip first arg.

        PyrParseNode* nextargnode = nullptr;
        const auto absoluteOffset = gCompilingBytecodes.length();
        int offset = 0;
        int lastOffset = 0;
        for (; argnode; argnode = nextargnode) {
            nextargnode = argnode->mNext;
            if (nextargnode != nullptr) {
                auto byteCodes = compileSubExpressionWithGoto((PyrSlotNode*)nextargnode, 0x6666, true);

                PyrSlot* key;
                PyrSlot value;
                SetInt(&value, offset);
                PyrSlotNode* keyargnode = (PyrSlotNode*)argnode;
                if (isAtomicLiteral(argnode)) {
                    key = &keyargnode->mSlot;
                } else {
                    PyrBlockNode* bnode = (PyrBlockNode*)slotRawPtr(&keyargnode->mSlot);
                    PyrDropNode* dropnode = (PyrDropNode*)bnode->mBody;
                    PyrSlotNode* litnode = (PyrSlotNode*)dropnode->mExpr1;
                    key = &litnode->mSlot;
                }

                int index = arrayAtIdentityHashInPairs(array, key);
                PyrSlot* slot = array->slots + index;
                slotCopy(slot, key);
                SetInt(slot + 1, offset);

                if (byteCodes.length() != 0) {
                    offset += byteCodes.length();
                    gCompilingBytecodes.consume(std::move(byteCodes));
                } else {
                    PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
                    offset += 1;
                }

                nextargnode = nextargnode->mNext;
                if (nextargnode == nullptr) {
                    PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
                    lastOffset = offset;
                    offset += 1;
                }
            } else {
                auto byteCodes = compileSubExpressionWithGoto((PyrSlotNode*)argnode, 0, true);

                lastOffset = offset;
                if (byteCodes.length() != 0) {
                    offset += byteCodes.length();
                    gCompilingBytecodes.consume(std::move(byteCodes));
                } else {
                    PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
                    lastOffset = offset;
                    offset += 1;
                }
            }
        }

        {
            const auto jumplen = offset - lastOffset;
            gCompilingBytecodes.backsetByte(absoluteOffset + lastOffset - 2, (jumplen >> 8) & 255);
            gCompilingBytecodes.backsetByte(absoluteOffset + lastOffset - 1, jumplen & 255);
        }

        PyrSlot* slots = array->slots;
        for (size_t i = 0; i < arraySize; i += 2) {
            PyrSlot* key = slots + i;
            PyrSlot* value = key + 1;

            if (IsNil(value)) {
                SetInt(value, lastOffset);
            } else {
                if (const auto offsetToHere = value->getInt()) {
                    const auto jumplen = offset - offsetToHere;
                    gCompilingBytecodes.backsetByte(absoluteOffset + offsetToHere - 2, (jumplen >> 8) & 255);
                    gCompilingBytecodes.backsetByte(absoluteOffset + offsetToHere - 1, jumplen & 255);
                }
            }
        }

    } else {
        PyrParseNode* argnode = node->mArglist;
        for (; argnode; argnode = argnode->mNext) {
            COMPILENODE(argnode, &dummy, false);
        }
        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::Switch });
        else
            SendSpecialMsgX.emit(node->location(), Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Switch)));
    }
}

void compileWhileMsg(PyrCallNodeBase2* node) {
    const auto numArgs = nodeListLength(node->mArglist);
    if (numArgs == 1 && isAnInlineableBlock(node->mArglist)) {
        auto whileByteCodes = compileSubExpression((PyrSlotNode*)node->mArglist, false);
        const auto whileByteCodeLen = whileByteCodes.length();
        gCompilingBytecodes.consume(std::move(whileByteCodes));

        const auto exprByteCodeLen = 1;
        // UNKNOWN: where does this '3' come from?
        const auto jumpIfFalseLength = exprByteCodeLen + 3;
        JumpIfFalsePushNil.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalseLength),
                                Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalseLength));

        // opcJumpBak does a drop...
        PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });

        // UNKNOWN: where does this '4' come from?
        const auto jumpBackLength = exprByteCodeLen + whileByteCodeLen + 4;
        JumpBack.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpBackLength),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackLength));


    } else if (numArgs == 2 && isWhileTrue(node->mArglist) && isAnInlineableBlock(node->mArglist->mNext)) {
        auto exprByteCodes = compileSubExpression((PyrSlotNode*)node->mArglist->mNext, false);
        const auto exprByteCodeLen = exprByteCodes.length();
        gCompilingBytecodes.consume(std::move(exprByteCodes));
        // UNKNOWN: where does this '1' come from?
        const auto jumpBackLength = exprByteCodeLen + 1;
        JumpBack.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpBackLength),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackLength));


    } else if (numArgs == 2 && isAnInlineableBlock(node->mArglist) && isAnInlineableBlock(node->mArglist->mNext)) {
        auto whileByteCodes = compileSubExpression((PyrSlotNode*)node->mArglist, false);
        auto exprByteCodes = compileSubExpression((PyrSlotNode*)node->mArglist->mNext, false);

        const auto whileByteCodeLen = whileByteCodes.length();
        gCompilingBytecodes.consume(std::move(whileByteCodes));

        if (exprByteCodes.length() != 0) {
            const auto exprByteCodeLen = exprByteCodes.length();
            // UNKNOWN: where does this '3' come from?
            const auto jumpIfFalsePushNilLength = exprByteCodeLen + 3;
            JumpIfFalsePushNil.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalsePushNilLength),
                                    Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalsePushNilLength));

            gCompilingBytecodes.consume(std::move(exprByteCodes));

            const auto jumpBackSize = exprByteCodeLen + whileByteCodeLen + 4;
            JumpBack.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                          Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));

        } else {
            const auto exprByteCodeLen = 1;
            // UNKNOWN: where does this '3' come from?
            const auto jumpIfFalsePushNilLength = exprByteCodeLen + 3;
            JumpIfFalsePushNil.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpIfFalsePushNilLength),
                                    Operands::UnsignedInt<16, 0>::fromFull(jumpIfFalsePushNilLength));
            // JumpBak does a drop..
            PushSpecialValue.emit(node->location(), { OpSpecialValue::Nil_ });
            const auto jumpBackSize = exprByteCodeLen + whileByteCodeLen + 4;
            JumpBack.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                          Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));
        }


    } else {
        PyrParseNode* argnode = node->mArglist;
        PyrSlot dummy;
        for (; argnode; argnode = argnode->mNext)
            COMPILENODE(argnode, &dummy, false);

        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::While });
        else
            SendSpecialMsgX.emit(node->location(), Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::While)));
    }
}

void compileLoopMsg(PyrCallNodeBase2* node) {
    const int numArgs = nodeListLength(node->mArglist);
    if (numArgs == 1 && isAnInlineableBlock(node->mArglist)) {
        auto exprByteCodes = compileSubExpression((PyrSlotNode*)node->mArglist, false);

        const auto exprByteCodeLen = exprByteCodes.length();
        gCompilingBytecodes.consume(std::move(exprByteCodes));

        // UNKNOWN: where does this '1' come from?
        const auto jumpBackSize = exprByteCodeLen + 1;
        JumpBack.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(jumpBackSize),
                      Operands::UnsignedInt<16, 0>::fromFull(jumpBackSize));

    } else {
        PyrParseNode* argnode = node->mArglist;
        PyrSlot dummy;
        for (; argnode; argnode = argnode->mNext)
            COMPILENODE(argnode, &dummy, false);

        emitTailCall(node->location());
        if (numArgs < 16)
            SendSpecialMsg.emit(node->location(), numArgs, { OpSpecialSelectors::Loop });
        else
            SendSpecialMsgX.emit(node->location(), Operands::ArgumentCount::fromRaw(numArgs),
                                 Operands::KwArgumentCount::fromRaw(0),
                                 Operands::Index::fromRaw(static_cast<int>(OpSpecialSelectors::Loop)));
    }
}


int PyrBinopCallNode::isPartialApplication() {
    int sum = 0;
    PyrParseNode* argnode = mArglist;
    for (; argnode; argnode = argnode->mNext) {
        if (argnode->mClassno == pn_CurryArgNode) {
            ((PyrCurryArgNode*)argnode)->mArgNum = sum;
            sum++;
        }
    }
    return sum;
}

void PyrBinopCallNode::compileCall(PyrSlot* result) {
    PyrSlot dummy;

    PyrParseNode* arg1 = mArglist;
    PyrParseNode* arg2 = arg1->mNext;
    PyrParseNode* arg3 = arg2->mNext;

    const int isSuper = isSuperObjNode(arg1);
    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
    int selType;
    const int index = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock, isSuper,
                                           slotRawSymbol(&mSelector->mSlot), &selType);

    const int numArgs = arg3 ? 3 : 2;
    if (isSuper) {
        COMPILENODE(arg1, &dummy, false);
        COMPILENODE(arg2, &dummy, false);
        if (arg3)
            COMPILENODE(arg3, &dummy, false);

        emitTailCall(location());
        SendSuperMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(index));

    } else {
        switch (selType) {
        case selNormal:
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            if (arg3)
                COMPILENODE(arg3, &dummy, false);

            emitTailCall(location());
            SendMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(index));
            break;

        case selSpecial:
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            if (arg3)
                COMPILENODE(arg3, &dummy, false);

            emitTailCall(location());
            SendSpecialMsg.emit(location(), numArgs, Operands::SpecialSelectors::fromRaw(index));
            break;

        case selUnary:
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            if (arg3)
                COMPILENODE(arg3, &dummy, false);

            emitTailCall(location());

            // Drop extra arguments
            if (arg3)
                Drop.emit(location());
            Drop.emit(location());

            // TODO: work on better conversions
            SendSpecialUnaryArithMsg.emit(location(), { static_cast<OpUnaryMathNibble>((Byte)index) });
            break;

        case selBinary:
            if (arg3) {
                COMPILENODE(arg1, &dummy, false);
                COMPILENODE(arg2, &dummy, false);
                COMPILENODE(arg3, &dummy, false);

                emitTailCall(location());
                SpecialBinaryOpWithAdverb.emit(location(), Operands::TrinaryMath::fromRaw(index));

            } else if (index == opAdd && arg2->mClassno == pn_PushLitNode && IsInt(&((PyrSlotNode*)arg2)->mSlot)
                       && slotRawInt(&((PyrSlotNode*)arg2)->mSlot) == 1) {
                COMPILENODE(arg1, &dummy, false);

                PushOneAndAddOne.emit(location());

            } else if (index == opSub && arg2->mClassno == pn_PushLitNode && IsInt(&((PyrSlotNode*)arg2)->mSlot)
                       && slotRawInt(&((PyrSlotNode*)arg2)->mSlot) == 1) {
                COMPILENODE(arg1, &dummy, false);

                emitTailCall(location());
                PushOneAndSubtract.emit(location());

            } else {
                COMPILENODE(arg1, &dummy, false);
                COMPILENODE(arg2, &dummy, false);

                emitTailCall(location());
                if (index < 16)
                    SendSpecialBinaryArithMsg.emit(location(), Operands::BinaryMathNibble::fromRaw(index));
                else
                    SendSpecialBinaryArithMsgX.emit(location(), Operands::BinaryMath::fromRaw(index));
            }
            break;
        case selIf:
            compileAnyIfMsg(this);
            break;
        case selCase:
            compileCaseMsg(this);
            break;
        case selWhile:
            compileWhileMsg(this);
            break;
        case selLoop:
            compileLoopMsg(this);
            break;
        case selAnd:
            compileAndMsg(arg1, arg2);
            break;
        case selOr:
            compileOrMsg(arg1, arg2);
            break;
        case selQuestionMark:
            compileQMsg(arg1, arg2);
            break;
        case selDoubleQuestionMark:
            compileQQMsg(arg1, arg2);
            break;
        case selExclamationQuestionMark:
            compileXQMsg(arg1, arg2);
            break;
        default:
            COMPILENODE(arg1, &dummy, false);
            COMPILENODE(arg2, &dummy, false);
            if (arg3)
                COMPILENODE(arg3, &dummy, false);

            emitTailCall(location());
            SendMsg.emit(location(), numArgs, Operands::SelectorIndex::fromRaw(index));
            break;
        }
    }
}


void PyrPushKeyArgNode::compile(PyrSlot* result) {
    PyrSlot dummy;
    // postfl("->compilePyrPushKeyArgNode\n");

    compilePushConstant((PyrParseNode*)this, &mSelector->mSlot);

    COMPILENODE(mExpr, &dummy, false);
}


void PyrDropNode::compile(PyrSlot* result) {
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
        // let the store do the drop, a bit more complex.
        // find the ultimate expression in the left subtree before the drop.
        PyrDropNode* znode = (PyrDropNode*)mExpr1;
        while (znode->mExpr2 && znode->mExpr2->mClassno == pn_DropNode) {
            znode = (PyrDropNode*)znode->mExpr2;
        }
        if (znode->mExpr2->mClassno == pn_AssignNode) {
            ((PyrAssignNode*)znode->mExpr2)->mDrop = 1;
            COMPILENODE(mExpr1, &dummy, false);
            COMPILENODE(mExpr2, &dummy, true);
        } else {
            COMPILENODE(mExpr1, &dummy, false);
            Drop.emit(location());
            COMPILENODE(mExpr2, &dummy, true);
        }
    } else {
        COMPILENODE(mExpr1, &dummy, false);
        Drop.emit(location());
        COMPILENODE(mExpr2, &dummy, true);
    }
}


void compilePushConstant(PyrParseNode* node, PyrSlot* slot) {
    const int index = conjureConstantIndex(node, gCompilingBlock, slot);

    if (index < (1 << 4))
        PushLiteral.emit(node->location(), index);

    else if (index < (1 << 8))
        PushConstant8.emit(node->location(), Operands::UnsignedInt<8, 0>::fromRaw(index));

    else if (index < (1 << 16))
        PushConstant16.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(index),
                            Operands::UnsignedInt<16, 0>::fromFull(index));

    else if (index < (1 << 24))
        PushConstant24.emit(node->location(), Operands::UnsignedInt<24, 2>::fromFull(index),
                            Operands::UnsignedInt<24, 1>::fromFull(index),
                            Operands::UnsignedInt<24, 0>::fromFull(index));

    else
        PushConstant32.emit(node->location(), Operands::UnsignedInt<32, 3>::fromFull(index),
                            Operands::UnsignedInt<32, 2>::fromFull(index),
                            Operands::UnsignedInt<32, 1>::fromFull(index),
                            Operands::UnsignedInt<32, 0>::fromFull(index));
}

void emitPushInt(LocationInSourceCode loc, int value) {
    if (value == -1)
        PushSpecialNumber.emit(loc, { OpSpecialNumbers::MinusOne });

    else if (value == 0)
        PushSpecialNumber.emit(loc, { OpSpecialNumbers::Zero });

    else if (value == 1)
        PushSpecialNumber.emit(loc, { OpSpecialNumbers::One });

    else if (value == 2)
        PushSpecialNumber.emit(loc, { OpSpecialNumbers::Two });

    else if (value >= -(1 << 7) && value <= ((1 << 7) - 1))
        PushInteger8.emit(loc, Operands::Int<8, 0>::fromFull(value));

    else if (value >= -(1 << 15) && value <= ((1 << 15) - 1))
        PushInteger16.emit(loc, Operands::Int<16, 1>::fromFull(value), Operands::Int<16, 0>::fromFull(value));

    else if (value >= -(1 << 23) && value <= ((1 << 23) - 1))
        PushInteger24.emit(loc, Operands::Int<24, 2>::fromFull(value), Operands::Int<24, 1>::fromFull(value),
                           Operands::Int<24, 0>::fromFull(value));

    else
        PushInteger32.emit(loc, Operands::Int<32, 3>::fromFull(value), Operands::Int<32, 2>::fromFull(value),
                           Operands::Int<32, 1>::fromFull(value), Operands::Int<32, 0>::fromFull(value));
}

void PyrSlotNode::compilePushLit(PyrSlot* result) {
    if (IsPtr(&mSlot)) {
        PyrParseNode* literalObj = (PyrParseNode*)slotRawPtr(&mSlot);
        const auto class_no = literalObj->mClassno;

        if (class_no == pn_BlockNode || class_no == pn_NewBlock) {
            CompilingBytecodes storedCodes { std::move(gCompilingBytecodes) };
            PyrSlot slot;
            COMPILENODE(literalObj, &slot, false);
            gCompilingBytecodes = std::move(storedCodes);

            const Byte index = conjureLiteralSlotIndex(literalObj, gCompilingBlock, &slot);
            PushLiteralX.emit(location(), Operands::Index::fromRaw(index));

            PyrFunctionDef* block = slotRawBlock(&slot);
            if (NotNil(&block->contextDef))
                METHRAW(gCompilingBlock)->needsHeapContext = 1;
        } else {
            PyrSlot slot;
            COMPILENODE(literalObj, &slot, false);
            compilePushConstant((PyrParseNode*)literalObj, &slot);
        }
    } else {
        PyrSlot slot = mSlot;
        if (IsInt(&slot)) {
            emitPushInt(location(), slotRawInt(&slot));
        } else if (SlotEq(&slot, &o_nil)) {
            PushSpecialValue.emit(location(), { OpSpecialValue::Nil_ });
        } else if (SlotEq(&slot, &o_true)) {
            PushSpecialValue.emit(location(), { OpSpecialValue::True });
        } else if (SlotEq(&slot, &o_false)) {
            PushSpecialValue.emit(location(), { OpSpecialValue::False });
        } else if (SlotEq(&slot, &o_fhalf)) {
            PushSpecialNumber.emit(location(), { OpSpecialNumbers::Half });
        } else if (SlotEq(&slot, &o_fnegone)) {
            PushSpecialNumber.emit(location(), { OpSpecialNumbers::MinusOneFloat });
        } else if (SlotEq(&slot, &o_fzero)) {
            PushSpecialNumber.emit(location(), { OpSpecialNumbers::ZeroFloat });
        } else if (SlotEq(&slot, &o_fone)) {
            PushSpecialNumber.emit(location(), { OpSpecialNumbers::OneFloat });
        } else if (SlotEq(&slot, &o_ftwo)) {
            PushSpecialNumber.emit(location(), { OpSpecialNumbers::TwoFloat });
        } else if (SlotEq(&slot, &o_inf)) {
            PushSpecialValue.emit(location(), { OpSpecialValue::Inf });
        } else if (IsFloat(&slot)) {
            compilePushConstant((PyrParseNode*)this, &slot);
        } else if (IsSym(&slot)) {
            compilePushConstant((PyrParseNode*)this, &slot);
        } else {
            compilePushConstant((PyrParseNode*)this, &slot);
        }
    }
}

void compilePyrLiteralNode(PyrSlotNode* node, PyrSlot* result) {
    if (!node) {
        SetNil(result);
    } else {
        node->compileLiteral(result);
    }
}

void PyrSlotNode::compileLiteral(PyrSlot* result) {
    if (IsPtr(&mSlot)) {
        PyrParseNode* literalObj = (PyrParseNode*)slotRawPtr(&mSlot);
        if (literalObj->mClassno == pn_BlockNode || literalObj->mClassno == pn_NewBlock) {
            CompilingBytecodes storedCodes { std::move(gCompilingBytecodes) };
            COMPILENODE(literalObj, result, false);
            gCompilingBytecodes = std::move(storedCodes);

            PyrFunctionDef* block = slotRawBlock(result);
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


void PyrReturnNode::compile(PyrSlot* result) {
    gFunctionCantBeClosed = true;
    if (!mExpr) {
        ReturnSelf.emit(location());
    } else if (mExpr->mClassno == pn_PushLitNode) {
        PyrSlotNode* lit = (PyrSlotNode*)mExpr;
        if (IsSym(&(lit->mSlot)) && slotRawSymbol(&lit->mSlot) == s_this) {
            ReturnSelf.emit(location());
        } else if (IsNil(&lit->mSlot)) {
            ReturnNil.emit(location());
        } else if (IsTrue(&lit->mSlot)) {
            ReturnTrue.emit(location());
        } else if (IsFalse(&lit->mSlot)) {
            ReturnFalse.emit(location());
        } else {
            PyrSlot dummy;
            COMPILENODE(lit, &dummy, false);
            Return.emit(location());
        }
    } else {
        SetTailBranch branch(true);
        SetTailIsMethodReturn mr(true);
        PyrSlot dummy;
        COMPILENODE(mExpr, &dummy, true);
        Return.emit(location());
    }
}


void PyrBlockReturnNode::compile(PyrSlot* result) {}


bool isUnassignableSymbol(PyrSymbol* varName) {
    return varName == s_this || varName == s_super || varName == s_curProcess || varName == s_curThread
        || varName == s_curMethod || varName == s_curBlock || varName == s_curClosure;
}

void compileAssignVar(PyrParseNode* node, PyrSymbol* varName, bool drop) {
    if (isUnassignableSymbol(varName)) {
        error("You may not assign to '%s'.", varName->name);
        nodePostErrorLine(node);
        compileErrors++;
        return;
    }
    if (std::isupper(varName->name[0])) {
        error("You may not assign to a class name.");
        nodePostErrorLine(node);
        compileErrors++;
        return;
    }

    const auto result = findVarName(gCompilingBlock, gCompilingClass, varName);
    if (!result) {
        error("Variable '%s' not defined.\n", varName->name);
        nodePostErrorLine(node);
        compileErrors++;
        return;
    }

    const FindVarNameResult findResult = *result;

    switch (findResult.varType) {
    case varInst: {
        if (drop) {
            if (findResult.index <= 15) {
                StoreInstVar.emit(node->location(), findResult.index);
            } else {
                StoreInstVarX.emit(node->location(), Operands::Index::fromRaw(findResult.index));
                Drop.emit(node->location());
            }
        } else {
            // TODO: why can't we use the shorter StoreInstVar here? It breaks for some reason.
            StoreInstVarX.emit(node->location(), Operands::Index::fromRaw(findResult.index));
        }
    } break;

    case varClass: {
        const auto index = findResult.index + slotRawInt(&findResult.classobj->classVarIndex);
        if (drop) {
            if (index < 4096) {
                StoreClassVar.emit(node->location(), index);
            } else {
                StoreClassVarX.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(index),
                                    Operands::UnsignedInt<16, 0>::fromFull(index));
                Drop.emit(node->location());
            }
        } else {
            StoreClassVarX.emit(node->location(), Operands::UnsignedInt<16, 1>::fromFull(index),
                                Operands::UnsignedInt<16, 0>::fromFull(index));
        }
    } break;

    case varConst: {
        error("You may not assign to a constant.");
        nodePostErrorLine(node);
        compileErrors++;
    } break;

    case varTemp: {
        if (drop) {
            if (findResult.index <= 15 && findResult.level < 8) {
                StoreTempVar.emit(node->location(), findResult.level, Operands::Index::fromRaw(findResult.index));
            } else {
                StoreTempVarX.emit(node->location(), Operands::FrameOffset::fromRaw(findResult.level),
                                   Operands::Index::fromRaw(findResult.index));
                Drop.emit(node->location());
            }
        } else {
            // TODO: why can't we use the shorter StoreTempVarX here? It breaks for some reason.
            StoreTempVarX.emit(node->location(), Operands::FrameOffset::fromRaw(findResult.level),
                               Operands::Index::fromRaw(findResult.index));
        }
    } break;

    default: {
        error("Should be impossible");
        nodePostErrorLine(node);
        compileErrors++;
    } break;
    }
}

void PyrAssignNode::compile(PyrSlot* result) {
    PyrSlot dummy;

    // postfl("compilePyrAssignNode\n");
    COMPILENODE(mExpr, &dummy, false);
    compileAssignVar((PyrParseNode*)this, slotRawSymbol(&mVarName->mSlot), mDrop);
}


int PyrSetterNode::isPartialApplication() {
    int sum = 0;
    if (mExpr1->mClassno == pn_CurryArgNode) {
        ((PyrCurryArgNode*)mExpr1)->mArgNum = sum;
        sum++;
    }
    if (mExpr2->mClassno == pn_CurryArgNode) {
        ((PyrCurryArgNode*)mExpr2)->mArgNum = sum;
        sum++;
    }
    return sum;
}

void PyrSetterNode::compileCall(PyrSlot* result) {
    PyrSlot dummy;
    char setterName[128];

    if (nodeListLength(mExpr1) > 1) {
        error("Setter method called with too many arguments.\n");
        nodePostErrorLine(mExpr1);
        compileErrors++;
        return;
    }

    COMPILENODE(mExpr1, &dummy, false);
    COMPILENODE(mExpr2, &dummy, false);

    const int isSuper = isSuperObjNode(mExpr1);

    sprintf(setterName, "%s_", slotRawSymbol(&mSelector->mSlot)->name);
    PyrSymbol* setterSym = getsym(setterName);

    slotRawSymbol(&mSelector->mSlot)->flags |= sym_Called;
    int selType; // TODO: this is not used.
    const int index = conjureSelectorIndex((PyrParseNode*)mSelector, gCompilingBlock, isSuper, setterSym, &selType);
    if (isSuper) {
        emitTailCall(location());
        SendSuperMsg.emit(location(), 2, Operands::SelectorIndex::fromRaw(index));
    } else {
        emitTailCall(location());
        SendMsg.emit(location(), 2, Operands::SelectorIndex::fromRaw(index));
    }
}

void PyrMultiAssignNode::compile(PyrSlot* result) {
    PyrSlot dummy;

    COMPILENODE(mExpr, &dummy, false);
    COMPILENODE(mVarList, &dummy, false);
}

void PyrMultiAssignVarListNode::compile(PyrSlot* result) {
    int i, numAssigns;
    PyrSlotNode* varname;

    // postfl("compilePyrMultiAssignVarListNode\n");
    numAssigns = nodeListLength((PyrParseNode*)mVarNames);
    varname = mVarNames;
    for (i = 0; i < numAssigns; ++i, varname = (PyrSlotNode*)varname->mNext) {
        Dup.emit(location());
        emitPushInt(location(), i);
        SendSpecialMsg.emit(location(), 2, { OpSpecialSelectors::At });
        compileAssignVar((PyrParseNode*)varname, slotRawSymbol(&varname->mSlot), 1);
    }

    if (mRest) {
        Dup.emit(location());
        emitPushInt(location(), i);
        SendSpecialMsg.emit(location(), 2, { OpSpecialSelectors::CopyToEnd });
        compileAssignVar((PyrParseNode*)mRest, slotRawSymbol(&mRest->mSlot), 1);
    }
}


int PyrDynDictNode::isPartialApplication() {
    int sum = 0;
    int numItems = nodeListLength(mElems);
    PyrParseNode* inode = mElems;
    for (int i = 0; i < numItems; ++i) {
        if (inode->mClassno == pn_CurryArgNode) {
            ((PyrCurryArgNode*)inode)->mArgNum = sum;
            sum++;
        }
        inode = (PyrParseNode*)inode->mNext;
    }
    return sum;
}

void PyrDynDictNode::compileCall(PyrSlot* result) {
    const int numItems = nodeListLength(mElems) >> 1;

    compilePushVar((PyrParseNode*)this, s_event);

    emitPushInt(location(), numItems);
    PushSpecialValue.emit(location(), { OpSpecialValue::Nil_ }); // push nil for proto
    PushSpecialValue.emit(location(), { OpSpecialValue::Nil_ }); // push nil for parent
    PushSpecialValue.emit(location(), { OpSpecialValue::True }); // push true for know
    SendSpecialMsg.emit(location(), 5, { OpSpecialSelectors::New });

    PyrParseNode* inode = mElems;
    PyrSlot dummy;
    for (int i = 0; i < numItems; ++i) {
        COMPILENODE(inode, &dummy, false);
        inode = (PyrParseNode*)inode->mNext;
        COMPILENODE(inode, &dummy, false);
        inode = (PyrParseNode*)inode->mNext;
        SendSpecialMsg.emit(location(), 3, { OpSpecialSelectors::Put });
    }
}


int PyrDynListNode::isPartialApplication() {
    int sum = 0;
    int numItems = nodeListLength(mElems);
    PyrParseNode* inode = mElems;
    for (int i = 0; i < numItems; ++i) {
        if (inode->mClassno == pn_CurryArgNode) {
            ((PyrCurryArgNode*)inode)->mArgNum = sum;
            sum++;
        }
        inode = (PyrParseNode*)inode->mNext;
    }
    return sum;
}

void PyrDynListNode::compileCall(PyrSlot* result) {
    const int numItems = nodeListLength(mElems);

    compilePushVar((PyrParseNode*)this, mClassname ? slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) : s_array);
    emitPushInt(location(), numItems);
    SendSpecialMsg.emit(location(), 2, { OpSpecialSelectors::New });

    PyrParseNode* inode = mElems;
    PyrSlot dummy;
    for (int i = 0; i < numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
        COMPILENODE(inode, &dummy, false);
        SendSpecialMsg.emit(location(), 2, { OpSpecialSelectors::Add });
    }
}

void PyrLitListNode::compile(PyrSlot* result) {
    PyrSlot* resultSlot;
    PyrSlot itemSlot;
    PyrObject* array;
    PyrParseNode* inode;
    int i, numItems, flags;

    // postfl("->compilePyrLitListNode\n");
    if (mClassname && slotRawSymbol(&((PyrSlotNode*)mClassname)->mSlot) != s_array) {
        error("Only Array is supported as literal type.\n");
        post("Compiling as an Array.\n");
    }
    resultSlot = (PyrSlot*)result;
    numItems = mElems ? nodeListLength(mElems) : 0;
    flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
    array = newPyrArray(compileGC(), numItems, flags, false);
    inode = mElems;
    for (i = 0; i < numItems; ++i, inode = (PyrParseNode*)inode->mNext) {
        COMPILENODE(inode, &itemSlot, false);
        array->slots[i] = itemSlot;
    }
    array->size = numItems;
    SetObject(resultSlot, array);
    // postfl("<-compilePyrLitListNode\n");
}


int litDictPut(PyrObject* dict, PyrSlot* key, PyrSlot* value);
int litDictPut(PyrObject* dict, PyrSlot* key, PyrSlot* value) { return errNone; }


void PyrLitDictNode::dump(int level) {}

void PyrLitDictNode::compile(PyrSlot* result) {}


extern LongStack closedFuncCharNo;
extern int lastClosedFuncCharNo;

bool nameOfIdentifierIsOkayOrPostError(PyrSymbol* name, PyrParseNode* node) {
    if (name == s_this) {
        error("Cannot redefine 'this'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_curProcess) {
        error("Cannot redefine 'thisProcess'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_curMethod) {
        error("Cannot redefine 'thisMethod'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_curBlock) {
        error("Cannot redefine 'thisFunctionDef'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_curClosure) {
        error("Cannot redefine 'thisFunction'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_curThread) {
        error("Cannot redefine 'thisThread'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    } else if (name == s_super) {
        error("Cannot redefine 'super'\n");
        nodePostErrorLine(node);
        emitCompilerErrorFromVersion({ 3, 16, 0 });
        return false;
    }
    return true;
}

void PyrBlockNode::compile(PyrSlot* slotResult) {
    const int flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;

    // Pushing stuff to global state
    const int prevFunctionHighestExternalRef = gFunctionHighestExternalRef;
    const bool prevFunctionCantBeClosed = gFunctionCantBeClosed;
    gFunctionHighestExternalRef = 0;
    gFunctionCantBeClosed = false;

    // This functionDef is what we actually produce here.
    auto fdef = newPyrFunctionDef(flags, location());
    SetObject(slotResult, fdef);

    // More global state.
    auto* prevFunctionDef = gCompilingBlock;
    gCompilingBlock = fdef;
    auto* prevClass = gCompilingClass;
    auto* prevPartiallyAppliedFunction = gPartiallyAppliedFunction;
    gPartiallyAppliedFunction = nullptr;
    if (mIsTopLevel) {
        gCompilingClass = class_interpreter;
        SetNil(&fdef->contextDef);
    } else {
        SetObject(&fdef->contextDef, prevFunctionDef);
    }

    // PyrMethodRaw holds meta data about the function def in a smaller format.
    auto* methraw = METHRAW(fdef);
    methraw->unused1 = 0;
    methraw->unused2 = 0;
    methraw->needsHeapContext = 0;
    methraw->numVariableArguments = 0;
    methraw->methType = methBlock;
    if (mArglist) {
        if (mArglist->mVariableArgumentName) {
            methraw->numVariableArguments += 1;
            if (mArglist->mVariableKeywordArgumentName) {
                methraw->numVariableArguments += 1;
            }
        } else {
            // We don't support variable keywords without variable arguments.
            assert(mArglist->mVariableKeywordArgumentName == nullptr);
        }
    }

    // Argument counts, there are a few different types.
    const uint32 numVariableArguments = methraw->numVariableArguments;
    const uint32 numNormalArguments = mArglist ? nodeListLength(mArglist->mVarDefs) : 0;
    const uint32 numVars = mVarlist ? nodeListLength(mVarlist->mVarDefs) : 0;
    const uint32 numArgsTotal = numNormalArguments + numVariableArguments;

    if (numNormalArguments > 255) {
        error("Too many arguments in function definition (> 255)\n");
        nodePostErrorLine((PyrParseNode*)mArglist->mVarDefs);
        compileErrors++;
    }

    if (numVars > 255) {
        error("Too many variables in function definition (> 255)\n");
        nodePostErrorLine((PyrParseNode*)mVarlist->mVarDefs);
        compileErrors++;
    }

    const uint32 numSlotsForProtoFrame = numArgsTotal + numVars;
    methraw->frameSize = (numSlotsForProtoFrame + FRAMESIZE) * sizeof(PyrSlot);
    if (numSlotsForProtoFrame) {
        auto proto = newPyrArray(compileGC(), numSlotsForProtoFrame, flags, false);
        proto->size = numSlotsForProtoFrame;
        SetObject(&fdef->prototypeFrame, proto);
    } else {
        SetNil(&fdef->prototypeFrame);
    }

    methraw->numNormalArguments = numNormalArguments;
    methraw->numVariables = numVars;
    methraw->totalNumArguments = numArgsTotal;
    methraw->numtemps = numSlotsForProtoFrame;
    methraw->popSize = numSlotsForProtoFrame;

    // Build argname and varname array
    auto* argNames = numArgsTotal > 0 ? newPyrSymbolArray(compileGC(), numArgsTotal, flags, false) : nullptr;
    if (numArgsTotal) {
        argNames->size = numArgsTotal;
        fdef->argNames = PyrSlot::make(argNames);
    } else {
        fdef->argNames = PyrSlot::make(PyrNil {});
    }

    auto* varNames = numVars > 0 ? newPyrSymbolArray(compileGC(), numVars, flags, false) : nullptr;
    if (numVars > 0) {
        varNames->size = numVars;
        fdef->varNames = PyrSlot::make(varNames);
    } else {
        fdef->varNames = PyrSlot::make(PyrNil {});
    }


    // Ensure no variable or argument in this scope collides.
    std::vector<PyrSymbol*> encounteredNames {};
    encounteredNames.reserve(numSlotsForProtoFrame);
    const auto compilerErrorIfDuplicate = [&](PyrSymbol* sym, PyrParseNode* where) {
        if (const auto fnd = std::find(encounteredNames.begin(), encounteredNames.end(), sym);
            fnd != encounteredNames.end()) {
            error("duplicate name");
            nodePostErrorLine(where);
            compileErrors++;
        }
        encounteredNames.push_back(sym);
    };

    // put normal argument names into argNames array.
    if (mArglist) {
        auto* argIt = mArglist->mVarDefs;
        for (size_t i { 0 }; i < numNormalArguments; ++i, argIt = reinterpret_cast<PyrVarDefNode*>(argIt->mNext)) {
            auto* sym = argIt->mVarName->mSlot.getSymbol();
            assert(sym);
            compilerErrorIfDuplicate(sym, argIt);
            argNames->symbols[i] = sym;
        }
    } else {
        assert(numNormalArguments == 0);
    }
    // put variable postiional and keyword argument names into argNames array.
    if (numVariableArguments >= 1) {
        assert(mArglist);
        assert(mArglist->mVariableArgumentName);
        auto* variableArgNameSym = mArglist->mVariableArgumentName->mSlot.getSymbol();
        assert(variableArgNameSym);
        compilerErrorIfDuplicate(variableArgNameSym, mArglist->mVariableArgumentName);
        argNames->symbols[numNormalArguments] = variableArgNameSym;
        if (numVariableArguments == 2) {
            // put variable keyword arguments into argNames array.
            auto* variableKeywordArgNameSym = mArglist->mVariableKeywordArgumentName->mSlot.getSymbol();
            assert(variableKeywordArgNameSym);
            compilerErrorIfDuplicate(variableKeywordArgNameSym, mArglist->mVariableKeywordArgumentName);
            argNames->symbols[numNormalArguments + 1] = variableKeywordArgNameSym;
        }
    }

    // put variable names into varNames array.
    if (mVarlist) {
        auto* varIt = mVarlist->mVarDefs;
        for (size_t i { 0 }; i < numVars; ++i, varIt = reinterpret_cast<PyrVarDefNode*>(varIt->mNext)) {
            auto* name = varIt->mVarName->mSlot.getSymbol();
            assert(name);
            if (nameOfIdentifierIsOkayOrPostError(name, varIt)) {
                compilerErrorIfDuplicate(name, varIt);
                varNames->symbols[i] = name;
            }
        }
    } else {
        assert(numVars == 0);
    }

    // fill prototype frame
    auto* prototypeFrame = fdef->prototypeFrame.getPyrObjType<PyrObject>();
    assert(numSlotsForProtoFrame > 0 ? prototypeFrame != nullptr : true);
    bool hasNonLiteralDefaultInitialisers = false; // Will require compiling the assignments if true.

    // Put argument defaults into proto if literals.
    if (mArglist) {
        auto* argIt = mArglist->mVarDefs;
        for (size_t i = 0; i < numNormalArguments; ++i, argIt = (PyrVarDefNode*)argIt->mNext) {
            PyrSlot litval;
            if (argIt->hasExpr(&litval))
                hasNonLiteralDefaultInitialisers = true;
            prototypeFrame->slots[i] = litval;
        }
    }

    // put variable argument and variable keyword arguments default into proto, always the empty array.
    for (size_t i = numNormalArguments; i < numArgsTotal; ++i) {
        prototypeFrame->slots[i] = o_emptyarray;
    }

    // put variable defaults into proto if literals.
    if (mVarlist) {
        auto* varIt = mVarlist->mVarDefs;
        for (size_t i = numArgsTotal, j = 0; i < numArgsTotal + numVars;
             ++i, ++j, varIt = reinterpret_cast<PyrVarDefNode*>(varIt->mNext)) {
            PyrSlot litval;
            if (varIt->hasExpr(&litval))
                hasNonLiteralDefaultInitialisers = true;
            prototypeFrame->slots[i] = litval;
        }
    }

    // compile stuff.
    gCompilingBytecodes.assertEmpty();
    {
        PyrSlot dummy;

        SetTailBranch branch(true);
        SetTailIsMethodReturn mr(false);

        // Compile the arguments if not literals (or nil).
        if (hasNonLiteralDefaultInitialisers) {
            // Only compiles the node if they aren't literals.
            if (mArglist) {
                auto* vardef = mArglist->mVarDefs;
                for (size_t i = 0; i < numNormalArguments; ++i, vardef = (PyrVarDefNode*)vardef->mNext)
                    vardef->compileArg(&dummy);
            }
            if (mVarlist) {
                auto* vardef = mVarlist->mVarDefs;
                for (size_t i = 0; i < numVars; ++i, vardef = (PyrVarDefNode*)vardef->mNext)
                    vardef->compile(&dummy);
            }
        }

        // Compile the body of the block.
        if (mBody->mClassno == pn_BlockReturnNode) {
            PushSpecialValue.emit(location(), { OpSpecialValue::Nil_ });
        } else {
            COMPILENODE(mBody, &dummy, true);
        }
    }
    BlockReturn.emit(location());
    installByteCodes(fdef, std::move(gCompilingBytecodes).finishGetData());

    if ((!gFunctionCantBeClosed && gFunctionHighestExternalRef == 0) || mIsTopLevel) {
        SetNil(&fdef->contextDef);
        fdef->isClosed = PyrSlot::make(true);
    } else {
        fdef->isClosed = PyrSlot::make(false);
    }

    // Pop global state, put things back.
    gCompilingBlock = prevFunctionDef;
    gCompilingClass = prevClass;
    gPartiallyAppliedFunction = prevPartiallyAppliedFunction;
    gFunctionCantBeClosed = gFunctionCantBeClosed || prevFunctionCantBeClosed;
    gFunctionHighestExternalRef = sc_max(gFunctionHighestExternalRef - 1, prevFunctionHighestExternalRef);
}


PyrParseNode* linkNextNode(PyrParseNode* a, PyrParseNode* b) {
    if (a == nullptr) {
        return b;
    }
    if (b) {
        a->mTail->mNext = b;
        a->mTail = b->mTail;
    }
    return a;
}

PyrParseNode* linkAfterHead(PyrParseNode* a, PyrParseNode* b) {
    assert(a);
    assert(b);
    b->mNext = a->mNext;
    if (!a->mNext)
        a->mTail = b;
    a->mNext = b;
    return a;
}

bool isSuperObjNode(PyrParseNode* node) {
    return node->mClassno == pn_PushNameNode && slotRawSymbol(&((PyrSlotNode*)node)->mSlot) == s_super;
}

bool isThisObjNode(PyrParseNode* node) {
    return node->mClassno == pn_PushNameNode && slotRawSymbol(&((PyrSlotNode*)node)->mSlot) == s_this;
}

int nodeListLength(PyrParseNode* node) {
    int length = 0;
    for (; node; node = node->mNext)
        length++;
    return length;
}


// TODO: refactor this somehow so it is clear **exactly** what all the return arguments do.
int conjureSelectorIndex(PyrParseNode* node, PyrFunctionDef* func, bool isSuper, PyrSymbol* selector, int* selType) {
    int i;
    PyrObject* selectors;
    PyrSlot* slot;
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
        selectors = (PyrObject*)newPyrArray(compileGC(), 2, flags, false);
        SetObject(&func->selectors, selectors);
    }
    // otherwise add it to the selectors table

    if (selectors->size + 1 >= 256) {
        error("Selector table too big: too many classes, method selectors or function definitions in this function. "
              "Simplify the function.\n");
        post("Next selector was: %s\n", selector->name);
        nodePostErrorLine(node);
        compileErrors++;
        return 0;
    }

    if (selectors->size + 1 > ARRAYMAXINDEXSIZE(selectors)) {
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
    return selectors->size - 1;
}

Byte conjureLiteralSlotIndex(PyrParseNode* node, PyrFunctionDef* func, PyrSlot* slot) {
    int i;
    PyrObject* selectors;
    PyrSlot* slot2;
    int newsize, flags;

    flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;
    // lookup slot in selectors table

    if (IsObj(&func->selectors)) {
        selectors = slotRawObject(&func->selectors);
        /*if (selectors->classptr != class_array) {
            post("compiling %s:%s\n", slotRawSymbol(&gCompilingClass->name)->name,
        slotRawSymbol(&gCompilingMethod->name)->name); post("selectors is a '%s'\n",
        selectors->classptr->name.us->name); dumpObjectSlot(slot); Debugger();
        }*/
        for (i = 0; i < selectors->size; ++i)
            if (SlotEq(&selectors->slots[i], slot))
                return i;
    } else {
        selectors = (PyrObject*)newPyrArray(compileGC(), 4, flags, false);
        SetObject(&func->selectors, selectors);
    }
    // otherwise add it to the selectors table

    if (selectors->size + 1 >= 256) {
        error("Selector table too big: too many classes, method selectors or function definitions in this function. "
              "Simplify the function.\n");
        post("Next literal was:\n");
        dumpPyrSlot(slot);
        nodePostErrorLine(node);
        compileErrors++;
        return 0;
    }
    if (selectors->size + 1 > ARRAYMAXINDEXSIZE(selectors)) {
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

    return static_cast<Byte>(selectors->size - 1);
}


int conjureConstantIndex(PyrParseNode* node, PyrFunctionDef* func, PyrSlot* slot) {
    const int flags = compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable;

    // lookup slot in constants table
    PyrObject* constants;
    if (IsObj(&func->constants)) {
        constants = slotRawObject(&func->constants);
        for (int i = 0; i < constants->size; ++i)
            if (SlotEq(&constants->slots[i], slot))
                return i;
    } else {
        constants = (PyrObject*)newPyrArray(compileGC(), 4, flags, false);
        SetObject(&func->constants, constants);
    }

    // otherwise add it to the constants table
    if (constants->size + 1 > ARRAYMAXINDEXSIZE(constants)) {
        // resize literal table
        int newsize = ARRAYMAXINDEXSIZE(constants) * 2;
        // resize literal table
        SetRaw(&func->constants, (PyrObject*)newPyrArray(compileGC(), newsize, flags, false));
        memcpy(slotRawObject(&func->constants)->slots, constants->slots, constants->size * sizeof(PyrSlot));
        slotRawObject(&func->constants)->size = constants->size;
        freePyrObject((PyrObject*)constants);
        constants = slotRawObject(&func->constants);
    }
    slotCopy(&constants->slots[constants->size++], slot);

    return constants->size - 1;
}

bool findVarName(PyrFunctionDef* func, PyrClass** classobj, PyrSymbol* name, int* varType, int* level, int* index,
                 PyrFunctionDef** tempfunc) {
    int i, j, k;
    int numargs;
    PyrSymbol *argname, *varname;
    PyrMethodRaw* methraw;

    // postfl("->findVarName %s\n", name->name);
    // find var in enclosing blocks, instance, class
    if (name == s_super) {
        gFunctionCantBeClosed = true;
        name = s_this;
    }
    if (name->name[0] >= 'A' && name->name[0] <= 'Z')
        return false;

    j = 0;
    while (func != nullptr) {
        methraw = METHRAW(func);
        numargs = methraw->totalNumArguments;
        for (i = 0; i < numargs; ++i) {
            argname = slotRawSymbolArray(&func->argNames)->symbols[i];
            // postfl("    %d %d arg '%s' '%s'\n", j, i, argname->name, name->name);
            if (argname == name) {
                *level = j;
                *index = i;
                *varType = varTemp;
                if (tempfunc)
                    *tempfunc = func;
                if (j > gFunctionHighestExternalRef)
                    gFunctionHighestExternalRef = j;
                return true;
            }
        }
        for (i = 0, k = numargs; i < methraw->numVariables; ++i, ++k) {
            varname = slotRawSymbolArray(&func->varNames)->symbols[i];
            // postfl("    %d %d %d var '%s' '%s'\n", j, i, k, varname->name, name->name);
            if (varname == name) {
                *level = j;
                *index = k;
                *varType = varTemp;
                if (tempfunc)
                    *tempfunc = func;
                if (j > gFunctionHighestExternalRef)
                    gFunctionHighestExternalRef = j;
                return true;
            }
        }

        func = slotRawBlock(&func->contextDef);
        ++j;
    }

    if (classFindInstVar(*classobj, name, index)) {
        *level = 0;
        *varType = varInst;
        if (gCompilingClass != class_interpreter)
            gFunctionCantBeClosed = true;
        return true;
    }
    if (classFindClassVar(classobj, name, index)) {
        *varType = varClass;
        if (gCompilingClass != class_interpreter)
            gFunctionCantBeClosed = true;
        return true;
    }
    if (classFindConst(classobj, name, index)) {
        *varType = varConst;
        // if (gCompilingClass != class_interpreter) gFunctionCantBeClosed = true;
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

std::vector<PyrVarDefNode*> PyrNewBlockNode::gather_vars() const {
    std::vector<PyrVarDefNode*> nodes {};
    for_each_var([&](auto* n, size_t) { nodes.push_back(n); });
    return nodes;
}

uint32_t PyrNewBlockNode::count_vars() const {
    uint32_t num_vars { 0 };
    const auto counter = [&](PyrVarDefNode*, size_t i) { ++num_vars; };
    for_each_var(counter);
    return num_vars;
}

template <typename T> class Defer {
public:
    constexpr Defer(T&& action): action(std::forward<T>(action)) {}
    constexpr Defer() = delete;
    constexpr Defer(Defer&&) = delete;
    constexpr Defer(const Defer&) = delete;
    constexpr Defer& operator=(Defer&&) = delete;
    constexpr Defer& operator=(const Defer&) = delete;
    ~Defer() { action(); }

private:
    T action;
};

template <class T> T replace_return_old(T& ptr, T v) {
    T old { ptr };
    ptr = v;
    return old;
}

template <typename Action> void PyrNewBlockNode::for_each_normal_arg(Action&& action) const {
    size_t i { 0 };
    if (!arguments)
        return;
    for (auto* arg_item_it = arguments->mVarDefs; arg_item_it;
         arg_item_it = reinterpret_cast<PyrVarDefNode*>(arg_item_it->mNext)) {
        action(arg_item_it, i);
        ++i;
    }
}
template <typename Action> void PyrNewBlockNode::for_each_var_arg_name(Action&& action) const {
    if (!arguments)
        return;
    if (arguments->mVariableArgumentName)
        action(arguments->mVariableArgumentName, 0);
    if (arguments->mVariableKeywordArgumentName)
        action(arguments->mVariableKeywordArgumentName, 1);
}

template <typename Action> void PyrNewBlockNode::for_each_var(Action&& action) const {
    size_t i { 0 };
    // Please note, in PyrBlock, we call catVarList to join all the PyrVarDefNodes into one list.
    // We do not do that here because we can have multiple lists across the whole body, therefore our iteration is a
    // little more complex.
    for (auto* body_item_it = body; body_item_it; body_item_it = body_item_it->mNext) {
        if (const auto varlist = body_item_it->tryCast<PyrVarListNode>()) {
            for (auto* var_it = (*varlist)->mVarDefs; var_it;
                 var_it = reinterpret_cast<PyrVarDefNode*>(var_it->mNext)) {
                assert(var_it->mClassno == PyrVarDefNode::nodeEnum);
                action(var_it, i);
                ++i;
            }
        }
    }
}

template <typename ExprAction, typename VarDefAction>
void PyrNewBlockNode::for_each_body_expr_and_var_def(ExprAction&& expr_action, VarDefAction&& var_action) const {
    size_t i { 0 };

    for (auto* body_item_it = body; body_item_it; body_item_it = body_item_it->mNext) {
        if (const auto varlist = body_item_it->tryCast<PyrVarListNode>()) {
            for (auto* var_it = (*varlist)->mVarDefs; var_it;
                 var_it = reinterpret_cast<PyrVarDefNode*>(var_it->mNext)) {
                var_action(var_it, i);
                i += 1;
            }
        } else {
            expr_action(body_item_it, i);
            i += 1;
        }
    }
}

void PyrNewBlockNode::compile(PyrSlot* result) {
    const uint32 num_varible_args = [&]() -> uint32 {
        if (!arguments)
            return 0;
        if (!arguments->mVariableArgumentName)
            return 0;
        return (arguments->mVariableKeywordArgumentName) ? 2 : 1;
    }();
    const uint32 num_normal_args { arguments ? static_cast<uint32>(nodeListLength(arguments->mVarDefs)) : 0 };
    const uint32 num_total_args = num_normal_args + num_varible_args;
    const uint32 num_vars { count_vars() };
    const uint32 num_slots = num_total_args + num_vars;

    if (num_total_args > 255) {
        error("Too many arguments in function definition (> 255)\n");
        nodePostErrorLine(arguments);
        ++compileErrors;
    }

    if (num_vars > 255) {
        error("Too many variables in function definition (> 255)\n");
        nodePostErrorLine(this);
        ++compileErrors;
    }

    const auto gc_flags { compilingCmdLine ? obj_immutable : obj_permanent | obj_immutable };

    // Throughout this function we will be building fdef, it is the only mutable variable.
    auto fdef { newPyrFunctionDef(gc_flags, location()) };
    *result = PyrSlot::make(fdef);

    *METHRAW(fdef) = { 0,
                       0,
                       methBlock,
                       static_cast<unsigned short>((num_slots + FRAMESIZE) * sizeof(PyrSlot)),
                       0,
                       static_cast<unsigned char>(num_normal_args),
                       static_cast<unsigned char>(num_varible_args),
                       static_cast<unsigned char>(num_vars),
                       static_cast<unsigned char>(num_slots),
                       0,
                       static_cast<unsigned char>(num_slots),
                       static_cast<unsigned char>(num_total_args) };

    // Push globals
    const int prevFunctionHighestExternalRef { replace_return_old(gFunctionHighestExternalRef, 0) };
    const bool prevFunctionCantBeClosed { replace_return_old(gFunctionCantBeClosed, false) };
    auto* const prevFunctionDef { replace_return_old(gCompilingBlock, fdef) };
    auto* const prevClass { replace_return_old(gCompilingClass, is_top_level ? class_interpreter : gCompilingClass) };
    auto* const prevPartiallyAppliedFunction { replace_return_old(gPartiallyAppliedFunction,
                                                                  static_cast<PyrFunctionDef*>(nullptr)) };

    // Pop globals at end of scope.
    const Defer reset_globals { [&]() {
        gCompilingBlock = prevFunctionDef;
        gCompilingClass = prevClass;
        gPartiallyAppliedFunction = prevPartiallyAppliedFunction;
        gFunctionCantBeClosed = gFunctionCantBeClosed || prevFunctionCantBeClosed;
        gFunctionHighestExternalRef = sc_max(gFunctionHighestExternalRef - 1, prevFunctionHighestExternalRef);
    } };


    // This is overridden later, but it is important to set this now so name resolution can work.
    fdef->contextDef = is_top_level ? PyrSlot {} : PyrSlot::make(prevFunctionDef);

    // Ensure no variable or argument name in this scope collide.
    std::vector<PyrSymbol*> encounteredNames {};
    encounteredNames.reserve(num_slots);
    const auto compilerErrorIfDuplicate = [&](PyrSymbol* sym, PyrParseNode* where) {
        if (const auto fnd = std::find(encounteredNames.begin(), encounteredNames.end(), sym);
            fnd != encounteredNames.end()) {
            error("duplicate name");
            nodePostErrorLine(where);
            compileErrors++;
        }
        encounteredNames.push_back(sym);
    };

    // Fill argument names array
    fdef->argNames = (num_total_args == 0) ? PyrSlot {} : [&]() -> PyrSlot {
        auto* array = newPyrSymbolArray(compileGC(), num_total_args, gc_flags, false);
        for_each_normal_arg([&](PyrVarDefNode* n, size_t i) {
            auto* sym = n->mVarName->mSlot.getSymbol();
            if (nameOfIdentifierIsOkayOrPostError(sym, n)) {
                compilerErrorIfDuplicate(sym, n);
                array->symbols[i] = sym;
            }
        });
        for_each_var_arg_name([&](PyrSlotNode* n, size_t i) {
            auto* sym = n->mSlot.getSymbol();
            if (nameOfIdentifierIsOkayOrPostError(sym, n)) {
                compilerErrorIfDuplicate(sym, n);
                array->symbols[num_normal_args + i] = sym;
            }
        });
        array->size = num_total_args;
        return PyrSlot::make(array);
    }();

    // Fill variable names array
    fdef->varNames = (num_vars == 0) ? PyrSlot {} : [&]() -> PyrSlot {
        auto* array = newPyrSymbolArray(compileGC(), num_vars, gc_flags, false);
        for_each_var([&](PyrVarDefNode* var, size_t i) {
            auto* sym = var->mVarName->mSlot.getSymbol();
            if (nameOfIdentifierIsOkayOrPostError(sym, var)) {
                compilerErrorIfDuplicate(sym, var);
                array->symbols[i] = sym;
            }
        });
        array->size = num_vars;
        return PyrSlot::make(array);
    }();


    fdef->prototypeFrame = (num_slots == 0) ? PyrSlot {} : [&]() -> PyrSlot {
        auto array = newPyrArray(compileGC(), num_slots, gc_flags, false);

        size_t index { 0 };

        for_each_normal_arg([&](PyrVarDefNode* node, size_t i) {
            PyrSlot lit {};
            // hasExpr actually fills the slot if it is a literal value.
            // It also returns true if there is a non-literal expression.
            node->hasExpr(&lit);
            array->slots[index] = lit;
            index += 1;
        });

        for_each_var_arg_name([&](PyrSlotNode*, size_t i) {
            array->slots[index] = o_emptyarray;
            index += 1;
        });

        assert(index == num_total_args);

        for_each_var([&](PyrVarDefNode* node, size_t i) {
            PyrSlot lit {};
            node->hasExpr(&lit);
            array->slots[index] = lit;
            index += 1;
        });

        assert(index == num_slots);

        array->size = num_slots;
        return PyrSlot::make(array);
    }();

    gCompilingBytecodes.assertEmpty();
    // Compile body and return whether the final statement was an assignment.
    {
        SetTailBranch branch { true };
        SetTailIsMethodReturn method_return { false };

        for_each_normal_arg([&](PyrVarDefNode* node, size_t) {
            PyrSlot dummy;
            node->compileArg(&dummy);
        });

        // No need to compile the variable arguments, you can't set them.

        // Count how many statements there are.
        size_t num_statements_in_body { 0 };
        for_each_body_expr_and_var_def([&](PyrParseNode*, size_t) { num_statements_in_body += 1; },
                                       [&](PyrVarDefNode*, size_t) { num_statements_in_body += 1; });

        bool final_statement_was_a_var_def { false };

        for_each_body_expr_and_var_def(
            [&](PyrParseNode* expr, size_t i) {
                PyrSlot dummy {};
                const auto is_final = i == num_statements_in_body - 1;
                if (auto assign = expr->tryCast<PyrAssignNode>()) {
                    // TODO: Why is this necessary, I have just copied it from PyrDropNode::compile.
                    (*assign)->mDrop = !is_final;
                    // Tail call only on the last.
                    COMPILENODE(*assign, &dummy, is_final);
                } else {
                    // Tail call only on the last.
                    COMPILENODE(expr, &dummy, is_final);
                    if (!is_final)
                        Drop.emit(expr->location());
                }

                final_statement_was_a_var_def = false;
            },
            [&](PyrVarDefNode* node, size_t i) {
                PyrSlot dummy {};
                // Always drop the assignment.
                // If it was the last statement, we will undo this shortly.
                node->mDrop = true; // always drop `var = 1;`
                node->compile(&dummy);
                final_statement_was_a_var_def = true;
            });

        if (num_statements_in_body == 0 || final_statement_was_a_var_def)
            PushSpecialValue.emit(location(), { OpSpecialValue::Nil_ });
    };


    BlockReturn.emit(location());

    auto data = std::move(gCompilingBytecodes).finishGetData();

    // {
    //     std::cout << "printing bytecodes\n";
    //     auto start = data.codes.begin().base();
    //     const auto end = data.codes.end().base();
    //     Opcode::printAllBytecodes(std::cout, start, end);
    //     std::cout << std::endl;
    // }

    installByteCodes(fdef, std::move(data));

    const auto is_closed = (!gFunctionCantBeClosed && gFunctionHighestExternalRef == 0) || is_top_level;
    fdef->contextDef = is_closed ? PyrSlot {} : PyrSlot::make(prevFunctionDef);
    fdef->isClosed = PyrSlot::make(is_closed);
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
