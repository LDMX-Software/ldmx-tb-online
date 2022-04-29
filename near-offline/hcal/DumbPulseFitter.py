import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad
from scipy.optimize import curve_fit
import numpy as np
from scipy.integrate import quad
import matplotlib
from matplotlib.backends.backend_pdf import PdfPages
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#Usage: ldmx python3 DumbPulseFitter.py <unpacked file>.root

fitfunc = TF1("fitfunc", "[0]*(x/[1])^4*exp(-x/[1])", 0, 8)

def FitPulse(histo, fitfunc, sample1 = 0, sample2 = 8):
    fitfunc.SetParameters(25., 1)
    fit = histo.Fit(fitfunc, "LSQIM", "", sample1, sample2)
    peaktime = 4*fit.Get().Parameter(1) #n*tau
    fitfunc.SetParameters(fit.Get().Parameter(0),fit.Get().Parameter(1),fit.Get().Parameter(2))
    peak = fitfunc(peaktime)
    integral = fitfunc.Integral(sample1, sample2)
    return peak, peaktime * 25, integral

def Pulsefit(x, a, b):
    return a*(x/b)**4 * np.exp(-x/b)

def FitSciPyPulse(histo, plotPulse=False):
    x = [0, 1, 2, 3, 4, 5, 6, 7]
    y = []
    for i in range(histo.GetNbinsX()):
        y.append(histo.GetBinContent(i+1))
    x_fit = np.linspace(0,7,100) # Create a simple list
    try:
        popt, pcov = curve_fit(Pulsefit, x, y, bounds=([0, 0], [50, 2]))
    except RuntimeError:
        popt = [0., 1.]
        pcov = [[1., 1.],[1., 1.]]
        print("Poor fit.")
    if(plotPulse):
        fig, ax = plt.subplots(1, 1)
        ax.scatter(x, y)
        ax.plot(x_fit, Pulsefit(x_fit, *popt), label = "Fit")
        ax.set_title("{0}".format(histo.GetTitle()))
        ax.set_xlabel("Sample") # Set x axis title
        ax.set_ylabel("ADC") # Set x axis title
        pp.savefig(fig)
        plt.close(fig)
        del fig
        del ax
    peaktime = 4*popt[1]
    peak = Pulsefit(peaktime, *popt)
    integral = quad(Pulsefit, 0, 30, args=(popt[0], popt[1]))[0]
    ls = 0
    maxerr = 0
    for i in range(len(x)):
        diff = y[i] - Pulsefit(x[i], *popt)
        ls = (diff)**2 + ls
        if(diff > maxerr):
            maxerr = diff
    chisq = ls/8**2
    return peak, peaktime * 25, integral, chisq, maxerr

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = '2hist_fit_exp_'+sys.argv[1] #ouptut file name
tree=inputFile.Get("LDMX_Events") #get tree

outfile = "2samplepulse_exp_"+sys.argv[1]
pp = PdfPages(outfile+".pdf")

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)

adc_hist = {} 
max_adc_hist = {}
fitpeak_hist = {}
fittime_hist = {}
sum_hist = {}
integral_hist = {}
chi2_2d_hist = {}
error_2d_hist = {}

#Pedestal is hard-coded for now
ped = {}

chanlist = [1275068416, 1275068417, 1275068418, 1275068419] #Hardcoded
pedlist = [91.04825, 91.561375, 94.45425, 94.91525] #Hardcoded

for i in range(len(chanlist)):
    ped[chanlist[i]] = pedlist[i]

plotpulse = True
npulse = 100 #Plot sample pulses
minadc = 0 #Require minadc
maxevent = 10000 #Number of events
n = 0 #Pulse Count
event = 0 #Event count

chisq_histo = ROOT.TH1F(f'chisq_', f'Chi Sq',250,0,2)
maxerror_histo = ROOT.TH1F(f'errormax_', f'Max Error',250,0,2)

for e in tree : #Loop over events in tree
    event = event + 1
    if(event%100 == 0):
        print("Event {0}".format(event))
    if True:
        for d in e.ChipSettingsTestDigis_unpack: #Loop over channels
            if d.id() not in adc_hist : #Create map key if not already there
               adc_hist[d.id()] = ROOT.TH1F(f'adc_eid_{d.id()}', f'ADC EID {d.id()}',1024,0,1024)
               max_adc_hist[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',9,0,9)
               fitpeak_hist[d.id()] = ROOT.TH1F(f'fitpeak_eid_{d.id()}', f'Fit Peak EID {d.id()}',100,0,50)
               fittime_hist[d.id()] = ROOT.TH1F(f'fittime_eid_{d.id()}', f'Fit Time EID {d.id()}',100,0,8*25)
               sum_hist[d.id()] = ROOT.TH1F(f'adcsum_eid_{d.id()}', f'ADC Sum EID {d.id()}',3000,0,3000)
               integral_hist[d.id()] = ROOT.TH1F(f'int_eid_{d.id()}', f'Integral EID {d.id()}',300,0,300)
               chi2_2d_hist[d.id()] = ROOT.TH2F(f'chisq_2d_eid_{d.id()}', f'Chisq 2D EID {d.id()}',100,0,2, 300,0,300)
               error_2d_hist[d.id()] = ROOT.TH2F(f'error_2d_eid_{d.id()}', f'Error 2D EID {d.id()}',100,0,2, 300,0,300)
            maxadc = -9999.
            maxsample = -9999
            histotemp = TH1D("histotemp", "histotemp", 8, 0, 8)
            pedestal = -9999.
            if d.id() in ped :
                pedestal = ped[d.id()]
            sumadc = 0
            for i in range(d.size()) :
               histotemp.Fill(i, d.at(i).adc_t()-pedestal)
               if(d.at(i).adc_t() > maxadc):
                   maxsample = i
                   maxadc = d.at(i).adc_t()
               sumadc = sumadc + d.at(i).adc_t()-pedestal
            peakadc = -9999.
            peaktime = -9999.
            integral = -9999.
            if(d.id() in chanlist and maxadc > minadc):
                #peakadc, peaktime, integral = FitPulse(histotemp, fitfunc)
                peakadc, peaktime, integral, chisq, max_err = FitSciPyPulse(histotemp, plotpulse)
                maxerror_histo.Fill(max_err/peakadc)
                chisq_histo.Fill(chisq)
                chi2_2d_hist[d.id()].Fill(chisq, integral)
                error_2d_hist[d.id()].Fill(max_err/peakadc, integral)
                if(n == npulse):
                    plotpulse = False
                    pp.close()
                n = n + 1
            adc_hist[d.id()].Fill(maxadc) #Fill ADC count
            max_adc_hist[d.id()].Fill(maxsample)
            fitpeak_hist[d.id()].Fill(peakadc)
            sum_hist[d.id()].Fill(sumadc/8.)
            fittime_hist[d.id()].Fill(peaktime)
            integral_hist[d.id()].Fill(integral)
            del histotemp
    if (event > maxevent):
        break

#Close and write file
chisq_histo.SetTitle("Chi sq")
chisq_histo.GetXaxis().SetTitle("Chi sq")
maxerror_histo.SetTitle("Max Error")
maxerror_histo.GetXaxis().SetTitle("Max Error")
for id in adc_hist:
    adc_hist[id].GetXaxis().SetTitle("max ADC")
    max_adc_hist[id].GetXaxis().SetTitle("max Sample")
    fitpeak_hist[id].GetXaxis().SetTitle("Fitted Peak (ADC)")
    fittime_hist[id].GetXaxis().SetTitle("Time at Peak (ns)")
    sum_hist[id].GetXaxis().SetTitle("Sum ADC / 8 (ADC)")
    integral_hist[id].GetXaxis().SetTitle("Fit Integral (ADC*ns)")
f.Write()
f.Close()
