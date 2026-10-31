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

#include "SpecialSelectorsOperatorsAndClasses.h"
#include "PyrKernel.h"
#include "PyrObject.h"
#include "PredefinedSymbols.h"
#include "PyrSymbol.h"
#include "SCBase.h"
#include <cstdint>

PyrSymbol* gSpecialUnarySelectors[opNumUnarySelectors];
PyrSymbol* gSpecialBinarySelectors[opNumBinarySelectors];
PyrSymbol* gSpecialSelectors[opmNumSpecialSelectors];
PyrSymbol* gSpecialClasses[op_NumSpecialClasses];
SpecialValuesStruct gSpecialValues;
SpecialNumberStruct gSpecialNumbers;

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
