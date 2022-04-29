#!/usr/bin/env sh

euRun -n DarkRunControl -a tcp://5001 &

sleep 10

euCliCollector -n TestBeamDataCollector -t test_beam_hcal1 -r tcp://localhost:5001 &

sleep 1

euCliProducer -n PolarfireProducer -t pf1 -r tcp://localhost:5001 &

sleep 5

euCliMonitor -n HCalTestBeamMonitor -t hcal_mon1 -r tcp://localhost:5001 &
