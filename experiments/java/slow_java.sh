#! /bin/bash
# Charles Sedgwick
# CMPT496W15
# These experiments are meant to help gather data on how the hueristics used by 
# the OOM Killer affect real world processes competing for memory

MB1=3
MB2=200
TIME1=500
TIME2=1000
SLOW=1000
MAXM="2048m"
COUNT=0

echo "One JVM set to allocate $MB1 MB per $SLOW milliseconds"
while :
do
java -Xms$MAXM -Xmx$MAXM AllocMem $MB1 $SLOW
COUNT=$((COUNT + 1)) 
echo "count: $COUNT"
done
wait $!
echo
