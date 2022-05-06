"""Base module for configuring a reformat of raw data files"""

class Converter :
    """Core converter class which handles converting input raw data files
    into an output Framework EventFile

    Attributes
    ----------
    lastConverter : Converter
        Holds the converter that was last constructed
    libraries : list[str]
        List of libraries to load
    input_files : list[RawDataFile]
        RawDataFile instances to use as input files for conversion
    run : int
        Run number to put into output EventFile
    start_event : int
        Starting event ID
    output_filename : str
        Output EventFile path
    pass_name : str
        Pass name in output EventFile
    """

    lastConverter = None

    def __init__(self, output_file) :
        Converter.lastConverter = self
        self.libraries = []
        self.input_files = []
        self.run = 1
        self.start_event = 0
        self.output_filename = output_file
        self.detector_name = 'ldmx-hcal-prototype-v1.0'
        self.pass_name = 'raw'
        self.keep_all = True
        self.max_diff = 10
        self.event_limit = -1
        self.term_level = 4
        self.file_level = 4
        self.log_file = ''

    def cli_parser(default_output = 'reformatted.root') :
        import argparse
        import sys

        c = Converter(default_output)
        parser = argparse.ArgumentParser(f'ldmx reformat {sys.argv[0]}')

        class SetConverterVar(argparse.Action) :
            def __init__(self, option_strings, dest, nargs=None, **kwargs) :
                if nargs is not None :
                    raise ValueError('nargs not allowed for SetConverterVar')
                super().__init__(option_strings, dest, **kwargs)
            def __call__(self, parser, namespace, values, option_string=None) :
                setattr(Converter.lastConverter, self.dest, values)

        for k, v in c.__dict__.items() :
            if k in ['libraries','input_files'] :
                continue
            parser.add_argument(f'--{k}',action=SetConverterVar,type=v.__class__,default=v)

        return c, parser

    def addLibrary(lib) :
        if Converter.lastConverter is None :
            raise Exception('Must define a Converter object before adding another library.')

        Converter.lastConverter.libraries.append(lib)

    def addModule(m) :
        if not m.startswith('lib') :
            m = 'lib'+m

        if not m.endswith('.so') :
            m += '.so'

        Converter.addLibrary(m)

    def __repr__(self) :
        return f'{self.input_files} -> {self.output_filename}'

    def __str__(self) :
        msg = f'Destination: {self.output_filename}\n'
        msg += f'  Run: {self.run}\n'
        msg += f'  Pass Name: {self.pass_name}\n'
        msg += f'  Start Event: {self.start_event}\n'
        msg += f'  Detector Name: {self.detector_name}\n'
        msg += f'  Keep All: {repr(self.keep_all)}\n'
        msg += f'  Max Diff: {self.max_diff}\n'
        msg += f'  Event Limit: {self.event_limit}\n'
        msg += 'Input Files:\n'
        for f in self.input_files :
            msg += f'  {f}\n'

        return msg

    def pause(self) :
        """Print the converter and wait for user to press continue"""
        print(self)
        input('Press Enter to continue...')

    def dump(self) :
        """Debug dump of all parameters that would be loaded into C++"

        The extract module mirrors the extraction procedure done in C++.
        It is not a perfect mirroring because C++ requires some type deduction,
        so not everything that can be dumped will be loaded into C++ without error.
        """

        def extract(obj):
            """Extract the parameter from the input object"""

            if isinstance(obj,list) :
                return [ extract(o) for o in obj ]
            elif hasattr(obj,'__dict__') :
                params = dict()
                for k in obj.__dict__ :
                    params[k] = extract(obj.__dict__[k])
                return params
            else :
                return obj

        return extract(self)
        

class RawDataFile :
    """Base class for configuring other raw data files

    Parameters
    ----------
    module : str
        Name of module to load
    class_name : str
        Full class name (including namespace) of data file
    name : str
        Human-friendly name for logging and event bus attachment
    input_name : str
        File to read from (optional)
    kwargs : dict
        Extra file parameters
    """

    def __init__(self, module, class_name, name, input_name = '', **kwargs) :
        Converter.addModule(module)
        self.__dict__ = kwargs
        self.class_name = class_name
        self.name = name
        self.input_name = input_name

    def __repr__(self) :
        return f'{self.__class__.__name__}({self.class_name})'

    def __str__(self) :
        return f'{self.__class__.__name__} {self.__dict__}'

