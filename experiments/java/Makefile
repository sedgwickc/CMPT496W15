JCC=javac
JFLAGS=-g
EXEC=AllocMem.class

all: $(EXEC)

alloc_mem:
	java -Xms2048m -Xmx2048m AllocMem 

AllocMem.class: AllocMem.java
	$(JCC) $(JFLAGS) $^

clean:
	rm *.class *.log

exp:
	./java_experiments.sh &> results.txt
