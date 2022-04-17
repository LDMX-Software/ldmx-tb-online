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

#Usage: ldmx python3 SampleMipAna.py <ntuplezed file fpga 0>.root <ntuplezed filefpga 1>.root <pedestal file>.csv
#It can also work with just a single fgpa file

def MIPFit(histo, canvas, outfile):
    fitfunc = TF1("fitfunc", "landau", 50, 300)
    fit = histo.Fit(fitfunc, "LSQIM", "", 100, 300)
    fitfunc.SetParameters(fit.Get().Parameter(0), fit.Get().Parameter(1), fit.Get().Parameter(2))
    pre_mpv = fitfunc.GetMaximumX(125,250)#-9999
    fit2 = histo.Fit(fitfunc, "LSQIM", "", pre_mpv-25, pre_mpv+125)
    fitfunc.SetParameters(fit2.Get().Parameter(0), fit2.Get().Parameter(1), fit2.Get().Parameter(2))
    mpv = fitfunc.GetMaximumX(0,500)#-9999
    maximum = fitfunc.GetMaximum(0,500)
    halfmax = maximum / 2
    FWHMx1 = fitfunc.GetX(halfmax, -500, mpv)
    FWHMx2 = fitfunc.GetX(halfmax, mpv, 1000)
    FWHM = FWHMx2 - FWHMx1
    isGoodFit = 1 #Update Goodness of Fit test
    canvas.SetLogy(1)
    histo.Draw()
    canvas.Print(outfile+".pdf")
    return mpv, FWHM, isGoodFit

def linefit(x, h):
    return h*x

def fitMipSlope(mpv, width):
    mip_calc_mpv = 4.66 #MeV
    mip_calc_width = 1.63 #MeV
    x = []
    x.append(0.)
    x.append(width)
    x.append(mpv)
    y = []
    y.append(0.)
    y.append(mip_calc_width)
    y.append(mip_calc_mpv)
    x_fit = np.linspace(0,200,200) # Create a simple list
    popt, pcov = curve_fit(linefit, x, y)
    fig, ax = plt.subplots(1, 1)
    ax.scatter(x, y, label = "Calib")
    ax.plot(x_fit, linefit(x_fit, *popt), label = "Fit")
    ax.set_title("EID {0}".format(id))
    ax.set_xlabel("Max ADC") # Set x axis title
    ax.set_ylabel("E dep (MeV)") # Set x axis title
    pp.savefig(fig)
    plt.close(fig)
    del fig
    del ax
    return popt[0]

gSystem.Load("libFramework.so")

pedfile = sys.argv[len(sys.argv)-1]
outputFileName = os.path.basename('hist_mip_'+sys.argv[1]).replace("_fpga_0", "").replace("_fpga_1", "") #ouptut file name
outputMipCsvName = os.path.basename('mip_calib_'+sys.argv[1]).replace('.root','.csv').replace("_fpga_0", "").replace("_fpga_1", "") #ouptut file name
fitfile1 = os.path.basename("mip_fits_landua_"+sys.argv[1]).replace('.root','').replace("_fpga_0", "").replace("_fpga_1", "")
fitfile2 = os.path.basename("mip_fits_linear_"+sys.argv[1]).replace('.root','.pdf').replace("_fpga_0", "").replace("_fpga_1", "")

infiles = []
for i in range(1, len(sys.argv)-1):
    infiles.append(TFile(sys.argv[i], "read"))

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)

ped = {}
did = {}
eloc = {}
with open(pedfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        did = int(row['DetID'])
        eloc[did] = row['ElLoc']
        #did[eloc] = int(row['DetID'])
        pedestal = float(row['ADC_PEDESTAL'])
        ped[did] = pedestal
        #e_loc[did] = eloc

maxadc_histo = {} #histo map to channel
alladc_histo = {}
maxsample_histo = {}
sumadc_histo = {}

prevID = 0
maxadc = -9999
maxsample = -9999
sumadc = -9999
#hasMIPs = [1275070253, 1275070254]
hasMIPs = [1275070250, 1275070251, 1275070252, 1275070254, 1275070255, 1275070256,
    1275070280, 1275070281, 1275070282, 1275070284, 1275070285, 1275070286,
    1275070306, 1275070307, 1275070308, 1275070310, 1275070311, 1275070312,
    1275070336, 1275070337, 1275070338, 1275070340, 1275070341, 1275070342,
    1275070364, 1275070365, 1275070366, 1275070368, 1275070369, 1275070370,
    1275068433, 1275068435, 1275068436, 1275068439, 1275068440, 1275068459,
    1275068460, 1275068467, 1275068471, 1275068475, 1275068476, 1275068479,
    1275068480, 1275068482, 1275068486, 1275068493, 1275068494, 1275068497,
    1275068498, 1275068509, 1275068514, 1275068524, 1275068535, 1275068547,
    1275068551, 1275068552, 1275068555, 1275068556, 1275068562, 1275068570,
    1275068573, 1275068581, 1275068585, 1275068596, 1275068601, 1275068602,
    1275068626, 1275068628, 1275068631, 1275068632]
n = 0
for infile in infiles:
    tree = infile.Get('ntuplizehgcroc').Get("hgcroc")
    for d in tree: #Loop over events in tree
        #if(n > 72*3*5000*8):
        #    break
        n = n + 1
        raw_id = d.raw_id
        #if(raw_id not in hasMIPs): continue
        link = d.link
        fpga = d.fpga
        channel = d.channel
        chan = (link%2) * 36 + channel #Get correct chan for odd numbered links
        roc = int(link / 2) #ROC ID in tuple starts at 1, needs to start at 1
        roc_index = (roc)%3
        elloc = "{0}:{1}:{2}".format(fpga, roc_index, chan)
        adc = d.adc
        if raw_id not in maxadc_histo : #Create map key if not already there
            maxadc_histo[raw_id] = ROOT.TH1F(f'maxadc_eid_{raw_id}', f'Max ADC EID {raw_id}',100,0,400)
            maxsample_histo[raw_id] = ROOT.TH1F(f'maxsample_eid_{raw_id}', f'Max Sample EID {raw_id}',8,0,8)
            alladc_histo[raw_id] = ROOT.TH1F(f'adc_eid_{raw_id}', f'ADCs EID {raw_id}',1024,0,1024)
            sumadc_histo[raw_id] = ROOT.TH1F(f'sumadc_eid_{raw_id}', f'Sum ADC EID {raw_id}',1024*4,0,1024*4)
        if(raw_id != prevID):
            maxadc_histo[raw_id].Fill(maxadc) #Fill ADC count
            maxsample_histo[raw_id].Fill(maxsample)
            sumadc_histo[raw_id].Fill(sumadc)
            maxadc = -9999
            maxsample = -9999
            sumadc = 0
            sample = 0
        if(raw_id in ped):
            adc_ped = adc - ped[raw_id]
        else:
            #print("Key {0} and Eloc {1} not found. Setting pedestal to 0".format(raw_id, elloc))
            adc_ped = adc
        alladc_histo[raw_id].Fill(adc_ped)
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

mpv_histo = ROOT.TH1F('mpv', 'MIP MPV ADC Max',100,0,100)
mipwidth_histo = ROOT.TH1F('mipwidth', 'MIP Width ADC Max',100,0,100)
mipslope_histo = ROOT.TH1F('mipslope', 'MIP Slope MeV/ADC',100,0,100)
#coverage_histo = ROOT.TH2F('coverage', 'In Muon Beam Acceptance',20, 0, 20, 12, 0, 12)

c.Print(fitfile1+".pdf[")
c.SetLogy(1)
fig = []
ax = []
pp = PdfPages(fitfile2)
csvfile = open(outputMipCsvName, 'w', newline='')
writer = csv.writer(csvfile, delimiter=',', quotechar='"')
header = ["DetID", "ElLoc", "ADC_PEDESTAL", "MIPMPV_ADC", "MIPWIDTH_ADC", "MIP_SLOPE"]
writer.writerow(header)
i = 0
calib = {}
mpv_id = {}
width_id = {}
mipslope_id = {}
for id in maxadc_histo:
    elloc = eloc[id]
    pedestal = ped[id]
    mpv_id[id], width_id[id], isGoodFit = MIPFit(maxadc_histo[id], c, fitfile1)
    mpv_histo.Fill(i, mpv_id[id])
    mipwidth_histo.Fill(i, width_id[id])
    if(np.isnan(mpv_id[id]) or np.isnan(width_id[id])):
        print("Fit failed {0}".format(elloc))
        line = [str(id), elloc, str(pedestal), "-9999", "-9999", "-9999"]
        writer.writerow(line)
        i = i + 1
        continue
    print("{0} {1}".format(mpv_id[id], width_id[id]))
    mipslope_id[id] = fitMipSlope(mpv_id[id], width_id[id])
    mipslope_histo.Fill(i, mipslope_id[id])
    line = [str(id), elloc, str(pedestal), str(mpv_id[id]), str(width_id[id]), str(mipslope_id[id])]
    writer.writerow(line)
    i = i + 1

c.SetLogy(0)
mpv_histo.Draw("hist")
c.Print(fitfile1+".pdf")
mipwidth_histo.Draw("hist")
c.Print(fitfile1+".pdf")
mipslope_histo.Draw("hist")
c.Print(fitfile1+".pdf")
#coverage_histo.Draw("COLZ")
#c.Print(outfile+".pdf")

c.Print(fitfile1+".pdf]")

c.Clear()
pp.close()

f.Write()
f.Close()
