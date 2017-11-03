# Change Log

3.9.0-beta1 (2017-10-25)
========================

We are proud to announce the arrival of SuperCollider 3.9.0-beta1! Apologies
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


IDE & SCDoc: Added
-----

Entries in the Documents docklet can be reordered, and document tabs will automatically reorder to reflect this ([#2555](https://github.com/supercollider/supercollider/pull/2555)).

"Edit > Preferences > Editor > Display" has a new option that allows replacing tabs with a dropdown whose items are alphabetically ordered ([#2555](https://github.com/supercollider/supercollider/pull/2555)). This makes navigation easier in some performance contexts.

IDE & SCDoc: Changed
-------

Server actions, which were previously in the "Language" menu, have been moved out to their own "Server" menu ([#3049](https://github.com/supercollider/supercollider/pull/3049)).

Changed "occurrences" to "matches" in the status bar in the Find and Replace features ([#2702](https://github.com/supercollider/supercollider/pull/2702)).

Many minor improvements were made to the look and feel of the documentation ([#2944](https://github.com/supercollider/supercollider/pull/2944), [#2945](https://github.com/supercollider/supercollider/pull/2945), [#2947](https://github.com/supercollider/supercollider/pull/2947), [#2948](https://github.com/supercollider/supercollider/pull/2948), [#2967](https://github.com/supercollider/supercollider/pull/2967), [#3006](https://github.com/supercollider/supercollider/pull/3006), [#3022](https://github.com/supercollider/supercollider/pull/3022), [#3025](https://github.com/supercollider/supercollider/pull/3025), [#3034](https://github.com/supercollider/supercollider/pull/3034), [#3175](https://github.com/supercollider/supercollider/pull/3175)).

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
  [#2273](https://github.com/supercollider/supercollider/pull/2273) by [tiagmoraismorgado](https://github.com/tiagmoraismorgado)
- Update MouseX.schelp
  [#2272](https://github.com/supercollider/supercollider/pull/2272) by [tiagmoraismorgado](https://github.com/tiagmoraismorgado)
- Update MouseButton.schelp
  [#2271](https://github.com/supercollider/supercollider/pull/2271) by [tiagmoraismorgado](https://github.com/tiagmoraismorgado)
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
  [#2150](https://github.com/supercollider/supercollider/pull/2150) by [tiagmoraismorgado](https://github.com/tiagmoraismorgado)
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

## [Version-3.6.6](https://github.com/supercollider/supercollider/tree/Version-3.6.6) (2013-11-27)
[Full Changelog](https://github.com/supercollider/supercollider/compare/Version-3.6.5...Version-3.6.6)

**Enhancements**

- Enhance OSX readme for 3.6.6 [\#1001](https://github.com/supercollider/supercollider/issues/1001)
- "Exception when parsing synthdef: corrupted synthdef" - report which synthdef caused the error. [\#904](https://github.com/supercollider/supercollider/issues/904)
- "start coding" window title [\#898](https://github.com/supercollider/supercollider/issues/898)
- default file extension missing [\#838](https://github.com/supercollider/supercollider/issues/838)

**Fixed bugs:**

- scide: Object::connect: No such slot ScIDE::DocumentManager::updateCurrentDocContents\(int, int, int\) in ../editors/sc-ide/core/doc\_manager.cpp:907 [\#985](https://github.com/supercollider/supercollider/issues/985)
- sclang consumes high CPU when opening a session with multiple files [\#980](https://github.com/supercollider/supercollider/issues/980)
- \[ide\] help-\>"how to use the supercollider ide" detaches help browser [\#976](https://github.com/supercollider/supercollider/issues/976)
- NdefGUI and related don't show vol labels in IDE [\#965](https://github.com/supercollider/supercollider/issues/965)
- Primitive error in \_ScIDE\_SetDocTextMirror called from openDocument [\#964](https://github.com/supercollider/supercollider/issues/964)
- ide document: errors when closing document windows during language init [\#961](https://github.com/supercollider/supercollider/issues/961)
- SC\_QT=OFF build flag still needs QT on OSX [\#959](https://github.com/supercollider/supercollider/issues/959)
- Sending to a NetAddr with broadcast IP fails [\#930](https://github.com/supercollider/supercollider/issues/930)
- supernova: setting an array onto a control that doesn't exist will set value that does exist [\#916](https://github.com/supercollider/supercollider/issues/916)
- BinaryOpUGen 'div' operator: off-by-one and not integer division [\#907](https://github.com/supercollider/supercollider/issues/907)
- SimpleNumber-curvelin broken for negative numbers [\#902](https://github.com/supercollider/supercollider/issues/902)
- switching session: cancel loses unsaved documents and dialog blocks reviewing them [\#899](https://github.com/supercollider/supercollider/issues/899)
- Ramp UGen fails in LTI test [\#888](https://github.com/supercollider/supercollider/issues/888)
- prSimpleNumberSeries throws incomprehensible error for bad inputs [\#882](https://github.com/supercollider/supercollider/issues/882)
- uiugens: osx uses deprecated API [\#880](https://github.com/supercollider/supercollider/issues/880)
- A few UGens failing the unit tests [\#879](https://github.com/supercollider/supercollider/issues/879)
- Alias Manager depreated: sc\_ResolveIfAlias will have to be rewritten [\#875](https://github.com/supercollider/supercollider/issues/875)
- NodeProxy: multi-channel expansion broken for SynthDefs [\#872](https://github.com/supercollider/supercollider/issues/872)
- git clone --recursive is broken [\#866](https://github.com/supercollider/supercollider/issues/866)
- Windows: No SCIDE-icon [\#854](https://github.com/supercollider/supercollider/issues/854)
- Linux FFT/IFFT: Hann window with hop size 0.25 sounds wrong [\#851](https://github.com/supercollider/supercollider/issues/851)
- SendReply kills server [\#850](https://github.com/supercollider/supercollider/issues/850)
- SendReply.ar crashes the server [\#841](https://github.com/supercollider/supercollider/issues/841)

**Closed issues:**

- Error in Help - Getting Started Tutorial Series - Section 2 [\#981](https://github.com/supercollider/supercollider/issues/981)
- Crash in creating SynthDef [\#957](https://github.com/supercollider/supercollider/issues/957)
- supernova compilation error [\#951](https://github.com/supercollider/supercollider/issues/951)
- supercollider-gedit plugin not loading [\#950](https://github.com/supercollider/supercollider/issues/950)
- OSCFunc addr.hostname returns nil [\#933](https://github.com/supercollider/supercollider/issues/933)
- iOS build, doesn't. [\#927](https://github.com/supercollider/supercollider/issues/927)
- Disable mathjax - an unused feature that can cause problems [\#909](https://github.com/supercollider/supercollider/issues/909)
- DiskIn channel limitation [\#901](https://github.com/supercollider/supercollider/issues/901)
- sclang \(readline\) cannot quit [\#897](https://github.com/supercollider/supercollider/issues/897)
- PV\_Copy breaks edge model [\#896](https://github.com/supercollider/supercollider/issues/896)
- linux \(debian x64\) compilation error [\#894](https://github.com/supercollider/supercollider/issues/894)
- osx/wii uses deprecated/removed API [\#876](https://github.com/supercollider/supercollider/issues/876)
- drawFunc drawingEnabled\_\(false\) not respected by refresh [\#869](https://github.com/supercollider/supercollider/issues/869)
- file browser lacks default [\#868](https://github.com/supercollider/supercollider/issues/868)
- Issue Tracker: user can't enter categories/lables [\#860](https://github.com/supercollider/supercollider/issues/860)
- Windows \(installer\): no program shortcuts [\#857](https://github.com/supercollider/supercollider/issues/857)
- Windows: installer does not register in system [\#856](https://github.com/supercollider/supercollider/issues/856)
- Windows: no file associations [\#855](https://github.com/supercollider/supercollider/issues/855)
- Server load percentage on Windows off by factor 100 [\#849](https://github.com/supercollider/supercollider/issues/849)
- scsynth sound stops in 3.5 not in 3.4 [\#343](https://github.com/supercollider/supercollider/issues/343)

**Merged pull requests:**

- class library: pbind midi type - fix for sending sysex [\#947](https://github.com/supercollider/supercollider/pull/947) ([redFrik](https://github.com/redFrik))
- supernova: TCP support [\#757](https://github.com/supercollider/supercollider/issues/757)
- sclang crashes when 'connect'ing a NetAddr for TCP [\#755](https://github.com/supercollider/supercollider/issues/755)
- EnvGen's gate argument can crash the server [\#753](https://github.com/supercollider/supercollider/issues/753)
- s.sync returns too soon after buffer load [\#750](https://github.com/supercollider/supercollider/issues/750)
- Broken color dialog [\#749](https://github.com/supercollider/supercollider/issues/749)
- Triple-click line selection only works when clicking on strings \[mac\] [\#748](https://github.com/supercollider/supercollider/issues/748)
- When undocked, docklets may appear off screen, impossible to move [\#730](https://github.com/supercollider/supercollider/issues/730)
- malformed YAML crashes interpreter [\#728](https://github.com/supercollider/supercollider/issues/728)
- SequenceableCollection resamp0/1 broken \(e.g. SplayAz\) [\#727](https://github.com/supercollider/supercollider/issues/727)
- Shaper should guard against negative buffer numbers [\#722](https://github.com/supercollider/supercollider/issues/722)
- wavetable oscillators broken for large wavetables [\#720](https://github.com/supercollider/supercollider/issues/720)
- prevent buffer overrun in IO UGens [\#716](https://github.com/supercollider/supercollider/issues/716)
- crash report Exception Type:  EXC\_BAD\_ACCESS \(SIGSEGV\) [\#715](https://github.com/supercollider/supercollider/issues/715)
- Dialog.openPanel hangs on Windows XP [\#707](https://github.com/supercollider/supercollider/issues/707)
- SCTextView cmd-d crash [\#702](https://github.com/supercollider/supercollider/issues/702)
- cmd+B quite server as default is really unsafe for performance [\#699](https://github.com/supercollider/supercollider/issues/699)
- Knob does not allow change of background color  [\#697](https://github.com/supercollider/supercollider/issues/697)
- Platform.getMouseCoords does not work [\#696](https://github.com/supercollider/supercollider/issues/696)
- scope issues: dies with CmdPeriod, not old behaviour  [\#695](https://github.com/supercollider/supercollider/issues/695)
- Spawner:suspend doesn't execute EventStreamCleanup actions [\#681](https://github.com/supercollider/supercollider/issues/681)
- IDE window looses focus when subwindow is closed without action \(OS X\) [\#678](https://github.com/supercollider/supercollider/issues/678)
- selected text gets deselected when IDE editor window gets clicked on to get it to front [\#677](https://github.com/supercollider/supercollider/issues/677)
- "External-file-change" dialog pops up when saving file in IDE [\#673](https://github.com/supercollider/supercollider/issues/673)
- Problems with DragSource [\#671](https://github.com/supercollider/supercollider/issues/671)
- Key commands like STOP \(cmd + . \) don't work in Help window when it's detached  [\#668](https://github.com/supercollider/supercollider/issues/668)
- ndef play producing click when evaluated  [\#664](https://github.com/supercollider/supercollider/issues/664)
- Ndef channel wrapping broken for many channels [\#658](https://github.com/supercollider/supercollider/issues/658)
- Window's "detached" state is not persisted if it was closed before quit [\#655](https://github.com/supercollider/supercollider/issues/655)
- Crash when re-opening undocked help window [\#650](https://github.com/supercollider/supercollider/issues/650)
- crash on recompile if OSCFunc receiving messages on non-sclang port [\#649](https://github.com/supercollider/supercollider/issues/649)
- Array:slide bug [\#645](https://github.com/supercollider/supercollider/issues/645)
- got NotYetImplementedError with 3.6beta2 [\#641](https://github.com/supercollider/supercollider/issues/641)
- jitlib guis hardly readable [\#637](https://github.com/supercollider/supercollider/issues/637)
- Server -O and -I options causes seg fault and crashes sclang [\#636](https://github.com/supercollider/supercollider/issues/636)
- crash when closing tab [\#635](https://github.com/supercollider/supercollider/issues/635)
- \[SC-IDE\]\[MAC\] cmd+f \(resp. cmd+r\) doesn't work in the IDE [\#634](https://github.com/supercollider/supercollider/issues/634)
- Invoking Help does not bring it to front if: \(1\) it is in a hidden tab AND \(2\) the query string is empty [\#631](https://github.com/supercollider/supercollider/issues/631)
- command FIFO Full again [\#613](https://github.com/supercollider/supercollider/issues/613)
- Scale with scale name doesn't work as described [\#611](https://github.com/supercollider/supercollider/issues/611)
- Drag text from document to GUI deletes the text [\#610](https://github.com/supercollider/supercollider/issues/610)
- Auto-indent doesn't respect string and symbol literals [\#609](https://github.com/supercollider/supercollider/issues/609)
- SoundFileView selectNone and selectAll does not work [\#607](https://github.com/supercollider/supercollider/issues/607)
- QWindow, QView background color alpha broken on 3.6beta? [\#606](https://github.com/supercollider/supercollider/issues/606)
- Crash when evaluating code in non-code files [\#605](https://github.com/supercollider/supercollider/issues/605)
- 3.5.6 with sc3-plugins: SCDoc.renderAll\(true\) fails [\#604](https://github.com/supercollider/supercollider/issues/604)
- Evaluate line doesn't work if text wraps [\#601](https://github.com/supercollider/supercollider/issues/601)
- crash in autocompleter [\#600](https://github.com/supercollider/supercollider/issues/600)
- inconsistent behavior of Env.duration\_  [\#598](https://github.com/supercollider/supercollider/issues/598)
- multichannel Envelope.plot broken [\#592](https://github.com/supercollider/supercollider/issues/592)
- Cmd-I doesn't work from the Command line pane [\#588](https://github.com/supercollider/supercollider/issues/588)
- The post window seems to use double the tab size of documents [\#586](https://github.com/supercollider/supercollider/issues/586)
- Freqscope and Scope can't be used at the same time [\#580](https://github.com/supercollider/supercollider/issues/580)
- Shift-up/down at start/end of file cancels the selection [\#579](https://github.com/supercollider/supercollider/issues/579)
- Close brace } put at the wrong indent level in new SC IDE editor [\#578](https://github.com/supercollider/supercollider/issues/578)
- Can't type \[ \(open bracket\) character with Spanish keyboard [\#577](https://github.com/supercollider/supercollider/issues/577)
- Regexp with "^" makes sc-ide hang [\#576](https://github.com/supercollider/supercollider/issues/576)
- File extension warning when saving a new file [\#575](https://github.com/supercollider/supercollider/issues/575)
- There are problems with copying text from the post window [\#574](https://github.com/supercollider/supercollider/issues/574)
- Do not open HelpBrowser on application start if it was closed in the restored window state [\#572](https://github.com/supercollider/supercollider/issues/572)
- IDE help browser incorrectly shows "old help" warning [\#571](https://github.com/supercollider/supercollider/issues/571)
- Hitting Cmd/Ctrl while inertial scrolling causes rapid zoom in / out [\#568](https://github.com/supercollider/supercollider/issues/568)
- Deleting a session will lose unsaved documents w/o warning [\#565](https://github.com/supercollider/supercollider/issues/565)
- Save All is useless with one or more read only documents open [\#564](https://github.com/supercollider/supercollider/issues/564)
- Inline help: example code cannot be executed with regular keyboard shortcut [\#563](https://github.com/supercollider/supercollider/issues/563)
- Document tabs can become unusably small [\#562](https://github.com/supercollider/supercollider/issues/562)
- Cmd+Period doesn't work in Qt GUI windows [\#561](https://github.com/supercollider/supercollider/issues/561)
- Inaccurate wording in language configuration message box [\#559](https://github.com/supercollider/supercollider/issues/559)
- Aggressive method call aid within class method arg lists [\#557](https://github.com/supercollider/supercollider/issues/557)
- Mouse pointer vanishes when modifier keys are pressed [\#554](https://github.com/supercollider/supercollider/issues/554)
- changing file type should update/reload CodeEditor [\#551](https://github.com/supercollider/supercollider/issues/551)
- Win XP only: TempoClock scheduling crashes sclang [\#547](https://github.com/supercollider/supercollider/issues/547)
- Help browser: progress indicator stuck when opening links in code editor [\#546](https://github.com/supercollider/supercollider/issues/546)
- Help browser: can not evaluate a single line in code examples [\#545](https://github.com/supercollider/supercollider/issues/545)
- Help browser: code examples not syntax highlighted [\#544](https://github.com/supercollider/supercollider/issues/544)
- Cmd-Period doesn't work with help focussed. [\#541](https://github.com/supercollider/supercollider/issues/541)
- Cmd-Return / Shift-Return inconsistency in IDE and lang. [\#540](https://github.com/supercollider/supercollider/issues/540)
- IDE hides when another application becomes front [\#539](https://github.com/supercollider/supercollider/issues/539)
- Cmd+Alt+\<Arrow key\> shortcuts not saved correctly [\#538](https://github.com/supercollider/supercollider/issues/538)
- IDE hides when another application becomes front [\#537](https://github.com/supercollider/supercollider/issues/537)
- Line numbers don't match lines [\#536](https://github.com/supercollider/supercollider/issues/536)
- scapp: library menu crash  [\#534](https://github.com/supercollider/supercollider/issues/534)
- Superfluous blank lines between postln lines [\#533](https://github.com/supercollider/supercollider/issues/533)
- "Save" action for read-only files should trigger "Save as" [\#532](https://github.com/supercollider/supercollider/issues/532)
- Ctrl-shift-C doesn't clear the post window in winxp [\#531](https://github.com/supercollider/supercollider/issues/531)
- call hints don't scroll [\#529](https://github.com/supercollider/supercollider/issues/529)
- wrong font spacing in QT [\#527](https://github.com/supercollider/supercollider/issues/527)
- some color configuration items don't show default values [\#526](https://github.com/supercollider/supercollider/issues/526)
- nowExecutingPath in a newly saved document [\#523](https://github.com/supercollider/supercollider/issues/523)
- Saving as .sc saves as .scd instead [\#518](https://github.com/supercollider/supercollider/issues/518)
- Shortcuts reset after quiting [\#517](https://github.com/supercollider/supercollider/issues/517)
- server does not boot [\#512](https://github.com/supercollider/supercollider/issues/512)
- Wacom Bamboo Pen & Touch crashes [\#509](https://github.com/supercollider/supercollider/issues/509)
- "maxval" ignored by Buffer.plot [\#507](https://github.com/supercollider/supercollider/issues/507)
- Autocomplete sometimes pops up when I type open-bracket [\#505](https://github.com/supercollider/supercollider/issues/505)
- drag/drop a WAV onto scide should not attempt to open it [\#502](https://github.com/supercollider/supercollider/issues/502)
- Plugging in headphones crashes scsynth [\#499](https://github.com/supercollider/supercollider/issues/499)
- Save All doesn't update all document tabs [\#497](https://github.com/supercollider/supercollider/issues/497)
- general text colors are applied everywhere [\#496](https://github.com/supercollider/supercollider/issues/496)
- \[supernova\] fix g\_dumpTree [\#494](https://github.com/supercollider/supercollider/issues/494)
- "Step forward": Cursor doesn't move if line starts with "\(" and cursor is inside [\#493](https://github.com/supercollider/supercollider/issues/493)
- Method Call Assist: infer the class of literal values as receivers [\#490](https://github.com/supercollider/supercollider/issues/490)
- Method Call Assist: Alt+Space doesn't trigger [\#489](https://github.com/supercollider/supercollider/issues/489)
- \[CRITICAL\] IDE crashes when autocompleting methods of an integer literal [\#488](https://github.com/supercollider/supercollider/issues/488)
- Sessions: Document tabs out of order sometimes [\#485](https://github.com/supercollider/supercollider/issues/485)
- QEnvelopeView does not support quadratic/cubic shapes [\#483](https://github.com/supercollider/supercollider/issues/483)
- SCDoc document browser & search broken [\#482](https://github.com/supercollider/supercollider/issues/482)
- Autocompletion box pops up when pasting text -- shouldn't [\#481](https://github.com/supercollider/supercollider/issues/481)
- Server control state problem [\#477](https://github.com/supercollider/supercollider/issues/477)
- Server-meter window errors [\#476](https://github.com/supercollider/supercollider/issues/476)
- evaluate regions when being at opening bracket [\#474](https://github.com/supercollider/supercollider/issues/474)
- Up-arrow after RET does not move the insertion point straight up [\#472](https://github.com/supercollider/supercollider/issues/472)
- Multichannel Function:plot is broken in master [\#470](https://github.com/supercollider/supercollider/issues/470)
- new lines do not scroll anymore [\#467](https://github.com/supercollider/supercollider/issues/467)
- line numbers when opening files [\#466](https://github.com/supercollider/supercollider/issues/466)
- goto next block broken [\#465](https://github.com/supercollider/supercollider/issues/465)
- \[QWebView Windows 2\] Handling of file:// in \<a href=""\> [\#463](https://github.com/supercollider/supercollider/issues/463)
- \[QWebView Windows 1\] Handling of file:// links [\#462](https://github.com/supercollider/supercollider/issues/462)
- session: save-as glitch [\#458](https://github.com/supercollider/supercollider/issues/458)
- Method Call Assist: fix for functional notation [\#457](https://github.com/supercollider/supercollider/issues/457)
- "Step forward": Put the cursor at the beginning of the next line [\#454](https://github.com/supercollider/supercollider/issues/454)
- ide: ctrl-enter adds strange invisible line breaks [\#451](https://github.com/supercollider/supercollider/issues/451)
- Interactive find behavior is... hyperactive [\#437](https://github.com/supercollider/supercollider/issues/437)
- Line breaks in the post window, not generated by sc code [\#436](https://github.com/supercollider/supercollider/issues/436)
- Cannot dismiss the method signature popup for a "new" class method [\#435](https://github.com/supercollider/supercollider/issues/435)
- SerialPort doneAction never called & port not properly closed? [\#434](https://github.com/supercollider/supercollider/issues/434)
- SC-ide runs its own server status thread? [\#433](https://github.com/supercollider/supercollider/issues/433)
- Questionable, but common, auto indent case [\#432](https://github.com/supercollider/supercollider/issues/432)
- Highlighting flash of just-executed code sometimes gets stuck [\#431](https://github.com/supercollider/supercollider/issues/431)
- Interpreter \*is\* running but "Interpreter is not running!" [\#425](https://github.com/supercollider/supercollider/issues/425)
- SC 3.5.4 crashes with buffers on windows 7 [\#423](https://github.com/supercollider/supercollider/issues/423)
- stack smashing crash on certain code [\#422](https://github.com/supercollider/supercollider/issues/422)
- set nowExecutingPath from scide [\#418](https://github.com/supercollider/supercollider/issues/418)
- running sclang script does not set nowExecutingPath [\#417](https://github.com/supercollider/supercollider/issues/417)
- \[SC-IDE\] Ctrl-R replace panel: Replace button doesn't replace [\#416](https://github.com/supercollider/supercollider/issues/416)
- \[SC 3.5.4\] GrainBuf SynthDef causes SC to crash [\#415](https://github.com/supercollider/supercollider/issues/415)
- Preference pane does not select default font [\#414](https://github.com/supercollider/supercollider/issues/414)
- indentation bug [\#409](https://github.com/supercollider/supercollider/issues/409)
- ctrl+c after selecting text in post window does not copy [\#408](https://github.com/supercollider/supercollider/issues/408)
- Keyboard shortcut for Shrink font in post window says 'Shink' [\#406](https://github.com/supercollider/supercollider/issues/406)
- make install fails on verify app sclang.app, and it's not fixed-up correctly [\#401](https://github.com/supercollider/supercollider/issues/401)
- Tabs don't overflow correctly [\#395](https://github.com/supercollider/supercollider/issues/395)
- Volume initialization bug [\#393](https://github.com/supercollider/supercollider/issues/393)
- Multiple clocks after fullscreen [\#390](https://github.com/supercollider/supercollider/issues/390)
- double-click behavior [\#388](https://github.com/supercollider/supercollider/issues/388)
- Syntax color settings are not saved [\#385](https://github.com/supercollider/supercollider/issues/385)
- scide: "Auto Scroll" looks like a button but it can't be clicked off [\#378](https://github.com/supercollider/supercollider/issues/378)
- Drag'n'drop in QtGui makes Sc-Client crahs [\#375](https://github.com/supercollider/supercollider/issues/375)
- Method call aid popup stays shown when main window minimized/deactivated [\#369](https://github.com/supercollider/supercollider/issues/369)
- Language Configuration broken for osx command line sclang [\#366](https://github.com/supercollider/supercollider/issues/366)
- Missing boundary check in prArrayWrapExtend [\#360](https://github.com/supercollider/supercollider/issues/360)
- MIDI listEndPoints crash [\#353](https://github.com/supercollider/supercollider/issues/353)
- GrainBuf crashes server when position = NaN  [\#352](https://github.com/supercollider/supercollider/issues/352)
- GrainBuf does not check buffer inputs to see if they exist [\#341](https://github.com/supercollider/supercollider/issues/341)
- \[SC-IDE\] Preferences -\> Keyboard shortcuts & font issues [\#339](https://github.com/supercollider/supercollider/issues/339)
- \[SC-IDE\] Region Detection [\#338](https://github.com/supercollider/supercollider/issues/338)
- \[SC-IDE\] Cannot save new file [\#337](https://github.com/supercollider/supercollider/issues/337)
- \[SC-IDE\] Open class definition + file already open [\#335](https://github.com/supercollider/supercollider/issues/335)
- \[SC-IDE\] Don't open same file twice [\#328](https://github.com/supercollider/supercollider/issues/328)
- \[SC-IDE\] Can't copy text from post window [\#322](https://github.com/supercollider/supercollider/issues/322)
- \[SC-IDE\] Add .scd file extension [\#320](https://github.com/supercollider/supercollider/issues/320)
- SCDoc.rendelAll broken in 3.5 branch [\#265](https://github.com/supercollider/supercollider/issues/265)
- Plotter domainSpecs bug [\#264](https://github.com/supercollider/supercollider/issues/264)
- \[Qt\] UserView doesn't pass click count for control clicks [\#263](https://github.com/supercollider/supercollider/issues/263)
- Sending sysex is broken [\#262](https://github.com/supercollider/supercollider/issues/262)
- sc3-plugins [\#261](https://github.com/supercollider/supercollider/issues/261)
- WARNING:  server 'localhost' not running.  nil [\#260](https://github.com/supercollider/supercollider/issues/260)
- Server crash on polling phase of DelTapWr [\#258](https://github.com/supercollider/supercollider/issues/258)
- opening this file crashes sc.app [\#257](https://github.com/supercollider/supercollider/issues/257)
- .add fails with SynthDefs containing \> 255 controls [\#256](https://github.com/supercollider/supercollider/issues/256)
- .freqscope not working on OS X 3.5 RC2 and RC3 [\#255](https://github.com/supercollider/supercollider/issues/255)
- HrEnvelopeView Crash in Cocoa [\#254](https://github.com/supercollider/supercollider/issues/254)
- KeyTrack not working with LocalBuf [\#253](https://github.com/supercollider/supercollider/issues/253)
- SCNSObject broken under osx 64bit [\#252](https://github.com/supercollider/supercollider/issues/252)
- 3.5.rc1 - -getControlBusValue only supports local servers [\#250](https://github.com/supercollider/supercollider/issues/250)
- 0 \> Signal\[0\] causes infinite loop and memory hogging [\#249](https://github.com/supercollider/supercollider/issues/249)
- 3.5 beta 3 crash on startup after forced quit [\#247](https://github.com/supercollider/supercollider/issues/247)
- same font looks different in qt/cocoa [\#245](https://github.com/supercollider/supercollider/issues/245)
- Segault:while loops with Routine in condition + empty bodies [\#244](https://github.com/supercollider/supercollider/issues/244)
- draw methods for String [\#242](https://github.com/supercollider/supercollider/issues/242)
- Some views does not work inside a Layout [\#240](https://github.com/supercollider/supercollider/issues/240)
- escapeWindow crashes app [\#239](https://github.com/supercollider/supercollider/issues/239)
- compiling error [\#238](https://github.com/supercollider/supercollider/issues/238)
- mouseOverActions trigger normal `action' [\#237](https://github.com/supercollider/supercollider/issues/237)
- alsa midi deadlock [\#236](https://github.com/supercollider/supercollider/issues/236)
- lib fixup for scsynth and sclang [\#235](https://github.com/supercollider/supercollider/issues/235)
- Outdated About dialog [\#234](https://github.com/supercollider/supercollider/issues/234)
- Pbind makes no sound when utilized in ProxySpace environment [\#233](https://github.com/supercollider/supercollider/issues/233)
- \[win\] UnixFILE GetInt primitives return unsigned [\#231](https://github.com/supercollider/supercollider/issues/231)
- Quarks GUI info button doesn't work [\#230](https://github.com/supercollider/supercollider/issues/230)
- SCEnvelopeView broken [\#229](https://github.com/supercollider/supercollider/issues/229)
- Internal server fails to quit on reboot [\#228](https://github.com/supercollider/supercollider/issues/228)
- Segmentation fault \(no X\) [\#227](https://github.com/supercollider/supercollider/issues/227)
- scvim: crash on startup [\#226](https://github.com/supercollider/supercollider/issues/226)
- DetectSilence initialization [\#225](https://github.com/supercollider/supercollider/issues/225)
- scel + qt: Document.open from qt doesn't run the callback [\#224](https://github.com/supercollider/supercollider/issues/224)
- QButton doesn't pass in modifier keycode to button action [\#223](https://github.com/supercollider/supercollider/issues/223)
- OSX does not resolve symlinks [\#222](https://github.com/supercollider/supercollider/issues/222)
- CmdPeriod + Task + AppClock = fail [\#221](https://github.com/supercollider/supercollider/issues/221)
- Broken: A sequence of multiple Pfxs with 'isolate' [\#220](https://github.com/supercollider/supercollider/issues/220)
- CmdPeriod broken [\#219](https://github.com/supercollider/supercollider/issues/219)
- UserView graphics on linux is \*very\* slow [\#218](https://github.com/supercollider/supercollider/issues/218)
- \[plot\] superposing envelopes [\#217](https://github.com/supercollider/supercollider/issues/217)
- SCDoc search results appear in arbitrary order [\#216](https://github.com/supercollider/supercollider/issues/216)
- FreeVerb/2 does not check if inputs are audio rate. [\#214](https://github.com/supercollider/supercollider/issues/214)
- randSeed doesn't effect Drand [\#212](https://github.com/supercollider/supercollider/issues/212)
- Subsection highlighting css bug [\#211](https://github.com/supercollider/supercollider/issues/211)
- cocoa window alpha\_ [\#210](https://github.com/supercollider/supercollider/issues/210)
- Minor colorizing bug in scdoc rendering [\#209](https://github.com/supercollider/supercollider/issues/209)
- SynthDef add fails silently [\#207](https://github.com/supercollider/supercollider/issues/207)
- cmd-f should work on OSX to search in helpbrowser [\#206](https://github.com/supercollider/supercollider/issues/206)
- cmd-d should open links in new Window if selected  [\#205](https://github.com/supercollider/supercollider/issues/205)
- Proposal for Helpbrowser layout [\#204](https://github.com/supercollider/supercollider/issues/204)
- a nonpositive number in a Dser ends the server: [\#203](https://github.com/supercollider/supercollider/issues/203)
- b\_allocReadChannel reads only 1024 samples [\#202](https://github.com/supercollider/supercollider/issues/202)
- \[linux\] internal server doesn't receive OSC - unresponsive [\#201](https://github.com/supercollider/supercollider/issues/201)
- Internal server \(sometimes?\) fails to connect to CoreAudio [\#200](https://github.com/supercollider/supercollider/issues/200)
- Peak broken on OS X 64-bit [\#199](https://github.com/supercollider/supercollider/issues/199)
- b\_allocReadChannel crashes supernova [\#198](https://github.com/supercollider/supercollider/issues/198)
- LFPar and LFCub return silence with ar rate as input [\#197](https://github.com/supercollider/supercollider/issues/197)
- pow wierdness [\#196](https://github.com/supercollider/supercollider/issues/196)
- DragSink [\#194](https://github.com/supercollider/supercollider/issues/194)
- GEdit plugin doesn't work in GEdit 3 [\#193](https://github.com/supercollider/supercollider/issues/193)
- Inappropriate writing inside "Program Files" causes problems [\#192](https://github.com/supercollider/supercollider/issues/192)
- Strange behaviour with Demand ugen at kr [\#191](https://github.com/supercollider/supercollider/issues/191)
- Dswitch repeats source patterns' ending values [\#190](https://github.com/supercollider/supercollider/issues/190)
- \[SC.app\] DeferredDelete events are not processed [\#188](https://github.com/supercollider/supercollider/issues/188)
- Error with help system using Gedit \(help doc cache issue\) [\#187](https://github.com/supercollider/supercollider/issues/187)
- yNjV79  \<a href="http://midjepicgais.com/"\>midjepicgais\</a\>, [\#185](https://github.com/supercollider/supercollider/issues/185)
- QSoundFileView read family: missing arguments [\#184](https://github.com/supercollider/supercollider/issues/184)
- Class browser crashes sc [\#183](https://github.com/supercollider/supercollider/issues/183)
- \[OS X\] QView, QWindow: .remove resp. .close don't work [\#182](https://github.com/supercollider/supercollider/issues/182)
- HelpBrowser find-box does not work correctly [\#180](https://github.com/supercollider/supercollider/issues/180)
- Help browser line execution doesn't always work [\#179](https://github.com/supercollider/supercollider/issues/179)
- Find field is misleading [\#178](https://github.com/supercollider/supercollider/issues/178)
- s.meter is broken after reboot [\#177](https://github.com/supercollider/supercollider/issues/177)
- QFont size does not allow floats [\#176](https://github.com/supercollider/supercollider/issues/176)
- \[OS X\] setting QtGUI.style does not work first time [\#175](https://github.com/supercollider/supercollider/issues/175)
- Selection of code within brackets [\#174](https://github.com/supercollider/supercollider/issues/174)
- QWebView - process selection before eval [\#173](https://github.com/supercollider/supercollider/issues/173)
- SCWebView - process selection before eval [\#172](https://github.com/supercollider/supercollider/issues/172)
- SCDoc - convert remaining helpfiles [\#171](https://github.com/supercollider/supercollider/issues/171)
- SCWebView copy plain text [\#170](https://github.com/supercollider/supercollider/issues/170)
- some shortcuts does not work in SCWebView [\#169](https://github.com/supercollider/supercollider/issues/169)
- Qt linking [\#168](https://github.com/supercollider/supercollider/issues/168)
- \[OS X\] HelpBrowser with Qt doesn't display text [\#167](https://github.com/supercollider/supercollider/issues/167)
- SC.app: Qt vs Cocoa windows Z order [\#166](https://github.com/supercollider/supercollider/issues/166)
- SC.app: class-lib recompilation leaves unresponsive windows [\#165](https://github.com/supercollider/supercollider/issues/165)
- SC.app: QWebView bug on false URL [\#164](https://github.com/supercollider/supercollider/issues/164)
- Dictionary putPairs [\#163](https://github.com/supercollider/supercollider/issues/163)
- right-click in Qt-help-browser crashes SC.app \(OSX only?\) [\#162](https://github.com/supercollider/supercollider/issues/162)
- GeneralHID is still broken on osx 10.6.7 [\#161](https://github.com/supercollider/supercollider/issues/161)
- Resonz after DiskOut = big BIG distortion [\#160](https://github.com/supercollider/supercollider/issues/160)
- SC.app crash when scrolling QListView [\#159](https://github.com/supercollider/supercollider/issues/159)
-  PopUpMenu problems in QtCollider under OSX [\#158](https://github.com/supercollider/supercollider/issues/158)
- QWebView a:hover underlined [\#157](https://github.com/supercollider/supercollider/issues/157)
- sclang does not start without X [\#156](https://github.com/supercollider/supercollider/issues/156)
- .wrap2 behavior [\#155](https://github.com/supercollider/supercollider/issues/155)
- SC.app hang in drawHook or drawFunc [\#154](https://github.com/supercollider/supercollider/issues/154)
- SC.app hangs when closing a Dialog [\#153](https://github.com/supercollider/supercollider/issues/153)
- \[Mac OS\] Button hit are too small [\#152](https://github.com/supercollider/supercollider/issues/152)
- SC.app crashes due to cmake's fixup\_bundle [\#151](https://github.com/supercollider/supercollider/issues/151)
- Strange display behavior in Help.gui [\#150](https://github.com/supercollider/supercollider/issues/150)
- Float error with QtCollider  [\#149](https://github.com/supercollider/supercollider/issues/149)
- Semaphore does not work propery [\#148](https://github.com/supercollider/supercollider/issues/148)
- Help files not found with prefix=/usr [\#145](https://github.com/supercollider/supercollider/issues/145)
- String.powerset dosent throw error and causes SC to crash [\#144](https://github.com/supercollider/supercollider/issues/144)
- Review patterns for old early termination [\#143](https://github.com/supercollider/supercollider/issues/143)
- DetectSilence may never detect silence [\#142](https://github.com/supercollider/supercollider/issues/142)
- Patch for minor 'links' bug in scvim\_make\_help [\#141](https://github.com/supercollider/supercollider/issues/141)
- scheduling infinite duration freezes SClang [\#140](https://github.com/supercollider/supercollider/issues/140)
- sc3-plugins-src archive is out of date on the website [\#139](https://github.com/supercollider/supercollider/issues/139)
- Demand ugens broken [\#138](https://github.com/supercollider/supercollider/issues/138)
- Demand broken for infinite-length input sequences [\#137](https://github.com/supercollider/supercollider/issues/137)
- /clearSched causes NRT Crash [\#136](https://github.com/supercollider/supercollider/issues/136)
- DemandEnvGen curve=0 is wrong [\#135](https://github.com/supercollider/supercollider/issues/135)
- Cocoa SCEnvelopeView crashes SC app. [\#134](https://github.com/supercollider/supercollider/issues/134)
- On the new 3.4RC [\#133](https://github.com/supercollider/supercollider/issues/133)
- loadRelative breaks on linux [\#131](https://github.com/supercollider/supercollider/issues/131)
- sclang parsing does not take bracket sematics into account [\#130](https://github.com/supercollider/supercollider/issues/130)
- sclang crashes routinely in prGetBackTrace -\> MakeDebugFrame [\#129](https://github.com/supercollider/supercollider/issues/129)
- Demand plugins built by scons on osx: bad repeats/le [\#128](https://github.com/supercollider/supercollider/issues/128)
- LFPar and LFCub audio rate modulation frequency argument bug [\#127](https://github.com/supercollider/supercollider/issues/127)
- Error in method .set 's "initAction" of EZSlider [\#126](https://github.com/supercollider/supercollider/issues/126)
- SimpleNumber:frac wrong for negative numbers [\#125](https://github.com/supercollider/supercollider/issues/125)
- SynthDef:writeDefFile dir-name trailing slash [\#124](https://github.com/supercollider/supercollider/issues/124)
- SoundFile.cue cuts off low-sample-rate sounds early [\#123](https://github.com/supercollider/supercollider/issues/123)
- Scroll View with Drawhook drawing errors [\#122](https://github.com/supercollider/supercollider/issues/122)
- Signal fft crash [\#121](https://github.com/supercollider/supercollider/issues/121)
- WaveTerrain Crash [\#120](https://github.com/supercollider/supercollider/issues/120)
- drawAtPoint primitive fails if Font not installed [\#119](https://github.com/supercollider/supercollider/issues/119)
- Some methods SimpleNumber now fail with floats [\#118](https://github.com/supercollider/supercollider/issues/118)
- BeatTrack2 unsafety [\#117](https://github.com/supercollider/supercollider/issues/117)
- SCUserView relativeOrigin = true is inefficient [\#116](https://github.com/supercollider/supercollider/issues/116)
- SCPen.fillRadialGradient is faulty [\#115](https://github.com/supercollider/supercollider/issues/115)
- Compilation failure on PPC/Linux [\#114](https://github.com/supercollider/supercollider/issues/114)
- OSCpathResponder broken, needs fix like OSCresponderNode [\#113](https://github.com/supercollider/supercollider/issues/113)
- TCP broken in sclang \(rev 9591 onwards?\) [\#112](https://github.com/supercollider/supercollider/issues/112)
- Buffer cueSoundFile does not update path var [\#111](https://github.com/supercollider/supercollider/issues/111)
- Incoming OSC messages are dropped when VM is busy [\#110](https://github.com/supercollider/supercollider/issues/110)
- Setting Document default font causes crashes [\#109](https://github.com/supercollider/supercollider/issues/109)
- SCUserView relativeOrigin=true outputs bitmap [\#108](https://github.com/supercollider/supercollider/issues/108)
- Stuck synths using Pdef and Pmono [\#107](https://github.com/supercollider/supercollider/issues/107)
- SuperCollider.app v3.3.1 crashes on launch \(OS X 10.6.2\) [\#106](https://github.com/supercollider/supercollider/issues/106)
- is /b\_gen asynchronous? [\#105](https://github.com/supercollider/supercollider/issues/105)
- /b\_zero crashes for illegal bufnum [\#104](https://github.com/supercollider/supercollider/issues/104)
- BufDelayN produces "clicks" if buffer size is not power of 2 [\#103](https://github.com/supercollider/supercollider/issues/103)
- Buffer docs:bad par. name for asWavetable \[3.3.1\] [\#102](https://github.com/supercollider/supercollider/issues/102)
- Bug o f SuperCollider 3.3 on windows  [\#100](https://github.com/supercollider/supercollider/issues/100)
- Bug o f SupperCollider 3.3 on windows  [\#99](https://github.com/supercollider/supercollider/issues/99)
- Printing Bug [\#98](https://github.com/supercollider/supercollider/issues/98)
- SuperCollider 3.3 win alpha6 sur Windows XP  [\#97](https://github.com/supercollider/supercollider/issues/97)
- SuperCollider 3.3 win alpha6 sur Windows XP  [\#96](https://github.com/supercollider/supercollider/issues/96)
- Memory leak in Convolution2 [\#95](https://github.com/supercollider/supercollider/issues/95)
- bug on Max os X tiger  intel  [\#94](https://github.com/supercollider/supercollider/issues/94)
- source-with-extras bundle: instructions don't work [\#93](https://github.com/supercollider/supercollider/issues/93)
- fullscreen windows draw in front of PopUpMenu [\#92](https://github.com/supercollider/supercollider/issues/92)
- Loudness doesn't understand LocalBuf [\#91](https://github.com/supercollider/supercollider/issues/91)
- Build fails [\#89](https://github.com/supercollider/supercollider/issues/89)
- scvim tries to install in absolute path [\#88](https://github.com/supercollider/supercollider/issues/88)
- SuperCollider Crash with Safari4 [\#87](https://github.com/supercollider/supercollider/issues/87)
- very large SimpleNumber:series can crash sclang [\#86](https://github.com/supercollider/supercollider/issues/86)
- OSCresponderNode SCUserView refresh crashes [\#85](https://github.com/supercollider/supercollider/issues/85)
- Cmd-R does not update the cmdLine before interpreting [\#83](https://github.com/supercollider/supercollider/issues/83)
- doneAction in RecordBuf doesn't work [\#82](https://github.com/supercollider/supercollider/issues/82)
- findHelpFile sometimes returns an empty string on Linux [\#81](https://github.com/supercollider/supercollider/issues/81)
- ServerBoot and ServerQuit are called at wrong time [\#80](https://github.com/supercollider/supercollider/issues/80)
- 3.3beta: MFCC fails to init outputs in ctor \(fix attached\) [\#79](https://github.com/supercollider/supercollider/issues/79)
- LocalBuf and {}.plot are incompatible \(3.3.beta\) [\#78](https://github.com/supercollider/supercollider/issues/78)
- OSC-dumping crashes sound [\#76](https://github.com/supercollider/supercollider/issues/76)
- SCScrollView relativeOrigin glitch [\#75](https://github.com/supercollider/supercollider/issues/75)
- allTuple returns out of bounds memory results sometimes [\#74](https://github.com/supercollider/supercollider/issues/74)
- SCRangeSlider bg draws wrong in relative origin parent [\#72](https://github.com/supercollider/supercollider/issues/72)
- Pen clipping bug [\#71](https://github.com/supercollider/supercollider/issues/71)
- Document.listener.alwaysOnTop\_\(true\) survives recompilation [\#70](https://github.com/supercollider/supercollider/issues/70)
- Routine / AppClock broken [\#68](https://github.com/supercollider/supercollider/issues/68)
- sclang crashes when compiling errorneous code [\#67](https://github.com/supercollider/supercollider/issues/67)
- OSCpathResponder is broken [\#66](https://github.com/supercollider/supercollider/issues/66)
- test [\#65](https://github.com/supercollider/supercollider/issues/65)
- MIDIIn.connect no longer works on Linux [\#64](https://github.com/supercollider/supercollider/issues/64)
- FlowView setting inital margin and gap broken [\#63](https://github.com/supercollider/supercollider/issues/63)
- findRegExp sometimes crashes SC - probable GC corruption [\#62](https://github.com/supercollider/supercollider/issues/62)
- Condition.wait breaks any AppClock routine [\#60](https://github.com/supercollider/supercollider/issues/60)
- Reproducable unmotivated sclang crash [\#59](https://github.com/supercollider/supercollider/issues/59)
- view.remove only works one at a time [\#58](https://github.com/supercollider/supercollider/issues/58)
- scsynth terminal printout rendezvous default [\#57](https://github.com/supercollider/supercollider/issues/57)
- b\_readChannel broken [\#56](https://github.com/supercollider/supercollider/issues/56)
- infinities sometimes render compilestrings wrong in Windows [\#55](https://github.com/supercollider/supercollider/issues/55)
- clip2 on Signal [\#54](https://github.com/supercollider/supercollider/issues/54)
- Advanced Find/Replace Syntax Highlight Issue [\#53](https://github.com/supercollider/supercollider/issues/53)
- OSX double-click bracket matching fails in Windows files [\#51](https://github.com/supercollider/supercollider/issues/51)
- Collection:removeAll does not catch repeated items [\#50](https://github.com/supercollider/supercollider/issues/50)
- \_BasicClipPut primitive doesn't work [\#49](https://github.com/supercollider/supercollider/issues/49)
- Fix takeAt documentation [\#48](https://github.com/supercollider/supercollider/issues/48)
- \[post-3.2\] Incomplete compilation warning posted [\#47](https://github.com/supercollider/supercollider/issues/47)
- String literals in functiondefs can get corrupted [\#46](https://github.com/supercollider/supercollider/issues/46)
- Midi SysEx Message Bug [\#45](https://github.com/supercollider/supercollider/issues/45)
- Timestamped OSC messages don't work in Psycollider 3.2b1 [\#44](https://github.com/supercollider/supercollider/issues/44)
- GrainBuf bug [\#43](https://github.com/supercollider/supercollider/issues/43)
- SC2DSlider problem in an absolute-bound compositeview [\#42](https://github.com/supercollider/supercollider/issues/42)
- Make non-timestamped messages execute on control block bound [\#41](https://github.com/supercollider/supercollider/issues/41)
- Document.new returns nil if SC is not the active application [\#40](https://github.com/supercollider/supercollider/issues/40)
- \[windows\] SoundFile info is wrong after openRead'ing a file [\#39](https://github.com/supercollider/supercollider/issues/39)
- \[windows\] buffer reading seems not to work in win2k [\#38](https://github.com/supercollider/supercollider/issues/38)
- SC\_SYNTHDEF\_DIR environment variable incorrectly read, win32 [\#37](https://github.com/supercollider/supercollider/issues/37)
- Pen: strokeOval ignores pen width [\#34](https://github.com/supercollider/supercollider/issues/34)
- fixing deprecated use of string constants [\#33](https://github.com/supercollider/supercollider/issues/33)
- scsynth.exe crashes upon quit after timestamped messages [\#32](https://github.com/supercollider/supercollider/issues/32)
- Pitch crashes with too high a median length [\#31](https://github.com/supercollider/supercollider/issues/31)
- Some array primitives crash the VM if fed an empty array [\#30](https://github.com/supercollider/supercollider/issues/30)
- 0.2.asFraction beachballs the lang  [\#29](https://github.com/supercollider/supercollider/issues/29)
- Server.bootSync optional argument [\#28](https://github.com/supercollider/supercollider/issues/28)
- sclang crash on invalid code [\#27](https://github.com/supercollider/supercollider/issues/27)
- Pen: bug with fillRect, fillOval and fillColor [\#26](https://github.com/supercollider/supercollider/issues/26)
- n\_free can crash the server [\#25](https://github.com/supercollider/supercollider/issues/25)
- SCTestView editable\_ [\#24](https://github.com/supercollider/supercollider/issues/24)
- Instr path variable is not populated [\#23](https://github.com/supercollider/supercollider/issues/23)
- Incorrect input/output device display on bootup [\#22](https://github.com/supercollider/supercollider/issues/22)
- .quark files saved as RTF? [\#21](https://github.com/supercollider/supercollider/issues/21)
- sclang memory usage [\#20](https://github.com/supercollider/supercollider/issues/20)
- Please ignore \(test 2\) [\#19](https://github.com/supercollider/supercollider/issues/19)
- Please ignore \(test item\) [\#18](https://github.com/supercollider/supercollider/issues/18)
- Audio dropout if starting UGens not used yet in session\(OSX\) [\#17](https://github.com/supercollider/supercollider/issues/17)
- Trouble finding libs when using PREFIX [\#16](https://github.com/supercollider/supercollider/issues/16)
- tiny doc bug [\#15](https://github.com/supercollider/supercollider/issues/15)
- server will not boot [\#14](https://github.com/supercollider/supercollider/issues/14)
- s\_new and synthdef names with hyphen under linux not working [\#13](https://github.com/supercollider/supercollider/issues/13)
- LPF fails due to NaN error when cutoff==0 [\#12](https://github.com/supercollider/supercollider/issues/12)
- exponentiation bug [\#11](https://github.com/supercollider/supercollider/issues/11)
- permission problem in synthdef [\#10](https://github.com/supercollider/supercollider/issues/10)
- SC folder rename introspection errors [\#9](https://github.com/supercollider/supercollider/issues/9)
- linux build error [\#8](https://github.com/supercollider/supercollider/issues/8)
- cannot build sc3 on linux [\#7](https://github.com/supercollider/supercollider/issues/7)
- cmd-line build fails \(needs -faltivec?\) [\#6](https://github.com/supercollider/supercollider/issues/6)
- views invisible on refresh if window too small [\#5](https://github.com/supercollider/supercollider/issues/5)
- SCSliderBase, knobColor bug [\#4](https://github.com/supercollider/supercollider/issues/4)
- SCRangeSlider dragging bug [\#3](https://github.com/supercollider/supercollider/issues/3)
- SCView.sc [\#2](https://github.com/supercollider/supercollider/issues/2)
- SCRangeSlider [\#1](https://github.com/supercollider/supercollider/issues/1)
- ide: fix issue with large documents [\#1849](https://github.com/supercollider/supercollider/pull/1849) ([miguel-negrao](https://github.com/miguel-negrao))
- Classlib: Update PauseStream stream's clock upon play [\#1711](https://github.com/supercollider/supercollider/pull/1711) ([jamshark70](https://github.com/jamshark70))
- Plugins: Clip NOVA functions should update unit-\> variables [\#1702](https://github.com/supercollider/supercollider/pull/1702) ([jamshark70](https://github.com/jamshark70))

**Closed issues:**

- copyRange vs. bracket ranges [\#1876](https://github.com/supercollider/supercollider/issues/1876)
- FloatArray prevents rounding [\#1873](https://github.com/supercollider/supercollider/issues/1873)
- Quark not properly checked-out and/or updated [\#1864](https://github.com/supercollider/supercollider/issues/1864)
- ScopeView broken in SuperCollider 3.7.0-beta1? [\#1862](https://github.com/supercollider/supercollider/issues/1862)
- sclang in master freezes on arm \(due to PR \#1677 / commit d0f475d\) [\#1842](https://github.com/supercollider/supercollider/issues/1842)
- Multiple schelp syntax errors [\#1837](https://github.com/supercollider/supercollider/issues/1837)
- scsynth: verbosity -1 & -2 not recognised - scsynth won't start [\#1825](https://github.com/supercollider/supercollider/issues/1825)
- Building master with gcc 4.7 fails due to 'is\_trivially\_destructible' in SC\_PlugIn.hpp [\#1820](https://github.com/supercollider/supercollider/issues/1820)
- compilation error on slackware linux  [\#1814](https://github.com/supercollider/supercollider/issues/1814)
- The Font help file has a syntax error [\#1811](https://github.com/supercollider/supercollider/issues/1811)
- Github's source code bundles are unhelpful for SuperCollider because submodules missing [\#1807](https://github.com/supercollider/supercollider/issues/1807)
- Error when saving with 3.7.0 beta1 [\#1806](https://github.com/supercollider/supercollider/issues/1806)
- cmake: if on ARM, automatically disable SSE and SSE2 flags [\#1802](https://github.com/supercollider/supercollider/issues/1802)
- sclang compilation error in Linux [\#1797](https://github.com/supercollider/supercollider/issues/1797)
- Confusing terminology in "02. First Steps" of Tutorial "Getting Started With SuperCollider" [\#1781](https://github.com/supercollider/supercollider/issues/1781)
- if duplicate quark folders exist in extensions ... [\#1767](https://github.com/supercollider/supercollider/issues/1767)
- Need updated Windows build instructions [\#1763](https://github.com/supercollider/supercollider/issues/1763)
- build: LFUGens.cpp requires nova\_simd code even if NOVA\_SIMD=off [\#1751](https://github.com/supercollider/supercollider/issues/1751)
- StkInst overloaded new and delete waiting supernova [\#1747](https://github.com/supercollider/supercollider/issues/1747)
- install instructions for ubuntu are out of date [\#1723](https://github.com/supercollider/supercollider/issues/1723)
- NodeProxy fails to detect containing audiorate \(irregularly\) [\#1672](https://github.com/supercollider/supercollider/issues/1672)
- Issues running with Portaudio \(without jack\) on linux [\#1658](https://github.com/supercollider/supercollider/issues/1658)
- OSCFunc with special recvPort can't be freed [\#1654](https://github.com/supercollider/supercollider/issues/1654)
- Pipe breaks if it receives lines longer than 1024 characters [\#1593](https://github.com/supercollider/supercollider/issues/1593)
- Multichannel recording does not work in Windows 7 [\#1560](https://github.com/supercollider/supercollider/issues/1560)
- concatenating large strings causes memory corruption [\#1543](https://github.com/supercollider/supercollider/issues/1543)
- Seems like interpreter \(or servers\) are not tracking osc messages to server correctly [\#1524](https://github.com/supercollider/supercollider/issues/1524)
- help: TempoClock set beats example is incorrect [\#1518](https://github.com/supercollider/supercollider/issues/1518)
- supernova was left out of the -v -V transition [\#1468](https://github.com/supercollider/supercollider/issues/1468)
- findRegexp does not compile escaped character sequences correctly [\#1411](https://github.com/supercollider/supercollider/issues/1411)
- blocksize and latency in scsynth and supernova [\#1314](https://github.com/supercollider/supercollider/issues/1314)
- EnvelopeView gridOn\_ doesn't work \(in Qt on MS Windows anyway\) [\#1284](https://github.com/supercollider/supercollider/issues/1284)
- minor bug in an IEnvGen example [\#1276](https://github.com/supercollider/supercollider/issues/1276)
- Signal.waveFill function parameters \(3\) don't match documentation \(2\) [\#1259](https://github.com/supercollider/supercollider/issues/1259)
- Env.step example is confusing [\#1143](https://github.com/supercollider/supercollider/issues/1143)
- \(unnecessary?\) warning "The HID scheme 'nil' is not installed" [\#1120](https://github.com/supercollider/supercollider/issues/1120)
- AudioStreamAddPropertyListener deprecated warning [\#1117](https://github.com/supercollider/supercollider/issues/1117)
- counter intuitive behaviour when embedding events/dictionaries in streams directly [\#1107](https://github.com/supercollider/supercollider/issues/1107)
- No implementation of "Preferences" in Cocoa application [\#1098](https://github.com/supercollider/supercollider/issues/1098)
- SplayAz behaviour doesn't match documentation [\#1059](https://github.com/supercollider/supercollider/issues/1059)
- Error in Help - Getting Started Tutorial Series - Section 2 [\#981](https://github.com/supercollider/supercollider/issues/981)
- SuperCollider won't build with Xcode 5 [\#958](https://github.com/supercollider/supercollider/issues/958)
- Crash in creating SynthDef [\#957](https://github.com/supercollider/supercollider/issues/957)
- supernova compilation error [\#951](https://github.com/supercollider/supercollider/issues/951)
- supercollider-gedit plugin not loading [\#950](https://github.com/supercollider/supercollider/issues/950)
- OSCFunc addr.hostname returns nil [\#933](https://github.com/supercollider/supercollider/issues/933)
- iOS build, doesn't. [\#927](https://github.com/supercollider/supercollider/issues/927)
- Disable mathjax - an unused feature that can cause problems [\#909](https://github.com/supercollider/supercollider/issues/909)
- DiskIn channel limitation [\#901](https://github.com/supercollider/supercollider/issues/901)
- sclang \(readline\) cannot quit [\#897](https://github.com/supercollider/supercollider/issues/897)
- PV\_Copy breaks edge model [\#896](https://github.com/supercollider/supercollider/issues/896)
- linux \(debian x64\) compilation error [\#894](https://github.com/supercollider/supercollider/issues/894)
- osx/wii uses deprecated/removed API [\#876](https://github.com/supercollider/supercollider/issues/876)
- drawFunc drawingEnabled\_\(false\) not respected by refresh [\#869](https://github.com/supercollider/supercollider/issues/869)
- file browser lacks default [\#868](https://github.com/supercollider/supercollider/issues/868)
- SerialPort Cleanup error [\#862](https://github.com/supercollider/supercollider/issues/862)
- Windows: preferably use native file selector on document open etc. [\#861](https://github.com/supercollider/supercollider/issues/861)
- Issue Tracker: user can't enter categories/lables [\#860](https://github.com/supercollider/supercollider/issues/860)
- Windows: curl support doesn't work and no alternative available [\#859](https://github.com/supercollider/supercollider/issues/859)
- Windows \(installer\): no program shortcuts [\#857](https://github.com/supercollider/supercollider/issues/857)
- Windows: installer does not register in system [\#856](https://github.com/supercollider/supercollider/issues/856)
- Windows: no file associations [\#855](https://github.com/supercollider/supercollider/issues/855)
- Server load percentage on Windows off by factor 100 [\#849](https://github.com/supercollider/supercollider/issues/849)
- SC IDE: Custom Shortcut ctrl-alt-\<arrow-key\> for Move Line Up/Down gets lost after quitting SC [\#836](https://github.com/supercollider/supercollider/issues/836)
- searched word is highlighted even after edit [\#833](https://github.com/supercollider/supercollider/issues/833)
- server-scope: zoom default value not set on SC 3.6.4 [\#825](https://github.com/supercollider/supercollider/issues/825)
- build error  [\#816](https://github.com/supercollider/supercollider/issues/816)
- Missing "\)" in HelpSource / Guides / MIDI.schelp [\#802](https://github.com/supercollider/supercollider/issues/802)
- Up-arrow after hitting return wrongly goes to the end of the previous line [\#800](https://github.com/supercollider/supercollider/issues/800)
- \[linux, gnome\] Tab-navigation in find/replace panel is invisible [\#797](https://github.com/supercollider/supercollider/issues/797)
- EnvelopeView and exponential, cubed and squared curves. [\#784](https://github.com/supercollider/supercollider/issues/784)
- Bug in Bus:get when using multichannel busses [\#778](https://github.com/supercollider/supercollider/issues/778)
- Scope won't show a channel count higher than 16 upon initialization [\#769](https://github.com/supercollider/supercollider/issues/769)
- FreqScope not updating correctly [\#764](https://github.com/supercollider/supercollider/issues/764)
- Supernova does not load UGens from ~/.local [\#763](https://github.com/supercollider/supercollider/issues/763)
- Convolution2: frameSize = 2\*\*15 \(32768\) fails [\#738](https://github.com/supercollider/supercollider/issues/738)
- Convolution2: kernel size is truncated to fftsize [\#737](https://github.com/supercollider/supercollider/issues/737)
- Convolution2: failure if frameSize \> numFrames, in some cases! [\#736](https://github.com/supercollider/supercollider/issues/736)
- sclang crash on quit - assertion failure [\#726](https://github.com/supercollider/supercollider/issues/726)
- Wrong results from something.kr / something.ar [\#721](https://github.com/supercollider/supercollider/issues/721)
- TreeView.clear works in IDE, but doesn't work in Standalone [\#719](https://github.com/supercollider/supercollider/issues/719)
- X11 crashes: \[xcb\] Unknown request in queue while dequeuing [\#717](https://github.com/supercollider/supercollider/issues/717)
- sclang -l configuration fails \(no warnings, no errors\) [\#713](https://github.com/supercollider/supercollider/issues/713)
- scsynth stops when running with RME Fireface UCX [\#701](https://github.com/supercollider/supercollider/issues/701)
- plot on a signal does not work \(3.6.1\) [\#698](https://github.com/supercollider/supercollider/issues/698)
- Document browser's "Tutorial" subsection should list all tutorials [\#693](https://github.com/supercollider/supercollider/issues/693)
- OpenBSD, runtime error: gNumClasses 'discrepancy' [\#684](https://github.com/supercollider/supercollider/issues/684)
- cmd-d in help document does not work [\#667](https://github.com/supercollider/supercollider/issues/667)
- SC 3.6beta3 - help docs don't compile [\#666](https://github.com/supercollider/supercollider/issues/666)
- 3.6.3 STANDARD plugin API mismatch [\#660](https://github.com/supercollider/supercollider/issues/660)
- Copy/paste from SC IDE to Mail loses formatting & tabs \(OS X\) [\#657](https://github.com/supercollider/supercollider/issues/657)
- scide: Ctrl+P does not focus Post Window [\#654](https://github.com/supercollider/supercollider/issues/654)
- Single channel NamedControl returns default values as one-item-array [\#652](https://github.com/supercollider/supercollider/issues/652)
- Cannot create new .sc class definition file from SC IDE [\#651](https://github.com/supercollider/supercollider/issues/651)
- \[SCIDE 3.6 beta 2\] Help pane browser pops up annoyingly on load  [\#643](https://github.com/supercollider/supercollider/issues/643)
- Env/event documentation error [\#638](https://github.com/supercollider/supercollider/issues/638)
- crash when evaluating exprand [\#632](https://github.com/supercollider/supercollider/issues/632)
- \[SC-IDE\] save/save as dialog extensions [\#615](https://github.com/supercollider/supercollider/issues/615)
- \[SC-IDE\] hang when trying to stop sclang [\#614](https://github.com/supercollider/supercollider/issues/614)
- Ndef clear/play sequence problem [\#612](https://github.com/supercollider/supercollider/issues/612)
- File-changed behavior is triggered while file is opened \(and closed\) in other applications [\#595](https://github.com/supercollider/supercollider/issues/595)
- Tab widths are radically different between post window and  [\#594](https://github.com/supercollider/supercollider/issues/594)
- Number views on Stethoscope are too narrow to be readable on OSX [\#583](https://github.com/supercollider/supercollider/issues/583)
- Line numbers do not sync for some fonts [\#567](https://github.com/supercollider/supercollider/issues/567)
- Duplicate keyboard shortcuts can be set, causing undefined behavior [\#566](https://github.com/supercollider/supercollider/issues/566)
- indent line or region sometimes doesn't work [\#528](https://github.com/supercollider/supercollider/issues/528)
- server meter menu option always localhost [\#525](https://github.com/supercollider/supercollider/issues/525)
- No sound after stopping and running Main again [\#520](https://github.com/supercollider/supercollider/issues/520)
- \[sc-ide\] Saving as .sc saves as .scd instead [\#519](https://github.com/supercollider/supercollider/issues/519)
- ~tildestuff is not treated as a unit for selection [\#508](https://github.com/supercollider/supercollider/issues/508)
- SCDoc: right-click on a link offers non-working "Open in new window" [\#504](https://github.com/supercollider/supercollider/issues/504)
- pressing ctrl+D with cursor just before bracket in "Latch\)" searches for bracket not Latch [\#503](https://github.com/supercollider/supercollider/issues/503)
- Cmd-Delete to have similar functionality to other OSX windows [\#501](https://github.com/supercollider/supercollider/issues/501)
- Ctrl-shift-I/U should not be separated from Ctrl-I/U in the menus [\#498](https://github.com/supercollider/supercollider/issues/498)
- Can't connect to and play sounds on already running scsynth [\#487](https://github.com/supercollider/supercollider/issues/487)
- Ctrl+. in helpfile doesn't stop it [\#480](https://github.com/supercollider/supercollider/issues/480)
- SC-IDE under Mac OSX: Open Class/Method Definition not working [\#479](https://github.com/supercollider/supercollider/issues/479)
- SC-IDE under Mac OSX: Open Class/Method Definition not working [\#478](https://github.com/supercollider/supercollider/issues/478)
- SuperCollider is renamed SuperColliderCocoa [\#473](https://github.com/supercollider/supercollider/issues/473)
- ide: simplifying find/replace? [\#468](https://github.com/supercollider/supercollider/issues/468)
- Copying from post window widget [\#460](https://github.com/supercollider/supercollider/issues/460)
- Enable word wrap in the post window [\#459](https://github.com/supercollider/supercollider/issues/459)
- keyboard shortcut for copy is disabled in post window [\#452](https://github.com/supercollider/supercollider/issues/452)
- Switching applications [\#449](https://github.com/supercollider/supercollider/issues/449)
- serialPort doneAction never called & port not properly closed? [\#438](https://github.com/supercollider/supercollider/issues/438)
- SC 3.5.4 crashes with buffers on windows 7 [\#424](https://github.com/supercollider/supercollider/issues/424)
- default for freqscope [\#421](https://github.com/supercollider/supercollider/issues/421)
- \[SC-IDE\] Evaluate a line within a region \(with the same shortcut\) [\#403](https://github.com/supercollider/supercollider/issues/403)
- Introspection thread safety [\#394](https://github.com/supercollider/supercollider/issues/394)
- Import RTF files [\#392](https://github.com/supercollider/supercollider/issues/392)
- Custom OSC Message processing section in OSC Communication doc's is incorrect [\#389](https://github.com/supercollider/supercollider/issues/389)
- font scaling behavior [\#379](https://github.com/supercollider/supercollider/issues/379)
- scide: arg tooltip for floats, display 1 not 1.000000 [\#377](https://github.com/supercollider/supercollider/issues/377)
- \[Qt IDE\] Tab not working [\#374](https://github.com/supercollider/supercollider/issues/374)
- sclang menu bar vs ide [\#367](https://github.com/supercollider/supercollider/issues/367)
- Buffer.free on 3.5.3 crash scsynth [\#359](https://github.com/supercollider/supercollider/issues/359)
- \[SC-IDE\] server status bar [\#348](https://github.com/supercollider/supercollider/issues/348)
- Can SFC\_SET\_CLIPPING fix the wrapped vs. clipped int-format recording problem? [\#345](https://github.com/supercollider/supercollider/issues/345)
- SynthDef compiler in Lua [\#344](https://github.com/supercollider/supercollider/issues/344)
- scsynth sound stops in 3.5 not in 3.4 [\#343](https://github.com/supercollider/supercollider/issues/343)
- Pseg duration in secs or beats? [\#340](https://github.com/supercollider/supercollider/issues/340)
- MIDIFunc.noteOn does not work with floats [\#325](https://github.com/supercollider/supercollider/issues/325)

**Merged pull requests:**

- fix: typo in CHANGELOG.md [\#1888](https://github.com/supercollider/supercollider/pull/1888) ([crucialfelix](https://github.com/crucialfelix))
- build: copy CHANGELOG.md not the old ChangeLog [\#1887](https://github.com/supercollider/supercollider/pull/1887) ([crucialfelix](https://github.com/crucialfelix))
- class library: poll treats numbers as signals [\#1885](https://github.com/supercollider/supercollider/pull/1885) ([telephon](https://github.com/telephon))
- class library display path in quark gui [\#1882](https://github.com/supercollider/supercollider/pull/1882) ([telephon](https://github.com/telephon))
- Topic/osx move scsynth [\#1881](https://github.com/supercollider/supercollider/pull/1881) ([crucialfelix](https://github.com/crucialfelix))
- fix \#1864 Quarks Git not detecting latest she correctly [\#1880](https://github.com/supercollider/supercollider/pull/1880) ([crucialfelix](https://github.com/crucialfelix))
- build: use direct test to decide if SSE available [\#1879](https://github.com/supercollider/supercollider/pull/1879) ([danstowell](https://github.com/danstowell))
- Improved documentation for copyRange and copySeries [\#1877](https://github.com/supercollider/supercollider/pull/1877) ([snappizz](https://github.com/snappizz))
- sclang: add explanation of standalone mode. [\#1872](https://github.com/supercollider/supercollider/pull/1872) ([miguel-negrao](https://github.com/miguel-negrao))
- Avoid misleading message for Xcode and Visual Studio [\#1871](https://github.com/supercollider/supercollider/pull/1871) ([bagong](https://github.com/bagong))
- ide: menu option "Show Quarks" [\#1867](https://github.com/supercollider/supercollider/pull/1867) ([miguel-negrao](https://github.com/miguel-negrao))
- ide: Add standalone option in settings. [\#1863](https://github.com/supercollider/supercollider/pull/1863) ([miguel-negrao](https://github.com/miguel-negrao))
- ide: fix memory leak [\#1859](https://github.com/supercollider/supercollider/pull/1859) ([miguel-negrao](https://github.com/miguel-negrao))
- plugins: allow Vibrato to be triggered [\#1858](https://github.com/supercollider/supercollider/pull/1858) ([sonoro1234](https://github.com/sonoro1234))
- 3.7win without IPC fix [\#1852](https://github.com/supercollider/supercollider/pull/1852) ([bagong](https://github.com/bagong))
- Topic fix allocations [\#1845](https://github.com/supercollider/supercollider/pull/1845) ([telephon](https://github.com/telephon))
- Fixes/for master [\#1844](https://github.com/supercollider/supercollider/pull/1844) ([timblechmann](https://github.com/timblechmann))
- build: bump GCC version requirement up from 4.7 to 4.8 [\#1839](https://github.com/supercollider/supercollider/pull/1839) ([danstowell](https://github.com/danstowell))
- Help source syntax fix [\#1838](https://github.com/supercollider/supercollider/pull/1838) ([gusano](https://github.com/gusano))
- build: fix PanUGens to build when NOVA\_SIMD=OFF [\#1834](https://github.com/supercollider/supercollider/pull/1834) ([danstowell](https://github.com/danstowell))
- Fix error message [\#1828](https://github.com/supercollider/supercollider/pull/1828) ([carlocapocasa](https://github.com/carlocapocasa))
- Server.sc: option for scsynth's verbosity now is "V" not "v". [\#1826](https://github.com/supercollider/supercollider/pull/1826) ([miczac](https://github.com/miczac))
- build: fix non-SSE2 compile of SC\_SndBuf.h \(Fixes \#1819\) [\#1821](https://github.com/supercollider/supercollider/pull/1821) ([danstowell](https://github.com/danstowell))
- cmake: disable SSE on ARM [\#1817](https://github.com/supercollider/supercollider/pull/1817) ([danstowell](https://github.com/danstowell))
- QView.sc: removes residue OS X Space after closing a fullscreen window.  [\#1816](https://github.com/supercollider/supercollider/pull/1816) ([miczac](https://github.com/miczac))
- build: fix an integer type mismatch that caused build fail for some [\#1815](https://github.com/supercollider/supercollider/pull/1815) ([danstowell](https://github.com/danstowell))
- fixes \#1811, also, allowing for more space for the last two examples [\#1813](https://github.com/supercollider/supercollider/pull/1813) ([miczac](https://github.com/miczac))
- cmake: avoid confusing "FATAL" messages if not a git checkout [\#1809](https://github.com/supercollider/supercollider/pull/1809) ([crucialfelix](https://github.com/crucialfelix))
- cmake: disable SSE on ARM [\#1803](https://github.com/supercollider/supercollider/pull/1803) ([danstowell](https://github.com/danstowell))
- plugins: fix plugin registration for sum3/sum4 [\#1799](https://github.com/supercollider/supercollider/pull/1799) ([timblechmann](https://github.com/timblechmann))
- Revert "subproject commit" which mistakenly downgraded nova dependencies [\#1795](https://github.com/supercollider/supercollider/pull/1795) ([danstowell](https://github.com/danstowell))
- class library: moved Model.sc out of GUI directory [\#1792](https://github.com/supercollider/supercollider/pull/1792) ([redFrik](https://github.com/redFrik))
- help guides: fix typo [\#1790](https://github.com/supercollider/supercollider/pull/1790) ([gusano](https://github.com/gusano))
- Incorporate these tutorial corrections [\#1787](https://github.com/supercollider/supercollider/pull/1787) ([afischli](https://github.com/afischli))
- FIx typo in tutorial on shortcut explanation to clear the post window [\#1784](https://github.com/supercollider/supercollider/pull/1784) ([afischli](https://github.com/afischli))
- scide: don't sync documents if language was not compiled [\#1780](https://github.com/supercollider/supercollider/pull/1780) ([gusano](https://github.com/gusano))
- Novacollider/cleanups [\#1779](https://github.com/supercollider/supercollider/pull/1779) ([timblechmann](https://github.com/timblechmann))
- supernova: tests - use thread and chrono [\#1778](https://github.com/supercollider/supercollider/pull/1778) ([timblechmann](https://github.com/timblechmann))
- Correcting a spelling of "Buffer" in Getting Started 13 [\#1777](https://github.com/supercollider/supercollider/pull/1777) ([meatballhat](https://github.com/meatballhat))
- plugin interface: avoid dependency on nova-tt [\#1776](https://github.com/supercollider/supercollider/pull/1776) ([timblechmann](https://github.com/timblechmann))
- Novacollider/cleanups [\#1775](https://github.com/supercollider/supercollider/pull/1775) ([timblechmann](https://github.com/timblechmann))
- nova-simd: fix lower boundary of exp approximation [\#1774](https://github.com/supercollider/supercollider/pull/1774) ([timblechmann](https://github.com/timblechmann))
- cmake: refuse to build if unrecognised AUDIOAPI [\#1773](https://github.com/supercollider/supercollider/pull/1773) ([danstowell](https://github.com/danstowell))
- jack audio driver: remove old unused codepath for jack\<0.100 [\#1772](https://github.com/supercollider/supercollider/pull/1772) ([danstowell](https://github.com/danstowell))
- Topic/boost 1.60 [\#1770](https://github.com/supercollider/supercollider/pull/1770) ([timblechmann](https://github.com/timblechmann))
- Novacollider/supernova refactoring [\#1769](https://github.com/supercollider/supercollider/pull/1769) ([timblechmann](https://github.com/timblechmann))
- external libraries: nova-simd [\#1768](https://github.com/supercollider/supercollider/pull/1768) ([timblechmann](https://github.com/timblechmann))
- supernova: tcp - catch exceptions when sending replies via tcp [\#1765](https://github.com/supercollider/supercollider/pull/1765) ([timblechmann](https://github.com/timblechmann))
- Refactor EnvirGui [\#1761](https://github.com/supercollider/supercollider/pull/1761) ([adcxyz](https://github.com/adcxyz))
- Fix string find [\#1759](https://github.com/supercollider/supercollider/pull/1759) ([muellmusik](https://github.com/muellmusik))
- scdoc: typo in Working\_with\_HID.schelp [\#1756](https://github.com/supercollider/supercollider/pull/1756) ([redFrik](https://github.com/redFrik))
- scdoc: typo in LID\_permissions.schelp [\#1755](https://github.com/supercollider/supercollider/pull/1755) ([redFrik](https://github.com/redFrik))
- scdoc: typo in HID\_permissions.schelp [\#1754](https://github.com/supercollider/supercollider/pull/1754) ([redFrik](https://github.com/redFrik))
- c++ interface: introduce templated registerUnit [\#1753](https://github.com/supercollider/supercollider/pull/1753) ([timblechmann](https://github.com/timblechmann))
- Topic/quarks git fixes [\#1752](https://github.com/supercollider/supercollider/pull/1752) ([crucialfelix](https://github.com/crucialfelix))
- Fixes/supernova unload plugins [\#1748](https://github.com/supercollider/supercollider/pull/1748) ([timblechmann](https://github.com/timblechmann))
- Novacollider/supernova refactoring [\#1745](https://github.com/supercollider/supercollider/pull/1745) ([timblechmann](https://github.com/timblechmann))
- Novacollider/prevent bad optimization [\#1744](https://github.com/supercollider/supercollider/pull/1744) ([timblechmann](https://github.com/timblechmann))
- plugin interface: provide numInputs/numOutputs as interface functions [\#1743](https://github.com/supercollider/supercollider/pull/1743) ([timblechmann](https://github.com/timblechmann))
- cleanup: detect apple via \_\_APPLE\_\_ [\#1741](https://github.com/supercollider/supercollider/pull/1741) ([timblechmann](https://github.com/timblechmann))
- build: modify trunc\(\) calls to compile on gcc 4.8.4 [\#1740](https://github.com/supercollider/supercollider/pull/1740) ([danstowell](https://github.com/danstowell))
- supernova: received\_packet - make implementation more robust [\#1739](https://github.com/supercollider/supercollider/pull/1739) ([timblechmann](https://github.com/timblechmann))
- Fixes/supernova handle out of memory [\#1733](https://github.com/supercollider/supercollider/pull/1733) ([timblechmann](https://github.com/timblechmann))
- Fix avoid calling nonexisting cleanup event type [\#1731](https://github.com/supercollider/supercollider/pull/1731) ([blacksound](https://github.com/blacksound))
- Fix obsolete server method call [\#1730](https://github.com/supercollider/supercollider/pull/1730) ([blacksound](https://github.com/blacksound))
- Consistent use of Event in example. [\#1729](https://github.com/supercollider/supercollider/pull/1729) ([blacksound](https://github.com/blacksound))
- example & help files: cleanup and streamlining of server handling [\#1728](https://github.com/supercollider/supercollider/pull/1728) ([miczac](https://github.com/miczac))
- example & help files: fix reference to default server  [\#1727](https://github.com/supercollider/supercollider/pull/1727) ([miczac](https://github.com/miczac))
- Simplify pspawner example [\#1726](https://github.com/supercollider/supercollider/pull/1726) ([blacksound](https://github.com/blacksound))
- typofix - make description match example code [\#1725](https://github.com/supercollider/supercollider/pull/1725) ([blacksound](https://github.com/blacksound))
- UGen-scope.sc: fixes method to determine default server [\#1724](https://github.com/supercollider/supercollider/pull/1724) ([miczac](https://github.com/miczac))
- Fix typo in NodeProxy roles help file [\#1722](https://github.com/supercollider/supercollider/pull/1722) ([blacksound](https://github.com/blacksound))
- ass Filters: warning in help-docs about filter frequencies close to zero [\#1721](https://github.com/supercollider/supercollider/pull/1721) ([miczac](https://github.com/miczac))
- Clean up classlib: consistent tabs and Egyptian braces [\#1714](https://github.com/supercollider/supercollider/pull/1714) ([crucialfelix](https://github.com/crucialfelix))
- fix \#1668 - make existsCaseSensitive work for directories [\#1713](https://github.com/supercollider/supercollider/pull/1713) ([crucialfelix](https://github.com/crucialfelix))
- fix \#1670 isPath and isAbsolutePath regexp for windows [\#1712](https://github.com/supercollider/supercollider/pull/1712) ([crucialfelix](https://github.com/crucialfelix))
- fixes for master [\#1710](https://github.com/supercollider/supercollider/pull/1710) ([timblechmann](https://github.com/timblechmann))
- supernova: silence clang warning [\#1709](https://github.com/supercollider/supercollider/pull/1709) ([timblechmann](https://github.com/timblechmann))
- sclang: file prim - remove unused carbon code [\#1708](https://github.com/supercollider/supercollider/pull/1708) ([timblechmann](https://github.com/timblechmann))
- fix issues found by clang's static analyzer [\#1707](https://github.com/supercollider/supercollider/pull/1707) ([timblechmann](https://github.com/timblechmann))
- class library: store floats with full precision [\#1701](https://github.com/supercollider/supercollider/pull/1701) ([telephon](https://github.com/telephon))
- docs: minor typo adjustment in Getting Started 11 [\#1698](https://github.com/supercollider/supercollider/pull/1698) ([meatballhat](https://github.com/meatballhat))
- modernize supernova [\#1697](https://github.com/supercollider/supercollider/pull/1697) ([timblechmann](https://github.com/timblechmann))
- docs: minor typo correction in Getting Started 10 [\#1694](https://github.com/supercollider/supercollider/pull/1694) ([meatballhat](https://github.com/meatballhat))
- sclang: slot - improve architecture detection [\#1693](https://github.com/supercollider/supercollider/pull/1693) ([timblechmann](https://github.com/timblechmann))
- scdoc/sclang lexers: use intptr\_t tof compatibility with LLP64 archtectures [\#1692](https://github.com/supercollider/supercollider/pull/1692) ([timblechmann](https://github.com/timblechmann))
- refactoring [\#1691](https://github.com/supercollider/supercollider/pull/1691) ([timblechmann](https://github.com/timblechmann))
- fix lost keyevents [\#1690](https://github.com/supercollider/supercollider/pull/1690) ([timblechmann](https://github.com/timblechmann))
- install help sources on linux [\#1689](https://github.com/supercollider/supercollider/pull/1689) ([timblechmann](https://github.com/timblechmann))
- cmake fixes [\#1688](https://github.com/supercollider/supercollider/pull/1688) ([timblechmann](https://github.com/timblechmann))
- class library: make control set atomic [\#1687](https://github.com/supercollider/supercollider/pull/1687) ([LFSaw](https://github.com/LFSaw))
- Primitive related cleanups [\#1683](https://github.com/supercollider/supercollider/pull/1683) ([muellmusik](https://github.com/muellmusik))
- fixed one broken link [\#1680](https://github.com/supercollider/supercollider/pull/1680) ([redFrik](https://github.com/redFrik))
- fixed two broken links [\#1679](https://github.com/supercollider/supercollider/pull/1679) ([redFrik](https://github.com/redFrik))
- scel: Fix mode-line update [\#1678](https://github.com/supercollider/supercollider/pull/1678) ([ptrv](https://github.com/ptrv))
- use c++17-style executors to compile class library [\#1677](https://github.com/supercollider/supercollider/pull/1677) ([timblechmann](https://github.com/timblechmann))
- supernova: guard gcc version check [\#1675](https://github.com/supercollider/supercollider/pull/1675) ([timblechmann](https://github.com/timblechmann))
- fixed two broken links [\#1673](https://github.com/supercollider/supercollider/pull/1673) ([redFrik](https://github.com/redFrik))
- Update 03-Start-Your-Engines.schelp [\#1667](https://github.com/supercollider/supercollider/pull/1667) ([danielmkarlsson](https://github.com/danielmkarlsson))
- Update 02-First-Steps.schelp [\#1666](https://github.com/supercollider/supercollider/pull/1666) ([danielmkarlsson](https://github.com/danielmkarlsson))
- Update 01-Introductory-Remarks.schelp [\#1665](https://github.com/supercollider/supercollider/pull/1665) ([danielmkarlsson](https://github.com/danielmkarlsson))
- Subclass searches [\#1663](https://github.com/supercollider/supercollider/pull/1663) ([jamshark70](https://github.com/jamshark70))
- scsynth: sends d\_removed from the right thread [\#1662](https://github.com/supercollider/supercollider/pull/1662) ([sonoro1234](https://github.com/sonoro1234))
- class library:  defer calls to ide [\#1661](https://github.com/supercollider/supercollider/pull/1661) ([telephon](https://github.com/telephon))
- EnvGen: fix cub nans [\#1660](https://github.com/supercollider/supercollider/pull/1660) ([sonoro1234](https://github.com/sonoro1234))
- scsynth: denormal handling in PortAudio and Jack [\#1659](https://github.com/supercollider/supercollider/pull/1659) ([sonoro1234](https://github.com/sonoro1234))
- QtCollider: TextView: Get code for enterInterpretsSelection from block\(\) [\#1652](https://github.com/supercollider/supercollider/pull/1652) ([jamshark70](https://github.com/jamshark70))
- some buildsystem fixes [\#1649](https://github.com/supercollider/supercollider/pull/1649) ([timblechmann](https://github.com/timblechmann))
- supernova: cpu\_time\_info compile fix [\#1647](https://github.com/supercollider/supercollider/pull/1647) ([timblechmann](https://github.com/timblechmann))
- scsynth:SC\_GraphDef.cpp correct d\_removed packet definition [\#1646](https://github.com/supercollider/supercollider/pull/1646) ([sonoro1234](https://github.com/sonoro1234))
- lang: prevent Signal.chebyFill from returning nan [\#1645](https://github.com/supercollider/supercollider/pull/1645) ([totalgee](https://github.com/totalgee))
- SequenceableCollection help: fix some erroneous links [\#1641](https://github.com/supercollider/supercollider/pull/1641) ([nuss](https://github.com/nuss))
- disallow newCopyArgs in Boolean [\#1638](https://github.com/supercollider/supercollider/pull/1638) ([miguel-negrao](https://github.com/miguel-negrao))
- Novacollider/freelist updates [\#1636](https://github.com/supercollider/supercollider/pull/1636) ([timblechmann](https://github.com/timblechmann))
- Non-characters pass ASCII code 0 to key actions [\#1632](https://github.com/supercollider/supercollider/pull/1632) ([jamshark70](https://github.com/jamshark70))
- plugin interface: fix compile failure with gcc-4.9 [\#1628](https://github.com/supercollider/supercollider/pull/1628) ([timblechmann](https://github.com/timblechmann))
- Classlib: SystemSynthDefs: Clean up temp defs properly on all platforms [\#1627](https://github.com/supercollider/supercollider/pull/1627) ([jamshark70](https://github.com/jamshark70))
- Gc fixes and doc final [\#1624](https://github.com/supercollider/supercollider/pull/1624) ([muellmusik](https://github.com/muellmusik))
- common: oscutils - correct printf format specifier for \(u\)int64\_t [\#1620](https://github.com/supercollider/supercollider/pull/1620) ([timblechmann](https://github.com/timblechmann))
- scide: fix float regexp [\#1619](https://github.com/supercollider/supercollider/pull/1619) ([timblechmann](https://github.com/timblechmann))
- supernova: some refactoring [\#1614](https://github.com/supercollider/supercollider/pull/1614) ([timblechmann](https://github.com/timblechmann))
- server: Assume requested SR was successfully set [\#1612](https://github.com/supercollider/supercollider/pull/1612) ([scztt](https://github.com/scztt))
- cmake: copy targets to ide app bundle in post-build steps [\#1608](https://github.com/supercollider/supercollider/pull/1608) ([timblechmann](https://github.com/timblechmann))
- Misc Quarks fixes [\#1606](https://github.com/supercollider/supercollider/pull/1606) ([crucialfelix](https://github.com/crucialfelix))
- sclang: fix gc-related bug [\#1605](https://github.com/supercollider/supercollider/pull/1605) ([timblechmann](https://github.com/timblechmann))
- Quarks: avoid a stack overflow from LanguageConfig.includePaths [\#1603](https://github.com/supercollider/supercollider/pull/1603) ([crucialfelix](https://github.com/crucialfelix))
- fix: Quarks.load quark set [\#1602](https://github.com/supercollider/supercollider/pull/1602) ([crucialfelix](https://github.com/crucialfelix))
- Topic/ide fixes for master [\#1601](https://github.com/supercollider/supercollider/pull/1601) ([timblechmann](https://github.com/timblechmann))
- Topic/ide keyevent fixes [\#1598](https://github.com/supercollider/supercollider/pull/1598) ([timblechmann](https://github.com/timblechmann))
- scide: preferences dialog - fix width of icon list widget [\#1597](https://github.com/supercollider/supercollider/pull/1597) ([timblechmann](https://github.com/timblechmann))
- Topic/fixes for master [\#1595](https://github.com/supercollider/supercollider/pull/1595) ([timblechmann](https://github.com/timblechmann))
- qtcollider/sc-ide: make use of QStringLiteral [\#1594](https://github.com/supercollider/supercollider/pull/1594) ([timblechmann](https://github.com/timblechmann))
- Topic/fixes for master [\#1592](https://github.com/supercollider/supercollider/pull/1592) ([timblechmann](https://github.com/timblechmann))
- Fix: \#1475 absolute path detection and conversion for windows [\#1590](https://github.com/supercollider/supercollider/pull/1590) ([crucialfelix](https://github.com/crucialfelix))
- Adding a note about W64, CAF and WAV support as proposed by @scztt [\#1584](https://github.com/supercollider/supercollider/pull/1584) ([gogobd](https://github.com/gogobd))
- Topic/fixes for master [\#1583](https://github.com/supercollider/supercollider/pull/1583) ([timblechmann](https://github.com/timblechmann))
- Classlib: Event: Add missing ~latency into ~schedBundleArray calls [\#1582](https://github.com/supercollider/supercollider/pull/1582) ([jamshark70](https://github.com/jamshark70))
- OSX: Allow cmake install prefix to be overridden [\#1576](https://github.com/supercollider/supercollider/pull/1576) ([bagong](https://github.com/bagong))
- HID final cleanup, and LID adaption to use similar API [\#1573](https://github.com/supercollider/supercollider/pull/1573) ([sensestage](https://github.com/sensestage))
- Midi connect all [\#1571](https://github.com/supercollider/supercollider/pull/1571) ([sensestage](https://github.com/sensestage))
- OSX: return to old default cmake install location ./Install [\#1569](https://github.com/supercollider/supercollider/pull/1569) ([bagong](https://github.com/bagong))
- class library: HID: fix else statement in findAvailable [\#1567](https://github.com/supercollider/supercollider/pull/1567) ([sensestage](https://github.com/sensestage))
- fix scope on internal, solves \#1527 [\#1552](https://github.com/supercollider/supercollider/pull/1552) ([adcxyz](https://github.com/adcxyz))
- add valueActionIfChanged to fix issue\#1460 [\#1551](https://github.com/supercollider/supercollider/pull/1551) ([adcxyz](https://github.com/adcxyz))
- scsynth: alow using malloc, realloc, free [\#1549](https://github.com/supercollider/supercollider/pull/1549) ([sonoro1234](https://github.com/sonoro1234))
- fix \#1476 stack corruption triggered by calling a git unixCmd [\#1542](https://github.com/supercollider/supercollider/pull/1542) ([crucialfelix](https://github.com/crucialfelix))
- quarks: protect during eval of isCompatible fix \#1463 [\#1541](https://github.com/supercollider/supercollider/pull/1541) ([crucialfelix](https://github.com/crucialfelix))
- validate that a refspec refers to an existing tag [\#1540](https://github.com/supercollider/supercollider/pull/1540) ([crucialfelix](https://github.com/crucialfelix))
- ide: Switch focus to help docklet on help request [\#1529](https://github.com/supercollider/supercollider/pull/1529) ([scztt](https://github.com/scztt))
- import boost-1.58 [\#1528](https://github.com/supercollider/supercollider/pull/1528) ([timblechmann](https://github.com/timblechmann))
- build: switch to alpha1 [\#1525](https://github.com/supercollider/supercollider/pull/1525) ([scztt](https://github.com/scztt))
- Topic/fix scrollbar hiding osx [\#1523](https://github.com/supercollider/supercollider/pull/1523) ([scztt](https://github.com/scztt))
- sclang: fix default fonts [\#1522](https://github.com/supercollider/supercollider/pull/1522) ([scztt](https://github.com/scztt))
- Chebyshev: zero-offset default change plus documentation updates [\#1520](https://github.com/supercollider/supercollider/pull/1520) ([totalgee](https://github.com/totalgee))
- Fix MIDIIn.connectAll on Linux to only connect to external sources and not internal ones [\#1517](https://github.com/supercollider/supercollider/pull/1517) ([sensestage](https://github.com/sensestage))
- sclang: Fix incorrect modifiers [\#1513](https://github.com/supercollider/supercollider/pull/1513) ([scztt](https://github.com/scztt))
- Update Ringz.schelp [\#1511](https://github.com/supercollider/supercollider/pull/1511) ([porres](https://github.com/porres))
- Update LFPar.schelp [\#1510](https://github.com/supercollider/supercollider/pull/1510) ([porres](https://github.com/porres))
- Update Resonz.schelp [\#1509](https://github.com/supercollider/supercollider/pull/1509) ([porres](https://github.com/porres))
- Update Ringz.schelp [\#1508](https://github.com/supercollider/supercollider/pull/1508) ([porres](https://github.com/porres))
- Update LFPar.schelp [\#1507](https://github.com/supercollider/supercollider/pull/1507) ([porres](https://github.com/porres))
- Chebyshev polynomials: various fixes \(issue \#1500\) [\#1504](https://github.com/supercollider/supercollider/pull/1504) ([totalgee](https://github.com/totalgee))
- plugins/LFUGens: generate symmetrical waveforms in LFPulse [\#1503](https://github.com/supercollider/supercollider/pull/1503) ([totalgee](https://github.com/totalgee))
- Timedll for supernova [\#1497](https://github.com/supercollider/supercollider/pull/1497) ([sonoro1234](https://github.com/sonoro1234))
- ide: Workaround for completion help view crash [\#1495](https://github.com/supercollider/supercollider/pull/1495) ([scztt](https://github.com/scztt))
- sclang: fix crash during SerialPort cleanup [\#1494](https://github.com/supercollider/supercollider/pull/1494) ([scztt](https://github.com/scztt))
- sclang: workaround webkit scrollbar hiding bug [\#1493](https://github.com/supercollider/supercollider/pull/1493) ([scztt](https://github.com/scztt))
- classlib: Don't pass unprintables in key events [\#1492](https://github.com/supercollider/supercollider/pull/1492) ([scztt](https://github.com/scztt))
- sclang: set max scroll according to req'd value [\#1491](https://github.com/supercollider/supercollider/pull/1491) ([scztt](https://github.com/scztt))
- ide: better newline behavior within brackets [\#1490](https://github.com/supercollider/supercollider/pull/1490) ([scztt](https://github.com/scztt))
- MIDI: make interpreting noteOnZeroAsNoteOff an option rather than a h… [\#1488](https://github.com/supercollider/supercollider/pull/1488) ([sensestage](https://github.com/sensestage))
- classlib: Use correct spec for x coord of edits [\#1486](https://github.com/supercollider/supercollider/pull/1486) ([scztt](https://github.com/scztt))
- Add two qt5 packages to dependencies list [\#1481](https://github.com/supercollider/supercollider/pull/1481) ([bagong](https://github.com/bagong))
- supernova: print version with -v and use -V for verbose [\#1470](https://github.com/supercollider/supercollider/pull/1470) ([sonoro1234](https://github.com/sonoro1234))
- Help: WritingClasses: Extension methods are like obj-c categories [\#1469](https://github.com/supercollider/supercollider/pull/1469) ([jamshark70](https://github.com/jamshark70))
- Pluginsunload2 [\#1467](https://github.com/supercollider/supercollider/pull/1467) ([sonoro1234](https://github.com/sonoro1234))
- remove obsolete Document autocomplete extensions [\#1461](https://github.com/supercollider/supercollider/pull/1461) ([crucialfelix](https://github.com/crucialfelix))
- Sclang: print version with -v [\#1459](https://github.com/supercollider/supercollider/pull/1459) ([gusano](https://github.com/gusano))
- Scsynth: print version with -v [\#1458](https://github.com/supercollider/supercollider/pull/1458) ([gusano](https://github.com/gusano))
- Topic/quarks window is path [\#1457](https://github.com/supercollider/supercollider/pull/1457) ([crucialfelix](https://github.com/crucialfelix))
- Classlib: lincurve: Prevent incorrect range clipping when curve ~= 0 [\#1441](https://github.com/supercollider/supercollider/pull/1441) ([jamshark70](https://github.com/jamshark70))
- lang: NetAddr: correctly disconnect tcp socket [\#1440](https://github.com/supercollider/supercollider/pull/1440) ([miguel-negrao](https://github.com/miguel-negrao))
- protect NetAddr-disconnectAll from failing during shutdown [\#1439](https://github.com/supercollider/supercollider/pull/1439) ([crucialfelix](https://github.com/crucialfelix))
- classlib: translate modifier keys on mac [\#1434](https://github.com/supercollider/supercollider/pull/1434) ([scztt](https://github.com/scztt))
- ide: fix crash on session switch [\#1433](https://github.com/supercollider/supercollider/pull/1433) ([scztt](https://github.com/scztt))
- classlib: Convert note/cc nums in array case [\#1432](https://github.com/supercollider/supercollider/pull/1432) ([scztt](https://github.com/scztt))
- qtcollider: defer setPropery if called inside drawFunc [\#1431](https://github.com/supercollider/supercollider/pull/1431) ([scztt](https://github.com/scztt))
- Fix: Install quark dependencies with refspecs [\#1427](https://github.com/supercollider/supercollider/pull/1427) ([crucialfelix](https://github.com/crucialfelix))
- classlib: note nums / cc channels must be integers [\#1425](https://github.com/supercollider/supercollider/pull/1425) ([scztt](https://github.com/scztt))
- FreqScope.sc: adapt to new fill property of scope. [\#1419](https://github.com/supercollider/supercollider/pull/1419) ([miczac](https://github.com/miczac))
- ide: hide toolbox after changing box focus [\#1418](https://github.com/supercollider/supercollider/pull/1418) ([scztt](https://github.com/scztt))
- lang: don't drop backslashes [\#1417](https://github.com/supercollider/supercollider/pull/1417) ([scztt](https://github.com/scztt))
- ide: track files opened in a session switch in the recent documents menu [\#1416](https://github.com/supercollider/supercollider/pull/1416) ([scztt](https://github.com/scztt))
- ide: do not save format settings for controls that are disabled. [\#1415](https://github.com/supercollider/supercollider/pull/1415) ([scztt](https://github.com/scztt))
- Quarks GUI tiny improvements [\#1414](https://github.com/supercollider/supercollider/pull/1414) ([gusano](https://github.com/gusano))
- scide: theme: introducing built-in "dark" theme [\#1410](https://github.com/supercollider/supercollider/pull/1410) ([vdonnefort](https://github.com/vdonnefort))
- \[Help\] add words "Fast Fourier Transform" to guide [\#1407](https://github.com/supercollider/supercollider/pull/1407) ([patrickdupuis](https://github.com/patrickdupuis))
- Topic/quarks gui enhancements [\#1394](https://github.com/supercollider/supercollider/pull/1394) ([crucialfelix](https://github.com/crucialfelix))
- Pattern.record method doesn't stop recording [\#1392](https://github.com/supercollider/supercollider/pull/1392) ([gurk](https://github.com/gurk))
- fixes \#1369 LanguageConfig.current returns "", should return nil [\#1388](https://github.com/supercollider/supercollider/pull/1388) ([crucialfelix](https://github.com/crucialfelix))
- class library: GridLayout - fix position when using spanning [\#1384](https://github.com/supercollider/supercollider/pull/1384) ([gusano](https://github.com/gusano))
- fix-DiskOut [\#1382](https://github.com/supercollider/supercollider/pull/1382) ([muellmusik](https://github.com/muellmusik))
- server GUI: fix initial volume button value [\#1372](https://github.com/supercollider/supercollider/pull/1372) ([gusano](https://github.com/gusano))
- ide: Fix triggering of doc modified signal [\#1371](https://github.com/supercollider/supercollider/pull/1371) ([scztt](https://github.com/scztt))
- Date.schelp: corrected peculiar format example [\#1368](https://github.com/supercollider/supercollider/pull/1368) ([miczac](https://github.com/miczac))
- Fix broken link in Ndef help file [\#1366](https://github.com/supercollider/supercollider/pull/1366) ([albertojgomez](https://github.com/albertojgomez))
- class library: collectCopy method implementation [\#1357](https://github.com/supercollider/supercollider/pull/1357) ([telephon](https://github.com/telephon))
- Revert "scide: DocumentSelectPopUp - convert to Popup to fix lost KeyRel... [\#1354](https://github.com/supercollider/supercollider/pull/1354) ([telephon](https://github.com/telephon))
- scide: DocumentSelectPopUp - convert to Popup to fix lost KeyRelease event [\#1352](https://github.com/supercollider/supercollider/pull/1352) ([timblechmann](https://github.com/timblechmann))
- cmake: use bundled yaml-cpp by default [\#1351](https://github.com/supercollider/supercollider/pull/1351) ([timblechmann](https://github.com/timblechmann))
- Topic/improve doc readability [\#1349](https://github.com/supercollider/supercollider/pull/1349) ([telephon](https://github.com/telephon))
- Classlib: Complex: Fix bugs in 'pow' method [\#1348](https://github.com/supercollider/supercollider/pull/1348) ([jamshark70](https://github.com/jamshark70))
- Topic/simplify gui documentation [\#1345](https://github.com/supercollider/supercollider/pull/1345) ([telephon](https://github.com/telephon))
- Tiny enhancements to README\_OSX.md [\#1344](https://github.com/supercollider/supercollider/pull/1344) ([bagong](https://github.com/bagong))
- Topic nodeproxy gui for numbers [\#1343](https://github.com/supercollider/supercollider/pull/1343) ([telephon](https://github.com/telephon))
- class library: FunctionList copies before iterating. [\#1341](https://github.com/supercollider/supercollider/pull/1341) ([telephon](https://github.com/telephon))
- Topic/level indicator [\#1338](https://github.com/supercollider/supercollider/pull/1338) ([scztt](https://github.com/scztt))
- Topic/ogl filled scope [\#1337](https://github.com/supercollider/supercollider/pull/1337) ([scztt](https://github.com/scztt))
- Topic/source preview [\#1336](https://github.com/supercollider/supercollider/pull/1336) ([scztt](https://github.com/scztt))
- Topic/qt5 tim all and cosmetics [\#1334](https://github.com/supercollider/supercollider/pull/1334) ([scztt](https://github.com/scztt))
- Scide/autocomplete help [\#1333](https://github.com/supercollider/supercollider/pull/1333) ([vdonnefort](https://github.com/vdonnefort))
- Topic language config current path [\#1327](https://github.com/supercollider/supercollider/pull/1327) ([telephon](https://github.com/telephon))
- supernova: fix endpoint handling for asynchronous commands [\#1326](https://github.com/supercollider/supercollider/pull/1326) ([timblechmann](https://github.com/timblechmann))
- qtcollider: avoid un-safe printf by using qWarning\(\) instead [\#1325](https://github.com/supercollider/supercollider/pull/1325) ([timblechmann](https://github.com/timblechmann))
- Sclang/print version [\#1324](https://github.com/supercollider/supercollider/pull/1324) ([blacksound](https://github.com/blacksound))
- scsynth: print version and exit with option -V [\#1322](https://github.com/supercollider/supercollider/pull/1322) ([blacksound](https://github.com/blacksound))
- supernova: use -Z in portaudio 0 hight \(negative\) low others desired har... [\#1318](https://github.com/supercollider/supercollider/pull/1318) ([sonoro1234](https://github.com/sonoro1234))
- server: GraphDef\_ReadVer1 now reads via ParamSpec\_ReadVer1 [\#1317](https://github.com/supercollider/supercollider/pull/1317) ([telephon](https://github.com/telephon))
- plugins: fix 10 trigger UGens that did not initialize ZOUT correcly [\#1311](https://github.com/supercollider/supercollider/pull/1311) ([jamshark70](https://github.com/jamshark70))
- Plugins: Demand: Add Dconst UGen, with schelp [\#1305](https://github.com/supercollider/supercollider/pull/1305) ([jamshark70](https://github.com/jamshark70))
- Scide/line number [\#1302](https://github.com/supercollider/supercollider/pull/1302) ([vdonnefort](https://github.com/vdonnefort))
- Topic/linxfade fix [\#1301](https://github.com/supercollider/supercollider/pull/1301) ([timblechmann](https://github.com/timblechmann))
- class library\(GUI\): rename arguments - \*new [\#1288](https://github.com/supercollider/supercollider/pull/1288) ([gurk](https://github.com/gurk))
- Topic/timing cleanup [\#1286](https://github.com/supercollider/supercollider/pull/1286) ([muellmusik](https://github.com/muellmusik))
- Topic/operators [\#1282](https://github.com/supercollider/supercollider/pull/1282) ([telephon](https://github.com/telephon))
- scide: add OSX delete word ctrl+w shortcut [\#1272](https://github.com/supercollider/supercollider/pull/1272) ([vdonnefort](https://github.com/vdonnefort))
- Topic/bounded controls: cleaned up branch ready. [\#1270](https://github.com/supercollider/supercollider/pull/1270) ([telephon](https://github.com/telephon))
- string: regex: rename firstRegexp [\#1269](https://github.com/supercollider/supercollider/pull/1269) ([sofakid](https://github.com/sofakid))
- scide: introducing restore function [\#1267](https://github.com/supercollider/supercollider/pull/1267) ([vdonnefort](https://github.com/vdonnefort))
- plugins: EnvGen - fix initialization of hold segment [\#1265](https://github.com/supercollider/supercollider/pull/1265) ([timblechmann](https://github.com/timblechmann))
- scide: add save-as-extension functionality [\#1264](https://github.com/supercollider/supercollider/pull/1264) ([timblechmann](https://github.com/timblechmann))
- cmake: externals - don't scare users about auto\_ptr [\#1263](https://github.com/supercollider/supercollider/pull/1263) ([timblechmann](https://github.com/timblechmann))
- Update Tdef.schelp [\#1262](https://github.com/supercollider/supercollider/pull/1262) ([vividsnow](https://github.com/vividsnow))
- class library: translations between key value pairs, asscociations, and dictionaries [\#1260](https://github.com/supercollider/supercollider/pull/1260) ([telephon](https://github.com/telephon))
- Apple build: Prevent supernova from being installed twice [\#1256](https://github.com/supercollider/supercollider/pull/1256) ([bagong](https://github.com/bagong))
- plugins: Linen checks for prehistoric release message [\#1255](https://github.com/supercollider/supercollider/pull/1255) ([telephon](https://github.com/telephon))
- Topic/boost 1.57 [\#1249](https://github.com/supercollider/supercollider/pull/1249) ([timblechmann](https://github.com/timblechmann))
- string: Fix regex cache init, implement firstRegex [\#1248](https://github.com/supercollider/supercollider/pull/1248) ([sofakid](https://github.com/sofakid))
- Orthographical adjustment to win installer script [\#1244](https://github.com/supercollider/supercollider/pull/1244) ([bagong](https://github.com/bagong))
- IFFT.schelp: fix example [\#1243](https://github.com/supercollider/supercollider/pull/1243) ([miczac](https://github.com/miczac))
- sclang: hidapi - fix gc corruption bug [\#1242](https://github.com/supercollider/supercollider/pull/1242) ([timblechmann](https://github.com/timblechmann))
- Classlib: Pstep: Fix unnecessary creation of an array on every iteration [\#1236](https://github.com/supercollider/supercollider/pull/1236) ([jamshark70](https://github.com/jamshark70))
- Classlib: QPenPrinter: Add 'bounds' method [\#1235](https://github.com/supercollider/supercollider/pull/1235) ([jamshark70](https://github.com/jamshark70))
- Classlib: Pfuncn: Like Pfunc, this should call processRest before yield [\#1234](https://github.com/supercollider/supercollider/pull/1234) ([jamshark70](https://github.com/jamshark70))
- Classlib: Rest: Add 'value' method to simplify mixing rests and numbers [\#1233](https://github.com/supercollider/supercollider/pull/1233) ([jamshark70](https://github.com/jamshark70))
- Topic/qt5 win [\#1229](https://github.com/supercollider/supercollider/pull/1229) ([timblechmann](https://github.com/timblechmann))
- cmake: fix & clean up xcode project [\#1225](https://github.com/supercollider/supercollider/pull/1225) ([timblechmann](https://github.com/timblechmann))
- Fix "QNetworkRequest header not found". [\#1223](https://github.com/supercollider/supercollider/pull/1223) ([ventosus](https://github.com/ventosus))
- fix help installation [\#1222](https://github.com/supercollider/supercollider/pull/1222) ([nuss](https://github.com/nuss))
- supernova: /notify - return client id [\#1221](https://github.com/supercollider/supercollider/pull/1221) ([timblechmann](https://github.com/timblechmann))
- tcp server, retry a couple of times until getting a tcp connection [\#1215](https://github.com/supercollider/supercollider/pull/1215) ([miguel-negrao](https://github.com/miguel-negrao))
- lang: Fix TCP bugs [\#1214](https://github.com/supercollider/supercollider/pull/1214) ([muellmusik](https://github.com/muellmusik))
- More document stuff [\#1213](https://github.com/supercollider/supercollider/pull/1213) ([muellmusik](https://github.com/muellmusik))
- rd/tgrains \(& localbuf\) [\#1207](https://github.com/supercollider/supercollider/pull/1207) ([rd--](https://github.com/rd--))
- Blocksize-\>BlockSize [\#1206](https://github.com/supercollider/supercollider/pull/1206) ([rd--](https://github.com/rd--))
- scel: highlight uppercase symbols in emacs [\#1205](https://github.com/supercollider/supercollider/pull/1205) ([bion](https://github.com/bion))
- HID\_API fix: initClass -\> initClassTree [\#1203](https://github.com/supercollider/supercollider/pull/1203) ([andersvi](https://github.com/andersvi))
- Adding drawImage example to Pen.schelp [\#1197](https://github.com/supercollider/supercollider/pull/1197) ([thormagnusson](https://github.com/thormagnusson))
- Create Image.schelp [\#1195](https://github.com/supercollider/supercollider/pull/1195) ([thormagnusson](https://github.com/thormagnusson))
- Update win32\_api.hpp [\#1192](https://github.com/supercollider/supercollider/pull/1192) ([bagong](https://github.com/bagong))
- Topic/boost updates [\#1191](https://github.com/supercollider/supercollider/pull/1191) ([timblechmann](https://github.com/timblechmann))
- cmake/apple: compile targets directly into the app bundle [\#1189](https://github.com/supercollider/supercollider/pull/1189) ([timblechmann](https://github.com/timblechmann))
- supernova: don't scare osx users by consumer-os [\#1187](https://github.com/supercollider/supercollider/pull/1187) ([timblechmann](https://github.com/timblechmann))
- fix assign path [\#1186](https://github.com/supercollider/supercollider/pull/1186) ([carlocapocasa](https://github.com/carlocapocasa))
- scide: mark current session in dialogs [\#1184](https://github.com/supercollider/supercollider/pull/1184) ([timblechmann](https://github.com/timblechmann))
- scide: fix preferences action name shortcut for osx menu placement [\#1183](https://github.com/supercollider/supercollider/pull/1183) ([timblechmann](https://github.com/timblechmann))
- cmake: fix typo [\#1182](https://github.com/supercollider/supercollider/pull/1182) ([timblechmann](https://github.com/timblechmann))
- scide: key up after line evaluation should go to original position [\#1178](https://github.com/supercollider/supercollider/pull/1178) ([timblechmann](https://github.com/timblechmann))
- cmake: link sclang with tlsf [\#1177](https://github.com/supercollider/supercollider/pull/1177) ([timblechmann](https://github.com/timblechmann))
- cmake: install help on non-apple systems [\#1175](https://github.com/supercollider/supercollider/pull/1175) ([timblechmann](https://github.com/timblechmann))
- Topic/scide enhancements [\#1172](https://github.com/supercollider/supercollider/pull/1172) ([timblechmann](https://github.com/timblechmann))
- boost updates & cmake modernization [\#1166](https://github.com/supercollider/supercollider/pull/1166) ([timblechmann](https://github.com/timblechmann))
- Fix Scale.newFromKey with tuning argument [\#1161](https://github.com/supercollider/supercollider/pull/1161) ([slpopejoy](https://github.com/slpopejoy))
- Update Date.schelp [\#1151](https://github.com/supercollider/supercollider/pull/1151) ([thormagnusson](https://github.com/thormagnusson))
- ide: introducing themes management [\#1150](https://github.com/supercollider/supercollider/pull/1150) ([vdonnefort](https://github.com/vdonnefort))
- help: chaotic generators warnings removed [\#1149](https://github.com/supercollider/supercollider/pull/1149) ([smrg-lm](https://github.com/smrg-lm))
- Topic/coremidi crash fix [\#1147](https://github.com/supercollider/supercollider/pull/1147) ([scztt](https://github.com/scztt))
- plugins: TrigControl behaves like Control. Fixes \#1145 [\#1146](https://github.com/supercollider/supercollider/pull/1146) ([telephon](https://github.com/telephon))
- Classlib: PlotView: Don't post spec in calcSpecs [\#1135](https://github.com/supercollider/supercollider/pull/1135) ([jamshark70](https://github.com/jamshark70))
- mergeCharFormat instead of setCharFormat, so existing format info isn't ... [\#1134](https://github.com/supercollider/supercollider/pull/1134) ([scztt](https://github.com/scztt))
- Colorize lines in the post window. [\#1131](https://github.com/supercollider/supercollider/pull/1131) ([scztt](https://github.com/scztt))
- Allow the addReplace action to replace existing nodes while keeping the ... [\#1130](https://github.com/supercollider/supercollider/pull/1130) ([scztt](https://github.com/scztt))
- Show line number \(v3\) [\#1129](https://github.com/supercollider/supercollider/pull/1129) ([vdonnefort](https://github.com/vdonnefort))
- supernova: ensure that daz/ftz are set in all audio threads [\#1127](https://github.com/supercollider/supercollider/pull/1127) ([timblechmann](https://github.com/timblechmann))
- File.schelp: changed pathnames to fully qualified [\#1126](https://github.com/supercollider/supercollider/pull/1126) ([miczac](https://github.com/miczac))
- some supernova fixes [\#1123](https://github.com/supercollider/supercollider/pull/1123) ([timblechmann](https://github.com/timblechmann))
- ide: add show/hide line number feature [\#1121](https://github.com/supercollider/supercollider/pull/1121) ([vdonnefort](https://github.com/vdonnefort))
- scsynth coreaudio fixes [\#1118](https://github.com/supercollider/supercollider/pull/1118) ([gurk](https://github.com/gurk))
- Topic/supernova tcp [\#1109](https://github.com/supercollider/supercollider/pull/1109) ([timblechmann](https://github.com/timblechmann))
- sclang: fix accidental number literal [\#1105](https://github.com/supercollider/supercollider/pull/1105) ([mohayonao](https://github.com/mohayonao))
- sclang: fix float radix with pi [\#1104](https://github.com/supercollider/supercollider/pull/1104) ([mohayonao](https://github.com/mohayonao))
- server: mapped audio bus for /g\_queryTree.reply [\#1103](https://github.com/supercollider/supercollider/pull/1103) ([8c6794b6](https://github.com/8c6794b6))
- Initialize openPorts variable before startup.scd file is executed. [\#1102](https://github.com/supercollider/supercollider/pull/1102) ([marierm](https://github.com/marierm))
- Fix ctrl-w [\#1101](https://github.com/supercollider/supercollider/pull/1101) ([muellmusik](https://github.com/muellmusik))
- supernova: protect synth controls [\#1095](https://github.com/supercollider/supercollider/pull/1095) ([timblechmann](https://github.com/timblechmann))
- update readline version from homebrew in os x readme [\#1093](https://github.com/supercollider/supercollider/pull/1093) ([seansay](https://github.com/seansay))
- HID fix cleaned up [\#1092](https://github.com/supercollider/supercollider/pull/1092) ([sensestage](https://github.com/sensestage))
- ClassLib: SimpleNumber: biexp return calculated value [\#1089](https://github.com/supercollider/supercollider/pull/1089) ([mohayonao](https://github.com/mohayonao))
- Netbsd compatibility [\#1088](https://github.com/supercollider/supercollider/pull/1088) ([danstowell](https://github.com/danstowell))
- Tiny correction [\#1080](https://github.com/supercollider/supercollider/pull/1080) ([arnaldorusso](https://github.com/arnaldorusso))
- Classlib: SystemSynthDefs: Remove postln that shouldn't have been committed [\#1076](https://github.com/supercollider/supercollider/pull/1076) ([jamshark70](https://github.com/jamshark70))
- Classlib: Server.sc: Prevent an inadvertent "non-inlined function" warning [\#1075](https://github.com/supercollider/supercollider/pull/1075) ([jamshark70](https://github.com/jamshark70))
- cmake: disable some msvc warnings [\#1074](https://github.com/supercollider/supercollider/pull/1074) ([timblechmann](https://github.com/timblechmann))
- Classlib: GUI: Reinstate proper functioning of ObjectGui system [\#1073](https://github.com/supercollider/supercollider/pull/1073) ([marierm](https://github.com/marierm))
- lang: include missing header [\#1072](https://github.com/supercollider/supercollider/pull/1072) ([timblechmann](https://github.com/timblechmann))
- class library: single lag value in NamedControl shouldn't result in an a... [\#1071](https://github.com/supercollider/supercollider/pull/1071) ([miguel-negrao](https://github.com/miguel-negrao))
- sclang: deepfreeze - do not freeze immutable / permanent objects [\#1069](https://github.com/supercollider/supercollider/pull/1069) ([timblechmann](https://github.com/timblechmann))
- DreamHouse.scd: replaced code on request by author [\#1067](https://github.com/supercollider/supercollider/pull/1067) ([miczac](https://github.com/miczac))
- supernova: make apple clang happy [\#1066](https://github.com/supercollider/supercollider/pull/1066) ([timblechmann](https://github.com/timblechmann))
- Help: Fix envelope handling in a pattern-cookbook example [\#1061](https://github.com/supercollider/supercollider/pull/1061) ([jamshark70](https://github.com/jamshark70))
- Fix NRT file paths [\#1060](https://github.com/supercollider/supercollider/pull/1060) ([jamshark70](https://github.com/jamshark70))
- lang: Fix memory corruption bug introduced in commit ac613331d5062bcd1ae... [\#1054](https://github.com/supercollider/supercollider/pull/1054) ([muellmusik](https://github.com/muellmusik))
- syntax highlight and allow running code from schelp files [\#1052](https://github.com/supercollider/supercollider/pull/1052) ([miguel-negrao](https://github.com/miguel-negrao))
- Classlib: fixed PatternProxy constrainStream so defaultQuant works [\#1043](https://github.com/supercollider/supercollider/pull/1043) ([d0kt0r0](https://github.com/d0kt0r0))
- Classlib: Change NetAddr's default port to nil [\#1040](https://github.com/supercollider/supercollider/pull/1040) ([jamshark70](https://github.com/jamshark70))
- class library: Env\#\*step easier creation of step envelopes [\#1039](https://github.com/supercollider/supercollider/pull/1039) ([miguel-negrao](https://github.com/miguel-negrao))
- sclang: Fixed bug where osc bundles were being chooped midway [\#1038](https://github.com/supercollider/supercollider/pull/1038) ([miguel-negrao](https://github.com/miguel-negrao))
- Small tweaks required for building libscsynth for Win 64-bit [\#1035](https://github.com/supercollider/supercollider/pull/1035) ([maedoc](https://github.com/maedoc))
- lang: capture elapsedTime\(\) when a packet is received, rather than just ... [\#1032](https://github.com/supercollider/supercollider/pull/1032) ([muellmusik](https://github.com/muellmusik))
- plugins: EnvGen - initialize level of initial \hold segments [\#1028](https://github.com/supercollider/supercollider/pull/1028) ([timblechmann](https://github.com/timblechmann))
- Update INSTALL [\#1025](https://github.com/supercollider/supercollider/pull/1025) ([jwakely](https://github.com/jwakely))
- plugins: EnvGen - update number of produced samples on audio-rate gates [\#1024](https://github.com/supercollider/supercollider/pull/1024) ([timblechmann](https://github.com/timblechmann))
- sclang: add terminal standalone option [\#1021](https://github.com/supercollider/supercollider/pull/1021) ([miguel-negrao](https://github.com/miguel-negrao))
- Server multi client [\#1019](https://github.com/supercollider/supercollider/pull/1019) ([muellmusik](https://github.com/muellmusik))
- sc class library: events: allocWrite type [\#1017](https://github.com/supercollider/supercollider/pull/1017) ([vividsnow](https://github.com/vividsnow))
- fix filepath typo in os x readme file [\#1015](https://github.com/supercollider/supercollider/pull/1015) ([seansay](https://github.com/seansay))
- sclang/identDictput: test if object is mutable before changing [\#1012](https://github.com/supercollider/supercollider/pull/1012) ([mortuosplango](https://github.com/mortuosplango))
- Disable AppNap in QtCollider, sclang, and scsynth [\#1011](https://github.com/supercollider/supercollider/pull/1011) ([snickell](https://github.com/snickell))
- Minor enhancements after first rewrite [\#1003](https://github.com/supercollider/supercollider/pull/1003) ([bagong](https://github.com/bagong))
- scdoc: minor correction to Dshuf new argument - should be repeats and no... [\#1002](https://github.com/supercollider/supercollider/pull/1002) ([redFrik](https://github.com/redFrik))
- Rework OSX Readme [\#1000](https://github.com/supercollider/supercollider/pull/1000) ([bagong](https://github.com/bagong))
- Add matchLangIP method and primitive [\#998](https://github.com/supercollider/supercollider/pull/998) ([muellmusik](https://github.com/muellmusik))
- Fix open document [\#997](https://github.com/supercollider/supercollider/pull/997) ([muellmusik](https://github.com/muellmusik))
- Mac OS 10.9 SDK compatibility [\#994](https://github.com/supercollider/supercollider/pull/994) ([mortuosplango](https://github.com/mortuosplango))
- update to boost-1.55 [\#993](https://github.com/supercollider/supercollider/pull/993) ([timblechmann](https://github.com/timblechmann))
- scvim: Adding tmux support, fixing screen support, fixing sclang restart/start/kill \(also in terminal multiplexers\) [\#990](https://github.com/supercollider/supercollider/pull/990) ([dvzrv](https://github.com/dvzrv))
- boost: fix build error with recent versions of glibc [\#988](https://github.com/supercollider/supercollider/pull/988) ([gusano](https://github.com/gusano))
- cmake: disable SC\_WII by default [\#987](https://github.com/supercollider/supercollider/pull/987) ([gusano](https://github.com/gusano))
- external libs: bump nova-tt [\#983](https://github.com/supercollider/supercollider/pull/983) ([timblechmann](https://github.com/timblechmann))
- help: update phasor [\#978](https://github.com/supercollider/supercollider/pull/978) ([miguel-negrao](https://github.com/miguel-negrao))
- Topic/env step2 [\#975](https://github.com/supercollider/supercollider/pull/975) ([timblechmann](https://github.com/timblechmann))
- class library: allow NamedControl.new\(\) to return non-arrayed Lag \(fixing issue 973\) [\#974](https://github.com/supercollider/supercollider/pull/974) ([totalgee](https://github.com/totalgee))
- Topic/for master [\#971](https://github.com/supercollider/supercollider/pull/971) ([timblechmann](https://github.com/timblechmann))
- Ide editor improvements improvements [\#970](https://github.com/supercollider/supercollider/pull/970) ([muellmusik](https://github.com/muellmusik))
- lang: Add editable property to QWebView [\#969](https://github.com/supercollider/supercollider/pull/969) ([muellmusik](https://github.com/muellmusik))
- Topic/for master [\#963](https://github.com/supercollider/supercollider/pull/963) ([timblechmann](https://github.com/timblechmann))
- class library: speed improvements in a few places where bit operations are used. [\#962](https://github.com/supercollider/supercollider/pull/962) ([redFrik](https://github.com/redFrik))
- Topic/for master [\#953](https://github.com/supercollider/supercollider/pull/953) ([timblechmann](https://github.com/timblechmann))
- Scide document rework text mirror [\#948](https://github.com/supercollider/supercollider/pull/948) ([muellmusik](https://github.com/muellmusik))
- class library: pbind midi type - fix for sending sysex [\#947](https://github.com/supercollider/supercollider/pull/947) ([redFrik](https://github.com/redFrik))
- help: changed .send\(s\) for .add in SynthDef example [\#946](https://github.com/supercollider/supercollider/pull/946) ([brunoruviaro](https://github.com/brunoruviaro))
- Classlib: Fix 2 issues regarding Rest and patterns [\#941](https://github.com/supercollider/supercollider/pull/941) ([jamshark70](https://github.com/jamshark70))
- Topic/asio appclock [\#940](https://github.com/supercollider/supercollider/pull/940) ([timblechmann](https://github.com/timblechmann))
- class library: NamedControl - avoid lags if possible [\#938](https://github.com/supercollider/supercollider/pull/938) ([timblechmann](https://github.com/timblechmann))
- Documentation: Escape char in string literals, and thisProcess.nowExecutingPath [\#935](https://github.com/supercollider/supercollider/pull/935) ([jamshark70](https://github.com/jamshark70))
- Filter Help Docs: Warning about frequencies close to 0 [\#934](https://github.com/supercollider/supercollider/pull/934) ([miczac](https://github.com/miczac))
- Simplify and enhance Windows installer [\#929](https://github.com/supercollider/supercollider/pull/929) ([bagong](https://github.com/bagong))
- Library: Prevent inline-function warning in FFTUnpacking.sc [\#922](https://github.com/supercollider/supercollider/pull/922) ([jamshark70](https://github.com/jamshark70))
- Class Library: Object - performance improvements for Object.dup  [\#921](https://github.com/supercollider/supercollider/pull/921) ([thormagnusson](https://github.com/thormagnusson))
- linux readme: qt5 limitation [\#917](https://github.com/supercollider/supercollider/pull/917) ([miguel-negrao](https://github.com/miguel-negrao))
- scsynth: support for receiving nested OSC bundles. [\#914](https://github.com/supercollider/supercollider/pull/914) ([ventosus](https://github.com/ventosus))
- supernova: synthdef corruption - added synthef path to error message [\#912](https://github.com/supercollider/supercollider/pull/912) ([miguel-negrao](https://github.com/miguel-negrao))
- SCDoc: remove MathJax support [\#910](https://github.com/supercollider/supercollider/pull/910) ([gusano](https://github.com/gusano))
- implement sclang sockets via boost.asio & move ReplyAddress out of the public interface [\#903](https://github.com/supercollider/supercollider/pull/903) ([timblechmann](https://github.com/timblechmann))
- HelpSource: MIDIFunc: show how to free a MIDIFunc [\#900](https://github.com/supercollider/supercollider/pull/900) ([gusano](https://github.com/gusano))
- make sure GUI.skins is not nil in ProxyMixerOld [\#893](https://github.com/supercollider/supercollider/pull/893) ([redFrik](https://github.com/redFrik))
- scide: sc\_editor: include parenthesis in regionAroundCursor\(\) [\#892](https://github.com/supercollider/supercollider/pull/892) ([gusano](https://github.com/gusano))
- Pproto minor typo fix [\#891](https://github.com/supercollider/supercollider/pull/891) ([blacksound](https://github.com/blacksound))
- Topic/markdown for readme files [\#890](https://github.com/supercollider/supercollider/pull/890) ([gusano](https://github.com/gusano))
- sclang: support for receiving nested OSC bundles. [\#881](https://github.com/supercollider/supercollider/pull/881) ([ventosus](https://github.com/ventosus))
- sclang: support for more non-standard OSC types [\#878](https://github.com/supercollider/supercollider/pull/878) ([ventosus](https://github.com/ventosus))
- sclang bugfix: proper handling of Open Sound Control Blob Arguments [\#877](https://github.com/supercollider/supercollider/pull/877) ([ventosus](https://github.com/ventosus))
- Quarks: use new sf.net repo url [\#873](https://github.com/supercollider/supercollider/pull/873) ([gusano](https://github.com/gusano))
- classlib \(quarks\): Defer svn path checking until needed; try{} the check [\#865](https://github.com/supercollider/supercollider/pull/865) ([jamshark70](https://github.com/jamshark70))
- Ndef.schelp, Shaper.schelp, Wavetable.schelp [\#846](https://github.com/supercollider/supercollider/pull/846) ([miczac](https://github.com/miczac))
- Server : pings before considered dead [\#842](https://github.com/supercollider/supercollider/pull/842) ([miguel-negrao](https://github.com/miguel-negrao))
- remove bundled elisp library tree-widget.el [\#840](https://github.com/supercollider/supercollider/pull/840) ([tarsius](https://github.com/tarsius))
- scdoc: Pseg: duration pattern in beats not seconds [\#827](https://github.com/supercollider/supercollider/pull/827) ([vividsnow](https://github.com/vividsnow))
- new version of README.txt, corrections to help docs [\#822](https://github.com/supercollider/supercollider/pull/822) ([miczac](https://github.com/miczac))
- examples: ASA - simplify and cleanup [\#819](https://github.com/supercollider/supercollider/pull/819) ([gusano](https://github.com/gusano))
- updated README.txt [\#818](https://github.com/supercollider/supercollider/pull/818) ([miczac](https://github.com/miczac))
- thisFunction/thisFunctionDef keywords in help [\#809](https://github.com/supercollider/supercollider/pull/809) ([vividsnow](https://github.com/vividsnow))
- Update the description of LinPan2. [\#808](https://github.com/supercollider/supercollider/pull/808) ([vanhuman](https://github.com/vanhuman))
- Updates the description. [\#807](https://github.com/supercollider/supercollider/pull/807) ([vanhuman](https://github.com/vanhuman))
- Changed the description of Balance2 into something a bit more explanatory.  [\#806](https://github.com/supercollider/supercollider/pull/806) ([vanhuman](https://github.com/vanhuman))
- Added missing closing "\)" in MIDI input example. [\#803](https://github.com/supercollider/supercollider/pull/803) ([attejensen](https://github.com/attejensen))
- sc ide: settings editor: move insertMatchingTokens setting [\#796](https://github.com/supercollider/supercollider/pull/796) ([gusano](https://github.com/gusano))
- Update GUI-Layout-Management.schelp [\#795](https://github.com/supercollider/supercollider/pull/795) ([olafklingt](https://github.com/olafklingt))
- sc ide: sc editor: slight improvement of matching token insertion [\#792](https://github.com/supercollider/supercollider/pull/792) ([gusano](https://github.com/gusano))
- sc ide: settings editor: move insertMatchingTokens setting [\#791](https://github.com/supercollider/supercollider/pull/791) ([gusano](https://github.com/gusano))
- class library: UGen - provide bilin [\#781](https://github.com/supercollider/supercollider/pull/781) ([timblechmann](https://github.com/timblechmann))
- scide: left|right arrow keys disable completion [\#773](https://github.com/supercollider/supercollider/pull/773) ([gusano](https://github.com/gusano))
- removed superfluous links under related [\#766](https://github.com/supercollider/supercollider/pull/766) ([redFrik](https://github.com/redFrik))
- added related link to SendReply [\#765](https://github.com/supercollider/supercollider/pull/765) ([redFrik](https://github.com/redFrik))
- Update Warp1.schelp [\#759](https://github.com/supercollider/supercollider/pull/759) ([redFrik](https://github.com/redFrik))
- fix git recursive flag command [\#747](https://github.com/supercollider/supercollider/pull/747) ([hems](https://github.com/hems))
- Adding "Contributing with the documentation" to "Writing Help.schelp" file [\#746](https://github.com/supercollider/supercollider/pull/746) ([hems](https://github.com/hems))
- cmake: link pthreads libraries [\#742](https://github.com/supercollider/supercollider/pull/742) ([gusano](https://github.com/gusano))
- Topic/simplify cpu dispatching [\#732](https://github.com/supercollider/supercollider/pull/732) ([timblechmann](https://github.com/timblechmann))
- Multichannel envelope fixes [\#718](https://github.com/supercollider/supercollider/pull/718) ([timblechmann](https://github.com/timblechmann))
- Topic/ide cmdline [\#711](https://github.com/supercollider/supercollider/pull/711) ([timblechmann](https://github.com/timblechmann))
- out-comment s\_tick [\#709](https://github.com/supercollider/supercollider/pull/709) ([2mc](https://github.com/2mc))
- include: split public and private headers & prototype libsclang interface [\#703](https://github.com/supercollider/supercollider/pull/703) ([timblechmann](https://github.com/timblechmann))
-  in OpenBSD search for classes using cmake configuration [\#685](https://github.com/supercollider/supercollider/pull/685) ([acamari](https://github.com/acamari))
- ports supercollider to OpenBSD [\#683](https://github.com/supercollider/supercollider/pull/683) ([acamari](https://github.com/acamari))
- scide: code editor - insert path when dropping unknown files [\#663](https://github.com/supercollider/supercollider/pull/663) ([totalgee](https://github.com/totalgee))
- asSynthDef and SynthDef.name should return symbols [\#661](https://github.com/supercollider/supercollider/pull/661) ([timblechmann](https://github.com/timblechmann))
- Fix for \#657; copy/paste to OS X Mail loses tabs [\#659](https://github.com/supercollider/supercollider/pull/659) ([totalgee](https://github.com/totalgee))
- Main:startup: initialize openPorts before StartUp.run to enable OSCFunc ... [\#647](https://github.com/supercollider/supercollider/pull/647) ([iani](https://github.com/iani))
- Properly initialize mSaveTime on document open and save. [\#597](https://github.com/supercollider/supercollider/pull/597) ([scztt](https://github.com/scztt))
- Corrected some typos and errors in the help docs [\#506](https://github.com/supercollider/supercollider/pull/506) ([andrewcsmith](https://github.com/andrewcsmith))
- Fixed two typos in the update news [\#500](https://github.com/supercollider/supercollider/pull/500) ([andrewcsmith](https://github.com/andrewcsmith))
- class library: fix server meter initialization [\#484](https://github.com/supercollider/supercollider/pull/484) ([timblechmann](https://github.com/timblechmann))
- Refer to TChoose from the TIRand help file [\#355](https://github.com/supercollider/supercollider/pull/355) ([rukano](https://github.com/rukano))
- Typo in PparGroup's help file example [\#351](https://github.com/supercollider/supercollider/pull/351) ([rukano](https://github.com/rukano))
- corrected pathname for sound file in LevelIndicator.schelp [\#347](https://github.com/supercollider/supercollider/pull/347) ([miczac](https://github.com/miczac))
