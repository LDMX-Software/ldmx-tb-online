import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad
import csv
from array import array
import numpy as np
import matplotlib
from matplotlib.backends.backend_pdf import PdfPages
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit # Import Curve Fits
import os

#Usage: ldmx python3 HcalLEDAna.py <ntuplezed file fpga 0>.root <ntuplezed filefpga 1>.root <pedestal file>.csv
#It can also work with just a single fgpa file

def GetHistoMedian(histo):
    median = 0
    prob = np.array([0.5])
    median = np.array([0.])
    q = histo.GetQuantiles(1, median, prob)
    if(median[0] > 1000): median[0] = 0.
    print(median[0])
    return median[0]

gSystem.Load("libFramework.so")

pedfile = sys.argv[len(sys.argv)-1]
outputFileName = os.path.basename('hist_led_'+sys.argv[1]).replace("_DPM0", "").replace("_DPM1", "") #ouptut file name
outputLedCsvName = os.path.basename('led_calib_'+sys.argv[1]).replace('.root','.csv').replace("_DPM0", "").replace("_DPM1", "") #ouptut file name

infiles = []
for i in range(1, len(sys.argv)-1):
    infiles.append(TFile(sys.argv[i], "read"))

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)

bright = 1625

both_pfs = False
if(len(sys.argv) == 4):
    both_pfs = True
    print("Using both DPMs")
elif(len(sys.argv) == 3):
    print("Using only 1 DPM")
else:
    print("Wrong number of files. Please check!")
    sys.exit()

outputFileName = os.path.basename(outputFileName).replace("_dac_{0}".format(bright),'') #ouptut file name
outputLedCsvName = os.path.basename(outputLedCsvName).replace("_dac_{0}".format(bright),'') #ouptut file name

ped = {}
#did = {}
eloc = {}
with open(pedfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        did = int(row['DetID'])
        eloc[did] = row['ElLoc']
        #did[eloc] = int(row['DetID'])
        pedestal = float(row['ADC_PEDESTAL'])
        ped[did] = pedestal
        print("{0} {1}".format(did, ped[did]))
        #e_loc[did] = eloc

maxadc_histo = {} #histo map to channel
maxsample_histo = {}

prevID = 0
maxadc = -9999
maxsample = -9999
sumadc = -9999

medians = {}
for infile in infiles:
    n = 0
    tree = infile.Get('ntuplizehgcroc').Get("hgcroc")
    for d in tree: #Loop over events in tree
        if(n > 72*3*8*100): break
        n = n + 1
        raw_id = d.raw_id
        link = d.link
        fpga = d.fpga
        channel = d.channel
        chan = (link%2) * 36 + channel #Get correct chan for odd numbered links
        roc = int(link / 2) #ROC ID in tuple starts at 1, needs to start at 1
        roc_index = (roc)%3
        elloc = "{0}:{1}:{2}".format(fpga, roc_index, chan)
        adc = d.adc
        if raw_id not in maxadc_histo : #Create map key if not already there
            maxadc_histo[raw_id] = ROOT.TH1F(f'maxadc_eid_{raw_id}', f'Max ADC EID {raw_id}', 1024, 0, 1024)
            maxsample_histo[raw_id] = ROOT.TH1F(f'maxsample_eid_{raw_id}', f'Max Sample EID {raw_id}', 8, 0, 8)
        if(raw_id != prevID):
            if(maxadc > 10):
                maxadc_histo[raw_id].Fill(maxadc) #Fill ADC count
                maxsample_histo[raw_id].Fill(maxsample)
            maxadc = -9999
            maxsample = -9999
            sumadc = 0
            sample = 0
        if(raw_id in ped):
            adc_ped = adc - ped[raw_id]
        else:
            #print("Key {0} and Eloc {1} not found. Setting pedestal to 0".format(raw_id, elloc))
            adc_ped = adc
        if(adc_ped > maxadc):
            maxsample = sample
            maxadc = adc_ped
        prevID = raw_id
        sample = sample + 1

median_histo = ROOT.TH1F(f'medians', f'medians', 72*6, 0, 72*6)
csvfile = open(outputLedCsvName, 'w', newline='')
writer = csv.writer(csvfile, delimiter=',', quotechar='"')
header = ["DetID", "ElLoc", "ADC_PEDESTAL", "LED_MEDIAN"]
writer.writerow(header)
i = 0
for id in maxadc_histo:
#for id in medians:
    maxadc_histo[id].GetXaxis().SetTitle("max ADC")
    maxsample_histo[id].GetXaxis().SetTitle("max Sample")
    medians[id] = GetHistoMedian(maxadc_histo[id])
    elloc = eloc[id]
    pedestal = ped[id]
    median_histo.Fill(i, medians[id])
    line = [str(id), elloc, str(pedestal), str(medians[id])]
    writer.writerow(line)
    i = i + 1

median_histo.SetTitle("LED Median DAC {0}".format(bright))
median_histo.GetXaxis().SetTitle("Channel (AU)")
median_histo.GetYaxis().SetTitle("LED Median")
median_histo.Draw()
f.Write()
f.Close()
