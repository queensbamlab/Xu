# Xu Makefile

# Required Packages: libjsoncpp-dev, libcurl4-openssl-dev

CC = g++
CFLAGS = -O3 -std=c++11 -ow2v -fopenmp -march=native -funroll-loops -lpthread
INCLUDES = -ljsoncpp -lcurl

OUT = xu

SOURCES = $(shell find src/ -name '*.cpp' | sort -k 1nr | cut -f2-)

.PHONY: default_target
default_target: all

.PHONY: all
all:
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES) -o $(OUT)
clean:
	rm $(OUT)