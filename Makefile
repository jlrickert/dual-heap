PROGRAM = dualheap

CXX = g++
CXXFLAGS = -g -Wall

TMP = tmp
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
	mkdir -p $(TMP)

# links all objects and compiles to program
bin/$(PROGRAM): $(OBJECTS) $(PROGRAM).cpp
	mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

# compiles all c++ files to an object file
$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(BUILD)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

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
	rm -r $(TMP)

.PHONY: clean
