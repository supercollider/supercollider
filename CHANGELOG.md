# Change Log

3.9.1 (2017-02-05)
==================

Contributors to this release: antonhornquist, aspiers, brianlheim, cappelnord, florian-grond, gusano, jamshark70, patrickdupuis, redFrik, shimpe, telephon

General: Added
--------------

Introduced Guard integration that allows sclang UnitTests to be automatically rerun whenever a file changes ([#3369](https://github.com/supercollider/supercollider/pull/3369)).

Bleeding-edge builds are now uploaded for Windows. See the Windows README for details ([#3441](https://github.com/supercollider/supercollider/pull/3441)).

General: Fixed
--------------

Debugging information is improved when building a Windows installer package ([#3464](https://github.com/supercollider/supercollider/pull/3464)).

scsynth and supernova: Fixed
----------------------------

supernova only looked for plugins in a `plugins/` subfolder of the provided extensions directory. This has been fixed to be consistent with scsynth ([#3433](https://github.com/supercollider/supercollider/pull/3433)).

UGens: Fixed
------------

Fixed `Index`, `IndexL`, `FoldIndex`, `WrapIndex`, `IndexInBetween`, and `DetectIndex` incorrectly downsampling audio-rate index arguments ([#3436](https://github.com/supercollider/supercollider/pull/3436)).

sclang: Fixed
-------------

The GUI class library folders were installed even when building sclang without Qt, resulting in unbound primitives. This has been fixed ([#3456](https://github.com/supercollider/supercollider/pull/3456)).

Some default class library directories had to be manually created. sclang will now create them for you if they don't exist ([#3469](https://github.com/supercollider/supercollider/pull/3469)).

Class library: Fixed
--------------------

Fixed a Routine not being properly terminated when running `CmdPeriod.run` (or hitting an equivalent shortcut) when a `Server:plotTree` window is open ([#3423](https://github.com/supercollider/supercollider/pull/3423)).

Fixed `LevelIndicator:style_` doing nothing and printing the warning `Qt: WARNING: Do not know how to use an instance of class 'Meta_QLevelIndicatorStyle'` ([#3398](https://github.com/supercollider/supercollider/pull/3398)).

Fixed `Git.checkForGit` returning `nil` ([#3445](https://github.com/supercollider/supercollider/issues/3445)).

The SynthDef compiler optimizes `a + b.neg` to `a - b`, but other UGens that depend on `b.neg` would also be incorrectly removed in some cases. This has been fixed ([#3437](https://github.com/supercollider/supercollider/pull/3437)).

In 3.9.0, the `group` key broke in the "grain" event type. This has been fixed ([#3483](https://github.com/supercollider/supercollider/pull/3483)).

IDE & SCDoc: Added
------------------

New IDE themes have been introduced for the editor and post window: Solarized, Solarized Dark, and Dracula ([#3412](https://github.com/supercollider/supercollider/pull/3412), [#3410](https://github.com/supercollider/supercollider/pull/3410)).

IDE & SCDoc: Changed
--------------------

Set the default font in the IDE for macOS to Monaco, instead of the rather silly non-monospace font that has been the SC default for over a decade ([#3404](https://github.com/supercollider/supercollider/pull/3404)).

IDE & SCDoc: Fixed
------------------

Fixed duplicate SCIDE icons in GNOME, and fixed the SCIDE icon looking wrong ([#3380](https://github.com/supercollider/supercollider/pull/3380)).

Fixed SCDoc breaking with page titles containing a single quote character ([#3301](https://github.com/supercollider/supercollider/pull/3301)).

Fixed an error due to lack of input sanitization when trying to open help (Cmd+D/Ctrl+D) or references (Cmd+U/Ctrl+U) on text containing a double quote character ([#3277](https://github.com/supercollider/supercollider/pull/3277)).

3.9.0 (2018-01-13)
======================

We are proud to announce the arrival of SuperCollider 3.9.0! Apologies
for being so far behind schedule; we hope the improvements you'll find here
will more than make up for it. In 3.9.0, determined contributors have fixed
some of SuperCollider's major cross-platform compatibility demons, addressed
longstanding issues in the IDE and language, and added new features and bugfixes
across the board.

Many thanks to all who contributed to this release: adcxyz, awson, bagong,
brianlheim, cappelnord, carlocapocasa, crucialfelix, danstowell, defaultxr,
dyfer, elifieldsteel, gagnonlg, ghost, gusano, jamshark70, jd-m, jleben,
jmckernon, joshpar, jreus, LFSaw, llloret, LucaDanieli, Magicking,
miguel-negrao, muellmusik, patrickdupuis, porres, privong, redFrik, samaaron,
scztt, simdax, smoge, smrg-lm, snappizz, telephon, thormagnusson,
tiagomoraismorgado88, timsutton, vivid-synth, vividsnow, yurivict, and many
more in the SC community who helped in ways other than participation on GitHub.

Known Issues
------------

The IDE server status display turns yellow after a few seconds when opening `s.makeGui`.  This does not cause any usability issues ([#3310](https://github.com/supercollider/supercollider/issues/3310)).

Only the first pages of the HTML files produced by SCDoc are printed in web browsers ([#3395](https://github.com/supercollider/supercollider/issues/3395)).

The help browser does not remember the last position open in a document when navigating through history, and just jumps to the top of the file ([#3396](https://github.com/supercollider/supercollider/issues/3396)).

Supernova loads plugins from "Extensions/plugins" rather than "Extensions" ([#3391](https://github.com/supercollider/supercollider/issues/3391)).

`LevelIndicator.style()` is broken, which leads to confusing warning messages ([#3398](https://github.com/supercollider/supercollider/pull/3398)).

`File.copy` crashes the interpreter if the destination file exists ([#3401](https://github.com/supercollider/supercollider/issues/3401)).

On Windows, SerialPort is not available ([#1008](https://github.com/supercollider/supercollider/issues/1008)).

On Windows, Supernova is not available.

On Windows, the command-line sclang interpreter is not available.

General: Added
-----

scvim has seen numerous enhancements now that an actively maintained fork has been merged in ([scvim #11](https://github.com/supercollider/scvim/pull/11)).

SuperCollider can now be built on Windows using the MSYS2 toolchain, thanks in particular to @awson and @bagong. ([PortAudio #1](https://github.com/supercollider/portaudio/pull/1), [HIDAPI #5](https://github.com/supercollider/hidapi/pull/5), [#2473](https://github.com/supercollider/supercollider/pull/2473), [#2704](https://github.com/supercollider/supercollider/pull/2704))

SuperCollider can now be built on FreeBSD, thanks to @shamazmazum and @yurivict ([#2834](https://github.com/supercollider/supercollider/pull/2834), [#2704](https://github.com/supercollider/supercollider/pull/2704), [HIDAPI #8](https://github.com/supercollider/hidapi/pull/8), [#3131](https://github.com/supercollider/supercollider/pull/3131)).

Detailed documentation on creating macOS standalone applications with SuperCollider has been added, thanks to @adcxyz ([#2881](https://github.com/supercollider/supercollider/pull/2881)).

Support for multiple sclang clients connecting to the same server is greatly improved, thanks to @adcxyz.

A CODE_OF_CONDUCT.md and CONTRIBUTING.md have been added to the repository ([#3001](https://github.com/supercollider/supercollider/pull/3001)).

Higher-resolution raster versions of the SC cube logo have been added to the top-level `icons/` directory ([#3023](https://github.com/supercollider/supercollider/pull/3023)), and a retina-friendly `.icns` file ([#3060](https://github.com/supercollider/supercollider/pull/3060)).

General: Changed
-------

**Breaking change:** `sc_gcd` in the plugin interface now conforms to `gcd(n, 0) == n` instead of `gcd(n, 0) == abs(n)` ([#2980](https://github.com/supercollider/supercollider/pull/2980)). This also affects the method `SimpleNumber:gcd`.

The macOS plist file now shows the full version number for both the Version String and Shortened Version String ([#2487](https://github.com/supercollider/supercollider/pull/2487)).

General: Fixed
-----

A typo in the build system prevented the `-msse` compiler flag from being properly set for gcc and clang ([#2623](https://github.com/supercollider/supercollider/pull/2623)). This *may* fix subnormal number issues in scsynth that some users have been experiencing.

Fixed a fontification break in scel when too many classes are defined ([#2508](https://github.com/supercollider/supercollider/pull/2508)).

Fixed build failures on FreeBSD ([#3126](https://github.com/supercollider/supercollider/pull/3126)), GCC 7 ([#3226](https://github.com/supercollider/supercollider/pull/3226)), and newer versions of Boost ([#3227](https://github.com/supercollider/supercollider/pull/3227)).


scsynth and supernova: Added
-----

scsynth and supernova now support a `/version` command, which responds with a message of the form `/version.reply program major minor patch branch commit` ([#2546](https://github.com/supercollider/supercollider/pull/2546), [#2598](https://github.com/supercollider/supercollider/pull/2598)). See the Server Command Reference for full details.

scsynth and supernova: Changed
-------

On macOS, if scsynth's input and output devices have mismatched sample rates, an error is thrown and the server does not boot. Setting the number of input channels to 0 (`-i 0` on the command line and `s.options.numInputBusChannels = 0` in sclang) now bypasses this error ([#2610](https://github.com/supercollider/supercollider/pull/2610)).

Disabled Nagle's algorithm for TCP communication in scsynth ([#2613](https://github.com/supercollider/supercollider/pull/2613)). Nagle's algorithm increases bandwidth at the cost of delay, which is undesirable in the context of SuperCollider. Both supernova and sclang have it turned off.

scsynth and supernova: Fixed
-----

The `/b_read` and `/b_readChannel` messages experienced intermittent failures to read sound files, most notably affecting `Buffer.cueSoundFile`. This has been fixed ([#2793](https://github.com/supercollider/supercollider/pull/2793)).


UGens: Added
-----

A new UGen, `Sanitize`, replaces infinities, NaNs, and subnormals with another signal, zero by default ([#2561](https://github.com/supercollider/supercollider/pull/2561)).

The `doneAction` argument to DetectSilence can now be modulated ([#2379](https://github.com/supercollider/supercollider/pull/2379)).

UnaryOpUGen now supports the bitwise not operator `bitNot` ([#2381](https://github.com/supercollider/supercollider/pull/2381)). It used to simply fail silently.

UGens: Changed
-------

**Breaking change:** The application binary interface (ABI) for server plugins has changed ([#3129](https://github.com/supercollider/supercollider/pull/3129)). This has an important impact: **plugin binaries compiled for SuperCollider 3.8 will not work with SuperCollider 3.9** and vice versa. Please recompile your plugins.

**Breaking change:** `FOS.ar` with control-rate coefficient inputs incorrectly initialized its coefficients at 0 and ramped to the correct values over the first control period. This has been fixed ([#2658](https://github.com/supercollider/supercollider/pull/2658)). To restore old behavior, multiply each coefficient by `Line.ar(0, 1, ControlDur.ir)`.

UGens: Deprecated
----------

`Donce`, a demand-rate UGen with no identifiable purpose, is deprecated ([#2564](https://github.com/supercollider/supercollider/pull/2562)). It was most likely used in the production of electronic donce music.

UGens: Fixed
-----

A number of UGens were discovered to have serious initialization bugs ([#2333](https://github.com/supercollider/supercollider/issues/2333)) where the UGen would output an initial sample of garbage memory. This can create audio explosions if the buggy UGen's output is fed into certain filter UGens like LPF or Delay1. These bugs have been fixed, affecting:

- BeatTrack
- BeatTrack2
- CoinGate
- Convolution
- Convolution2
- Convolution2L
- Convolution3
- DetectSilence
- DiskIn
- DiskOut
- IFFT
- KeyTrack
- LFGauss
- PartConv
- PV_JensenAndersen
- PV_HainsworthFoote
- RunningSum
- StereoConvolution2L
- Unpack1FFT

Fixed a bug with `TGrains` ignoring the `amp` parameter ([#2809](https://github.com/supercollider/supercollider/pull/2809)).

`Dibrown` no longer ignores the `length` argument ([#2654](https://github.com/supercollider/supercollider/pull/2654)).

`Pitch` no longer ignores the `median` argument ([#2953](https://github.com/supercollider/supercollider/pull/2953)).

Fixed a build error in DiskIOUGens on Windows ([#3015](https://github.com/supercollider/supercollider/pull/3015)).

Fixed `AudioControl` outputting garbage data if a bus is mapped to it but nothing is playing to the bus ([#3063](https://github.com/supercollider/supercollider/pull/3063)).

Fixed incorrect math in `PanAz.ar` with audio-rate input signal and position ([3139](https://github.com/supercollider/supercollider/pull/3139)).


sclang: Added
-----

Regression tests for the sclang lexer, parser, and compiler have been added ([#2751](https://github.com/supercollider/supercollider/pull/2751)). This will make it easier to make fixes to these components in the future.

sclang: Changed
-------

**Breaking change:** sclang's nestable multiline comments had some mistakes. In particular, sometimes sclang's lexer would incorrectly process overlapping combinations of `/*` and `*/`, so e.g. `*/*/` would be interpreted like `*/ /* */`. This has been fixed ([#2625](https://github.com/supercollider/supercollider/pull/2625)).

The maximum number of MIDI ports has been increased from 16 to 128 ([#2494](https://github.com/supercollider/supercollider/pull/2494)).

The startup post "NumPrimitives = #" is reworded to "Found # primitives" ([#3139](https://github.com/supercollider/supercollider/pull/3139)).

sclang: Removed
-------

Removed some unhelpful memory addresses from call stack output in error printing ([#2951](https://github.com/supercollider/supercollider/pull/2951)).

Removed some accidentally retained debug posts when the language starts up ([#3135](https://github.com/supercollider/supercollider/pull/3135)).

sclang: Fixed
-----

Fixed help files failing to open on Windows if the user's name contains a non-ASCII character ([#2861](https://github.com/supercollider/supercollider/pull/2861)).

Fixed non-ASCII characters breaking the Visual Studio debugger ([#2861](https://github.com/supercollider/supercollider/pull/2861)).

Fixed a crash in `Object:perform` when the selector is an Array whose first element is not a Symbol, e.g. `0.perform([0])` ([#2617](https://github.com/supercollider/supercollider/pull/2617)).

`thisProcess.nowExecutingPath` is no longer corrupted by `Routine:stop` ([#2620](https://github.com/supercollider/supercollider/pull/2620)).

`TextView:selectedString_` now works when the selection size is zero ([#2648](https://github.com/supercollider/supercollider/pull/2648)).

Fixed a crash when a method or class/instance variable is named "`none`" ([#2638](https://github.com/supercollider/supercollider/pull/2638)).

Exceptions occurring in primitives no longer print unavoidable error messages even when wrapped in try-catch ([#2876](https://github.com/supercollider/supercollider/pull/2876)).

Fixed a crash when `Dictionary:keysValuesArrayDo` is called with `nil` as an argument ([#2799](https://github.com/supercollider/supercollider/pull/2799)).

Fixed `WebView:onLinkActivated` handler failing to fire ([#3003](https://github.com/supercollider/supercollider/pull/3003)).

Fixed GUI objects failing to display when launched from the `action` of `unixCmd` ([#3009](https://github.com/supercollider/supercollider/pull/3009)). You will still need `{ }.defer`, however.

Fixed `QImage:getColor` always returning zero for the green channel ([#3190](https://github.com/supercollider/supercollider/pull/3190)).


Class library: Added
-----

The UnitTest quark has been incorporated into the main repository ([#3168](https://github.com/supercollider/supercollider/pull/3168)).

Added a `rewind` method to `CollStream` ([#2400](https://github.com/supercollider/supercollider/pull/2400)).

Added four new class methods to `File` for convenience: `readAllString`, `readAllSignal`, `readAllStringHTML`, `readAllStringRTF` ([#2410](https://github.com/supercollider/supercollider/pull/2410)).

`Pstep` accepts an array as a duration argument ([#2511](https://github.com/supercollider/supercollider/pull/2511)).

Help files originating from extensions now display a plaque for visibility ([#2449](https://github.com/supercollider/supercollider/pull/2449)).

For consistency with other `Platform` class methods, `Platform.recordingsDir` may be used instead of `thisProcess.platform.recordingsDir` ([#2877](https://github.com/supercollider/supercollider/pull/2877)).

`SequenceableCollection` has two new instance methods: `flatten2` and `flatBelow` ([#2527](https://github.com/supercollider/supercollider/pull/2527)). Additionally, `flatten` is faster now.

The `~callback` function is now available for all `Event` types instead of just "on" events ([#2376](https://github.com/supercollider/supercollider/pull/2376)).

Event types now include a `parentEvent`, which provides default values. ([#3021](https://github.com/supercollider/supercollider/pull/3021)).

New aliases for done actions, e.g. `Done.freeSelf == 2`, are introduced for better readability ([#2616](https://github.com/supercollider/supercollider/pull/2616)). See the `Done` helpfile for details.

A new class, `Recorder`, allows recording independently of the `Server` object ([#2422](https://github.com/supercollider/supercollider/pull/2422)).

`SequenceableCollection:reduce` supports an adverb argument ([#2863](https://github.com/supercollider/supercollider/pull/2863)).

A `recordingsDir` method has been added directly to `Platform`, which transparently calls `thisProcess.platform.recordingsDir` ([#2877](https://github.com/supercollider/supercollider/pull/2877)).

`View:-resizeToBounds`, `View:-resizeToHint`, and `Window:-resizeToHint` were added to make it easier to force Views and Windows to automatically resize ([#2865](https://github.com/supercollider/supercollider/pull/2865)).

`Maybe` now supports collection methods `at`, `atAll`, `put`, `putAll`, `add`, `addAll` ([#2437](https://github.com/supercollider/supercollider/pull/2437)).

`BusPlug:-play` can now accept a `Bus` object ([#2845](https://github.com/supercollider/supercollider/pull/2845)).

Breadcrumb links in helpfiles now have separate links for each node in the hierarchy, and pages with multiple categories have separators between the categories ([#2916](https://github.com/supercollider/supercollider/pull/2916)).

`SoundFile:*openWrite` now takes additional parameters ([#2926](https://github.com/supercollider/supercollider/pull/2926)).

Two new instance methods were added to Symbol: `isBinaryOp` and `isIdentifier` ([#2955](https://github.com/supercollider/supercollider/pull/2955)).

Added three convenience methods: `View:resizeToBounds`, `View:resizeToHint`, and `Window:resizeToHint` ([#2865](https://github.com/supercollider/supercollider/pull/2865)).

Added `Collection:asEvent` for easy conversion to an `Event` ([#2871](https://github.com/supercollider/supercollider/pull/2871)).

`DeprecatedError` now shows you the file path of the deprecated method ([#3039](https://github.com/supercollider/supercollider/pull/3039)).

Added two new methods to `SimpleNumber`: `snap` and `softRound` ([#3160](https://github.com/supercollider/supercollider/pull/3160)).

`ReadableNodeIDAllocator` offers a new optional replacement for `PowerOfTwoAllocator` that assigns node IDs in a way more readable to humans when working with multiclient setups ([#3179](https://github.com/supercollider/supercollider/pull/3179)).

A new "booted" stage has been added to Server objects that have been booted but
may not be running yet, accessible via `Server:hasBooted` and
`Server.allBootedServers`
([#3275](https://github.com/supercollider/supercollider/pull/3275)).

Class library: Changed
-------

**Breaking change:** Rests in the patterns system have been restructured ([#2802](https://github.com/supercollider/supercollider/pull/2802)). Instead of using the `isRest` event property, events are considered rests if one of their properties is a `Rest` object. You must use instances of `Rest` rather than the rest class itself -- use of `Rest` instead of `Rest()` is now deprecated.

**Breaking change:** Fixed `Dictionary:==` only comparing the values of the two dictionaries, not the keys ([#2737](https://github.com/supercollider/supercollider/issues/2737)).

**Breaking change:** Fixed a mistake where `Pen.quadCurveTo` used the primitive for a cubic Bézier instead of quadratic ([#2553](https://github.com/supercollider/supercollider/pull/2553)). To restore the old behavior, change `Pen.quadCurveTo` to `Pen.curveTo`.

**Breaking change:** The convenience instance methods `Env:kr` and `Env:ar` had the arguments `mul` and `add` renamed to `levelScale` and `levelBias`, since they don't behave like typical `mul` and `add` arguments ([#2866](https://github.com/supercollider/supercollider/pull/2866)).

`Collection:processRest` returns the processed collection rather than the original ([#2497](https://github.com/supercollider/supercollider/pull/2497)).

The maximum number of MIDI ports has been increased ([#2494](https://github.com/supercollider/supercollider/pull/2494)).

Attempting to use a control-rate signal as an input to `Hasher.ar` now results in an error ([#2589](https://github.com/supercollider/supercollider/pull/2589)).

The "Cleaning up temp synthdefs..." post message is suppressed if there is nothing to clean up ([#2629](https://github.com/supercollider/supercollider/pull/2629)).

To match `Out` and `ReplaceOut`, `LocalOut` and `XOut` now correctly validate their input, checking for a non-zero number of channels ([#2659](https://github.com/supercollider/supercollider/pull/2657), [#2659](https://github.com/supercollider/supercollider/pull/2659)).

The argument to `Pattern:fin` has a default of 1 for consistency with `Object:fin` ([#2480](https://github.com/supercollider/supercollider/pull/2840/files)).

`Complex:reciprocal` is faster now ([#2890](https://github.com/supercollider/supercollider/pull/2890)).

`Buffer:write` takes floating point arguments, truncating them to integers ([#2547](https://github.com/supercollider/supercollider/pull/2547)).

Conversion methods among collection types has been improved and documented ([#2871](https://github.com/supercollider/supercollider/pull/2871)).

`clientID` is now protected from being changed while the server is running
([#3275](https://github.com/supercollider/supercollider/pull/3275)).

Class library: Deprecated
----------

`OSCresponder`, `OSCresponderNode`, and `OSCpathResponder` now emit deprecation messages, and will be removed after at least a year ([#2870](https://github.com/supercollider/supercollider/pull/2870)). Use `OSCFunc` or `OSCdef` instead.

`Speech` is deprecated ([#2424](https://github.com/supercollider/supercollider/pull/2424)), and will be removed in 3.10. The rationale is that its audio output is independent of the server (severely limiting use in compositions), it depends on a proprietary macOS API with no prospect of cross-platform compatibility, and it is too niche to justify inclusion in the core library.

The WiiMote classes (`WiiMote`, `WiiMoteIRObject`, `WiiCalibrationInfo`, `WiiMoteGUI`, `WiiRemoteGUI`, `WiiNunchukGUI`) are deprecated ([#2698](https://github.com/supercollider/supercollider/pull/2698)). They never reached a stable state and have gone unmaintained and unused for years.

`AudioIn` is deprecated and will be removed in some future version ([#2482](https://github.com/supercollider/supercollider/pull/2482)). It was provided only for backward compatibility with SC2, so its deprecation is long overdue. Use `SoundIn` instead.

`SplayZ` has been deprecated for a long time, but it's finally on the "official" deprecation track and will be removed in 3.10 ([#2631](https://github.com/supercollider/supercollider/pull/2631)). Use `SplayAz` instead.

`TDuty_old` has been deprecated for a long time, but it now emits a warning and will be removed in 3.10 ([#2677](https://github.com/supercollider/supercollider/pull/2677)). Use `TDuty` instead.

`Watcher` is an old alias for `SkipJack` provided for backward compatibility. It is officially deprecated and will be removed in 3.10 ([#2700](https://github.com/supercollider/supercollider/pull/2700)).

`Server:recordNode` is deprecated. Use `Recorder:recordNode` instead (e.g. `s.recorder.recordNode`) ([#2422](https://github.com/supercollider/supercollider/pull/2422)).

The `Server.set` class variable is deprecated. Use `Server.all` instead ([#2422](https://github.com/supercollider/supercollider/pull/2422)).

`SimpleNumber:quantize` is deprecated. Use `SimpleNumber:snap` instead ([#3160](https://github.com/supercollider/supercollider/pull/3160)).

`Server:userSpecifiedClientID` is deprecated. Use `Server:clientID` instead
([#3275](https://github.com/supercollider/supercollider/pull/3275)).

Class library: Removed
-------

Removed non-functional stub methods and classes related to Image: the classes ImageFilter and ImageKernel, and the Image instance methods lockFocus, unlockFocus, applyFilters, filters, filteredWith, addFilter, removeFilter, flatten, invert, crop, applyKernel ([#2867](https://github.com/supercollider/supercollider/pull/2867)).

`Module`, an unmaintained and unused class for serialization of Synths, has been moved to a quark ([#2703](https://github.com/supercollider/supercollider/pull/2703)).

Removed the `openHelpFile` instance methods of `Object`, `String`, `Method`, and `Quark`. These methods have been deprecated since 3.8.

Removed `String:openTextFile` and `Symbol:openTextFile`. Use `String:openDocument` and `Symbol:openTextFile` instead. These methods have been deprecated since 3.8.

Class library: Fixed
-----

A number of instance methods in `Buffer` and `Bus` did not properly check to see if the object has already been freed, and would act on buffer #0 or bus #0 (which is especially dangerous for the `free` instance method). They now safeguard against this case and throw errors ([#2936](https://github.com/supercollider/supercollider/pull/2936), [#2960](https://github.com/supercollider/supercollider/pull/2960), [#2993](https://github.com/supercollider/supercollider/pull/2993)).

The `useRanger` option in `EnvirGui` broke in 3.7. This has been fixed ([#2418](https://github.com/supercollider/supercollider/pull/2418)).

`IdentityDictionary` methods `collect`, `select`, and `reject` retain references to the `parent` and `proto` objects ([#2507](https://github.com/supercollider/supercollider/pull/2507)).

On Linux, some MIDI methods created method override warnings. These have been silenced ([#2717](https://github.com/supercollider/supercollider/pull/2717)).

The "key" argument to `Pn` was not properly set on the first repeat. This has been fixed ([#2833](https://github.com/supercollider/supercollider/pull/2833)).

Fixed errors when using a DragSource inside a CompositeView object ([#2804](https://github.com/supercollider/supercollider/issues/2804)).

Fixed an interpreter crash when defining a SynthDef whose name is too long ([#2821](https://github.com/supercollider/supercollider/pull/2821)). More specifically, the inputs to `UnixFILE:putPascalString` and `CollStream:putPascalString` are now validated.

Server crashes are better handled by the interpreter ([#2453](https://github.com/supercollider/supercollider/pull/2453)).

The time display and the "start recording", "pause recording", and "stop recording" menu items now cooperate better with running `Server:record`, `Server:pauseRecording`, and `Server:stopRecording` ([#2422](https://github.com/supercollider/supercollider/pull/2422)).

`Server:makeGui` and `Server:makeWindow` broke in 3.8 — the fields in the windows went blank. They are working again ([#2422](https://github.com/supercollider/supercollider/pull/2422)).

A timing error with `NodeProxy:-clear` was fixed ([#2845](https://github.com/supercollider/supercollider/pull/2845)).

`SoundFileView` correctly displays its grid and does not draw the grid on top of the selection box ([#2872](https://github.com/supercollider/supercollider/pull/2872)).

The macOS plist file now shows the full version number for both the Version String and Shortened Version String ([#2487](https://github.com/supercollider/supercollider/pull/2487)).

Fixed instances of accidentally silencing error messages caused by neglecting to call `Object:primitiveFailed` ([#2908](https://github.com/supercollider/supercollider/pull/2908)).

Patched the possibility of inconsistent `TempoClock` state when the tempo is set via `setTempoAtSec` ([#2078](https://github.com/supercollider/supercollider/pull/2078)).

Fixed memory spikes when using `MIDIFunc.sysex` with a large `srcID` ([#3005](https://github.com/supercollider/supercollider/pull/3005)).

Fixed spaces sometimes being rendered as `%20` in links in SCDoc ([#3033](https://github.com/supercollider/supercollider/pull/3033)).

Fixed `Function:plot` showing an empty graph if the server wasn't booted when the method was invoked ([#3047](https://github.com/supercollider/supercollider/pull/3047)).

Fixed blatant errors in `Collection:asAssociations` and `Collection:asPairs` where elements were dropped ([#3101](https://github.com/supercollider/supercollider/pull/3101)).

Fixed bugs in `NodeProxy` when using external servers ([#3103](https://github.com/supercollider/supercollider/pull/3103)).

`History` now outputs a correct timestamp on Windows ([#3045](https://github.com/supercollider/supercollider/pull/3045)).

Fixed Volume control failing to be persistent when rebooting the server ([#3125](https://github.com/supercollider/supercollider/pull/3125)).

Fixed `SimpleNumber:asTimeString` producing nonsensical results with the "precision" argument ([#3166](https://github.com/supercollider/supercollider/pull/3166)).

`Server:clientID` can now be changed, allowing multiple clients connect to the same server ([#3178](https://github.com/supercollider/supercollider/pull/3178)).

History and HistoryGui have been cleaned up
([#3267](https://github.com/supercollider/supercollider/pull/3267)).

Fixed duplicate node IDs involving `Server.initTree`
([#3265](https://github.com/supercollider/supercollider/pull/3265)).

Fixed supernova crashing when too many controls are used
([#3196](https://github.com/supercollider/supercollider/issues/3196)).

`Volume` now respects lag time when it is instantiated or destroyed
([#3332](https://github.com/supercollider/supercollider/pull/3332)).

IDE & SCDoc: Added
-----

Entries in the Documents docklet can be reordered, and document tabs will automatically reorder to reflect this ([#2555](https://github.com/supercollider/supercollider/pull/2555)).

"Edit > Preferences > Editor > Display" has a new option that allows replacing tabs with a dropdown whose items are alphabetically ordered ([#2555](https://github.com/supercollider/supercollider/pull/2555)). This makes navigation easier in some performance contexts.

IDE & SCDoc: Changed
-------

Server actions, which were previously in the "Language" menu, have been moved out to their own "Server" menu ([#3049](https://github.com/supercollider/supercollider/pull/3049)).

Changed "occurrences" to "matches" in the status bar in the Find and Replace features ([#2702](https://github.com/supercollider/supercollider/pull/2702)).

Many minor improvements were made to the look and feel of the documentation ([#2944](https://github.com/supercollider/supercollider/pull/2944), [#2945](https://github.com/supercollider/supercollider/pull/2945), [#2947](https://github.com/supercollider/supercollider/pull/2947), [#2948](https://github.com/supercollider/supercollider/pull/2948), [#2967](https://github.com/supercollider/supercollider/pull/2967), [#3006](https://github.com/supercollider/supercollider/pull/3006), [#3022](https://github.com/supercollider/supercollider/pull/3022), [#3025](https://github.com/supercollider/supercollider/pull/3025), [#3034](https://github.com/supercollider/supercollider/pull/3034), [#3175](https://github.com/supercollider/supercollider/pull/3175), [#3346](https://github.com/supercollider/supercollider/pull/3346)).

IDE & SCDoc: Fixed
-----

Fixed SCDoc refusing to index any further documents if one document has a malformed `copymethod::` command ([#3050](https://github.com/supercollider/supercollider/pull/3050)).

Some Linux systems had unreadable font colors in the autocomplete tooltips. This has been (finally) fixed ([#2672](https://github.com/supercollider/supercollider/pull/2762)).

Fixed a bug where `Document:selectedString_` had no effect ([#2849](https://github.com/supercollider/supercollider/pull/2849)).

New tabs are now inserted to the right of the current tab instead of all the way at the end ([#3053](https://github.com/supercollider/supercollider/pull/3053)).

The help browser now has keyboard shortcuts for navigating back and forward ([#3056](https://github.com/supercollider/supercollider/pull/3056)). These shortcuts are OS-dependent and given to us by [Qt](http://doc.qt.io/qt-5.9/qkeysequence.html#standard-shortcuts).

Fixed the "Find in page..." feature in the help viewer skipping every other occurrence ([#2903](https://github.com/supercollider/supercollider/pull/2903)).

Fixed HTML checkboxes appearing in the upper left of the help viewer ([#3028](https://github.com/supercollider/supercollider/pull/3028)).

Fixed the right-click menu for the tabs appearing in the wrong place in macOS ([#3042](https://github.com/supercollider/supercollider/issues/3042)).

# [3.8.0](https://github.com/supercollider/supercollider/tree/3.8.0) (2016-09-23)
[Full Changelog](https://github.com/supercollider/supercollider/compare/3.7.2...3.8.0)

##  API change

- Increase the default number of audio buses from 128 to 1024
  [#2239](https://github.com/supercollider/supercollider/pull/2239) by [vivid-synth](https://github.com/vivid-synth)
- server plugins: Unify panning behavior of granular ugens
  [#2136](https://github.com/supercollider/supercollider/pull/2136) by [snappizz](https://github.com/snappizz)
- scsynth: commandline option  (-B) to bind to specific address
  [#2095](https://github.com/supercollider/supercollider/pull/2095) by [llloret](https://github.com/llloret)
- PathName has potentially superfluous methods
  [#1909](https://github.com/supercollider/supercollider/issues/1909) by [telephon](https://github.com/telephon)
- class library: sound file view - rename argument startframe -> startFrame to match convention
  [#1684](https://github.com/supercollider/supercollider/pull/1684) by [telephon](https://github.com/telephon)

##  comp: scsynth

- Add commit to version info
  [#2243](https://github.com/supercollider/supercollider/pull/2243) by [vivid-synth](https://github.com/vivid-synth)
- fftlib: remove duplicate defines
  [#2089](https://github.com/supercollider/supercollider/pull/2089) by [sonoro1234](https://github.com/sonoro1234)
- SC_fftlib: allow ensurewindow to be called
  [#2008](https://github.com/supercollider/supercollider/pull/2008) by [sonoro1234](https://github.com/sonoro1234)
- jack: add metadata support
  [#1951](https://github.com/supercollider/supercollider/pull/1951) by [ventosus](https://github.com/ventosus)
- reboot of the internal server crashes interpreter
  [#1526](https://github.com/supercollider/supercollider/issues/1526) by [ceremona](https://github.com/ceremona)

##  comp: server plugins

- server plugins: Gendy*: fix initialization bug
  [#2331](https://github.com/supercollider/supercollider/pull/2331) by [snappizz](https://github.com/snappizz)
- Bug 1355 demand env overshoot
  [#2164](https://github.com/supercollider/supercollider/pull/2164) by [baconpaul](https://github.com/baconpaul)
- Allow audio-rate phasein argument to VOsc
  [#2140](https://github.com/supercollider/supercollider/pull/2140) by [snappizz](https://github.com/snappizz)
- PartConv avoid using first ir section twice
  [#2015](https://github.com/supercollider/supercollider/pull/2015) by [sonoro1234](https://github.com/sonoro1234)
- plugins: do not advance stages before env start
  [#1424](https://github.com/supercollider/supercollider/pull/1424) by [scztt](https://github.com/scztt)

##  comp: supernova

- Add supernova to some scsynth-specific docs
  [#2256](https://github.com/supercollider/supercollider/pull/2256) by [vivid-synth](https://github.com/vivid-synth)
- build: don't auto-enable supernova if old cmake
  [#2170](https://github.com/supercollider/supercollider/pull/2170) by [danstowell](https://github.com/danstowell)
- supernova: use c++14 move captures and proper move semantics
  [#2141](https://github.com/supercollider/supercollider/pull/2141) by [timblechmann](https://github.com/timblechmann)
- supernova: relax handling of malformed c_set messages
  [#2113](https://github.com/supercollider/supercollider/pull/2113) by [timblechmann](https://github.com/timblechmann)
- supernova: portaudio_backend changed #elif for #else
  [#1947](https://github.com/supercollider/supercollider/pull/1947) by [sonoro1234](https://github.com/sonoro1234)
- supernova: minor improvements
  [#1908](https://github.com/supercollider/supercollider/pull/1908) by [timblechmann](https://github.com/timblechmann)

##  comp: sclang

- remove references to CocoaBridge
  [#2351](https://github.com/supercollider/supercollider/pull/2351) by [snappizz](https://github.com/snappizz)
- lang: Remove debug message
  [#2250](https://github.com/supercollider/supercollider/pull/2250) by [gusano](https://github.com/gusano)
- sclang: Ensure git object is defined for checkout
  [#2216](https://github.com/supercollider/supercollider/pull/2216) by [scztt](https://github.com/scztt)
- asStringPerc SCLang Crash
  [#2168](https://github.com/supercollider/supercollider/pull/2168) by [baconpaul](https://github.com/baconpaul)
- Classname as Selector crashes
  [#2166](https://github.com/supercollider/supercollider/pull/2166) by [baconpaul](https://github.com/baconpaul)
- Reimplement match lang ip
  [#1972](https://github.com/supercollider/supercollider/pull/1972) by [muellmusik](https://github.com/muellmusik)
- sclang resolves relative paths in the language configuration file relative to the current working directory
  [#1927](https://github.com/supercollider/supercollider/issues/1927) by [miguel-negrao](https://github.com/miguel-negrao)
- class library: plot does not specify min and max, so add 'plotAudio' method with -1 .. 1 range
  [#1846](https://github.com/supercollider/supercollider/pull/1846) by [telephon](https://github.com/telephon)
- Fix presumed bug in d0f475d (min should be max). Fixes #1842
  [#1843](https://github.com/supercollider/supercollider/pull/1843) by [danstowell](https://github.com/danstowell)
- parser doesn't catch backward variable definitions
  [#1514](https://github.com/supercollider/supercollider/issues/1514) by [telephon](https://github.com/telephon)
- sclang crashes on 0.exit
  [#1438](https://github.com/supercollider/supercollider/issues/1438) by [jamshark70](https://github.com/jamshark70)
- Crash when using a class name in binop method call syntax
  [#669](https://github.com/supercollider/supercollider/issues/669) by [jamshark70](https://github.com/jamshark70)

##  comp: class library

- Fix printing filepath at end of recording
  [#2435](https://github.com/supercollider/supercollider/pull/2435) by [bagong](https://github.com/bagong)
- Add default value to second argument in string replace
  [#2433](https://github.com/supercollider/supercollider/pull/2433) by [bagong](https://github.com/bagong)
- server: don't set client id on failure
  [#2405](https://github.com/supercollider/supercollider/pull/2405) by [telephon](https://github.com/telephon)
- class library: add missing proxy init in Ndef
  [#2387](https://github.com/supercollider/supercollider/pull/2387) by [telephon](https://github.com/telephon)
- EnvirGui calls `this.widgets` but has none
  [#2371](https://github.com/supercollider/supercollider/issues/2371) by [telephon](https://github.com/telephon)
- Topic/deprecating
  [#2370](https://github.com/supercollider/supercollider/pull/2370) by [crucialfelix](https://github.com/crucialfelix)
- Make sure Spec is inited before ControlSpec
  [#2346](https://github.com/supercollider/supercollider/pull/2346) by [antonhornquist](https://github.com/antonhornquist)
- Handle spaces in SCDoc internal links
  [#2336](https://github.com/supercollider/supercollider/pull/2336) by [crucialfelix](https://github.com/crucialfelix)
- Usage of Class.initClassTree(ControlSpec) can remove common mappings
  [#2318](https://github.com/supercollider/supercollider/issues/2318) by [antonhornquist](https://github.com/antonhornquist)
- SoundFileView.schelp wrong argument names
  [#2311](https://github.com/supercollider/supercollider/issues/2311) by [jamshark70](https://github.com/jamshark70)
- Add linting and fix classlib indention
  [#2298](https://github.com/supercollider/supercollider/pull/2298) by [gusano](https://github.com/gusano)
- class library: TreeView: add alias methods addChild, insertChild, childAt
  [#2260](https://github.com/supercollider/supercollider/pull/2260) by [snappizz](https://github.com/snappizz)
- Classlib: Quarks: Fix typo in incompatibility message (SC: camel case…
  [#2245](https://github.com/supercollider/supercollider/pull/2245) by [jamshark70](https://github.com/jamshark70)
- Classlib: GUI: asLayoutElement interface for non-Views that work in layouts
  [#2234](https://github.com/supercollider/supercollider/pull/2234) by [jamshark70](https://github.com/jamshark70)
- ServerStatus - watcher notifies the server
  [#2226](https://github.com/supercollider/supercollider/pull/2226) by [gusano](https://github.com/gusano)
- class library: server gui updates better
  [#2215](https://github.com/supercollider/supercollider/pull/2215) by [telephon](https://github.com/telephon)
- class library: update link when server failed to start
  [#2209](https://github.com/supercollider/supercollider/pull/2209) by [snappizz](https://github.com/snappizz)
- s.makeGui server window broken in master
  [#2202](https://github.com/supercollider/supercollider/issues/2202) by [jamshark70](https://github.com/jamshark70)
- Classlib: GUI: Support "has-a" GUI objects by calling asView within Layouts
  [#2188](https://github.com/supercollider/supercollider/pull/2188) by [jamshark70](https://github.com/jamshark70)
- Move Spec/Warp etc. out of GUI back into Control
  [#2182](https://github.com/supercollider/supercollider/pull/2182) by [crucialfelix](https://github.com/crucialfelix)
- asOSCArgArray : do not expand a string into an array
  [#2133](https://github.com/supercollider/supercollider/pull/2133) by [telephon](https://github.com/telephon)
- class library: implement audio rate lag control
  [#2127](https://github.com/supercollider/supercollider/pull/2127) by [telephon](https://github.com/telephon)
- cmake needs to install HUT directory on OSX
  [#2116](https://github.com/supercollider/supercollider/issues/2116) by [sensestage](https://github.com/sensestage)
- class library: streamArg correctly yields
  [#2110](https://github.com/supercollider/supercollider/pull/2110) by [telephon](https://github.com/telephon)
- class library: move asOSCArgArray out of backwards_compatibility
  [#2108](https://github.com/supercollider/supercollider/pull/2108) by [telephon](https://github.com/telephon)
- class library: fix compatibility of asOSCArgArray
  [#2097](https://github.com/supercollider/supercollider/pull/2097) by [telephon](https://github.com/telephon)
- List can't be used in Synth arg list
  [#2096](https://github.com/supercollider/supercollider/issues/2096) by [jamshark70](https://github.com/jamshark70)
- deprecated-3.7: remove it in master?
  [#2038](https://github.com/supercollider/supercollider/issues/2038) by [danstowell](https://github.com/danstowell)
- Topic/server unresponsive
  [#1935](https://github.com/supercollider/supercollider/pull/1935) by [crucialfelix](https://github.com/crucialfelix)
- clean up PathName code
  [#1912](https://github.com/supercollider/supercollider/pull/1912) by [telephon](https://github.com/telephon)
- returning nil as UGen graph returns uninformative error
  [#1771](https://github.com/supercollider/supercollider/issues/1771) by [telephon](https://github.com/telephon)
- Improve error handling in Server:prepareForRecord
  [#1580](https://github.com/supercollider/supercollider/issues/1580) by [bagong](https://github.com/bagong)
- Fix range in Function:plot
  [#1454](https://github.com/supercollider/supercollider/pull/1454) by [thormagnusson](https://github.com/thormagnusson)
- Move Spec back into Control
  [#2181](https://github.com/supercollider/supercollider/issues/2181) by [multivac61](https://github.com/multivac61)

##  comp: help

- add News in 3.8
  [#2365](https://github.com/supercollider/supercollider/pull/2365) by [snappizz](https://github.com/snappizz)
- help: History: remove use of .speak in examples
  [#2352](https://github.com/supercollider/supercollider/pull/2352) by [snappizz](https://github.com/snappizz)
- help: SoundFileView: fix argument names
  [#2350](https://github.com/supercollider/supercollider/pull/2350) by [snappizz](https://github.com/snappizz)
- Add example to LatoocarfianL
  [#2335](https://github.com/supercollider/supercollider/pull/2335) by [crucialfelix](https://github.com/crucialfelix)
- Update Pitch.schelp
  [#2334](https://github.com/supercollider/supercollider/pull/2334) by [crucialfelix](https://github.com/crucialfelix)
- Revert "Update Ndef.schelp"
  [#2323](https://github.com/supercollider/supercollider/pull/2323) by [nuss](https://github.com/nuss)
- move SVG logo from HelpSource/images to icons
  [#2312](https://github.com/supercollider/supercollider/pull/2312) by [snappizz](https://github.com/snappizz)
- Signal.schelp: small typo fix in play:loop desc
  [#2308](https://github.com/supercollider/supercollider/pull/2308) by [jaschanarveson](https://github.com/jaschanarveson)
- Fix some anchor links in help
  [#2293](https://github.com/supercollider/supercollider/pull/2293) by [vivid-synth](https://github.com/vivid-synth)
- Reference: Server-Command: clarify Wave Fill flags
  [#2288](https://github.com/supercollider/supercollider/pull/2288) by [jaschanarveson](https://github.com/jaschanarveson)
- help: SC3 vs SC2: add historical note
  [#2287](https://github.com/supercollider/supercollider/pull/2287) by [snappizz](https://github.com/snappizz)
- help: create help file for ScIDE
  [#2285](https://github.com/supercollider/supercollider/pull/2285) by [snappizz](https://github.com/snappizz)
- help: VLayout: change QLineLayout to LineLayout
  [#2283](https://github.com/supercollider/supercollider/pull/2283) by [snappizz](https://github.com/snappizz)
- help: Dialog: expand explanation of openPanel and savePanel
  [#2282](https://github.com/supercollider/supercollider/pull/2282) by [snappizz](https://github.com/snappizz)
- Add "(NRT)" to the NRT help file title (searchability)
  [#2281](https://github.com/supercollider/supercollider/pull/2281) by [vivid-synth](https://github.com/vivid-synth)
- Update Ndef.schelp
  [#2273](https://github.com/supercollider/supercollider/pull/2273) by [tiagmoraismorgado](https://github.com/tmm2018)
- Update MouseX.schelp
  [#2272](https://github.com/supercollider/supercollider/pull/2272) by [tiagmoraismorgado](https://github.com/tmm2018)
- Update MouseButton.schelp
  [#2271](https://github.com/supercollider/supercollider/pull/2271) by [tiagmoraismorgado](https://github.com/tmm2018)
- help: Remove some outdated GUI info
  [#2248](https://github.com/supercollider/supercollider/pull/2248) by [snappizz](https://github.com/snappizz)
- Add supernova to the list of components
  [#2244](https://github.com/supercollider/supercollider/pull/2244) by [vivid-synth](https://github.com/vivid-synth)
- help: add SVG logo to images dir
  [#2235](https://github.com/supercollider/supercollider/pull/2235) by [snappizz](https://github.com/snappizz)
- Replace use of .send(s) with .add in class examples.
  [#2223](https://github.com/supercollider/supercollider/pull/2223) by [kisielk](https://github.com/kisielk)
- help: update Document.schelp to match current API
  [#2219](https://github.com/supercollider/supercollider/pull/2219) by [snappizz](https://github.com/snappizz)
- help: RangeSlider: correct dragging instructions
  [#2210](https://github.com/supercollider/supercollider/pull/2210) by [snappizz](https://github.com/snappizz)
- examples: replace .send(s) with .add
  [#2208](https://github.com/supercollider/supercollider/pull/2208) by [snappizz](https://github.com/snappizz)
- PulseDivider.schelp: Fix typo and clarify div
  [#2199](https://github.com/supercollider/supercollider/pull/2199) by [kisielk](https://github.com/kisielk)
- Pmono.schelp: fix duplicate "the"
  [#2187](https://github.com/supercollider/supercollider/pull/2187) by [kisielk](https://github.com/kisielk)
- Fix a typo in SynthDef.schelp
  [#2186](https://github.com/supercollider/supercollider/pull/2186) by [kisielk](https://github.com/kisielk)
- fixing Henon help
  [#2150](https://github.com/supercollider/supercollider/pull/2150) by [tiagmoraismorgado](https://github.com/tmm2018)
- document DelTapWr/DelTapRd/MultiTap delay time caveats
  [#2132](https://github.com/supercollider/supercollider/pull/2132) by [snappizz](https://github.com/snappizz)
- help: document range better for LFGauss UGen
  [#2121](https://github.com/supercollider/supercollider/pull/2121) by [telephon](https://github.com/telephon)
- help: hidfunc, clarify nil on usage and usageID
  [#2104](https://github.com/supercollider/supercollider/pull/2104) by [llloret](https://github.com/llloret)
- help fixes  for OSCfunc & Env
  [#2087](https://github.com/supercollider/supercollider/pull/2087) by [miczac](https://github.com/miczac)
- RecordBuf.schelp: corrected Synthnames for proper playback, lower volume for overdub
  [#2071](https://github.com/supercollider/supercollider/pull/2071) by [miczac](https://github.com/miczac)
- AudioIn.schelp: tamed feedback in example, removed "patching" example
  [#2070](https://github.com/supercollider/supercollider/pull/2070) by [miczac](https://github.com/miczac)
- Pulse.schelp: added missing .kr method, beautified examples
  [#2069](https://github.com/supercollider/supercollider/pull/2069) by [miczac](https://github.com/miczac)
- Helpfile fixing
  [#2061](https://github.com/supercollider/supercollider/pull/2061) by [LFSaw](https://github.com/LFSaw)
- Helpfile fixing
  [#2057](https://github.com/supercollider/supercollider/pull/2057) by [jreus](https://github.com/jreus)
- Revert "LFSaw.schelp: Note and example for special initial-phase behaviour"
  [#2056](https://github.com/supercollider/supercollider/pull/2056) by [miczac](https://github.com/miczac)
- Klank & DynKlank - better structure for examples
  [#2055](https://github.com/supercollider/supercollider/pull/2055) by [miczac](https://github.com/miczac)
- RLPF.schelp: adjust example to avoid exploding filter due to frequency folding when modulated.
  [#2053](https://github.com/supercollider/supercollider/pull/2053) by [miczac](https://github.com/miczac)
- Helpfile fixing
  [#2042](https://github.com/supercollider/supercollider/pull/2042) by [adcxyz](https://github.com/adcxyz)
- Added closeWhenDone to .cue
  [#2039](https://github.com/supercollider/supercollider/pull/2039) by [tapage](https://github.com/tapage)
- help: minor spell fixes in tutorials area
  [#2004](https://github.com/supercollider/supercollider/pull/2004) by [llloret](https://github.com/llloret)
- help: and some more help typos and spell fixes
  [#2003](https://github.com/supercollider/supercollider/pull/2003) by [llloret](https://github.com/llloret)
- help: fixed some more typos and spelling
  [#2002](https://github.com/supercollider/supercollider/pull/2002) by [llloret](https://github.com/llloret)
- help: Help updates for the Classes directory
  [#1999](https://github.com/supercollider/supercollider/pull/1999) by [llloret](https://github.com/llloret)
- Improve "Writing UGens" documentation
  [#1997](https://github.com/supercollider/supercollider/pull/1997) by [snappizz](https://github.com/snappizz)
- More help documentation updates
  [#1989](https://github.com/supercollider/supercollider/pull/1989) by [llloret](https://github.com/llloret)
- help: fixed some typos and spelling
  [#1988](https://github.com/supercollider/supercollider/pull/1988) by [llloret](https://github.com/llloret)
- link was wrong
  [#1980](https://github.com/supercollider/supercollider/pull/1980) by [grirgz](https://github.com/grirgz)
- Document ServerOptions.*devices as OS X only
  [#1949](https://github.com/supercollider/supercollider/pull/1949) by [snappizz](https://github.com/snappizz)
- Clarified nil argument behavior in OSCdef help
  [#1940](https://github.com/supercollider/supercollider/pull/1940) by [antonhornquist](https://github.com/antonhornquist)
- Change title of main help file from "Help" to "SuperCollider [version]"
  [#1928](https://github.com/supercollider/supercollider/pull/1928) by [snappizz](https://github.com/snappizz)
- MultiTap and DelTapRd/Wr could use a Note in help doc
  [#1883](https://github.com/supercollider/supercollider/issues/1883) by [mtmccrea](https://github.com/mtmccrea)
- CocoaBridge seems dead, but examples and doc are still there
  [#1629](https://github.com/supercollider/supercollider/issues/1629) by [muellmusik](https://github.com/muellmusik)

##  comp: HID

- Update pointer to submodule hidapi
  [#2420](https://github.com/supercollider/supercollider/pull/2420) by [bagong](https://github.com/bagong)
- Adjust pointer to hidapi to fix cmp0048 bug breaking build for cmake …
  [#2342](https://github.com/supercollider/supercollider/pull/2342) by [bagong](https://github.com/bagong)
- Update pointer to hidapi submodule
  [#2330](https://github.com/supercollider/supercollider/pull/2330) by [bagong](https://github.com/bagong)
- HID: various small additions to adjust to developments in hid submodule
  [#2123](https://github.com/supercollider/supercollider/pull/2123) by [bagong](https://github.com/bagong)
- Switch to hidapi subomodule in sc org repo
  [#2111](https://github.com/supercollider/supercollider/pull/2111) by [bagong](https://github.com/bagong)

##  comp: Qt GUI

- Move Qt primitives out of "common" to fix non-Qt builds
  [#2299](https://github.com/supercollider/supercollider/pull/2299) by [vivid-synth](https://github.com/vivid-synth)
- build: fix qt configuration for case-sensitive OS X
  [#2262](https://github.com/supercollider/supercollider/pull/2262) by [snappizz](https://github.com/snappizz)
- Document Qt >= 5.6 not working in Linux
  [#2206](https://github.com/supercollider/supercollider/pull/2206) by [snappizz](https://github.com/snappizz)
- QPen - add RenderHints to StringInRect
  [#2019](https://github.com/supercollider/supercollider/pull/2019) by [gusano](https://github.com/gusano)

##  comp: SCDoc

- Adjust Help title for Windows return value of folder
  [#2392](https://github.com/supercollider/supercollider/pull/2392) by [bagong](https://github.com/bagong)
- fix link to class file source in scdoc header
  [#2131](https://github.com/supercollider/supercollider/pull/2131) by [snappizz](https://github.com/snappizz)
- Change "source" to "helpfile source" in scdoc footer
  [#2130](https://github.com/supercollider/supercollider/pull/2130) by [snappizz](https://github.com/snappizz)
- SCDoc HTML renderer includes literal spaces in links with anchors
  [#1650](https://github.com/supercollider/supercollider/issues/1650) by [jamshark70](https://github.com/jamshark70)
- SCDoc shows getters where there are only setters
  [#837](https://github.com/supercollider/supercollider/issues/837) by [muellmusik](https://github.com/muellmusik)

##  comp: build

- Document cmake dependency for supernova
  [#2207](https://github.com/supercollider/supercollider/pull/2207) by [snappizz](https://github.com/snappizz)
- Explain /path/to/qt5 in Linux README
  [#2205](https://github.com/supercollider/supercollider/pull/2205) by [snappizz](https://github.com/snappizz)
- move jackey include dir from server to scsynth
  [#2179](https://github.com/supercollider/supercollider/pull/2179) by [flv0](https://github.com/flv0)
- Fix oscpack build fail on various architectures
  [#2174](https://github.com/supercollider/supercollider/pull/2174) by [danstowell](https://github.com/danstowell)
- Simplify MS Compiler detection to avoid cmake warning
  [#2120](https://github.com/supercollider/supercollider/pull/2120) by [bagong](https://github.com/bagong)
- Travis: Update OSX build system and correct omissions
  [#2092](https://github.com/supercollider/supercollider/pull/2092) by [bagong](https://github.com/bagong)
- sclang: changed some boost code to std
  [#2091](https://github.com/supercollider/supercollider/pull/2091) by [llloret](https://github.com/llloret)
- Switch to portaudio repo in supercollider org
  [#2088](https://github.com/supercollider/supercollider/pull/2088) by [bagong](https://github.com/bagong)
- travis: move git key to env, aws fixes
  [#1987](https://github.com/supercollider/supercollider/pull/1987) by [scztt](https://github.com/scztt)
- Set correct hash for portaudio submodule
  [#1971](https://github.com/supercollider/supercollider/pull/1971) by [bagong](https://github.com/bagong)
- cmake: library locations, hide them from the default listing of user cmake variables
  [#1968](https://github.com/supercollider/supercollider/pull/1968) by [danstowell](https://github.com/danstowell)
- Add cmake options list to READMEs
  [#1965](https://github.com/supercollider/supercollider/pull/1965) by [vivid-synth](https://github.com/vivid-synth)
- Update linux travis recipe
  [#1932](https://github.com/supercollider/supercollider/pull/1932) by [patrickdupuis](https://github.com/patrickdupuis)
- build: bump GCC version requirement up from 4.7 to 4.8
  [#1839](https://github.com/supercollider/supercollider/pull/1839) by [danstowell](https://github.com/danstowell)
- Building master with gcc 4.7 fails due to 'is_trivially_destructible' in SC_PlugIn.hpp
  [#1820](https://github.com/supercollider/supercollider/issues/1820) by [danstowell](https://github.com/danstowell)

##  env: Qt IDE

- IDE: Server status bar should send properly formatted /status message (not 'status')
  [#2450](https://github.com/supercollider/supercollider/pull/2450) by [jamshark70](https://github.com/jamshark70)
- Fix Document path sync problems
  [#2222](https://github.com/supercollider/supercollider/pull/2222) by [jamshark70](https://github.com/jamshark70)
- fix #1985
  [#2102](https://github.com/supercollider/supercollider/pull/2102) by [miguel-negrao](https://github.com/miguel-negrao)
- scide: update document path also if nil
  [#2098](https://github.com/supercollider/supercollider/pull/2098) by [telephon](https://github.com/telephon)
- sc-ide: fix behaviour of right context button when out of tab
  [#2085](https://github.com/supercollider/supercollider/pull/2085) by [llloret](https://github.com/llloret)
- sc-ide: middle mouse button closes tab
  [#2083](https://github.com/supercollider/supercollider/pull/2083) by [llloret](https://github.com/llloret)
- ide: menu option "Show Quarks"
  [#1867](https://github.com/supercollider/supercollider/pull/1867) by [miguel-negrao](https://github.com/miguel-negrao)

##  env: scel

- bug with scide_scel
  [#2036](https://github.com/supercollider/supercollider/pull/2036) by [simdax](https://github.com/simdax)

##  env: scvim

- Replace built-in scvim with submodule scvim
  [#1991](https://github.com/supercollider/supercollider/pull/1991) by [danstowell](https://github.com/danstowell)
- Makes Vim support more reliable by sending larger buffers
  [#1930](https://github.com/supercollider/supercollider/pull/1930) by [mzyzik](https://github.com/mzyzik)
- scvim as submodules repo
  [#1921](https://github.com/supercollider/supercollider/issues/1921) by [blacksound](https://github.com/blacksound)

##  os: Linux

- Do not allocate all channels reported by Pa_GetDeviceInfo / use memcpy instead of for-loop
  [#1943](https://github.com/supercollider/supercollider/pull/1943) by [hzulla](https://github.com/hzulla)
- Fixes/alsa midi fixes
  [#1760](https://github.com/supercollider/supercollider/pull/1760) by [timblechmann](https://github.com/timblechmann)
- HID final cleanup, and LID adaption to use similar API
  [#1573](https://github.com/supercollider/supercollider/pull/1573) by [sensestage](https://github.com/sensestage)

##  os: Windows

- Fix problem with boost interprocess module on Win
  [#2457](https://github.com/supercollider/supercollider/pull/2457) by [llloret](https://github.com/llloret)
- Update Windows Readme
  [#2419](https://github.com/supercollider/supercollider/pull/2419) by [bagong](https://github.com/bagong)
- Exception in World_New: boost::interprocess::intermodule_singleton initialization failed
  [#2409](https://github.com/supercollider/supercollider/issues/2409) by [brachna](https://github.com/brachna)
- Fix QLocalSocket problem under Windows
  [#2197](https://github.com/supercollider/supercollider/pull/2197) by [llloret](https://github.com/llloret)
- Windows exit nicely master
  [#2107](https://github.com/supercollider/supercollider/pull/2107) by [llloret](https://github.com/llloret)
- Make MIDI work on Windows (PR for master)
  [#2106](https://github.com/supercollider/supercollider/pull/2106) by [llloret](https://github.com/llloret)
- nsis windows for master branch
  [#2103](https://github.com/supercollider/supercollider/pull/2103) by [llloret](https://github.com/llloret)
- Make Vista the minimum required Windows version
  [#2017](https://github.com/supercollider/supercollider/pull/2017) by [llloret](https://github.com/llloret)
- Make the required version Windows Vista
  [#2016](https://github.com/supercollider/supercollider/pull/2016) by [llloret](https://github.com/llloret)
- sclang: Fix to get Object: render to work on Windows
  [#1899](https://github.com/supercollider/supercollider/pull/1899) by [antonhornquist](https://github.com/antonhornquist)
- Windows: sclang crashes on executing menu-item "Quit interpreter"/freezes on evaluating 0.exit
  [#1578](https://github.com/supercollider/supercollider/issues/1578) by [bagong](https://github.com/bagong)
- Server not shut down on IDE-close
  [#1449](https://github.com/supercollider/supercollider/issues/1449) by [bagong](https://github.com/bagong)
- MIDI sysex is not implemented for Windows (SC_PortMIDI.cpp)
  [#1200](https://github.com/supercollider/supercollider/issues/1200) by [sensestage](https://github.com/sensestage)

##  qt5

- WIP: ide/qtcollider: prototype port to qwebengine
  [#1936](https://github.com/supercollider/supercollider/pull/1936) by [timblechmann](https://github.com/timblechmann)

##  quarks

- quarks: sort list by name
  [#2214](https://github.com/supercollider/supercollider/pull/2214) by [gusano](https://github.com/gusano)
- quarks: throw an error when updating without name
  [#2183](https://github.com/supercollider/supercollider/pull/2183) by [gusano](https://github.com/gusano)

##  architecture: arm

- Provide compiler flags for armv6l and armv7l and add a few hints for building on RPi and headless
  [#2065](https://github.com/supercollider/supercollider/pull/2065) by [bagong](https://github.com/bagong)

##  bug

- ServerStatus failOSCFunc shouldn't set clientID
  [#2328](https://github.com/supercollider/supercollider/issues/2328) by [crucialfelix](https://github.com/crucialfelix)
- class library: server notify dependants correctly
  [#2093](https://github.com/supercollider/supercollider/pull/2093) by [telephon](https://github.com/telephon)
- Topic fix server notify
  [#2066](https://github.com/supercollider/supercollider/pull/2066) by [telephon](https://github.com/telephon)
- OSC/Trigger functionality broken in master
  [#2058](https://github.com/supercollider/supercollider/issues/2058) by [miczac](https://github.com/miczac)
- Problem with matchLangIP primitive: boost seems to give wrong hostname
  [#1950](https://github.com/supercollider/supercollider/issues/1950) by [muellmusik](https://github.com/muellmusik)
- crash on exit, 3.7b
  [#1422](https://github.com/supercollider/supercollider/issues/1422) by [chriskiefer](https://github.com/chriskiefer)
- DemandEnvGen overshooting at high curve
  [#1355](https://github.com/supercollider/supercollider/issues/1355) by [eleses](https://github.com/eleses)
- scsynth OSC packet size in NRT mode
  [#61](https://github.com/supercollider/supercollider/issues/61) by [jleben](https://github.com/jleben)

##  enhancement

- Novacollider/alignment cleanups
  [#1906](https://github.com/supercollider/supercollider/pull/1906) by [timblechmann](https://github.com/timblechmann)
- ide: introspection - use qt's concurrency functionality
  [#1905](https://github.com/supercollider/supercollider/pull/1905) by [timblechmann](https://github.com/timblechmann)
- Novacollider/dll
  [#1904](https://github.com/supercollider/supercollider/pull/1904) by [timblechmann](https://github.com/timblechmann)
- Tab does not work in IDE (on OSX)
  [#1453](https://github.com/supercollider/supercollider/issues/1453) by [thormagnusson](https://github.com/thormagnusson)
- Use reader/writer thread for all disk IO (DiskIn / DiskOut ugens, others if applicable)
  [#1381](https://github.com/supercollider/supercollider/issues/1381) by [scztt](https://github.com/scztt)

##  Miscellaneous

- Add missing proxy init 3.8
  [#2407](https://github.com/supercollider/supercollider/pull/2407) by [telephon](https://github.com/telephon)
- Merge 3.7.2 to master
  [#2177](https://github.com/supercollider/supercollider/pull/2177) by [crucialfelix](https://github.com/crucialfelix)
- Update CombC.schelp
  [#2157](https://github.com/supercollider/supercollider/pull/2157) by [tiagmoraismorgado](https://github.com/tiagmoraismorgado)
- Array2D.schelp: Fix put example
  [#2154](https://github.com/supercollider/supercollider/pull/2154) by [kisielk](https://github.com/kisielk)
- jitlib: better warnings when Server is not available
  [#2119](https://github.com/supercollider/supercollider/pull/2119) by [telephon](https://github.com/telephon)
- Topic/boost 1.61
  [#2086](https://github.com/supercollider/supercollider/pull/2086) by [timblechmann](https://github.com/timblechmann)
- lang: correctly join resync thread
  [#2079](https://github.com/supercollider/supercollider/pull/2079) by [timblechmann](https://github.com/timblechmann)
- Helpfile-fixing branch, final merge
  [#2076](https://github.com/supercollider/supercollider/pull/2076) by [LFSaw](https://github.com/LFSaw)
- ide: mark scide as able to handle multiple files
  [#2062](https://github.com/supercollider/supercollider/pull/2062) by [fsateler](https://github.com/fsateler)
- removed method::preferencesAction
  [#2046](https://github.com/supercollider/supercollider/pull/2046) by [tapage](https://github.com/tapage)
- fixed naming startframe & aSoundFile
  [#2045](https://github.com/supercollider/supercollider/pull/2045) by [tapage](https://github.com/tapage)
- prReadDirectoryFile private, added done as arg
  [#2044](https://github.com/supercollider/supercollider/pull/2044) by [tapage](https://github.com/tapage)
- added Pmul arguments name & pattern
  [#2043](https://github.com/supercollider/supercollider/pull/2043) by [tapage](https://github.com/tapage)
- LFSaw.schelp: Note and example for special initial-phase behaviour
  [#2041](https://github.com/supercollider/supercollider/pull/2041) by [miczac](https://github.com/miczac)
- Emacs and extBuffer.sc
  [#2035](https://github.com/supercollider/supercollider/issues/2035) by [simdax](https://github.com/simdax)
- Help: Removed double PlayBuf in doneAction help file
  [#2026](https://github.com/supercollider/supercollider/pull/2026) by [cappelnord](https://github.com/cappelnord)
- Classlib: Add NodeProxy:trace
  [#2020](https://github.com/supercollider/supercollider/pull/2020) by [jamshark70](https://github.com/jamshark70)
- PartConv not working (duplicate impulses)
  [#2014](https://github.com/supercollider/supercollider/issues/2014) by [sonoro1234](https://github.com/sonoro1234)
- Server crash when calling play on a not-yet ready Buffer
  [#2005](https://github.com/supercollider/supercollider/issues/2005) by [patrickdupuis](https://github.com/patrickdupuis)
- Make sure NodeProxy generates unique name.
  [#1994](https://github.com/supercollider/supercollider/pull/1994) by [blacksound](https://github.com/blacksound)
- class library: node proxy, improve documentation
  [#1986](https://github.com/supercollider/supercollider/pull/1986) by [telephon](https://github.com/telephon)
- Osc.sc: fix audio rate TChoose
  [#1979](https://github.com/supercollider/supercollider/pull/1979) by [miczac](https://github.com/miczac)
- Osc.sc: fix audio rate TChoose
  [#1974](https://github.com/supercollider/supercollider/pull/1974) by [miczac](https://github.com/miczac)
- Readd wrongly removed part of system-boost fix
  [#1970](https://github.com/supercollider/supercollider/pull/1970) by [bagong](https://github.com/bagong)
- Merge 3.7 into master
  [#1969](https://github.com/supercollider/supercollider/pull/1969) by [bagong](https://github.com/bagong)
- Add CommonTests and CommonTestsGUI to travis
  [#1967](https://github.com/supercollider/supercollider/pull/1967) by [scztt](https://github.com/scztt)
- Fix lines and functions with mixed tabs and spaces
  [#1963](https://github.com/supercollider/supercollider/pull/1963) by [vivid-synth](https://github.com/vivid-synth)
- Remove unused variable
  [#1960](https://github.com/supercollider/supercollider/pull/1960) by [patrickdupuis](https://github.com/patrickdupuis)
- error message: wrong path in unsaved file
  [#1953](https://github.com/supercollider/supercollider/issues/1953) by [telephon](https://github.com/telephon)
- class library: don't declare variables in an if statement, please.
  [#1925](https://github.com/supercollider/supercollider/pull/1925) by [telephon](https://github.com/telephon)
- Add custom.css to help files that don't include it
  [#1920](https://github.com/supercollider/supercollider/pull/1920) by [snappizz](https://github.com/snappizz)
- Document SynthDef.writeOnce as a legacy method
  [#1918](https://github.com/supercollider/supercollider/pull/1918) by [snappizz](https://github.com/snappizz)
- Move internal css to scdoc.css
  [#1917](https://github.com/supercollider/supercollider/pull/1917) by [rygen](https://github.com/rygen)
- Fix Scope Window Error
  [#1915](https://github.com/supercollider/supercollider/pull/1915) by [patrickdupuis](https://github.com/patrickdupuis)
- Quarks: fix save method
  [#1897](https://github.com/supercollider/supercollider/pull/1897) by [jpburstrom](https://github.com/jpburstrom)
- ide: Add standalone option in settings.
  [#1863](https://github.com/supercollider/supercollider/pull/1863) by [miguel-negrao](https://github.com/miguel-negrao)
- sclang: introduce unixCmd for array of arguments
  [#1856](https://github.com/supercollider/supercollider/pull/1856) by [miguel-negrao](https://github.com/miguel-negrao)
- uiugens: correctly terminate input threads
  [#1855](https://github.com/supercollider/supercollider/pull/1855) by [timblechmann](https://github.com/timblechmann)
- supernova: don't delete shared memory data
  [#1854](https://github.com/supercollider/supercollider/pull/1854) by [timblechmann](https://github.com/timblechmann)
- Novacollider/mac
  [#1853](https://github.com/supercollider/supercollider/pull/1853) by [timblechmann](https://github.com/timblechmann)
- class library: HIDMatchers: Add missing if statement
  [#1851](https://github.com/supercollider/supercollider/pull/1851) by [davidgranstrom](https://github.com/davidgranstrom)
- class library: protect MultiOutUGen from void numChannels
  [#1847](https://github.com/supercollider/supercollider/pull/1847) by [telephon](https://github.com/telephon)
- Fixes/for master
  [#1844](https://github.com/supercollider/supercollider/pull/1844) by [timblechmann](https://github.com/timblechmann)
- Topic/rate fallthrough
  [#1835](https://github.com/supercollider/supercollider/pull/1835) by [telephon](https://github.com/telephon)
- Novacollider/cmake modernisation
  [#1822](https://github.com/supercollider/supercollider/pull/1822) by [timblechmann](https://github.com/timblechmann)
- LinXFade2 - fix pos slope
  [#1798](https://github.com/supercollider/supercollider/pull/1798) by [timblechmann](https://github.com/timblechmann)
- ide cleanup
  [#1715](https://github.com/supercollider/supercollider/pull/1715) by [timblechmann](https://github.com/timblechmann)
- MultiOutUGen with numchannels less than 1 return an empty array
  [#1686](https://github.com/supercollider/supercollider/issues/1686) by [telephon](https://github.com/telephon)
- use c++17-style executors to compile class library
  [#1677](https://github.com/supercollider/supercollider/pull/1677) by [timblechmann](https://github.com/timblechmann)
- scide: improve dark color scheme
  [#1609](https://github.com/supercollider/supercollider/pull/1609) by [timblechmann](https://github.com/timblechmann)
- scide: add qt-creator style shortcut sequence to visualise whitespaces
  [#1607](https://github.com/supercollider/supercollider/pull/1607) by [timblechmann](https://github.com/timblechmann)
- Topic/refactor server status
  [#1547](https://github.com/supercollider/supercollider/pull/1547) by [telephon](https://github.com/telephon)
- Enabling multi-touch on Qt widgets (for multi-touch screens)
  [#1533](https://github.com/supercollider/supercollider/pull/1533) by [scazan](https://github.com/scazan)
- import boost-1.58
  [#1528](https://github.com/supercollider/supercollider/pull/1528) by [timblechmann](https://github.com/timblechmann)
- Scide/line number fix
  [#1330](https://github.com/supercollider/supercollider/pull/1330) by [vdonnefort](https://github.com/vdonnefort)
- Scide/theme mgmt fix
  [#1297](https://github.com/supercollider/supercollider/pull/1297) by [vdonnefort](https://github.com/vdonnefort)
- Made performKeyToDegree much closer to inverse of performDegreeToKey
  [#1164](https://github.com/supercollider/supercollider/pull/1164) by [triss](https://github.com/triss)


## [3.7.2](https://github.com/supercollider/supercollider/tree/3.7.2) (2016-06-03)
[Full Changelog](https://github.com/supercollider/supercollider/compare/Version-3.7.1...Version-3.7.2)

This patch release fixes the Windows including MIDI. HID is still not quite working on Windows. Many thanks to: @bagong and @llloret

SC VIM is now a git submodule. This affects mainly developers. VIM support can be installed as per the documentation - nothing has changed in how you use it.  We changed this in 3.7.2 as well as on master (3.8 development) so that switching back and forth between branches wouldn't be super annoying.

**Fixes:**

- Midi not working on Windows [\#1922](https://github.com/supercollider/supercollider/issues/1922)
- Windows: opening SC via system registered document types faulty [\#2022](https://github.com/supercollider/supercollider/issues/2022)
- HIDdef.element forwards arguments incorrectly [\#2090](https://github.com/supercollider/supercollider/issues/2090)

**Closed Pull Requests**

- midi: make midi work in Windows [\#2009](https://github.com/supercollider/supercollider/pull/2009) ([llloret](https://github.com/llloret))
- Classlib: Fix HIDdef.element arg list passed to super.element [\#2105](https://github.com/supercollider/supercollider/pull/2105) ([jamshark70](https://github.com/jamshark70))
- Fix build on debian, add -fPIC to TLSF target [\#2031](https://github.com/supercollider/supercollider/pull/2031) ([danstowell](https://github.com/danstowell))
- Update linux readme, add missing dependency [\#2030](https://github.com/supercollider/supercollider/pull/2030) ([danstowell](https://github.com/danstowell))
- editor: windows: fix double click when app already open [\#2029](https://github.com/supercollider/supercollider/pull/2029) ([llloret](https://github.com/llloret))
- nsis: windows: add path information [\#2028](https://github.com/supercollider/supercollider/pull/2028) ([llloret](https://github.com/llloret))
- Convert scvim to submodule, on 3.7 branch [\#2025](https://github.com/supercollider/supercollider/pull/2025) ([danstowell](https://github.com/danstowell))
- lang: Not wait for keystroke when exiting [\#2012](https://github.com/supercollider/supercollider/pull/2012) ([llloret](https://github.com/llloret))
- Windows Readme: tiny enhancements [\#2006](https://github.com/supercollider/supercollider/pull/2006) ([bagong](https://github.com/bagong))
- Make sure NodeProxy generates unique name. [\#1998](https://github.com/supercollider/supercollider/pull/1998) ([blacksound](https://github.com/blacksound))
- Cherry pick telefon's nodeproxy documentation enhancements [\#1996](https://github.com/supercollider/supercollider/pull/1996) ([bagong](https://github.com/bagong))



## [3.7.1](https://github.com/supercollider/supercollider/tree/3.7.1) (2016-04-10)
[Full Changelog](https://github.com/supercollider/supercollider/compare/Version-3.7.0...Version-3.7.1)

**Enhancements**

- Native FLAC support for scsynth on OS X [\#1783](https://github.com/supercollider/supercollider/issues/1783)
- Libsndfile: have cmake prefer homebrew install over bundled version [\#1870](https://github.com/supercollider/supercollider/pull/1870) ([bagong](https://github.com/bagong))
- OS X Readme: note that Qt 5.5 is required, not Qt 5.6 [\#1931](https://github.com/supercollider/supercollider/issues/1931)
- class library: node proxy: improve shape error post [\#1889](https://github.com/supercollider/supercollider/pull/1889) ([telephon](https://github.com/telephon))
- Server.schelp: fixed description of scsynth method, changed wording, … [\#1894](https://github.com/supercollider/supercollider/pull/1894) ([miczac](https://github.com/miczac))

**Fixes:**

- Windows build system [\#1900](https://github.com/supercollider/supercollider/pull/1900) ([bagong](https://github.com/bagong))
- cmake: fix build when using system boost [\#1896](https://github.com/supercollider/supercollider/pull/1896) ([danstowell](https://github.com/danstowell))
- Correct accidental msys leftovers in findPortaudio [\#1941](https://github.com/supercollider/supercollider/pull/1941) ([bagong](https://github.com/bagong))
- Remove -fstrict-aliasing from the MinGW build to allow using MinGW 4.9.2 [\#1923](https://github.com/supercollider/supercollider/pull/1923) ([bagong](https://github.com/bagong))

- Quarks.update\("quarkname"\) does not always update correctly [\#1895](https://github.com/supercollider/supercollider/issues/1895)
- fix \#1895 : update Quark by `git pull` and `git checkout master` [\#1954](https://github.com/supercollider/supercollider/pull/1954) ([crucialfelix](https://github.com/crucialfelix))
- Quarks Windows fixes [\#1956](https://github.com/supercollider/supercollider/pull/1956) ([bagong](https://github.com/bagong))

- Errors when closing scope window [\#1878](https://github.com/supercollider/supercollider/issues/1878)


## [3.7.0](https://github.com/supercollider/supercollider/tree/3.7.0) (2016-03-13)
[Full Changelog](https://github.com/supercollider/supercollider/compare/Version-3.6.6...Version-3.7.0)

**Enhancements**

- New Quarks system using Git [\#1800](https://github.com/supercollider/supercollider/issues/1800)
- Qt GUI: TextView's enterInterpretsSelection treats line wrapping like a hard line break [\#1637](https://github.com/supercollider/supercollider/issues/1637)
- UnitTest missing documentation [\#1610](https://github.com/supercollider/supercollider/issues/1610)
- LinLog function? [\#1555](https://github.com/supercollider/supercollider/issues/1555)
- sclang and scserver use the same icon [\#1548](https://github.com/supercollider/supercollider/issues/1548)
- help: recent changes for 3.7 documentation [\#1516](https://github.com/supercollider/supercollider/issues/1516)
- MIDIFunc does not deal with allNotesOff message [\#1485](https://github.com/supercollider/supercollider/issues/1485)
- Tab does not work in IDE \(on OSX\) [\#1453](https://github.com/supercollider/supercollider/issues/1453)
- Feature: IDE preference to disable displaying help pages in the autocomplete popup [\#1435](https://github.com/supercollider/supercollider/issues/1435)
- quarks \(new\) - suggestion - directory.txt use https instead of git protocol. [\#1397](https://github.com/supercollider/supercollider/issues/1397)
- quarks \(new\) - Add a way to update quarks \(git pull or delete folder and clone again\) [\#1386](https://github.com/supercollider/supercollider/issues/1386)
- Use reader/writer thread for all disk IO \(DiskIn / DiskOut ugens, others if applicable\) [\#1381](https://github.com/supercollider/supercollider/issues/1381)
- quarks \(new\) - canceling git checkout [\#1376](https://github.com/supercollider/supercollider/issues/1376)
- help: News in 3.7 [\#1347](https://github.com/supercollider/supercollider/issues/1347)
- class library: addition of SimpleController::removeAt [\#1328](https://github.com/supercollider/supercollider/issues/1328)
- scsynth ought to print the version number when passed the --version flag [\#1310](https://github.com/supercollider/supercollider/issues/1310)
- UGen to report node ID [\#1212](https://github.com/supercollider/supercollider/issues/1212)
- incomprehenisble error message with Function-play and empty array [\#1128](https://github.com/supercollider/supercollider/issues/1128)
- pattern cleanup shouldn't be passed around [\#1049](https://github.com/supercollider/supercollider/issues/1049)
- README file and windows build instructions [\#1034](https://github.com/supercollider/supercollider/issues/1034)
- Enhance OSX readme for 3.6.6 [\#1001](https://github.com/supercollider/supercollider/issues/1001)
- README\_OS\_X.txt is overly complex [\#960](https://github.com/supercollider/supercollider/issues/960)
- "Exception when parsing synthdef: corrupted synthdef" - report which synthdef caused the error. [\#904](https://github.com/supercollider/supercollider/issues/904)
- "start coding" window title [\#898](https://github.com/supercollider/supercollider/issues/898)
- Windows: No 64-bit version [\#853](https://github.com/supercollider/supercollider/issues/853)
- toggle dumpOSC via menu entry [\#848](https://github.com/supercollider/supercollider/issues/848)
- default file extension missing [\#838](https://github.com/supercollider/supercollider/issues/838)
- open file dialog: start in last used directory across sessions [\#805](https://github.com/supercollider/supercollider/issues/805)
- recent files: hide no-longer existent files [\#804](https://github.com/supercollider/supercollider/issues/804)
- Faint highlighting on the cursor's line [\#793](https://github.com/supercollider/supercollider/issues/793)
- display volume in server status bar [\#762](https://github.com/supercollider/supercollider/issues/762)
- mouse wheel volume control \(server status bar\) [\#760](https://github.com/supercollider/supercollider/issues/760)
- post message in post window when sclang quits/crashes [\#743](https://github.com/supercollider/supercollider/issues/743)
- I would like to ask for a ListView.selection\_ method.  [\#741](https://github.com/supercollider/supercollider/issues/741)
- Autosave [\#725](https://github.com/supercollider/supercollider/issues/725)
- NRT message length limit [\#714](https://github.com/supercollider/supercollider/issues/714)
- problems with  PandaboardES and SC [\#689](https://github.com/supercollider/supercollider/issues/689)
- More obvious access to SCIDE help file [\#682](https://github.com/supercollider/supercollider/issues/682)
- get rid of jitter for non time stamped bundles [\#648](https://github.com/supercollider/supercollider/issues/648)
- Popup Index for class file tabs [\#629](https://github.com/supercollider/supercollider/issues/629)
- semicolon ending block evaluation [\#627](https://github.com/supercollider/supercollider/issues/627)
- Missing functionality in Qt: QuartzComposer [\#624](https://github.com/supercollider/supercollider/issues/624)
- Missing functionality in Qt: Checking whether a window is closed [\#622](https://github.com/supercollider/supercollider/issues/622)
- Missing functionality in Qt: SCMenuItem [\#617](https://github.com/supercollider/supercollider/issues/617)
- Missing functionality in Qt: SCImage [\#616](https://github.com/supercollider/supercollider/issues/616)
- Please make ctrl-return behavior configurable WRT execute region [\#603](https://github.com/supercollider/supercollider/issues/603)
- Command like "Balance Parens" from sc.app [\#602](https://github.com/supercollider/supercollider/issues/602)
- Soften, or allow customizing, the bracket-mismatch color [\#599](https://github.com/supercollider/supercollider/issues/599)
- Tab behaviour [\#593](https://github.com/supercollider/supercollider/issues/593)
- scide - help browser window font should be adjustable [\#587](https://github.com/supercollider/supercollider/issues/587)
- hide line numbers [\#585](https://github.com/supercollider/supercollider/issues/585)
- "Start sclang" should be greyed out when language is running [\#584](https://github.com/supercollider/supercollider/issues/584)
- Double-click on a float doesn't select all of it [\#581](https://github.com/supercollider/supercollider/issues/581)
- Hitting Cmd/Ctrl while inertial scrolling causes rapid zoom in / out [\#568](https://github.com/supercollider/supercollider/issues/568)
- find next / previous key binding [\#555](https://github.com/supercollider/supercollider/issues/555)
- If untitled doc from start has not been used close it when first file is opened [\#553](https://github.com/supercollider/supercollider/issues/553)
- Undocking widgets and tabs as proper windows [\#550](https://github.com/supercollider/supercollider/issues/550)
- Move cursor to end of document on down arrow key in last line. [\#543](https://github.com/supercollider/supercollider/issues/543)
- New method to get complete \(multiple\) item selection from QListView and QTreeView  [\#535](https://github.com/supercollider/supercollider/issues/535)
- Several items in the preferences can be set to the same shortcut [\#516](https://github.com/supercollider/supercollider/issues/516)
- language config dialog [\#515](https://github.com/supercollider/supercollider/issues/515)
- bracket auto-paring [\#513](https://github.com/supercollider/supercollider/issues/513)
- A few basic issues concerning the replacement of SCApp by SCIDE [\#511](https://github.com/supercollider/supercollider/issues/511)
- ctrl-click on document icon to open file path [\#510](https://github.com/supercollider/supercollider/issues/510)
- support for editing plain-text files [\#495](https://github.com/supercollider/supercollider/issues/495)
- Method Call Assist: Handy way to insert an argument name for keyword addressing [\#492](https://github.com/supercollider/supercollider/issues/492)
- integrate scdoc into ide [\#471](https://github.com/supercollider/supercollider/issues/471)
- Class method autocompletion and method signatures of view redirect classes [\#469](https://github.com/supercollider/supercollider/issues/469)
- \(Future\) More sophisticated regexp find/replace [\#464](https://github.com/supercollider/supercollider/issues/464)
- Looking up definition from post window [\#461](https://github.com/supercollider/supercollider/issues/461)
- Request keyboard shortcut to navigate up/down to the next empty line [\#453](https://github.com/supercollider/supercollider/issues/453)
- rtf import of \(erroneous\) weblinks [\#448](https://github.com/supercollider/supercollider/issues/448)
- Document open/save dialogs: Keyboard shortcut to navigate to the parent folder [\#445](https://github.com/supercollider/supercollider/issues/445)
- Keyboard shortcut to select a code block enclosed in \(\) [\#444](https://github.com/supercollider/supercollider/issues/444)
- Dark-on-light color scheme: orange for ~envVars is too bright [\#442](https://github.com/supercollider/supercollider/issues/442)
- Shortcuts to add and remove /\* \*/ pairs [\#441](https://github.com/supercollider/supercollider/issues/441)
- Add menu command to save all dirty files [\#439](https://github.com/supercollider/supercollider/issues/439)
- make post window scrollback configurable [\#430](https://github.com/supercollider/supercollider/issues/430)
- switch session dialog [\#429](https://github.com/supercollider/supercollider/issues/429)
- case-insensitive search in `open definition' dialog [\#428](https://github.com/supercollider/supercollider/issues/428)
- Documentation dock widget [\#420](https://github.com/supercollider/supercollider/issues/420)
- SC-IDE: Show full path somewhere [\#413](https://github.com/supercollider/supercollider/issues/413)
- autocompletion for literals and brackets [\#410](https://github.com/supercollider/supercollider/issues/410)
- insert key should toggle overwrite mode [\#407](https://github.com/supercollider/supercollider/issues/407)
- Save dialog should provide options for scd and sc [\#405](https://github.com/supercollider/supercollider/issues/405)
- keyboard shortcut for "clear post window" [\#404](https://github.com/supercollider/supercollider/issues/404)
- .sc and .scd links in Help should open in scide [\#402](https://github.com/supercollider/supercollider/issues/402)
- Quick toggle of spaces/tab indentation [\#400](https://github.com/supercollider/supercollider/issues/400)
- Word wrap [\#399](https://github.com/supercollider/supercollider/issues/399)
- Show help and definition don't work on post window [\#398](https://github.com/supercollider/supercollider/issues/398)
- F1 is an awkward keyboard shortcut for help on Macs [\#397](https://github.com/supercollider/supercollider/issues/397)
- Split pane tabs [\#396](https://github.com/supercollider/supercollider/issues/396)
- Provide built-in color schemes [\#387](https://github.com/supercollider/supercollider/issues/387)
- Preview of color settings [\#386](https://github.com/supercollider/supercollider/issues/386)
- show references to symbol [\#384](https://github.com/supercollider/supercollider/issues/384)
- drag/drop support for scide [\#383](https://github.com/supercollider/supercollider/issues/383)
- popup list view to cycle open documents [\#382](https://github.com/supercollider/supercollider/issues/382)
- scide desktop integration [\#381](https://github.com/supercollider/supercollider/issues/381)
- key combinations for commenting/uncommenting [\#380](https://github.com/supercollider/supercollider/issues/380)
- programmatically resize or set a preferences for IDE window placement [\#376](https://github.com/supercollider/supercollider/issues/376)
- server control widget [\#372](https://github.com/supercollider/supercollider/issues/372)
- Instance method auto-completion: group offered methods by name [\#371](https://github.com/supercollider/supercollider/issues/371)
- Method call aid: highlight proper argument when entered "by name" [\#370](https://github.com/supercollider/supercollider/issues/370)
- Integrate help with auto-completion [\#368](https://github.com/supercollider/supercollider/issues/368)
- Do not require a keyword to be selected to lookup implementations or help [\#364](https://github.com/supercollider/supercollider/issues/364)
- Related to 312 and 313: Completion of instance methods by user selection of class [\#362](https://github.com/supercollider/supercollider/issues/362)
- Option to preserve Poll output w/ lower verbosity [\#350](https://github.com/supercollider/supercollider/issues/350)
- Support NRT with no output file, for analysis [\#349](https://github.com/supercollider/supercollider/issues/349)
- \[SC-IDE\] Document implementation [\#333](https://github.com/supercollider/supercollider/issues/333)
- \[SC-IDE\] Make OSX Bundle [\#331](https://github.com/supercollider/supercollider/issues/331)
- \[SC-IDE\] Syntax colorize schelp files [\#330](https://github.com/supercollider/supercollider/issues/330)
- \[SC-IDE\] Open file by searching for Class or method in a pop up window. [\#329](https://github.com/supercollider/supercollider/issues/329)
- \[SC-IDE\] save or discard changes on quit per unsaved document [\#327](https://github.com/supercollider/supercollider/issues/327)
- \[SC-IDE\] Update file view if it is updated by another program [\#326](https://github.com/supercollider/supercollider/issues/326)
- \[SC-IDE\] Open recent file menu entry. [\#321](https://github.com/supercollider/supercollider/issues/321)
- \[SC-IDE\] Open multiple files at the same time [\#318](https://github.com/supercollider/supercollider/issues/318)
- \[SC-IDE\] Visual indicator if file is edited or not  [\#317](https://github.com/supercollider/supercollider/issues/317)
- show search-replace results in realtime [\#316](https://github.com/supercollider/supercollider/issues/316)
- Code completion for Class names with hovering dropdown menu. [\#314](https://github.com/supercollider/supercollider/issues/314)
- \[SC-IDE\] Code completion for Class methods with hovering drop-down menu. [\#313](https://github.com/supercollider/supercollider/issues/313)
- Hovering popup with argument names for class methods [\#312](https://github.com/supercollider/supercollider/issues/312)
- \[SC-IDE\] Open definition of method [\#309](https://github.com/supercollider/supercollider/issues/309)
- postln with prefix and suffix [\#306](https://github.com/supercollider/supercollider/issues/306)
- .data\_ method for QSoundFileView implementation [\#304](https://github.com/supercollider/supercollider/issues/304)
- Proposal for Helpbrowser layout [\#300](https://github.com/supercollider/supercollider/issues/300)
- \[Cocoa GUI\] allowsReselection flag on SCPopUpMenu [\#299](https://github.com/supercollider/supercollider/issues/299)
- allowsReselection for SC- and Q- listviews and popup menus [\#298](https://github.com/supercollider/supercollider/issues/298)
- Implement focusGainedAction/focusLostAction for QtGUI [\#295](https://github.com/supercollider/supercollider/issues/295)
- HTML doc and web standards [\#294](https://github.com/supercollider/supercollider/issues/294)
- Control over the numeric x-labels in plot2 [\#293](https://github.com/supercollider/supercollider/issues/293)
- Implement focusGainedAction/focusLostAction for CocoaGUI [\#291](https://github.com/supercollider/supercollider/issues/291)
- Dwrand doesn't exist [\#287](https://github.com/supercollider/supercollider/issues/287)
- Send to bus without creating new synths [\#286](https://github.com/supercollider/supercollider/issues/286)
- BinaryOpUGen optimization needs review [\#284](https://github.com/supercollider/supercollider/issues/284)
- Convolution UGens should all use SC\_fftlib [\#276](https://github.com/supercollider/supercollider/issues/276)
- Buffer:normalize [\#275](https://github.com/supercollider/supercollider/issues/275)
- test 2 [\#274](https://github.com/supercollider/supercollider/issues/274)
- test [\#273](https://github.com/supercollider/supercollider/issues/273)
- mapping audio buses as synth controls [\#272](https://github.com/supercollider/supercollider/issues/272)
- Getting sound card list with \# of ins/outs from scsynth [\#271](https://github.com/supercollider/supercollider/issues/271)
- source code [\#270](https://github.com/supercollider/supercollider/issues/270)
- Post window should have correct window title [\#269](https://github.com/supercollider/supercollider/issues/269)
- Menu Icon request [\#268](https://github.com/supercollider/supercollider/issues/268)
- sources tarball  [\#267](https://github.com/supercollider/supercollider/issues/267)
- Windows Port [\#266](https://github.com/supercollider/supercollider/issues/266)
- Topic/release notes 3 7 0 [\#1791](https://github.com/supercollider/supercollider/pull/1791) ([crucialfelix](https://github.com/crucialfelix))
- Topic/select reject indices [\#1591](https://github.com/supercollider/supercollider/pull/1591) ([LFSaw](https://github.com/LFSaw))
- class library: fuzzy equal with relative precision [\#1587](https://github.com/supercollider/supercollider/pull/1587) ([telephon](https://github.com/telephon))
- Fix range in Function:plot [\#1454](https://github.com/supercollider/supercollider/pull/1454) ([thormagnusson](https://github.com/thormagnusson))

**Fixed bugs:**

- scel: Directory change of sclang/scsynth in 3.7-beta1 causing error [\#1860](https://github.com/supercollider/supercollider/issues/1860)
- interpreter crashes after server boot when very large scd file is currently open [\#1823](https://github.com/supercollider/supercollider/issues/1823)
- SC\_SndBuf.h \(in master\) can't compile on ARM because uses \_mm\_pause  [\#1819](https://github.com/supercollider/supercollider/issues/1819)
- OS X: scsynth icon bounces forever in the dock [\#1804](https://github.com/supercollider/supercollider/issues/1804)
- supernova compilation error [\#1794](https://github.com/supercollider/supercollider/issues/1794)
- MulAddUGens broken [\#1793](https://github.com/supercollider/supercollider/issues/1793)
- Evaluate commands \(Shift-Enter/Return, Ctrl-Enter/Cmd-Return, menus\) work not reliably [\#1786](https://github.com/supercollider/supercollider/issues/1786)
- Bad values \(nan\) from underflow with pow\(\) on Server [\#1766](https://github.com/supercollider/supercollider/issues/1766)
- setProperties fails for plots of signals [\#1762](https://github.com/supercollider/supercollider/issues/1762)
- Quarks: update fail when missing tag or no new version number [\#1735](https://github.com/supercollider/supercollider/issues/1735)
- Quarks.update calls wrong git method [\#1734](https://github.com/supercollider/supercollider/issues/1734)
- Typo / wrong cmake function name in CMakeLists.txt [\#1705](https://github.com/supercollider/supercollider/issues/1705)
- TwoWayIdentityDictionary.asCompileString misses to include key  [\#1699](https://github.com/supercollider/supercollider/issues/1699)
- error with soundfileview.schelp [\#1682](https://github.com/supercollider/supercollider/issues/1682)
- Quarks on Windows: isPath regex is wrong [\#1670](https://github.com/supercollider/supercollider/issues/1670)
- Windows File.existsCaseSensitive does not work for directories [\#1668](https://github.com/supercollider/supercollider/issues/1668)
- calling ScIDE prSend from a routine makes the ide unreachable [\#1657](https://github.com/supercollider/supercollider/issues/1657)
- EnvGen \cub produces NaNs [\#1656](https://github.com/supercollider/supercollider/issues/1656)
- Scale.choose throws error [\#1653](https://github.com/supercollider/supercollider/issues/1653)
- d\_removed [\#1648](https://github.com/supercollider/supercollider/issues/1648)
- Triggering EnvGen with Impulse.ar crashes the server [\#1642](https://github.com/supercollider/supercollider/issues/1642)
- DetectSilence won't free if it never encounters nonzero value [\#1639](https://github.com/supercollider/supercollider/issues/1639)
- IDE: Colorizer highlights "pi" at the start of a word [\#1613](https://github.com/supercollider/supercollider/issues/1613)
- LanguageConfig-includePaths stack overflow [\#1604](https://github.com/supercollider/supercollider/issues/1604)
- Removing  Application Support/SuperCollider causes SCDoc to crash the interpreter [\#1589](https://github.com/supercollider/supercollider/issues/1589)
- supernova tries to set unallocated memory in b\_set and b\_setn [\#1588](https://github.com/supercollider/supercollider/issues/1588)
- failed to read Quark directory listing [\#1585](https://github.com/supercollider/supercollider/issues/1585)
- Example code in documentation doesn't play for LocalIn function.  Missing Out.ar\(0,local\);  [\#1563](https://github.com/supercollider/supercollider/issues/1563)
- OscUGens.cpp omits the required DefineSimpleUnit call for PSinGrain [\#1556](https://github.com/supercollider/supercollider/issues/1556)
- MethodOverride.printAll fails on OSX 10.8 [\#1538](https://github.com/supercollider/supercollider/issues/1538)
- NRT realloc failure [\#1537](https://github.com/supercollider/supercollider/issues/1537)
- useSystemClock breaks the server [\#1534](https://github.com/supercollider/supercollider/issues/1534)
- evaluating code in the help browser evaluates code in the document window instead [\#1532](https://github.com/supercollider/supercollider/issues/1532)
- Installing Quark with an invalid refspec should throw an error [\#1531](https://github.com/supercollider/supercollider/issues/1531)
- FreqScope doesn't work [\#1527](https://github.com/supercollider/supercollider/issues/1527)
- reboot of the internal server crashes interpreter [\#1526](https://github.com/supercollider/supercollider/issues/1526)
- Scroll view issue in Qt -- no vertical scroll bar [\#1521](https://github.com/supercollider/supercollider/issues/1521)
- \(regression\) Document keyDownActions: discrepancy between global and instance [\#1512](https://github.com/supercollider/supercollider/issues/1512)
- Issues with maximized \(fullscreen\) Qt windows on OS X [\#1506](https://github.com/supercollider/supercollider/issues/1506)
- WebView auto-closes when used without explicit parent [\#1505](https://github.com/supercollider/supercollider/issues/1505)
- LFPulse UGen gives asymmetrical waveform when duty \(width\) is 0.5 [\#1501](https://github.com/supercollider/supercollider/issues/1501)
- MIDIdef.mtcQuarterFrame [\#1496](https://github.com/supercollider/supercollider/issues/1496)
- WebView closes after displaying scrollbar [\#1489](https://github.com/supercollider/supercollider/issues/1489)
- MIDIIn.connectAll connects to wrong ports on Linux [\#1487](https://github.com/supercollider/supercollider/issues/1487)
- Sclang client crashes on serial port close from data sender [\#1479](https://github.com/supercollider/supercollider/issues/1479)
- Quarks: memory corruption\(s\) [\#1476](https://github.com/supercollider/supercollider/issues/1476)
- Windows: New Quarks system: installed Quarks not recognized [\#1475](https://github.com/supercollider/supercollider/issues/1475)
- How to complete unload plugins PR? [\#1473](https://github.com/supercollider/supercollider/issues/1473)
- Quarks: refix the valueing of isCompatible [\#1463](https://github.com/supercollider/supercollider/issues/1463)
- Slider with an increment fires action even when value doesn't change [\#1460](https://github.com/supercollider/supercollider/issues/1460)
- Scsynth zombie when quitting scide [\#1456](https://github.com/supercollider/supercollider/issues/1456)
- IDE Help popup -\> crash [\#1452](https://github.com/supercollider/supercollider/issues/1452)
- Quarks windows path error bug [\#1451](https://github.com/supercollider/supercollider/issues/1451)
- Error on interpreter startup [\#1448](https://github.com/supercollider/supercollider/issues/1448)
- Font.defaultMonoFace/.defaultSansFace/.defaultSerifFace return the same value [\#1447](https://github.com/supercollider/supercollider/issues/1447)
- Switching a session causes the IDE to segfault [\#1430](https://github.com/supercollider/supercollider/issues/1430)
- Quarks conflicts with case insensitive file systems [\#1429](https://github.com/supercollider/supercollider/issues/1429)
- Using Function-try inside an ArrayedCollection.do loop corrupts interpreter [\#1428](https://github.com/supercollider/supercollider/issues/1428)
- MIDIFunc.noteOn does not work with floats when Arrays of numbers are passed [\#1426](https://github.com/supercollider/supercollider/issues/1426)
- Sclang keeps crashing on recent build \(Win\) [\#1405](https://github.com/supercollider/supercollider/issues/1405)
- Preferences/Editor/Fonts&Colors/Color:Current Line: sample text display doesn't update to the color selected in Text [\#1403](https://github.com/supercollider/supercollider/issues/1403)
- icon representation of sclang and scserver: and still jumping [\#1399](https://github.com/supercollider/supercollider/issues/1399)
- Blank help window [\#1395](https://github.com/supercollider/supercollider/issues/1395)
- ERROR: ScIDE not connected [\#1390](https://github.com/supercollider/supercollider/issues/1390)
- scdoc - can't run code from help window. [\#1385](https://github.com/supercollider/supercollider/issues/1385)
- GridLayout spanning bug [\#1383](https://github.com/supercollider/supercollider/issues/1383)
- Server window should use reasonable default record buffer size [\#1380](https://github.com/supercollider/supercollider/issues/1380)
- quarks \(new\) - installing local quark does not install it's dependencies it only links. [\#1378](https://github.com/supercollider/supercollider/issues/1378)
- quarks \(new\) - dependencies specified with git url and refspec not working ? [\#1377](https://github.com/supercollider/supercollider/issues/1377)
- Server Gui volume bug \(gitreports.com\) [\#1370](https://github.com/supercollider/supercollider/issues/1370)
- LanguageConfig.current problems [\#1369](https://github.com/supercollider/supercollider/issues/1369)
- same cubic icon with scide and sclang \(OS X\) [\#1367](https://github.com/supercollider/supercollider/issues/1367)
- OSX/IDE: default key for "trigger autocomplete" cmd-space conflicts with spotlight [\#1365](https://github.com/supercollider/supercollider/issues/1365)
- IDE: Splits & Tool panels: after closing panel with esc keyboard focus will always move to top left split/pane [\#1363](https://github.com/supercollider/supercollider/issues/1363)
- New Quarks: if sclang\_config.yaml doesn't exist prior to install of Quark, include-path is not written [\#1362](https://github.com/supercollider/supercollider/issues/1362)
- Quarks install on Windows fails in parseQuarkName [\#1346](https://github.com/supercollider/supercollider/issues/1346)
- fix travis builds posting to github [\#1339](https://github.com/supercollider/supercollider/issues/1339)
- DiskIOUGens zombies [\#1331](https://github.com/supercollider/supercollider/issues/1331)
- supernova dont report lateness [\#1323](https://github.com/supercollider/supercollider/issues/1323)
- .argumentString fails for Methods with argNames==nil [\#1320](https://github.com/supercollider/supercollider/issues/1320)
- Delay1's first output sample is its input, rather than 0 [\#1313](https://github.com/supercollider/supercollider/issues/1313)
- Poll ignores initial trigger [\#1312](https://github.com/supercollider/supercollider/issues/1312)
- OSCdef:free does not free OSCdef when \(srcID != nil\) [\#1306](https://github.com/supercollider/supercollider/issues/1306)
- GeneralHID documentation misleading [\#1303](https://github.com/supercollider/supercollider/issues/1303)
- drag and drop files to expand paths broken [\#1295](https://github.com/supercollider/supercollider/issues/1295)
- LinXFade2 pans backward in at least some cases [\#1294](https://github.com/supercollider/supercollider/issues/1294)
- Disable tokenizing and auto-indent for schelp documents [\#1292](https://github.com/supercollider/supercollider/issues/1292)
- \_String\_Format primitive eats backslashes [\#1291](https://github.com/supercollider/supercollider/issues/1291)
- argumentString includes 'this' for Method [\#1289](https://github.com/supercollider/supercollider/issues/1289)
- ide "Open Recent" doesn't update when loading a new session [\#1287](https://github.com/supercollider/supercollider/issues/1287)
- MultiSliderView.action\_ not triggered by value changes via keyboard [\#1285](https://github.com/supercollider/supercollider/issues/1285)
- TRand, TExpRand, TIRand are broken for audio-rate inputs [\#1278](https://github.com/supercollider/supercollider/issues/1278)
- UserView with drawFunc loops infinitely when resized by code  [\#1274](https://github.com/supercollider/supercollider/issues/1274)
- HelpSource is not installed on Linux anymore [\#1273](https://github.com/supercollider/supercollider/issues/1273)
- MoreDocument: selectionStart doesn't update upon document changes [\#1254](https://github.com/supercollider/supercollider/issues/1254)
- cmake version should be over 2.8.11 for 3.7 [\#1247](https://github.com/supercollider/supercollider/issues/1247)
- notes may hang in NodeProxy when using synth def names \(symbols\) [\#1246](https://github.com/supercollider/supercollider/issues/1246)
- "String".newTextWindow complaints [\#1238](https://github.com/supercollider/supercollider/issues/1238)
- Primitive '\_ScIDE\_SetDocSelectionMirror' failed. [\#1228](https://github.com/supercollider/supercollider/issues/1228)
- dumpOSC method posts status messages when it shouldn't  [\#1227](https://github.com/supercollider/supercollider/issues/1227)
- recent build - osx crashes 10.7 [\#1226](https://github.com/supercollider/supercollider/issues/1226)
- getting these errors WARNING: Attempted to modify missing Text Mirror for Document [\#1220](https://github.com/supercollider/supercollider/issues/1220)
- .post in .onFree\(\)  doesn't post. [\#1219](https://github.com/supercollider/supercollider/issues/1219)
- help-installation broken - possibly under Linux only [\#1218](https://github.com/supercollider/supercollider/issues/1218)
- REPL sometimes doesn't Post [\#1216](https://github.com/supercollider/supercollider/issues/1216)
- TGrains & LocalBuf [\#1204](https://github.com/supercollider/supercollider/issues/1204)
- linux: MIDI - rescanning MIDI.connectAll does not note new ports without disposing client first [\#1194](https://github.com/supercollider/supercollider/issues/1194)
- incorrect plugin directory in cmake: exception in GraphDef\_Recv: UGen 'Control' not installed [\#1181](https://github.com/supercollider/supercollider/issues/1181)
- BlockSize UGen not working due to typo Blocksize -\> BlockSize [\#1180](https://github.com/supercollider/supercollider/issues/1180)
- fatal error: tlsf.h: No such file or directory [\#1176](https://github.com/supercollider/supercollider/issues/1176)
- help not installed properly \(at least under Linux\) [\#1174](https://github.com/supercollider/supercollider/issues/1174)
- Keyboard-shortcuts that contain right or left arrow are not persistent across sc restart [\#1170](https://github.com/supercollider/supercollider/issues/1170)
- Menu-item 'Show Spaces and Docs' confusing [\#1169](https://github.com/supercollider/supercollider/issues/1169)
- Pbindef broken when overloading play key [\#1167](https://github.com/supercollider/supercollider/issues/1167)
- DiskIn channel limitation [\#1162](https://github.com/supercollider/supercollider/issues/1162)
- Document.globalKeyDownAction doesn't apply to new code windows in the IDE [\#1159](https://github.com/supercollider/supercollider/issues/1159)
- synth order shouldn't matter for TrigControl [\#1145](https://github.com/supercollider/supercollider/issues/1145)
- two Dictionaries merge to an Event [\#1142](https://github.com/supercollider/supercollider/issues/1142)
- Crash after launching sc with floating Doc panel [\#1139](https://github.com/supercollider/supercollider/issues/1139)
- Qt5 branch in Ubuntu exhibits some seriously weird behavior [\#1138](https://github.com/supercollider/supercollider/issues/1138)
- ide toggle "Dump OSC" sticks when recompiling sclang [\#1136](https://github.com/supercollider/supercollider/issues/1136)
- Commit f7b708553 breaks Pan2 where a mono NodeProxy is the source [\#1116](https://github.com/supercollider/supercollider/issues/1116)
- JitLib: Ndef.play sending stereo output to bus 0 [\#1114](https://github.com/supercollider/supercollider/issues/1114)
- Jitlib: Ndef\(\y\).play\(numChannels:2\) causes error [\#1113](https://github.com/supercollider/supercollider/issues/1113)
- scvim doesn't close down properly [\#1111](https://github.com/supercollider/supercollider/issues/1111)
- SynthDescLib send can cause recursive sends [\#1110](https://github.com/supercollider/supercollider/issues/1110)
- findAllRegexp seems to be broken [\#1108](https://github.com/supercollider/supercollider/issues/1108)
- tcp server\<-\>sclang communication is broken in 3.7 [\#1099](https://github.com/supercollider/supercollider/issues/1099)
- setting font in TextView fails [\#1097](https://github.com/supercollider/supercollider/issues/1097)
- NdefMixer clips output channel to 99 [\#1096](https://github.com/supercollider/supercollider/issues/1096)
- Incorrect window position with multiple displays and negative x value [\#1090](https://github.com/supercollider/supercollider/issues/1090)
- NdefMixer bug - calling controlspec on EZText [\#1083](https://github.com/supercollider/supercollider/issues/1083)
- keyDown in QtGUI doesn't pass char as indicated for non-printing chars [\#1081](https://github.com/supercollider/supercollider/issues/1081)
- Plotter doesn't respond to arrays of nil as minval/maxval as expected [\#1078](https://github.com/supercollider/supercollider/issues/1078)
- Function-plot scales each channel separately [\#1077](https://github.com/supercollider/supercollider/issues/1077)
- server crashes on control rate input to audio rate DelTapWr [\#1065](https://github.com/supercollider/supercollider/issues/1065)
- dumpOSC doesn't post array type chars [\#1064](https://github.com/supercollider/supercollider/issues/1064)
- EnvGen releases incorrectly for early gate [\#1063](https://github.com/supercollider/supercollider/issues/1063)
- switch called with just one function without var declarations crashes sclang [\#1056](https://github.com/supercollider/supercollider/issues/1056)
- sclang  EXC\_BAD\_ACCESS \(SIGSEGV\) [\#1051](https://github.com/supercollider/supercollider/issues/1051)
- Pcollect behaves incorrectly with event as input [\#1048](https://github.com/supercollider/supercollider/issues/1048)
- Pcollect behaves incorrectly with event as input [\#1047](https://github.com/supercollider/supercollider/issues/1047)
- SplayAz fails when input array has only one element [\#1045](https://github.com/supercollider/supercollider/issues/1045)
- SplayAz fails when input is not an array [\#1044](https://github.com/supercollider/supercollider/issues/1044)
- multichannel control mapping spill over [\#1037](https://github.com/supercollider/supercollider/issues/1037)
- Document title method returns a Symbol - should be a String [\#1031](https://github.com/supercollider/supercollider/issues/1031)
- OSX - sclang crashing [\#1030](https://github.com/supercollider/supercollider/issues/1030)
- sclang - currently not starting properly on terminal due to new document stuff [\#1026](https://github.com/supercollider/supercollider/issues/1026)
- EnvGen releases incorrectly for very very short envelope segments [\#1023](https://github.com/supercollider/supercollider/issues/1023)
- 'Host not found'  error in 10.9 [\#1016](https://github.com/supercollider/supercollider/issues/1016)
- supernova: UIUGens get built but not installed [\#1010](https://github.com/supercollider/supercollider/issues/1010)
- Windows - possible fix for if there is no default input or output device on a machine [\#1009](https://github.com/supercollider/supercollider/issues/1009)
- dictionary doesn't freeze properly [\#1007](https://github.com/supercollider/supercollider/issues/1007)
- .blend outputs array rather than signal in Signal [\#1004](https://github.com/supercollider/supercollider/issues/1004)
- long enough function chain causes sclang to output bogus results [\#999](https://github.com/supercollider/supercollider/issues/999)
- Cmd-W doesn't close help window when it is detached and front [\#996](https://github.com/supercollider/supercollider/issues/996)
- scide: Object::connect: No such slot ScIDE::DocumentManager::updateCurrentDocContents\(int, int, int\) in ../editors/sc-ide/core/doc\_manager.cpp:907 [\#985](https://github.com/supercollider/supercollider/issues/985)
- scrolling scide is extremely slow on osx [\#984](https://github.com/supercollider/supercollider/issues/984)
- sc does not build against 10.9 sdk [\#982](https://github.com/supercollider/supercollider/issues/982)
- sclang consumes high CPU when opening a session with multiple files [\#980](https://github.com/supercollider/supercollider/issues/980)
- \[ide\] help-\>"how to use the supercollider ide" detaches help browser [\#976](https://github.com/supercollider/supercollider/issues/976)
- NamedControl.new\(\) returns single-element array of Lags when it should return a Lag [\#973](https://github.com/supercollider/supercollider/issues/973)
- NdefGUI and related don't show vol labels in IDE [\#965](https://github.com/supercollider/supercollider/issues/965)
- Primitive error in \_ScIDE\_SetDocTextMirror called from openDocument [\#964](https://github.com/supercollider/supercollider/issues/964)
- ide document: errors when closing document windows during language init [\#961](https://github.com/supercollider/supercollider/issues/961)
- SC\_QT=OFF build flag still needs QT on OSX [\#959](https://github.com/supercollider/supercollider/issues/959)
- dumpOSC is broken [\#955](https://github.com/supercollider/supercollider/issues/955)
- IDE passes wrong Document modifiers to mouseDownAction [\#952](https://github.com/supercollider/supercollider/issues/952)
- scope buffer allocation problem [\#937](https://github.com/supercollider/supercollider/issues/937)
- Programmatically opening help files bug on linux [\#931](https://github.com/supercollider/supercollider/issues/931)
- Sending to a NetAddr with broadcast IP fails [\#930](https://github.com/supercollider/supercollider/issues/930)
- Windows - SCIDE: font-size snaps down in editor pane on preferences save/apply [\#928](https://github.com/supercollider/supercollider/issues/928)
- QView doesn't receive all key down actions [\#918](https://github.com/supercollider/supercollider/issues/918)
- supernova: setting an array onto a control that doesn't exist will set value that does exist [\#916](https://github.com/supercollider/supercollider/issues/916)
-  Qt menu's with \> 10 items get a scrollbar, and at the same time the view doesn't get wider [\#915](https://github.com/supercollider/supercollider/issues/915)
- BinaryOpUGen 'div' operator: off-by-one and not integer division [\#907](https://github.com/supercollider/supercollider/issues/907)
- SimpleNumber-curvelin broken for negative numbers [\#902](https://github.com/supercollider/supercollider/issues/902)
- switching session: cancel loses unsaved documents and dialog blocks reviewing them [\#899](https://github.com/supercollider/supercollider/issues/899)
- Ramp UGen fails in LTI test [\#888](https://github.com/supercollider/supercollider/issues/888)
- Pitch UGen fails in unit test [\#884](https://github.com/supercollider/supercollider/issues/884)
- prSimpleNumberSeries throws incomprehensible error for bad inputs [\#882](https://github.com/supercollider/supercollider/issues/882)
- uiugens: osx uses deprecated API [\#880](https://github.com/supercollider/supercollider/issues/880)
- A few UGens failing the unit tests [\#879](https://github.com/supercollider/supercollider/issues/879)
- Alias Manager depreated: sc\_ResolveIfAlias will have to be rewritten [\#875](https://github.com/supercollider/supercollider/issues/875)
- NodeProxy: multi-channel expansion broken for SynthDefs [\#872](https://github.com/supercollider/supercollider/issues/872)
- git clone --recursive is broken [\#866](https://github.com/supercollider/supercollider/issues/866)
- Windows: No SCIDE-icon [\#854](https://github.com/supercollider/supercollider/issues/854)
- Linux FFT/IFFT: Hann window with hop size 0.25 sounds wrong [\#851](https://github.com/supercollider/supercollider/issues/851)
- SendReply kills server [\#850](https://github.com/supercollider/supercollider/issues/850)
- SuperCollider can't boot on disabling microphone [\#844](https://github.com/supercollider/supercollider/issues/844)
- SendReply.ar crashes the server [\#841](https://github.com/supercollider/supercollider/issues/841)
- chained function application giving strange results [\#839](https://github.com/supercollider/supercollider/issues/839)
- QListView:items\_ can crash sclang [\#835](https://github.com/supercollider/supercollider/issues/835)
- menu items in the title bar vanish when detached window comes in focus [\#829](https://github.com/supercollider/supercollider/issues/829)
- ScrollView in Qt - setting visibleOrigin doesn't work unless deferred [\#823](https://github.com/supercollider/supercollider/issues/823)
- SinOsc phase argument fails for values outside +-8pi [\#815](https://github.com/supercollider/supercollider/issues/815)
- Return key before closing bracket: empty line is not always appropriate [\#814](https://github.com/supercollider/supercollider/issues/814)
- Drag-and-drop broken on windows with 'alwaysOnTop' enabled [\#812](https://github.com/supercollider/supercollider/issues/812)
- IDE Config \> Editor \> Font Settings: Unsetting "Show only monospaced" doesn't return the previously selected font [\#811](https://github.com/supercollider/supercollider/issues/811)
- sclang crashes for some GUI operation involving QTextEngine::itemize\(\) [\#794](https://github.com/supercollider/supercollider/issues/794)
- LocalOut without LocalIn crashes the server [\#780](https://github.com/supercollider/supercollider/issues/780)
- QWindow crash with wrong argument types [\#770](https://github.com/supercollider/supercollider/issues/770)
- s.options.sampleRate\_ setter requires boot then reboot to take effect [\#768](https://github.com/supercollider/supercollider/issues/768)
- FreqScope not updating correctly on current development version. [\#767](https://github.com/supercollider/supercollider/issues/767)
- Wrongly reported external document changes [\#758](https://github.com/supercollider/supercollider/issues/758)
