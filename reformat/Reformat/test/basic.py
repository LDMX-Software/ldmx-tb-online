"""Test core functionality by running dummy test file(s)"""

from Reformat import reformat

def TestFile(n) :
    return reformat.RawDataFile(
            module = 'Reformat',
            class_name = 'reformat::test::TestFile',
            num = n
            )

import argparse, sys

parser = argparse.ArgumentParser(f'ldmx reformat {sys.argv[0]}')

parser.add_argument('input_cfg',choices=['single','multi','misalign'])
parser.add_argument('--pause',action='store_true')

arg = parser.parse_args()

c = reformat.Converter('test_output.root')

if arg.input_cfg == 'single' :
    c.input_files = [TestFile(5)]
elif arg.input_cfg == 'multi' :
    c.input_files = [TestFile(5), TestFile(5)]
elif arg.input_cfg == 'misalign' :
    c.input_files = [TestFile(3), TestFile(5)]

if arg.pause :
    c.pause()
