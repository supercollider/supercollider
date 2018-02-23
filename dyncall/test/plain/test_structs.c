/*

 Package: dyncall
 Library: test
 File: test/plain/test_structs.c
 Description: 
 License:

   Copyright (c) 2010-2015 Olivier Chafik <olivier.chafik@gmail.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/




#include "../common/test_framework.h"
#include "../../dyncall/dyncall.h"
#include "../../dyncall/dyncall_signature.h"
#include "../../dyncall/dyncall_struct.h"
#include "../common/platformInit.h"

#define DC_TEST_INT_EQUAL(expected, computed) { \
	if (expected != computed) \
		printf("expected = %d, computed = %d\n\n", (int)expected, (int)computed); \
	DC_TEST(expected == computed); \
}
#define DC_TEST_STRUCT_SIZE(type, s) { \
	DCsize expected = sizeof(type), computed = dcStructSize(s);\
	DC_TEST_INT_EQUAL(expected, computed); \
}

DC_DEFINE_TEST_FUNC_BEGIN(testStructSizes)

	{
		typedef struct {
			double a, b, c, d;
		} S;

		size_t size;
		DCstruct* s = dcNewStruct(4, DEFAULT_ALIGNMENT);
		dcStructField(s, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 1);
		dcCloseStruct(s);

		DC_TEST_STRUCT_SIZE(S, s);
		dcFreeStruct(s);
	}
	{
		typedef struct {
			char a, b;
			void* p[3];
		} S;

		size_t size;
		DCstruct* s = dcNewStruct(3, DEFAULT_ALIGNMENT);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_POINTER, DEFAULT_ALIGNMENT, 3);
		dcCloseStruct(s);

		DC_TEST_STRUCT_SIZE(S, s);
		dcFreeStruct(s);
	}
	{
		typedef struct {
			short a;
			struct {
			char a, b;
			void* p[3];
			} sub;
			short b;
		} S;
	 	
		size_t size;
		DCstruct* s = dcNewStruct(3, DEFAULT_ALIGNMENT);
		dcStructField(s, DC_SIGCHAR_SHORT, DEFAULT_ALIGNMENT, 1);
		dcSubStruct(s, 3, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_POINTER, DEFAULT_ALIGNMENT, 3);
		dcCloseStruct(s);
		dcStructField(s, DC_SIGCHAR_SHORT, DEFAULT_ALIGNMENT, 1);
		dcCloseStruct(s);

		DC_TEST_STRUCT_SIZE(S, s);
		dcFreeStruct(s);
	}

#define TEST_MONO_STRUCT(type, sig) \
	{ \
		typedef struct { \
			type v; \
		} S; \
		 \
		DCstruct* s = dcNewStruct(1, DEFAULT_ALIGNMENT); \
		dcStructField(s, sig, DEFAULT_ALIGNMENT, 1); \
		dcCloseStruct(s); \
		 \
		DC_TEST_STRUCT_SIZE(S, s); \
		dcFreeStruct(s); \
	}

	TEST_MONO_STRUCT(char,               DC_SIGCHAR_CHAR);
	TEST_MONO_STRUCT(unsigned char,      DC_SIGCHAR_UCHAR);
	TEST_MONO_STRUCT(short,              DC_SIGCHAR_SHORT);
	TEST_MONO_STRUCT(unsigned short,     DC_SIGCHAR_USHORT);
	TEST_MONO_STRUCT(int,                DC_SIGCHAR_INT);
	TEST_MONO_STRUCT(unsigned int,       DC_SIGCHAR_UINT);
	TEST_MONO_STRUCT(long,               DC_SIGCHAR_LONG);
	TEST_MONO_STRUCT(unsigned long,      DC_SIGCHAR_ULONG);
	TEST_MONO_STRUCT(long long,          DC_SIGCHAR_LONGLONG);
	TEST_MONO_STRUCT(unsigned long long, DC_SIGCHAR_ULONGLONG);
	TEST_MONO_STRUCT(void*,              DC_SIGCHAR_POINTER);
	TEST_MONO_STRUCT(float,              DC_SIGCHAR_FLOAT);
	TEST_MONO_STRUCT(double,             DC_SIGCHAR_DOUBLE);

DC_DEFINE_TEST_FUNC_END



typedef struct
{
	char a, b, c;
} FewValues;

double sum_FewValues(FewValues values)
{
	printf("sum_FewValues(a = %d, b = %d, c = %d)\n", (int)values.a, (int)values.b, (int)values.c);
	return ((double)values.a) + ((double)values.b) + ((double)values.c);
}


typedef struct
{
	char a, b;
	double p[10];
} SomeValues;

double sum_SomeValues(SomeValues values)
{
	return ((double)values.a) + ((double)values.b) + values.p[0] + values.p[1] + values.p[2];
}


DC_DEFINE_TEST_FUNC_BEGIN(testCallStructs)

	DCCallVM* pc = dcNewCallVM(4096);
	{
		FewValues values;
		double calledSum, expectedSum;
		DCstruct* s = dcNewStruct(3, DEFAULT_ALIGNMENT);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcCloseStruct(s);

		DC_TEST_STRUCT_SIZE(FewValues, s);

		values.a = 1;
		values.b = 2;
		values.c = 3;

		dcMode(pc, DC_CALL_C_DEFAULT);
		dcReset(pc);
		printf("BEFORE dcArgStruct\n");
		dcArgStruct(pc, s, &values);
		printf("AFTER dcArgStruct\n");
		calledSum = dcCallDouble(pc, (DCpointer)&sum_FewValues);
		expectedSum = sum_FewValues(values);

		DC_TEST_INT_EQUAL(expectedSum, calledSum);
		dcFreeStruct(s);
	}
	{
		SomeValues values;
		double calledSum, expectedSum;
		DCstruct* s = dcNewStruct(3, DEFAULT_ALIGNMENT);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_CHAR, DEFAULT_ALIGNMENT, 1);
		dcStructField(s, DC_SIGCHAR_DOUBLE, DEFAULT_ALIGNMENT, 10);
		dcCloseStruct(s);

		DC_TEST_STRUCT_SIZE(SomeValues, s);

		values.a = 1;
		values.b = 2;
		values.p[0] = 10;
		values.p[1] = 11;
		values.p[2] = 12;

		dcMode(pc, DC_CALL_C_DEFAULT);
		dcReset(pc);
		dcArgStruct(pc, s, &values);
		calledSum = dcCallDouble(pc, (DCpointer) &sum_SomeValues);
		expectedSum = sum_SomeValues(values);

		DC_TEST_INT_EQUAL(expectedSum, calledSum);
		dcFreeStruct(s);
	}

	dcFree(pc);

DC_DEFINE_TEST_FUNC_END

