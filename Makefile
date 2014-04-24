CXX = gcc
LDFLAGS = `pkg-config --libs gtk+-3.0`
CFLAGS = `pkg-config --cflags gtk+-3.0` -ggdb -Wall -Werror -Wno-trampolines 

TARGET = jld

all: $(TARGET)

build/:
	mkdir build/

build/%.o: src/%.c
	$(CXX) -c $(CFLAGS) -o $@ $<

SRC = $(wildcard src/*.c)
OSRC = $(patsubst src/%,%,$(SRC))
OBJ = $(foreach obj, $(OSRC:.c=.o), build/$(obj))


$(TARGET): build/ $(OBJ) 
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)


clean:
	rm -r build
	rm $(TARGET)

