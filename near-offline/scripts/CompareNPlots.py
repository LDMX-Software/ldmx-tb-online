import sys
#tmpargv = sys.argv
#sys.argv = []
import getopt
import ROOT
from ROOT import gROOT, TFile, gDirectory, gStyle, TCanvas, TH1, TLegend
#sys.argv = tmpargv

#List arguments
def print_usage():
	print ("\nUsage: {0} <outfile base name> <root file 1> ... <root file n> <label 1> ... <label n>".format(sys.argv[0]))
	print ("Arguments: ")
	print ('\t-h: this help message')
	print ('\t-y: set log y axis')
	print ('\t-z: set log z axis')

logy = False
logz = False

options, remainder = getopt.gnu_getopt(sys.argv[1:], 'yzh')

# Parse the command line arguments
for opt, arg in options:
	if opt=='-y':
		logy = True
	if opt=='-z':
		logz = True
	if opt=='-h':
		print_usage()
		sys.exit(0)

def openPDF(outfile,canvas):
	canvas.Print(outfile+".pdf[")

def closePDF(outfile,canvas):
	canvas.Print(outfile+".pdf]")

def savehisto(histos,labels,outfile,canvas,XaxisTitle="",YaxisTitle="",plotTitle="",stats=0,logY=0):
	histos[0].SetTitle(plotTitle)
	histos[0].SetStats(0)
	histos[0].GetXaxis().SetTitle(XaxisTitle)
	histos[0].GetYaxis().SetTitle(YaxisTitle)
	maximum = histos[0].GetMaximum()
	for i in range(len(histos)):
		if(histos[i].GetMaximum() > maximum):
			maximum = histos[i].GetMaximum()
		histos[i].SetLineColor(i+1)
		if(i == 0):
			histos[i].Draw("")
		else:
			histos[i].Draw("same")
	if(not logY):
		histos[0].GetYaxis().SetRangeUser(0.,1.2*maximum)
	else:
		histos[0].GetYaxis().SetRangeUser(0.1,1.2*maximum)
	legend = TLegend(.68,.66,.92,.87)
	legend.SetBorderSize(0)
	legend.SetFillColor(0)
	legend.SetFillStyle(0)
	legend.SetTextFont(42)
	legend.SetTextSize(0.035)
	for i in range(len(labels)):
		legend.AddEntry(histos[i],labels[i],"LP")
	legend.Draw("")
	canvas.SetLogy(logY)
	canvas.Print(outfile+".pdf")
	del histos

def savehisto2D(histos,labels,outfile,canvas,XaxisTitle="",YaxisTitle="",plotTitle="",stats=1,logZ=0):
	for i in range(len(histos)):
		histos[i].SetTitle(plotTitle+" "+labels[i])
		histos[i].GetXaxis().SetTitle(XaxisTitle)
		histos[i].GetYaxis().SetTitle(YaxisTitle)
		histos[i].SetStats(stats)
		histos[i].Draw("COLZ")
		canvas.SetLogz(logZ)
		canvas.Print(outfile+".pdf")

gStyle.SetOptStat(110011)
c = TCanvas("c","c",800,600)

n = int((len(remainder) - 1) / 2)
n_histo = 0
n_histo_2D = 0
outfile = remainder[0]
infiles = []
labels = []
for i in range(n):
	infiles.append(TFile(remainder[i+1]))
	labels.append(remainder[i+n+1])

histofiles = []
histofiles_2D = []
for i in range(len(infiles)):
	infiles[i].cd()
	histos = []
	histos_2D = []
	for h in infiles[i].GetListOfKeys():
		h = h.ReadObj()
		if(h.ClassName() == "TH1F" or h.ClassName() == "TH1D" or h.ClassName() == "TH1"):
			histos.append(h)
		if(h.ClassName() == "TH2F" or h.ClassName() == "TH2D" or h.ClassName() == "TH2"):
			histos_2D.append(h)
	n_histo = len(histos)
	n_histo_2D = len(histos_2D)
	histofiles.append(histos)
	histofiles_2D.append(histos_2D)
	del histos
	del histos_2D

openPDF(outfile,c)

for i in range(n_histo):
	histos = []
	for j in range(n):
		histos.append(histofiles[j][i])
	savehisto(histos, labels, outfile,c, histos[0].GetXaxis().GetTitle(), "", histos[0].GetTitle(), logY=logy)
	del histos

for i in range(n_histo_2D):
	histos = []
	for j in range(n):
		histos.append(histofiles_2D[j][i])
	savehisto2D(histos, labels, outfile, c, histos[0].GetXaxis().GetTitle(), histos[0].GetYaxis().GetTitle(), histos[0].GetTitle(), logZ=logz)

closePDF(outfile,c)
