#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = DisplayTool_RawViewerCint
SHLIB = libDisplayTool_RawViewer.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LARLITE_COREDIR)
INCFLAGS += -I$(LARLITE_USERDEVDIR)/DisplayTool
INCFLAGS += $(shell python-config --includes)
INCFLAGS += -I$(shell python -c "import numpy; print numpy.get_include()")

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)



include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# call the common GNUmakefile
LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(shell python-config --prefix)/lib/
LDFLAGS += $(shell python-config --ldflags)
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE

