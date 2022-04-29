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

#Usage: ldmx python3 SimpleEventDisplay.py <ntupleized file>.root.csv

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
#outputFileName = 'hist_mip_'+sys.argv[1] #ouptut file name
tree = inputFile.Get('ntuplizehgcroc').Get("hgcroc")
c = TCanvas("c","c",800,600)
outfile = "SampleEvents"

horizontal = {}
vertical = {}

prevID = 0
maxadc = -9999
minadc = 9999
maxsample = -9999
nevents = 100
prevEvent = -9999
adc_thresh = 30

c.Print(outfile+".pdf[")

for d in tree: #Loop over events in tree
    event = d.aligned_event
    if event not in horizontal : #Create map key if not already there
        horizontal[event] = ROOT.TH2F(f'horizontal_event_{event}', f' Event {event}', 20, 0, 20, 12, 0, 12)
        vertical[event] = ROOT.TH2F(f'vertical_event_{event}', f' Event {event}', 20, 0, 20, 12, 0, 12)
    if(event > nevents): break
    raw_id = d.raw_id
    if(raw_id != prevID):
        if(maxadc > adc_thresh + minadc):
            if(layer < 10):
                strip = strip + 2 #Align the front and back detector
            if(layer%2 == 0):
                horizontal[event].Fill(layer, strip)
            else:
                vertical[event].Fill(layer, strip)
        maxadc = -9999
        minadc = 9999
        maxsample = -9999
        sample = 0
    layer = d.layer
    strip = d.strip
    end = d.end
    adc = d.adc
    adc_ped = adc
    if(adc_ped > maxadc):
        maxsample = sample
        maxadc = adc_ped
    if(adc_ped < minadc):
        minadc = adc_ped
    prevID = raw_id
    prevEvent = event
    sample = sample + 1

for i in horizontal:
    horizontal[i].Draw("COLZ")
    horizontal[i].SetTitle("Horizontal Event {0}".format(i))
    horizontal[i].GetXaxis().SetTitle("Bar")
    horizontal[i].GetXaxis().SetTitle("Plane")
    c.Print(outfile+".pdf")
    vertical[i].Draw("COLZ")
    vertical[i].SetTitle("Vertical Event {0}".format(i))
    vertical[i].GetXaxis().SetTitle("Bar")
    vertical[i].GetXaxis().SetTitle("Plane")
    c.Print(outfile+".pdf")

c.Print(outfile+".pdf]")
