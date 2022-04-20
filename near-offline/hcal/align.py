import uproot
import ROOT
import numpy as np
import sys
from array import array

"""
dirty script for aligning hcals doing some root stuff I dont love
prepare to use:
    1) ldmx-sw/Hcal event-alignment branch
    2) ldmx-sw hcal-event-alignment branch
preparation of files:
    1) ntuplize 2 files to compare (decode.py)
run this script:
    1) ldmx python3 align.py [file1.root] [file2.root]
output:
    1) same format as file1/2.root
    2) merged together with new branch "aligned_event" where aligned events have same "aligned_event" number (unaligned are given -1)
"""
tree = ROOT.TTree("hgcroc","hgcroc")
branches = []

branchnames = ['raw_id','adc','tot','toa','raw_adc','i_sample','ldmxsw_event','pf_event','pf_spill','pf_ticks','tot_prog','tot_comp','section','layer','strip','end']

for i in range(len(branchnames)):
    temparray = array('i',[0])
    branches.append(array('i',[0]))
    tree.Branch(branchnames[i],branches[i],branchnames[i]+'/I')
aligned_array = array('i',[0])
tree.Branch('aligned_event',aligned_array,'aligned_event/I')

events0 = uproot.open(sys.argv[1]+":ntuplizehgcroc/hgcroc")
events1 = uproot.open(sys.argv[2]+":ntuplizehgcroc/hgcroc")

data0 = events0.arrays(library="np")
data1 = events1.arrays(library="np")

spill0 = np.unique(data0['pf_spill'])
spill1 = np.unique(data1['pf_spill'])

if(spill0.size == spill1.size):
    print('Number of spills are the same')

align_counter = 0

last_event0 = 0
last_event1 = 0

for k in range(spill0.size):
    tempdata0_ = events0.arrays(branchnames,"pf_spill=="+str(spill0[k]),library="np")
    tempdata1_ = events1.arrays(branchnames,"pf_spill=="+str(spill1[k]),library="np")

    num0 = np.unique(tempdata0_['pf_ticks'])
    num1 = np.unique(tempdata1_['pf_ticks'])

    loop_range = min([len(num0),len(num1)])

    counter0 = 0
    counter1 = 0
    for i in range(loop_range):
        ##Not trivial to deal with duplicate events without only looking at header info
        """
        if(tempdata0['pf_ticks'][counter0] == last_event0):
            print('skipping')
            for j in range(len(branchnames)):
                branches[j][0] = tempdata0[branchnames[j]][counter0]
            counter0 = counter0 + 1
            aligned_array[0] = -1
            tree.Fill()
            last_event0 = tempdata0['pf_ticks'][counter0]
            continue
        if(tempdata1['pf_ticks'][counter1] == last_event1):
            print('skipping')
            for j in range(len(branchnames)):
                branches[j][0] = tempdata1[branchnames[j]][counter1]
            counter1 = counter1 + 1
            aligned_array[0] = -1
            tree.Fill()
            last_event1 = tempdata1['pf_ticks'][counter1]
            continue
        """
        tempdata0 = events0.arrays(branchnames,"(pf_spill=="+str(spill0[k])+") & (pf_ticks=="+str(num0[counter0])+")",library="np")
        tempdata1 = events1.arrays(branchnames,"(pf_spill=="+str(spill1[k])+") & (pf_ticks=="+str(num1[counter1])+")",library="np")
        if(abs(num0[counter0]-num1[counter1]) < 10):
            for h in range(tempdata0['raw_adc'].size):
                for j in range(len(branchnames)):
                    branches[j][0] = tempdata0[branchnames[j]][h]
                aligned_array[0] = align_counter
                tree.Fill()
            for h in range(tempdata1['raw_adc'].size):
                for j in range(len(branchnames)):
                    branches[j][0] = tempdata1[branchnames[j]][h]
                aligned_array[0] = align_counter
                tree.Fill()
            counter0 = counter0 + 1
            counter1 = counter1 + 1
            align_counter = align_counter + 1
        elif(num0[counter0]>num1[counter1]):
            for h in range(tempdata1['raw_adc'].size):
                for j in range(len(branchnames)):
                    branches[j][0] = tempdata1[branchnames[j]][h]
                aligned_array[0] = -1
                tree.Fill()
            counter1 = counter1 + 1 
        elif(num0[counter0]<num1[counter1]):
            for h in range(tempdata0['raw_adc'].size):
                for j in range(len(branchnames)):
                    branches[j][0] = tempdata0[branchnames[j]][h]
                aligned_array[0] = -1
                tree.Fill()
            counter0 = counter0 + 1
        else:
            print('Weirdness...')

outfile = ROOT.TFile("merged.root","RECREATE")
directory = outfile.mkdir("ntuplizehgcroc")
directory.cd()
tree.Write();
outfile.Close()
