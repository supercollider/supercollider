#include "SCBase.h"

#include "GC.h"
#include "PyrParseNode.h"
#include "PyrSlot.h"
#include "PyrSymbol.h"

const char* nodeNameLabels[] = {
    "class",
    "classExtension",
    "method",
    "block",
    "slot",
    "variableList",
    "variableDefinition",
    "dictionary",
    "dictionaryList",
    "literalList",
    "literalDictionary",
    "staticVariableList",
    "instanceVariableList",
    "poolVariableList",
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
    "blockReturn"
};

int countParseNodeSiblings(PyrParseNode* node, int currentCount) {
    if (node) {
        int nextCount = countParseNodeSiblings(node->mNext);
        return currentCount + nextCount + 1;
    }
    return currentCount;
}

PyrObject* addDefaultReflectionProperties(VMGlobals* g, PyrObject* parentArray, int classNo, int lineNo, int charNo,
    int additionalProperties = 0) {
    PyrObject* properties = newPyrArray(g->gc, 1 + ((3 + additionalProperties) * 2), 0, true);
    SetObject(parentArray->slots + parentArray->size, properties);
    parentArray->size++;
    g->gc->GCWriteNew(parentArray, properties);

    // postfl("reflect: %s\n", nodeNameLabels[classNo]);

    // Add properties dictionary signifier.
    SetSymbol(properties->slots, getsym("parseNodeProperties"));
    ++properties->size;

    // Add nodeType pair.
    SetSymbol(properties->slots + 1, getsym("nodeType"));
    ++properties->size;
    SetSymbol(properties->slots + 2, getsym(nodeNameLabels[classNo]));
    ++properties->size;

    // Add lineNumber pair.
    SetSymbol(properties->slots + 3, getsym("lineNumber"));
    ++properties->size;
    SetInt(properties->slots + 4, lineNo);
    ++properties->size;

    // Add characterNumber pair.
    SetSymbol(properties->slots + 5, getsym("characterNumber"));
    ++properties->size;
    SetInt(properties->slots + 6, charNo);
    ++properties->size;

    return properties;
}

void appendNodeProperty(VMGlobals* g, PyrObject* propertiesArray, const char* propertyName, PyrParseNode* node) {
    SetSymbol(propertiesArray->slots + propertiesArray->size, getsym(propertyName));
    ++propertiesArray->size;
    int nodeCount = countParseNodeSiblings(node);
    PyrObject* parseNodes = newPyrArray(g->gc, nodeCount, 0, true);
    SetObject(propertiesArray->slots + propertiesArray->size, parseNodes);
    ++propertiesArray->size;
    g->gc->GCWriteNew(propertiesArray, parseNodes);
    REFLECTNODE(g, node, parseNodes);
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

void reflectSlotNode(VMGlobals* g, PyrSlotNode* node, PyrObject* reflect) {
    SetSymbol(reflect->slots + reflect->size, getsym("slot"));
    ++reflect->size;
    if (IsObj(&node->mSlot)) {
        PyrObject* copy = copyObject(g->gc, slotRawObject(&node->mSlot), true);
        SetObject(reflect->slots + reflect->size, copy);
        g->gc->GCWriteNew(reflect, copy);
    } else {
        slotCopy(reflect->slots + reflect->size, &node->mSlot);
    }
    ++reflect->size;
}

void reflectPushLiteralNode(VMGlobals* g, PyrSlotNode* node, PyrObject* reflect) {
    if (!IsPtr(&(node->mSlot))) {
        reflectSlotNode(g, node, reflect);
    } else {
        appendNodeProperty(g, reflect, "pushLiteral", (PyrParseNode*)slotRawPtr(&(node->mSlot)));
    }
}

void reflectPushNameNode(VMGlobals* g, PyrSlotNode* node, PyrObject* reflect) {
    SetSymbol(reflect->slots + reflect->size, getsym("name"));
    ++reflect->size;
    SetSymbol(reflect->slots + reflect->size, getsym(slotRawSymbol(&node->mSlot)->name));
    ++reflect->size;
}

void reflectLiteralNode(VMGlobals* g, PyrSlotNode* node, PyrObject* reflect) {
    if (!IsPtr(&(node->mSlot))) {
        reflectSlotNode(g, node, reflect);
    } else {
        appendNodeProperty(g, reflect, "literal", (PyrParseNode*)slotRawPtr(&(node->mSlot)));
    }
}

void PyrSlotNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    switch(mClassno) {
    case pn_PushLitNode:
        reflectPushLiteralNode(g, this, reflect);
        break;

    case pn_PushNameNode:
        reflectPushNameNode(g, this, reflect);
        break;

    case pn_LiteralNode:
        reflectLiteralNode(g, this, reflect);
        break;

    default:
        reflectSlotNode(g, this, reflect);
        break;
    }

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrCurryArgNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    SetSymbol(reflect->slots + reflect->size, getsym("argumentNumber"));
    ++reflect->size;
    SetInt(reflect->slots + reflect->size, mArgNum);
    ++reflect->size;

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrClassExtNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendSlotNodeAsSymbol(reflect, "name", mClassName);
    appendNodeProperty(g, reflect, "methods", mMethods);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrClassNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 4);

    appendSlotNodeAsSymbol(reflect, "name", mClassName);
    appendNodeProperty(g, reflect, "superClassName", mSuperClassName);
    appendNodeProperty(g, reflect, "variables", mVarlists);
    appendNodeProperty(g, reflect, "methods", mMethods);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrMethodNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 6);

    appendSlotNodeAsSymbol(reflect, "name", mMethodName);
    appendSlotNodeAsSymbol(reflect, "primitiveName", mPrimitiveName);
    appendNodeProperty(g, reflect, "argumentList", mArglist);
    appendNodeProperty(g, reflect, "variableList", mVarlist);
    appendNodeProperty(g, reflect, "body", mBody);
    SetSymbol(reflect->slots + reflect->size, getsym("isClassMethod"));
    ++reflect->size;
    SetBool(reflect->slots + reflect->size, mIsClassMethod != 0);
    ++reflect->size;
    SetSymbol(reflect->slots + reflect->size, getsym("isExtension"));
    ++reflect->size;
    SetBool(reflect->slots + reflect->size, mExtension);
    ++reflect->size;

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrVarListNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    appendNodeProperty(g, reflect, "variableDefinitions", mVarDefs);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrVarDefNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendSlotNodeAsSymbol(reflect, "name", mVarName);
    appendNodeProperty(g, reflect, "definitionValue", mDefVal);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrCallNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 4);

    appendSlotNodeAsSymbol(reflect, "name", mSelector);
    appendNodeProperty(g, reflect, "arguments", mArglist);
    appendNodeProperty(g, reflect, "keyArguments", mKeyarglist);
    SetSymbol(reflect->slots + reflect->size, getsym("isTailCall"));
    ++reflect->size;
    SetBool(reflect->slots + reflect->size, mTailCall);
    ++reflect->size;

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrBinopCallNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendSlotNodeAsSymbol(reflect, "name", mSelector);
    appendNodeProperty(g, reflect, "arguments", mArglist);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrSetterNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 3);

    appendSlotNodeAsSymbol(reflect, "name", mSelector);
    appendNodeProperty(g, reflect, "selector", mSelector);
    appendNodeProperty(g, reflect, "firstExpression", mExpr1);
    appendNodeProperty(g, reflect, "secondExpression", mExpr2);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrDynListNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "className", mClassname);
    appendNodeProperty(g, reflect, "elements", mElems);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrDynDictNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    appendNodeProperty(g, reflect, "elements", mElems);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrDropNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "firstExpression", mExpr1);
    appendNodeProperty(g, reflect, "secondExpression", mExpr2);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrPushKeyArgNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "selector", mSelector);
    appendNodeProperty(g, reflect, "expression", mExpr);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrReturnNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    appendNodeProperty(g, reflect, "expression", mExpr);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrBlockReturnNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno);

    // mExpr may not necessarily be a valid pointer.

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrAssignNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "variableName", mVarName);
    appendNodeProperty(g, reflect, "expression", mExpr);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrMultiAssignNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "variables", mVarList);
    appendNodeProperty(g, reflect, "expression", mExpr);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrMultiAssignVarListNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "variableNames", mVarNames);
    appendNodeProperty(g, reflect, "rest", mRest);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrBlockNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 5);

    appendNodeProperty(g, reflect, "arguments", mArglist);
    appendNodeProperty(g, reflect, "variables", mVarlist);
    appendNodeProperty(g, reflect, "body", mBody);

    SetSymbol(reflect->slots + reflect->size, getsym("isTopLevel"));
    ++reflect->size;
    SetBool(reflect->slots + reflect->size, mIsTopLevel);
    ++reflect->size;

    SetSymbol(reflect->slots + reflect->size, getsym("beginsAt"));
    ++reflect->size;
    SetInt(reflect->slots + reflect->size, mBeginCharNo);
    ++reflect->size;

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrArgListNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "variableDefinitions", mVarDefs);
    appendNodeProperty(g, reflect, "rest", mRest);

    REFLECTNODE(g, mNext, siblingsArray);
}

void PyrLitListNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 2);

    appendNodeProperty(g, reflect, "classname", mClassname);
    appendNodeProperty(g, reflect, "elements", mElems);

    REFLECTNODE(g, mNext, siblingsArray);
}

// this one is missing from DumpParseNode.cpp
void PyrLitDictNode::reflect(VMGlobals* g, PyrObject* siblingsArray) {
    PyrObject* reflect = addDefaultReflectionProperties(g, siblingsArray, mClassno, mLineno, mCharno, 1);

    appendNodeProperty(g, reflect, "elements", mElems);

    REFLECTNODE(g, mNext, siblingsArray);
}


