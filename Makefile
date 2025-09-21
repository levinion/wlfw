BUILD_TYPE ?= Release

build: init
	cmake --build build -j

init: CMakeLists.txt
	mkdir -p protocols/src
	mkdir -p protocols/include
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml protocols/src/xdg-shell-protocol.c
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml protocols/include/xdg-shell-client-protocol.h
	wayland-scanner private-code ./protocols/wlr-layer-shell-unstable-v1.xml protocols/src/wlr-layer-shell-unstable-v1-protocol.c
	wayland-scanner client-header ./protocols/wlr-layer-shell-unstable-v1.xml protocols/include/wlr-layer-shell-unstable-v1-protocol.h
	cmake -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

clean:
	rm -rf build

debug:
	$(MAKE) BUILD_TYPE=Debug

EXAMPLE ?= drawing_tablet

run:
	$(MAKE) -C ./examples/$(EXAMPLE) run

.PHONY: init build clean debug run
