import sys
import csv
import matplotlib
from matplotlib.backends.backend_pdf import PdfPages
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#Usage: ldmx python3 hcal_calibration.py <ntuplized root file>.root
#Outputs csv file DumbReconConditions.csv

outputFileName = 'DumbReconConditions.csv' #hardcoded name for now

ledfile = sys.argv[1]
mipfile = sys.argv[2]
chargefile = sys.argv[3]

pp = PdfPages("Run205_calib.pdf")

chan = []
for i in range(72):
    chan.append(i)

ped = {}
led = {}
slope_mip = {}
width = {}
peak = {}
gainadc = {}
totadc = {}
gaintot = {}

for i in range(6):
    ped[i] = []
    led[i] = []
    slope_mip[i] = []
    width[i] = []
    peak[i] = []
    gainadc[i] = []
    totadc[i] = []
    gaintot[i] = []
    for j in range(72):
        ped[i].append(-9999)
        led[i].append(-9999)
        slope_mip[i].append(-9999)
        width[i].append(-9999)
        peak[i].append(-9999)
        gainadc[i].append(-9999)
        totadc[i].append(-9999)
        gaintot[i].append(-9999)

#elloc = {}
did = {}
adcped = {}
el_loc = []
ledmedian = {}

with open(ledfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        #did = row["DetID"]
        #elloc[did] = row["ElLoc"]
        elloc = row["ElLoc"]
        el_loc.append(elloc)
        did[elloc] = row["DetID"]
        adcped[elloc] = str(row["ADC_PEDESTAL"])
        ledmedian[elloc] = str(row["LED_MEDIAN"])
        e = elloc.split(":", 2)
        roc = int(e[0])*3 + int(e[1])
        channel = int(e[2])
        ped[roc][channel] = float(adcped[elloc])
        led[roc][channel] = float(ledmedian[elloc])

figped, [[axped0, axped1] , [axped2, axped3], [axped4, axped5]] = plt.subplots(3, 2)
axped0.scatter(chan, ped[0])
axped0.set_title("Pedestals ROC {0}".format(0))
#axped0.set_xlabel("Channel")
axped0.set_ylabel("ADC")
axped0.set_ylim(0,300)
axped1.scatter(chan, ped[1])
axped1.set_title("Pedestals ROC {0}".format(1))
#axped1.set_xlabel("Channel")
#axped1.set_ylabel("ADC")
axped1.set_ylim(0,300)
axped2.scatter(chan, ped[2])
#axped2.set_title("Pedestals ROC {0}".format(2))
#axped2.set_xlabel("Channel")
axped2.set_ylabel("ADC")
axped2.set_ylim(0,300)
axped3.scatter(chan, ped[3])
#axped3.set_title("Pedestals ROC {0}".format(3))
#axped3.set_xlabel("Channel")
#axped3.set_ylabel("ADC")
axped3.set_ylim(0,300)
axped4.scatter(chan, ped[4])
#axped4.set_title("Pedestals ROC {0}".format(4))
axped4.set_xlabel("Channel")
axped4.set_ylabel("ADC")
axped4.set_ylim(0,300)
axped5.scatter(chan, ped[5])
#axped5.set_title("Pedestals ROC {0}".format(5))
axped5.set_xlabel("Channel")
#axped5.set_ylabel("ADC")
axped5.set_ylim(0,300)
pp.savefig(figped)
plt.close(figped)

figled, [[axled0, axled1] , [axled2, axled3], [axled4, axled5]] = plt.subplots(3, 2)
axled0.scatter(chan, led[0])
axled0.set_title("LED Median ROC {0}".format(0))
#axled0.set_xlabel("Channel")
axled0.set_ylabel("Sum ADC")
axled0.set_ylim(0,800)
axled1.scatter(chan, led[1])
axled1.set_title("LED Median ROC {0}".format(1))
#axled1.set_xlabel("Channel")
#axled1.set_ylabel("Sum ADC")
axled1.set_ylim(0,800)
axled2.scatter(chan, led[2])
#axled2.set_title("LED Median ROC {0}".format(2))
#axled2.set_xlabel("Channel")
axled2.set_ylabel("Sum ADC")
axled2.set_ylim(0,800)
axled3.scatter(chan, led[3])
#axled3.set_title("LED Median ROC {0}".format(3))
#axled3.set_xlabel("Channel")
#axled3.set_ylabel("Sum ADC")
axled3.set_ylim(0,800)
axled4.scatter(chan, led[4])
#axled4.set_title("LED Median ROC {0}".format(4))
axled4.set_xlabel("Channel")
axled4.set_ylabel("Sum ADC")
axled4.set_ylim(0,800)
axled5.scatter(chan, led[5])
#axled5.set_title("LED Median ROC {0}".format(5))
axled5.set_xlabel("Channel")
#axled5.set_ylabel("Sum ADC")
axled5.set_ylim(0,800)
pp.savefig(figled)
plt.close(figled)

pedestal = {}
mipwidth = {}
mipmpv = {}
mipslope = {}
with open(mipfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        #did = row["DetID"]
        elloc = row["ElLoc"]
        #elloc[did] = row["ElLoc"]
        #pedestal = str(row["ADC_PEDESTAL"])
        mipmpv[elloc] = str(row["MIPMPV_ADC"])
        mipwidth[elloc] = str(row["MIPWIDTH_ADC"])
        mipslope[elloc] = str(row["MIP_SLOPE"])
        e = elloc.split(":", 2)
        roc = int(e[0])*3 + int(e[1])
        channel = int(e[2])
        slope_mip[roc][channel] = float(mipslope[elloc])
        width[roc][channel] = float(mipwidth[elloc])
        peak[roc][channel] = float(mipmpv[elloc])

figmip, [[axmip0, axmip1] , [axmip2, axmip3], [axmip4, axmip5]] = plt.subplots(3, 2)
axmip0.scatter(chan, slope_mip[0])
axmip0.set_title("MIP Slope ROC {0}".format(0))
#axmip0.set_xlabel("Channel")
axmip0.set_ylabel("Slope [MeV/Sum ADC]")
axmip0.set_ylim(0,0.02)
axmip1.scatter(chan, slope_mip[1])
axmip1.set_title("MIP Slope ROC {0}".format(1))
#axmip1.set_xlabel("Channel")
#axmip1.set_ylabel("Slope [MeV/Sum ADC]")
axmip1.set_ylim(0,0.02)
axmip2.scatter(chan, slope_mip[2])
#axmip2.set_title("MIP Slope ROC {0}".format(2))
#axmip2.set_xlabel("Channel")
axmip2.set_ylabel("Slope [MeV/Sum ADC]")
axmip2.set_ylim(0,0.02)
axmip3.scatter(chan, slope_mip[3])
#axmip3.set_title("MIP Slope ROC {0}".format(3))
#axmip3.set_xlabel("Channel")
#axmip3.set_ylabel("Slope [MeV/Sum ADC]")
axmip3.set_ylim(0,0.02)
axmip4.scatter(chan, slope_mip[4])
#axmip4.set_title("MIP Slope ROC {0}".format(4))
axmip4.set_xlabel("Channel")
axmip4.set_ylabel("Slope [MeV/Sum ADC]")
axmip4.set_ylim(0,0.02)
axmip5.scatter(chan, slope_mip[5])
#axmip5.set_title("MIP Slope ROC {0}".format(5))
axmip5.set_xlabel("Channel")
#axmip5.set_ylabel("Slope [MeV/Sum ADC]")
axmip5.set_ylim(0,0.02)
pp.savefig(figmip)
plt.close(figmip)

figmpv, [[axmpv0, axmpv1] , [axmpv2, axmpv3], [axmpv4, axmpv5]] = plt.subplots(3, 2)
axmpv0.scatter(chan, peak[0])
axmpv0.set_title("MIP MPV ROC {0}".format(0))
#axmpv0.set_xlabel("Channel")
axmpv0.set_ylabel("Sum ADC")
axmpv0.set_ylim(0,800)
axmpv1.scatter(chan, peak[1])
axmpv1.set_title("MIP MPV ROC {0}".format(1))
#axmpv1.set_xlabel("Channel")
#axmpv1.set_ylabel("Sum ADC")
axmpv1.set_ylim(0,800)
axmpv2.scatter(chan, peak[2])
#axmpv2.set_title("MIP MPV ROC {0}".format(2))
#axmpv2.set_xlabel("Channel")
axmpv2.set_ylabel("Sum ADC")
axmpv2.set_ylim(0,800)
axmpv3.scatter(chan, peak[3])
#axmpv3.set_title("MIP MPV ROC {0}".format(3))
#axmpv3.set_xlabel("Channel")
#axmpv3.set_ylabel("Sum ADC")
axmpv3.set_ylim(0,800)
axmpv4.scatter(chan, peak[4])
#axmpv4.set_title("MIP MPV ROC {0}".format(4))
axmpv4.set_xlabel("Channel")
axmpv4.set_ylabel("Sum ADC")
axmpv4.set_ylim(0,800)
axmpv5.scatter(chan, peak[5])
#axmpv5.set_title("MIP MPV ROC {0}".format(5))
axmpv5.set_xlabel("Channel")
#axmpv5.set_ylabel("Sum ADC")
axmpv5.set_ylim(0,800)
pp.savefig(figmpv)
plt.close(figmpv)

figwid, [[axwid0, axwid1] , [axwid2, axwid3], [axwid4, axwid5]] = plt.subplots(3, 2)
axwid0.scatter(chan, width[0])
axwid0.set_title("MIP FWHM ROC {0}".format(0))
#axwid0.set_xlabel("Channel")
axwid0.set_ylabel("Sum ADC")
axwid0.set_ylim(0,400)
axwid1.scatter(chan, width[1])
axwid1.set_title("MIP FWHM ROC {0}".format(1))
#axwid1.set_xlabel("Channel")
#axwid1.set_ylabel("Sum ADC")
axwid1.set_ylim(0,400)
axwid2.scatter(chan, width[2])
#axwid2.set_title("MIP FWHM ROC {0}".format(2))
#axwid2.set_xlabel("Channel")
axwid2.set_ylabel("Sum ADC")
axwid2.set_ylim(0,400)
axwid3.scatter(chan, width[3])
#axwid3.set_title("MIP FWHM ROC {0}".format(3))
#axwid3.set_xlabel("Channel")
#axwid3.set_ylabel("Sum ADC")
axwid3.set_ylim(0,400)
axwid4.scatter(chan, width[4])
#axwid4.set_title("MIP FWHM ROC {0}".format(4))
axwid4.set_xlabel("Channel")
axwid4.set_ylabel("Sum ADC")
axwid4.set_ylim(0,400)
axwid5.scatter(chan, width[5])
#axwid5.set_title("MIP FWHM ROC {0}".format(5))
axwid5.set_xlabel("Channel")
#axwid5.set_ylabel("ADC")
axwid5.set_ylim(0,400)
pp.savefig(figwid)
plt.close(figwid)

adcgain = {}
totped = {}
totgain = {}
with open(chargefile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        #did = row["DetID"]
        elloc = row["ELLOC"]
        #elloc[did] = row["ElLoc"]
        adcgain[elloc] = str(row["SLOPE_ADC"])
        totped[elloc] = "0."
        totgain[elloc] = str(row["SLOPE_TOT"])
        e = elloc.split(":", 2)
        roc = int(e[0])*3 + int(e[1])
        channel = int(e[2])
        gainadc[roc][channel] = float(adcgain[elloc])
        totadc[roc][channel] = float(totped[elloc])
        gaintot[roc][channel] = float(totgain[elloc])

figgain, [[axgain0, axgain1] , [axgain2, axgain3], [axgain4, axgain5]] = plt.subplots(3, 2)
axgain0.scatter(chan, gainadc[0])
axgain0.set_title("ADC Gain ROC {0}".format(0))
#axgain0.set_xlabel("Channel")
axgain0.set_ylabel("Gain [mV/ADC]")
axgain1.scatter(chan, gainadc[1])
axgain1.set_title("ADC Gain ROC {0}".format(1))
#axgain1.set_xlabel("Channel")
axgain1.set_ylabel("Gain [mV/ADC]")
axgain2.scatter(chan, gainadc[2])
#axgain2.set_title("ADC Gain ROC {0}".format(2))
#axgain2.set_xlabel("Channel")
axgain2.set_ylabel("Gain [mV/ADC]")
axgain3.scatter(chan, gainadc[3])
#axgain3.set_title("ADC Gain ROC {0}".format(3))
#axgain3.set_xlabel("Channel")
axgain3.set_ylabel("Gain [mV/ADC]")
axgain4.scatter(chan, gainadc[4])
#axgain4.set_title("ADC Gain ROC {0}".format(4))
axgain4.set_xlabel("Channel")
axgain4.set_ylabel("Gain [mV/ADC]")
axgain5.scatter(chan, gainadc[5])
#axgain5.set_title("ADC Gain ROC {0}".format(5))
axgain5.set_xlabel("Channel")
axgain0.set_ylabel("Gain [mV/ADC]")
pp.savefig(figgain)
plt.close(figgain)

figtot, [[axtot0, axtot1] , [axtot2, axtot3], [axtot4, axtot5]] = plt.subplots(3, 2)
axtot0.scatter(chan, totadc[0])
axtot0.set_title("TOT Threshold ROC {0}".format(0))
#axtot0.set_xlabel("Channel")
axtot0.set_ylabel("ADC")
axtot1.scatter(chan, totadc[1])
axtot1.set_title("TOT Threshold ROC {0}".format(1))
#axtot1.set_xlabel("Channel")
axtot1.set_ylabel("ADC")
axtot2.scatter(chan, totadc[2])
#axtot2.set_title("TOT Threshold ROC {0}".format(2))
#axtot2.set_xlabel("Channel")
axtot2.set_ylabel("ADC")
axtot3.scatter(chan, totadc[3])
#axtot3.set_title("TOT Threshold ROC {0}".format(3))
#axtot3.set_xlabel("Channel")
axtot3.set_ylabel("ADC")
axtot4.scatter(chan, totadc[4])
#axtot4.set_title("TOT Threshold ROC {0}".format(4))
axtot4.set_xlabel("Channel")
axtot4.set_ylabel("ADC")
axtot5.scatter(chan, totadc[5])
#axtot5.set_title("TOT Threshold ROC {0}".format(5))
axtot5.set_xlabel("Channel")
axtot5.set_ylabel("ADC")
pp.savefig(figtot)
plt.close(figtot)

figtgain, [[axtgain0, axtgain1] , [axtgain2, axtgain3], [axtgain4, axtgain5]] = plt.subplots(3, 2)
axtgain0.scatter(chan, gaintot[0])
axtgain0.set_title("TOT Gain ROC {0}".format(0))
#axtgain0.set_xlabel("Channel")
axtgain0.set_ylabel("Gain [mV/ADC]")
axtgain1.scatter(chan, gaintot[1])
axtgain1.set_title("TOT Gain ROC {0}".format(1))
#axtgain1.set_xlabel("Channel")
axtgain1.set_ylabel("Gain [mV/ADC]")
axtgain2.scatter(chan, gaintot[2])
#axtgain2.set_title("TOT Gain ROC {0}".format(2))
#axtgain2.set_xlabel("Channel")
axtgain2.set_ylabel("Gain [mV/ADC]")
axtgain3.scatter(chan, gaintot[3])
#axtgain3.set_title("TOT Gain ROC {0}".format(3))
#axtgain3.set_xlabel("Channel")
axtgain3.set_ylabel("Gain [mV/ADC]")
axtgain4.scatter(chan, gaintot[4])
#axtgain4.set_title("TOT Gain ROC {0}".format(4))
axtgain4.set_xlabel("Channel")
axtgain4.set_ylabel("Gain [mV/ADC]")
axtgain5.scatter(chan, gaintot[5])
#axtgain5.set_title("TOT Gain ROC {0}".format(5))
axtgain5.set_xlabel("Channel")
axtgain5.set_ylabel("Gain [mV/ADC]")
pp.savefig(figtgain)
plt.close(figtgain)

pp.close()

header1 = ["# ElLoc Format: (polarfire:hgcrocindex:channel)"]
header2 = ["DetID", "ElLoc", "ADC_PEDESTAL", "ADC_GAIN", "TOT_PEDESTAL", "TOT_GAIN", "MIPMPV_ADC", "MIPWIDTH_ADC", "MIPSLOPE_ADC", "LED_MEDIAN"]
csvfile = open(outputFileName, 'w', newline='')
writer = csv.writer(csvfile, delimiter=',', quotechar='"')

#writer.writerow(header1)
writer.writerow(header2)
for el in el_loc:
    det_ID = "-9999"
    adc_ped = "-9999"
    adc_gain = "-9999"
    tot_ped = "-9999"
    tot_gain = "9999"
    mip_mpv = "-9999"
    mip_width = "-9999"
    mip_slope = "-9999"
    led_median = "-9999"
    if el in did:
        det_ID = str(did[el])
    else:
        print("detID not found for {0}".format(el))
    if el in adcped:
        adc_ped = str(adcped[el])
    else:
        print("ADC PED not found for {0}".format(el))
    if el in adcgain:
        adc_gain = str(adcgain[el])
    else:
        print("ADC GAIN not found for {0}".format(el))
    if el in totped:
        tot_ped = str(totped[el])
    else:
        print("TOT PED not found for {0}".format(el))
    if el in totgain:
        tot_gain = str(totgain[el])
    else:
        print("TOT GAIN not found for {0}".format(el))
    if el in mipmpv:
        mip_mpv = str(mipmpv[el])
    #else:
        #print("MIP MPV not found for {0}".format(el))
    if el in mipwidth:
        mip_width = str(mipwidth[el])
    #else:
    #    print("MIP WIDTH not found for {0}".format(el))
    if el in mipslope:
        mip_slope = str(mipslope[el])
    #else:
    #    print("MIP SLOPE not found for {0}".format(el))
    if el in ledmedian:
        led_median = str(ledmedian[el])
    else:
        print("LED MEDIAN not found for {0}".format(el))
    line = [det_ID, el, adc_ped, adc_gain, tot_ped, tot_gain, mip_mpv, mip_width, mip_slope, led_median]
    writer.writerow(line)
