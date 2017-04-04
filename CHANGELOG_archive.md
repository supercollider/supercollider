See CHANGELOG.md for recent releases.

SuperCollider v3.6.5, released 2013-04
======================================

Jakob Leben (10):
- sc class library: fix regression in Server:-scope
- scide: add "reset font size" action to post window and help browser
- scide: autocompletion: order methods by class hierarchy when class is known
- documentation: improve info on logical time, clocks and threads
- documentation: more info on threads, clocks and time
- sclang: PyrThread: ensure slot type safety
- documentation: clarify the functioning of Thread and Routine
- streamline README.txt
- documentation: improve thisFunction and thisFunctionDef

Julian Rohrhuber (3):
- sc class library: replacing the source of a node proxy led to hanging patterns
- sc class library: NodeProxy:cleanNodeMapnow works even if no settings are present
- fix typo /  removing the implication that ansi-C isn't appropriate

Michael Zacherl (5):
- In.schelp: replaced AudioIn w/ SoundIn in reference, added loudness in example section
- Knob.schelp: repositioned text in mouseOverAction example
- Klang.schelp: changed 'filter' to 'oscillator' in methods section
- DynKlang.schelp: changed 'filter' to 'oscillator' in methods section
- README.txt: reworked and simplified with focus on SC IDE and version 3.6

vividsnow (2):
- scdoc: Pseg: duration pattern in beats not seconds
- scdoc: add thisFunctionDef/thisFunction


SuperCollider v3.6.4, released 2013-04
======================================

Dan Stowell (1):
- SinOsc and Osc: note phase issue beyond +-8pi. Fixes #815

Jakob Leben (34):
- sclang: fix Char:-isUpper/isLower
- qtcollider: add QListView:-selectionAction
- qtcollider: add QListView:-selection setter
- scide: remove credits for kiberpipa
- help: GUI - improve documentation of alignment options
- help: add guide on creating standalone applications
- sc ide: show impl/ref lookup dialogs even when no text under cursor
- sc class library: ClassBrowser: fix search with empty query string
- sc ide: interpreter: post notification on quit or crash
- qtcollider: pass exit code up to SC_TerminalClient
- sc ide: fix and improve region detection
- sc ide: sc editor: add action to select pair of brackets enclosing cursor
- sc ide: sc editor: update bracket match highlight after applying settings
- qtcollider: QTextView: increase 'selectedString' compatibility, fix docs
- qtcollider: envelope view: fix drawing of quadratic and cubic curves
- sc ide: help browser: delegate docklet focus to webpage view
- sc ide: docklet: when focusing, also activate window
- sc ide: fix auto-indenting closing brackets on certain locales
- sc ide: ensure dock widgets within screen bounds when first undocked
- qtcollider: QTextView: set 'enterInterpretsSelection' to true by default
- scide: config dialog: preserve font when toggling "show only monospaced"
- scide: select line in code on triple click
- scide: ensure last active window activated after open/save file dialog
- scide: on startup, remove invalid file paths from "recent documents" list
- scide: improve default paths in open/save dialogs
- scide: save document dialog: always allow saving with any extension
- scide: editor: highlight unmatched brackets just like mismatched ones
- qtcollider: StackLayout: fix crash when removing contained widget
- qtcollider: do not allow reparenting layouts, to avoid crashing
- scide: fix closing tool panels on Escape
- scide: impl/ref lookup: close dialog when opening documentation for class
- Revert "Revert "scide: on Mac, make one global menu to share by all windows""
- scide: prevent erroneous overriding of shortcuts on Mac OS

James Harkins (2):
- Library: Bugfix for PmonoArtic inside other patterns w/cleanup
- Library: Fix Pfset passing child cleanups up to its parent(s)

Tim Blechmann (10):
- Help: fix rlpf help file
- plugins: DemandEnv - fix shape polling
- plugins: GrainBuf - catch both inf and NaN phase arguments
- scsynth: prevent possible buffer overflow
- cmake build system: fix x11 include paths
- class library: Bus - fix get method for multi-channel busses
- class library: Server.scope - remove limitation to 16 channels
- plugins: LocalOut - don't crash server if LocalIn is missing
- sclang: prevent buffer overflow
- scide: link with librt

Victor Bombi (1):
- supernova: CMakeLists.txt must set include dirs for fftw3f

attejensen (1):
- Update MIDI.schelp


SuperCollider v3.6.3, released 2013-02
======================================

Dan Stowell (2):
- Add cmake option NO_GPL3 to simplify building of GPL2 binaries
- SCDoc: generalise licensing from GPL3+ to GPL2+

Graeme Urquhart (2):
- Issue #702 fix: sendSelection receives NSString
- String:Help of split method matches implementation

Jakob Leben (24):
- qtcollider: relicense to GPL version 2
- sclang: terminal client - fix and simplify request handling
- qtcollider: support String:-speak when Speech class is available
- cmake: set LIBSCSYNTH=ON by default on Windows
- qtcollider: QView - do not block beginDrag if currentDrag is already set
- qtcollider: QKnob - let 'background' affect knob color
- sc ide: improve server boot/quit actions
- sc ide: improve interpreter start/stop actions
- sc ide: improve default server booting shortcuts
- qtcollider: sf view: fix loading non-power-of-two floating point files
- sc ide: disable zooming by mouse wheel (scrolling)
- sc ide: editor - set Qt::WA_MacNoClickThrough on viewport
- help: improve the SC IDE guide
- qtcollider: implement QtGUI:*cursorPosition
- class library: Platform - redirect getMouseCoords to GUI
- sc ide: post window - disable click-through on Mac OS X
- sc ide: add Help menu action to open the SuperCollider IDE guide
- help: SC IDE guide - show scaled screenshot, with a link to unscaled one
- sc ide: docklets - fix geometry after undocking
- sc ide: change default shortcuts for Go To Next/Previous Region
- sc ide: make cmd-period silent
- sc ide: improve status box context menu interaction
- sc ide: add context menu to interpreter status box

James Harkins (4):
- Fix title:: tags in the practical guide: user-friendly titles vs. filenames
- Add Practical Guide Cookbook entry on swing rhythms
- PG_Cookbook_08: Fix an omitted copy/paste
- Fix typo in analysis example: BufWr.ar on a kr signal is bad

Tim Blechmann (22):
- supernova: fix crash on /quit with portaudio
- class library: PlusFreqScope - survive server actions
- scide: remove ctrl-b shortcut
- class library: FreqScope - fix for starting scope after booting
- common: introduce new autogenerated SC_Version.hpp header
- class library: fix Array-unlace
- supernova: plugin interface - guard access to rt-pool
- plugins: IOUgens - prevent buffer overflow
- Help: BrownNoise - use a convention of -20db
- supernova: sized array - assert boundaries
- supernova: sndfile backend - correctly use correct blocksize for temp buffer
- supernova: jack backend - avoid uninitialized value
- supernova: nrt engine - nicer formatting of message log
- plugins: ui ugens - initialize libx11 for threading
- supernova: start dsp threads from run methods
- sclang: library config - correcty handle library config command line argument
- server plugins: RecordBuf - fix multichannel corruption and buffer overrun
- fftlib: for now we avoid intptr_t
- server plugins: fix div_ka
- plugins: osc ugens - fix GET_TABLE macro
- plugins: OscUGens - ensure buffer initialization
- scide: add menu item to open the user application support directory

Victor Bombi (2):
- common: win32 - avoid integer truncation
- supernova: correctly print synthdef path


SuperCollider v3.6.2, released 2012-12
======================================

BS Collist (1):
- qtcollider: QEnvelopeView - add method to return all selected indexes

Jakob Leben (32):
- common (windows): unify access to known folder paths
- sclang (windows): add primitive to access "My Documents" dir
- cmake: expand the search for libsndfile and libfftw3f
- cmake (Windows): use CMAKE_LIBRARY_PATH for fixup_bundle() search dirs
- scide: let cmd-period have an application-wide shortcut context
- scide: DocumentManager - refresh cached file info before storing save time
- scide: help browser - support doc/impl/ref lookup for selected text
- scide: search widget hierarchy upwards for first handler of lookup actions
- scide: GenericLookupDialog - no need for subclassing QTreeView anymore
- scide: make doc/impl/ref lookup shortcuts work on detached docklets
- scide: always pop up lookup dialogs with the active window as the parent
- scide: update translation sources, add italian
- qtcollider: start drag within mouse event handler
- qtcollider: QStethoscope2 - reverse operation of horizontal zoom slider
- scide: GenericCodeEditor - set Qt::WA_MacNoClickThrough widget attribute
- scide: SyntaxHighlighter - swap QChar::toAscii() for toLatin1()
- scide: Document - swap QString::toAscii() for QString::toLatin1()
- scide: MainWindow - substitute deprecated QFileDialog::setFilter(QString)
- scide: MainWindow - include QMimeData
- scide: PostWindow - include QMimeData
- scide: GenericCodeEditor - include QMimeData
- qtcollider: QWidgetProxy - include QDrag
- sclang: SCIpcClient - fix includes
- cmake: sclang - fix building when SC_IDE=ON and SC_QT=OFF
- cmake: scide - add QtNetwork to required Qt modules
- qtcollider: QStethoscope2 - refactor for robustness
- qtcollider: QListView - add 'selection' method to get all selected indexes
- help: document new 'selection' methods of EnvelopeView and ListView
- help: View - improve documentation, fix links
- help: fix a large amount of broken links due to changes in SCDoc
- cmake: FindPortmidi - actually implement auto-finding portmidi

James Harkins (1):
- Fix bug introduced by 7f29d322: Don't free the same alloc'ed index twice

Tim Blechmann (18):
- scide: DocumentManager - read files via QTextStream to decode characters
- supernova: osc handler - fix completion message and done message for /b_close
- supernova: asynchronous log - fix string splitting
- supernova: compile fix
- supernova: send /fail messages on /notify commands
- supernova: send /fail on buffer commands
- supernova: fix sndfile error handling
- win32: ensure stack alignment
- plugins: fix GrainBuf cleanup
- Help: SymbolicNotations - replace SCSlider with Slider
- supernova: plugin interface - protect against multiple done actions
- Help: remove memStore
- class library: Buffer - freeMsg should clear all cached information
- supernova: osc interface - fix bug with node reordering
- supernova: buffer_read - don't check samplerate when queueing soundfiles
- class library: fix Function.plot
- plugins: RecordBuf - fix recordbuf overrun & fix done action handling
- Help: RecordBuf - RecordBuf is recording, not playing


SuperCollider v3.6.1, released 2012-11
======================================

Dan Stowell (1):
- SpecFlatness: prevent NaN output for silence (thanks nick collins)

Glen Fraser (1):
- scide: code editor / post window - copy using plain text

Jakob Leben (13):
- update README_WINDOWS.txt for changed application data locations
- fix compilation with MinGW (broken static initialization)
- scide: find/replace - use Qt translation system to handle singular/plural
- cmake: scide - improve handling translations
- scide: load translations from app resource directory
- scide: update translation source files
- scide: change english translation file name to serve as fallback
- sclang: (Windows) change app support dir from roaming to local
- scide: load fallback translation in addition to locale translation
- sclang: add primitive to allow Platform to access user home directory
- class library: WindowsPlatform - set a user-friendly default recordingsDir
- readme (windows): add instructions on moving application data

Tim Blechmann (1):
- class library: SynthDef - writeDefFile should use default SynthDef path


SuperCollider v3.6.0, released 2012-11
======================================

Major release with many new features - please see the help doc
"News in 3.6" for more information.
http://doc.sccode.org/Guides/News-3_6.html


SuperCollider v3.5.7, released 2012-11
======================================

Jakob Leben (6):
- sclang: (Windows) fix String:-getenv to return variables set with -setenv
- class library: ServerMeter - fix closing window when server has never run
- sclang: fix 'gcd' and 'lcm' methods
- qtcollider: QStethoscope2 - fix width of number boxes
- qtcollider: fix SoundFileView:-selectAll and -selectNone
- qtcollider: fix QPen:*matrix setter - combine instead of replace matrix

Julian Rohrhuber (1):
- class library: jitlib - Avoiding sync problems with free/play

Tim Blechmann (9):
- plugins: filters - fix initialization of filter parameters
- external libraries: nova-simd update
- external libraries: move nova-simd submodule to github
- plugins: DelayN - fix initialization code
- Revert "plugins: DelayN - fix initialization code"
- common: fftlib - increase size limit for ffts
- sclang: server shm interface - fix setting of multiple values
- plugin interface: provide wrapper class for c++-style unit generators


SuperCollider v3.5.6, released 2012-10
======================================

Dan Stowell (2):
- Improve error messages when cmake can't find optional things
- Compile fix for Qt widget on arm.     Upstreamed from debian-multimedia (thanks Felipe Sateler)

James Harkins (1):
- Fix Spawner bug: cleanup.update is mandatory, including rest events

Jonatan Liljedahl (7):
- Quarks: fix typo and also open old-style helpfiles ending with .htm
- Include old non-converted helpfiles in SCDoc document index
- HelpBrowser: also open RTF files with whatever is available
- Even more support for old help files
- scdoc: use JS hasOwnProperty instead of testing the property directly
- HelpBrowser: post javascript errors
- SCDoc: properly escape keys in generated docmap.js

Joshua Parmenter (1):
- Fix ServerOptions instance var ordering, etc., to make internal server booting use correct number of audio bus channels.

Tim Blechmann (4):
- cmake: provide explicit option to use system-installed boost libraries
- external libraries - revert submodule updates
- lang: SerialPort - fix invocation of done action


SuperCollider v3.5.5, released 2012-09
======================================

Dan Stowell (1):
- Fix bug in Complex:exp

James Harkins (1):
- Convert misleading and confusing OSC-style example into object-style

Joshua Parmenter (2):
- fix IEnvGen kr UGen
- fix cocoa window alpha setting

Tim Blechmann (12):
- sclang: fix Array:extendWrap for negative size argument
- sclang: array primitivies - protect all array extend primitives against negative sizes
- scdoc: fix string comparison in parser
- supernova: sized_array - don't allocate memory for zero-sized array
- plugins: GrainBuf - fix crash when using nan as position control
- scsynth: ensure alignment of wire buffers
- supernova: catch exceptions when reading synthdefs
- supernova: free_aligned - fix fallback implementation for null pointers
- cmake build system: dont compile shared library with -fwhole-program
- plugins: GrainBuf - allocate grain after reading window
- plugins: GrainBuf - fix access to default hann window

Victor Bombi (1):
- cpu usage for portaudio_backend.hpp


SuperCollider v3.5.4, released 2012-08
======================================

Dan Stowell (5):
- Fix typo that causes build fail on big-endian archs, thanks Felipe Sateler
- fix build on ARM (where qreal==float); thanks Felipe Sateler
- Strip gremlin characters from JITLib wrapForNodeProxy.sc
- choose clipping rather than wraparound for writing integer-format audio     files (libsndfile setting)
- arm build fix: another double->qreal in QcMultiSlider

James Harkins (1):
- Improve documentation of GUI kits and kit switching

Jonatan Liljedahl (2):
- SCDoc: Use proper static string constants instead of comparing string literals.
- Revert "reinstate Mix.arFill and Mix.krFill for backward compatibility reasons"

Julian Rohrhuber (2):
- reinstate Mix.arFill and Mix.krFill for backward compatibility reasons
- improve string helpfile

Tim Blechmann (10):
- plugins: GrainUGens - handle unallocated window buffers
- plugins: GrainBuf - reject multi-channel buffers
- plugins: grain ugens - treat empty window buffers correctly
- server: provide memory alignment wrappers for msvc
- server: scsynth - ensure correct deallocation of SndBuffer memory
- server/language/supernova: automatically clip integer audio files
- scsynth: correctly free aligned buffers
- Help: fix OSC function in SendPeakRMS help file
- package: use alternative implementation of git-archive-all

Victor Bombi (1):
- MSVC fix


SuperCollider v3.5.3, released 2012-06
======================================

Dan Stowell (6):
- LocalIn helpfile fix, thanks Bruno Ruviaro
- Fix scvim regsitry file for updated filename (thanks Carlo Capocasa)
- version number to 3.5.3
- Server helpfile: see-also reference docs
- SCVim.sc should not be executable
- cmake build system: use system boost libraries if available

Jakob Leben (1):
- cmake: fix Boost Thread linking on Windows

James Harkins (10):
- EnvGen_next_ak_nova: Hardcoded blocksize=64, change to
- inNumSamples
- Per Scott W., initSiblings is not needed
- Reinstate Mix.ar and Mix.kr, with rate checks
- Fix crossplatform fail: Scale.directory shouldn’t always depend
- on Document
- ListPatterns: offset.value omitted (inval) as an argument
- Fix PbindProxy:storeArgs - should NOT call “source” on keys in
- the array!
- Scale:degreeToRatio should handle degrees outside of one
- octave’s range
- More meaningful error message for too many selectors
- Explain the limitation on the number of selectors in one
- FunctionDef
- Correct spelling error

Jonatan Liljedahl (3):
- Methods.html: auto-redirect to Search if method not found
- SCDoc: fix detection of old format class docs
- Mix.ar was un-deprecated, so remove the deprecated method

Joshua Parmenter (2):
- fix scroll view problem for OS X 10.7.4
- update SC_DirUtils to look at the name of the app bundle on osx

Julian Rohrhuber (14):
- fix bugs due to wrong usage of partial application
- PV_BinShift helpfile improved
- PV_Diffuser helpfile improved
- reformat statement for readability (no change of functionality)
- helpfile improvements
- improve array helpfile
- add note to the loop argument of DiskIn (thanks Stefan).
- improve helpfile
- some helpfile improvements
- improve helpfile
- improve helpfile
- improve and simplify FFT overview helpfile: fix some errors in
- examples.
- improve and simplify IFFT helpfile.
- improve and simplify FFT helpfile, mention that hopsize must be
- larger than 0.0

Tim Blechmann (11):
- external libraries: update nova-tt (gcc 4.7 fix)
- supernova: correctly implement replace semantics for /s_new
- Help: Function.scope is not limited to OSX anymore
- cmake build system: locate server plugins on freebsd
- server: add support for RF64
- cmake build system: ensure boost include path for scsynth
- cmake build system: set boost library path
- cmake build system: link scapp with correct version of
- libboost_thread
- cmake build system: minor cleanup
- supernova: fix asynchronous commands for empty reply address
- common: fix non-apple builds


SuperCollider v3.5.2, released 201
======================================

Dan Stowell (3):
- Remove outdated Japanese menus
- Cannot use indentation for CMAKE example - on mac it is rendered as     &nbsp; which then breaks cmake compilation
- Fix bug in FFT library if winsize != audiosize

Jakob Leben (21):
- qtcollider: fix QTextView:-background and QSoundFileView:-background
- cmake: improve message if Qt4 or one of its components not found
- qtcollider: QKnob: fix mouse response when mouseOverAction is set
- qtcollider: implement missing QPopUpMenu:-background
- qtcollider: QTextView fixes and improvements
- help: add missing GUI examples
- qtcollider: support use of UTF-8 encoded strings
- qtcollider: QTextView: improve -enterInterpretsSelection
- qtcollider: QTextField: never propagate Enter to parent
- qtcollider: QEnvelopeView: improve node selection API and UI
- help: update EnvelopeView documentation
- help: fix incorrect info in EnvelopeView documentation
- qtcollider: QObject:-getProperty: turn an error into a debug warning
- qtcollider: implement drag-and-drop for data outside SC
- qtcollider: improve key propagation in QListView and QTreeView
- qtcollider: optimize view instantiation (take 2)
- qtcollider: fix mouse wheel event being forwarded to SC for no reason
- qtcollider: fix potential null pointer dereference
- qtcollider: optimization - partially revert event handling changes
- qtcollider: optimization - avoid a signal connection at QObject construction
- qtcollider: optimization - avoid connecting signals with unnormalized signatures

James Harkins (2):
- Fix Pcollect/select/reject:embedInStream to pass inval to the function
- setTheme: Inherit colors from parent theme if the user didn't specify

Jonatan Liljedahl (41):
- scdoc: MathJax: don't use fonts installed on users computer
- New SCDoc parser and renderer. Faster, more stable, less buggy.
- fix some helpfiles for new scdoc
- scdoc.css update
- scdoc: scapp compile fix
- scdoc: defer indexAllDocuments until first use
- HelpBrowser tweaks
- scdoc: warn on additions for non-existent help doc
- scdoc: fill in argument names for argument:: with no name given
- SCDocRenderer: warn on broken links
- scdoc: fix classtree:: rendering bug
- scdoc: only warn on grouped methods argnames mismatch if argument:: tag is used
- scdoc: avoid GC error in primitive
- scdoc: collect metadata also from *.ext.schelp (doc additions)
- scdoc: warn if argument:: name does not match real method arg
- scdoc: updated SCDoc related docs
- scdoc: warn if classdoc title and filename mismatch
- scdoc: fix varargs name match warning
- scdoc: render getter/setter combinations as two different methods
- scdoc: warn if setter methods (trailing underscore) is documented explicitly
- scdoc: more helpfile fixes
- scdoc: fix some bugs, handle class docs with missing classes
- scdoc Search.html: match also on filename for 'title'
- schelp: fix some broken links
- scdoc: add clearCache arg to indexAllDocuments, and don't render undocumented classes more than once per session
- scdoc: updated SCDoc related helpfiles
- schelp: more doc error fixes
- scdoc: improve argument:: auto-fill and checks
- String-warn and -error: don't print newline after WARNING: and ERROR:
- scdoc: tweak warnings
- scdoc: fix escaping of :: in metadata parsing and block verbatim
- schelp: add keywords for scdoc tags in SCDocSyntax.schelp
- scdoc: allow end-of-file as newline terminator, and improve error messages
- scdoc: use setter_() syntax if more than one argument
- scdoc: render method arg defaults as "foo: val" instead of "foo = val"
- mention new scdoc implementation in News-3_5.schelp
- scdoc parser: allow empty lines before headertags
- SCDoc: fix escaping of & < and >
- SCDoc: fix inf loop at missing :: end-tag in code blocks
- SCDoc: allow EOF as terminator for private:: and similar tags
- SCDoc: don't warn on missing trailing mul & add args

Miguel Negrão (1):
- [Class Libray] Quarks GUI - sort quarks by name

Tim Blechmann (10):
- plugins: fix Clip.kr
- class library: archive TempoClock as compile string
- cmake build system: restrict win32-specific cflags to win32
- external libraries: nova-simd update
- external libraries: nova-simd compile fix
- plugins: fix StereoConvolution2L constructor
- scsynth: use aligned memory allocation functions from supernova
- external libraries: nova-simd update
- scsynth: provide zalloc as symbol

redFrik (1):
- scdoc: fixed a bunch of helpfile errors


SuperCollider v3.5.1, released 2012-04
======================================

Jakob Leben (13):
- windows: properly pass the SC version to NSIS
- qtcollider: QPopUpMenu: fix action triggering
- qtcollider: get rid of "X is not implemented" message
- class library: make Server:-plotTree resilient to GUI kit switching
- help: improve Stethoscope documentation
- class library: QStethoscope2: add missing class methods
- class library: fix UGen scoping on out-of-process servers
- class library: PlusFreqScope: simplify server checking
- class library: fix and improve various 'scope' and 'freqscope' methods
- help: fix Stethoscope:*isValidServer documentation
- class library: ServerMeter: fix synth startup and cleanup
- update README_WINDOWS.txt
- windows: improve building and installation

Jonatan Liljedahl (6):
- lang11d: Fix parse tree generation of expr.(key:value, ...)
- SC.app: allow saving plain text .schelp files
- SCDoc: copymethod:: also search *.ext.schelp files
- Update News for 3.5 doc
- Fix typo in News-3_5.schelp and improve StartupFile.schelp
- Update WritingPrimitives.schelp regarding GC safety

Joshua Parmenter (1):
- prevent HID crashes on OS X. Devices still aren't added to the queue though (longs for the locID aren't correctly set up)

Scott Wilson (1):
- Make Unpack1FFT a subclass of UGen, rather than of PV_ChainUGen

Tim Blechmann (4):
- class library: SynthDef - fix uploading of large synthdefs
- sclang: block evaluation typesafety
- sclang: signal primitives - fix Signal-fft


SuperCollider v3.5.0, released 2012-03
======================================

Major release with many new features - please see the help doc
"News in 3.5" for more information.
http://doc.sccode.org/Guides/News-3_5.html


SuperCollider v3.4.5, released 2012-01
======================================

Tim Blechmann (7):
- class library: FreqScope fix
- sclang: fix crash of scpacket overflow by using exception handling
- sclang: pad PyrMethodRaw struct
- sclang: force size of PyrSlot to 16 byte and fix PyrMethodRaw size
- server plugins: fix div_ai_nova
- plugins: Resonz - fix initialization
- plugins: disable simd-optimization for tanh

James Harkins (3):
- Explicitly show the command to uninstall (for scons idiots like me).
- (3.4) PathName now sets tmp directory using Platform
- SimpleController:update would throw error if no actions had been 'put' in

Dan Stowell (1):
- Remove waf file from 3.4.x - was never used, and contains binary code,     causing linux packaging problems.     See ubuntu bug #529154 for details, and debian bug #529154 for     sc-specific

Mathieu Trudel-Lapierre (1):
- Fixup environment variables used for linking against readline, libicu, curl, cwiid.

Nick Collins (1):
- Fix bug in MFCC ugen

Noe Rubinstein (1):
- Fix PMOsc doc: index -> pmindex

dmotd (1):
- Include altivec.h on linux powerpc, fixing FTBFS


SuperCollider v3.4.4, released 2011-06
======================================

Dan Stowell (4):
- Improve format of copyright/GPL notices (issue raised in debian pkging)
- Clarify Fontana copyright in MoogFF (and don't use keyword 'copyright'     in files where he doesn't have copyright)
- Update AUTHORS file
- Remove unneeded PDF (debian raised query over copyright)

Nick Collins (1):
- Initial fix for headphones problem where plugging in or out headphones while using Built-in Output leads to loss of audio on OS X. Aggregate Devices not tackled at this point

Tim Blechmann (15):
- sclang: mathematical operators - clip2 fix
- plugins: LPF - fix control-rate initialization
- sclang: wii - don't use address of temporary
- SCClassLibrary: ScoreStreamPlayer - do not add instances to server list
- scsynth: apple - set denormal handling flags, if __SSE__ is defined
- sclang: slotString - crash fix
- plugins: XLine - correct handling of done actions
- sclang: gc - introduce LazyCollect to avoid leak of frames and argument lists
- plugins: Pitch.ar - fix crash for high execution period
- changelog: fix version number
- update changelog
- sclang: parser - support message send syntax for unary operators
- plugins: delay ugens - rt memory allocation may fail
- sclang: compile fix


SuperCollider v3.4.3
======================================

Dan Stowell (2):
- SC 3.4 set correct SOVERSION 1.0.0 for libs, and install more properly.     (Changes ported from downstream debian packaging.)
- lib SOVERSIONs back from 1.0.0 to 1, following debian-multimedia advice

James Harkins (8):
- Fix nowExecutingPath bug in scel (never backported?)
- fix two bugs in NotificationCenter registerOneShot:
- fix corner case in ClassBrowser
- Fix asPseg bug for short curves array (which should wrap, not kill the stream)
- Clear dataptr when closing a file (so that isClosed answers correctly)
- Incorrectly used dataptr instead of fileptr in previous commit on this file
- replace old, unsafe Dictionary test with a safer (but less OOPy) test
- rats... I missed two others of the same

Joshua Parmenter (1):
- update version number

Tim Blechmann (3):
- scsynth: set ftz flag on osx
- two commits: (1) simplify access to the superclass tree in Class. (2) when looking for a code file (openCodeFile) or cmd-J, it is now enough to select a full line, instead of havin
- scons build system: libsclang build fix


SuperCollider v3.4.2, released 2011-03
======================================

Bugfixes:
---------

- 2010-06-05 fix Latch first sample output bug: if trigger > 0 initially, latch should not output 0 - jh
- 2010-09-04 fix firstArg behavior in BinaryOpUGen by a list-approved hack - jh
- 2010-10-01 fix SConstruct so that libscsynth and libsclang get SONAME entries - ds
- 2010-11-13 grainBuf: audio-rate trigger fix - tb
- 2010-11-15 generate libsclang and libscsynth with .so.1 extension (and soname) on linux - ds
- 2010-11-15 scons create symlinks from libX.so to libX.so.1 on linux, and install them - ds
- 2010-11-16 added .htm files to SConstruct as approved help file extension - mb
- 2010-11-28 compile fix for curl support - tb
- 2010-11-28 prevent asBus from breaking when called with no numChannels - jh
- 2010-12-03 grain ugens: demand ugen input fix - tb
- 2010-12-05 SystemClock and TempoClock sched and schedAbs with inf time doesn't schedule the task in the first place. backported from master - tb
- 2010-12-08 prString_FindRegexp fix: match char array was too short to hold null termination - jli
- 2010-12-11 fix classbrowser colors bugs. backported from master - tb
- 2010-12-12 fixes the bug where installed quark help files would not be detected - tb/ar
- 2010-12-13 mark inherited methods in class browser by background colour. backported from master - tb
- 2010-12-30 Pipe does not remove closed pipes from openFiles - jh
- 2010-12-30 fix String:rotate - pb
- 2011-01-02 unit generators: LagControl - fix initialization order - jh
- 2011-01-02 unit generators: LagControl - dynamically allocate buffer for filter states - tb
- 2011-01-07 fixed iOS compilation and backported changes from master branch - ab
- 2011-01-06 array primitives: fix allTuples and unlace - pb
- 2011-01-07 sclang: makeIntrinsicClass - correct bounds for memcpy - tb
- 2011-01-08 sclang: prString_FindRegexp - fill array after allocating objects - tb
- 2011-01-14 sclang: prString_FindRegexp ensure correct size of results array during gc calls - tb
- 2011-02-27 sclang: ensure minimum stack size - tb
- 2011-03-09 SCVim: avoid generating scvim help cache if not currently in scvim - ds
- 2011-03-11 fix the Event type 'note' (fixes rendering patterns to audio files) - rk


SuperCollider v3.4.1, released 2010-11
======================================

- 2010-07-12 remove accidental debug messages from SCView (on mac, posted a lot of info to Console, could affect performance) - ds
- 2010-07-11 Collections should behave as reasonably as possible when empty - some fixes to better this - jr
- 2010-07-11 SynthDef:add now sends to all running servers if no libname is given. SynthDescs are still added to the global SynthDescLib. If you want to handle multiple SynthDesc libs, you have to add the servers to each of them explicitly - jr
- 2010-07-12 PanAz: added support for audio-rate pos arg - lfsaw
- 2010-07-18 improved the sclang syntax highlighting parses - Patrick Borgeat
- 2010-07-30 Dreset UGen allows to reset the child UGens on its input - jr
- 2010-08-05 storeOn / asCompileString now simplifies its output. Default arguments that are given in the *new method anyhow are omitted - jr
- 2010-08-06 Dictionary merge and blend methods - jr
- 2010-08-09 method overwrite messages not posted by default, rather a message inviting people to run Main:overwriteMsg for the info - ds
- 2010-08-13 MethodOverride class to encapsule information on overridden messages, inviting people to run MethodOverride.printAll  - jr
- 2010-08-13 add size arg to Signal:zeroPad - jr and jh
- 2010-08-18 Pevent now uses default event if no event is passed in - jr
- 2010-08-18 added a shortcut to the rather tedious .asCompileString method. In analogy to object.postcs, object.cs returns the compile string - jr
- 2010-08-20 audio driver for scsynth running on Android (through JNI) - ds
- 2010-08-24 un-deprecate scsynth's ability to use internal "green" FFT lib, for embedded devices etc - ds
- 2010-08-28 no 'record' button for remote server GUIs, since path not generally known - ds
- 2010-09-02 token threading for sclang interpreter - tb
- 2010-09-07 when looking for a code file (openCodeFile) or cmd-J, it is now enough to select a full line, instead of having to select both words around the colon - jr
- 2010-09-07 added methods for better navigation in the class tree (findOverriddenMethod) - jr
- 2010-09-10 add method: Complex:abs to fit common usage - jr
- 2010-09-12 added Dwrand UGen - jr
- 2010-09-15 SystemClock and TempoClock sched and schedAbs with inf time doesn't schedule the task in the first place - jr
- 2010-10-07 change the mac HID error-handler code to output errors to sc post window rather than to mac log; removes a pascal-string issue - ds
- 2010-10-19 Ndef now releses its bus when server was quit or just booted - jr
- 2010-10-20 retain the path to the file in which an error has occurred and post it - jr


Bugfixes:
---------
- 2010-07-10 protecting the server against malformatted SynthDef names - jr
- 2010-06-28 syntaxColorize fix for double-backslashes, thanks Patrick Borgeat for the patch - ds
- 2010-07-24 catch crash in the case that one tries to define a unique method using a return value directly - jr
- 2010-09-07 UGen:clip, :wrap, :fold now apply correctly to scalar-rate signals; also methodSelectorForRate tweak for which class is asked - ds
- 2010-09-09 fix a bug for trigger signals in Demand.kr that hold longer than one control period - jr
- 2010-09-11 bug in audio rate mapping fixed, when new source object was inserted in a mapped node proxy - jr
- 2010-09-12 fix bug: 2994009. LFPar and LFCub audio rate modulation frequency argument work now - jr
- 2010-09-19 fix to JITGui, when numItems is not supplied - jr
- 2010-10-10 remove more crufty NSLog debug messages - ds
- 2010-10-13 fix SCUserView:receiveDrag to receive mouse co-ordinates; thanks Daniel van den Eijkel - ds
- 2010-10-19 debian-style scvim-check-if-plugin-is-active, brought upstream - ds
- 2010-10-19 bug in audio rate mapping fixed, when new source object was inserted in a mapped node proxy - jr
- 2010-10-19 partial fix for bugs item #2994009 - seems to fix LFPar but not LFCub. More work needed - ds
- 2010-10-19 DC: fix multichannel expansion - tb
- 2010-10-19 fix to demand rate unary op ugens, thanks james harkins - tb
- 2010-10-19 Ugens: LinLin/LinExp fixes - tb
- 2010-10-19 only /clearSched if RT - to fix tracker item #3033454 - tb
- 2010-10-19 UGens: binary operators - fix scalar/signal division - tb
- 2010-10-19 fix bug 2988525: SynthDef:writeDefFile appends path correctly - tb
- 2010-10-19 ProcessOSCPacket: fix possible deadlock - tb
- 2010-10-19 fix network address handling - albert graef
- 2010-11-05 fix memory issues in regular expressions: correct memory management in prString_FindRegexp - tb
- 2010-11-07 sclang: correct symlink handling - tb, ar

SuperCollider v3.4, released 2010-07
====================================

Headlines:
----------

- 2009-09-03 add support for Mac OS 10.5 and greater 64-bit builds of plugins and scsynth
- 2009-07-xx iphone support by Axel Balley added - ab
- 2009-07-21 EnvirGui added, a gui for livecoding/editing environments - adc
- 2009-07-24 Server.plotTree method for visualising the groups and synths on the server - sw
- 2009-07-31 mac osx text-completion feature now includes sclang objects - ds
- 2009-08-01 sclang now has a flag (Platform.ideName) for which IDE is in use (scapp, scvim, scel, sced, jsceclipse...) so that the same class-library can be used with different IDEs, enabling IDE-specific code as necessary - ds
- 2009-08-16 add emergency escape route: if sclang is caught in an infinite loop, send it a USR1 signal to break out of it - ds
- 2009-09-12 String:findRegexp and other regular expressions now available on linux as well as mac - mb,ds
- 2009-09-18 n_order and Server:reorder allow one to specify chains of nodes - sw
- 2009-09-20 simplify the Server recording interface. prepareForRecord is now optional (will be automatically invoked if you don't), and the server gui button is now just two-state "record" "stop" - ds
- 2009-10-04 support multichannel indices for Env:at - jr
- 2009-10-29 improve OSC message correctness: for convenience, sclang allows command names as symbols with no leading slash e.g. \g_new. To improve compliance with the OSC standard, the leading slash is now added to those symbols before dispatch - ds
- 2009-11-07 use nova-simd framework for performance improvements of unit generators - tb
- 2009-11-21 Event type \note supports polyphonic sustain, lag and timingOffset, and responds correctly to free and release. Add \grain event type. - jr
- 2009-11-28 windows: system "application support path", previously hardcoded as C:\SuperCollider, now settable by environment variable SC_SYSAPPSUP_PATH. Default setting for that env var (when using official wix bundle) will be [SC3INSTALLLOCATION] - ds
- 2009-12-15 sclang: 64-bit safety - tb
- 2009-12-15 sclang: performance improvement of math ops - tb
- 2010-01-02 scsynth: use osc-compilant address patterns for server/lang communication - tb
- 2010-01-24 add readline interface to sclang command-line. This is used by default when invoking "sclang" (to use the non-readline interface set the "-i" option to something other than "none") - ds
- 2010-01-24 enable GPL3 code by default - this 'upgrades' the overall binary license from GPL2+ to GPL3+, and allows supercollider to benefit from GPL3+ libraries such as libsimdmath and gnu readline  - ds
- 2010-02-04 Improvements to SC.app editor: Split pane documents, AutoInOutdent - sw
- 2010-02-18 scvim: now compatible with gnu screen, opens post window using screen, making it compatible with a pure-CLI environment - ds
- 2010-02-xx add the Deployment32-64 build style for building on OS X (10.5 and greater) - jp
- 2010-03-10 SynthDef:memStore deprecated in favour of the more coherent and typeable SynthDef:add - jr
- 2010-04-11 Moved some more experimental JITLib classes to "JITLib extensions" Quark - jr


Bugfixes:
---------

- 2009-06-12 fix for level indicator: critical and warning now display based on peak if it is shown rather than on value - sw
- 2009-06-18 fix for mouse coordinates bug - sw
- 2009-06-22 fix for negative bounds issue in SCUserView - sw
- 2009-06-23 avoid memory corruption when unknown OSC type tags are received. Instead forward them to sclang - jr
- 2009-06-23 Fix server crash with negative buffer numbers. - jr
- 2009-07-20 factors(): no prime factors exist below the first prime - jr
- 2009-07-21 Loudness ugen now supports LocalBuf - nc
- 2009-07-23 Fix very nasty bug in Pbindf: if a key is an array, new values were written into the incoming event, instead of the outgoing event - jh
- 2009-07-28 catch unintialised value in sc_GetUserHomeDirectory(), fixing potential memory corruption if HOME not set - ds
- 2009-08-01 SpecCentroid, fix its reaction to silence (output zero instead of NaN) - ds
- 2009-08-01 NamedControl: single default value now returns instance, not array, default values are obtained in a consistent way - jr
- 2009-08-04 fix the CPU-usage issue when calling plain "./sclang" from the terminal on OSX (seems it was caused by a bug in how OSX handles poll() calls) - ds
- 2009-08-15 LinPan2: fix initialisation issue - panning was not correctly applied during the first calc block - ds
- 2009-09-28 Workaround for faded colours in HTML docs - sw
- 2009-09-13 fix PV_MagShift argument handling, so that the defaults mean no-change, matching the behaviour of PV_BinShift - ds
- 2009-09-20 warn about weirdness of Float:switch - ds
- 2009-09-30 prevent NaN output from SpecFlatness when input is silence - ds
- 2009-10-16 fix cropping issue in printing SuperCollider.app documents - cq
- 2009-10-17 many phase-vocoder (PV_) ugens previously didn't handle the DC/nyquist bins as expected. fixed most of these (PV_MagAbove, PV_MagBelow, PV_MagClip, PV_LocalMax, PV_BrickWall, PV_MagSquared, PV_BinWipe, PV_CopyPhase, PV_Max, PV_RandComb) - ds
- 2009-11-01 fix audio rate arg problem in PlayBuf - jp
- 2009-11-02 fix amplitude-convergence issue in Pan2, Balance2, LinPan2, XFade2, which could sometimes result in sound despite zero amp, as discovered by jh - ds
- 2009-11-03 fix unsafe implementation of methods that allow sending collections to buffers - jr
- 2009-11-04 fix signalRange for MouseX, MouseY and KeyState, so that the range message works now - jr
- 2009-11-19 Fix for PV chains and LocalBuf - sw
- 2009-12-14 fix uninitialised variable in Pulse (could sometimes cause small glitch on init), thanks to rhian lloyd - ds
- 2010-01-10 Demand ugens can now handle more than 32 channels, thanks Patrick Borgeat for the patch - ds
- 2010-02-05 scsynth now respects the -D commandline option when running in NRT mode - ds
- 2010-02-11 Fix for nowExecutingPath with Routines - sw
- 2010-02-23 Performance fixes for SCUserView - sw
- 2010-02-25 Fix interpolation / indexing problem in VDiskIn that caused slight pitch fluctuations - jp
- 2010-03-11 SequenceableCollection:reduce no longer returns nil if the collection has only 1 element - ds
- 2010-03-28 fix memory leak of empty command line, for interactive sclang mode - tb
- 2010-03-29 main menu for Mac lang editor app: correction to key for evaluate selection, used to be return, now return+shift - nc
- 2010-04-19 fix missing font issue in Plotter -jr

Other additions/improvements:
-----------------------------

- 2009-06-11 Evaluate Selection menu command - sw
- 2009-06-23 allow remote apps to send type chars - jr
- 2009-06-27 build 32bit sclang on x86_64 - tb
- 2009-07-xx efficiency improvements on some UGens - tb
- 2009-07-xx improve Quarks use of svn for smoother user experience - ds
- 2009-07-22 catch the case when a user tries to compile into a synthdef, a unary/binary operator that the server can't apply - jh
- 2009-08-29 String:toUpper and String:toLower - ds
- 2009-09-06 Boolean:while now throws an informative error, since Boolean:while has no particular use but is often used in error by beginners in code where Function:while is intended - ds
- 2009-09-12 method FunctionDef:makeEnvirFromArgs allows to create template events from a function - jr
- 2009-09-30 Error is now posted if maxSynthDefs exceeded -sw
- 2009-11-03 TwoWayIdentityDictionary has a removeAt method now - jr
- 2009-11-04 update of deferredTaskInterval from 0.038 to 0.01667 - fo
- 2009-11-07 improved PyrSlot typesafety - tb
- 2009-11-23 menu system improvements in Windows IDE - mv
- 2009-12-13 tidyups for "sclang when on osx but not in sc.app" - ds
- 2009-12-13 added lincurve and curvelin methods for numbers and UGens - jr
- 2010-01-01 OSCresponder and OSCresponderNode respond equally to messages with or without preceding slash - jr
- 2010-01-04 sclang: deprecated Proutine - switch back to the original Prout
- 2010-01-06 UnitTest Quark improved, added script support - jr
- 2010-01-23 Improved NodeProxy and ProxySpace helpfiles. Added proxy composition syntax to NodeProxy - jr
- 2010-01-30 Make multichannel plotting easier. If no numChannels is given, find out automatically  - jr
- 2010-01-31 add new LOOP1 macro - tb
- 2010-01-31 use c99 log2 functions for sc_log2 - tb
- 2010-02-09 rearrangement of supercollider source code tree - ds
- 2010-02-11 Server:default_ now assigns to s by default. Settable with flag - sw
- 2010-02-27 removed SCAnimationView and added SCUserView:animate_ - fo
- 2010-03-10 SCPen:setSmoothing changed to SCPen:smoothing_, harmonised change with swingosc - ds
- 2010-03-23 exponentiation for Complex numbers - jr
- 2010-xx-xx many helpfiles improved - various authors
- 2010-03-30 Image class added, a redirect for SCImage or JSCImage - hr
- 2010-03-30 Pitch ugen ability to output clarity measure (by default not activated, for backwards compat) - ds

SuperCollider v3.3.1, released 2009-06-19
=========================================

Headlines:
----------

- 2009-05-11 SCWindow additions for visible, visible_, unminimize - cq
- 2009-05-17 server guis (on osx) now indicate which one is currently default - adc
- 2009-05-18 enabled control rate versions of Ball, TBall and Spring - mb
- 2009-05-18 LID support for setting "MSC" state as well as "LED" on devices - ds
- 2009-06-19 patched for compatibility with Safari 4, fixing a lockup issue when opening help docs - ar

Bugfixes:
---------

- 2009-05-11 fix keyword addressing for the order: argument - jmc
- 2009-05-15 update libsndfile to 1.0.20 to fix security issues (overflow vulnerabilities) in libsndfile - ds
- 2009-05-20 fix bug #2790649, "very large SimpleNumber:series can crash sclang" - ds
- 2009-05-25 mac icons for document types .quark .scd .rtfd were omitted from the app bundle, now fixed - ds
- 2009-06-02 EnvGen: fix off by one block latency in envelope attacks and releases - jr
- 2009-06-12 bug fix for level indicator: critical and warning now display based on peak if it is shown rather than on value - sw
- 2009-06-12 mouse coordinates fix, deprecate SCUserView:mousePosition - sw
- 2009-06-17 some issues fixed in SCUserView - cq
- 2009-06-20 fix redirect for Stethoscope - adc

Other additions/improvements:
-----------------------------

- 2009-05-05 fixes/improvements to cocoabridge primitives - cq
- 2009-05-06 SCImage various minor improvements - cq
- 2009-05-16 optimisation for scrollview drawing, remove VIEWHACK - sw
- 2009-05-xx various documentation updates - various
- 2009-05-xx various improvements to ubuntu-debian packaging scripts - ds, am
- 2009-05-20 SynthDef:writeOnce now available as an instance method as well as a class method - ds
- 2009-06-11 sc.app gets a menu command for "Evaluate selection" - sw
- 2009-06-17 adjusted SCKnob to use relative mouse coordinates - jm
- 2009-06-17 small fix to SConstruct to allow for new Debian X11 location when compiling on linux - mb
- 2009-06-19 Blip ugen: prevent sound blowup by never letting numharm be less than 1 - fo
- 2009-06-20 SCPen: fillStroke changed default from draw(4) to draw(3) - fo
- 2009-06-21 Fold, Clip and Wrap can now modulate the low and high inputs.

SuperCollider v3.3, released 2009-04-30
=======================================

Headlines:
----------
- 2008-04-08 scvim is now part of the distro - ds
- 2008-04-20 improvements to MIDI sysex handling - added sysex parsing directly in source - thanks to charles picasso
- 2008-07-12 scsynth on Mac can now use separate devices for audio input vs audio output. Thanks to Axel Balley for much of the work on this, also a bit by ds.
- 2008-07-12 PlayBuf, RecordBuf, BufWr, BufRd, ScopeOut - used to be limited to 16-channel audio maximum. Now can handle massively multichannel audio - ds
- 2008-07-19 Buffer:normalize method added - ds
- 2008-07-23 FFT and IFFT added option for zero-padding, by optional "framesize" argument - ds
- 2008-09-03 new VDiskIn ugen - jp
- 2008-10-08 SCImage for manipulating bitmap image objects (mac only) - ch
- 2008-10-09 LocalBuf system to allow synths to manage their own purely-local buffers - jr
- 2008-10-17 Added "-P" option to scsynth (accessible as s.options.restrictedPath) to allow restricting which paths scsynth is allowed to read/write - ds
- 2008-10-18 new PartConv ugen, performs efficient frequency-domain convolution - nc
- 2008-10-26 support on mac for "modal windows/sheets" (for user dialogs etc) - sw
- 2008-xx-xx various behind-the-scenes efficiency improvements, for a sleeker audio server that can do more on a given machine - various contributors
- 2008-11-01 add BEQSuite filter UGens (blackrain, jp)
- 2008-11-11 add Pfxb pattern - jr
- 2008-11-25 new EZPopUpMenu - jm
- 2008-11-29 Pitch ugen can now also track the pitch of control-rate signals - mb
- 2008-11-30 Drag and drop paths from Finder to Documents and SCViews - sw
- 2008-12-03 added PV_Div ugen for complex division - ds
- 2008-12-07 added PV_Conj ugen for complex conjugate - ds
- 2008-12-15 new ViewRedirect for easier cross-platform gui syntax. e.g. Window now redirects to SCWindow or JWindow. ds & jm
- 2008-12-15 revised and updated all SC Gui documentation. New gui introduction. New SCUserView subclassing tutorial. - jm
- 2008-12-15 the /done message for Buffer allocation/free/etc now also includes the buffer index - jt
- 2008-12-15 added methods to SCFreqScope for "special" SynthDef, and for visualising frequency responses - ds
- 2008-12-18 the main windows version of sc is now called "SuperCollider" rather than "PsyCollider" (although psycollider is the name of the code editor). SuperCollider on windows now has a different (better? who knows) installer, uses the main sc3 icon, and has some other tweaks that make it different from version 3.2 - ds
- 2008-12-19 new EZListView - jm
- 2009-01-02 sced (the gedit sc plugin) is now part of the distro - mb/artem
- 2009-01-06 SendReply UGen - jr
- 2009-01-06 VDiskIn sends file position to client - jr
- 2009-01-12 map audio to SynthDef controls. new OSC messages n_mapa and n_mapan. - jp, jr, rk
- 2009-01-13 relativeOrigin=true. SC's coordinate system in container views and user views are now by default relative.
- 2009-01-15 SCLevelIndicator view added - sw
- 2009-01-16 Scale and Tuning classes added - tw
- 2009-01-17 SuperColliderAU (scsynth as a Mac OSX "Audio Unit") added to main distribution - gr
- 2009-02-03 EZKnob revised and now part of distro - br, jm
- 2009-02-23 SystemActions refactored - jr
- 2009-02-23 SCMenuItem, SCMenuGroup, and SCMenuSeparator for user customisable menus - sw
- 2009-02-23 LFGauss UGen added - jr
- 2009-03-14 Added GeneralHID based patterns PhidKey and PhidSlot - mb

Bugfixes:
---------
- 2008-05-20 fix for the special case when 0.2.asFraction beachballs the lang (bug id 1856972) - jr
- 2008-05-20 fix slight mistake in the defaults printed by scsynth on command-line (bug id 1953392) - ds
- 2008-07-24 Routine / AppClock fix setting the clock of the thread (bug id 2023852) - jr
- 2008-09-16 stability fixes to FFT and IFFT - ds
- 2008-09-27 fix TExpRand.ar - ds
- 2008-11-11 SystemSynthDefs.numChannels can now be set from the startup file - jr
- 2008-11-24 avoid FFT failure when buffer not allocated - jr
- 2008-11-29 resolved inconsistency in Server:waitForBoot - function is always executed in a Routine, whether or not the server is booted - ds
- 2008-12-07 FlowView setting inital margin and gap fixed (bug id 1986059) - jh
- 2008-12-07 OSCpathResponder fixed (bug id 2021481) - jh
- 2009-01-08 b_readChannel fixed (bug id 1938480) - mb
- 2009-01-08 MIDIIn.connect on Linux fixed (bug id 1986850) - mb
- 2009-01-09 Tabbing in SCTextView - sw
- 2008-08-23 fix for sclang crashing sometimes when compiling erroneous code (bug id 2022297) - rb
- 2009-01-18 SCScrollView relativeOrigin glitch fixed (bug id 2508451) - jr, sw
- 2009-01-28 Fixed QuartzComposer view bounds bug - sw
- 2009-02-21 NodeProxy handles groups more consistently - jr
- 2009-04-16 asFraction fix by JMcC - jr

Other additions/improvements:
-----------------------------
- 2008-03-22 added open Method and link handling to SCTextView - sw
- 2008-04-04 SoundFile:toCSV - ds
- 2008-04-29 buffer UGens now post a warning (rather than failing silently) if buffer channels doesn't match num ins/outs - ds
- 2008-07-14 Deprecated rendezvous in favour of zeroConf - sw
- 2008-09-xx various code improvements, including compiling for 64-bit linux - tb
- 2008-10-03 improvements to standalone build - jp
- 2008-10-03 SCEnvelopeView remembers drawing order. - sw
- 2008-10-05 Maintain initial offset when dragging on an Envelope View node. This avoids nodes jumping to a new position on mouse down. - sw
- 2008-10-05 Enabled gridOn, gridResolution, gridColor, timeCursorOn, timeCursorPosition, and timeCursorColor for SCSoundFileViews. - sw
- 2008-10-31 thisProcess.pid - sclang now can know what its process id is - ds
- 2008-11-21 support for LocalBuf in FFT UGens - jr
- 2008-11-27 SC3 will ignore ugens/class-files in folders named "ignore". Previously the name has been "test" - ignoring folders named "test" is now deprecated and will be removed - ds
- 2008-12-06 Added Main:recompile to allow recompiling from code (SC.app only so far) - sw
- 2008-12-08 Added custom drag label for SCView - sw
- 2008-12-15 Buffer's done osc reply now includes the bufnum - jt
- 2008-12-20 Help tree in help menu (OSX) - sw
- 2008-12-24 EZSLider and EZNumber now have an enclosing containers, as well labelPosition =\left, \right, or \stack modes - jm
- 2009-01-03 Help browser text is editable/executable (CocoaGUI) - sw
- 2009-01-04 Escape exits modal and fullscreen states (OSX) - sw
- 2009-01-08 interface change to ProxySpace.stop (now stops all proxies, just like free/end/clear) - jr
- 2009-01-08 improved Ndef implementation, stores values in an internal ProxySpace, Ndef takes server names for multiple servers. - jr
- 2009-01-08 improved ProxyMixer implementation, added NdefMixer. - adc
- 2009-01-11 Added class browser to help menu (OSX) - sw
- 2009-01-20 New Cocoa based SCTextField - sw
- 2009-01-28 More helpful error string for operation cannot be called from this Process - sw
- 2009-02-23 CocoaDialog takes allowsMultiple arg rather than maxItems - sw


SuperCollider v3.2, released 2008-02-21
=======================================

Headlines:
----------
- 2007-11-xx new suite of machine listening ugens - Loudness, BeatTrack, Onsets, KeyTrack, SpecCentroid, SpecPcile, SpecFlatness - nc, ds
- 2008-01-06 FreeBSD compatibility - hb
- 2008-01-10 Quarks updating on OSX should now be easier for first-time users; commands are run in a separate terminal window - ds
- 2008-01-15 "Advanced find" in Mac interface - jt
- 2008-01-20 Buffer.copy changed to match other .copy methods - now copies language-side object rather than server buffer. Buffer.copyData can be used to copy data from one server buffer to another - jh
- 2008-01-20 - add volume controls to the Server and Server guis - jp
- 2008-01-xx Pattern library implementation changes, Pfx, Pbus, Pgroup etc. - rk, jr, jh
- 2008-01-26 TDuty outputs trigger first, not level. for backwards compatibility TDuty_old - jr
- 2008-02-03 moved the search location for "startup.rtf" on Mac - now searches in system, then user, "Application Support/SuperCollider" folders - ds

Bugfixes:
---------
- 2007-11-16 bug fixes for MIDIIn in connect/disconnect methods. split MIDIOut.sysex into user method and primitive (breaks with previous implementation). default value for uid arg in MIDIOut.new. - mb
- 2007-11-18 fixed a bug in prTry / protect - jr
- 2007-11-27 lock avoided in nextTimeOnGrid
- 2007-12-12 Node-setn fixed when using integers as control indices - jr
- 2008-01-16 fixed Pen: bug with fillRect, fillOval and fillColor (bugtracker id 1837775) - jt
- 2008-01-20 CheckBadValues rate-checking was too restrictive - ds
- 2008-01-20 fix for Saw and Pulse's offset noise on first instantiation, thanks to hisao takagi - ds
- 2008-01-26 TDuty / Duty does not drift anymore - jr
- 2008-02-07 Fixed hang and incorrect background drawing in Cocoa scrollviews - sw

Other additions/improvements:
-----------------------------
- 2007-11-16 MIDIOut.connect and disconnect - mb
- 2007-11-18 added T2A UGen - jr
- 2007-11-18 Refactoring of Document class, including new CocoaDocument class to handle the Cocoa-specific (SuperCollider.app) document management - ds
- 2007-11-18 More macros available in the plugin API for UGen programmers: GET_BUF, SIMPLE_GET_BUF, FULLRATE, RGET, RPUT - ds
- 2007-11-20 UnixPlatform:arch method - jp
- 2007-11-20 FFTTrigger UGen - a ugen to create "fake" (empty) FFT chains - jp
- 2007-11-21 StartUp protects its added functions from each other - if one fails this no longer prevents others from running - ds
- 2007-11-25 added Pclutch and moved StreamClutch to common - jr
- 2007-11-27 Function:inEnvir added - jh
- 2007-12-12 added Collection.flatIf - jr
- 2007-12-15 added control rate functionality to NumRunningSynths - jr
- 2008-01-08 martin rumori's DiskIn bugfix and loop enhancement - jp
- 2008-01-10 String:runInTerminal method - ds
- 2008-01-11 poll now works for scalar ugens - jr
- 2008-01-15 Collection:maxIndex and Collection:minIndex - nc
- 2008-01-24 Server.options.rendezvous to (de)activate Rendezvous if desired - ds
- 2008-01-24 demand ugens accept audio rate inputs correctly - jr
- 2008-01-26 added Dbufwr ugen, for writing to buffers from a demand ugen chain  - jr
- 2008-01-27 Main:version and associated methods for programmatically determining which version SC is - ds
- 2008-02-03 Server:defaultRecDir class variable, to allow user to specify default rec location - ds
- 2008-02-07 SCScrollView and SCScrollTopView no longer fire their action when scrolled programatically - sw


SuperCollider v3.1.1, released 2007-11-16
=========================================
Bugfixes:
---------
- 2007-11-09 re-organized the main help file - rb
- 2007-11-14 fix for .asStringPrec, to avoid crashes on intel systems for large precision values - jt

Other additions/improvements:
-----------------------------
- 2007-11-14 added a preprocessor to the interpreter - jr
- 2007-11-14 added a startup message specifying how to get help - rk


SuperCollider v3.1, released 2007-10-31
=======================================
(changes below are since 2007-09-22, for first ever point release)

Headlines:
----------
- 2007-09-27 SparseArray class added - jr
- 2007-09-28 Help.gui added - ds
- 2007-10-01 FFT and IFFT rewrite - now using more efficient libs, also allows user to vary the overlap and the window type, also large-sized FFTs are possible - ds
- 2007-10-02 UnpackFFT and PackFFT added - these allow for flexible frequency-domain manipulations inside synths - ds
- 2007-10-04 Pkey and Pif added - hjh
- 2007-10-05 reformed Patterns - all patterns accept patterns as arguments - jr
- 2007-10-08 change to UGen plugin loading fixes the audio dropout issue that various users have experienced - rb
- 2007-10-08 GeneralHID crossplatform HID wrapper - mb
- 2007-xx-xx many improvements to Quarks package-management system. gui improvements, dependency-handling improvements, etc - various
- 2007-10-20 added a Glossary file - sw
- 2007-10-xx various new help files added, and many help files improved - various
- 2007-10-26 changed Cmd-? to Cmd-D in lieu of the default help menu shortcut in Leopard. Also changed Cmd-Shift-K (clear post window) to Cmd-Shift-C to avoid accidental recompiles. - rb

Other additions/improvements:
---------------------------
- 2007-09-22 change log added, much rejoicing
- 2007-09-25 added packagesource.sh script to produce source code bundles - ds
- 2007-09-28 IdentityDictionary:doesNotUnderstand now warns if adding a pseudo-method which overrides a real method - jr
- 2007-09-28 String:openHTMLFile added - ds
- 2007-10-04 Integer:collect and Integer:collectAs methods added - ds/jr
- 2007-10-05 Dwhite:new and Dbrown:new have default values for lo and hi - jr
- 2007-10-10 SC no longer automatically writes data (synthdefs, archive.scxtar) to the application folder - instead writes to "app support". This fixes problems with running SC using an unprivileged user account - ds
- 2007-10-16 SequenceableCollection:median speed improvement, approx ten times faster in many cases - ds
- 2007-10-20 Object:deprecated and DeprecatedError added to allow for method deprecation - sw
- 2007-10-21 Amplitude : attackTime and releaseTime can be modulated now - jr
- 2007-10-25 Collection : histo method improved and moved from mathLib to common - jr
- 2007-10-30 improvements to cocoa Gui, including SCUserView improved to support layering and own draw hook - jt, sciss
- 2007-10-31 refactored Pbrown, added Pgbrown - jr

Bugfixes:
---------
- 2007-09-29 takekos bug fixed (obscure issue with garbage collection in arrays) - jm
- 2007-10-01 fixed off by one bug in Dswitch and Dswitch1 that caused a server crash - jr
- 2007-10-09 fixed deadlock and other problems in NSAttributedStringAdditions.m - rb
- 2007-10-11 fixed inaccurate automatic determination of whether SC is running as standalone - ds
- 2007-10-14 .quark files now saved correctly as plain-text, not RTF - ds
- 2007-10-24 fixed a bug in Pbeta - jp


\* *This Change Log was automatically generated by [github_changelog_generator](https://github.com/skywinder/Github-Changelog-Generator)*



SuperCollider v3.6.5, released 2013-04
======================================

    Jakob Leben (10):
          sc class library: fix regression in Server:-scope
          scide: add "reset font size" action to post window and help browser
          scide: autocompletion: order methods by class hierarchy when class is
             known
          documentation: improve info on logical time, clocks and threads
          documentation: more info on threads, clocks and time
          sclang: PyrThread: ensure slot type safety
          documentation: clarify the functioning of Thread and Routine
          streamline README.txt
          documentation: improve thisFunction and thisFunctionDef

    Julian Rohrhuber (3):
          sc class library: replacing the source of a node proxy led to hanging
             patterns
          sc class library: NodeProxy:cleanNodeMapnow works even if no settings are
             present
          fix typo /  removing the implication that ansi-C isn't appropriate

    Michael Zacherl (5):
          In.schelp: replaced AudioIn w/ SoundIn in reference, added loudness
             warning in example section
          Knob.schelp: repositioned text in mouseOverAction example
          Klang.schelp: changed 'filter' to 'oscillator' in methods section
          DynKlang.schelp: changed 'filter' to 'oscillator' in methods section
          README.txt: reworked and simplified with focus on SC IDE and version 3.6

    vividsnow (2):
          scdoc: Pseg: duration pattern in beats not seconds
          scdoc: add thisFunctionDef/thisFunction


SuperCollider v3.6.4, released 2013-04
======================================

    Dan Stowell (1):
          SinOsc and Osc: note phase issue beyond +-8pi. Fixes #815

    Jakob Leben (34):
          sclang: fix Char:-isUpper/isLower
          qtcollider: add QListView:-selectionAction
          qtcollider: add QListView:-selection setter
          scide: remove credits for kiberpipa
          help: GUI - improve documentation of alignment options
          help: add guide on creating standalone applications
          sc ide: show impl/ref lookup dialogs even when no text under cursor
          sc class library: ClassBrowser: fix search with empty query string
          sc ide: interpreter: post notification on quit or crash
          qtcollider: pass exit code up to SC_TerminalClient
          sc ide: fix and improve region detection
          sc ide: sc editor: add action to select pair of brackets enclosing cursor
          sc ide: sc editor: update bracket match highlight after applying settings
          qtcollider: QTextView: increase 'selectedString' compatibility, fix docs
          qtcollider: envelope view: fix drawing of quadratic and cubic curves
          sc ide: help browser: delegate docklet focus to webpage view
          sc ide: docklet: when focusing, also activate window
          sc ide: fix auto-indenting closing brackets on certain locales
          sc ide: ensure dock widgets within screen bounds when first undocked
          qtcollider: QTextView: set 'enterInterpretsSelection' to true by default
          scide: config dialog: preserve font when toggling "show only monospaced"
          scide: select line in code on triple click
          scide: ensure last active window activated after open/save file dialog
          scide: on startup, remove invalid file paths from "recent documents" list
          scide: improve default paths in open/save dialogs
          scide: save document dialog: always allow saving with any extension
          scide: editor: highlight unmatched brackets just like mismatched ones
          qtcollider: StackLayout: fix crash when removing contained widget
          qtcollider: do not allow reparenting layouts, to avoid crashing
          scide: fix closing tool panels on Escape
          scide: impl/ref lookup: close dialog when opening documentation for class
          Revert "Revert "scide: on Mac, make one global menu to share by all
             windows""
          scide: prevent erroneous overriding of shortcuts on Mac OS

    James Harkins (2):
          Library: Bugfix for PmonoArtic inside other patterns w/cleanup
          Library: Fix Pfset passing child cleanups up to its parent(s)

    Tim Blechmann (10):
          Help: fix rlpf help file
          plugins: DemandEnv - fix shape polling
          plugins: GrainBuf - catch both inf and NaN phase arguments
          scsynth: prevent possible buffer overflow
          cmake build system: fix x11 include paths
          class library: Bus - fix get method for multi-channel busses
          class library: Server.scope - remove limitation to 16 channels
          plugins: LocalOut - don't crash server if LocalIn is missing
          sclang: prevent buffer overflow
          scide: link with librt

    Victor Bombi (1):
          supernova: CMakeLists.txt must set include dirs for fftw3f

    attejensen (1):
          Update MIDI.schelp


SuperCollider v3.6.3, released 2013-02
======================================

    Dan Stowell (2):
          Add cmake option NO_GPL3 to simplify building of GPL2 binaries
          SCDoc: generalise licensing from GPL3+ to GPL2+

    Graeme Urquhart (2):
          Issue #702 fix: sendSelection receives NSString
          String:Help of split method matches implementation

    Jakob Leben (24):
          qtcollider: relicense to GPL version 2
          sclang: terminal client - fix and simplify request handling
          qtcollider: support String:-speak when Speech class is
             available
          cmake: set LIBSCSYNTH=ON by default on Windows
          qtcollider: QView - do not block beginDrag if currentDrag is
             already set
          qtcollider: QKnob - let 'background' affect knob color
          sc ide: improve server boot/quit actions
          sc ide: improve interpreter start/stop actions
          sc ide: improve default server booting shortcuts
          qtcollider: sf view: fix loading non-power-of-two floating
             point files
          sc ide: disable zooming by mouse wheel (scrolling)
          sc ide: editor - set Qt::WA_MacNoClickThrough on viewport
          help: improve the SC IDE guide
          qtcollider: implement QtGUI:*cursorPosition
          class library: Platform - redirect getMouseCoords to GUI
          sc ide: post window - disable click-through on Mac OS X
          sc ide: add Help menu action to open the SuperCollider IDE
             guide
          help: SC IDE guide - show scaled screenshot, with a link to
             unscaled one
          sc ide: docklets - fix geometry after undocking
          sc ide: change default shortcuts for Go To Next/Previous Region
          sc ide: make cmd-period silent
          sc ide: improve status box context menu interaction
          sc ide: add context menu to interpreter status box

    James Harkins (4):
          Fix title:: tags in the practical guide: user-friendly titles
             vs. filenames
          Add Practical Guide Cookbook entry on swing rhythms
          PG_Cookbook_08: Fix an omitted copy/paste
          Fix typo in analysis example: BufWr.ar on a kr signal is bad

    Tim Blechmann (22):
          supernova: fix crash on /quit with portaudio
          class library: PlusFreqScope - survive server actions
          scide: remove ctrl-b shortcut
          class library: FreqScope - fix for starting scope after booting
          common: introduce new autogenerated SC_Version.hpp header
          class library: fix Array-unlace
          supernova: plugin interface - guard access to rt-pool
          plugins: IOUgens - prevent buffer overflow
          Help: BrownNoise - use a convention of -20db
          supernova: sized array - assert boundaries
          supernova: sndfile backend - correctly use correct blocksize
             for temp buffer
          supernova: jack backend - avoid uninitialized value
          supernova: nrt engine - nicer formatting of message log
          plugins: ui ugens - initialize libx11 for threading
          supernova: start dsp threads from run methods
          sclang: library config - correcty handle library config command
             line argument
          server plugins: RecordBuf - fix multichannel corruption and
             buffer overrun
          fftlib: for now we avoid intptr_t
          server plugins: fix div_ka
          plugins: osc ugens - fix GET_TABLE macro
          plugins: OscUGens - ensure buffer initialization
          scide: add menu item to open the user application support
             directory

    Victor Bombi (2):
          common: win32 - avoid integer truncation
          supernova: correctly print synthdef path


SuperCollider v3.6.2, released 2012-12
======================================

    BS Collist (1):
          qtcollider: QEnvelopeView - add method to return all selected
             indexes

    Jakob Leben (32):
          common (windows): unify access to known folder paths
          sclang (windows): add primitive to access "My Documents" dir
          cmake: expand the search for libsndfile and libfftw3f
          cmake (Windows): use CMAKE_LIBRARY_PATH for fixup_bundle()
             search dirs
          scide: let cmd-period have an application-wide shortcut context
          scide: DocumentManager - refresh cached file info before
             storing save time
          scide: help browser - support doc/impl/ref lookup for selected
             text
          scide: search widget hierarchy upwards for first handler of
             lookup actions
          scide: GenericLookupDialog - no need for subclassing QTreeView
             anymore
          scide: make doc/impl/ref lookup shortcuts work on detached
             docklets
          scide: always pop up lookup dialogs with the active window as
             the parent
          scide: update translation sources, add italian
          qtcollider: start drag within mouse event handler
          qtcollider: QStethoscope2 - reverse operation of horizontal
             zoom slider
          scide: GenericCodeEditor - set Qt::WA_MacNoClickThrough widget
             attribute
          scide: SyntaxHighlighter - swap QChar::toAscii() for toLatin1()
          scide: Document - swap QString::toAscii() for
             QString::toLatin1()
          scide: MainWindow - substitute deprecated
             QFileDialog::setFilter(QString)
          scide: MainWindow - include QMimeData
          scide: PostWindow - include QMimeData
          scide: GenericCodeEditor - include QMimeData
          qtcollider: QWidgetProxy - include QDrag
          sclang: SCIpcClient - fix includes
          cmake: sclang - fix building when SC_IDE=ON and SC_QT=OFF
          cmake: scide - add QtNetwork to required Qt modules
          qtcollider: QStethoscope2 - refactor for robustness
          qtcollider: QListView - add 'selection' method to get all
             selected indexes
          help: document new 'selection' methods of EnvelopeView and
             ListView
          help: View - improve documentation, fix links
          help: fix a large amount of broken links due to changes in
             SCDoc
          cmake: FindPortmidi - actually implement auto-finding portmidi

    James Harkins (1):
          Fix bug introduced by 7f29d322: Don't free the same alloc'ed
             index twice

    Tim Blechmann (18):
          scide: DocumentManager - read files via QTextStream to decode
             characters
          supernova: osc handler - fix completion message and done
             message for /b_close
          supernova: asynchronous log - fix string splitting
          supernova: compile fix
          supernova: send /fail messages on /notify commands
          supernova: send /fail on buffer commands
          supernova: fix sndfile error handling
          win32: ensure stack alignment
          plugins: fix GrainBuf cleanup
          Help: SymbolicNotations - replace SCSlider with Slider
          supernova: plugin interface - protect against multiple done
             actions
          Help: remove memStore
          class library: Buffer - freeMsg should clear all cached
             information
          supernova: osc interface - fix bug with node reordering
          supernova: buffer_read - don't check samplerate when queueing
             soundfiles
          class library: fix Function.plot
          plugins: RecordBuf - fix recordbuf overrun & fix done action
             handling
          Help: RecordBuf - RecordBuf is recording, not playing


SuperCollider v3.6.1, released 2012-11
======================================

    Dan Stowell (1):
          SpecFlatness: prevent NaN output for silence (thanks nick
             collins)

    Glen Fraser (1):
          scide: code editor / post window - copy using plain text

    Jakob Leben (13):
          update README_WINDOWS.txt for changed application data
             locations
          fix compilation with MinGW (broken static initialization)
          scide: find/replace - use Qt translation system to handle
             singular/plural
          cmake: scide - improve handling translations
          scide: load translations from app resource directory
          scide: update translation source files
          scide: change english translation file name to serve as
             fallback
          sclang: (Windows) change app support dir from roaming to local
          scide: load fallback translation in addition to locale
             translation
          sclang: add primitive to allow Platform to access user home
             directory
          class library: WindowsPlatform - set a user-friendly default
             recordingsDir
          readme (windows): add instructions on moving application data

    Tim Blechmann (1):
          class library: SynthDef - writeDefFile should use default
             SynthDef path


SuperCollider v3.6.0, released 2012-11
======================================

Major release with many new features - please see the help doc
"News in 3.6" for more information.
http://doc.sccode.org/Guides/News-3_6.html


SuperCollider v3.5.7, released 2012-11
======================================

    Jakob Leben (6):
          sclang: (Windows) fix String:-getenv to return variables set
             with -setenv
          class library: ServerMeter - fix closing window when server has
             never run
          sclang: fix 'gcd' and 'lcm' methods
          qtcollider: QStethoscope2 - fix width of number boxes
          qtcollider: fix SoundFileView:-selectAll and -selectNone
          qtcollider: fix QPen:*matrix setter - combine instead of
             replace matrix

    Julian Rohrhuber (1):
          class library: jitlib - Avoiding sync problems with free/play

    Tim Blechmann (9):
          plugins: filters - fix initialization of filter parameters
          external libraries: nova-simd update
          external libraries: move nova-simd submodule to github
          plugins: DelayN - fix initialization code
          Revert "plugins: DelayN - fix initialization code"
          common: fftlib - increase size limit for ffts
          sclang: server shm interface - fix setting of multiple values
          plugin interface: provide wrapper class for c++-style unit
             generators


SuperCollider v3.5.6, released 2012-10
======================================

    Dan Stowell (2):
          Improve error messages when cmake can't find optional things
          Compile fix for Qt widget on arm.     Upstreamed from
             debian-multimedia (thanks Felipe Sateler)

    James Harkins (1):
          Fix Spawner bug: cleanup.update is mandatory, including rest
             events

    Jonatan Liljedahl (7):
          Quarks: fix typo and also open old-style helpfiles ending with
             .htm
          Include old non-converted helpfiles in SCDoc document index
          HelpBrowser: also open RTF files with whatever is available
          Even more support for old help files
          scdoc: use JS hasOwnProperty instead of testing the property
             directly
          HelpBrowser: post javascript errors
          SCDoc: properly escape keys in generated docmap.js

    Joshua Parmenter (1):
          Fix ServerOptions instance var ordering, etc., to make internal
             server booting use correct number of audio bus channels.

    Tim Blechmann (4):
          cmake: provide explicit option to use system-installed boost
             libraries
          external libraries - revert submodule updates
          lang: SerialPort - fix invocation of done action


SuperCollider v3.5.5, released 2012-09
======================================

    Dan Stowell (1):
          Fix bug in Complex:exp

    James Harkins (1):
          Convert misleading and confusing OSC-style example into
             object-style

    Joshua Parmenter (2):
          fix IEnvGen kr UGen
          fix cocoa window alpha setting

    Tim Blechmann (12):
          sclang: fix Array:extendWrap for negative size argument
          sclang: array primitivies - protect all array extend primitives
             against negative sizes
          scdoc: fix string comparison in parser
          supernova: sized_array - don't allocate memory for zero-sized
             array
          plugins: GrainBuf - fix crash when using nan as position
             control
          scsynth: ensure alignment of wire buffers
          supernova: catch exceptions when reading synthdefs
          supernova: free_aligned - fix fallback implementation for null
             pointers
          cmake build system: dont compile shared library with
             -fwhole-program
          plugins: GrainBuf - allocate grain after reading window
          plugins: GrainBuf - fix access to default hann window

    Victor Bombi (1):
          cpu usage for portaudio_backend.hpp


SuperCollider v3.5.4, released 2012-08
======================================

    Dan Stowell (5):
          Fix typo that causes build fail on big-endian archs, thanks
             Felipe Sateler
          fix build on ARM (where qreal==float); thanks Felipe Sateler
          Strip gremlin characters from JITLib wrapForNodeProxy.sc
          choose clipping rather than wraparound for writing
             integer-format audio     files (libsndfile setting)
          arm build fix: another double->qreal in QcMultiSlider

    James Harkins (1):
          Improve documentation of GUI kits and kit switching

    Jonatan Liljedahl (2):
          SCDoc: Use proper static string constants instead of comparing
             string literals.
          Revert "reinstate Mix.arFill and Mix.krFill for backward
             compatibility reasons"

    Julian Rohrhuber (2):
          reinstate Mix.arFill and Mix.krFill for backward compatibility
             reasons
          improve string helpfile

    Tim Blechmann (10):
          plugins: GrainUGens - handle unallocated window buffers
          plugins: GrainBuf - reject multi-channel buffers
          plugins: grain ugens - treat empty window buffers correctly
          server: provide memory alignment wrappers for msvc
          server: scsynth - ensure correct deallocation of SndBuffer
             memory
          server/language/supernova: automatically clip integer audio
             files
          scsynth: correctly free aligned buffers
          Help: fix OSC function in SendPeakRMS help file
          package: use alternative implementation of git-archive-all

    Victor Bombi (1):
          MSVC fix


SuperCollider v3.5.3, released 2012-06
======================================

    Dan Stowell (6):
        LocalIn helpfile fix, thanks Bruno Ruviaro
        Fix scvim regsitry file for updated filename (thanks Carlo Capocasa)
        version number to 3.5.3
        Server helpfile: see-also reference docs
        SCVim.sc should not be executable
        cmake build system: use system boost libraries if available

    Jakob Leben (1):
        cmake: fix Boost Thread linking on Windows

    James Harkins (10):
        EnvGen_next_ak_nova: Hardcoded blocksize=64, change to
        inNumSamples
        Per Scott W., initSiblings is not needed
        Reinstate Mix.ar and Mix.kr, with rate checks
        Fix crossplatform fail: Scale.directory shouldn’t always depend
        on Document
        ListPatterns: offset.value omitted (inval) as an argument
        Fix PbindProxy:storeArgs - should NOT call “source” on keys in
        the array!
        Scale:degreeToRatio should handle degrees outside of one
        octave’s range
        More meaningful error message for too many selectors
        Explain the limitation on the number of selectors in one
        FunctionDef
        Correct spelling error

    Jonatan Liljedahl (3):
        Methods.html: auto-redirect to Search if method not found
        SCDoc: fix detection of old format class docs
        Mix.ar was un-deprecated, so remove the deprecated method

    Joshua Parmenter (2):
        fix scroll view problem for OS X 10.7.4
        update SC_DirUtils to look at the name of the app bundle on osx

    Julian Rohrhuber (14):
        fix bugs due to wrong usage of partial application
        PV_BinShift helpfile improved
        PV_Diffuser helpfile improved
        reformat statement for readability (no change of functionality)
        helpfile improvements
        improve array helpfile
        add note to the loop argument of DiskIn (thanks Stefan).
        improve helpfile
        some helpfile improvements
        improve helpfile
        improve helpfile
        improve and simplify FFT overview helpfile: fix some errors in
        examples.
        improve and simplify IFFT helpfile.
        improve and simplify FFT helpfile, mention that hopsize must be
        larger than 0.0

    Tim Blechmann (11):
        external libraries: update nova-tt (gcc 4.7 fix)
        supernova: correctly implement replace semantics for /s_new
        Help: Function.scope is not limited to OSX anymore
        cmake build system: locate server plugins on freebsd
        server: add support for RF64
        cmake build system: ensure boost include path for scsynth
        cmake build system: set boost library path
        cmake build system: link scapp with correct version of
        libboost_thread
        cmake build system: minor cleanup
        supernova: fix asynchronous commands for empty reply address
        common: fix non-apple builds


SuperCollider v3.5.2, released 201
======================================

    Dan Stowell (3):
          Remove outdated Japanese menus
          Cannot use indentation for CMAKE example - on mac it is rendered
             as     &nbsp; which then breaks cmake compilation
          Fix bug in FFT library if winsize != audiosize

    Jakob Leben (21):
          qtcollider: fix QTextView:-background and
             QSoundFileView:-background
          cmake: improve message if Qt4 or one of its components not found
          qtcollider: QKnob: fix mouse response when mouseOverAction is set
          qtcollider: implement missing QPopUpMenu:-background
          qtcollider: QTextView fixes and improvements
          help: add missing GUI examples
          qtcollider: support use of UTF-8 encoded strings
          qtcollider: QTextView: improve -enterInterpretsSelection
          qtcollider: QTextField: never propagate Enter to parent
          qtcollider: QEnvelopeView: improve node selection API and UI
          help: update EnvelopeView documentation
          help: fix incorrect info in EnvelopeView documentation
          qtcollider: QObject:-getProperty: turn an error into a debug
             warning
          qtcollider: implement drag-and-drop for data outside SC
          qtcollider: improve key propagation in QListView and QTreeView
          qtcollider: optimize view instantiation (take 2)
          qtcollider: fix mouse wheel event being forwarded to SC for no
             reason
          qtcollider: fix potential null pointer dereference
          qtcollider: optimization - partially revert event handling
             changes
          qtcollider: optimization - avoid a signal connection at QObject
             construction
          qtcollider: optimization - avoid connecting signals with
             unnormalized signatures

    James Harkins (2):
          Fix Pcollect/select/reject:embedInStream to pass inval to the
             function
          setTheme: Inherit colors from parent theme if the user didn't
             specify

    Jonatan Liljedahl (41):
          scdoc: MathJax: don't use fonts installed on users computer
          New SCDoc parser and renderer. Faster, more stable, less buggy.
          fix some helpfiles for new scdoc
          scdoc.css update
          scdoc: scapp compile fix
          scdoc: defer indexAllDocuments until first use
          HelpBrowser tweaks
          scdoc: warn on additions for non-existent help doc
          scdoc: fill in argument names for argument:: with no name given
          SCDocRenderer: warn on broken links
          scdoc: fix classtree:: rendering bug
          scdoc: only warn on grouped methods argnames mismatch if
             argument:: tag is used
          scdoc: avoid GC error in primitive
          scdoc: collect metadata also from *.ext.schelp (doc additions)
          scdoc: warn if argument:: name does not match real method arg
          scdoc: updated SCDoc related docs
          scdoc: warn if classdoc title and filename mismatch
          scdoc: fix varargs name match warning
          scdoc: render getter/setter combinations as two different methods
          scdoc: warn if setter methods (trailing underscore) is documented
             explicitly
          scdoc: more helpfile fixes
          scdoc: fix some bugs, handle class docs with missing classes
          scdoc Search.html: match also on filename for 'title'
          schelp: fix some broken links
          scdoc: add clearCache arg to indexAllDocuments, and don't render
             undocumented classes more than once per session
          scdoc: updated SCDoc related helpfiles
          schelp: more doc error fixes
          scdoc: improve argument:: auto-fill and checks
          String-warn and -error: don't print newline after WARNING: and
             ERROR:
          scdoc: tweak warnings
          scdoc: fix escaping of :: in metadata parsing and block verbatim
          schelp: add keywords for scdoc tags in SCDocSyntax.schelp
          scdoc: allow end-of-file as newline terminator, and improve error
             messages
          scdoc: use setter_() syntax if more than one argument
          scdoc: render method arg defaults as "foo: val" instead of "foo =
             val"
          mention new scdoc implementation in News-3_5.schelp
          scdoc parser: allow empty lines before headertags
          SCDoc: fix escaping of & < and >
          SCDoc: fix inf loop at missing :: end-tag in code blocks
          SCDoc: allow EOF as terminator for private:: and similar tags
          SCDoc: don't warn on missing trailing mul & add args

    Miguel Negrão (1):
          [Class Libray] Quarks GUI - sort quarks by name

    Tim Blechmann (10):
          plugins: fix Clip.kr
          class library: archive TempoClock as compile string
          cmake build system: restrict win32-specific cflags to win32
          external libraries: nova-simd update
          external libraries: nova-simd compile fix
          plugins: fix StereoConvolution2L constructor
          scsynth: use aligned memory allocation functions from supernova
          external libraries: nova-simd update
          scsynth: provide zalloc as symbol

    redFrik (1):
          scdoc: fixed a bunch of helpfile errors


SuperCollider v3.5.1, released 2012-04
======================================

    Jakob Leben (13):
          windows: properly pass the SC version to NSIS
          qtcollider: QPopUpMenu: fix action triggering
          qtcollider: get rid of "X is not implemented" message
          class library: make Server:-plotTree resilient to GUI kit
             switching
          help: improve Stethoscope documentation
          class library: QStethoscope2: add missing class methods
          class library: fix UGen scoping on out-of-process servers
          class library: PlusFreqScope: simplify server checking
          class library: fix and improve various 'scope' and 'freqscope'
             methods
          help: fix Stethoscope:*isValidServer documentation
          class library: ServerMeter: fix synth startup and cleanup
          update README_WINDOWS.txt
          windows: improve building and installation

    Jonatan Liljedahl (6):
          lang11d: Fix parse tree generation of expr.(key:value, ...)
          SC.app: allow saving plain text .schelp files
          SCDoc: copymethod:: also search *.ext.schelp files
          Update News for 3.5 doc
          Fix typo in News-3_5.schelp and improve StartupFile.schelp
          Update WritingPrimitives.schelp regarding GC safety

    Joshua Parmenter (1):
          prevent HID crashes on OS X. Devices still aren't added to the
             queue though (longs for the locID aren't correctly set up)

    Scott Wilson (1):
          Make Unpack1FFT a subclass of UGen, rather than of PV_ChainUGen

    Tim Blechmann (4):
          class library: SynthDef - fix uploading of large synthdefs
          sclang: block evaluation typesafety
          sclang: signal primitives - fix Signal-fft


SuperCollider v3.5.0, released 2012-03
======================================

Major release with many new features - please see the help doc
"News in 3.5" for more information.
http://doc.sccode.org/Guides/News-3_5.html


SuperCollider v3.4.5, released 2012-01
======================================

    Tim Blechmann (7):
          class library: FreqScope fix
          sclang: fix crash of scpacket overflow by using exception handling
          sclang: pad PyrMethodRaw struct
          sclang: force size of PyrSlot to 16 byte and fix PyrMethodRaw size
          server plugins: fix div_ai_nova
          plugins: Resonz - fix initialization
          plugins: disable simd-optimization for tanh

    James Harkins (3):
          Explicitly show the command to uninstall (for scons idiots like me).
          (3.4) PathName now sets tmp directory using Platform
          SimpleController:update would throw error if no actions had been 'put' in

    Dan Stowell (1):
          Remove waf file from 3.4.x - was never used, and contains binary code,     causing linux packaging problems.     See ubuntu bug #529154 for details, and debian bug #529154 for     sc-specific

    Mathieu Trudel-Lapierre (1):
          Fixup environment variables used for linking against readline, libicu, curl, cwiid.

    Nick Collins (1):
          Fix bug in MFCC ugen

    Noe Rubinstein (1):
          Fix PMOsc doc: index -> pmindex

    dmotd (1):
          Include altivec.h on linux powerpc, fixing FTBFS


SuperCollider v3.4.4, released 2011-06
======================================

    Dan Stowell (4):
          Improve format of copyright/GPL notices (issue raised in debian pkging)
          Clarify Fontana copyright in MoogFF (and don't use keyword 'copyright'     in files where he doesn't have copyright)
          Update AUTHORS file
          Remove unneeded PDF (debian raised query over copyright)

    Nick Collins (1):
          Initial fix for headphones problem where plugging in or out headphones while using Built-in Output leads to loss of audio on OS X. Aggregate Devices not tackled at this point

    Tim Blechmann (15):
          sclang: mathematical operators - clip2 fix
          plugins: LPF - fix control-rate initialization
          sclang: wii - don't use address of temporary
          SCClassLibrary: ScoreStreamPlayer - do not add instances to server list
          scsynth: apple - set denormal handling flags, if __SSE__ is defined
          sclang: slotString - crash fix
          plugins: XLine - correct handling of done actions
          sclang: gc - introduce LazyCollect to avoid leak of frames and argument lists
          plugins: Pitch.ar - fix crash for high execution period
          changelog: fix version number
          update changelog
          sclang: parser - support message send syntax for unary operators
          plugins: delay ugens - rt memory allocation may fail
          sclang: compile fix


SuperCollider v3.4.3
======================================

    Dan Stowell (2):
          SC 3.4 set correct SOVERSION 1.0.0 for libs, and install more properly.     (Changes ported from downstream debian packaging.)
          lib SOVERSIONs back from 1.0.0 to 1, following debian-multimedia advice

    James Harkins (8):
          Fix nowExecutingPath bug in scel (never backported?)
          fix two bugs in NotificationCenter registerOneShot:
          fix corner case in ClassBrowser
          Fix asPseg bug for short curves array (which should wrap, not kill the stream)
          Clear dataptr when closing a file (so that isClosed answers correctly)
          Incorrectly used dataptr instead of fileptr in previous commit on this file
          replace old, unsafe Dictionary test with a safer (but less OOPy) test
          rats... I missed two others of the same

    Joshua Parmenter (1):
          update version number

    Tim Blechmann (3):
          scsynth: set ftz flag on osx
          two commits: (1) simplify access to the superclass tree in Class. (2) when looking for a code file (openCodeFile) or cmd-J, it is now enough to select a full line, instead of havin
          scons build system: libsclang build fix


SuperCollider v3.4.2, released 2011-03
======================================

Bugfixes:
---------

* 2010-06-05 fix Latch first sample output bug: if trigger > 0 initially, latch should not output 0 - jh
* 2010-09-04 fix firstArg behavior in BinaryOpUGen by a list-approved hack - jh
* 2010-10-01 fix SConstruct so that libscsynth and libsclang get SONAME entries - ds
* 2010-11-13 grainBuf: audio-rate trigger fix - tb
* 2010-11-15 generate libsclang and libscsynth with .so.1 extension (and soname) on linux - ds
* 2010-11-15 scons create symlinks from libX.so to libX.so.1 on linux, and install them - ds
* 2010-11-16 added .htm files to SConstruct as approved help file extension - mb
* 2010-11-28 compile fix for curl support - tb
* 2010-11-28 prevent asBus from breaking when called with no numChannels - jh
* 2010-12-03 grain ugens: demand ugen input fix - tb
* 2010-12-05 SystemClock and TempoClock sched and schedAbs with inf time doesn't schedule the task in the first place. backported from master - tb
* 2010-12-08 prString_FindRegexp fix: match char array was too short to hold null termination - jli
* 2010-12-11 fix classbrowser colors bugs. backported from master - tb
* 2010-12-12 fixes the bug where installed quark help files would not be detected - tb/ar
* 2010-12-13 mark inherited methods in class browser by background colour. backported from master - tb
* 2010-12-30 Pipe does not remove closed pipes from openFiles - jh
* 2010-12-30 fix String:rotate - pb
* 2011-01-02 unit generators: LagControl - fix initialization order - jh
* 2011-01-02 unit generators: LagControl - dynamically allocate buffer for filter states - tb
* 2011-01-07 fixed iOS compilation and backported changes from master branch - ab
* 2011-01-06 array primitives: fix allTuples and unlace - pb
* 2011-01-07 sclang: makeIntrinsicClass - correct bounds for memcpy - tb
* 2011-01-08 sclang: prString_FindRegexp - fill array after allocating objects - tb
* 2011-01-14 sclang: prString_FindRegexp ensure correct size of results array during gc calls - tb
* 2011-02-27 sclang: ensure minimum stack size - tb
* 2011-03-09 SCVim: avoid generating scvim help cache if not currently in scvim - ds
* 2011-03-11 fix the Event type 'note' (fixes rendering patterns to audio files) - rk


SuperCollider v3.4.1, released 2010-11
======================================

* 2010-07-12 remove accidental debug messages from SCView (on mac, posted a lot of info to Console, could affect performance) - ds
* 2010-07-11 Collections should behave as reasonably as possible when empty - some fixes to better this - jr
* 2010-07-11 SynthDef:add now sends to all running servers if no libname is given. SynthDescs are still added to the global SynthDescLib. If you want to handle multiple SynthDesc libs, you have to add the servers to each of them explicitly - jr
* 2010-07-12 PanAz: added support for audio-rate pos arg - lfsaw
* 2010-07-18 improved the sclang syntax highlighting parses - Patrick Borgeat
* 2010-07-30 Dreset UGen allows to reset the child UGens on its input - jr
* 2010-08-05 storeOn / asCompileString now simplifies its output. Default arguments that are given in the *new method anyhow are omitted - jr
* 2010-08-06 Dictionary merge and blend methods - jr
* 2010-08-09 method overwrite messages not posted by default, rather a message inviting people to run Main:overwriteMsg for the info - ds
* 2010-08-13 MethodOverride class to encapsule information on overridden messages, inviting people to run MethodOverride.printAll  - jr
* 2010-08-13 add size arg to Signal:zeroPad - jr and jh
* 2010-08-18 Pevent now uses default event if no event is passed in - jr
* 2010-08-18 added a shortcut to the rather tedious .asCompileString method. In analogy to object.postcs, object.cs returns the compile string - jr
* 2010-08-20 audio driver for scsynth running on Android (through JNI) - ds
* 2010-08-24 un-deprecate scsynth's ability to use internal "green" FFT lib, for embedded devices etc - ds
* 2010-08-28 no 'record' button for remote server GUIs, since path not generally known - ds
* 2010-09-02 token threading for sclang interpreter - tb
* 2010-09-07 when looking for a code file (openCodeFile) or cmd-J, it is now enough to select a full line, instead of having to select both words around the colon - jr
* 2010-09-07 added methods for better navigation in the class tree (findOverriddenMethod) - jr
* 2010-09-10 add method: Complex:abs to fit common usage - jr
* 2010-09-12 added Dwrand UGen - jr
* 2010-09-15 SystemClock and TempoClock sched and schedAbs with inf time doesn't schedule the task in the first place - jr
* 2010-10-07 change the mac HID error-handler code to output errors to sc post window rather than to mac log; removes a pascal-string issue - ds
* 2010-10-19 Ndef now releses its bus when server was quit or just booted - jr
* 2010-10-20 retain the path to the file in which an error has occurred and post it - jr


Bugfixes:
---------
* 2010-07-10 protecting the server against malformatted SynthDef names - jr
* 2010-06-28 syntaxColorize fix for double-backslashes, thanks Patrick Borgeat for the patch - ds
* 2010-07-24 catch crash in the case that one tries to define a unique method using a return value directly - jr
* 2010-09-07 UGen:clip, :wrap, :fold now apply correctly to scalar-rate signals; also methodSelectorForRate tweak for which class is asked - ds
* 2010-09-09 fix a bug for trigger signals in Demand.kr that hold longer than one control period - jr
* 2010-09-11 bug in audio rate mapping fixed, when new source object was inserted in a mapped node proxy - jr
* 2010-09-12 fix bug: 2994009. LFPar and LFCub audio rate modulation frequency argument work now - jr
* 2010-09-19 fix to JITGui, when numItems is not supplied - jr
* 2010-10-10 remove more crufty NSLog debug messages - ds
* 2010-10-13 fix SCUserView:receiveDrag to receive mouse co-ordinates; thanks Daniel van den Eijkel - ds
* 2010-10-19 debian-style scvim-check-if-plugin-is-active, brought upstream - ds
* 2010-10-19 bug in audio rate mapping fixed, when new source object was inserted in a mapped node proxy - jr
* 2010-10-19 partial fix for bugs item #2994009 - seems to fix LFPar but not LFCub. More work needed - ds
* 2010-10-19 DC: fix multichannel expansion - tb
* 2010-10-19 fix to demand rate unary op ugens, thanks james harkins - tb
* 2010-10-19 Ugens: LinLin/LinExp fixes - tb
* 2010-10-19 only /clearSched if RT - to fix tracker item #3033454 - tb
* 2010-10-19 UGens: binary operators - fix scalar/signal division - tb
* 2010-10-19 fix bug 2988525: SynthDef:writeDefFile appends path correctly - tb
* 2010-10-19 ProcessOSCPacket: fix possible deadlock - tb
* 2010-10-19 fix network address handling - albert graef
* 2010-11-05 fix memory issues in regular expressions: correct memory management in prString_FindRegexp - tb
* 2010-11-07 sclang: correct symlink handling - tb, ar

SuperCollider v3.4, released 2010-07
====================================

Headlines:
----------

* 2009-09-03 add support for Mac OS 10.5 and greater 64-bit builds of plugins and scsynth
* 2009-07-xx iphone support by Axel Balley added - ab
* 2009-07-21 EnvirGui added, a gui for livecoding/editing environments - adc
* 2009-07-24 Server.plotTree method for visualising the groups and synths on the server - sw
* 2009-07-31 mac osx text-completion feature now includes sclang objects - ds
* 2009-08-01 sclang now has a flag (Platform.ideName) for which IDE is in use (scapp, scvim, scel, sced, jsceclipse...) so that the same class-library can be used with different IDEs, enabling IDE-specific code as necessary - ds
* 2009-08-16 add emergency escape route: if sclang is caught in an infinite loop, send it a USR1 signal to break out of it - ds
* 2009-09-12 String:findRegexp and other regular expressions now available on linux as well as mac - mb,ds
* 2009-09-18 n_order and Server:reorder allow one to specify chains of nodes - sw
* 2009-09-20 simplify the Server recording interface. prepareForRecord is now optional (will be automatically invoked if you don't), and the server gui button is now just two-state "record" "stop" - ds
* 2009-10-04 support multichannel indices for Env:at - jr
* 2009-10-29 improve OSC message correctness: for convenience, sclang allows command names as symbols with no leading slash e.g. \g_new. To improve compliance with the OSC standard, the leading slash is now added to those symbols before dispatch - ds
* 2009-11-07 use nova-simd framework for performance improvements of unit generators - tb
* 2009-11-21 Event type \note supports polyphonic sustain, lag and timingOffset, and responds correctly to free and release. Add \grain event type. - jr
* 2009-11-28 windows: system "application support path", previously hardcoded as C:\SuperCollider, now settable by environment variable SC_SYSAPPSUP_PATH. Default setting for that env var (when using official wix bundle) will be [SC3INSTALLLOCATION] - ds
* 2009-12-15 sclang: 64-bit safety - tb
* 2009-12-15 sclang: performance improvement of math ops - tb
* 2010-01-02 scsynth: use osc-compilant address patterns for server/lang communication - tb
* 2010-01-24 add readline interface to sclang command-line. This is used by default when invoking "sclang" (to use the non-readline interface set the "-i" option to something other than "none") - ds
* 2010-01-24 enable GPL3 code by default - this 'upgrades' the overall binary license from GPL2+ to GPL3+, and allows supercollider to benefit from GPL3+ libraries such as libsimdmath and gnu readline  - ds
* 2010-02-04 Improvements to SC.app editor: Split pane documents, AutoInOutdent - sw
* 2010-02-18 scvim: now compatible with gnu screen, opens post window using screen, making it compatible with a pure-CLI environment - ds
* 2010-02-xx add the Deployment32-64 build style for building on OS X (10.5 and greater) - jp
* 2010-03-10 SynthDef:memStore deprecated in favour of the more coherent and typeable SynthDef:add - jr
* 2010-04-11 Moved some more experimental JITLib classes to "JITLib extensions" Quark - jr


Bugfixes:
---------

* 2009-06-12 fix for level indicator: critical and warning now display based on peak if it is shown rather than on value - sw
* 2009-06-18 fix for mouse coordinates bug - sw
* 2009-06-22 fix for negative bounds issue in SCUserView - sw
* 2009-06-23 avoid memory corruption when unknown OSC type tags are received. Instead forward them to sclang - jr
* 2009-06-23 Fix server crash with negative buffer numbers. - jr
* 2009-07-20 factors(): no prime factors exist below the first prime - jr
* 2009-07-21 Loudness ugen now supports LocalBuf - nc
* 2009-07-23 Fix very nasty bug in Pbindf: if a key is an array, new values were written into the incoming event, instead of the outgoing event - jh
* 2009-07-28 catch unintialised value in sc_GetUserHomeDirectory(), fixing potential memory corruption if HOME not set - ds
* 2009-08-01 SpecCentroid, fix its reaction to silence (output zero instead of NaN) - ds
* 2009-08-01 NamedControl: single default value now returns instance, not array, default values are obtained in a consistent way - jr
* 2009-08-04 fix the CPU-usage issue when calling plain "./sclang" from the terminal on OSX (seems it was caused by a bug in how OSX handles poll() calls) - ds
* 2009-08-15 LinPan2: fix initialisation issue - panning was not correctly applied during the first calc block - ds
* 2009-09-28 Workaround for faded colours in HTML docs - sw
* 2009-09-13 fix PV_MagShift argument handling, so that the defaults mean no-change, matching the behaviour of PV_BinShift - ds
* 2009-09-20 warn about weirdness of Float:switch - ds
* 2009-09-30 prevent NaN output from SpecFlatness when input is silence - ds
* 2009-10-16 fix cropping issue in printing SuperCollider.app documents - cq
* 2009-10-17 many phase-vocoder (PV_) ugens previously didn't handle the DC/nyquist bins as expected. fixed most of these (PV_MagAbove, PV_MagBelow, PV_MagClip, PV_LocalMax, PV_BrickWall, PV_MagSquared, PV_BinWipe, PV_CopyPhase, PV_Max, PV_RandComb) - ds
* 2009-11-01 fix audio rate arg problem in PlayBuf - jp
* 2009-11-02 fix amplitude-convergence issue in Pan2, Balance2, LinPan2, XFade2, which could sometimes result in sound despite zero amp, as discovered by jh - ds
* 2009-11-03 fix unsafe implementation of methods that allow sending collections to buffers - jr
* 2009-11-04 fix signalRange for MouseX, MouseY and KeyState, so that the range message works now - jr
* 2009-11-19 Fix for PV chains and LocalBuf - sw
* 2009-12-14 fix uninitialised variable in Pulse (could sometimes cause small glitch on init), thanks to rhian lloyd - ds
* 2010-01-10 Demand ugens can now handle more than 32 channels, thanks Patrick Borgeat for the patch - ds
* 2010-02-05 scsynth now respects the -D commandline option when running in NRT mode - ds
* 2010-02-11 Fix for nowExecutingPath with Routines - sw
* 2010-02-23 Performance fixes for SCUserView - sw
* 2010-02-25 Fix interpolation / indexing problem in VDiskIn that caused slight pitch fluctuations - jp
* 2010-03-11 SequenceableCollection:reduce no longer returns nil if the collection has only 1 element - ds
* 2010-03-28 fix memory leak of empty command line, for interactive sclang mode - tb
* 2010-03-29 main menu for Mac lang editor app: correction to key for evaluate selection, used to be return, now return+shift - nc
* 2010-04-19 fix missing font issue in Plotter -jr

Other additions/improvements:
-----------------------------

* 2009-06-11 Evaluate Selection menu command - sw
* 2009-06-23 allow remote apps to send type chars - jr
* 2009-06-27 build 32bit sclang on x86_64 - tb
* 2009-07-xx efficiency improvements on some UGens - tb
* 2009-07-xx improve Quarks use of svn for smoother user experience - ds
* 2009-07-22 catch the case when a user tries to compile into a synthdef, a unary/binary operator that the server can't apply - jh
* 2009-08-29 String:toUpper and String:toLower - ds
* 2009-09-06 Boolean:while now throws an informative error, since Boolean:while has no particular use but is often used in error by beginners in code where Function:while is intended - ds
* 2009-09-12 method FunctionDef:makeEnvirFromArgs allows to create template events from a function - jr
* 2009-09-30 Error is now posted if maxSynthDefs exceeded -sw
* 2009-11-03 TwoWayIdentityDictionary has a removeAt method now - jr
* 2009-11-04 update of deferredTaskInterval from 0.038 to 0.01667 - fo
* 2009-11-07 improved PyrSlot typesafety - tb
* 2009-11-23 menu system improvements in Windows IDE - mv
* 2009-12-13 tidyups for "sclang when on osx but not in sc.app" - ds
* 2009-12-13 added lincurve and curvelin methods for numbers and UGens - jr
* 2010-01-01 OSCresponder and OSCresponderNode respond equally to messages with or without preceding slash - jr
* 2010-01-04 sclang: deprecated Proutine - switch back to the original Prout
* 2010-01-06 UnitTest Quark improved, added script support - jr
* 2010-01-23 Improved NodeProxy and ProxySpace helpfiles. Added proxy composition syntax to NodeProxy - jr
* 2010-01-30 Make multichannel plotting easier. If no numChannels is given, find out automatically  - jr
* 2010-01-31 add new LOOP1 macro - tb
* 2010-01-31 use c99 log2 functions for sc_log2 - tb
* 2010-02-09 rearrangement of supercollider source code tree - ds
* 2010-02-11 Server:default_ now assigns to s by default. Settable with flag - sw
* 2010-02-27 removed SCAnimationView and added SCUserView:animate_ - fo
* 2010-03-10 SCPen:setSmoothing changed to SCPen:smoothing_, harmonised change with swingosc - ds
* 2010-03-23 exponentiation for Complex numbers - jr
* 2010-xx-xx many helpfiles improved - various authors
* 2010-03-30 Image class added, a redirect for SCImage or JSCImage - hr
* 2010-03-30 Pitch ugen ability to output clarity measure (by default not activated, for backwards compat) - ds

SuperCollider v3.3.1, released 2009-06-19
=========================================

Headlines:
----------

* 2009-05-11 SCWindow additions for visible, visible_, unminimize - cq
* 2009-05-17 server guis (on osx) now indicate which one is currently default - adc
* 2009-05-18 enabled control rate versions of Ball, TBall and Spring - mb
* 2009-05-18 LID support for setting "MSC" state as well as "LED" on devices - ds
* 2009-06-19 patched for compatibility with Safari 4, fixing a lockup issue when opening help docs - ar

Bugfixes:
---------

* 2009-05-11 fix keyword addressing for the order: argument - jmc
* 2009-05-15 update libsndfile to 1.0.20 to fix security issues (overflow vulnerabilities) in libsndfile - ds
* 2009-05-20 fix bug #2790649, "very large SimpleNumber:series can crash sclang" - ds
* 2009-05-25 mac icons for document types .quark .scd .rtfd were omitted from the app bundle, now fixed - ds
* 2009-06-02 EnvGen: fix off by one block latency in envelope attacks and releases - jr
* 2009-06-12 bug fix for level indicator: critical and warning now display based on peak if it is shown rather than on value - sw
* 2009-06-12 mouse coordinates fix, deprecate SCUserView:mousePosition - sw
* 2009-06-17 some issues fixed in SCUserView - cq
* 2009-06-20 fix redirect for Stethoscope - adc

Other additions/improvements:
-----------------------------

* 2009-05-05 fixes/improvements to cocoabridge primitives - cq
* 2009-05-06 SCImage various minor improvements - cq
* 2009-05-16 optimisation for scrollview drawing, remove VIEWHACK - sw
* 2009-05-xx various documentation updates - various
* 2009-05-xx various improvements to ubuntu-debian packaging scripts - ds, am
* 2009-05-20 SynthDef:writeOnce now available as an instance method as well as a class method - ds
* 2009-06-11 sc.app gets a menu command for "Evaluate selection" - sw
* 2009-06-17 adjusted SCKnob to use relative mouse coordinates - jm
* 2009-06-17 small fix to SConstruct to allow for new Debian X11 location when compiling on linux - mb
* 2009-06-19 Blip ugen: prevent sound blowup by never letting numharm be less than 1 - fo
* 2009-06-20 SCPen: fillStroke changed default from draw(4) to draw(3) - fo
* 2009-06-21 Fold, Clip and Wrap can now modulate the low and high inputs.

SuperCollider v3.3, released 2009-04-30
=======================================

Headlines:
----------
* 2008-04-08 scvim is now part of the distro - ds
* 2008-04-20 improvements to MIDI sysex handling - added sysex parsing directly in source - thanks to charles picasso
* 2008-07-12 scsynth on Mac can now use separate devices for audio input vs audio output. Thanks to Axel Balley for much of the work on this, also a bit by ds.
* 2008-07-12 PlayBuf, RecordBuf, BufWr, BufRd, ScopeOut - used to be limited to 16-channel audio maximum. Now can handle massively multichannel audio - ds
* 2008-07-19 Buffer:normalize method added - ds
* 2008-07-23 FFT and IFFT added option for zero-padding, by optional "framesize" argument - ds
* 2008-09-03 new VDiskIn ugen - jp
* 2008-10-08 SCImage for manipulating bitmap image objects (mac only) - ch
* 2008-10-09 LocalBuf system to allow synths to manage their own purely-local buffers - jr
* 2008-10-17 Added "-P" option to scsynth (accessible as s.options.restrictedPath) to allow restricting which paths scsynth is allowed to read/write - ds
* 2008-10-18 new PartConv ugen, performs efficient frequency-domain convolution - nc
* 2008-10-26 support on mac for "modal windows/sheets" (for user dialogs etc) - sw
* 2008-xx-xx various behind-the-scenes efficiency improvements, for a sleeker audio server that can do more on a given machine - various contributors
* 2008-11-01 add BEQSuite filter UGens (blackrain, jp)
* 2008-11-11 add Pfxb pattern - jr
* 2008-11-25 new EZPopUpMenu - jm
* 2008-11-29 Pitch ugen can now also track the pitch of control-rate signals - mb
* 2008-11-30 Drag and drop paths from Finder to Documents and SCViews - sw
* 2008-12-03 added PV_Div ugen for complex division - ds
* 2008-12-07 added PV_Conj ugen for complex conjugate - ds
* 2008-12-15 new ViewRedirect for easier cross-platform gui syntax. e.g. Window now redirects to SCWindow or JWindow. ds & jm
* 2008-12-15 revised and updated all SC Gui documentation. New gui introduction. New SCUserView subclassing tutorial. - jm
* 2008-12-15 the /done message for Buffer allocation/free/etc now also includes the buffer index - jt
* 2008-12-15 added methods to SCFreqScope for "special" SynthDef, and for visualising frequency responses - ds
* 2008-12-18 the main windows version of sc is now called "SuperCollider" rather than "PsyCollider" (although psycollider is the name of the code editor). SuperCollider on windows now has a different (better? who knows) installer, uses the main sc3 icon, and has some other tweaks that make it different from version 3.2 - ds
* 2008-12-19 new EZListView - jm
* 2009-01-02 sced (the gedit sc plugin) is now part of the distro - mb/artem
* 2009-01-06 SendReply UGen - jr
* 2009-01-06 VDiskIn sends file position to client - jr
* 2009-01-12 map audio to SynthDef controls. new OSC messages n_mapa and n_mapan. - jp, jr, rk
* 2009-01-13 relativeOrigin=true. SC's coordinate system in container views and user views are now by default relative.
* 2009-01-15 SCLevelIndicator view added - sw
* 2009-01-16 Scale and Tuning classes added - tw
* 2009-01-17 SuperColliderAU (scsynth as a Mac OSX "Audio Unit") added to main distribution - gr
* 2009-02-03 EZKnob revised and now part of distro - br, jm
* 2009-02-23 SystemActions refactored - jr
* 2009-02-23 SCMenuItem, SCMenuGroup, and SCMenuSeparator for user customisable menus - sw
* 2009-02-23 LFGauss UGen added - jr
* 2009-03-14 Added GeneralHID based patterns PhidKey and PhidSlot - mb

Bugfixes:
---------
* 2008-05-20 fix for the special case when 0.2.asFraction beachballs the lang (bug id 1856972) - jr
* 2008-05-20 fix slight mistake in the defaults printed by scsynth on command-line (bug id 1953392) - ds
* 2008-07-24 Routine / AppClock fix setting the clock of the thread (bug id 2023852) - jr
* 2008-09-16 stability fixes to FFT and IFFT - ds
* 2008-09-27 fix TExpRand.ar - ds
* 2008-11-11 SystemSynthDefs.numChannels can now be set from the startup file - jr
* 2008-11-24 avoid FFT failure when buffer not allocated - jr
* 2008-11-29 resolved inconsistency in Server:waitForBoot - function is always executed in a Routine, whether or not the server is booted - ds
* 2008-12-07 FlowView setting inital margin and gap fixed (bug id 1986059) - jh
* 2008-12-07 OSCpathResponder fixed (bug id 2021481) - jh
* 2009-01-08 b_readChannel fixed (bug id 1938480) - mb
* 2009-01-08 MIDIIn.connect on Linux fixed (bug id 1986850) - mb
* 2009-01-09 Tabbing in SCTextView - sw
* 2008-08-23 fix for sclang crashing sometimes when compiling erroneous code (bug id 2022297) - rb
* 2009-01-18 SCScrollView relativeOrigin glitch fixed (bug id 2508451) - jr, sw
* 2009-01-28 Fixed QuartzComposer view bounds bug - sw
* 2009-02-21 NodeProxy handles groups more consistently - jr
* 2009-04-16 asFraction fix by JMcC - jr

Other additions/improvements:
-----------------------------
* 2008-03-22 added open Method and link handling to SCTextView - sw
* 2008-04-04 SoundFile:toCSV - ds
* 2008-04-29 buffer UGens now post a warning (rather than failing silently) if buffer channels doesn't match num ins/outs - ds
* 2008-07-14 Deprecated rendezvous in favour of zeroConf - sw
* 2008-09-xx various code improvements, including compiling for 64-bit linux - tb
* 2008-10-03 improvements to standalone build - jp
* 2008-10-03 SCEnvelopeView remembers drawing order. - sw
* 2008-10-05 Maintain initial offset when dragging on an Envelope View node. This avoids nodes jumping to a new position on mouse down. - sw
* 2008-10-05 Enabled gridOn, gridResolution, gridColor, timeCursorOn, timeCursorPosition, and timeCursorColor for SCSoundFileViews. - sw
* 2008-10-31 thisProcess.pid - sclang now can know what its process id is - ds
* 2008-11-21 support for LocalBuf in FFT UGens - jr
* 2008-11-27 SC3 will ignore ugens/class-files in folders named "ignore". Previously the name has been "test" - ignoring folders named "test" is now deprecated and will be removed - ds
* 2008-12-06 Added Main:recompile to allow recompiling from code (SC.app only so far) - sw
* 2008-12-08 Added custom drag label for SCView - sw
* 2008-12-15 Buffer's done osc reply now includes the bufnum - jt
* 2008-12-20 Help tree in help menu (OSX) - sw
* 2008-12-24 EZSLider and EZNumber now have an enclosing containers, as well labelPosition =\left, \right, or \stack modes - jm
* 2009-01-03 Help browser text is editable/executable (CocoaGUI) - sw
* 2009-01-04 Escape exits modal and fullscreen states (OSX) - sw
* 2009-01-08 interface change to ProxySpace.stop (now stops all proxies, just like free/end/clear) - jr
* 2009-01-08 improved Ndef implementation, stores values in an internal ProxySpace, Ndef takes server names for multiple servers. - jr
* 2009-01-08 improved ProxyMixer implementation, added NdefMixer. - adc
* 2009-01-11 Added class browser to help menu (OSX) - sw
* 2009-01-20 New Cocoa based SCTextField - sw
* 2009-01-28 More helpful error string for operation cannot be called from this Process - sw
* 2009-02-23 CocoaDialog takes allowsMultiple arg rather than maxItems - sw


SuperCollider v3.2, released 2008-02-21
=======================================

Headlines:
----------
* 2007-11-xx new suite of machine listening ugens - Loudness, BeatTrack, Onsets, KeyTrack, SpecCentroid, SpecPcile, SpecFlatness - nc, ds
* 2008-01-06 FreeBSD compatibility - hb
* 2008-01-10 Quarks updating on OSX should now be easier for first-time users; commands are run in a separate terminal window - ds
* 2008-01-15 "Advanced find" in Mac interface - jt
* 2008-01-20 Buffer.copy changed to match other .copy methods - now copies language-side object rather than server buffer. Buffer.copyData can be used to copy data from one server buffer to another - jh
* 2008-01-20 - add volume controls to the Server and Server guis - jp
* 2008-01-xx Pattern library implementation changes, Pfx, Pbus, Pgroup etc. - rk, jr, jh
* 2008-01-26 TDuty outputs trigger first, not level. for backwards compatibility TDuty_old - jr
* 2008-02-03 moved the search location for "startup.rtf" on Mac - now searches in system, then user, "Application Support/SuperCollider" folders - ds

Bugfixes:
---------
* 2007-11-16 bug fixes for MIDIIn in connect/disconnect methods. split MIDIOut.sysex into user method and primitive (breaks with previous implementation). default value for uid arg in MIDIOut.new. - mb
* 2007-11-18 fixed a bug in prTry / protect - jr
* 2007-11-27 lock avoided in nextTimeOnGrid
* 2007-12-12 Node-setn fixed when using integers as control indices - jr
* 2008-01-16 fixed Pen: bug with fillRect, fillOval and fillColor (bugtracker id 1837775) - jt
* 2008-01-20 CheckBadValues rate-checking was too restrictive - ds
* 2008-01-20 fix for Saw and Pulse's offset noise on first instantiation, thanks to hisao takagi - ds
* 2008-01-26 TDuty / Duty does not drift anymore - jr
* 2008-02-07 Fixed hang and incorrect background drawing in Cocoa scrollviews - sw

Other additions/improvements:
-----------------------------
* 2007-11-16 MIDIOut.connect and disconnect - mb
* 2007-11-18 added T2A UGen - jr
* 2007-11-18 Refactoring of Document class, including new CocoaDocument class to handle the Cocoa-specific (SuperCollider.app) document management - ds
* 2007-11-18 More macros available in the plugin API for UGen programmers: GET_BUF, SIMPLE_GET_BUF, FULLRATE, RGET, RPUT - ds
* 2007-11-20 UnixPlatform:arch method - jp
* 2007-11-20 FFTTrigger UGen - a ugen to create "fake" (empty) FFT chains - jp
* 2007-11-21 StartUp protects its added functions from each other - if one fails this no longer prevents others from running - ds
* 2007-11-25 added Pclutch and moved StreamClutch to common - jr
* 2007-11-27 Function:inEnvir added - jh
* 2007-12-12 added Collection.flatIf - jr
* 2007-12-15 added control rate functionality to NumRunningSynths - jr
* 2008-01-08 martin rumori's DiskIn bugfix and loop enhancement - jp
* 2008-01-10 String:runInTerminal method - ds
* 2008-01-11 poll now works for scalar ugens - jr
* 2008-01-15 Collection:maxIndex and Collection:minIndex - nc
* 2008-01-24 Server.options.rendezvous to (de)activate Rendezvous if desired - ds
* 2008-01-24 demand ugens accept audio rate inputs correctly - jr
* 2008-01-26 added Dbufwr ugen, for writing to buffers from a demand ugen chain  - jr
* 2008-01-27 Main:version and associated methods for programmatically determining which version SC is - ds
* 2008-02-03 Server:defaultRecDir class variable, to allow user to specify default rec location - ds
* 2008-02-07 SCScrollView and SCScrollTopView no longer fire their action when scrolled programatically - sw


SuperCollider v3.1.1, released 2007-11-16
=========================================
Bugfixes:
---------
* 2007-11-09 re-organized the main help file - rb
* 2007-11-14 fix for .asStringPrec, to avoid crashes on intel systems for large precision values - jt

Other additions/improvements:
-----------------------------
* 2007-11-14 added a preprocessor to the interpreter - jr
* 2007-11-14 added a startup message specifying how to get help - rk


SuperCollider v3.1, released 2007-10-31
=======================================
(changes below are since 2007-09-22, for first ever point release)

Headlines:
----------
* 2007-09-27 SparseArray class added - jr
* 2007-09-28 Help.gui added - ds
* 2007-10-01 FFT and IFFT rewrite - now using more efficient libs, also allows user to vary the overlap and the window type, also large-sized FFTs are possible - ds
* 2007-10-02 UnpackFFT and PackFFT added - these allow for flexible frequency-domain manipulations inside synths - ds
* 2007-10-04 Pkey and Pif added - hjh
* 2007-10-05 reformed Patterns - all patterns accept patterns as arguments - jr
* 2007-10-08 change to UGen plugin loading fixes the audio dropout issue that various users have experienced - rb
* 2007-10-08 GeneralHID crossplatform HID wrapper - mb
* 2007-xx-xx many improvements to Quarks package-management system. gui improvements, dependency-handling improvements, etc - various
* 2007-10-20 added a Glossary file - sw
* 2007-10-xx various new help files added, and many help files improved - various
* 2007-10-26 changed Cmd-? to Cmd-D in lieu of the default help menu shortcut in Leopard. Also changed Cmd-Shift-K (clear post window) to Cmd-Shift-C to avoid accidental recompiles. - rb

Other additions/improvements:
---------------------------
* 2007-09-22 change log added, much rejoicing
* 2007-09-25 added packagesource.sh script to produce source code bundles - ds
* 2007-09-28 IdentityDictionary:doesNotUnderstand now warns if adding a pseudo-method which overrides a real method - jr
* 2007-09-28 String:openHTMLFile added - ds
* 2007-10-04 Integer:collect and Integer:collectAs methods added - ds/jr
* 2007-10-05 Dwhite:new and Dbrown:new have default values for lo and hi - jr
* 2007-10-10 SC no longer automatically writes data (synthdefs, archive.scxtar) to the application folder - instead writes to "app support". This fixes problems with running SC using an unprivileged user account - ds
* 2007-10-16 SequenceableCollection:median speed improvement, approx ten times faster in many cases - ds
* 2007-10-20 Object:deprecated and DeprecatedError added to allow for method deprecation - sw
* 2007-10-21 Amplitude : attackTime and releaseTime can be modulated now - jr
* 2007-10-25 Collection : histo method improved and moved from mathLib to common - jr
* 2007-10-30 improvements to cocoa Gui, including SCUserView improved to support layering and own draw hook - jt, sciss
* 2007-10-31 refactored Pbrown, added Pgbrown - jr

Bugfixes:
---------
* 2007-09-29 takekos bug fixed (obscure issue with garbage collection in arrays) - jm
* 2007-10-01 fixed off by one bug in Dswitch and Dswitch1 that caused a server crash - jr
* 2007-10-09 fixed deadlock and other problems in NSAttributedStringAdditions.m - rb
* 2007-10-11 fixed inaccurate automatic determination of whether SC is running as standalone - ds
* 2007-10-14 .quark files now saved correctly as plain-text, not RTF - ds
* 2007-10-24 fixed a bug in Pbeta - jp
