CXX=g++
CC=g++

CXXFLAGS=-g -Wall -O2

ALL_CXXFLAGS = -I. -I${ASCDS_LIB}/../include $(CXXFLAGS)

.cc.o:
	$(CXX) -c $(CPPFLAGS) $(ALL_CXXFLAGS) $<

all: hideregions2

clean:
	rm -f hideregions2
	@${MAKE} -C dm clean

dm/libdmxx.a:
	@${MAKE} -C dm

hideregions2: hideregions2.o dm/libdmxx.a
	$(CXX) -o hideregions2 hideregions2.o -Ldm -ldmxx -L$(ASCDS_LIB) -lregion -lascdm -Wl,-rpath $(ASCDS_LIB) -Wl,-rpath $(ASCDS_LIB)/../ots/lib
