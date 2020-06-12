build:
	g++ ../BeeARM/*.cpp tests.cpp -std=c++17 -DBEEARM_ENABLE_THUMB -o test

buildclang:
	clang++ ../BeeARM/*.cpp tests.cpp -std=c++17 -DBEEARM_ENABLE_THUMB -o test

buildtest:
	g++ ../BeeARM/*.cpp testenv.cpp -std=c++17 -O3 -DBEEARM_ENABLE_THUMB -o testenv

clean:
	rm -r test

cleantest:
	rm -r testenv
