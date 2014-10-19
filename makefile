CC=clang++
CFLAGS=-c -O3 -w -std=c++11 
LDFLAGS=
SOURCES = SingleCharacterSeparater.cpp TextClassifier.cpp BayesianTextClassifier.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE=main

all:clean0 $(SOURCES) $(EXECUTABLE) clean1
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean0:
	rm -rf *.o main
clean1:
	rm -rf *.o