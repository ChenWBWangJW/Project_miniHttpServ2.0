CC = g++
CFLAGS = -Wall -g
LDFLAGS = -pthread

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, bin/%.o, $(SRCS))

.PHONY: all clean

all: bin httpServ.exe

bin:
	mkdir -p bin

httpServ.exe: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

bin/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin httpServ.exe
