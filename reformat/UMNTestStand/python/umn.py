"""UMN Test Stand configuration"""

from Reformat import reformat

class HGCROCv2RawDataFile(reformat.RawDataFile) :
    """Configuration for HGCROC v2 raw data file"""
    def __init__(self, filepath) :
        super().__init__('UMNTestStand','reformat::umnteststand::HGCROCv2RawDataFile')
        self.input_file = filepath
        self.name = 'HgcrocRawData'
