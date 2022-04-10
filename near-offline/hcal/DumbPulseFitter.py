import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors, TF1
from ROOT import gROOT, gStyle, gSystem, gPad

#Usage: ldmx python3 DumbPulseFitter.py <unpacked file>.root

fitfunc = TF1("fitfunc", "[0]*(x/[2])^4*exp(-(x-[1])/[2]^2)", 1, 8) #A * (t/tau)^n * e^(-(t-B)/tau)
#fitfunc = TF1("fitfunc", "gaus", 2, 8)

def FitPulse(histo, fitfunc, sample1 = 1, sample2 = 8):
    fitfunc.SetParameters(25., 2, 1)
    fit = histo.Fit(fitfunc, "LSQIM", "", sample1, sample2)
    peaktime = 4*fit.Get().Parameter(2) #n*tau
    fitfunc.SetParameters(fit.Get().Parameter(0),fit.Get().Parameter(1),fit.Get().Parameter(2))
    peak = fitfunc(peaktime)
    #return fit.Get().Parameter(0), fit.Get().Parameter(1) * 25
    return peak, peaktime * 25

gSystem.Load("libFramework.so")

inputFile=TFile(sys.argv[1], "read") #input file from arguments
outputFileName = 'hist_fit_exp_'+sys.argv[1] #ouptut file name
tree=inputFile.Get("LDMX_Events") #get tree

outfile = "samplepulse_exp_"+sys.argv[1]

f = ROOT.TFile(outputFileName,'recreate') #create root output file
c = TCanvas("c","c",800,600)

adc_hist = {} #histo map to channel
max_adc_hist = {} #histo map to channel
fitpeak_hist = {} #histo map to channel
fittime_hist = {} #histo map to channel
sum_hist = {} #histo map to channel

#Pedestal is hard-coded for now
ped = {}
chanlist = [411042048,411042049,411042050,411042051,411042052]
#pedlist = [171,163,198,163,192]
pedlist = [127,109,157,89,150]

for i in range(len(chanlist)):
    ped[chanlist[i]] = pedlist[i]

plotpulse = True
npulse = 100 #Plot sample pulses
minadc = 0 #Require minadc
maxevent = 10000 #Number of events
n = 0 #Pulse Count
event = 0 #Event count

c.Print(outfile+".pdf[")
for e in tree : #Loop over events in tree
    event = event + 1
    if True:
        for d in e.ChipSettingsTestDigis_unpack: #Loop over channels
            if d.id() not in adc_hist : #Create map key if not already there
               adc_hist[d.id()] = ROOT.TH1F(f'adc_eid_{d.id()}', f'ADC EID {d.id()}',1024,0,1024)
               max_adc_hist[d.id()] = ROOT.TH1F(f'maxsample_eid_{d.id()}', f'Max Sample EID {d.id()}',9,0,9)
               fitpeak_hist[d.id()] = ROOT.TH1F(f'fitpeak_eid_{d.id()}', f'Fit Peak EID {d.id()}',1000,0,500)
               fittime_hist[d.id()] = ROOT.TH1F(f'fittime_eid_{d.id()}', f'Fit Time EID {d.id()}',100,0,8*25)
               sum_hist[d.id()] = ROOT.TH1F(f'adcsum_eid_{d.id()}', f'ADC Sum EID {d.id()}',1000,0,500)
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
               sumadc = sumadc + d.at(i).adc_t()
            peakadc = -9999.
            peaktime = -9999.
            if(d.id() in chanlist and maxadc > minadc):
                peakadc, peaktime = FitPulse(histotemp, fitfunc)
                if(plotpulse):
                    histotemp.Draw()
                    c.Print(outfile+".pdf")
                    for i in range(d.size()) :
                       print("{0} {1}".format(i, d.at(i).adc_t()))
                    if(n == npulse):
                        c.Print(outfile+".pdf]")
                        plotpulse = False
                n = n + 1
            adc_hist[d.id()].Fill(maxadc) #Fill ADC count
            max_adc_hist[d.id()].Fill(maxsample)
            fitpeak_hist[d.id()].Fill(peakadc)
            sum_hist[d.id()].Fill(sumadc/8.)
            fittime_hist[d.id()].Fill(peaktime)
            del histotemp
    if (event > maxevent):
        break

#Close and write file
for id in adc_hist:
    adc_hist[id].GetXaxis().SetTitle("max ADC")
    max_adc_hist[id].GetXaxis().SetTitle("max Sample")
    fitpeak_hist[id].GetXaxis().SetTitle("Fitted Peak (ADC)")
    fittime_hist[id].GetXaxis().SetTitle("Time at Peak (ns)")
    sum_hist[id].GetXaxis().SetTitle("Sum ADC / 8 (ADC)")
f.Write()
f.Close()
