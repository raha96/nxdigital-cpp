flags=-std=c++17 -fconcepts

demo: build
	./build/demo

build: buildir
	g++ demo.cpp $(flags) -o build/demo

buildir:
	mkdir -p build