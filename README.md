[SuperCollider](https://supercollider.github.io/) [![Build Status](https://github.com/supercollider/supercollider/actions/workflows/actions.yml/badge.svg?branch=develop)](https://github.com/supercollider/supercollider/actions/workflows/actions.yml)
=========================

**SuperCollider** is a platform for audio synthesis and algorithmic composition, used by musicians, artists, and researchers working with sound. It consists of:

- **scsynth**, a real-time audio server with hundreds of unit generators ("UGens") for audio analysis, synthesis, and processing
- **supernova**, an alternative server to scsynth with support for parallel DSP on multi-core processors
- **sclang**, an interpreted programming language that controls the servers
- **scide**, an editing environment for sclang with an integrated help system

sclang comes with its own package manager, called Quarks. scsynth and supernova both support third-party plugins via C and C++ APIs.

SuperCollider is written in C++17 using several third-party libraries, including Qt and Boost. It can be used on Windows, macOS, a variety of Linux and BSD distributions, Raspberry Pi, and BeagleBone Black.

Install
-------

macOS and Windows builds for stable releases are provided at our [downloads page][downloads page]. See the [macOS README](README_MACOS.md) and [Windows README](README_WINDOWS.md) for instructions on usage, and how to build SC yourself.

To get the latest stable version, Linux users will need to build SuperCollider themselves. See the [Linux README](README_LINUX.md) for instructions.

See the [Raspberry Pi](README_RASPBERRY_PI.md) and [BeagleBone Black](README_BEAGLEBONE_BLACK.md) READMEs for instructions on building on those platforms.

[downloads page]: https://supercollider.github.io/downloads

### Platform support

SuperCollider is tested with:
- Windows 10 64-bit and MSVC 2022
- macOS 12 and Xcode 15.2
- Ubuntu 22.04 and gcc 12

SuperCollider is known to support these platforms:
- Windows 10, 11
- macOS 11-15
- Ubuntu 22.04-24.04


SuperCollider has guaranteed support for:
- Windows 10, 11
- MSVC 2019, 2022
- macOS 13-15
- Xcode 14-16
- Debian >= 11
- Ubuntu 22.04, 24.04
- Fedora 36, 37
- Arch Linux
- gcc >= 9
- clang >= 11
- Qt >= 6.2

There is still limited support for Qt 5.15. Further details are described in [README_MACOS.md](README_MACOS.md).

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
You can join our forum at [scsynth.org][Discourse] or our [Slack channel][Slack].

You can also view the archives for the old mailing lists at [sc-users][sc-users archives] and [sc-dev][sc-dev archives].

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
