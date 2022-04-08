#!/usr/bin/env sh

euRun -n DarkRunControl -a tcp://4001 &
sleep 20

euCliCollector -n TestBeamDataCollector -t test_beam_hcal -r tcp://localhost:4001 &

sleep 1

euCliCollector -n TestBeamDataCollector -t test_beam_event_display -r tcp://localhost:4001 &

sleep 1

euCliProducer -n HgcrocFileReaderProducer -t hgcroc_file_hcal -r tcp://localhost:4001 &

sleep 1

euCliProducer -n HgcrocFileReaderProducer -t hgcroc_file_event_display -r tcp://localhost:4001 &

sleep 1

euCliMonitor -n TestBeamEventDisplayMonitor -t event_display -r tcp://localhost:4001 & 

sleep 5

euCliMonitor -n HCalTestBeamMonitor -t hcal_mon -r tcp://localhost:4001 &
