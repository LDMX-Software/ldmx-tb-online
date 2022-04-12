import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad
import csv

#Usage: ldmx python3 SampleMipAna.py <unpacked file>.root <pedestal file>.csv

def MIPFit(histo, canvas, outfile):
    fitfunc = TF1("fitfunc", "landau", 50, 5000)
    fit = histo.Fit(fitfunc, "LSQIM", "", 50, 5000)
    histo.Draw()
    canvas.Print(outfile+".pdf")
    mpv = fit.Get().Parameter(0)
    width = fit.Get().Parameter(1) #Double check this width
    isGoodFit = 1 #Update Goodness of Fit test
    return mpv, width, isGoodFit

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'hist_mip_'+sys.argv[1] #ouptut file name
outputMipCsvName = 'hist_mip_'+sys.argv[1] #ouptut file name
#tree=inputFile.Get("LDMX_Events") #get tree
tree = inputFile.Get('ntuplizehgcroc').Get("hgcroc")
pedfile = sys.argv[2]

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)
outfile = "test"

ped = {}
did = {}
#e_loc = {}
with open(pedfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        #did = int(row['DetID'])
        eloc = row['ElLoc']
        did[eloc] = int(row['DetID'])
        pedestal = float(row['ADC_PEDESTAL'])
        ped[eloc] = pedestal
        #e_loc[did] = eloc

maxadc_histo = {} #histo map to channel
alladc_histo = {}
maxsample_histo = {}
sumadc_histo = {}

prevID = 0
maxadc = -9999
maxsample = -9999
sumadc = -9999
n = 0
for d in tree: #Loop over events in tree
        #if(n > 100):
        #    break
        n = n + 1
    #for d in e:
        link = d.link
        fpga = d.fpga
        channel = d.channel
        chan = (link%2) * 36 + channel #Get correct chan for odd numbered links
        raw_id = d.raw_id
        roc = int(link / 2) #ROC ID in tuple starts at 1, needs to start at 1
        roc_index = (roc)%3
        elloc = "{0}:{1}:{2}".format(fpga+1, roc_index, chan)
        adc = d.adc - 175
        if elloc not in maxadc_histo : #Create map key if not already there
            maxadc_histo[elloc] = ROOT.TH1F(f'maxadc_eloc_{elloc}', f'Max ADC Eloc {elloc}',100,0,400)
            maxsample_histo[elloc] = ROOT.TH1F(f'maxsample_eid_{elloc}', f'Max Sample Eloc {elloc}',8,0,8)
            alladc_histo[elloc] = ROOT.TH1F(f'adc_eloc_3_{elloc}', f'ADCs Eloc {elloc}',1024,0,1024)
            sumadc_histo[elloc] = ROOT.TH1F(f'sumadc_eloc_{elloc}', f'Sum ADC Eloc {elloc}',1024*4,0,1024*4)
        if(raw_id != prevID):
            maxadc_histo[elloc].Fill(maxadc) #Fill ADC count
            maxsample_histo[elloc].Fill(maxsample)
            sumadc_histo[elloc].Fill(sumadc)
            maxadc = -9999
            maxsample = -9999
            sumadc = 0
            sample = 0
        #adc_ped = adc - ped[elloc]
        adc_ped = adc
        alladc_histo[elloc].Fill(adc_ped)
        sumadc = sumadc + adc_ped
        if(adc_ped > maxadc):
            maxsample = sample
            maxadc = adc_ped
        prevID = raw_id
        sample = sample + 1

#Close and write file
for id in maxadc_histo:
    maxadc_histo[id].GetXaxis().SetTitle("max ADC")
    maxsample_histo[id].GetXaxis().SetTitle("max Sample")
    sumadc_histo[id].GetXaxis().SetTitle("Sum ADC")
    alladc_histo[id].GetXaxis().SetTitle("ADC")

mpv_histo = ROOT.TH1F('mpv', 'MIP MPV ADC Max',5,0,5)
mipwidth_histo = ROOT.TH1F('mipwidth', 'MIP Width ADC Max',5,0,5)
#coverage_histo = ROOT.TH2F('coverage', 'In Muon Beam Acceptance',20, 0, 20, 12, 0, 12)

c.Print(outfile+".pdf[")
c.SetLogy(0)
csvfile = open(outputMipCsvName+".csv", 'w', newline='')
writer = csv.writer(csvfile, delimiter=',', quotechar='"')
header = ["DetID","ElLoc","ADC_PEDESTAL","MIPMPV_ADC","MIPWIDTH_ADC"]
writer.writerow(header)
i = 0
for id in sumadc_histo:
    mpv, width, isGoodFit = MIPFit(maxadc_histo[id], c, outfile)
    mpv_histo.Fill(i, mpv)
    mipwidth_histo.Fill(i, width)
    #coverage_histo.Fill(0,0) #Update
    #elloc = e_loc[id]
    #pedestal = ped[id]
    elloc = "-9999"
    pedestal = "-9999"
    line = [str(id), elloc, str(pedestal), str(mpv), str(width)]
    writer.writerow(line)
    i = i + 1
    if(i > 3):
        break

c.SetLogy(0)
mpv_histo.Draw("hist")
c.Print(outfile+".pdf")
mipwidth_histo.Draw("hist")
c.Print(outfile+".pdf")
#coverage_histo.Draw("COLZ")
#c.Print(outfile+".pdf")

c.Print(outfile+".pdf]")

f.Write()
f.Close()
