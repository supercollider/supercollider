Purpose
=======

This document provides useful information for developers and maintainers of SuperCollider. It's a yellow-pages of sorts.

Practices
=========

Git Workflow
------------

You can find full information about our git workflow
[here](https://github.com/supercollider/supercollider/wiki/git-workflow-and-guidelines). In summary, we use the
_git-flow_ branching model with `master` as the stable branch, `develop` as unstable, and `x.y` branches for release
branches.

Milestones
----------

We use four tags to keep track of issues:

- Next patch (example: "3.9.1")
- Some patch (example: "3.9.x")
- Next minor (example: "3.10")
- Some minor (example: "3.x")

Patch-milestoned issues are those that will require bug fixes, while minor-milestoned issues are things like new
features and major changes that are better left for a minor release. The "Next patch"/"Next minor" milestones mark those
issues that we've decided _must_ be addressed before the next respective release. They're either the most painful bugs
or most requested features.

When, in the example milestones above, 3.10 is released, we would move all 3.9.x-milestoned issues to 3.10.x,
and then collectively decide which issues ought to move from 3.x to 3.11 and from 3.10.x to 3.10.1.

Deprecation
-----------

SuperCollider class library deprecations are currently made on a case-by-case basis using the `Object:-deprecated`
method. When a method or class is deprecated, it is moved to `SCClassLibrary/deprecated`. Deprecations are removed on a
case-by-case basis with each minor (3.x) release.

Standards for New UGens
=======================

UGens proposed for inclusion in the main SuperCollider project should meet more rigorous standards than those found "in
the wild." These are standards the community has agreed upon for new UGens:

- **General software quality concerns:**
  - UGens in core should never break backward compatibility.
  - UGens should be fully documented, with a clear explanation of what it does and an appropriate collection of
    examples. Be sure to document which UGen inputs are modulatable at which rates.
  - UGens should be efficient. SuperCollider takes pride in being easy on the CPU, and UGens should help support that
    reputation.
  - The UGen should be deemed useful enough to the general SC user base.
  - Each UGen should be in a separate C++ source file (or multiple source files if it's really that long).
  - Don't leave any unnecessary print statements lying around.
- **Safety:**
  - Check input rates in the sclang class.
  - UGens should be real-time safe. Ross Bencina's ["Real-time audio programming 101"]
    (http://www.rossbencina.com/code/real-time-audio-programming-101-time-waits-for-nothing) is required reading.
    Here is a quick and incomplete blacklist of dangerous features:
    - `malloc`/`free`/`new`: use `RTAlloc` instead.
    - `throw`/`catch`: use return codes instead.
    - `dynamic_cast`
    - System calls. These are complicated to handle and require use of an NRT thread.
  - Any calls to `RTAlloc` should check the result for `NULL`. This usually happens when there
    isn't enough real-time memory left. The server will crash if this check is not made.
  - The Ctor sample should be initialized. If this is not done, very nasty bugs can occur.
  - Zap dangerous values (subnormals, infinities, nans) in feedback loops to zero. SC provides a
    `zapgremlins` function that does this for you.
- **Utility:**
  - UGens should have both `.ar` and `.kr` methods if applicable.
  - Sample rate and block size independence should be maintained if applicable. For example, audio UGens shouldn't sound
    radically different if the sample rate is increased.
  - For audio UGens, control-rate inputs should be interpolated if applicable.
  - Don't arbitrarily make certain inputs nonmodulatable just for programming convenience -- carefully anticipate what's
    worth modulating. Either way, don't forget to document it.
- **Modernization and deprecated features:**
  - When writing a new UGen from scratch, it is recommended to use the modern C++ style seen in `SC_PlugIn.hpp`.
  - Don't use `mul` and `add` arguments. These were originally introduced for efficiency, but now `Foo.ar * 2 + 1` gets
    optimized into a `MulAdd`.
  - Don't use a `doneAction` argument. Set the done flag instead.

Tools
=====

Continuous Integration and Skipping Commits
-------------------------------------------

We have CI provided by AppVeyor (Windows) and Travis (Linux, macOS). If a commit changes _only_
non-schelp documentation, _without_ renaming, adding, or removing files, you may want to consider
adding `[skip ci]` to the commit message so it does not waste CI resources.

S3 Build Hosting
----------------

Builds of all commits to branches on the main project repository are uploaded to Amazon's S3 hosting service. This means that commits in Pull Requests are often not uploaded, which is the case if they originate from other respositories (i.e. forks). These builds are available for macOS, Windows x86, and Windows x86-64. To download the latest build of a branch named `topic/foo`, the URLs are of the form:

* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/osx/topic/foo-latest.html
* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/win32/topicfoo-latest.html
* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/win64/topicfoo-latest.html

Note that for the Windows builds only, the branch name is stripped of forward slashes.

To find a build for a specific commit (assuming it exists), use the full SHA hash of the commit. For example, the
binaries for commit
[8c3563a8065cb623087f267dfe50e228224a4572](https://github.com/supercollider/supercollider/commit/8c3563a8065cb623087f267dfe50e228224a4572)
are at:

* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/osx/SC-8c3563a8065cb623087f267dfe50e228224a4572.zip
* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/win32/SC-Windows-x86-8c3563a8065cb623087f267dfe50e228224a4572.zip
* http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/win64/SC-Windows-x64-8c3563a8065cb623087f267dfe50e228224a4572.zip

A build for a specific commit may not always be available: for instance, if the build was cancelled early or failed to
complete.

AppVeyor builds
---------------
Independently from Amazon S3 hosting service, AppVeyor stores its Windows builds temporarily for 6 months. In contrast to the Amazon S3 hosting, these builds are available for _all_ pull requests. The URL to these builds includes AppVeyor's job number and is unrelated to commit SHA, e.g.

`https://ci.appveyor.com/api/buildjobs/76sh4cu20rh50bp1/artifacts/art_folder.zip`

These builds may be accessed either from [AppVeyor's build history](https://ci.appveyor.com/project/supercollider/supercollider/history), or from individual pull request's page. For the latter, find section `All checks have passed` at the bottom of the page, toggle `Show all checks` and choose `Details` next to the AppVeyor entry.

On the page with the selected build, choose the desired job from the list (32bit vs 64bit), and then `artifacts` on the right. This will reveal a link to `art_folder.zip` which contains the build.

Boost Update Script
-------------------

Location: `external_libraries/extract_boost.sh`, `external_libraries/boost_sc_changes.patch`

### Purpose

Update Boost libraries packaged with SuperCollider, and apply the SuperCollider organization's patches for Boost.

### Usage

Should be run as soon as possible after a new release of Boost. See instructions in `external_libraries/README_BOOST.md`
for more information.

QtGUI Debug Capability
----------------------

### Purpose

Print verbose debug messages while diagnosing issues with the Qt GUI features.

### Usage

Only enabled on a debug build.

```sclang
QtGUI.debugLevel_(0) // 0 = no output, 3 = verbose
```

Test Suite
----------

### Purpose

Unit testing suite for SuperCollider language core library

### Usage

If developing on a version older than 3.9.0, download `UnitTesting` quark separately. Add the test suite folder to your
SuperCollider compile paths. Recompile. Run `UnitTest.runAll`, or run tests from the GUI via `UnitTest.gui`.

### Continuous automatic red/green testing via guard-sclang

Rather than manually running appropriate tests after editing a library class or a `UnitTest` class, you can use
[aspiers/guard-sclang](https://github.com/aspiers/guard-sclang) which will watch for file changes and automatically run
tests in reaction to them.  The steps to set this up are as follows:

- Make sure you have Ruby installed.
- Make sure you have [Bundler](http://bundler.io/) installed (usually this is as simple as running `gem install bundler`).
- Make sure `sclang` is somewhere on your `$PATH`.
- `cd $supercollider_source/tools/guard`
- `bundle install`

Now you should be ready to launch Guard via:

    bundle exec guard

Then start hacking on SuperCollider classes, and enjoy the immediate feedback!

The mapping between implementation classes and test classes is defined in `tools/guard/Guardfile`.  This is currently by
far from perfect, because it naively assumes a 1:1 mapping between class `Foo` and test class `TestFoo`.  However it
should be easy to make it more intelligent, even if you don't know Ruby; please feel free to contribute improvements!

`SC_DOC_RENDER` Target
----------------------

### Purpose

Render all schelp documents to HTML to check for warnings and errors.

### Usage

During configuration, pass `-DSC_DOC_RENDER=ON` to CMake. This provides a target called `doc` which can be built to
render all schelp documents:

    cmake .. -DSC_DOC_RENDER=ON # <other options>
    cmake --build . --target doc

qpm Test Runner
---------------

qpm is a Python-based Quarks package manager and test runner for SuperCollider. It is used by our CI services to run the
SuperCollider-based test suite. See https://github.com/scztt/qpm/tree/qpm-unit for more information.

Changelog-to-schelp script
--------------------------

Location: https://gist.github.com/brianlheim/443ae188dee8f7a85e7f34c04cc66d2b

### Purpose

Converts a changelog in markdown format into schelp format. A little extra work required but saves a lot of tedium. See
script for usage.

sclang Lexer, Parser, & Compiler Regression Test Suite
------------------------------------------------------

TODO

Specific Contribution Workflows
===============================

Updating translation files
--------------------------

To update translation files for the IDE, you'll first need to make sure Qt's `lupdate` program is in your path, then
build the `update_ide_translations` CMake target. This will overwrite the `.ts` files in `editors/sc-ide/translations`
to reflect the most recent source code.

Adding IDE translations
-----------------------

Use [Qt Linguist](http://doc.qt.io/qt-5/qtlinguist-index.html) to update the `.ts` files in
`editors/sc-ide/translations`. Qt Linguist can be included when you install Qt Creator or a general Qt distribution.

### Adding a new translation language

If you can't find the language you want to add translations for, first make sure you have `lupdate` and Qt Linguist, then:

1. Determine its two-letter [language code](https://www.loc.gov/standards/iso639-2/php/code_list.php).
2. Add a new filename for it in `editors/sc-ide/CMakeLists.txt`, in the section marked `# Translation files`.
3. Re-run the CMake generation phase (in your `build` directory, execute `cmake ..`)
4. Build the `update_ide_translations` CMake target to generate a `.ts` file
5. Add translations using Qt Linguist.

Adding to QtCollider widgets
----------------------------

These instructions detail how to add functionality to existing QtCollider widgets (such as TreeView and Knob). Each
widget typically corresponds to a single Qt class, and the simplest way to add functionality is by providing access to
an existing C++ function of the Qt widget.

1. Locate the C++ source for the widget in `/QtCollider/widgets/`.
2. Add the signature for the function in the header file.
3. Add the implementation for the function in the implementation file.
4. Locate the SC source for the widget in `/SCClassLibrary/Common/GUI/Base/`.
5. Add a suitably named new method in the class file that calls down to the QtCollider widget using `invokeMethod`.
6. Document the method in the corresponding schelp file in `/HelpSource/Classes/`.

Technical note: not all types can be transcoded between SuperCollider and QtCollider widgets. However, all primitive
types, some collection types, and a few class types are supported. The code which controls this can be found in
`QtCollider::MetaType::find( PyrSlot * )`.

### Example

[PR #3560](https://github.com/supercollider/supercollider/pull/3560) demonstrates how to do this by adding
`setColumnWidth` to `TreeView`:

1. The C++ files are `/QtCollider/widgets/QcTreeWidget.{cpp,h}`
2. The function signature is:

```cpp
Q_INVOKABLE void setColumnWidth( int column, int width );
```

3. The implementation simply calls up to `QTreeView`:

```cpp
void QcTreeWidget::setColumnWidth( int column, int width )
{
  QTreeWidget::setColumnWidth( column, width );
}
```

4. The SC file is `/SCClassLibrary/Common/GUI/Base/QTreeView.sc`
5. The new method simply forwards arguments to `invokeMethod`:

```supercollider
setColumnWidth { arg column, width;
	this.invokeMethod( \setColumnWidth, [column, width] )
}
```

6. The documentation is added to `/HelpSource/Classes/TreeView.schelp`:

```
METHOD:: setColumnWidth

ARGUMENT:: column
	The integer index of the column to modify
ARGUMENT:: width
	Integer width in pixels
```

Project Structure
=================

- **`.github`**: PR and issue template for GitHub
- **`.travis`**: scripts for Travis CI
- **`cmake_modules`**: Find and Config modules for CMake
- **`common`**: C++ files needed by multiple components of the project
- **`editors`**: Files for `scide` and editor extensions
  - **`sc-el`**: the SuperCollider Emacs package, `scel`
  - **`sc-ide`**: source for `scide`
  - **`sced`**: source for the SuperCollider `gedit` plugin, `sced`
  - **`scvim`**: the SuperCollider Vim package, `scvim`
- **`examples`**: SuperCollider code examples, packaged into release assets
- **`external_libraries`**: C and C++ third-party libraries
  - **`boost`**: root of Boost C++ sources
  - **`boost_sync`**: contains [Boost::sync](https://github.com/boostorg/sync), a synchronization lib which was never
    integrated into Boost proper.
  - **`hidapi`**: [HID API](http://www.signal11.us/oss/hidapi/) project, which `sclang` uses for HID capability. Our
    fork is significantly patched for cross-platform compatibility.
  - **`icu`**: IBM's [ICU](http://site.icu-project.org/) library
  - **`jackey`**: a simple header for Jack that makes working with Jack Metadata easier. Used by `scsynth` and
    `supernova`, but not in current builds (only when `SC_JACK_USE_METADATA_API` is defined, may be old work that was
    never completed)
  - **`libsndfile`**: header for [`libsndfile`](http://www.mega-nerd.com/libsndfile/), for working with audio file
    formats. Used by `sclang` and the servers
  - **`nova-simd`**: `nova-simd`, a framework for SIMD vector functions
  - **`nova-tt`**: `nova-tt`, a library for cross-platform thread synchronization
  - **`oscpack_1_1_0`**: [Oscpack](http://www.rossbencina.com/code/oscpack), a library for working with OSC packets.
  - **`portaudio_sc_org`**: SC's fork of [PortAudio](http://www.portaudio.com/), which has been modded for
    cross-platform work
  - **`portmidi`**: [PortMIDI](http://portmedia.sourceforge.net/portmidi/), a cross-platform library for MIDI I/O
  - **`simplejson-2.3.2`**: [simplejson](https://pypi.python.org/pypi/simplejson/), a Python package used by `sced`
  - **`yaml-cpp`**: [yaml-cpp](https://github.com/jbeder/yaml-cpp), a C++ library for YAML
- **`HelpSource`**: schelp source for SCDoc
- **`icons`**: contains icon files for the project
- **`include`**: C++ headers for client code, including server plugins
- **`lang`**: C++ source for `sclang`
  - **`LangSource`**: sources for the core of the language, including the interpreter
  - **`LangPrimSource`**: sources for SuperCollider primitive functions. Loosely organized by functionality.
- **`package`**: tools for preparing and packaging release assets, including changelog-related scripts
- **`platform`**: various platform-specific bits
- **`QtCollider`**: sources for the Qt GUI extensions to `sclang`
- **`SCClassLibrary`**: SuperCollider sources for the core class library
- **`SCDoc`**: C++ sources for SCDoc, the .schelp parser
- **`server`**: C++ sources for `scsynth`, `supernova`, and server plugins
  - **`plugins`**: sources for server plugins (UGens)
  - **`scsynth`**: sources for scsynth
  - **`supernova`**: sources for supernova
- **`sounds`**: sound files, packaged into release assets
- **`testsuite`**: test files
  - **`classlibrary`**: SuperCollider-language tests for the core class library
  - **`sclang`**: tests for `sclang`
  - **`server`**: C++ tests for `scsynth` and `supernova`
- **`tools`**: various tools useful for maintainers

Important files in the root directory are:

- **`.appveyor.yml`**: config file for AppVeyor CI
- **`.travis.yml`**: config file for Travis CI
- **`build_sclang_cfg.in`**: config file used for the special `SC_DOC_RENDER` build target
- **`CMakeLists.txt`**: main CMake file
- **`README_*`**: readmes for various platforms
- **`SCVersion.txt`**: the master versioning document. The version number is stored here and nowhere else.
- **`travis_test_run_proto.json`**: config file used by qpm to run tests in CI
