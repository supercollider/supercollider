[SuperCollider](https://supercollider.github.io/) [![Build Status](https://github.com/supercollider/supercollider/actions/workflows/actions.yml/badge.svg?branch=develop)](https://github.com/supercollider/supercollider/actions/workflows/actions.yml)
=========================

**SuperCollider** serves as a comprehensive platform for audio synthesis and algorithmic composition, catering to musicians, artists, and researchers engaged in sound-related work. The core components include:

- **scsynth**: A real-time audio server featuring a diverse array of unit generators ("UGens") for tasks such as audio analysis, synthesis, and processing.
- **supernova**: An alternative server to scsynth designed to leverage parallel DSP capabilities on multi-core processors.
- **sclang**: An interpreted programming language responsible for controlling the servers.
- **scide**: An integrated editing environment for sclang with a built-in help system.

The `sclang` component introduces its package manager known as Quarks. Both `scsynth` and `supernova` support third-party plugins through C and C++ APIs.

SuperCollider is implemented in C++17, utilizing various third-party libraries such as Qt and Boost. It offers cross-platform support for Windows, macOS, a variety of Linux and BSD distributions, Raspberry Pi, and BeagleBone Black.

Install
-------

macOS and Windows builds for stable releases are provided at our [downloads page][downloads page]. See the [macOS README](README_MACOS.md) and [Windows README](README_WINDOWS.md) for instructions on usage, and how to build SC yourself.

To get the latest stable version, Linux users will need to build SuperCollider themselves. See the [Linux README](README_LINUX.md) for instructions.

See the [Raspberry Pi](README_RASPBERRY_PI.md) and [BeagleBone Black](README_BEAGLEBONE_BLACK.md) READMEs for instructions on building on those platforms.

[downloads page]: https://supercollider.github.io/downloads

### Platform support

SuperCollider is tested with:
- Windows 10 (32- and 64-bit) and MSVC 2019
- macOS 12 and Xcode 13.4.1
- Ubuntu 22.04 and gcc 12

SuperCollider is known to support these platforms:
- Windows Vista, 7, 8, 10, 11
- macOS 10.14-12.x
- Ubuntu 18.04-22.04

We also provide a legacy macOS binary for macOS 10.11 and above using Qt 5.9.

SuperCollider has guaranteed support for:
- Windows 10, 11
- MSVC 2017, 2019
- macOS 11, 12
- Xcode 11-13
- Debian >= 11
- Ubuntu 20.04, 22.04
- Fedora 36, 37
- Arch Linux
- gcc >= 9
- clang >= 11
- Qt >= 5.11

For more information on platform support guarantees, see the [project
Wiki](https://github.com/supercollider/supercollider/wiki/Platform-Support).

Learn
-----

The official docs can be viewed in the SuperCollider IDE's built-in documentation browser. You can also view them online at [doc.sccode.org](http://doc.sccode.org/).

We recommend the following resources for learning SC:

- [A Gentle Introduction to SuperCollider](https://works.bepress.com/bruno-ruviaro/3/), a free ebook by Bruno Ruviaro
- [Eli Fieldsteel's video tutorials](https://www.youtube.com/playlist?list=PLPYzvS8A_rTaNDweXe6PX4CXSGq4iEWYC)
- [Getting Started with SC](http://doc.sccode.org/Tutorials/Getting-Started/00-Getting-Started-With-SC.html)
- [Nick Collins' SC tutorial](https://composerprogrammer.com/teaching/supercollider/sctutorial/tutorial.html)
- [SCCode.org](http://sccode.org/), a repository of user-submitted examples

Discuss
-------

You can [sign up for mailing lists][list signup], and view the archives for [sc-users][sc-users archives] and [sc-dev][sc-dev archives].

We also have a [forum][Discourse] and a [Slack chat][Slack].

Please read our adopted [code of conduct](CODE_OF_CONDUCT.md), which applies to all the above communities.

[list signup]: http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx
[sc-users archives]: https://www.listarc.bham.ac.uk/lists/sc-users/
[sc-dev archives]: https://www.listarc.bham.ac.uk/lists/sc-dev/
[Discourse]: https://scsynth.org/
[Slack]: https://join.slack.com/t/scsynth/shared_invite/zt-ezoyz15j-SVM7JVul94pxtDiUDRnd0w

Contribute
----------

Development of SuperCollider happens here on GitHub, and we are grateful to the community for contributing bugfixes and improvements. Read below to learn how you can take part in improving SuperCollider.

### [Code of Conduct](CODE_OF_CONDUCT.md)

Please read our adopted [code of conduct](CODE_OF_CONDUCT.md) before contributing, so that you can understand what actions will and will not be tolerated.

### [Contributing Guide](https://github.com/supercollider/supercollider/wiki)

Read our [contributing guide](https://github.com/supercollider/supercollider/wiki) to learn about our development process, how to propose bugfixes and improvements, and how to build and test your changes.

### Good First Issues

To help you get your feet wet and get you familiar with our contribution process, we have a list of [good first issues](https://github.com/supercollider/supercollider/labels/good%20first%20issue) that contain bugs which have a relatively limited scope. This is a great place to get started. You can also ask on our developer's mailing list, on [Slack][Slack], or on the [forum][Discourse].

### Requests for Comment

SuperCollider uses a Requests for Comment (RFC) system to propose, design, and discuss 'big' changes. Submitting a new RFC or discussing an existing one is a great way to get involved, especially if you are looking for a way to contribute that doesn't involve writing code or using a terminal. You can learn more at the [RFC repository](https://github.com/supercollider/rfcs).

License
-------

SuperCollider is free software available under Version 3 the GNU General Public License. See [COPYING](COPYING) for details.
