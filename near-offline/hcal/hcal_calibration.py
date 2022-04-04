import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors
from ROOT import gROOT, gStyle, gSystem, gPad
import csv

#Usage: ldmx python3 hcal_calibration.py <ntuplized root file>.root
#Outputs csv file DumbReconConditions.csv

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'DumbReconConditions.csv' #hardcoded name for now
tree=inputFile.Get("ntuplizehgcroc/hgcroc") #get tree

energy_per_mip = 4.66; #MeV/MIP
voltage_hcal = 5.; #mV/PE
PE_per_mip = 68.; #PEs/mip
mV_per_PE = 1/energy_per_mip * voltage_hcal * PE_per_mip; #mV per MIP is about 73 for now

header1 = ["# ElLoc Format: (rocid:channel)"]
header2 = ["DetID", "ElLoc", "ADC_PEDESTAL", "ADC_GAIN", "TOT_PEDESTAL", "TOT_GAIN"]

usedchans = {}

with open(outputFileName, mode='w') as datafile:
    writer = csv.writer(datafile, delimiter=',', quotechar='"',)
    writer.writerow(header1)
    writer.writerow(header2)
    for e in tree : #Loop over events in tree
        for d in e:

            link = d.link
            channel = d.channel - 1 #Channel in tuple starts at 1, needs to start at 0
            chan = (link%2) * 36 + channel #Get correct chan for odd numbered links
            raw_id = d.raw_id
            roc = int(link / 2) + 1 #ROC ID in tuple starts at 0, needs to start at 1
            if(raw_id in usedchans): #Don't duplicate channels
                continue
            adc_ped = 1. #Dummy Value
            adc_gain = 1.2 #Dummy Value
            tot_ped = 1. #Dummy Value
            tot_gain = 2.5 #Dummy Value
            elloc = "{0}:{1}".format(roc, chan)
            PE_chan = (maxadc - adcped) / mV_per_PE * adcgain; 
            #adc_gain = mV_per_PE / (maxadc - adc_ped) #where maxadc is at single PE peak
            line = [str(raw_id), elloc, str(adc_ped), str(adc_gain), str(tot_ped), str(tot_gain)]
            writer.writerow(line)
            usedchans[raw_id] = True
