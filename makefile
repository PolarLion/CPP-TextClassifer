CC=clang++
CFLAGS=-c -O3 -Wall -std=c++11 
LDFLAGS=
SOURCES = SingleCharacterSeparater.cpp TextClassifier.cpp \
	LogitRegressionClassifier.cpp RandomForestClassifier.cpp BayesianTextClassifier.cpp \
	ap.cpp alglibinternal.cpp linalg.cpp statistics.cpp \
	alglibmisc.cpp specialfunctions.cpp solvers.cpp optimization.cpp \
	dataanalysis.cpp \
	main.cpp
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