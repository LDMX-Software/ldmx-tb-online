#!/usr/bin/env python3
#-----------------------------------------------------------------------------
# Title      : PRBS Test Data Source
#-----------------------------------------------------------------------------
# This file is part of the LDMX DAQ software. It is subject to
# the license terms in the LICENSE.txt file found in the top-level directory
# of this distribution and at:
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
# No part of the LDMX DAQ software, including this file, may be
# copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------
import pyrogue
import rogue
import pyrogue.utilities.prbs
import pyrogue.protocols
import time

class PrbsSourceRoot(pyrogue.Root):

    def __init__(self):

        pyrogue.Root.__init__(self,
                              name='PrbsSourceRoot',
                              description="PRBS Source Root",
                              pollEn=True,
                              serverPort=8900)

        prbsTx = pyrogue.utilities.prbs.PrbsTx(expand=True)

        udp = pyrogue.protocols.UdpRssiPack(jumbo=True, packVer=2, server=True, port=8800)

        prbsTx >> udp.application(0)

        self.add(prbsTx)
        self.add(udp)


if __name__ == "__main__":

    with PrbsSourceRoot() as root:

        import pyrogue.pydm
        pyrogue.pydm.runPyDM(root=root,title='PrbsTx',sizeX=1000,sizeY=1000)

