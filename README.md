# swallow

[![Build Status](https://travis-ci.org/pusidun/swallow.svg?branch=master)](https://travis-ci.org/pusidun/swallow)

## Introduction

A c++ library

## Dependencies

boost

## Build

```
mkdir build && cd build
cmake ..
make
```

After make, shared library will be in ${project}/lib and binary file in ${project}/bin.

## Module

- log
- config
- coroutine
- redis

## Benchmark

1. log module

log_bench 300000
```
Async Benchmark
Elapsed:{3.2528 secs} 92228/sec
Sync Benchmark
Elapsed:{2.42646 secs} 123636/sec
```

## TODO

- [x] Travis-ci

- [x] Async log

- [ ] add test

- [ ] net library

## LICENSE

This code is released under MIT.

## Contribute to this repository

Issue reports and code fixes are welcome.
This repo obeys [Google C++ Style Guide](http://google.github.io/styleguide/cppguide.html).
