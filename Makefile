CC=g++
CFLAGS=-c -Wall -std=c++11 -O2
LDFLAGS=-lPocoFoundation -lPocoNet  -lPocoNetSSL -lPocoUtil -lPocoXML -LPoco/Libraries
SOURCES=main.cpp TodoServerApp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=TodoServerApp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

