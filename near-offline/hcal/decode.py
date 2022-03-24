"""Basic HGCROCv2RawDataFile reformatting configuration"""

import argparse, sys

parser = argparse.ArgumentParser(f'ldmx fire {sys.argv[0]}')

parser.add_argument('input_file')
parser.add_argument('--pause',action='store_true')
parser.add_argument('--max_events',default=100,type=int)
parser.add_argument('--pedestals',default=None,type=str)

arg = parser.parse_args()

from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('unpack')
p.maxEvents = arg.max_events
p.termLogLevel = 0
p.logFrequency = 1

import LDMX.Hcal.hgcrocFormat as hcal_format
import LDMX.Hcal.digi as hcal_digi
import LDMX.Hcal.HcalGeometry
import LDMX.Hcal.hcal_hardcoded_conditions
from LDMX.DQM import dqm
from LDMX.Packing import rawio

import os
base_name = os.path.basename(arg.input_file).replace('.raw','')
dir_name  = os.path.dirname(arg.input_file)
if not dir_name :
    dir_name = '.'

p.outputFiles = [f'{dir_name}/unpacked_{base_name}.root']

# where the ntuplizing tree will go
p.histogramFile = f'adc_{base_name}.root'

# sequence
#   1. decode event packet into digi collection
#   2. ntuplize digi collection
p.sequence = [ 
        hcal_format.HcalRawDecoder(
            input_file = arg.input_file,
            output_name = 'ChipSettingsTestDigis'
            ),
        dqm.NtuplizeHgcrocDigiCollection(
            input_name = 'ChipSettingsTestDigis',
            pedestal_table = arg.pedestals
            )
        ]

if arg.pause :
    p.pause()
