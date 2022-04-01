#How to use:
#Decode a x.raw file using decode.py
#Use the following to get a map of ADCs:
# ldmx python3 map.py adc_x.root

#A program that maps the testbeam channels to the sipms.
#So far untested. Expected to work when all HGCROCs are connected

from numpy import *
import sys
import ROOT
from ROOT import TCanvas, TPad, TFile, TPaveLabel, TPaveText, TStyle, TTree, TH1D, TH2D, TLegend, TGraph, TGraphErrors
from ROOT import gROOT, gStyle, gSystem, gPad
ROOT.gROOT.SetBatch(1); 
gSystem.Load("libFramework.so")
inputFile=TFile(sys.argv[1], "read")

# converts the link-channel provided by the HGCROC into a 'real' channel: goes from 1 to 384, representing a SiPM each
def FpgaLinkChannel_to_realChannel(FpgaLinkChannel): #link is the chip halves, channel is just the channel
    channel = FpgaLinkChannel[2]-1
    if 0 <= channel and channel <= 7:  realChannel = channel
    elif 9 <= channel and channel <= 16:  realChannel = channel-1
    elif 19 <= channel and channel <= 26:  realChannel = channel-3
    elif 28 <= channel and channel <= 35:  realChannel = channel-4
    else: realChannel = None

    if realChannel != None: realChannel+=FpgaLinkChannel[1]*32
    if realChannel != None: realChannel+=FpgaLinkChannel[0]*32*6
    return realChannel

#converts the 'real' channel into a 3 vector that describes the SiPM really well
def realChannel_to_SipM(c):#[layer,bar,side]  
    if c == None: return None      
    for i in range(1,10):
        if 0 <= c and c <= 3: return [i,c,0]
        if 4 <= c and c <= 7: return [i,c-4,1]
        if 8 <= c and c <= 11: return [i,c-4,0]
        if 12 <= c and c <= 15: return [i,c-8,1]
        c-=16
    for i in range(10,20): 
        if 0 <= c and c <= 3: return [i,c,0]
        if 4 <= c and c <= 7: return [i,c-4,1]
        if 8 <= c and c <= 11: return [i,c-4,0]
        if 12 <= c and c <= 15: return [i,c-8,1]
        if 16 <= c and c <= 19: return [i,c-8,0]
        if 20 <= c and c <= 23: return [i,c-12,1]
        c-=24   
    return 'too many layers'    


#prepares the plot
allData=inputFile.Get('ntuplizehgcroc').Get("hgcroc") #
ROOT.gStyle.SetOptStat("ne")
c=ROOT.TCanvas('t','The canvas of anything', 1100, 900)
c.cd()
hist = ROOT.TH2F('Map', "Mapped average SiPM ADCs",40, 0.5, 40.5, 12, -0.5, 11.5)
adcCountMap = zeros((40,12))
adcSumMap = zeros((40,12))






#averages all adcs in a python array
for t in allData : #for timestamp in allData
    realChannel = FpgaLinkChannel_to_realChannel([t.fpga,t.link,t.channel])
    if realChannel != None:
        LayerBarSide = realChannel_to_SipM(realChannel)
        if LayerBarSide[2]==1: LayerBarSide[0] +=20
        adcCountMap[LayerBarSide[0],LayerBarSide[1]] +=1 
        adcSumMap[LayerBarSide[0],LayerBarSide[1]] +=t.adc


#fills a ROOT histogram with the sipm averages
adcCountMap[adcCountMap == 0 ] = 1 
for i in range(40):
    for j in range(12):
        hist.Fill(i+1,j,adcSumMap[i,j]/adcCountMap[i,j])        


hist.SetYTitle('Bar number')
hist.SetXTitle('Layer number')
hist.Draw("COLZ")

#a helpful label to explain the plot
label = ROOT.TLatex()
label.SetTextFont(42)
label.SetTextSize(0.025)
label.SetNDC()
label.DrawLatex(0,  0.92, "Left side: left and top SiPMs. Right side: right and bottom SiPMs.")
label.DrawLatex(0,  0.04, "Odd layers: vertical bars; Even layers: horizontal bars")

c.SaveAs("map.png")  