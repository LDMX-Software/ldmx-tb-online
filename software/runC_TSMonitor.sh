euRun -n DarkRunControl -a tcp://4000 &
#Increase this if you experience connection issues
sleep 5

euCliProducer -n CaptanTrigScintTestBeamProducer -t captan -r tcp://localhost:4000 &
sleep 1

euCliCollector -n TestBeamDataCollector -t ts_dc -r tcp://localhost:4000 &
sleep 1

euCliMonitor -n TrigScintTestBeamMonitor -t ts_mon -r tcp://localhost:4000 &
sleep 1
