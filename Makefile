CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall $(shell root-config --cflags) -Iinclude
LDFLAGS  := $(shell root-config --libs)

SRCDIR   := src
OBJDIR   := obj
INCDIR   := include

SOURCES  := $(wildcard $(SRCDIR)/*.cc)
OBJECTS  := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES))

TARGET   := run_analysis

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(OBJDIR)/run_analysis.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/run_analysis.o: run_analysis.cc | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cc | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)
