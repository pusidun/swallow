# swallow

[![Build Status](https://travis-ci.com/pusidun/swallow.svg?branch=master)](https://travis-ci.com/pusidun/swallow)

## Introduction

A c++ library

Only compile under Linux, Windows will be supported in future if necessary.

More details and API can be found in [wiki](https://github.com/pusidun/swallow/wiki)

## Dependencies

- boost
- gtest

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

CPU:AMD Ryzen 5 PRO 2500U w/ Radeon Vega Mobile Gfx
Mem:1G SCSI:25G

```
$benchmark/log_bench 300000
=================Async Benchmark=================
Elapsed:{2.27361 secs} 131948/sec
=================Sync Benchmark==================
Elapsed:{2.1495 secs} 139567/sec
=================All FINISHED====================
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

## Reference

[chenshuo/muduo](https://github.com/chenshuo/muduo)

[sylar-yin/sylar](https://github.com/sylar-yin/sylar)

[pusidun's blog: muduo notes](https://pusidun.github.io/tags/#muduo-ref)
