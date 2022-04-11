euRun -n DarkRunControl -a tcp://$1 &
#Increase this if you experience connection issues
sleep 10

Port_to_use=$1

euCliProducer -n CaptanTrigScintTestBeamProducer -t captan -r tcp://localhost:$1 &
sleep 1

#euCliProducer -n TrigScintFileReaderProducer  -t ts_file -r tcp://localhost:$1 &

euCliCollector -n TestBeamDataCollector -t ts_dc -r tcp://localhost:$1 &
sleep 1

euCliMonitor -n TrigScintTestBeamMonitor -t ts_mon -r tcp://localhost:$1 &
sleep 1
