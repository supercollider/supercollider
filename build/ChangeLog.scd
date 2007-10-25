
//////////// change log /////////////

// user-visible changes since 22.09.2007, in preparation for SuperCollider v3.1 release:

// HEADLINES:

* 27.09.2007 SparseArray class added - jr
* 28.09.2007 Help.gui added - ds
* 01.10.2007 FFT and IFFT rewrite - now using more efficient libs, also allows user to vary the overlap and the window type, also large-sized FFTs are possible - ds
* 02.10.2007 UnpackFFT and PackFFT added - these allow for flexible frequency-domain manipulations inside synths - ds
* 04.10.2007 Pkey and Pif added - hjh
* 05.10.2007 reformed Patterns - all patterns accept patterns as arguments - jr
* 08.10.2007 change to UGen plugin loading fixes the audio dropout issue that various users have experienced - rb
* 08.10.2007 GeneralHID crossplatform HID wrapper - mb
* --.--.2007 many improvements to Quarks package-management system. gui improvements, dependency-handling improvements, etc - various
* 20.10.2007 added a Glossary file - sw
* --.10.2007 various new help files added, and many help files improved - various

// OTHER ADDITIONS / IMPROVEMENTS:

* 22.09.2007 change log added, much rejoicing
* 25.09.2007 added packagesource.sh script to produce source code bundles - ds
* 28.09.2007 IdentityDictionary:doesNotUnderstand now warns if adding a pseudo-method which overrides a real method - jr
* 28.09.2007 String:openHTMLFile added - ds
* 04.10.2007 Integer:collect and Integer:collectAs methods added - ds/jr
* 05.10.2007 Dwhite:new and Dbrown:new have default values for lo and hi - jr
* 10.10.2007 SC no longer automatically writes data (synthdefs, archive.scxtar) to the application folder - instead writes to "app support". This fixes problems with running SC using an unprivileged user account - ds
* 16.10.2007 SequenceableCollection:median speed improvement, approx ten times faster in many cases - ds
* 20.10.2007 Object:deprecated and DeprecatedError added to allow for method deprecation - sw
* 21.10.2007 Amplitude : attackTime and releaseTime can be modulated now - jr
* 25.10.2007 Collection : histo method improved and moved from mathLib to common - jr
 

// BUGFIXES:

* 29.09.2007 takekos bug fixed (obscure issue with garbage collection in arrays) - jm
* 01.10.2007 fixed off by one bug in Dswitch and Dswitch1 that caused a server crash - jr
* 09.10.2007 fixed deadlock and other problems in NSAttributedStringAdditions.m - rb
* 11.10.2007 fixed inaccurate automatic determination of whether SC is running as standalone - ds
* 14.10.2007 .quark files now saved correctly as plain-text, not RTF - ds
* 24.10.2007 fixed a bug in Pbeta - jp
