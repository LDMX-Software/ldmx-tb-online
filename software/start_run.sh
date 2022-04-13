euRun -n DarkRunControl -a tcp://4000 &
sleep 5

euCliProducer -n CaptanTrigScintTestBeamProducer -t captan -r tcp://192.168.1.11:4000 &
sleep 1

euCliProducer -n PolarfireProducer -t pf1 -r tcp://192.168.1.11:4000 &
sleep 1

euCliProducer -n PolarfireProducer -t pf0 -r tcp://192.168.1.11:4000 &
sleep 1

euCliProducer -n DipClientProducer -t dip -r tcp://192.168.1.11:4000 &
sleep 1

