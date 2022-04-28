import mplhep as hep
import pandas as pd
import csv
import uproot
import argparse
import hist
import os
import matplotlib.pyplot as plt
import matplotlib.colors as clt
from matplotlib.backends.backend_pdf import PdfPages

def event_display(fname,events_list=[12],odir='./',pedestals=None):
    """
    Make event display:
    - decode options:
      ldmx_eid has (fpga,ch,link) columns and uses testbeam_connections_Apr13_long.csv to translate into (layer,bar)
      ldmx_translate has (layer,bar,end) columns and uses testbeam_connections_Apr13.csv
    """

    os.system(f'mkdir -p {odir}')

    # get info from pedestals csv file
    pedestals_adc = {}
    if pedestals is not None:
        csv_reader = csv.reader(open(pedestals), delimiter=',')
        for row in csv_reader:
            (fpga,ROC,channel) = row[1].split(':')
            fpga = int(fpga)
            link = int(ROC)*2+int( (int(channel)) /36 )
            channel = int(channel)%36
            pedestals_adc[(fpga,ROC,channel)] = row[2]
            
    with uproot.open(fname) as f:
        data = f['ntuplizehgcroc/hgcroc'].arrays(library='pd')
        
    layer_axis = hist.axis.Variable(list(range(1,20)), name='layer', label='Layer')
    bar_axis = hist.axis.Regular(12, 0, 12, name='bar', label='Bar')

    # group by eid, layer, strip, end and find maxadc for each event
    maxadc = data.groupby(["raw_id","end","layer","strip","event"])["adc"].max()
    maxadc = maxadc.reset_index()
    for event in events_list:
        hists = {}
        hists['eventdisplay'] = hist.Hist(layer_axis,bar_axis)
        hists['eventdisplay'].fill(
            layer=maxadc.query('event==%i'%event)['layer'],
            bar=maxadc.query('event==%i'%event)['strip'],
            weight=maxadc.query('event==%i'%event)['adc']
        )

        # plot
        fig, axs = plt.subplots(1,1)
        hep.hist2dplot(hists['eventdisplay'], ax=axs, cmap="plasma")
        axs.set_title(f'Event {event}')
        fig.savefig(f"{odir}/event_{event}.png")
        plt.close(fig)
    
def main(arg):
    event_display(arg.fname,arg.events,odir=f"plots/{arg.odir}",pedestals=arg.pedestal)
    
if __name__=="__main__":
    parser = argparse.ArgumentParser(f'python event_display.py ',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--pedestal',dest='pedestal',default=None,help='Pedestal csv file')
    parser.add_argument('--events',dest='events',required=True,nargs="+", type=int,help='List of events to plot split by spaces')
    parser.add_argument('--fname',dest='fname',required=True,help='Decoded file with adc_histograms from ldmx-sw')
    parser.add_argument('--odir',dest='odir',required=True,help='Output directory')
    arg = parser.parse_args()

    main(arg)

