// Knuth Constants.h

// Knuth Appendix A Table I  TABLES OF  NUMERICAL QUANTITIES   Page 701
// Scanned in from printed book using OCR software to convert to a text file.

// (C) Copyright Paul A Bristow, hetp Chromatography, 2001, 2002,
// and D E Knuth, Addison-Welsey ISBN 0201896834
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" with express or implied warranty,
// and with no claim as to its suitability for any purpose.

// Quantities which are frequently used in standard subroutines
// and in analysis of computer programs.             (40 decimal places)
// Used to check agreement with the values in math_constants.h

#ifndef KNUTH_CONSTANTS_H
#define KNUTH_CONSTANTS_H

namespace knuth
{
    const long double sqrtTwo        = 1.4142135623730950488016887242096980785697L; // sqrt 2
    const long double sqrtThree      = 1.7320508075688772935274463415058723669428L; // sqrt 3
    const long double sqrtFive       = 2.2360679774997896964091736687312762354406L; // sqrt 5
    const long double sqrtTen        = 3.1622776601683793319988935444327185337196L; // sqrt 10
    const long double cubeRootTwo    = 1.2599210498948731647672106072782283505703L; // cubert 2
    const long double cubeRootThree  = 1.4422495703074083823216383107801095883919L; // cubert 3
    const long double fourthRootTwo  = 1.1892071150027210667174999705604759152930L; // 4 rt 2
    const long double lnTwo          = 0.6931471805599453094172321214581765680755L; // ln 2
    const long double lnThree        = 1.0986122886681096913952452369225257046475L; // ln 3
    const long double lnTen          = 2.3025850929940456840179914546843642076011L; // ln 10
    const long double oneDivLnTwo    = 1.4426950408889634073599246810018921374266L; // 1 / ln 2
    const long double oneDivLnTen    = 0.4342944819032518276511289189166050822944L; // 1 / ln 10
    const long double pi             = 3.1415926535897932384626433832795028841972L; // pi
    const long double degree         = 0.0174532925199432957692369076848861271344L; // pi/180 degree
    const long double oneDivPi       = 0.3183098861837906715377675267450287240689L; // 1/pi
    const long double piSqr          = 9.8696044010893586188344909998761511353137L; // pi^2
    const long double sqrtPi         = 1.7724538509055160272981674833411451827975L; // sqrt(pi) == gamma(1/2)
    const long double gammaThird     = 2.6789385347077476336556929409746776441287L; // gamma 1/3
    const long double gammaTwoThirds = 1.3541179394264004169452880281545137855193L; // gamma 2/3
    const long double e              = 2.7182818284590452353602874713526624977572L; // e
    const long double oneDivE        = 0.3678794411714423215955237701614608674458L; // 1/e
    const long double eSqr           = 7.3890560989306502272304274605750078131803L; // e^2
    const long double euler          = 0.5772156649015328606065120900824024310422L; // euler
    const long double lnPi           = 1.1447298858494001741434273513530587116473L; // ln pi
    const long double phi            = 1.6180339887498948482045868343656381177203L; // phi
    const long double ePowEuler      = 1.7810724179901979852365041031071795491696L; // e^euler
    const long double ePowQuarterPi  = 2.1932800507380154565597696592787382234616L; // e^(pi/4)
    const long double sinOne         = 0.8414709848078965066525023216302989996226L; // sin(1)
    const long double cosOne         = 0.5403023058681397174009366074429766037323L; // cos(1)
    const long double zetaThree      = 1.2020569031595942853997381615114499907650L; // theta(3)
    const long double lnPhi          = 0.4812118250596034474977589134243684231352L; // ln phi
    const long double oneDivLnPhi    = 2.0780869212350275376013226061177957677422L; // 1/ln(phi)
}

// (C) Copyright Paul A Bristow, hetp Chromatography, 2001, 2002,
// and D E Knuth, Addison-Welsey ISBN 0201896834

#endif /* KNUTH_CONSTANTS_H */
