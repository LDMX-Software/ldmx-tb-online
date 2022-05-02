"""HexaBoard.hgcroc Python module"""

from Reformat import reformat

class HGCROCv2RawDataFile(reformat.RawDataFile) :
    """Configuration for HGCROC v2 raw data file"""
    def __init__(self, filepath) :
        super().__init__('HexaBoard','reformat::hexaboard::HGCROCv2RawDataFile')
        self.input_file = filepath
        self.name = 'HgcrocRawData'
