TARGET=bakalarka
SOURCES := main.cpp automata.cpp vtf_input.cpp
BUILDDIR=build

CXX=g++

OBJECTS=$(addprefix $(BUILDDIR)/,$(SOURCES:.cpp=.o))
CXXFLAGS = -O2 -g

all: dir $(BUILDDIR)/$(TARGET)

dir: 
	mkdir -p $(BUILDDIR)

run_det: 
	cd build | ./bakalarka -t min_det 

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OBJECTS) $(BUILDDIR)/$(TARGET)

.PHONY: all clean

