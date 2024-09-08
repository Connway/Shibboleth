/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_AngelScriptMath.h"
#include "Shibboleth_IncludeAngelScript.h"
#include <string.h>
#include <float.h>
#include <math.h>

#define CHECK_RESULT(expr) if (!(expr)) { return false; }

// This was mainly copied from scriptmath.cpp, but removing the AS_USE_FLOAT stuff.

namespace
{
	// The modf function doesn't seem very intuitive, so I'm writing this
	// function that simply returns the fractional part of the float value
	float fractionf(float v)
	{
		float intPart;
		return modff(v, &intPart);
	}

	double fraction(double v)
	{
		double intPart;
		return modf(v, &intPart);
	}

	// As AngelScript doesn't allow bitwise manipulation of float types we'll provide a couple of
	// functions for converting float values to IEEE 754 formatted values etc. This also allow us to
	// provide a platform agnostic representation to the script so the scripts don't have to worry
	// about whether the CPU uses IEEE 754 floats or some other representation
	float fpFromIEEE(asUINT raw)
	{
		// TODO: Identify CPU family to provide proper conversion
		//        if the CPU doesn't natively use IEEE style floats
		union conv
		{
			asUINT raw;
			float fp;
		} value;
		value.raw = raw;
		return value.fp;
	}
	asUINT fpToIEEE(float fp)
	{
		union conv
		{
			asUINT raw;
			float fp;
		} value;
		value.fp = fp;
		return value.raw;
	}
	double fpFromIEEE(asQWORD raw)
	{
		union conv
		{
			asQWORD raw;
			double fp;
		} value;
		value.raw = raw;
		return value.fp;
	}
	asQWORD fpToIEEE(double fp)
	{
		union conv
		{
			asQWORD raw;
			double fp;
		} value;
		value.fp = fp;
		return value.raw;
	}

	// closeTo() is used to determine if the binary representation of two numbers are
	// relatively close to each other. Numerical errors due to rounding errors build
	// up over many operations, so it is almost impossible to get exact numbers and
	// this is where closeTo() comes in.
	//
	// It shouldn't be used to determine if two numbers are mathematically close to
	// each other.
	//
	// ref: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	// ref: http://www.gamedev.net/topic/653449-scriptmath-and-closeto/
	bool closeTo(float a, float b, float epsilon)
	{
		// Equal numbers and infinity will return immediately
		if( a == b ) return true;

		// When very close to 0, we can use the absolute comparison
		float diff = fabsf(a - b);
		if( (a == 0 || b == 0) && (diff < epsilon) )
		return true;

		// Otherwise we need to use relative comparison to account for precision
		return diff / (fabs(a) + fabs(b)) < epsilon;
	}

	bool closeTo(double a, double b, double epsilon)
	{
		if( a == b ) return true;

		double diff = fabs(a - b);
		if( (a == 0 || b == 0) && (diff < epsilon) )
		return true;

		return diff / (fabs(a) + fabs(b)) < epsilon;
	}
}

NS_SHIBBOLETH

bool RegisterScriptMath_Native(asIScriptEngine *engine)
{
	int r;

	// Conversion between floating point and IEEE bits representations
	r = engine->RegisterGlobalFunction("float fpFromIEEE(uint)", asFUNCTIONPR(fpFromIEEE, (asUINT), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("uint fpToIEEE(float)", asFUNCTIONPR(fpToIEEE, (float), asUINT), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double fpFromIEEE(uint64)", asFUNCTIONPR(fpFromIEEE, (asQWORD), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("uint64 fpToIEEE(double)", asFUNCTIONPR(fpToIEEE, (double), asQWORD), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Close to comparison with epsilon
	r = engine->RegisterGlobalFunction("bool closeTo(float, float, float = 0.00001f)", asFUNCTIONPR(closeTo, (float, float, float), bool), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("bool closeTo(double, double, double = 0.0000000001)", asFUNCTIONPR(closeTo, (double, double, double), bool), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Trigonometric functions
	r = engine->RegisterGlobalFunction("float cos(float)", asFUNCTIONPR(cosf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float sin(float)", asFUNCTIONPR(sinf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float tan(float)", asFUNCTIONPR(tanf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	r = engine->RegisterGlobalFunction("float acos(float)", asFUNCTIONPR(acosf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float asin(float)", asFUNCTIONPR(asinf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float atan(float)", asFUNCTIONPR(atanf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float atan2(float,float)", asFUNCTIONPR(atan2f, (float, float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Hyberbolic functions
	r = engine->RegisterGlobalFunction("float cosh(float)", asFUNCTIONPR(coshf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float sinh(float)", asFUNCTIONPR(sinhf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float tanh(float)", asFUNCTIONPR(tanhf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Exponential and logarithmic functions
	r = engine->RegisterGlobalFunction("float log(float)", asFUNCTIONPR(logf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float log10(float)", asFUNCTIONPR(log10f, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Power functions
	r = engine->RegisterGlobalFunction("float pow(float, float)", asFUNCTIONPR(powf, (float, float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float sqrt(float)", asFUNCTIONPR(sqrtf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Nearest integer, absolute value, and remainder functions
	r = engine->RegisterGlobalFunction("float ceil(float)", asFUNCTIONPR(ceilf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float abs(float)", asFUNCTIONPR(fabsf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float floor(float)", asFUNCTIONPR(floorf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("float fraction(float)", asFUNCTIONPR(fractionf, (float), float), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	// Don't register modf because AngelScript already supports the % operator

	// double versions of the same
	r = engine->RegisterGlobalFunction("double cos(double)", asFUNCTIONPR(cos, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double sin(double)", asFUNCTIONPR(sin, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double tan(double)", asFUNCTIONPR(tan, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double acos(double)", asFUNCTIONPR(acos, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double asin(double)", asFUNCTIONPR(asin, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double atan(double)", asFUNCTIONPR(atan, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double atan2(double,double)", asFUNCTIONPR(atan2, (double, double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double cosh(double)", asFUNCTIONPR(cosh, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double sinh(double)", asFUNCTIONPR(sinh, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double tanh(double)", asFUNCTIONPR(tanh, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double log(double)", asFUNCTIONPR(log, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double log10(double)", asFUNCTIONPR(log10, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double pow(double, double)", asFUNCTIONPR(pow, (double, double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double sqrt(double)", asFUNCTIONPR(sqrt, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double ceil(double)", asFUNCTIONPR(ceil, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double abs(double)", asFUNCTIONPR(fabs, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double floor(double)", asFUNCTIONPR(floor, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );
	r = engine->RegisterGlobalFunction("double fraction(double)", asFUNCTIONPR(fraction, (double), double), asCALL_CDECL); CHECK_RESULT( r >= 0 );

	return true;
}

NS_END
