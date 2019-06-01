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
#include "PyrSymbol.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "MiscInlineMath.h"
#include "PyrKernelProto.h"

#include <limits>

double hypotx(double x, double y);

#define IS_BINARY_BOOL_OP(op) ((op) >= opEQ && (op) <= opGE)

int doSpecialUnaryArithMsg(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrSymbol* msg;
    int opcode = g->primitiveIndex;

    a = g->sp;

    switch (GetTag(a)) {
    case tagInt:
        switch (opcode) {
        case opNeg:
            SetRaw(a, -slotRawInt(a));
            break;
        // case opNot : goto send_normal_1;
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil:
            SetTrue(a);
            break;
        case opBitNot:
            SetRaw(a, ~slotRawInt(a));
            break;
        case opAbs:
            SetRaw(a, sc_abs(slotRawInt(a)));
            break;
        case opAsFloat:
            SetFloat(a, (double)slotRawInt(a));
            break;
        case opAsInt:
            SetRaw(a, (int)slotRawInt(a));
            break;
        case opCeil:
            SetRaw(a, slotRawInt(a));
            break;
        case opFloor:
            SetRaw(a, slotRawInt(a));
            break;
        case opFrac:
            SetRaw(a, 0);
            break;
        case opSign:
            SetRaw(a, slotRawInt(a) > 0 ? 1 : (slotRawInt(a) == 0 ? 0 : -1));
            break;
        case opSquared:
            SetRaw(a, slotRawInt(a) * slotRawInt(a));
            break;
        case opCubed:
            SetRaw(a, slotRawInt(a) * slotRawInt(a) * slotRawInt(a));
            break;
        case opSqrt:
            SetFloat(a, sqrt((double)slotRawInt(a)));
            break;
        case opExp:
            SetFloat(a, exp((double)slotRawInt(a)));
            break;
        case opRecip:
            SetFloat(a, 1. / slotRawInt(a));
            break;
        case opMIDICPS:
            SetFloat(a, sc_midicps((double)slotRawInt(a)));
            break;
        case opCPSMIDI:
            SetFloat(a, sc_cpsmidi((double)slotRawInt(a)));
            break;
        case opMIDIRatio:
            SetFloat(a, sc_midiratio((double)slotRawInt(a)));
            break;
        case opRatioMIDI:
            SetFloat(a, sc_ratiomidi((double)slotRawInt(a)));
            break;
        case opAmpDb:
            SetFloat(a, sc_ampdb((double)slotRawInt(a)));
            break;
        case opDbAmp:
            SetFloat(a, sc_dbamp((double)slotRawInt(a)));
            break;
        case opOctCPS:
            SetFloat(a, sc_octcps((double)slotRawInt(a)));
            break;
        case opCPSOct:
            SetFloat(a, sc_cpsoct((double)slotRawInt(a)));
            break;
        case opLog:
            SetFloat(a, log((double)slotRawInt(a)));
            break;
        case opLog2:
            SetFloat(a, sc_log2((double)slotRawInt(a)));
            break;
        case opLog10:
            SetFloat(a, log10((double)slotRawInt(a)));
            break;
        case opSin:
            SetFloat(a, sin((double)slotRawInt(a)));
            break;
        case opCos:
            SetFloat(a, cos((double)slotRawInt(a)));
            break;
        case opTan:
            SetFloat(a, tan((double)slotRawInt(a)));
            break;
        case opArcSin:
            SetFloat(a, asin((double)slotRawInt(a)));
            break;
        case opArcCos:
            SetFloat(a, acos((double)slotRawInt(a)));
            break;
        case opArcTan:
            SetFloat(a, atan((double)slotRawInt(a)));
            break;
        case opSinH:
            SetFloat(a, sinh((double)slotRawInt(a)));
            break;
        case opCosH:
            SetFloat(a, cosh((double)slotRawInt(a)));
            break;
        case opTanH:
            SetFloat(a, tanh((double)slotRawInt(a)));
            break;
        case opRand:
            SetRaw(a, g->rgen->irand(slotRawInt(a)));
            break;
        case opRand2:
            SetRaw(a, g->rgen->irand2(slotRawInt(a)));
            break;
        case opLinRand:
            SetRaw(a, g->rgen->ilinrand(slotRawInt(a)));
            break;
        case opBiLinRand:
            SetRaw(a, g->rgen->ibilinrand(slotRawInt(a)));
            break;

            //				case opExpRand : SetFloat(a, g->rgen->exprand(slotRawInt(a))); break;
            //				case opBiExpRand : SetFloat(a, g->rgen->biexprand(slotRawInt(a))); break;
        case opSum3Rand:
            SetFloat(a, g->rgen->sum3rand(slotRawInt(a)));
            break;

            //				case opGammaRand : SetFloat(a, g->rgen->gammarand(slotRawInt(a))); break;
            //				case opGaussRand : SetFloat(a, g->rgen->gaussrand(slotRawInt(a))); break;
            //				case opPoiRand : SetFloat(a, g->rgen->poirand(slotRawInt(a))); break;

        case opDistort:
            SetFloat(a, sc_distort((double)slotRawInt(a)));
            break;
        case opSoftClip:
            SetFloat(a, sc_softclip((double)slotRawInt(a)));
            break;
        case opCoin:
            SetBool(a, (slotRawInt(a)));
            break;
        case opRectWindow:
            SetFloat(a, sc_rectwindow((double)slotRawInt(a)));
            break;
        case opHanWindow:
            SetFloat(a, sc_hanwindow((double)slotRawInt(a)));
            break;
        case opWelchWindow:
            SetFloat(a, sc_welwindow((double)slotRawInt(a)));
            break;
        case opTriWindow:
            SetFloat(a, sc_triwindow((double)slotRawInt(a)));
            break;
        case opSCurve:
            SetFloat(a, sc_scurve((double)slotRawInt(a)));
            break;
        case opRamp:
            SetFloat(a, sc_ramp((double)slotRawInt(a)));
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagChar:
        switch (opcode) {
        // case opNot : goto send_normal_1;
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil:
            SetTrue(a);
            break;
        case opAsInt:
            SetTagRaw(a, tagInt);
            break;
        case opDigitValue:
            if (slotRawInt(a) >= '0' && slotRawInt(a) <= '9')
                SetInt(a, slotRawInt(a) - '0');
            else if (slotRawInt(a) >= 'A' && slotRawInt(a) <= 'Z')
                SetInt(a, slotRawInt(a) - 'A');
            else if (slotRawInt(a) >= 'a' && slotRawInt(a) <= 'z')
                SetInt(a, slotRawInt(a) - 'a');
            else
                SetInt(a, 0);
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagPtr:
        switch (opcode) {
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil:
            SetTrue(a);
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagNil:
        switch (opcode) {
        case opIsNil:
            SetTrue(a);
            break;
        case opNotNil:
            SetFalse(a);
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagFalse:
        switch (opcode) {
        case opNot:
            SetTrue(a);
            break;
        case opIsNil: /*SetFalse(a);*/
            break;
        case opNotNil:
            SetTrue(a);
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagTrue:
        switch (opcode) {
        case opNot:
            SetFalse(a);
            break;
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil: /*SetTrue(a);*/
            break;
        default:
            goto send_normal_1;
        }
        break;
    case tagSym:
        switch (opcode) {
        case opAsFloat:
        case opAsInt:
            goto send_normal_1;
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil:
            SetTrue(a);
            break;
        default: /* SetSymbol(a, slotRawSymbol(a)); */
            break;
        }
        break;
    case tagObj:
        if (isKindOf(slotRawObject(a), class_signal)) {
            switch (opcode) {
            case opNeg:
                SetRaw(a, signal_invert(g, slotRawObject(a)));
                break;
            case opIsNil:
                SetFalse(a);
                break;
            case opNotNil:
                SetTrue(a);
                break;
            case opAbs:
                SetRaw(a, signal_abs(g, slotRawObject(a)));
                break;
            case opSign:
                SetRaw(a, signal_sign(g, slotRawObject(a)));
                break;
            case opSquared:
                SetRaw(a, signal_squared(g, slotRawObject(a)));
                break;
            case opCubed:
                SetRaw(a, signal_cubed(g, slotRawObject(a)));
                break;
            case opSqrt:
                SetRaw(a, signal_sqrt(g, slotRawObject(a)));
                break;
            case opExp:
                SetRaw(a, signal_exp(g, slotRawObject(a)));
                break;
            case opRecip:
                SetRaw(a, signal_recip(g, slotRawObject(a)));
                break;
            case opLog:
                SetRaw(a, signal_log(g, slotRawObject(a)));
                break;
            case opLog2:
                SetRaw(a, signal_log2(g, slotRawObject(a)));
                break;
            case opLog10:
                SetRaw(a, signal_log10(g, slotRawObject(a)));
                break;
            case opSin:
                SetRaw(a, signal_sin(g, slotRawObject(a)));
                break;
            // case opSin : SetRaw(a, signal_fsin(g, slotRawObject(a))); break;
            case opCos:
                SetRaw(a, signal_cos(g, slotRawObject(a)));
                break;
            case opTan:
                SetRaw(a, signal_tan(g, slotRawObject(a)));
                break;
            case opArcSin:
                SetRaw(a, signal_asin(g, slotRawObject(a)));
                break;
            case opArcCos:
                SetRaw(a, signal_acos(g, slotRawObject(a)));
                break;
            case opArcTan:
                SetRaw(a, signal_atan(g, slotRawObject(a)));
                break;
            case opSinH:
                SetRaw(a, signal_sinh(g, slotRawObject(a)));
                break;
            case opCosH:
                SetRaw(a, signal_cosh(g, slotRawObject(a)));
                break;
            case opTanH:
                SetRaw(a, signal_tanh(g, slotRawObject(a)));
                break;
            case opDistort:
                SetRaw(a, signal_distort(g, slotRawObject(a)));
                break;
            case opSoftClip:
                SetRaw(a, signal_softclip(g, slotRawObject(a)));
                break;
            default:
                goto send_normal_1;
            }
        } else {
            goto send_normal_1;
        }
        break;
    default: // double
        switch (opcode) {
        case opNeg:
            SetRaw(a, -slotRawFloat(a));
            break;
        case opIsNil:
            SetFalse(a);
            break;
        case opNotNil:
            SetTrue(a);
            break;
        case opBitNot:
            SetRaw(a, ~(int)slotRawFloat(a));
            break;
        case opAbs:
            SetRaw(a, sc_abs(slotRawFloat(a)));
            break;
        case opAsFloat:
            SetRaw(a, (double)slotRawFloat(a));
            break;
        case opAsInt: {
            double val = slotRawFloat(a);
            if (val == std::numeric_limits<double>::infinity())
                SetInt(a, std::numeric_limits<int>::max());
            else
                SetInt(a, (int)val);
            break;
        }
        case opCeil:
            SetRaw(a, ceil(slotRawFloat(a)));
            break;
        case opFloor:
            SetRaw(a, floor(slotRawFloat(a)));
            break;
        case opFrac:
            SetRaw(a, sc_frac(slotRawFloat(a)));
            break;
        case opSign:
            SetRaw(a, slotRawFloat(a) > 0. ? 1.0 : (slotRawFloat(a) == 0 ? 0.0 : -1.0));
            break;
        case opSquared:
            SetRaw(a, slotRawFloat(a) * slotRawFloat(a));
            break;
        case opCubed:
            SetRaw(a, slotRawFloat(a) * slotRawFloat(a) * slotRawFloat(a));
            break;
        case opSqrt:
            SetRaw(a, sqrt(slotRawFloat(a)));
            break;
        case opExp:
            SetRaw(a, exp(slotRawFloat(a)));
            break;
        case opRecip:
            SetRaw(a, 1. / slotRawFloat(a));
            break;
        case opMIDICPS:
            SetRaw(a, sc_midicps(slotRawFloat(a)));
            break;
        case opCPSMIDI:
            SetRaw(a, sc_cpsmidi(slotRawFloat(a)));
            break;
        case opMIDIRatio:
            SetRaw(a, sc_midiratio((double)slotRawFloat(a)));
            break;
        case opRatioMIDI:
            SetRaw(a, sc_ratiomidi((double)slotRawFloat(a)));
            break;
        case opAmpDb:
            SetRaw(a, sc_ampdb(slotRawFloat(a)));
            break;
        case opDbAmp:
            SetRaw(a, sc_dbamp(slotRawFloat(a)));
            break;
        case opOctCPS:
            SetRaw(a, sc_octcps(slotRawFloat(a)));
            break;
        case opCPSOct:
            SetRaw(a, sc_cpsoct(slotRawFloat(a)));
            break;
        case opLog:
            SetRaw(a, log(slotRawFloat(a)));
            break;
        case opLog2:
            SetRaw(a, sc_log2(slotRawFloat(a)));
            break;
        case opLog10:
            SetRaw(a, log10(slotRawFloat(a)));
            break;
        case opSin:
            SetRaw(a, sin(slotRawFloat(a)));
            break;
        case opCos:
            SetRaw(a, cos(slotRawFloat(a)));
            break;
        case opTan:
            SetRaw(a, tan(slotRawFloat(a)));
            break;
        case opArcSin:
            SetRaw(a, asin(slotRawFloat(a)));
            break;
        case opArcCos:
            SetRaw(a, acos(slotRawFloat(a)));
            break;
        case opArcTan:
            SetRaw(a, atan(slotRawFloat(a)));
            break;
        case opSinH:
            SetRaw(a, sinh(slotRawFloat(a)));
            break;
        case opCosH:
            SetRaw(a, cosh(slotRawFloat(a)));
            break;
        case opTanH:
            SetRaw(a, tanh(slotRawFloat(a)));
            break;
        case opRand:
            SetRaw(a, g->rgen->frand() * slotRawFloat(a));
            break;
        case opRand2:
            SetRaw(a, g->rgen->frand2() * slotRawFloat(a));
            break;
        case opLinRand:
            SetRaw(a, g->rgen->linrand(slotRawFloat(a)));
            break;
        case opBiLinRand:
            SetRaw(a, g->rgen->bilinrand(slotRawFloat(a)));
            break;

            //				case opExpRand   : SetRaw(a, g->rgen->exprand(slotRawFloat(a))); break;
            //				case opBiExpRand : SetRaw(a, g->rgen->biexprand(slotRawFloat(a))); break;
        case opSum3Rand:
            SetRaw(a, g->rgen->sum3rand(slotRawFloat(a)));
            break;

            //				case opGammaRand : SetRaw(a, g->rgen->gammarand(slotRawFloat(a))); break;
            //				case opGaussRand : SetRaw(a, g->rgen->gaussrand(slotRawFloat(a))); break;
            //				case opPoiRand   : SetRaw(a, g->rgen->poirand(slotRawFloat(a))); break;

        case opDistort:
            SetRaw(a, sc_distort(slotRawFloat(a)));
            break;
        case opSoftClip:
            SetRaw(a, sc_softclip(slotRawFloat(a)));
            break;
        case opCoin:
            SetBool(a, g->rgen->frand() < slotRawFloat(a));
            break;
        case opRectWindow:
            SetRaw(a, sc_rectwindow(slotRawFloat(a)));
            break;
        case opHanWindow:
            SetRaw(a, sc_hanwindow(slotRawFloat(a)));
            break;
        case opWelchWindow:
            SetRaw(a, sc_welwindow(slotRawFloat(a)));
            break;
        case opTriWindow:
            SetRaw(a, sc_triwindow(slotRawFloat(a)));
            break;
        case opSCurve:
            SetRaw(a, sc_scurve(slotRawFloat(a)));
            break;
        case opRamp:
            SetRaw(a, sc_ramp(slotRawFloat(a)));
            break;
        default:
            goto send_normal_1;
        }
        break;
    }

#if TAILCALLOPTIMIZE
    g->tailCall = 0;
#endif
    return errNone;

send_normal_1:
    if (numArgsPushed != -1) // special case flag meaning it is a primitive
        return errFailed;

    msg = gSpecialUnarySelectors[opcode];
    sendMessage(g, msg, 1);

    return errNone;
}

int prSpecialBinaryArithMsg(VMGlobals* g, int numArgsPushed) { return doSpecialBinaryArithMsg(g, numArgsPushed, true); }

int doSpecialBinaryArithMsg(VMGlobals* g, int numArgsPushed, bool isPrimitive) {
    PyrSlot *a, *b;
    PyrSymbol* msg;
    int opcode = g->primitiveIndex;

    a = g->sp - (numArgsPushed - 1);
    b = a + 1;

    switch (GetTag(a)) {
    case tagInt: {
        switch (GetTag(b)) {
        case tagInt:
            switch (opcode) {
            case opAdd:
                SetRaw(a, slotRawInt(a) + slotRawInt(b));
                break;
            case opSub:
                SetRaw(a, slotRawInt(a) - slotRawInt(b));
                break;
            case opMul:
                SetRaw(a, slotRawInt(a) * slotRawInt(b));
                break;
            case opIDiv:
                SetRaw(a, sc_div(slotRawInt(a), slotRawInt(b)));
                break;
            case opFDiv:
                SetFloat(a, (double)slotRawInt(a) / (double)slotRawInt(b));
                break;
            case opMod:
                SetRaw(a, sc_mod((int)slotRawInt(a), (int)slotRawInt(b)));
                break;
            case opEQ:
                SetBool(a, slotRawInt(a) == slotRawInt(b));
                break;
            case opNE:
                SetBool(a, slotRawInt(a) != slotRawInt(b));
                break;
            case opLT:
                SetBool(a, slotRawInt(a) < slotRawInt(b));
                break;
            case opGT:
                SetBool(a, slotRawInt(a) > slotRawInt(b));
                break;
            case opLE:
                SetBool(a, slotRawInt(a) <= slotRawInt(b));
                break;
            case opGE:
                SetBool(a, slotRawInt(a) >= slotRawInt(b));
                break;
            // case opIdentical : SetBool(a, slotRawInt(a) == slotRawInt(b)); break;
            // case opNotIdentical : SetBool(a, slotRawInt(a) != slotRawInt(b)); break;
            case opMin:
                SetRaw(a, sc_min(slotRawInt(a), slotRawInt(b)));
                break;
            case opMax:
                SetRaw(a, sc_max(slotRawInt(a), slotRawInt(b)));
                break;
            case opBitAnd:
                SetRaw(a, slotRawInt(a) & slotRawInt(b));
                break;
            case opBitOr:
                SetRaw(a, slotRawInt(a) | slotRawInt(b));
                break;
            case opBitXor:
                SetRaw(a, slotRawInt(a) ^ slotRawInt(b));
                break;
            case opLCM:
                SetRaw(a, sc_lcm((long)slotRawInt(a), (long)slotRawInt(b)));
                break;
            case opGCD:
                SetRaw(a, sc_gcd((long)slotRawInt(a), (long)slotRawInt(b)));
                break;
            case opRound:
                SetRaw(a, sc_round((int)slotRawInt(a), (int)slotRawInt(b)));
                break;
            case opRoundUp:
                SetRaw(a, sc_roundUp((int)slotRawInt(a), (int)slotRawInt(b)));
                break;
            case opTrunc:
                SetRaw(a, sc_trunc((int)slotRawInt(a), (int)slotRawInt(b)));
                break;
            case opAtan2:
                SetFloat(a, atan2((double)slotRawInt(a), (double)slotRawInt(b)));
                break;
            case opHypot:
                SetFloat(a, hypot((double)slotRawInt(a), (double)slotRawInt(b)));
                break;
            case opHypotx:
                SetFloat(a, hypotx((double)slotRawInt(a), (double)slotRawInt(b)));
                break;
            case opPow:
                SetFloat(a, pow((double)slotRawInt(a), (double)slotRawInt(b)));
                break;
            case opShiftLeft: {
                long ia = slotRawInt(a);
                long ib = slotRawInt(b);
                if (ib > 0)
                    ia <<= ib;
                else if (ib < 0)
                    ia >>= -ib;
                SetRaw(a, ia);
            } break;
            case opShiftRight: {
                long ia = slotRawInt(a);
                long ib = slotRawInt(b);
                if (ib > 0)
                    ia >>= ib;
                else if (ib < 0)
                    ia <<= -ib;
                SetRaw(a, ia);
            } break;
            case opUnsignedShift: {
                unsigned long ia = slotRawInt(a);
                long ib = slotRawInt(b);
                if (ib > 0)
                    ia >>= ib;
                else if (ib < 0)
                    ia <<= -ib;
                SetRaw(a, (long)ia);
            } break;
            case opRing1:
                SetRaw(a, sc_ring1(slotRawInt(a), slotRawInt(b)));
                break;
            case opRing2:
                SetRaw(a, sc_ring2(slotRawInt(a), slotRawInt(b)));
                break;
            case opRing3:
                SetRaw(a, sc_ring3(slotRawInt(a), slotRawInt(b)));
                break;
            case opRing4:
                SetRaw(a, sc_ring4(slotRawInt(a), slotRawInt(b)));
                break;
            case opDifSqr:
                SetRaw(a, sc_difsqr(slotRawInt(a), slotRawInt(b)));
                break;
            case opSumSqr:
                SetRaw(a, sc_sumsqr(slotRawInt(a), slotRawInt(b)));
                break;
            case opSqrSum:
                SetRaw(a, sc_sqrsum(slotRawInt(a), slotRawInt(b)));
                break;
            case opSqrDif:
                SetRaw(a, sc_sqrdif(slotRawInt(a), slotRawInt(b)));
                break;
            case opAbsDif:
                SetRaw(a, sc_abs(slotRawInt(a) - slotRawInt(b)));
                break;
            case opThresh:
                SetRaw(a, sc_thresh(slotRawInt(a), slotRawInt(b)));
                break;
            case opAMClip:
                SetRaw(a, sc_amclip(slotRawInt(a), slotRawInt(b)));
                break;
            case opScaleNeg:
                SetRaw(a, sc_scaleneg(slotRawInt(a), slotRawInt(b)));
                break;
            case opClip2:
                SetRaw(a, sc_clip2(slotRawInt(a), slotRawInt(b)));
                break;
            case opFold2:
                SetRaw(a, sc_fold2(slotRawInt(a), slotRawInt(b)));
                break;
            case opWrap2:
                SetRaw(a, sc_wrap2(slotRawInt(a), slotRawInt(b)));
                break;
            case opExcess:
                SetRaw(a, sc_excess(slotRawInt(a), slotRawInt(b)));
                break;
            case opFirstArg:
                SetRaw(a, slotRawInt(a));
                break;
            case opRandRange:
                SetRaw(a,
                       slotRawInt(b) > slotRawInt(a)
                           ? slotRawInt(a) + g->rgen->irand(slotRawInt(b) - slotRawInt(a) + 1)
                           : slotRawInt(b) + g->rgen->irand(slotRawInt(a) - slotRawInt(b) + 1));
                break;
            case opExpRandRange:
                SetFloat(a, g->rgen->exprandrng(slotRawInt(a), slotRawInt(b)));
                break;
            default:
                goto send_normal_2;
            }
            break;
        case tagChar:
        case tagPtr:
        case tagNil:
        case tagFalse:
        case tagTrue:
            goto send_normal_2;
        case tagSym:
            if (IS_BINARY_BOOL_OP(opcode))
                SetBool(a, opcode == opNE);
            else
                SetSymbol(a, slotRawSymbol(b));
            break;
        case tagObj:
            if (isKindOf(slotRawObject(b), class_signal)) {
                switch (opcode) {
                case opAdd:
                    SetObject(a, signal_add_xf(g, slotRawObject(b), slotRawInt(a)));
                    break;
                case opSub:
                    SetObject(a, signal_sub_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opMul:
                    SetObject(a, signal_mul_xf(g, slotRawObject(b), slotRawInt(a)));
                    break;
                case opIDiv:
                    SetObject(a, signal_div_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opFDiv:
                    SetObject(a, signal_div_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opEQ:
                    SetFalse(a);
                    break;
                case opNE:
                    SetTrue(a);
                    break;
                case opMin:
                    SetObject(a, signal_min_xf(g, slotRawObject(b), slotRawInt(a)));
                    break;
                case opMax:
                    SetObject(a, signal_max_xf(g, slotRawObject(b), slotRawInt(a)));
                    break;
                case opRing1:
                    SetObject(a, signal_ring1_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opRing2:
                    SetObject(a, signal_ring2_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opRing3:
                    SetObject(a, signal_ring3_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opRing4:
                    SetObject(a, signal_ring4_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opDifSqr:
                    SetObject(a, signal_difsqr_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opSumSqr:
                    SetObject(a, signal_sumsqr_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opSqrSum:
                    SetObject(a, signal_sqrsum_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opSqrDif:
                    SetObject(a, signal_sqrdif_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opAbsDif:
                    SetObject(a, signal_absdif_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opThresh:
                    SetObject(a, signal_thresh_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opAMClip:
                    SetObject(a, signal_amclip_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opScaleNeg:
                    SetObject(a, signal_scaleneg_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opClip2:
                    SetObject(a, signal_clip2_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opFold2:
                    SetObject(a, signal_fold2_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opWrap2:
                    SetObject(a, signal_wrap2_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opExcess:
                    SetObject(a, signal_excess_fx(g, slotRawInt(a), slotRawObject(b)));
                    break;
                case opFirstArg:
                    SetObject(a, slotRawObject(a));
                    break;
                default:
                    goto send_normal_2;
                }
            } else {
                goto send_normal_2;
            }
            break;
        default:
            switch (opcode) {
            case opAdd:
                SetFloat(a, slotRawInt(a) + slotRawFloat(b));
                break;
            case opSub:
                SetFloat(a, slotRawInt(a) - slotRawFloat(b));
                break;
            case opMul:
                SetFloat(a, slotRawInt(a) * slotRawFloat(b));
                break;
            case opIDiv:
                SetRaw(a, (long)floor(slotRawInt(a) / slotRawFloat(b)));
                break;
            case opFDiv:
                SetFloat(a, slotRawInt(a) / slotRawFloat(b));
                break;
            case opMod:
                SetFloat(a, sc_mod((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opEQ:
                SetBool(a, slotRawInt(a) == slotRawFloat(b));
                break;
            case opNE:
                SetBool(a, slotRawInt(a) != slotRawFloat(b));
                break;
            case opLT:
                SetBool(a, slotRawInt(a) < slotRawFloat(b));
                break;
            case opGT:
                SetBool(a, slotRawInt(a) > slotRawFloat(b));
                break;
            case opLE:
                SetBool(a, slotRawInt(a) <= slotRawFloat(b));
                break;
            case opGE:
                SetBool(a, slotRawInt(a) >= slotRawFloat(b));
                break;
            // case opIdentical : SetFalse(a); break;
            // case opNotIdentical : SetTrue(a); break;
            case opMin:
                SetFloat(a, sc_min((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opMax:
                SetFloat(a, sc_max((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRound:
                SetFloat(a, sc_round((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRoundUp:
                SetFloat(a, sc_roundUp((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opTrunc:
                SetFloat(a, sc_trunc((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opAtan2:
                SetFloat(a, atan2(slotRawInt(a), slotRawFloat(b)));
                break;
            case opHypot:
                SetFloat(a, hypot(slotRawInt(a), slotRawFloat(b)));
                break;
            case opHypotx:
                SetFloat(a, hypotx(slotRawInt(a), slotRawFloat(b)));
                break;
            case opPow:
                SetFloat(a, pow((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRing1:
                SetFloat(a, sc_ring1((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRing2:
                SetFloat(a, sc_ring2((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRing3:
                SetFloat(a, sc_ring3((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opRing4:
                SetFloat(a, sc_ring4((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opDifSqr:
                SetFloat(a, sc_difsqr((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opSumSqr:
                SetFloat(a, sc_sumsqr((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opSqrSum:
                SetFloat(a, sc_sqrsum((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opSqrDif:
                SetFloat(a, sc_sqrdif((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opAbsDif:
                SetFloat(a, sc_abs(slotRawInt(a) - slotRawFloat(b)));
                break;
            case opThresh:
                SetRaw(a, sc_thresh(slotRawInt(a), slotRawFloat(b)));
                break;
            case opAMClip:
                SetFloat(a, sc_amclip((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opScaleNeg:
                SetFloat(a, sc_scaleneg((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opClip2:
                SetFloat(a, sc_clip2((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opFold2:
                SetFloat(a, sc_fold2((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opWrap2:
                SetFloat(a, sc_wrap2((double)slotRawInt(a), -slotRawFloat(b)));
                break;
            case opExcess:
                SetFloat(a, sc_excess((double)slotRawInt(a), slotRawFloat(b)));
                break;
            case opFirstArg:
                SetInt(a, slotRawInt(a));
                break;
            case opRandRange:
                SetFloat(a, slotRawInt(a) + g->rgen->frand() * (slotRawFloat(b) - slotRawInt(a)));
                break;
            case opExpRandRange:
                SetFloat(a, g->rgen->exprandrng(slotRawInt(a), slotRawFloat(b)));
                break;
            default:
                goto send_normal_2;
            }
            break;
        }
    } break;
    case tagChar: {
        if (IsChar(b)) {
            switch (opcode) {
            case opEQ:
                SetBool(a, slotRawChar(a) == slotRawChar(b));
                break;
            case opNE:
                SetBool(a, slotRawChar(a) != slotRawChar(b));
                break;
            case opLT:
                SetBool(a, slotRawChar(a) < slotRawChar(b));
                break;
            case opGT:
                SetBool(a, slotRawChar(a) > slotRawChar(b));
                break;
            case opLE:
                SetBool(a, slotRawChar(a) <= slotRawChar(b));
                break;
            case opGE:
                SetBool(a, slotRawChar(a) >= slotRawChar(b));
                break;
            // case opIdentical : SetBool(a, slotRawChar(a) == slotRawChar(b)); break;
            // case opNotIdentical : SetBool(a, slotRawChar(a) != slotRawChar(b)); break;
            case opMin:
                SetRawChar(a, sc_min(slotRawChar(a), slotRawChar(b)));
                break;
            case opMax:
                SetRawChar(a, sc_max(slotRawChar(a), slotRawChar(b)));
                break;
            default:
                goto send_normal_2;
            }
        } else {
            goto send_normal_2;
        }
    } break;
    case tagPtr:
    case tagNil:
    case tagFalse:
    case tagTrue:
        goto send_normal_2;
    case tagSym:
        if (IsSym(b)) {
            switch (opcode) {
            case opEQ:
                SetBool(a, slotRawSymbol(a) == slotRawSymbol(b));
                break;
            case opNE:
                SetBool(a, slotRawSymbol(a) != slotRawSymbol(b));
                break;
            case opLT:
                SetBool(a, strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) < 0);
                break;
            case opGT:
                SetBool(a, strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) > 0);
                break;
            case opLE:
                SetBool(a, strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) <= 0);
                break;
            case opGE:
                SetBool(a, strcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name) >= 0);
                break;
                // default : leave first operand on stack
            }
        } else {
            if (IS_BINARY_BOOL_OP(opcode))
                SetBool(a, opcode == opNE);
        }
        break;
    case tagObj: {
        if (isKindOf(slotRawObject(a), class_signal)) {
            switch (GetTag(b)) {
            case tagInt:
                switch (opcode) {
                case opAdd:
                    SetRaw(a, signal_add_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opSub:
                    SetRaw(a, signal_sub_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opMul:
                    SetRaw(a, signal_mul_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opIDiv:
                    SetRaw(a, signal_div_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opFDiv:
                    SetRaw(a, signal_div_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opEQ:
                    SetFalse(a);
                    break;
                case opNE:
                    SetTrue(a);
                    break;
                case opMin:
                    SetRaw(a, signal_min_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opMax:
                    SetRaw(a, signal_max_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opFill:
                    SetRaw(a, signal_fill(slotRawObject(a), slotRawInt(b)));
                    break;
                case opRing1:
                    SetRaw(a, signal_ring1_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opRing2:
                    SetRaw(a, signal_ring2_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opRing3:
                    SetRaw(a, signal_ring3_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opRing4:
                    SetRaw(a, signal_ring4_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opDifSqr:
                    SetRaw(a, signal_difsqr_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opSumSqr:
                    SetRaw(a, signal_sumsqr_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opSqrSum:
                    SetRaw(a, signal_sqrsum_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opSqrDif:
                    SetRaw(a, signal_sqrdif_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opAbsDif:
                    SetRaw(a, signal_absdif_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opThresh:
                    SetRaw(a, signal_thresh_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opAMClip:
                    SetRaw(a, signal_amclip_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opScaleNeg:
                    SetRaw(a, signal_scaleneg_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opClip2:
                    SetRaw(a, signal_clip2_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opFold2:
                    SetRaw(a, signal_fold2_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opWrap2:
                    SetRaw(a, signal_wrap2_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opExcess:
                    SetRaw(a, signal_excess_xf(g, slotRawObject(a), slotRawInt(b)));
                    break;
                case opFirstArg:
                    SetRaw(a, slotRawObject(a));
                    break;
                default:
                    goto send_normal_2;
                }
                break;
            case tagChar:
            case tagPtr:
            case tagNil:
            case tagFalse:
            case tagTrue:
                goto send_normal_2;
            case tagSym:
                if (IS_BINARY_BOOL_OP(opcode))
                    SetBool(a, opcode == opNE);
                else
                    SetSymbol(a, slotRawSymbol(b));
                break;
            case tagObj:
                if (isKindOf(slotRawObject(b), class_signal)) {
                    switch (opcode) {
                    case opAdd:
                        SetRaw(a, signal_add_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opSub:
                        SetRaw(a, signal_sub_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opMul:
                        SetRaw(a, signal_mul_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opIDiv:
                        SetRaw(a, signal_div_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opFDiv:
                        SetRaw(a, signal_div_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opEQ:
                        SetBool(a, signal_equal_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opNE:
                        SetBool(a, !signal_equal_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opMin:
                        SetRaw(a, signal_min_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opMax:
                        SetRaw(a, signal_max_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opRing1:
                        SetRaw(a, signal_ring1_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opRing2:
                        SetRaw(a, signal_ring2_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opRing3:
                        SetRaw(a, signal_ring3_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opRing4:
                        SetRaw(a, signal_ring4_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opDifSqr:
                        SetRaw(a, signal_difsqr_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opSumSqr:
                        SetRaw(a, signal_sumsqr_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opSqrSum:
                        SetRaw(a, signal_sqrsum_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opSqrDif:
                        SetRaw(a, signal_sqrdif_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opAbsDif:
                        SetRaw(a, signal_absdif_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opThresh:
                        SetRaw(a, signal_thresh_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opAMClip:
                        SetRaw(a, signal_amclip_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opScaleNeg:
                        SetRaw(a, signal_scaleneg_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opClip2:
                        SetRaw(a, signal_clip2_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opFold2:
                        SetRaw(a, signal_fold2_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opWrap2:
                        SetRaw(a, signal_wrap2_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opExcess:
                        SetRaw(a, signal_excess_xx(g, slotRawObject(a), slotRawObject(b)));
                        break;
                    case opFirstArg:
                        SetRaw(a, slotRawObject(a));
                        break;
                    default:
                        goto send_normal_2;
                    }
                } else
                    goto send_normal_2;
                break;
            default: // double
                switch (opcode) {
                case opAdd:
                    SetRaw(a, signal_add_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opSub:
                    SetRaw(a, signal_sub_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opMul:
                    SetRaw(a, signal_mul_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opIDiv:
                    SetRaw(a, signal_div_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opFDiv:
                    SetRaw(a, signal_div_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opEQ:
                    SetFalse(a);
                    break;
                case opNE:
                    SetTrue(a);
                    break;
                case opMin:
                    SetRaw(a, signal_min_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opMax:
                    SetRaw(a, signal_max_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opFill:
                    SetRaw(a, signal_fill(slotRawObject(a), slotRawFloat(b)));
                    break;
                case opRing1:
                    SetRaw(a, signal_ring1_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opRing2:
                    SetRaw(a, signal_ring2_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opRing3:
                    SetRaw(a, signal_ring3_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opRing4:
                    SetRaw(a, signal_ring4_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opDifSqr:
                    SetRaw(a, signal_difsqr_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opSumSqr:
                    SetRaw(a, signal_sumsqr_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opSqrSum:
                    SetRaw(a, signal_sqrsum_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opSqrDif:
                    SetRaw(a, signal_sqrdif_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opAbsDif:
                    SetRaw(a, signal_absdif_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opThresh:
                    SetRaw(a, signal_thresh_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opAMClip:
                    SetRaw(a, signal_amclip_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opScaleNeg:
                    SetRaw(a, signal_scaleneg_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opClip2:
                    SetRaw(a, signal_clip2_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opFold2:
                    SetRaw(a, signal_fold2_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opWrap2:
                    SetRaw(a, signal_wrap2_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opExcess:
                    SetRaw(a, signal_excess_xf(g, slotRawObject(a), slotRawFloat(b)));
                    break;
                case opFirstArg:
                    SetRaw(a, slotRawObject(a));
                    break;
                default:
                    goto send_normal_2;
                }
                break;
            }
        } else {
            goto send_normal_2;
        }
    } break;
    default: { // double
        switch (GetTag(b)) {
        case tagInt:
            switch (opcode) {
            case opAdd:
                SetRaw(a, slotRawFloat(a) + slotRawInt(b));
                break;
            case opSub:
                SetRaw(a, slotRawFloat(a) - slotRawInt(b));
                break;
            case opMul:
                SetRaw(a, slotRawFloat(a) * slotRawInt(b));
                break;
            case opIDiv:
                SetInt(a, (long)floor(slotRawFloat(a) / slotRawInt(b)));
                break;
            case opFDiv:
                SetRaw(a, slotRawFloat(a) / slotRawInt(b));
                break;
            case opMod:
                SetRaw(a, sc_mod(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opEQ:
                SetBool(a, slotRawFloat(a) == slotRawInt(b));
                break;
            case opNE:
                SetBool(a, slotRawFloat(a) != slotRawInt(b));
                break;
            case opLT:
                SetBool(a, slotRawFloat(a) < slotRawInt(b));
                break;
            case opGT:
                SetBool(a, slotRawFloat(a) > slotRawInt(b));
                break;
            case opLE:
                SetBool(a, slotRawFloat(a) <= slotRawInt(b));
                break;
            case opGE:
                SetBool(a, slotRawFloat(a) >= slotRawInt(b));
                break;
            // case opIdentical : SetFalse(a); break;
            // case opNotIdentical : SetTrue(a); break;
            case opMin:
                SetRaw(a, sc_min(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opMax:
                SetRaw(a, sc_max(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRound:
                SetRaw(a, sc_round(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRoundUp:
                SetRaw(a, sc_roundUp(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opTrunc:
                SetRaw(a, sc_trunc(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opAtan2:
                SetRaw(a, atan2(slotRawFloat(a), slotRawInt(b)));
                break;
            case opHypot:
                SetRaw(a, hypot(slotRawFloat(a), slotRawInt(b)));
                break;
            case opHypotx:
                SetRaw(a, hypotx(slotRawFloat(a), slotRawInt(b)));
                break;
            case opPow:
                SetRaw(a, pow(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRing1:
                SetRaw(a, sc_ring1(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRing2:
                SetRaw(a, sc_ring2(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRing3:
                SetRaw(a, sc_ring3(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opRing4:
                SetRaw(a, sc_ring4(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opDifSqr:
                SetRaw(a, sc_difsqr(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opSumSqr:
                SetRaw(a, sc_sumsqr(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opSqrSum:
                SetRaw(a, sc_sqrsum(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opSqrDif:
                SetRaw(a, sc_sqrdif(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opAbsDif:
                SetRaw(a, sc_abs(slotRawFloat(a) - slotRawInt(b)));
                break;
            case opThresh:
                SetRaw(a, sc_thresh(slotRawFloat(a), slotRawInt(b)));
                break;
            case opAMClip:
                SetRaw(a, sc_amclip(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opScaleNeg:
                SetRaw(a, sc_scaleneg(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opClip2:
                SetRaw(a, sc_clip2(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opFold2:
                SetRaw(a, sc_fold2(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opWrap2:
                SetRaw(a, sc_wrap2(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opExcess:
                SetRaw(a, sc_excess(slotRawFloat(a), (double)slotRawInt(b)));
                break;
            case opFirstArg:
                SetRaw(a, slotRawFloat(a));
                break;
            case opRandRange:
                SetRaw(a, slotRawFloat(a) + g->rgen->frand() * (slotRawInt(b) - slotRawFloat(a)));
                break;
            case opExpRandRange:
                SetRaw(a, g->rgen->exprandrng(slotRawFloat(a), slotRawInt(b)));
                break;
            default:
                goto send_normal_2;
            }
            break;
        case tagChar:
        case tagPtr:
        case tagNil:
        case tagFalse:
        case tagTrue:
            goto send_normal_2;
        case tagSym:
            if (IS_BINARY_BOOL_OP(opcode))
                SetBool(a, opcode == opNE);
            else
                SetSymbol(a, slotRawSymbol(b));
            break;
        case tagObj:
            if (isKindOf(slotRawObject(b), class_signal)) {
                switch (opcode) {
                case opAdd:
                    SetObject(a, signal_add_xf(g, slotRawObject(b), slotRawFloat(a)));
                    break;
                case opSub:
                    SetObject(a, signal_sub_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opMul:
                    SetObject(a, signal_mul_xf(g, slotRawObject(b), slotRawFloat(a)));
                    break;
                case opIDiv:
                    SetObject(a, signal_div_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opFDiv:
                    SetObject(a, signal_div_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opEQ:
                    SetFalse(a);
                    break;
                case opNE:
                    SetTrue(a);
                    break;
                case opMin:
                    SetObject(a, signal_min_xf(g, slotRawObject(b), slotRawFloat(a)));
                    break;
                case opMax:
                    SetObject(a, signal_max_xf(g, slotRawObject(b), slotRawFloat(a)));
                    break;
                case opRing1:
                    SetObject(a, signal_ring1_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opRing2:
                    SetObject(a, signal_ring2_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opRing3:
                    SetObject(a, signal_ring3_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opRing4:
                    SetObject(a, signal_ring4_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opDifSqr:
                    SetObject(a, signal_difsqr_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opSumSqr:
                    SetObject(a, signal_sumsqr_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opSqrSum:
                    SetObject(a, signal_sqrsum_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opSqrDif:
                    SetObject(a, signal_sqrdif_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opAbsDif:
                    SetObject(a, signal_absdif_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opThresh:
                    SetObject(a, signal_thresh_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opAMClip:
                    SetObject(a, signal_amclip_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opScaleNeg:
                    SetObject(a, signal_scaleneg_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opClip2:
                    SetObject(a, signal_clip2_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opFold2:
                    SetObject(a, signal_fold2_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opWrap2:
                    SetObject(a, signal_wrap2_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opExcess:
                    SetObject(a, signal_excess_fx(g, slotRawFloat(a), slotRawObject(b)));
                    break;
                case opFirstArg:
                    SetObject(a, slotRawObject(a));
                    break;
                default:
                    goto send_normal_2;
                }
            } else
                goto send_normal_2;
            break;
        default: // double
            switch (opcode) {
            case opAdd:
                SetRaw(a, slotRawFloat(a) + slotRawFloat(b));
                break;
            case opSub:
                SetRaw(a, slotRawFloat(a) - slotRawFloat(b));
                break;
            case opMul:
                SetRaw(a, slotRawFloat(a) * slotRawFloat(b));
                break;
            case opIDiv:
                SetInt(a, (long)floor(slotRawFloat(a) / slotRawFloat(b)));
                break;
            case opFDiv:
                SetRaw(a, slotRawFloat(a) / slotRawFloat(b));
                break;
            case opMod:
                SetRaw(a, sc_mod(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opEQ:
                SetBool(a, slotRawFloat(a) == slotRawFloat(b));
                break;
            case opNE:
                SetBool(a, slotRawFloat(a) != slotRawFloat(b));
                break;
            case opLT:
                SetBool(a, slotRawFloat(a) < slotRawFloat(b));
                break;
            case opGT:
                SetBool(a, slotRawFloat(a) > slotRawFloat(b));
                break;
            case opLE:
                SetBool(a, slotRawFloat(a) <= slotRawFloat(b));
                break;
            case opGE:
                SetBool(a, slotRawFloat(a) >= slotRawFloat(b));
                break;
            // case opIdentical  : SetBool(a, slotRawFloat(a) == slotRawFloat(b)); break;
            // case opNotIdentical  : SetBool(a, slotRawFloat(a) != slotRawFloat(b)); break;
            case opMin:
                SetRaw(a, sc_min(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opMax:
                SetRaw(a, sc_max(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRound:
                SetRaw(a, sc_round(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRoundUp:
                SetRaw(a, sc_roundUp(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opTrunc:
                SetRaw(a, sc_trunc(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opAtan2:
                SetRaw(a, atan2(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opHypot:
                SetRaw(a, hypot(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opHypotx:
                SetRaw(a, hypotx(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opPow:
                SetRaw(a, pow(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRing1:
                SetRaw(a, sc_ring1(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRing2:
                SetRaw(a, sc_ring2(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRing3:
                SetRaw(a, sc_ring3(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opRing4:
                SetRaw(a, sc_ring4(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opDifSqr:
                SetRaw(a, sc_difsqr(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opSumSqr:
                SetRaw(a, sc_sumsqr(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opSqrSum:
                SetRaw(a, sc_sqrsum(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opSqrDif:
                SetRaw(a, sc_sqrdif(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opAbsDif:
                SetRaw(a, sc_abs(slotRawFloat(a) - slotRawFloat(b)));
                break;
            case opThresh:
                SetRaw(a, sc_thresh(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opAMClip:
                SetRaw(a, sc_amclip(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opScaleNeg:
                SetRaw(a, sc_scaleneg(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opClip2:
                SetRaw(a, sc_clip2(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opFold2:
                SetRaw(a, sc_fold2(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opWrap2:
                SetRaw(a, sc_wrap2(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opExcess:
                SetRaw(a, sc_excess(slotRawFloat(a), slotRawFloat(b)));
                break;
            case opFirstArg:
                SetRaw(a, slotRawFloat(a));
                break;
            case opRandRange:
                SetRaw(a, slotRawFloat(a) + g->rgen->frand() * (slotRawFloat(b) - slotRawFloat(a)));
                break;
            case opExpRandRange:
                SetRaw(a, g->rgen->exprandrng(slotRawFloat(a), slotRawFloat(b)));
                break;
            default:
                goto send_normal_2;
            }
            break;
        }
    } break;
    }
    g->sp = a; // drop
    g->numpop = 0;
#if TAILCALLOPTIMIZE
    g->tailCall = 0;
#endif
    return errNone;

send_normal_2:
    if (isPrimitive) // special case flag meaning it is a primitive
        return errFailed; // arguments remain on the stack

    msg = gSpecialBinarySelectors[opcode];
    sendMessage(g, msg, numArgsPushed);
    return errNone;
}
