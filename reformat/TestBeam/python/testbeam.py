"""Test Beam configuration"""

from Reformat import reformat

class HGCROCv2RawDataFile(reformat.RawDataFile) :
    """Configuration for HGCROC v2 raw data file"""
    def __init__(self, filepath) :
        super().__init__('TestBeam','reformat::testbeam::HGCROCv2RawDataFile')
        self.input_file = filepath
        self.name = 'HgcrocRawData'
