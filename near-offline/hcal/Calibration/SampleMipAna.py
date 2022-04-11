import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad
import csv

#Usage: ldmx python3 SampleMipAna.py <unpacked file>.root <pedestal file>.csv

def MIPFit(histo, canvas, outfile):
    fitfunc = TF1("fitfunc", "landau", 0, 5000)
    fit = histo.Fit(fitfunc, "LSQIM", "", 0, 5000)
    histo.Draw()
    canvas.Print(outfile+".pdf")
    mpv = fit.Get().Parameter(0)
    width = 4 * fit.Get().Parameter(1) #Double check this width
    isGoodFit = 1 #Update Goodness of Fit test
    return mpv, width, isGoodFit

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'hist_mip_'+sys.argv[1] #ouptut file name
outputMipCsvName = 'hist_mip_'+sys.argv[1] #ouptut file name
tree=inputFile.Get("LDMX_Events") #get tree
pedfile = sys.argv[2]

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)
outfile = "test"

ped = {}
e_loc = {}
with open(pedfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        did = int(row['DetID'])
        pedestal = float(row['ADC_PEDESTAL'])
        eloc = row['ElLoc']
        ped[did] = pedestal
        e_loc[did] = eloc

maxadc_histo = {} #histo map to channel
alladc_histo = {}
maxsample_histo = {}
sumadc_histo = {}
for e in tree : #Loop over events in tree
    if True:
        for d in e.ChipSettingsTestDigis_unpack : #Loop over channels
            if d.id() not in maxadc_histo : #Create map key if not already there
               maxadc_histo[d.id()] = ROOT.TH1F(f'maxadc_eid_{d.id()}', f'Max ADC EID {d.id()}',1024,0,1024)
               maxsample_histo[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',8,0,8)
               alladc_histo[d.id()] = ROOT.TH1F(f'adc_eid_3_{d.id()}', f'ADCs EID {d.id()}',1024,0,1024)
               sumadc_histo[d.id()] = ROOT.TH1F(f'sumadc_eid_{d.id()}', f'Sum ADC EID {d.id()}',1024*4,0,1024*4)
            maxadc = -9999.
            maxsample = -9999
            sumadc = 0
            for i in range(d.size()) : #Loop over sample number
               adc_ped = d.at(i).adc_t() - ped[d.id()]
               alladc_histo[d.id()].Fill(adc_ped)
               sumadc = sumadc + adc_ped
               if(adc_ped > maxadc):
                   maxsample = i
                   maxadc = adc_ped
            maxadc_histo[d.id()].Fill(maxadc) #Fill ADC count
            maxsample_histo[d.id()].Fill(maxsample)
            sumadc_histo[d.id()].Fill(sumadc)

#Close and write file
for id in maxadc_histo:
    maxadc_histo[id].GetXaxis().SetTitle("max ADC")
    maxsample_histo[id].GetXaxis().SetTitle("max Sample")
    sumadc_histo[id].GetXaxis().SetTitle("Sum ADC")
    alladc_histo[id].GetXaxis().SetTitle("ADC")

mpv_histo = ROOT.TH1F('mpv', 'MIP MPV ADC Sum',400,0,400)
mipwidth_histo = ROOT.TH1F('mipwidth', 'MIP Width ADC Sum',400,0,400)
coverage_histo = ROOT.TH2F('coverage', 'In Muon Beam Acceptance',20, 0, 20, 12, 0, 12)

c.Print(outfile+".pdf[")
header = ["DetID","ElLoc","ADC_PEDESTAL","MIPMPV_ADC","MIPWIDTH_ADC"]
writer = csv.writer(outputMipCsvName, delimiter=',', quotechar='"',)
writer.writerow(header)
i = 0
for id in sumadc_histo:
    mpv, width, isGoodFit = MIPFit(sumadc_histo[id], c, outfile)
    mpv_histo.Fill(i, mpv)
    mipwidth_histo.Fill(i, width)
    coverage.Fill(0,0) #Update physical Map
    elloc = e_loc[id]
    pedestal = ped[id]
    line = [str(id), elloc, str(pedestal), str(mpv), str(width)]
    writer.writerow(line)
    i = i + 1

mpv_histo.Draw()
c.Print(outfile+".pdf")
mipwidth_histo.Draw()
c.Print(outfile+".pdf")
coverage_histo.Draw()
c.Print(outfile+".pdf")

c.Print(outfile+".pdf]")

f.Write()
f.Close()
