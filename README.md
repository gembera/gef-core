# Why Gembera
C language is one of the oldest languages. It is widely used and at same time heavily criticized. Many modern languages want to be "C/C++ killer", like Rust, Golang, Zig etc. They all try to solve part of the problems, but far from perfect. Gembera does not want to invent another wheel. It try to achieve the goal from the architectural level.

Main features:
* Ultra light framework for universal usages, from IoT, software defined vehicles to cloud native WebAssembly
* Build-in memory profiling and tracing
* Concurrent programming based on coroutines
* Pure C OOP implementation
* Package management with npm

# How to build
Make sure you have installed [gef-cli](https://github.com/gembera/gef-cli). 

## Desktop 
* Debug version 
```shell
yarn debug make
yarn debug build
yarn debug test
```
* Release version
```shell
yarn release make
yarn release build
yarn release test
```

## WebAssembly
Install [wasienv](https://github.com/wasienv/wasienv) at first.
* Debug version 
```shell
yarn wasm debug make
yarn wasm debug build
yarn wasm debug test
```
* Release version
```shell
yarn wasm release make
yarn wasm release build
yarn wasm release test
```

# Sponsors
[<img src="https://www.carota.ai/logo-512.png" width="128">](https://www.carota.ai)
