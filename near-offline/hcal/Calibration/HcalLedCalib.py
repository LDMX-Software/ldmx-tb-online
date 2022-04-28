import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad
import csv
from array import array
import numpy as np
import os

#Usage: ldmx python3 HcalLEDAna.py <ntuplezed file fpga 0>.root <ntuplezed filefpga 1>.root <pedestal file>.csv
#It can also work with just a single fgpa file

def GetHistoMedian(histo):
    median = 0
    prob = np.array([0.5])
    median = np.array([0.])
    q = histo.GetQuantiles(1, median, prob)
    if(median[0] > 3000): median[0] = 0.
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

bright = 1650

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
    print(infile)
    tree = infile.Get("LDMX_Events")
    for e in tree : #Loop over events in tree
        for d in e.ChipSettingsTestDigis_unpack : #Loop over channels
            if d.id() not in maxadc_histo : #Create map key if not already there
               #maxadc_histo[d.id()] = ROOT.TH1F(f'maxadc_eid_{d.id()}', f'Max ADC EID {d.id()}',1024,0,1024)
               maxadc_histo[d.id()] = ROOT.TH1F(f'maxadc_eid_{d.id()}', f'Max ADC EID {d.id()}',3000,0,3000)
               maxsample_histo[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',8,0,8)
            maxadc = -9999.
            maxsample = -9999
            sumadc = 0
            for i in range(d.size()) : #Loop over sample number
               if(d.id() in ped):
                   adc_ped = d.at(i).adc_t() - ped[d.id()]
               else:
                   print("Key {0} and Eloc {1} not found. Setting pedestal to 0".format(raw_id, elloc))
                   adc_ped = adc
               if(adc_ped > maxadc):
                   maxadc = adc_ped
                   maxsample = i
               sumadc = sumadc + adc_ped
            #if(maxsample != 0 and maxsample != 1 and maxadc > 10):
            #maxadc_histo[d.id()].Fill(maxadc) #Fill ADC count
            if(sumadc > 20):
                maxadc_histo[d.id()].Fill(sumadc) #Fill ADC count
            maxsample_histo[d.id()].Fill(maxsample)

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
