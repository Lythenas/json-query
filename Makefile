# CC=clang
CXX=clang++
RM=rm -f

WARNINGS=-Wall -Wextra -Wpedantic
DEBUG=-g

CXXFLAGS=$(WARNINGS) $(DEBUG) -std=c++2a -I./src
LDFLAGS=$(DEBUG)
LDLIBS=

ifdef FUZZ
CXXFLAGS += -g -O1 -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping
LDFLAGS += -fsanitize=fuzzer,address
endif

SRCS_LIB:=$(filter-out src/main.cpp,$(shell find src -type f -name '*.cpp'))
OBJS_LIB:=$(patsubst %.cpp,%.o,$(SRCS_LIB))

SRCS_APP:=src/main.cpp
OBJS_APP:=$(patsubst %.cpp,%.o,$(SRCS_APP))

SRCS_TEST:=$(shell find test -type f -name '*.cpp')
OBJS_TEST:=$(pathsubst %cpp,%.o,$(SRCS_TEST))

SRCS_ALL:=$(SRCS_LIB) $(SRCS_APP) $(SRCS_TEST)
OBJS_ALL:=$(OBJS_LIB) $(OBJS_APP) $(OBJS_TEST)

# default target
all: jsonquery

# LIBRARY
lib.a: $(OBJS_LIB)
	ar crvs $@ $^

# EXECUTABLE
jsonquery: src/main.o lib.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# TESTS
check: jsonquery_test
	./jsonquery_test

# main test suite
jsonquery_test: test/main.o lib.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# fuzzer test
jsonquery_fuzz: test/test_fuzzer.cpp lib.a
	@test $(FUZZ) || echo -e "NOTE: Fuzzing only works with clang and with FUZZ=1 when compiling ALL targets (you should run make clean if you built without fuzzing before)! But with FUZZ=1 you can't make the other binaries.\n"
	$(CXX) $(CXXFLAGS) $(LDFLAGS) \
	    -fsanitize=fuzzer,address \
	    -fprofile-instr-generate \
	    -fcoverage-mapping \
	    -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Generate a .depend file that contains all the header dependencies
# in Makefile format so we can include it below
# depend: .depend
.depend: $(SRCS_ALL)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ > ./.depend;

clean:
	$(RM) $(OBJS_ALL) lib.a

distclean: clean
	$(RM) *~ .depend

cleanall: distclean
	$(RM) jsonquery jsonquery_test jsonquery_fuzz

.PHONY: clean distclean all test

include .depend
