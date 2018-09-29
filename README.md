# utilities
[![Build Status](https://travis-ci.org/NivenT/utilities.svg?branch=master)](https://travis-ci.org/NivenT/utilities)

This is a collection of C++ classes that I've enjoyed writing and think are interesting for one reason or another. Nothing here is something that hasn't been done before, but for each "utility," it's my first time trying to figure out how to make it work in C++. 

This repository originated in the [jubilant-funicular](https://github.com/NivenT/jubilant-funicular) gaming engine, but I eventually decided to pull these pieces out into their own repository since they could be used more generally.

## Descriptions
For more details and example usage, check out the `utils` folder. However, for short descriptions, look below

Utility | Short Descriptions
--- | ---
TypeMap | A map that can store one value of each type (one `int`, one `MyClass`, one `int&`, etc.)
Wrapper | A template for creating wrappers that inherit operations from the inner type (e.g. `Wrapper<int>` can be added and subtracted and whatnot, and `Wrapper<std::string>` can be added but not subtracted)
Json | A type for storing an arbitrary JSON value.
Path | A type representating a path in an OS-independent way (disclaimer: not tested on Windows)

You can also check out the [documentation](https://nivent.github.io/utilities/html/index.html) for even more information.

## How to run tests
```bash
git clone https://github.com/NivenT/utilities
cd utilities
mkdir build
cd build
cmake ../
make
```

After that, to run all the tests you just need to call `make test`. The tests are built inside `tests_bin`, so if you want to run one individually, then do something like the following (from inside the build folder)
```bash
cd tests_bin/path
./path_tests
```