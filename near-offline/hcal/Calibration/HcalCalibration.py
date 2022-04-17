import sys
import csv

#Usage: ldmx python3 hcal_calibration.py <ntuplized root file>.root
#Outputs csv file DumbReconConditions.csv

outputFileName = 'DumbReconConditions.csv' #hardcoded name for now

ledfile = sys.argv[1]
mipfile = sys.argv[2]
chargefile = sys.argv[3]

#elloc = {}
did = {}
adcped = {}
el_loc = []

ledmean = {}
with open(ledfile, 'r', newline = "") as datafile:
    reader = csv.DictReader(datafile)
    for row in reader:
        #did = row["DetID"]
        #elloc[did] = row["ElLoc"]
        elloc = row["ElLoc"]
        el_loc.append(elloc)
        did[elloc] = row["DetID"]
        adcped[elloc] = str(row["ADC_PEDESTAL"])
        ledmean[elloc] = str(row["LED_MEDIAN"])

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

header1 = ["# ElLoc Format: (polarfire:hgcrocindex:channel)"]
header2 = ["DetID", "ElLoc", "ADC_PEDESTAL", "ADC_GAIN", "TOT_PEDESTAL", "TOT_GAIN", "MIPMPV_ADC", "MIPWIDTH_ADC", "MIPSLOPE_ADC", "LEDMEAN_ADC"]
csvfile = open(outputFileName, 'w', newline='')
writer = csv.writer(csvfile, delimiter=',', quotechar='"')

writer.writerow(header1)
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
    led_mean = "-9999"
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
    else:
        print("MIP MPV not found for {0}".format(el))
    if el in mipwidth:
        mip_width = str(mipwidth[el])
    else:
        print("MIP WIDTH not found for {0}".format(el))
    if el in mipslope:
        mip_slope = str(mipslope[el])
    else:
        print("MIP SLOPE not found for {0}".format(el))
    if el in ledmean:
        led_mean = str(ledmean[el])
    else:
        print("LED MEAN not found for {0}".format(el))
    line = [det_ID, el, adc_ped, adc_gain, tot_ped, tot_gain, mip_mpv, mip_width, mip_slope, led_mean]
    writer.writerow(line)
