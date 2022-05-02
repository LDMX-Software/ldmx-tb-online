# usage
```
reformat {configuration_script.py} [arguments]
```

The design of this program is similar to Frameworks' `fire`. 
We are configured at runtime using a python script.
For someone familiar with `fire` a simple look-up-table will help you understand.

`fire` | `reformat`
---|---
`Process` | `Converter`
`EventProcessor` | `RawDataFile`

Each run of `reformat` needs to have a single `Converter` object created with
one or more input `RawDataFile`s attached. Each `RawDataFile` is dynamically loaded
in a similar manner as processors, so users can define their own types of raw data files
to be reformatted into an output EventFile.

The **crucial** difference between `reformat` and `fire` 
(and the main reason we needed a separate program) is that
`fire` requires the input EventFile or the configuration to define
the number of events. This is not well suited to reformatting data files
where the input files are not EventFiles but we still want them to define the number of events.

For this reason, the `Converter` continues to create new events until **all** RawDataFiles that
are attached report that they have no more events to add.

## RawDataFile

Each `RawDataFile` that is attached to the `Converter` is given the event bus
to add objects to via the pure virtual function
```cpp
bool next(framework::Event& event)
```

This function has two purposes:

1. Add objects for a single event to the event bus.
2. Return `true` if there are no more events to be added and `false` otherwise.

The implementation of (1) varies wildly depending on the type of raw data file you are using.
An example implementation is in the HexaBoard module. This implementation inserts headers
and computes checksums that are expected to be done by front-end boards but is not done by
the test-stand setup that outputs the raw data file.

The most important comment about (2) is that the input raw data file is _removed from the processing list_ upon the first return of `true`.
i.e. Returning `true` is saying "I'm all done".
Note: This last call to `next` which returns `true` can (and should) still add the last event to the event bus.

Declaration of a new RawDataFile is done through a C++ macro:
```cpp
// at the bottom of the ClassName.cxx file
DECLARE_RAW_DATA_FILE(full::name::space, ClassName)
```

And configuration of a RawDataFile is done through a derived Python class:
```python
from Reformat import reformat

class MyDataFile(reformat.RawDataFile) :
    def __init__(self) :
        super().__init__('ModuleName','full::name::space::ClassName')
        # put other input parameters and their defaults here
```

The module organization is identical to the modules used in ldmx-sw
(we actually use the same `cmake` macros for configuring the libraries).

## Building Comments
The default installation prefix is this directory,
but this installation prefix is not included as a path to load libraries/headers/python modules
from in the default ldmx-sw development container.

There are two solutions to this:
1. Use the [daq-env image](./../context/README.md) which includes this installation prefix
    as a path for loading if the `daq` repo is placed within `LDMX_BASE` (i.e. similar to ldmx-sw).
2. Change the installation prefix to the `ldmx-sw` one at configuration time.
```bash
ldmx cmake -DCMAKE_INSTALL_PREFIX=$LDMX_BASE/ldmx-sw/install ..
```
