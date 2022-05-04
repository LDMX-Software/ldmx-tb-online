"""Test Beam configuration"""

from Reformat import reformat

def HGCROCv2RawDataFile(filepath) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'reformat::testbeam::HGCROCv2RawDataFile',
            input_file = filepath,
            name = 'HgcrocRawData'
            )
