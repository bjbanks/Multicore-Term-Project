IDIR = ../include
SDIR = ../src
ODIR = ./obj
CXX = g++
LDFLAGS =  -lpthread #-lgtest_main -lgtest
CPPFLAGS = -Wall -g -pthread -std=c++11

_DEPS = scheduler.h worker.h deque.h task.h utils.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = scheduler.o worker.o deque.o task.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

APPS = parallelArray.cpp
BENCHMARK = benchmark.cpp

all: fibonacci

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	mkdir -p $(ODIR)
	$(CXX) $(CPPFLAGS) -c -o $@ $< -I$(IDIR)

fibonacci: $(OBJ) $(APPS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) -I$(IDIR)

benchmark: $(OBJ) $(APPS) $(BENCHMARK)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) -I$(IDIR)

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f fibonacci
	rm -f benchmark
