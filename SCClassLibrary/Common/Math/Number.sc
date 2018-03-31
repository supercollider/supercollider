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

	bernouliB2n { _BernouliB2n; ^this.primitiveFailed }
	tangentT2n { _TangentT2n; ^this.primitiveFailed }

	/*  Gamma  */

	// "true gamma" of input value
	tgamma { _TGamma; ^this.primitiveFailed }
	// gamma(value + 1) - 1
	tgamma1pm1 { _TGamma1pm1; ^this.primitiveFailed }
	// log gamma
	lgamma { _LGamma; ^this.primitiveFailed }
	// digamma, trigamma, polygamma
	digamma { _Digamma; ^this.primitiveFailed }
	trigamma { _Trigamma; ^this.primitiveFailed }
	polygamma { |z| _Polygamma; ^this.primitiveFailed }
	// gamma(this) / gamma(b)
	tgammaRatio { |b| _TGammaRatio; ^this.primitiveFailed }
	// gamma(this) / gamma(this + delta)
	tgammaDeltaRatio { |delta| _TGammaDeltaRatio; ^this.primitiveFailed }
	// normalised lower/upper incomplete gamma function of a and z
	gammaP { |z| _GammaP; ^this.primitiveFailed }
	gammaQ { |z| _GammaQ; ^this.primitiveFailed }
	// full (non-normalised) lower/upper incomplete gamma function of a and z
	tgammaLower { |z| _TGammaLower; ^this.primitiveFailed }
	tgammaUpper { |z| _TGammaI; ^this.primitiveFailed }

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
	beta { |b| _Beta; ^this.primitiveFailed }
	// incomplete beta functions
	// All require 0 <= x <= 1
	// normalised incomplete beta function of a, b and x
	// require a,b >= 0, and in addition that not both a and b are zero
	ibeta { |b, x| _IBeta; ^this.primitiveFailed }
	// normalised complement of the incomplete beta function of a, b and x
	// require a,b >= 0, and in addition that not both a and b are zero
	ibetaC { |b, x| _IBetaC; ^this.primitiveFailed }
	// full (non-normalised) incomplete beta function of a, b and x
	// require a,b > 0
	betaFull { |b, x| _BetaFull; ^this.primitiveFailed }
	// full (non-normalised) complement of the incomplete beta function of a, b and x
	// require a,b > 0
	betaIFullC { |b, x| _BetaCFull; ^this.primitiveFailed }
	// Incomplete beta function inverses
	// Requires: a,b > 0 and 0 <= p <= 1
	ibetaInv { |b, p| _IBetaInv; ^this.primitiveFailed }
	// Requires: a,b > 0 and 0 <= q <= 1
	ibetaCInv { |b, q| _IBetaCInv; ^this.primitiveFailed }
	// Requires: b > 0, 0 < x < 1 and 0 <= p <= 1
	ibetaInvA { |x, p| _IBetaInvA; ^this.primitiveFailed }
	// Requires: b > 0, 0 < x < 1 and 0 <= q <= 1
	ibetaCInvA { |x, q| _IBetaCInvA; ^this.primitiveFailed }
	// Requires: a > 0, 0 < x < 1 and 0 <= p <= 1
	ibetaInvB { |x, p| _IBetaInvB; ^this.primitiveFailed }
	// Requires: a > 0, 0 < x < 1 and 0 <= q <= 1
	ibetaCInvB { |x, q| _IBetaCInvB; ^this.primitiveFailed }
	// Incomplete beta function derivative
	ibetaDerivative { |b, x| _IBetaDerivative; ^this.primitiveFailed }


	/*  Error functions */
	// error function of z
	erf { _Erf; ^this.primitiveFailed }
	// complement of error function
	erfC { _ErfC; ^this.primitiveFailed }
	// inverse error function of z
	erfInv { _ErfInv; ^this.primitiveFailed }
	// inverse of the complement of the error function of z
	erfCInv { _ErfCInv; ^this.primitiveFailed }


	/*  Polynomials  */
	// Legendre (and Associated) Polynomials
	//
	// See boost documentation for a note about the Condon-Shortley phase term of (-1)^m
	// "http://www.boost.org/doc/libs/1_65_1/libs/math/doc/html/math_toolkit/sf_poly/legendre.html"]
	//
	// TODO: where to clip values at [-1,1] ??
	legendreP { |x| _LegendreP; ^this.primitiveFailed }
	legendrePPrime { |x| _LegendrePPrime; ^this.primitiveFailed }
	legendrePZeros { _LegendrePZeros; ^this.primitiveFailed }
	legendrePAssoc { |m, x| _LegendrePAssoc; ^this.primitiveFailed }
	// TODO: proper way to catch error?
	legendreQ { |x|
		if (this < 0) {
			format("n = %, but Legendre Polynomial of the Second Kind requires n >= 0", this).throw
		};
		^prLegendreQ(this, x)
	}
	// TODO: name?
	prLegendreQ { |x| _LegendreQ; ^this.primitiveFailed }

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
	chebyshevT { |x| _ChebyshevT; ^this.primitiveFailed }
	chebyshevU { |x| _ChebyshevU; ^this.primitiveFailed }
	chebyshevTPrime { |x| _ChebyshevTPrime; ^this.primitiveFailed }

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

    sphericalHarmonic { |m, theta, phi| _SphericalHarmonic; ^this.primitiveFailed }
    sphericalHarmonicR { |m, theta, phi| _SphericalHarmonicR; ^this.primitiveFailed }
    sphericalHarmonicI { |m, theta, phi| _SphericalHarmonicI; ^this.primitiveFailed }


	/*	Bessel Functions  */

	//  First and Second kind
	cylBesselJ { |x| _CylBesselJ; ^this.primitiveFailed }
	cylNeumann { |x| _CylNeumann; ^this.primitiveFailed }
	// Zero finder for Bessel Functions of first and second kind
	// Retrieve one zero at a time, by index
	cylBesselJZero { |index| _CylBesselJZero; ^this.primitiveFailed }
	cylNeumannZero { |index| _CylNeumannZero; ^this.primitiveFailed }
	//  Modified, First and Second Kinds
	cylBesselI { |x| _CylBesselI; ^this.primitiveFailed }
	cylBesselK { |x| _CylBesselK; ^this.primitiveFailed }
	// Spherical, first and second kind
	sphBessel { |x| _SphBessel; ^this.primitiveFailed }
	sphNeumann { |x| _SphNeumann; ^this.primitiveFailed }
	// Derivatives
	cylBesselJPrime { |x| _CylBesselJPrime; ^this.primitiveFailed }
	cylNeumannPrime { |x| _CylNeumannPrime; ^this.primitiveFailed }
	cylBesselIPrime { |x| _CylBesselIPrime; ^this.primitiveFailed }
	cylBesselKPrime { |x| _CylBesselKPrime; ^this.primitiveFailed }
	sphBesselPrime { |x| _SphBesselPrime; ^this.primitiveFailed }
	sphNeumannPrime { |x| _SphNeumannPrime; ^this.primitiveFailed }


	/*  Hankel Functions  */

	// Cyclic
	cylHankel1 { |x| _CylHankel1; ^this.primitiveFailed }
	cylHankel2 { |x| _CylHankel2; ^this.primitiveFailed }
	// Spherical
	sphHankel1 { |x| _SphHankel1; ^this.primitiveFailed }
	sphHankel2 { |x| _SphHankel2; ^this.primitiveFailed }


	/*  Airy Functions  */

	airyAi { _AiryAi; ^this.primitiveFailed }
	airyBi { _AiryBi; ^this.primitiveFailed }
	airyAiPrime { _AiryAiPrime; ^this.primitiveFailed }
	airyBiPrime { _AiryBiPrime; ^this.primitiveFailed }
	airyAiZero { _AiryAiZero; ^this.primitiveFailed }
	airyBiZero { _AiryBiZero; ^this.primitiveFailed }


	/*  Elliptic Integrals  */

	// Carlson Form
	// Requires that all of the arguments are non-negative, and at most one may be zero
	ellintRf { |y, z| _EllintRf; ^this.primitiveFailed }
	// Requires that x and y are non-negative, with at most one of them zero, and that z >= 0
	ellintRd { |y, z| _EllintRd; ^this.primitiveFailed }
	// Requires that x, y and z are non-negative, with at most one of them zero, and that p != 0
	ellintRj { |y, z, p| _EllintRj; ^this.primitiveFailed }
	// Requires that x > 0 and that y != 0
	ellintRc { |y| _EllintRc; ^this.primitiveFailed }
	// Requires that x and y are non-negative
	ellintRg { |y, z| _EllintRg; ^this.primitiveFailed }

	//  Elliptic Integrals of the First, Second, Third Kind, D - Legendre Form
	// Requires -1 <= k <= 1
	ellint1 { |phi| _Ellint1; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellint1C { _Ellint1C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellint2 { |phi| _Ellint2; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellint2C { _Ellint2C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1 and n < 1/sin2(φ)
	ellint3 { |n, phi| _Ellint3; ^this.primitiveFailed	}
	// Requires -1 <= k <= 1 and n < 1
	ellint3C { |n| _Ellint3C; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellintD { |phi| _EllintD; ^this.primitiveFailed }
	// Requires -1 <= k <= 1
	ellintDC { _EllintDC; ^this.primitiveFailed }

	// Jacobi Zeta Function
	// Requires -1 <= k <= 1
	jacobiZeta { |phi| _JacobiZeta; ^this.primitiveFailed }

	// Heuman Lambda Function
	// Requires -1 <= k <= 1
	heumanLambda { |phi| _HeumanLambda; ^this.primitiveFailed }


	/*  Jacobi Elliptic Functions  */

	// k must be positive
	jacobiCd { |u| _JacobiCd; ^this.primitiveFailed }
	jacobiCn { |u| _JacobiCn; ^this.primitiveFailed }
	jacobiCs { |u| _JacobiCs; ^this.primitiveFailed }
	jacobiDc { |u| _JacobiDc; ^this.primitiveFailed }
	jacobiDn { |u| _JacobiDn; ^this.primitiveFailed }
	jacobiDs { |u| _JacobiDs; ^this.primitiveFailed }
	jacobiNc { |u| _JacobiNc; ^this.primitiveFailed }
	jacobiNd { |u| _JacobiNd; ^this.primitiveFailed }
	jacobiNs { |u| _JacobiNs; ^this.primitiveFailed }
	jacobiSc { |u| _JacobiSc; ^this.primitiveFailed }
	jacobiSd { |u| _JacobiSd; ^this.primitiveFailed }
	jacobiSn { |u| _JacobiSn; ^this.primitiveFailed }


	/*  Riemann Zeta Function  */

	zeta { _Zeta; ^this.primitiveFailed }


	/*  Exponential Integrals */

	expintEn { |z| _ExpintEn; ^this.primitiveFailed }
	expintEi { _ExpintEi; ^this.primitiveFailed }


	/*  Basic Functions  */

	sinPi { _SinPi; ^this.primitiveFailed }
	cosPi { _CosPi; ^this.primitiveFailed }
	log1p { _Log1p; ^this.primitiveFailed }
	expm1{ _ExpM1; ^this.primitiveFailed }
	cbrt { _Cbrt; ^this.primitiveFailed }
	sqrt1pm1 { _Sqrt1pm1; ^this.primitiveFailed }
	powm1{ |y| _PowM1; ^this.primitiveFailed }
	// hypot not needed... slightly slower than current implementation of hypot


	/*  Sinus Cardinal ("sinc") and Hyperbolic Sinus Cardinal Functions  */

	// sin(x) / x
	sincPi { _SincPi; ^this.primitiveFailed }
	// sinh(x) / x
	sinhcPi { _SinhcPi; ^this.primitiveFailed }


	/*  Inverse Hyperbolic Functions  */

	asinh { _Asinh; ^this.primitiveFailed }
	// requires x >= 1
	acosh { _Acosh; ^this.primitiveFailed }
	atanh { _Atanh; ^this.primitiveFailed }


	/*	Owen's T function */

	owensT { |a| _OwensT; ^this.primitiveFailed }
}
