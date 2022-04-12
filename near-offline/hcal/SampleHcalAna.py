import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors
from ROOT import gROOT, gStyle, gSystem, gPad

#Usage: ldmx python3 SampleHcalAna.py <unpacked file>.root
#Outputs file hist_adc_<unpacked file>.root

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'hist_adc_'+sys.argv[1] #ouptut file name
tree=inputFile.Get("LDMX_Events") #get tree

c = TCanvas("c","c",800,600)

f = ROOT.TFile(outputFileName,'recreate') #create root output file

maxadc_histo = {} #histo map to channel
alladc_histo = {}
maxsample_histo = {}
sumadc_histo = {}
for e in tree : #Loop over events in tree
    if True:
        for d in e.ChipSettingsTestDigis_unpack : #Loop over channels
            if d.id() not in maxadc_histo : #Create map key if not already there
               maxadc_histo[d.id()] = ROOT.TH1F(f'maxadc_eid_{d.id()}', f'Max ADC EID {d.id()}',1024,0,1024)
               alladc_histo[d.id()] = ROOT.TH1F(f'alladc_eid_{d.id()}', f'All ADC EID {d.id()}',1024,0,1024)
               maxsample_histo[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',8,0,8)
               sumadc_histo[d.id()] = ROOT.TH1F(f'sumadc_eid_{d.id()}', f'Sum ADC EID {d.id()}',1024*4,0,1024*4)
            maxadc = -9999.
            sumadc = 0
            maxsample = -9999
            for i in range(d.size()) : #Loop over sample number
               sumadc = sumadc + d.at(i).adc_t()
               alladc_histo[d.id()].Fill(d.at(i).adc_t())
               if(d.at(i).adc_t() > maxadc):
                   maxadc = d.at(i).adc_t()
                   maxsample = i
            maxadc_histo[d.id()].Fill(maxadc) #Fill ADC count
            maxsample_histo[d.id()].Fill(maxsample)
            sumadc_histo[d.id()].Fill(sumadc)

#Close and write file
f.Write()
f.Close()
