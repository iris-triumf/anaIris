# Makefile
BASEDIR	      = $(shell pwd)/..
INSTALLDIR    = $(BASEDIR)/anaIris
INCLUDEDIR    = $(INSTALLDIR)/include
LIBDIR        = $(INSTALLDIR)/lib
BINARYDIR     = $(INSTALLDIR)/bin
SOURCEDIR     = $(INSTALLDIR)/src
OBJECTDIR     = $(INSTALLDIR)/obj
DATASTRUCTDIR = $(BASEDIR)/DataStructure

CXX = g++
# ROOT library
ROOTLIBS  = $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread -lRHTTP -Wl,-rpath,$(ROOTSYS)/lib
ROOTGLIBS = $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread -lRHTTP -Wl,-rpath,$(ROOTSYS)/lib

ifdef ROOTSYS
CXXFLAGS += -DHAVE_LIBNETDIRECTORY
OBJS     += $(ROOTANA)/libNetDirectory/netDirectoryServer.o

NETDIRLIB = $(ROOTANA)/libNetDirectory/libNetDirectory.a
endif

# ROOT analyzer
	ROOTANA = $(HOME)/packages/rootana

CXXFLAGS += -g -O -Wall -Wuninitialized -I./  -I$(INCLUDEDIR) -I$(ROOTSYS)/include -I$(ROOTANA) -I$(ROOTANA)/include
CFLAGS += -I$(INCLUDEDIR)

ROOTCFLAGS    = $(shell root-config --cflags)
CXXFLAGS      += -g -Wall -ansi -Df2cFortran -fPIC $(ROOTCFLAGS) 
CXXFLAGS      += -I$(DATASTRUCTDIR)/include -I$(RECDIR)/include -L$(LIBDIR) 

ANAOBJECTS  =  $(OBJECTDIR)/HistPar.o $(OBJECTDIR)/SetupHistos.o $(OBJECTDIR)/eloss.o $(OBJECTDIR)/Graphsdedx.o $(OBJECTDIR)/nucleus.o $(OBJECTDIR)/runDepPar.o $(OBJECTDIR)/CalibMesytec.o $(OBJECTDIR)/geometry.o $(OBJECTDIR)/HandleMesytec.o $(OBJECTDIR)/HandleV1190.o $(OBJECTDIR)/HandleV1740.o   $(OBJECTDIR)/HandleSTAT.o $(OBJECTDIR)/HandlePHYSICS.o $(OBJECTDIR)/HandleScaler.o    

ifdef MIDASSYS
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a
endif

#DPP tools, added 2021-03-01 (Jonas Refsgaard)
IRISDIR = /home/iris/packages/iris-daqtools
IRISLIB = $(IRISDIR)/build/libdaqtools_static.a
CXXFLAGS += -I$(IRISDIR)/include

SOFLAGS       = -g -shared

all: $(BINARYDIR)/anaIris

$(BINARYDIR)/anaIris: $(ANAOBJECTS) $(OBJECTDIR)/anaIris.o $(OBJECTDIR)/web_server.o $(MIDASLIBS) $(ROOTANA)/lib/librootana.a 
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(NETDIRLIB) $(ROOTGLIBS) $(IRISLIB) -lm -lz -lutil -lnsl -lpthread -lrt

#___ALL THE OBJECT FILES____

$(OBJECTDIR)/anaIris.o: $(SOURCEDIR)/anaIris.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

$(OBJECTDIR)/HandleMesytec.o: $(SOURCEDIR)/HandleMesytec.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/HandleV1190.o: $(SOURCEDIR)/HandleV1190.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJECTDIR)/HandleV1740.o: $(SOURCEDIR)/HandleV1740.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/HandleSTAT.o: $(SOURCEDIR)/HandleSTAT.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

 $(OBJECTDIR)/HandleScaler.o: $(SOURCEDIR)/HandleScaler.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/HandlePHYSICS.o: $(SOURCEDIR)/HandlePHYSICS.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/web_server.o: $(SOURCEDIR)/web_server.c
	$(CXX) $(USERFLAGS) $(ROOTCFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

$(OBJECTDIR)/geometry.o: $(SOURCEDIR)/geometry.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/eloss.o: $(SOURCEDIR)/eloss.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/Graphsdedx.o: $(SOURCEDIR)/Graphsdedx.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/nucleus.o: $(SOURCEDIR)/nucleus.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/runDepPar.o: $(SOURCEDIR)/runDepPar.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/CalibMesytec.o: $(SOURCEDIR)/CalibMesytec.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/SetupHistos.o: $(SOURCEDIR)/SetupHistos.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTDIR)/HistPar.o: $(SOURCEDIR)/HistPar.cxx 
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean::
	rm -f $(BINARYDIR)/anaIris 
	rm -f $(OBJECTDIR)/*.o 
	rm -f $(SOURCEDIR)/*Dict.cxx 
	rm -f $(SOURCEDIR)/*Dict.cpp 
	rm -f $(SOURCEDIR)/*Dict.h 
	rm -f core 
	rm -f $(SOURCEDIR)/*~ $(INCLUDEDIR)/*~ 

# end
