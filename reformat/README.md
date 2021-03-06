# reformat

Reformat raw data into a Framework EventFile.

This reformatting is not meant to decode any data, that is meant to be done downstream inside of ldmx-sw.
Instead, this set of modules is meant to be a starting point for future online event building.

## Building
The requirements of the software (currently) in this directory is ROOT, C++17, and Boost - 
all three of which are in the canonical "dev" containers that ldmx-sw is built with.

When updating ldmx-tb-online to use `reformat` for the first time, make sure that the cmake and Framework
submodules are initialized.
```
git submodule update --init
```

Building this software is similar to ldmx-sw.
```bash
cd reformat
mkdir build
cd build
ldmx cmake ..
ldmx make install
```

Browse the [list of issues labeled as 'question'](https://github.com/LDMX-Software/ldmx-tb-online/issues?q=label%3Aquestion) for answers to FAQs.

## Usage
This code-base is designed very similarly to the core Framework.
The `reformat` program is configured using a python script at run-time,
and the different input files can be dynamically loaded.
On-going notes and documentation can be seen in the [Reformat README](Reformat/README.md) document.

## Table of Contents

### Framework
The Framework module from ldmx-sw.

### cmake
Common cmake macros used in LDMX-Software.

### Reformat
This is the module that is the main interface between different serialization schemes (Rogue, Boost.Serialization,...)
and the Framework EventFile.

### TestBeam
The raw data files unpacking data recorded during April 2022 testbeam.
