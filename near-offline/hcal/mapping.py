from numpy import *
import sys
import ROOT as r
r.gROOT.SetBatch(1); 
r.gSystem.Load("libFramework.so")




# converts the link-channel provided by the HGCROC into a 'real' channel: goes from 1 to 384, representing a SiPM each
def FpgaLinkChannel_to_realChannel(FpgaLinkChannel): #link is the chip halves, channel is just the channel
    channel = FpgaLinkChannel[2]-1
    if 0 <= channel and channel <= 7:  realChannel = channel
    elif 9 <= channel and channel <= 16:  realChannel = channel-1
    elif 19 <= channel and channel <= 26:  realChannel = channel-3
    elif 28 <= channel and channel <= 35:  realChannel = channel-4
    else: return None
    realChannel+=FpgaLinkChannel[1]*32
    realChannel+=(FpgaLinkChannel[0]-2)*32*6
    return realChannel

#converts the 'real' channel into a 3 vector that describes the SiPM really well
def realChannel_to_SipM(c):#[layer,bar,side]  
    if c == None: return None  
    for i in range(1,2):
        if 0 <= c and c <= 3: return [i,c,0]
        if 4 <= c and c <= 7: return [i,c-4,1]
        if 8 <= c and c <= 11: return [i,c-4,0]
        if 12 <= c and c <= 15: return [i,c-8,1]
        c-=16
    # for i in range(2,3): #this one CMB row is quite flipped
    #     if 0 <= c and c <= 3: return [i,4-(c),0]
    #     if 4 <= c and c <= 7: return [i,4-(c-4),1]
    #     if 8 <= c and c <= 11: return [i,4-(c-4),0]
    #     if 12 <= c and c <= 15: return [i,4-(c-8),1]
    #     c-=16                
    for i in range(2,3):
        if 0 <= c and c <= 3: return [i,c,0]
        if 4 <= c and c <= 7: return [i,c-4,1]
        if 8 <= c and c <= 11: return [i,c-4,0]
        if 12 <= c and c <= 15: return [i,c-8,1]
        c-=16
    for i in range(3,10):
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

realChannel_to_SipM_fast={}
for c in range(0,385):
    realChannel_to_SipM_fast[c] = realChannel_to_SipM(c)
# print(realChannel_to_SipM_fast)                

