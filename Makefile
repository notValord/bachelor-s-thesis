TARGET=reduction
TARGET2=sat_red
TARGET3=qbf_red

SOURCES := main.cpp vtf_input.cpp automata_base.cpp auto_language_check.cpp det_auto.cpp min_det_auto.cpp rezidual_auto.cpp simulation_auto.cpp auto_dictionary.cpp auto_stats.cpp
SOURCES2 := sat_main.cpp vtf_input.cpp automata_base.cpp det_auto.cpp auto_language_check.cpp auto_dictionary.cpp sat.cpp tseitsen.cpp auto_stats.cpp
SOURCES3 := qbf_main.cpp vtf_input.cpp automata_base.cpp det_auto.cpp auto_language_check.cpp auto_dictionary.cpp qbf.cpp tseitsen.cpp auto_stats.cpp

BUILDDIR=build
BUILDDIR2=build_sat
BUILDDIR3=build_qbf

SRCDIR=src

CXX=g++

OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(addprefix $(SRCDIR)/,$(SOURCES)))
OBJECTS2=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR2)/%.o,$(addprefix $(SRCDIR)/,$(SOURCES2)))
OBJECTS3=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR3)/%.o,$(addprefix $(SRCDIR)/,$(SOURCES3)))

CXXFLAGS = -O2 -g -std=c++2a

all: dir satdir qbfdir $(BUILDDIR)/$(TARGET) $(BUILDDIR2)/$(TARGET2) $(BUILDDIR3)/$(TARGET3)

min: dir $(BUILDDIR)/$(TARGET)

sat: satdir $(BUILDDIR2)/$(TARGET2)

qbf: qbfdir $(BUILDDIR3)/$(TARGET3)

dir: 
	mkdir -p $(BUILDDIR)

satdir:
	mkdir -p $(BUILDDIR2)

qbfdir:
	mkdir -p $(BUILDDIR3)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR2)/$(TARGET2): $(OBJECTS2)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR2)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR3)/$(TARGET3): $(OBJECTS3)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR3)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OBJECTS2) $(OBJECTS3) $(BUILDDIR)/$(TARGET) $(BUILDDIR2)/$(TARGET2) $(BUILDDIR3)/$(TARGET3)

.PHONY: all clean

