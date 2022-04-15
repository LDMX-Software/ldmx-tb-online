from numpy import *
import sys
import ROOT as r
r.gROOT.SetBatch(1); 
r.gSystem.Load("libFramework.so")




# converts the link-channel provided by the HGCROC into a 'real' channel: goes from 1 to 384, representing a SiPM each
def FpgaLinkChannel_to_realChannel(FpgaLinkChannel): #link is the chip halves, channel is just the channel
    channel = FpgaLinkChannel[2]
    if 0 <= channel and channel <= 7:  realChannel = channel
    elif 9 <= channel and channel <= 16:  realChannel = channel-1
    elif 18 <= channel and channel <= 25:  realChannel = channel-2
    elif 27 <= channel and channel <= 34:  realChannel = channel-3
    else: return None
    realChannel+=FpgaLinkChannel[1]*32
    realChannel+=(FpgaLinkChannel[0])*32*6
    return realChannel

#converts the 'real' channel into a 3 vector that describes the SiPM really well
def realChannel_to_SipM(c):#[layer,bar,side]  
    side=int(c/4)%2
    if c in range(0,9*16): layer=int(c/16)
    elif c in range(9*16,9*16+10*24): layer=int( (c-9*16)/24 )+9
    else: print (c)

    if layer in range(0,9): index =  c-layer*16-side*4
    if layer in range(9,19): index =  (c-9*16)-(layer-9)*24-side*4

    quadbar = int(index/8)
    quadbar_bar=index-quadbar*8

    if layer in (0,1,2,4,6,8,10,12,14,16,18): #if it's in a vertical layer or the flipped horizontal one
        bar = 3-quadbar_bar+quadbar*4
    if layer in (3,5,7,9,11,13,15,17): 
        bar = quadbar_bar+quadbar*4


    return [layer,bar,side]



realChannel_to_SipM_fast={}
for c in range(0,384):
    realChannel_to_SipM_fast[c] = realChannel_to_SipM(c)
# print(realChannel_to_SipM_fast)                
# for c in range(0,384):
#             print(c,realChannel_to_SipM(c))
