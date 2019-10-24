#
# Example Makefile for ROOTANA-based projects
#

CXXFLAGS = -g -O2 -Wall -Wuninitialized

# required ZLIB library

CXXFLAGS += -DHAVE_LIBZ -std=c++11

# required ROOTANA library

ifndef ROOTANASYS
ROOTANASYS=..
endif

CXXFLAGS += -I$(ROOTANASYS)/include
LIBS += -L$(ROOTANASYS)/lib -lrootana

# ROOT libraries

ifdef ROOTSYS

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTFEATURES := $(shell root-config --features)
ROOTGLIBS    := $(shell root-config --glibs) -lThread
HAVE_ROOT_HTTP := $(findstring http,$(ROOTFEATURES))
HAVE_ROOT_XML  := $(findstring xml,$(ROOTFEATURES))

CXXFLAGS  += -DHAVE_ROOT $(ROOTCFLAGS)

ifdef HAVE_ROOT_XML
CXXFLAGS  += -DHAVE_ROOT_XML
ROOTGLIBS += -lXMLParser -lThread -lRHTTP
endif

ifdef HAVE_ROOT_HTTP
CXXFLAGS  += -DHAVE_ROOT_HTTP -DHAVE_THTTP_SERVER
ROOTGLIBS += -lRHTTP
endif

endif # ROOTSYS

# optional MIDAS library

ifdef MIDASSYS

MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
MIDASLIBS = $(MIDASSYS)/darwin/lib/libmidas.a
endif

LIBS += $(MIDASLIBS)

endif # MIDASSYS



CAENDIGLIB = $(HOME)/Andrew/CAENStuff/CAENDigitizer_2.7.9/include
CAENCOMMLIB = $(HOME)/Andrew/CAENStuff/CAENComm-1.2/include

OBJS:=
OBJS += TV792Histogram.o
OBJS += TV1720Waveform.o
OBJS += TUCNHit.o
OBJS += TUCNDetectorBaseClass.o
OBJS += TUCNDetectorCharge.o
OBJS += TUCNRateVsTime.o
OBJS += THe3Detector.o
OBJS += TLi6Detector.o
OBJS += THe3CountsInSequence.o
OBJS += TAnaManager.o
OBJS += TV1720Histograms.o
OBJS += PulseShapeStruct.o
OBJS += TUCNAnaViewer3.o
OBJS += TV1720WaveformDisplay.o
OBJS += TV1725DppPsdData.o
OBJS += TV1725WaveformDisplay.o
OBJS += TUCNTreeMaker.o
OBJS += TLi6GainCalib.o
OBJS += TUCNCycleParameters.o
OBJS += TUCNChronobox.o

all: $(OBJS) online_analysis.exe anaDisplay.exe midas2root.exe analyzer_persist.exe data_quality_checker.exe
# UCNAnalyzer.exe UCNDisplay.exe UCNRateMonitor.exe UCNDisplay3.exe

online_analysis.exe: online_analysis.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

anaDisplay.exe: anaDisplay.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

midas2root.exe: midas2root.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

analyzer_persist.exe: analyzer_persist.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

data_quality_checker.exe: data_quality_checker.cxx $(OBJS) 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil


#UCNDisplay.exe: UCNDisplay.cxx $(OBJS)
#	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

#UCNDisplay3.exe: UCNDisplay3.cxx $(OBJS)
#	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

#UCNRateMonitor.exe: UCNRateMonitor.cxx $(OBJS)
#	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

#UCNAnalyzer.exe: UCNAnalyzer.cxx $(OBJS)
#	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

%.o: %.cxx
	$(CXX) -o $@ $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a
	-rm -f *.exe
	-rm -rf *.exe.dSYM

# end
