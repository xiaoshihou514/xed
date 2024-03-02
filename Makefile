# compiler and its flags
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c2x
# the dependencies
LIBDESTDIR=build/src/lib
LIBSRCS=$(wildcard src/lib/*.c)
LIBOBJS=$(patsubst src/lib/%.c,$(LIBDESTDIR)/%.o,$(LIBSRCS))
# the tests
TESTDESTDIR=build/tests
TESTSRCS=$(wildcard tests/*.c)
TESTBINS=$(patsubst tests/%.c,$(TESTDESTDIR)/%,$(TESTSRCS))

# what happens if we run make without any flags
output: main

# creates the directories as needed
init:
	@mkdir -p build/src/lib
	@mkdir -p build/tests

# compile src/lib/*
lib: init $(LIBOBJS)
	@echo "src/lib built"
# for each .o file, we compile the .c file to its needed location
$(LIBDESTDIR)/%.o: src/lib/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# build tests, we are targeting on lib files so we would build that first
test: $(TESTBINS)
	for test in $(TESTBINS) ; do ./$$test ; done
# build rules for every test
$(TESTDESTDIR)/%: tests/%.c lib
	# we need to compile with our lib files and our test framework
	$(CC) $(CFLAGS) $< $(LIBOBJS) -o $@ -lcriterion


# outputs an executable
main: lib
	$(CC) $(CFLAGS) src/main.c $(LIBOBJS) -o xed

# ... and run it
run: main
	./xed

# compile and run with debug info
debug: lib
	$(CC) $(CFLAGS) -g src/main.c $(OBJS) -o xed
	gdb ./xed

clean:
	rm -r build
