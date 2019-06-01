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

#include "PyrKernel.h"
#include "PyrParseNode.h"
#include "PyrSymbol.h"
#include "SCBase.h"

void numBlockTemps(PyrBlock* block, long level, long* numArgNames, long* numVarNames);
void numBlockTemps(PyrBlock* block, long level, long* numArgNames, long* numVarNames) {
    long i;
    for (i = 0; i < level; ++i) {
        block = slotRawBlock(&block->contextDef);
    }
    *numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
    *numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
}

unsigned char* dumpOneByteCode(PyrBlock* theBlock, PyrClass* theClass, unsigned char* ip);
unsigned char* dumpOneByteCode(PyrBlock* theBlock, PyrClass* theClass, unsigned char* ip) {
    PyrClass* classobj;
    PyrBlock* block;
    PyrSlot* slot;
    PyrSymbol* selector;
    char str[256];
    long op1, op2, op3, op4, op5;
    long i, n, ival, jmplen;
    long numArgNames, numVarNames, numTemps;
    unsigned char* ipbeg;

    if (theClass == NULL) {
        block = theBlock;
        theClass = 0;
        while (block) {
            // dumpObject((PyrObject*)block);
            // post("block->classptr %d class_method %d  %d\n",
            //	block->classptr, class_method, isKindOf((PyrObject*)block, class_method));
            // if (block->classptr == class_method) {
            if (isKindOf((PyrObject*)block, class_method)) {
                theClass = slotRawClass(&((PyrMethod*)block)->ownerclass);
                break;
            }
            block = slotRawBlock(&block->contextDef);
        }
        if (theClass == NULL) {
            theClass = s_interpreter->u.classobj;
            // error("dumpByteCodes: no Class found.\n");
            // return NULL;
        }
    }
    ipbeg = slotRawInt8Array(&theBlock->code)->b;
    n = ip - ipbeg;
    op1 = *ip++;
    post("%3d   %02X", n, op1);
    switch (op1) {
    case 0: //	push class
        op2 = *ip++; // get literal index
        post(" %02X    PushClassX '%s'\n", op2, slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2])->name);
        break;
    case 1: // Extended, PushInstVar
        op2 = *ip++; // get inst var index
        post(" %02X    PushInstVarX '%s'\n", op2, slotRawSymbolArray(&theClass->instVarNames)->symbols[op2]->name);
        break;
    case 2: // Extended, PushTempVar
        op2 = *ip++; // get temp var level

        block = theBlock;
        for (i = op2; i--; block = slotRawBlock(&block->contextDef)) { /* noop */
        }
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        op3 = *ip++; // get temp var index
        if (op3 < numArgNames) {
            post(" %02X %02X PushTempVarX '%s'\n", op2, op3, slotRawSymbolArray(&block->argNames)->symbols[op3]->name);
        } else {
            post(" %02X %02X PushTempVarX '%s'\n", op2, op3,
                 slotRawSymbolArray(&block->varNames)->symbols[op3 - numArgNames]->name);
        }
        break;
    case 3: // Extended, PushTempZeroVar

        block = theBlock;
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        op2 = *ip++; // get temp var index
        if (op2 < numArgNames) {
            post(" %02X	   PushTempZeroVarX '%s'\n", op2, slotRawSymbolArray(&theBlock->argNames)->symbols[op2]->name);
        } else {
            post(" %02X	   PushTempZeroVarX '%s'\n", op2,
                 slotRawSymbolArray(&theBlock->varNames)->symbols[op2 - numArgNames]->name);
        }
        break;
    case 4: // Extended, PushLiteral
        op2 = *ip++; // get literal index
        // push a block if it is one
        slot = slotRawObject(&theBlock->selectors)->slots + op2;
        slotString(slot, str);
        post(" %02X    PushLiteralX %s\n", op2, str);
        break;
    case 5: // Extended, PushClassVar
        op2 = *ip++; // get class var literal index
        op3 = *ip++; // get class var index
        post(" %02X %02X PushClassVarX\n", op2, op3);
        break;
    case 6: // Extended, PushSpecialValue == push a special class
        op2 = *ip++; // get class name index
        classobj = gSpecialClasses[op2]->u.classobj;
        post(" %02X    PushSpecialClass '%s'\n", op2, slotRawSymbol(&classobj->name)->name);
        break;
    case 7: // Extended, StoreInstVar
        op2 = *ip++; // get inst var index
        post(" %02X    StoreInstVarX '%s'\n", op2, slotRawSymbolArray(&theClass->instVarNames)->symbols[op2]->name);
        break;
    case 8: // Extended, StoreTempVar
        op2 = *ip++; // get temp var level

        block = theBlock;
        for (i = op2; i--; block = slotRawBlock(&block->contextDef)) { /* noop */
        }
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        op3 = *ip++; // get temp var index
        if (op3 < numArgNames) {
            post(" %02X %02X StoreTempVarX '%s'\n", op2, op3, slotRawSymbolArray(&block->argNames)->symbols[op3]->name);
        } else {
            post(" %02X %02X StoreTempVarX '%s'\n", op2, op3,
                 slotRawSymbolArray(&block->varNames)->symbols[op3 - numArgNames]->name);
        }
        break;
    case 9: // Extended, StoreClassVar
        op2 = *ip++; // get class var literal index
        op3 = *ip++; // get class var index
        post(" %02X %02X StoreClassVarX\n", op2, op3);
        break;
    case 10: // Extended, SendMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op4]);
        post(" %02X %02X %02X SendMsgX '%s'\n", op2, op3, op4, selector->name);
        break;
    case 11: // Extended, SuperMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op4]);
        post(" %02X %02X %02X SuperMsgX '%s'\n", op2, op3, op4, selector->name);
        break;
    case 12: // Extended, SendSpecialMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        post(" %02X %02X %02X SendSpecialMsgX '%s'\n", op2, op3, op4, gSpecialSelectors[op4]->name);
        break;
    case 13: // Extended, SendSpecialUnaryArithMsg
        op2 = *ip++; // get selector index
        post(" %02X    SendSpecialUnaryArithMsgX '%s'\n", op2, gSpecialUnarySelectors[op2]->name);
        break;
    case 14: // Extended, SendSpecialBinaryArithMsg
        op2 = *ip++; // get selector index
        post(" %02X    SendSpecialBinaryArithMsgX '%s'\n", op2, gSpecialBinarySelectors[op2]->name);
        break;
    case 15: // Extended, SpecialOpcode (none yet)
        op2 = *ip++; // get extended special opcode
        switch (op2) {
        case opgProcess: // push thisProcess
            post(" %02X    opgProcess\n", op2);
            break;
        case opgThread: // push thisThread
            post(" %02X    opgThread\n", op2);
            break;
        case opgMethod: // push thisMethod
            post(" %02X    opgMethod\n", op2);
            break;
        case opgFunctionDef: // push thisBlock
            post(" %02X    opgFunctionDef\n", op2);
            break;
        case opgFunction: // push thisClosure
            post(" %02X    opgFunction\n", op2);
            break;
        }
        break;

    // PushInstVar, 0..15
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        post("       PushInstVar '%s'\n", slotRawSymbolArray(&theClass->instVarNames)->symbols[op1 & 15]->name);
        break;

    case 32:
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpIfTrue %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;

    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
        op2 = op1 & 15; // get temp var level

        block = theBlock;
        for (i = op2; i--; block = slotRawBlock(&block->contextDef)) { /* noop */
        }
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        op3 = *ip++; // get temp var index
        if (op3 >= 0 && op3 < numArgNames) {
            post(" %02X    PushTempVar '%s'\n", op3, slotRawSymbolArray(&block->argNames)->symbols[op3]->name);
        } else if (op3 >= 0) {
            post(" %02X    PushTempVar '%s'\n", op3,
                 slotRawSymbolArray(&block->varNames)->symbols[op3 - numArgNames]->name);
        }
        break;

    case 40:
        op5 = *ip++;
        ival = op5;
        slot = slotRawObject(&theBlock->constants)->slots + ival;
        slotString(slot, str);
        post(" %02X    PushConstant %s\n", op5, str);
        break;

    case 41:
        op4 = *ip++;
        op5 = *ip++;
        ival = (op4 << 8) | op5;
        slot = slotRawObject(&theBlock->constants)->slots + ival;
        slotString(slot, str);
        post(" %02X %02X PushConstant %s\n", op4, op5, str);
        break;

    case 42:
        op3 = *ip++;
        op4 = *ip++;
        op5 = *ip++;
        ival = (op3 << 16) | (op4 << 8) | op5;
        slot = slotRawObject(&theBlock->constants)->slots + ival;
        slotString(slot, str);
        post(" %02X %02X %02X PushConstant %s\n", op3, op4, op5, str);
        break;

    case 43:
        op2 = *ip++;
        op3 = *ip++;
        op4 = *ip++;
        op5 = *ip++;
        ival = (op2 << 24) | (op3 << 16) | (op4 << 8) | op5;
        slot = slotRawObject(&theBlock->constants)->slots + ival;
        slotString(slot, str);
        post(" %02X %02X %02X %02X PushConstant %s\n", op2, op3, op4, op5, str);
        break;

    case 44:
        op5 = *ip++;
        ival = (int32)(op5 << 24) >> 24;
        post(" %02X    PushInt %d\n", op5, ival);
        break;

    case 45:
        op4 = *ip++;
        op5 = *ip++;
        ival = (int32)((op4 << 24) | (op5 << 16)) >> 16;
        post(" %02X %02X PushInt %d\n", op4, op5, ival);
        break;

    case 46:
        op3 = *ip++;
        op4 = *ip++;
        op5 = *ip++;
        ival = (int32)((op3 << 24) | (op4 << 16) | (op5 << 8)) >> 8;
        post(" %02X %02X %02X PushInt %d\n", op3, op4, op5, ival);
        break;

    case 47:
        op2 = *ip++;
        op3 = *ip++;
        op4 = *ip++;
        op5 = *ip++;
        ival = (int32)((op2 << 24) | (op3 << 16) | (op4 << 8) | op5);
        post(" %02X %02X %02X %02X PushInt %d\n", op2, op3, op4, op5, ival);
        break;

    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
        op2 = op1 & 15; // get temp var index

        block = theBlock;
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        if (op2 < numArgNames) {
            post("		 PushTempZeroVar '%s'\n", slotRawSymbolArray(&theBlock->argNames)->symbols[op2]->name);
        } else {
            post("		 PushTempZeroVar '%s'\n",
                 slotRawSymbolArray(&theBlock->varNames)->symbols[op2 - numArgNames]->name);
        }
        break;

    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
        op2 = op1 & 15; // get temp var level
        slot = slotRawObject(&theBlock->constants)->slots + op2;
        slotString(slot, str);
        post("       PushLiteral %s\n", str);
        break;

    //	PushClassVar
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
        op3 = *ip++; // get class var index
        post(" %02X PushClassVar\n", op3);
        break;

    // PushSpecialValue
    case 96:
        post("       PushSpecialValue this\n");
        break;
    case 97:
        post("       PushOneAndSubtract\n");
        break;
    case 98:
        post("       PushSpecialValue -1\n");
        break;
    case 99:
        post("       PushSpecialValue 0\n");
        break;
    case 100:
        post("       PushSpecialValue 1\n");
        break;
    case 101:
        post("       PushSpecialValue 2\n");
        break;
    case 102:
        post("       PushSpecialValue 0.5\n");
        break;
    case 103:
        post("       PushSpecialValue -1.0\n");
        break;
    case 104:
        post("       PushSpecialValue 0.0\n");
        break;
    case 105:
        post("       PushSpecialValue 1.0\n");
        break;
    case 106:
        post("       PushSpecialValue 2.0\n");
        break;
    case 107:
        post("       PushOneAndAdd\n");
        break;
    case 108:
        post("       PushSpecialValue true\n");
        break;
    case 109:
        post("       PushSpecialValue false\n");
        break;
    case 110:
        post("       PushSpecialValue nil\n");
        break;
    case 111:
        post("       PushSpecialValue 'end'\n");
        break;

    // StoreInstVar, 0..15
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
        post("       StoreInstVar '%s'\n", slotRawSymbolArray(&theClass->instVarNames)->symbols[op1 & 15]->name);
        break;

    // StoreTempVar
    case 128:
    case 129:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
        op2 = op1 & 15; // get temp var level

        block = theBlock;
        for (i = op2; i--; block = slotRawBlock(&block->contextDef)) { /* noop */
        }
        numArgNames = slotRawSymbolArray(&block->argNames) ? slotRawSymbolArray(&block->argNames)->size : 0;
        numVarNames = slotRawSymbolArray(&block->varNames) ? slotRawSymbolArray(&block->varNames)->size : 0;
        numTemps = numArgNames + numVarNames;

        op3 = *ip++; // get temp var index
        if (op3 < numArgNames) {
            post(" %02X    StoreTempVar '%s'\n", op3, slotRawSymbolArray(&block->argNames)->symbols[op3]->name);
        } else {
            post(" %02X    StoreTempVar '%s'\n", op3,
                 slotRawSymbolArray(&block->varNames)->symbols[op3 - numArgNames]->name);
        }
        break;
    case 136:
        op2 = *ip++; // get inst var index
        op3 = *ip++; // get selector index
        selector = gSpecialSelectors[op3];
        post(" %02X %02X PushInstVarAndSendSpecialMsg '%s' '%s'\n", op2, op3,
             slotRawSymbolArray(&theClass->instVarNames)->symbols[op2]->name, selector->name);
        break;
    case 137:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        post(" %02X    PushAllArgs+SendMsg '%s'\n", op2, selector->name);
        break;
    case 138:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        post(" %02X     PushAllButFirstArg+SendMsg '%s'\n", op2, selector->name);
        break;
    case 139:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        post(" %02X    PushAllArgs+SendSpecialMsg '%s'\n", op2, selector->name);
        break;
    case 140:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        post(" %02X     PushAllButFirstArg+SendSpecialMsg '%s'\n", op2, selector->name);
        break;
    case 141:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        post(" %02X     PushAllButFirstTwoArgs+SendMsg '%s'\n", op2, selector->name);
        break;
    case 142:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        post(" %02X     PushAllButFirstTwoArgs+SendSpecialMsg '%s'\n", op2, selector->name);
        break;
    case 143:
        op2 = *ip++; // get loop opcode
        if (op2 < 23 || op2 > 27) {
            post(" %02X    ControlOpcode\n", op2);
            break;
        } else {
            op3 = *ip++; // jump
            op4 = *ip++; // jump
            jmplen = ((op3 & 0xFF) << 8) | (op4 & 0xFF);
            post(" %02X %02X %02X ControlOpcode %d  (%d)\n", op2, op3, op4, jmplen, n + jmplen + 3);
            break;
        }
        break;

    //	StoreClassVar
    case 144:
    case 145:
    case 146:
    case 147:
    case 148:
    case 149:
    case 150:
    case 151:
    case 152:
    case 153:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 159:
        op2 = op1 & 15;
        op3 = *ip++; // get class var index
        post(" %02X    StoreClassVar\n", op3);
        break;

    // SendMsg
    case 160:
    case 161:
    case 162:
    case 163:
    case 164:
    case 165:
    case 166:
    case 167:
    case 168:
    case 169:
    case 170:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        post(" %02X    SendMsg '%s'\n", op2, selector->name);
        break;

    // TailCallReturnFromFunction
    case 176:
        post("       TailCallReturnFromFunction\n");
        break;

    // SuperMsg
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        post(" %02X    SuperMsg '%s'\n", op2, selector->name);
        break;

    // SendSpecialMsg
    case 192:
    case 193:
    case 194:
    case 195:
    case 196:
    case 197:
    case 198:
    case 199:
    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        post(" %02X    SendSpecialMsg '%s'\n", op2, selector->name);
        break;

    // SendSpecialUnaryArithMsg
    case 208:
    case 209:
    case 210:
    case 211:
    case 212:
    case 213:
    case 214:
    case 215:
    case 216:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
    case 222:
    case 223:
        op2 = op1 & 15;
        selector = gSpecialUnarySelectors[op2];
        post("       SendSpecialUnaryArithMsg '%s'\n", selector->name);
        break;

    // SendSpecialBinaryArithMsg
    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
        op2 = op1 & 15;
        selector = gSpecialBinarySelectors[op2];
        post("       SendSpecialBinaryArithMsg '%s'\n", selector->name);
        break;

    // SpecialOpcodes
    case 240:
        post("       Drop\n");
        break;
    case 241:
        post("       Dup\n");
        break; // Dup

    case 242:
        post("       BlockReturn\n");
        break; // BlockReturn
    case 243:
        post("       Return\n");
        break; // Return
    case 244:
        post("       ReturnSelf\n");
        break; // ReturnSelf
    case 245:
        post("       ReturnTrue\n");
        break; // ReturnTrue
    case 246:
        post("       ReturnFalse\n");
        break; // ReturnFalse
    case 247:
        post("       ReturnNil\n");
        break; // ReturnNil

    case 248: // JumpIfFalse
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpIfFalse %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;
    case 249: // JumpIfFalsePushNil
        op2 = *ip++;
        op3 = *ip++;
        jmplen = ((op2 & 0xFF) << 8) | (op3 & 0xFF);
        post(" %02X %02X JumpIfFalsePushNil %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;
    case 250: // JumpIfFalsePushFalse
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpIfFalsePushFalse %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;
    case 251: // JumpIfTruePushTrue
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpIfTruePushTrue %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;
    case 252: // JumpFwd
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpFwd %d  (%d)\n", op2, op3, jmplen, n + jmplen + 3);
        break;
    case 253: // JumpBak
        op2 = *ip++;
        op3 = *ip++;
        jmplen = (op2 << 8) | op3;
        post(" %02X %02X JumpBak %d  (%d)\n", op2, op3, jmplen, n - jmplen + 1);
        break;
    case 254:
        op2 = *ip++;
        post(" %02X    SpecialBinaryOpWithAdverb\n", op2);
        break;
    case 255:
        post("       TailCallReturnFromMethod\n");
        break;
    }
    return ip;
}


bool detectSendSelector(PyrBlock* theBlock, PyrClass* theClass, unsigned char** ipp, PyrSymbol* testSelector);
bool detectSendSelector(PyrBlock* theBlock, PyrClass* theClass, unsigned char** ipp, PyrSymbol* testSelector) {
    PyrBlock* block;
    PyrSymbol* selector = 0;
    long op1, op2, op3, op4, op5, op6;
    unsigned char* ip = *ipp;
    if (theClass == NULL) {
        block = theBlock;
        theClass = 0;
        while (block) {
            if (isKindOf((PyrObject*)block, class_method)) {
                theClass = slotRawClass(&((PyrMethod*)block)->ownerclass);
                break;
            }
            block = slotRawBlock(&block->contextDef);
        }
        if (theClass == NULL) {
            theClass = s_interpreter->u.classobj;
        }
    }
    op1 = *ip++;
    switch (op1) {
    case 0: //	push class
        op2 = *ip++; // get literal index
        break;
    case 1: // Extended, PushInstVar
        op2 = *ip++; // get inst var index
        break;
    case 2: // Extended, PushTempVar
        op2 = *ip++; // get temp var level
        op3 = *ip++; // get temp var index
        break;
    case 3: // Extended, PushTempZeroVar
        op2 = *ip++; // get temp var level
        break;
    case 4: // Extended, PushLiteral
        op2 = *ip++; // get literal index
        break;
    case 5: // Extended, PushClassVar
        op2 = *ip++; // get class var literal index
        op3 = *ip++; // get class var index
        break;
    case 6: // Extended, PushSpecialValue == push a special class
        op2 = *ip++; // get class name index
        break;
    case 7: // Extended, StoreInstVar
        op2 = *ip++; // get inst var index
        break;
    case 8: // Extended, StoreTempVar
        op2 = *ip++; // get temp var level
        op3 = *ip++; // get class var index
        break;
    case 9: // Extended, StoreClassVar
        op2 = *ip++; // get class var literal index
        op3 = *ip++; // get class var index
        break;
    case 10: // Extended, SendMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op4]);
        break;
    case 11: // Extended, SuperMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op4]);
        break;
    case 12: // Extended, SendSpecialMsg
        op2 = *ip++; // get num args
        op3 = *ip++; // get num key args
        op4 = *ip++; // get selector index
        selector = gSpecialSelectors[op4];
        break;
    case 13: // Extended, SendSpecialUnaryArithMsg
        op2 = *ip++; // get selector index
        selector = gSpecialUnarySelectors[op2];
        break;
    case 14: // Extended, SendSpecialBinaryArithMsg
        op2 = *ip++; // get selector index
        selector = gSpecialBinarySelectors[op2];
        break;
    case 15: // Extended, SpecialOpcode (none yet)
        op2 = *ip++; // get extended special opcode
        break;

    // PushInstVar, 0..15
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        break;
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
        op2 = op1 & 15; // get temp var level
        op3 = *ip++; // get num key args
        break;

    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
        break;

    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
        break;

    //	PushClassVar
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
        op2 = op1 & 15;
        op3 = *ip++; // get class var index
        break;

    // PushSpecialValue
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
        break;

    // StoreInstVar, 0..15
    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
        break;

    // StoreTempVar
    case 128:
    case 129:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
        op2 = op1 & 15; // get temp var level
        op3 = *ip++; // get class var index
        break;
    case 136:
        op2 = *ip++; // get inst var index
        op3 = *ip++; // get selector index
        selector = gSpecialSelectors[op3];
        break;
    case 137:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        break;
    case 138:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        break;
    case 139:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        break;
    case 140:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        break;
    case 141:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        break;
    case 142:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        break;
    case 143:
        op2 = *ip++; // get selector index
        break;

    //	StoreClassVar
    case 144:
    case 145:
    case 146:
    case 147:
    case 148:
    case 149:
    case 150:
    case 151:
    case 152:
    case 153:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 159:
        op2 = op1 & 15;
        op3 = *ip++; // get class var index
        break;

    // SendMsg
    case 160:
    case 161:
    case 162:
    case 163:
    case 164:
    case 165:
    case 166:
    case 167:
    case 168:
    case 169:
    case 170:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        break;

    // SuperMsg
    case 176:
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        op2 = *ip++; // get selector index
        selector = slotRawSymbol(&slotRawObject(&theBlock->selectors)->slots[op2]);
        break;

    // SendSpecialMsg
    case 192:
    case 193:
    case 194:
    case 195:
    case 196:
    case 197:
    case 198:
    case 199:
    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
        op2 = *ip++; // get selector index
        selector = gSpecialSelectors[op2];
        break;

    // SendSpecialUnaryArithMsg
    case 208:
    case 209:
    case 210:
    case 211:
    case 212:
    case 213:
    case 214:
    case 215:
    case 216:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
    case 222:
    case 223:
        op2 = op1 & 15;
        selector = gSpecialUnarySelectors[op2];
        break;

    // SendSpecialBinaryArithMsg
    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
        op2 = op1 & 15;
        selector = gSpecialBinarySelectors[op2];
        break;

    // SpecialOpcodes
    case 240:
    case 241:
    case 242:
    case 243:
    case 244:
    case 245:
    case 246:
    case 247:
        break;

    case 248: // JumpIfFalse
    case 249: // JumpIfFalsePushNil
    case 250: // JumpIfFalsePushFalse
    case 251: // JumpIfTruePushTrue
    case 252: // JumpFwd
    case 253: // JumpBak
        op2 = *ip++;
        op3 = *ip++;
        break;
    case 254: // PushPosInt
    case 255: // PushNegInt
        op2 = *ip++;
        if (op2 & 0x80) {
            op3 = *ip++;
            if (op3 & 0x80) {
                op4 = *ip++;
                if (op4 & 0x80) {
                    op5 = *ip++;
                    if (op5 & 0x80) {
                        op6 = *ip++;
                    }
                }
            }
        }
        break;
    }

    *ipp = ip;
    return testSelector == selector;
}


void dumpByteCodes(PyrBlock* theBlock);
void dumpByteCodes(PyrBlock* theBlock) {
    PyrClass* theClass;
    PyrBlock* block;
    long size;
    unsigned char *ip, *ipbeg, *ipend;

    if (slotRawInt8Array(&theBlock->code) == NULL) {
        post("Code empty.\n");
        return;
    }
    block = theBlock;
    theClass = 0;
    while (block) {
        if (isKindOf((PyrObject*)block, class_method)) {
            theClass = slotRawClass(&((PyrMethod*)block)->ownerclass);
            break;
        }
        block = slotRawBlock(&block->contextDef);
    }
    if (theClass == NULL) {
        theClass = s_interpreter->u.classobj;
    }

    ip = ipbeg = slotRawInt8Array(&theBlock->code)->b;
    size = slotRawInt8Array(&theBlock->code)->size;
    ipend = ip + size;
    post("BYTECODES: (%d)\n", size);
    while (ip < ipend) {
        ip = dumpOneByteCode(theBlock, theClass, ip);
    }
}

bool detectSendSelectorIn(PyrBlock* theBlock, PyrSymbol* testSelector);
bool detectSendSelectorIn(PyrBlock* theBlock, PyrSymbol* testSelector) {
    PyrClass* theClass;
    PyrBlock* block;
    PyrSymbol* selector;
    long size;
    unsigned char *ip, *ipbeg, *ipend;

    if (slotRawInt8Array(&theBlock->code) == NULL) {
        PyrMethodRaw* methraw = METHRAW(theBlock);
        switch (methraw->methType) {
        case methRedirect:
        case methRedirectSuper:
        case methForwardInstVar:
        case methForwardClassVar:
            selector = slotRawSymbol(&theBlock->selectors);
            return selector == testSelector;
        default:
            return false;
        }
    }
    block = theBlock;
    theClass = 0;
    while (block) {
        if (isKindOf((PyrObject*)block, class_method)) {
            theClass = slotRawClass(&((PyrMethod*)block)->ownerclass);
            break;
        }
        block = slotRawBlock(&block->contextDef);
    }
    if (theClass == NULL) {
        theClass = s_interpreter->u.classobj;
    }

    ip = ipbeg = slotRawInt8Array(&theBlock->code)->b;
    size = slotRawInt8Array(&theBlock->code)->size;
    ipend = ip + size;
    bool res = false;
    while (ip < ipend) {
        if ((res = detectSendSelector(theBlock, theClass, &ip, testSelector)))
            break;
    }
    return res;
}


const char* byteCodeString(int code);
const char* byteCodeString(int code) {
    switch (code) {
    case 0:
        return "PushClassX";
    case 1:
        return "PushInstVarX";
    case 2:
        return "PushTempVarX";
    case 3:
        return "PushTempZeroVarX";
    case 4:
        return "PushLiteralX";
    case 5:
        return "PushClassVarX";
    case 6:
        return "PushSpecialClass";
    case 7:
        return "StoreInstVarX";
    case 8:
        return "StoreTempVarX";
    case 9:
        return "StoreClassVarX";
    case 10:
        return "SendMsgX";
    case 11:
        return "SuperMsgX";
    case 12:
        return "SendSpecialMsgX";
    case 13:
        return "SendSpecialUnaryArithMsgX";
    case 14:
        return "SendSpecialBinaryArithMsgX";
    case 15:
        return "PushGlobal";

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        return "PushInstVar";

    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
        return "PushTempVar";

    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
        return "PushTempZeroVar";

    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
        return "PushLiteral";

    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
        return "PushClassVar";

    case 96:
        return "PushSpecialValue this";
    case 97:
        return "PushOneAndSubtract";
    case 98:
        return "PushSpecialValue -1";
    case 99:
        return "PushSpecialValue 0";
    case 100:
        return "PushSpecialValue 1";
    case 101:
        return "PushSpecialValue 2";
    case 102:
        return "PushSpecialValue 0.5";
    case 103:
        return "PushSpecialValue -1.0";
    case 104:
        return "PushSpecialValue 0.0";
    case 105:
        return "PushSpecialValue 1.0";
    case 106:
        return "PushSpecialValue 2.0";
    case 107:
        return "PushOneAndAdd";
    case 108:
        return "PushSpecialValue true";
    case 109:
        return "PushSpecialValue false";
    case 110:
        return "PushSpecialValue nil";
    case 111:
        return "PushSpecialValue 'end'";

    case 112:
    case 113:
    case 114:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
        return "StoreInstVar";

    case 128:
    case 129:
    case 130:
    case 131:
        return "StoreTempVar";

    case 132:
        return "PushInstVarAndSendNext";
    case 133:
        return "PushInstVarAndSendPrNext";
    case 134:
        return "PushInstVarAndSendDemandThisNode";
    case 135:
        return "PushInstVarAndSendSpecialMsg";

    case 136:
    case 137:
    case 138:
    case 139:
    case 140:
    case 141:
    case 142:
    case 143:
        return "UndefinedOpcode";

    case 144:
    case 145:
    case 146:
    case 147:
    case 148:
    case 149:
    case 150:
    case 151:
    case 152:
    case 153:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 159:
        return "StoreClassVar";

    case 160:
    case 161:
    case 162:
    case 163:
    case 164:
    case 165:
    case 166:
    case 167:
    case 168:
    case 169:
    case 170:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
        return "SendMsg";

    case 176:
        return "TailCallReturnFromFunction";

    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
        return "SuperMsg";

    case 192:
    case 193:
    case 194:
    case 195:
    case 196:
    case 197:
    case 198:
    case 199:
    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
        return "SendSpecialMsg";

    case 208:
    case 209:
    case 210:
    case 211:
    case 212:
    case 213:
    case 214:
    case 215:
    case 216:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
    case 222:
    case 223:
        return "SendSpecialUnaryArithMsg";

    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
        return "SendSpecialBinaryArithMsg";

    // SpecialOpcodes
    case 240:
        return "Drop";
    case 241:
        return "Dup"; // Dup

    case 242:
        return "BlockReturn"; // BlockReturn
    case 243:
        return "Return"; // Return
    case 244:
        return "ReturnSelf"; // ReturnSelf
    case 245:
        return "ReturnTrue"; // ReturnTrue
    case 246:
        return "ReturnFalse"; // ReturnFalse
    case 247:
        return "ReturnNil"; // ReturnNil

    case 248:
        return "JumpIfFalse"; // JumpIfFalse
    case 249:
        return "JumpIfFalsePushNil"; // JumpIfFalsePushNil

    case 250:
        return "JumpIfFalsePushFalse"; // JumpIfFalsePushFalse
    case 251:
        return "JumpIfTruePushTrue"; // JumpIfTruePushTrue
    case 252:
        return "JumpFwd"; // JumpFwd
    case 253:
        return "JumpBak"; // JumpBak
    case 254:
        return "PushPosInt"; // PushPosInt
    case 255:
        return "TailCallReturnFromMethod"; // TailCallReturnFromMethod
    }
    return "unknown opcode";
}
