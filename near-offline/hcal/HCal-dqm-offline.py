#usage: ldmx python3 HCal-dqm-offline.py adc_<run-number>.root
#dqm = data quality monitoring

from mapping import *
from optparse import OptionParser
import os
import csv

for directory in ['Plots','Raws','Decoded','LEDs','Pedestals']:
    try: os.stat(directory)
    except: os.mkdir(directory)

parser = OptionParser()	
parser.add_option('-t','--threshold', dest='includeThresholdPlots', default = True, help='Determines if PE threshold plots should be included. Leave blank or True to inlcude, anything else ot exclude.')
parser.add_option('-p','--pedestal', dest='pedestalFile', default = 'Pedestals/DumbReconConditions.csv', help='Determines the pedestals from provided file')
options = parser.parse_args()[0]
includeThresholdPlots = options.includeThresholdPlots
pedestalFile = options.pedestalFile

# csvFile=open()
ReconConditionsFileLocation = pedestalFile
csv_reader = csv.reader(open(ReconConditionsFileLocation), delimiter=',')

def addLabel():
    label = r.TLatex()
    label.SetTextFont(42)
    label.SetTextSize(0.025)
    label.SetNDC()
    return label
 


#keeps only events we are interested in
eventsOfInterest = range(12,100)
channelsOfInterest = range(0,40)

inputFileName=sys.argv[1]
inputFile=r.TFile(inputFileName, "read")
allData=inputFile.Get('ntuplizehgcroc').Get("hgcroc") #
r.gStyle.SetOptStat("ne")

#defines boundaries
def getTimestampRange(allData):
    samples=[]
    sample_sample=40 #the number of timestamps it looks at before it decides the range
    for t in allData:
        samples.append(t.i_sample)
        sample_sample+=1
        if len(samples)>sample_sample: break
    return  range(min(samples),max(samples)+1)
timestampRange=getTimestampRange(allData)
print('The number of timestamps is',len(timestampRange))
channelRange=range(0,384)
ADCRange=range(0,1024)
TOTRange=range(0,1024)
TOARange=range(0,1024)
PERange=range(0,30)
barMapRange=range(1,13)
layerRange=range(1,41)
realLayerRange=range(1,21)
thresholdCountMap = zeros((40,13))
adcCountMap = zeros((40,13))
adcSumMap = zeros((40,13))

#sets the threshold
threshold_PE = 5. #aribtrary for now
energy_per_mip = 4.66 #MeV/MIP
voltage_hcal = 5. #mV/PE
PE_per_mip = 68. #PEs/mip
mV_per_PE = 1/energy_per_mip * voltage_hcal * PE_per_mip #mV per MIP is about 73 for now
adc_ped = 1. #Dummy Value
# adc_gain = 1.2 #Dummy Value
# threshold = adc_ped + mV_per_PE / adc_gain * threshold_PE
# print('threshold is an ADC of',threshold)
def ADC_to_PE(adc): return adc*adc_gain/mV_per_PE 
def calculateThreshold(adc_gain): return adc_ped + mV_per_PE / adc_gain * threshold_PE
def ADC_to_E(adc): return adc*energy_per_mip/(2*mV_per_MIP)
mV_per_MIP = 340 #varies per bar...
#energy calculations

thresholds=[]
pedestals=[]
for row in csv_reader:
    try:    
        [fpga,ROC,channel] = row[1].split(':') 
        # a slightly different format, that shall now be converted
        fpga = int(fpga)
        link = int(ROC)*2+int( (int(channel)) /36 ) 
        channel = int(channel)%36
        RealChannel = FpgaLinkChannel_to_realChannel([fpga,link,channel])
    except: RealChannel = None
    if RealChannel != None: 
        adc_gain = 1.2#float(row[3]) 
        pedestal = float(row[2])
        thresholds.append(calculateThreshold(adc_gain))
        pedestals.append(pedestal)

# print(len(pedestals))
# print(pedestals)
#prepares plots
hists = {}

hists["ADC-of-channel"] =  r.TH2F("ADC-of-channel", "ADC-of-channel", 
        len(channelRange), channelRange[0]-0.5, channelRange[-1]-0.5,
        len(ADCRange), ADCRange[0]-0.5, ADCRange[-1]-0.5,)
hists["ADC-of-channel"].SetYTitle('ADC (of timestamps)')
hists["ADC-of-channel"].SetXTitle('Channel (only real ones)')   

hists["TOT-of-channel"] =  r.TH2F("TOT-of-channel", "TOT-of-channel", 
        len(channelRange), channelRange[0]-0.5, channelRange[-1]-0.5,
        len(TOTRange), TOTRange[0]-0.5, TOTRange[-1]-0.5,)
hists["TOT-of-channel"].SetYTitle('TOT (of timestamps)')
hists["TOT-of-channel"].SetXTitle('Channel (only real ones)')        

hists["TOA-of-channel"] =  r.TH2F("TOA-of-channel", "TOA-of-channel", 
        len(channelRange), channelRange[0]-0.5, channelRange[-1]-0.5,
        len(TOARange), TOARange[0]-0.5, TOARange[-1]-0.5,)
hists["TOA-of-channel"].SetYTitle('TOA (of timestamps)')
hists["TOA-of-channel"].SetXTitle('Channel (only real ones)')        

hists["ADC-of-sample"] =  r.TH2F("ADC-of-sample", "ADC-of-sample", 
        len(timestampRange), timestampRange[0]-0.5, timestampRange[-1]+0.5,
        len(ADCRange), ADCRange[0]-0.5, ADCRange[-1]-0.5,)
hists["ADC-of-sample"].SetYTitle('ADC')
hists["ADC-of-sample"].SetXTitle('Sample')  

hists["event-of-max_sample"] =  r.TH1F("event-of-max_sample", "# of times x sample had the highest ADC", 
        len(timestampRange), timestampRange[0]-0.5, timestampRange[-1]+0.5,)
hists["event-of-max_sample"].SetYTitle('# of SiPMs')
hists["event-of-max_sample"].SetXTitle('Sample')  

hists["event-of-PE"] =  r.TH1F("event-of-PE", "# of times a SiPM got x # of PEs", 
        len(PERange), PERange[0]-0.5, PERange[-1]+0.5,)
hists["event-of-PE"].SetYTitle('# of SiPMs')
hists["event-of-PE"].SetXTitle('PE')  

hists["PE-of-channel"] =  r.TH2F("PE-of-channel", "PE-of-channel", 
        len(channelRange), channelRange[0]-0.5, channelRange[-1]-0.5,
        len(PERange), PERange[0]-0.5, PERange[-1]+0.5,)
hists["PE-of-channel"].SetYTitle('PE')
hists["PE-of-channel"].SetXTitle('Channel')  

hists["max_sample-of-channel"] =  r.TH2F("max_sample-of-channel", "max_sample-of-channel", 
        len(channelRange), channelRange[0]-0.5, channelRange[-1]-0.5,
        len(timestampRange), timestampRange[0]-0.5, timestampRange[-1]+0.5,)
hists["max_sample-of-channel"].SetYTitle('Timestamp')
hists["max_sample-of-channel"].SetXTitle('Channel') 

hists["map"] =  r.TH2F("map", "Map of average ADCs", 
        len(layerRange), layerRange[0]-0.5, layerRange[-1]+0.5,
        len(barMapRange), barMapRange[0]-0.5, barMapRange[-1]+0.5,)
hists["map"].SetYTitle('Bar')
hists["map"].SetXTitle('Layer') 

hists["thresholdMap"] =  r.TH2F("thresholdMap", "Map of threshold exceeding hits", 
        len(layerRange), layerRange[0]-0.5, layerRange[-1]+0.5,
        len(barMapRange), barMapRange[0]-0.5, barMapRange[-1]+0.5,)
hists["thresholdMap"].SetYTitle('Bar')
hists["thresholdMap"].SetXTitle('Layer') 


for i in range(eventsOfInterest[0],eventsOfInterest[0]+12):
    hists["eventDisplay"+str(i)] =  r.TH2F("eventDisplay"+str(i), "(not yet accurate) Edep map event "+str(i), 
        len(realLayerRange), realLayerRange[0]-0.5, realLayerRange[-1]+0.5,
        len(barMapRange), barMapRange[0]-0.5, barMapRange[-1]+0.5,)
    hists["eventDisplay"+str(i)].SetYTitle('Bar')
    hists["eventDisplay"+str(i)].SetXTitle('Layer') 

#Gets data from interesting events
maxADC=0
maxSample=-1


for t in allData : #for timestamp in allData
    if t.event in eventsOfInterest:
        realChannel = FpgaLinkChannel_to_realChannel([t.fpga,t.link,t.channel])
        # print(([t.fpga,t.link,t.channel]))
        if realChannel != None: 
   
            hists["ADC-of-channel"].Fill(realChannel,t.adc)
            hists["TOT-of-channel"].Fill(realChannel,t.tot)
            hists["TOA-of-channel"].Fill(realChannel,t.toa)
            hists["ADC-of-sample"].Fill(t.i_sample,t.adc)


            #fills the map
            LayerBarSide = realChannel_to_SiPM_fast[realChannel].copy() #the copy is what makes the fast not so fast
            if LayerBarSide[0] in range(0,9): visual_offset=3
            else: visual_offset=1
            if LayerBarSide[2]==1: visual_SiPM_offset=20
            else: visual_SiPM_offset=0       

            adcCountMap[LayerBarSide[0]+visual_SiPM_offset,LayerBarSide[1]+visual_offset] +=1 
            adcSumMap[LayerBarSide[0]+visual_SiPM_offset,LayerBarSide[1]+visual_offset] +=t.adc   

            if maxADC<t.adc: 
                maxADC=t.adc
                maxSample=t.i_sample
            if t.i_sample == timestampRange[-1]: 
                # if maxADC>0: #future option for non-PE related threshold
                hists["event-of-max_sample"].Fill(maxSample)
                hists["max_sample-of-channel"].Fill(realChannel,maxSample)

                if "eventDisplay"+str(t.event) in hists:
                    
                    if maxADC>pedestals[realChannel]+20 : #temporary arbitrary adc threshold for the event displays
                        # hists["eventDisplay"+str(t.event)].Fill(LayerBarSide[0],LayerBarSide[1]+visual_offset,ADC_to_E(maxADC))
                        hists["eventDisplay"+str(t.event)].Fill(LayerBarSide[0],LayerBarSide[1]+visual_offset,maxADC)
                        # if t.event==13:
                        #     print('For channel:',[t.fpga,t.link,t.channel])
                        #     print('Which is in:',LayerBarSide[0],LayerBarSide[1]+visual_offset)
                        #     print('It records as',maxADC)
                        #     print('With a pedestal of',pedestals[realChannel],)

                if maxADC>thresholds[realChannel]:   
                    hists["event-of-PE"].Fill(ADC_to_PE(maxADC))
                    hists["PE-of-channel"].Fill(realChannel,ADC_to_PE(maxADC))
                    thresholdCountMap[LayerBarSide[0]+visual_SiPM_offset,LayerBarSide[1]+visual_offset] +=1 
                    
                maxADC=0
                maxSample=-1

 

adcCountMap[adcCountMap == 0 ] = 1 
for i in range(40):
    for j in range(13):
        # if i in range(0,10) or i in range(20,30): visual_offset=2
        # print()
        hists["map"].Fill(i,j,adcSumMap[i,j]/adcCountMap[i,j])     
        hists["thresholdMap"].Fill(i,j,thresholdCountMap[i,j])


#overview page
saveFileName="Plots/Hcal-dqm_"+inputFileName[inputFileName.find('adc'):inputFileName.find('.root')]
c = r.TCanvas('','', 300, 300)
c.Divide(3,3)
c.cd(1)
hists["ADC-of-channel"].Draw('COLZ')
c.cd(2)
hists["TOT-of-channel"].Draw('COLZ')
c.cd(3)
hists["TOA-of-channel"].Draw('COLZ')
c.cd(4)
hists["ADC-of-sample"].Draw('COLZ')
c.cd(5)
hists["event-of-max_sample"].Draw('HIST')
if includeThresholdPlots == True:
    c.cd(6)
    hists["event-of-PE"].Draw('HIST')
    c.cd(7)
    hists["PE-of-channel"].Draw('COLZ')
c.cd(8)
hists["max_sample-of-channel"].Draw('COLZ')



for i in range(1,10):c.GetPad(i).SetLeftMargin(0.12)
c.cd(0)
label = addLabel() 
if len(eventsOfInterest) == 1: context= "This is only event "+str(eventsOfInterest[0])
else: context="These are events "+str(eventsOfInterest[0])+" to "+str(eventsOfInterest[-1]) 
label.DrawLatex(0,  0, context)  

c.Print(saveFileName+".pdf(","Title:Overview page");
c.Close()

#event display page
c = r.TCanvas('','', 400, 300)
c.Divide(4,3)

for i in range(0,12):
    c.cd(1+i)
    hists["eventDisplay"+str(i+eventsOfInterest[0])].Draw('COLZ')
label = addLabel()
c.cd(0)
label.DrawLatex(0,  0, "Odd layers: vertical bars; Even layers: horizontal bars")
label.DrawLatex(0.45,  0.98, "Event display")
c.SetTopMargin(0.12)
c.Print(saveFileName+".pdf(","Title: Event display page");
c.Close()

#SiPM map page
c = r.TCanvas('','', 200, 100)
c.Divide(2,1)
c.cd(1)
hists["thresholdMap"].Draw('COLZ')
c.cd(2)
hists["map"].Draw('COLZ')
c.cd(1)
label = addLabel()
label.DrawLatex(0,  0.91, "Left side: left and top SiPMs. Right side: right and bottom SiPMs.")
label.DrawLatex(0,  0.04, "Odd layers: vertical bars; Even layers: horizontal bars")
c.Print(saveFileName+".pdf(","Title:Mappage");
c.Close()


#per ROC pages
import copy
channelsPerROC=64
plots=(hists["ADC-of-channel"],hists["TOT-of-channel"],hists["TOA-of-channel"],hists["max_sample-of-channel"])
for hist in plots:
    c = r.TCanvas('','', 300, 200)
    c.Divide(3,2)
    subplots=[]
    for i in range(0,6):
        c.cd(i+1)
        c.GetPad(i+1).SetLeftMargin(0.12)
        subplots.append(copy.deepcopy(hist))
        subplots[-1].GetXaxis().SetRangeUser(i*channelsPerROC, (i+1)*channelsPerROC-1)
        subplots[-1].SetTitle('ROC '+str(i))
        subplots[-1].Draw('COLZ')
    if hist!= plots[-1]:c.Print(saveFileName+".pdf","Title:ROC breakdown");
    else:c.Print(saveFileName+".pdf)","Title:ROC breakdown");
    c.Close()




#makes the root histos
# for hist in hists:
#     file = r.TFile("plots/"+hists[hist].GetName()+".root", "RECREATE")
#     hists[hist].SetDirectory(file)
#     hists[hist].Write()
#     file.Close()
