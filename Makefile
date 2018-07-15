CC = gcc
CXX = g++

LIBS += -L . libdvp.so libhzd.so -lpthread   -Wl,-rpath=.

OPENCV = $(LIBS)+ `pkg-config --cflags --libs opencv`
all: Demo main

main:main.cpp
	g++ main.cpp -o main `pkg-config --cflags --libs opencv`

Demo:Demo.cpp
	$(CXX) -o $@ $< $(LIBS) 

clean:
	rm Demo 
