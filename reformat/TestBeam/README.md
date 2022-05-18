# April 2022 Test Beam
Event building was not done online, so we need to do it now.
We can mimic event building by reformating the raw data files written by Rogue
into our ldmx event files with the subsystem buffers as event bus objects.

## Usage
Included in this directory is a configuration script for the `reformat` program.
After [building and installing `reformat`](./../README.md#Building), you can list all of its arguments.
```
ldmx reformat TestBeam/config.py -h
```
A common run example would be aligning of the two Polarfire files for the HCal.
```
ldmx reformat TestBeam/config.py \
  --pf0 path/to/fpga_0_run_XXX.raw \
  --pf1 path/to/fpga_1_run_XXX.raw \
  --output_filename hcal_run_XXX_reformat.root
```
