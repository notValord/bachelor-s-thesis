TARGET=fsa_red

SOURCES := main.cpp vtf_input.cpp automata_base.cpp auto_language_check.cpp det_auto.cpp min_det_auto.cpp rezidual_auto.cpp simulation_auto.cpp auto_dictionary.cpp auto_stats.cpp sat.cpp qbf.cpp tseitsen.cpp

BUILDDIR=build

SRCDIR=src

CXX=g++

OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(addprefix $(SRCDIR)/,$(SOURCES)))

CXXFLAGS = -O2 -g -std=c++2a

all: dir $(BUILDDIR)/$(TARGET)

dir: 
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

.PHONY: all clean

