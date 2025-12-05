
all: clean build start

build:
		mkdir -p build
		cd build && cmake ..
		$(MAKE) -C build

start:
		./build/MyProj

clean:
		rm -rf build