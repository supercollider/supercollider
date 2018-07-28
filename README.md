[SuperCollider](https://supercollider.github.io/) [![Build Status](https://travis-ci.org/supercollider/supercollider.svg?branch=master)](https://travis-ci.org/supercollider/supercollider) [![Appveyor](https://ci.appveyor.com/api/projects/status/github/supercollider/supercollider?branch=develop&svg=true)](https://ci.appveyor.com/project/brianlheim/supercollider-mu8dk)
=========================

**SuperCollider** is a platform for audio synthesis and algorithmic composition, used by musicians, artists, and researchers working with sound. It consists of:

- scsynth, a real-time audio server, forms the core of the platform. It features 400+ unit generators ("UGens") for analysis, synthesis, and processing. Some of the audio techniques it supports include additive synthesis, subtractive, FM, granular, FFT, and physical modeling. You can write your own UGens in C++, and users have already contributed several hundred more to the sc3-plugins repository.
- sclang, an interpreted programming language, controls scsynth via Open Sound Control. You can use sclang for algorithmic sequencing, connecting your app to external hardware including MIDI controllers, or writing GUIs and visual displays. sclang has a stock of user-contributed extensions called Quarks.
- scide is an editor for sclang with an integrated help system.

Install
-------

macOS and Windows builds for stable releases are provided at our [downloads page][downloads page]. See the [macOS README](README_MACOS.md) and [Windows README](README_WINDOWS.md) for instructions on usage, and how to build SC yourself.

To get the latest stable version, Linux users will need to build SuperCollider themselves. See the [Linux README](LINUX_README.md) for instructions.

[downloads page]: https://supercollider.github.io/download

Learn
-----

The official SC docs can be viewed in the SuperCollider IDE's built-in help viewer. A copy is also hosted online at [doc.sccode.org](http://doc.sccode.org/).

We recommend the following third-party tutorials:

- [A Gentle Introduction to SuperCollider](https://works.bepress.com/bruno-ruviaro/3/), a free ebook by Bruno Ruviaro
- [Eli Fieldsteel's video tutorials](https://www.youtube.com/playlist?list=PLPYzvS8A_rTaNDweXe6PX4CXSGq4iEWYC)

Discuss
-------

You can [sign up for mailing lists][list signup], and view the archives for [sc-users][sc-users archives] and [sc-dev][sc-dev archives].

We also have a [forum][Discourse] and a [Slack][Slack].

[list signup]: http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx
[sc-users archives]: https://www.listarc.bham.ac.uk/lists/sc-users/
[sc-dev archives]: https://www.listarc.bham.ac.uk/lists/sc-dev/
[Discourse]: https://scsynth.org/
[Slack]: https://join.slack.com/t/scsynth/shared_invite/enQtMzk3OTY3MzE0MTAyLWY1ZGE1MTJjYmI5NTRkZjFmNjZmNmYxOWI0NDZkNjdkMzdkNjgxNTJhZGVlOTEwYjdjMDY5OWM0ZTA4NWFiOGY

Contribute
----------

Development of SuperCollider happens here on GitHub, and we are grateful to the community for contributing bugfixes and improvements. Read below to learn how you can take part in improving SuperCollider.

### [Code of Conduct](CODE_OF_CONDUCT.md)

Please read our adopted [code of conduct](CODE_OF_CONDUCT.md) before contributing, so that you can understand what actions will and will not be tolerated.

### [Contributing Guide](CONTRIBUTING.md)

Read our [contributing guide](CONTRIBUTING.md) to learn about our development process, how to propose bugfixes and improvements, and how to build and test your changes.


### Good First Issues

To help you get your feet wet and get you familiar with our contribution process, we have a list of [good first issues](https://github.com/supercollider/supercollider/labels/good%20first%20issue) that contain bugs which have a relatively limited scope. This is a great place to get started. You can also ask on our developer's mailing list or on [Slack][Slack].

License
-------

SuperCollider is free software available under Version 3 the GNU General Public License. See [COPYING](COPYING) for details.
