"""Basic HGCROCv2RawDataFile reformatting configuration"""

from Reformat import reformat

c, parser = reformat.Converter.cli_parser(
        term_level = 1,
        output_filename = 'reformatted.root'
        )

parser.add_argument('--pf1')
parser.add_argument('--pf0')
parser.add_argument('--ts')
parser.add_argument('--wr')
parser.add_argument('--ft41')
parser.add_argument('--ft42')
parser.add_argument('--ft50')
parser.add_argument('--ft51')
parser.add_argument('--pause',action='store_true')

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
