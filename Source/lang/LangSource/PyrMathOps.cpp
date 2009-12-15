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

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef __APPLE__
double log2(double x);
#endif

#include "Opcodes.h"
#include "PyrInterpreter.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrMathPrim.h"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "PyrParseNode.h"
#include "PyrSignal.h"
#include "PyrSched.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "MiscInlineMath.h"
#include "PyrKernelProto.h"

double hypotx(double x, double y);

#define BOOL(val) ((val) ? o_true : o_false)

#define IS_BINARY_BOOL_OP(op)  ((op)>=opEQ && (op)<=opGE)

int doSpecialUnaryArithMsg(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrSymbol *msg;
	int opcode = g->primitiveIndex;

	a = g->sp;

	switch (GetTag(a)) {
		case tagInt :
			switch (opcode) {
				case opNeg : SetRaw(a, -slotRawInt(a)); break;
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opBitNot : SetRaw(a, ~slotRawInt(a)); break;
				case opAbs : SetRaw(a, sc_abs(slotRawInt(a))); break;
				case opAsFloat : SetFloat(a, (double)slotRawInt(a)); break;
				case opAsInt : SetRaw(a, (int)slotRawInt(a)); break;
				case opCeil : SetRaw(a, slotRawInt(a)); break;
				case opFloor : SetRaw(a, slotRawInt(a)); break;
				case opFrac : SetRaw(a, 0); break;
				case opSign : SetRaw(a, slotRawInt(a) > 0 ? 1 : (slotRawInt(a) == 0 ? 0 : -1)); break;
				case opSquared : SetRaw(a, slotRawInt(a) * slotRawInt(a)); break;
				case opCubed : SetRaw(a, slotRawInt(a) * slotRawInt(a) * slotRawInt(a)); break;
				case opSqrt : SetFloat(a, sqrt((double)slotRawInt(a))); break;
				case opExp : SetFloat(a, exp((double)slotRawInt(a))); break;
				case opRecip : SetFloat(a, 1. / slotRawInt(a)); break;
				case opMIDICPS : SetFloat(a, sc_midicps((double)slotRawInt(a))); break;
				case opCPSMIDI : SetFloat(a, sc_cpsmidi((double)slotRawInt(a))); break;
				case opMIDIRatio : SetFloat(a, sc_midiratio((double)slotRawInt(a))); break;
				case opRatioMIDI : SetFloat(a, sc_ratiomidi((double)slotRawInt(a))); break;
				case opAmpDb : SetFloat(a, sc_ampdb((double)slotRawInt(a))); break;
				case opDbAmp : SetFloat(a, sc_dbamp((double)slotRawInt(a))); break;
				case opOctCPS : SetFloat(a, sc_octcps((double)slotRawInt(a))); break;
				case opCPSOct : SetFloat(a, sc_cpsoct((double)slotRawInt(a))); break;
				case opLog : SetFloat(a, log((double)slotRawInt(a))); break;
				case opLog2 : SetFloat(a, log2((double)slotRawInt(a))); break;
				case opLog10 : SetFloat(a, log10((double)slotRawInt(a))); break;
				case opSin : SetFloat(a, sin((double)slotRawInt(a))); break;
				case opCos : SetFloat(a, cos((double)slotRawInt(a))); break;
				case opTan : SetFloat(a, tan((double)slotRawInt(a))); break;
				case opArcSin : SetFloat(a, asin((double)slotRawInt(a))); break;
				case opArcCos : SetFloat(a, acos((double)slotRawInt(a))); break;
				case opArcTan : SetFloat(a, atan((double)slotRawInt(a))); break;
				case opSinH : SetFloat(a, sinh((double)slotRawInt(a))); break;
				case opCosH : SetFloat(a, cosh((double)slotRawInt(a))); break;
				case opTanH : SetFloat(a, tanh((double)slotRawInt(a))); break;
				case opRand : SetRaw(a, g->rgen->irand(slotRawInt(a))); break;
				case opRand2 : SetRaw(a, g->rgen->irand2(slotRawInt(a))); break;
				case opLinRand : SetRaw(a, g->rgen->ilinrand(slotRawInt(a))); break;
				case opBiLinRand : SetRaw(a, g->rgen->ibilinrand(slotRawInt(a))); break;

//				case opExpRand : SetFloat(a, g->rgen->exprand(slotRawInt(a))); break;
//				case opBiExpRand : SetFloat(a, g->rgen->biexprand(slotRawInt(a))); break;
				case opSum3Rand : SetFloat(a, g->rgen->sum3rand(slotRawInt(a))); break;

//				case opGammaRand : SetFloat(a, g->rgen->gammarand(slotRawInt(a))); break;
//				case opGaussRand : SetFloat(a, g->rgen->gaussrand(slotRawInt(a))); break;
//				case opPoiRand : SetFloat(a, g->rgen->poirand(slotRawInt(a))); break;

				case opDistort : SetFloat(a, sc_distort((double)slotRawInt(a))); break;
				case opSoftClip : SetFloat(a, sc_softclip((double)slotRawInt(a))); break;
				case opCoin : SetBool(a, (slotRawInt(a))); break;
				case opRectWindow : SetFloat(a, sc_rectwindow((double)slotRawInt(a))); break;
				case opHanWindow : SetFloat(a, sc_hanwindow((double)slotRawInt(a))); break;
				case opWelchWindow : SetFloat(a, sc_welwindow((double)slotRawInt(a))); break;
				case opTriWindow : SetFloat(a, sc_triwindow((double)slotRawInt(a))); break;
				case opSCurve : SetFloat(a, sc_scurve((double)slotRawInt(a))); break;
				case opRamp : SetFloat(a, sc_ramp((double)slotRawInt(a))); break;
				default : goto send_normal_1;
			}
			break;
		case tagChar :
			switch (opcode) {
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opAsInt : SetTagRaw(a, tagInt); break;
				case opDigitValue :
					if (slotRawInt(a) >= '0' && slotRawInt(a) <= '9') SetInt(a, slotRawInt(a) - '0');
					else if (slotRawInt(a) >= 'A' && slotRawInt(a) <= 'Z') SetInt(a, slotRawInt(a) - 'A');
					else if (slotRawInt(a) >= 'a' && slotRawInt(a) <= 'z') SetInt(a, slotRawInt(a) - 'a');
					else SetInt(a, 0);
					break;
				default : goto send_normal_1;
			}
			break;
		case tagPtr :
			switch (opcode) {
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				default : goto send_normal_1;
			}
			break;
		case tagNil :
			switch (opcode) {
				case opIsNil : SetTrue(a); break;
				case opNotNil : SetFalse(a); break;
				default : goto send_normal_1;
			}
			break;
		case tagFalse :
			switch (opcode) {
				case opNot : SetTrue(a); break;
				case opIsNil : /*SetFalse(a);*/ break;
				case opNotNil : SetTrue(a); break;
				default : goto send_normal_1;
			}
			break;
		case tagTrue :
			switch (opcode) {
				case opNot : SetFalse(a); break;
				case opIsNil : SetFalse(a); break;
				case opNotNil : /*SetTrue(a);*/ break;
				default : goto send_normal_1;
			}
			break;
		case tagSym :
			switch (opcode) {
				case opAsFloat :
				case opAsInt :
					goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				default : /* SetSymbol(a, slotRawSymbol(a)); */ break;
			}
		case tagObj :
			if (isKindOf(slotRawObject(a), class_signal)) {
				switch (opcode) {
					case opNeg : SetRaw(a, signal_invert(g, slotRawObject(a))); break;
					case opIsNil : SetFalse(a); break;
					case opNotNil : SetTrue(a); break;
					case opAbs : SetRaw(a, signal_abs(g, slotRawObject(a))); break;
					case opSign : SetRaw(a, signal_sign(g, slotRawObject(a))); break;
					case opSquared : SetRaw(a, signal_squared(g, slotRawObject(a))); break;
					case opCubed : SetRaw(a, signal_cubed(g, slotRawObject(a))); break;
					case opSqrt : SetRaw(a, signal_sqrt(g, slotRawObject(a))); break;
					case opExp : SetRaw(a, signal_exp(g, slotRawObject(a))); break;
					case opRecip : SetRaw(a, signal_recip(g, slotRawObject(a))); break;
					case opLog : SetRaw(a, signal_log(g, slotRawObject(a))); break;
					case opLog2 : SetRaw(a, signal_log2(g, slotRawObject(a))); break;
					case opLog10 : SetRaw(a, signal_log10(g, slotRawObject(a))); break;
					case opSin : SetRaw(a, signal_sin(g, slotRawObject(a))); break;
					//case opSin : SetRaw(a, signal_fsin(g, slotRawObject(a))); break;
					case opCos : SetRaw(a, signal_cos(g, slotRawObject(a))); break;
					case opTan : SetRaw(a, signal_tan(g, slotRawObject(a))); break;
					case opArcSin : SetRaw(a, signal_asin(g, slotRawObject(a))); break;
					case opArcCos : SetRaw(a, signal_acos(g, slotRawObject(a))); break;
					case opArcTan : SetRaw(a, signal_atan(g, slotRawObject(a))); break;
					case opSinH : SetRaw(a, signal_sinh(g, slotRawObject(a))); break;
					case opCosH : SetRaw(a, signal_cosh(g, slotRawObject(a))); break;
					case opTanH : SetRaw(a, signal_tanh(g, slotRawObject(a))); break;
					case opDistort : SetRaw(a, signal_distort(g, slotRawObject(a))); break;
					case opSoftClip : SetRaw(a, signal_softclip(g, slotRawObject(a))); break;
					default : goto send_normal_1;
				}
			} else {
				goto send_normal_1;
			}
			break;
		default : // double
			switch (opcode) {
				case opNeg : SetRaw(a, -slotRawFloat(a)); break;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opBitNot : SetRaw(a, ~(int)slotRawFloat(a)); break;
				case opAbs : SetRaw(a, sc_abs(slotRawFloat(a))); break;
				case opAsFloat : SetRaw(a, (double)slotRawFloat(a)); break;
				case opAsInt : SetInt(a, (int)slotRawFloat(a)); break;
				case opCeil : SetRaw(a, ceil(slotRawFloat(a))); break;
				case opFloor : SetRaw(a, floor(slotRawFloat(a))); break;
				case opFrac : SetRaw(a, sc_frac(slotRawFloat(a))); break;
				case opSign : SetRaw(a, slotRawFloat(a) > 0. ? 1 : (slotRawFloat(a) == 0 ? 0 : -1)); break;
				case opSquared : SetRaw(a, slotRawFloat(a) * slotRawFloat(a)); break;
				case opCubed : SetRaw(a, slotRawFloat(a) * slotRawFloat(a) * slotRawFloat(a)); break;
				case opSqrt : SetRaw(a, sqrt(slotRawFloat(a))); break;
				case opExp : SetRaw(a, exp(slotRawFloat(a))); break;
				case opRecip : SetRaw(a, 1./slotRawFloat(a)); break;
				case opMIDICPS : SetRaw(a, sc_midicps(slotRawFloat(a))); break;
				case opCPSMIDI : SetRaw(a, sc_cpsmidi(slotRawFloat(a))); break;
				case opMIDIRatio : SetRaw(a, sc_midiratio((double)slotRawFloat(a))); break;
				case opRatioMIDI : SetRaw(a, sc_ratiomidi((double)slotRawFloat(a))); break;
				case opAmpDb : SetRaw(a, sc_ampdb(slotRawFloat(a))); break;
				case opDbAmp : SetRaw(a, sc_dbamp(slotRawFloat(a))); break;
				case opOctCPS : SetRaw(a, sc_octcps(slotRawFloat(a))); break;
				case opCPSOct : SetRaw(a, sc_cpsoct(slotRawFloat(a))); break;
				case opLog : SetRaw(a, log(slotRawFloat(a))); break;
				case opLog2 : SetRaw(a, log2(slotRawFloat(a))); break;
				case opLog10 : SetRaw(a, log10(slotRawFloat(a))); break;
				case opSin : SetRaw(a, sin(slotRawFloat(a))); break;
				case opCos : SetRaw(a, cos(slotRawFloat(a))); break;
				case opTan : SetRaw(a, tan(slotRawFloat(a))); break;
				case opArcSin : SetRaw(a, asin(slotRawFloat(a))); break;
				case opArcCos : SetRaw(a, acos(slotRawFloat(a))); break;
				case opArcTan : SetRaw(a, atan(slotRawFloat(a))); break;
				case opSinH : SetRaw(a, sinh(slotRawFloat(a))); break;
				case opCosH : SetRaw(a, cosh(slotRawFloat(a))); break;
				case opTanH : SetRaw(a, tanh(slotRawFloat(a))); break;
				case opRand : SetRaw(a, g->rgen->frand() * slotRawFloat(a)); break;
				case opRand2 : SetRaw(a, g->rgen->frand2() * slotRawFloat(a)); break;
				case opLinRand   : SetRaw(a, g->rgen->linrand(slotRawFloat(a))); break;
				case opBiLinRand : SetRaw(a, g->rgen->bilinrand(slotRawFloat(a))); break;

//				case opExpRand   : SetRaw(a, g->rgen->exprand(slotRawFloat(a))); break;
//				case opBiExpRand : SetRaw(a, g->rgen->biexprand(slotRawFloat(a))); break;
				case opSum3Rand : SetRaw(a, g->rgen->sum3rand(slotRawFloat(a))); break;

//				case opGammaRand : SetRaw(a, g->rgen->gammarand(slotRawFloat(a))); break;
//				case opGaussRand : SetRaw(a, g->rgen->gaussrand(slotRawFloat(a))); break;
//				case opPoiRand   : SetRaw(a, g->rgen->poirand(slotRawFloat(a))); break;

				case opDistort : SetRaw(a, sc_distort(slotRawFloat(a))); break;
				case opSoftClip : SetRaw(a, sc_softclip(slotRawFloat(a))); break;
				case opCoin : SetBool(a, g->rgen->frand() < slotRawFloat(a)); break;
				case opRectWindow : SetRaw(a, sc_rectwindow(slotRawFloat(a))); break;
				case opHanWindow : SetRaw(a, sc_hanwindow(slotRawFloat(a))); break;
				case opWelchWindow : SetRaw(a, sc_welwindow(slotRawFloat(a))); break;
				case opTriWindow : SetRaw(a, sc_triwindow(slotRawFloat(a))); break;
				case opSCurve : SetRaw(a, sc_scurve(slotRawFloat(a))); break;
				case opRamp : SetRaw(a, sc_ramp(slotRawFloat(a))); break;
				default : goto send_normal_1;
			}
			break;
	}

#if TAILCALLOPTIMIZE
	g->tailCall = 0;
#endif
	return errNone;

send_normal_1:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;

	msg = gSpecialUnarySelectors[opcode];
	sendMessage(g, msg, 1);

	return errNone;
}

int prSpecialBinaryArithMsg(VMGlobals *g, int numArgsPushed)
{
	return doSpecialBinaryArithMsg(g, numArgsPushed, true);
}

int doSpecialBinaryArithMsg(VMGlobals *g, int numArgsPushed, bool isPrimitive)
{
	PyrSlot *a, *b, res;
	PyrSymbol *msg;
	int opcode = g->primitiveIndex;

	a = g->sp - (numArgsPushed - 1);
	b = a + 1;

	switch (GetTag(a)) {
		case tagInt : {
			switch (GetTag(b)) {
				case tagInt :
					switch (opcode) {
						case opAdd :	SetInt(&res, slotRawInt(a) + slotRawInt(b)); break;
						case opSub :	SetInt(&res, slotRawInt(a) - slotRawInt(b)); break;
						case opMul :	SetInt(&res, slotRawInt(a) * slotRawInt(b)); break;
						case opIDiv :	SetInt(&res, sc_div(slotRawInt(a), slotRawInt(b))); break;
						case opFDiv :	SetFloat(&res, (double)slotRawInt(a) / (double)slotRawInt(b)); break;
						case opMod :	SetInt(&res, sc_mod((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opEQ  :	res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
						case opNE  :	res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
						case opLT  :	res = BOOL(slotRawInt(a) <  slotRawInt(b)); break;
						case opGT  :	res = BOOL(slotRawInt(a) >  slotRawInt(b)); break;
						case opLE  :	res = BOOL(slotRawInt(a) <= slotRawInt(b)); break;
						case opGE  :	res = BOOL(slotRawInt(a) >= slotRawInt(b)); break;
						//case opIdentical : res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
						//case opNotIdentical : res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
						case opMin :	SetInt(&res, sc_min(slotRawInt(a), slotRawInt(b))); break;
						case opMax :	SetInt(&res, sc_max(slotRawInt(a), slotRawInt(b))); break;
						case opBitAnd :	SetInt(&res, slotRawInt(a) & slotRawInt(b)); break;
						case opBitOr  :	SetInt(&res, slotRawInt(a) | slotRawInt(b)); break;
						case opBitXor :	SetInt(&res, slotRawInt(a) ^ slotRawInt(b)); break;
						case opLCM   :	SetInt(&res, sc_lcm(slotRawInt(a), slotRawInt(b))); break;
						case opGCD   :	SetInt(&res, sc_gcd(slotRawInt(a), slotRawInt(b))); break;
						case opRound :	SetInt(&res, sc_round((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opRoundUp :SetInt(&res, sc_roundUp((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opTrunc :	SetInt(&res, sc_trunc((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opAtan2 :	SetFloat(&res, atan2((double)slotRawInt(a), (double)slotRawInt(b))); break;
						case opHypot :	SetFloat(&res, hypot((double)slotRawInt(a), (double)slotRawInt(b))); break;
						case opHypotx : SetFloat(&res, hypotx((double)slotRawInt(a), (double)slotRawInt(b))); break;
						case opPow   :	SetFloat(&res, pow((double)slotRawInt(a), (double)slotRawInt(b))); break;
						case opShiftLeft : {
							long ia = slotRawInt(a);
							long ib = slotRawInt(b);
							if (ib>0) ia <<= ib;
							else if (ib<0) ia >>= -ib;
							SetInt(&res, ia);
						} break;
						case opShiftRight : {
							long ia = slotRawInt(a);
							long ib = slotRawInt(b);
							if (ib>0) ia >>= ib;
							else if (ib<0) ia <<= -ib;
							SetInt(&res, ia);
						} break;
						case opUnsignedShift : {
							unsigned long ia = slotRawInt(a);
							long ib = slotRawInt(b);
							if (ib>0) ia >>= ib;
							else if (ib<0) ia <<= -ib;
							SetInt(&res, ia);
						} break;
						case opRing1 : SetInt(&res, sc_ring1(slotRawInt(a), slotRawInt(b))); break;
						case opRing2 : SetInt(&res, sc_ring2(slotRawInt(a), slotRawInt(b))); break;
						case opRing3 : SetInt(&res, sc_ring3(slotRawInt(a), slotRawInt(b))); break;
						case opRing4 : SetInt(&res, sc_ring4(slotRawInt(a), slotRawInt(a))); break;
						case opDifSqr : SetInt(&res, sc_difsqr(slotRawInt(a), slotRawInt(b))); break;
						case opSumSqr : SetInt(&res, sc_sumsqr(slotRawInt(a), slotRawInt(b))); break;
						case opSqrSum : SetInt(&res, sc_sqrsum(slotRawInt(a), slotRawInt(b))); break;
						case opSqrDif : SetInt(&res, sc_sqrdif(slotRawInt(a), slotRawInt(b))); break;
						case opAbsDif :		SetInt(&res, sc_abs(slotRawInt(a) - slotRawInt(b))); break;
						case opThresh :	    SetInt(&res, sc_thresh(slotRawInt(a), slotRawInt(b))); break;
						case opAMClip :		SetInt(&res, sc_amclip(slotRawInt(a), slotRawInt(b))); break;
						case opScaleNeg :	SetInt(&res, sc_scaleneg(slotRawInt(a), slotRawInt(b))); break;
						case opClip2 :		SetInt(&res, sc_clip2(slotRawInt(a), -slotRawInt(b))); break;
						case opFold2 :		SetInt(&res, sc_fold2(slotRawInt(a), slotRawInt(b))); break;
						case opWrap2 :		SetInt(&res, sc_wrap2(slotRawInt(a), slotRawInt(b))); break;
						case opExcess :		SetInt(&res, sc_excess(slotRawInt(a), slotRawInt(b))); break;
						case opFirstArg : 	SetInt(&res, slotRawInt(a)); break;
						case opRandRange :	SetInt(&res, slotRawInt(b) > slotRawInt(a) ? slotRawInt(a) + g->rgen->irand(slotRawInt(b) - slotRawInt(a) + 1)
								   : slotRawInt(b) + g->rgen->irand(slotRawInt(a) - slotRawInt(b) + 1));
							break;
						case opExpRandRange :
							SetFloat(&res, g->rgen->exprandrng(slotRawInt(a), slotRawInt(b)));
							break;
						default : goto send_normal_2;
					}
					break;
				case tagChar :
				case tagPtr :
				case tagNil :
				case tagFalse :
				case tagTrue :
					goto send_normal_2;
				case tagSym :
					if (IS_BINARY_BOOL_OP(opcode)) {
						res = opcode == opNE ? o_true : o_false;
					} else {
						SetSymbol(&res, slotRawSymbol(b));
					}
					break;
				case tagObj :
					if (isKindOf(slotRawObject(b), class_signal)) {
						switch (opcode) {
							case opAdd : SetObject(&res, signal_add_xf(g, slotRawObject(b), slotRawInt(a))); break;
							case opSub : SetObject(&res, signal_sub_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opMul : SetObject(&res, signal_mul_xf(g, slotRawObject(b), slotRawInt(a))); break;
							case opIDiv : SetObject(&res, signal_div_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opFDiv : SetObject(&res, signal_div_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin : SetObject(&res, signal_min_xf(g, slotRawObject(b), slotRawInt(a))); break;
							case opMax : SetObject(&res, signal_max_xf(g, slotRawObject(b), slotRawInt(a))); break;
							case opRing1 : SetObject(&res, signal_ring1_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opRing2 : SetObject(&res, signal_ring2_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opRing3 : SetObject(&res, signal_ring3_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opRing4 : SetObject(&res, signal_ring4_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opDifSqr : SetObject(&res, signal_difsqr_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opAbsDif : SetObject(&res, signal_absdif_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opThresh : SetObject(&res, signal_thresh_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opAMClip : SetObject(&res, signal_amclip_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opClip2 : SetObject(&res, signal_clip2_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opFold2 : SetObject(&res, signal_fold2_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opWrap2 : SetObject(&res, signal_wrap2_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opExcess : SetObject(&res, signal_excess_fx(g, slotRawInt(a), slotRawObject(b))); break;
							case opFirstArg : SetObject(&res, slotRawObject(a)); break;
							default : goto send_normal_2;
						}
					} else {
						goto send_normal_2;
					}
					break;
				default :
					switch (opcode) {
						case opAdd :		SetFloat(&res, slotRawInt(a) + slotRawFloat(b)); break;
						case opSub :		SetFloat(&res, slotRawInt(a) - slotRawFloat(b)); break;
						case opMul :		SetFloat(&res, slotRawInt(a) * slotRawFloat(b)); break;
						case opIDiv :		SetInt(&res, (long)floor(slotRawInt(a) / slotRawFloat(b))); break;
						case opFDiv :		SetFloat(&res, slotRawInt(a) / slotRawFloat(b)); break;
						case opMod :		SetFloat(&res, sc_mod((double)slotRawInt(a), slotRawFloat(b))); break;
						case opEQ  :		res = BOOL(slotRawInt(a) == slotRawFloat(b)); break;
						case opNE  :		res = BOOL(slotRawInt(a) != slotRawFloat(b)); break;
						case opLT  :		res = BOOL(slotRawInt(a) <  slotRawFloat(b)); break;
						case opGT  :		res = BOOL(slotRawInt(a) >  slotRawFloat(b)); break;
						case opLE  :		res = BOOL(slotRawInt(a) <= slotRawFloat(b)); break;
						case opGE  :		res = BOOL(slotRawInt(a) >= slotRawFloat(b)); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin :		SetFloat(&res, sc_min((double)slotRawInt(a), slotRawFloat(b))); break;
						case opMax :		SetFloat(&res, sc_max((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRound :		SetFloat(&res, sc_round((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRoundUp :	SetFloat(&res, sc_roundUp((double)slotRawInt(a), slotRawFloat(b))); break;
						case opTrunc :		SetFloat(&res, sc_trunc((double)slotRawInt(a), slotRawFloat(b))); break;
						case opAtan2 :		SetFloat(&res, atan2(slotRawInt(a), slotRawFloat(b))); break;
						case opHypot :		SetFloat(&res, hypot(slotRawInt(a), slotRawFloat(b))); break;
						case opHypotx : 	SetFloat(&res, hypotx(slotRawInt(a), slotRawFloat(b))); break;
						case opPow   :		SetFloat(&res, pow((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRing1 :		SetFloat(&res, sc_ring1((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRing2 :		SetFloat(&res, sc_ring2((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRing3 :		SetFloat(&res, sc_ring3((double)slotRawInt(a), slotRawFloat(b))); break;
						case opRing4 :		SetFloat(&res, sc_ring4((double)slotRawInt(a), slotRawFloat(b))); break;
						case opDifSqr :		SetFloat(&res, sc_difsqr((double)slotRawInt(a), slotRawFloat(b))); break;
						case opSumSqr :		SetFloat(&res, sc_sumsqr((double)slotRawInt(a), slotRawFloat(b))); break;
						case opSqrSum :		SetFloat(&res, sc_sqrsum((double)slotRawInt(a), slotRawFloat(b))); break;
						case opSqrDif :		SetFloat(&res, sc_sqrdif((double)slotRawInt(a), slotRawFloat(b))); break;
						case opAbsDif :		SetFloat(&res, sc_abs(slotRawInt(a) - slotRawFloat(b))); break;
						case opThresh :		SetInt(&res, sc_thresh((double)slotRawInt(a), slotRawFloat(b))); break;
						case opAMClip :		SetFloat(&res, sc_amclip((double)slotRawInt(a), slotRawFloat(b))); break;
						case opScaleNeg : 	SetFloat(&res, sc_scaleneg((double)slotRawInt(a), slotRawFloat(b))); break;
						case opClip2 :		SetFloat(&res, sc_clip2((double)slotRawInt(a), -slotRawFloat(b))); break;
						case opFold2 :		SetFloat(&res, sc_fold2((double)slotRawInt(a), slotRawFloat(b))); break;
						case opWrap2 :		SetFloat(&res, sc_wrap2((double)slotRawInt(a), -slotRawFloat(b))); break;
						case opExcess :		SetFloat(&res, sc_excess((double)slotRawInt(a), slotRawFloat(b))); break;
						case opFirstArg :	SetInt(&res, slotRawInt(a)); break;
						case opRandRange :
							SetFloat(&res, slotRawInt(a) + g->rgen->frand() * (slotRawFloat(b) - slotRawInt(a)));
							break;
						case opExpRandRange :
							SetFloat(&res, g->rgen->exprandrng(slotRawInt(a), slotRawFloat(b)));
							break;
						default : goto send_normal_2;
					}
					break;
			}
		} break;
		case tagChar : {
			if (IsChar(b)) {
				switch (opcode) {
					case opEQ  : res = BOOL(slotRawChar(a) == slotRawChar(b)); break;
					case opNE  : res = BOOL(slotRawChar(a) != slotRawChar(b)); break;
					case opLT  : res = BOOL(slotRawChar(a) <  slotRawChar(b)); break;
					case opGT  : res = BOOL(slotRawChar(a) >  slotRawChar(b)); break;
					case opLE  : res = BOOL(slotRawChar(a) <= slotRawChar(b)); break;
					case opGE  : res = BOOL(slotRawChar(a) >= slotRawChar(b)); break;
					//case opIdentical : res = BOOL(slotRawChar(a) == slotRawChar(b)); break;
					//case opNotIdentical : res = BOOL(slotRawChar(a) != slotRawChar(b)); break;
					case opMin : SetInt(&res, sc_min(slotRawChar(a), slotRawChar(b))); break;
					case opMax : SetInt(&res, sc_max(slotRawChar(a), slotRawChar(b))); break;
					default : goto send_normal_2;
				}
			} else {
				goto send_normal_2;
			}
		} break;
		case tagPtr :
		case tagNil :
		case tagFalse :
		case tagTrue :
			goto send_normal_2;
		case tagSym :
			if (IsSym(b)) {
				switch (opcode) {
					case opEQ  : res = BOOL(slotRawSymbol(a) == slotRawSymbol(b)); break;
					case opNE  : res = BOOL(slotRawSymbol(a) != slotRawSymbol(b)); break;
					case opLT  : res = BOOL(strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) <  0); break;
					case opGT  : res = BOOL(strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) >  0); break;
					case opLE  : res = BOOL(strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) <= 0); break;
					case opGE  : res = BOOL(strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) >= 0); break;
					//default : leave first operand on stack
				}
			} else {
				if (IS_BINARY_BOOL_OP(opcode)) {
					res = opcode == opNE ? o_true : o_false;
				} else {
					SetSymbol(&res, slotRawSymbol(a));
				}
			}
			break;
		case tagObj : {
			if (isKindOf(slotRawObject(a), class_signal)) {
				switch (GetTag(b)) {
					case tagInt :
						switch (opcode) {
							case opAdd  : SetObject(&res, signal_add_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opSub  : SetObject(&res, signal_sub_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opMul  : SetObject(&res, signal_mul_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opIDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opFDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin  : SetObject(&res, signal_min_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opMax  : SetObject(&res, signal_max_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opFill : SetObject(&res, signal_fill(slotRawObject(a), slotRawInt(b))); break;
							case opRing1 : SetObject(&res, signal_ring1_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opRing2 : SetObject(&res, signal_ring2_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opRing3 : SetObject(&res, signal_ring3_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opRing4 : SetObject(&res, signal_ring4_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opDifSqr : SetObject(&res, signal_difsqr_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opAbsDif : SetObject(&res, signal_absdif_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opThresh : SetObject(&res, signal_thresh_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opAMClip : SetObject(&res, signal_amclip_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opClip2 : SetObject(&res, signal_clip2_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opFold2 : SetObject(&res, signal_fold2_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opWrap2 : SetObject(&res, signal_wrap2_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opExcess : SetObject(&res, signal_excess_xf(g, slotRawObject(a), slotRawInt(b))); break;
							case opFirstArg : SetObject(&res, slotRawObject(a)); break;
							default : goto send_normal_2;
						}
						break;
					case tagChar :
					case tagPtr :
					case tagNil :
					case tagFalse :
					case tagTrue :
						goto send_normal_2;
					case tagSym :
						if (IS_BINARY_BOOL_OP(opcode)) {
							res = opcode == opNE ? o_true : o_false;
						} else {
							SetSymbol(&res, slotRawSymbol(b));
						}
						break;
					case tagObj :
						if (isKindOf(slotRawObject(b), class_signal)) {
							switch (opcode) {
								case opAdd : SetObject(&res, signal_add_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opSub : SetObject(&res, signal_sub_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opMul : SetObject(&res, signal_mul_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opIDiv : SetObject(&res, signal_div_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opFDiv : SetObject(&res, signal_div_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opEQ  : res = BOOL(signal_equal_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opNE  : res = BOOL(!signal_equal_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opMin : SetObject(&res, signal_min_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opMax : SetObject(&res, signal_max_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opRing1 : SetObject(&res, signal_ring1_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opRing2 : SetObject(&res, signal_ring2_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opRing3 : SetObject(&res, signal_ring3_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opRing4 : SetObject(&res, signal_ring4_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opDifSqr : SetObject(&res, signal_difsqr_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opSumSqr : SetObject(&res, signal_sumsqr_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opSqrSum : SetObject(&res, signal_sqrsum_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opSqrDif : SetObject(&res, signal_sqrdif_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opAbsDif : SetObject(&res, signal_absdif_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opThresh : SetObject(&res, signal_thresh_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opAMClip : SetObject(&res, signal_amclip_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opScaleNeg : SetObject(&res, signal_scaleneg_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opClip2 : SetObject(&res, signal_clip2_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opFold2 : SetObject(&res, signal_fold2_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opWrap2 : SetObject(&res, signal_wrap2_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opExcess : SetObject(&res, signal_excess_xx(g, slotRawObject(a), slotRawObject(b))); break;
								case opFirstArg : SetObject(&res, slotRawObject(a)); break;
								default : goto send_normal_2;
							}
						} else goto send_normal_2;
						break;
					default : // double
						switch (opcode) {
							case opAdd  : SetObject(&res, signal_add_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opSub  : SetObject(&res, signal_sub_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opMul  : SetObject(&res, signal_mul_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opIDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opFDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin  : SetObject(&res, signal_min_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opMax  : SetObject(&res, signal_max_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opFill : SetObject(&res, signal_fill(slotRawObject(a), slotRawFloat(b))); break;
							case opRing1 : SetObject(&res, signal_ring1_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opRing2 : SetObject(&res, signal_ring2_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opRing3 : SetObject(&res, signal_ring3_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opRing4 : SetObject(&res, signal_ring4_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opDifSqr : SetObject(&res, signal_difsqr_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opAbsDif : SetObject(&res, signal_absdif_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opThresh : SetObject(&res, signal_thresh_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opAMClip : SetObject(&res, signal_amclip_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opClip2 : SetObject(&res, signal_clip2_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opFold2 : SetObject(&res, signal_fold2_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opWrap2 : SetObject(&res, signal_wrap2_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opExcess : SetObject(&res, signal_excess_xf(g, slotRawObject(a), slotRawFloat(b))); break;
							case opFirstArg : SetObject(&res, slotRawObject(a)); break;
							default : goto send_normal_2;
						}
						break;
				}
			} else {
				goto send_normal_2;
			}
		} break;
		default : { // double
			switch (GetTag(b)) {
				case tagInt :
					switch (opcode) {
						case opAdd :		SetFloat(&res, slotRawFloat(a) + slotRawInt(b)); break;
						case opSub :		SetFloat(&res, slotRawFloat(a) - slotRawInt(b)); break;
						case opMul :		SetFloat(&res, slotRawFloat(a) * slotRawInt(b)); break;
						case opIDiv :		SetInt(&res, (long)floor(slotRawFloat(a) / slotRawInt(b))); break;
						case opFDiv :		SetFloat(&res, slotRawFloat(a) / slotRawInt(b)); break;
						case opMod :		SetFloat(&res, sc_mod(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opEQ  :		res = BOOL(slotRawFloat(a) == slotRawInt(b)); break;
						case opNE  :		res = BOOL(slotRawFloat(a) != slotRawInt(b)); break;
						case opLT  :		res = BOOL(slotRawFloat(a) <  slotRawInt(b)); break;
						case opGT  :		res = BOOL(slotRawFloat(a) >  slotRawInt(b)); break;
						case opLE  :		res = BOOL(slotRawFloat(a) <= slotRawInt(b)); break;
						case opGE  :		res = BOOL(slotRawFloat(a) >= slotRawInt(b)); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin :		SetFloat(&res, sc_min(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opMax :		SetFloat(&res, sc_max(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRound :		SetFloat(&res, sc_round(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRoundUp :	SetFloat(&res, sc_roundUp(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opTrunc :		SetFloat(&res, sc_trunc(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opAtan2 :		SetFloat(&res, atan2(slotRawFloat(a), slotRawInt(b))); break;
						case opHypot :		SetFloat(&res, hypot(slotRawFloat(a), slotRawInt(b))); break;
						case opHypotx :		SetFloat(&res, hypotx(slotRawFloat(a), slotRawInt(b))); break;
						case opPow   :		SetFloat(&res, pow(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRing1 :		SetFloat(&res, sc_ring1(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRing2 :		SetFloat(&res, sc_ring2(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRing3 :		SetFloat(&res, sc_ring3(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opRing4 :		SetFloat(&res, sc_ring4(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opDifSqr :		SetFloat(&res, sc_difsqr(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opSumSqr :		SetFloat(&res, sc_sumsqr(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opSqrSum :		SetFloat(&res, sc_sqrsum(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opSqrDif :		SetFloat(&res, sc_sqrdif(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opAbsDif :		SetFloat(&res, sc_abs(slotRawFloat(a) - slotRawInt(b))); break;
						case opThresh :		SetFloat(&res, sc_thresh(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opAMClip :		SetFloat(&res, sc_amclip(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opScaleNeg :	SetFloat(&res, sc_scaleneg(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opClip2 :		SetFloat(&res, sc_clip2(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opFold2 :		SetFloat(&res, sc_fold2(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opWrap2 :		SetFloat(&res, sc_wrap2(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opExcess :		SetFloat(&res, sc_excess(slotRawFloat(a), (double)slotRawInt(b))); break;
						case opFirstArg :	SetFloat(&res, slotRawFloat(a)); break;
						case opRandRange :
							SetFloat(&res, slotRawFloat(a) + g->rgen->frand() * (slotRawInt(b) - slotRawFloat(a)));
							break;
						case opExpRandRange :
							SetFloat(&res, g->rgen->exprandrng(slotRawFloat(a), slotRawInt(b)));
							break;
						default : goto send_normal_2;
					}
					break;
				case tagChar :
				case tagPtr :
				case tagNil :
				case tagFalse :
				case tagTrue :
					goto send_normal_2;
				case tagSym :
					if (IS_BINARY_BOOL_OP(opcode)) {
						res = opcode == opNE ? o_true : o_false;
					} else {
						SetSymbol(&res, slotRawSymbol(b));
					}
					break;
				case tagObj :
					if (isKindOf(slotRawObject(b), class_signal)) {
						switch (opcode) {
							case opAdd : SetObject(&res, signal_add_xf(g, slotRawObject(b), slotRawFloat(a))); break;
							case opSub : SetObject(&res, signal_sub_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opMul : SetObject(&res, signal_mul_xf(g, slotRawObject(b), slotRawFloat(a))); break;
							case opIDiv : SetObject(&res, signal_div_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opFDiv : SetObject(&res, signal_div_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin : SetObject(&res, signal_min_xf(g, slotRawObject(b), slotRawFloat(a))); break;
							case opMax : SetObject(&res, signal_max_xf(g, slotRawObject(b), slotRawFloat(a))); break;
							case opRing1 : SetObject(&res, signal_ring1_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opRing2 : SetObject(&res, signal_ring2_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opRing3 : SetObject(&res, signal_ring3_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opRing4 : SetObject(&res, signal_ring4_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opDifSqr : SetObject(&res, signal_difsqr_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opAbsDif : SetObject(&res, signal_absdif_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opThresh : SetObject(&res, signal_thresh_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opAMClip : SetObject(&res, signal_amclip_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opClip2 : SetObject(&res, signal_clip2_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opFold2 : SetObject(&res, signal_fold2_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opWrap2 : SetObject(&res, signal_wrap2_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opExcess : SetObject(&res, signal_excess_fx(g, slotRawFloat(a), slotRawObject(b))); break;
							case opFirstArg : SetObject(&res, slotRawObject(a)); break;
							default : goto send_normal_2;
						}
					} else goto send_normal_2;
					break;
				default : // double
					switch (opcode) {
						case opAdd :		SetFloat(&res, slotRawFloat(a) + slotRawFloat(b)); break;
						case opSub :		SetFloat(&res, slotRawFloat(a) - slotRawFloat(b)); break;
						case opMul :		SetFloat(&res, slotRawFloat(a) * slotRawFloat(b)); break;
						case opIDiv :		SetInt(&res, (long)floor(slotRawFloat(a) / slotRawFloat(b))); break;
						case opFDiv :		SetFloat(&res, slotRawFloat(a) / slotRawFloat(b)); break;
						case opMod :		SetFloat(&res, sc_mod(slotRawFloat(a), slotRawFloat(b))); break;
						case opEQ  :		res = BOOL(slotRawFloat(a) == slotRawFloat(b)); break;
						case opNE  :		res = BOOL(slotRawFloat(a) != slotRawFloat(b)); break;
						case opLT  :		res = BOOL(slotRawFloat(a) <  slotRawFloat(b)); break;
						case opGT  :		res = BOOL(slotRawFloat(a) >  slotRawFloat(b)); break;
						case opLE  :		res = BOOL(slotRawFloat(a) <= slotRawFloat(b)); break;
						case opGE  :		res = BOOL(slotRawFloat(a) >= slotRawFloat(b)); break;
						//case opIdentical  : res = BOOL(slotRawFloat(a) == slotRawFloat(b)); break;
						//case opNotIdentical  : res = BOOL(slotRawFloat(a) != slotRawFloat(b)); break;
						case opMin : 		SetFloat(&res, sc_min(slotRawFloat(a), slotRawFloat(b))); break;
						case opMax :		SetFloat(&res, sc_max(slotRawFloat(a), slotRawFloat(b))); break;
						case opRound :		SetFloat(&res, sc_round(slotRawFloat(a), slotRawFloat(b))); break;
						case opRoundUp :	SetFloat(&res, sc_roundUp(slotRawFloat(a), slotRawFloat(b))); break;
						case opTrunc :		SetFloat(&res, sc_trunc(slotRawFloat(a), slotRawFloat(b))); break;
						case opAtan2 :		SetFloat(&res, atan2(slotRawFloat(a), slotRawFloat(b))); break;
						case opHypot :		SetFloat(&res, hypot(slotRawFloat(a), slotRawFloat(b))); break;
						case opHypotx :		SetFloat(&res, hypotx(slotRawFloat(a), slotRawFloat(b))); break;
						case opPow   :		SetFloat(&res, pow(slotRawFloat(a), slotRawFloat(b))); break;
						case opRing1 :		SetFloat(&res, sc_ring1(slotRawFloat(a), slotRawFloat(b))); break;
						case opRing2 :		SetFloat(&res, sc_ring2(slotRawFloat(a), slotRawFloat(b))); break;
						case opRing3 :		SetFloat(&res, sc_ring3(slotRawFloat(a), slotRawFloat(b))); break;
						case opRing4 :		SetFloat(&res, sc_ring4(slotRawFloat(a), slotRawFloat(b))); break;
						case opDifSqr :		SetFloat(&res, sc_difsqr(slotRawFloat(a), slotRawFloat(b))); break;
						case opSumSqr :		SetFloat(&res, sc_sumsqr(slotRawFloat(a), slotRawFloat(b))); break;
						case opSqrSum :		SetFloat(&res, sc_sqrsum(slotRawFloat(a), slotRawFloat(b))); break;
						case opSqrDif :		SetFloat(&res, sc_sqrdif(slotRawFloat(a), slotRawFloat(b))); break;
						case opAbsDif :		SetFloat(&res, sc_abs(slotRawFloat(a) - slotRawFloat(b))); break;
						case opThresh :		SetFloat(&res, sc_thresh(slotRawFloat(a), slotRawFloat(b))); break;
						case opAMClip :		SetFloat(&res, sc_amclip(slotRawFloat(a), slotRawFloat(b))); break;
						case opScaleNeg :	SetFloat(&res, sc_scaleneg(slotRawFloat(a), slotRawFloat(b))); break;
						case opClip2 :		SetFloat(&res, sc_clip2(slotRawFloat(a), slotRawFloat(b))); break;
						case opFold2 :		SetFloat(&res, sc_fold2(slotRawFloat(a), slotRawFloat(b))); break;
						case opWrap2 :		SetFloat(&res, sc_wrap2(slotRawFloat(a), slotRawFloat(b))); break;
						case opExcess :		SetFloat(&res, sc_excess(slotRawFloat(a), slotRawFloat(b))); break;
						case opFirstArg :	SetFloat(&res, slotRawFloat(a)); break;
						case opRandRange :
							SetFloat(&res, slotRawFloat(a) + g->rgen->frand() * (slotRawFloat(b) - slotRawFloat(a)));
							break;
						case opExpRandRange :
							SetFloat(&res, g->rgen->exprandrng(slotRawFloat(a), slotRawFloat(b)));
							break;
						default : goto send_normal_2;
					}
					break;
			}
		} break;
	}
	g->sp -= numArgsPushed - 1; // drop
	slotCopy(&g->sp[0], &res);
	g->numpop = 0;
#if TAILCALLOPTIMIZE
	g->tailCall = 0;
#endif
	return errNone;

send_normal_2:
	if (isPrimitive)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack

	msg = gSpecialBinarySelectors[opcode];
	sendMessage(g, msg, numArgsPushed);
	return errNone;

}

