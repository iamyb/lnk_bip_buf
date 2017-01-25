CC = gcc

CCFLAGS = -I./include -I./src -DLBB_TEST  -O3
SOURCES=$(wildcard ./src/*.c)
TEST=$(wildcard ./test/test_lnk_bip_buf.c)
TEST_PERF=$(wildcard ./test/test_performance.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
TESTOBJ=$(patsubst %.c, %.o, $(TEST))
TESTOBJ_PERF=$(patsubst %.c, %.o, $(TEST_PERF))

EXECUTABLE=bin/testlbb
EXECUTABLE_PERF=bin/testlbbperf

all: build $(EXECUTABLE) $(EXECUTABLE_PERF)

build:
	@mkdir -p bin

$(EXECUTABLE): $(OBJECTS) $(TESTOBJ)
	$(CC) $(CCFLAGS) -o $@ $(OBJECTS) $(TESTOBJ)

$(EXECUTABLE_PERF): $(OBJECTS) $(TESTOBJ_PERF)
	$(CC) $(CCFLAGS) -o $@ $(OBJECTS) $(TESTOBJ_PERF)

$(OBJECTS): ./src/%.o : ./src/%.c
	$(CC) $(CCFLAGS) -c $<  -o $@

$(TESTOBJ): $(TEST) 
	$(CC) $(CCFLAGS) -c $<  -o $@

$(TESTOBJ_PERF): $(TEST_PERF)
	$(CC) $(CCFLAGS) -c $<  -o $@
clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) $(TESTOBJ) $(TESTOBJ_PERF)  bin
