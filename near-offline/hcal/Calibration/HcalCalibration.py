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

ledfile = ""
mipfile = ""

elloc = {}
ped = {}
adc_gain = {}
tot_ped = {}
tot_ped = {}
mipmpv = {}
mipwidth = {}
led_med = {}

with open(ledfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        did = int(row["DetID"])
        elloc[did] = row["ElLoc"]
        ped[did] = str(row["ADC_PEDESTAL"])
        led_med[did] = str(row["LED_MEDIAN"])

with open(mipfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        did = int(row["DetID"])
        mipmpv[did] = str(row["ADC_PEDESTAL"])
        mipwidth[did] = str(row["LED_MEDIAN"])

header1 = ["# ElLoc Format: (polarfire:hgcrocindex:channel)"]
header2 = ["DetID", "ElLoc", "ADC_PEDESTAL", "ADC_GAIN", "TOT_PEDESTAL", "TOT_GAIN", "MIPMPV_ADC", "MIPWIDTH_ADC", "LED_MEDIAN"]

with open(outputFileName, mode='w', newline="") as datafile:
    writer = csv.writer(datafile, delimiter=',', quotechar='"')
    writer.writerow(header1)
    writer.writerow(header2)
    for did in elloc:
        line = [str(did), elloc[did], ped[did], gain[did], tot_ped[did], tot_gain[did], mipmpv[did], mipwidth[did], led_med[did]]
        writer.writerow[line]
