# reformat

Reformat raw data into a Framework EventFile.

This reformatting is not meant to decode any data, that is meant to be done downstream inside of ldmx-sw.
Instead, this set of modules is meant to be a starting point for future online event building.

## Building
The requirements of the software (currently) in this directory is ROOT, C++17, and Boost - 
all three of which are in the canonical "dev" containers that ldmx-sw is built with.

Building this software is similar to ldmx-sw.
```bash
cd reformat
mkdir build
cd build
ldmx cmake ..
ldmx make install
```

## Usage
This code-base is designed very similarly to the core Framework.
The `reformat` program is configured using a python script at run-time,
and the different input files can be dynamically loaded.
On-going notes and documentation can be seen in the [usage](usage.md) document.

## Table of Contents

### Framework
The Framework module from ldmx-sw.

### cmake
Common cmake macros used in LDMX-Software.

### Reformat
This is the module that is the main interface between different serialization schemes (Rogue, Boost.Serialization,...)
and the Framework EventFile.

### HexaBoard
A band-aid solution for converting the Boost.Serialization output of hexactrl-sw.
