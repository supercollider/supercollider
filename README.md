[SuperCollider](https://supercollider.github.io/) [![Build Status](https://github.com/supercollider/supercollider/actions/workflows/actions.yml/badge.svg?branch=develop)](https://github.com/supercollider/supercollider/actions/workflows/actions.yml)
=========================

**SuperCollider** is a comprehensive platform for audio synthesis and algorithmic composition, used by musicians, artists, and researchers working with sound. It comprises:

- **scsynth**, a real-time audio server featuring numerous unit generators ("UGens") for audio analysis, synthesis, and processing.
- **supernova**, an alternative server to scsynth, designed to support parallel DSP on multi-core processors.
- **sclang**, an interpreted programming language tailored to control the servers.
- **scide**, an integrated development environment (IDE) for sclang, which includes a built-in help system.

sclang also includes a package manager called Quarks, while both scsynth and supernova support third-party plugins via C and C++ APIs.

SuperCollider is developed using C++17 and relies on several third-party libraries, including Qt and Boost. It is compatible with Windows, macOS, various Linux and BSD distributions, Raspberry Pi, and BeagleBone Black.

Install
-------

For stable releases, macOS and Windows builds are available on our [downloads page][downloads page]. Please refer to the [macOS README](README_MACOS.md) and [Windows README](README_WINDOWS.md) for installation and build instructions.

Linux users can build SuperCollider from source to obtain the latest stable version. Detailed instructions are provided in the [Linux README](README_LINUX.md).

Instructions for building SuperCollider on Raspberry Pi and BeagleBone Black can be found in the [Raspberry Pi](README_RASPBERRY_PI.md) and [BeagleBone Black](README_BEAGLEBONE_BLACK.md) READMEs, respectively.

[downloads page]: https://supercollider.github.io/downloads

### Platform Support

SuperCollider is tested on:
- Windows 10 (32- and 64-bit) with MSVC 2019
- macOS 12 with Xcode 13.4.1
- Ubuntu 22.04 with gcc 12

SuperCollider is known to support the following platforms:
- Windows Vista, 7, 8, 10, 11
- macOS 10.14-12.x
- Ubuntu 18.04-22.04

A legacy macOS binary is available for macOS 10.11 and later, using Qt 5.9.

Supported platforms with guaranteed compatibility include:
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

For further details on platform support guarantees, please visit the [project Wiki](https://github.com/supercollider/supercollider/wiki/Platform-Support).

Learn
-----

Access the official documentation through the SuperCollider IDE's built-in browser or online at [doc.sccode.org](http://doc.sccode.org/).

Recommended resources for learning SC include:
- [A Gentle Introduction to SuperCollider](https://works.bepress.com/bruno-ruviaro/3/), a free ebook by Bruno Ruviaro
- [Eli Fieldsteel's video tutorials](https://www.youtube.com/playlist?list=PLPYzvS8A_rTaNDweXe6PX4CXSGq4iEWYC)
- [Getting Started with SC](http://doc.sccode.org/Tutorials/Getting-Started/00-Getting-Started-With-SC.html)
- [Nick Collins' SC tutorial](https://composerprogrammer.com/teaching/supercollider/sctutorial/tutorial.html)
- [SCCode.org](http://sccode.org/), a repository of user-submitted examples

Discuss
-------
Join our community on [scsynth.org][Discourse] or our [Slack channel][Slack].

You can also explore the archives of our old mailing lists at [sc-users][sc-users archives] and [sc-dev][sc-dev archives].

Please review our [code of conduct](CODE_OF_CONDUCT.md) which applies to all community interactions.

[list signup]: http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx
[sc-users archives]: https://www.listarc.bham.ac.uk/lists/sc-users/
[sc-dev archives]: https://www.listarc.bham.ac.uk/lists/sc-dev/
[Discourse]: https://scsynth.org/
[Slack]: https://join.slack.com/t/scsynth/shared_invite/zt-ezoyz15j-SVM7JVul94pxtDiUDRnd0w

Contribute
----------

SuperCollider's development takes place on GitHub, and we value contributions from the community. To get involved, read the following:

### [Code of Conduct](CODE_OF_CONDUCT.md)

Before contributing, please review our [code of conduct](CODE_OF_CONDUCT.md) to understand the expected behavior and community standards.

### [Contributing Guide](https://github.com/supercollider/supercollider/wiki)

Our [contributing guide](https://github.com/supercollider/supercollider/wiki) provides details on our development process, how to propose bug fixes and improvements, and how to build and test your changes.

### Good First Issues

For newcomers, we offer a list of [good first issues](https://github.com/supercollider/supercollider/labels/good%20first%20issue) featuring manageable bugs to help you get started. You can also seek guidance on our developer mailing list, [Slack][Slack], or the [forum][Discourse].

### Requests for Comment

SuperCollider employs a Requests for Comment (RFC) system to propose and discuss significant changes. Engaging with an existing RFC or submitting a new one is a great way to contribute, especially if you prefer not to code. More information is available at the [RFC repository](https://github.com/supercollider/rfcs).

License
-------

SuperCollider is licensed under Version 3 of the GNU General Public License. For more details, see [COPYING](COPYING).
