#! /bin/bash
# Charles Sedgwick
# CMPT496W15
# These experiments are meant to help gather data on how the hueristics used by 
# the OOM Killer affect real world processes competing for memory

echo "Experiment One"
echo " One JVM set to use all the system memory"
java -Xms2048m -Xmx2048m AllocMem & 
wait $!

echo "Experiment Two"
echo "Two processes set use a maximum of half the system memory"

java -Xms1024m -Xmx1024m AllocMem & 
java -Xms1024m -Xmx1024m AllocMem &
# wait until last process run finishes
wait $!

echo "Experiment Three"
echo "One process allocates 50mb every .5sec and another allocates memory every"
echo " 1 second"

echo "Experiment Four"
echo "One process allocates 50 MB of memory every second while another allocates"
echo " 150MB of memory every second"
