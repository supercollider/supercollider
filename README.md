[SuperCollider](https://supercollider.github.io/) [![Build Status](https://travis-ci.org/supercollider/supercollider.svg?branch=master)](https://travis-ci.org/supercollider/supercollider) [![Appveyor](https://ci.appveyor.com/api/projects/status/github/supercollider/supercollider?branch=develop&svg=true)](https://ci.appveyor.com/project/brianlheim/supercollider-mu8dk)
=========================

**SuperCollider** is a platform for audio synthesis and algorithmic composition, used by musicians, artists, and researchers working with sound. It consists of:

- **scsynth**, a real-time audio server with hundreds of unit generators ("UGens") for audio analysis, synthesis, and processing
- **supernova**, an alternative server to scsynth with support for parallel DSP on multi-core processors
- **sclang**, an interpreted programming language that controls the servers
- **scide**, an editing environment for sclang with an integrated help system

sclang comes with its own package manager, called Quarks. scsynth and supernova both support third-party plugins via C and C++ APIs.

SuperCollider is written in C++11 using several third-party libraries, including Qt and Boost.

Install
-------

macOS and Windows builds for stable releases are provided at our [downloads page][downloads page]. See the [macOS README](README_MACOS.md) and [Windows README](README_WINDOWS.md) for instructions on usage, and how to build SC yourself.

To get the latest stable version, Linux users will need to build SuperCollider themselves. See the [Linux README](README_LINUX.md) for instructions.

[downloads page]: https://supercollider.github.io/download

### Platform requirements

The minimum supported version of macOS is 10.10 Yosemite; the minimum supported version of Windows
is Windows Vista.

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
[Slack]: https://join.slack.com/t/scsynth/shared_invite/enQtMzk3OTY3MzE0MTAyLTFmMTliMmU0NjIwYzgyZGY1MjgxNGI2MGUxMTNlMWVhZWQzZjFmOTJhNzg2NDdmNzZmM2JiYTkyMjRiNDBiOTk

Contribute
----------

Development of SuperCollider happens here on GitHub, and we are grateful to the community for contributing bugfixes and improvements. Read below to learn how you can take part in improving SuperCollider.

### [Code of Conduct](CODE_OF_CONDUCT.md)

Please read our adopted [code of conduct](CODE_OF_CONDUCT.md) before contributing, so that you can understand what actions will and will not be tolerated.

### [Contributing Guide](CONTRIBUTING.md)

Read our [contributing guide](CONTRIBUTING.md) to learn about our development process, how to propose bugfixes and improvements, and how to build and test your changes.

### Good First Issues

To help you get your feet wet and get you familiar with our contribution process, we have a list of [good first issues](https://github.com/supercollider/supercollider/labels/good%20first%20issue) that contain bugs which have a relatively limited scope. This is a great place to get started. You can also ask on our developer's mailing list, on [Slack][Slack], or on the [forum][Discourse].

### Requests for Comment

SuperCollider uses a Requests for Comment (RFC) system to propose, design, and discuss 'big' changes. Submitting a new RFC or discussing an existing one is a great way to get involved, especially if you are looking for a way to contribute that doesn't involve writing code or using a terminal. You can learn more at the [RFC repository](https://github.com/supercollider/rfcs).

License
-------

SuperCollider is free software available under Version 3 the GNU General Public License. See [COPYING](COPYING) for details.
