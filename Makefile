CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=CS100HW-03.cpp UCShell.cpp UCShellPipes.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=UCShell

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)
