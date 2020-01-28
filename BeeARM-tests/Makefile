build:
	g++ ../BeeARM/*.cpp tests.cpp -DBEEARM_ENABLE_THUMB -o test

buildclang:
	clang++ ../BeeARM/*.cpp tests.cpp -DBEEARM_ENABLE_THUMB -o test

buildtest:
	g++ ../BeeARM/*.cpp testenv.cpp -DBEEARM_ENABLE_THUMB -o testenv

clean:
	rm -r test

cleantest:
	rm -r testenv
