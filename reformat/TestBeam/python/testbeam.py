"""Test Beam configuration"""

from Reformat import reformat

def PolarfireRawFile(filepath,name) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::PolarfireRawFile',
            name = name,
            input_file = filepath,
            )

def WhiteRabbitRawFile(filepath) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::WhiteRabbitRawFile',
            name = 'WhiteRabbitRaw',
            input_file = filepath)

def FiberTrackerRawFile(filepath,name) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::FiberTrackerRawFile',
            name = name,
            input_file = filepath)
