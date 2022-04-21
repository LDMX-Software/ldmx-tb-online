#usage: 
# For 1 pedestal file:  ldmx python3 pedestal-finder.py pedestal_run.root 
# For 2 pedestal files: ldmx python3 pedestal-finder.py pedestal_run.root pedestal_run2.root 

from mapping import *
import csv
import ROOT as r
from optparse import OptionParser

parser = OptionParser()	
parser.add_option('-o','--outputPath', dest='outputPath', default = '', help='Determines the output folder')
options = parser.parse_args()[0]
outputPath = options.outputPath
if outputPath != '' and outputPath[-1] != '/': outputPath +='/'

IDpositions={}
hists={}
inputFiles=[]

try:
    inputFileName=sys.argv[1]
    inputFileNameNoExtension=sys.argv[1][inputFileName.find('adc'):inputFileName.find('.root')]
    inputFile=r.TFile(inputFileName, "read")
    inputFiles.append(inputFile)
except: raise NameError('At least one pedestal file needs to be specified.')

try:
# I can't make this into a simple loop because root, for some completely cursed reason, 
# kills any histograms created after inputFileName if inputFileName is replaced with the second file
    inputFileName2=sys.argv[2]
    inputFileName2NoExtension=sys.argv[2][inputFileName2.find('adc'):inputFileName2.find('.root')]
    inputFile2=r.TFile(inputFileName2, "read")
    inputFiles.append(inputFile2)


except: print('Only 1 pedestal file was specified, which is fine.')




for dataFile in inputFiles:
    allData=dataFile.Get('ntuplizehgcroc').Get("hgcroc") #
    for t in allData : #for timestamp in allData
        if t.raw_id not in hists:
            hists[t.raw_id] = r.TH1F(str(t.raw_id),'',1024,0,1024)
            try: #if the eids are NOT kept during decoding, fills in incorrect values until someone asks me to fix it
                polarfire= t.layer
                hrocindex= t.strip
                channel= t.end
            except: #if the eids are kept during decoding, fills in correct values
                polarfire= t.fpga
                hrocindex= int(t.link/2)
                channel= 36*(t.link%2) + t.channel

            IDpositions[t.raw_id] = str(polarfire)+':'+str(hrocindex)+':'+str(channel)            
        hists[t.raw_id].Fill(t.adc)
# print(hists)
# print('____________________________________________________________________________________________')
# print(hists)
# for i in range(2,3):


#     allData=inputFile.Get('ntuplizehgcroc').Get("hgcroc") #
#     for t in allData : #for timestamp in allData
#         if t.raw_id not in hists:
#             hists[t.raw_id] = r.TH1F(str(t.raw_id),'',1024,0,1024)
#             polarfire= t.fpga
#             hrocindex= int(t.link/2)
#             channel= 36*(t.link%2) + t.channel
#             IDpositions[t.raw_id] = str(polarfire)+':'+str(hrocindex)+':'+str(channel)
#         hists[t.raw_id].Fill(t.adc)
# print(hists)

# try:
#     inputFileName2=sys.argv[2]
#     inputFileName2NoExtension=sys.argv[2][inputFileName2.find('adc'):inputFileName2.find('.root')]
#     inputFile2=r.TFile(inputFileName2, "read")
#     allData=inputFile2.Get('ntuplizehgcroc').Get("hgcroc") #
#     for t in allData : #for timestamp in allData
#         if t.raw_id not in hists:
#             hists[t.raw_id] = r.TH1F(str(t.raw_id),'',1024,0,1204)
#             polarfire= t.fpga
#             hrocindex= int(t.link/2)
#             channel= 36*t.link%2 + t.channel
#             IDpositions[t.raw_id] = str(polarfire)+':'+str(hrocindex)+':'+str(channel)
#         hists[t.raw_id].Fill(t.adc)
# except: print("Second pedestal root file unspecified, missing, or invalid. Moving on.")

outputFileName = outputPath+'pedestals_'+inputFileNameNoExtension

try: outputFileName += '_'+inputFileName2NoExtension
except: pass

csvfile = open('Pedestals/'+outputFileName+'.csv', 'w', newline='')
csvwriter = csv.writer(csvfile, delimiter=',')

csvwriter.writerow(['DetID', 'ElLoc', 'ADC_PEDESTAL','ADC_GAIN','TOT_PEDESTAL','TOT_GAIN'])
pedestalPlot = r.TH1F('','Pedestals',384,0,0)

for i in hists: 
    # print(hists[i])
    μ = hists[i].GetMean()
    pedestalPlot.Fill(int(i),μ)
    csvwriter.writerow([i, IDpositions[i], μ,1.2,1.0,2.5])

# file = r.TFile(outputFileName+'.root', "RECREATE")
# pedestalPlot.SetDirectory(file)
# pedestalPlot.Write()
# file.Close()

        

 