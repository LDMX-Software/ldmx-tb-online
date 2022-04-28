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

#Usage: ldmx python3 HcalMipAna.py <unpacked file fpga 0>.root <unpacked filefpga 1>.root <pedestal file>.csv
#It can also work with just a single fgpa file

def MIPFit(histo, canvas, outfile):
    fitfunc = TF1("fitfunc", "landau", 200, 900)
    fit = histo.Fit(fitfunc, "LSQIM", "", 200, 900)
    fitfunc.SetParameters(fit.Get().Parameter(0), fit.Get().Parameter(1), fit.Get().Parameter(2))
    pre_mpv = fitfunc.GetMaximumX(200,900)#-9999
    fit2 = histo.Fit(fitfunc, "LSQIM", "", pre_mpv-150, pre_mpv+400)
    fitfunc.SetParameters(fit2.Get().Parameter(0), fit2.Get().Parameter(1), fit2.Get().Parameter(2))
    pre_mpv2 = fitfunc.GetMaximumX(200,900)#-9999
    fit3 = histo.Fit(fitfunc, "LSQIM", "", pre_mpv2-100, pre_mpv2+300)
    fitfunc.SetParameters(fit3.Get().Parameter(0), fit3.Get().Parameter(1), fit3.Get().Parameter(2))
    mpv = fitfunc.GetMaximumX(0,1000)#-9999
    maximum = fitfunc.GetMaximum(0,1000)
    halfmax = maximum / 2
    FWHMx1 = fitfunc.GetX(halfmax, -500, mpv)
    FWHMx2 = fitfunc.GetX(halfmax, mpv, 1000)
    FWHM = FWHMx2 - FWHMx1
    isGoodFit = 1 #Update Goodness of Fit test
    canvas.SetLogy(1)
    histo.Draw()
    canvas.Print(outfile+".pdf")
    return mpv, FWHM, isGoodFit
    #return 0, 0, isGoodFit

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
    x_fit = np.linspace(0,800,800) # Create a simple list
    popt, pcov = curve_fit(linefit, x, y)
    fig, ax = plt.subplots(1, 1)
    ax.scatter(x, y, label = "Calib")
    ax.plot(x_fit, linefit(x_fit, *popt), label = "Fit")
    ax.set_title("EID {0}".format(id))
    ax.set_xlabel("Sum ADC") # Set x axis title
    ax.set_ylabel("E dep (MeV)") # Set x axis title
    pp.savefig(fig)
    plt.close(fig)
    del fig
    del ax
    return popt[0]

gSystem.Load("libFramework.so")

pedfile = sys.argv[len(sys.argv)-1]
outputFileName = os.path.basename('sum_hist_mip_'+sys.argv[1]).replace("_fpga_0", "").replace("_fpga_1", "") #ouptut file name
outputMipCsvName = os.path.basename('sum_mip_calib_'+sys.argv[1]).replace('.root','.csv').replace("_fpga_0", "").replace("_fpga_1", "") #ouptut file name
fitfile1 = os.path.basename("sum_mip_fits_landua_"+sys.argv[1]).replace('.root','').replace("_fpga_0", "").replace("_fpga_1", "")
fitfile2 = os.path.basename("sum_mip_fits_linear_"+sys.argv[1]).replace('.root','.pdf').replace("_fpga_0", "").replace("_fpga_1", "")

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
#alladc_histo = {}
maxsample_histo = {}
max_histo = {}
sumadc_histo = {}

maxadc = -9999
maxsample = -9999
sumadc = -9999

hasMIPs = [1275068416, 1275068417, 1275068420, 1275068421,
    1275068428, 1275068432, 1275068434, 1275068435,
    1275068438, 1275068439, 1275068446, 1275068450,
    1275068454, 1275068455, 1275068458, 1275068459,
    1275068466, 1275068470, 1275068474, 1275068475,
    1275068478, 1275068481, 1275068485, 1275068492,
    1275068493, 1275068496, 1275068497, 1275068504,
    1275068508, 1275068512, 1275068513, 1275068516,
    1275068517, 1275068519, 1275068523, 1275068530,
    1275068531, 1275068534, 1275068535, 1275068542,
    1275068546, 1275068550, 1275068551, 1275068554,
    1275068555, 1275068557, 1275068561, 1275068568,
    1275068569, 1275068572, 1275068573, 1275068580,
    1275068584, 1275068595, 1275068596, 1275068597,
    1275068599, 1275068600, 1275068601, 1275068625,
    1275068626, 1275068627, 1275068629, 1275068630,
    1275068631,
    1275070249, 1275070250, 1275070251, 1275070253,
    1275070254, 1275070255, 1275070279, 1275070280,
    1275070281, 1275070283, 1275070284, 1275070285,
    1275070305, 1275070306, 1275070307, 1275070309,
    1275070310, 1275070311, 1275070335, 1275070336,
    1275070337, 1275070339, 1275070340, 1275070341]

for infile in infiles:
    print(infile)
    tree = infile.Get("LDMX_Events")
    for e in tree : #Loop over events in tree
        for d in e.ChipSettingsTestDigis_unpack : #Loop over channels
            #if(d.id() not in hasMIPs):
            #    continue
            if d.id() not in maxadc_histo : #Create map key if not already there
               maxadc_histo[d.id()] = ROOT.TH1F(f'maxadc_eid_{d.id()}', f'Max ADC EID {d.id()}',200,0,400)
               sumadc_histo[d.id()] = ROOT.TH1F(f'sumadc_eid_{d.id()}', f'Sum ADC EID {d.id()}',200,0,2000)
               maxsample_histo[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',8,0,8)
               max_histo[d.id()] = ROOT.TH2F(f'max_eid_{d.id()}', f'Max Sample vs Max ADC EID {d.id()}',1024,0,1024,8,0,8)
            maxadc = -9999.
            maxsample = -9999
            sumadc = 0
            for i in range(d.size()) : #Loop over sample number
               if(d.id() in ped):
                   adc_ped = d.at(i).adc_t() - ped[d.id()]
               else:
                   print("Key {0} not found. Setting pedestal to 0".format(d.id()))
                   adc_ped = d.at(i).adc_t()
               sumadc = sumadc + adc_ped
               if(adc_ped > maxadc):
                   maxadc = adc_ped
                   maxsample = i
            if(maxsample != 0 and maxsample != 1):
                maxsample_histo[d.id()].Fill(maxsample)
                maxadc_histo[d.id()].Fill(maxadc) #Fill ADC count
            sumadc_histo[d.id()].Fill(sumadc) #Fill ADC count
            max_histo[d.id()].Fill(maxadc, maxsample)

#Close and write file
for id in maxadc_histo:
    print("Set title {0}".format(id))
    maxadc_histo[id].GetXaxis().SetTitle("max ADC")
    maxsample_histo[id].GetXaxis().SetTitle("max Sample")
    max_histo[id].GetXaxis().SetTitle("max ADC")
    max_histo[id].GetYaxis().SetTitle("max Sample")
    sumadc_histo[id].GetXaxis().SetTitle("Sum ADC")
    #alladc_histo[id].GetXaxis().SetTitle("ADC")

mpv_histo = ROOT.TH1F('mpv', 'MIP MPV ADC Max',100,0,100)
mipwidth_histo = ROOT.TH1F('mipwidth', 'MIP Width ADC Max',100,0,100)
mipslope_histo = ROOT.TH1F('mipslope', 'MIP Slope MeV/ADC',100,0,100)

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
for id in sumadc_histo:
    elloc = eloc[id]
    pedestal = ped[id]
    mpv_id[id], width_id[id], isGoodFit = MIPFit(sumadc_histo[id], c, fitfile1)
    if(np.isnan(mpv_id[id]) or np.isnan(width_id[id])):
        print("Fit failed {0}".format(elloc))
        line = [str(id), elloc, str(pedestal), "-9999", "-9999", "-9999"]
        writer.writerow(line)
        i = i + 1
        continue
    print("{0} {1}".format(mpv_id[id], width_id[id]))
    mpv_histo.Fill(i, mpv_id[id])
    mipwidth_histo.Fill(i, width_id[id])
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

c.Print(fitfile1+".pdf]")

c.Clear()
pp.close()

f.Write()
f.Close()
