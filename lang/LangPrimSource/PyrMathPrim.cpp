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
#include "PyrPrimitive.h"
#include "PyrMathPrim.h"
#include "MiscInlineMath.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSignal.h"
#include "PyrParseNode.h"
#include "PyrMessage.h"
#include "clz.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SC_Endian.h"
#include "SCBase.h"

#define BOOST_MATH_DISABLE_FLOAT128 1
#include "boost/math/special_functions.hpp"
#include "boost/math/special_functions/spherical_harmonic.hpp"
#include "boost/math/special_functions/legendre.hpp"
#include "PyrSymbol.h"
#include <vector>

const int INT_MAX_BY_PyrSlot = INT_MAX / sizeof(PyrSlot);

inline bool IsSignal(PyrSlot* slot) { return (IsObj(slot) && slotRawObject(slot)->classptr == class_signal); }
inline bool NotSignal(PyrSlot* slot) { return (NotObj(slot) || slotRawObject(slot)->classptr != class_signal); }


/* functors for dispatching template code */

struct addNum {
    static inline double run(double lhs, double rhs) { return lhs + rhs; }
    static inline int run(int lhs, int rhs) { return lhs + rhs; }
    static inline PyrObject* signal_xf(VMGlobals* g, PyrObject* ina, float inb) { return signal_add_xf(g, ina, inb); }
    static inline PyrObject* signal_fx(VMGlobals* g, float ina, PyrObject* inb) { return signal_xf(g, inb, ina); }
    static inline PyrObject* signal_xx(VMGlobals* g, PyrObject* ina, PyrObject* inb) {
        return signal_add_xx(g, ina, inb);
    }
};

struct mulNum {
    static inline double run(double lhs, double rhs) { return lhs * rhs; }
    static inline int run(int lhs, int rhs) { return lhs * rhs; }
    static inline PyrObject* signal_xf(VMGlobals* g, PyrObject* ina, float inb) { return signal_mul_xf(g, ina, inb); }
    static inline PyrObject* signal_fx(VMGlobals* g, float ina, PyrObject* inb) { return signal_xf(g, inb, ina); }
    static inline PyrObject* signal_xx(VMGlobals* g, PyrObject* ina, PyrObject* inb) {
        return signal_mul_xx(g, ina, inb);
    }
};

struct subNum {
    static inline double run(double lhs, double rhs) { return lhs - rhs; }
    static inline int run(int lhs, int rhs) { return lhs - rhs; }
    static inline PyrObject* signal_xf(VMGlobals* g, PyrObject* ina, float inb) { return signal_sub_xf(g, ina, inb); }
    static inline PyrObject* signal_fx(VMGlobals* g, float ina, PyrObject* inb) { return signal_sub_fx(g, ina, inb); }
    static inline PyrObject* signal_xx(VMGlobals* g, PyrObject* ina, PyrObject* inb) {
        return signal_sub_xx(g, ina, inb);
    }
};

template <typename Functor> inline int prOpNum(VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrSymbol* msg;

    a = g->sp - 1;
    b = g->sp;

    switch (GetTag(a)) {
    case tagInt:
        switch (GetTag(b)) {
        case tagInt:
            SetRaw(a, Functor::run(slotRawInt(a), slotRawInt(b)));
            break;
        case tagChar:
        case tagPtr:
        case tagNil:
        case tagFalse:
        case tagTrue:
            goto send_normal_2;
        case tagSym:
            SetSymbol(a, slotRawSymbol(b));
            break;
        case tagObj:
            if (isKindOf(slotRawObject(b), class_signal))
                SetObject(a, Functor::signal_fx(g, slotRawInt(a), slotRawObject(b)));
            else
                goto send_normal_2;
            break;
        default:
            SetFloat(a, slotRawInt(a) + slotRawFloat(b));
            break;
        }
        break;
    case tagChar:
    case tagPtr:
    case tagNil:
    case tagFalse:
    case tagTrue:
        goto send_normal_2;
    case tagSym:
        // leave self in 'a'
        break;
    case tagObj:
        if (isKindOf(slotRawObject(a), class_signal)) {
            switch (GetTag(b)) {
            case tagInt:
                SetRaw(a, Functor::signal_xf(g, slotRawObject(a), slotRawInt(b)));
                break;
            case tagChar:
            case tagPtr:
            case tagNil:
            case tagFalse:
            case tagTrue:
                goto send_normal_2;
            case tagSym:
                SetSymbol(a, slotRawSymbol(b));
                break;
            case tagObj:
                if (isKindOf(slotRawObject(b), class_signal)) {
                    SetRaw(a, Functor::signal_xx(g, slotRawObject(a), slotRawObject(b)));
                } else
                    goto send_normal_2;
                break;
            default: // double
                SetRaw(a, Functor::signal_xf(g, slotRawObject(a), slotRawFloat(b)));
                break;
            }
        } else
            goto send_normal_2;
        break;
    default: // double
        switch (GetTag(b)) {
        case tagInt:
            SetRaw(a, Functor::run(slotRawFloat(a), (double)slotRawInt(b)));
            break;
        case tagChar:
        case tagPtr:
        case tagNil:
        case tagFalse:
        case tagTrue:
            goto send_normal_2;
        case tagSym:
            SetSymbol(a, slotRawSymbol(b));
            break;
        case tagObj:
            if (isKindOf(slotRawObject(b), class_signal))
                SetObject(a, Functor::signal_fx(g, slotRawFloat(a), slotRawObject(b)));
            else
                goto send_normal_2;
            break;
        default: // double
            SetRaw(a, Functor::run(slotRawFloat(a), slotRawFloat(b)));
            break;
        }
        break;
    }
    g->sp--; // drop
    g->numpop = 0;
#if TAILCALLOPTIMIZE
    g->tailCall = 0;
#endif
    return errNone;

send_normal_2:
    if (numArgsPushed != -1) // special case flag meaning it is a primitive
        return errFailed; // arguments remain on the stack

    msg = gSpecialBinarySelectors[g->primitiveIndex];
    sendMessage(g, msg, 2);
    return errNone;
}

template <typename Functor> inline int prOpInt(VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrSymbol* msg;

    a = g->sp - 1;
    b = g->sp;

    switch (GetTag(b)) {
    case tagInt:
        SetRaw(a, Functor::run(slotRawInt(a), slotRawInt(b)));
        break;
    case tagChar:
    case tagPtr:
    case tagNil:
    case tagFalse:
    case tagTrue:
        goto send_normal_2;
    case tagSym:
        SetSymbol(a, slotRawSymbol(b));
        break;
    case tagObj:
        if (isKindOf(slotRawObject(b), class_signal))
            SetObject(a, Functor::signal_fx(g, slotRawInt(a), slotRawObject(b)));
        else
            goto send_normal_2;
        break;
    default:
        SetFloat(a, Functor::run((double)slotRawInt(a), slotRawFloat(b)));
        break;
    }
    g->sp--; // drop
    g->numpop = 0;
#if TAILCALLOPTIMIZE
    g->tailCall = 0;
#endif
    return errNone;

send_normal_2:
    if (numArgsPushed != -1) // special case flag meaning it is a primitive
        return errFailed; // arguments remain on the stack

    msg = gSpecialBinarySelectors[g->primitiveIndex];
    sendMessage(g, msg, 2);
    return errNone;
}

template <typename Functor> inline int prOpFloat(VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    PyrSymbol* msg;

    a = g->sp - 1;
    b = g->sp;

    switch (GetTag(b)) {
    case tagInt:
        SetRaw(a, Functor::run(slotRawFloat(a), (double)slotRawInt(b)));
        break;
    case tagChar:
    case tagPtr:
    case tagNil:
    case tagFalse:
    case tagTrue:
        goto send_normal_2;
    case tagSym:
        SetSymbol(a, slotRawSymbol(b));
        break;
    case tagObj:
        if (isKindOf(slotRawObject(b), class_signal))
            SetObject(a, Functor::signal_fx(g, slotRawFloat(a), slotRawObject(b)));
        else
            goto send_normal_2;
        break;
    default:
        SetRaw(a, Functor::run(slotRawFloat(a), slotRawFloat(b)));
        break;
    }
    g->sp--; // drop
    g->numpop = 0;
#if TAILCALLOPTIMIZE
    g->tailCall = 0;
#endif
    return errNone;

send_normal_2:
    if (numArgsPushed != -1) // special case flag meaning it is a primitive
        return errFailed; // arguments remain on the stack

    msg = gSpecialBinarySelectors[g->primitiveIndex];
    sendMessage(g, msg, 2);
    return errNone;
}

int prAddNum(VMGlobals* g, int numArgsPushed) { return prOpNum<addNum>(g, numArgsPushed); }

int prSubNum(VMGlobals* g, int numArgsPushed) { return prOpNum<subNum>(g, numArgsPushed); }

int prMulNum(VMGlobals* g, int numArgsPushed) { return prOpNum<mulNum>(g, numArgsPushed); }


int prAddFloat(VMGlobals* g, int numArgsPushed) { return prOpFloat<addNum>(g, numArgsPushed); }

int prSubFloat(VMGlobals* g, int numArgsPushed) { return prOpFloat<subNum>(g, numArgsPushed); }

int prMulFloat(VMGlobals* g, int numArgsPushed) { return prOpFloat<mulNum>(g, numArgsPushed); }

int prAddInt(VMGlobals* g, int numArgsPushed) { return prOpInt<addNum>(g, numArgsPushed); }

int prSubInt(VMGlobals* g, int numArgsPushed) { return prOpInt<subNum>(g, numArgsPushed); }

int prMulInt(VMGlobals* g, int numArgsPushed) { return prOpInt<mulNum>(g, numArgsPushed); }


int prNthPrime(VMGlobals* g, int numArgsPushed);
int prNthPrime(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int n, p;

    a = g->sp;
    n = slotRawInt(a);
    p = nthPrime(n);
    if (p == 0) {
        SetNil(a);
    } else {
        SetInt(a, p);
    }
    return errNone;
}

int prPrevPrime(VMGlobals* g, int numArgsPushed);
int prPrevPrime(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int n, p, i;

    a = g->sp;
    n = slotRawInt(a);
    i = prevPrime(n);
    p = nthPrime(i);
    if (p == 0) {
        SetNil(a);
    } else {
        SetInt(a, p);
    }
    return errNone;
}

int prNextPrime(VMGlobals* g, int numArgsPushed);
int prNextPrime(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int n, p, i;

    a = g->sp;
    n = slotRawInt(a);
    i = nextPrime(n);
    p = nthPrime(i);
    if (p == 0) {
        SetNil(a);
    } else {
        SetInt(a, p);
    }
    return errNone;
}


int prIsPrime(VMGlobals* g, int numArgsPushed);
int prIsPrime(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int n, p, sqrtn, i;

    a = g->sp;
    n = slotRawInt(a);
    SetNil(a);
    if (n <= 2) {
        if (n == 2) {
            SetTrue(a);
        } else {
            SetFalse(a);
        }
    } else if (n <= nthPrime(NUMPRIMES - 1)) {
        // do a search of the primes table
        i = findPrime(n);
        if (i >= 0) {
            SetTrue(a);
        } else {
            SetFalse(a);
        }
    } else {
#ifdef _WIN32
        sqrtn = (int)sqrt(static_cast<double>(n));
#else
        sqrtn = (int)sqrt(n);
#endif
        for (i = 0; i < NUMPRIMES; ++i) {
            p = nthPrime(i);
            if (n % p == 0) {
                SetFalse(a);
                break;
            }
            if (p >= sqrtn) {
                SetTrue(a);
                break;
            }
        }
    }
    return errNone;
}

int prIndexOfPrime(VMGlobals* g, int numArgsPushed);
int prIndexOfPrime(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int n, p;

    a = g->sp;
    n = slotRawInt(a);
    if (n <= 2) {
        if (n == 2) {
            SetInt(a, 0);
        } else {
            SetNil(a);
        }
    } else if (n <= nthPrime(NUMPRIMES - 1)) {
        p = findPrime(n);
        if (p < 0) {
            SetNil(a);
        } else {
            SetInt(a, p);
        }
    } else {
        SetNil(a);
    }
    return errNone;
}


int prAs32Bits(VMGlobals* g, int numArgsPushed);
int prAs32Bits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    // return an integer that is a bit pattern for the 32 bit float representation
    union {
        float f;
        int32 i;
    } u;
    u.f = slotRawFloat(a);
    SetInt(a, u.i);
    return errNone;
}

int prHigh32Bits(VMGlobals* g, int numArgsPushed);
int prHigh32Bits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

#if BYTE_ORDER == BIG_ENDIAN
    union {
        struct {
            uint32 hi, lo;
        } i;
        double f;
    } du;
#else
    union {
        struct {
            uint32 lo, hi;
        } i;
        double f;
    } du;
#endif

    du.f = slotRawFloat(a);
    SetInt(a, du.i.hi);
    return errNone;
}

int prLow32Bits(VMGlobals* g, int numArgsPushed);
int prLow32Bits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

#if BYTE_ORDER == BIG_ENDIAN
    union {
        struct {
            uint32 hi, lo;
        } i;
        double f;
    } du;
#else
    union {
        struct {
            uint32 lo, hi;
        } i;
        double f;
    } du;
#endif

    du.f = slotRawFloat(a);
    SetInt(a, du.i.lo);
    return errNone;
}

int prFrom32Bits(VMGlobals* g, int numArgsPushed);
int prFrom32Bits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    int err, word;

    err = slotIntVal(b, &word);
    if (err)
        return err;

    union {
        float f;
        int32 i;
    } u;
    u.i = word;
    SetFloat(a, u.f);

    return errNone;
}

int prFrom64Bits(VMGlobals* g, int numArgsPushed);
int prFrom64Bits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;
    int err, hi, lo;

    err = slotIntVal(b, &hi);
    if (err)
        return err;

    err = slotIntVal(c, &lo);
    if (err)
        return err;

#if BYTE_ORDER == BIG_ENDIAN
    union {
        struct {
            uint32 hi, lo;
        } i;
        double f;
    } du;
#else
    union {
        struct {
            uint32 lo, hi;
        } i;
        double f;
    } du;
#endif
    du.i.hi = hi;
    du.i.lo = lo;
    SetFloat(a, du.f);

    return errNone;
}

int mathClipInt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    double lo, hi;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsInt(b) && IsInt(c)) {
        SetRaw(a, sc_clip(slotRawInt(a), slotRawInt(b), slotRawInt(c)));
    } else {
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetFloat(a, sc_clip((double)slotRawInt(a), lo, hi));
    }
    return errNone;
}

int mathClipFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    double lo, hi;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else {
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetRaw(a, sc_clip(slotRawFloat(a), lo, hi));
    }
    return errNone;
}

int mathClipSignal(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    float lo, hi;
    int err;
    PyrObject* sig;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsSignal(b) && IsSignal(c)) {
        sig = signal_clip_x(g, slotRawObject(a), slotRawObject(b), slotRawObject(c));
        SetObject(a, sig);
    } else {
        err = slotFloatVal(b, &lo);
        if (err)
            return err;
        err = slotFloatVal(c, &hi);
        if (err)
            return err;
        sig = signal_clip_f(g, slotRawObject(a), lo, hi);
        SetObject(a, sig);
    }
    return errNone;
}

int mathWrapInt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsInt(b) && IsInt(c)) {
        SetRaw(a,
               sc_mod((int)(slotRawInt(a) - slotRawInt(b)), (int)(slotRawInt(c) - slotRawInt(b) + 1)) + slotRawInt(b));
    } else {
        double x, lo, hi;
        x = slotRawInt(a);
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetFloat(a, sc_mod(x - lo, hi - lo) + lo);
    }
    return errNone;
}

int mathWrapFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    double lo, hi;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else {
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetRaw(a, sc_mod(slotRawFloat(a) - lo, hi - lo) + lo);
    }
    return errNone;
}

int mathWrapSignal(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    float lo, hi;
    int err;
    PyrObject* sig;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsSignal(b) && IsSignal(c)) {
        sig = signal_wrap_x(g, slotRawObject(a), slotRawObject(b), slotRawObject(c));
        SetObject(a, sig);
    } else {
        err = slotFloatVal(b, &lo);
        if (err)
            return err;
        err = slotFloatVal(c, &hi);
        if (err)
            return err;
        sig = signal_wrap_f(g, slotRawObject(a), lo, hi);
        SetObject(a, sig);
    }
    return errNone;
}

int mathFoldInt(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsInt(b) && IsInt(c)) {
        SetRaw(a, sc_fold(slotRawInt(a), slotRawInt(b), slotRawInt(c)));
    } else {
        double x, lo, hi;
        x = slotRawInt(a);
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetFloat(a, sc_fold(x, lo, hi));
    }
    return errNone;
}

int mathFoldFloat(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    double lo, hi;
    int err;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else {
        err = slotDoubleVal(b, &lo);
        if (err)
            return err;
        err = slotDoubleVal(c, &hi);
        if (err)
            return err;
        SetRaw(a, sc_fold(slotRawFloat(a), lo, hi));
    }
    return errNone;
}

int mathFoldSignal(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    float lo, hi;
    int err;
    PyrObject* sig;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (IsSym(b)) {
        *a = *b;
    } else if (IsSym(c)) {
        *a = *c;
    } else if (IsSignal(b) && IsSignal(c)) {
        sig = signal_fold_x(g, slotRawObject(a), slotRawObject(b), slotRawObject(c));
        SetObject(a, sig);
    } else {
        err = slotFloatVal(b, &lo);
        if (err)
            return err;
        err = slotFloatVal(c, &hi);
        if (err)
            return err;
        sig = signal_fold_f(g, slotRawObject(a), lo, hi);
        SetObject(a, sig);
    }
    return errNone;
}

int prSimpleNumberSeries(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int err, size;

    if (IsInt(a) && (IsInt(b) || IsNil(b)) && IsInt(c)) {
        int first, second, last, step;
        first = slotRawInt(a);
        last = slotRawInt(c);
        second = IsInt(b) ? slotRawInt(b) : (first < last ? first + 1 : first - 1);
        step = second - first;

        if (step == 0)
            size = 1;
        else
            size = ((last - first) / step) + 1;

        if ((size < 1) || ((step >= 0) && (last < first)) || ((step <= 0) && (last > first))) {
            post(
                "prSimpleNumberSeries: arguments do not form an arithmetic progression: first: %i, step: %i, last %i\n",
                first, step, last);
            return errFailed;
        }
        if (size > INT_MAX_BY_PyrSlot) {
            post("prSimpleNumberSeries: array size %i exceeds limit (%i)\n", size, INT_MAX_BY_PyrSlot);
            return errFailed;
        }

        PyrObject* obj = newPyrArray(g->gc, size, 0, true);
        obj->size = size;
        PyrSlot* slots = obj->slots;
        if (step == 1) {
            // Faster iteration for common case
            if (first == 0) {
                for (int i = 0; i < size; ++i) {
                    SetInt(slots + i, i);
                }
            } else {
                for (int i = 0; i < size; ++i) {
                    SetInt(slots + i, first++);
                }
            }
        } else {
            int val = first;
            for (int i = 0; i < size; ++i) {
                SetInt(slots + i, val);
                val += step;
            }
        }
        SetObject(a, obj);
    } else {
        double first, second, last, step;
        err = slotDoubleVal(a, &first);
        if (err)
            return err;
        err = slotDoubleVal(c, &last);
        if (err)
            return err;
        err = slotDoubleVal(b, &second);
        if (err) {
            if (first < last)
                second = first + 1.;
            else
                second = first - 1.;
        }

        step = second - first;
        size = (int)floor((last - first) / step + 0.001) + 1;

        if ((size < 1) || ((step >= 0) && (last < first)) || ((step <= 0) && (last > first))) {
            post(
                "prSimpleNumberSeries: arguments do not form an arithmetic progression: first: %f, step: %f, last %f\n",
                first, step, last);
            return errFailed;
        }
        if (size > INT_MAX_BY_PyrSlot) {
            post("prSimpleNumberSeries: array size %i exceeds limit (%i)\n", size, INT_MAX_BY_PyrSlot);
            return errFailed;
        }
        PyrObject* obj = newPyrArray(g->gc, size, 0, true);
        obj->size = size;
        PyrSlot* slots = obj->slots;
        if (first == 0. && step == 1.) {
            // Faster iteration for common case
            for (long i = 0; i < size; ++i) {
                SetFloat(slots + i, i);
            }
        } else {
            double val = first;
            for (long i = 0; i < size; ++i) {
                val = first + step * i;
                SetFloat(slots + i, val);
            }
        }
        SetObject(a, obj);
    }
    return errNone;
}

/*

    asFraction {|maxDenominator=100|
        var mediant, lower, upper, temp;
        var n,d, k, k1;
        if (this < 0) {
            #n, d = this.neg.asFraction(maxDenominator);
            ^[n.neg, d]
        };
        if (this < 1.0) {
            upper = [1.0, this.reciprocal.floor];
            lower = [1.0, upper[1]+1.0];
        }{
            lower = [this.floor, 1.0];
            upper = [lower[0]+1.0, 1.0];
        };
        mediant = [lower[0] + upper[0], lower[1] + upper[1]];
        loop {
            mediant = [lower[0] + upper[0], lower[1] + upper[1]];
            case
            { (this * mediant[1]) > mediant[0] }
            {
                if (maxDenominator < mediant[1]) {^upper};
                d = upper[0] - (this * upper[1]);
                if (d == 0) {^upper};
                lower = mediant;
                k = floor(((this * lower[1]) - lower[0]) / d);
                k1 = k + 1;
                temp = [lower[0] + (k1 * upper[0]), lower[1] + (k1 * upper[1])];
                lower = [lower[0] + (k * upper[0]), lower[1] + (k * upper[1])];
                upper = temp;
            }
            { (this * mediant[1]) == mediant[0] }
            {
                if (maxDenominator >= mediant[1]) {^mediant};
                if (lower[1] < upper[1]) {^lower};
                ^upper
            }
            {
                if (maxDenominator < mediant[1]) {^lower};
                d = lower[0] - (this * lower[1]);
                if (d == 0) {^lower};
                upper = mediant;
                k = floor(((this * upper[1]) - upper[0]) / d);
                k1 = k + 1;
                temp = [(k1 * lower[0]) + upper[0], (k1 * lower[1]) + upper[1]];
                upper = [(k * lower[0]) + upper[0], (k * lower[1]) + upper[1]];
                lower = temp;
            };
        }
    }

*/

int prAsFraction(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    double mediant_num, lower_num, upper_num, temp_num;
    double mediant_den, lower_den, upper_den, temp_den;
    double x, d;
    int k, k1;
    int maxDenominator;
    int err;
    bool neg = false;

    err = slotDoubleVal(a, &x);
    if (err)
        return err;

    err = slotIntVal(b, &maxDenominator);
    if (err)
        return err;

    bool faster = IsTrue(c);

    PyrObject* obj = newPyrArray(g->gc, 2, 0, true);
    obj->size = 2;
    PyrSlot* slots = obj->slots;
    SetObject(a, obj);

    if (x < 0.0) {
        x = -x;
        neg = true;
    }

    if (x == 0.0) {
        SetInt(slots + 0, 0);
        SetInt(slots + 1, 1);
        return errNone;
    }

    if (x < 1.0) {
        upper_num = 1.0;
        upper_den = floor(1. / x);
        lower_num = 1.0;
        lower_den = upper_den + 1.;
    } else {
        lower_num = floor(x);
        lower_den = 1.0;
        upper_num = lower_num + 1.;
        upper_den = 1.0;
    }

    while (true) {
        mediant_num = lower_num + upper_num;
        mediant_den = lower_den + upper_den;
        // post("  md %g %g    %g %g    %g %g\n", mediant_num, mediant_den, lower_num, lower_den, upper_num, upper_den);

        if (x * mediant_den > mediant_num) {
            d = upper_num - (x * upper_den);
            if (maxDenominator < mediant_den || fabs(d) < 1e-5) {
                if (neg)
                    upper_num = -upper_num;
                SetInt(slots + 0, (int)upper_num);
                SetInt(slots + 1, (int)upper_den);
                return errNone;
            }
            lower_num = mediant_num;
            lower_den = mediant_den;
            if (faster) {
                k = (int)floor(((x * lower_den) - lower_num) / d);
                if (k < 10000) {
                    k1 = k + 1;
                    temp_num = lower_num + (k1 * upper_num);
                    temp_den = lower_den + (k1 * upper_den);
                    lower_num = lower_num + (k * upper_num);
                    lower_den = lower_den + (k * upper_den);
                    upper_num = temp_num;
                    upper_den = temp_den;
                }
            }
        } else if (x * mediant_den == mediant_num) {
            if (maxDenominator >= mediant_den) {
                if (neg)
                    mediant_num = -mediant_num;
                SetInt(slots + 0, (int)mediant_num);
                SetInt(slots + 1, (int)mediant_den);
                return errNone;
            } else if (lower_den < upper_den) {
                if (neg)
                    lower_num = -lower_num;
                SetInt(slots + 0, (int)lower_num);
                SetInt(slots + 1, (int)lower_den);
                return errNone;
            } else {
                if (neg)
                    upper_num = -upper_num;
                SetInt(slots + 0, (int)upper_num);
                SetInt(slots + 1, (int)upper_den);
                return errNone;
            }
        } else {
            d = lower_num - (x * lower_den);
            if (maxDenominator < mediant_den || fabs(d) < 1e-5) {
                if (neg)
                    lower_num = -lower_num;
                SetInt(slots + 0, (int)lower_num);
                SetInt(slots + 1, (int)lower_den);
                return errNone;
            }
            upper_num = mediant_num;
            upper_den = mediant_den;
            if (faster) {
                k = (int)floor(((x * upper_den) - upper_num) / d);
                if (k < 10000) {
                    k1 = k + 1;
                    temp_num = (k1 * lower_num) + upper_num;
                    temp_den = (k1 * lower_den) + upper_den;
                    upper_num = (k * lower_num) + upper_num;
                    upper_den = (k * lower_den) + upper_den;
                    lower_num = temp_num;
                    lower_den = temp_den;
                }
            }
        }
    }
}

int prSphericalHarmonic(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 3; // n (spherical order)
    PyrSlot* b = g->sp - 2; // m (index within the order)
    PyrSlot* c = g->sp - 1; // theta
    PyrSlot* d = g->sp; // phi

    int err;

    int n = 0;
    int m = 0;
    double theta = 0.0;
    double phi = 0.0;

    err = slotDoubleVal(d, &phi);
    if (err)
        return err;
    err = slotDoubleVal(c, &theta);
    if (err)
        return err;
    err = slotIntVal(b, &m);
    if (err)
        return err;
    err = slotIntVal(a, &n);
    if (err)
        return err;

    auto res_cmplx = boost::math::spherical_harmonic(n, m, theta, phi);

    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, getsym("Complex")->u.classobj, 0, true, true);
    SetObject(a, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, res_cmplx.real());
    SetFloat(slots + 1, res_cmplx.imag());

    return errNone;
}

template <typename Arg1T, typename Arg2T, std::complex<double> BoostFunctionT(Arg1T, Arg2T)>
int prBoostTwoArgRetComplex(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    Arg1T arg1 = {};
    Arg2T arg2 = {};

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;
    err = slotVal(b, &arg2);
    if (err)
        return err;

    auto res_cmplx = BoostFunctionT(arg1, arg2);

    PyrObject* obj = instantiateObject(gMainVMGlobals->gc, getsym("Complex")->u.classobj, 0, true, true);
    SetObject(a, obj);

    PyrSlot* slots = obj->slots;
    SetFloat(slots + 0, res_cmplx.real());
    SetFloat(slots + 1, res_cmplx.imag());

    return errNone;
}

template <typename Arg1T, std::vector<double> BoostFunctionT(Arg1T)>
int prBoostOneArgRetArray(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    Arg1T arg1;

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;

    auto res = BoostFunctionT(arg1);

    PyrObject* array = newPyrArray(g->gc, res.size(), 0, true);
    SetObject(a, array);

    PyrSlot* s = array->slots;
    for (auto item : res) {
        SetFloat(s, item);
        ++array->size;
        ++s;
    }

    return errNone;
}

template <typename RetT, RetT BoostFunctionT(unsigned, double const&)>
int prBoostCheby(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    int arg1 = 0;
    double arg2 = 0.0;

    int err = slotIntVal(a, &arg1);
    if (err)
        return err;
    err = slotDoubleVal(b, &arg2);
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1, arg2);
    setSlotVal(a, res);

    return errNone;
}

template <typename RetT, RetT BoostFunctionT(double const&)>
int prBoostsqrt1pm1(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    double arg1 = 0;

    int err = slotDoubleVal(a, &arg1);
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1);
    setSlotVal(a, res);

    return errNone;
}

template <typename RetT, typename Arg1T, RetT BoostFunctionT(Arg1T)>
int prBoostOneArg(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    Arg1T arg1;

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1);
    setSlotVal(a, res);

    return errNone;
}

template <typename RetT, typename Arg1T, typename Arg2T, RetT BoostFunctionT(Arg1T, Arg2T)>
int prBoostTwoArg(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    Arg1T arg1 = {};
    Arg2T arg2 = {};

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;
    err = slotVal(b, &arg2);
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1, arg2);
    setSlotVal(a, res);

    return errNone;
}

template <typename RetT, typename Arg1T, typename Arg2T, typename Arg3T, RetT BoostFunctionT(Arg1T, Arg2T, Arg3T)>
int prBoostThreeArg(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    Arg1T arg1 = {};
    Arg2T arg2 = {};
    Arg3T arg3 = {};

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;
    err = slotVal(b, &arg2);
    if (err)
        return err;
    err = slotVal(c, &arg3);
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1, arg2, arg3);
    setSlotVal(a, res);

    return errNone;
}

template <typename RetT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T,
          RetT BoostFunctionT(Arg1T, Arg2T, Arg3T, Arg4T)>
int prBoostFourArg(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 3;
    PyrSlot* b = g->sp - 2;
    PyrSlot* c = g->sp - 1;
    PyrSlot* d = g->sp;

    Arg1T arg1 = {};
    Arg2T arg2 = {};
    Arg3T arg3 = {};
    Arg4T arg4 = {};

    int err = slotVal(a, &arg1); // slotVal only unwraps/extracts numeric values
    if (err)
        return err;
    err = slotVal(b, &arg2);
    if (err)
        return err;
    err = slotVal(c, &arg3);
    if (err)
        return err;
    err = slotVal(d, &arg4);
    if (err)
        return err;

    RetT res = BoostFunctionT(arg1, arg2, arg3, arg4);
    setSlotVal(a, res);

    return errNone;
}


void initMathPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;
    definePrimitive(base, index++, "_AddInt", prAddInt, 2, 0);
    definePrimitive(base, index++, "_SubInt", prSubInt, 2, 0);
    definePrimitive(base, index++, "_MulInt", prMulInt, 2, 0);
    definePrimitive(base, index++, "_AddFloat", prAddFloat, 2, 0);
    definePrimitive(base, index++, "_SubFloat", prSubFloat, 2, 0);
    definePrimitive(base, index++, "_MulFloat", prMulFloat, 2, 0);
    definePrimitive(base, index++, "_NthPrime", prNthPrime, 1, 0);
    definePrimitive(base, index++, "_PrevPrime", prPrevPrime, 1, 0);
    definePrimitive(base, index++, "_NextPrime", prNextPrime, 1, 0);
    definePrimitive(base, index++, "_IsPrime", prIsPrime, 1, 0);
    definePrimitive(base, index++, "_IndexOfPrime", prIndexOfPrime, 1, 0);
    definePrimitive(base, index++, "_As32Bits", prAs32Bits, 1, 0);
    definePrimitive(base, index++, "_High32Bits", prHigh32Bits, 1, 0);
    definePrimitive(base, index++, "_Low32Bits", prLow32Bits, 1, 0);
    definePrimitive(base, index++, "_From32Bits", prFrom32Bits, 2, 0);
    definePrimitive(base, index++, "_From64Bits", prFrom64Bits, 3, 0);

    definePrimitive(base, index++, "_ClipInt", mathClipInt, 3, 0);
    definePrimitive(base, index++, "_ClipFloat", mathClipFloat, 3, 0);
    definePrimitive(base, index++, "_ClipSignal", mathClipSignal, 3, 0);
    definePrimitive(base, index++, "_WrapInt", mathWrapInt, 3, 0);
    definePrimitive(base, index++, "_WrapFloat", mathWrapFloat, 3, 0);
    definePrimitive(base, index++, "_WrapSignal", mathWrapSignal, 3, 0);
    definePrimitive(base, index++, "_FoldInt", mathFoldInt, 3, 0);
    definePrimitive(base, index++, "_FoldFloat", mathFoldFloat, 3, 0);
    definePrimitive(base, index++, "_FoldSignal", mathFoldSignal, 3, 0);

    definePrimitive(base, index++, "_SimpleNumberSeries", prSimpleNumberSeries, 3, 0);
    definePrimitive(base, index++, "_AsFraction", prAsFraction, 3, 0);

    //  Number Series:
    definePrimitive(base, index++, "_BernouliB2n", prBoostOneArg<double, int, boost::math::bernoulli_b2n<double>>, 1,
                    0);
    definePrimitive(base, index++, "_TangentT2n", prBoostOneArg<double, int, boost::math::tangent_t2n<double>>, 1, 0);

    //  Gamma:
    definePrimitive(base, index++, "_TGamma", prBoostOneArg<double, double, boost::math::tgamma<double>>, 1, 0);
    definePrimitive(base, index++, "_TGamma1pm1", prBoostOneArg<double, double, boost::math::tgamma1pm1<double>>, 1, 0);
    definePrimitive(base, index++, "_LGamma", prBoostOneArg<double, double, boost::math::lgamma<double>>, 1, 0);
    definePrimitive(base, index++, "_Digamma", prBoostOneArg<double, double, boost::math::digamma<double>>, 1, 0);
    definePrimitive(base, index++, "_Trigamma", prBoostOneArg<double, double, boost::math::trigamma<double>>, 1, 0);
    definePrimitive(base, index++, "_Polygamma", prBoostTwoArg<double, int, double, boost::math::polygamma<double>>, 2,
                    0);
    definePrimitive(base, index++, "_TGammaRatio",
                    prBoostTwoArg<double, double, double, boost::math::tgamma_ratio<double>>, 2, 0);
    definePrimitive(base, index++, "_TGammaDeltaRatio",
                    prBoostTwoArg<double, double, double, boost::math::tgamma_delta_ratio<double>>, 2, 0);
    // Incomplete Gamma Functions
    definePrimitive(base, index++, "_GammaP", prBoostTwoArg<double, double, double, boost::math::gamma_p<double>>, 2,
                    0);
    definePrimitive(base, index++, "_GammaQ", prBoostTwoArg<double, double, double, boost::math::gamma_q<double>>, 2,
                    0);
    definePrimitive(base, index++, "_TGammaLower",
                    prBoostTwoArg<double, double, double, boost::math::tgamma_lower<double>>, 2, 0);
    definePrimitive(base, index++, "_TGammaI", prBoostTwoArg<double, double, double, boost::math::tgamma<double>>, 2,
                    0);
    // Incomplete Gamma Function Inverses
    definePrimitive(base, index++, "_GammaPInv",
                    prBoostTwoArg<double, double, double, boost::math::gamma_p_inv<double>>, 2, 0);
    definePrimitive(base, index++, "_GammaQInv",
                    prBoostTwoArg<double, double, double, boost::math::gamma_q_inv<double>>, 2, 0);
    definePrimitive(base, index++, "_GammaPInvA",
                    prBoostTwoArg<double, double, double, boost::math::gamma_p_inva<double>>, 2, 0);
    definePrimitive(base, index++, "_GammaQInvA",
                    prBoostTwoArg<double, double, double, boost::math::gamma_q_inva<double>>, 2, 0);
    // Incomplete Gamma Function Derivative
    definePrimitive(base, index++, "_GammaPDerivative",
                    prBoostTwoArg<double, double, double, boost::math::gamma_p_derivative<double>>, 2, 0);

    //	Factorials and Binomial Coefficients:
    definePrimitive(base, index++, "_Factorial", prBoostOneArg<double, unsigned, boost::math::factorial<double>>, 1, 0);
    definePrimitive(base, index++, "_DoubleFactorial",
                    prBoostOneArg<double, unsigned int, boost::math::double_factorial<double>>, 1, 0);
    definePrimitive(base, index++, "_RisingFactorial",
                    prBoostTwoArg<double, double, int, boost::math::rising_factorial<double>>, 2, 0);
    definePrimitive(base, index++, "_FallingFactorial",
                    prBoostTwoArg<double, double, unsigned, boost::math::falling_factorial<double>>, 2, 0);
    definePrimitive(base, index++, "_BinomialCoefficient",
                    prBoostTwoArg<double, unsigned, unsigned, boost::math::binomial_coefficient<double>>, 2, 0);

    //	Beta Functions:
    definePrimitive(base, index++, "_Beta", prBoostTwoArg<double, double, double, boost::math::beta<double>>, 2, 0);
    // Incomplete Betas, normalized and non-normalized
    definePrimitive(base, index++, "_IBeta",
                    prBoostThreeArg<double, double, double, double, boost::math::ibeta<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaC",
                    prBoostThreeArg<double, double, double, double, boost::math::ibetac<double>>, 3, 0);
    definePrimitive(base, index++, "_BetaFull",
                    prBoostThreeArg<double, double, double, double, boost::math::beta<double>>, 3, 0);
    definePrimitive(base, index++, "_BetaCFull",
                    prBoostThreeArg<double, double, double, double, boost::math::betac<double>>, 3, 0);
    // Incomplete Betas, inverse
    definePrimitive(base, index++, "_IBetaInv",
                    prBoostThreeArg<double, double, double, double, boost::math::ibeta_inv<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaCInv",
                    prBoostThreeArg<double, double, double, double, boost::math::ibetac_inv<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaInvA",
                    prBoostThreeArg<double, double, double, double, boost::math::ibeta_inva<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaCInvA",
                    prBoostThreeArg<double, double, double, double, boost::math::ibetac_inva<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaInvB",
                    prBoostThreeArg<double, double, double, double, boost::math::ibeta_invb<double>>, 3, 0);
    definePrimitive(base, index++, "_IBetaCInvB",
                    prBoostThreeArg<double, double, double, double, boost::math::ibetac_invb<double>>, 3, 0);
    // 	Incomplete Betas, derivative
    definePrimitive(base, index++, "_IBetaDerivative",
                    prBoostThreeArg<double, double, double, double, boost::math::ibeta_derivative<double>>, 3, 0);

    //  Error functions:
    definePrimitive(base, index++, "_Erf", prBoostOneArg<double, double, boost::math::erf<double>>, 1, 0);
    definePrimitive(base, index++, "_ErfC", prBoostOneArg<double, double, boost::math::erfc<double>>, 1, 0);
    definePrimitive(base, index++, "_ErfInv", prBoostOneArg<double, double, boost::math::erf_inv<double>>, 1, 0);
    definePrimitive(base, index++, "_ErfCInv", prBoostOneArg<double, double, boost::math::erfc_inv<double>>, 1, 0);

    //  Polynomials:
    // Legendre (and Associated)
    definePrimitive(base, index++, "_LegendreP", prBoostTwoArg<double, int, double, boost::math::legendre_p<double>>, 2,
                    0);
    definePrimitive(base, index++, "_LegendrePPrime",
                    prBoostTwoArg<double, int, double, boost::math::legendre_p_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_LegendrePZeros", prBoostOneArgRetArray<int, boost::math::legendre_p_zeros<double>>,
                    1, 0); // TODO: generalize return array form?
    definePrimitive(base, index++, "_LegendrePAssoc",
                    prBoostThreeArg<double, int, int, double, boost::math::legendre_p<double>>, 3, 0);
    definePrimitive(base, index++, "_LegendreQ",
                    prBoostTwoArg<double, unsigned, double, boost::math::legendre_q<double>>, 2, 0);
    // Laguerre (and Associated)
    definePrimitive(base, index++, "_Laguerre", prBoostTwoArg<double, unsigned, double, boost::math::laguerre<double>>,
                    2, 0);
    definePrimitive(base, index++, "_LaguerreAssoc",
                    prBoostThreeArg<double, unsigned, double, double, boost::math::laguerre<double>>, 3, 0);
    // Hermite
    definePrimitive(base, index++, "_Hermite", prBoostTwoArg<double, unsigned, double, boost::math::hermite<double>>, 2,
                    0);
    // Chebyshev

    definePrimitive(base, index++, "_ChebyshevT", prBoostCheby<double, boost::math::chebyshev_t<double>>, 2, 0);
    definePrimitive(base, index++, "_ChebyshevU", prBoostCheby<double, boost::math::chebyshev_u<double>>, 2, 0);
    definePrimitive(base, index++, "_ChebyshevTPrime", prBoostCheby<double, boost::math::chebyshev_t_prime<double>>, 2,
                    0);
    // Spherical Harmonics
    definePrimitive(base, index++, "_SphericalHarmonic", prSphericalHarmonic, 4, 0);
    definePrimitive(base, index++, "_SphericalHarmonicR",
                    prBoostFourArg<double, unsigned, int, double, double, boost::math::spherical_harmonic_r<double>>, 4,
                    0);
    definePrimitive(base, index++, "_SphericalHarmonicI",
                    prBoostFourArg<double, unsigned, int, double, double, boost::math::spherical_harmonic_i<double>>, 4,
                    0);

    //	Bessel Functions:
    // First and Second Kinds
    definePrimitive(base, index++, "_CylBesselJ",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_j<double>>, 2, 0);
    definePrimitive(base, index++, "_CylNeumann",
                    prBoostTwoArg<double, double, double, boost::math::cyl_neumann<double>>, 2, 0);
    // Zero finder
    definePrimitive(base, index++, "_CylBesselJZero",
                    prBoostTwoArg<double, double, int, boost::math::cyl_bessel_j_zero<double>>, 2, 0);
    definePrimitive(base, index++, "_CylNeumannZero",
                    prBoostTwoArg<double, double, int, boost::math::cyl_neumann_zero<double>>, 2, 0);
    // Modified, First and Second Kinds
    definePrimitive(base, index++, "_CylBesselI",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_i<double>>, 2, 0);
    definePrimitive(base, index++, "_CylBesselK",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_k<double>>, 2, 0);
    // Spherical, First and Second Kinds
    definePrimitive(base, index++, "_SphBessel",
                    prBoostTwoArg<double, unsigned, double, boost::math::sph_bessel<double>>, 2, 0);
    definePrimitive(base, index++, "_SphNeumann",
                    prBoostTwoArg<double, unsigned, double, boost::math::sph_neumann<double>>, 2, 0);
    // Derivatives
    definePrimitive(base, index++, "_CylBesselJPrime",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_j_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_CylNeumannPrime",
                    prBoostTwoArg<double, double, double, boost::math::cyl_neumann_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_CylBesselIPrime",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_i_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_CylBesselKPrime",
                    prBoostTwoArg<double, double, double, boost::math::cyl_bessel_k_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_SphBesselPrime",
                    prBoostTwoArg<double, unsigned, double, boost::math::sph_bessel_prime<double>>, 2, 0);
    definePrimitive(base, index++, "_SphNeumannPrime",
                    prBoostTwoArg<double, unsigned, double, boost::math::sph_neumann_prime<double>>, 2, 0);

    //  Hankel Functions:
    // Cyclic
    definePrimitive(base, index++, "_CylHankel1",
                    prBoostTwoArgRetComplex<double, double, boost::math::cyl_hankel_1<double>>, 2, 0);
    definePrimitive(base, index++, "_CylHankel2",
                    prBoostTwoArgRetComplex<double, double, boost::math::cyl_hankel_2<double>>, 2, 0);
    // Spherical
    definePrimitive(base, index++, "_SphHankel1",
                    prBoostTwoArgRetComplex<double, double, boost::math::sph_hankel_1<double>>, 2, 0);
    definePrimitive(base, index++, "_SphHankel2",
                    prBoostTwoArgRetComplex<double, double, boost::math::sph_hankel_2<double>>, 2, 0);

    //  Airy Functions:
    definePrimitive(base, index++, "_AiryAi", prBoostOneArg<double, double, boost::math::airy_ai<double>>, 1, 0);
    definePrimitive(base, index++, "_AiryBi", prBoostOneArg<double, double, boost::math::airy_bi<double>>, 1, 0);
    definePrimitive(base, index++, "_AiryAiPrime", prBoostOneArg<double, double, boost::math::airy_ai_prime<double>>, 1,
                    0);
    definePrimitive(base, index++, "_AiryBiPrime", prBoostOneArg<double, double, boost::math::airy_bi_prime<double>>, 1,
                    0);
    definePrimitive(base, index++, "_AiryAiZero", prBoostOneArg<double, int, boost::math::airy_ai_zero<double>>, 1, 0);
    definePrimitive(base, index++, "_AiryBiZero", prBoostOneArg<double, int, boost::math::airy_bi_zero<double>>, 1, 0);

    //  Elliptic Integrals:
    // Elliptic Integrals - Carlson Form
    definePrimitive(base, index++, "_EllintRf",
                    prBoostThreeArg<double, double, double, double, boost::math::ellint_rf<double>>, 3, 0);
    definePrimitive(base, index++, "_EllintRd",
                    prBoostThreeArg<double, double, double, double, boost::math::ellint_rd<double>>, 3, 0);
    definePrimitive(base, index++, "_EllintRj",
                    prBoostFourArg<double, double, double, double, double, boost::math::ellint_rj<double>>, 4, 0);
    definePrimitive(base, index++, "_EllintRc", prBoostTwoArg<double, double, double, boost::math::ellint_rc<double>>,
                    2, 0);
    definePrimitive(base, index++, "_EllintRg",
                    prBoostThreeArg<double, double, double, double, boost::math::ellint_rg<double>>, 3, 0);
    // Elliptic Integrals of the First, Second, Third Kind, D - Legendre Form
    definePrimitive(base, index++, "_Ellint1", prBoostTwoArg<double, double, double, boost::math::ellint_1<double>>, 2,
                    0);
    definePrimitive(base, index++, "_Ellint1C", prBoostOneArg<double, double, boost::math::ellint_1<double>>, 1, 0);
    definePrimitive(base, index++, "_Ellint2", prBoostTwoArg<double, double, double, boost::math::ellint_2<double>>, 2,
                    0);
    definePrimitive(base, index++, "_Ellint2C", prBoostOneArg<double, double, boost::math::ellint_2<double>>, 1, 0);
    definePrimitive(base, index++, "_Ellint3",
                    prBoostThreeArg<double, double, double, double, boost::math::ellint_3<double>>, 3, 0);
    definePrimitive(base, index++, "_Ellint3C", prBoostTwoArg<double, double, double, boost::math::ellint_3<double>>, 2,
                    0);
    definePrimitive(base, index++, "_EllintD", prBoostTwoArg<double, double, double, boost::math::ellint_d<double>>, 2,
                    0);
    definePrimitive(base, index++, "_EllintDC", prBoostOneArg<double, double, boost::math::ellint_d<double>>, 1, 0);
    // Jacobi Zeta, Heuman Lambda Function
    definePrimitive(base, index++, "_JacobiZeta",
                    prBoostTwoArg<double, double, double, boost::math::jacobi_zeta<double>>, 2, 0);
    definePrimitive(base, index++, "_HeumanLambda",
                    prBoostTwoArg<double, double, double, boost::math::heuman_lambda<double>>, 2, 0);


    //  Jacobi Elliptic Functions:
    definePrimitive(base, index++, "_JacobiCd", prBoostTwoArg<double, double, double, boost::math::jacobi_cd<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiCn", prBoostTwoArg<double, double, double, boost::math::jacobi_cn<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiCs", prBoostTwoArg<double, double, double, boost::math::jacobi_cs<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiDc", prBoostTwoArg<double, double, double, boost::math::jacobi_dc<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiDn", prBoostTwoArg<double, double, double, boost::math::jacobi_dn<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiDs", prBoostTwoArg<double, double, double, boost::math::jacobi_ds<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiNc", prBoostTwoArg<double, double, double, boost::math::jacobi_nc<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiNd", prBoostTwoArg<double, double, double, boost::math::jacobi_nd<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiNs", prBoostTwoArg<double, double, double, boost::math::jacobi_ns<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiSc", prBoostTwoArg<double, double, double, boost::math::jacobi_sc<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiSd", prBoostTwoArg<double, double, double, boost::math::jacobi_sd<double>>,
                    2, 0);
    definePrimitive(base, index++, "_JacobiSn", prBoostTwoArg<double, double, double, boost::math::jacobi_sn<double>>,
                    2, 0);

    //  Zeta Function:
    definePrimitive(base, index++, "_Zeta", prBoostOneArg<double, double, boost::math::zeta<double>>, 1, 0);

    //  Exponential Integrals:
    definePrimitive(base, index++, "_ExpintEn", prBoostTwoArg<double, double, double, boost::math::expint<double>>, 2,
                    0);
    definePrimitive(base, index++, "_ExpintEi", prBoostOneArg<double, double, boost::math::expint<double>>, 1, 0);

    //  Basic functions:
    definePrimitive(base, index++, "_SinPi", prBoostOneArg<double, double, boost::math::sin_pi<double>>, 1, 0);
    definePrimitive(base, index++, "_CosPi", prBoostOneArg<double, double, boost::math::cos_pi<double>>, 1, 0);
    definePrimitive(base, index++, "_Log1p", prBoostOneArg<double, double, boost::math::log1p<double>>, 1, 0);
    definePrimitive(base, index++, "_ExpM1", prBoostOneArg<double, double, boost::math::expm1<double>>, 1, 0);
    definePrimitive(base, index++, "_Cbrt", prBoostOneArg<double, double, boost::math::cbrt<double>>, 1, 0);
    definePrimitive(base, index++, "_Sqrt1pm1", prBoostsqrt1pm1<double, boost::math::sqrt1pm1<double>>, 1, 0);
    definePrimitive(base, index++, "_PowM1", prBoostTwoArg<double, double, double, boost::math::powm1<double>>, 2, 0);
    definePrimitive(base, index++, "_Hypot2", prBoostTwoArg<double, double, double, boost::math::hypot<double>>, 2, 0);

    // Sinus Cardinal and Hyperbolic Sinus Cardinal Functions:
    definePrimitive(base, index++, "_SincPi", prBoostOneArg<double, double, boost::math::sinc_pi<double>>, 1, 0);
    definePrimitive(base, index++, "_SinhcPi", prBoostOneArg<double, double, boost::math::sinhc_pi<double>>, 1, 0);

    // Inverse Hyperbolic Functions:
    definePrimitive(base, index++, "_Asinh", prBoostOneArg<double, double, boost::math::asinh<double>>, 1, 0);
    definePrimitive(base, index++, "_Acosh", prBoostOneArg<double, double, boost::math::acosh<double>>, 1, 0);
    definePrimitive(base, index++, "_Atanh", prBoostOneArg<double, double, boost::math::atanh<double>>, 1, 0);

    //	Owen's T function:
    definePrimitive(base, index++, "_OwensT", prBoostTwoArg<double, double, double, boost::math::owens_t<double>>, 2,
                    0);
}
