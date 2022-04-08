#!/usr/bin/env sh

euRun -n DarkRunControl -a tcp://4001 &

sleep 10

euCliCollector -n TestBeamDataCollector -t test_beam_hcal -r tcp://localhost:4001 &

sleep 1

euCliCollector -n TestBeamDataCollector -t test_beam_event_display -r tcp://localhost:4001 &

sleep 1

euCliProducer -n PolarfireProducer -t pf0 -r tcp://localhost:4001 &

sleep 1

#euCliProducer -n PolarfireProducer -t pf1 &

sleep 1

euCliMonitor -n TestBeamEventDisplayMonitor -t event_display -r tcp://localhost:4001 &

sleep 5

euCliMonitor -n HCalTestBeamMonitor -t hcal_mon -r tcp://localhost:4001 &
