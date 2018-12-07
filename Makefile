CXX = g++
OPT =	-O2
NOOPT =	-O0
CXXFLAGS =	--std=c++17 -Wall -Isrc -pedantic-errors -Wno-unused-variable -fopenmp
SRCDIR   = src
BINDIR   = test
TESTDIR   = test
HEADER	 = $(SRCDIR)/Backus.hpp \
					 $(SRCDIR)/Object.hpp	\
					 $(SRCDIR)/Functions.hpp \
					 $(SRCDIR)/Functionals.hpp
TARGETS	 = matrix_mul \
					 toy_example	\
					 sort_all

all: $(TARGETS)

%: $(TESTDIR)/%.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(NOOPT) $^ -o $(BINDIR)/$@
	$(CXX) $(CXXFLAGS) $(OPT) $^ -o $(BINDIR)/$@_opt

.PHONY: clean
clean:
	rm -f $(BINDIR)/*
