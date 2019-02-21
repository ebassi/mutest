## µTest - A small C testing library

**Linux**: [![Build Status](https://travis-ci.org/ebassi/graphene.svg?branch=master)](https://travis-ci.org/ebassi/graphene)

**Windows**: ...

µTest aims to be a small unit testing library for C projects, with an API
heavily modelled on high level frameworks like Jasmine or Mocha.

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
$ ninja install
```

## Using µTest

### Adding µTest to your project

µTest provides a shared library and a [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
file to describe the necessary compiler and linker flags. If you are
using Meson:

```
mutest_dep = dependency('mutest-1')
```

And if you're using Autotools:

```
PKG_CHECK_MODULES(MUTEST, [mutest-1])
AC_SUBST(MUTEST_CFLAGS)
AC_SUBST(MUTEST_LIBS)
```

You can also depend on µTest as a subproject in existing Meson-based
projects through a wrap file:

```
$ cat >subprojects/mutest.wrap <<HERE
> [wrap-git]
> directory=mutest
> url=https://github.com/ebassi/mutest.git
> revision=master
> HERE
```

And using the `fallback` option in your Meson build:

```
mutest_dep = dependency('mutest-1', fallback: [ 'mutest', 'mutest_dep' ])
```

The sub-project route is strongly encouraged.

### Getting started

The µTest API is heavily influenced by frameworks such as [Mocha][mocha]
and [Jasmine][jasmine].

You start from a *suite*, by calling `mutest_describe()` with a string
containing the description of the suite, and a function pointer:

```cpp
MUTEST_MAIN (
  mutest_describe ("Hello World", hello_suite);
)
```

Inside each suite you define a *spec*, by calling `mutest_it()`, with a
string containing the description of the spec, and a function pointer:

```cpp
static void
hello_suite (mutest_suite_t *suite)
{
  mutest_it ("hello_world()", call_hello_world);
}
```

Each spec contains *expectations*, one for each condition that needs to be
satisfied:

```cpp
static void
call_hello_world (mutest_spec_t *spec)
{
  const char *str = hello_world ();

  mutest_expect ("Returns 'hello, world'",
  		 mutest_string_value (str),
		 mutest_to_be_string, "hello, world",
		 NULL);

  // You can chain multiple expectations together; they must
  // all be satisfied for the expectation to succeed.
  mutest_expect ("Contains all components",
  		 mutest_string_value (str),
		 mutest_to_start_with_string, "hello",
		 mutest_to_contain_string, ",",
		 mutest_to_end_with_string, "world",
		 NULL);

  // You can also negate the condition.
  mutest_expect ("Does not contain 'goodbye'",
  		 mutest_string_value (str),
		 mutest_not, mutest_to_contain_string, "goodbye",
		 NULL);

  // Or you can skip an expectation altogether.
  mutest_expect ("Should be skipped",
  		 mutest_string_value (str),
		 mutest_skip,
		 NULL);
}
```

Building the test case and running it will generate the following result:

```sh
$ ./hello

  Hello World
    hello_world()
      ✓ Returns 'hello, world'
      ✓ Contains all components
      ✓ Does not contain 'goodbye'

    3 passing (2 ms)
    0 failing

  Total
  3 passing (3 ms)
  0 failing

```

## License

µTest is released under the terms of the [MIT](./LICENSE.txt) license.
