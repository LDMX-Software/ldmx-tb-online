# Reformat
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

## Alignment Procedure
Each RawDataFile provides the raw data for an event alongside a timestamp for the event.
The alignment procedure only considers the timestamp which is simply a 64-bit unsigned integer.
There is currently no definition of the units for this timestamp, so each RawDataFile needs
to make sure it is using the same units as any others where alignment is attempted.

Initialize empty queues for reach RawDataFile.

While there are still RawDataFiles providing events, we undergo the following procedure.
1. Pop the next event from each RawDataFile into their queue. If a RawDataFile provides the "end of file" event packet,
   remove it from the list of files that are being aligned.
2. Find the earliest timestamp at the front of each RawDataFile's queue - call this the reference time stamp.
3. Remove all event packets from the front of each queue that are withing `max_diff` of the referent time stamp.
  - This is guaranteed to remove at least one event packet (the one that has the earliest time stamp).
  - Since the units are undefined, the `max_diff` parameter will change depending on which RawDataFiles you are trying to align.
4. If all files provide an event packet (or if the user asked for all events to be kept with `keep_all`),
   then put the raw data from those event packets into the output EventFile and move on to the next output event.

## RawDataFile

Each `RawDataFile` that is attached to the `Converter` pops events from itself
and provides it to the alignment system via the pure virtual function
```cpp
std::optional<EventPacket> next()
```

This function has two purposes:

1. Defines an event and its data from the raw data file
2. Returns the `std::null_opt` if the raw data file is over

The implementation of (1) varies wildly depending on the type of raw data file you are using.

The most important comment about (2) is that the input raw data file is 
_removed from the processing list_ upon the first return of `std::null_opt`.
i.e. Returning `std::null_opt` is saying "I'm all done".

Since the empty braces provided to the constructor of `std::optional` defines the null option,
a common design pattern is
```cpp
std::optional<EventPacket> next() {
  // empty constructor for std::optional defines it as "not given"
  if (my_file_is_over) return {};

  EventPacket ep;
  /**
   * insert actual unpacking of event here
   */
  ep.append(my_data); // my_data is any int-type or vector of int-types
  ep.setTimestamp(my_timestamp); // my_time
  return ep;
}
```

Declaration of a new RawDataFile is done through a C++ macro:
```cpp
// at the bottom of the ClassName.cxx file
DECLARE_RAW_DATA_FILE(full::name::space::ClassName)
```

And configuration of a RawDataFile is done through a Python function:
```python
from Reformat import reformat

def MyDataFile(args) :
    return reformat.RawDataFile(
              module = 'ModuleName',
              class_name = 'full::name::space::ClassName',
              # put other input parameters here
              )
```

The module organization is identical to the modules used in ldmx-sw
(we actually use the same `cmake` macros for configuring the libraries).

## Building Comments
The default installation prefix is this directory,
but this installation prefix is not included as a path to load libraries/headers/python modules
from in the default ldmx-sw development container.
You can solve this by installing this package into the same location as ldmx-sw.
```bash
ldmx cmake -DCMAKE_INSTALL_PREFIX=$LDMX_BASE/ldmx-sw/install ..
```
**Note**: This causes issues if your ldmx-sw install uses a different Framework than this program.
This program uses Framework v1.0.2 which is used by only the most recent versions of ldmx-sw.
