## µTest - A small C testing library

 - **Linux**: [![Build Status](https://travis-ci.org/ebassi/mutest.svg?branch=master)](https://travis-ci.org/ebassi/mutest)
 - **Windows**: [![Build status](https://ci.appveyor.com/api/projects/status/1ghtdpt42u3vy8s9/branch/master?svg=true)](https://ci.appveyor.com/project/ebassi/mutest/branch/master)

µTest aims to be a small unit testing library for C projects, with an API
heavily modelled on high level frameworks like Jasmine or Mocha.

**WARNING**: µTest's API is still in flux!

## Building µTest

Simply clone this repository, and use [Meson](http://mesonbuild.com) to
generate the appropriate build instructions:

```sh
$ git clone https://github.com/ebassi/mutest.git
$ cd mutest
$ meson _build .
$ cd _build
$ ninja
$ meson test
$ sudo ninja install
```

## Getting started

The documentation of µTest is [available online](https://ebassi.github.io/mutest/mutest.md.html).

## Contributing

See the [Contribution guide](./CONTRIBUTING.md).

## License

µTest is released under the terms of the [MIT](./LICENSE.txt) license.
