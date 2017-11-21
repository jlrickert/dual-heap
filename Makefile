PROGRAM = dualheap

CXX = g++
CXXFLAGS = -g -Wall

BIN = bin
TEST = test
SRC = src
BUILD = build

################################################################################
#### Build logic below
####
#### should not need to touch anything below this
################################################################################
SOURCES := $(shell find $(SRC) -name "*.cpp")
OBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(SOURCES:.cpp=.o))
INCLUDES := -Iinclude

# build program and test program
build: bin/$(PROGRAM)

# compile program if out of date and run executable with arguments
# example: `make exe arg1 arg2 arg3`
exe: build
	@bin/$(PROGRAM) $(filter-out $@, $(MAKECMDGOALS))

# links all objects and compiles to program
bin/$(PROGRAM): $(OBJECTS) $(PROGRAM).cpp
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

# compiles all c++ files to an object file
$(BUILD)/%.o: $(SRC)/%.cpp
	@mkdir -p $(BUILD)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

########################################
#### Test section
########################################
TEST_BUILD = $(BUILD)/test
TEST_SOURCES = $(shell find $(TEST) -name "*.cpp")
TEST_OBJECTS := $(patsubst $(TEST)/%,$(TEST_BUILD)/%,$(TEST_SOURCES:.cpp=.o))
GTEST_CFLAGS = $(shell pkg-config --cflags gtest)
GTEST_LIBS = $(shell pkg-config --libs gtest)

bin/test_$(PROGRAM): $(TEST_OBJECTS) $(OBJECTS)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(GTEST_LIBS)

$(TEST_BUILD)/%.o: $(TEST)/%.cpp
	@mkdir -p $(TEST_BUILD)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $< $(GTEST_CFLAGS)

########################################
#### Documentation section
########################################
docs:
	doxygen

########################################
#### Utility section
########################################
clean:
	rm -r $(BUILD)
	rm -r $(BIN)

.PHONY: clean

%:
	@true
