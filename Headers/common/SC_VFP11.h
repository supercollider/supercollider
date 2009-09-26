#ifndef _SC_VFP11_
#define _SC_VFP11_

#include <TargetConditionals.h>

#define IPHONE_VEC



inline void initVFP()
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
	__asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"
        : [tmp] "=&r" (tmp)
        :
        : "cc", "memory");
#endif
}

inline void releaseVFP()
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [tmp] "=&r" (tmp)
        :
        : "cc", "memory");
#endif
}


inline void vfill(float *dest, float val, int len)
{
#if !TARGET_IPHONE_SIMULATOR
    float t[4] = {val, val, val, val};
	float *v = (float *) t;
	int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src1], {s8-s11}\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmiasge %[dst]!, {s8-s11}\n\t"
        "fstmiasge %[dst]!, {s8-s11}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (v), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}


// a + b*c
inline void vmuladd(float *dest, float *a, float *b, float *c, int len)
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src2]!, {s0-s3}\n\t"
        "fldmias %[src1]!, {s8-s11}\n\t"
        "fldmias %[src3]!, {s24-s27}\n\t"
        "fldmias %[src2]!, {s4-s7}\n\t"
        "fldmias %[src1]!, {s12-s15}\n\t"
        "fldmias %[src3]!, {s28-s31}\n\t"
        "fmacs s8, s0, s24\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fmacs s12, s4, s28\n\t"
        "fldmiasge %[src2]!, {s0-s3}\n\t"
        "fldmiasge %[src1]!, {s16-s19}\n\t"
        "fldmiasge %[src3]!, {s24-s27}\n\t"
        "fldmiasge %[src2]!, {s4-s7}\n\t"
        "fldmiasge %[src1]!, {s20-s23}\n\t"
        "fldmiasge %[src3]!, {s28-s31}\n\t"
        "fmacsge s16, s0, s24\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s12-s15}\n\t"
        "fmacsge s20, s4, s28\n\t"
        "fldmiasgt %[src2]!, {s0-s3}\n\t"
        "fldmiasgt %[src1]!, {s8-s11}\n\t"
        "fldmiasgt %[src3]!, {s24-s27}\n\t"
        "fldmiasgt %[src2]!, {s4-s7}\n\t"
        "fldmiasgt %[src1]!, {s12-s15}\n\t"
        "fldmiasgt %[src3]!, {s28-s31}\n\t"
        "fmacsge s8, s0, s24\n\t"
        "fstmiasge %[dst]!, {s16-s19}\n\t"
        "fstmiasge %[dst]!, {s20-s23}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (a), [src2] "+&r" (b), [src3] "+&r" (c), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}

inline void vscalarmul(float *dest, float scalar, float *b, int len)
{
#if !TARGET_IPHONE_SIMULATOR
	float t[4] = {scalar, scalar, scalar, scalar};
	float *s = t;
	int tmp;

    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src1], {s0-s3}\n\t"
        "fldmias %[src2]!, {s8-s11}\n\t"
        "fldmias %[src2]!, {s12-s15}\n\t"
        "fmuls s8, s8, s0\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fmuls s12, s12, s0\n\t"
        "fldmiasge %[src2]!, {s24-s27}\n\t"
        "fldmiasge %[src2]!, {s28-s31}\n\t"
        "fmulsge s24, s24, s0\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s12-s15}\n\t"
        "fmulsge s28, s28, s0\n\t"
        "fldmiasgt %[src2]!, {s8-s11}\n\t"
        "fldmiasgt %[src2]!, {s12-s15}\n\t"
        "fmulsge s8, s8, s0\n\t"
        "fstmiasge %[dst]!, {s24-s27}\n\t"
        "fstmiasge %[dst]!, {s28-s31}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (s), [src2] "+&r" (b), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}

inline void vmul(float *dest, float *a, const float *b, int len)
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src1]!, {s0-s3}\n\t"
        "fldmias %[src2]!, {s8-s11}\n\t"
        "fldmias %[src1]!, {s4-s7}\n\t"
        "fldmias %[src2]!, {s12-s15}\n\t"
        "fmuls s8, s0, s8\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fmuls s12, s4, s12\n\t"
        "fldmiasge %[src1]!, {s16-s19}\n\t"
        "fldmiasge %[src2]!, {s24-s27}\n\t"
        "fldmiasge %[src1]!, {s20-s23}\n\t"
        "fldmiasge %[src2]!, {s28-s31}\n\t"
        "fmulsge s24, s16, s24\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s12-s15}\n\t"
        "fmulsge s28, s20, s28\n\t"
        "fldmiasgt %[src1]!, {s0-s3}\n\t"
        "fldmiasgt %[src2]!, {s8-s11}\n\t"
        "fldmiasgt %[src1]!, {s4-s7}\n\t"
        "fldmiasgt %[src2]!, {s12-s15}\n\t"
        "fmulsge s8, s0, s8\n\t"
        "fstmiasge %[dst]!, {s24-s27}\n\t"
        "fstmiasge %[dst]!, {s28-s31}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (a), [src2] "+&r" (b), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}


inline void vadd(float *dest, float *a, const float *b, int len)
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src1]!, {s0-s3}\n\t"
        "fldmias %[src2]!, {s8-s11}\n\t"
        "fldmias %[src1]!, {s4-s7}\n\t"
        "fldmias %[src2]!, {s12-s15}\n\t"
        "fadds s8, s0, s8\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fadds s12, s4, s12\n\t"
        "fldmiasge %[src1]!, {s16-s19}\n\t"
        "fldmiasge %[src2]!, {s24-s27}\n\t"
        "fldmiasge %[src1]!, {s20-s23}\n\t"
        "fldmiasge %[src2]!, {s28-s31}\n\t"
        "faddsge s24, s16, s24\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s12-s15}\n\t"
        "faddsge s28, s20, s28\n\t"
        "fldmiasgt %[src1]!, {s0-s3}\n\t"
        "fldmiasgt %[src2]!, {s8-s11}\n\t"
        "fldmiasgt %[src1]!, {s4-s7}\n\t"
        "fldmiasgt %[src2]!, {s12-s15}\n\t"
        "faddsge s8, s0, s8\n\t"
        "fstmiasge %[dst]!, {s24-s27}\n\t"
        "fstmiasge %[dst]!, {s28-s31}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (a), [src2] "+&r" (b), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}

inline void vcopy(float *dest, float *a, int len)
{
#if !TARGET_IPHONE_SIMULATOR
    int tmp;
    __asm__ __volatile__(
        "fmrx %[tmp], fpscr\n\t"
        "orr %[tmp], %[tmp], #(3 << 16)\n\t" /* set vector size to 4 */
        "fmxr fpscr, %[tmp]\n\t"

        "fldmias %[src1]!, {s8-s11}\n\t"
        "fldmias %[src1]!, {s12-s15}\n\t"
        "1:\n\t"
        "subs %[len], %[len], #16\n\t"
        "fldmiasge %[src1]!, {s24-s27}\n\t"
        "fldmiasge %[src1]!, {s28-s31}\n\t"
        "fstmias %[dst]!, {s8-s11}\n\t"
        "fstmias %[dst]!, {s12-s15}\n\t"
        "fldmiasgt %[src1]!, {s8-s11}\n\t"
        "fldmiasgt %[src1]!, {s12-s15}\n\t"
        "fstmiasge %[dst]!, {s24-s27}\n\t"
        "fstmiasge %[dst]!, {s28-s31}\n\t"
        "bgt 1b\n\t"

        "bic %[tmp], %[tmp], #(7 << 16)\n\t" /* set vector size back to 1 */
        "fmxr fpscr, %[tmp]\n\t"
        : [dst] "+&r" (dest), [src1] "+&r" (a), [len] "+&r" (len), [tmp] "=&r" (tmp)
        :
        : "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
          "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
          "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
          "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
          "cc", "memory");
#endif
}



#endif
