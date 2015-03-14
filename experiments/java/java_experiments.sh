#! /bin/bash
# Charles Sedgwick
# CMPT496W15
# These experiments are meant to help gather data on how the hueristics used by 
# the OOM Killer affect real world processes competing for memory

MB1=100
MB2=200
TIME1=500
TIME2=1000
MAXM="2048m"

echo "-----> Experiment One"
echo " One JVM set to use all the system memory"
java -Xms$MAXM -Xmx$MAXM AllocMem $MB1 & 
wait $!
echo
echo "-----> Experiment Two"
echo "Two processes use a maximum of half the system memory"
echo
java -Xms1024m -Xmx1024m AllocMem $MB1 & 
wait $!
java -Xms1024m -Xmx1024m AllocMem $MB1 &
wait $!
echo
echo "-----> Experiment Three"
echo "One process allocates 100mb every .5sec and another allocates memory every"
echo " 1 second"
echo
java -Xms1024m -Xmx1024m AllocMem $MB1 $TIME1 &
wait $!
java -Xms1024m -Xmx1024m AllocMem $MB1 $TIME2 &
wait $!
echo
echo "-----> Experiment Four"
echo "One process allocates 100 MB of memory every second while another allocates"
echo " 200MB of memory every second"
echo
java -Xms1024m -Xmx1024m AllocMem $MB1 $TIME2 &
wait $!
java -Xms1024m -Xmx1024m AllocMem $MB2 $TIME1 &
wait $!
