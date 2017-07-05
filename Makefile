all:
	clang++ -std=c++14 -I/Users/jroesch/Git/lean/src -L/Users/jroesch/Git/lean/build/release -lleanshared -lgmp -shared -fPIC ext/net.cpp -o liblean_net.dylib

test: all
	clang++ -std=c++14 -I/Users/jroesch/Git/lean/src -L/Users/jroesch/Git/lean/build/release -L. -lleanshared -lgmp -llean_net ext/net_test.cpp -o net_test
