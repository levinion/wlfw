BUILD_TYPE ?= Release
PROJECT_NAME = $(shell cat build/CMakeCache.txt | grep CMAKE_PROJECT_NAME | cut -d '=' -f 2) 

init: CMakeLists.txt
	cmake -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: init
	cmake --build build -j

clean:
	rm -rf build

debug:
	make BUILD_TYPE=Debug

run: build
	./build/$(PROJECT_NAME)

.PHONY: init build clean debug run
