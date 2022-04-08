#!/usr/bin/env sh

euRun -n DarkRunControl -a tcp://4000 &
sleep 5

#euLog &
#sleep 1

euCliCollector -n TestBeamDataCollector -t test_beam -r tcp://localhost:4000 &

euCliProducer -n RogueTcpClientProducer -t hcal -r tcp://localhost:4000 &

euCliProducer -n RogueTcpClientProducer -t trig -r tcp://localhost:4000 &

#euCliMonitor -n DarkMonitor -t mon -r tcp://localhost:4005 &

euCliMonitor -n SimpleMonitor -t mon -r tcp://localhost:4000 &
