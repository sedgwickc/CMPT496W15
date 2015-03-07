JCC=javac
JFLAGS=-g
EXEC=AllocMem.class

all: $(EXEC)

alloc_mem:
	java -Xms2048m -Xmx2048m AllocMem 

AllocMem.class: AllocMem.java
	$(JCC) $(JFLAGS) $^

#OOMExp.class: OOMExp.java
#	$(JCC) $(JFLAGS) $^

clean:
	rm *.class
