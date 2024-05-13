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

/* opcodes */
enum {
    opExtended, //  0
    opPushInstVar,
    opPushTempVar,
    opPushTempZeroVar,
    opPushLiteral,
    opPushClassVar, //  5
    opPushSpecialValue,
    opStoreInstVar,
    opStoreTempVar,
    opStoreClassVar,
    opSendMsg, // 10
    opSendSuper,
    opSendSpecialMsg,
    opSendSpecialUnaryArithMsg,
    opSendSpecialBinaryArithMsg,
    opSpecialOpcode, // 15


    opNumOpcodes
};

/* special opcodes */
enum {
    opcDrop, // 0
    opcDup,
    opcFunctionReturn,
    opcReturn,
    opcReturnSelf,
    opcReturnTrue, //  5
    opcReturnFalse,
    opcReturnNil,
    opcJumpIfFalse, // IF 3 args
    opcJumpIfFalsePushNil, // IF 2 args
    opcJumpIfFalsePushFalse, // AND:		(10)
    opcJumpIfTruePushTrue, // OR:
    opcJumpFwd,
    opcJumpBak,
    opcSpecialBinaryOpWithAdverb,
    opcSuperSpecial, // 15

    opcNewList,

    opcNumSpecialOpcodes
    /*
        opcSlotAt,
        opcByteAt,			// 15
        opcShortAt,
        opcInt32At,
        opcColorAt,
        opcFloatAt,
        opcDoubleAt
    */
};

/* special unary math operators */
enum {
    opNeg,
    opNot,
    opIsNil,
    opNotNil,
    opBitNot,
    opAbs,
    opAsFloat,
    opAsInteger,
    opCeil, // 5
    opFloor,
    opFrac,
    opSign,
    opSquared,
    opCubed, // 10
    opSqrt,
    opExp,
    opRecip,
    opMIDICPS,
    opCPSMIDI, // 15

    opMIDIRatio,
    opRatioMIDI,
    opDbAmp,
    opAmpDb,
    opOctCPS, // 20
    opCPSOct,
    opLog,
    opLog2,
    opLog10,
    opSin, // 25
    opCos,
    opTan,
    opArcSin,
    opArcCos,
    opArcTan,
    opSinH,
    opCosH, // 30
    opTanH,
    opRand,
    opRand2,
    opLinRand,
    opBiLinRand,

    //	opExpRand,
    //	opBiExpRand,
    opSum3Rand,
    //	opGammaRand,
    //	opGaussRand,
    //	opPoiRand,

    opDistort,
    opSoftClip,
    opCoin,

    opDigitValue,
    opSilence,
    opThru,
    opRectWindow,
    opHanWindow,
    opWelchWindow,
    opTriWindow,

    opRamp,
    opSCurve,

    opNumUnarySelectors
};

#define IS_UNARY_BOOL_OP(op) ((op) >= opCoin && (op) <= opOdd)
#define IS_BINARY_BOOL_OP(op) ((op) >= opEQ && (op) <= opGE)

/* special binary math operators */
enum {
    opAdd,
    opSub,
    opMul,
    opIDiv,
    opFDiv,
    opMod,
    opEQ,
    opNE,
    opLT,
    opGT,
    opLE,
    opGE,
    // opIdentical,
    // opNotIdentical,

    opMin,
    opMax,
    opBitAnd,
    opBitOr,
    opBitXor,
    opLCM,
    opGCD,
    opRound,
    opRoundUp,
    opTrunc,
    opAtan2,
    opHypot,
    opHypotx,
    opPow,
    opShiftLeft,
    opShiftRight,
    opUnsignedShift,
    opFill,
    opRing1, // a * (b + 1) == a * b + a
    opRing2, // a * b + a + b
    opRing3, // a*a*b
    opRing4, // a*a*b - a*b*b
    opDifSqr, // a*a - b*b
    opSumSqr, // a*a + b*b
    opSqrSum, // (a + b)^2
    opSqrDif, // (a - b)^2
    opAbsDif, // |a - b|
    opThresh,
    opAMClip,
    opScaleNeg,
    opClip2,
    opExcess,
    opFold2,
    opWrap2,
    opFirstArg,
    opRandRange,
    opExpRandRange,

    opNumBinarySelectors
};

/* other special math operators */
enum {
    /* 3 operands */
    opDivz,
    opClip,
    opWrap,
    opFold,
    opRampMult,
    opMix,
    /* 4 operands */
    opPoly3,
    /* 5 operands */
    opMapRange
};

enum {
    opmNew,
    opmInit,
    opmAt,
    opmPut,
    opmNext,
    opmReset,
    opmValue,
    opmCopyToEnd, // used by multi assign
    opmAdd, // used by dynamic list
    // opmIsNil,
    // opmNotNil,
    opmSize,
    opmClass,
    opmIf,
    opmWhile,
    opmFor,
    opmAnd,
    opmOr,
    opmCase,
    opmSwitch,
    opmIdentical,
    opmNotIdentical,
    opmPrint,
    opmRemove,
    opmIndexOf,
    opmWrapAt,
    opmClipAt,
    opmFoldAt,
    opmWrapPut,
    opmClipPut,
    opmFoldPut,
    opmDo,
    opmCollect,
    opmSelect,
    opmReject,
    opmAny,
    opmEvery,
    opmFind,
    opmChoose,
    opmValueList,
    opmAddFirst,
    opmPrimitiveFailed,
    opmSubclassResponsibility,
    opmShouldNotImplement,
    opmNotYetImplemented,
    opmDoesNotUnderstand,

    opmAtSign,
    opmWrapAtSign,
    opmClipAtSign,
    opmFoldAtSign,

    opmNewClear,
    opmNewCopyArgs,
    opmMultiNew,
    opmMultiNewList,
    opmAR,
    opmKR,
    opmIR,

    opmCopy,
    opmPerformList,
    opmIsKindOf,
    opmPostln,
    opmAsString,

    opmEnvirGet,
    opmEnvirPut,

    opmHalt,
    opmForBy,
    opmForSeries,
    opmReverseDo,
    opmLoop,

    opmNonBooleanError,

    opmPlusPlus,
    opmLTLT,
    opmQuestionMark,
    opmDoubleQuestionMark,
    opmExclamationQuestionMark,

    opmYield,
    opmName,
    opmMulAdd,

    opmSeries,

    opmNumSpecialSelectors
};

enum {
    opsvSelf, // 0
    opsvMinusOne,
    opsvNegOne,
    opsvZero,
    opsvOne,
    opsvTwo, // 5
    opsvFHalf,
    opsvFNegOne,
    opsvFZero,
    opsvFOne,
    opsvFTwo, // 10
    opsvPlusOne,
    opsvTrue,
    opsvFalse,
    opsvNil,
    opsvInf, // 15

    opsvNumSpecialValues
};

enum {
    opgProcess,
    opgMethod,
    opgFunctionDef,
    opgFunction,
    opgThread,
    // opgSampleRate,
    // opgAudioClock,
    // opgLogicalClock,

    opgNumPseudoVars
};

/* selector types */
enum {
    selNormal,
    selSpecial,
    selUnary,
    selBinary,
    selIf,
    selWhile,
    selAnd,
    selOr,
    selCase,
    selSwitch,
    selLoop,
    selSuperNew,
    selQuestionMark,
    selDoubleQuestionMark,
    selExclamationQuestionMark,

    selNumSelectorTypes
};


/*
    special classes:
    Object, List, Number, Int, Float, Signal, Complex, Point
*/
enum {
    op_class_object,
    op_class_symbol,
    op_class_nil,
    op_class_boolean,
    op_class_true,
    op_class_false,
    op_class_magnitude,
    op_class_char,
    op_class_number,
    op_class_complex,
    op_class_simple_number,
    op_class_int,
    op_class_float,
    op_class_method,
    op_class_fundef,
    op_class_stream,
    op_class_func,
    op_class_frame,
    op_class_process,
    op_class_main,
    op_class_class,
    op_class_string,
    op_class_collection,
    op_class_sequenceable_collection,
    op_class_arrayed_collection,
    op_class_array,
    op_class_int8array,
    op_class_int16array,
    op_class_int32array,
    op_class_floatarray,
    op_class_signal,
    op_class_doublearray,
    op_class_symbolarray,
    op_class_list,
    op_class_linkedlist,
    op_class_bag,
    op_class_set,
    op_class_identityset,
    op_class_dictionary,
    op_class_identitydictionary,
    op_class_sortedlist,
    op_class_synth,
    op_class_ref,
    op_class_environment,
    op_class_event,
    op_class_wavetable,
    op_class_env,

    op_class_routine,
    op_class_color,
    op_class_rect,

    op_NumSpecialClasses
};
