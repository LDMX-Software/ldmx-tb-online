"""Basic HGCROCv2RawDataFile reformatting configuration"""

from Reformat import reformat

c, parser = reformat.Converter.cli_parser(
        term_level = 1,
        output_filename = 'reformatted.root'
        )

parser.add_argument('--pf1')
parser.add_argument('--pf0')
parser.add_argument('--pause',action='store_true')

arg = parser.parse_args()

from Reformat import testbeam
if arg.pf0 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf0,"Polarfire0Raw"))

if arg.pf1 is not None :
    c.input_files.append(testbeam.PolarfireRawFile(arg.pf1,"Polarfire1Raw"))

if arg.pause :
    c.pause()
