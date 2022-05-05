"""Basic HGCROCv2RawDataFile reformatting configuration"""

from Reformat import reformat

import argparse, sys

parser = argparse.ArgumentParser(f'ldmx reformat {sys.argv[0]}')

parser.add_argument('--pf1')
parser.add_argument('--pf0')
parser.add_argument('--pause',action='store_true')
parser.add_argument('--output',default='reformatted.root')

arg = parser.parse_args()

c = reformat.Converter(arg.output)
c.term_level = 1
c.event_limit = 10
c.keep_all = False

from Reformat import testbeam
if arg.pf0 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf0,"Polarfire0Raw"))

if arg.pf1 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf1,"Polarfire1Raw"))

if arg.pause :
    c.pause()
