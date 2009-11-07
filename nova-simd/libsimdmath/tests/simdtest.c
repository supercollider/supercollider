/*  
    Universal SIMD Mathlibrary  
    Copyright 2008 Helmut Dersch 
    der (at) fh-furtwangen.de

    This file is part of the Universal SIMD Mathlibrary.
    The Universal SIMD Mathlibrary is free software: 
    you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
   Tests various SIMD libraries
   Must be compiled without optimization.
*/

#include <stdio.h>
#include <math.h>
#include "simdmath.h"
#include "simdconst.h"


/*
      Select the distribution:
            LIBSIMD - Universal SIMD Mathlibrary
            LIBMATH - Standard C libm
            MACVEC  - MACOS Accelerate Framework (vecLib)
            CELL    - CELL SDK (PPU and/or SPU
*/

#if defined( LIBSIMD ) || defined( LIBMATH ) 

#ifdef LIBSIMD
      const char *lib = "libsimdm";
#else
      const char *lib = "libm";
#endif

#define RUN_TEST1( func ) \
      printf("\n\n%s:\n", # func);  \
      printf("Interval %g .... %g\n", a, b);    \
      test_function1(a,b, func ## f4, func, &ft); \
      print_results(&ft); 

#define RUN_TEST2( func ) \
      printf("\n\n%s:\n", # func);  \
      printf("Interval x: %g .... %g\n", a, b); \
      printf("Interval y: %g .... %g\n", c, d); \
      test_function2(a,b,c,d, func ## f4, func, &ft); \
      print_results(&ft); 

#elif defined( MACVEC )

#include <Accelerate/Accelerate.h>

const char *lib = "macos vecLib";
#define RUN_TEST1( func ) \
      printf("\n\n%s:\n", # func);  \
      test_function1(a,b,v ## func ## f, func, &ft); \
      print_results(&ft);

#define RUN_TEST2( func ) \
      printf("\n\n%s:\n", # func);  \
      printf("Interval x: %g .... %g\n", a, b); \
      printf("Interval y: %g .... %g\n", c, d); \
      test_function2(a,b,c,d,v ## func ## f, func, &ft); \
      print_results(&ft);


#elif defined( CELL )

vec_float4 ps3atanf4( vec_float4 x );
vec_float4 ps3atan2f4( vec_float4 y, vec_float4 x );
vec_float4 ps3sqrtf4( vec_float4 x );
vec_float4 ps3sinf4( vec_float4 x );
vec_float4 ps3cosf4( vec_float4 x );
vec_float4 ps3tanf4( vec_float4 x );
vec_float4 ps3asinf4( vec_float4 x );
vec_float4 ps3acosf4( vec_float4 x );
vec_float4 ps3expf4( vec_float4 x );
vec_float4 ps3logf4( vec_float4 x );
vec_float4 ps3powf4( vec_float4 x, vec_float4 y );

#ifdef __SPU__
      const char *lib = "ps3/spu simd math";
#else
      const char *lib = "ps3/ppu simd math";
#endif
#define RUN_TEST1( func ) \
      printf("\n\n%s:\n", # func);  \
      printf("Interval %g .... %g\n", a, b);    \
      test_function1(a,b,ps3 ## func ## f4, func, &ft); \
      print_results(&ft);
            
#define RUN_TEST2( func ) \
      printf("\n\n%s:\n", # func);  \
      printf("Interval x: %g .... %g\n", a, b); \
      printf("Interval y: %g .... %g\n", c, d); \
      test_function2(a,b,c,d,ps3 ## func ## f4, func, &ft); \
      print_results(&ft);

#endif // __PPU__ || __SPU__


/* 
      Create a stopwatch:
      SPU - uses decrementer register
      all others - use gettimeofday() call
*/

#ifdef __SPU__

static int tmode = 0;

static inline double mysecond(){
      unsigned int start = 0xffffffffu;
      if(tmode){
            double timebase = 79800000. ; // from /proc/cpuinfo
            unsigned int time = start-spu_read_decrementer();
            tmode = 0;
            return (double)time / timebase;
      }else{
            tmode=1;
            spu_write_decrementer(start);
            return 0.;
      }
}

#else

#include <time.h>
#include <sys/time.h>

static inline double mysecond(){
      struct timeval tp;
      struct timezone tzp;
      int i;

      i = gettimeofday(&tp,&tzp);
      return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
#endif


/*    
      A few debugging functions for printing vectors 
*/

static inline void print_f4( vec_float4 x ){
      union{
            vec_float4 v;
            float f[4];
      }xv;
      xv.v = x;
      printf("%g %f %f %g\n",xv.f[0],xv.f[1],xv.f[2],xv.f[3]);
}

static inline void print_i4( vec_int4 x ){
      union{
            vec_int4 v;
            int i[4];
      }xv;
      xv.v = x;
      printf("%d %d %d %d\n",xv.i[0],xv.i[1],xv.i[2],xv.i[3]);
}

static inline void print_ix4( vec_int4 x ){
      union{
            vec_int4 v;
            int i[4];
      }xv;
      xv.v = x;
      printf("%x %x %x %x\n",xv.i[0],xv.i[1],xv.i[2],xv.i[3]);
}




struct func_test_results{
      double time;
      double rms_abs_error;
      double max_abs_error;
      double rms_rel_error;
      double max_rel_error;
      double result;
// Regression:
      double a1;
      double a0;
      double delta_a1;
      double delta_a0;
};

void print_results( struct func_test_results *f ){
      printf("time: %lgs\nrms abs: %lg, rms rel: %lg\n",
            f->time, f->rms_abs_error, f->rms_rel_error);
      printf("max abs: %lg, max rel: %lg\nsum: %lg\n",
            f->max_abs_error,f->max_rel_error, f->result);
/*
      printf("Regression:\na1-1= %lg, delta_a1= %lg\n",
            f->a1-1.0, f->delta_a1);
      printf("a0= %lg, delta_a0= %lg\n",
            f->a0, f->delta_a0);
*/
}


/* 
      Test of single variable functions 
*/
void test_function1(  float a, float b,                     // Interval to test
                      vec_float4 (*vfunc)(vec_float4),      // Function to test
                      double (*dfunc)(double) ,             // double function 
                      struct func_test_results *f ){        // results

      int N = 1000000;
      union{
            vec_float4 v;
            float     f[4];
      }r,x;

 /*
      Speed test
 */
      x.v         = (vec_float4){a,b,a,b};
      float d     = (b-a)/(float)N;
      vec_float4 delta = (vec_float4){d,-d,d,-d};
      r.v = CF4_0;
      int i;

      // estimate loopoverhead (dry run)
      double t=mysecond();
      for(i=0; i<N; i++){
            r.v += x.v;
            x.v += delta;
      }
      double t_loop = mysecond()-t;
      x.v = (vec_float4){a,b,a,b};
      r.v = CF4_0;

      // time the vector function
      t=mysecond();
      for(i=0; i<N; i++){
            r.v += (*vfunc)(x.v);
            x.v += delta;
      }
      t = mysecond()-t-t_loop;
      f->result = (double)r.f[0];
      f->time   = t/N;

/*      
      Accuracy 
*/
      int n = 0;
      N = 100000;
      d = (b-a)/(float)N;
      
      double sr = 0.,sa = 0., maxr=0., maxa=0.;
      // x - approx.; y - "exact" values
      double sxy=0., sx=0., sy=0, sx2=0., a0=a;

      for(i=0; i<N; i++){
            x.v = (vec_float4){a,a,a,a};
            x.v = (*vfunc)(x.v);
            double X = (double)a, Y;
            X =(*dfunc)(X);
            sx  += (double)x.f[0];
            sy  += X;
            sx2 += (double)x.f[0]*(double)x.f[0];
            sxy += (double)x.f[0]*X;

            double e = X-(double)x.f[0];        
            e *= e;
            sa += e;
            if(e > maxa)
                  maxa = e;
            if(X!=0.0){
                  e/=(X*X);
                  sr += e;
                  if(e>maxr)
                        maxr=e;
                  n++;
            }
            a += d;
      }
      f->rms_abs_error = sqrt(sa/N);
      f->max_abs_error = sqrt(maxa);
      f->rms_rel_error = sqrt(sr/n);
      f->max_rel_error = sqrt(maxr);
      double xm = sx/N, ym = sy/N;
      f->a1 = (sxy-ym*sx)/(sx2-xm*sx);
      f->a0 = ym - f->a1*xm;
      double Sy=0.;
      a = a0;
      for(i=0; i<N; i++){
            x.v = (vec_float4){a,a,a,a};
            x.v = (*vfunc)(x.v);
            double X = (double)a;
            X =(*dfunc)(X);
            double xt = X - (f->a1*(double)x.f[0]+f->a0);
            Sy += xt*xt;
            a += d;
      }
      Sy = sqrt(Sy/(N-2));
      f->delta_a1=Sy*sqrt(N/(N*sx2-sx*sx));
      f->delta_a0=Sy*sqrt(sx2/(N*sx2-sx*sx));
}

/* 
      Test of dual variable functions 
*/
void test_function2(  float a, float b,                     // Interval to test
            float c, float d,
            vec_float4 (*vfunc)(vec_float4,vec_float4),     // Function to test
            double (*dfunc)(double, double) ,               // double function 
            struct func_test_results *f ){                  // results
      // Time
      int N1 = 1000, N2 = 1000;
      union{
            vec_float4 v;
            float     f[4];
      }r,x,y;
      
      x.v                = (vec_float4){a,b,a,b};
      y.v                = (vec_float4){c,d,d,c};
      float d1           = (b-a)/(float)N1;
      vec_float4 delta1= (vec_float4){d1,-d1,d1,-d1};
      float d2           = (d-c)/(float)N2;
      vec_float4 delta2= (vec_float4){d2,-d2,-d2,d2};
      r.v = CF4_0;
      int i,k;

      // speed
      // estimate loopoverhead
      double t=mysecond();
      for(i=0; i<N1; i++){
            y.v = (vec_float4){c,d,d,c};
            for(k=0; k<N2; k++){
                  r.v += y.v;
                  y.v += delta2;                
            }
            x.v += delta1;
      }
      double t_loop = mysecond()-t;

      x.v                = (vec_float4){a,b,a,b};
      y.v                = (vec_float4){c,d,d,c};
      r.v = CF4_0;
      t=mysecond();
      for(i=0; i<N1; i++){
            y.v = (vec_float4){c,d,d,c};
            for(k=0; k<N2; k++){
                  r.v += (*vfunc)(x.v, y.v);
                  y.v += delta2;                
            }
            x.v += delta1;
      }
      t = mysecond()-t-t_loop;
      f->result = (double)r.f[0];
      f->time   = t/(N1*N2);
      
      // Accuracy
      int n = 0;
      N1 = 500, N2=500;
      d1           = (b-a)/(float)N1;
      d2           = (d-c)/(float)N2;
      
      double sr = 0.,sa = 0., maxr=0., maxa=0.;
      float c0 = c;
      for(i=0; i<N1; i++){
            c = c0;
            for(k=0; k<N2; k++){
                  x.v = (vec_float4){a,a,a,a};
                  y.v = (vec_float4){c,c,c,c};
                  x.v = (*vfunc)(x.v, y.v);
                  double X = (double)a;
                  double Y = (double)c;
                  X =(*dfunc)(X,Y);
                  double e = X-(double)x.f[0];
                  e *= e;
                  sa += e;
                  if(e > maxa)
                        maxa = e;
                  if(X!=0.0){
                        e/=(X*X);
                        sr += e;
                        if(e>maxr)
                              maxr=e;
                        n++;
                  }
                  c += d2;
            }
            a += d1;
      }
      f->rms_abs_error = sqrt(sa/ (N1*N2));
      f->max_abs_error = sqrt(maxa);
      f->rms_rel_error = sqrt(sr/ n);
      f->max_rel_error = sqrt(maxr);
}

/*
     Determine  FLT_EPSILON
*/ 
static void print_vec_float4_epsilon(){
      union{
            vec_uint4 v;
            int i[4];
      }at;
      union{
            vec_float4 v;
            float f[4];
      }x,y;
      x.v = y.v = CF4_1;
      at.v = VEC_GT( CF4_1 + y.v, CF4_1 );
      while(at.i[0]){
            y.v = x.v*CF4_05;
            at.v = VEC_GT( CF4_1 + y.v, CF4_1 );
            if(at.i[0])
                  x.v=y.v;
      }
      printf("Vector Float Epsilon: %g\n", x.f[0]);
}

void run_function_test(int i){
      struct func_test_results ft;
      float pi=3.14159265358979323846f;
      float a,b,c,d; // interval
      switch(i){
            case 1:
                  a = -4.0f*pi;     b = 4.0f*pi;
                  RUN_TEST1( sin )
                  break;
            case 2:
                  a = -4.0f*pi;     b = 4.0f*pi;
                  RUN_TEST1( cos )
                  break;
            case 3:
                  a = -pi/2.0+.001f;      b = pi/2.0f-0.001f;
                  RUN_TEST1( tan )
                  break;
            case 4:
                  a = -0.95f; b = 0.95f;
                  RUN_TEST1( asin )
                  break;
            case 5:
                  a = -0.95f; b = 0.95f;
                  RUN_TEST1( acos )
                  break;
            case 6:
                  a = -10.0f; b = 10.0f;
                  RUN_TEST1( atan )
                  break;
            case 7:
                  a = -5.0f;  b = 5.0f;  c = -5.0f; d = 5.0f;
                  RUN_TEST2( atan2 )
                  break;
            case 8:
                  a = 0.0f;   b = 1000.f;
                  RUN_TEST1( sqrt )
                  break;
            case 9:
                  a = -2.0f;  b = 2.0f;
                  RUN_TEST1( exp )
                  break;
            case 10:
                  a = 0.001f; b = 1000.0f;
                  RUN_TEST1( log )
                  break;
            case 11:
                  a = 0.0f;  b = 1.5f;  c = 0.5f; d = 1.5f;
                  RUN_TEST2( pow )
                  break;
      }
}


      
int main(int argc, char** argv){          
      int c;
      print_vec_float4_epsilon();   
      if(argc>1 && strcmp(argv[1],"-r")==0){
            c=12;
            while(c-->0)
                  run_function_test(11-c);
            return;
      }
            
      while(1){
            printf("\nTesting library: %s\n",lib);
            printf("\nSelect function:\n");
            printf("sin(1), cos(2), tan(3),\n");
            printf("asin(4),acos(5),atan(6),atan2(7)\n");
            printf("sqrt(8),exp(9),log(10),pow(11)\n");
            printf("Enter number (12-all,0-exit):\n");
            
            scanf("%d",&c);
            if(c<=0 || c>12)
                  break;
            if(c==12){
                  while(c-->0)
                        run_function_test(11-c);
            }else
                  run_function_test(c);
            
      }
}
