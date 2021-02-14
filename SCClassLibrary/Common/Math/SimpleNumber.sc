SimpleNumber : Number {

	*new { ^this.shouldNotImplement(thisMethod) }

	isValidUGenInput { ^this.isNaN.not }
	numChannels { ^1 }

	magnitude { ^this.abs }
	angle { ^if(this >= 0) { 0.0 } { pi } }


	neg { _Neg; ^this.primitiveFailed }
	bitNot { _BitNot; ^this.primitiveFailed }
	abs { _Abs; ^this.primitiveFailed }
	ceil { _Ceil; ^this.primitiveFailed }
	floor { _Floor; ^this.primitiveFailed }
	frac { _Frac; ^this.primitiveFailed }
	sign { _Sign; ^this.primitiveFailed }
	squared { _Squared; ^this.primitiveFailed }
	cubed { _Cubed; ^this.primitiveFailed }
	sqrt { _Sqrt; ^this.primitiveFailed }
	exp { _Exp; ^this.primitiveFailed }
	reciprocal { _Recip; ^this.primitiveFailed }
	midicps { _MIDICPS; ^this.primitiveFailed }
	cpsmidi { _CPSMIDI; ^this.primitiveFailed }
	midiratio { _MIDIRatio; ^this.primitiveFailed }
	ratiomidi { _RatioMIDI; ^this.primitiveFailed }
	ampdb { _AmpDb; ^this.primitiveFailed }
	dbamp { _DbAmp; ^this.primitiveFailed }
	octcps { _OctCPS; ^this.primitiveFailed }
	cpsoct { _CPSOct; ^this.primitiveFailed }
	log { _Log; ^this.primitiveFailed }
	log2 { _Log2; ^this.primitiveFailed }
	log10 { _Log10; ^this.primitiveFailed }
	sin { _Sin; ^this.primitiveFailed }
	cos { _Cos; ^this.primitiveFailed }
	tan { _Tan; ^this.primitiveFailed }
	asin { _ArcSin; ^this.primitiveFailed }
	acos { _ArcCos; ^this.primitiveFailed }
	atan { _ArcTan; ^this.primitiveFailed }
	sinh { _SinH; ^this.primitiveFailed }
	cosh { _CosH; ^this.primitiveFailed }
	tanh { _TanH; ^this.primitiveFailed }
	rand { _Rand; ^this.primitiveFailed }
	rand2 { _Rand2; ^this.primitiveFailed }
	linrand { _LinRand; ^this.primitiveFailed }
	bilinrand { _BiLinRand; ^this.primitiveFailed }
	sum3rand { _Sum3Rand; ^this.primitiveFailed }

	distort { _Distort; ^this.primitiveFailed }
	softclip { _SoftClip; ^this.primitiveFailed }
	coin { _Coin; ^this.primitiveFailed }
	isPositive { ^this >= 0 }
	isNegative { ^this < 0 }
	isStrictlyPositive { ^this > 0 }
	isNaN { ^(this >= 0 or: { this <= 0 }).not }
	asBoolean    { ^this > 0 }
	booleanValue { ^this > 0 } // TODO in the long-run, deprecate for asBoolean
	binaryValue { ^this.sign.max(0) } // TODO in the long-run, deprecate for asInteger

	rectWindow { _RectWindow; ^this.primitiveFailed }
	hanWindow { _HanWindow; ^this.primitiveFailed }
	welWindow { _WelchWindow; ^this.primitiveFailed }
	triWindow { _TriWindow; ^this.primitiveFailed }

	scurve { _SCurve; ^this.primitiveFailed }
	ramp { _Ramp; ^this.primitiveFailed }

	+ { |aNumber, adverb| _Add; ^aNumber.performBinaryOpOnSimpleNumber('+', this, adverb) }
	- { |aNumber, adverb| _Sub; ^aNumber.performBinaryOpOnSimpleNumber('-', this, adverb) }
	* { |aNumber, adverb| _Mul; ^aNumber.performBinaryOpOnSimpleNumber('*', this, adverb) }
	/ { |aNumber, adverb| _FDiv; ^aNumber.performBinaryOpOnSimpleNumber('/', this, adverb) }
	mod { |aNumber, adverb| _Mod; ^aNumber.performBinaryOpOnSimpleNumber('mod', this, adverb) }
	div { |aNumber, adverb| _IDiv; ^aNumber.performBinaryOpOnSimpleNumber('div', this, adverb) }
	pow { |aNumber, adverb| _Pow; ^aNumber.performBinaryOpOnSimpleNumber('pow', this, adverb) }
	min { |aNumber, adverb| _Min; ^aNumber.performBinaryOpOnSimpleNumber('min', this, adverb) }
	max { |aNumber=0.0, adverb| _Max; ^aNumber.performBinaryOpOnSimpleNumber('max', this, adverb) }
	bitAnd { |aNumber, adverb| _BitAnd; ^aNumber.performBinaryOpOnSimpleNumber('bitAnd', this, adverb) }
	bitOr { |aNumber, adverb| _BitOr; ^aNumber.performBinaryOpOnSimpleNumber('bitOr', this, adverb) }
	bitXor { |aNumber, adverb| _BitXor; ^aNumber.performBinaryOpOnSimpleNumber('bitXor', this, adverb) }
	bitHammingDistance { |aNumber, adverb| _HammingDistance  ^aNumber.performBinaryOpOnSimpleNumber('hammingDistance', this, adverb) }
	bitTest { arg bit; ^( (this.bitAnd(1.leftShift(bit))) != 0) }
	lcm { |aNumber, adverb| _LCM; ^aNumber.performBinaryOpOnSimpleNumber('lcm', this, adverb) }
	gcd { |aNumber, adverb| _GCD; ^aNumber.performBinaryOpOnSimpleNumber('gcd', this, adverb) }
	round { |aNumber=1.0, adverb| _Round; ^aNumber.performBinaryOpOnSimpleNumber('round', this, adverb) }
	roundUp { |aNumber=1.0, adverb| _RoundUp; ^aNumber.performBinaryOpOnSimpleNumber('roundUp', this, adverb) }
	trunc { |aNumber=1.0, adverb| _Trunc; ^aNumber.performBinaryOpOnSimpleNumber('trunc', this, adverb) }
	atan2 { |aNumber, adverb| _Atan2; ^aNumber.performBinaryOpOnSimpleNumber('atan2', this, adverb) }
	hypot { |aNumber, adverb| _Hypot; ^aNumber.performBinaryOpOnSimpleNumber('hypot', this, adverb) }
	hypotApx { |aNumber, adverb| _HypotApx; ^aNumber.performBinaryOpOnSimpleNumber('hypotApx', this, adverb) }
	leftShift { |aNumber=1, adverb| _ShiftLeft; ^aNumber.performBinaryOpOnSimpleNumber('leftShift', this, adverb) }
	rightShift { |aNumber=1, adverb| _ShiftRight; ^aNumber.performBinaryOpOnSimpleNumber('rightShift', this, adverb) }
	unsignedRightShift { |aNumber, adverb| _UnsignedShift; ^aNumber.performBinaryOpOnSimpleNumber('unsignedRightShift', this, adverb) }
	ring1 { |aNumber, adverb| _Ring1; ^aNumber.performBinaryOpOnSimpleNumber('ring1', this, adverb) }
	ring2 { |aNumber, adverb| _Ring2; ^aNumber.performBinaryOpOnSimpleNumber('ring2', this, adverb) }
	ring3 { |aNumber, adverb| _Ring3; ^aNumber.performBinaryOpOnSimpleNumber('ring3', this, adverb) }
	ring4 { |aNumber, adverb| _Ring4; ^aNumber.performBinaryOpOnSimpleNumber('ring4', this, adverb) }
	difsqr { |aNumber, adverb| _DifSqr; ^aNumber.performBinaryOpOnSimpleNumber('difsqr', this, adverb) }
	sumsqr { |aNumber, adverb| _SumSqr; ^aNumber.performBinaryOpOnSimpleNumber('sumsqr', this, adverb) }
	sqrsum { |aNumber, adverb| _SqrSum; ^aNumber.performBinaryOpOnSimpleNumber('sqrsum', this, adverb) }
	sqrdif { |aNumber, adverb| _SqrDif; ^aNumber.performBinaryOpOnSimpleNumber('sqrdif', this, adverb) }
	absdif { |aNumber, adverb| _AbsDif; ^aNumber.performBinaryOpOnSimpleNumber('absdif', this, adverb) }
	thresh { |aNumber, adverb| _Thresh; ^aNumber.performBinaryOpOnSimpleNumber('thresh', this, adverb) }
	amclip { |aNumber, adverb| _AMClip; ^aNumber.performBinaryOpOnSimpleNumber('amclip', this, adverb) }
	scaleneg { |aNumber, adverb| _ScaleNeg; ^aNumber.performBinaryOpOnSimpleNumber('scaleneg', this, adverb) }
	clip2 { |aNumber=1, adverb| _Clip2; ^aNumber.performBinaryOpOnSimpleNumber('clip2', this, adverb) }
	fold2 { |aNumber=1, adverb| _Fold2; ^aNumber.performBinaryOpOnSimpleNumber('fold2', this, adverb) }
	wrap2 { |aNumber=1, adverb| _Wrap2; ^aNumber.performBinaryOpOnSimpleNumber('wrap2', this, adverb) }

	excess { |aNumber=1, adverb| _Excess; ^aNumber.performBinaryOpOnSimpleNumber('excess', this, adverb) }
	firstArg { |aNumber, adverb| _FirstArg; ^aNumber.performBinaryOpOnSimpleNumber('firstArg', this, adverb) }
	rrand { |aNumber, adverb| _RandRange; ^aNumber.performBinaryOpOnSimpleNumber('rrand', this, adverb) }
	exprand { |aNumber, adverb| _ExpRandRange; ^aNumber.performBinaryOpOnSimpleNumber('exprand', this, adverb) }

	== { |aNumber, adverb| _EQ; ^aNumber.perform('==', this, adverb) }
	!= { |aNumber, adverb| _NE; ^aNumber.perform('!=', this, adverb) }
	< { |aNumber, adverb| _LT; ^aNumber.performBinaryOpOnSimpleNumber('<', this, adverb) }
	> { |aNumber, adverb| _GT; ^aNumber.performBinaryOpOnSimpleNumber('>', this, adverb) }
	<= { |aNumber, adverb| _LE; ^aNumber.performBinaryOpOnSimpleNumber('<=', this, adverb) }
	>= { |aNumber, adverb| _GE; ^aNumber.performBinaryOpOnSimpleNumber('>=', this, adverb) }

	equalWithPrecision { |that, precision = 0.0001, relativePrecision = 0|
		^if(relativePrecision > 0) {
			absdif(this, that) < max(precision, relativePrecision * min(abs(this), abs(that)))
		} {
			absdif(this, that) < precision
		}
	}

	hash { _ObjectHash; ^this.primitiveFailed }

	asInteger { _AsInteger; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	asRect { ^Rect(this, this, this, this) }

	degrad { ^this * pi / 180 }
	raddeg { ^this * 180 / pi }

	performBinaryOpOnSimpleNumber { |aSelector, aNumber, adverb|
		BinaryOpFailureError(this, aSelector, [aNumber, adverb]).throw;
	}
	performBinaryOpOnComplex { |aSelector, aComplex, adverb| ^aComplex.perform(aSelector, this.asComplex, adverb) }
	performBinaryOpOnSignal { |aSelector, aSignal, adverb|
		BinaryOpFailureError(this, aSelector, [aSignal, adverb]).throw;
	}

	nextPowerOfTwo { ^this.nextPowerOf(2) }
	nextPowerOf { |base| ^pow(base, ceil(log(this) / log(base))) }
	nextPowerOfThree { ^pow(3, ceil(log(this) / log(3))) }
	previousPowerOf { |base| ^pow(base, ceil(log(this) / log(base)) - 1) }


	/*  Boost Special Functions  */

	//  Number Series:
	bernouliB2n { _BernouliB2n; ^this.primitiveFailed }
	tangentT2n { _TangentT2n; ^this.primitiveFailed }

	//  Gamma:
	tgamma { _TGamma; ^this.primitiveFailed }
	tgamma1pm1 { _TGamma1pm1; ^this.primitiveFailed }
	lgamma { _LGamma; ^this.primitiveFailed }
	digamma { _Digamma; ^this.primitiveFailed }
	trigamma { _Trigamma; ^this.primitiveFailed }
	polygamma { |z| _Polygamma; ^this.primitiveFailed }
	tgammaRatio { |b| _TGammaRatio; ^this.primitiveFailed }
	tgammaDeltaRatio { |delta| _TGammaDeltaRatio; ^this.primitiveFailed }
	gammaP { |z| _GammaP; ^this.primitiveFailed }
	gammaQ { |z| _GammaQ; ^this.primitiveFailed }
	tgammaLower { |z| _TGammaLower; ^this.primitiveFailed }
	tgammaUpper { |z| _TGammaI; ^this.primitiveFailed }
	//  Incomplete Gamma Function Inverses
	gammaPInv { |p| _GammaPInv; ^this.primitiveFailed }
	gammaQInv { |q| _GammaQInv; ^this.primitiveFailed }
	gammaPInvA { |p| _GammaPInvA; ^this.primitiveFailed }
	gammaQInvA { |q| _GammaQInvA; ^this.primitiveFailed }
	//  Derivatives of the Incomplete Gamma Function
	gammaPDerivative { |x| _GammaPDerivative; ^this.primitiveFailed }
	gammaQDerivative { |x| ^this.gammaPDerivative(x).neg }

	//	Factorials and Binomial Coefficients:
	factorial { _Factorial; ^this.primitiveFailed }
	doubleFactorial { _DoubleFactorial; ^this.primitiveFailed }
	risingFactorial { |i| _RisingFactorial; ^this.primitiveFailed }
	fallingFactorial { |i| _FallingFactorial; ^this.primitiveFailed }
	binomialCoefficient { |k| _BinomialCoefficient; ^this.primitiveFailed }

	//  Beta functions:
	beta { |b| _Beta; ^this.primitiveFailed }
	//  Incomplete beta functions
	ibeta { |b, x| _IBeta; ^this.primitiveFailed }
	ibetaC { |b, x| _IBetaC; ^this.primitiveFailed }
	betaFull { |b, x| _BetaFull; ^this.primitiveFailed }
	betaFullC { |b, x| _BetaCFull; ^this.primitiveFailed }
	//  Incomplete beta function inverses
	ibetaInv { |b, p| _IBetaInv; ^this.primitiveFailed }
	ibetaCInv { |b, q| _IBetaCInv; ^this.primitiveFailed }
	ibetaInvA { |x, p| _IBetaInvA; ^this.primitiveFailed }
	ibetaCInvA { |x, q| _IBetaCInvA; ^this.primitiveFailed }
	ibetaInvB { |x, p| _IBetaInvB; ^this.primitiveFailed }
	ibetaCInvB { |x, q| _IBetaCInvB; ^this.primitiveFailed }
	//  Incomplete beta function derivative
	ibetaDerivative { |b, x| _IBetaDerivative; ^this.primitiveFailed }

	//  Error functions:
	erf { _Erf; ^this.primitiveFailed }
	erfC { _ErfC; ^this.primitiveFailed }
	erfInv { _ErfInv; ^this.primitiveFailed }
	erfCInv { _ErfCInv; ^this.primitiveFailed }

	//  Polynomials:
	// Legendre (and Associated), Laguerre (and Associated),
	// Hermite, Chebyshev Polynomials (first & second kind, derivative, zero (root) finder)
	// See boost documentation for a note about the Condon-Shortley phase term of (-1)^m
	// "http://www.boost.org/doc/libs/1_65_1/libs/math/doc/html/math_toolkit/sf_poly/legendre.html"]
	legendreP { |x| _LegendreP; ^this.primitiveFailed }
	legendrePPrime { |x| _LegendrePPrime; ^this.primitiveFailed }
	legendrePZeros { _LegendrePZeros; ^this.primitiveFailed }
	legendrePAssoc { |m, x| _LegendrePAssoc; ^this.primitiveFailed }
	legendreQ { |x|
		//  Protect from l < 0. Boost won't catch this out of range value and interpreter hangs.
		if(this < 0) {
			format("n = %, but Legendre Polynomial of the Second Kind requires n >= 0", this).throw
		};
		^prLegendreQ(this, x)
	}
	prLegendreQ { |x| _LegendreQ; ^this.primitiveFailed }
	laguerre { |x| _Laguerre; ^this.primitiveFailed }
	laguerreAssoc { |m, x|
		//  Protect from m < 0. Boost won't catch this out of range value and interpreter hangs.
		if(this < 0) {
			format("n = %, but Associated Laguerre Polynomial requires n >= 0", this).throw
		};
		^prLaguerreAssoc(this, m, x)
	}
	prLaguerreAssoc { |m, x| _LaguerreAssoc; ^this.primitiveFailed }
	hermite { |x|
		//  Protect from m < 0. Boost won't catch this out of range value and interpreter hangs.
		if(this < 0) {
			format("n = %, but Hermite Polynomial requires n >= 0", this).throw
		};
		^prHermite(this, x)
	}
	prHermite { |x| _Hermite; ^this.primitiveFailed }
	chebyshevT { |x| _ChebyshevT; ^this.primitiveFailed }
	chebyshevU { |x| _ChebyshevU; ^this.primitiveFailed }
	chebyshevTPrime { |x| _ChebyshevTPrime; ^this.primitiveFailed }
	//  "https://en.wikipedia.org/wiki/Chebyshev_polynomials#Roots_and_extrema"
	//  "http://mathworld.wolfram.com/ChebyshevPolynomialoftheFirstKind.html"
	chebyshevTZeros {
		var n = this.asInteger;
		^(1..n).collect { |k|
			cos(pi * ((2 * k) - 1) / (2 * n))
		}
	}

	//  Spherical Harmonics
	sphericalHarmonic { |m, theta, phi| _SphericalHarmonic; ^this.primitiveFailed }
	sphericalHarmonicR { |m, theta, phi| _SphericalHarmonicR; ^this.primitiveFailed }
	sphericalHarmonicI { |m, theta, phi| _SphericalHarmonicI; ^this.primitiveFailed }

	//	Bessel Functions:
	//  First and second kind, zero finders, modified first and second kinds,
	//  spherical first and second kinds, derivatives
	cylBesselJ { |x| _CylBesselJ; ^this.primitiveFailed }
	cylNeumann { |x| _CylNeumann; ^this.primitiveFailed }
	cylBesselJZero { |index| _CylBesselJZero; ^this.primitiveFailed }
	cylNeumannZero { |index| _CylNeumannZero; ^this.primitiveFailed }
	cylBesselI { |x| _CylBesselI; ^this.primitiveFailed }
	cylBesselK { |x| _CylBesselK; ^this.primitiveFailed }
	sphBessel { |x| _SphBessel; ^this.primitiveFailed }
	sphNeumann { |x| _SphNeumann; ^this.primitiveFailed }
	cylBesselJPrime { |x| _CylBesselJPrime; ^this.primitiveFailed }
	cylNeumannPrime { |x| _CylNeumannPrime; ^this.primitiveFailed }
	cylBesselIPrime { |x| _CylBesselIPrime; ^this.primitiveFailed }
	cylBesselKPrime { |x| _CylBesselKPrime; ^this.primitiveFailed }
	sphBesselPrime { |x| _SphBesselPrime; ^this.primitiveFailed }
	sphNeumannPrime { |x| _SphNeumannPrime; ^this.primitiveFailed }

	//  Hankel Functions:
	cylHankel1 { |x| _CylHankel1; ^this.primitiveFailed }
	cylHankel2 { |x| _CylHankel2; ^this.primitiveFailed }
	sphHankel1 { |x| _SphHankel1; ^this.primitiveFailed }
	sphHankel2 { |x| _SphHankel2; ^this.primitiveFailed }

	//  Airy Functions:
	airyAi { _AiryAi; ^this.primitiveFailed }
	airyBi { _AiryBi; ^this.primitiveFailed }
	airyAiPrime { _AiryAiPrime; ^this.primitiveFailed }
	airyBiPrime { _AiryBiPrime; ^this.primitiveFailed }
	airyAiZero { _AiryAiZero; ^this.primitiveFailed }
	airyBiZero { _AiryBiZero; ^this.primitiveFailed }

	//  Elliptic Integrals:
	//  Carlson Form
	ellintRf { |y, z| _EllintRf; ^this.primitiveFailed }
	ellintRd { |y, z| _EllintRd; ^this.primitiveFailed }
	ellintRj { |y, z, p| _EllintRj; ^this.primitiveFailed }
	ellintRc { |y| _EllintRc; ^this.primitiveFailed }
	ellintRg { |y, z| _EllintRg; ^this.primitiveFailed }
	//  Legendre Form - First, Second, Third Kind, D
	ellint1 { |phi| _Ellint1; ^this.primitiveFailed }
	ellint1C { _Ellint1C; ^this.primitiveFailed }
	ellint2 { |phi| _Ellint2; ^this.primitiveFailed }
	ellint2C { _Ellint2C; ^this.primitiveFailed }
	ellint3 { |n, phi| _Ellint3; ^this.primitiveFailed	}
	ellint3C { |n| _Ellint3C; ^this.primitiveFailed }
	ellintD { |phi| _EllintD; ^this.primitiveFailed }
	ellintDC { _EllintDC; ^this.primitiveFailed }
	//  Jacobi Zeta, Heuman Lambda Functions
	jacobiZeta { |phi| _JacobiZeta; ^this.primitiveFailed }
	heumanLambda { |phi| _HeumanLambda; ^this.primitiveFailed }

	//  Jacobi Elliptic Functions:
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

	//  Riemann Zeta Function:
	zeta { _Zeta; ^this.primitiveFailed }

	//  Exponential Integrals:
	expintEn { |z| _ExpintEn; ^this.primitiveFailed }
	expintEi { _ExpintEi; ^this.primitiveFailed }

	//  Basic Functions:
	sinPi { _SinPi; ^this.primitiveFailed }
	cosPi { _CosPi; ^this.primitiveFailed }
	log1p { _Log1p; ^this.primitiveFailed }
	expm1{ _ExpM1; ^this.primitiveFailed }
	cbrt { _Cbrt; ^this.primitiveFailed }
	sqrt1pm1 { _Sqrt1pm1; ^this.primitiveFailed }
	powm1{ |y| _PowM1; ^this.primitiveFailed }
	// hypot not needed... slightly slower than current implementation of hypot

	//  Sinus Cardinal ("sinc") and Hyperbolic Sinus Cardinal Functions:
	sincPi { _SincPi; ^this.primitiveFailed }
	sinhcPi { _SinhcPi; ^this.primitiveFailed }

	//  Inverse Hyperbolic Functions:
	asinh { _Asinh; ^this.primitiveFailed }
	acosh { _Acosh; ^this.primitiveFailed }
	atanh { _Atanh; ^this.primitiveFailed }

	//	Owen's T function:
	owensT { |a| _OwensT; ^this.primitiveFailed }

	/*  end Boost Special Functions  */


	snap { |resolution = 1.0, margin = 0.05, strength = 1.0|
		var round = round(this, resolution);
		var diff = round - this;
		^if(abs(diff) < margin) { this + (strength * diff) } { this }
	}


	softRound { |resolution = 1.0, margin = 0.05, strength = 1.0|
		var round = round(this, resolution);
		var diff = round - this;
		^if(abs(diff) > margin) { this + (strength * diff) } { this }
	}

	linlin { |inMin, inMax, outMin, outMax, clip = \minmax|
		// linear to linear mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^(this - inMin) / (inMax - inMin) * (outMax - outMin) + outMin
	}

	linexp { |inMin, inMax, outMin, outMax, clip=\minmax|
		// linear to exponential mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^pow(outMax / outMin, (this - inMin) / (inMax - inMin)) * outMin
	}

	explin { |inMin, inMax, outMin, outMax, clip=\minmax|
		// exponential to linear mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^(log(this / inMin)) / (log(inMax / inMin)) * (outMax - outMin) + outMin
	}

	expexp { |inMin, inMax, outMin, outMax, clip=\minmax|
		// exponential to exponential mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^pow(outMax / outMin, log(this / inMin) / log(inMax / inMin)) * outMin
	}

	lincurve { |inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax|
		var grow, a, b, scaled;
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		if(abs(curve) < 0.001) {
			// If the value should be clipped, it has already been clipped (above).
			// If we got this far, then linlin does not need to do any clipping.
			// Inlining the formula here makes it even faster.
			^(this - inMin) / (inMax - inMin) * (outMax - outMin) + outMin
		};

		grow = exp(curve);
		a = outMax - outMin / (1.0 - grow);
		b = outMin + a;
		scaled = (this - inMin) / (inMax - inMin);

		^b - (a * pow(grow, scaled))
	}

	curvelin { |inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax|
		var grow, a, b, scaled;
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		if(abs(curve) < 0.001) {
			// If the value should be clipped, it has already been clipped (above).
			^(this - inMin) / (inMax - inMin) * (outMax - outMin) + outMin
		};

		grow = exp(curve);
		a = inMax - inMin / (1.0 - grow);
		b = inMin + a;

		^log((b - this) / a) * (outMax - outMin) / curve + outMin
	}

	bilin { |inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax|
		// triangular linear mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^if(this >= inCenter) {
			this.linlin(inCenter, inMax, outCenter, outMax, \none)
		} {
			this.linlin(inMin, inCenter, outMin, outCenter, \none)
		}
	}

	biexp { |inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax|
		// triangular exponential mapping
		switch(clip,
			\minmax, {
				if(this <= inMin) { ^outMin };
				if(this >= inMax) { ^outMax };
			},
			\min, {
				if(this <= inMin) { ^outMin };
			},
			\max, {
				if(this >= inMax) { ^outMax };
			}
		);
		^if(this >= inCenter) {
			this.explin(inCenter, inMax, outCenter, outMax, \none)
		} {
			this.explin(inMin, inCenter, outMin, outCenter, \none)
		}
	}

	moddif { |aNumber = 0.0, mod = 1.0|
		var diff = absdif(this, aNumber) % mod;
		var modhalf = mod * 0.5;
		^modhalf - absdif(diff, modhalf)
	}

	lcurve { |a = 1.0, m = 0.0, n = 1.0, tau = 1.0|
		var rTau, x = this.neg;
		^if(tau == 1.0) {
			a * (m * exp(x) + 1) / (n * exp(x) + 1)
		} {
			rTau = tau.reciprocal;
			a * (m * exp(x) * rTau + 1) / (n * exp(x) * rTau + 1)
		}
	}
	gauss { |standardDeviation|
		^(((sqrt(-2 * log(1.0.rand)) * sin(2pi.rand)) * standardDeviation) + this)
	}
	gaussCurve { |a = 1.0, b = 0.0, c = 1.0|
		^a * (exp(squared(this - b) / (-2.0 * squared(c))))
	}

	asPoint {
		^Point.new(this, this)
	}

	asWarp { |spec|
		^CurveWarp.new(spec, this)
	}

	// scheduled Routine support
	wait { ^this.yield }
	waitUntil { ^(this - thisThread.beats).max(0).yield }
	sleep {
		var thread = thisThread;
		thread.clock.sched(this, { thread.next; nil });
		nil.yield;
	}

	printOn { |stream|
		stream.putAll(this.asString);
	}
	storeOn { |stream|
		stream.putAll(this.asString);
	}


	rate { ^'scalar' } // scalarRate constant
	asAudioRateInput { ^if(this == 0) { Silent.ar } { DC.ar(this) } }

	madd  { |mul, add|
		^(this * mul) + add
	}

	lag    { ^this }
	lag2   { ^this }
	lag3   { ^this }
	lagud  { ^this }
	lag2ud { ^this }
	lag3ud { ^this }
	varlag { ^this }
	slew   { ^this }

	poll { |trig = 10, label, trigid = -1|
		^Poll(trig, this, label, trigid)
	}

	// support for writing synth defs
	writeInputSpec { |file, synth|
		var constIndex = synth.constants.at(this.asFloat);
		if(constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;
		};
		file.putInt32(-1);
		file.putInt32(constIndex);
	}

	series { |second, last|
		_SimpleNumberSeries
		^this.primitiveFailed
		/* var step, size;
		second = second ?? { if(this < last) { this + 1 } { this - 1 } };
		step = second - this;
		size = floor((last - this) / step + 0.001).asInteger + 1;
		^Array.series(size, this, step) */
	}

	seriesIter { |second, last|
		var step, size;

		if(second.isNil) {
			last = last ? inf;
			step = if(this < last, 1, -1);
		}{
			last ?? { last = if(second < this, -inf, inf) };
			step = second - this;
		};

		^if(step < 0) {
			r {
				var val = this;
				while {
					val >= last;
				} {
					val.yield;
					val = val + step;
				};
			}
		} {
			r {
				var val = this;
				while {
					val <= last;
				} {
					val.yield;
					val = val + step;
				}
			}
		}
	}


	degreeToKey { |scale, stepsPerOctave = 12|
		var scaleDegree = this.round.asInteger;
		var accidental = (this - scaleDegree) * 10.0;
		^scale.performDegreeToKey(scaleDegree, stepsPerOctave, accidental)
	}

	keyToDegree { |scale, stepsPerOctave = 12|
		^scale.performKeyToDegree(this, stepsPerOctave)
	}

	nearestInList { |list|  // collection is sorted
		^list.performNearestInList(this);
	}

	nearestInScale { |scale, stepsPerOctave = 12| // collection is sorted
		^scale.performNearestInScale(this, stepsPerOctave);
	}

	partition { |parts = 2, min = 1|
		// randomly partition a number into parts of at least min size :
		var n = this - (min - 1 * parts);
		^(1..n-1).scramble.keep(parts-1).sort.add(n).differentiate + (min - 1)
	}

	nextTimeOnGrid { |clock|
		^clock.nextTimeOnGrid(this, 0)
	}

	playAndDelta {}

	asQuant { ^Quant(this) }

	// a clock format inspired by ISO 8601 time interval display (truncated representation)
	// receiver is a time in seconds, returns string "ddd:hh:mm:ss.sss"
	// see String:asSecs for complement

	asTimeString { |precision = 0.001, maxDays = 365, dropDaysIfPossible = true|
		var number, decimal, days, hours, minutes, seconds, mseconds, isNegative;

		// min value of precision is 0.001; this ensures that we stick to 3 decimal places in the
		// formatted string.
		precision = max(precision, 0.001);

		number = this.round(precision);
		isNegative = number < 0;
		number = number.abs;
		decimal = number.asInteger;
		days = decimal.div(86400).min(maxDays);
		days = if(dropDaysIfPossible and: { days == 0 }) {
			days = ""
		} {
			days.asString.padLeft(3, "0").add($:);
		};
		if(isNegative) {days = "-" ++ days};
		hours = (decimal.div(3600) % 24).asString.padLeft(2, "0").add($:);
		minutes = (decimal.div(60) % 60).asString.padLeft(2, "0").add($:);
		seconds = (decimal % 60).asString.padLeft(2, "0").add($.);
		mseconds = number.frac * 1000;
		mseconds = mseconds.round.asInteger.asString.padLeft(3, "0");
		^days ++ hours ++ minutes ++ seconds ++ mseconds
	}

	asFraction { |denominator = 100, fasterBetter = true|
		_AsFraction
		// asFraction will return a fraction that is the best approximation up to the given
		// denominator.
		// if fasterBetter is true it may find a much closer approximation and do it faster.
		^this.primitiveFailed
	}

	prSimpleNumberSeries { | second, last |
		_SimpleNumberSeries
		^this.primitiveFailed
	}

	asBufWithValues {
		^this
	}

	// this method could be refactored by dispatching, but we're trying to keep the overhead low.

	schedBundleArrayOnClock { |clock, bundleArray, lag = 0, server, latency|
		var sendBundle = { server.sendBundle(latency ? server.latency, *bundleArray) };
		// "this" is the delta time for the clock (usually in beats)
		// "lag" is a tempo independent absolute lag time (in seconds)
		if (lag != 0) {
			if(this != 0) {
				// schedule on both clocks
				clock.sched(this, { SystemClock.sched(lag, sendBundle) })
			} {
				// only lag specified: schedule only on the system clock
				SystemClock.sched(lag, sendBundle)
			}
		} {
			if(this != 0) {
				// only delta specified: schedule only on the clock passed in
				clock.sched(this, sendBundle)
			} {
				// no delays: send directly
				sendBundle.value
			}
		}

	}
}
