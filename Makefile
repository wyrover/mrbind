.PHONY: all cmake build test clean

all: build

cmake:	
	mkdir -p build && cd build && cmake ..

build: cmake
	cd build && make

test: build
	./build/exec_test

clean:
	rm -rf build/
