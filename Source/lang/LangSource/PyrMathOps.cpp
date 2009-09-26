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

	switch (a->utag) {
		case tagInt :
			switch (opcode) {
				case opNeg : res.ui = -a->ui; res.utag = tagInt; break;
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opBitNot : res.ui = ~a->ui; res.utag = tagInt; break;
				case opAbs : res.ui = a->ui < 0 ? -a->ui : a->ui; res.utag = tagInt; break;
				case opAsFloat : res.uf = (double)a->ui; break;
				case opAsInt : res.ui = (int)a->ui; res.utag = tagInt; break;
				case opCeil : res.ui = a->ui; res.utag = tagInt; break;
				case opFloor : res.ui = a->ui; res.utag = tagInt; break;
				case opFrac : res.ui = 0; res.utag = tagInt; break;
				case opSign : res.ui = a->ui > 0 ? 1 : (a->ui == 0 ? 0 : -1); res.utag = tagInt; break;
				case opSquared : res.ui = a->ui * a->ui; res.utag = tagInt; break;
				case opCubed : res.ui = a->ui * a->ui * a->ui; res.utag = tagInt; break;
				case opSqrt : res.uf = sqrt((double)a->ui); break;
				case opExp : res.uf = exp((double)a->ui); break;
				case opRecip : res.uf = 1. / a->ui; break;
				case opMIDICPS : res.uf = sc_midicps((double)a->ui); break;
				case opCPSMIDI : res.uf = sc_cpsmidi((double)a->ui); break;
				case opMIDIRatio : res.uf = sc_midiratio((double)a->ui); break;
				case opRatioMIDI : res.uf = sc_ratiomidi((double)a->ui); break;
				case opAmpDb : res.uf = sc_ampdb((double)a->ui); break;
				case opDbAmp : res.uf = sc_dbamp((double)a->ui); break;
				case opOctCPS : res.uf = sc_octcps((double)a->ui); break;
				case opCPSOct : res.uf = sc_cpsoct((double)a->ui); break;
				case opLog : res.uf = log((double)a->ui); break;
				case opLog2 : res.uf = log2((double)a->ui); break;
				case opLog10 : res.uf = log10((double)a->ui); break;
				case opSin : res.uf = sin((double)a->ui); break;
				case opCos : res.uf = cos((double)a->ui); break;
				case opTan : res.uf = tan((double)a->ui); break;
				case opArcSin : res.uf = asin((double)a->ui); break;
				case opArcCos : res.uf = acos((double)a->ui); break;
				case opArcTan : res.uf = atan((double)a->ui); break;
				case opSinH : res.uf = sinh((double)a->ui); break;
				case opCosH : res.uf = cosh((double)a->ui); break;
				case opTanH : res.uf = tanh((double)a->ui); break;
				case opRand : res.ui = g->rgen->irand(a->ui); res.utag = tagInt; break;
				case opRand2 : res.ui = g->rgen->irand2(a->ui); res.utag = tagInt; break;
				case opLinRand : res.ui = g->rgen->ilinrand(a->ui); res.utag = tagInt; break;
				case opBiLinRand : res.ui = g->rgen->ibilinrand(a->ui); res.utag = tagInt; break;

//				case opExpRand : res.uf = g->rgen->exprand(a->ui); break;
//				case opBiExpRand : res.uf = g->rgen->biexprand(a->ui); break;
				case opSum3Rand : res.uf = g->rgen->sum3rand(a->ui); break;

//				case opGammaRand : res.uf = g->rgen->gammarand(a->ui); break;
//				case opGaussRand : res.uf = g->rgen->gaussrand(a->ui); break;
//				case opPoiRand : res.uf = g->rgen->poirand(a->ui); break;

				case opDistort : res.uf = sc_distort((double)a->ui); break;
				case opSoftClip : res.uf = sc_softclip((double)a->ui); break;
				case opCoin : res = BOOL(a->ui); break;
				case opRectWindow : res.uf = sc_rectwindow((double)a->ui); break;
				case opHanWindow : res.uf = sc_hanwindow((double)a->ui); break;
				case opWelchWindow : res.uf = sc_welwindow((double)a->ui); break;
				case opTriWindow : res.uf = sc_triwindow((double)a->ui); break;
				case opSCurve : res.uf = sc_scurve((double)a->ui); break;
				case opRamp : res.uf = sc_ramp((double)a->ui); break;
				default : goto send_normal_1;
			}
			break;
		case tagChar :
			switch (opcode) {
				//case opNot : goto send_normal_1;
				case opIsNil : SetFalse(a); break;
				case opNotNil : SetTrue(a); break;
				case opAsInt : res.utag = tagInt; break;
				case opDigitValue :
					if (a->ui >= '0' && a->ui <= '9') res.ui = a->ui - '0';
					else if (a->ui >= 'A' && a->ui <= 'Z') res.ui = a->ui - 'A';
					else if (a->ui >= 'a' && a->ui <= 'z') res.ui = a->ui - 'a';
					else res.ui = 0;
					res.utag = tagInt;
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
				case opNot : a->utag = tagTrue; break;
				case opIsNil : /*SetFalse(a);*/ break;
				case opNotNil : SetTrue(a); break;
				default : goto send_normal_1;
			}
			break;
		case tagTrue :
			switch (opcode) {
				case opNot : a->utag = tagFalse; break;
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
				default : res.us = a->us; res.utag = tagSym;
			}
			break;
		case tagObj :
			if (isKindOf(a->uo, class_signal)) {
				switch (opcode) {
					case opNeg : res.uo = signal_invert(g, a->uo); res.utag = tagObj; break;
					case opIsNil : SetFalse(a); break;
					case opNotNil : SetTrue(a); break;
					case opAbs : res.uo = signal_abs(g, a->uo); res.utag = tagObj; break;
					case opSign : res.uo = signal_sign(g, a->uo); res.utag = tagObj; break;
					case opSquared : res.uo = signal_squared(g, a->uo); res.utag = tagObj; break;
					case opCubed : res.uo = signal_cubed(g, a->uo); res.utag = tagObj; break;
					case opSqrt : res.uo = signal_sqrt(g, a->uo); res.utag = tagObj; break;
					case opExp : res.uo = signal_exp(g, a->uo); res.utag = tagObj; break;
					case opRecip : res.uo = signal_recip(g, a->uo); res.utag = tagObj; break;
					case opLog : res.uo = signal_log(g, a->uo); res.utag = tagObj; break;
					case opLog2 : res.uo = signal_log2(g, a->uo); res.utag = tagObj; break;
					case opLog10 : res.uo = signal_log10(g, a->uo); res.utag = tagObj; break;
					case opSin : res.uo = signal_sin(g, a->uo); res.utag = tagObj; break;
					//case opSin : res.uo = signal_fsin(g, a->uo); res.utag = tagObj; break;
					case opCos : res.uo = signal_cos(g, a->uo); res.utag = tagObj; break;
					case opTan : res.uo = signal_tan(g, a->uo); res.utag = tagObj; break;
					case opArcSin : res.uo = signal_asin(g, a->uo); res.utag = tagObj; break;
					case opArcCos : res.uo = signal_acos(g, a->uo); res.utag = tagObj; break;
					case opArcTan : res.uo = signal_atan(g, a->uo); res.utag = tagObj; break;
					case opSinH : res.uo = signal_sinh(g, a->uo); res.utag = tagObj; break;
					case opCosH : res.uo = signal_cosh(g, a->uo); res.utag = tagObj; break;
					case opTanH : res.uo = signal_tanh(g, a->uo); res.utag = tagObj; break;
					case opDistort : res.uo = signal_distort(g, a->uo); res.utag = tagObj; break;
					case opSoftClip : res.uo = signal_softclip(g, a->uo); res.utag = tagObj; break;
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
				case opAsInt : res.ui = (int)a->uf; res.utag = tagInt; break;
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

	g->sp[0].ucopy = res.ucopy;
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

	switch (a->utag) {
		case tagInt : {
			switch (b->utag) {
				case tagInt :
					switch (opcode) {
						case opAdd : res.ui = a->ui + b->ui; res.utag = tagInt; break;
						case opSub : res.ui = a->ui - b->ui; res.utag = tagInt; break;
						case opMul : res.ui = a->ui * b->ui; res.utag = tagInt; break;
						case opIDiv : res.ui = sc_div(a->ui, b->ui); res.utag = tagInt; break;
						case opFDiv : res.uf = (double)a->ui / (double)b->ui; break;
						case opMod : res.ui = sc_mod((int)a->ui, (int)b->ui); res.utag = tagInt; break;
						case opEQ  : res = BOOL(a->ui == b->ui); break;
						case opNE  : res = BOOL(a->ui != b->ui); break;
						case opLT  : res = BOOL(a->ui <  b->ui); break;
						case opGT  : res = BOOL(a->ui >  b->ui); break;
						case opLE  : res = BOOL(a->ui <= b->ui); break;
						case opGE  : res = BOOL(a->ui >= b->ui); break;
						//case opIdentical : res = BOOL(a->ui == b->ui); break;
						//case opNotIdentical : res = BOOL(a->ui != b->ui); break;
						case opMin : res.ui = a->ui < b->ui ? a->ui : b->ui; res.utag = tagInt; break;
						case opMax : res.ui = a->ui > b->ui ? a->ui : b->ui; res.utag = tagInt; break;
						case opBitAnd : res.ui = a->ui & b->ui; res.utag = tagInt; break;
						case opBitOr  : res.ui = a->ui | b->ui; res.utag = tagInt; break;
						case opBitXor : res.ui = a->ui ^ b->ui; res.utag = tagInt; break;
						case opLCM   : res.ui = sc_lcm(a->ui, b->ui); res.utag = tagInt; break;
						case opGCD   : res.ui = sc_gcd(a->ui, b->ui); res.utag = tagInt; break;
						case opRound : res.ui = sc_round((int)a->ui, (int)b->ui); res.utag = tagInt; break;
						case opRoundUp : res.ui = sc_roundUp((int)a->ui, (int)b->ui); res.utag = tagInt; break;
						case opTrunc : res.ui = sc_trunc((int)a->ui, (int)b->ui); res.utag = tagInt; break;
						case opAtan2 : res.uf = atan2((double)a->ui, (double)b->ui); break;
						case opHypot : res.uf = hypot((double)a->ui, (double)b->ui); break;
						case opHypotx : res.uf = hypotx((double)a->ui, (double)b->ui); break;
						case opPow   : res.uf = pow((double)a->ui, (double)b->ui); break;
						case opShiftLeft : {
							long ia = a->ui;
							long ib = b->ui;
							if (ib>0) ia <<= ib;
							else if (ib<0) ia >>= -ib;
							res.ui = ia;
							res.utag = tagInt;
						} break;
						case opShiftRight : {
							long ia = a->ui;
							long ib = b->ui;
							if (ib>0) ia >>= ib;
							else if (ib<0) ia <<= -ib;
							res.ui = ia;
							res.utag = tagInt;
						} break;
						case opUnsignedShift : {
              unsigned long ia = a->ui;
              long ib = b->ui;
							if (ib>0) ia >>= ib;
							else if (ib<0) ia <<= -ib;
							res.ui = ia;
							res.utag = tagInt;
						} break;
						case opRing1 : res.ui = a->ui * b->ui + a->ui; res.utag = tagInt; break;
						case opRing2 : res.ui = a->ui * b->ui + a->ui + b->ui; res.utag = tagInt; break;
						case opRing3 : res.ui = a->ui * a->ui * b->ui; res.utag = tagInt; break;
						case opRing4 : res.uf = a->ui * a->ui * b->ui
											- a->ui * b->ui * b->ui; res.utag = tagInt; break;
						case opDifSqr : res.ui = a->ui * a->ui - b->ui * b->ui; res.utag = tagInt; break;
						case opSumSqr : res.ui = a->ui * a->ui + b->ui * b->ui; res.utag = tagInt; break;
						case opSqrSum : {
							long z;
							z = a->ui + b->ui;
							res.ui = z*z;
							res.utag = tagInt;
						} break;
						case opSqrDif : {
							long z;
							z = a->ui - b->ui;
							res.ui = z*z;
							res.utag = tagInt;
						} break;
						case opAbsDif : res.ui = abs(a->ui - b->ui); res.utag = tagInt; break;
						case opThresh : res.ui = a->ui<b->ui ? 0 : a->ui; res.utag = tagInt; break;
						case opAMClip : res.ui = b->ui<0 ? 0 : a->ui*b->ui; res.utag = tagInt; break;
						case opScaleNeg : res.ui = a->ui<0 ? a->ui*b->ui : a->ui; res.utag = tagInt; break;
						case opClip2 :
							res.ui = a->ui < -b->ui ? -b->ui : (a->ui > b->ui ? b->ui : a->ui);
							res.utag = tagInt;
							break;
						case opFold2 :
							res.ui = sc_fold2(a->ui, b->ui);
							res.utag = tagInt;
							break;
						case opWrap2 :
							res.ui = sc_mod((int)(a->ui + b->ui), (int)(2 * b->ui)) - b->ui;
							res.utag = tagInt;
							break;
						case opExcess :
							res.ui = a->ui - (a->ui < -b->ui ? -b->ui : (a->ui > b->ui ? b->ui : a->ui));
							res.utag = tagInt;
							break;
						case opFirstArg : res.ui = a->ui; res.utag = tagInt; break;
						case opRandRange :
							res.ui = b->ui > a->ui ? a->ui + g->rgen->irand(b->ui - a->ui + 1)
								: b->ui + g->rgen->irand(a->ui - b->ui + 1);
							res.utag = tagInt;
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(a->ui, b->ui);
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
						res.us = b->us; res.utag = tagSym;
					}
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						switch (opcode) {
							case opAdd : res.uo = signal_add_xf(g, b->uo, a->ui); res.utag = tagObj; break;
							case opSub : res.uo = signal_sub_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opMul : res.uo = signal_mul_xf(g, b->uo, a->ui); res.utag = tagObj; break;
							case opIDiv : res.uo = signal_div_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opFDiv : res.uo = signal_div_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin : res.uo = signal_min_xf(g, b->uo, a->ui); res.utag = tagObj; break;
							case opMax : res.uo = signal_max_xf(g, b->uo, a->ui); res.utag = tagObj; break;
							case opRing1 : res.uo = signal_ring1_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opRing2 : res.uo = signal_ring2_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opRing3 : res.uo = signal_ring3_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opRing4 : res.uo = signal_ring4_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opDifSqr : res.uo = signal_difsqr_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opSumSqr : res.uo = signal_sumsqr_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opSqrSum : res.uo = signal_sqrsum_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opSqrDif : res.uo = signal_sqrdif_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opAbsDif : res.uo = signal_absdif_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opThresh : res.uo = signal_thresh_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opAMClip : res.uo = signal_amclip_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opScaleNeg : res.uo = signal_scaleneg_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opClip2 : res.uo = signal_clip2_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opFold2 : res.uo = signal_fold2_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opWrap2 : res.uo = signal_wrap2_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opExcess : res.uo = signal_excess_fx(g, a->ui, b->uo); res.utag = tagObj; break;
							case opFirstArg : res.uo = a->uo; res.utag = tagObj; break;
							default : goto send_normal_2;
						}
					} else {
						goto send_normal_2;
					}
					break;
				default :
					switch (opcode) {
						case opAdd : res.uf = a->ui + b->uf; break;
						case opSub : res.uf = a->ui - b->uf; break;
						case opMul : res.uf = a->ui * b->uf; break;
						case opIDiv : res.ui = (long)floor(a->ui / b->uf); res.utag = tagInt; break;
						case opFDiv : res.uf = a->ui / b->uf; break;
						case opMod : res.uf = sc_mod((double)a->ui, b->uf); break;
						case opEQ  : res = BOOL(a->ui == b->uf); break;
						case opNE  : res = BOOL(a->ui != b->uf); break;
						case opLT  : res = BOOL(a->ui <  b->uf); break;
						case opGT  : res = BOOL(a->ui >  b->uf); break;
						case opLE  : res = BOOL(a->ui <= b->uf); break;
						case opGE  : res = BOOL(a->ui >= b->uf); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin : res.uf = a->ui < b->uf ? a->ui : b->uf; break;
						case opMax : res.uf = a->ui > b->uf ? a->ui : b->uf; break;
						case opRound : res.uf = sc_round((double)a->ui, b->uf); break;
						case opRoundUp : res.uf = sc_roundUp((double)a->ui, b->uf); break;
						case opTrunc : res.uf = sc_trunc((double)a->ui, b->uf); break;
						case opAtan2 : res.uf = atan2(a->ui, b->uf); break;
						case opHypot : res.uf = hypot(a->ui, b->uf); break;
						case opHypotx : res.uf = hypotx(a->ui, b->uf); break;
						case opPow   : res.uf = pow((double)a->ui, b->uf); break;
						case opRing1 : res.uf = a->ui * b->uf + a->ui; break;
						case opRing2 : res.uf = a->ui * b->uf + a->ui + b->uf; break;
						case opRing3 : res.uf = a->ui * a->ui * b->uf; break;
						case opRing4 : res.uf = a->ui * a->ui * b->uf
											- a->ui * b->uf * b->uf; break;
						case opDifSqr : res.uf = a->ui * a->ui - b->uf * b->uf; break;
						case opSumSqr : res.uf = a->ui * a->ui + b->uf * b->uf; break;
						case opSqrSum : {
							double z;
							z = a->ui + b->uf;
							res.uf = z*z;
						} break;
						case opSqrDif : {
							double z;
							z = a->ui - b->uf;
							res.uf = z*z;
						} break;
						case opAbsDif : res.uf = fabs(a->ui - b->uf); break;
						case opThresh : res.ui = a->ui<b->uf ? 0 : a->ui; res.utag = tagInt; break;
						case opAMClip : res.uf = b->uf<0 ? 0 : a->ui*b->uf; break;
						case opScaleNeg : res.uf = a->ui<0 ? a->ui*b->uf : a->ui; break;
						case opClip2 :
							res.uf = a->ui < -b->uf ? -b->uf : (a->ui > b->uf ? b->uf : a->ui);
							break;
						case opFold2 :
							res.uf = sc_fold2((double)a->ui, b->uf);
							break;
						case opWrap2 :
							res.uf = sc_wrap((double)a->ui, -b->uf, -b->uf);
							break;
						case opExcess :
							res.uf = a->ui - (a->ui < -b->uf ? -b->uf : (a->ui > b->uf ? b->uf : a->ui));
							break;
						case opFirstArg : res.ui = a->ui; res.utag = tagInt; break;
						case opRandRange :
							res.uf = a->ui + g->rgen->frand() * (b->uf - a->ui);
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(a->ui, b->uf);
							break;
						default : goto send_normal_2;
					}
					break;
			}
		} break;
		case tagChar : {
			if (b->utag == tagChar) {
				switch (opcode) {
					case opEQ  : res = BOOL(a->ui == b->ui); break;
					case opNE  : res = BOOL(a->ui != b->ui); break;
					case opLT  : res = BOOL(a->ui <  b->ui); break;
					case opGT  : res = BOOL(a->ui >  b->ui); break;
					case opLE  : res = BOOL(a->ui <= b->ui); break;
					case opGE  : res = BOOL(a->ui >= b->ui); break;
					//case opIdentical : res = BOOL(a->ui == b->ui); break;
					//case opNotIdentical : res = BOOL(a->ui != b->ui); break;
					case opMin : res.ui = a->ui < b->ui ? a->ui : b->ui; res.utag = tagInt; break;
					case opMax : res.ui = a->ui > b->ui ? a->ui : b->ui; res.utag = tagInt; break;
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
			if (b->utag == tagSym) {
				switch (opcode) {
					case opEQ  : res = BOOL(a->us == b->us); break;
					case opNE  : res = BOOL(a->us != b->us); break;
					case opLT  : res = BOOL(strcmp(a->us->name, b->us->name) <  0); break;
					case opGT  : res = BOOL(strcmp(a->us->name, b->us->name) >  0); break;
					case opLE  : res = BOOL(strcmp(a->us->name, b->us->name) <= 0); break;
					case opGE  : res = BOOL(strcmp(a->us->name, b->us->name) >= 0); break;
					//default : leave first operand on stack
				}
			} else {
				if (IS_BINARY_BOOL_OP(opcode)) {
					res = opcode == opNE ? o_true : o_false;
				} else {
					res.us = a->us; res.utag = tagSym;
				}
			}
			break;
		case tagObj : {
			if (isKindOf(a->uo, class_signal)) {
				switch (b->utag) {
					case tagInt :
						switch (opcode) {
							case opAdd  : res.uo = signal_add_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opSub  : res.uo = signal_sub_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opMul  : res.uo = signal_mul_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opIDiv  : res.uo = signal_div_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opFDiv  : res.uo = signal_div_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin  : res.uo = signal_min_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opMax  : res.uo = signal_max_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opFill : res.uo = signal_fill(a->uo, b->ui); res.utag = tagObj; break;
							case opRing1 : res.uo = signal_ring1_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opRing2 : res.uo = signal_ring2_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opRing3 : res.uo = signal_ring3_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opRing4 : res.uo = signal_ring4_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opDifSqr : res.uo = signal_difsqr_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opSumSqr : res.uo = signal_sumsqr_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opSqrSum : res.uo = signal_sqrsum_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opSqrDif : res.uo = signal_sqrdif_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opAbsDif : res.uo = signal_absdif_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opThresh : res.uo = signal_thresh_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opAMClip : res.uo = signal_amclip_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opScaleNeg : res.uo = signal_scaleneg_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opClip2 : res.uo = signal_clip2_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opFold2 : res.uo = signal_fold2_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opWrap2 : res.uo = signal_wrap2_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opExcess : res.uo = signal_excess_xf(g, a->uo, b->ui); res.utag = tagObj; break;
							case opFirstArg : res.uo = a->uo; res.utag = tagObj; break;
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
							res.us = b->us; res.utag = tagSym;
						}
						break;
					case tagObj :
						if (isKindOf(b->uo, class_signal)) {
							switch (opcode) {
								case opAdd : res.uo = signal_add_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opSub : res.uo = signal_sub_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opMul : res.uo = signal_mul_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opIDiv : res.uo = signal_div_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opFDiv : res.uo = signal_div_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opEQ  : res = BOOL(signal_equal_xx(g, a->uo, b->uo)); break;
								case opNE  : res = BOOL(!signal_equal_xx(g, a->uo, b->uo)); break;
								case opMin : res.uo = signal_min_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opMax : res.uo = signal_max_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opRing1 : res.uo = signal_ring1_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opRing2 : res.uo = signal_ring2_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opRing3 : res.uo = signal_ring3_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opRing4 : res.uo = signal_ring4_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opDifSqr : res.uo = signal_difsqr_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opSumSqr : res.uo = signal_sumsqr_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opSqrSum : res.uo = signal_sqrsum_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opSqrDif : res.uo = signal_sqrdif_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opAbsDif : res.uo = signal_absdif_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opThresh : res.uo = signal_thresh_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opAMClip : res.uo = signal_amclip_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opScaleNeg : res.uo = signal_scaleneg_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opClip2 : res.uo = signal_clip2_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opFold2 : res.uo = signal_fold2_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opWrap2 : res.uo = signal_wrap2_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opExcess : res.uo = signal_excess_xx(g, a->uo, b->uo); res.utag = tagObj; break;
								case opFirstArg : res.uo = a->uo; res.utag = tagObj; break;
								default : goto send_normal_2;
							}
						} else goto send_normal_2;
						break;
					default : // double
						switch (opcode) {
							case opAdd  : res.uo = signal_add_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opSub  : res.uo = signal_sub_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opMul  : res.uo = signal_mul_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opIDiv  : res.uo = signal_div_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opFDiv  : res.uo = signal_div_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin  : res.uo = signal_min_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opMax  : res.uo = signal_max_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opFill : res.uo = signal_fill(a->uo, b->uf); res.utag = tagObj; break;
							case opRing1 : res.uo = signal_ring1_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opRing2 : res.uo = signal_ring2_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opRing3 : res.uo = signal_ring3_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opRing4 : res.uo = signal_ring4_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opDifSqr : res.uo = signal_difsqr_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opSumSqr : res.uo = signal_sumsqr_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opSqrSum : res.uo = signal_sqrsum_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opSqrDif : res.uo = signal_sqrdif_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opAbsDif : res.uo = signal_absdif_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opThresh : res.uo = signal_thresh_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opAMClip : res.uo = signal_amclip_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opScaleNeg : res.uo = signal_scaleneg_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opClip2 : res.uo = signal_clip2_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opFold2 : res.uo = signal_fold2_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opWrap2 : res.uo = signal_wrap2_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opExcess : res.uo = signal_excess_xf(g, a->uo, b->uf); res.utag = tagObj; break;
							case opFirstArg : res.uo = a->uo; res.utag = tagObj; break;
							default : goto send_normal_2;
						}
						break;
				}
			} else {
				goto send_normal_2;
			}
		} break;
		default : { // double
			switch (b->utag) {
				case tagInt :
					switch (opcode) {
						case opAdd : res.uf = a->uf + b->ui; break;
						case opSub : res.uf = a->uf - b->ui; break;
						case opMul : res.uf = a->uf * b->ui; break;
						case opIDiv : res.ui = (long)floor(a->uf / b->ui); res.utag = tagInt; break;
						case opFDiv : res.uf = a->uf / b->ui; break;
						case opMod : res.uf = sc_mod(a->uf, (double)b->ui); break;
						case opEQ  : res = BOOL(a->uf == b->ui); break;
						case opNE  : res = BOOL(a->uf != b->ui); break;
						case opLT  : res = BOOL(a->uf <  b->ui); break;
						case opGT  : res = BOOL(a->uf >  b->ui); break;
						case opLE  : res = BOOL(a->uf <= b->ui); break;
						case opGE  : res = BOOL(a->uf >= b->ui); break;
						//case opIdentical : res = o_false; break;
						//case opNotIdentical : res = o_true; break;
						case opMin : res.uf = a->uf < b->ui ? a->uf : b->ui; break;
						case opMax : res.uf = a->uf > b->ui ? a->uf : b->ui; break;
						case opRound : res.uf = sc_round(a->uf, (double)b->ui); break;
						case opRoundUp : res.uf = sc_roundUp(a->uf, (double)b->ui); break;
						case opTrunc : res.uf = sc_trunc(a->uf, (double)b->ui); break;
						case opAtan2 : res.uf = atan2(a->uf, b->ui); break;
						case opHypot : res.uf = hypot(a->uf, b->ui); break;
						case opHypotx : res.uf = hypotx(a->uf, b->ui); break;
						case opPow   : res.uf = pow(a->uf, (double)b->ui); break;
						case opRing1 : res.uf = a->uf * b->ui + a->uf; break;
						case opRing2 : res.uf = a->uf * b->ui + a->uf + b->ui; break;
						case opRing3 : res.uf = a->uf * a->uf * b->ui; break;
						case opRing4 : res.uf = a->uf * a->uf * b->ui
											- a->uf * b->ui * b->ui; break;
						case opDifSqr : res.uf = a->uf * a->uf - b->ui * b->ui; break;
						case opSumSqr : res.uf = a->uf * a->uf + b->ui * b->ui; break;
						case opSqrSum : {
							double z;
							z = a->uf + b->ui;
							res.uf = z*z;
						} break;
						case opSqrDif : {
							double z;
							z = a->uf - b->ui;
							res.uf = z*z;
						} break;
						case opAbsDif : res.uf = fabs(a->uf - b->ui); break;
						case opThresh : res.uf = a->uf<b->ui ? 0. : a->uf; break;
						case opAMClip : res.uf = b->ui<0 ? 0. : a->uf*b->ui; break;
						case opScaleNeg : res.uf = a->uf<0 ? a->uf*b->ui : a->uf; break;
						case opClip2 :
							res.uf = a->uf < -b->ui ? -b->ui : (a->uf > b->ui ? b->ui : a->uf);
							break;
						case opFold2 :
							res.uf = sc_fold2(a->uf, (double)b->ui);
							break;
						case opWrap2 :
							res.uf = sc_wrap(a->uf, (double)-b->ui, (double)b->ui);
							break;
						case opExcess :
							res.uf = a->uf - (a->uf < -b->ui ? -b->ui : (a->uf > b->ui ? b->ui : a->uf));
							break;
						case opFirstArg : res.uf = a->uf; break;
						case opRandRange :
							res.uf = a->uf + g->rgen->frand() * (b->ui - a->uf);
							break;
						case opExpRandRange :
							res.uf = g->rgen->exprandrng(a->uf, b->ui);
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
						res.us = b->us; res.utag = tagSym;
					}
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						switch (opcode) {
							case opAdd : res.uo = signal_add_xf(g, b->uo, a->uf);  res.utag = tagObj; break;
							case opSub : res.uo = signal_sub_fx(g, a->uf, b->uo);  res.utag = tagObj; break;
							case opMul : res.uo = signal_mul_xf(g, b->uo, a->uf);  res.utag = tagObj; break;
							case opIDiv : res.uo = signal_div_fx(g, a->uf, b->uo);  res.utag = tagObj; break;
							case opFDiv : res.uo = signal_div_fx(g, a->uf, b->uo);  res.utag = tagObj; break;
							case opEQ  : res = o_false; break;
							case opNE  : res = o_true; break;
							case opMin : res.uo = signal_min_xf(g, b->uo, a->uf);  res.utag = tagObj; break;
							case opMax : res.uo = signal_max_xf(g, b->uo, a->uf);  res.utag = tagObj; break;
							case opRing1 : res.uo = signal_ring1_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opRing2 : res.uo = signal_ring2_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opRing3 : res.uo = signal_ring3_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opRing4 : res.uo = signal_ring4_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opDifSqr : res.uo = signal_difsqr_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opSumSqr : res.uo = signal_sumsqr_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opSqrSum : res.uo = signal_sqrsum_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opSqrDif : res.uo = signal_sqrdif_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opAbsDif : res.uo = signal_absdif_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opThresh : res.uo = signal_thresh_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opAMClip : res.uo = signal_amclip_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opScaleNeg : res.uo = signal_scaleneg_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opClip2 : res.uo = signal_clip2_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opFold2 : res.uo = signal_fold2_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opWrap2 : res.uo = signal_wrap2_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opExcess : res.uo = signal_excess_fx(g, a->uf, b->uo); res.utag = tagObj; break;
							case opFirstArg : res.uo = a->uo; res.utag = tagObj; break;
							default : goto send_normal_2;
						}
					} else goto send_normal_2;
					break;
				default : // double
					switch (opcode) {
						case opAdd : res.uf = a->uf + b->uf; break;
						case opSub : res.uf = a->uf - b->uf; break;
						case opMul : res.uf = a->uf * b->uf; break;
						case opIDiv : res.ui = (long)floor(a->uf / b->uf); res.utag = tagInt; break;
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
	g->sp[0].ucopy = res.ucopy;
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

