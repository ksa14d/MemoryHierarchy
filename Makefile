CC = g++
CFLAGS= -O0 -Wall -std=c++11 -pedantic
LIBS = -lpthread\

all: hierarchy 

clean:
	rm -f *.o
	rm -f hierarchy
	rm -f *~
	rm -f core

hierarchy.o: hierarchy.cpp
	$(CC) $(CFLAGS) -g -c hierarchy.cpp

hierarchy:  hierarchy.o
	$(CC) $(CFLAGS) -g -o hierarchy hierarchy.o      

demoCache: 
	./hierarchy < physical.dat
demoTLB: 
	./hierarchy < trace.dat
