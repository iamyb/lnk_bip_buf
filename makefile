CC = gcc

CCFLAGS = -I./include -I./src -DLBB_TEST
SOURCES=$(wildcard ./src/*.c)
TEST=$(wildcard ./test/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
TESTOBJ=$(patsubst %.c, %.o, $(TEST))

EXECUTABLE=bin/testlbb

all: build $(EXECUTABLE)

build:
	@mkdir -p bin

$(EXECUTABLE): $(OBJECTS) $(TESTOBJ)
	$(CC) $(CCFLAGS) -o $@ $(OBJECTS) $(TESTOBJ)

$(OBJECTS): ./src/%.o : ./src/%.c
	$(CC) $(CCFLAGS) -c $<  -o $@

$(TESTOBJ): ./test/%.o : ./test/%.c
	$(CC) $(CCFLAGS) -c $<  -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) $(TESTOBJ) bin
