Number : Magnitude {
	isNumber { ^true }

	+ { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	- { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	* { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	/ { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	mod { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	div { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	pow { arg aNumber; ^this.subclassResponsibility(thisMethod) }

	performBinaryOpOnSeqColl { arg aSelector, aSeqColl, adverb;
		^aSeqColl.collect({ arg item;
			item.perform(aSelector, this, adverb)
		})
	}
	performBinaryOpOnPoint { arg op, aPoint, adverb;
		^Point.new(this.perform(op, aPoint.x, adverb), this.perform(op, aPoint.y, adverb));
	}

	// polar support
	rho { ^this }
	theta { ^0.0 }

	// complex support
	real { ^this }
	imag { ^0.0 }

	// conversion
	@ { arg aNumber; ^Point.new(this, aNumber) }
	complex { arg imaginaryPart; ^Complex.new(this, imaginaryPart) }
	polar { arg angle; ^Polar.new(this, angle) }

	// iteration
	for { arg endValue, function;
		var i, j = 0;
		i = this;
		while ({ i <= endValue }, { function.value(i, j); i = i + 1; j = j + 1 });
	}
	forBy { arg endValue, stepValue, function;
		var i, j=0;
		i = this;
		(stepValue > 0).if({
			while ({ i <= endValue }, { function.value(i,j); i = i + stepValue; j=j+1; });
		}, {
			while ({ i >= endValue }, { function.value(i,j); i = i + stepValue; j=j+1; });
		});
	}

	forSeries { arg second, last, function;
		// called by generator expression
		// compiler replaces this with special byte codes.
		var step, j=0;
		if (second.isNil) {
			last = last ? inf;
			step = if (this < last, 1, -1);
		}{
			last ?? { last = if (second < this, -inf, inf) };
			step = second - this;
		};
		^this.forBy(last, step, function)
	}


	/*  Boost Special Functions  */

	/* Number Series */

	bernouliNum { _BernouliB2n; ^this.primitiveFailed }
	tangentNum { _TangentT2n; ^this.primitiveFailed }

	/*  Gamma  */

	// "true gamma" of input value
	gamma { _TGamma; ^this.primitiveFailed }
	// gamma(value + 1) - 1
	gamma1pm1 { _TGamma1pm1; ^this.primitiveFailed }
	// log gamma
	gammaLn { _LGamma; ^this.primitiveFailed }
	// digamma, trigamma, polygamma
	gammaDi { _Digamma; ^this.primitiveFailed }
	gammaTri { _Trigamma; ^this.primitiveFailed }
	gammaPoly { |z| _Polygamma; ^this.primitiveFailed }
	// gamma(this) / gamma(b)
	gammaRatio { |b| _TGammaRatio; ^this.primitiveFailed }
	// gamma(this) / gamma(this + delta)
	gammaDeltaRatio { |delta| _TGammaDeltaRatio; ^this.primitiveFailed }
	// normalised lower/upper incomplete gamma function of a and z
	gammaP { |z| _GammaP; ^this.primitiveFailed }
	gammaQ { |z| _GammaQ; ^this.primitiveFailed }
	// full (non-normalised) lower/upper incomplete gamma function of a and z
	gammaFullLower { |z| _TGammaLower; ^this.primitiveFailed }
	gammaFullUpper { |z| _TGammaI; ^this.primitiveFailed }

	// Incomplete Gamma Function Inverses
	// Returns a value x such that: p = gammaP(a, x);
	// Requires: a > 0 and 1 >= p >= 0.
	gammaPInv { |p| _GammaPInv; ^this.primitiveFailed }
	// Returns a value x such that: q = gammaQ(a, x);
	// Requires: a > 0 and 1 >= q >= 0.
	gammaQInv { |q| _GammaQInv; ^this.primitiveFailed }
	// Returns a value a such that: p = gammaP(a, x);
	// Requires: x > 0 and 1 >= p >= 0.
	gammaPInvA { |p| _GammaPInvA; ^this.primitiveFailed }
	// Returns a value a such that: q = gammaQ(a, x);
	// Requires: x > 0 and 1 >= q >= 0.
	gammaQInvA { |q| _GammaQInvA; ^this.primitiveFailed }
	// Derivatives of the Incomplete Gamma Function
	gammaPDerivative { |x| _GammaPDerivative; ^this.primitiveFailed }
	gammaQDerivative { |x| ^this.gammaPDerivative(x).neg }


	/*	Factorials and Binomial Coefficients */

	// TODO: replace Integer:factorial?
	factorial { _Factorial; ^this.primitiveFailed }
	// TODO: overflow_error handling?
	// factorial {
	//     if (this > 170) {
	//         format("Resolution is too low for this factorial: % (max 170)", this).throw;
	//     } {
	//         ^prFactorial(this)
	//     }
	// }
	doubleFactorial { _DoubleFactorial; ^this.primitiveFailed }
	// both x and i can be negative as well as positive
	risingFactorial { |i| _RisingFactorial; ^this.primitiveFailed }
	// only defined for positive i, x can be either positive or negative
	fallingFactorial { |i| _FallingFactorial; ^this.primitiveFailed }
	// Requires k <= n
	binomialCoefficient { |k| _BinomialCoefficient; ^this.primitiveFailed }


	/* Beta functions */
	beta { |z| _Beta; ^this.primitiveFailed }
	// incomplete beta functions
	// All require 0 <= x <= 1
	// normalised incomplete beta function of a, b and x
	// require a,b >= 0, and in addition that not both a and b are zero
	betaI { |b, x| _IBeta; ^this.primitiveFailed }
	// normalised complement of the incomplete beta function of a, b and x
	// require a,b >= 0, and in addition that not both a and b are zero
	betaIC { |b, x| _IBetaC; ^this.primitiveFailed }
	// full (non-normalised) incomplete beta function of a, b and x
	// require a,b > 0
	betaIFull { |b, x| _IBetaFull; ^this.primitiveFailed }
	// full (non-normalised) complement of the incomplete beta function of a, b and x
	// require a,b > 0
	betaIFullC { |b, x| _IBetaFullC; ^this.primitiveFailed }
	// Incomplete beta function inverses
	// Requires: a,b > 0 and 0 <= p <= 1
	betaIInv { |b, p| _IBetaInv; ^this.primitiveFailed }
	// Requires: a,b > 0 and 0 <= q <= 1
	betaICInv { |b, q| _IBetaCInv; ^this.primitiveFailed }
	// Requires: b > 0, 0 < x < 1 and 0 <= p <= 1
	betaIInvA { |x, p| _IBetaInvA; ^this.primitiveFailed }
	// Requires: b > 0, 0 < x < 1 and 0 <= q <= 1
	betaICInvA { |x, q| _IBetaCInvA; ^this.primitiveFailed }
	// Requires: a > 0, 0 < x < 1 and 0 <= p <= 1
	betaIInvB { |x, p| _IBetaInvB; ^this.primitiveFailed }
	// Requires: a > 0, 0 < x < 1 and 0 <= q <= 1
	betaICInvB { |x, q| _IBetaCInvB; ^this.primitiveFailed }
	// Incomplete beta function derivative
	betaIDerivative { |b, x| _IBetaDerivative; ^this.primitiveFailed }


	/*  Error functions */
	// error function of z
	errorFunc { _Erf; ^this.primitiveFailed }
	// complement of error function
	errorFuncC { _ErfC; ^this.primitiveFailed }
	// inverse error function of z
	errorFuncInv { _ErfInv; ^this.primitiveFailed }
	// inverse of the complement of the error function of z
	errorFuncCInv { _ErfCInv; ^this.primitiveFailed }


	/*  Polynomials  */
	// Legendre (and Associated) Polynomials
	//
	// See boost documentation for a note about the Condon-Shortley phase term of (-1)^m
	// "http://www.boost.org/doc/libs/1_65_1/libs/math/doc/html/math_toolkit/sf_poly/legendre.html"]
	//
	// TODO: where to clip values at [-1,1] ??
	legendre { |x| _LegendreP; ^this.primitiveFailed }
	legendrePrime { |x| _LegendrePPrime; ^this.primitiveFailed }
	legendreZeros { _LegendrePZeros; ^this.primitiveFailed }
	legendreAssoc { |m, x| _LegendrePAssoc; ^this.primitiveFailed }
	// TODO: proper way to catch error?
	legendre2 { |x|
		if (this < 0) {
			format("n = %, but Legendre Polynomial of the Second Kind requires n >= 0", this).throw
		};
		^prLegendre2(this, x)
	}
	// TODO: name?
	prLegendre2 { |x| _LegendreQ; ^this.primitiveFailed }

	//  Laguerre polynomial

	laguerre { |x| _Laguerre; ^this.primitiveFailed }
	// TODO: proper way to catch error?
	laguerreAssoc { |m, x|
		if (this < 0) {
			format("n = %, but Associated Laguerre Polynomial requires n >= 0", this).throw
		};
		^prLaguerreAssoc(this, m, x);
	}
	prLaguerreAssoc { |m, x| _LaguerreAssoc; ^this.primitiveFailed }

	// Hermite Polynomial

	// TODO: proper way to catch error?
	hermite { arg x;
		if (this < 0) {
			format("n = %, but Hermite Polynomial requires n >= 0", this).throw
		};
		^prHermite(this, x);
	}
	prHermite { |x| _Hermite; ^this.primitiveFailed }

	// Chebyshev polynomials - first, second kind & derivative
	chebyshev { |x| _ChebyshevT; ^this.primitiveFailed }
	chebyshev2 { |x| _ChebyshevU; ^this.primitiveFailed }
	chebyshevPrime { |x| _ChebyshevTPrime; ^this.primitiveFailed }

	// calculate the roots (zeros) of Chebyshev polynomial
	// "https://en.wikipedia.org/wiki/Chebyshev_polynomials#Roots_and_extrema"
	// "http://mathworld.wolfram.com/ChebyshevPolynomialoftheFirstKind.html"
	chebyshevZeros {
		var n = this.asInt;
		^(1..n).collect({ arg k;
			cos(pi* ((2*k) - 1) / (2*n))
		});
	}

	// Spherical Harmonics

    sphHarm { |m, theta, phi| _SphericalHarmonic; ^this.primitiveFailed }
    sphHarmReal { |m, theta, phi| _SphericalHarmonicR; ^this.primitiveFailed }
    sphHarmImag { |m, theta, phi| _SphericalHarmonicI; ^this.primitiveFailed }


	/*	Bessel Functions  */

	//  First and Second Kinds
	bessel { |x| _CylBesselJ; ^this.primitiveFailed }
	bessel2 { |x| _CylNeumann; ^this.primitiveFailed }
	//  Modified, First and Second Kinds
	besselMod { |x| _CylBesselI; ^this.primitiveFailed }
	besselMod2 { |x| _CylBesselK; ^this.primitiveFailed }
	// Spherical, First and Second Kinds
	besselSph { |x| _SphBessel; ^this.primitiveFailed }
	besselSph2 { |x| _SphNeumann; ^this.primitiveFailed }
	// Derivatives
	besselPrime { |x| _CylBesselJPrime; ^this.primitiveFailed }
	besselPrime2 { |x| _CylNeumannPrime; ^this.primitiveFailed }
	besselModPrime { |x| _CylBesselIPrime; ^this.primitiveFailed }
	besselModPrime2 { |x| _CylBesselKPrime; ^this.primitiveFailed }
	besselSphPrime { |x| _SphBesselSphPrime; ^this.primitiveFailed }
	besselSphPrime2 { |x| _SphNeumannPrime; ^this.primitiveFailed }
	// Zero finder for Bessel Functions of first and second kind
	// Retrieve one zero at a time, by index
	besselZero { |index| _CylBesselJZero; ^this.primitiveFailed }
	besselZero2 { |index| _CylNeumannZero; ^this.primitiveFailed }


	/*  Hankel Functions  */

	// Cyclic
	hankelCyc { |x| _CylHankel1; ^this.primitiveFailed }
	hankelCyc2 { |x| _CylHankel2; ^this.primitiveFailed }
	// Spherical
	hankelSph { |x| _SphHankel1; ^this.primitiveFailed }
	hankelSph2 { |x| _SphHankel2; ^this.primitiveFailed }


	/*  Airy Functions  */

	airyAi { _AiryAi; ^this.primitiveFailed }
	airyBi { _AiryBi; ^this.primitiveFailed }
	airyAiPrime { _AiryAiPrime; ^this.primitiveFailed }
	airyBiPrime { _AiryBiPrime; ^this.primitiveFailed }
	airyZero { _AiryZero; ^this.primitiveFailed }


	/*  Elliptic Integrals  */

	// Carlson Form
	// Requires that all of the arguments are non-negative, and at most one may be zero
	ellipIntRf { |y, z| _EllintRf; ^this.primitiveFailed }
	// Requires that x and y are non-negative, with at most one of them zero, and that z >= 0
	ellipIntRd { |y, z| _EllintRd; ^this.primitiveFailed }
	// Requires that x, y and z are non-negative, with at most one of them zero, and that p != 0
	ellipIntRj { |y, z, p| _EllintRj; ^this.primitiveFailed }
	// Requires that x > 0 and that y != 0
	ellipIntRc { |y| _EllintRc; ^this.primitiveFailed }
	// Requires that x and y are non-negative
	ellipIntRg { |y, z| _EllintRg; ^this.primitiveFailed }

	//  Elliptic Integrals of the First, Second, Third Kind, D - Legendre Form
	// Requires -1 <= k <= 1
	ellipInt1i { |phi| _Ellint1I; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellipInt1c { _Ellint1C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellipInt2i { |phi| _Ellint2I; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellipInt2c { _Ellint2C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1 and n < 1/sin2(φ)
	ellipInt3i { |n, phi| _Ellint3I; ^this.primitiveFailed	}
	// Requires -1 <= k <= 1 and n < 1
	ellipInt3c { |n| _Ellint3C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellipIntDi { |phi| _EllintDI; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellipIntDc { _EllintDC; ^this.primitiveFailed }

	// Jacobi Zeta Function
	// Requires -1 <= k <= 1
	zetaJ { |phi| _JacobiZeta; ^this.primitiveFailed }

	// Heuman Lambda Function
	// Requires -1 <= k <= 1
	lambdaH { |phi| _HeumanLambda; ^this.primitiveFailed }


	/*  Jacobi Elliptic Functions  */

	// k must be positive
	ellipJcd { |u| _JacobiCd; ^this.primitiveFailed }
	ellipJcn { |u| _JacobiCn; ^this.primitiveFailed }
	ellipJcs { |u| _JacobiCs; ^this.primitiveFailed }
	ellipJdc { |u| _JacobiDc; ^this.primitiveFailed }
	ellipJdn { |u| _JacobiDn; ^this.primitiveFailed }
	ellipJds { |u| _JacobiDs; ^this.primitiveFailed }
	ellipJnc { |u| _JacobiNc; ^this.primitiveFailed }
	ellipJnd { |u| _JacobiNd; ^this.primitiveFailed }
	ellipJns { |u| _JacobiNs; ^this.primitiveFailed }
	ellipJsc { |u| _JacobiSc; ^this.primitiveFailed }
	ellipJsd { |u| _JacobiSd; ^this.primitiveFailed }
	ellipJsn { |u| _JacobiSn; ^this.primitiveFailed }


	/*  Zeta Function - Riemann  */

	zetaR { _Zeta; ^this.primitiveFailed }


	/*  Exponential Integrals */

	expIntEn { |z| _ExpIntEn; ^this.primitiveFailed }
	expInt { _ExpInt; ^this.primitiveFailed }


	/*  Basic Functions  */

	sinpi { _SinPi; ^this.primitiveFailed }
	cospi { _CosPi; ^this.primitiveFailed }
	log1p { _Log1p; ^this.primitiveFailed }
	expm1{ _ExpM1; ^this.primitiveFailed }
	cbrt { _Cbrt; ^this.primitiveFailed }
	sqrt1pm1 { _Sqrt1pm1; ^this.primitiveFailed }
	powm1{ |y| _PowM1; ^this.primitiveFailed }
	// not needed... slightly slower than current implementation of hypot
	// hypot { _Hypot2; ^this.primitiveFailed }


	/*  Sinus Cardinal ("sinc") and Hyperbolic Sinus Cardinal Functions  */

	// sin(x) / x
	sinc { _SinCpi; ^this.primitiveFailed }
	// sinh(x) / x
	sinhc { _SinHCpi; ^this.primitiveFailed }


	/*  Inverse Hyperbolic Functions  */

	asinh { _AsinH; ^this.primitiveFailed }
	// requires x >= 1
	acosh { _AcosH; ^this.primitiveFailed }
	atanh { _AtanH; ^this.primitiveFailed }


	/*	Owen's T function */

	owensT { |a| _OwensT; ^this.primitiveFailed }
}
