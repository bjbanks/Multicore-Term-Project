IDIR = ../include
SDIR = ../src
ODIR = ./obj
CXX = g++
LDFLAGS = -lgtest_main -lgtest -lpthread
CPPFLAGS = -Wall -g -pthread -std=c++11

_DEPS = scheduler.h worker.h deque.h task.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = scheduler.o worker.o deque.o task.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

TESTS = tests-runner.cpp scheduler-tests.cpp worker-tests.cpp deque-tests.cpp \
	task-tests.cpp

TASKS = increment-task.h fib-task.h

all: unit_tests

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	mkdir -p $(ODIR)
	$(CXX) $(CPPFLAGS) -c -o $@ $< -I$(IDIR)

unit_tests: $(OBJ) $(TESTS) $(TASKS)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) -I$(IDIR)
	echo "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./unit_tests" > vg_unit_tests
	chmod +x vg_unit_tests

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f unit_tests vg_unit_tests valgrind-out.txt
