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
	PyrSlot *a, res;
	PyrSymbol *msg;
	int opcode = g->primitiveIndex;

	a = g->sp;

	switch (GetTag(a)) {
		case tagInt :
			switch (opcode) {
				case opNeg : SetInt(&res, -slotRawInt(a)); break;
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opBitNot : SetInt(&res, ~slotRawInt(a)); break;
				case opAbs : SetInt(&res, slotRawInt(a) < 0 ? -slotRawInt(a) : slotRawInt(a)); break;
				case opAsFloat : res.uf = (double)slotRawInt(a); break;
				case opAsInt : SetInt(&res, (int)slotRawInt(a)); break;
				case opCeil : SetInt(&res, slotRawInt(a)); break;
				case opFloor : SetInt(&res, slotRawInt(a)); break;
				case opFrac : SetInt(&res, 0); break;
				case opSign : SetInt(&res, slotRawInt(a) > 0 ? 1 : (slotRawInt(a) == 0 ? 0 : -1)); break;
				case opSquared : SetInt(&res, slotRawInt(a) * slotRawInt(a)); break;
				case opCubed : SetInt(&res, slotRawInt(a) * slotRawInt(a) * slotRawInt(a)); break;
				case opSqrt : res.uf = sqrt((double)slotRawInt(a)); break;
				case opExp : res.uf = exp((double)slotRawInt(a)); break;
				case opRecip : res.uf = 1. / slotRawInt(a); break;
				case opMIDICPS : res.uf = sc_midicps((double)slotRawInt(a)); break;
				case opCPSMIDI : res.uf = sc_cpsmidi((double)slotRawInt(a)); break;
				case opMIDIRatio : res.uf = sc_midiratio((double)slotRawInt(a)); break;
				case opRatioMIDI : res.uf = sc_ratiomidi((double)slotRawInt(a)); break;
				case opAmpDb : res.uf = sc_ampdb((double)slotRawInt(a)); break;
				case opDbAmp : res.uf = sc_dbamp((double)slotRawInt(a)); break;
				case opOctCPS : res.uf = sc_octcps((double)slotRawInt(a)); break;
				case opCPSOct : res.uf = sc_cpsoct((double)slotRawInt(a)); break;
				case opLog : res.uf = log((double)slotRawInt(a)); break;
				case opLog2 : res.uf = log2((double)slotRawInt(a)); break;
				case opLog10 : res.uf = log10((double)slotRawInt(a)); break;
				case opSin : res.uf = sin((double)slotRawInt(a)); break;
				case opCos : res.uf = cos((double)slotRawInt(a)); break;
				case opTan : res.uf = tan((double)slotRawInt(a)); break;
				case opArcSin : res.uf = asin((double)slotRawInt(a)); break;
				case opArcCos : res.uf = acos((double)slotRawInt(a)); break;
				case opArcTan : res.uf = atan((double)slotRawInt(a)); break;
				case opSinH : res.uf = sinh((double)slotRawInt(a)); break;
				case opCosH : res.uf = cosh((double)slotRawInt(a)); break;
				case opTanH : res.uf = tanh((double)slotRawInt(a)); break;
				case opRand : SetInt(&res, g->rgen->irand(slotRawInt(a))); break;
				case opRand2 : SetInt(&res, g->rgen->irand2(slotRawInt(a))); break;
				case opLinRand : SetInt(&res, g->rgen->ilinrand(slotRawInt(a))); break;
				case opBiLinRand : SetInt(&res, g->rgen->ibilinrand(slotRawInt(a))); break;

//				case opExpRand : res.uf = g->rgen->exprand(slotRawInt(a)); break;
//				case opBiExpRand : res.uf = g->rgen->biexprand(slotRawInt(a)); break;
				case opSum3Rand : res.uf = g->rgen->sum3rand(slotRawInt(a)); break;

//				case opGammaRand : res.uf = g->rgen->gammarand(slotRawInt(a)); break;
//				case opGaussRand : res.uf = g->rgen->gaussrand(slotRawInt(a)); break;
//				case opPoiRand : res.uf = g->rgen->poirand(slotRawInt(a)); break;

				case opDistort : res.uf = sc_distort((double)slotRawInt(a)); break;
				case opSoftClip : res.uf = sc_softclip((double)slotRawInt(a)); break;
				case opCoin : res = BOOL(slotRawInt(a)); break;
				case opRectWindow : res.uf = sc_rectwindow((double)slotRawInt(a)); break;
				case opHanWindow : res.uf = sc_hanwindow((double)slotRawInt(a)); break;
				case opWelchWindow : res.uf = sc_welwindow((double)slotRawInt(a)); break;
				case opTriWindow : res.uf = sc_triwindow((double)slotRawInt(a)); break;
				case opSCurve : res.uf = sc_scurve((double)slotRawInt(a)); break;
				case opRamp : res.uf = sc_ramp((double)slotRawInt(a)); break;
				default : goto send_normal_1;
			}
			break;
		case tagChar :
			switch (opcode) {
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opAsInt : SetTagRaw(&res, tagInt); break;
				case opDigitValue :
					if (slotRawInt(a) >= '0' && slotRawInt(a) <= '9') SetRaw(&res, slotRawInt(a) - '0');
					else if (slotRawInt(a) >= 'A' && slotRawInt(a) <= 'Z') SetRaw(&res, slotRawInt(a) - 'A');
					else if (slotRawInt(a) >= 'a' && slotRawInt(a) <= 'z') SetRaw(&res, slotRawInt(a) - 'a');
					else SetRaw(&res, 0);
					SetTagRaw(&res, tagInt);
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
				case opNot : SetTagRaw(a, tagTrue); break;
				case opIsNil : /*SetFalse(a);*/ break;
				case opNotNil : SetTrue(a); break;
				default : goto send_normal_1;
			}
			break;
		case tagTrue :
			switch (opcode) {
				case opNot : SetTagRaw(a, tagFalse); break;
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
				default : SetSymbol(&res, slotRawSymbol(a));
			}
			break;
		case tagObj :
			if (isKindOf(slotRawObject(a), class_signal)) {
				switch (opcode) {
					case opNeg : SetObject(&res, signal_invert(g, slotRawObject(a))); break;
					case opIsNil : SetFalse(a); break;
					case opNotNil : SetTrue(a); break;
					case opAbs : SetObject(&res, signal_abs(g, slotRawObject(a))); break;
					case opSign : SetObject(&res, signal_sign(g, slotRawObject(a))); break;
					case opSquared : SetObject(&res, signal_squared(g, slotRawObject(a))); break;
					case opCubed : SetObject(&res, signal_cubed(g, slotRawObject(a))); break;
					case opSqrt : SetObject(&res, signal_sqrt(g, slotRawObject(a))); break;
					case opExp : SetObject(&res, signal_exp(g, slotRawObject(a))); break;
					case opRecip : SetObject(&res, signal_recip(g, slotRawObject(a))); break;
					case opLog : SetObject(&res, signal_log(g, slotRawObject(a))); break;
					case opLog2 : SetObject(&res, signal_log2(g, slotRawObject(a))); break;
					case opLog10 : SetObject(&res, signal_log10(g, slotRawObject(a))); break;
					case opSin : SetObject(&res, signal_sin(g, slotRawObject(a))); break;
					//case opSin : SetObject(&res, signal_fsin(g, slotRawObject(a))); break;
					case opCos : SetObject(&res, signal_cos(g, slotRawObject(a))); break;
					case opTan : SetObject(&res, signal_tan(g, slotRawObject(a))); break;
					case opArcSin : SetObject(&res, signal_asin(g, slotRawObject(a))); break;
					case opArcCos : SetObject(&res, signal_acos(g, slotRawObject(a))); break;
					case opArcTan : SetObject(&res, signal_atan(g, slotRawObject(a))); break;
					case opSinH : SetObject(&res, signal_sinh(g, slotRawObject(a))); break;
					case opCosH : SetObject(&res, signal_cosh(g, slotRawObject(a))); break;
					case opTanH : SetObject(&res, signal_tanh(g, slotRawObject(a))); break;
					case opDistort : SetObject(&res, signal_distort(g, slotRawObject(a))); break;
					case opSoftClip : SetObject(&res, signal_softclip(g, slotRawObject(a))); break;
					default : goto send_normal_1;
				}
			} else {
				goto send_normal_1;
			}
			break;
		default : // double
			switch (opcode) {
				case opNeg : res.uf = -a->uf; break;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opBitNot : res.uf = ~(int)a->uf; break;
				case opAbs : res.uf = a->uf < 0. ? -a->uf : a->uf; break;
				case opAsFloat : res.uf = (double)a->uf; break;
				case opAsInt : SetInt(&res, (int)a->uf); break;
				case opCeil : res.uf = ceil(a->uf); break;
				case opFloor : res.uf = floor(a->uf); break;
				case opFrac : res.uf = a->uf - floor(a->uf); break;
				case opSign : res.uf = a->uf > 0. ? 1 : (a->uf == 0 ? 0 : -1); break;
				case opSquared : res.uf = a->uf * a->uf; break;
				case opCubed : res.uf = a->uf * a->uf * a->uf; break;
				case opSqrt : res.uf = sqrt(a->uf); break;
				case opExp : res.uf = exp(a->uf); break;
				case opRecip : res.uf = 1./a->uf; break;
				case opMIDICPS : res.uf = sc_midicps(a->uf); break;
				case opCPSMIDI : res.uf = sc_cpsmidi(a->uf); break;
				case opMIDIRatio : res.uf = sc_midiratio((double)a->uf); break;
				case opRatioMIDI : res.uf = sc_ratiomidi((double)a->uf); break;
				case opAmpDb : res.uf = sc_ampdb(a->uf); break;
				case opDbAmp : res.uf = sc_dbamp(a->uf); break;
				case opOctCPS : res.uf = sc_octcps(a->uf); break;
				case opCPSOct : res.uf = sc_cpsoct(a->uf); break;
				case opLog : res.uf = log(a->uf); break;
				case opLog2 : res.uf = log2(a->uf); break;
				case opLog10 : res.uf = log10(a->uf); break;
				case opSin : res.uf = sin(a->uf); break;
				case opCos : res.uf = cos(a->uf); break;
				case opTan : res.uf = tan(a->uf); break;
				case opArcSin : res.uf = asin(a->uf); break;
				case opArcCos : res.uf = acos(a->uf); break;
				case opArcTan : res.uf = atan(a->uf); break;
				case opSinH : res.uf = sinh(a->uf); break;
				case opCosH : res.uf = cosh(a->uf); break;
				case opTanH : res.uf = tanh(a->uf); break;
				case opRand : res.uf = g->rgen->frand() * a->uf; break;
				case opRand2 : res.uf = g->rgen->frand2() * a->uf; break;
				case opLinRand   : res.uf = g->rgen->linrand(a->uf); break;
				case opBiLinRand : res.uf = g->rgen->bilinrand(a->uf); break;

//				case opExpRand   : res.uf = g->rgen->exprand(a->uf); break;
//				case opBiExpRand : res.uf = g->rgen->biexprand(a->uf); break;
				case opSum3Rand : res.uf = g->rgen->sum3rand(a->uf); break;

//				case opGammaRand : res.uf = g->rgen->gammarand(a->uf); break;
//				case opGaussRand : res.uf = g->rgen->gaussrand(a->uf); break;
//				case opPoiRand   : res.uf = g->rgen->poirand(a->uf); break;

				case opDistort :
					res.uf = sc_distort(a->uf);
					break;
				case opSoftClip :
					res.uf = sc_softclip(a->uf);
					break;
				case opCoin : res = BOOL(g->rgen->frand() < a->uf); break;
				case opRectWindow : res.uf = sc_rectwindow(a->uf); break;
				case opHanWindow : res.uf = sc_hanwindow(a->uf); break;
				case opWelchWindow : res.uf = sc_welwindow(a->uf); break;
				case opTriWindow : res.uf = sc_triwindow(a->uf); break;
				case opSCurve : res.uf = sc_scurve(a->uf); break;
				case opRamp : res.uf = sc_ramp(a->uf); break;
				default : goto send_normal_1;
			}
			break;
	}

	slotCopy(&g->sp[0], &res);
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
						case opAdd : SetInt(&res, slotRawInt(a) + slotRawInt(b)); break;
						case opSub : SetInt(&res, slotRawInt(a) - slotRawInt(b)); break;
						case opMul : SetInt(&res, slotRawInt(a) * slotRawInt(b)); break;
						case opIDiv : SetInt(&res, sc_div(slotRawInt(a), slotRawInt(b))); break;
						case opFDiv : res.uf = (double)slotRawInt(a) / (double)slotRawInt(b); break;
						case opMod : SetInt(&res, sc_mod((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opEQ  : res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
						case opNE  : res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
						case opLT  : res = BOOL(slotRawInt(a) <  slotRawInt(b)); break;
						case opGT  : res = BOOL(slotRawInt(a) >  slotRawInt(b)); break;
						case opLE  : res = BOOL(slotRawInt(a) <= slotRawInt(b)); break;
						case opGE  : res = BOOL(slotRawInt(a) >= slotRawInt(b)); break;
						//case opIdentical : res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
						//case opNotIdentical : res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
						case opMin : SetInt(&res, slotRawInt(a) < slotRawInt(b) ? slotRawInt(a) : slotRawInt(b)); break;
						case opMax : SetInt(&res, slotRawInt(a) > slotRawInt(b) ? slotRawInt(a) : slotRawInt(b)); break;
						case opBitAnd : SetInt(&res, slotRawInt(a) & slotRawInt(b)); break;
						case opBitOr  : SetInt(&res, slotRawInt(a) | slotRawInt(b)); break;
						case opBitXor : SetInt(&res, slotRawInt(a) ^ slotRawInt(b)); break;
						case opLCM   : SetInt(&res, sc_lcm(slotRawInt(a), slotRawInt(b))); break;
						case opGCD   : SetInt(&res, sc_gcd(slotRawInt(a), slotRawInt(b))); break;
						case opRound : SetInt(&res, sc_round((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opRoundUp : SetInt(&res, sc_roundUp((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opTrunc : SetInt(&res, sc_trunc((int)slotRawInt(a), (int)slotRawInt(b))); break;
						case opAtan2 : res.uf = atan2((double)slotRawInt(a), (double)slotRawInt(b)); break;
						case opHypot : res.uf = hypot((double)slotRawInt(a), (double)slotRawInt(b)); break;
						case opHypotx : res.uf = hypotx((double)slotRawInt(a), (double)slotRawInt(b)); break;
						case opPow   : res.uf = pow((double)slotRawInt(a), (double)slotRawInt(b)); break;
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
						case opRing1 : SetInt(&res, slotRawInt(a) * slotRawInt(b) + slotRawInt(a)); break;
						case opRing2 : SetInt(&res, slotRawInt(a) * slotRawInt(b) + slotRawInt(a) + slotRawInt(b)); break;
						case opRing3 : SetInt(&res, slotRawInt(a) * slotRawInt(a) * slotRawInt(b)); break;
						case opRing4 : SetInt(&res, slotRawInt(a) * slotRawInt(a) * slotRawInt(b)
											  - slotRawInt(a) * slotRawInt(b) * slotRawInt(b)); break;
						case opDifSqr : SetInt(&res, slotRawInt(a) * slotRawInt(a) - slotRawInt(b) * slotRawInt(b)); break;
						case opSumSqr : SetInt(&res, slotRawInt(a) * slotRawInt(a) + slotRawInt(b) * slotRawInt(b)); break;
						case opSqrSum : {
							long z;
							z = slotRawInt(a) + slotRawInt(b);
							SetInt(&res, z*z);
						} break;
						case opSqrDif : {
							long z;
							z = slotRawInt(a) - slotRawInt(b);
							SetInt(&res, z*z);
						} break;
						case opAbsDif : SetInt(&res, abs(slotRawInt(a) - slotRawInt(b))); break;
						case opThresh : SetInt(&res, slotRawInt(a)<slotRawInt(b) ? 0 : slotRawInt(a)); break;
						case opAMClip : SetInt(&res, slotRawInt(b)<0 ? 0 : slotRawInt(a)*slotRawInt(b)); break;
						case opScaleNeg : SetInt(&res, slotRawInt(a)<0 ? slotRawInt(a)*slotRawInt(b) : slotRawInt(a)); break;
						case opClip2 :
							SetInt(&res, slotRawInt(a) < -slotRawInt(b) ? -slotRawInt(b) : (slotRawInt(a) > slotRawInt(b) ? slotRawInt(b) : slotRawInt(a)));
							break;
						case opFold2 :
							SetInt(&res, sc_fold2(slotRawInt(a), slotRawInt(b)));
							break;
						case opWrap2 :
							SetInt(&res, sc_mod((int)(slotRawInt(a) + slotRawInt(b)), (int)(2 * slotRawInt(b))) - slotRawInt(b));
							break;
						case opExcess :
							SetInt(&res, slotRawInt(a) - (slotRawInt(a) < -slotRawInt(b) ? -slotRawInt(b) : (slotRawInt(a) > slotRawInt(b) ? slotRawInt(b) : slotRawInt(a))));
							break;
						case opFirstArg : SetInt(&res, slotRawInt(a)); break;
						case opRandRange :
							SetInt(&res, slotRawInt(b) > slotRawInt(a) ? slotRawInt(a) + g->rgen->irand(slotRawInt(b) - slotRawInt(a) + 1)
								   : slotRawInt(b) + g->rgen->irand(slotRawInt(a) - slotRawInt(b) + 1));
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(slotRawInt(a), slotRawInt(b));
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
						case opAdd : res.uf = slotRawInt(a) + b->uf; break;
						case opSub : res.uf = slotRawInt(a) - b->uf; break;
						case opMul : res.uf = slotRawInt(a) * b->uf; break;
						case opIDiv : SetInt(&res, (long)floor(slotRawInt(a) / b->uf)); break;
						case opFDiv : res.uf = slotRawInt(a) / b->uf; break;
						case opMod : res.uf = sc_mod((double)slotRawInt(a), b->uf); break;
						case opEQ  : res = BOOL(slotRawInt(a) == b->uf); break;
						case opNE  : res = BOOL(slotRawInt(a) != b->uf); break;
						case opLT  : res = BOOL(slotRawInt(a) <  b->uf); break;
						case opGT  : res = BOOL(slotRawInt(a) >  b->uf); break;
						case opLE  : res = BOOL(slotRawInt(a) <= b->uf); break;
						case opGE  : res = BOOL(slotRawInt(a) >= b->uf); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin : res.uf = slotRawInt(a) < b->uf ? slotRawInt(a) : b->uf; break;
						case opMax : res.uf = slotRawInt(a) > b->uf ? slotRawInt(a) : b->uf; break;
						case opRound : res.uf = sc_round((double)slotRawInt(a), b->uf); break;
						case opRoundUp : res.uf = sc_roundUp((double)slotRawInt(a), b->uf); break;
						case opTrunc : res.uf = sc_trunc((double)slotRawInt(a), b->uf); break;
						case opAtan2 : res.uf = atan2(slotRawInt(a), b->uf); break;
						case opHypot : res.uf = hypot(slotRawInt(a), b->uf); break;
						case opHypotx : res.uf = hypotx(slotRawInt(a), b->uf); break;
						case opPow   : res.uf = pow((double)slotRawInt(a), b->uf); break;
						case opRing1 : res.uf = slotRawInt(a) * b->uf + slotRawInt(a); break;
						case opRing2 : res.uf = slotRawInt(a) * b->uf + slotRawInt(a) + b->uf; break;
						case opRing3 : res.uf = slotRawInt(a) * slotRawInt(a) * b->uf; break;
						case opRing4 : res.uf = slotRawInt(a) * slotRawInt(a) * b->uf
											- slotRawInt(a) * b->uf * b->uf; break;
						case opDifSqr : res.uf = slotRawInt(a) * slotRawInt(a) - b->uf * b->uf; break;
						case opSumSqr : res.uf = slotRawInt(a) * slotRawInt(a) + b->uf * b->uf; break;
						case opSqrSum : {
							double z;
							z = slotRawInt(a) + b->uf;
							res.uf = z*z;
						} break;
						case opSqrDif : {
							double z;
							z = slotRawInt(a) - b->uf;
							res.uf = z*z;
						} break;
						case opAbsDif : res.uf = fabs(slotRawInt(a) - b->uf); break;
						case opThresh : SetInt(&res, slotRawInt(a)<b->uf ? 0 : slotRawInt(a)); break;
						case opAMClip : res.uf = b->uf<0 ? 0 : slotRawInt(a)*b->uf; break;
						case opScaleNeg : res.uf = slotRawInt(a)<0 ? slotRawInt(a)*b->uf : slotRawInt(a); break;
						case opClip2 :
							res.uf = slotRawInt(a) < -b->uf ? -b->uf : (slotRawInt(a) > b->uf ? b->uf : slotRawInt(a));
							break;
						case opFold2 :
							res.uf = sc_fold2((double)slotRawInt(a), b->uf);
							break;
						case opWrap2 :
							res.uf = sc_wrap((double)slotRawInt(a), -b->uf, -b->uf);
							break;
						case opExcess :
							res.uf = slotRawInt(a) - (slotRawInt(a) < -b->uf ? -b->uf : (slotRawInt(a) > b->uf ? b->uf : slotRawInt(a)));
							break;
						case opFirstArg : SetInt(&res, slotRawInt(a)); break;
						case opRandRange :
							res.uf = slotRawInt(a) + g->rgen->frand() * (b->uf - slotRawInt(a));
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(slotRawInt(a), b->uf);
							break;
						default : goto send_normal_2;
					}
					break;
			}
		} break;
		case tagChar : {
			if (IsChar(b)) {
				switch (opcode) {
					case opEQ  : res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
					case opNE  : res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
					case opLT  : res = BOOL(slotRawInt(a) <  slotRawInt(b)); break;
					case opGT  : res = BOOL(slotRawInt(a) >  slotRawInt(b)); break;
					case opLE  : res = BOOL(slotRawInt(a) <= slotRawInt(b)); break;
					case opGE  : res = BOOL(slotRawInt(a) >= slotRawInt(b)); break;
					//case opIdentical : res = BOOL(slotRawInt(a) == slotRawInt(b)); break;
					//case opNotIdentical : res = BOOL(slotRawInt(a) != slotRawInt(b)); break;
					case opMin : SetInt(&res, slotRawInt(a) < slotRawInt(b) ? slotRawInt(a) : slotRawInt(b)); break;
					case opMax : SetInt(&res, slotRawInt(a) > slotRawInt(b) ? slotRawInt(a) : slotRawInt(b)); break;
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
							case opAdd  : SetObject(&res, signal_add_xf(g, slotRawObject(a), b->uf)); break;
							case opSub  : SetObject(&res, signal_sub_xf(g, slotRawObject(a), b->uf)); break;
							case opMul  : SetObject(&res, signal_mul_xf(g, slotRawObject(a), b->uf)); break;
							case opIDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), b->uf)); break;
							case opFDiv  : SetObject(&res, signal_div_xf(g, slotRawObject(a), b->uf)); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin  : SetObject(&res, signal_min_xf(g, slotRawObject(a), b->uf)); break;
							case opMax  : SetObject(&res, signal_max_xf(g, slotRawObject(a), b->uf)); break;
							case opFill : SetObject(&res, signal_fill(slotRawObject(a), b->uf)); break;
							case opRing1 : SetObject(&res, signal_ring1_xf(g, slotRawObject(a), b->uf)); break;
							case opRing2 : SetObject(&res, signal_ring2_xf(g, slotRawObject(a), b->uf)); break;
							case opRing3 : SetObject(&res, signal_ring3_xf(g, slotRawObject(a), b->uf)); break;
							case opRing4 : SetObject(&res, signal_ring4_xf(g, slotRawObject(a), b->uf)); break;
							case opDifSqr : SetObject(&res, signal_difsqr_xf(g, slotRawObject(a), b->uf)); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_xf(g, slotRawObject(a), b->uf)); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_xf(g, slotRawObject(a), b->uf)); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_xf(g, slotRawObject(a), b->uf)); break;
							case opAbsDif : SetObject(&res, signal_absdif_xf(g, slotRawObject(a), b->uf)); break;
							case opThresh : SetObject(&res, signal_thresh_xf(g, slotRawObject(a), b->uf)); break;
							case opAMClip : SetObject(&res, signal_amclip_xf(g, slotRawObject(a), b->uf)); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_xf(g, slotRawObject(a), b->uf)); break;
							case opClip2 : SetObject(&res, signal_clip2_xf(g, slotRawObject(a), b->uf)); break;
							case opFold2 : SetObject(&res, signal_fold2_xf(g, slotRawObject(a), b->uf)); break;
							case opWrap2 : SetObject(&res, signal_wrap2_xf(g, slotRawObject(a), b->uf)); break;
							case opExcess : SetObject(&res, signal_excess_xf(g, slotRawObject(a), b->uf)); break;
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
						case opAdd : res.uf = a->uf + slotRawInt(b); break;
						case opSub : res.uf = a->uf - slotRawInt(b); break;
						case opMul : res.uf = a->uf * slotRawInt(b); break;
						case opIDiv : SetInt(&res, (long)floor(a->uf / slotRawInt(b))); break;
						case opFDiv : res.uf = a->uf / slotRawInt(b); break;
						case opMod : res.uf = sc_mod(a->uf, (double)slotRawInt(b)); break;
						case opEQ  : res = BOOL(a->uf == slotRawInt(b)); break;
						case opNE  : res = BOOL(a->uf != slotRawInt(b)); break;
						case opLT  : res = BOOL(a->uf <  slotRawInt(b)); break;
						case opGT  : res = BOOL(a->uf >  slotRawInt(b)); break;
						case opLE  : res = BOOL(a->uf <= slotRawInt(b)); break;
						case opGE  : res = BOOL(a->uf >= slotRawInt(b)); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin : res.uf = a->uf < slotRawInt(b) ? a->uf : slotRawInt(b); break;
						case opMax : res.uf = a->uf > slotRawInt(b) ? a->uf : slotRawInt(b); break;
						case opRound : res.uf = sc_round(a->uf, (double)slotRawInt(b)); break;
						case opRoundUp : res.uf = sc_roundUp(a->uf, (double)slotRawInt(b)); break;
						case opTrunc : res.uf = sc_trunc(a->uf, (double)slotRawInt(b)); break;
						case opAtan2 : res.uf = atan2(a->uf, slotRawInt(b)); break;
						case opHypot : res.uf = hypot(a->uf, slotRawInt(b)); break;
						case opHypotx : res.uf = hypotx(a->uf, slotRawInt(b)); break;
						case opPow   : res.uf = pow(a->uf, (double)slotRawInt(b)); break;
						case opRing1 : res.uf = a->uf * slotRawInt(b) + a->uf; break;
						case opRing2 : res.uf = a->uf * slotRawInt(b) + a->uf + slotRawInt(b); break;
						case opRing3 : res.uf = a->uf * a->uf * slotRawInt(b); break;
						case opRing4 : res.uf = a->uf * a->uf * slotRawInt(b)
											- a->uf * slotRawInt(b) * slotRawInt(b); break;
						case opDifSqr : res.uf = a->uf * a->uf - slotRawInt(b) * slotRawInt(b); break;
						case opSumSqr : res.uf = a->uf * a->uf + slotRawInt(b) * slotRawInt(b); break;
						case opSqrSum : {
							double z;
							z = a->uf + slotRawInt(b);
							res.uf = z*z;
						} break;
						case opSqrDif : {
							double z;
							z = a->uf - slotRawInt(b);
							res.uf = z*z;
						} break;
						case opAbsDif : res.uf = fabs(a->uf - slotRawInt(b)); break;
						case opThresh : res.uf = a->uf<slotRawInt(b) ? 0. : a->uf; break;
						case opAMClip : res.uf = slotRawInt(b)<0 ? 0. : a->uf*slotRawInt(b); break;
						case opScaleNeg : res.uf = a->uf<0 ? a->uf*slotRawInt(b) : a->uf; break;
						case opClip2 :
							res.uf = a->uf < -slotRawInt(b) ? -slotRawInt(b) : (a->uf > slotRawInt(b) ? slotRawInt(b) : a->uf);
							break;
						case opFold2 :
							res.uf = sc_fold2(a->uf, (double)slotRawInt(b));
							break;
						case opWrap2 :
							res.uf = sc_wrap(a->uf, (double)-slotRawInt(b), (double)slotRawInt(b));
							break;
						case opExcess :
							res.uf = a->uf - (a->uf < -slotRawInt(b) ? -slotRawInt(b) : (a->uf > slotRawInt(b) ? slotRawInt(b) : a->uf));
							break;
						case opFirstArg : res.uf = a->uf; break;
						case opRandRange :
							res.uf = a->uf + g->rgen->frand() * (slotRawInt(b) - a->uf);
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(a->uf, slotRawInt(b));
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
							case opAdd : SetObject(&res, signal_add_xf(g, slotRawObject(b), a->uf)); break;
							case opSub : SetObject(&res, signal_sub_fx(g, a->uf, slotRawObject(b))); break;
							case opMul : SetObject(&res, signal_mul_xf(g, slotRawObject(b), a->uf)); break;
							case opIDiv : SetObject(&res, signal_div_fx(g, a->uf, slotRawObject(b))); break;
							case opFDiv : SetObject(&res, signal_div_fx(g, a->uf, slotRawObject(b))); break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin : SetObject(&res, signal_min_xf(g, slotRawObject(b), a->uf)); break;
							case opMax : SetObject(&res, signal_max_xf(g, slotRawObject(b), a->uf)); break;
							case opRing1 : SetObject(&res, signal_ring1_fx(g, a->uf, slotRawObject(b))); break;
							case opRing2 : SetObject(&res, signal_ring2_fx(g, a->uf, slotRawObject(b))); break;
							case opRing3 : SetObject(&res, signal_ring3_fx(g, a->uf, slotRawObject(b))); break;
							case opRing4 : SetObject(&res, signal_ring4_fx(g, a->uf, slotRawObject(b))); break;
							case opDifSqr : SetObject(&res, signal_difsqr_fx(g, a->uf, slotRawObject(b))); break;
							case opSumSqr : SetObject(&res, signal_sumsqr_fx(g, a->uf, slotRawObject(b))); break;
							case opSqrSum : SetObject(&res, signal_sqrsum_fx(g, a->uf, slotRawObject(b))); break;
							case opSqrDif : SetObject(&res, signal_sqrdif_fx(g, a->uf, slotRawObject(b))); break;
							case opAbsDif : SetObject(&res, signal_absdif_fx(g, a->uf, slotRawObject(b))); break;
							case opThresh : SetObject(&res, signal_thresh_fx(g, a->uf, slotRawObject(b))); break;
							case opAMClip : SetObject(&res, signal_amclip_fx(g, a->uf, slotRawObject(b))); break;
							case opScaleNeg : SetObject(&res, signal_scaleneg_fx(g, a->uf, slotRawObject(b))); break;
							case opClip2 : SetObject(&res, signal_clip2_fx(g, a->uf, slotRawObject(b))); break;
							case opFold2 : SetObject(&res, signal_fold2_fx(g, a->uf, slotRawObject(b))); break;
							case opWrap2 : SetObject(&res, signal_wrap2_fx(g, a->uf, slotRawObject(b))); break;
							case opExcess : SetObject(&res, signal_excess_fx(g, a->uf, slotRawObject(b))); break;
							case opFirstArg : SetObject(&res, slotRawObject(a)); break;
							default : goto send_normal_2;
						}
					} else goto send_normal_2;
					break;
				default : // double
					switch (opcode) {
						case opAdd : res.uf = a->uf + b->uf; break;
						case opSub : res.uf = a->uf - b->uf; break;
						case opMul : res.uf = a->uf * b->uf; break;
						case opIDiv : SetInt(&res, (long)floor(a->uf / b->uf)); break;
						case opFDiv : res.uf = a->uf / b->uf; break;
						case opMod : res.uf = sc_mod(a->uf, b->uf); break;
						case opEQ  : res = BOOL(a->uf == b->uf); break;
						case opNE  : res = BOOL(a->uf != b->uf); break;
						case opLT  : res = BOOL(a->uf <  b->uf); break;
						case opGT  : res = BOOL(a->uf >  b->uf); break;
						case opLE  : res = BOOL(a->uf <= b->uf); break;
						case opGE  : res = BOOL(a->uf >= b->uf); break;
						//case opIdentical  : res = BOOL(a->uf == b->uf); break;
						//case opNotIdentical  : res = BOOL(a->uf != b->uf); break;
						case opMin : res.uf = a->uf < b->uf ? a->uf : b->uf; break;
						case opMax : res.uf = a->uf > b->uf ? a->uf : b->uf; break;
						case opRound : res.uf = sc_round(a->uf, b->uf); break;
						case opRoundUp : res.uf = sc_roundUp(a->uf, b->uf); break;
						case opTrunc : res.uf = sc_trunc(a->uf, b->uf); break;
						case opAtan2 : res.uf = atan2(a->uf, b->uf); break;
						case opHypot : res.uf = hypot(a->uf, b->uf); break;
						case opHypotx : res.uf = hypotx(a->uf, b->uf); break;
						case opPow   : res.uf = pow(a->uf, b->uf); break;
						case opRing1 : res.uf = a->uf * b->uf + a->uf; break;
						case opRing2 : res.uf = a->uf * b->uf + a->uf + b->uf; break;
						case opRing3 : res.uf = a->uf * a->uf * b->uf; break;
						case opRing4 : res.uf = a->uf * a->uf * b->uf
											- a->uf * b->uf * b->uf; break;
						case opDifSqr : res.uf = a->uf * a->uf - b->uf * b->uf; break;
						case opSumSqr : res.uf = a->uf * a->uf + b->uf * b->uf; break;
						case opSqrSum : {
							double z;
							z = a->uf + b->uf;
							res.uf = z*z;
						} break;
						case opSqrDif : {
							double z;
							z = a->uf - b->uf;
							res.uf = z*z;
						} break;
						case opAbsDif : res.uf = fabs(a->uf - b->uf); break;
						case opThresh : res.uf = a->uf<b->uf ? 0. : a->uf; break;
						case opAMClip : res.uf = b->uf<0 ? 0 : a->uf*b->uf; break;
						case opScaleNeg : res.uf = a->uf<0 ? a->uf*b->uf : a->uf; break;
						case opClip2 :
							res.uf = a->uf < -b->uf ? -b->uf : (a->uf > b->uf ? b->uf : a->uf);
							break;
						case opFold2 :
							res.uf = sc_fold2(a->uf, b->uf);
							break;
						case opWrap2 :
							res.uf = sc_wrap(a->uf, -b->uf, b->uf);
							break;
						case opExcess :
							res.uf = a->uf - (a->uf < -b->uf ? -b->uf : (a->uf > b->uf ? b->uf : a->uf));
							break;
						case opFirstArg : res.uf = a->uf; break;
						case opRandRange :
							res.uf = a->uf + g->rgen->frand() * (b->uf - a->uf);
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(a->uf, b->uf);
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

