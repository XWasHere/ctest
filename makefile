CXX ?= g++
CXX_ARGS ?= -O3 -ggdb

OBJS = build/lib/test.o \
       build/lib/testcontroller.o \
	   build/lib/testio.o

PLATFORM_EXT = .dll

.PHONY: all ctest clean test headers

all: ctest 

clean:
	rm -rf build/*
	rm -rf out/*
	mkdir out/include
	mkdir out/lib
	mkdir out/test
	mkdir build/lib
	mkdir build/test

ctest: out/lib/libctest$(PLATFORM_EXT) headers

headers:
	cp -f src/ctest.h out/include/ 
	cp -f src/test.h out/include/
	cp -f src/testcontroller.h out/include/
	cp -f src/testio.h out/include/

out/lib/libctest.dll: $(OBJS)
	$(CXX) $(CXX_ARGS) $(OBJS) -o out/lib/libctest.dll -fPIC -shared

build/lib/test.o: src/test.cc src/test.h
	$(CXX) $(CXX_ARGS) -fpermissive -c src/test.cc -o build/lib/test.o

build/lib/testcontroller.o: src/testcontroller.cc src/testcontroller.h
	$(CXX) $(CXX_ARGS) -fpermissive -c src/testcontroller.cc -o build/lib/testcontroller.o

build/lib/testio.o: src/testio.cc src/testio.h
	$(CXX) $(CXX_ARGS) -fpermissive -c src/testio.cc -o build/lib/testio.o

test: ctest test/main.cc
	$(CXX) $(CXX_ARGS) -c test/main.cc -o build/test/main.o -Iout/include
	$(CXX) $(CXX_ARGS) build/test/main.o -o out/test/ctest_test -Lout/lib -lctest
	
	cp -f ./out/lib/libctest$(PLATFORM_EXT) ./out/test/libctest$(PLATFORM_EXT)

	out/test/ctest_test