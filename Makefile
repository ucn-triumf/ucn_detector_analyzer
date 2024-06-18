# Makefile for ucn_detector_analyzer executables

MODULES   := TUCNTree TV Detector Exp Misc
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cxx))
OBJS      := $(patsubst src/%.cxx,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cxx $(SRC_DIR)

CXXFLAGS = -g -O2 -Wall -Wuninitialized

# required ZLIB library
CXXFLAGS += -DHAVE_LIBZ -std=c++11

# required ROOTANA library
ifndef ROOTANASYS
	ROOTANASYS=..
endif
CXXFLAGS += -I$(ROOTANASYS)/include $(INCLUDES)
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

	MIDASLIBS = $(MIDASSYS)/lib/libmidas.a -lutil -lrt
	CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

	UNAME := $(shell uname)

	ifeq ($(UNAME),Darwin)
		MIDASLIBS = $(MIDASSYS)/darwin/lib/libmidas.a
	endif
	LIBS += $(MIDASLIBS)

endif # MIDASSYS

# for compiling all the src files in the various modules
define make-goal
$1/%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(OBJS) anaDisplay.exe midas2root.exe analyzer_persist.exe data_quality_checker.exe ana.exe

midas2root.exe: src/midas2root.cxx $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

anaDisplay.exe: src/anaDisplay.cxx $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

analyzer_persist.exe: src/analyzer_persist.cxx $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

data_quality_checker.exe: src/data_quality_checker.cxx $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

ana.exe: src/data_quality_checker.cxx $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBS) $(ROOTGLIBS) -lm -lz -lpthread -lssl -lutil

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)
	-rm -rf build
	-rm -f *.exe

# build all the files in the various modules
$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
