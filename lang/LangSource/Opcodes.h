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

#ifndef _OPCODES_H_
#define _OPCODES_H_

// "X Macro" idiom [https://en.wikipedia.org/wiki/X_Macro]
// Allows for quick and correct lists in multiple locations
#define SPECIAL_UNARY_MATH_OPS \
	X(Neg, neg)                \
	X(Not, not)                \
	X(IsNil, isNil)            \
	X(NotNil, notNil)          \
	X(BitNot, bitNot)          \
	X(Abs, abs)                \
	X(AsFloat, asFloat)        \
	X(AsInt, asInt)            \
	X(Ceil, ceil)              \
	X(Floor, floor)            \
	X(Frac, frac)              \
	X(Sign, sign)              \
	X(Squared, squared)        \
	X(Cubed, cubed)            \
	X(Sqrt, sqrt)              \
	X(Exp, exp)                \
	X(Recip, reciprocal)       \
	X(MIDICPS, midicps)        \
	X(CPSMIDI, cpsmidi)        \
	X(MIDIRatio, midiratio)    \
	X(RatioMIDI, ratiomidi)    \
	X(DbAmp, dbamp)            \
	X(AmpDb, ampdb)            \
	X(OctCPS, octcps)          \
	X(CPSOct, cpsoct)          \
	X(Log, log)                \
	X(Log2, log2)              \
	X(Log10, log10)            \
	X(Sin, sin)                \
	X(Cos, cos)                \
	X(Tan, tan)                \
	X(ArcSin, asin)            \
	X(ArcCos, acos)            \
	X(ArcTan, atan)            \
	X(SinH, sinh)              \
	X(CosH, cosh)              \
	X(TanH, tanh)              \
	X(Rand, rand)              \
	X(Rand2, rand2)            \
	X(LinRand, linrand)        \
	X(BiLinRand, bilinrand)    \
	X(Sum3Rand, sum3rand)      \
	X(Distort, distort)        \
	X(SoftClip, softclip)      \
	X(Coin, coin)              \
	X(DigitValue, digitValue)  \
	X(Silence, silence)        \
	X(Thru, thru)              \
	X(RectWindow, rectWindow)  \
	X(HanWindow, hanWindow)    \
	X(WelchWindow, welWindow)  \
	X(TriWindow, triWindow)    \
	X(Ramp, ramp)              \
	X(SCurve, scurve)

// These need to be separate from the others because they are handled differently when defining
// primitives (see PyrPrimitive.cpp)
#define SPECIAL_BINARY_MATH_OPS_WITHOUT_ADVERBS \
	X(Add, +)                                   \
	X(Sub, -)                                   \
	X(Mul, *)

#define SPECIAL_BINARY_MATH_OPS_WITH_ADVERBS         \
	X(IDiv, div)                                     \
	X(FDiv, /)                                       \
	X(Mod, mod)                                      \
	X(EQ, ==)                                        \
	X(NE, !=)                                        \
	X(LT, <)                                         \
	X(GT, >)                                         \
	X(LE, <=)                                        \
	X(GE, >=)                                        \
	X(Min, min)                                      \
	X(Max, max)                                      \
	X(BitAnd, bitAnd)                                \
	X(BitOr, bitOr)                                  \
	X(BitXor, bitXor)                                \
	X(LCM, lcm)                                      \
	X(GCD, gcd)                                      \
	X(Round, round)                                  \
	X(RoundUp, roundUp)                              \
	X(Trunc, trunc)                                  \
	X(Atan2, atan2)                                  \
	X(Hypot, hypot)                                  \
	X(Hypotx, hypotApx)                              \
	X(Pow, pow)                                      \
	X(ShiftLeft, leftShift)                          \
	X(ShiftRight, rightShift)                        \
	X(UnsignedShift, unsignedRightShift)             \
	X(Fill, fill)                                    \
	X(Ring1, ring1)   /* a * (b + 1) == a * b + a */ \
	X(Ring2, ring2)   /* a * b + a + b */            \
	X(Ring3, ring3)   /* a*a*b */                    \
	X(Ring4, ring4)   /* a*a*b - a*b*b */            \
	X(DifSqr, difsqr) /* a*a - b*b */                \
	X(SumSqr, sumsqr) /* a*a + b*b */                \
	X(SqrSum, sqrsum) /* (a + b)^2 */                \
	X(SqrDif, sqrdif) /* (a - b)^2 */                \
	X(AbsDif, absdif) /* |a - b| */                  \
	X(Thresh, thresh)                                \
	X(AMClip, amclip)                                \
	X(ScaleNeg, scaleneg)                            \
	X(Clip2, clip2)                                  \
	X(Excess, excess)                                \
	X(Fold2, fold2)                                  \
	X(Wrap2, wrap2)                                  \
	X(FirstArg, firstArg)                            \
	X(RandRange, rrand)                              \
	X(ExpRandRange, exprand)

#define SPECIAL_SELECTORS                             \
	X(New, new)                                       \
	X(Init, init)                                     \
	X(At, at)                                         \
	X(Put, put)                                       \
	X(Next, next)                                     \
	X(Reset, reset)                                   \
	X(Value, value)                                   \
	X(CopyToEnd, copyToEnd)                           \
	X(Add, add)                                       \
	X(Size, size)                                     \
	X(Class, class)                                   \
	X(If, if)                                         \
	X(While, while)                                   \
	X(For, for)                                       \
	X(And, and)                                       \
	X(Or, or)                                         \
	X(Case, case)                                     \
	X(Switch, switch)                                 \
	X(Identical, ===)                                 \
	X(NotIdentical, !==)                              \
	X(Print, print)                                   \
	X(Remove, remove)                                 \
	X(IndexOf, indexOf)                               \
	X(WrapAt, wrapAt)                                 \
	X(ClipAt, clipAt)                                 \
	X(FoldAt, foldAt)                                 \
	X(WrapPut, wrapPut)                               \
	X(ClipPut, clipPut)                               \
	X(FoldPut, foldPut)                               \
	X(Do, do)                                         \
	X(Collect, collect)                               \
	X(Select, select)                                 \
	X(Reject, reject)                                 \
	X(Any, any)                                       \
	X(Every, every)                                   \
	X(Find, find)                                     \
	X(Choose, choose)                                 \
	X(ValueList, valueList)                           \
	X(AddFirst, addFirst)                             \
	X(PrimitiveFailed, primitiveFailed)               \
	X(SubclassResponsibility, subclassResponsibility) \
	X(ShouldNotImplement, shouldNotImplement)         \
	X(NotYetImplemented, notYetImplemented)           \
	X(DoesNotUnderstand, doesNotUnderstand)           \
	X(AtSign, @)                                      \
	X(WrapAtSign, @@)                                 \
	X(ClipAtSign, |@|)                                \
	X(FoldAtSign, @|@)                                \
	X(NewClear, newClear)                             \
	X(NewCopyArgs, newCopyArgs)                       \
	X(MultiNew, multiNew)                             \
	X(MultiNewList, multiNewList)                     \
	X(AR, ar)                                         \
	X(KR, kr)                                         \
	X(IR, ir)                                         \
	X(Copy, copy)                                     \
	X(PerformList, performList)                       \
	X(IsKindOf, isKindOf)                             \
	X(Postln, postln)                                 \
	X(AsString, asString)                             \
	X(EnvirGet, envirGet)                             \
	X(EnvirPut, envirPut)                             \
	X(Halt, halt)                                     \
	X(ForBy, forBy)                                   \
	X(ForSeries, forSeries)                           \
	X(ReverseDo, reverseDo)                           \
	X(Loop, loop)                                     \
	X(NonBooleanError, mustBeBoolean)                 \
	X(PlusPlus, ++)                                   \
	X(LTLT, <<)                                       \
	X(QuestionMark, ?)                                \
	X(DoubleQuestionMark, ?? ) /* ??) is a trigraph*/ \
	X(ExclamationQuestionMark, !?)                    \
	X(Yield, yield)                                   \
	X(Name, name)                                     \
	X(MulAdd, madd)                                   \
	X(Series, series)

// The layout of classes here reflects the inheritance hierarchy
#define BUILTIN_CLASSES                \
	X(object)                          \
		X(symbol)                      \
		X(nil)                         \
		X(boolean)                     \
			X(true)                    \
			X(false)                   \
		X(magnitude)                   \
			X(char)                    \
			X(number)                  \
				X(complex)             \
				X(simple_number)       \
					X(int)             \
					X(float)           \
		X(method)                      \
		X(fundef)                      \
		X(stream)                      \
		X(func)                        \
		X(frame)                       \
		X(process)                     \
			X(main)                    \
		X(class)                       \
		X(string)                      \
		X(collection)                  \
			X(sequenceable_collection) \
				X(arrayed_collection)  \
					X(array)           \
					X(int8array)       \
					X(int16array)      \
					X(int32array)      \
					X(floatarray)      \
						X(signal)      \
					X(doublearray)     \
					X(symbolarray)     \
				X(list)                \
				X(linkedlist)          \
			X(bag)                     \
			X(set)                     \
			X(identityset)             \
			X(dictionary)              \
			X(identitydictionary)      \
			X(sortedlist)              \
		X(synth)                       \
		X(ref)                         \
		X(environment)                 \
		X(event)                       \
		X(wavetable)                   \
		X(env)                         \
		X(routine)                     \
		X(color)                       \
		X(rect)

/* opcodes */
enum {
	opExtended,				//  0
	opPushInstVar,
	opPushTempVar,
	opPushTempZeroVar,
	opPushLiteral,
	opPushClassVar,			//  5
	opPushSpecialValue,
	opStoreInstVar,
	opStoreTempVar,
	opStoreClassVar,
	opSendMsg,				// 10
	opSendSuper,
	opSendSpecialMsg,
	opSendSpecialUnaryArithMsg,
	opSendSpecialBinaryArithMsg,
	opSpecialOpcode,		// 15


	opNumOpcodes
};

/* special opcodes */
enum {
	opcDrop,		// 0
	opcDup,
	opcFunctionReturn,
	opcReturn,
	opcReturnSelf,
	opcReturnTrue,	//  5
	opcReturnFalse,
	opcReturnNil,
	opcJumpIfFalse,				// IF 3 args
	opcJumpIfFalsePushNil,		// IF 2 args
	opcJumpIfFalsePushFalse,	// AND:		(10)
	opcJumpIfTruePushTrue,		// OR:
	opcJumpFwd,
	opcJumpBak,
	opcSpecialBinaryOpWithAdverb,
	opcSuperSpecial,		// 15

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
#define X( op_name, op_desc ) op ## op_name,
	SPECIAL_UNARY_MATH_OPS
#undef X
	opNumUnarySelectors
};

#define IS_UNARY_BOOL_OP(op)  ((op)>=opCoin && (op)<=opOdd)
#define IS_BINARY_BOOL_OP(op)  ((op)>=opEQ && (op)<=opGE)

/* special binary math operators */
enum {
#define X( op_name, op_desc ) op ## op_name,
	SPECIAL_BINARY_MATH_OPS_WITHOUT_ADVERBS
	SPECIAL_BINARY_MATH_OPS_WITH_ADVERBS
#undef X
	opNumBinarySelectors
};

enum {
#define X( op_name, op_desc ) opm ## op_name,
	SPECIAL_SELECTORS
#undef X
	opmNumSpecialSelectors
};

enum {
	opsvSelf,		// 0
	opsvMinusOne,
	opsvNegOne,
	opsvZero,
	opsvOne,
	opsvTwo,		// 5
	opsvFHalf,
	opsvFNegOne,
	opsvFZero,
	opsvFOne,
	opsvFTwo,		// 10
	opsvPlusOne,
	opsvTrue,
	opsvFalse,
	opsvNil,
	opsvInf,		// 15

	opsvNumSpecialValues
};

enum {
	opgProcess,
	opgMethod,
	opgFunctionDef,
	opgFunction,
	opgThread,
	//opgSampleRate,
	//opgAudioClock,
	//opgLogicalClock,

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

enum {
#define X( klass ) op_class_ ## klass,
	BUILTIN_CLASSES
#undef X
	op_NumSpecialClasses
};



#endif
