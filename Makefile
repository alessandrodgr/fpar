UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # MacOS (su mac omp funziona solo con g++-v)
CXX = $(shell ls /usr/local/bin | grep ^g++- | head -n 1) # latest g++-version
else
CXX	=	g++
endif

OPT =	-O3
CXXFLAGS	=	--std=c++17 $(OPT) -Wall -pedantic-errors -Wno-unused-variable -fopenmp -pthread
TARGET   = main
SRCDIR   = src
BINDIR   = bin

$(BINDIR)/$(TARGET):	$(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean memcheck
clean:
	rm -f $(OBJ) $(BINDIR)/$(TARGET)

memcheck:
	valgrind --leak-check=full $(BINDIR)/$(TARGET)
