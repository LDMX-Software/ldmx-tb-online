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

def TrigScintEventGroupedRawFile(filepath,nsamples = 30, nchannels = 16, header_len = 4+4+4+3+1) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::TrigScintEventGroupedRawFile',
            name = 'TrigScintRaw',
            input_file = filepath,
            bytes_per_event = 2*nchannels*nsamples + header_len)

def TrigScintTwoFibersRawFile(filepath) :
    return reformat.RawDataFile(
            module = 'TestBeam',
            class_name = 'testbeam::TrigScintTwoFibersRawFile',
            name = 'TrigScintRaw',
            input_file = filepath)
