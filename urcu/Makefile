
MYDEPS = \
	URCU.hpp \
	LinkedListURCU.hpp \
	QueueURCU.hpp\
	StackURCU.hpp \

bench: $(MYDEPS) Benchmark.cpp
	g++ -g -O3 -std=c++14 Benchmark.cpp -o benchmark -lstdc++ -lpthread


bench-asan: $(MYDEPS) bench.cpp Benchmark.hpp
	g++-7 -fuse-ld=gold -fsanitize=address -g -O3 -std=c++14 Benchmark.cpp -o benchmark-asan -lstdc++ -lpthread


all: benchmark