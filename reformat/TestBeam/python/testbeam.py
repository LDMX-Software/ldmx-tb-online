"""Test Beam configuration"""

from Reformat import reformat

def PolarfireRawFile(filepath,name) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::PolarfireRawFile',
            name = name,
            input_file = filepath,
            )
