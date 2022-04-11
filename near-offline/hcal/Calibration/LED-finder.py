#usage: ldmx python3 LED-finder.py LED_run.root pedestals.csv

from mapping import *
import csv
import ROOT as r

pedestalFileName=sys.argv[2]
csv_reader = csv.reader(open(pedestalFileName), delimiter=',')

pedestals ={}
for row in csv_reader:
    try:  pedestals[int(row[0])] = float(row[2])
    except:pass
# print(pedestals)


inputFileName=sys.argv[1]
inputFile=r.TFile(inputFileName, "read")
allData=inputFile.Get('ntuplizehgcroc').Get("hgcroc") #

IDpositions={}
hists={}
for t in allData : #for timestamp in allData
    if t.raw_id not in hists:
        hists[t.raw_id] = r.TH1F(str(t.raw_id),'',1024,0,1204)
        polarfire= t.fpga
        hrocindex= int(t.link/2)
        channel= 36*t.link%2 + t.channel
        IDpositions[t.raw_id] = str(polarfire)+':'+str(hrocindex)+':'+str(channel)
    hists[t.raw_id].Fill(t.adc)

csvfile = open('LEDs.csv', 'w', newline='')
csvwriter = csv.writer(csvfile, delimiter=',')

csvwriter.writerow(['DetID', 'ElLoc', 'ADC_PEDESTAL', 'LED_MEDIAN'])

for i in hists:
    # fit = hists[i].Fit('gaus','Sq') #so fits are awful
    # μ = fit.Parameter(1)
    μ = hists[i].GetMean()-pedestals[i]
    # print(i)
    # print(type(i))
    csvwriter.writerow([i, IDpositions[i], pedestals[i], μ])



 
