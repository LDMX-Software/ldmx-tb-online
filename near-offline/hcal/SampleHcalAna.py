import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors
from ROOT import gROOT, gStyle, gSystem, gPad

#Usage: ldmx python3 SampleHcalAna.py <unpacked file>.root
#Run in the same directory as <unpacked file>.root
#Outputs file hist_adc_<unpacked file>.root

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'hist_adc_'+sys.argv[1] #ouptut file name
tree=inputFile.Get("LDMX_Events") #get tree

f = ROOT.TFile(outputFileName,'recreate') #create root output file

h = {} #histo map to channel
for e in tree : #Loop over events in tree
    if True:
        for d in e.ChipSettingsTestDigis_unpack : #Loop over channels
            if d.id() not in h : #Create map key if not already there
               h[d.id()] = ROOT.TH1F(f'adc_eid_{d.id()}', f'ADC EID {d.id()}',1024,0,1024)
            for i in range(d.size()) : #Loop over sample number
               h[d.id()].Fill(d.at(i).adc_t()) #Fill ADC count

#Close and write file
f.Write()
f.Close()
