from Reformat import reformat
import os

c, parser = reformat.Converter.cli_parser(
        drop = ['output_filename','run','pass_name','detector_name','start_event'],
        description="""
        HCal-specific reformating config script.
        We deduce the run number from the input paths and verify
        that the two files have the same run number. Only one 
        path is required so users can unpack raw files in preparation
        for decoding without attempting to align the two polarfires.
        """,
        term_level = 2,
        keep_all = False,
        )

def extract_params(fp) :
    params = os.path.basename(fp).replace('.raw','').split('_')
    return int(params[params.index('fpga')+1]), int(params[params.index('run')+1]), params[-2], params[-1]

parser.add_argument('raw_files', help='Raw files to reformat (and align if >1)',nargs='+')
parser.add_argument('--pf1-spill-offset',type=int,default=0,
    help='Number of spills to skip at start of PF1 data')
parser.add_argument('--intra-spill-range',type=int,default=[-1,1<<31-1],nargs=2,
    help='Range of ticks to keep for alignment (i.e. to be consider intra-spill)')
parser.add_argument('--pause', help='Print configuration and wait for user confirmation',
    action='store_true')

arg = parser.parse_args()

from Reformat import testbeam

pf = None
day = None
time = None
for rf in arg.raw_files :
    pf, run, pf_day, pf_time = extract_params(rf)
    if len(c.input_files) > 1 and (c.run != run or day != pf_day or time != pf_time) :
        raise Exception('Two files provided and they dont have matching run numbers and timestamps.')
    c.run = run
    day = pf_day
    time = pf_time
    c.input_files.append(testbeam.PolarfireRawFile(rf, f'Polarfire{pf}Raw', intra_spill_tick_range = arg.intra_spill_range))
    if pf == 1 :
        c.input_files[-1].spills_to_skip = arg.pf1_spill_offset

if len(c.input_files) == 1 :
    alignment = 'unaligned'
    provided = f'fpga_{pf}'
elif len(c.input_files) == 2 :
    alignment = 'aligned'
    provided = 'hcal'
else :
    parser.error('Cannot provide more than two files.')

dir_name = f'{os.environ["LDMX_BASE"]}/testbeam/{alignment}/reformat'
os.makedirs(dir_name, exist_ok=True)
c.output_filename = f'{dir_name}/reformat_{provided}_run_{c.run}_{day}_{time}.root'

if arg.pause :
    c.pause()
