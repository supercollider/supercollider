// types:

typedef struct { int x, y; } s2i; 
typedef struct { int x, y, z, w; } s4i; 
typedef struct { int x, y, z, w, a, b, c, d; } s8i; 

typedef struct { float x, y; } s2f; 
typedef struct { float x, y, z, w; } s4f; 

typedef struct { double x; } s1d; 
typedef struct { double x, y; } s2d; 
typedef struct { double x, y, z, w; } s4d; 

typedef struct { long double x; } s1q;

#if defined(__ARM_NEON__) 
#include <arm_neon.h>
#endif

#if defined(__i386__) || defined(__x86_64__) && ( defined(__MMX__) || defined(__SSE__) )
#if defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ > 2)

#include <immintrin.h> 

#if defined(__AVX__)
#define V256 1
typedef __m256 v256_t;
#endif

#else

#include <emmintrin.h>

#endif

#if defined(__MMX__)
#define V64 1
typedef __m64  v64_t;
#endif

#if defined(__SSE__)
#define V128 1
typedef __m128 v128_t;
#endif

#endif

#if V64

typedef struct { v64_t f1; } s1w;
typedef struct { v64_t f1, f2; } s2w;
typedef struct { v64_t f1, f2, f3, f4; } s4w;

#endif

#if V128

typedef struct { v128_t f1; } s1x;
typedef struct { v128_t f1, f2; } s2x;

typedef struct { v64_t f1; v128_t f2; } swx;

#endif

// x86_64:

/*
                  x86_64
                  ------------
  (v{ii})         regs  int 8
  (v{iiii})       regs  int 16
  (v{iiiiiiii})   stack     32
  (v{ff})         regs  sse 8
  (v{ffff})       regs  sse 16
  (v{d})          regs  sse 8
  (v{dd})         regs  sse 16
  (v{dddd})       stack     32
  (v{w})          regs  sse 8
  (v{ww})         regs  sse 16         
  (v{wwww})       stack     32
  (v{x})          regs  sse 16  
  (v{xx})         stack     32
  (v(wx})         stack     32 
  (v{xw})         stack     32
  (v{y})          stack     32
  (v{yy}{x})      stack     32
  (v{ifif})              
*/

#if V256

typedef struct { v256_t f1; } s1y;
typedef struct { v256_t f1; v256_t f2; } s2y;

typedef v256_t y;

extern void f_y(y a1); void call_f_y() { y a1 = {0,}; f_y(a1); }


extern void f_s2ys1x(s2y a1,s1x a2); void call_f_s2ys1x() { s2y a1 = {0,}; s1x a2 = {0,}; f_s2ys1x(a1,a2); }
extern void f_s1xs2y(s1x a1,s2y a2); void call_f_s1xs2y() { s1x a1 = {0,}; s2y a2 = {0,}; f_s1xs2y(a1,a2); }

extern void f_s1y(s1y a1); void call_f_s1y() { s1y a1 = {0,}; f_s1y(a1); }
extern void f_s2y(s2y a1); void call_f_s2y() { s2y a1 = {0,}; f_s2y(a1); }

extern void f_s1ys1y(s1y a1,s1y a2); void call_f_s1ys1y() { s1y a1 = {0,}, a2 = {0,}; f_s1ys1y(a1,a2); }
extern void f_s2ys1y(s2y a1,s1y a2); void call_f_s2ys1y() { s2y a1 = {0,}; s1y a2 = {0,}; f_s2ys1y(a1,a2); }

#endif

#if V128

extern void f_swx(swx a1); void call_f_swx() { swx a1 = {0,}; f_swx(a1); }

extern void f_s2x(s2x a1); void call_f_s2x() { s2x a1 = {0,}; f_s2x(a1); }
extern void f_s1x(s1x a1); void call_f_s1x() { s1x a1 = {0,}; f_s1x(a1); }

#endif

#if V64

extern void f_s4w(s4w a1); void call_f_s4w() { s4w a1 = {0,}; f_s4w(a1); }
extern void f_s2w(s2w a1); void call_f_s2w() { s2w a1 = {0,}; f_s2w(a1); }
extern void f_s1w(s1w a1); void call_f_s1w() { s1w a1 = {0,}; f_s1w(a1); }

#endif

// one composite argument:

extern void f_s2i(s2i a1); void call_f_s2i() { s2i a1 = {0,}; f_s2i(a1); }
extern void f_s4i(s4i a1); void call_f_s4i() { s4i a1 = {0,}; f_s4i(a1); }
extern void f_s8i(s8i a1); void call_f_s8i() { s8i a1 = {0,}; f_s8i(a1); }

extern void f_s2f(s2f a1); void call_f_s2f() { s2f a1 = {0,}; f_s2f(a1); }
extern void f_s4f(s4f a1); void call_f_s4f() { s4f a1 = {0,}; f_s4f(a1); }

extern void f_s1d(s1d a1); void call_f_s1d() { s1d a1 = {0,}; f_s1d(a1); }
extern void f_s2d(s2d a1); void call_f_s2d() { s2d a1 = {0,}; f_s2d(a1); }
extern void f_s4d(s4d a1); void call_f_s4d() { s4d a1 = {0,}; f_s4d(a1); }

extern void f_s1q(s1q a1); void call_f_s1q() { s1q a1 = {0,}; f_s1q(a1); }


// multiple arguments:

extern void f_s2ds2d(s2d a1, s2d a2); 
void call_f_s2ds2d() { 
  s2d a1={0,}; 
  s2d a2={0,}; 
  f_s2ds2d(a1,a2); 
} 

typedef struct { float x; int y; float z; int w; } sfifi;
extern void f_sfifi(sfifi a1); 
void call_f_sfifi() { sfifi a1 = {0,1,2,3,}; f_sfifi(a1); }

// int/float 4-byte alignment.

typedef struct { int x; float y; int z; float w; } sifif;
extern void f_sifif(sifif a1); 
void call_f_sifif() { sifif a1 = {0,1,2,3,}; f_sifif(a1); }

#define FULL_T char, short, int, long int, char, char, char, char
#define FULL_V 0,1,2,3,4,5,6,7

#define REMAIN64_T char, char
#define REMAIN64_V 0,1
extern void f_full_sifif(FULL_T, sifif a1); void call_f_full_sifif() { sifif a1 = {0,}; f_full_sifif(FULL_V,a1); }
extern void f_remain64_sifif(REMAIN64_T, sifif a1); void call_f_remain64_sifif() { sifif a1 = {0,}; f_remain64_sifif(REMAIN64_V,a1); }

typedef struct { float f1,f2,f3; char f4; } sfffc;
extern void f_sfffc(sfffc a1); 
void call_f_sfffc() { sfffc a1 = {0,1,2,3,}; f_sfffc(a1); }

typedef struct { char f1; float f2,f3,f4; } scfff;
extern void f_scfff(scfff a1); 
void call_f_scfff() { scfff a1 = {0,1,2,3,}; f_scfff(a1); }

