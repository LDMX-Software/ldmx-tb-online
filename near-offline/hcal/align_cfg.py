"""Basic HGCROCv2RawDataFile reformatting configuration"""

import argparse, sys

parser = argparse.ArgumentParser(f'ldmx fire {sys.argv[0]}')

parser.add_argument('pf0_raw')
parser.add_argument('pf1_raw')
parser.add_argument('--pause',action='store_true')
grp = parser.add_mutually_exclusive_group()
grp.add_argument('--keep_eids',action='store_true',
        help='Dont translate electronic into detector IDs.')
grp.add_argument('--recon',help='Attempt to reconstruct.',action='store_true')
parser.add_argument('--max_events',default=100,type=int)
parser.add_argument('--pedestals',default=None,type=str)

arg = parser.parse_args()

from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('unpack')
p.maxEvents = arg.max_events
p.termLogLevel = 0
p.logFrequency = 100

import LDMX.Hcal.hgcrocFormat as hcal_format
import LDMX.Hcal.digi as hcal_digi
import LDMX.Hcal.HcalGeometry
import LDMX.Hcal.hcal_hardcoded_conditions
from LDMX.DQM import dqm
from LDMX.Packing import rawio

import os
base_name = os.path.basename(arg.pf0_raw).replace('.raw','').replace('fpga_0_','')
dir_name  = os.path.dirname(arg.pf0_raw)
if not dir_name :
    dir_name = '.'

p.outputFiles = [f'{dir_name}/unpacked_{base_name}.root']

# where the ntuplizing tree will go
p.histogramFile = f'adc_{base_name}.root'

if arg.keep_eids :
    tbl = None
else :
    tbl = f'{os.environ["LDMX_BASE"]}/ldmx-sw/Hcal/data/testbeam_connections.csv'

# sequence
#   1. decode event packet into digi collection
#   2. ntuplize digi collection
p.sequence = [ 
        hcal_format.HcalRawDecoder(
            input_file = arg.pf0_raw,
            connections_table = tbl,
            output_name = 'PF0RawDigis'
            ),
        hcal_format.HcalRawDecoder(
            input_file = arg.pf1_raw,
            connections_table = tbl,
            output_name = 'PF1RawDigis'
            ),
        hcal_format.HcalAlignPolarfires(
            input_names = ['PF0RawDigis','PF1RawDigis'],
            output_name = 'HcalRawDigis'
            ),
        dqm.NtuplizeHgcrocDigiCollection(
            input_name = 'HcalRawDigis',
            already_aligned = True)
        ]

# add recon if requested
if arg.recon :
    recon = hcal_digi.HcalRecProducer()
    recon.digiCollName = 'HcalRawDigis'
    p.sequence.append(recon)

if arg.pause :
    p.pause()
