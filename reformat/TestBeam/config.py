"""April 2022 TestBeam Reformat configuraiton"""

from Reformat import reformat

c, parser = reformat.Converter.cli_parser(
        description="""
        Currently, alignment can be done between the two Polarfire FPGAs of the HCal.
        Alignment across other subsystems (e.g. TS and WR) is still in development.

        WARNING: This program does not check that input raw files are from the same run.
        """,
        term_level = 1,
        keep_all = False,
        output_filename = 'reformatted.root'
        )

parser.add_argument('--pf1', help='Path to raw file for Polarfire FPGA 1 of the HCal')
parser.add_argument('--pf0', help='Path to raw file for Polarfire FPGA 0 of the HCal')
parser.add_argument('--ts' , help='Path to raw file (already grouped into events) for TS')
parser.add_argument('--wr' , help='Path to raw White Rabbit file')
parser.add_argument('--ft41', help='Path to raw FiberTracker 41 file')
parser.add_argument('--ft42', help='Path to raw FiberTracker 42 file')
parser.add_argument('--ft50', help='Path to raw FiberTracker 50 file')
parser.add_argument('--ft51', help='Path to raw FiberTracker 51 file')
parser.add_argument('--pause', help='Print configuration and wait for user confirmation',action='store_true')

arg = parser.parse_args()

from Reformat import testbeam
if arg.pf0 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf0,"Polarfire0Raw"))

if arg.pf1 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf1,"Polarfire1Raw"))

if arg.ts is not None :
    c.input_files.append(testbeam.TrigScintEventGroupedRawFile(arg.ts))

if arg.wr is not None :
    c.input_files.append(testbeam.WhiteRabbitRawFile(arg.wr))

if arg.ft41 is not None :
    c.input_files.append(testbeam.FiberTrackerRawFile(arg.ft41,'FT41Raw'))

if arg.ft42 is not None :
    c.input_files.append(testbeam.FiberTrackerRawFile(arg.ft42,'FT42Raw'))

if arg.ft50 is not None :
    c.input_files.append(testbeam.FiberTrackerRawFile(arg.ft50,'FT50Raw'))

if arg.ft51 is not None :
    c.input_files.append(testbeam.FiberTrackerRawFile(arg.ft51,'FT51Raw'))

if arg.pause :
    c.pause()
