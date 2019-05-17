#include "SCBase.h"

#include "GC.h"
#include "PyrParseNode.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"

const char* nodeNameLabels[] = { "class",
                                 "classExtension",
                                 "method",
                                 "block",
                                 "slot",
                                 "variableList",
                                 "variableDefinition",
                                 "dynamicDictionary",
                                 "dynamicList",
                                 "literalList",
                                 "literalDictionary",
                                 "staticVariableList", // does not actually seem to ever be created
                                 "instanceVariableList", // does not actually seem to ever be created
                                 "poolVariableList", // does not actually seem to ever be created
                                 "argumentList",
                                 "slotDefinition",
                                 "literal",
                                 "pushLiteral",
                                 "pushName",
                                 "pushKeyArgument",
                                 "call",
                                 "binaryOperationCall",
                                 "drop",
                                 "assignment",
                                 "multiAssignment",
                                 "multiAssignmentVariableList",
                                 "setter",
                                 "curryArgument",
                                 "return",
                                 "blockReturn" };

int countParseNodeSiblings(PyrParseNode* node, int currentCount) {
    if (node) {
        int nextCount = countParseNodeSiblings(node->mNext);
        return currentCount + nextCount + 1;
    }
    return currentCount;
}

PyrObject* addDefaultReflectionProperties(PyrGC* gc, PyrObject* nodeArray, PyrParseNode* node,
                                          int additionalProperties) {
    assert(IsNil(nodeArray->slots + node->mNodeId));
    PyrObject* properties = newPyrArray(gc, (4 + additionalProperties) * 2, 0, true);
    SetObject(nodeArray->slots + node->mNodeId, properties);
    gc->GCWriteNew(nodeArray, properties);

    SetSymbol(properties->slots + properties->size, getsym("nodeType"));
    ++properties->size;
    SetSymbol(properties->slots + properties->size, getsym(nodeNameLabels[node->mClassno]));
    ++properties->size;

    SetSymbol(properties->slots + properties->size, getsym("textPosition"));
    ++properties->size;
    SetInt(properties->slots + properties->size, node->mTextpos);
    ++properties->size;

    SetSymbol(properties->slots + properties->size, getsym("parseLength"));
    ++properties->size;
    SetInt(properties->slots + properties->size, node->mParselen);
    ++properties->size;

    SetSymbol(properties->slots + properties->size, getsym("isParenthesis"));
    ++properties->size;
    SetBool(properties->slots + properties->size, node->mParens != 0);
    ++properties->size;

    return properties;
}

void appendNodeProperty(PyrGC* gc, PyrObject* nodeArray, PyrObject* propertiesArray, const char* propertyName,
                        PyrParseNode* node) {
    SetSymbol(propertiesArray->slots + propertiesArray->size, getsym(propertyName));
    ++propertiesArray->size;
    int nodeCount = countParseNodeSiblings(node);
    PyrObject* parseNodes = newPyrArray(gc, nodeCount, 0, true);
    SetObject(propertiesArray->slots + propertiesArray->size, parseNodes);
    ++propertiesArray->size;
    gc->GCWriteNew(propertiesArray, parseNodes);
    REFLECTNODE(gc, node, nodeArray, parseNodes);
}

// The parser often uses PyrSlotNode* objects as a way to attach a symbol (like a name) to another Node. In those
// instances instead of building a containing array with a single parseNodeProperties Array inside to describe a single
// symbol, we validate the assumption that this is the case with asserts and directly append the symbol instead.
void appendSlotNodeAsSymbol(PyrObject* propertiesArray, const char* propertyName, PyrSlotNode* slotNode) {
    SetSymbol(propertiesArray->slots + propertiesArray->size, getsym(propertyName));
    ++propertiesArray->size;
    if (slotNode) {
        assert(slotNode->mNext == nullptr);
        SetSymbol(propertiesArray->slots + propertiesArray->size, slotRawSymbol(&slotNode->mSlot));
    } else {
        SetNil(propertiesArray->slots + propertiesArray->size);
    }
    ++propertiesArray->size;
}

void reflectSlotNode(PyrGC* gc, PyrSlotNode* node, PyrObject* reflect) {
    SetSymbol(reflect->slots + reflect->size, getsym("slot"));
    ++reflect->size;
    if (IsObj(&node->mSlot)) {
        PyrObject* copy = copyObject(gc, slotRawObject(&node->mSlot), true);
        SetObject(reflect->slots + reflect->size, copy);
        gc->GCWriteNew(reflect, copy);
    } else {
        slotCopy(reflect->slots + reflect->size, &node->mSlot);
    }
    ++reflect->size;
}

void reflectPushLiteralNode(PyrGC* gc, PyrObject* nodeArray, PyrSlotNode* node, PyrObject* reflect) {
    if (!IsPtr(&(node->mSlot))) {
        reflectSlotNode(gc, node, reflect);
    } else {
        appendNodeProperty(gc, nodeArray, reflect, "pushLiteral", (PyrParseNode*)slotRawPtr(&(node->mSlot)));
    }
}

void reflectPushNameNode(PyrGC* gc, PyrSlotNode* node, PyrObject* reflect) {
    SetSymbol(reflect->slots + reflect->size, getsym("name"));
    ++reflect->size;
    SetSymbol(reflect->slots + reflect->size, getsym(slotRawSymbol(&node->mSlot)->name));
    ++reflect->size;
}

void reflectLiteralNode(PyrGC* gc, PyrObject* nodeArray, PyrSlotNode* node, PyrObject* reflect) {
    if (!IsPtr(&(node->mSlot))) {
        reflectSlotNode(gc, node, reflect);
    } else {
        appendNodeProperty(gc, nodeArray, reflect, "literal", (PyrParseNode*)slotRawPtr(&(node->mSlot)));
    }
}

void PyrSlotNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);

        switch (mClassno) {
        case pn_PushLitNode:
            reflectPushLiteralNode(gc, nodeArray, this, reflect);
            break;

        case pn_PushNameNode:
            reflectPushNameNode(gc, this, reflect);
            break;

        case pn_LiteralNode:
            reflectLiteralNode(gc, nodeArray, this, reflect);
            break;

        default:
            reflectSlotNode(gc, this, reflect);
            break;
        }

        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrCurryArgNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);
        SetSymbol(reflect->slots + reflect->size, getsym("argumentNumber"));
        ++reflect->size;
        SetInt(reflect->slots + reflect->size, mArgNum);
        ++reflect->size;
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrClassExtNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendSlotNodeAsSymbol(reflect, "name", mClassName);
        appendNodeProperty(gc, nodeArray, reflect, "methods", mMethods);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrClassNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 4);
        appendSlotNodeAsSymbol(reflect, "name", mClassName);
        appendNodeProperty(gc, nodeArray, reflect, "superClassName", mSuperClassName);
        appendNodeProperty(gc, nodeArray, reflect, "variables", mVarlists);
        appendNodeProperty(gc, nodeArray, reflect, "methods", mMethods);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrMethodNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 7);
        appendSlotNodeAsSymbol(reflect, "name", mMethodName);
        appendSlotNodeAsSymbol(reflect, "primitiveName", mPrimitiveName);
        appendNodeProperty(gc, nodeArray, reflect, "argumentList", mArglist);
        appendNodeProperty(gc, nodeArray, reflect, "variableList", mVarlist);
        appendNodeProperty(gc, nodeArray, reflect, "body", mBody);
        SetSymbol(reflect->slots + reflect->size, getsym("isClassMethod"));
        ++reflect->size;
        SetBool(reflect->slots + reflect->size, mIsClassMethod != 0);
        ++reflect->size;
        SetSymbol(reflect->slots + reflect->size, getsym("isExtension"));
        ++reflect->size;
        SetBool(reflect->slots + reflect->size, mExtension);
        ++reflect->size;
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrVarListNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);
        appendNodeProperty(gc, nodeArray, reflect, "variableDefinitions", mVarDefs);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrVarDefNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendSlotNodeAsSymbol(reflect, "name", mVarName);
        appendNodeProperty(gc, nodeArray, reflect, "definitionValue", mDefVal);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrCallNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 4);
        appendSlotNodeAsSymbol(reflect, "name", mSelector);
        appendNodeProperty(gc, nodeArray, reflect, "arguments", mArglist);
        appendNodeProperty(gc, nodeArray, reflect, "keyArguments", mKeyarglist);
        SetSymbol(reflect->slots + reflect->size, getsym("isTailCall"));
        ++reflect->size;
        SetBool(reflect->slots + reflect->size, mTailCall);
        ++reflect->size;
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrBinopCallNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendSlotNodeAsSymbol(reflect, "name", mSelector);
        appendNodeProperty(gc, nodeArray, reflect, "arguments", mArglist);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrSetterNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 4);
        appendSlotNodeAsSymbol(reflect, "name", mSelector);
        appendNodeProperty(gc, nodeArray, reflect, "selector", mSelector);
        appendNodeProperty(gc, nodeArray, reflect, "firstExpression", mExpr1);
        appendNodeProperty(gc, nodeArray, reflect, "secondExpression", mExpr2);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrDynListNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "className", mClassname);
        appendNodeProperty(gc, nodeArray, reflect, "elements", mElems);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrDynDictNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);
        appendNodeProperty(gc, nodeArray, reflect, "elements", mElems);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrDropNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "firstExpression", mExpr1);
        appendNodeProperty(gc, nodeArray, reflect, "secondExpression", mExpr2);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrPushKeyArgNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "selector", mSelector);
        appendNodeProperty(gc, nodeArray, reflect, "expression", mExpr);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrReturnNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);
        appendNodeProperty(gc, nodeArray, reflect, "expression", mExpr);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrBlockReturnNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 0);
        // mExpr may not necessarily be a valid pointer.
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrAssignNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "variableName", mVarName);
        appendNodeProperty(gc, nodeArray, reflect, "expression", mExpr);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrMultiAssignNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "variables", mVarList);
        appendNodeProperty(gc, nodeArray, reflect, "expression", mExpr);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrMultiAssignVarListNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "variableNames", mVarNames);
        appendNodeProperty(gc, nodeArray, reflect, "rest", mRest);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrBlockNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 5);
        appendNodeProperty(gc, nodeArray, reflect, "arguments", mArglist);
        appendNodeProperty(gc, nodeArray, reflect, "variables", mVarlist);
        appendNodeProperty(gc, nodeArray, reflect, "body", mBody);
        SetSymbol(reflect->slots + reflect->size, getsym("isTopLevel"));
        ++reflect->size;
        SetBool(reflect->slots + reflect->size, mIsTopLevel);
        ++reflect->size;
        SetSymbol(reflect->slots + reflect->size, getsym("beginsAt"));
        ++reflect->size;
        SetInt(reflect->slots + reflect->size, mBeginCharNo);
        ++reflect->size;
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrArgListNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "variableDefinitions", mVarDefs);
        appendNodeProperty(gc, nodeArray, reflect, "rest", mRest);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

void PyrLitListNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 2);
        appendNodeProperty(gc, nodeArray, reflect, "className", mClassname);
        appendNodeProperty(gc, nodeArray, reflect, "elements", mElems);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}

// this one is missing from DumpParseNode.cpp
void PyrLitDictNode::reflect(PyrGC* gc, PyrObject* nodeArray, PyrObject* idArray) {
    if (idArray) {
        SetInt(idArray->slots + idArray->size, mNodeId);
        ++idArray->size;
    }
    if (IsNil(nodeArray->slots + mNodeId)) {
        PyrObject* reflect = addDefaultReflectionProperties(gc, nodeArray, this, 1);
        appendNodeProperty(gc, nodeArray, reflect, "elements", mElems);
        REFLECTNODE(gc, mNext, nodeArray, idArray);
    }
}
