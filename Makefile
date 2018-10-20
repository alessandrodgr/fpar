CXX = g++
OPT =	-O2
NOOPT =	-O0
CXXFLAGS =	--std=c++17 -Wall -Isrc -pedantic-errors -Wno-unused-variable -fcilkplus -lcilkrts
SRCDIR   = src/test
BINDIR   = bin
HEADER	 = src/Backus.hpp src/Object.hpp	\
					 src/Functions.hpp src/Functionals.hpp

all: matrix_mul num_of_evens toy_example

matrix_mul: $(SRCDIR)/matrix_mul.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(NOOPT) $^ -o $(BINDIR)/$@ &
	$(CXX) $(CXXFLAGS) $(OPT) $^ -o $(BINDIR)/$@_opt

num_of_evens: $(SRCDIR)/num_of_evens.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(NOOPT) $^ -o $(BINDIR)/$@ &
	$(CXX) $(CXXFLAGS) $(OPT) $^ -o $(BINDIR)/$@_opt

toy_example: $(SRCDIR)/toy_example.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(NOOPT) $^ -o $(BINDIR)/$@ &
	$(CXX) $(CXXFLAGS) $(OPT) $^ -o $(BINDIR)/$@_opt

.PHONY: clean
clean:
	rm -f $(BINDIR)/*
