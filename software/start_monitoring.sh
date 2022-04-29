euCliCollector -n TestBeamDataCollector -t test_beam_hcal1 -r tcp://192.168.1.11:4000 &
sleep 1

euCliCollector -n TestBeamDataCollector -t test_beam_hcal0 -r tcp://192.168.1.11:4000 &
sleep 1

euCliCollector -n TestBeamDataCollector -t ts_dc -r tcp://192.168.1.11:4000 &
sleep 1

euCliMonitor -n HCalTestBeamMonitor -t hcal_mon1 -r tcp://192.168.1.11:4000 &
sleep 1
euCliMonitor -n HCalTestBeamMonitor -t hcal_mon0 -r tcp://192.168.1.11:4000 &
sleep 1

euCliMonitor -n TrigScintTestBeamMonitor -t ts_mon -r tcp://192.168.1.11:4000 &
sleep 1
