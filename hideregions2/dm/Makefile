CXX=g++
CXXFLAGS = -g -Wall -I$(ASCDS_LIB)/../include/ -O2

objects = dataset.o general.o descriptor.o image.o block.o memimage.o coord.o

all: libdmxx.a test.out

clean:
	rm -f *.o libdmxx.a

dataset.o: dataset.hh image.hh block.hh
general.o: general.hh
descriptor.o: descriptor.hh
image.o: image.hh descriptor.hh block.hh memimage.hh
block.o: block.hh
memimage.o: memimage.hh
coord.o: coord.hh

libdmxx.a: $(objects)
	ar -rcs libdmxx.a $(objects)

test.out : test.cc libdmxx.a
	$(CXX) -o test.out test.cc $(CXXFLAGS) -L. -ldmxx -L$(ASCDS_LIB) \
	-lascdm -lregion
